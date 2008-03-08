#include "config.h"

hlaeConfig g_config;

hlaeConfigListObject::hlaeConfigListObject(const wxString& name)
{
	m_name = name;
}

const wxString& hlaeConfigListObject::GetName()
{
	return m_name;
}

hlaeConfigListGroup::hlaeConfigListGroup(const wxString& name)
	: hlaeConfigListObject(name)
{
	m_objectlist = new hlaeList();
	m_objectlist->DeleteContents(true);
}

hlaeConfigListGroup::~hlaeConfigListGroup()
{
	m_objectlist->Clear();
	delete m_objectlist;
}

void hlaeConfigListGroup::AddObject(hlaeConfigListObject* config_object)
{
	m_objectlist->Append(config_object);
}

size_t hlaeConfigListGroup::GetCount()
{
	return m_objectlist->GetCount();
}

hlaeConfigListObject* hlaeConfigListGroup::GetObject(size_t index)
{
	return dynamic_cast<hlaeConfigListObject*>(m_objectlist->Item(index));
}

hlaeConfigListDataObject::hlaeConfigListDataObject(const wxString& name)
	: hlaeConfigListObject(name)
{}

hlaeConfigListIntegerObject::hlaeConfigListIntegerObject(const wxString& name, int integer_value)
	: hlaeConfigListDataObject(name)
{
	m_integervalue = integer_value;
}

void hlaeConfigListIntegerObject::Parse(const wxString& string)
{
}

int hlaeConfigListIntegerObject::GetInteger()
{
	return m_integervalue;
}

hlaeConfigListStringObject::hlaeConfigListStringObject(const wxString& name, const wxString& string_value)
	: hlaeConfigListDataObject(name)
{
	m_stringvalue = string_value;
}

void hlaeConfigListStringObject::Parse(const wxString& string)
{
}

const wxString& hlaeConfigListStringObject::GetString()
{
	return m_stringvalue;
}

hlaeConfigListFloatObject::hlaeConfigListFloatObject(const wxString& name, float float_value)
	: hlaeConfigListDataObject(name)
{
	m_floatvalue = float_value;
}

void hlaeConfigListFloatObject::Parse(const wxString& string)
{
}

float hlaeConfigListFloatObject::GetFloat()
{
	return m_floatvalue;
}

hlaeConfigListDwordObject::hlaeConfigListDwordObject(const wxString& name, wxUint32 dword_value)
	: hlaeConfigListDataObject(name)
{
	m_dwordvalue = dword_value;
}

void hlaeConfigListDwordObject::Parse(const wxString& string)
{
}

wxUint32 hlaeConfigListDwordObject::GetDword()
{
	return m_dwordvalue;
}

hlaeConfig::hlaeConfig()
	: wxObject()
{
	m_document = new wxXmlDocument();

	m_propertylist = new hlaeList();
	m_propertylist->DeleteContents(true);
}

hlaeConfig::~hlaeConfig()
{
	delete m_document;

	m_propertylist->Clear();
	delete m_propertylist;
}

hlaeConfigListObject* hlaeConfig::GetProperty(const wxString& group_name, const wxString& property_name)
{
	/* for (hlaeList::iterator iter = m_propertylist->begin(); iter != m_propertylist->end(); ++iter)
	{
		hlaeConfigListGroup* current = *iter;

		if (current->GetName() == group_name)
		{

			break;
		}
		if (iter == m_propertylist->end())
		{
			g_debug.SendMessage(wxT("Config: There is no group called") + group_name, hlaeDEBUG_FATALERROR);
			return
	}*/
	return 0;
}