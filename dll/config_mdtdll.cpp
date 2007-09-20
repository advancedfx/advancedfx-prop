/*
File        : dll/config_mdtdll
Started     : 2007-09-20 08:33:00
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : see config.h
*/

#include "windows.h"
#include "config_mdtdll.h"

/*
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
*/

cConfig_mdtdll::cConfig_mdtdll(char *pFileNamez)
{
	_bLoadOk=false;
	_pFileNamez=(char *)malloc(strlen(pFileNamez)+1);

	if (_pFileNamez)
	{
		strcpy(_pFileNamez,pFileNamez);
		_bLoadOk=true;
	};

	//_doc = TiXmlDocument::TiXmlDocument(_pFileNamez); // doesn't work for some reason and pisses me off like shit
	//_bLoadOk = _doc.LoadFile();
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

typedef struct AddrEntry_s
{
	char*	pszName;	// name it should have in the config
	DWORD	dTabEntry;	// pointer onto the table entry
	bool	bSet;		// indicates if the value has been already set or not
} AddrEntry_t;

#define CONFIG_MDTDLL_ADDR_ENTRIES_NUM 7

AddrEntry_t pAddrEntries[CONFIG_MDTDLL_ADDR_ENTRIES_NUM] =
{
//	{"", (char *)&, false},
	{"cl_enginefuncs_s", (DWORD)HL_ADDR_CL_ENGINEFUNCS_S, false},
	{"engine_studio_api_s", (DWORD)HL_ADDR_ENGINE_STUDIO_API_S, false},
	{"playermove_s", (DWORD)HL_ADDR_PLAYERMOVE_S, false},
	{"R_RenderView", (DWORD)HL_ADDR_R_RenderView, false},
	{"r_refdef", (DWORD)HL_ADDR_r_refdef, false},
	{"HudSpectator_tfc", (DWORD)HL_ADDR_HUDSPECTATOR_FUNC_TFC, false},
	{"HudSpectator_cmp_tfc", (DWORD)HL_ADDR_HUDSPECTATOR_CMPA0_TFC, false},
};

void cConfig_mdtdll::ApplyAddresses(hl_addresses_t *pTargetAddresses)
{
	pTargetAddresses->p_cl_enginefuncs_s	=(char *)pAddrEntries[0].dTabEntry;
	pTargetAddresses->p_engine_studio_api_s	=(char *)pAddrEntries[1].dTabEntry;
	pTargetAddresses->p_playermove_s		=(char *)pAddrEntries[2].dTabEntry;
	pTargetAddresses->p_R_RenderView		=(char *)pAddrEntries[3].dTabEntry;
	pTargetAddresses->p_r_refdef			=(char *)pAddrEntries[4].dTabEntry;
	pTargetAddresses->p_HudSpectator_tfc	=(char *)pAddrEntries[5].dTabEntry;
	pTargetAddresses->p_HudSpectator_cmp_tfc=(char *)pAddrEntries[6].dTabEntry;
}

AddrEntry_t *FindAddrEntry(char* pszName)
{
	for (int i=0;i<CONFIG_MDTDLL_ADDR_ENTRIES_NUM;i++)
	{
		AddrEntry_t *pCurr=&(pAddrEntries[i]);

		if (strcmp(pCurr->pszName,pszName)==0)
			return pCurr;
	}

	return NULL;
}

#define CONFIG_MDTDLL_MAX_OPPER_SIZE 40

bool RetriveOpper(char *pos_begin, char *pos_end, DWORD* value)
// opos:  points onto first char
// pos :  points onto last char
{
	static char myspace [CONFIG_MDTDLL_MAX_OPPER_SIZE+1];

	//pEngfuncs->Con_Printf("[0x%08x-0x%08x] ",pos_begin,pos_end);

	if ((pos_end<=pos_begin) || ((pos_end-pos_begin)>CONFIG_MDTDLL_MAX_OPPER_SIZE)) return false;

	bool bIsVariable=true;

	if (pos_end-pos_begin>1)
		if (*pos_begin=='0' && *(pos_begin+1) == 'x') bIsVariable=false;;

	memcpy(myspace,pos_begin,pos_end-pos_begin);
	myspace[(pos_end-pos_begin+1)]=NULL;
	//pEngfuncs->Con_Printf("(%i) %s |",bIsVariable,myspace);
	
	if (bIsVariable)
	{
		AddrEntry_t *pEntry=FindAddrEntry(myspace);
		if (!pEntry || (!pEntry->bSet)) return false;
		else {
			*value=pEntry->dTabEntry;
			return true;
		}
	} else {
		// we
		*value = strtoul(myspace,NULL,16);
		return true;
	}
}

bool parseAddrExpression(AddrEntry_t *pTargetEntry,char *pExpression)
{
	if (!pExpression) return false;

	// const = 0x[0-9]*
	// var = [a-zA-Z_]+
	// 
	//
	// terminal = const
	//            | var
	//
	// expr = terminal
	//        | terminal [+-] expr

	DWORD accu,value;
	enum eACCU_OP { AO_SET, AO_PLUS, AO_MINUS };
	enum eAUTO_STATE {AS_FAIL, AS_START, AS_LTRIM, AS_READ, AS_RTRIM, AS_PLUS, AS_MINUS, AS_FINISH, AS_DONE};

	eACCU_OP accu_op=AO_SET;
	eAUTO_STATE astate=AS_START;

	char *pos_begin,*pos_end;
	char *pos=pExpression;
	char ccur;

	while ((astate!=AS_FAIL)&&(astate!=AS_DONE))
	{
		ccur=*pos;
		//pEngfuncs->Con_Printf("%c",ccur);

		switch (astate)
		{
		case AS_START:
			if (ccur==NULL) astate=AS_FAIL;
			else if (ccur==' ') astate=AS_LTRIM;
			else astate=AS_READ;
			pos_begin=pos;
			pos++;
			break;
		case AS_LTRIM:
			if (ccur==NULL) astate=AS_FAIL;
			else if (ccur!=' ') astate=AS_READ;
			pos_begin=pos;
			pos++;
			break;
		case AS_READ:
			if (ccur==NULL) astate=AS_FINISH;
			else if(ccur==' ') astate=AS_RTRIM;
			pos_end=pos;
			pos++;
			break;
		case AS_RTRIM:
			switch(ccur)
			{
			case NULL:
				astate = AS_FINISH;
				break;
			case ' ':
				break;
			case '+':
				astate = AS_PLUS;
				break;
			case '-':
				astate = AS_MINUS;
				break;
			default:
				// every other char.
				astate = AS_FAIL;
			}
			pos++;
			break;
		case AS_PLUS:
		case AS_MINUS:
		case AS_FINISH:
			// AS_PLUS, AS_MINUS and AS_FINISH have a common part we do first:
			//pEngfuncs->Con_Printf("Finsih!");
			if(!RetriveOpper(pos_begin,pos_end,&value)) astate=AS_FAIL;
			else
			{
				switch(accu_op)
				{
				case AO_SET:
					accu=value;
					break;
				case AO_PLUS:
					accu+=value;
					break;
				case AO_MINUS:
					accu-=value;
				}

				// and now we differ AS_PLUS, AS_MINUS and AS_FINISH (all 3 states exactly one edge which is an epsilon edge):
				if (astate==AS_PLUS)
				{
					accu_op=AO_PLUS;
					astate=AS_START;
				} else if (astate==AS_MINUS)
				{
					accu_op=AO_MINUS;
					astate=AS_START;
				} else astate=AS_DONE; // done with finishing.

			}
			break;

		default:
			// should not happen.
			astate=AS_FAIL;
		}
	}

	if (astate!=AS_DONE) return false;
	else
	{
		// we had succes, save the value and mark the variable as set:
		//*(DWORD *)(pTargetEntry->pTabEntry) = accu;
		pTargetEntry->dTabEntry=accu;
		pTargetEntry->bSet=true;
		
		return true;
	}
}


bool cConfig_mdtdll::UpdateAddressByName(char *pout,char *pout2)
{
	AddrEntry_t *pEntry=FindAddrEntry(pout);

	if (!pEntry) return false;

	if (parseAddrExpression(pEntry,pout2)) return true;

	return false;
}

bool cConfig_mdtdll::GetAddresses(hl_addresses_t *pDefaultHlAddresses)
{
	if (!_bLoadOk) return false;

	// stub for xml:
 
	/*// find the first product entry with id=="mdt":

	TiXmlNode* curr = _doc.FirstChild("hlae");
	if (!curr) return false;

	TiXmlNode* child = curr->FirstChild("product");
	if (!child) return false;

	char* pszattrib;
	TiXmlElement*	pElement;
	int isres=-1;

	while	(	child
				&&	(pElement=child->ToElement())
				&&	(	!(pszattrib=(char *)pElement->Attribute("id"))
						||(isres=strcmp(pszattrib,"mdt"))
					)
			)
	{
		child = curr->IterateChildren("product",child);
	}

	if(!child || !pszattrib || isres) return false;

	// ok no we have the right product

	child = child->FirstChild("addresses");
	if (!child) return false;

	child = child->FirstChild("address");
	if (!child) return false;

	char *pout,*pout2;
	bool bres=true;

	TiXmlElement* celem = child->ToElement();

	while (celem)
	{
		if(pout = (char *)celem->Attribute("id"))
		{
			if(pout2 = (char *)celem->Value() )
				bres=bres && UpdateAddressByName(pout,pout2);
		}
	}

	return bres;
*/
	// currently we us ini instead:

	bool bres = true;

	GetPrivateProfileString("mdt_current","mdt_useaddr","mdt_addr_current",_tmp1,CONFIG_MDTDLL_MAX_CFGSTRLEN,_pFileNamez);

	for (int i=0;i<CONFIG_MDTDLL_ADDR_ENTRIES_NUM;i++)
	{
		_tmp2[0]='0'; _tmp2[1]='x'; ultoa(pAddrEntries[i].dTabEntry,_tmp2+2,16); GetPrivateProfileString(_tmp1,pAddrEntries[i].pszName,_tmp2,_tmp2,CONFIG_MDTDLL_MAX_CFGSTRLEN,_pFileNamez);
		bres = bres && parseAddrExpression(&(pAddrEntries[i]),_tmp2);
		//pEngfuncs->Con_Printf("| %s == 0x%08x (%i)\n",pAddrEntries[i].pszName,pAddrEntries[i].dTabEntry,bres);
	}

	return bres;
}
