#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-13-31 by dominik.matrixstorm.com
//
// First changes:
// 2008-12-23 by dominik.matrixstorm.com

#define HLAE_UPDATER_URL "http://update.matrixstorm.com/61b65ac26b714c41a1d998af3c5bd6dd.xml"

// definition of HLAE_UPDATER_OLD_GUID is optional
#define HLAE_UPDATER_OLD_GUID "f5610e64-bb95-44f6-8e17-d2900a434d19"

#define HLAE_UPDATER_CURRENT_GUID "def6946d-cf48-40dd-9a71-95be3facfba7"
#define HLAE_UPDATER_MAX_XML_REDIRECTS 1

using namespace System;
using namespace System::Threading;

namespace hlae {

interface class IUpdaterCheckResult
{
	//
	// Properties:

	property bool IsUpdated {
		bool get();
	}

	property System::Uri ^ Uri {
		System::Uri ^ get();
	}
};

delegate void UpdaterCheckedHandler(System::Object ^sender, IUpdaterCheckResult ^checkResult);

ref class UpdaterNotificationTarget
{
public:
	UpdaterNotificationTarget(
		System::Windows::Forms::Control ^ targetThreadControl,
		UpdaterCheckedHandler ^ targetHandler
	) {
		m_TargetHandler = targetHandler;
		m_TargetThreadControl = targetThreadControl;
	}

	void Notify(System::Object ^ sender, IUpdaterCheckResult ^ checkResult) {
		m_TargetThreadControl->Invoke(m_TargetHandler, gcnew cli::array<System::Object ^,1>(2) {sender, checkResult}); 
	}

private:
	UpdaterCheckedHandler ^ m_TargetHandler;
	System::Windows::Forms::Control ^ m_TargetThreadControl;
};


// Updater /////////////////////////////////////////////////////////////////////

ref class Updater
{
public:
	Updater();
	~Updater();

	/// <summary> Triggers a new (asynchronus) update check. </summary>
	void StartCheck();

	/// <summary> Eventhandler (re-)triggered when a updatecheck completed </summary>
	void BeginCheckedNotification(UpdaterNotificationTarget ^ target);

	/// <remarks> Do not call upon notification (will cause deadlock). </remarks>
	void EndCheckedNotification(UpdaterNotificationTarget ^ target);

	//
	// Properties:

	property Guid ^ OwnGuid {
		Guid ^ get();
	}

	static property Updater ^ Singelton {
		Updater ^ get();
		void set(Updater ^ value);
	}
	
private:
	IUpdaterCheckResult ^ m_CheckResult;
	Thread ^ m_CheckThread;
	bool m_CheckThreadQuit;
	AutoResetEvent ^ m_CheckThreadWork;
	System::Collections::Generic::LinkedList<UpdaterNotificationTarget ^> ^ m_CheckedNotificationTargets;
	Guid ^ m_OwnGuid;
	Guid ^ m_OldGuid;
	static Updater ^ m_Singelton;

	void CheckWorker();
};

} // namespace hlae

