// Project :  Half-Life Advanced Effects
// File    :  gui/demo_fix.cpp

// Authors : last change / first change / name

// 2008-03-15 / 2008-03-14 / Dominik Tugend

// Comment: see gui/demo_fix.h

///////////////////////////////////////////////////////////////////////////////

#include <wx/file.h>
#include <wx/progdlg.h>

#include <queue>

#include "../debug.h"

#include "demo_fix.h"

// char         :  8 bit (signed)
// unsigned int : 32 bit

const char HLDEMO_MAGIC[8] = { 'H','L','D','E','M','O','\0','\0' };
const unsigned int HLDEMO_DEMO_VERSION = 5;
const unsigned int HLDEMO_NETWORK_VERSION = 47;


#pragma pack(push)
#pragma pack(1)
	struct hldemo_header_s
	{
		char magic[8];
		unsigned int demo_version;
		unsigned int network_version;
		char map_name[260];
		char game_dll[264];
		unsigned int dir_offset;
	};
#pragma pack(pop)

const size_t HLDEMO_HEADER_SIZE = sizeof(hldemo_header_s);

#pragma pack(push)
#pragma pack(1)
	struct hldemo_dir_entry_s
	{
		unsigned int number;
		char title[64];
		unsigned int flags;
		unsigned int play;
		float time;
		unsigned int frames;
		unsigned int offset;
		unsigned int length;
	};
#pragma pack(pop)

const size_t HLDEMO_DIR_ENTRY_SIZE = sizeof(hldemo_dir_entry_s);

#pragma pack(push)
#pragma pack(1)
	struct hldemo_macroblock_header_s
	{
		unsigned char type;
		float time;
		unsigned int frame;
	};
#pragma pack(pop)

const size_t HLDEMO_MACROBLOCK_HEADER_SIZE = sizeof(hldemo_macroblock_header_s);

bool compare_bytes (const char *bytes1, const char *bytes2, size_t ilen)
{
	while (ilen--) if (*(bytes1++)!=*(bytes2++)) return false;
	return true;
}

CHlaeDemoFix::CHlaeDemoFix()
{
}

CHlaeDemoFix::~CHlaeDemoFix()
{
}

bool CHlaeDemoFix::fix_demo ( const wxChar* infilename, const wxChar* outfilename)
{
	wxString tempstr;
	bool bOK=false;

	wxFile *infile = new wxFile(infilename,wxFile::read);
	wxFile *outfile = new wxFile(outfilename, wxFile::write);
	wxProgressDialog* tickerdlg = new wxProgressDialog(wxT("DemoFix"),wxT("Starting ..."),(int)(infile->Length()),NULL,wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME);

	if (infile->IsOpened() && outfile->IsOpened())
	{
		wxString tstr;
		hldemo_header_s * header = new hldemo_header_s;

		g_debug.SendMessage(wxT("DemoFix: reading demo header ..."),hlaeDEBUG_VERBOSE_LEVEL1);

		if (read_header(infile,header))
		{
			if(header->dir_offset)
			{
				g_debug.SendMessage(wxT("DemoFix: Directory entries already present, ignoring."),hlaeDEBUG_WARNING);
				header->dir_offset = 0;
			}

			if (write_header(outfile,header))
			{
				bool bSearchNewSegment=true;
				int iNumSegments=0;
				copy_macroblock_e eCopyState;

				std::queue<hldemo_dir_entry_s> direntries_que;
				
				while (bSearchNewSegment)
				{
					tstr.Printf(wxT("Scanning for segment %i"),iNumSegments);
					g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					tstr.Printf(wxT("Segment: %i"),iNumSegments);
					tickerdlg->Update(infile->Tell(),tstr);

					unsigned int segmentoffset=infile->Tell();
					unsigned int totalframes=0;
					float starttime=0;
					hldemo_macroblock_header_s lastheader = { 5, 0.0f, 0 };

					eCopyState = copy_macroblock(infile, outfile, &lastheader);

					if(eCopyState==CPMB_OK || eCopyState==CPMB_OKSTOP)
						starttime=lastheader.time;

					while (eCopyState==CPMB_OK)
					{
						totalframes++;
						if (! ( tickerdlg->Update(infile->Tell()) ) )
						{
							eCopyState=CPMB_USERABORT;
							break;
						}
						eCopyState = copy_macroblock(infile, outfile, &lastheader);
					}
					
					if (eCopyState==CPMB_OKSTOP)
					{
						// normal stop

						// munch repeated stops:
						wxFileOffset fpos = infile->Tell();
						hldemo_macroblock_header_s mbheader;

						while (HLDEMO_MACROBLOCK_HEADER_SIZE==infile->Read(&mbheader,HLDEMO_MACROBLOCK_HEADER_SIZE) && mbheader.type == 5)
						{
							lastheader = mbheader;
							totalframes++;
							fpos += HLDEMO_MACROBLOCK_HEADER_SIZE;
							outfile->Write(&mbheader,HLDEMO_MACROBLOCK_HEADER_SIZE);
						}

						// rewind behind last stop:
						infile->Seek(fpos);

					} else if (eCopyState==CPMB_ERROR)
					{
						// some error, we drop the rest and add the missing stop:
						if (totalframes>0)
						{
							g_debug.SendMessage(wxT("DemoFix: Found incomplete or unknown message or file end. Asuming file end, finishing segment (appending STOP), dropping rest."),hlaeDEBUG_VERBOSE_LEVEL1);
							hldemo_macroblock_header_s mbheader;

							mbheader.type = 0x05;
							mbheader.frame = totalframes;
							mbheader.time = lastheader.time;
							outfile->Write(&mbheader,HLDEMO_MACROBLOCK_HEADER_SIZE);
							totalframes++;
						} else g_debug.SendMessage(wxT("DemoFix: Found incomplete or unknown message or file end. Asuming file end, ignoring segment (was empty), dropping rest."),hlaeDEBUG_VERBOSE_LEVEL1);
					} else if (eCopyState==CPMB_USERABORT) {
						// fatal error (user abort)
						g_debug.SendMessage(wxT("DemoFix: User aborted."),hlaeDEBUG_ERROR);
					} else {
						// fatal error
						g_debug.SendMessage(wxT("DemoFix: Cannot recover from last error, failed."),hlaeDEBUG_ERROR);
					}

					if ((eCopyState==CPMB_OKSTOP || eCopyState==CPMB_ERROR)&&totalframes>0)
					{
						// add to directory entry
						hldemo_dir_entry_s direntry;
						direntry.number = iNumSegments;
						if (iNumSegments==0) strcpy(direntry.title,"LOADING"); else strcpy(direntry.title,"Playback");
						direntry.flags = 0;
						direntry.play = 0x0FF;
						direntry.time = lastheader.time - starttime;
						direntry.frames = totalframes;
						direntry.offset = segmentoffset;
						direntry.length = infile->Tell() - segmentoffset;

						direntries_que.push(direntry);
						iNumSegments++;
					}

					if (eCopyState!=CPMB_OKSTOP)
						bSearchNewSegment=false;
				}

				// no new segments, create directory entries (if any):
				if (eCopyState==CPMB_OKSTOP || eCopyState==CPMB_ERROR)
				{
					tstr.Printf(wxT("Building %i directory entries"),iNumSegments);
					tickerdlg->Update(infile->Tell(),tstr);
					unsigned int fdiroffset = outfile->Tell(); // remember offset of directory entries

					tstr.Printf(wxT("DemoFix: building directroy entries for %i segments ..."),iNumSegments);
					g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					outfile->Write(&iNumSegments,sizeof(iNumSegments)); // write number of directroy entries;

					// spew out directory entries (if any):
					while (iNumSegments>0)
					{

						hldemo_dir_entry_s curentry = direntries_que.front();
						direntries_que.pop();
						iNumSegments--;

						outfile->Write(&curentry,HLDEMO_DIR_ENTRY_SIZE);
					}

					// patch dir_offset in header:
					outfile->Seek(HLDEMO_HEADER_SIZE-sizeof(unsigned int));
					outfile->Write(&fdiroffset,sizeof(unsigned int));

					// that's it guys.
					g_debug.SendMessage(wxT("DemoFix: Finished."),hlaeDEBUG_INFO);
					bOK=true;
				}

			} else g_debug.SendMessage(wxT("DemoFix: Failed to write demo header."),hlaeDEBUG_ERROR);

		} else g_debug.SendMessage(wxT("DemoFix: Failed to read demo header."),hlaeDEBUG_ERROR);

		delete header;

	} else {
		if (!(infile->IsOpened())) g_debug.SendMessage(wxT("DemoFix: Could not open input file."),hlaeDEBUG_ERROR);
		if (!(outfile->IsOpened())) g_debug.SendMessage(wxT("DemoFix: Could not open output file."),hlaeDEBUG_ERROR);
	}

	delete infile;
	delete outfile;
	
	tickerdlg->Destroy();
	delete tickerdlg;

	return bOK;
}

bool CHlaeDemoFix::read_header(wxFile* infile, hldemo_header_s * header)
{
	wxString tstr;
	size_t iread;

	//iread = sizeof (hldemo_header_s);
	//if (iread != infile->Read(header,iread)) return false;

	iread = sizeof (header->magic);
	if (iread != infile->Read(header->magic,iread)) return false;
	if (strnlen(header->magic,iread)>=iread) { g_debug.SendMessage(wxT("file identifier malformed"),hlaeDEBUG_ERROR); return false; }
	if (!compare_bytes(header->magic,HLDEMO_MAGIC,iread)) { g_debug.SendMessage(wxT("file identifier invalid"),hlaeDEBUG_ERROR); return false; }
	tstr.Printf(wxT("File identifier: %s"),wxString(header->magic,wxConvUTF8)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	iread = sizeof(header->demo_version);
	if (iread != infile->Read(&(header->demo_version),iread)) return false;
	if (HLDEMO_DEMO_VERSION != header->demo_version)
	{
		tstr.Printf(wxT("Demo version is: %u, but I expected %u. (ignoring)"),header->demo_version,HLDEMO_DEMO_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		tstr.Printf(wxT("Demo version: %u"),header->demo_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	iread = sizeof(header->network_version);
	if (iread != infile->Read(&(header->network_version),iread)) return false;
	if (HLDEMO_NETWORK_VERSION != header->network_version)
	{
		tstr.Printf(wxT("Network version is: %u, but I expected %u. (ignoring)"),header->network_version,HLDEMO_NETWORK_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		tstr.Printf(wxT("Network version: %u"),header->network_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	iread = sizeof(header->map_name);
	if (iread != infile->Read(header->map_name,iread)) return false;
	if (strnlen(header->map_name,iread)>=iread) {
		g_debug.SendMessage(wxT("map_name string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->map_name[sizeof(header->map_name)-1]=0;
	}
	tstr.Printf(wxT("Map name: %s"),wxString(header->map_name,wxConvUTF8)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	iread = sizeof(header->game_dll);
	if (iread != infile->Read(header->game_dll,iread)) return false;
	if (strnlen(header->game_dll,iread)>=iread) {
		g_debug.SendMessage(wxT("game_dll string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->game_dll[sizeof(header->game_dll)-1]=0;
	}
	tstr.Printf(wxT("Game DLL: %s"),wxString(header->game_dll,wxConvUTF8)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	iread = sizeof(header->dir_offset);
	if (iread != infile->Read(&(header->dir_offset),iread)) return false;
	return true;
}

bool CHlaeDemoFix::write_header(wxFile* outfile, const hldemo_header_s * header)
{
	size_t iwrite;

	iwrite = sizeof(header->magic);
	if (iwrite != outfile->Write(header->magic,iwrite)) return false;

	iwrite = sizeof(header->demo_version);
	if (iwrite != outfile->Write(&(header->demo_version),iwrite)) return false;
	
	iwrite = sizeof(header->network_version);
	if (iwrite != outfile->Write(&(header->network_version),iwrite)) return false;

	iwrite = sizeof(header->map_name);
	if (iwrite != outfile->Write(header->map_name,iwrite)) return false;

	iwrite = sizeof(header->game_dll);
	if (iwrite != outfile->Write(header->game_dll,iwrite)) return false;

	iwrite = sizeof(header->dir_offset);
	if (iwrite != outfile->Write(&(header->dir_offset),iwrite)) return false;

	return true;
}

bool CHlaeDemoFix::copy_bytes(wxFile* infile, wxFile* outfile, size_t cbCopy)

{
	#define BUFFER_SIZE 1024

	bool bRes = true;
	char *pdata = new char[BUFFER_SIZE];
	size_t readbytes;
	
	while (cbCopy)
	{
		if (cbCopy<BUFFER_SIZE) readbytes=cbCopy;
		else readbytes = BUFFER_SIZE;

		if ( readbytes != infile->Read(pdata,readbytes) )
		{
			g_debug.SendMessage(wxT("error when reading from input"),hlaeDEBUG_ERROR);
			bRes=false;
			break;
		}

		if ( readbytes != outfile->Write(pdata,readbytes ) )
		{
			g_debug.SendMessage(wxT("error when writing to output"),hlaeDEBUG_ERROR);
			bRes=false;
			break;
		}

		cbCopy -= readbytes;
	}

	delete pdata;
	return bRes;
}

CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_macroblock(wxFile* infile, wxFile* outfile, hldemo_macroblock_header_s* pblock_header)
{
	copy_macroblock_e bReturn=CPMB_OK;

	unsigned int dwreadbytes;
	
	wxFileOffset fsize;
	wxFileOffset fpos;
	wxFileOffset ftarget;

	hldemo_macroblock_header_s macroblock_header;

	// let's define two things using the tools of the devil (macros):

	#define COPY_BYTES(numofbytes) \
		copy_bytes((infile->Seek(fpos),infile),outfile,numofbytes)

	#define RETURN_REWIND(returnthis) \
		( \
			wxInvalidOffset==infile->Seek(fpos) ? CPMB_FATALERROR : returnthis \
		)

	#define RETURN_BLOCK(returnthis) \
		( \
		( pblock_header ? (void)(*pblock_header=macroblock_header) : (void)0 ), returnthis \
		)

	#if 0
		#pragma message("warning: CHlaeDemoFix::copy_macroblock_e: DebugMessages enabled, this can be extremely slow!")
		wxString tstr;
		#define DEBUG_MESSAGE(thismsg) \
		tstr.Printf(wxT("0x%08x: %s"),(unsigned int)fpos,wxString(wxT(thismsg))); \
			g_debug.SendMessage(tstr, hlaeDEBUG_DEBUG);
	#else
		#define DEBUG_MESSAGE(thismsg)
	#endif

	// get filesize and backup filepos:
	fsize=infile->Length();
	fpos=infile->Tell();

	// read macroblock_header:
	int iread = HLDEMO_MACROBLOCK_HEADER_SIZE;
	if (iread != infile->Read((char *)&macroblock_header,iread)) return RETURN_REWIND(CPMB_ERROR);

	// parse macroblock:
	switch (macroblock_header.type)
	{
	case 0:
	case 1:
		DEBUG_MESSAGE("0/1 game data")

		if(wxInvalidOffset==infile->Seek(464,wxFromCurrent)) return RETURN_REWIND(CPMB_ERROR);
		if (4!=infile->Read(&dwreadbytes,4)) return RETURN_REWIND(CPMB_ERROR);
		if ((ftarget = infile->Tell()+dwreadbytes) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 2:
		DEBUG_MESSAGE("2 unknown empty")

		ftarget = infile->Tell();
		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 3:
		DEBUG_MESSAGE("3 client command")


		if((ftarget = infile->Tell()+64) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 4:
		DEBUG_MESSAGE("4 unknown data")

		if((ftarget=infile->Tell()+32) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 5:
		DEBUG_MESSAGE("5 last in segment")

		ftarget = infile->Tell();
		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OKSTOP);
	case 6:
		DEBUG_MESSAGE("6 unknown")

		if((ftarget=infile->Tell()+84) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 7:
		DEBUG_MESSAGE("7 unknown")

		if((ftarget=infile->Tell()+8) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 8:
		DEBUG_MESSAGE("8 sound")

		if(wxInvalidOffset==infile->Seek(4,wxFromCurrent)) return RETURN_REWIND(CPMB_ERROR);
		if (4!=infile->Read(&dwreadbytes,4)) return RETURN_REWIND(CPMB_ERROR);
		if (wxInvalidOffset==infile->Seek(dwreadbytes,wxFromCurrent)) return RETURN_REWIND(CPMB_ERROR);
		if ((ftarget = infile->Tell()+16) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 9:
		DEBUG_MESSAGE("9 dynamic length data")

		if (4!=infile->Read(&dwreadbytes,4)) return RETURN_REWIND(CPMB_ERROR);
		if ((ftarget = infile->Tell()+dwreadbytes) > fsize) return RETURN_REWIND(CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	}

	// unsupported block type
	DEBUG_MESSAGE("unknown block type")

	// return OK:
	return RETURN_REWIND(CPMB_ERROR);
}