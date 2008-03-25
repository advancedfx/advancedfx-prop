// Project :  Half-Life Advanced Effects
// File    :  gui/demo_fix.cpp

// Authors : last change / first change / name

// 2008-03-18 / 2008-03-14 / Dominik Tugend

// Comment: see gui/demo_fix.h

///////////////////////////////////////////////////////////////////////////////

#include <wx/file.h>
#include <wx/progdlg.h>
#include <wx/string.h>

#include <list>
#include <queue>

#include "../debug.h"
#include "shared/hldemo/hldemo.h"

#include "demo_fix.h"

bool compare_bytes (const char *bytes1, const char *bytes2, size_t ilen)
{
	while (ilen--) if (*(bytes1++)!=*(bytes2++)) return false;
	return true;
}

CHlaeDemoFix::CHlaeDemoFix()
{
	_bEnableDirectoryFix = false;
	_bEnableDemoCleanUp = false;
	_bEnableHltvFix = false;
	_bEnableWaterMarks = true;

	strncpy(_watermark64, HLAE_WATERMARK64, 64);
	_watermark64[63]=0;
	strncpy(_watermark260, HLAE_WATERMARK260, 260);
	_watermark260[259]=0;
}

CHlaeDemoFix::~CHlaeDemoFix()
{
	ClearCommandMap();
}

void CHlaeDemoFix::EnableDirectoryFix(bool bEnable)
{
	_bEnableDirectoryFix = bEnable;
}
void CHlaeDemoFix::EnableDemoCleanUp(bool bEnable)
{
	_bEnableDemoCleanUp = bEnable;
}

void CHlaeDemoFix::EnableHltvFix(bool bEnable)
{
	_bEnableHltvFix = bEnable;
}

void CHlaeDemoFix::EnableWaterMarks(bool bEnable)
{
	_bEnableWaterMarks = bEnable;
}

unsigned char CHlaeDemoFix::GetHltvFixBell()
{
	return _ucHltvFixBell;
}

void CHlaeDemoFix::AddCommandMapping ( wxString & srcmap, wxString & targetmap)
{
	cmd_mapping_s mymapping;
	strncpy(mymapping.src, srcmap.mb_str(wxConvISO8859_1) , sizeof(mymapping.src)-1);
	mymapping.src[sizeof(mymapping.src)-1]=0;
	strncpy(mymapping.dst, targetmap.mb_str(wxConvISO8859_1) , sizeof(mymapping.dst)-1);
	mymapping.dst[sizeof(mymapping.dst)-1]=0;

	_CommandMap.push_front(mymapping);
}

void CHlaeDemoFix::ClearCommandMap( void )
{
	_CommandMap.clear();
}

bool CHlaeDemoFix::Run ( const wxChar* infilename, const wxChar* outfilename)
{
	_ucHltvFixBell = 2;

	if (_bEnableDirectoryFix)
		return fix_demo(infilename,outfilename);
	else
		return normal_demo(infilename,outfilename);
}

bool CHlaeDemoFix::normal_demo ( const wxChar* infilename, const wxChar* outfilename)
{
	wxString tempstr;
	bool bOK=false;

	wxFile *infile = new wxFile(infilename,wxFile::read);
	wxFile *outfile = new wxFile(outfilename, wxFile::write);
	wxProgressDialog* tickerdlg = new wxProgressDialog(wxT("DemoTools"),wxT("Starting ..."),(int)(infile->Length()),NULL,wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME);

	if (infile->IsOpened() && outfile->IsOpened())
	{
		wxString tstr;
		hldemo_header_s * header = new hldemo_header_s;

		g_debug.SendMessage(wxT("reading demo header ..."),hlaeDEBUG_VERBOSE_LEVEL1);

		if (read_header(infile,header))
		{
			if (_bEnableWaterMarks) watermark_header(header);

			if(!(header->dir_offset))
			{
				g_debug.SendMessage(wxT("Directory entries not present, aborting. Use DemoFix."),hlaeDEBUG_ERROR);
			}

			else if (write_header(outfile,header))
			{
				bool bSearchNewSegment = true && (infile->Tell() < header->dir_offset);
				int iNumSegments = 0;
				copy_macroblock_e eCopyState=CPMB_OKSTOP;

				std::queue<hldemo_dir_entry_s> direntries_que;
				
				while (bSearchNewSegment)
				{
					tstr.Printf(wxT("Processing segment %i"),iNumSegments);
					g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					tstr.Printf(wxT("Segment: %i"),iNumSegments);
					tickerdlg->Update(infile->Tell(),tstr);

					hldemo_macroblock_header_s lastheader = { 5, 0.0f, 0 };

					eCopyState = copy_macroblock(infile, outfile, &lastheader);

					while (eCopyState==CPMB_OK && (infile->Tell() < header->dir_offset))
					{
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

						while ( (infile->Tell() < header->dir_offset) &&  sizeof(hldemo_macroblock_header_s)==infile->Read(&mbheader,sizeof(hldemo_macroblock_header_s)) && mbheader.type == 5 )
						{
							lastheader = mbheader;
							fpos += sizeof(hldemo_macroblock_header_s);
							outfile->Write(&mbheader,sizeof(hldemo_macroblock_header_s));
						}

						// rewind behind last stop:
						infile->Seek(fpos);

					} else if (eCopyState==CPMB_ERROR)
					{
						g_debug.SendMessage(wxT("DemoFix: Found incomplete or unknown message or file end, aborting. Use DemoFix."),hlaeDEBUG_ERROR);
					} else if (eCopyState==CPMB_USERABORT) {
						// fatal error (user abort)
						g_debug.SendMessage(wxT("DemoFix: User aborted."),hlaeDEBUG_ERROR);
					} else {
						// fatal error
						g_debug.SendMessage(wxT("DemoFix: Cannot recover from last error, failed."),hlaeDEBUG_ERROR);
					}

					if (eCopyState==CPMB_OKSTOP)
					{
						iNumSegments++;
					}

					bSearchNewSegment = (eCopyState == CPMB_OKSTOP) && (infile->Tell() < header->dir_offset);
				}

				// no new segments, copy directory entries (if any):
				if (eCopyState==CPMB_OKSTOP)
				{
					tstr.Printf(wxT("copying %i directory entries"),iNumSegments);
					tickerdlg->Update(infile->Tell(),tstr);
					g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					if(copy_bytes(infile,outfile,infile->Length()-infile->Tell()))
					{
						// that's it guys.
						g_debug.SendMessage(wxT("DemoTools: Finished."),hlaeDEBUG_INFO);
						tickerdlg->Update(infile->Length());
						bOK=true;
					};
				}

			} else g_debug.SendMessage(wxT("Failed to write demo header."),hlaeDEBUG_ERROR);

		} else g_debug.SendMessage(wxT("Failed to read demo header."),hlaeDEBUG_ERROR);

		delete header;

	} else {
		if (!(infile->IsOpened())) g_debug.SendMessage(wxT("Could not open input file."),hlaeDEBUG_ERROR);
		if (!(outfile->IsOpened())) g_debug.SendMessage(wxT("Could not open output file."),hlaeDEBUG_ERROR);
	}

	delete infile;
	delete outfile;
	
	tickerdlg->Destroy();
	delete tickerdlg;

	return bOK;
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
			if (_bEnableWaterMarks) watermark_header(header);

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

						while (sizeof(hldemo_macroblock_header_s)==infile->Read(&mbheader,sizeof(hldemo_macroblock_header_s)) && mbheader.type == 5)
						{
							lastheader = mbheader;
							totalframes++;
							fpos += sizeof(hldemo_macroblock_header_s);
							outfile->Write(&mbheader,sizeof(hldemo_macroblock_header_s));
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
							outfile->Write(&mbheader,sizeof(hldemo_macroblock_header_s));
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

						outfile->Write(&curentry,sizeof(hldemo_dir_entry_s));
					}

					// patch dir_offset in header:
					outfile->Seek(sizeof(hldemo_header_s)-sizeof(unsigned int));
					outfile->Write(&fdiroffset,sizeof(unsigned int));

					// that's it guys.
					g_debug.SendMessage(wxT("DemoFix: Finished."),hlaeDEBUG_INFO);
					tickerdlg->Update(infile->Length());
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

	iread = sizeof (hldemo_header_s);
	if (iread != infile->Read(header,iread)) return false;

	//iread = sizeof (header->magic);
	//if (iread != infile->Read(header->magic,iread)) return false;
	if (!compare_bytes(header->magic,HLDEMO_MAGIC,sizeof(HLDEMO_MAGIC))) { g_debug.SendMessage(wxT("file identifier invalid"),hlaeDEBUG_ERROR); return false; }
	tstr.Printf(wxT("File identifier: %s"),wxString(header->magic,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	//iread = sizeof(header->demo_version);
	//if (iread != infile->Read(&(header->demo_version),iread)) return false;
	if (HLDEMO_DEMO_VERSION != header->demo_version)
	{
		tstr.Printf(wxT("Demo version is: %u, but I expected %u. (ignoring)"),header->demo_version,HLDEMO_DEMO_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		tstr.Printf(wxT("Demo version: %u"),header->demo_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	//iread = sizeof(header->network_version);
	//if (iread != infile->Read(&(header->network_version),iread)) return false;
	if (HLDEMO_NETWORK_VERSION != header->network_version)
	{
		tstr.Printf(wxT("Network version is: %u, but I expected %u. (ignoring)"),header->network_version,HLDEMO_NETWORK_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		tstr.Printf(wxT("Network version: %u"),header->network_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	iread = sizeof(header->map_name);
	//if (iread != infile->Read(header->map_name,iread)) return false;
	if (strnlen(header->map_name,iread)>=iread) {
		g_debug.SendMessage(wxT("map_name string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->map_name[sizeof(header->map_name)-1]=0;
	}
	tstr.Printf(wxT("Map name: %s"),wxString(header->map_name,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	iread = sizeof(header->game_dll);
	//if (iread != infile->Read(header->game_dll,iread)) return false;
	if (strnlen(header->game_dll,iread)>=iread) {
		g_debug.SendMessage(wxT("game_dll string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->game_dll[sizeof(header->game_dll)-1]=0;
	}
	tstr.Printf(wxT("Game DLL: %s"),wxString(header->game_dll,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	//iread = sizeof(header->dir_offset);
	//if (iread != infile->Read(&(header->dir_offset),iread)) return false;
	return true;
}

void CHlaeDemoFix::watermark_header(hldemo_header_s * header)
{
	char *tdst1;
	char *tdst2;
	char *tmrk=_watermark260;
	bool bmark1=false;
	bool bmark2=false;

	tdst1 = header->game_dll;
	tdst2 = header->map_name;
	for (int i=0;i<260;i++)
	{
		*tdst1 = bmark1 ? *tmrk: *tdst1;
		*tdst2 = bmark2 ? *tmrk: *tdst2;
		if (*(tdst1++) == 0) bmark1=true;
		if (*(tdst2++) == 0) bmark2=true;
		tmrk++;
	}
	
}

bool CHlaeDemoFix::write_header(wxFile* outfile, const hldemo_header_s * header)
{
	size_t iwrite;

	iwrite = sizeof(const hldemo_header_s );
	if (iwrite != outfile->Write(header,iwrite)) return false;

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
	int iread = sizeof(hldemo_macroblock_header_s);
	if (iread != infile->Read((char *)&macroblock_header,iread)) return RETURN_REWIND(CPMB_ERROR);

	// parse macroblock:
	switch (macroblock_header.type)
	{
	case 0:
	case 1:
		DEBUG_MESSAGE("0/1 game data")

		if(wxInvalidOffset==infile->Seek(464,wxFromCurrent)) return RETURN_REWIND(CPMB_ERROR);
		if (4!=infile->Read(&dwreadbytes,4)) return RETURN_REWIND(CPMB_ERROR);

		if (_bEnableHltvFix)
		{
			if ((ftarget = infile->Tell())+dwreadbytes > fsize) return RETURN_REWIND(CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
			
			if (copy_gamedata(infile,outfile,dwreadbytes)!=CPMB_OK) return CPMB_FATALERROR;
		} else {
			if ((ftarget = infile->Tell()+dwreadbytes) > fsize) return RETURN_REWIND(CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		}
		return RETURN_BLOCK(CPMB_OK);
	case 2:
		DEBUG_MESSAGE("2 unknown empty")

		ftarget = infile->Tell();
		if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		return RETURN_BLOCK(CPMB_OK);
	case 3:
		DEBUG_MESSAGE("3 client command")

		if (_bEnableDemoCleanUp)
		{
			if((ftarget = infile->Tell()) > fsize) return RETURN_REWIND(CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
			if (copy_command(infile,outfile)!=CPMB_OK) return CPMB_FATALERROR;
		} else {
			if((ftarget = infile->Tell()+64) > fsize) return RETURN_REWIND(CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return CPMB_FATALERROR;
		}

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

CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_command(wxFile* infile, wxFile* outfile)
{
	copy_macroblock_e bReturn=CPMB_OK;
	char stmp[64];

	// read:
	if (64 != infile->Read(stmp,64))
	{
		g_debug.SendMessage(wxT("failed reading command from demo"),hlaeDEBUG_ERROR);
		return CPMB_FATALERROR;
	}

	// check in debug mode:
	#ifdef _DEBUG
		if (strnlen(stmp,64)>=64) g_debug.SendMessage(wxT("demo command not null terminated, forcing \\0"),hlaeDEBUG_WARNING);
	#endif

	stmp[63]=0; // force term

	// process mappings:
	for (std::list<cmd_mapping_s>::iterator iter = _CommandMap.begin(); iter != _CommandMap.end(); iter++)
	{
		cmd_mapping_s curmap=*iter;

		if (!strcmp(curmap.src,stmp))
		{
			if (_bEnableWaterMarks) strcpy(stmp,_watermark64);
			strcpy(stmp,curmap.dst);
			break;
		}
	}

	// write:
	if (64 != outfile->Write(stmp,64))
	{
		g_debug.SendMessage(wxT("failed writing command to demo"),hlaeDEBUG_ERROR);
		return CPMB_FATALERROR;
	}

	return bReturn;
}

void copy_gamedata_ferror(unsigned char cmdcode, size_t initialpos, unsigned int dwreadorg, unsigned int dwreadlast, unsigned int dwreadbytes)
{
	wxString tstr;

	tstr.Printf(wxT("copy_gamedata failed at 0x%08x in block 0x%08x when parsing cmd %i at 0x%08x"),(unsigned int)initialpos+dwreadorg-dwreadbytes,(unsigned int)initialpos,(unsigned int)cmdcode,(unsigned int)initialpos+dwreadorg-dwreadlast );
	g_debug.SendMessage(tstr,hlaeDEBUG_ERROR);
}
CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_gamedata(wxFile* infile,wxFile* outfile, unsigned int dwreadbytes)
{
	// for debugging errors:
	wxString tstr;
	unsigned int dwreadorg = dwreadbytes;
	unsigned int dwreadlast = dwreadbytes;
	size_t initialpos = infile->Tell();

	unsigned char cmdcode;

	#define PRINT_FERROR(cmdcodethis,returnthis) \
		( \
			 copy_gamedata_ferror(cmdcodethis, initialpos, dwreadorg, dwreadlast, dwreadbytes), returnthis \
		)

	unsigned char ctmp;
	unsigned int uitmp;

	while (dwreadbytes>0)
	{
		dwreadlast = dwreadbytes;

		// get cmd code:
		if (sizeof(cmdcode) != infile->Read(&cmdcode,sizeof(cmdcode))) return PRINT_FERROR(0,CPMB_FATALERROR);
		if (sizeof(cmdcode) != outfile->Write(&cmdcode,sizeof(cmdcode))) return PRINT_FERROR(0,CPMB_FATALERROR);

		dwreadbytes--;

		switch (cmdcode)
		{
		case svc_nop: // 1
			continue;
		
		case svc_time: // 7
			if (dwreadbytes<4) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,4)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=4;
			continue;
		
		case svc_print: // 8
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (ctmp==0) break; // string end, get out of here
			}
			continue;
		
		case svc_serverinfo: // 11
			uitmp = 4 +		4 + 4 + 16;
			if (dwreadbytes<uitmp+1) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,uitmp)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=uitmp;
			
			// now we are at the Holy Grail of maxclients
			if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			if (ctmp<DEMOFIX_MAXPLAYERS)
			{
				ctmp++; // add a slut slot
				_ucHltvFixBell = 0; // ring the bell happy :)
			} else if (_ucHltvFixBell) _ucHltvFixBell = 1; // ring the bell sad :..(
			if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);

			tstr.Printf(wxT("dem_forcehltv 1 fix: 0x%08x: serverinfo maxplayers: %u -> %u"),(unsigned int)(infile->Tell()),(unsigned int)(ctmp==DEMOFIX_MAXPLAYERS? ctmp : ctmp-1),(unsigned int)ctmp);
			g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

			dwreadbytes-=1;
			break; // our work is done here, get us out of here!
		
		case svc_updateuserinfo: // 13
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<5) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,5)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=5;
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (ctmp==0) break; // string end, get out of here
			}
			continue;

		//case svc_deltadescription: // 14
		// i am to lazy to support that now, hope we don't need to parse it before serverinfo : )

		case svc_clientdata: // 15
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<5) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,5)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=5;
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (ctmp==0) break; // string end, get out of here
			}
			continue;

		case svc_spawnstaticsound: // 29
			// deactivated cause s.th. in here is too buggy:
			break; 
			uitmp = 2 +2 +2 +2 +1 +1 +2 +1 +1;
			if (dwreadbytes<uitmp) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,uitmp)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=uitmp;
			continue;

		case svc_roomtype: // 37
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<2) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,2)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=2;
			continue;

		case svc_resourcerequest: // 45
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<8) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,8)) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			dwreadbytes-=8;
			continue;			

		case svc_sendextrainfo: // 54
			// deactivated cause s.th. in here is too buggy:
			break; 
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
				if (ctmp==0) break; // string end, get out of here
			}
			if (dwreadbytes<1) break; // invalid, let copy bytes handle it
			dwreadbytes--;
			if (sizeof(ctmp) != infile->Read(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			if (sizeof(ctmp) != outfile->Write(&ctmp,sizeof(ctmp))) return PRINT_FERROR(cmdcode,CPMB_FATALERROR);
			continue;
		}

		break; // not handled / unknown, cannot continue
	}

	// copy unhandled data:
	if (dwreadbytes>0 && !copy_bytes(infile,outfile,dwreadbytes)) return PRINT_FERROR(0,CPMB_FATALERROR);

	return CPMB_OK;
}