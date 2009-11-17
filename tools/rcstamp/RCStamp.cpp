// Origininal Copyright (c) by some guy from the CodeProject.org
// modified by Gavin Bramhill to fit the needs for MDT


//#include "stdafx.h"

#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>

using namespace std;

char const * strUsage =
" RCSTAMP command line:\r\n"
"\r\n"
"  rcstamp file <format> [options...]\r\n"
"  rcstamp @file [<format>] [options...]\r\n"
"\r\n"
"  file  : resource script to modify\r\n"
"  @file : file containing a list of file (see below)\r\n"
"\r\n"
"  format: specifies version number changes, e.g. *.*.33.+\r\n"
"          * : don't modify position\r\n"
"          + : increment position by one\r\n"
"          number : set position to this value\r\n"
"  \r\n"
"  options:    \r\n"
"    -n  :         don't update the FileVersion string \r\n"
"                  (default: set to match the FILEVERSION value)\r\n"
"      \r\n"
"    -rRESNAME:    update only version resource with resource id RESNAME\r\n"
"                  (default: update all Version resources)\r\n"
"\r\n"
"    -l            the specified file(s) are file list files\r\n"
"\r\n"
"    -v            Verbose\r\n"
"    \r\n"
"  file list files:\r\n"
"\r\n"
"  can specify one file on each line, e.g.\r\n"
"  d:\\sources\\project1\\project1.rc\r\n"
"  d:\\sources\\project2\\project2.rc=*.*.*.+\r\n"
"\r\n"
"  a format in the list file overrides the format from the command line\r\n"
"  using the -l option, list files itself can be modified\r\n"
"\r\n";

// command line options:
//
//  rcstamp file <format> [options...]
//  rcstamp @file [<format>] [options...]
//
//  file  : resource script to modify
//  @file : file containing a list of file (see below)
//
//  
//  format: specifies verison numbr changes, e.g. *.*.33.+
//          * : don't modify
//          + : increment by one
//          number : set to this value
//          P : take from product version
//  
//  options:    
//    -n  :         don't update the FileVersion string 
//                  (by default, set to match the FILEVERSION value)
//      
//    -rRESNAME:    replace version resource with resource id RESNAME
//                  (default: replace 
//
//    -l            the specified file (or files in the file list) are file list files
//
//    -v            Verbose
//    
//  file list files:
//
//  can specify one file on each line, e.g.
//  d:\sources\project1\project1.rc
//  d:\sources\project2\project2.rc=*.*.*.+
//
//  specifying a format overrides the format specified on the command line
//  using the -l option, list files itself can be modified
//
//


// Command Line Information

char const *	versionFile		= NULL;
char const *    scriptFile      = NULL;         // single resource script (.rc), or name of list file
bool            scriptFileList  = false;        // script file is a file list file
char const *    format          = NULL;         // format specifier
char const *    resName         = NULL;

bool            processListFile = false;        // the file(s) specified are list files, not resource files
bool            noReplaceString = false;        // don't replace fileversion string
bool            verbose         = false;
bool			onlyupdate		= false;


bool ParseArg(char const * arg)
{
    if (*arg!='-' && *arg!='/') {
        if (scriptFile == NULL) {
            scriptFile = arg;
            if (*scriptFile=='@') {
                scriptFileList = true;
                ++scriptFile;
            }
        }
        else if (!onlyupdate && format == NULL) {
            format = arg;
        }
		else if (versionFile == NULL) {
			versionFile = arg;
		}
        else {
            cerr << "Unexpected argument\"" << arg << "\"\r\n";
            return false;
        }
        return true;
    }

    ++arg;
    char c = tolower(*arg);

    if (c=='n')         noReplaceString = true;
    else if (c=='r')    resName = arg+1;
    else if (c=='l')    processListFile = true;
    else if (c=='v')    verbose = true;
	else if (c=='u')	onlyupdate = true;
    else {
        cerr << "Unknown option\"" << arg << "\"\r\n";
        return false;
    }
    return true;
}


bool CalcNewVersion(char const * oldVersion, char const * fmtstr, char * version)
{
    if (!fmtstr) fmtstr = format;

    char const * fmt[4];
    char * fmtDup = strdup(fmtstr);
    fmt[0] = strtok(fmtDup, " .,");
    fmt[1] = strtok(NULL,   " .,");
    fmt[2] = strtok(NULL,   " .,");
    fmt[3] = strtok(NULL,   " .,");

    if (fmt[3] == 0) 
	{
        cerr << "Invalid Format\r\n";
        return false;
    }

    char * outHead = version;
    char * verDup = strdup(oldVersion);
    char * verStr = strtok(verDup, " ,.");

    *version = 0;

    for(int i=0; i<4; ++i) 
	{
        int oldVersion = atoi(verStr);
        int newVersion = oldVersion;

        char c = fmt[i][0];

        if (strcmp(fmt[i], "*")==0)
            newVersion = oldVersion;
        else if (isdigit(c))
            newVersion = atoi(fmt[i]);
        else if (c=='+' || c=='-') 
		{
            if (isdigit(fmt[i][1]))
                newVersion = oldVersion + atoi(fmt[i]);
            else
                newVersion = oldVersion + ((c=='+') ? 1 : -1);
        }

        itoa(newVersion, outHead, 10);
        outHead += strlen(outHead);

        if (i != 3) {
            strcpy(outHead, ",");
            outHead += 1;
            verStr = strtok(NULL, ",");
        }
    }
    free(fmtDup);
    free(verDup);
    return true;
}

bool ProcessFile(char const * fileName, char const * fmt = NULL)
{
	// Save the last-modified date for later
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FILETIME modifyTimeResource;
	BOOL b = GetFileTime(hFile, NULL, NULL, &modifyTimeResource);
	CloseHandle(hFile);

	hFile = CreateFile(versionFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FILETIME modifyTimeVersion;
	b = GetFileTime(hFile, NULL, NULL, &modifyTimeVersion);
	CloseHandle(hFile);

	ofstream version_cpp(versionFile);
	version_cpp << endl << "// Autogenerated file, do not edit." << endl << endl;

    const int MAXLINELEN = 2048;

    ifstream is(fileName);
    if (is.fail()) {
        cerr << "cannot open " << fileName << "\r\n";
        return false;
    }

    string result;

    char line[MAXLINELEN];
    char fileversion[64] = { 0 };       // "final" version string
    char productversion[64] = { 0 };       // "final" version string

    bool inReplaceBlock = false;

    while (!is.eof()) {
        is.getline(line, MAXLINELEN);
        if (is.bad()) {
            cerr << "Error reading " << fileName << "\r\n";
            return false;
        }

        if (processListFile) 
		{
            char * pos = strchr(line, '=');
            if (pos) {
                CalcNewVersion(pos+1, fmt, pos+1);  // in-place replace
            }
        }
        else 
		{
            char * dupl = strdup(line);
            char * word1 = strtok(dupl, " \t");
            char * word2 = strtok(NULL, " \t,");  // allow comma for [VALUE "FileVersion",] entry

            if (word1 && word2 && strcmpi(word2, "VERSIONINFO") == 0 && strnicmp(word1, "//",2)!=0) 
			{
              if (resName==NULL || strcmpi(resName, word1)==0) 
                  inReplaceBlock = true;
              else
                  inReplaceBlock = false;
            }

            if (inReplaceBlock) {
                if ( word1 && (strcmpi(word1, "FILEVERSION") == 0 || strcmpi(word1, "PRODUCTVERSION") == 0) )
				{
					//int offset;
					if (strcmpi(word1, "FILEVERSION") == 0)
					{
                        int offset = strlen("FILEVERSION") + word1 - dupl + 1;
						CalcNewVersion(line+offset, fmt, fileversion);
						strcpy(line+offset, fileversion);
						version_cpp << "int dwFileVersion[4] = { " << fileversion << " };" << endl;
					}
					else
					{
						int offset = strlen("PRODUCTVERSION") + word1 - dupl + 1;
						CalcNewVersion(line+offset, fmt, productversion);
						strcpy(line+offset, productversion);
						version_cpp << "int dwProductVersion[4] = { " << productversion << " };" << endl << endl;
					}


                    if (verbose) cout << line << "\r\n";
                }

                if (!noReplaceString && word1 && word2 && strcmpi(word1, "VALUE")==0)
				{
					if( strcmpi(word2, "\"FileVersion\"")==0 )
					{
						if (!*fileversion) 
						{
							cerr << "Error: VALUE \"FileVersion\" encountered before FILEVERSION\r\n";
						}
						else 
						{
							for (unsigned int j=0; j<strlen(fileversion); j++) 
								if (fileversion[j] == ',') fileversion[j] = '.';
	                       
							char * output = strchr(line, ',');
							if (!output) 
								output = line + strlen(line);
							else 
								++output;

							sprintf(output, " \"%s\"", fileversion);
							version_cpp << "const char *pszFileVersion = \"" << fileversion << "\";" << endl;
							if (verbose) cout << line << "\r\n";
						}
					}
					if( strcmpi(word2, "\"ProductVersion\"")==0 )
					{
						if (!*productversion) 
						{
							cerr << "Error: VALUE \"ProductVersion\" encountered before PRODUCTVERSION\r\n";
						}
						else 
						{
							for (unsigned int j=0; j<strlen(productversion); j++) 
								if (productversion[j] == ',') productversion[j] = '.';
	                       
							char * output = strchr(line, ',');
							if (!output) 
								output = line + strlen(line);
							else 
								++output;

							sprintf(output, " \"%s\"", productversion);
							version_cpp << "const char *pszProductVersion = \"" << productversion << "\";" << endl;
							if (verbose) cout << line << "\r\n";
						}
					}
                }

            }
            free(dupl);
        }

        result += line;
        if (!is.eof()) result += "\n";
    }

    // re-write file
    is.close();

    ofstream os(fileName);
    if (os.fail()) {
        cerr << "Cannot write " << fileName << "\r\n";
        return false;
    }

    os.write( &result[0], result.size());

    if (os.fail()) {
        cerr << "Error writing " << fileName << "\r\n";
        return false;
    }

    os.close();
	version_cpp.close();

	// Restore modified date
	hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	b = SetFileTime(hFile, NULL, NULL, &modifyTimeResource);
	CloseHandle(hFile);

	hFile = CreateFile(versionFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	b = SetFileTime(hFile, NULL, NULL, &modifyTimeVersion);
	CloseHandle(hFile);

    return true;
}



int main(int argc, char* argv[])
{
    if (argc == 1) {
        cerr << strUsage;
        return 3;
    }

    for(int i=1; i<argc; ++i) {
        if (!ParseArg(argv[i]))
            return 3;
    }

	if (onlyupdate && scriptFile && versionFile)
	{
		FILETIME now;
		GetSystemTimeAsFileTime(&now);

		HANDLE hFile = CreateFile(scriptFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		BOOL b = SetFileTime(hFile, NULL, NULL, &now);
		CloseHandle(hFile);

		//hFile = CreateFile(versionFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		//b = SetFileTime(hFile, NULL, NULL, &now);
		//CloseHandle(hFile);

		return 0;
	}

    if (!scriptFile || (!scriptFileList && !format)) {
        cerr << "No formatting options specified\r\n";
        return 3;
    }

    bool errorOccured = false;

    if (scriptFileList) {
        const int MAXLINELEN = 2048;
        ifstream is(scriptFile);
        char line[MAXLINELEN];

        while (!is.bad() && !is.eof()) {
            is.getline(line, MAXLINELEN);

            if (*line==0 || *line == ';')
                continue;

            if (verbose)
                cout << line << "\r\n";

            char * eqpos = strchr(line, '=');
            char const * fmt = format;

            if (eqpos) {        // '=' found
                *eqpos = 0;     // Null-terminate the file name
                fmt = eqpos + 1;
            }
            ProcessFile(line, fmt);
        }
    }
    else {
        errorOccured = ProcessFile(scriptFile, NULL);
    }

	return 0;
}