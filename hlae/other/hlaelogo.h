#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes:
// 2009-11-14 dominik.matrixstorm.com

ref class HlaeLogo {
public:
	HlaeLogo();
	void Draw(System::Drawing::Graphics ^graphics, System::Drawing::RectangleF rect);
	void SetRed(bool value) {
		m_Red = value;
	}

private:
	bool m_Red;
	
};