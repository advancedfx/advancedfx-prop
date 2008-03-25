#ifndef HLAE_LAUNCHER_H
#define HLAE_LAUNCHER_H

#include <wx/wx.h>
#include <wx/statline.h>

#include <defines.h>


struct CChoiceElement : public wxObject
{
	wxString describtion;
	wxString value;
};

class CChoiceList : public wxObject
{

private:
	wxList* m_list;
	wxChoice* m_choice;
	wxTextCtrl* m_textctrl;

public:
	CChoiceList();
	~CChoiceList();

	wxArrayString GetChoices();
	wxString GetCurrentChoice(const wxString& format);	
	
	void AddChoice(const wxString& describtion, const wxString& value);
	void SetControls(wxChoice* choice, wxTextCtrl* textctrl);
	void Update();

};

class CLauncherDialog : public wxDialog 
{

private:

	CChoiceList* m_depthchoices;
	CChoiceList* m_modchoices;

	wxString m_cmdline;

	// Preset variables
	wxString m_additionalcmdline;
	wxString m_path;
	wxString m_depth;
	wxString m_width; 
	wxString m_height;
	bool m_force;

	enum {
		ID_Changed = wxID_HIGHEST+1,
		ID_Launch,
		ID_Browse,
		ID_ChoiceDepth,
		ID_ChoiceMod,
		ID_SavePreset,
		ID_DeletePreset
	};

	wxComboBox* m_cb_preset;
	wxCheckBox* m_ch_force;
	wxTextCtrl* m_tc_path;
	wxChoice* m_c_mod;
	wxTextCtrl* m_tc_mod;
	wxTextCtrl* m_tc_width;
	wxTextCtrl* m_tc_height;
	wxChoice* m_c_depth;
	wxTextCtrl* m_tc_depth;
	wxTextCtrl* m_tc_additionalcmdline;
	wxTextCtrl* m_tc_fullcmdline;

	void OnLaunch(wxCommandEvent& evt);
	void OnSavePreset(wxCommandEvent& evt);
	void OnDeletePreset(wxCommandEvent& evt);
	void OnChanged(wxCommandEvent& evt);
	void OnBrowse(wxCommandEvent& evt);

	void UpdateCmdline();

public:
	CLauncherDialog(wxWindow* parent);
	~CLauncherDialog();

};

#endif
