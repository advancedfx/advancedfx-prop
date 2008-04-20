#include <wx/wx.h>

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
	m_datalist = new wxList();
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
		g_debug.SendMessage(_T("Config: The group named \"") + GetName() + _T("\" does not have any properties!"),
		hlaeDEBUG_WARNING);
	return m_datalist->GetCount();
}

hlaeConfig::hlaeConfig()
	: wxObject()
{
	m_document = new wxXmlDocument();
	m_propertylist = new wxList();
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
	m_filename->SetFullName(_T("config.xml"));

	// add the variables
	hlaeConfigListGroup* general = new hlaeConfigListGroup(_T("general"));
	general->AppendObject(new hlaeConfigListData(_T("advanced_view"),_T("false")));
	general->AppendObject(new hlaeConfigListData(_T("language"),_T("0")));
	AppendPropertyGroup(general);

	hlaeConfigListGroup* adresses = new hlaeConfigListGroup(_T("adresses"));
	adresses->AppendObject(new hlaeConfigListData(_T("base"),_T("0x00000000")));
	adresses->AppendObject(new hlaeConfigListData(_T("scr"),_T("0x00000000")));
	AppendPropertyGroup(adresses);

	hlaeConfigListGroup* launcher = new hlaeConfigListGroup(_T("launcher"));
	launcher->AppendObject(new hlaeConfigListData(_T("path"),_T("Please select HL.exe...")));
	launcher->AppendObject(new hlaeConfigListData(_T("modsel"),_T("2")));
	launcher->AppendObject(new hlaeConfigListData(_T("mod"),_T("")));
	launcher->AppendObject(new hlaeConfigListData(_T("depthsel"),_T("1")));
	launcher->AppendObject(new hlaeConfigListData(_T("depth"),_T("")));
	launcher->AppendObject(new hlaeConfigListData(_T("width"),_T("800")));
	launcher->AppendObject(new hlaeConfigListData(_T("height"),_T("600")));
	launcher->AppendObject(new hlaeConfigListData(_T("force"),_T("true")));
	launcher->AppendObject(new hlaeConfigListData(_T("cmdline"),_T("-demoedit")));
	launcher->AppendObject(new hlaeConfigListData(_T("capturemethod"),_T("1")));
	AppendPropertyGroup(launcher);

	// check if there is a file to load
	if (m_filename->FileExists(m_filename->GetFullPath())) Reload();
	else Flush();
}

void hlaeConfig::AppendPropertyGroup(hlaeConfigListGroup* group)
{
	m_propertylist->Append(group);
}

const wxString& hlaeConfig::GetPropertyString(const wxString& group_name, const wxString& property_name)
{
	return GetPropertyData(GetPropertyGroup(group_name), property_name)->GetData();
}

int hlaeConfig::GetPropertyInteger(const wxString& group_name, const wxString& property_name)
{
	return wxAtoi(GetPropertyString(group_name, property_name));
}

bool hlaeConfig::GetPropertyBoolean(const wxString& group_name, const wxString& property_name)
{
	bool retval;
	wxString value = GetPropertyString(group_name, property_name);

	if (value == _T("true")) retval = true;
	else if (value == _T("false")) retval = false;
	// else TODO: log -> undefined

	return retval;
}

void hlaeConfig::SetPropertyString(const wxString& group_name, const wxString& property_name,
	const wxString& property_value)
{
	GetPropertyData(GetPropertyGroup(group_name), property_name)->SetData(property_value);
}

void hlaeConfig::SetPropertyInteger(const wxString& group_name, const wxString& property_name, int property_value)
{
	SetPropertyString(group_name, property_name, wxString::Format(_T("%i"), property_value));
}

void hlaeConfig::SetPropertyBoolean(const wxString& group_name, const wxString& property_name, bool property_value)
{
	wxString retval;

	if (property_value) retval = _T("true");
	else retval = _T("false");

	SetPropertyString(group_name, property_name, retval);
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
			g_debug.SendMessage(_T("Config: There is no group named \"") + group_name + _T("\"!"), hlaeDEBUG_FATALERROR);
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
			g_debug.SendMessage(_T("Config: There is no data named \"") + property_name + _T("\"!"), hlaeDEBUG_FATALERROR);
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
	wxXmlNode* root_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("hlaeconfig"));
	m_document->SetRoot(root_node);

	wxXmlNode* settings_node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("settings"));
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
			wxXmlNode* node = new wxXmlNode(parent_node, wxXML_ELEMENT_NODE, _T("property"),
				wxEmptyString, new wxXmlProperty(_T("name"), data->GetName()));
			node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, data->GetData()));
		}
	}

	// save the file
	m_document->Save(m_filename->GetFullPath());
}

#include <wx/msgdlg.h>

void hlaeConfig::Reload()
{
	m_document->Load(m_filename->GetFullPath());
	if (m_document->IsOk()) g_debug.SendMessage(_T("Config: \"") + m_filename->GetFullPath() +
		wxT ("\" sucessfully loaded"), hlaeDEBUG_VERBOSE_LEVEL1);

	wxXmlNode* root_node = m_document->GetRoot();
	wxXmlNode* parent_node = root_node->GetChildren();

	while (parent_node != NULL)
	{
		// settings
		if (parent_node->GetName() == _T("settings"))
		{
			// get first group from the root
			wxXmlNode* group_node = parent_node->GetChildren();
			// check group nodes
			while (group_node != NULL)	
			{
				// get first node from the group
				wxXmlNode* node = group_node->GetChildren();
				// check property nodes
				while (node != NULL)
				{
					// only accept (our) "property" tags :)
					if (node->GetName() == _T("property"))
					{
						wxXmlProperty* prop = node->GetProperties();
						// search properties for the name tag
						while (prop != NULL)
						{
							// if found update the variable
							if (prop->GetName() == _T("name"))
							{
								hlaeConfigListData* data = GetPropertyData(GetPropertyGroup(group_node->GetName()),
									prop->GetValue());

								wxString value;

								if (node->GetChildren() != NULL) value = node->GetChildren()->GetContent();
								else value = _T("");

								data->SetData(value);
								break;
							}
							prop = prop->GetNext();
						}
					}
					// go to the next property element
					node = node->GetNext();
				}
				// go to the next group
				group_node = group_node->GetNext();
			}
			// (not needed yet) go the next category
			parent_node = parent_node->GetNext();
		}
	}
}