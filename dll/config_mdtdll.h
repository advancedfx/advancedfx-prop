/*
File        : dll/config_mdtdll.h
Started     : 2007-09-20 08:33:00
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : Functions for loading data from the mdt mdt_config.xml file
*/

#ifndef CONFIG_MDTDLL_H
#define CONFIG_MDTDLL_H

#include "hl_addresses.h"

class cConfig_mdtdll
// config singelton
{
public:
	cConfig_mdtdll(char* pFileNamez); // path to the config has to be supplied
	~cConfig_mdtdll();

	bool bLoadOk();
	bool bAddressesApplied();

    bool LoadAndApplyAddresses(); // loads and applies the addresses

private:
	#define CONFIG_MDTDLL_MAX_CFGSTRLEN 200

	char	_tmp1[CONFIG_MDTDLL_MAX_CFGSTRLEN+1];

	size_t	_bytesFileName;

	char*	_pFileNamez;
	bool	_bLoadOk;
	bool	_bAddressesApplied;
} ;

#endif