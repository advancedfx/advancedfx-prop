#include <wx/statusbr.h>
#include <wx/button.h>
#include <wx/scrolwin.h>
#include <wx/memory.h>

#include "demo_tools/demo_tools.h"
#include "gamewindow.h"
#include "settings.h"

#include "launcher.h"

#include "console.h"
#include "layout.h"

#include "debug.h"
#include "config.h"

#include "main.h" // always include own header last.

BEGIN_EVENT_TABLE(hlaeMainWindow, wxFrame)
	EVT_MENU(wxID_EXIT, hlaeMainWindow::OnExit)
	EVT_MENU(wxID_ABOUT, hlaeMainWindow::OnAbout)
	EVT_MENU(hlaeMainWindow::hlaeID_Launch, hlaeMainWindow::OnLaunch)
	EVT_MENU(hlaeMainWindow::hlaeID_DemoTools, hlaeMainWindow::OnDemoTools)
	EVT_MENU(hlaeMainWindow::hlaeID_SaveLayout, hlaeMainWindow::OnSaveLayout)
	EVT_MENU(hlaeMainWindow::hlaeID_LayoutManager, hlaeMainWindow::OnLayoutManager)
	EVT_SIZE(hlaeMainWindow::OnSize)
END_EVENT_TABLE()

hlaeMainWindow::hlaeMainWindow()
		: wxFrame(NULL, wxID_ANY,
			wxT("Half-Life After Effects - Developer Edition"),
			wxDefaultPosition, wxSize(800,600))
{
	g_layoutmanager.SetMainWindow(this);

	m_toolbarmenu = new wxMenu;
	m_windowmenu = new wxMenu;
	m_layoutmenu = new wxMenu;

	CreateMenuBar();
	CreateStatusBar();
    GetStatusBar()->SetStatusText(wxT("Ready"));

	g_layoutmanager.AddPane(new hlaeConsoleWindow(this),
		wxAuiPaneInfo().Bottom().MinSize(wxSize(-1,100)));

	g_layoutmanager.AddLayout(wxT("Default"),true);

	// create the gamewindow that also manages the communication with the game:
	m_HlaeGameWindow = new CHlaeGameWindow(this,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,wxT("Game Window"));
	g_layoutmanager.AddPane(m_HlaeGameWindow, wxAuiPaneInfo().CentrePane().Caption(wxT("Game Window")));


	// g_debug.SendMessage(wxT("This is a fatal error"), hlaeDEBUG_FATALERROR);
	g_debug.SendMessage(wxT("This is an error"), hlaeDEBUG_ERROR);
	g_debug.SendMessage(wxT("This is a warning"), hlaeDEBUG_WARNING);
	g_debug.SendMessage(wxT("This is verbose output level 1"), hlaeDEBUG_VERBOSE_LEVEL1);
	g_debug.SendMessage(wxT("This is verbose output level 2"), hlaeDEBUG_VERBOSE_LEVEL2);
	g_debug.SendMessage(wxT("This is verbose output level 3"), hlaeDEBUG_VERBOSE_LEVEL3);
	g_debug.SendMessage(wxT("This is a debug message"), hlaeDEBUG_DEBUG);
}

hlaeMainWindow::~hlaeMainWindow()
{
	g_layoutmanager.UnInit();
}

void hlaeMainWindow::OnAbout(wxCommandEvent& WXUNUSED(event)) {

	hlaeDialogSettings* bla = new hlaeDialogSettings(this);
	bla->Show();

	/*
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
	*/

}

void hlaeMainWindow::OnExit(wxCommandEvent& WXUNUSED(evt)) {
	Close(true);
}

void hlaeMainWindow::OnDemoTools(wxCommandEvent& WXUNUSED(evt))
{
	CHlaeDemoTools demo_tools(this);
	demo_tools.Run();
}

void hlaeMainWindow::OnSaveLayout(wxCommandEvent& WXUNUSED(evt)) {
	g_layoutmanager.AddLayout();
}

void hlaeMainWindow::OnLaunch(wxCommandEvent& WXUNUSED(evt)) {
	CLauncherDialog* launch_dialog = new CLauncherDialog(this);
	launch_dialog->Show();
}

void hlaeMainWindow::OnLayoutManager(wxCommandEvent& WXUNUSED(evt))
{

}

void hlaeMainWindow::OnSize(wxSizeEvent& evt)
{
	Layout();
}


wxMenu* hlaeMainWindow::GetWindowMenu() const {
	return m_windowmenu;
}

wxMenu* hlaeMainWindow::GetToolBarMenuMenu() const {
	return m_toolbarmenu;
}

wxMenu* hlaeMainWindow::GetLayoutMenu() const {
	return m_layoutmenu;
}

void hlaeMainWindow::CreateMenuBar() {

	// initialize the menubar
	wxMenuBar* menubar = new wxMenuBar;

	// file menu
	wxMenu* file_menu = new wxMenu;
	
	file_menu->Append(hlaeID_Launch, wxT("Launch"));
    file_menu->Append(wxID_EXIT, wxT("E&xit"), wxT("Quit the program"));

	// view menu
    wxMenu* view_menu = new wxMenu;
	view_menu->AppendSubMenu(m_layoutmenu, wxT("&Layouts"));
    view_menu->Append(hlaeID_SaveLayout, wxT("&Save Layout"), wxT("Save the actual layout"));
	view_menu->Append(hlaeID_LayoutManager, wxT("Layout &Manager"), wxT("Manage the layouts"));
	view_menu->AppendSeparator();
	view_menu->AppendSubMenu(m_windowmenu, wxT("&Windows"));
	view_menu->AppendSubMenu(m_toolbarmenu, wxT("&Toolbars"));

	// tools menu
	wxMenu* tools_menu = new wxMenu;
	tools_menu->Append(hlaeID_DemoTools, wxT("&Demo Tool"), wxT("Half-Life Demo Tool"));

	// help menu
	wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxT("&About..."), wxT("About Half-Life After Effects"));

	// append menus to menubar
	menubar->Append(file_menu, wxT("&File"));
    menubar->Append(view_menu, wxT("&View"));
	menubar->Append(tools_menu, wxT("&Tools"));
	menubar->Append(help_menu, wxT("&?"));

	// associate the menubar to the frame
	SetMenuBar(menubar);
}

IMPLEMENT_APP(hlaeApp)

bool hlaeApp::OnInit()
{
	// initialize the config system
	g_config.Initialize();

	hlaeMainWindow* mainframe = new hlaeMainWindow();
	SetTopWindow(mainframe);
	mainframe->Show();

	return true;
}