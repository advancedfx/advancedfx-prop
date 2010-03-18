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

	property System::Guid ^ Guid {
		System::Guid ^ get() { return m_Guid; }
		void set(System::Guid ^ value) { m_Guid = value; }
	}

	property bool IsUpdated {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual bool get() { return m_IsUpdated; }

		void set(bool value) { m_IsUpdated = value; }
	}

	property System::Uri ^ Uri {

		/// <summary> Implements IUpdaterCheckResult. </summary>
		virtual System::Uri ^ get() { return m_Uri; }

		void set(System::Uri ^ value) { m_Uri = value; }
	}

private:
	System::Guid ^ m_Guid;
	bool m_IsUpdated;
	System::Uri ^ m_Uri;
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
			request->Timeout = 10000;
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
				helper->Uri =
					(anode = nodeUpdate["link"]) ? gcnew Uri(anode->InnerText) : nullptr
				;
				break;
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
	m_CheckResult = nullptr;
	m_CheckThreadQuit = false;
	m_CheckThreadWork = gcnew AutoResetEvent(false);
	m_CheckedNotificationTargets = gcnew System::Collections::Generic::LinkedList<UpdaterNotificationTarget ^>();
	m_OwnGuid = gcnew Guid(HLAE_UPDATER_CURRENT_GUID);

	m_OldGuid =
#ifndef HLAE_UPDATER_OLD_GUID
		null
#else
		gcnew Guid(HLAE_UPDATER_OLD_GUID)
#endif
	;

	m_CheckThread = gcnew Thread(gcnew ThreadStart(this, &Updater::CheckWorker));
	m_CheckThread->Name = "hlae Updater CheckThread";
}

Updater::~Updater()
{
	while(m_CheckThread->IsAlive) {
		m_CheckThreadQuit = true;
		m_CheckThreadWork->Set();
		m_CheckThread->Join(250);
	}
}


void Updater::BeginCheckedNotification(UpdaterNotificationTarget ^ target) {
	try {
		Monitor::Enter(m_CheckedNotificationTargets);

		m_CheckedNotificationTargets->AddLast(target);
	}
	finally {
		Monitor::Exit(m_CheckedNotificationTargets);
	}
}


void Updater::CheckWorker()
{
	while(!m_CheckThreadQuit)
	{
		try {
			UpdateInfoHelper ^ helper = gcnew UpdateInfoHelper();
			GetUpdateInfo(HLAE_UPDATER_URL, helper, HLAE_UPDATER_MAX_XML_REDIRECTS);
			helper->IsUpdated = m_OwnGuid->CompareTo(helper->Guid) && (!m_OldGuid || m_OldGuid->CompareTo(helper->Guid));

			m_CheckResult = helper;
		}
		catch(...) {
			m_CheckResult = nullptr;
		}

		try {
			Monitor::Enter(m_CheckedNotificationTargets);

			for(
				System::Collections::Generic::LinkedListNode<UpdaterNotificationTarget ^> ^ cur = m_CheckedNotificationTargets->First;
				nullptr != cur;
				cur = cur->Next
			)
				cur->Value->Notify(this, m_CheckResult);
		}
		finally {
			Monitor::Exit(m_CheckedNotificationTargets);
		}

		m_CheckThreadWork->WaitOne();
	}
}


void Updater::EndCheckedNotification(UpdaterNotificationTarget ^ target) {
	try {
		Monitor::Enter(m_CheckedNotificationTargets);

		m_CheckedNotificationTargets->Remove(target);
	}
	finally {
		Monitor::Exit(m_CheckedNotificationTargets);
	}
}


Guid ^ Updater::OwnGuid::get() {
	return m_OwnGuid;
}



Updater ^ Updater::Singelton::get()
{
	return m_Singelton;
}

void Updater::Singelton::set(Updater ^ value)
{
	m_Singelton = value;
}


void Updater::StartCheck()
{
	if(!m_CheckThread->IsAlive)
		m_CheckThread->Start();
	else
		m_CheckThreadWork->Set();
}

