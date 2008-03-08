#ifndef _HLAE_CONFIG_H_
#define _HLAE_CONFIG_H_

#include "debug.h"
#include "list.h"

#include <wx/xml/xml.h>

class hlaeConfigListObject : public wxObject
{
public:
	hlaeConfigListObject(const wxString& name);
	const wxString& GetName();
private:
	wxString m_name;
};

class hlaeConfigListGroup : public hlaeConfigListObject
{
public:
	hlaeConfigListGroup(const wxString& name);
	~hlaeConfigListGroup();
	void AddObject(hlaeConfigListObject* config_object);
	size_t GetCount();
	hlaeConfigListObject* GetObject(size_t index);
private:
	hlaeList* m_objectlist;
};

class hlaeConfigListDataObject : public hlaeConfigListObject
{
public:
	hlaeConfigListDataObject(const wxString& name);
	virtual void Parse(const wxString& string) = 0;
};

class hlaeConfigListIntegerObject : public hlaeConfigListDataObject
{
public:
	hlaeConfigListIntegerObject(const wxString& name, int integer_value);
	int GetInteger();
	virtual void Parse(const wxString& string);
private:
	int m_integervalue;
};

class hlaeConfigListStringObject : public hlaeConfigListDataObject
{
public:
	hlaeConfigListStringObject(const wxString& name, const wxString& string_value);
	const wxString& GetString();
	virtual void Parse(const wxString& string);
private:
	wxString m_stringvalue;
};

class hlaeConfigListFloatObject : public hlaeConfigListDataObject
{
public:
	hlaeConfigListFloatObject(const wxString& name, float float_value);
	float GetFloat();
	virtual void Parse(const wxString& string);
private:
	float m_floatvalue;
};

class hlaeConfigListDwordObject : public hlaeConfigListDataObject
{
public:
	hlaeConfigListDwordObject(const wxString& name, wxUint32 dword_value);
	wxUint32 GetDword();
	virtual void Parse(const wxString& string);
private:
	wxUint32 m_dwordvalue;
};

class hlaeConfig : public wxObject
{
public:
	hlaeConfig();
	~hlaeConfig();
	hlaeConfigListObject* GetProperty(const wxString& group_name, const wxString& property_name);
private:
	hlaeList* m_propertylist;
	wxXmlDocument* m_document;
	
};

extern hlaeConfig g_config;

#endif