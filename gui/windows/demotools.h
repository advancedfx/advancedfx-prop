// Project :  Half-Life Advanced Effects
// File    :  gui/demo_wizard.h

// Authors : last change / first change / name

// 2008-03-15 / 2008-03-15 / Dominik Tugend

// Comment: Contains the demo fix wizard.

#ifndef HLAE_DEMOTOOLS_H
#define HLAE_DEMOTOOLS_H

class CHlaeDemoFix;

class CHlaeDemoTools
{
public:
    CHlaeDemoTools(wxWindow* parent);
	~CHlaeDemoTools();

	void Run();
private:
	CHlaeDemoFix * _DemoFix;
	wxWindow * _parent;

};

#endif