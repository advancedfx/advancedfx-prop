/*
File        : dll/hl_addresses.h
Started     : 2007-09-13 13:37:00
Project     : Mirv Demo Tool
Authors     : Gavin Bramhill, Dominik Tugend
Description : Address defintions inside Half-Life until we have an better system.
*/

#ifndef HL_ADDRESSES_H
#define HL_ADDRESSES_H

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
	~cHLAddresses(); // destructor

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
	cHLAddressEntry(cHLAddresses *pHLAddresses,char *pszName,char *pstrExpression,unsigned long *pulPrviateCopy);


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


//
// g_HLAddresses
//

extern cHLAddresses g_HLAddresses;

#define HL_ADDR_EXPOSE(name) \
	extern unsigned long g_HL_ADDR_##name;

#define HL_ADDR_GET(name) \
	g_HL_ADDR_##name

//
// Header exports of address variables (should be only for reading!):
// usage in code: use the HL_ADDR_GET(name) macro
//
HL_ADDR_EXPOSE(p_cl_enginefuncs_s)
HL_ADDR_EXPOSE(p_engine_studio_api_s)
HL_ADDR_EXPOSE(p_playermove_s)
HL_ADDR_EXPOSE(HUD_TOURIN)
HL_ADDR_EXPOSE(HUD_TOUROUT)
HL_ADDR_EXPOSE(R_RenderView)
HL_ADDR_EXPOSE(DTOURSZ_R_RenderView)
HL_ADDR_EXPOSE(Mod_LeafPVS)
HL_ADDR_EXPOSE(DTOURSZ_Mod_LeafPVS)
HL_ADDR_EXPOSE(r_refdef)
HL_ADDR_EXPOSE(HudSpectator_tfc)
HL_ADDR_EXPOSE(HudSpectator_cmp_tfc)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_ag_clofs)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_tfc)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_ns_clofs)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_valve)
HL_ADDR_EXPOSE(GetSoundtime)
HL_ADDR_EXPOSE(DTOURSZ_GetSoundtime)
HL_ADDR_EXPOSE(S_PaintChannels)
HL_ADDR_EXPOSE(DTOURSZ_S_PaintChannels)
HL_ADDR_EXPOSE(S_TransferPaintBuffer)
HL_ADDR_EXPOSE(DTOURSZ_S_TransferPaintBuffer)
HL_ADDR_EXPOSE(shm)
HL_ADDR_EXPOSE(paintbuffer)
HL_ADDR_EXPOSE(soundtime)
HL_ADDR_EXPOSE(paintedtime)
HL_ADDR_EXPOSE(CL_ParseServerMessage)
HL_ADDR_EXPOSE(net_message_cursize)
HL_ADDR_EXPOSE(net_message)
HL_ADDR_EXPOSE(msg_readcount)
HL_ADDR_EXPOSE(CL_ParseServerMessage_CmdRead)
HL_ADDR_EXPOSE(DTOURSZ_CL_ParseServerMessage_CmdRead)
HL_ADDR_EXPOSE(GetClientColor)
HL_ADDR_EXPOSE(DTOURSZ_GetClientColor)
HL_ADDR_EXPOSE(unkInlineClientColorA)
HL_ADDR_EXPOSE(SZ_unkInlineClientColorA)
HL_ADDR_EXPOSE(unkInlineClientColorV)
HL_ADDR_EXPOSE(SZ_unkInlineClientColorV)
HL_ADDR_EXPOSE(CLOFS_UnkCstrikeCrosshairFn)
HL_ADDR_EXPOSE(DTOURSZ_UnkCstrikeCrosshairFn)
HL_ADDR_EXPOSE(CLOFS_UnkCstrikeCh_mul_fac)
HL_ADDR_EXPOSE(CLOFS_UnkCstrikeCh_add_fac)
HL_ADDR_EXPOSE(R_PolyBlend)
HL_ADDR_EXPOSE(DTOURSZ_R_PolyBlend)
HL_ADDR_EXPOSE(UnkGetDecalTexture)
HL_ADDR_EXPOSE(DTOURSZ_UnkGetDecalTexture)
HL_ADDR_EXPOSE(R_DrawParticles)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawParticles)
HL_ADDR_EXPOSE(R_DrawEntitiesOnList)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawEntitiesOnList)
HL_ADDR_EXPOSE(R_DrawViewModel)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawViewModel)



#endif // #ifndef HL_ADDRESSES_H

