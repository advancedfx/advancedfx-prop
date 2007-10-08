#include "hl_addresses.h"

//
// definitions of default addresses that need to be present early in case we use prints for debug:
//

//
// g_hl_addresses initalization:
//


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
};

#include <list>
#include <string>

typedef class cHLAddressEntry cHLAddressEntry_t; // forward decleration+


/* The MDT Addresses Config Syntax:

SIGMA = the alphabet (8 Bit American ASCII)
EPS = Epsylon (the empty word)
WS  = ' ' | '\t' (white space)

COMMENT = ';' (SIGMA / {'\0','\n'})*

CONST = 0x[0-9A-Fa-f]+
VAR = [A-Za-z_][0-9A-Za-z_]*

TERMINAL = CONST | VAR

EXPRESSION = TERMINAL 
           | EXPRESSION WS* ('+','-') WS* TERMINAL
	       | EXPRESSION WS* '^'

ALLOCATION = VAR WS* '=' WS* EXPRESSION

LINE = WS* (EPS | EXPRESSION WS* ) (EPS | COMMENT)

LINES = '\0'
       (LINE '\n')* LINE '\0'

*/

class cHLAddresses
{
private:
	std::list<cHLAddressEntry_t*> _HLAddressEntryList;
	void _RegisterEntry(cHLAddressEntry_t *pHLAddressEntry);
	void _UnregisterEntry(cHLAddressEntry_t *pHLAddressEntry);
public:
	cHLAddressEntry* GiveEntry(char *pszName); // retrives entry by name

	bool EvaluateExpression(std::string *pstrExpression,unsigned long &pulValue);
	// tries to parse an Expression.
	// if it succeeds it returns true and pulValue is the Value of the expression.
	// if it fails (syntax or seantic error (used uninitalized or unknown variable)) it returns false
	
	friend cHLAddressEntry_t;
};

void cHLAddresses::_RegisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	_HLAddressEntryList.push_back(pHLAddressEntry);
}

void cHLAddresses::_UnregisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	std::list<cHLAddressEntry_t*>::iterator it;
	for (it=_HLAddressEntryList.begin();it!=_HLAddressEntryList.end();it++)
		if (*it==pHLAddressEntry) _HLAddressEntryList.erase(it); // WARNING: this asumes address of object won't change
}

// todo: cHLAddressEntry* cHLAddresses::GiveEntry(char *pszName); // retrives entry by name

bool cHLAddresses::EvaluateExpression(std::string *pstrExpression,unsigned long &pulValue)
{
	//
	// todo
	//
	return false;
}

class cHLAddressEntry
{
private:
	bool _bIsValid;
	unsigned long _ulValue;

	std::list<unsigned long*> _lstPrivateCopies;

	std::string _strName;
	std::string _strExpression;

	cHLAddresses *_pHLAddresses;

	void _cHLAddressEntry_Init(cHLAddresses *pHLAddresses,char *pszName);

	void _makeExpression_From_Value();

	void _updatePrivateCopy(unsigned long* pulPrivateCopy, unsigned long ulValue);
	void _updatePrivateCopies(unsigned long ulValue);

public:
	cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName);
	cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,unsigned long ulValue);
	cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,char *pstrExpression);

	~cHLAddressEntry();

	bool bIsValid();

	void SetValue(unsigned long ulValue);
	bool SetExpression(char *pszExpression);

	void RegisterPrivateCopy(unsigned long *pulPrviateCopy);
	void UnRegisterPrivateCopy(unsigned long *pulPrivateCopy);

	unsigned long GetValue();
	char *GetExpression();
};

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

cHLAddressEntry::cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,char* pszExpression)
{
	_cHLAddressEntry_Init(pHLAddresses,pszName);
	SetExpression(pszExpression);
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

/*

01dd0370 SCR_UpdateScreen:
...
01dd041e e83d74f8ff      call    launcher!CreateInterface+0x956471 (01d57860) == SCR_SetUpToDrawConsole ()
01dd0423 e8a8480000      call    launcher!CreateInterface+0x9d38e1 (01dd4cd0) == V_RenderView
01dd0428 e823b5f8ff      call    launcher!CreateInterface+0x95a561 (01d5b950) == GL_Set2D

V_RenderView calls 01d51d90 R_RenderView

*/