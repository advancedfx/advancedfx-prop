#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com
//
// First changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com


using namespace System;


enum class AfxGoldSrcRenderMode {
	Default,
	FrameBufferObject,
	MemoryDC
};

ref class AfxGoldSrc;

ref class AfxGoldSrcSettings {
public:
	AfxGoldSrcSettings(AfxGoldSrc ^ afxGoldSrc) {
		m_AfxGoldSrc = afxGoldSrc;
		m_Alpha8 = false;
		m_Bpp = 32;
		m_CustomLaunchOptions = "";
		m_FullScreen = false;
		m_HalfLifePath = "";
		m_Height = 640;
		m_Modification = "valve";
		m_OptWindowVisOnRec = true;
		m_RenderMode = AfxGoldSrcRenderMode::Default;
		m_Width = 480;
	}

	//
	// Properties:

	property bool Alpha8 {
		bool get() { return m_Alpha8; }
		void set(bool value) {
			if(!Running()) m_Alpha8 = value;
		}
	}

	property unsigned short int Bpp {
		unsigned short int get() { return m_Bpp; }
		void set(unsigned short int value) {
			if(!Running()) m_Bpp = value < 24 ? (value < 16 ? 8 : 16) : (value < 32 ? 24 : 32);
		}
	}

	property String ^ CustomLaunchOptions {
		String ^ get() { return m_CustomLaunchOptions; }
		void set(String ^ value) {
			if(!Running()) m_CustomLaunchOptions = gcnew String(value);
		}
	}

	property bool FullScreen {
		bool get() { return m_FullScreen; }
		void set(bool value) {
			if(!Running()) m_FullScreen = value;
		}
	}

	property String ^ HalfLifePath {
		String ^ get() { return m_HalfLifePath; }
		void set(String ^ value) {
			if(!Running()) m_HalfLifePath = gcnew String(value);
		}
	}

	property unsigned int Height {
		unsigned int get() { return m_Height; }
		void set(unsigned int value) {
			if(!Running()) m_Height = value;
		}
	}

	property String ^ Modification {
		String ^ get() { return m_Modification; }
		void set(String ^ value) {
			if(!Running()) m_Modification = gcnew String(value);
		}
	}

	property bool OptWindowVisOnRec {
		bool get() { return m_OptWindowVisOnRec; }
		void set(bool value) {
			if(!Running()) m_OptWindowVisOnRec = value;
		}
	}

	property AfxGoldSrcRenderMode RenderMode {
		AfxGoldSrcRenderMode get() { return m_RenderMode; }
		void set(AfxGoldSrcRenderMode value) {
			if(!Running()) m_RenderMode = value;
		}
	}

	property unsigned int Width {
		unsigned int get() { return m_Width; }
		void set(unsigned int value) {
			if(!Running()) m_Width = value;
		}
	}

private:
	AfxGoldSrc ^ m_AfxGoldSrc;
	bool m_Alpha8;
	unsigned short int m_Bpp;
	String ^ m_CustomLaunchOptions;
	bool m_FullScreen;
	String ^ m_HalfLifePath;
	unsigned int m_Height;
	String ^ m_Modification;
	bool m_OptWindowVisOnRec;
	AfxGoldSrcRenderMode m_RenderMode;
	unsigned int m_Width;

	bool Running();
};


// AfxGoldSrc //////////////////////////////////////////////////////////////////

ref class AfxGoldSrc
{
public:
	AfxGoldSrc();
	~AfxGoldSrc();

	bool Launch();

	//
	// Properties:

	property bool Running { bool get() { return m_Running; } }

	property AfxGoldSrcSettings ^ Settings { AfxGoldSrcSettings ^ get() { return m_Settings; } }

private:
	bool m_Running;
	AfxGoldSrcSettings ^ m_Settings;

};