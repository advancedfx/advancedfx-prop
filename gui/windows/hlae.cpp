#include <wx/wx.h>
#include <wx/hyperlink.h>

#include <system/layout.h>
#include <system/debug.h>
#include <system/config.h>
#include <windows/demotools.h>
#include <windows/game.h>
#include <windows/settings.h>
#include <windows/launcher.h>
#include <windows/console.h>

#include "hlae.h"

CAboutDialog::CAboutDialog(wxWindow* parent)
: wxDialog(parent, wxID_ANY, _T("About ") _T( HLAE_NAME ), wxDefaultPosition, wxSize(450,350))
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bs_main;
	bs_main = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bs_link;
	bs_link = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_link;
	st_link = new wxStaticText( this, wxID_ANY, _T("You can retrive the latest HLAE, infos, reports bugs and post suggestions at"), wxDefaultPosition, wxDefaultSize, 0 );
	st_link->Wrap( -1 );
	bs_link->Add( st_link, 0, 0, 5 );
	
	wxHyperlinkCtrl* hl_link;
	hl_link = new wxHyperlinkCtrl( this, wxID_ANY, _T("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), _T("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	
	hl_link->SetHoverColour( wxColour( 0, 88, 255 ) );
	hl_link->SetNormalColour( wxColour( 0, 0, 255 ) );
	hl_link->SetVisitedColour( wxColour( 0, 0, 255 ) );
	bs_link->Add( hl_link, 0, 0, 5 );
	
	bs_main->Add( bs_link, 0, wxALL|wxEXPAND, 5 );
	
	wxScrolledWindow* sw_info;
	sw_info = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN|wxVSCROLL );
	sw_info->SetScrollRate( 5, 5 );
	wxBoxSizer* bs_info;
	bs_info = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_info;
	st_info = new wxStaticText( sw_info, wxID_ANY, _T("Epilepsy warning:\n\n  This software will cause fast changing colors and images on your screen.\n\n\nUsing the HLAE:\n\n  Extract all contents of this archive into a folder you like.\n  You should not extract into the game's folder, this is not neccessary.\n\n  We suggest using Steam's Offline Mode.\n  Please stick to the manuals and tutorials on the web for more information.\n\n\nDisclaimer:\n\n  Altered versions must be cleary marked as such and must not be misrepresented\n  as being the original software.\n  \n  This software is for your enjoyment and you are using it at your own risk.\n\n\nSincerly,\nyour Half-Life Advanced Effects team.\n\n\n\n\nCredits:\n\nIn addition to the mentions in the changelog we want to credit the following\npeople, companys or organistations:\n\nMicrosoft\n  for VC++, MSDN2, Windows, ...\n  http://www.Microsoft.com\n  \nValve\n  for HLSDK\n  http://www.ValveSoftware.com\n  \nMetaMod\n  for HLSDK p3\n  http://www.MetaMod.org\n\nid Software\n  for Quake 1 source code\n  http://www.idsoftware.com\n  \nwxWidgets Project\n  for wxWidgets\n  http://wxwidgets.org\n  \nAnd everyone that feels he/she/it should have been mentioned here.\n\n\nGreetings:\n\nYou :)\n\n\nThe HLAE Team:\n\nMIRVIN_monkey\n  the creator and programmer\n  http://www.madabouthats.org\n  \nripieces\n  programer\n  http://dominik.matrixstorm.com\n  \nneomic\n  programer\n  http://neomic.xail.net/\n\nmsthavoc\n  documentation, testing, community relations\n  http://www.mst-gaming.de"), wxDefaultPosition, wxDefaultSize, 0 );
	st_info->Wrap( -1 );
	bs_info->Add( st_info, 0, 0, 5 );
	
	sw_info->SetSizer( bs_info );
	sw_info->Layout();
	bs_info->Fit( sw_info );
	bs_main->Add( sw_info, 1, wxALL|wxEXPAND, 5 );
	
	wxButton* bt_ok;
	bt_ok = new wxButton( this, wxID_OK, _T("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bs_main->Add( bt_ok, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bs_main );
	this->Layout();
}

BEGIN_EVENT_TABLE(hlaeMainWindow, wxFrame)
	EVT_ACTIVATE(OnActivate) 
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
		_T( HLAE_NAME ) _T(" ") _T( HLAE_VERSIONSTR) _T( " ( ") _T(__DATE__)  _T(" ") _T(__TIME__) _T (" )"),
			wxDefaultPosition, wxSize(800,600))
{
	g_layoutmanager.SetMainWindow(this);

	this->SetIcon( wxIcon(wxICON(hlaeICON_AAA)) );

	m_toolbarmenu = new wxMenu;
	m_windowmenu = new wxMenu;
	m_layoutmenu = new wxMenu;

	CreateMenuBar();
	CreateStatusBar();
    GetStatusBar()->SetStatusText(_T("Ready"));

	g_layoutmanager.AddPane(new hlaeConsoleWindow(this),
		wxAuiPaneInfo().Bottom().MinSize(wxSize(-1,100)));

	g_layoutmanager.AddLayout(_T("Default"),true);

	// create the gamewindow that also manages the communication with the game:
	m_HlaeGameWindow = new CHlaeGameWindow(this,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,_T("Game Window"));
	g_layoutmanager.AddPane(m_HlaeGameWindow, wxAuiPaneInfo().CenterPane().Caption(_T("Game Window")));


	// g_debug.SendMessage(_T("This is a fatal error"), hlaeDEBUG_FATALERROR);
	g_debug.SendMessage(_T("This is an error"), hlaeDEBUG_ERROR);
	g_debug.SendMessage(_T("This is a warning"), hlaeDEBUG_WARNING);
	g_debug.SendMessage(_T("This is verbose output level 1"), hlaeDEBUG_VERBOSE_LEVEL1);
	g_debug.SendMessage(_T("This is verbose output level 2"), hlaeDEBUG_VERBOSE_LEVEL2);
	g_debug.SendMessage(_T("This is verbose output level 3"), hlaeDEBUG_VERBOSE_LEVEL3);
	g_debug.SendMessage(_T("This is a debug message"), hlaeDEBUG_DEBUG);
}

hlaeMainWindow::~hlaeMainWindow()
{
	g_layoutmanager.UnInit();
}

void hlaeMainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	CAboutDialog* about_dgl = new CAboutDialog(this);
	about_dgl->Show();
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

void hlaeMainWindow::OnActivate( wxActivateEvent &myevent)
{
	if (myevent.GetActive())
	{
		SetStatusText(wxT("Active"));
	}
	else
	{
		SetStatusText(wxT("Inactive"));
	}

	m_HlaeGameWindow->OnHlaeActivate(myevent.GetActive());

	myevent.Skip();
}

void hlaeMainWindow::CreateMenuBar() {

	// initialize the menubar
	wxMenuBar* menubar = new wxMenuBar;

	// file menu
	wxMenu* file_menu = new wxMenu;
	
	file_menu->Append(hlaeID_Launch, _T("Launch"));
    file_menu->Append(wxID_EXIT, _T("E&xit"), _T("Quit the program"));

	// view menu
    wxMenu* view_menu = new wxMenu;
	view_menu->AppendSubMenu(m_layoutmenu, _T("&Layouts"));
    view_menu->Append(hlaeID_SaveLayout, _T("&Save Layout"), _T("Save the actual layout"));
	view_menu->Append(hlaeID_LayoutManager, _T("Layout &Manager"), _T("Manage the layouts"));
	view_menu->AppendSeparator();
	view_menu->AppendSubMenu(m_windowmenu, _T("&Windows"));
	view_menu->AppendSubMenu(m_toolbarmenu, _T("&Toolbars"));

	// tools menu
	wxMenu* tools_menu = new wxMenu;
	tools_menu->Append(hlaeID_DemoTools, _T("&Demo Tool"), _T("Half-Life Demo Tool"));

	// help menu
	wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("&About..."), _T("About ") _T(HLAE_NAME) );

	// append menus to menubar
	menubar->Append(file_menu, _T("&File"));
    menubar->Append(view_menu, _T("&View"));
	menubar->Append(tools_menu, _T("&Tools"));
	menubar->Append(help_menu, _T("&?"));

	// associate the menubar to the frame
	SetMenuBar(menubar);
}