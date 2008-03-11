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

hlaeConfigListData::hlaeConfigListData(const wxString& name, const wxString& default_data) : hlaeConfigListObject(name)
{
	m_data = default_data;
}

void hlaeConfigListData::SetData(const wxString& data)
{
	m_data = data;
}

const wxString& hlaeConfigListData::GetData()
{
	return m_data;
}

hlaeConfigListGroup::hlaeConfigListGroup(const wxString& name) : hlaeConfigListObject(name)
{
	m_datalist = new hlaeList();
	m_datalist->DeleteContents(true);
}

hlaeConfigListGroup::~hlaeConfigListGroup()
{
	m_datalist->Clear();
	delete m_datalist;
}

void hlaeConfigListGroup::AppendObject(hlaeConfigListData* object)
{
	m_datalist->Append(object);
}

size_t hlaeConfigListGroup::GetCount()
{
	return m_datalist->GetCount();
}

hlaeConfigListData* hlaeConfigListGroup::GetObject(size_t index)
{
	return dynamic_cast<hlaeConfigListData*>(m_datalist->Item(index));
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

void hlaeConfig::AppendPropertyGroup(hlaeConfigListGroup* group)
{
	m_propertylist->Append(group);
}

const wxString& hlaeConfig::GetString(const wxString& group_name, const wxString& property_name)
{
	return GetPropertyData(GetPropertyGroup(group_name),property_name);
}

hlaeConfigListGroup* hlaeConfig::GetPropertyGroup(const wxString& group_name)
{
	hlaeConfigListGroup* retval;

	for (hlaeList::iterator iter = m_propertylist->begin(); iter != m_propertylist->end(); iter++)
	{
		wxObject* object = *iter;
		hlaeConfigListGroup* current = dynamic_cast<hlaeConfigListGroup*>(object);

		if (current->GetName() == group_name)
		{
			retval = current;
			break;
		}

		if (iter == m_propertylist->end())
		{
			g_debug.SendMessage(wxT("Config: There is no group named \"") + group_name + wxT("\"!"), hlaeDEBUG_FATALERROR);
			retval = 0;
		}
	}

	return retval;
}

const wxString& hlaeConfig::GetPropertyData(hlaeConfigListGroup* group, const wxString& property_name)
{
	wxString retval;

	for (size_t i = 0; i < group->GetCount(); i++)
	{
		if (group->GetObject(i)->GetName() == property_name)
		{
			retval = group->GetObject(i)->GetName();
			break;
		}

		if (i + 1 == group->GetCount())
		{
			g_debug.SendMessage(wxT("Config: There is no property named \"") + property_name + wxT("\"!"), hlaeDEBUG_FATALERROR);
			retval = wxT("");
		}
	}

	return wxT("");
}