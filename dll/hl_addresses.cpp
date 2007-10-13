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


/*

The MDT Addresses Config Syntax:

SIGMA = the alphabet (8 Bit American ASCII)
EPS = Epsylon (the empty word)
WS  = ' ' | '\t' (white space)

COMMENT = ';' (SIGMA / {'\0','\n'})*

CONST = 0x[0-9A-Fa-f]+
actually CONST has a few more restrictions (those of strtoul)
VAR = [A-Za-z_][0-9A-Za-z_]*

TERMINAL = CONST | VAR

EXPRESSION = TERMINAL 
           | EXPRESSION WS* ('+','-') WS* TERMINAL
	       | EXPRESSION WS* '^'

ALLOCATION = VAR WS* '=' WS* EXPRESSION

LINE = WS* (EPS |ALLOCATION WS* ) (EPS | COMMENT)

LINES = '\0'
       (LINE '\n')* LINE '\0'


The MDT Addresses Config Semantics:

The following things have to be ensured by the programmer using the object:

- Variables should be registered with the Object before any config is loaded.
  Also variables shouldn't be unregistered if any config action could take
  place later; The reason is expressions of still existing variables might
  rellay on them and an requested reevaluation may fail.

- Variable names must match the syntax or otherwise they will be inaccessable
  to the config parsing system.

- The expressions used for default values of variables must be valid in the
  order they will be applied (object initialisation order). Otherwise there
  will be no valid fall back values in case the config syntax is broken.

The following things are checked and forced by the config system:

- Variables have to have been initalized / allocated (ALLOCATION) before they
  are allowed to be used in expressions. The alloaction order is the config's
  line order (first line parsed first).
  
- Expressions are calculated using an accumulator, all expressions apply to the
  accumulated value, so the operator precendence (First Come First Served)
  differs from the one you may be used to from mathematics.

*/

class cHLAddresses
{
public:
	#define MDT_OPCHAR_PLUS '+'
	#define MDT_OPCHAR_MINUS '-'
	#define MDT_OPCHAR_INDIRECTION '^'
	#define MDT_CHAR_ALLOCATION '='

	enum MDT_PRESULT // Result indication type for parsing functions
	{
		MDTP_OK,				// OK means there was no error till the currently parsed position
		MDTP_EUNKNOWN,			// unknown error
		MDTP_EPROGRAM,			// s.th. is wrong with the program (MDT) if this happend
		MDTP_EMEMALLOC,			// memory allocation failed (currently not signaled, program might just crash)
		MDTP_ESYNTAX,			// Syntax error
		MDTP_ESEMANTICS,		// (unspecified) Semantic error
		MDTP_ESEM_VARUNK,		// Semantic error: unknown variable was used
		MDTP_ESEM_VARUNINIT,	// Semantic error: the variable is non but has not been initalized / is not valid
		MDTP_EINDIRECTION,		// Error: Indirection failed
	};
    
private:
	std::list<cHLAddressEntry_t*> _HLAddressEntryList;
	void _RegisterEntry(cHLAddressEntry_t *pHLAddressEntry);
	void _UnregisterEntry(cHLAddressEntry_t *pHLAddressEntry);

public:
	cHLAddressEntry* GiveEntry(char *pszName); // retrives entry by name

	bool Do_Indirection(unsigned long ulPointer,unsigned long & ulPointed);
	// tries indirect ulPointer and return the result in ulPointed
	// if it returns false s.th. went wron for sure
	// please understand that the protections for indirection are (currently very) limited.
	// in the future i.e. addresses should be limited to being in the launcher module), but that is not the case yet, also the whole class structure would have to be extended to only allow such addresses.

	MDT_PRESULT Parse_WhiteSpaceKleene(char *pszInput,char *&pszRemainder);
	// parses WS*
	// munches as much WhiteSpace (WS) as it can. in it's current implementation (asuming supplied arguments are valid) it never fails.
	// the returned pszRemainder will point on the first non WhiteSpace char

	MDT_PRESULT Parse_Variable(char *pszInput,char *&pszRemainder,cHLAddressEntry *&pEntry);
	// parses VARIABLE
	// on MDTP_OK pEntry points onto the variable entry that has been found
	// on otherwise pEntry will be 0 (NULL)
	// returned pszRemainder always points onto the first char that could not be parsed (yet)

	MDT_PRESULT Parse_Terminal(char *pszInput,char *&pszRemainder,unsigned long &ulValue);
	// tries to parse TERMINAL
	// on MDT_OK ulValue contains the current value of the expression
	// on otherwise the result indicates parsing errors / problems and ulValue remains untouched
	// returned pszRemainder always points on the first char that could not be parsed (yet)

	MDT_PRESULT Parse_Expression(char *pszInput,char *&pszRemainder,unsigned long &ulValue);
	// tries to parse EXPRESSION
	// on MDT_OK pszRemainder points onto the next char that could not be parsed and ulValue is the Value of the expression
	// on otherwise pszRemainder points onto the char where an error was identified

	bool EvaluateExpression(std::string *pstrExpression,unsigned long &pulValue);
	// this is a wrapper around Parse_Expression for use by cHLAddressEntry.
	// on MDTP_OK it would return true, otherwise false

	MDT_PRESULT Parse_Allocation(char *pszInput,char *&pszRemainder);
	// tries to parse ALLOCATION
	// on MDT_OK the ALLOCATION was parsed successfully and the variable has been updated
	// on otherwise an error happend (the variable should remain unchanged in this case)
	// returned pszRemainder always points on the first char that could not be parsed (yet)

	MDT_PRESULT Parse_Comment(char *pszInput,char *&pszRemainder);
	// tries to parse COMMENT
	// returned pszRemainder always points on the first char that could not be parsed (yet or anymore)

	MDT_PRESULT Parse_Line(char *pszInput,char *&pszRemainder);
	// tries to parse LINE
	// on MDTP_OK the parsing seems to be ok.
	// returned pszRemainder always points on the first char that could not be parsed (yet or anymore)

	MDT_PRESULT Parse_AddressConfig(char *pszInput,unsigned long & ulErrLinesTotal,unsigned long & ulFirstErrLine, char *& pFirstErrorLine, char *& pFirstErrorChar);
	// parses LINES with BEST EFFORT:
	// if One Line fails it will still try to parse the following lines, hoping they are independant.
	// on MDTP_OK there was no error and the return Values of the other returns will be 0 or NULL
	// on otherwise there was at least one error (the code is the one of the first that happend) and ulErrorNum, ulFirstErrLine, ulFirstErrCol, pErrorChar will be set accordingly
	// description of the other returns:
	// ulErrLinesTotal: the total number of errorneous lines
	// ulFirstErrLine: the line number, where the first error happend, the first Line is 1.
    // pFirstErrorLine: A pointer onto the line in pszInput, where the first error happend.
	// pFirstErrorChar: A pointer onto the char in pszInput, where the first error happend.
	// hints:
	// you can retrive the col number where the first error happend by (unsigned long)(pFirstErrorChar - pFirstErrorLine)+1
	// you can retrive an (English) error description string if you pass the result (MDT_PRESULT) to the GiveErrorString function
	// this is the only function that checks pszInput == 0 == NULL, in this case it will return an MDTP_ESYNTAX error (and the other returns will be all 0)

	friend cHLAddressEntry_t;
};

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
	char *GetName();
};

void cHLAddresses::_RegisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	_HLAddressEntryList.push_back(pHLAddressEntry);
}

void cHLAddresses::_UnregisterEntry(cHLAddressEntry_t *pHLAddressEntry)
{
	std::list<cHLAddressEntry_t*>::iterator it;
	for (it=_HLAddressEntryList.begin();it!=_HLAddressEntryList.end();it++)
		if (*it==pHLAddressEntry) _HLAddressEntryList.erase(it); // WARNING: this asumes addresses of object won't change
}

cHLAddressEntry* cHLAddresses::GiveEntry(char *pszName)
{
	std::list<cHLAddressEntry*>::iterator it;
	for (it=_HLAddressEntryList.begin();it!=_HLAddressEntryList.end();it++)
	{
		if(strcmp((*it)->GetName(),pszName)==0)
			return (*it);
	}
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
	pEntry=0; // NULL
	
	// variable strings may not be empty:
	if(cTmp==0) return MDTP_ESYNTAX;
	
	// check [A-Za-z_]
	if (!( (cTmp>='A' && cTmp<='Z') || (cTmp>='a' && cTmp<='z') || (cTmp=='_') )) return MDTP_ESYNTAX;

	// parse [0-9A-Za-z_]*
	do cTmp = *(pszRemainder++); while ( (cTmp>='0' && cTmp <='9') || (cTmp>='A' && cTmp<='Z') || (cTmp>='a' && cTmp<='z') || (cTmp=='_') );

	// well I seriously thought if we should simply modify the original string temporary or not (put null in and preserve and restore original value)
	// but finally I decided I should rather make a copy of it for some unknown reason:

	char *pszT;

	MDT_PRESULT mRes=MDTP_OK;

	// get memory:
	try
	{
		pszT = new char[pszRemainder-pszInput+1];
	}
	catch(...)
	{
		mRes=MDTP_EMEMALLOC;
	}

	if (mRes==MDTP_OK)
	{
		// only when allocation was successfull
		memcpy(pszT,pszInput,pszRemainder-pszInput);
		pszT[pszRemainder-pszInput]=0; // place NULL termination char

		// retrive entry if possible:
		if (!(pEntry=GiveEntry(pszT)))
			mRes=MDTP_ESEM_VARUNK; // entry not found
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

		if (mRes==MDTP_OK && pEntry && pEntry->bIsValid())
			ulValue=pEntry->GetValue();
		else mRes=MDTP_ESEM_VARUNINIT; // not valid for use
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
			if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszT,pszT))) return mRes; // munch WS* (if any)
			break;
		case MDT_OPCHAR_MINUS:
			mAccuOp = MDTAO_MINUS;
			if (MDTP_OK != (mRes = Parse_WhiteSpaceKleene(pszT,pszT))) return mRes; // munch WS* (if any)
			break;
		default:
			// there was no op
			return MDTP_OK; // quit instantly (returns current pszRemainder and accu)
		}

		// if we are here the expression syntax is not finished yet, so update the pszRemainder:
		pszRemainder = pszT; // update!!
	}
	return MDTP_OK;
}

bool cHLAddresses::EvaluateExpression(std::string *pstrExpression,unsigned long &pulValue)
{
	static char *pszDummy;
	return Parse_Expression((char *)pstrExpression->c_str(),(char *)pszDummy,pulValue)==MDTP_OK;
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
	do cTmp=(pszRemainder++)[0];
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
	if (pszInput=0) return MDTP_ESYNTAX; // actually this should have never been supplied, but we are a friendly kind :)

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
	}


	return mRes;
}

/////

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