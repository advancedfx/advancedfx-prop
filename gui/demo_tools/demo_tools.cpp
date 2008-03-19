// Project :  Half-Life Advanced Effects
// File    :  gui/demo_wizard.cpp

// Authors : last change / first change / name

// 2008-03-18 / 2008-03-15 / Dominik Tugend

// Comment: see gui/demo_wizard.h

///////////////////////////////////////////////////////////////////////////////



#include <wx/wizard.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>

#include "demo_fix.h"
#include "../debug.h"

#include "demo_tools.h"


//
// The hlaeDemoWizard classes:
//

//
// hlaeDemoWizardTaskPage
//

class hlaeDemoTaskPage : public wxWizardPage
{
public:
    hlaeDemoTaskPage( wxWizard *parent ): wxWizardPage(parent)
    {
        m_prev = NULL;
        m_next = NULL;

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(
			new wxStaticText(this, wxID_ANY, wxT("Please select the tasks you want to carry out (at least one):")),
            0, // No vertical stretching
            wxALL,
            5 // Border width
        );

		mainSizer->AddSpacer(5);

		m_check_democleanup = new wxCheckBox(this, wxID_ANY,
			wxT("Demo&Cleanup:")
		);
        mainSizer->Add( m_check_democleanup, 0, wxALL, 5 );
		mainSizer->Add(new wxStaticText(this, wxID_ANY,
			wxT("Allows you to remove scoreboard and commandmenus.")
		), 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );

		mainSizer->AddSpacer(5);

		m_check_demofix = new wxCheckBox(this, wxID_ANY
			,wxT("Demo&Fix:")
		);
        mainSizer->Add( m_check_demofix, 0, wxALL, 5 );
		mainSizer->Add(new wxStaticText(this, wxID_ANY,
			wxT("Don't use it if you don't need it.\n")
			wxT("When the game crashes while the demo is being recorded the demo can't be played because\n")
			wxT("the directory entries are missing or malformed.\n")
			wxT("In this case viewdemo reports \"WARNING! Demo had bogus number of directory entries!\"\n")
			wxT("This fix tries to rebuild the directory entries and thus fixes such demos.")
		), 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );

		mainSizer->AddSpacer(5);

		m_check_forcehltvfix = new wxCheckBox(this, wxID_ANY,
			wxT("dem_force&hltv getting stuck fix:")
		);
        mainSizer->Add( m_check_forcehltvfix, 0, wxALL, 5 );
		mainSizer->Add(new wxStaticText(this, wxID_ANY,
			wxT("Don't use it if you don't need it.\n")
			wxT("In some cases when using dem_forcehltv 1 the camera get's stuck and can't be moved.\n")
			wxT("Also typing spec_mode 3 can't free it in this case. This fix tries to add an extra\n")
			wxT("player slot for the dem_forcehltv spectator and thus usually fixes the problem.")
		), 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );


		SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    // implement wxWizardPage functions
    virtual wxWizardPage *GetPrev() const { return m_prev; }
    virtual wxWizardPage *GetNext() const
    {
		return !(m_check_democleanup->GetValue()) ? m_next->GetNext() : m_next;
	}

	// Setter functions for the wizard:
	void SetPrev(wxWizardPage* prev) { m_prev = prev; };
	void SetNext(wxWizardPage* next) { m_next = next; };

	// Getter functions for the wizard:
	bool Get_DemoFix() { return m_check_demofix->GetValue(); }
	bool Get_FrocehltvFix() { return m_check_forcehltvfix->GetValue(); }
	bool Get_DemoCleanup() { return m_check_democleanup->GetValue(); }

private:
    wxWizardPage *m_prev,
                 *m_next;

	wxCheckBox *m_check_demofix;
	wxCheckBox *m_check_forcehltvfix;
    wxCheckBox *m_check_democleanup;
};

//
// hlaeDemoCleanupPage
//

class hlaeDemoCleanupPage : public wxWizardPage
{
public:
    hlaeDemoCleanupPage( wxWizard *parent ): wxWizardPage(parent)
    {
        m_prev = NULL;
        m_next = NULL;

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(
			new wxStaticText(this, wxID_ANY,
			wxT("DemoCleanup:\n")
			wxT("\n")
			wxT("By default the scoreboard and the commandmenu are removed.\n")
			wxT("If that is fine with you just continue to the next page (Click Next)")
			), 0, wxALL, 5 );

		mainSizer->AddSpacer(5);

		wxBoxSizer *myAsizer = new wxBoxSizer(wxHORIZONTAL);
		m_cmdAsrc = new wxTextCtrl(this, wxID_ANY, wxT("+showscores"));
		m_cmdAdst = new wxTextCtrl(this, wxID_ANY, wxT("+sh0wscores"));
		myAsizer->Add(m_cmdAsrc, 0, 0, 0);
		myAsizer->Add(new wxStaticText(this,wxID_ANY,wxT("->")), 0, wxLEFT|wxRIGHT , 5);
		myAsizer->Add(m_cmdAdst, 0, 0, 0);
		mainSizer->Add(myAsizer, 0, wxALL, 5);

		wxBoxSizer *myBsizer = new wxBoxSizer(wxHORIZONTAL);
		m_cmdBsrc = new wxTextCtrl(this, wxID_ANY, wxT("+commandmenu"));
		m_cmdBdst = new wxTextCtrl(this, wxID_ANY, wxT("+c0mmandmenu"));
		myBsizer->Add(m_cmdBsrc, 0, 0, 0);
		myBsizer->Add(new wxStaticText(this,wxID_ANY,wxT("->")), 0, wxLEFT|wxRIGHT , 5);
		myBsizer->Add(m_cmdBdst, 0, 0, 0);
		mainSizer->Add(myBsizer, 0, wxALL, 5);

		wxBoxSizer *myCsizer = new wxBoxSizer(wxHORIZONTAL);
		m_cmdCsrc = new wxTextCtrl(this, wxID_ANY, wxT(""));
		m_cmdCdst = new wxTextCtrl(this, wxID_ANY, wxT("yet unused"));
		myCsizer->Add(m_cmdCsrc, 0, 0, 0);
		myCsizer->Add(new wxStaticText(this,wxID_ANY,wxT("->")), 0, wxLEFT|wxRIGHT , 5);
		myCsizer->Add(m_cmdCdst, 0, 0, 0);
		mainSizer->Add(myCsizer, 0, wxALL, 5);

        mainSizer->Add(
			new wxStaticText(this, wxID_ANY,
			wxT("If the left of the two fields in a row is empty, the row is ignored.")
			), 0, wxALL, 5 );

		SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    // implement wxWizardPage functions
    virtual wxWizardPage *GetPrev() const { return m_prev; }
	virtual wxWizardPage *GetNext() const { return m_next; }

	// Setter functions for the wizard:
	void SetPrev(wxWizardPage* prev) { m_prev = prev; };
	void SetNext(wxWizardPage* next) { m_next = next; };

	// Getter functions for the wizard:
	wxString GetAsrc() { return m_cmdAsrc->GetValue(); };
	wxString GetAdst() { return m_cmdAdst->GetValue(); };
	wxString GetBsrc() { return m_cmdBsrc->GetValue(); };
	wxString GetBdst() { return m_cmdBdst->GetValue(); };
	wxString GetCsrc() { return m_cmdCsrc->GetValue(); };
	wxString GetCdst() { return m_cmdCdst->GetValue(); };

private:
    wxWizardPage *m_prev,
                 *m_next;

	wxTextCtrl	*m_cmdAsrc,
				*m_cmdAdst;
	wxTextCtrl	*m_cmdBsrc,
				*m_cmdBdst;
	wxTextCtrl	*m_cmdCsrc,
				*m_cmdCdst;
};

//
// hlaeDemoFilePage
//

class hlaeDemoFilePage : public wxWizardPage
{
public:
    hlaeDemoFilePage( wxWizard *parent ): wxWizardPage(parent)
    {
        m_prev = NULL;
        m_next = NULL;

        wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(
			new wxStaticText(this, wxID_ANY,
			wxT("FileSelect:")
			), 0, wxALL, 5 );

		mainSizer->AddSpacer(5);

		wxChar *myfilext=wxT("Half-Life demo (*.dem)|*.dem");

		wxBoxSizer *srcSizer = new wxStaticBoxSizer(wxVERTICAL,this,wxT(" demo to fix "));
		m_srcfile = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select demo to fix ..."), myfilext, wxDefaultPosition, wxDefaultSize,wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST );
		srcSizer->Add(m_srcfile, 0, wxALL|wxEXPAND, 5);
		mainSizer->Add(srcSizer, 0, wxALL|wxEXPAND, 5);

		mainSizer->AddSpacer(5);

		wxBoxSizer *dstSizer = new wxStaticBoxSizer(wxVERTICAL,this,wxT(" file to save to "));
		m_dstfile = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select name to save to ..."), myfilext, wxDefaultPosition, wxDefaultSize,wxFLP_USE_TEXTCTRL|wxFLP_SAVE|wxFLP_OVERWRITE_PROMPT );
		dstSizer->Add(m_dstfile, 0, wxALL|wxEXPAND, 5);
		m_watermark = new wxCheckBox(this, wxID_ANY,
			wxT("add HLAE watermarks")
		);
		m_watermark->SetValue(true);
		dstSizer->Add(m_watermark, 0, wxALL, 5);

		mainSizer->Add(dstSizer, 0, wxALL|wxEXPAND, 5);

		//mainSizer->AddSpacer(5);



		SetSizer(mainSizer);
        mainSizer->Fit(this);
    }

    // implement wxWizardPage functions
    virtual wxWizardPage *GetPrev() const { return m_prev; }
	virtual wxWizardPage *GetNext() const { return m_next; }

	// Setter functions for the wizard:
	void SetPrev(wxWizardPage* prev) { m_prev = prev; };
	void SetNext(wxWizardPage* next) { m_next = next; };

	// Getter functions for the wizard:
	wxString GetSrcPath() { return m_srcfile->GetPath(); }
	wxString GetDstPath() { return m_dstfile->GetPath(); }
	bool Get_WaterMark() { return m_watermark->GetValue(); }

private:
    wxWizardPage *m_prev,
                 *m_next;

	wxFilePickerCtrl* m_srcfile;
	wxFilePickerCtrl* m_dstfile;

	wxCheckBox*	m_watermark;
};

//
// hlaeDemoWizard
//

// Our wizard has the following pages:
//
// Pages marked with* may be overjumped depending on user input on page 01.
//
// 01  : hlaeDemoTaskPage
//       Allows the user to select tasks
// 02* : hlaeDemoCleanupPage
//       Allows the user to select commands it shall block (cleanup).
// 03  : hlaeDemoFilePage
//       Select Input and Output file

class hlaeDemoWizard : public wxWizard
{
public:
	hlaeDemoWizard(wxWindow* parent, bool useSizer = true)
        : wxWizard(parent,wxID_ANY,_T("DemoTools Wizard"),
                   wxNullBitmap,wxDefaultPosition,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	{
		m_page1 = new hlaeDemoTaskPage(this);
		m_page2 = new hlaeDemoCleanupPage(this);
		m_page3 = new hlaeDemoFilePage(this);

		m_page1->SetNext(m_page2);
		m_page2->SetPrev(m_page1);
		m_page2->SetNext(m_page3);
		m_page3->SetPrev(m_page1); // we don't want the user to see the democleanup dialog on prev

		if ( useSizer )
		{
			// allow the wizard to size itself around the pages
			GetPageAreaSizer()->Add(m_page1);
		}
	}

    wxWizardPage *GetFirstPage() const { return m_page1; }

	// Getter functions for CHlaeDemoTools:

	bool Get_DemoFix() { return m_page1->Get_DemoFix(); }
	bool Get_FrocehltvFix() { return m_page1->Get_FrocehltvFix(); }
	bool Get_DemoCleanup() { return m_page1->Get_DemoCleanup(); }

	wxString GetAsrc() { return m_page2->GetAsrc(); };
	wxString GetAdst() { return m_page2->GetAdst(); };
	wxString GetBsrc() { return m_page2->GetBsrc(); };
	wxString GetBdst() { return m_page2->GetBdst(); };
	wxString GetCsrc() { return m_page2->GetCsrc(); };
	wxString GetCdst() { return m_page2->GetCdst(); };

	wxString GetSrcPath() { return m_page3->GetSrcPath(); }
	wxString GetDstPath() { return m_page3->GetDstPath(); }
	bool Get_WaterMark() { return m_page3->Get_WaterMark(); }

private:
 	hlaeDemoTaskPage *m_page1;
	hlaeDemoCleanupPage *m_page2;
	hlaeDemoFilePage *m_page3;
protected:
	hlaeDemoWizard(); // don't let them access wrond constructor
};

//
// CHlaeDemoWizard implementation
//



//
// CHlaeDemoTools
//

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

	hlaeDemoWizard DlgWizard(_parent);

	bRes = ( DlgWizard.RunWizard(DlgWizard.GetFirstPage()) );

	if (bRes)
	{
		_DemoFix->EnableDirectoryFix(DlgWizard.Get_DemoFix());
		_DemoFix->EnableHltvFix(DlgWizard.Get_FrocehltvFix());
		_DemoFix->EnableWaterMarks(DlgWizard.Get_WaterMark());

		if (DlgWizard.Get_DemoCleanup())
		{
			if(!DlgWizard.GetAsrc().IsEmpty()) _DemoFix->AddCommandMapping(DlgWizard.GetAsrc(),DlgWizard.GetAdst());
			if(!DlgWizard.GetBsrc().IsEmpty()) _DemoFix->AddCommandMapping(DlgWizard.GetBsrc(),DlgWizard.GetBdst());
			if(!DlgWizard.GetCsrc().IsEmpty()) _DemoFix->AddCommandMapping(DlgWizard.GetCsrc(),DlgWizard.GetCdst());
			_DemoFix->EnableDemoCleanUp(true);
		} else _DemoFix->EnableDemoCleanUp(false);

		if(_DemoFix->Run(DlgWizard.GetSrcPath().GetData(),DlgWizard.GetDstPath().GetData()))
		{
			// successfully run

			// check if bell was rung (in case fixforcehltv was wished) and if not inform the user:
			if (DlgWizard.Get_FrocehltvFix() && !(_DemoFix->GetHltvFixBell()==0))
			{
				wxChar *mymsg;

				if ( (_DemoFix->GetHltvFixBell()) == 1)
				{
					mymsg = wxT("Found serverinfo, but could not add the additonal slot (maxplayers already reached)");
				} else {
					mymsg = wxT("Serverinfo not found, so no place to add the additonal slot.\nIf it's very important please report with a download link to your demo\nin the HLAE Community Help and Tips Forum.");
				}

				wxMessageDialog mymsgdlg(_parent,mymsg,wxT("demo_forcehltv 1 slot fix failed:"),wxOK|wxICON_ERROR);
				mymsgdlg.ShowModal();
				mymsgdlg.Destroy();
			}
		}
	} else {
		g_debug.SendMessage(wxT("User abborted."),hlaeDEBUG_VERBOSE_LEVEL3);
	}

	DlgWizard.Destroy();
}