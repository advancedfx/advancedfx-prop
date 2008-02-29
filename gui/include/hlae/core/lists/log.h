#ifndef _HLAE_LOGLISTELEMENT_H_
#define _HLAE_LOGLISTELEMENT_H_

#include <wx/object.h>
#include <wx/datetime.h>
#include <wx/string.h>

class hlaeLogListElement : public wxObject
{

private:

	int m_debuglevel;
	wxDateTime m_date;
	wxString m_message;

public:

	hlaeLogListElement(const wxString& message, const wxDateTime& date, int debuglevel);
	~hlaeLogListElement();
	const wxDateTime& GetDate();
	const wxString& GetMessage();
	int GetDebugLevel();

};

#endif // _HLAE_LOGLISTELEMENT_H_