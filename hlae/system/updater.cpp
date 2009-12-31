#include <stdafx.h>

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-13-31 by dominik.matrixstorm.com
//
// First changes:
// 2008-12-23 by dominik.matrixstorm.com

#include "updater.h"

using namespace System::IO;
using namespace System::Net;
using namespace System::Xml;

using namespace hlae;

ref class UpdateInfoHelper :
	IUpdaterCheckResult
{
public:
	//
	// Properties:

	property String ^ Description {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual String ^ get() { return m_Description; }

		void set(String ^ value) { m_Description = value; }
	}

	property System::Guid ^ Guid {
		System::Guid ^ get() { return m_Guid; }
		void set(System::Guid ^ value) { m_Guid = value; }
	}

	property bool IsUpdated {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual bool get() { return m_IsUpdated; }

		void set(bool value) { m_IsUpdated = value; }
	}

	property String ^ Title {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual String ^ get() { return m_Title; }

		void set(String ^ value) { m_Title = value; }
	}

	property String ^ Url {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual String ^ get() { return m_Url; }

		void set(String ^ value) { m_Url = value; }
	}

private:
	String ^ m_Description;
	System::Guid ^ m_Guid;
	bool m_IsUpdated;
	String ^ m_Url;
	String ^ m_Title;
};


void GetUpdateInfo(System::String ^ url, UpdateInfoHelper ^ helper, int maxRedirects)
{
	HttpWebRequest ^ request;
	HttpWebResponse ^ response;
	Stream ^ stream;
	XmlDocument ^ doc;

	while(true)
	{
		try {
			request = (HttpWebRequest^)( WebRequest::Create( url ) );
			request->MaximumAutomaticRedirections = 1;
			request->AllowAutoRedirect = true;
			HttpWebResponse^ response = (HttpWebResponse^)( request->GetResponse() );

			doc = gcnew XmlDocument();
			stream = response->GetResponseStream();
			doc->Load( stream );

			XmlNode ^ anode;
			XmlNode ^ nodeUpdate = doc->SelectSingleNode("update");
			XmlAttribute ^ attr = nodeUpdate->Attributes["redirect"];

			if(!attr) {
				helper->Guid =
					gcnew System::Guid(nodeUpdate["guid"]->InnerText)
				;
				helper->Title =
					(anode = nodeUpdate["title"]) ? anode->InnerText : ""
				;
				helper->Url =
					(anode = nodeUpdate["url"]) ? anode->InnerText : ""
				;
				helper->Description =
					(anode = nodeUpdate["description"]) ? anode->InnerText : ""
				;
			}
			else if(0 < maxRedirects) {
				// follow redirect:
				maxRedirects--;
				url = attr->InnerText;
			}
			else
				throw gcnew System::ApplicationException();
		}
		finally {
			delete doc;
			if(stream) stream->Close();
			if(response) response->Close();
		}
	}
}


// Updater /////////////////////////////////////////////////////////////////////

Updater::Updater()
{
	m_CheckState = UpdaterCheckState::Unknown;
	m_CheckResult = nullptr;
	m_OwnGuid = gcnew Guid("2f1b50f2-78c0-405f-804a-97db10482595");

	m_CheckThread = gcnew Thread(gcnew ThreadStart(this, &Updater::CheckWorker));
	m_CheckThread->Name = "hlae Updater CheckThread";
}

Updater::~Updater()
{
	if(m_CheckThread->IsAlive)
		m_CheckThread->Join();
}


IUpdaterCheckResult ^ Updater::CheckResult::get()
{
	return m_CheckResult;
}


UpdaterCheckState Updater::CheckState::get()
{
	return m_CheckState;
}


void Updater::CheckWorker()
{
	try {
		UpdateInfoHelper ^ helper = gcnew UpdateInfoHelper();
		GetUpdateInfo(HLAE_UPDATER_URL, helper, HLAE_UPDATER_MAX_XML_REDIRECTS);
		helper->IsUpdated = m_OwnGuid != helper->Guid;

		m_CheckResult = helper;
	}
	catch(...) {
		m_CheckResult = nullptr;
	}

	m_CheckState = UpdaterCheckState::Checked;
}



void Updater::StartCheck()
{
	if(
		UpdaterCheckState::Checking == m_CheckState
		|| m_CheckThread->IsAlive
	)
		return;

	m_CheckState = UpdaterCheckState::Checking;
	m_CheckResult = nullptr;

	m_CheckThread->Start();
}

