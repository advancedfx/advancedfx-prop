#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-13-31 by dominik.matrixstorm.com
//
// First changes:
// 2008-12-23 by dominik.matrixstorm.com

#define HLAE_UPDATER_URL "http://update.matrixstorm.com/61b65ac26b714c41a1d998af3c5bd6dd.xml"
#define HLAE_UPDATER_CURRENT_GUID "2f1b50f2-78c0-405f-804a-97db10482595"
#define HLAE_UPDATER_MAX_XML_REDIRECTS 1

using namespace System;
using namespace System::Threading;

namespace hlae {

enum class UpdaterCheckState
{
	Unknown,
	Checked,
	Checking
};

interface class IUpdaterCheckResult
{
	//
	// Properties:

	property String ^ Description {
		String ^ get();
	}

	property bool IsUpdated {
		bool get();
	}

	property String ^ Title {
		String ^ get();
	}

	property String ^ Url {
		String ^ get();
	}
};


// Updater /////////////////////////////////////////////////////////////////////

ref class Updater
{
public:
	Updater();
	~Updater();

	/// <summary> Triggers a new (asynchronus) update check. </summary>
	void StartCheck();

	//
	// Properties:

	/// <summmary> result of a check or nullptr </summary>
	property IUpdaterCheckResult ^ CheckResult {
		IUpdaterCheckResult ^ get();
	}

	property UpdaterCheckState CheckState {
		UpdaterCheckState get();
	}
	
private:
	IUpdaterCheckResult ^ m_CheckResult;
	UpdaterCheckState m_CheckState;
	Thread ^ m_CheckThread;
	Guid ^ m_OwnGuid;

	void CheckWorker();
};

} // namespace hlae

