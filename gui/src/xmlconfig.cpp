//*****************************************************************************
// @file	wxXMLConfig.cpp
// @author	Nicolas Dextraze
// @web		http://www.nicdex.com
// @date	February 17, 2007
// @desc	wxXMLConfig class implementation
//*****************************************************************************
#include <wx/xmlconfig.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/utils.h>

#define XMLCONF_TRACE_MASK _T("xmlconf")

wxXMLConfig::wxXMLConfig( const wxString& appName,
				 const wxString& vendorName,
				 const wxString& localFilename,
				 const wxString& globalFilename,
				 long style,
				 const wxMBConv& conv )
				 : wxConfigBase(appName, vendorName, m_strLocalFile, m_strGlobalFile, style),
				 m_strLocalFile(localFilename),
				 m_strGlobalFile(globalFilename),
				 m_pCurrentGroup(NULL),
				 m_xmlDoc(NULL)
{
    // Make up names for files if empty
    if ( m_strLocalFile.empty() && (style & wxCONFIG_USE_LOCAL_FILE) )
        m_strLocalFile = GetLocalFileName(GetAppName());

    if ( m_strGlobalFile.empty() && (style & wxCONFIG_USE_GLOBAL_FILE) )
        m_strGlobalFile = GetGlobalFileName(GetAppName());

    // Check if styles are not supplied, but filenames are, in which case
    // add the correct styles.
    if ( !m_strLocalFile.empty() )
        SetStyle(GetStyle() | wxCONFIG_USE_LOCAL_FILE);

    if ( !m_strGlobalFile.empty() )
        SetStyle(GetStyle() | wxCONFIG_USE_GLOBAL_FILE);

    // if the path is not absolute, prepend the standard directory to it
    // UNLESS wxCONFIG_USE_RELATIVE_PATH style is set
    if ( !(style & wxCONFIG_USE_RELATIVE_PATH) )
    {
        if ( !m_strLocalFile.empty() && !wxIsAbsolutePath(m_strLocalFile) )
        {
            m_strLocalFile.Prepend( GetLocalDir() );
        }

        if ( !m_strGlobalFile.empty() && !wxIsAbsolutePath(m_strGlobalFile) )
        {
            m_strGlobalFile.Prepend( GetGlobalDir() );
        }
    }

    Init();
}

#ifdef wxUSE_STREAMS
wxXMLConfig::wxXMLConfig( wxInputStream& inStream, const wxMBConv& conv )
: m_pCurrentGroup(NULL),
m_xmlDoc(NULL)
{
    // always local_file when this constructor is called (?)
    SetStyle(GetStyle() | wxCONFIG_USE_LOCAL_FILE);

	m_xmlDoc = new wxXmlDocument( inStream );
	if ( !m_xmlDoc->IsOk() )
	{
		wxLogError( _("can't parse user configuration") );
	}
	else
	{
		m_pCurrentGroup = m_xmlDoc->GetRoot();

		SetRootPath();
		ResetDirty();
	}
}
#endif

wxXMLConfig::~wxXMLConfig()
{
	Flush();

	CleanUp();
}

wxString wxXMLConfig::GetGlobalFileName( const wxChar *szFile )
{
	wxString str = GetGlobalDir();
	str << szFile;

	if ( wxStrchr(szFile, wxT('.')) == NULL )
	{
		str << wxT(".config");
	}

	return str;
}

wxString wxXMLConfig::GetLocalFileName( const wxChar *szFile )
{
	wxString str = GetLocalDir();
	str << szFile;

	if ( wxStrchr(szFile, wxT('.')) == NULL )
	{
		str << wxT('.');
		str << ::wxGetUserId();
		str << wxT(".config");
	}

	return str;
}

void wxXMLConfig::SetPath( const wxString& strPath )
{
	DoSetPath( strPath, true );
}

bool wxXMLConfig::GetFirstGroup( wxString& str, long& lIndex ) const
{
	lIndex = 0;
	return GetNextGroup( str, lIndex );
}

bool wxXMLConfig::GetNextGroup( wxString& str, long& lIndex ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );
	bool getNextGroup = false;

	if ( size_t(lIndex) < GetNumberOfGroups() )
	{
		wxXMLConfigEntry *pEntry = m_pCurrentGroup->GetChildren();

		long groupPos = 0;
		while( pEntry )
		{
			if ( IsGroup( pEntry ) )
			{
				if ( groupPos == lIndex )
				{
					lIndex++;
					str = pEntry->GetName();
					getNextGroup = true;
					break;
				}
				groupPos++;
			}
			pEntry = pEntry->GetNext();
		}
	}

	return getNextGroup;
}

bool wxXMLConfig::GetFirstEntry( wxString& str, long& lIndex ) const
{
	lIndex = 0;
	return GetNextEntry( str, lIndex );
}

bool wxXMLConfig::GetNextEntry( wxString& str, long& lIndex ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );
	bool getFirstEntry = false;

	if ( size_t(lIndex) < GetNumberOfEntries() )
	{
		wxXMLConfigEntry *pEntry = m_pCurrentGroup->GetChildren();

		long entryPos = 0;
		while( pEntry )
		{
			if ( IsEntry( pEntry ) )
			{
				if ( entryPos == lIndex )
				{
					lIndex++;
					str = pEntry->GetName();
					getFirstEntry = true;
					break;
				}
				entryPos++;
			}
			pEntry = pEntry->GetNext();
		}
	}

	return getFirstEntry;
}

size_t wxXMLConfig::GetNumberOfEntries( bool bRecursive ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigEntry *pEntry = m_pCurrentGroup->GetChildren();
	size_t entriesCount = 0;
	while( pEntry != NULL )
	{
		if ( IsGroup(pEntry) )
		{
			if ( bRecursive )
			{
				wxConfigPathChanger	path( this, GetPath() + wxCONFIG_PATH_SEPARATOR + pEntry->GetName() + wxCONFIG_PATH_SEPARATOR );
				entriesCount += GetNumberOfEntries( bRecursive );
			}
		}
		else if ( IsEntry(pEntry) )
		{
			entriesCount++;
		}

		pEntry = pEntry->GetNext();
	}

	return entriesCount;
}

size_t wxXMLConfig::GetNumberOfGroups( bool bRecursive ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigGroup *pGroup = m_pCurrentGroup->GetChildren();
	size_t groupsCount = 0;
	while( pGroup != NULL )
	{
		if ( IsGroup(pGroup) ) // group
		{
			groupsCount++;
			if ( bRecursive )
			{
				wxConfigPathChanger path( this, GetPath() + wxCONFIG_PATH_SEPARATOR + pGroup->GetName() + wxCONFIG_PATH_SEPARATOR );
				groupsCount += GetNumberOfGroups( bRecursive );
			}
		}
	
		pGroup = pGroup->GetNext();
	}

	return groupsCount;
}

bool wxXMLConfig::HasGroup( const wxString& strName ) const
{
	if ( strName.empty() )
		return false;

	const wxString pathOld = GetPath();

	wxXMLConfig *self = wx_const_cast( wxXMLConfig *, this );

	const bool rc = self->DoSetPath( strName,false );

	self->SetPath( pathOld );

	return rc;
}

bool wxXMLConfig::HasEntry( const wxString& entry ) const
{
	wxString path = entry.BeforeLast(wxCONFIG_PATH_SEPARATOR);

	if ( path.empty() && *entry.c_str() == wxCONFIG_PATH_SEPARATOR )
	{
		path = wxCONFIG_PATH_SEPARATOR;
	}

	// set the path and keep a copy of the the current path to set it back after
	wxString pathOld;
	wxXMLConfig* const self = wx_const_cast( wxXMLConfig *, this );
	if ( !path.empty() )
	{
		pathOld = GetPath();
		if ( pathOld.empty() )
			pathOld = wxCONFIG_PATH_SEPARATOR;

		if ( !self->DoSetPath( path, false ) )
			return false;
	}

	// we look for the entry
	bool exists = ( FindEntry( entry ) != NULL );

	// we set back the old path
	if ( !pathOld.empty() )
	{
		self->SetPath( pathOld );
	}

	return exists;
}

bool wxXMLConfig::Flush( bool bCurrentOnly )
{
	if ( !IsDirty() || !m_strLocalFile )
		return true;

	if ( !m_xmlDoc->Save( m_strLocalFile ) )
	{
		wxLogError(_("can't save user configuration file."));
		return false;
	}

	ResetDirty();

	return true;
}

bool wxXMLConfig::RenameEntry( const wxString& oldName, const wxString& newName )
{
	wxASSERT_MSG( !wxStrchr(oldName, wxCONFIG_PATH_SEPARATOR), wxT("RenameEntry(): paths are not supported") );

	wxXMLConfigEntry *oldEntry = FindEntry( oldName );
	if ( !oldEntry )
		return false;

	if ( FindEntry( newName ) )
		return false;

	wxString value = oldEntry->GetNodeContent();
	if ( DeleteEntry( oldName ) )
		return false;

	SetDirty();

	wxXMLConfigEntry *newEntry = AddEntry( newName );
	newEntry->GetChildren()->SetContent( value );

	return true;
}

bool wxXMLConfig::RenameGroup( const wxString& oldName, const wxString& newName )
{
	wxASSERT_MSG( !wxStrchr(oldName, wxCONFIG_PATH_SEPARATOR), wxT("RenameGroup(): paths are not supported") );

	if ( !FindGroup( oldName ) )
		return false;

	if ( FindGroup( newName ) )
		return false;

	if ( DeleteGroup( oldName ) )
		return false;

	SetDirty();

	AddGroup( newName );

	return true;
}

bool wxXMLConfig::DeleteEntry( const wxString& key, bool bGroupIfEmptyAlso )
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	bool deleteEntry = false;

	wxXMLConfigEntry *pLastEntry = NULL;
	wxXMLConfigEntry *pCurrentEntry = m_pCurrentGroup->GetChildren();
	while( pCurrentEntry )
	{
		if ( IsEntry( pCurrentEntry ) && ( pCurrentEntry->GetName() == key ) )
			break;

		pLastEntry = pCurrentEntry;
		pCurrentEntry = pCurrentEntry->GetNext();
	}

	if ( pCurrentEntry )
	{
		wxXMLConfigEntry *pNextEntry = pCurrentEntry->GetNext();
		wxXMLConfigGroup *pParentGroup = pCurrentEntry->GetParent();

		if ( pLastEntry )
		{
			pLastEntry->SetNext( pNextEntry );
		}
		else
		{
			if ( pParentGroup && ( pParentGroup->GetChildren() == pCurrentEntry ) )
			{
				pParentGroup->SetChildren( pNextEntry );
				if ( pNextEntry )
					pNextEntry->SetParent( pParentGroup );
			}
		}

		delete pCurrentEntry;

		deleteEntry = true;
	}

	if ( ( GetNumberOfGroups() == 0 ) && ( GetNumberOfEntries() == 0 ) && bGroupIfEmptyAlso )
	{
		wxString strPath = GetPath().BeforeLast(wxCONFIG_PATH_SEPARATOR);
		wxString groupName = m_pCurrentGroup->GetName();

		if ( !strPath.empty() )
		{
			SetPath( strPath );
			DeleteGroup( groupName );
		}
	}

	return deleteEntry;
}

bool wxXMLConfig::DeleteGroup( const wxString& key )
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	bool deleteGroup = false;

	wxXMLConfigGroup *pLastGroup = NULL;
	wxXMLConfigGroup *pCurrentGroup = m_pCurrentGroup->GetChildren();
	while( pCurrentGroup )
	{
		if ( IsGroup( pCurrentGroup ) && ( pCurrentGroup->GetName() == key ) )
		{
			wxXMLConfigEntry *pNextGroup = pCurrentGroup->GetNext();

			if ( pLastGroup )
			{
				pLastGroup->SetNext( pNextGroup );
			}
			else
			{
				wxXMLConfigGroup *pParentGroup = pCurrentGroup->GetParent();

				if ( pParentGroup && ( pParentGroup->GetChildren() == pCurrentGroup ) )
				{
					pParentGroup->SetChildren( pNextGroup );
					if ( pNextGroup )
						pNextGroup->SetParent( pParentGroup );
				}
			}

			delete pCurrentGroup;

			deleteGroup = true;
			break;
		}

		pLastGroup = pCurrentGroup;
		pCurrentGroup = pCurrentGroup->GetNext();
	}

	return deleteGroup;
}

bool wxXMLConfig::DeleteAll()
{
	CleanUp();

	if ( !m_strLocalFile.empty() )
	{
	  if ( wxFile::Exists(m_strLocalFile) && wxRemove(m_strLocalFile) == -1 )
	  {
		  wxLogSysError(_("can't delete user configuration file '%s'"),
						m_strLocalFile.c_str());
		  return false;
	  }
	}

	Init();

	return true;
}

#if wxUSE_STREAMS
bool wxXMLConfig::Save( wxOutputStream& os, const wxMBConv& conv )
{
	wxASSERT_MSG( m_xmlDoc != NULL, wxT("xml document is null") );

	if ( !m_xmlDoc->Save( os ) )
	{
		wxLogError( _("can't save user configuration file") );
		return false;
	}

	ResetDirty();

	return true;
}
#endif

bool wxXMLConfig::DoReadString( const wxString& key, wxString* pStr ) const
{
	wxConfigPathChanger path(this, key);

    wxXMLConfigEntry *pEntry = FindEntry(path.Name());
    if (pEntry == NULL) {
        return false;
    }

    *pStr = pEntry->GetNodeContent();

    return true;
}

bool wxXMLConfig::DoReadLong( const wxString& key, long* pl ) const
{
	wxString str;
    if ( !Read(key, &str) )
        return false;

    // extra spaces shouldn't prevent us from reading numeric values
    str.Trim();

    return str.ToLong(pl);
}

bool wxXMLConfig::DoWriteString( const wxString& key, const wxString& szValue )
{
	wxConfigPathChanger     path(this, key);
    wxString                strName = path.Name();

    wxLogTrace( XMLCONF_TRACE_MASK,
                _T("  Writing String '%s' = '%s' to Group '%s'"),
                strName.c_str(),
                szValue.c_str(),
                GetPath().c_str() );

    if ( strName.empty() )
    {
		// setting the value of a group is an error
        wxASSERT_MSG( szValue.empty(), wxT("can't set value of a group!") );

		// ... except if it's empty in which case it's a way to force it's creation
        wxLogTrace( XMLCONF_TRACE_MASK,
                    _T("  Creating group '%s'"),
                    m_pCurrentGroup->GetName().c_str() );

        SetDirty();
    }
    else
    {
        // writing an entry check that the name is reasonable
        if ( strName[0u] == wxCONFIG_IMMUTABLE_PREFIX )
        {
            wxLogError( _("Config entry name cannot start with '%c'."),
                        wxCONFIG_IMMUTABLE_PREFIX);
            return false;
        }

        wxXMLConfigEntry *pEntry = FindEntry(strName);

        if ( pEntry == NULL )
        {
            wxLogTrace( XMLCONF_TRACE_MASK,
                        _T("  Adding Entry '%s'"),
                        strName.c_str() );
            pEntry = AddEntry(strName);
        }

        wxLogTrace( XMLCONF_TRACE_MASK,
                    _T("  Setting value '%s'"),
                    szValue.c_str() );
        pEntry->GetChildren()->SetContent(szValue);

        SetDirty();
    }

    return true;
}

bool wxXMLConfig::DoWriteLong( const wxString& key, long lValue )
{
	return Write(key, wxString::Format(_T("%ld"), lValue));
}

wxString wxXMLConfig::GetGlobalDir()
{
	return wxT(".\\");
}

wxString wxXMLConfig::GetLocalDir()
{
	return wxT(".\\");
}

void wxXMLConfig::Init()
{
	wxXmlDocument *globalXmlDoc = NULL;
	wxXmlDocument *localXmlDoc = NULL;

	m_xmlDoc = new wxXmlDocument();

	// open global file
    if ( !m_strGlobalFile.empty() && wxFile::Exists(m_strGlobalFile) )
    {
		wxXmlDocument globalXmlDoc( m_strGlobalFile );
		if ( globalXmlDoc.IsOk() )
		{
			Parse( globalXmlDoc, false );
		}
		else
		{
			wxLogWarning( _("can't open global configuration file '%s'."), m_strGlobalFile.c_str() );
		}
    }

    // parse the local file
    if ( !m_strLocalFile.empty() && wxFile::Exists(m_strLocalFile) )
    {
        wxXmlDocument localXmlDoc( m_strLocalFile );
        if ( localXmlDoc.IsOk() )
        {
			Parse( localXmlDoc, true );
        }
        else
        {
            wxLogWarning( _("can't open user configuration file '%s'."), m_strLocalFile.c_str() );
        }
    }

	if ( !m_xmlDoc->IsOk() )
	{
		m_xmlDoc->SetRoot( new wxXmlNode( NULL, wxXML_ELEMENT_NODE, wxT("config") ) );
	}
	
	m_pCurrentGroup = m_xmlDoc->GetRoot();

    m_isDirty = false;
}

void wxXMLConfig::CleanUp()
{
	delete m_xmlDoc;
	m_xmlDoc = NULL;
}

//TODO: combining of xml tree
void wxXMLConfig::Parse( const wxXmlDocument& xmlDocument, bool bLocal )
{
	// for now we replace the document with the one provided in parameter
	if ( m_xmlDoc )
		delete m_xmlDoc;

	m_xmlDoc = new wxXmlDocument( xmlDocument );	
}

void wxXMLConfig::SetRootPath()
{
	m_strPath.Empty();
	m_pCurrentGroup = m_xmlDoc->GetRoot();
}

bool wxXMLConfig::DoSetPath( const wxString& strPath, bool createMissingComponents )
{
    wxArrayString aParts;

    if ( strPath.empty() ) {
        SetRootPath();
        return true;
    }

    if ( strPath[0] == wxCONFIG_PATH_SEPARATOR ) {
        // absolute path
        wxSplitPath(aParts, strPath);
    }
    else {
        // relative path, combine with current one
        wxString strFullPath = m_strPath;
        strFullPath << wxCONFIG_PATH_SEPARATOR << strPath;
        wxSplitPath(aParts, strFullPath);
    }

    // change current group
    size_t n;
    m_pCurrentGroup = m_xmlDoc->GetRoot();
    for ( n = 0; n < aParts.Count(); n++ ) {
        wxXMLConfigGroup *pNextGroup = FindGroup( aParts[n] );
        if ( pNextGroup == NULL )
        {
            if ( !createMissingComponents )
                return false;

            pNextGroup = AddGroup( aParts[n] );
        }

        m_pCurrentGroup = pNextGroup;
    }

    // recombine path parts in one variable
    m_strPath.Empty();
    for ( n = 0; n < aParts.Count(); n++ ) {
        m_strPath << wxCONFIG_PATH_SEPARATOR << aParts[n];
    }

    return true;
}

bool wxXMLConfig::IsGroup( const wxXMLConfigGroup *group ) const
{
	return !IsEntry( group );
}

bool wxXMLConfig::IsEntry( const wxXMLConfigEntry *entry ) const
{
	if ( entry )
	{
		wxXmlNode *children = entry->GetChildren();

		if ( children && ( children->GetType() == wxXML_TEXT_NODE ) )
			return true;

		return false;
	}
	else
	{
		return false;
	}
}

wxXMLConfigEntry *wxXMLConfig::FindEntry( const wxString& key ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigEntry *entry = NULL;

	wxXMLConfigEntry *pCurrent = m_pCurrentGroup->GetChildren();
	while( pCurrent )
	{
		if ( IsEntry( pCurrent ) && ( pCurrent->GetName() == key ) )
		{
			entry = pCurrent;
			break;
		}

		pCurrent = pCurrent->GetNext();
	}

	return entry;
}

wxXMLConfigEntry *wxXMLConfig::AddEntry( const wxString& key )
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigGroup *pParent = m_pCurrentGroup;
	wxXMLConfigEntry *pNext = m_pCurrentGroup->GetChildren();

	// create the new entry
	wxXMLConfigEntry *newEntry = new wxXMLConfigEntry( pParent, wxXML_ELEMENT_NODE, key, wxEmptyString, NULL, pNext );
	wxXmlNode *textNode = new wxXmlNode( newEntry, wxXML_TEXT_NODE, wxT("") );
	newEntry->AddChild( textNode );
	
	newEntry->SetName( key );
	
	// adjust tree
	pParent->SetChildren( newEntry );
	if ( pNext )
		pNext->SetParent( NULL );

	return newEntry;
}

wxXMLConfigGroup *wxXMLConfig::FindGroup( const wxString& key ) const
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigGroup *group = NULL;

	wxXMLConfigGroup *pCurrent = m_pCurrentGroup->GetChildren();
	while( pCurrent )
	{
		if ( IsGroup( pCurrent ) && ( pCurrent->GetName() == key ) )
		{
			group = pCurrent;
			break;
		}

		pCurrent = pCurrent->GetNext();
	}

	return group;
}

wxXMLConfigGroup *wxXMLConfig::AddGroup( const wxString& key )
{
	wxASSERT_MSG( m_pCurrentGroup != NULL, wxT("current group is null") );
	wxASSERT_MSG( IsGroup(m_pCurrentGroup), wxT("current group is not a group") );

	wxXMLConfigGroup *pParent = m_pCurrentGroup;
	wxXMLConfigGroup *pNext = m_pCurrentGroup->GetChildren();

	// create the new entry
	wxXMLConfigGroup *newGroup = new wxXMLConfigGroup( pParent, wxXML_ELEMENT_NODE, key, wxEmptyString, NULL, pNext );
	newGroup->SetName( key );

	// adjust tree
	pParent->SetChildren( newGroup );
	if ( pNext )
		pNext->SetParent( NULL );

	return newGroup;
}