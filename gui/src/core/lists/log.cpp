#include <hlae/core/lists/log.h>

hlaeLogListElement::hlaeLogListElement(const wxString& message, const wxDateTime& date, int debuglevel)
	: m_message(message), m_date(date), m_debuglevel(debuglevel), wxObject()
{
	/*m_message = message;
	m_date = date;
	m_debuglevel = debuglevel;*/

}

hlaeLogListElement::~hlaeLogListElement()
{}

const wxDateTime& hlaeLogListElement::GetDate()
{
	return m_date;
}

const wxString& hlaeLogListElement::GetMessage()
{
	return m_message;
}

int hlaeLogListElement::GetDebugLevel()
{
	return m_debuglevel;
}