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
	const int m_CheckRestoreEveryTicks = 5000;
	const int m_ThreadSleepMsIfNoData = 1;
	const uint32_t m_Version = 1;

	CamData::CamData()
	{

	}

	CamData::CamData(float time, float xPosition, float yPosition, float zPosition, float xRotation, float yRotation, float zRotation, float fov)
	: Time(time)
	, XPosition(xPosition)
	, YPosition(yPosition)
	, ZPosition(zPosition)
	, XRotation(xRotation)
	, YRotation(yRotation)
	, ZRotation(zRotation)
	, Fov(fov)
	{
	}

	bool m_WsaActive = false;

	WebSocket * m_Ws = 0;
	bool m_WantWs = false;
	std::string m_WsUrl("ws://host:port/path");
	std::mutex m_WsMutex;

	std::list<std::string> m_Commands;
	std::mutex m_CommandsMutex;

	std::vector<uint8_t> m_Data;
	std::condition_variable m_DataAvailableCondition;
	std::mutex m_DataMutex;
	std::vector<uint8_t> m_TempData;

	std::thread * m_Thread = 0;
	bool m_WantClose = false;

	DWORD m_LastCheckRestoreTick = 0;

	bool m_CamDataAvailable = false;
	CamData m_CamData;

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

	void AppendCamData(CamData const camData, std::vector<uint8_t> &outVec)
	{
		uint8_t data[8 * sizeof(float)];

		memcpy(&(data[0 * sizeof(float)]), &camData.Time, sizeof(float));
		memcpy(&(data[1 * sizeof(float)]), &camData.XPosition, sizeof(float));
		memcpy(&(data[2 * sizeof(float)]), &camData.YPosition, sizeof(float));
		memcpy(&(data[3 * sizeof(float)]), &camData.ZPosition, sizeof(float));
		memcpy(&(data[4 * sizeof(float)]), &camData.XRotation, sizeof(float));
		memcpy(&(data[5 * sizeof(float)]), &camData.YRotation, sizeof(float));
		memcpy(&(data[6 * sizeof(float)]), &camData.ZRotation, sizeof(float));
		memcpy(&(data[7 * sizeof(float)]), &camData.Fov, sizeof(float));

		outVec.insert(m_Data.end(), std::begin(data), std::end(data));
	}

	void Recv_String(const std::string & message)
	{
		// lul
	}

	void Recv_Bytes(const std::vector<uint8_t>& message)
	{
		std::vector<uint8_t>::const_iterator itBegin = message.begin();

		while (itBegin != message.end())
		{
			std::vector<uint8_t>::const_iterator itDelim = message.end();

			for (std::vector<uint8_t>::const_iterator it = itBegin; it != message.end(); ++it)
			{
				if ((uint8_t)'\0' == *it)
				{
					itDelim = it;
					break;
				}
			}

			if (message.end() != itDelim && itBegin != itDelim)
			{
				std::string strCode(itBegin, itDelim);

				char const * code = strCode.c_str();

				if (0 == strcmp("exec", code))
				{
					std::unique_lock<std::mutex> lock(m_CommandsMutex);

					std::vector<uint8_t>::const_iterator itCmdStart = itDelim + 1;
					std::vector<uint8_t>::const_iterator itCmdEnd = itCmdStart;

					bool foundDelim = false;

					for (std::vector<uint8_t>::const_iterator it = itCmdStart; it != message.end(); ++it)
					{
						if ((uint8_t)'\0' == *it)
						{
							foundDelim = true;
							itCmdEnd = it;
							break;
						}					
					}

					if (!foundDelim)
						break;

					std::string cmds(itCmdStart, itCmdEnd);

					m_Commands.push_back(cmds);

					itBegin = itCmdEnd + 1;

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
			{
				std::unique_lock<std::mutex> wsLock(m_WsMutex);

				if (WebSocket::CLOSED == m_Ws->getReadyState())
				{
					delete m_Ws;
					m_Ws = 0;
					break;
				}
			}

			m_Ws->poll();

			// this would eat our shit: m_Ws->dispatch(Recv_String); 
			m_Ws->dispatchBinary(Recv_Bytes);

			std::unique_lock<std::mutex> dataLock(m_DataMutex);
			m_DataAvailableCondition.wait_for(dataLock, m_ThreadSleepMsIfNoData * 1ms, [] { return !m_Data.empty() || m_WantClose; }); // if we don't need to send data, we are a bit lazy in order to save some CPU. Of course this assumes, that the data we get from network can wait that long ;)

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

			if (m_WantClose)
				m_Ws->close();
		}
	}
	
	void EndThread()
	{
		if (0 != m_Thread)
		{
			m_WantClose = true;
			
			m_DataAvailableCondition.notify_one();

			m_Thread->join();
			
			delete m_Thread;
			m_Thread = 0;
			
			m_WantClose = false;
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

		m_WsaActive = 0 == WSAStartup(MAKEWORD(2, 2), &wsaData);
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

	void Url_set(char const * url)
	{
		m_WsUrl = url;
	}

	char const * Url_get(void)
	{
		return m_WsUrl.c_str();
	}

	void Start()
	{
		Stop();

		if(m_WsaActive)
		{
			m_WantWs = true;

			m_Ws = WebSocket::from_url(m_WsUrl);

			if (0 != m_Ws)
			{
				AppendHello(m_Data);

				if (!m_CurrentLevel.empty())
				{
					AppendCString("levelInit", m_Data);
					AppendCString(m_CurrentLevel.c_str(), m_Data);
				}

				CreateThread();
			}
		}
	}

	void Stop()
	{
		EndThread();

		m_WantWs = false;
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

			if (m_CheckRestoreEveryTicks <= abs((int)m_LastCheckRestoreTick - (int)curTick))
			{
				m_LastCheckRestoreTick = curTick;

				bool needRestore = false;
				{
					std::unique_lock<std::mutex> lock(m_WsMutex);

					needRestore = 0 == m_Ws;
				}

				if (needRestore)
				{
					Start();
				}
			}
		}
	}

	void SupplyCamData(CamData const & camData)
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


	void DrawingThread_SupplyCamData(CamData const & camData)
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

CON_COMMAND(mirv_pgl, "PGL")
{
	if (!MirvPgl::m_WsaActive)
	{
		Tier0_Warning("Error: WinSock(2.2) not active, feature not available!\n");
		return;
	}

	int argc = args->ArgC();

	if (2 <= argc)
	{
		char const * cmd1 = args->ArgV(1);

		if (0 == _stricmp("start", cmd1))
		{
			MirvPgl::Start();
			return;
		}
		else if (0 == _stricmp("stop", cmd1))
		{
			MirvPgl::Stop();
			return;
		}
		else if (0 == _stricmp("url", cmd1))
		{
			if (3 <= argc)
			{
				MirvPgl::Url_set(args->ArgV(2));
				return;
			}

			Tier0_Msg(
				"mirv_pgl url <url> - Set url to use with start.\n"
				"Current value: %s\n"
				, MirvPgl::Url_get()
			);
			return;
		}

	}

	Tier0_Msg(
		"mirv_pgl start \"ws://host:port/path\" - (Re-)Starts connectinion to server, using the URL set with url.\n"
		"mirv_pgl stop - Stops connection to server.\n"
		"mirv_pgl url [...] - Set url to use with start.\n"
	);

}


#endif // ifdef AFX_MIRV_PGL
