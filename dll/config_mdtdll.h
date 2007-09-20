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
// Warning: UpdateAddressByName is probably not thread safe currently since it calls another function in the cpp which operates on structures outside of the class's scope!
{
private:
	#define CONFIG_MDTDLL_MAX_CFGSTRLEN 200
	// min: 35
public:
	cConfig_mdtdll(char* pFileNamez);
	~cConfig_mdtdll();
	bool bLoadOk();

    bool GetAddresses(hl_addresses_t *pDefaultHlAddresses);
	void ApplyAddresses(hl_addresses_t *pTargetAddresses);

private:
	char _tmp1[CONFIG_MDTDLL_MAX_CFGSTRLEN+1];
	char _tmp2[CONFIG_MDTDLL_MAX_CFGSTRLEN+1];

	size_t	_bytesFileName;
	char*	_pFileNamez;
	bool	_bLoadOk;

	bool UpdateAddressByName(char *pout,char *pout2);
} ;

#endif