// Project :  Half-Life Advanced Effects
// File    :  gui/demo_wizard.cpp

// Authors : last change / first change / name

// 2008-03-15 / 2008-03-15 / Dominik Tugend

// Comment: see gui/demo_wizard.h

///////////////////////////////////////////////////////////////////////////////

#include <wx/msgdlg.h>
#include <wx/filedlg.h>

#include "demo_fix.h"
#include "../debug.h"

#include "demo_tools.h"

CHlaeDemoTools::CHlaeDemoTools(wxWindow* parent)
{
	_parent = parent;

	_DemoFix = new CHlaeDemoFix();
}

CHlaeDemoTools::~CHlaeDemoTools()
{
	delete  _DemoFix;
}

void CHlaeDemoTools::Run()
{
	bool bRes=true;

	wxMessageDialog DlgMessage(_parent,wxT("Currently the Demo Tools only supports fixing demos that are corrupted\nbecause i.e. the game crashed during recording\n(so the directory entries are missing or malformed, in this case viewdemo reports\n\"WARNING! Demo had bogus number of directory entries!\")\n\nPress yes if you want to try to fix such a demo:"),wxT("What it does"),wxYES_NO|wxICON_INFORMATION);
	wxFileDialog DlgInfile(_parent,wxT("Select demo to fix ..."),wxEmptyString,wxEmptyString,wxT("Half-Life demo (*.dem)|*.dem"),wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	wxFileDialog DlgOutfile(_parent,wxT("Select name to save to ..."),wxEmptyString,wxEmptyString,wxT("Half-Life demo (*.dem)|*.dem"),wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	bRes = (wxID_YES == DlgMessage.ShowModal()) && (wxID_OK == DlgInfile.ShowModal()) && (wxID_OK == DlgOutfile.ShowModal());

	if (bRes)
	{
		_DemoFix->fix_demo(DlgInfile.GetPath().GetData(),DlgOutfile.GetPath().GetData());
	} else {
		g_debug.SendMessage(wxT("User abborted."),hlaeDEBUG_INFO);
	}

	DlgInfile.Destroy();
	DlgOutfile.Destroy();
}