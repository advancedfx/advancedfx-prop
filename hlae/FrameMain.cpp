#include <wx/panel.h>
#include <wx/button.h>
#include <wx/statusbr.h>
#include <wx/textdlg.h>

//#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/utils.h> // wxGetProcessId

#include "hlae/framemain.h"

#include "basecomServer.h"

BEGIN_EVENT_TABLE(hlaeFrameMain, wxFrame)

	// standard indentifiers
	EVT_MENU(wxID_EXIT, hlaeFrameMain::OnExit)
	EVT_MENU(wxID_ABOUT, hlaeFrameMain::OnAbout)

	// hlae indentifiers
	EVT_MENU(hlaeFrameMain::hlaeID_SaveLayout, hlaeFrameMain::OnSaveLayout)
	EVT_MENU(hlaeFrameMain::hlaeID_LayoutManager, hlaeFrameMain::OnLayoutManager)

END_EVENT_TABLE()


hlaeFrameMain::hlaeFrameMain()
		: wxFrame(NULL, wxID_ANY,
			wxT("Half-Life After Effects - Developer Edition"),
			wxDefaultPosition, wxSize(800,600))
{
	// initialization
	m_auimanager = new hlaeAuiManager(this);

	m_toolbarmenu = new wxMenu;
	m_windowmenu = new wxMenu;
	m_layoutmenu = new wxMenu;

	// set the menu- and statusbar
	CreateMenuBar();
	CreateStatusBar();
    GetStatusBar()->SetStatusText(wxT("Ready"));

	// some test panes
	m_auimanager->AddPane(new wxButton(this, wxID_ANY, wxT("test"),
		wxDefaultPosition, wxSize(20,20), wxNO_BORDER),
		wxAuiPaneInfo().Left().MinSize(wxSize(100,100)));
	
	//wxWindow* window = new wxWindow(this, wxID_ANY);
	//new wxButton(window, wxID_ANY, wxT("1"), wxPoint(10,10), wxSize(20,20));
	//new wxButton(window, wxID_ANY, wxT("2"), wxPoint(10,30), wxSize(20,20));
	//m_auimanager->AddPane(window, wxAuiPaneInfo().Center());

	wxScrolledWindow *myGameWindow=new wxScrolledWindow(this);

	m_auimanager->AddPane(myGameWindow, wxAuiPaneInfo().Right());

	m_auimanager->AddLayout(wxT("Default"),true);

	HlaeBcSrvStart(wxGetProcessId(),myGameWindow->GetHWND());
}

hlaeFrameMain::~hlaeFrameMain() {
	HlaeBcSrvStop();
	delete m_auimanager;
}

void hlaeFrameMain::OnAbout(wxCommandEvent& WXUNUSED(event)) {

    wxDialog* h_about = new wxDialog(this ,wxID_ANY,
		wxT("About Half-Life After Effects"), wxDefaultPosition);
	
	const wchar_t* text =
		wxT("Half-Life After Effects:\n")
		wxT("\n")
		wxT("...");
		
	new wxStaticText(h_about, wxID_ANY, text, wxPoint(10,10));
	new wxButton(h_about, wxID_OK, wxT("OK"), wxPoint(100,100));

	h_about->ShowModal();

	delete h_about;

}

void hlaeFrameMain::OnExit(wxCommandEvent& WXUNUSED(evt)) {
	Close(true);
}

void hlaeFrameMain::OnSaveLayout(wxCommandEvent& WXUNUSED(evt)) {
	m_auimanager->AddLayout();
}

void hlaeFrameMain::OnLayoutManager(wxCommandEvent& WXUNUSED(evt)) {
	m_auimanager->ShowManager();
}

wxMenu* hlaeFrameMain::GetWindowMenu() const {
	return m_windowmenu;
}

wxMenu* hlaeFrameMain::GetToolBarMenuMenu() const {
	return m_toolbarmenu;
}

wxMenu* hlaeFrameMain::GetLayoutMenu() const {
	return m_layoutmenu;
}

void hlaeFrameMain::CreateMenuBar() {

	// initialize the menubar
	wxMenuBar* menubar = new wxMenuBar;

	// file menu
	wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_EXIT, wxT("E&xit"), wxT("Quit the program"));

	// view menu
    wxMenu* view_menu = new wxMenu;
	view_menu->AppendSubMenu(m_layoutmenu, wxT("&Layouts"));
    view_menu->Append(hlaeID_SaveLayout, wxT("&Save Layout"), wxT("Save the actual layout"));
	view_menu->Append(hlaeID_LayoutManager, wxT("Layout &Manager"), wxT("Manage the layouts"));
	view_menu->AppendSeparator();
	view_menu->AppendSubMenu(m_windowmenu, wxT("&Windows"));
	view_menu->AppendSubMenu(m_toolbarmenu, wxT("&Toolbars"));

	// help menu
	wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxT("&About..."), wxT("About Half-Life After Effects"));

	// append menus to menubar
	menubar->Append(file_menu, wxT("&File"));
    menubar->Append(view_menu, wxT("&View"));
	menubar->Append(help_menu, wxT("&?"));

	// associate the menubar to the frame
	SetMenuBar(menubar);
}