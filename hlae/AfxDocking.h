#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-01-02 by dominik.matrixstorm.com
//
// First changes:
// 2010-01-02 by dominik.matrixstorm.com

// Description:
// Interfaces and baseclasses for dockable controls.

using namespace System;

namespace hlae {

enum class DockControlTarget
{
	Top,
	Left,
	Right,
	Bottom,
	Tab
};


/// <summary> A control that 
interface class IDockControl
{
	void Dock(System::Windows::Forms::Control ^ dockableControl, DockControlTarget target);

	//
	// Properties:

	property System::Drawing::Rectangle ScreenBounds {
		System::Drawing::Rectangle get();
	}


};

} // namespace hlae {
