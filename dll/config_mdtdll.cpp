/*
File        : dll/config_mdtdll
Started     : 2007-09-20 08:33:00
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : see config.h
*/

#include "windows.h"
#include "io.h" // filelength lol
#include "config_mdtdll.h"

#if 0
// only needed when doing console prints:
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
extern cl_enginefuncs_s *pEngfuncs;
#endif

cConfig_mdtdll::cConfig_mdtdll(char *pFileNamez)
{
	_bLoadOk=false;
	_bAddressesApplied=false;
	_pFileNamez=(char *)malloc(strlen(pFileNamez)+1);

	if (_pFileNamez)
	{
		strcpy(_pFileNamez,pFileNamez);
		_bLoadOk=true;
	};

}

cConfig_mdtdll::~cConfig_mdtdll()
{
	_bLoadOk=false;
	free(_pFileNamez);
}

bool cConfig_mdtdll::bLoadOk()
{
	return bLoadOk;
}

bool cConfig_mdtdll::LoadAndApplyAddresses()
{
	if (!_bLoadOk) return false;

	// currently we us ini instead:

	bool bres = false;

	FILE *pAFile;

	// GetPrivateProfileString("mdt_current","mdt_useaddr","mdt_addr_current",_tmp1,CONFIG_MDTDLL_MAX_CFGSTRLEN,_pFileNamez);
	
	if ((pAFile = fopen(_pFileNamez, "rb")) != NULL)
	{
		unsigned long ulFileSize = _filelength(_fileno(pAFile));

		char *pTempMem = (char *)malloc(ulFileSize+1);

		if (pTempMem)
		{
			fread(pTempMem,sizeof(char),ulFileSize, pAFile);
			pTempMem[ulFileSize]=0; // place terminating char

			unsigned long ulT;
			char *pT;

			cHLAddresses::MDT_PRESULT mRes = g_HLAddresses.Parse_AddressConfig(pTempMem,ulT,ulT,pT,pT);

			if (mRes==cHLAddresses::MDTP_OK) bres=true;

		}

		free(pTempMem);
		fclose(pAFile);
	}

	//pEngfuncs->Con_Printf("| %s == 0x%08x (%i)\n",pAddrEntries[i].pszName,pAddrEntries[i].dTabEntry,bres);
	return bres;
}