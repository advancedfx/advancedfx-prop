#include "StdAfx.h"

#include "Launcher.h"
#include "../system/loader.h"


using namespace hlae;

void Launcher::OnFormClosed( FormClosedEventArgs^ e )
{
	InitLoader( 0, this->textBoxExe->Text, this->textBoxCmdPrev->Text );

	Form::OnFormClosed( e ); // call base class
}