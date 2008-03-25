#ifndef HLAE_CONFIG_H
#define HLAE_CONFIG_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>

#include <system/debug.h>


class hlaeConfigListObject : public wxObject
{
public:
	hlaeConfigListObject(const wxString& name);
	const wxString& GetName();
private:
	wxString m_name;
};

class hlaeConfigListData : public hlaeConfigListObject
{
public:
	hlaeConfigListData(const wxString& name, const wxString& default_data);
	void SetData(const wxString& data);
	const wxString& GetData();
	void RestoreDefault();
private:
	wxString m_defaultdata;
	wxString m_data;
};

class hlaeConfigListGroup : public hlaeConfigListObject
{
public:
	hlaeConfigListGroup(const wxString& name);
	~hlaeConfigListGroup();
	void AppendObject(hlaeConfigListData* object);
	hlaeConfigListData* GetObject(size_t index);
	size_t GetCount();
private:
	wxList* m_datalist;
};

class hlaeConfig : public wxObject
{
public:
	hlaeConfig();
	~hlaeConfig();
	void Initialize();
	void AppendPropertyGroup(hlaeConfigListGroup* group);
	void SetPropertyString (const wxString& group_name, const wxString& property_name, const wxString& property_value);
	void SetPropertyInteger (const wxString& group_name, const wxString& property_name, int property_value);
	void SetPropertyBoolean (const wxString& group_name, const wxString& property_name, bool property_value);
	const wxString& GetPropertyString(const wxString& group_name, const wxString& property_name);
	int GetPropertyInteger(const wxString& group_name, const wxString& property_name);
	bool GetPropertyBoolean(const wxString& group_name, const wxString& property_name);
	void RestoreDefaults();
	void Flush();
	void Reload();
private:
	hlaeConfigListGroup* GetPropertyGroup(const wxString& group_name);
	hlaeConfigListData* GetPropertyData(hlaeConfigListGroup* group, const wxString& property_name);
	wxList* m_propertylist;
	wxFileName* m_filename;
	wxXmlDocument* m_document;
};

extern hlaeConfig g_config;

#endif