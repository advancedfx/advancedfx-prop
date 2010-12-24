#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-08-01 dominik.matrixstorm.com
//
// First changes
// 2008-11-10 dominik.matrixstorm.com


// Description: Counter-Strike 1.6 cross-hair fix related.


/// <summary>Whether to prevent cross-hair cool-down for the current pass (true) or not (false).</summary>
extern bool g_Cstrike_CrossHair_Block;

/// <returns>Current setting for the cross-hair fix.</returns>
double Cstrike_CrossHair_Fps_get();

/// <summary>Enables the CrossHair fix (0.0 < value) or disables it (value &lt;= 0)</summary>
void Cstrike_CrossHair_Fps_set(double value);

/// <summary>Installs the cstrike cross-hair fix hook.</summary>
void Hook_Cstrike_CrossHair_Fix();