#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-01-04 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

bool csgo_Stdshader_dx9_Hooks_Init(void);

void csgo_Stdshader_dx9_Hooks_OnLevelShutdown(void);

class csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20
{
public:
	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int staticIndex, int dynamicIndex)
	{
		return CalcCombos(staticIndex +dynamicIndex);
	}

	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int index)
	{
		// DYNAMIC COMBOS:

		m_WRITEWATERFOGTODESTALPHA = index % 2;
		index = index / 2;

		m_CASCADE_SIZE = index % 1;
		index = index / 1;

		m_UNKNOWN_DYNAMIC_4 = index % 2;
		index = index / 2;

		// STATIC COMBOS:

		m_SFM = index % 1;
		index = index / 1;

		m_DETAILTEXTURE = index % 2;
		index = index / 2;

		m_CUBEMAP = index % 2;
		index = index / 2;

		m_DIFFUSELIGHTING = index % 2;
		index = index / 2;

		m_ENVMAPMASK = index % 2;
		index = index / 2;

		m_BASEALPHAENVMAPMASK = index % 2;
		index = index / 2;

		m_SELFILLUM = index % 2;
		index = index / 2;

		m_VERTEXCOLOR = index % 2;
		index = index / 2;

		m_FLASHLIGHT = index % 2;
		index = index / 2;

		m_SELFILLUM_ENVMAPMASK_ALPHA = index % 2;
		index = index / 2;

		m_DETAIL_BLEND_MODE = index % 10;
		index = index / 10;

		m_OUTLINE = index % 1;
		index = index / 1;

		m_OUTER_GLOW = index % 1;
		index = index / 1;

		m_SOFT_MASK = index % 1;
		index = index / 1;

		m_SEAMLESS_BASE = index % 1;
		index = index / 1;

		m_SEAMLESS_DETAIL = index % 1;
		index = index / 1;

		m_DISTANCEALPHA = index % 1;
		index = index / 1;

		m_DISTANCEALPHAFROMDETAIL = index % 1;
		index = index / 1;

		m_SHADER_SRGB_READ = index % 1;
		index = index / 1;

		m_DESATURATEWITHBASEALPHA = index % 2;
		index = index / 2;

		m_LIGHTNING_PREVIEW = index % 4;
		index = index / 4;

		m_ENVMAPFRESNEL = index % 2;
		index = index / 2;

		m_CASCADED_SHADOW_MAPPING = index % 1;
		index = index / 1;

		m_CSM_MODE = index % 1;
		index = index / 1;

		m_UNKNOWN_STATIC_60 = index % 1;
		index = index / 1;

		// index should be 0 now.
		return index;
	}

	// DYNAMIC COMBOS:
	int m_WRITEWATERFOGTODESTALPHA; // 0..1
	int m_CASCADE_SIZE; // 0..0
	int m_UNKNOWN_DYNAMIC_4; // 0..1 (maybe PIXELFOGTYPE 0..1)

	// STATIC COMBOS:
	int m_SFM; // 0..0
	int m_DETAILTEXTURE; // 0..1
	int m_CUBEMAP; // 0..1
	int m_DIFFUSELIGHTING; // 0..1
	int m_ENVMAPMASK; // 0..1
	int m_BASEALPHAENVMAPMASK; // 0..1
	int m_SELFILLUM; // 0..1
	int m_VERTEXCOLOR; // 0..1
	int m_FLASHLIGHT; // 0..1
	int m_SELFILLUM_ENVMAPMASK_ALPHA; // 0..1
	int m_DETAIL_BLEND_MODE; // 0..9
	int m_OUTLINE; // 0..0
	int m_OUTER_GLOW; // 0..0
	int m_SOFT_MASK; // 0..0
	int m_SEAMLESS_BASE; // 0..0
	int m_SEAMLESS_DETAIL; // 0..0
	int m_DISTANCEALPHA; // 0..0
	int m_DISTANCEALPHAFROMDETAIL; // 0..0
	int m_SHADER_SRGB_READ; // 0..0
	int m_DESATURATEWITHBASEALPHA; // 0..1
	int m_LIGHTNING_PREVIEW; // 0..3
	int m_ENVMAPFRESNEL; // 0..1
	int m_CASCADED_SHADOW_MAPPING; // 0..0
	int m_CSM_MODE; // 0..0
	int m_UNKNOWN_STATIC_60; // 0..0 (maybe DOPIXELFOG 0..0)
};

class csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20b
{
public:
	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int staticIndex, int dynamicIndex)
	{
		return CalcCombos(staticIndex +dynamicIndex);
	}

	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int index)
	{
		// DYNAMIC COMBOS:

		m_FLASHLIGHTSHADOWS = index % 2;
		index = index / 2;

		m_WRITEWATERFOGTODESTALPHA = index % 2;
		index = index / 2;

		m_CASCADE_SIZE = index % 2;
		index = index / 2;

		m_UNKNOWN_DYNAMIC_10 = index % 2;
		index = index / 2;

		// STATIC COMBOS:

		m_SFM = index % 1;
		index = index / 1;

		m_DETAILTEXTURE = index % 2;
		index = index / 2;

		m_CUBEMAP = index % 2;
		index = index / 2;

		m_DIFFUSELIGHTING = index % 2;
		index = index / 2;

		m_ENVMAPMASK = index % 2;
		index = index / 2;

		m_BASEALPHAENVMAPMASK = index % 2;
		index = index / 2;

		m_SELFILLUM = index % 2;
		index = index / 2;

		m_VERTEXCOLOR = index % 2;
		index = index / 2;

		m_FLASHLIGHT = index % 2;
		index = index / 2;

		m_SELFILLUM_ENVMAPMASK_ALPHA = index % 2;
		index = index / 2;

		m_DETAIL_BLEND_MODE = index % 10;
		index = index / 10;

		m_OUTLINE = index % 1;
		index = index / 1;

		m_OUTER_GLOW = index % 1;
		index = index / 1;

		m_SOFT_MASK = index % 1;
		index = index / 1;

		m_SEAMLESS_BASE = index % 1;
		index = index / 1;

		m_SEAMLESS_DETAIL = index % 1;
		index = index / 1;

		m_DISTANCEALPHA = index % 1;
		index = index / 1;

		m_DISTANCEALPHAFROMDETAIL = index % 1;
		index = index / 1;

		m_FLASHLIGHTDEPTHFILTERMODE = index % 4;
		index = index / 4;

		m_SHADER_SRGB_READ = index % 1;
		index = index / 1;

		m_DESATURATEWITHBASEALPHA = index % 2;
		index = index / 2;

		m_LIGHTNING_PREVIEW = index % 4;
		index = index / 4;

		m_ENVMAPFRESNEL = index % 2;
		index = index / 2;

		m_SRGB_INPUT_ADAPTER = index % 2;
		index = index / 2;

		m_CASCADED_SHADOW_MAPPING = index % 2;
		index = index / 2;

		m_CSM_MODE = index % 1;
		index = index / 1;

		m_UNKNOWN_STATIC_68 = index % 2;
		index = index / 2;

		// index should be 0 now.
		return index;
	}

	// DYNAMIC COMBOS:
	int m_FLASHLIGHTSHADOWS; // 0..1
	int m_WRITEWATERFOGTODESTALPHA; // 0..1
	int m_CASCADE_SIZE; // 0..1 // this is strange, it will always be 0
	int m_UNKNOWN_DYNAMIC_10; // 0..1 (maybe PIXELFOGTYPE 0..1)

	// STATIC COMBOS:
	int m_SFM; // 0..0
	int m_DETAILTEXTURE; // 0..1
	int m_CUBEMAP; // 0..1
	int m_DIFFUSELIGHTING; // 0..1
	int m_ENVMAPMASK; // 0..1
	int m_BASEALPHAENVMAPMASK; // 0..1
	int m_SELFILLUM; // 0..1
	int m_VERTEXCOLOR; // 0..1
	int m_FLASHLIGHT; // 0..1
	int m_SELFILLUM_ENVMAPMASK_ALPHA; // 0..1
	int m_DETAIL_BLEND_MODE; // 0..9
	int m_OUTLINE; // 0..0
	int m_OUTER_GLOW; // 0..0
	int m_SOFT_MASK; // 0..0
	int m_SEAMLESS_BASE; // 0..0
	int m_SEAMLESS_DETAIL; // 0..0
	int m_DISTANCEALPHA; // 0..0
	int m_DISTANCEALPHAFROMDETAIL; // 0..0
	int m_FLASHLIGHTDEPTHFILTERMODE; // 0..3 // this is strange, expected 0..2
	int m_SHADER_SRGB_READ; // 0..0
	int m_DESATURATEWITHBASEALPHA; // 0..1
	int m_LIGHTNING_PREVIEW; // 0..3
	int m_ENVMAPFRESNEL; // 0..1
	int m_SRGB_INPUT_ADAPTER; // 0..1 // yep they wasted another combo here, will always be 0
	int m_CASCADED_SHADOW_MAPPING; // 0..1
	int m_CSM_MODE; // 0..0
	int m_UNKNOWN_STATIC_68; // 0..1 (maybe DOPIXELFOG 0..1)
};

class csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps30
{
public:
	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int staticIndex, int dynamicIndex)
	{
		return CalcCombos(staticIndex +dynamicIndex);
	}

	/// <returns>Remainder, should be 0, otherwise indicates error</retruns>
	int CalcCombos(int index)
	{
		// DYNAMIC COMBOS:

		m_FLASHLIGHTSHADOWS = index % 2;
		index = index / 2;

		m_WRITEWATERFOGTODESTALPHA = index % 2;
		index = index / 2;

		m_UBERLIGHT = index % 2;
		index = index / 2;

		m_CASCADE_SIZE = index % 1;
		index = index / 1;

		m_UNKNOWN_DYNAMIC_10 = index % 2;
		index = index / 2;

		// STATIC COMBOS:

		m_SFM = index % 2;
		index = index / 2;

		m_DETAILTEXTURE = index % 2;
		index = index / 2;

		m_CUBEMAP = index % 2;
		index = index / 2;

		m_DIFFUSELIGHTING = index % 2;
		index = index / 2;

		m_ENVMAPMASK = index % 2;
		index = index / 2;

		m_BASEALPHAENVMAPMASK = index % 2;
		index = index / 2;

		m_SELFILLUM = index % 2;
		index = index / 2;

		m_VERTEXCOLOR = index % 2;
		index = index / 2;

		m_FLASHLIGHT = index % 2;
		index = index / 2;

		m_SELFILLUM_ENVMAPMASK_ALPHA = index % 2;
		index = index / 2;

		m_DETAIL_BLEND_MODE = index % 10;
		index = index / 10;

		m_OUTLINE = index % 1;
		index = index / 1;

		m_OUTER_GLOW = index % 1;
		index = index / 1;

		m_SOFT_MASK = index % 1;
		index = index / 1;

		m_SEAMLESS_BASE = index % 1;
		index = index / 1;

		m_SEAMLESS_DETAIL = index % 1;
		index = index / 1;

		m_DISTANCEALPHA = index % 1;
		index = index / 1;

		m_DISTANCEALPHAFROMDETAIL = index % 1;
		index = index / 1;

		m_FLASHLIGHTDEPTHFILTERMODE = index % 3;
		index = index / 3;

		m_SHADER_SRGB_READ = index % 1;
		index = index / 1;

		m_DESATURATEWITHBASEALPHA = index % 2;
		index = index / 2;

		m_LIGHTNING_PREVIEW = index % 4;
		index = index / 4;

		m_ENVMAPFRESNEL = index % 2;
		index = index / 2;

		m_CASCADED_SHADOW_MAPPING = index % 2;
		index = index / 2;

		m_CSM_MODE = index % 4;
		index = index / 4;

		m_UNKNOWN_STATIC_64 = index % 2;
		index = index / 2;

		// index should be 0 now.
		return index;
	}

	// DYNAMIC COMBOS:
	int m_FLASHLIGHTSHADOWS; // 0..1
	int m_WRITEWATERFOGTODESTALPHA; // 0..1
	int m_UBERLIGHT; // 0..1
	int m_CASCADE_SIZE; // 0..0
	int m_UNKNOWN_DYNAMIC_10; // 0..1 (maybe PIXELFOGTYPE 0..1)

	// STATIC COMBOS:
	int m_SFM; // 0..1
	int m_DETAILTEXTURE; // 0..1
	int m_CUBEMAP; // 0..1
	int m_DIFFUSELIGHTING; // 0..1
	int m_ENVMAPMASK; // 0..1
	int m_BASEALPHAENVMAPMASK; // 0..1
	int m_SELFILLUM; // 0..1
	int m_VERTEXCOLOR; // 0..1
	int m_FLASHLIGHT; // 0..1
	int m_SELFILLUM_ENVMAPMASK_ALPHA; // 0..1
	int m_DETAIL_BLEND_MODE; // 0..9
	int m_OUTLINE; // 0..0
	int m_OUTER_GLOW; // 0..0
	int m_SOFT_MASK; // 0..0
	int m_SEAMLESS_BASE; // 0..0
	int m_SEAMLESS_DETAIL; // 0..0
	int m_DISTANCEALPHA; // 0..0
	int m_DISTANCEALPHAFROMDETAIL; // 0..0
	int m_FLASHLIGHTDEPTHFILTERMODE; // 0..2
	int m_SHADER_SRGB_READ; // 0..0
	int m_DESATURATEWITHBASEALPHA; // 0..1
	int m_LIGHTNING_PREVIEW; // 0..3
	int m_ENVMAPFRESNEL; // 0..1
	int m_CASCADED_SHADOW_MAPPING; // 0..1
	int m_CSM_MODE; // 0..3
	int m_UNKNOWN_STATIC_64; // 0..1 (maybe DOPIXELFOG 0..1)
};
