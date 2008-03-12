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
	m_defaultdata = default_data;
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

void hlaeConfigListData::RestoreDefault()
{
	m_data = m_defaultdata;
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

hlaeConfigListData* hlaeConfigListGroup::GetObject(size_t index)
{
	return dynamic_cast<hlaeConfigListData*>(m_datalist->Item(index)->GetData());
}

size_t hlaeConfigListGroup::GetCount()
{
	if (m_datalist->GetCount() == 0)
		g_debug.SendMessage(wxT("Config: A group named \"") + GetName() + wxT("\" does not have any properties!"),
		hlaeDEBUG_WARNING);
	return m_datalist->GetCount();
}

hlaeConfig::hlaeConfig()
	: wxObject()
{
	m_document = new wxXmlDocument();
	m_propertylist = new hlaeList();
	m_filename = new wxFileName();
}

hlaeConfig::~hlaeConfig()
{
	delete m_document;
	delete m_filename;

	m_propertylist->Clear();
	delete m_propertylist;
}

void hlaeConfig::Initialize()
{
	// we have to manually initialize the config system, because if we want to use the debug features
	// there would be no reference to the global debug vaiable because there are not created at runtime

	m_propertylist->DeleteContents(true);

	// path and name of the config file
	m_filename->AssignCwd(); // current work directory
	m_filename->SetFullName(wxT("config.xml"));

	// add the variables
	hlaeConfigListGroup* general = new hlaeConfigListGroup(wxT("general"));
	general->AppendObject(new hlaeConfigListData(wxT("advanced_view"),wxT("false")));
	general->AppendObject(new hlaeConfigListData(wxT("language"),wxT("0")));
	AppendPropertyGroup(general);

	hlaeConfigListGroup* adresses = new hlaeConfigListGroup(wxT("adresses"));
	adresses->AppendObject(new hlaeConfigListData(wxT("base"),wxT("0x00000000")));
	adresses->AppendObject(new hlaeConfigListData(wxT("scr"),wxT("0x00000000")));
	AppendPropertyGroup(adresses);

	// check if there is a file to load
	if (m_filename->FileExists(m_filename->GetFullPath())) Reload();
	else Flush();
}

void hlaeConfig::AppendPropertyGroup(hlaeConfigListGroup* group)
{
	m_propertylist->Append(group);
}

const wxString& hlaeConfig::GetString(const wxString& group_name, const wxString& property_name)
{
	return GetPropertyData(GetPropertyGroup(group_name), property_name)->GetData();
}

int hlaeConfig::GetInteger(const wxString& group_name, const wxString& property_name)
{
	return wxAtoi(GetString(group_name, property_name));
}

hlaeConfigListGroup* hlaeConfig::GetPropertyGroup(const wxString& group_name)
{
	hlaeConfigListGroup* retval;

	for (size_t i = 0; i < m_propertylist->GetCount(); i++)
	{
		hlaeConfigListGroup* group = dynamic_cast<hlaeConfigListGroup*>(m_propertylist->Item(i)->GetData());

		if (group->GetName() == group_name)
		{
			retval = group;
			break;
		}

		if (i + 1 == m_propertylist->GetCount())
		{
			g_debug.SendMessage(wxT("Config: There is no group named \"") + group_name + wxT("\"!"), hlaeDEBUG_FATALERROR);
			retval = 0;
		}
	}

	return retval;
}

hlaeConfigListData* hlaeConfig::GetPropertyData(hlaeConfigListGroup* group, const wxString& property_name)
{
	hlaeConfigListData* retval;

	for (size_t i = 0; i < group->GetCount(); i++)
	{
		hlaeConfigListData* data = group->GetObject(i);

		if (data->GetName() == property_name)
		{
			retval = data;
			break;
		}

		if (i + 1 == group->GetCount())
		{
			g_debug.SendMessage(wxT("Config: There is no data named \"") + property_name + wxT("\"!"), hlaeDEBUG_FATALERROR);
			retval = 0;
		}
	}

	return retval;
}

void hlaeConfig::RestoreDefaults()
{
	for (size_t i = 0; i < m_propertylist->GetCount(); i++)
	{
		hlaeConfigListGroup* group = dynamic_cast<hlaeConfigListGroup*>(m_propertylist->Item(i)->GetData());

		for (size_t h = 0; h < group->GetCount(); h++)
		{
			group->GetObject(h)->RestoreDefault();
		}
	}
}

void hlaeConfig::Flush()
{
	// add all nodes to the xml tree
	wxXmlNode* root_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("hlaeconfig"));
	m_document->SetRoot(root_node);

	wxXmlNode* settings_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("settings"));
	root_node->AddChild(settings_node);

	// add the groups to the settingsnode
	for (size_t i = 0; i < m_propertylist->GetCount(); i++)
	{
		hlaeConfigListGroup* group = dynamic_cast<hlaeConfigListGroup*>(m_propertylist->Item(i)->GetData());

		// add the group node
		wxXmlNode* parent_node = new  wxXmlNode(settings_node, wxXML_ELEMENT_NODE, group->GetName());

		for (size_t h = 0; h < group->GetCount(); h++)
		{
			hlaeConfigListData* data = group->GetObject(h);

			// add the property node
			wxXmlNode* node = new wxXmlNode(parent_node, wxXML_ELEMENT_NODE, wxT("property"),
				wxEmptyString, new wxXmlProperty(wxT("name"), data->GetName()));
			node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, data->GetData()));
		}
	}

	// save the file
	m_document->Save(m_filename->GetFullPath());
}

void hlaeConfig::Reload()
{
	m_document->Load(m_filename->GetFullPath());
	if (m_document->IsOk()) g_debug.SendMessage(wxT("Config: \"") + m_filename->GetFullPath() +
		wxT ("\" sucessfully loaded"), hlaeDEBUG_VERBOSE_LEVEL1);

	wxXmlNode* root_node = m_document->GetRoot();
	wxXmlNode* group_node = root_node->GetChildren();

	while (group_node != NULL)
	{
		// settings
		if (group_node->GetName() == wxT("settings"))
		{
			wxXmlNode* node = group_node->GetChildren();

			while (node != NULL)
			{
				if (node->GetName() == wxT("property"))
				{
					wxXmlProperty* prop = node->GetProperties();

					while (prop != NULL)
					{
						if (prop->GetName() == wxT("name"))
						{
							hlaeConfigListData* data = GetPropertyData(GetPropertyGroup(wxT("settings")),
								prop->GetValue());
							data->SetData(node->GetContent());
							break;
						}
					}
				}

				node = group_node->GetNext();
			}
		}

		group_node = root_node->GetNext();
	}
}