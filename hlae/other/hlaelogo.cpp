#include "StdAfx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes:
// 2009-11-14 dominik.matrixstorm.com

#include "hlaelogo.h"

using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;

// HlaeLogo ////////////////////////////////////////////////////////////////////

HlaeLogo::HlaeLogo() {

}

void HlaeLogo::Draw(System::Drawing::Graphics ^graphics, System::Drawing::RectangleF rect) {
	float size = rect.Height <= rect.Width ? rect.Height : rect.Width -1; // scale

	RectangleF r(
		(rect.Width -size) / 2 +rect.X,
		(rect.Height -size) / 2 +rect.Y,
		size,size
	);

	graphics->FillEllipse(Brushes::Turquoise, r);
	graphics->FillEllipse(Brushes::Aqua, r.Left +0.1f*size, r.Top +0.1f*size, 0.8f*size, 0.8f*size);
	graphics->FillEllipse(Brushes::Aquamarine, r.Left +0.2f*size, r.Top +0.2f*size, 0.6f*size, 0.6f*size);
	graphics->FillEllipse(m_Red ? Brushes::Yellow : Brushes::Olive, r.Left +0.3f*size, r.Top +0.3f*size, 0.4f*size, 0.4f*size);
	graphics->FillEllipse(m_Red ? Brushes::Red : Brushes::Black, r.Left +0.4f*size, r.Top +0.4f*size, 0.2f*size, 0.2f*size
	);
}
