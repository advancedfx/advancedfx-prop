#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com
//
// First changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com

#include <windows.h>

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

	// Create Pipes:

	SECURITY_ATTRIBUTES secAttrib;
	secAttrib.nLength = sizeof(secAttrib);
	secAttrib.lpSecurityDescriptor = 0;
	secAttrib.bInheritHandle = true;

	HANDLE hRead, hWrite;

	if(!CreatePipe(&hRead, &hWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;

	m_SendPipeReadHandle = System::IntPtr(hRead);
	m_SendPipeWriteHandle = System::IntPtr(hWrite);

	if(!CreatePipe(&hRead, &hWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;

	m_RecvPipeReadHandle = System::IntPtr(hRead);
	m_RecvPipeWriteHandle = System::IntPtr(hWrite);
}


AfxGoldSrc::~AfxGoldSrc()
{
	CloseHandle((HANDLE)m_RecvPipeReadHandle.ToPointer());
	CloseHandle((HANDLE)m_RecvPipeWriteHandle.ToPointer());
	CloseHandle((HANDLE)m_SendPipeReadHandle.ToPointer());
	CloseHandle((HANDLE)m_SendPipeWriteHandle.ToPointer());

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


bool AfxGoldSrc::Launch() {
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
		m_SendPipeReadHandle.ToString(),
		",",
		m_RecvPipeWriteHandle.ToString()
	);

	// advanced

	if( m_Settings->Alpha8 )
		cmds = String::Concat( cmds, " -mdtalpha8" );

	switch(m_Settings->RenderMode) {
	case AfxGoldSrcRenderMode::FrameBufferObject:
		cmds = String::Concat( cmds, " -mdtrender fbo" );
		break;
	case AfxGoldSrcRenderMode::MemoryDC:
		cmds = String::Concat( cmds, " -mdtrender memdc" );
		break;
	}

	if(m_Settings->OptWindowVisOnRec)
		cmds = String::Concat( cmds, " -mdtoptvis" );

	// custom command line

	s1 = m_Settings->CustomLaunchOptions;
	if( 0 < s1->Length)
		cmds = String::Concat( cmds," ", s1 );

	//
	// Launch:

	m_Running = CustomLoader(
		String::Concat(System::Windows::Forms::Application::StartupPath, "\\AfxHookGoldSrc.dll"),
		m_Settings->HalfLifePath,
		cmds
	);

	return m_Running;
}


// AfxGoldSrcSettings //////////////////////////////////////////////////////////

bool AfxGoldSrcSettings::Running() {
	return m_AfxGoldSrc->Running;
}