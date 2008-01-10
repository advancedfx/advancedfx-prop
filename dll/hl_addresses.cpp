#include "hl_addresses.h"

#include <windows.h>

//
// definitions of default addresses that need to be present early in case we use prints for debug:
//

//
// g_hl_addresses initalization:
//
/*

hl_addresses_t g_hl_addresses = {
// be carefull when you change s.th. here and keep hl_addresses_s and g_hl_addresses in sync
// NULL values are calculated at runtime from the .ini system
	(char *)0x01ECCB00, // p_cl_enginefuncs_s
	0, // p_engine_studio_api_s
	0, // p_playermove_s
	0, // p_SCR_UpdateScreen
	0, // p_SCR_SetUpToDrawConsole
	0, // p_V_RenderView
	0, // p_R_RenderView_
	0, // p_GL_Set2D
	0, // p_r_refdef
	0, // p_HudSpectator_tfc
	0, // p_HudSpectator_cmp_tfc
};*/


///////////////////////////////////////////////////////////////////////////////
// g_HLAddresses global and Addresses Initialisation
///////////////////////////////////////////////////////////////////////////////

cHLAddresses g_HLAddresses;

#define HL_ADDR_ENTRY(name,expression) \
	unsigned long ulHLAddr_##name; \
	cHLAddressEntry HLAddrObj_##name(&g_HLAddresses,#name,##expression,&ulHLAddr_##name);

#define HL_ADDR_VAR(name) \
	cHLAddressEntry HLAddrObj_##name(&g_HLAddresses,#name);

// Variables:
HL_ADDR_VAR(_v0)
HL_ADDR_VAR(_v1)
HL_ADDR_VAR(_v2)
HL_ADDR_VAR(_v3)
HL_ADDR_VAR(_v4)
HL_ADDR_VAR(_v5)
HL_ADDR_VAR(_v6)
HL_ADDR_VAR(_v7)
HL_ADDR_VAR(_v8)
HL_ADDR_VAR(_v9)
HL_ADDR_VAR(_vA)
HL_ADDR_VAR(_vB)
HL_ADDR_VAR(_vC)
HL_ADDR_VAR(_vD)
HL_ADDR_VAR(_vE)
HL_ADDR_VAR(_vF)

// Addresses:
HL_ADDR_ENTRY(p_cl_enginefuncs_s,"0x0")
HL_ADDR_ENTRY(p_enginefuncs_s,"0x0") // unused
HL_ADDR_ENTRY(p_engine_studio_api_s,"0x0")
HL_ADDR_ENTRY(p_playermove_s,"0x0")
HL_ADDR_ENTRY(SCR_UpdateScreen,"0x0") // unused
HL_ADDR_ENTRY(SCR_SetUpToDrawConsole,"0x0") // unused
HL_ADDR_ENTRY(V_RenderView,"0x0") // unused
HL_ADDR_ENTRY(GL_Set2D,"0x0") // unused
HL_ADDR_ENTRY(R_RenderView_,"0x0")
HL_ADDR_ENTRY(R_MarkLeaves,"0x0")
HL_ADDR_ENTRY(p_r_refdef,"0x0")
HL_ADDR_ENTRY(HudSpectator_tfc,"0x0")
HL_ADDR_ENTRY(HudSpectator_cmp_tfc,"0x0")
HL_ADDR_ENTRY(UnkIGAWorld,"0x0")

///////////////////////////////////////////////////////////////////////////////
// cHLAddresses
///////////////////////////////////////////////////////////////////////////////


void cHLAddresses::_RegisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	_HLAddressEntryList.push_back(pHLAddressEntry);
}

void cHLAddresses::_UnregisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	//MessageBox(NULL,pHLAddressEntry->GetName(),"unregistering",MB_OK);
	
	std::list<cHLAddressEntry_t*>::iterator it;
	for (it=_HLAddressEntryList.begin();it!=_HLAddressEntryList.end();it++)
	{
		if ((*it)==pHLAddressEntry)
		{
			_HLAddressEntryList.erase(it); // WARNING: this asumes addresses of object won't change
			break; // we have to break since this op could change the list and render our iterators into crap
		}
	}
}

cHLAddressEntry* cHLAddresses::GiveEntry(char *pszName)
{
	std::list<cHLAddressEntry*>::iterator it;
	for (it=_HLAddressEntryList.begin();it!=_HLAddressEntryList.end();it++)
	{
		if(strcmp((*it)->GetName(),pszName)==0)
			return (*it);
	}

	return NULL;
}

cHLAddresses::~cHLAddresses()
{
	// we have to destroy all entries first since the current Entry Implementation relays on the presense of the cHLAddress Object:
	
	// note: the entries will operate on the list during their destruction, since they will unregister themselfs!

	while(!_HLAddressEntryList.empty()) delete (*_HLAddressEntryList.end());

}

bool cHLAddresses::Do_Indirection(unsigned long ulPointer,unsigned long & ulPointed)
{
	bool mRes=true;
	try
	{
		ulPointed = *((unsigned long *)ulPointer);
	}
	catch(...)
	{
		mRes=false;
	}
	return mRes;
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_WhiteSpaceKleene(char *pszInput,char *&pszRemainder)
{
	pszRemainder=pszInput; // we start on the input of course

	while (*pszRemainder==' ' || *pszRemainder=='\t') pszRemainder++; // eat whitespace

	return MDTP_OK; // we ate as much as we could, return OK
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Variable(char *pszInput,char *&pszRemainder,cHLAddressEntry *&pEntry)
{
	char cTmp = pszInput[0];
	pszRemainder = pszInput;
	char *pszR = pszRemainder;
	pEntry=0; // NULL
	
	// variable strings may not be empty:
	if(cTmp==0) return MDTP_ESYNTAX;
	
	// check [A-Za-z_]
	if (!( (cTmp>='A' && cTmp<='Z') || (cTmp>='a' && cTmp<='z') || (cTmp=='_') )) return MDTP_ESYNTAX;

	// parse [0-9A-Za-z_]*
	do cTmp = *(++pszR); while ( (cTmp>='0' && cTmp <='9') || (cTmp>='A' && cTmp<='Z') || (cTmp>='a' && cTmp<='z') || (cTmp=='_') );

	// well I seriously thought if we should simply modify the original string temporary or not (put null in and preserve and restore original value)
	// but finally I decided I should rather make a copy of it for some unknown reason:

	char *pszT;

	MDT_PRESULT mRes=MDTP_OK;

	// get memory:
	try
	{
		pszT = new char[pszR-pszInput+1];
	}
	catch(...)
	{
		mRes=MDTP_EMEMALLOC;
	}

	if (mRes==MDTP_OK)
	{
		// only when allocation was successfull
		memcpy(pszT,pszInput,pszR-pszInput);
		pszT[pszR-pszInput]=0; // place NULL termination char

		// retrive entry if possible:
		if ((pEntry=GiveEntry(pszT))) pszRemainder=pszR;
		else mRes=MDTP_ESEM_VARUNK; // entry not found

	}

	// free memory
	delete pszT;
	return mRes; // return
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Terminal(char *pszInput,char *&pszRemainder,unsigned long &ulValue)
{
	#define IS_CHAR_HEX(c) ((c>='0' && c <='9') || (c>='A' && c <='F') || (c>='a' && c <='f'))
	size_t slen=strlen(pszInput);
	char cTmp = pszInput[0];

	pszRemainder = pszInput;

	if (slen<1) return MDTP_ESYNTAX; // at least one char!

	// this strictly rellays on C++' left to right evaluation order and operator precedence:
	if (slen>=3 && cTmp=='0' && pszInput[1]=='x' )
	{

		// check that also 3rd char is valid:
		cTmp=pszInput[2];
		if (!IS_CHAR_HEX(cTmp)) return MDTP_ESYNTAX;

		// it has to be a constant, parse as much as possible (see strtoul ms doc):
		ulValue=strtoul(pszInput,&pszRemainder,16);
		return MDTP_OK;

	} else {

		// it has to be a variable
		cHLAddressEntry *pEntry;
		MDT_PRESULT mRes=Parse_Variable(pszRemainder,pszRemainder,pEntry);

		if (mRes==MDTP_OK)
		{
			if( pEntry && pEntry->bIsValid()) ulValue=pEntry->GetValue();
			else mRes=MDTP_ESEM_VARUNINIT; // not valid for use
		}

		return mRes;
	}
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Expression(char *pszInput,char *&pszRemainder,unsigned long &ulValue)
{
	MDT_PRESULT mRes;
	char *pszT;
	enum MDT_ACCU_OP {MDTAO_NOP,MDTAO_SET,MDTAO_PLUS,MDTAO_MINUS } mAccuOp=MDTAO_SET; // acc op to carry out (next)
	unsigned long ulAccu,ulTerminal;

	pszRemainder=pszInput; // we start on the input of course

	while (true)
	{
		// we expect TERMINAL:
		if (MDTP_OK != (mRes = Parse_Terminal(pszRemainder,pszRemainder,ulTerminal))) return mRes;

		// apply the accu:
		switch (mAccuOp)
		{
		case MDTAO_SET:
			ulAccu = ulTerminal;
			break;
		case MDTAO_PLUS:
			ulAccu += ulTerminal;
			break;
		case MDTAO_MINUS:
			ulAccu -= ulTerminal;
			break;
		default:
			return MDTP_EPROGRAM; // this should not happen
		}

		// check for operators:
		if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszRemainder,pszT))) return mRes; // munch WS* (if any)

		switch(*pszT)
		{
		case MDT_OPCHAR_INDIRECTION:
			if (!Do_Indirection(ulAccu,ulAccu)) return MDTP_EINDIRECTION;

			break;
		case MDT_OPCHAR_PLUS:
			mAccuOp = MDTAO_PLUS;
			pszT++;
			if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszT,pszT))) return mRes; // munch WS* (if any)
			break;
		case MDT_OPCHAR_MINUS:
			mAccuOp = MDTAO_MINUS;
			pszT++;
			if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszT,pszT))) return mRes; // munch WS* (if any)
			break;
		default:
			// there was no op
			ulValue=ulAccu;
			return MDTP_OK; // quit instantly (returns current pszRemainder and accu)
		}

		// if we are here the expression syntax is not finished yet, so update the pszRemainder:
		pszRemainder = pszT; // update!!
	}
	
	return MDTP_EPROGRAM;
}

bool cHLAddresses::EvaluateExpression(std::string *pstrExpression,unsigned long &ulValue)
{
	static char *pszDummy;

	bool btmp=Parse_Expression((char *)pstrExpression->c_str(),(char *)pszDummy,ulValue)==MDTP_OK;

	//char ttt[1000];
	//sprintf(ttt,"%s: %s->0x%08x (%i)","?",(char *)pstrExpression->c_str(),ulValue,btmp);
	//MessageBox(NULL,ttt,"DebugInfo",MB_OK);

	return btmp;
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Allocation(char *pszInput,char *&pszRemainder)
{
	MDT_PRESULT mRes=MDTP_OK;

	cHLAddressEntry *pTargetEntry;
	unsigned long ulValue;

	pszRemainder=pszInput;

	// we expect VARIABLE
	mRes=Parse_Variable(pszRemainder,pszRemainder,pTargetEntry);
	if (mRes!=MDTP_OK) return mRes;
	if (!pTargetEntry) return MDTP_EPROGRAM;

	// munch WS*:
	if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszRemainder,pszRemainder))) return mRes;

	// we expect the MDT_CHAR_ALLOCATION char ('='):
	if(pszRemainder[0]!=MDT_CHAR_ALLOCATION) return MDTP_ESYNTAX;
	pszRemainder++;

	// munch WS*:
	if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszRemainder,pszRemainder))) return mRes;

	// we expect EXPRESSION:
	mRes=Parse_Expression(pszRemainder,pszRemainder,ulValue);

	if (mRes==MDTP_OK)
		pTargetEntry->SetValue(ulValue); // do the allocation

	return mRes;
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Comment(char *pszInput,char *&pszRemainder)
{
	pszRemainder = pszInput;

    // we expect ';'
	if (pszRemainder[0]!=';') return MDTP_ESYNTAX;

	// munch everything except new line char '\n'or (string-) termination char 0
	char cTmp;
	do cTmp=(++pszRemainder)[0];
	while (cTmp!='\n' && cTmp!=0);

	return MDTP_OK;
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_Line(char *pszInput,char *&pszRemainder)
{
	MDT_PRESULT mRes;
	char *pszT;

	pszRemainder=pszInput;

	// munch any whitespace:
	if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszRemainder,pszRemainder))) return mRes;

	// first try to parse as comment:
	mRes = Parse_Comment(pszRemainder,pszT);
	if (mRes==MDTP_OK)
	{
		pszRemainder = pszT;
		return mRes;
	}

	// so it was not a comment, then it could be an ALLOCATION:
	mRes = Parse_Allocation(pszRemainder,pszT);
	if (mRes==MDTP_OK)
	{
		// is ALLOCATION
		pszRemainder=pszT;

		// munch WS*:
		if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszRemainder,pszRemainder))) return mRes;

		// munch any trailing comment:
		mRes = Parse_Comment(pszRemainder,pszT);
		if (mRes==MDTP_OK) pszRemainder=pszT;

		return MDTP_OK;
	}

	// it could be an empty line, so return (lines will handle new line '\n' and termination char 0):
	return MDTP_OK;
}

cHLAddresses::MDT_PRESULT cHLAddresses::Parse_AddressConfig(char *pszInput,unsigned long & ulErrLinesTotal,unsigned long & ulFirstErrLine, char *& pFirstErrorLine, char *& pFirstErrorChar)
// this parses LINES with best effort!
{
	MDT_PRESULT mRes=MDTP_OK;

	MDT_PRESULT tRes;
	char *pszT;

	unsigned long ulLineNum=0;

	ulErrLinesTotal = 0;
	ulFirstErrLine = 0;
	pFirstErrorLine = 0;
	pFirstErrorChar = 0;

	// Note: this is the only function that checks NULL == 0 on pszInput!
	if (pszInput==0) return MDTP_ESYNTAX; // actually this should have never been supplied, but we are a friendly kind :)

	// parse a Line (if any)
	while (pszInput[0]!=0)
	{	
		ulLineNum++; // increment Line Number

		tRes=Parse_Line(pszInput,pszT);

		if (tRes!=MDTP_OK)
		{
			// this Line has errors
			if (ulErrLinesTotal==0)
			{
				// this is the first errorneous line, store info!

				ulFirstErrLine=ulLineNum;
				pFirstErrorLine = pszInput;
				pFirstErrorChar = pszT;

				mRes = tRes;

			}

			ulErrLinesTotal++;
		}

		// in any case proceed to line end (even if it was errorneous):
		pszInput=pszT;
		while(pszInput[0]!=0 && pszInput[0]!='\n') pszInput++;
		if(pszInput[0]=='\n') pszInput++;
	}


	return mRes;
}


///////////////////////////////////////////////////////////////////////////////
// cHLAddressEntry
///////////////////////////////////////////////////////////////////////////////


void cHLAddressEntry::_cHLAddressEntry_Init(cHLAddresses *pHLAddresses,char *pszName)
{
	_bIsValid = false;
	_ulValue = 0;
	
	_pHLAddresses = pHLAddresses;

	_strName.assign(pszName);
	// by std::string() _strExpression.clear();

	_pHLAddresses->_RegisterEntry(this);
}

void cHLAddressEntry::_makeExpression_From_Value()
{
	static char pszTemp[2+33]="0x0"; // warning I don't know if static could cause problems with multithreading
	
	if (_ulValue) ultoa(_ulValue,pszTemp+2,16);
	else *(pszTemp+2)=*"0";	
	_strExpression.assign(pszTemp);
}

void cHLAddressEntry::_updatePrivateCopy(unsigned long* pulPrivateCopy, unsigned long ulValue)
{
	*pulPrivateCopy = ulValue;
}

void cHLAddressEntry::_updatePrivateCopies(unsigned long ulValue)
{
	std::list<unsigned long*>::iterator it;
	for (it = _lstPrivateCopies.begin();it!=_lstPrivateCopies.end();it++)
		_updatePrivateCopy(*it,ulValue);
}

cHLAddressEntry::cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName)
{
	_cHLAddressEntry_Init(pHLAddresses,pszName);
	SetValue(0);
}

cHLAddressEntry::cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,unsigned long ulValue)
{
	_cHLAddressEntry_Init(pHLAddresses,pszName);
	SetValue(ulValue);
}

cHLAddressEntry::cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,char* pszExpression,unsigned long *pulPrviateCopy)
{
	_cHLAddressEntry_Init(pHLAddresses,pszName);
	SetExpression(pszExpression);
	RegisterPrivateCopy(pulPrviateCopy);
}

cHLAddressEntry::~cHLAddressEntry()
{
	_pHLAddresses->_UnregisterEntry(this);
}

bool cHLAddressEntry::bIsValid()
{
	return _bIsValid;
}

void cHLAddressEntry::SetValue(unsigned long ulValue)
{
	_ulValue=ulValue;
	_makeExpression_From_Value();
	_bIsValid=true;
	_updatePrivateCopies(_ulValue);
}

bool cHLAddressEntry::SetExpression(char *pszExpression)
{
	unsigned long ulTemp;
	std::string *pstrTemp = new std::string(pszExpression);

	if (_pHLAddresses->EvaluateExpression(pstrTemp,ulTemp))
	{
		_ulValue = ulTemp;
		_strExpression=*pstrTemp;
		_bIsValid = true;
		_updatePrivateCopies(_ulValue);
	} else
		_bIsValid=false;

    delete pstrTemp;
	return _bIsValid;
}

void cHLAddressEntry::RegisterPrivateCopy(unsigned long *pulPrviateCopy)
{
	_updatePrivateCopy(pulPrviateCopy,_ulValue);
	_lstPrivateCopies.push_back(pulPrviateCopy);
}

void cHLAddressEntry::UnRegisterPrivateCopy(unsigned long *pulPrivateCopy)
{
	std::list<unsigned long*>::iterator it;
	for (it=_lstPrivateCopies.begin();it!=_lstPrivateCopies.end();it++)
		if (*it==pulPrivateCopy) _lstPrivateCopies.erase(it);
}

unsigned long cHLAddressEntry::GetValue()
{
	return _ulValue;
}

char *cHLAddressEntry::GetExpression()
{
	return 0;
}

char *cHLAddressEntry::GetName()
{
	return (char *)_strName.c_str();
}
/*

01dd0370 SCR_UpdateScreen:
...
01dd041e e83d74f8ff      call    launcher!CreateInterface+0x956471 (01d57860) == SCR_SetUpToDrawConsole ()
01dd0423 e8a8480000      call    launcher!CreateInterface+0x9d38e1 (01dd4cd0) == V_RenderView
01dd0428 e823b5f8ff      call    launcher!CreateInterface+0x95a561 (01d5b950) == GL_Set2D

V_RenderView calls 01d51d90 R_RenderView

*/