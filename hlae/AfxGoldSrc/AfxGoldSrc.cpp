#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com
//
// First changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com

#include <windows.h>

#include <shared/com/PipeCom.h>

#include "AfxGoldSrc.h"

#include <system/loader.h>

using namespace System::Runtime::InteropServices;

// AfxGoldSrc //////////////////////////////////////////////////////////////////

AfxGoldSrc::AfxGoldSrc()
{
	if(m_SingeltonInstance)
		throw gcnew System::InvalidOperationException
	;
	m_SingeltonInstance = this;

	m_Running = false;
	m_Settings = gcnew AfxGoldSrcSettings(this);
}


AfxGoldSrc::~AfxGoldSrc()
{
	Stop();

	m_SingeltonInstance = nullptr;
}


AfxGoldSrc ^ AfxGoldSrc::Get() {
	return m_SingeltonInstance;
}


AfxGoldSrc ^ AfxGoldSrc::GetOrCreate() {
	if(!m_SingeltonInstance)
		gcnew AfxGoldSrc(); 

	return m_SingeltonInstance;
}


bool AfxGoldSrc::Launch(System::Windows::Forms::Panel ^ gamePanel)
{
	Stop();

	m_ComServer = gcnew AfxGoldSrcComServer();

	String ^cmds, ^s1;
	array<System::Diagnostics::Process^> ^ procs;
	
	procs = System::Diagnostics::Process::GetProcessesByName( "hl" );

	if(0 < procs->Length)
		return false;

	//
	//	build parameters:

	cmds = "-steam -gl";
	
	cmds = String::Concat( cmds, " -game ", m_Settings->Modification );

	// gfx settings

	if( m_Settings->FullScreen )
		cmds = String::Concat(cmds," -full");
	else
		cmds = String::Concat(cmds," -window");


	s1 = m_Settings->Bpp.ToString();
	if( 0 < s1->Length) cmds = String::Concat( cmds," -", s1, "bpp" );

	s1 = m_Settings->Width.ToString();
	if( 0 < s1->Length) cmds = String::Concat( cmds," -w ", s1 );

	s1 = m_Settings->Height.ToString();
	if( 0 < s1->Length) cmds = String::Concat( cmds," -h ", s1 );

	if(m_Settings->ForceRes) cmds = String::Concat(cmds, " -forceres");

	// pipe handles:
	cmds = String::Concat(cmds,
		" -afxpipes ",
		m_ComServer->ClientRecvPipeHandle.ToString(),
		",",
		m_ComServer->ClientSendPipeHandle.ToString()
	);

	// advanced

	// custom command line

	s1 = m_Settings->CustomLaunchOptions;
	if( 0 < s1->Length)
		cmds = String::Concat( cmds," ", s1 );

	//
	// Launch:

//	System::Windows::Forms::MessageBox::Show(cmds);

	m_Running = CustomLoader(
		String::Concat(System::Windows::Forms::Application::StartupPath, "\\AfxHookGoldSrc.dll"),
		m_Settings->HalfLifePath,
		cmds
	);

	if(m_Running)
	{
		m_ComServer->Start(
			m_Settings->FullScreen,
			m_Settings->Alpha8 ,
			gamePanel,
			m_Settings->RenderMode,
			m_Settings->OptWindowVisOnRec
		);
	}

	return m_Running;
}


void AfxGoldSrc::Stop()
{
	if(nullptr != m_ComServer)
	{
		delete m_ComServer;
		m_ComServer = nullptr;
	}
}

// AfxGoldSrcSettings //////////////////////////////////////////////////////////

//bool AfxGoldSrcSettings::Running() {
//	return m_AfxGoldSrc->Running;
//}