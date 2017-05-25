#include "stdafx.h"

#include "MirvPgl.h"

#ifdef AFX_MIRV_PGL

#include "WrpVEngineClient.h"
#include "WrpConsole.h"

#include "../../shared/easywsclient/easywsclient.hpp"
#include "../../shared/easywsclient/easywsclient.cpp"

#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>

#include <math.h>

#include <mutex>
#include <condition_variable>
#include <string>
#include <list>
#include <chrono>


extern WrpVEngineClient * g_VEngineClient;


using namespace std::chrono_literals;

using easywsclient::WebSocket;

namespace MirvPgl
{
	const int m_ThreadSleepMsIfNoData = 1;
	const uint32_t m_Version = 0;

	bool m_WsaActive = false;

	WebSocket * m_Ws = 0;
	bool m_WantWs = false;
	std::string m_WsUrl;
	std::mutex m_WsMutex;

	std::list<std::string> m_Commands;
	std::mutex m_CommandsMutex;

	std::vector<uint8_t> m_Data;
	std::condition_variable m_DataAvailableCondition;
	std::mutex m_DataMutex;
	std::vector<uint8_t> m_TempData;

	std::thread * m_Thread = 0;

	DWORD m_LastRestoreTick = 0;

	bool m_CamDataAvailable = false;
	CamIO::CamData m_CamData;

	std::string m_CurrentLevel;

	void AppendCString(char const * cstr, std::vector<uint8_t> &outVec)
	{

		std::string str(cstr);
		outVec.insert(outVec.end(), str.begin(), str.end());
		outVec.push_back(static_cast<uint8_t>('\0'));
	}

	void AppendHello(std::vector<uint8_t> &outVec)
	{
		uint8_t data[1 * sizeof(uint32_t)];
		uint32_t version = m_Version;

		AppendCString("hello", outVec);
		memcpy(&(data[0 * sizeof(uint32_t)]), &version, sizeof(uint32_t));

		outVec.insert(m_Data.end(), std::begin(data), std::end(data));
	}

	void AppendCamData(CamIO::CamData const camData, std::vector<uint8_t> &outVec)
	{
		uint8_t data[8 * sizeof(double)];

		memcpy(&(data[0 * sizeof(double)]), &camData.Time, sizeof(double));
		memcpy(&(data[1 * sizeof(double)]), &camData.XPosition, sizeof(double));
		memcpy(&(data[2 * sizeof(double)]), &camData.YPosition, sizeof(double));
		memcpy(&(data[3 * sizeof(double)]), &camData.ZPosition, sizeof(double));
		memcpy(&(data[4 * sizeof(double)]), &camData.XRotation, sizeof(double));
		memcpy(&(data[5 * sizeof(double)]), &camData.YRotation, sizeof(double));
		memcpy(&(data[6 * sizeof(double)]), &camData.ZRotation, sizeof(double));
		memcpy(&(data[7 * sizeof(double)]), &camData.Fov, sizeof(double));

		outVec.insert(m_Data.end(), std::begin(data), std::end(data));
	}

	void Recv_String(const std::string & message)
	{
		// lul
	}

	void Recv_Bytes(const std::vector<uint8_t>& message)
	{
		while (true)
		{
			std::vector<uint8_t>::const_iterator itDelim = message.end();

			for (std::vector<uint8_t>::const_iterator it = message.begin(); it != message.end(); ++it)
			{
				if ((uint8_t)'\0' == *it)
				{
					itDelim;
					break;
				}
			}

			if (message.end() != itDelim && message.begin() != itDelim)
			{
				std::string strCode(message.begin(), itDelim - 1);

				char const * code = strCode.c_str();

				if (0 == strcmp("exec", code))
				{
					std::unique_lock<std::mutex> lock(m_CommandsMutex);

					std::vector<uint8_t>::const_iterator itStart = itDelim + 1;
					std::vector<uint8_t>::const_iterator itEnd = itStart;

					bool foundDelim = false;

					for (std::vector<uint8_t>::const_iterator it = message.begin(); it != message.end(); ++it)
					{
						if ((uint8_t)'\0' == *it)
						{
							foundDelim = true;
							break;
						}
						
						itEnd = it;
					}

					if (!foundDelim)
						break;

					std::string cmds(itStart, itEnd);

					m_Commands.push_back(cmds);

					continue;
				}
			}

			break;
		}
	}

	void Thread()
	{
		m_TempData.clear();

		while (true)
		{
			std::unique_lock<std::mutex> lock(m_WsMutex);

			if (0 == m_Ws || WebSocket::CLOSED != m_Ws->getReadyState())
				break;

			m_Ws->poll();

			std::unique_lock<std::mutex> dataLock(m_DataMutex);
			m_DataAvailableCondition.wait_for(dataLock, m_ThreadSleepMsIfNoData * 1ms, [] { return !m_Data.empty(); }); // if we don't need to send data, we are a bit lazy in order to save some CPU. Of course this assumes, that the data we get from network can wait that long ;)

			if (!m_Data.empty())
			{
				m_TempData = std::move(m_Data);
				m_Data.clear();
				dataLock.unlock();

				m_Ws->sendBinary(m_TempData);
				m_TempData.clear();
			}
			else
				dataLock.unlock();
		}

		{
			std::unique_lock<std::mutex> dataLock(m_DataMutex);
			m_Data.clear();
		}
	}
	
	void EndThread()
	{
		if (0 != m_Thread)
		{
			m_Thread->join();
			m_Thread = 0;
		}
	}

	void CreateThread()
	{
		EndThread();

		m_Thread = new std::thread(Thread);
	}


	void Init()
	{
		Shutdown();

		WSADATA wsaData;

		m_WsaActive = 0 != WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	void Shutdown()
	{
		Stop();

		if (m_WsaActive)
		{
			WSACleanup();
			m_WsaActive = false;
		}
	}

	void Start(char const * url)
	{
		Stop();

		if(m_WsaActive)
		{
			std::unique_lock<std::mutex> lock(m_WsMutex);

			m_WantWs = true;
			m_WsUrl = url;

			if (0 == m_Ws)
			{
				m_Ws = WebSocket::from_url(m_WsUrl);

				{
					std::unique_lock<std::mutex> lock(m_DataMutex);

					AppendHello(m_Data);

					if (!m_CurrentLevel.empty())
					{

						AppendCString("levelInit", m_Data);
						AppendCString(m_CurrentLevel.c_str(), m_Data);
					}
				}

				CreateThread();
			}
		}
	}

	void Stop()
	{
		{
			std::unique_lock<std::mutex> lock(m_WsMutex);

			if (0 != m_Ws)
			{
				m_Ws->close();

				delete m_Ws;
				m_Ws = 0;
			}

			m_WantWs = false;
		}

		EndThread();
	}

	bool IsStarted()
	{
		return m_WantWs;
	}

	void CheckStartedAndRestoreIfDown()
	{
		if (m_WantWs)
		{
			DWORD curTick = GetTickCount();

			if (5000 <= abs((int)m_LastRestoreTick - (int)curTick))
			{
				m_LastRestoreTick = curTick;

				bool needRestore = false;
				{

					std::unique_lock<std::mutex> lock(m_WsMutex);

					if (m_WantWs)
					{
						needRestore = 0 == m_Ws || WebSocket::CLOSED == m_Ws->getReadyState();
					}
				}

				if (needRestore)
				{
					std::string oldUrl = m_WsUrl;

					Start(oldUrl.c_str());
				}
			}
		}
	}

	void SupplyCamData(CamIO::CamData const & camData)
	{
		if (!m_WantWs)
			return;

		{
			std::unique_lock<std::mutex> lock(m_DataMutex);

			AppendCString("cam", m_Data);
			AppendCamData(camData, m_Data);
		}
		m_DataAvailableCondition.notify_one();
	}

	void SupplyLevelInit(char const * mapName)
	{
		m_CurrentLevel = mapName;

		if (!m_WantWs)
			return;

		{
			std::unique_lock<std::mutex> lock(m_DataMutex);

			AppendCString("levelInit", m_Data);
			AppendCString(mapName, m_Data);
		}
		m_DataAvailableCondition.notify_one();
	}

	void SupplyLevelShutdown()
	{
		m_CurrentLevel.clear();

		if (!m_WantWs)
			return;

		{
			std::unique_lock<std::mutex> lock(m_DataMutex);

			AppendCString("levelShutdown", m_Data);
		}
		m_DataAvailableCondition.notify_one();
	}

	void ExecuteQueuedCommands()
	{
		std::unique_lock<std::mutex> lock(m_CommandsMutex);

		while (0 < m_Commands.size())
		{
			std::string cmd = m_Commands.front();
			m_Commands.pop_front();
			lock.unlock();

			g_VEngineClient->ExecuteClientCmd(cmd.c_str());

			lock.lock();
		}
	}


	void DrawingThread_SupplyCamData(CamIO::CamData const & camData)
	{
		m_CamDataAvailable = true;
		m_CamData = camData;
	}

	void DrawingThread_PresentedUnleashCamDataOnFirstCall()
	{
		if (m_CamDataAvailable)
		{
			m_CamDataAvailable = false;
			SupplyCamData(m_CamData);
		}
	}

}
#endif // ifdef AFX_MIRV_PGL
