#ifndef HLAE_SETTINGS_H
#define HLAE_SETTINGS_H

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/treectrl.h>

#include <vector>

using namespace std; 


enum hlaeSettingsPageID_e
{
	ID_LastPage,
	ID_General,
	ID_Advanced
};


class hlaeSettingsPageBase : public wxWindow
{

public:

	hlaeSettingsPageBase(wxWindow* parent);
	virtual void ApplyChanges() = 0;
};


class hlaeSettingsPageGeneral : public hlaeSettingsPageBase
{

public:

	hlaeSettingsPageGeneral(wxWindow* parent);
	virtual void ApplyChanges();
};


class hlaeSettingsPageAdvanced : public hlaeSettingsPageBase
{

public:

	hlaeSettingsPageAdvanced(wxWindow* parent);
	virtual void ApplyChanges();
};


class hlaeSettingsBaseElement
{

public:

	hlaeSettingsBaseElement(hlaeSettingsPageBase* page, hlaeSettingsPageID_e page_id); 

	hlaeSettingsPageID_e GetPageID();
	hlaeSettingsPageBase* GetPage();

protected:

	hlaeSettingsPageID_e m_pageid;
	hlaeSettingsPageBase* m_page;
};


class hlaeSettingsListElement : public hlaeSettingsBaseElement
{

public:

	hlaeSettingsListElement(hlaeSettingsPageBase* page, hlaeSettingsPageID_e page_id,
		wxTreeItemId tree_id);

	wxTreeItemId GetTreeID();

private:

	wxTreeItemId m_treeid;

};


class hlaeSettingsTreeElement : public hlaeSettingsBaseElement
{

public:

	hlaeSettingsTreeElement(hlaeSettingsPageBase* page, const wxString& name,
		hlaeSettingsPageID_e page_id, bool is_advanced);

	size_t GetSubpagesCount();

	void AppendSubpage(const hlaeSettingsTreeElement& subpage_element);
	const hlaeSettingsTreeElement& GetSubpage(size_t index);

	bool IsAdvanced();
	wxString GetName();

private:

	bool m_isadvanced;
	wxString m_name;
	vector<hlaeSettingsTreeElement> m_subpagetree;
};


class hlaeSettingsDialog : public wxDialog
{

public:

	hlaeSettingsDialog(wxWindow* parent, hlaeSettingsPageID_e page_id);

private:

	enum
	{
		ID_AdvancedMode = wxID_HIGHEST+1,
		ID_OK,
		ID_Apply,
		ID_SelectionChanged
	};

	bool m_advancedmode;
	hlaeSettingsPageBase* m_lastpage;
	
	wxBoxSizer* m_bs_treebook;
	wxTreeCtrl* m_treectrl;
	wxCheckBox* m_ch_advancedview;

	// this represents the structure in the menu
	vector<hlaeSettingsTreeElement> m_pagetree;
	// for better access to the ids
	vector<hlaeSettingsListElement> m_pagelist;

	void ApplyChanges();

	void UpdateTreeCtrl();
	void UpdateTreeCtrlNodes(hlaeSettingsTreeElement element, wxTreeItemId id);

	void OnSelectionChanged(wxTreeEvent& evt);
	void OnAdvancedMode(wxCommandEvent& evt);
	void OnApply(wxCommandEvent& evt);
	void OnOK(wxCommandEvent& evt);
};

#endif