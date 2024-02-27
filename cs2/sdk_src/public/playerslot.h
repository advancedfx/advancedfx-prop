#ifndef SOURCESDK_CS2_PLAYERSLOT_H
#define SOURCESDK_CS2_PLAYERSLOT_H

#if _WIN32
#pragma once
#endif


namespace SOURCESDK {
namespace CS2 {

class CPlayerSlot
{
public:
	CPlayerSlot(int slot) : m_Data(slot)
	{
	}

	int Get() const
	{
		return m_Data;
	}

	bool operator==(const CPlayerSlot &other) const {
		return other.m_Data == m_Data;
	}
	bool operator!=(const CPlayerSlot &other) const {
		return other.m_Data != m_Data;
	}

private:
	int m_Data;
};


} // namespace SOURCESDK {
} // namespace CS2 {


#endif // SOURCESDK_CS2_PLAYERSLOT_H
