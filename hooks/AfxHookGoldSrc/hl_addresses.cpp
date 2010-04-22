#include "stdafx.h"

#include "hl_addresses.h"

#include <windows.h>

#include <list>

class HlAddressEntries;

class HlAddressEntry {
public:
	HlAddress_t * m_Entry;
	char const * m_Name;

	HlAddressEntry(HlAddressEntries * entries, HlAddress_t *entry, static char const * name);
};

class HlAddressEntries {
public:
	void Register(HlAddressEntry *entry);
	HlAddress_t * GetByName(char const * name);

	// slow
	HlAddressEntry * Debug_GetAt(unsigned int index);

	unsigned int Debug_GetCount();
	
private:
	std::list<HlAddressEntry *> m_HlAddressEntryList;

} g_HlAddressEntries;

HlAddressEntry::HlAddressEntry(HlAddressEntries * entries, HlAddress_t *entry, static char const * name) {
	m_Entry = entry;
	m_Name = name;
	entries->Register(this);
}

void HlAddressEntries::Register(HlAddressEntry *entry) {
	m_HlAddressEntryList.push_back(entry);
}

HlAddress_t * HlAddressEntries::GetByName(char const * name) {
	std::list<HlAddressEntry *>::iterator it;
	for (it=m_HlAddressEntryList.begin();it!=m_HlAddressEntryList.end();it++)
	{
		if(strcmp((*it)->m_Name, name)==0)
			return (*it)->m_Entry;
	}

	return 0;
}

HlAddressEntry * HlAddressEntries::Debug_GetAt(unsigned int index) {
	if(index < m_HlAddressEntryList.size()) {
		std::list<HlAddressEntry *>::iterator it = m_HlAddressEntryList.begin(); 
		while(0 < index) {
			it++;
			index--;
		}

		return (*it);
	}

	return 0;
}

unsigned int HlAddressEntries::Debug_GetCount() {
	return m_HlAddressEntryList.size();
}


HlAddress_t * HlAddr_GetByName(char const * name) {
	return g_HlAddressEntries.GetByName(name);
}

bool HlAddr_Debug_GetAt(unsigned int index, HlAddress_t & outAddr, char const * & outName) {
	HlAddressEntry * entry = g_HlAddressEntries.Debug_GetAt(index);

	if(entry) {
		outAddr = *(entry->m_Entry);
		outName = entry->m_Name;
	}

	return 0 != entry;
}

unsigned int HlAddr_Debug_GetCount() {
	return g_HlAddressEntries.Debug_GetCount();
}


#define HL_ADDR_DEF(name) \
	HlAddress_t g_HlAddr_##name; \
	HlAddressEntry g_HlAddressEntry##name (&g_HlAddressEntries, &g_HlAddr_##name, #name);

HL_ADDR_DEF(CL_EmitEntities)
HL_ADDR_DEF(CL_EmitEntities_DSZ)
HL_ADDR_DEF(CL_ParseServerMessage)
HL_ADDR_DEF(CL_ParseServerMessage_CmdRead)
HL_ADDR_DEF(DTOURSZ_CL_ParseServerMessage_CmdRead)
HL_ADDR_DEF(DTOURSZ_GetClientColor)
HL_ADDR_DEF(DTOURSZ_GetSoundtime)
HL_ADDR_DEF(DTOURSZ_Mod_LeafPVS)
HL_ADDR_DEF(DTOURSZ_R_DrawEntitiesOnList)
HL_ADDR_DEF(DTOURSZ_R_DrawParticles)
HL_ADDR_DEF(DTOURSZ_R_DrawViewModel)
HL_ADDR_DEF(DTOURSZ_R_PolyBlend)
HL_ADDR_DEF(DTOURSZ_R_RenderView)
HL_ADDR_DEF(DTOURSZ_SCR_UpdateScreen)
HL_ADDR_DEF(DTOURSZ_SND_PickChannel)
HL_ADDR_DEF(DTOURSZ_S_PaintChannels)
HL_ADDR_DEF(DTOURSZ_S_TransferPaintBuffer)
HL_ADDR_DEF(DTOURSZ_UnkCstrikeCrosshairFn)
HL_ADDR_DEF(DTOURSZ_UnkGetDecalTexture)
HL_ADDR_DEF(GetClientColor)
HL_ADDR_DEF(GetSoundtime)
HL_ADDR_DEF(HUD_TOURIN)
HL_ADDR_DEF(HUD_TOUROUT)
HL_ADDR_DEF(HudSpectator_cmp_tfc)
HL_ADDR_DEF(HudSpectator_tfc)
HL_ADDR_DEF(Mod_LeafPVS)
HL_ADDR_DEF(R_DrawEntitiesOnList)
HL_ADDR_DEF(R_DrawParticles)
HL_ADDR_DEF(R_DrawViewModel)
HL_ADDR_DEF(R_PolyBlend)
HL_ADDR_DEF(R_RenderView)
HL_ADDR_DEF(SCR_UpdateScreen)
HL_ADDR_DEF(SND_PickChannel)
HL_ADDR_DEF(SZ_unkInlineClientColorA)
HL_ADDR_DEF(SZ_unkInlineClientColorV)
HL_ADDR_DEF(S_PaintChannels)
HL_ADDR_DEF(S_TransferPaintBuffer)
HL_ADDR_DEF(UnkCstrikeCh_add_fac)
HL_ADDR_DEF(UnkCstrikeCh_mul_fac)
HL_ADDR_DEF(UnkCstrikeCrosshairFn)
HL_ADDR_DEF(UnkGetDecalTexture)
HL_ADDR_DEF(UnkDevCapsChecks)
HL_ADDR_DEF(UnkDevCapsChecks_BYTES)
HL_ADDR_DEF(UpdateSpectatorPanel_checkjmp_ag)
HL_ADDR_DEF(UpdateSpectatorPanel_checkjmp_ns)
HL_ADDR_DEF(UpdateSpectatorPanel_checkjmp_tfc)
HL_ADDR_DEF(UpdateSpectatorPanel_checkjmp_valve)
HL_ADDR_DEF(clientDll)
HL_ADDR_DEF(cstrike_CHudDeathNotice_Draw)
HL_ADDR_DEF(cstrike_CHudDeathNotice_MsgFunc_DeathMsg)
HL_ADDR_DEF(cstrike_EV_CreateSmoke)
HL_ADDR_DEF(cstrike_MsgFunc_DeathMsg)
HL_ADDR_DEF(cstrike_rgDeathNoticeList)
HL_ADDR_DEF(hlExe)
HL_ADDR_DEF(hwDll)
HL_ADDR_DEF(msg_readcount)
HL_ADDR_DEF(net_message)
HL_ADDR_DEF(net_message_cursize)
HL_ADDR_DEF(p_cl_enginefuncs_s)
HL_ADDR_DEF(p_engine_studio_api_s)
HL_ADDR_DEF(p_playermove_s)
HL_ADDR_DEF(paintbuffer)
HL_ADDR_DEF(paintedtime)
HL_ADDR_DEF(r_refdef)
HL_ADDR_DEF(shm)
HL_ADDR_DEF(soundtime)
HL_ADDR_DEF(unkInlineClientColorA)
HL_ADDR_DEF(unkInlineClientColorV)

