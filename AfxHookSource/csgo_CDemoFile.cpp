#include "stdafx.h"

#include "csgo_CDemoFile.h"

#include "addresses.h"

#include <SourceInterfaces.h>
#include "WrpConsole.h"
#include <csgo/sdk_src/public/tier0/memalloc.h>

#include <shared/csgo-demoinfo/demoinfogo/demofilebitbuf.h>
#include <shared/csgo-demoinfo/demoinfogo/generated_proto/netmessages_public.pb.h>

#include <Windows.h>
#include <shared/Detours/src/detours.h>
#include <shared/detours.h>

#include <shared/FileTools.h>
#include <shared/StringTools.h>

#include <map>
#include <set>
#include <limits>
#include <vector>

#undef max

int g_bFixCDemoFileTicks = 7680;

static unsigned int g_Crc32Table[256] = {
0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	 0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	 0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	 0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

unsigned int ComputeCrc32(unsigned char* data, size_t size)
{
	unsigned int crc32 = 0xffffffff;

	Tier0_Warning("CRC32: 0x%08x %u\n", data, size);

	for (size_t i = 0; i < size; ++i)
	{
		unsigned int nLookUpIndex = (crc32 ^ *data) & 0xff;
		crc32 = (crc32 >> 8) ^ g_Crc32Table[nLookUpIndex];

		++data;
	}

	return crc32;
}

class CDemoFileIndexValue
{
public:
	CDemoFileIndexValue()
		: m_Valid(false)
	{

	}

	void Init(int tick, size_t demoFilePos, unsigned int crc32)
	{
		m_Buffer.clear();
		m_DemoFilePos = demoFilePos;
		m_Crc32 = crc32;
		m_Tick = tick;
		m_Valid = true;
	}

	void Invalidate()
	{
		m_Valid = false;
	}

	bool ReadFromFile(FILE* file, int tick, size_t pos)
	{
		if (0 != fseek(file, pos, SEEK_SET))
			return false;

		if (1 != fread(&m_Crc32, sizeof(m_Crc32), 1, file))
			return false;

		if (1 != fread(&m_DemoFilePos, sizeof(m_DemoFilePos), 1, file))
				return false;

		size_t size;

		if (1 != fread(&size, sizeof(size), 1, file))
			return false;

		m_Buffer.resize(size);	

		if (size != fread(&(*m_Buffer.begin()), sizeof(unsigned char), size, file))
			return false;

		m_Tick = tick;

		return true;
	}

	bool WriteToFile(FILE* file, size_t & outPos)
	{
		outPos = ftell(file);

		if (-1L == outPos)
			return false;

		if (1 != fwrite(&m_Crc32, sizeof(m_Crc32), 1, file))
			return false;

		if (1 != fwrite(&m_DemoFilePos, sizeof(m_DemoFilePos), 1, file))
			return false;

		size_t size = m_Buffer.size();

		if (1 != fwrite(&size, sizeof(size), 1, file))
			return false;

		if (size != fwrite(&m_Buffer.cbegin(), sizeof(unsigned char), size, file))
			return false;

		return true;
	}

	bool GetValid()
	{
		return m_Valid;
	}

	size_t GetDemoFilePos()
	{
		return m_DemoFilePos;
	}

	std::vector<unsigned char>& GetBuffer()
	{
		return m_Buffer;
	}

	int GetTick()
	{
		return m_Tick;
	}

	unsigned int GetCrc32()
	{
		return m_Crc32;
	}

private:
	bool m_Valid;
	size_t m_DemoFilePos;
	int m_Crc32;
	int m_Tick;
	std::vector<unsigned char> m_Buffer;
};


struct CDemoFileIndex
{
public:
	CDemoFileIndex()
	{

	}

	void StatPlayback(const char* fileName, bool asTimeDemo)
	{
		m_FileName = "afxDemoIndex";

		std::wstring wideString;

		if(!(AnsiStringToWideString(m_FileName.c_str(), wideString) && (CreatePath(wideString.c_str(), wideString)) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			Tier0_Warning("AFXERROR: Failed to create \"%s\" directory.\n", m_FileName.c_str());
		}

		m_FileName += "\\";
		m_FileName += fileName;
		m_FileName += ".adi";

		m_MaxPos = 0;
		m_IndexUpdated = false;
		m_TickToPos.clear();

		m_File = fopen(m_FileName.c_str(), "rb+");

		if (nullptr != m_File)
		{
			unsigned char size_tSize;
			unsigned int version;
			if (1 != fread(&size_tSize, sizeof(size_tSize), 1, m_File)
				|| (unsigned char)sizeof(m_MaxPos) != size_tSize
				|| 1 != fread(&version, sizeof(version), 1, m_File)
				|| version != m_Version
				|| 1 != fread(&m_MaxPos, sizeof(m_MaxPos), 1, m_File)
				|| m_MaxPos < sizeof(size_tSize) + sizeof(m_Version) + sizeof(m_MaxPos)
				|| 0 != fseek(m_File, m_MaxPos, SEEK_SET))
			{
				RecreateIndexFile();
			}
			else
			{
				for (size_t curPos = 0; curPos < m_MaxPos; ++curPos)
				{
					int tick;
					size_t filePos;

					if (1 != fread(&tick, sizeof(tick), 1, m_File)
						|| 1 != fread(&filePos, sizeof(filePos), 1, m_File))
					{
						RecreateIndexFile();
						break;
					}

					m_TickToPos[tick] = filePos;
				}
			}
		}
		else
			RecreateIndexFile();
	}

	void StopPlayback()
	{
		if (m_File)
		{
			if (m_IndexUpdated)
			{
				if (0 == fseek(m_File, m_MaxPos, SEEK_SET))
				{
					size_t size = m_TickToPos.size();
					if (1 == fwrite(&size, sizeof(size), 1, m_File))
					{
						for (auto it = m_TickToPos.begin(); it != m_TickToPos.end(); ++it)
						{
							if (1 != fwrite(&it->first, sizeof(it->first), 1, m_File)) break;
							if (1 != fwrite(&it->second, sizeof(it->second), 1, m_File)) break;
						}
					}

					if (0 == fseek(m_File, sizeof(unsigned char), SEEK_SET))
					{
						fwrite(&m_MaxPos, sizeof(m_MaxPos), 1, m_File);
					}
				}
			}
			fclose(m_File);
			m_File = nullptr;
		}
		m_Value.Invalidate();
	}

	void SkipToTick(int tick)
	{
		if (nullptr == m_File)
		{
			m_Value.Invalidate();
			return;
		}

		auto it = m_TickToPos.lower_bound(tick);
		if (it != m_TickToPos.end())
		{
			if (m_Value.ReadFromFile(m_File, it->first, it->second))
				return;
			else
				RecreateIndexFile();
		}

		m_Value.Invalidate();
	}

	CDemoFileIndexValue& GetValue()
	{
		return m_Value;
	}

	void IndexValue(CDemoFileIndexValue& value)
	{
		if (value.GetValid() && nullptr != m_File)
		{
			size_t indexPos;
			if (
				0 != fseek(m_File, m_MaxPos, SEEK_SET)
				|| !value.WriteToFile(m_File, indexPos)
				|| (m_MaxPos = ftell(m_File), m_MaxPos == -1L))
			{
				RecreateIndexFile();
			}
			else
			{
				m_TickToPos[value.GetTick()] = indexPos;
				m_IndexUpdated = true;
			}
		}
	}

	void RecreateIndexFile()
	{
		m_TickToPos.clear();
		m_IndexUpdated = true;
		if (nullptr != m_File)
		{
			unsigned char size_tSize = (unsigned char)sizeof(size_t);
			m_MaxPos = 0;
			m_MaxPos = sizeof(size_tSize) + sizeof(m_Version) + sizeof(m_MaxPos);
			if (0 != fseek(m_File, 0, SEEK_SET)
				|| 1 != fwrite(&size_tSize, sizeof(size_tSize), 1, m_File)
				|| 1 != fwrite(&m_Version, sizeof(m_Version), 1, m_File)
				|| 1 != fwrite(&m_MaxPos, sizeof(m_MaxPos), 1, m_File))
			{
				fclose(m_File);
				m_File = nullptr;
			}
		}
		else
		{
			m_File = fopen(m_FileName.c_str(), "wb+");
			if (nullptr != m_File)
			{
				unsigned char size_tSize = (unsigned char)sizeof(size_t);
				m_MaxPos = sizeof(size_tSize) + sizeof(m_Version) + sizeof(m_MaxPos);
				if (1 != fwrite(&size_tSize, sizeof(size_tSize), 1, m_File)
					|| 1 != fwrite(&m_Version, sizeof(m_Version), 1, m_File)
					|| 1 != fwrite(&m_MaxPos, sizeof(m_MaxPos), 1, m_File))
				{
					fclose(m_File);
					m_File = nullptr;
				}
			}
		}
	}

private:
	const unsigned int m_Version = 0;
	bool m_IndexUpdated;
	FILE* m_File;
	size_t m_MaxPos;
	CDemoFileIndexValue m_Value;
	std::map<int, size_t> m_TickToPos;
	std::string m_FileName;

} g_DemoFileIndex;

struct incomplete_cl_frame_t
{
	char unkown[8];
	int tick_count;
	struct incomplete_cl_frame_t* next;
};


//
// BEGIN Copyright © 1996-2005, Valve Corporation, All rights reserved.

class CGameServer;
class CBaseClient
{
};
class CGameClient : public CBaseClient
{
};
class CClientState;
class CClientFrame;
class bf_write;

//-----------------------------------------------------------------------------
// Used for serialization
//-----------------------------------------------------------------------------

#define LittleDWord( val )			( val )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )

inline uint32 LoadLittleDWord(uint32* base, unsigned int dwordIndex)
{
	return LittleDWord(base[dwordIndex]);
}

inline void StoreLittleDWord(uint32* base, unsigned int dwordIndex, uint32 dword)
{
	base[dwordIndex] = LittleDWord(dword);
}

inline int BitByte(int bits)
{
	// return PAD_NUMBER( bits, 8 ) >> 3;
	return (bits + 7) >> 3;
}

//

inline int BitForBitnum(int bitnum)
{
	return 1 << (bitnum & 31);
}

unsigned long g_BitWriteMasks[32][33];

// (1 << i) - 1
unsigned long g_ExtraMasks[32];

class CBitWriteMasksInit
{
public:
	CBitWriteMasksInit()
	{
		for (unsigned int startbit = 0; startbit < 32; startbit++)
		{
			for (unsigned int nBitsLeft = 0; nBitsLeft < 33; nBitsLeft++)
			{
				unsigned int endbit = startbit + nBitsLeft;
				g_BitWriteMasks[startbit][nBitsLeft] = BitForBitnum(startbit) - 1;
				if (endbit < 32)
					g_BitWriteMasks[startbit][nBitsLeft] |= ~(BitForBitnum(endbit) - 1);
			}
		}

		for (unsigned int maskBit = 0; maskBit < 32; maskBit++)
			g_ExtraMasks[maskBit] = BitForBitnum(maskBit) - 1;
	}
};
CBitWriteMasksInit g_BitWriteMasksInit;

//

class bf_write
{
public:
	bf_write();

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nMaxBits = -1);
	bf_write(const char* pDebugName, void* pData, int nBytes, int nMaxBits = -1);

	// Start writing to the specified buffer.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void			StartWriting(void* pData, int nBytes, int iStartBit = 0, int nMaxBits = -1);

	// Restart buffer writing.
	void			Reset();

	// Get the base pointer.
	unsigned char* GetBasePointer() { return m_pData; }

	// Enable or disable assertion on overflow. 99% of the time, it's a bug that we need to catch,
	// but there may be the occasional buffer that is allowed to overflow gracefully.
	void			SetAssertOnOverflow(bool bAssert);

	// This can be set to assign a name that gets output if the buffer overflows.
	const char* GetDebugName();
	void			SetDebugName(const char* pDebugName);


	// Seek to a specific position.
public:

	void			SeekToBit(int bitPos);


	// Bit functions.
public:

	void			WriteOneBit(int nValue);
	void			WriteOneBitNoCheck(int nValue);
	void			WriteOneBitAt(int iBit, int nValue);

	// Write signed or unsigned. Range is only checked in debug.
	void			WriteUBitLong(unsigned int data, int numbits, bool bCheckRange = true);
	void			WriteSBitLong(int data, int numbits);

	// Tell it whether or not the data is unsigned. If it's signed,
	// cast to unsigned before passing in (it will cast back inside).
	void			WriteBitLong(unsigned int data, int numbits, bool bSigned);

	// Write a list of bits in.
	bool			WriteBits(const void* pIn, int nBits);

	// writes an unsigned integer with variable bit length
	void			WriteUBitVar(unsigned int data);

	// Copy the bits straight out of pIn. This seeks pIn forward by nBits.
	// Returns an error if this buffer or the read buffer overflows.
	bool			WriteBitsFromBuffer(class bf_read* pIn, int nBits);

	void			WriteBitAngle(float fAngle, int numbits);
	void			WriteBitCoord(const float f);
	void			WriteBitCoordMP(const float f, EBitCoordType coordType);
	void 			WriteBitCellCoord(const float f, int bits, EBitCoordType coordType);
	void			WriteBitFloat(float val);
	void			WriteBitVec3Coord(const Vector& fa);
	void			WriteBitNormal(float f);
	void			WriteBitVec3Normal(const Vector& fa);
	void			WriteBitAngles(const QAngle& fa);


	// Byte functions.
public:

	void			WriteChar(int val);
	void			WriteByte(unsigned int val);
	void			WriteShort(int val);
	void			WriteWord(unsigned int val);
	void			WriteLong(long val);
	void			WriteLongLong(int64 val);
	void			WriteFloat(float val);
	bool			WriteBytes(const void* pBuf, int nBytes);

	// Returns false if it overflows the buffer.
	bool			WriteString(const char* pStr);
	bool			WriteString(const wchar_t* pStr);


	// Status.
public:

	// How many bytes are filled in?
	int				GetNumBytesWritten();
	int				GetNumBitsWritten();
	int				GetMaxNumBits();
	int				GetNumBitsLeft();
	int				GetNumBytesLeft();
	unsigned char* GetData();

	// Has the buffer overflowed?
	bool			CheckForOverflow(int nBits);
	inline bool		IsOverflowed() const { return m_bOverflow; }

	inline void		SetOverflowFlag();


public:
	// The current buffer.
	unsigned char* m_pData;
	int				m_nDataBytes;
	int				m_nDataBits;

	// Where we are in the buffer.
	int				m_iCurBit;

private:

	// Errors?
	bool			m_bOverflow;

	bool			m_bAssertOnOverflow;
	const char* m_pDebugName;
};


//-----------------------------------------------------------------------------
// Inlined methods
//-----------------------------------------------------------------------------

// How many bytes are filled in?
inline int bf_write::GetNumBytesWritten()
{
	return BitByte(m_iCurBit);
}

inline int bf_write::GetNumBitsWritten()
{
	return m_iCurBit;
}

inline int bf_write::GetMaxNumBits()
{
	return m_nDataBits;
}

inline int bf_write::GetNumBitsLeft()
{
	return m_nDataBits - m_iCurBit;
}

inline int bf_write::GetNumBytesLeft()
{
	return GetNumBitsLeft() >> 3;
}

inline unsigned char* bf_write::GetData()
{
	return m_pData;
}

inline bool bf_write::CheckForOverflow(int nBits)
{
	if (m_iCurBit + nBits > m_nDataBits)
	{
		SetOverflowFlag();
		//CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
	}

	return m_bOverflow;
}

inline void bf_write::SetOverflowFlag()
{
	if (m_bAssertOnOverflow)
	{
		Assert(false);
	}

	m_bOverflow = true;
}

inline void bf_write::WriteOneBitNoCheck(int nValue)
{
	if (nValue)
		m_pData[m_iCurBit >> 3] |= (1 << (m_iCurBit & 7));
	else
		m_pData[m_iCurBit >> 3] &= ~(1 << (m_iCurBit & 7));

	++m_iCurBit;
}

inline void bf_write::WriteOneBit(int nValue)
{
	if (!CheckForOverflow(1))
		WriteOneBitNoCheck(nValue);
}


inline void	bf_write::WriteOneBitAt(int iBit, int nValue)
{
	if (iBit + 1 > m_nDataBits)
	{
		SetOverflowFlag();
		//CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return;
	}

	if (nValue)
		m_pData[iBit >> 3] |= (1 << (iBit & 7));
	else
		m_pData[iBit >> 3] &= ~(1 << (iBit & 7));
}


inline void bf_write::WriteUBitLong(unsigned int curData, int numbits, bool bCheckRange)
{
#ifdef _DEBUG
	// Make sure it doesn't overflow.
	if (bCheckRange && numbits < 32)
	{
		if (curData >= (unsigned long)(1 << numbits))
		{
			CallErrorHandler(BITBUFERROR_VALUE_OUT_OF_RANGE, GetDebugName());
		}
	}
	Assert(numbits >= 0 && numbits <= 32);
#endif

	// Bounds checking..
	if ((m_iCurBit + numbits) > m_nDataBits)
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		//CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return;
	}

	int nBitsLeft = numbits;
	int iCurBit = m_iCurBit;

	// Mask in a dword.
	unsigned int iDWord = iCurBit >> 5;
	Assert((iDWord * 4 + sizeof(long)) <= (unsigned int)m_nDataBytes);

	unsigned long iCurBitMasked = iCurBit & 31;

	uint32 dword = LoadLittleDWord((uint32*)m_pData, iDWord);

	dword &= g_BitWriteMasks[iCurBitMasked][nBitsLeft];
	dword |= curData << iCurBitMasked;

	// write to stream (lsb to msb ) properly
	StoreLittleDWord((uint32*)m_pData, iDWord, dword);

	// Did it span a dword?
	int nBitsWritten = 32 - iCurBitMasked;
	if (nBitsWritten < nBitsLeft)
	{
		nBitsLeft -= nBitsWritten;
		curData >>= nBitsWritten;

		// read from stream (lsb to msb) properly 
		dword = LoadLittleDWord((uint32*)m_pData, iDWord + 1);

		dword &= g_BitWriteMasks[0][nBitsLeft];
		dword |= curData;

		// write to stream (lsb to msb) properly 
		StoreLittleDWord((uint32*)m_pData, iDWord + 1, dword);
	}

	m_iCurBit += numbits;
}

//

bf_write::bf_write()
{
	m_pData = NULL;
	m_nDataBytes = 0;
	m_nDataBits = -1; // set to -1 so we generate overflow on any operation
	m_iCurBit = 0;
	m_bOverflow = false;
	m_bAssertOnOverflow = true;
	m_pDebugName = NULL;
}

bf_write::bf_write(const char* pDebugName, void* pData, int nBytes, int nBits)
{
	m_bAssertOnOverflow = true;
	m_pDebugName = pDebugName;
	StartWriting(pData, nBytes, 0, nBits);
}

bf_write::bf_write(void* pData, int nBytes, int nBits)
{
	m_bAssertOnOverflow = true;
	m_pDebugName = NULL;
	StartWriting(pData, nBytes, 0, nBits);
}

void bf_write::StartWriting(void* pData, int nBytes, int iStartBit, int nBits)
{
	// The writing code will overrun the end of the buffer if it isn't dword aligned, so truncate to force alignment
	nBytes &= ~3;

	m_pData = (unsigned char*)pData;
	m_nDataBytes = nBytes;

	if (nBits == -1)
	{
		m_nDataBits = nBytes << 3;
	}
	else
	{
		m_nDataBits = nBits;
	}

	m_iCurBit = iStartBit;
	m_bOverflow = false;
}

void bf_write::Reset()
{
	m_iCurBit = 0;
	m_bOverflow = false;
}

void bf_write::SeekToBit(int bitPos)
{
	m_iCurBit = bitPos;
}

bool bf_write::WriteBits(const void* pInData, int nBits)
{
	unsigned char* pOut = (unsigned char*)pInData;
	int nBitsLeft = nBits;

	if ((m_iCurBit + nBits) > m_nDataBits)
	{
		SetOverflowFlag();
		return false;
	}

	while (((unsigned long)pOut & 3) != 0 && nBitsLeft >= 8)
	{

		WriteUBitLong(*pOut, 8, false);
		++pOut;
		nBitsLeft -= 8;
	}

	if ((nBitsLeft >= 32) && (m_iCurBit & 7) == 0)
	{
		int numbytes = nBitsLeft >> 3;
		int numbits = numbytes << 3;

		memcpy(m_pData + (m_iCurBit >> 3), pOut, numbytes);
		pOut += numbytes;
		nBitsLeft -= numbits;
		m_iCurBit += numbits;
	}

	if (nBitsLeft >= 32)
	{
		unsigned long iBitsRight = (m_iCurBit & 31);
		unsigned long iBitsLeft = 32 - iBitsRight;
		unsigned long bitMaskLeft = g_BitWriteMasks[iBitsRight][32];
		unsigned long bitMaskRight = g_BitWriteMasks[0][iBitsRight];

		unsigned long* pData = &((unsigned long*)m_pData)[m_iCurBit >> 5];

		while (nBitsLeft >= 32)
		{
			unsigned long curData = *(unsigned long*)pOut;
			pOut += sizeof(unsigned long);

			*pData &= bitMaskLeft;
			*pData |= curData << iBitsRight;

			pData++;

			if (iBitsLeft < 32)
			{
				curData >>= iBitsLeft;
				*pData &= bitMaskRight;
				*pData |= curData;
			}

			nBitsLeft -= 32;
			m_iCurBit += 32;
		}
	}

	while (nBitsLeft >= 8)
	{
		WriteUBitLong(*pOut, 8, false);
		++pOut;
		nBitsLeft -= 8;
	}

	if (nBitsLeft)
	{
		WriteUBitLong(*pOut, nBitsLeft, false);
	}

	return !IsOverflowed();
}

bool bf_write::WriteBytes(const void* pBuf, int nBytes)
{
	return WriteBits(pBuf, nBytes << 3);
}

void bf_write::WriteUBitVar(unsigned int data)
{
	if ((data & 0xf) == data)
	{
		WriteUBitLong(0, 2);
		WriteUBitLong(data, 4);
	}
	else
	{
		if ((data & 0xff) == data)
		{
			WriteUBitLong(1, 2);
			WriteUBitLong(data, 8);
		}
		else
		{
			if ((data & 0xfff) == data)
			{
				WriteUBitLong(2, 2);
				WriteUBitLong(data, 12);
			}
			else
			{
				WriteUBitLong(0x3, 2);
				WriteUBitLong(data, 32);
			}
		}
	}
}

// END Copyright © 1996-2005, Valve Corporation, All rights reserved.
//


bool g_bGotoTick = false;

typedef void(__fastcall* CDemoPlayer_SkipToTick_t)(void* This, void* Edx, int tick, bool bRelative, bool bPause);

CDemoPlayer_SkipToTick_t True_CDemoPlayer_SkipToTick;

int g_bInSkipToTick = 0;
int g_bLastGetTick = 0;

typedef int(__fastcall* CDemoPlayer_GetPlaybackTick_t)(void* This, void* Edx);

CDemoPlayer_GetPlaybackTick_t True_CDemoPlayer_GetPlaybackTick;

int __fastcall My_CDemoPlayer_GetPlaybackTick(void* This, void* Edx)
{
	int curTick = True_CDemoPlayer_GetPlaybackTick(This, Edx);

	if (0 < g_bInSkipToTick)
	{
		if (1 == g_bInSkipToTick)
		{
			g_bLastGetTick = curTick;
			//return 0; // HUE HUE HUE
		}

		--g_bInSkipToTick;
	}

	return curTick;
}

void __fastcall My_CDemoPlayer_SkipToTick(void* This, void* Edx, int tick, bool bRelative, bool bPause)
{
	g_bInSkipToTick = bRelative ? 2 : 1;
	True_CDemoPlayer_SkipToTick(This, Edx, tick, bRelative, bPause);
	g_bInSkipToTick = 0;

	int actual_tick = *(DWORD*)((const char*)This + 0x5BC);

	if (-1 != actual_tick && 0 < (actual_tick & 0x00ffffff))
	{
		tick = actual_tick & 0x00ffffff;
		g_DemoFileIndex.SkipToTick(actual_tick);

		g_bGotoTick = g_DemoFileIndex.GetValue().GetValid();
	}
}

const size_t myTempBufSize = 30000;
const size_t myWriteBufSize = 1048576;
void* myWriteBuf = nullptr;
bool g_bHadFirstPackeEntites = false;

typedef bool(__fastcall* CDemoPlayer_StartPlayback_t)(void* This, void* Edx, const char* filename, bool bAsTimeDemo, void* unk1, void* unk2);

CDemoPlayer_StartPlayback_t True_CDemoPlayer_StartPlayback;

bool __fastcall My_CDemoPlayer_StartPlayback(void* This, void* Edx, const char* filename, bool bAsTimeDemo, void* unk1, void* unk2)
{
	g_bHadFirstPackeEntites = false;

	bool result = True_CDemoPlayer_StartPlayback(This, Edx, filename, bAsTimeDemo, unk1, unk2);

	if (result)
	{
		g_DemoFileIndex.StatPlayback(filename, bAsTimeDemo);
	}

	if (!myWriteBuf) myWriteBuf = malloc(myWriteBufSize); // 3/2 of csgo

	return result;
}


typedef void(__fastcall* CDemoPlayer_StopPlayback_t)(void* This, void* Edx);

CDemoPlayer_StopPlayback_t True_CDemoPlayer_StopPlayback;

void __fastcall My_CDemoPlayer_StopPlayback(void* This, void* Edx)
{
	g_bGotoTick = false;

	g_DemoFileIndex.StopPlayback();

	True_CDemoPlayer_StopPlayback(This, Edx);

	if(myWriteBuf) free(myWriteBuf);
	myWriteBuf = nullptr;
}

class CClientFrame* GetClientFrame(CClientState * cl, int tick)
{
	incomplete_cl_frame_t** pFrames = (incomplete_cl_frame_t**)(*(char**)cl + 0x4CDC);
	incomplete_cl_frame_t* f = *pFrames;

	while (f)
	{
		if (f->tick_count >= tick)
		{
			if (f->tick_count == tick) return (CClientFrame*)f;
			break;
		}

		f = f->next;
	}

	return nullptr;
}

class INetMessage
{
public:
	// just so we can force the vtable.
	virtual void Dummy()
	{

	}
};

class CNetMessagePB_SVCMsg_PacketEntities : public INetMessage, public CSVCMsg_PacketEntities
{
public:
};

typedef void(__fastcall* WriteDeltaEntities_t)(void* This, CGameClient* gameClient, CClientFrame* to, CClientFrame* from, CNetMessagePB_SVCMsg_PacketEntities* msg, unsigned char* pBuf, int & inOutSize);

typedef CGameClient* (__fastcall* CBaseServer_CreateFakeClient_t)(void* This, void* Edx, const char* name);
typedef CGameClient* (__fastcall* CGameServer_CreateNewClient_t)(void* This, void* Edx, int slot);


extern HMODULE g_H_EngineDll;

bool WriteFullUpdate(CNetMessagePB_SVCMsg_PacketEntities* msg, unsigned char * buffer, int & inOutSize, int tick)
{
	CClientState* cl = (CClientState*)AFXADDR_GET(csgo_cl);
	CGameServer* sv = (CGameServer* )AFXADDR_GET(csgo_sv);
	CGameClient** vecGameClient0 = *(CGameClient***)((char*)sv + 0x274);

	if (!cl || !sv) return false;

	class CClientFrame* clientFrame = GetClientFrame(cl, tick);

	Tier0_Msg("AFXINFO: GetClientFrame: %i -> 0x%08x\n", tick, clientFrame);

	if (!clientFrame) return false;

	if (!vecGameClient0)
	{
		Tier0_Warning("AFXERROR: Game client in slot 0 not found, state error!\n");
		return false;
		/*
		Tier0_Msg("AFXINFO: Creating fake \"advancedfx\" client ...\n");

		//CBaseServer_CreateFakeClient_t createFakeClient = *(CBaseServer_CreateFakeClient_t *)(*(char**)sv + 0xB8);
		//gameClient0 = createFakeClient((char*)sv, 0, "advancedfx");
		CGameServer_CreateNewClient_t createNewClient = *(CGameServer_CreateNewClient_t*)(*(char**)sv + 0x0e0);
		gameClient0 = createNewClient(sv, 0, 0);

		if (!gameClient0) {
			Tier0_Warning("AFXERROR: Failed to create new client on server.\n");
			return false;
		}
		*/
	}

	Tier0_Msg("AFXINFO: WriteDeltaEntities ...\n", tick, clientFrame);

	//WriteDeltaEntities_t writeDeltaEntities = (WriteDeltaEntities_t)AFXADDR_GET(csgo_WriteDeltaEntities);
	//writeDeltaEntities(sv, gameClient0, clientFrame, nullptr, msg, buffer, inOutSize);

	void* writeDeltaEntities = (void*)AFXADDR_GET(csgo_WriteDeltaEntities);

	// framesnapshotmanager->TakeTickSnapshot( m_nTickCount )
	__asm mov eax, tick
	__asm push tick
	__asm mov ecx, g_H_EngineDll
	__asm add ecx, 0x1966A0
	__asm call ecx
	__asm mov ecx, clientFrame
	__asm mov [ecx + 0x118], eax

	/*
	__asm mov eax, inOutSize
	__asm push eax

	__asm mov eax, buffer
	__asm push eax

	__asm mov eax, msg
	__asm push eax

	__asm push 0
	
	__asm mov eax, clientFrame
	__asm push eax

	__asm mov edx, vecGameClient0
	__asm mov edx, [edx]
	
	__asm mov ecx, sv

	__asm mov eax, writeDeltaEntities
	__asm call eax

	__asm add esp, 0x14
	*/

	return true;
}


namespace SOURCESDK {
	namespace CSGO {

		typedef void netadr_t;

		class INetChannel;

		class INetChannelHandler
		{
		public:
			virtual	~INetChannelHandler(void) {};

			virtual void ConnectionStart(INetChannel* chan) = 0;	// called first time network channel is established

			virtual void Unknown_002() = 0; // new

			virtual void ConnectionClosing(const char* reason) = 0; // network channel is being closed by remote site

			virtual void ConnectionCrashed(const char* reason) = 0; // network error occured

			virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

			virtual void PacketEnd(void) = 0; // all messages has been parsed

			virtual void FileRequested(const char* fileName, unsigned int transferID, bool isReplayDemoFile) = 0; // other side request a file for download

			virtual void FileReceived(const char* fileName, unsigned int transferID, bool isReplayDemoFile) = 0; // we received a file

			virtual void FileDenied(const char* fileName, unsigned int transferID, bool isReplayDemoFile) = 0;	// a file request was denied by other side

			virtual void FileSent(const char* fileName, unsigned int transferID, bool isReplayDemoFile) = 0;	// we sent a file

			virtual bool SetActiveChannel(int slot) = 0; // new
		};

		class CAfxNetChannelSplitter : public INetChannelHandler
		{
		public:
			CAfxNetChannelSplitter(INetChannelHandler* a, INetChannelHandler* b)
				: m_A(a)
				, m_B(b)
			{

			}
			
			virtual	~CAfxNetChannelSplitter(void) {
				delete m_A;
				delete m_B;
			};

			virtual void ConnectionStart(INetChannel* chan)
			{
				m_A->ConnectionStart(chan);
				m_B->ConnectionStart(chan);
			}

			virtual void Unknown_002()
			{
				m_A->Unknown_002();
				m_B->Unknown_002();
			}

			virtual void ConnectionClosing(const char* reason)
			{
				m_A->ConnectionClosing(reason);
				m_B->ConnectionClosing(reason);
			}

			virtual void ConnectionCrashed(const char* reason)
			{
				m_A->ConnectionCrashed(reason);
				m_B->ConnectionCrashed(reason);
			}

			virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged)
			{
				m_A->PacketStart(incoming_sequence, outgoing_acknowledged);
				m_B->PacketStart(incoming_sequence, outgoing_acknowledged);
			}

			virtual void PacketEnd(void)
			{
				m_A->PacketEnd();
				m_B->PacketEnd();
			}

			virtual void FileRequested(const char* fileName, unsigned int transferID, bool isReplayDemoFile)
			{
				m_A->FileRequested(fileName, transferID, isReplayDemoFile);
				m_B->FileRequested(fileName, transferID, isReplayDemoFile);
			}

			virtual void FileReceived(const char* fileName, unsigned int transferID, bool isReplayDemoFile)
			{
				m_A->FileReceived(fileName, transferID, isReplayDemoFile);
				m_B->FileReceived(fileName, transferID, isReplayDemoFile);
			}

			virtual void FileDenied(const char* fileName, unsigned int transferID, bool isReplayDemoFile)
			{
				m_A->FileDenied(fileName, transferID, isReplayDemoFile);
				m_B->FileDenied(fileName, transferID, isReplayDemoFile);
			}

			virtual void FileSent(const char* fileName, unsigned int transferID, bool isReplayDemoFile)
			{
				m_A->FileSent(fileName, transferID, isReplayDemoFile);
				m_B->FileSent(fileName, transferID, isReplayDemoFile);
			}

			virtual bool SetActiveChannel(int slot)
			{
				return m_A->SetActiveChannel(slot) && m_B->SetActiveChannel(slot);
			}

		private:
			INetChannelHandler* m_A;
			INetChannelHandler* m_B;
		};
	}
}

typedef struct netpacket_s
{
	unsigned char dummy[0x30];
	unsigned char* data;
	unsigned char dummy2[0x0C];
	int size;
	int wiresize;

} netpacket_t;

typedef netpacket_t*(__fastcall* CDemoPlayer_ReadPacket_t)(void* This, void* Edx);

CDemoPlayer_ReadPacket_t True_CDemoPlayer_ReadPacket;

bool g_bFirstReadCmdHeader = false;

netpacket_t* g_LastPacket;
unsigned char* g_LastData = nullptr;

netpacket_t* __fastcall My_CDemoPlayer_ReadPacket(void* This, void* Edx)
{
	if (g_LastData)
	{
		g_LastPacket->data = g_LastData;
		g_LastData = nullptr;
	}

	if (g_LastPacket)
	{
		CDemoFileIndexValue idx = g_DemoFileIndex.GetValue();

		if (idx.GetValid())
		{
			int tick = idx.GetTick();

			Tier0_Msg("AFX: Building index packet data for %i.\n", tick);

			CBitRead buf(g_LastPacket->data, g_LastPacket->size);
			bf_write bfww(myWriteBuf, myWriteBufSize);

			bool fullUpdateWritten = false;

			while (buf.GetNumBytesRead() < g_LastPacket->size)
			{
				int Cmd = buf.ReadVarInt32();

				bfww.WriteUBitVar(Cmd);

				int Size = buf.ReadVarInt32();

				if (buf.GetNumBytesRead() + Size <= g_LastPacket->size)
				{
					bool handled = false;

					if (!fullUpdateWritten && Cmd == 26) // svc_PacketEntities
					{
						CSVCMsg_PacketEntities msg;

						if (msg.ParseFromArray(buf.GetBasePointer() + buf.GetNumBytesRead(), Size))
						{
							unsigned char* tempBuf = (unsigned char*)malloc(myWriteBufSize);
							int size = myTempBufSize;

							CNetMessagePB_SVCMsg_PacketEntities msg2;

							WriteFullUpdate(&msg2, tempBuf, size, idx.GetTick());

							bfww.WriteUBitVar(size);
							bfww.WriteBytes(tempBuf, size);

							Tier0_Msg("AFXINFO: WriteFullUpdate: %i (%i)\n", size, Size);

							free(tempBuf);

							handled = true;
							fullUpdateWritten = true;
						}
					}

					if (!handled)
					{
						bfww.WriteUBitVar(Size);
						bfww.WriteBytes(buf.GetBasePointer() + buf.GetNumBytesRead(), Size);
					}

					buf.SeekRelative(Size * 8);
				}
				else break;
			}

			idx.GetBuffer().resize(bfww.GetNumBytesWritten());
			memcpy(&(*idx.GetBuffer().begin()), bfww.GetBasePointer(), bfww.GetNumBytesWritten());
			g_DemoFileIndex.IndexValue(idx);
		}
	}

	g_LastPacket = nullptr;

	void* pDemoFile = (void*)((char*)This + 0x4);
	void* pBuffer = pDemoFile ? *(void**)((char*)pDemoFile + 0x538) : nullptr;

	if (!pBuffer)
	{
		return True_CDemoPlayer_ReadPacket(This, Edx);
	}

	typedef bool(__fastcall* CDemoFile_m_Buffer_IsOpen_t)(void* This, void* Edx);
	CDemoFile_m_Buffer_IsOpen_t bufferIsOpen = (CDemoFile_m_Buffer_IsOpen_t)((*(void***)pBuffer)[3]);

	if (!bufferIsOpen(pBuffer, 0))
	{
		return True_CDemoPlayer_ReadPacket(This, Edx);
	}

	typedef int(__fastcall* CDemoFile_m_Buffer_TellGet_t)(void* This, void* Edx);
	CDemoFile_m_Buffer_TellGet_t bufferTellGet = (CDemoFile_m_Buffer_TellGet_t)((*(void***)pBuffer)[11]);

	typedef void(__fastcall* CDemoFile_m_Buffer_SeekGet_t)(void* This, void* Edx, int direction, int size);
	CDemoFile_m_Buffer_SeekGet_t bufferSeekGet = (CDemoFile_m_Buffer_SeekGet_t)((*(void***)pBuffer)[9]);

	//int* p_servertickcount = (int*)(*(char**)AFXADDR_GET(m_ClockDriftMgr) + 0x16c);
	//int* p_clienttickcount = (int*)(*(char**)AFXADDR_GET(m_ClockDriftMgr) + 0x170);

	if (g_bGotoTick && g_bHadFirstPackeEntites && g_DemoFileIndex.GetValue().GetValid())
	{
		typedef void(__fastcall* CDemoFile_m_Buffer_SeekGet_t)(void* This, void* Edx, int direction, int size);
		CDemoFile_m_Buffer_SeekGet_t bufferSeekGet = (CDemoFile_m_Buffer_SeekGet_t)((*(void***)pBuffer)[9]);

		bufferSeekGet(pBuffer, 0, 1, g_DemoFileIndex.GetValue().GetDemoFilePos());
	}

	int bufferPos = bufferTellGet(pBuffer, 0);

	//int preCallTick = *p_servertickcount;

	netpacket_t* packet = True_CDemoPlayer_ReadPacket(This, Edx);

	if (g_bGotoTick && g_bHadFirstPackeEntites && g_DemoFileIndex.GetValue().GetValid() && packet)
	{
		g_bGotoTick = false;

		unsigned int crc32 = ComputeCrc32(packet->data, packet->size);

		if (crc32 == g_DemoFileIndex.GetValue().GetCrc32())
		{
			packet->data = (unsigned char*) & (*g_DemoFileIndex.GetValue().GetBuffer().begin());
			packet->size = g_DemoFileIndex.GetValue().GetBuffer().size();
			return packet;
		}
		else Tier0_Warning("AFXERROR: CRC MISMATCH\n");

		g_DemoFileIndex.RecreateIndexFile();
	}

	if (0 < g_bFixCDemoFileTicks)
	{
		if (packet)
		{
			CBitRead buf(packet->data, packet->size);

			bool firstInPacket = true;
			bool hasNetTick = false;
			int netTick;

			bool writing = false;

			while (buf.GetNumBytesRead() < packet->size)
			{
				int Cmd = buf.ReadVarInt32();
				int Size = buf.ReadVarInt32();

				bool written = false;

				if (buf.GetNumBytesRead() + Size <= packet->size)
				{
					if (Cmd == 4) // net_Tick
					{
						CNETMsg_Tick msg;

						if (msg.ParseFromArray(buf.GetBasePointer() + buf.GetNumBytesRead(), Size) && msg.has_tick())
						{
							hasNetTick = true;
							netTick = msg.tick();
						}
						else
							hasNetTick = false;
					}
					if (Cmd == 26) // svc_PacketEntities
					{
						CSVCMsg_PacketEntities msg;

						if (g_bHadFirstPackeEntites)
						{

							if (msg.ParseFromArray(buf.GetBasePointer() + buf.GetNumBytesRead(), Size))
							{
								if (hasNetTick)
								{
									CDemoFileIndexValue& indexVal = g_DemoFileIndex.GetValue();

									if (!indexVal.GetValid() || netTick > indexVal.GetTick() && netTick - indexVal.GetTick() >= g_bFixCDemoFileTicks)
									{
										// New index frame:

										if (firstInPacket)
										{
											Tier0_Msg("AFX: New Index tick at %i.\n", netTick);

											g_LastPacket = packet;
											indexVal.Init(netTick, bufferPos, ComputeCrc32(packet->data, packet->size));
										}
									}
								}
							}
						}
						else g_bHadFirstPackeEntites = true;

						firstInPacket = false;
					}

					buf.SeekRelative(Size * 8);
				}
				else break;
			}
		}
	}

	return packet;
}

/*

void* g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueYes;
void* g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueNo;

bool __fastcall My_CClientFrameManager_DeleteClientFrames_Comparison_Do(int frame_tick_count, int nTick)
{
	auto it = g_KeepFames.find(frame_tick_count);
	if (it != g_KeepFames.end())
	{
		//Tier0_Warning("KEEPING FRAME: %i\n", frame_tick_count);
		return false;
	}

	return 0 == g_bInSkipToTick && !g_bGotoTick && frame_tick_count < nTick;
}

void __declspec(naked) My_CClientFrameManager_DeleteClientFrames_Comparison(void)
{
	__asm push eax
	__asm push edx
	__asm push ecx
	__asm mov edx, eax
	__asm mov ecx, [esi + 8]
	__asm call My_CClientFrameManager_DeleteClientFrames_Comparison_Do
	__asm cmp al, 0
	__asm pop ecx
	__asm pop edx
	__asm pop eax
	__asm jz _No
	__asm jmp [g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueYes]
	__asm _No:
	__asm jmp [g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueNo]
}


void* g_csgo_CL_ProcessPacketEntities_AddClientFrame_Continue;

void __fastcall My_csgo_CL_ProcessPacketEntities_AddClientFrame_Do(incomplete_cl_frame_t* frame)
{
	CClientState * cl = (CClientState*)AFXADDR_GET(csgo_cl);;

	incomplete_cl_frame_t** pFrames = (incomplete_cl_frame_t**)(*(char**)cl + 0x4CDC);

	incomplete_cl_frame_t* f = nullptr;
	
	
	//bool keptFrameAdd = false;
	//
	//auto it = g_KeepFames.lower_bound(frame->tick_count);
	//
	//if (it != g_KeepFames.end())
	//{
	//	f = it->second;
	//
	//	if (it->first == frame->tick_count && it->second == nullptr)
	//	{
	//		it->second = frame;
	//
	//	}
	//}
	
	if(f == nullptr)
	{
		f = *pFrames;
	}

	if (nullptr == f)
	{
		*pFrames = frame;
		return;
	}

	while (f->next && f->next->tick_count <= frame->tick_count) f = f->next;

	frame->next = f->next;
	f->next = frame;

	if (g_LastPacket)
	{
		CDemoFileIndexValue& idx = g_DemoFileIndex.GetValue();

		int tick;

		__asm mov eax, frame
		__asm mov eax, [eax + 0x8]
		__asm mov tick, eax

		
	}
}


void __declspec(naked) My_csgo_CL_ProcessPacketEntities_AddClientFrame(void)
{
	__asm push eax
	__asm push ecx
	__asm push edx
	__asm mov ecx, ebx

	__asm call My_csgo_CL_ProcessPacketEntities_AddClientFrame_Do

	__asm pop edx
	__asm pop ecx
	__asm pop eax

	__asm jmp [g_csgo_CL_ProcessPacketEntities_AddClientFrame_Continue]
}
*/

typedef int(__fastcall* csgo_CL_ProcessPacketEntities_AddClientFrame_t)(void* This, void* Edx, CClientFrame* frame);

csgo_CL_ProcessPacketEntities_AddClientFrame_t True_csgo_CL_ProcessPacketEntities_AddClientFrame;

typedef SOURCESDK::CSGO::INetChannel* (__fastcall * csgo_NET_CreateNetChannel_t)(int socketnumber, SOURCESDK::CSGO::netadr_t* adr, const char* name, SOURCESDK::CSGO::INetChannelHandler* handler, bool bForceNew, bool bUnknown);

void *  True_csgo_NET_CreateNetChannel;

SOURCESDK::CSGO::INetChannel* Call_True_csgo_NET_CreateNetChannel(int socketnumber, SOURCESDK::CSGO::netadr_t* adr, const char* name, SOURCESDK::CSGO::INetChannelHandler* handler, bool bForceNew = false, bool bUnknown = false)
{
	SOURCESDK::CSGO::INetChannel* result;

	__asm mov ecx, socketnumber
	__asm mov edx, adr
	__asm movzx eax, bUnknown
	__asm push eax
	__asm movzx eax, bForceNew
	__asm push eax
	__asm mov eax, handler
	__asm push eax
	__asm mov eax, name
	__asm push eax
	__asm call True_csgo_NET_CreateNetChannel
	__asm mov result, eax
	__asm add esp, 0x10

	return result;
}


typedef CBaseClient* (__fastcall* CBaseServer_GetFreeClient_t)(void* This, void* Edx, SOURCESDK::CSGO::netadr_t * adr);

typedef void(__fastcall* CBaseServer_SetMaxClients_t)(void* This, void* Edx, int number);

SOURCESDK::CSGO::INetChannel* __fastcall My_csgo_NET_CreateNetChannel(int socketnumber, SOURCESDK::CSGO::netadr_t* adr, const char* name, SOURCESDK::CSGO::INetChannelHandler* handler, bool bForceNew = false, bool bUnknown = false)
{
	if (0 == strcmp(name, "DEMO"))
	{
		Tier0_Msg("AFXINFO: NET_CreateNetChannel for DEMO, splitting the handler ...\n");

		CGameServer* sv = (CGameServer*)AFXADDR_GET(csgo_sv);
		CGameClient* gameClient0 = *(CGameClient**)((char*)sv + 0x274);

		if (!gameClient0)
		{
			Tier0_Msg("AFXINFO: Creating dummy server and client ...\n");

			const char* oldMap = "";
			const char* newMap = "de_mirage";

			bool result;

			// void Host_NewGame(bool bDedicated);
			/*
			__asm push 0 
			__asm push 0
			__asm push 0
			__asm push 0
			__asm push 0

			__asm mov edx, oldMap
			__asm mov ecx, newMap

			__asm mov eax, g_H_EngineDll
			__asm add eax, 0x217750
			__asm call eax
			__asm test eax, eax
			__asm setnz hostNewGameResult
			__asm add esp, 0x14
			*/


			for (int i = 0; i < 3; ++i)
			{
				__asm push 0

				__asm push 0

				__asm mov eax, newMap
				__asm push eax

				__asm mov ecx, sv

				__asm mov eax, g_H_EngineDll
				__asm add eax, 0x19FBC0
				__asm call eax
				__asm test eax, eax
				__asm setnz result

				if (!result)
				{
					Tier0_Warning("AFXERROR: CGameServer::SpawnServer failed.\n");
					return nullptr;
				}
			}
			
			//CBaseServer_CreateFakeClient_t createFakeClient = *(CBaseServer_CreateFakeClient_t *)(*(char**)sv + 0xB8);
			//gameClient0 = createFakeClient(sv, 0, "advancedfx");

			//CGameServer_CreateNewClient_t createNewClient = *(CGameServer_CreateNewClient_t*)(*(char**)sv + 0x0e0);
			//gameClient0 = createNewClient(sv, 0, 0);

			/*
			Tier0_Msg("AFXINFO: CGameServer::SetMaxClients(1) ...\n");

			CBaseServer_SetMaxClients_t setMaxClients = *(CBaseServer_SetMaxClients_t*)(*(char**)sv + 0x0a0);

			setMaxClients((char*)sv, 0, 1);
			*/
			
			Tier0_Msg("AFXINFO: CGameServer::GetFreeClient ...\n");

			CBaseServer_GetFreeClient_t getFreeClient = (CBaseServer_GetFreeClient_t)AFXADDR_GET(csgo_CBaseServer_GetFreeCLient);
			
			gameClient0 = (CGameClient *)getFreeClient(sv, 0, adr);

			if (!gameClient0) {
				Tier0_Warning("AFXERROR: CGameServer::GetFreeClient failed.\n");
				return nullptr;
			}

			//*(CGameClient**)((char*)sv + 0x274) = gameClient0;

			Tier0_Warning("AFXINFO: Continuing with NET_CreateNetChannel ...\n");
		}

		//return Call_True_csgo_NET_CreateNetChannel(socketnumber, adr, name, new SOURCESDK::CSGO::CAfxNetChannelSplitter(handler, (SOURCESDK::CSGO::INetChannelHandler*)((char*)gameClient0 + 0x04)), bForceNew, bUnknown);
		return Call_True_csgo_NET_CreateNetChannel(socketnumber, adr, name, (SOURCESDK::CSGO::INetChannelHandler*)((char*)gameClient0 + 0x04), bForceNew, bUnknown);
	}

	return Call_True_csgo_NET_CreateNetChannel(socketnumber, adr, name, handler, bForceNew, bUnknown);
}

void __declspec(naked) Wrap_My_csgo_NET_CreateNetChannel(void)
{
	// We are called like __fastcall, but not caller pops the args
	__asm sub esp, 0x10
	__asm mov eax, [esp + 0x4 + 0x10 + 0x0c]
	__asm mov [esp + 0x0c], eax
	__asm mov eax, [esp + 0x4 + 0x10 + 0x8]
	__asm mov [esp + 0x8], eax
	__asm mov eax, [esp + 0x4 + 0x10 + 0x4]
	__asm mov [esp + 0x4], eax
	__asm mov eax, [esp + 0x4 + 0x10 + 0x0]
	__asm mov [esp + 0x0], eax
	__asm call My_csgo_NET_CreateNetChannel
	__asm ret
}

bool Hook_csgo_DemoFile(void)
{
	return false;

	static bool firstResult = false;
	static bool firstRun = true;
	if (!firstRun) return firstResult;
	firstRun = false;

	if (AFXADDR_GET(csgo_CDemoPlayer)
		//&& AFXADDR_GET(csgo_CL_ProcessPacketEntities_AddClientFrame)
		&& AFXADDR_GET(csgo_sv)
		&& AFXADDR_GET(csgo_cl)
//		&& AFXADDR_GET(csgo_host_client)
		&&AFXADDR_GET(csgo_WriteDeltaEntities)
		&&AFXADDR_GET(csgo_NET_CreateNetChannel)
		&& AFXADDR_GET(csgo_CBaseServer_GetFreeCLient))
	{
		LONG error = NO_ERROR;

		DWORD* vtableCdemoPlayer = (DWORD*)(AFXADDR_GET(csgo_CDemoPlayer));

		True_CDemoPlayer_GetPlaybackTick = (CDemoPlayer_GetPlaybackTick_t)vtableCdemoPlayer[3];
		True_CDemoPlayer_SkipToTick = (CDemoPlayer_SkipToTick_t)vtableCdemoPlayer[12];
		True_CDemoPlayer_StopPlayback = (CDemoPlayer_StopPlayback_t)vtableCdemoPlayer[15];
		True_CDemoPlayer_ReadPacket = (CDemoPlayer_ReadPacket_t)vtableCdemoPlayer[17];

		True_CDemoPlayer_StartPlayback = (CDemoPlayer_StartPlayback_t)AFXADDR_GET(csgo_CDemoPlayer_InternalStartPlayback);
//		True_csgo_CL_ProcessPacketEntities_AddClientFrame = (csgo_CL_ProcessPacketEntities_AddClientFrame_t)AFXADDR_GET(csgo_CL_ProcessPacketEntities_AddClientFrame);
		True_csgo_NET_CreateNetChannel = (csgo_NET_CreateNetChannel_t)AFXADDR_GET(csgo_NET_CreateNetChannel);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)True_CDemoPlayer_GetPlaybackTick, My_CDemoPlayer_GetPlaybackTick);
		DetourAttach(&(PVOID&)True_CDemoPlayer_SkipToTick, My_CDemoPlayer_SkipToTick);
		DetourAttach(&(PVOID&)True_CDemoPlayer_StopPlayback, My_CDemoPlayer_StopPlayback);
		DetourAttach(&(PVOID&)True_CDemoPlayer_ReadPacket, My_CDemoPlayer_ReadPacket);
		DetourAttach(&(PVOID&)True_CDemoPlayer_StartPlayback, My_CDemoPlayer_StartPlayback);
//		DetourAttach(&(PVOID&)True_csgo_CL_ProcessPacketEntities_AddClientFrame, My_csgo_CL_ProcessPacketEntities_AddClientFrame);
		//DetourAttach(&(PVOID&)True_csgo_NET_CreateNetChannel, Wrap_My_csgo_NET_CreateNetChannel);
		error = DetourTransactionCommit();

		/*
		DWORD addr = AFXADDR_GET(csgo_CClientFrameManager_DeleteClientFrames);
		addr += 24;

		g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueNo = (void*)(addr + 0x05);
		g_My_CClientFrameManager_DeleteClientFrames_Comparison_ContinueYes = (void*)(addr + 0x0c);

		Asm32ReplaceWithJmp((void*)addr, 0x5, My_CClientFrameManager_DeleteClientFrames_Comparison);

		DWORD addr = AFXADDR_GET(csgo_CL_ProcessPacketEntities_AddClientFrame);

		g_csgo_CL_ProcessPacketEntities_AddClientFrame_Continue = (void*)(addr + 0x54);

		Asm32ReplaceWithJmp((void*)addr, 0x54, My_csgo_CL_ProcessPacketEntities_AddClientFrame);
		*/

		firstResult = NO_ERROR == error;
	}

	return firstResult;
}
