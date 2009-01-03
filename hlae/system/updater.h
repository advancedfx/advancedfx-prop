#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/updater.h

// Authors : last change / first change / name
// 2008-12-23 / 2008-12-23 / Dominik Tugend

#include <system/debug.h>
#include <system/config.h>
#include <system/globals.h>

using namespace System::Net;
using namespace System::Xml;

using namespace hlae::globals;

namespace hlae {
namespace updater {

enum class UpdateCheckSate
{
	US_FAILED,
	US_RECENT,
	US_OLD
};

public ref class Updater
{
public:
	Updater(CGlobals ^Globals)
	{
		this->Globals = Globals;
		m_valid = false;
		m_ownguid = gcnew System::Guid("2f1b50f2-78c0-405f-804a-97db10482595");
	}

	System::String ^GetTitle() { return m_title; };
	System::String ^GetUrl() { return m_url; };
	System::String ^GetDescription() { return m_description; };
	
	UpdateCheckSate CheckForUpdate()
	{
		UpdateCheckSate myRet = UpdateCheckSate::US_FAILED;
		if(RetriveData())
		{
			if(0 != m_guid->CompareTo(m_ownguid))
				myRet = UpdateCheckSate::US_OLD;
			else
				myRet = UpdateCheckSate::US_RECENT;
		}

		return myRet;
	}
	
private:
	CGlobals ^Globals;

	bool m_valid;
	System::Guid ^m_ownguid;
	System::Guid ^m_guid;
	System::String ^m_title;
	System::String ^m_url;
	System::String ^m_description;
	

	bool RetriveData()
	{
		// u want ugly code?
		// ok u get ugly code:

		bool bRes = true;
		HttpWebRequest^ request;
		HttpWebResponse^ response;
		System::IO::Stream ^stream;
		XmlDocument ^doc;

		try {
			request = (HttpWebRequest^)( WebRequest::Create( "http://update.advancedfx.org/61b65ac26b714c41a1d998af3c5bd6dd.xml" ) );
			request->MaximumAutomaticRedirections = 1;
			request->AllowAutoRedirect = true;
			HttpWebResponse^ response = (HttpWebResponse^)( request->GetResponse() );

			doc = gcnew XmlDocument();
			stream = response->GetResponseStream();
			doc->Load( stream );

			if(bRes)
			{
				XmlNode ^nodeUpdate = doc->SelectSingleNode("update");
				XmlNode ^anode;
				anode = nodeUpdate->SelectSingleNode("title");
				m_title = anode->InnerText;
				anode = nodeUpdate->SelectSingleNode("url");
				m_url = anode->InnerText;
				anode = nodeUpdate->SelectSingleNode("description");
				m_description = anode->InnerText;
				anode = nodeUpdate->SelectSingleNode("guid");
				m_guid = gcnew System::Guid( anode->InnerText );
			}

		}
		catch(System::Exception ^e) {
			ERROR_MESSAGE( Globals->debugMaster, System::String::Format("Updater exception: {0}",e) );
			bRes = false;
		}

		if(doc) delete doc;
		if(stream) stream->Close();
		if(response) response->Close();

		m_valid = bRes;

		return bRes;
	}
};

} // namespace updater
} // namespace hlae