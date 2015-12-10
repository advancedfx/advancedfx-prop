#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-06 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

#include "csgo_ShaderApi_Hooks.h"

#include "asmClassTools.h"

#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxShaderShadow
: public IShaderShadow_csgo
, public IAfxShaderShadow
{
private:
	IShaderShadow_csgo * m_Parent;
	IAfxShaderShadowSetVertexShader * m_OnSetVertexShader;
	IAfxShaderShadowSetPixelShader * m_OnSetPixelShader;
	IAfxShaderShadowEnableColorWrites * m_OnEnableColorWrites;
	IAfxShaderShadowEnableAlphaWrites * m_OnEnableAlphaWrites;

public:
	CAfxShaderShadow(IShaderShadow_csgo * parent)
	: m_Parent(parent)
	, m_OnSetVertexShader(0)
	, m_OnSetPixelShader(0)
	, m_OnEnableColorWrites(0)
	, m_OnEnableAlphaWrites(0)
	{
	}

	void SetParent(IShaderShadow_csgo * parent)
	{
		m_Parent = parent;
	}

	void OnSetVertexShader_set(IAfxShaderShadowSetVertexShader * value)
	{
		m_OnSetVertexShader = value;
	}

	void OnSetPixelShader_set(IAfxShaderShadowSetPixelShader * value)
	{
		m_OnSetPixelShader = value;
	}

	void OnEnableColorWrites_set(IAfxShaderShadowEnableColorWrites * value)
	{
		m_OnEnableColorWrites = value;
	}

	void OnEnableAlphaWrites_set(IAfxShaderShadowEnableAlphaWrites * value)
	{
		m_OnEnableAlphaWrites = value;
	}

	//
	// IShaderShadow_csgo:

	virtual void _UNKOWN_000(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 0) }

	virtual void _UNKOWN_001(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 1) }

	virtual void _UNKOWN_002(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 2) }

	virtual void _UNKOWN_003(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 3) }

	virtual void _UNKOWN_004(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 4) }

	virtual void EnableColorWrites( bool bEnable )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 5)

		m_Parent->EnableColorWrites(
			m_OnEnableColorWrites ? m_OnEnableColorWrites->EnableColorWrites(this, bEnable) : bEnable
		);
	}

	virtual void EnableAlphaWrites( bool bEnable )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 6)
	
		m_Parent->EnableAlphaWrites(
			m_OnEnableAlphaWrites ? m_OnEnableAlphaWrites->EnableAlphaWrites(this, bEnable) : bEnable
		);
	}

	virtual void _UNKOWN_007(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 7) }

	virtual void _UNKOWN_008(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 8) }

	virtual void _UNKOWN_009(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 9) }

	virtual void _UNKOWN_010(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 10) }

	virtual void _UNKOWN_011(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 11) }

	virtual void _UNKOWN_012(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 12) }

	virtual void _UNKOWN_013(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 13) }

	virtual void _UNKOWN_014(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 14) }

	virtual void _UNKOWN_015(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 15) }

	virtual void _UNKOWN_016(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 16) }

	virtual void SetVertexShader( const char* pFileName, int nStaticVshIndex )
	{
		//JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 17)
	
		if(m_OnSetVertexShader) m_OnSetVertexShader->SetVertexShader(this, pFileName, nStaticVshIndex);

		m_Parent->SetVertexShader(pFileName, nStaticVshIndex);
	}

	virtual	void SetPixelShader( const char* pFileName, int nStaticPshIndex = 0 )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 18)

		if(m_OnSetPixelShader) m_OnSetPixelShader->SetPixelShader(this, pFileName, nStaticPshIndex);

		m_Parent->SetPixelShader(pFileName, nStaticPshIndex);
	}

	virtual void EnableSRGBWrite( bool bEnable )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 19) }

	virtual void _UNKOWN_020(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 20) }

	virtual void _UNKOWN_021(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 21) }

	virtual void _UNKOWN_022(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 22) }

	virtual void _UNKOWN_023(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 23) }

	virtual void _UNKOWN_024(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 24) }

	virtual void _UNKOWN_025(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 25) }

	virtual void _UNKOWN_026(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 26) }

	virtual void _UNKOWN_027(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 27) }

	virtual void _UNKOWN_028(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 28) }

	virtual void _UNKOWN_029(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 29) }

	virtual void _UNKOWN_030(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 30) }

	virtual void _UNKOWN_031(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 31) }

	virtual void _UNKOWN_032(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 32) }

	virtual void _UNKOWN_033(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderShadow, m_Parent, 33) }
} g_AfxShaderShadow(0);

class CAfxShaderDynamicAPI
: public IShaderDynamicAPI_csgo
, public IAfxShaderDynamicAPI
{
private:
	IShaderDynamicAPI_csgo * m_Parent;
	IAfxShaderDynamicAPISetVertexShaderIndex * m_OnSetVertexShaderIndex;
	IAfxShaderDynamicAPISetPixelShaderIndex * m_OnSetPixelShaderIndex;

public:
	CAfxShaderDynamicAPI(IShaderDynamicAPI_csgo * parent)
	: m_Parent(parent)
	, m_OnSetVertexShaderIndex(0)
	, m_OnSetPixelShaderIndex(0)
	{
	}

	void SetParent(IShaderDynamicAPI_csgo * parent)
	{
		m_Parent = parent;
	}

	void OnSetVertexShaderIndex_set(IAfxShaderDynamicAPISetVertexShaderIndex * value)
	{
		m_OnSetVertexShaderIndex = value;
	}

	void OnSetPixelShaderIndex_Set(IAfxShaderDynamicAPISetPixelShaderIndex * value)
	{
		m_OnSetPixelShaderIndex = value;
	}

	//
	// IShaderDynamicAPI_csgo:

	virtual void _UNKOWN_000(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 0) }

	virtual void _UNKOWN_001(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 1) }

	virtual void _UNKOWN_002(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 2) }

	virtual void _UNKOWN_003(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 3) }

	virtual void _UNKOWN_004(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 4) }

	virtual void _UNKOWN_005(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 5) }

	virtual void _UNKOWN_006(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 6) }

	virtual void _UNKOWN_007(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 7) }

	virtual void _UNKOWN_008(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 8) }

	virtual void _UNKOWN_009(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 9) }

	virtual void _UNKOWN_010(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 10) }

	virtual void _UNKOWN_011(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 11) }

	virtual void _UNKOWN_012(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 12) }

	virtual void _UNKOWN_013(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 13) }

	virtual void _UNKOWN_014(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 14) }

	virtual void _UNKOWN_015(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 15) }

	virtual void _UNKOWN_016(void) 
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 16) }

	virtual void _UNKOWN_017(void) 
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 17) }

	virtual void _UNKOWN_018(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 18) }

	virtual void SetVertexShaderIndex( int vshIndex = -1 )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 19)
	
		if(m_OnSetVertexShaderIndex) m_OnSetVertexShaderIndex->SetVertexShaderIndex(this, vshIndex);

		m_Parent->SetVertexShaderIndex(vshIndex);
	}

	virtual void SetPixelShaderIndex( int pshIndex = 0 )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 20)
	
		if(m_OnSetPixelShaderIndex) m_OnSetPixelShaderIndex->SetPixelShaderIndex(this, pshIndex);

		m_Parent->SetPixelShaderIndex(pshIndex);
	}

	virtual void _UNKOWN_021(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 21) }

	virtual void _UNKOWN_022(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 22) }

	virtual void _UNKOWN_023(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 23) }

	virtual void _UNKOWN_024(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 24) }

	virtual void _UNKOWN_025(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 25) }

	virtual void _UNKOWN_026(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 26) }

	virtual void _UNKOWN_027(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 27) }

	virtual void _UNKOWN_028(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 28) }

	virtual void _UNKOWN_029(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 29) }

	virtual void _UNKOWN_030(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 30) }

	virtual void _UNKOWN_031(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 31) }

	virtual void _UNKOWN_032(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 32) }

	virtual void _UNKOWN_033(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 33) }

	virtual void _UNKOWN_034(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 34) }

	virtual void _UNKOWN_035(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 35) }

	virtual void _UNKOWN_036(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 36) }

	virtual void _UNKOWN_037(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 37) }

	virtual void _UNKOWN_038(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 38) }

	virtual void _UNKOWN_039(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 39) }

	virtual void _UNKOWN_040(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 40) }

	virtual void _UNKOWN_041(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 41) }

	virtual void _UNKOWN_042(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 42) }

	virtual void _UNKOWN_043(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 43) }

	virtual void _UNKOWN_044(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 44) }

	virtual void _UNKOWN_045(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 45) }

	virtual void _UNKOWN_046(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 46) }

	virtual void _UNKOWN_047(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 47) }

	virtual void _UNKOWN_048(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 48) }

	virtual void _UNKOWN_049(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 49) }

	virtual void _UNKOWN_050(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 50) }

	virtual void _UNKOWN_051(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 51) }

	virtual void _UNKOWN_052(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 52) }

	virtual void _UNKOWN_053(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 53) }

	virtual void _UNKOWN_054(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 54) }

	virtual void _UNKOWN_055(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 55) }

	virtual void _UNKOWN_056(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 56) }

	virtual void _UNKOWN_057(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 57) }

	virtual void _UNKOWN_058(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 58) }

	virtual void _UNKOWN_059(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 59) }

	virtual void _UNKOWN_060(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 60) }

	virtual void _UNKOWN_061(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 61) }

	virtual void _UNKOWN_062(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 62) }

	virtual void _UNKOWN_063(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 63) }

	virtual void _UNKOWN_064(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 64) }

	virtual void _UNKOWN_065(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 65) }

	virtual void _UNKOWN_066(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 66) }

	virtual void _UNKOWN_067(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 67) }

	virtual void _UNKOWN_068(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 68) }

	virtual void _UNKOWN_069(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 69) }

	virtual void _UNKOWN_070(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 70) }

	virtual void _UNKOWN_071(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 71) }

	virtual void _UNKOWN_072(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 72) }

	virtual void _UNKOWN_073(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 73) }

	virtual void _UNKOWN_074(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 74) }

	virtual void _UNKOWN_075(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 75) }

	virtual void _UNKOWN_076(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 76) }

	virtual void _UNKOWN_077(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 77) }

	virtual void _UNKOWN_078(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 78) }

	virtual void _UNKOWN_079(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 79) }

	virtual void _UNKOWN_080(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 80) }

	virtual void _UNKOWN_081(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 81) }

	virtual void _UNKOWN_082(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 82) }

	virtual void _UNKOWN_083(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 83) }

	virtual void _UNKOWN_084(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 84) }

	virtual void _UNKOWN_085(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 85) }

	virtual void _UNKOWN_086(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 86) }

	virtual void _UNKOWN_087(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 87) }

	virtual void _UNKOWN_088(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 88) }

	virtual void _UNKOWN_089(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 89) }

	virtual void _UNKOWN_090(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 90) }

	virtual void _UNKOWN_091(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 91) }

	virtual void _UNKOWN_092(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 92) }

	virtual void _UNKOWN_093(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 93) }

	virtual void _UNKOWN_094(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 94) }

	virtual void _UNKOWN_095(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 95) }

	virtual void _UNKOWN_096(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 96) }

	virtual void _UNKOWN_097(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 97) }

	virtual void _UNKOWN_098(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 98) }

	virtual void _UNKOWN_099(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 99) }

	virtual void _UNKOWN_100(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 100) }

	virtual void _UNKOWN_101(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 101) }

	virtual void _UNKOWN_102(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 102) }

	virtual void _UNKOWN_103(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 103) }

	virtual void _UNKOWN_104(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 104) }

	virtual void _UNKOWN_105(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 105) }

	virtual void _UNKOWN_106(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 106) }

	virtual void _UNKOWN_107(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 107) }

	virtual void _UNKOWN_108(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 108) }

	virtual void _UNKOWN_109(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 109) }

	virtual void _UNKOWN_110(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 110) }

	virtual void _UNKOWN_111(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 111) }

	virtual void _UNKOWN_112(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 112) }

	virtual void _UNKOWN_113(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 113) }

	virtual void _UNKOWN_114(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 114) }

	virtual void _UNKOWN_115(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 115) }

	virtual void _UNKOWN_116(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 116) }

	virtual void _UNKOWN_117(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 117) }

	virtual void _UNKOWN_118(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 118) }

	virtual void _UNKOWN_119(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 119) }

	virtual void _UNKOWN_120(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 120) }

	virtual void _UNKOWN_121(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 121) }

	virtual void _UNKOWN_122(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 122) }

	virtual void _UNKOWN_123(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 123) }

	virtual void _UNKOWN_124(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 124) }

	virtual void _UNKOWN_125(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 125) }

	virtual void _UNKOWN_126(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 126) }

	virtual void _UNKOWN_127(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 127) }

	virtual void _UNKOWN_128(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 128) }

	virtual void _UNKOWN_129(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 129) }

	virtual void _UNKOWN_130(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 130) }

	virtual void _UNKOWN_131(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 131) }

	virtual void _UNKOWN_132(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 132) }

	virtual void _UNKOWN_133(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 133) }

	virtual void _UNKOWN_134(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 134) }

	virtual void _UNKOWN_135(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 135) }

	virtual void _UNKOWN_136(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 136) }

	virtual void _UNKOWN_137(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 137) }

	virtual void _UNKOWN_138(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 138) }

	virtual void _UNKOWN_139(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 139) }

	virtual void _UNKOWN_140(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 140) }

	virtual void _UNKOWN_141(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 141) }

	virtual void _UNKOWN_142(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 142) }

	virtual void _UNKOWN_143(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 143) }

	virtual void _UNKOWN_144(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 144) }

	virtual void _UNKOWN_145(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 145) }

	virtual void _UNKOWN_146(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxShaderDynamicAPI, m_Parent, 146) }
} g_AfxShaderDynamicAPI(0);

#pragma warning(pop)

IShaderShadow_csgo * Wrap_IShaderShadow_csgo(IShaderShadow_csgo * parent,
	IAfxShaderShadowSetVertexShader * onSetVertexShader,
	IAfxShaderShadowSetPixelShader * onSetPixelShader,
	IAfxShaderShadowEnableColorWrites * onEnableColorWrites,
	IAfxShaderShadowEnableAlphaWrites * onEnableAlphaWrites)
{
	if(!parent) return 0;

	g_AfxShaderShadow.SetParent(parent);
	g_AfxShaderShadow.OnSetVertexShader_set(onSetVertexShader);
	g_AfxShaderShadow.OnSetPixelShader_set(onSetPixelShader);
	g_AfxShaderShadow.OnEnableColorWrites_set(onEnableColorWrites);
	g_AfxShaderShadow.OnEnableAlphaWrites_set(onEnableAlphaWrites);

	return &g_AfxShaderShadow;
}

IShaderDynamicAPI_csgo * Wrap_IShaderDynamicAPI_csgo(IShaderDynamicAPI_csgo * parent, IAfxShaderDynamicAPISetVertexShaderIndex * onSetVertexShaderIndex, IAfxShaderDynamicAPISetPixelShaderIndex * onSetPixelShaderIndex)
{
	if(!parent) return 0;

	g_AfxShaderDynamicAPI.SetParent(parent);
	g_AfxShaderDynamicAPI.OnSetVertexShaderIndex_set(onSetVertexShaderIndex);
	g_AfxShaderDynamicAPI.OnSetPixelShaderIndex_Set(onSetPixelShaderIndex);

	return &g_AfxShaderDynamicAPI;
}
