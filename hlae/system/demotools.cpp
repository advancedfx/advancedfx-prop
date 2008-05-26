#include <stdafx.h>

// Project :  Half-Life Advanced Effects
// File    :  gui/demo_fix.cpp

// Authors : last change / first change / name

// 2008-05-23 / 2008-03-14 / Dominik Tugend

// Comment: see gui/demo_fix.h

///////////////////////////////////////////////////////////////////////////////

#include "demotools.h"
#include <forms/ProgressDialog.h>
#include <tchar.h>
#include <shared/hldemo/hldemo_clr.h>


using namespace hlae;

bool compare_bytes (const char *bytes1, const char *bytes2, size_t ilen)
{
	while (ilen--) if (*(bytes1++)!=*(bytes2++)) return false;
	return true;
}

CHlaeDemoFix::CHlaeDemoFix(System::Windows::Forms::Form ^parent)
{
	_parent = parent;

	_bEnableDirectoryFix = false;
	_bEnableDemoCleanUp = false;
	_bEnableHltvFix = false;
	_bEnableWaterMarks = true;

	int i=0;

	_watermark64 = gcnew array<unsigned char>(64);
	System::String ^tstr = gcnew System::String(HLAE_WATERMARK64);
	for (i=0;i<64-1;i++)
		_watermark64[i] = (char)(tstr[i]);
	delete tstr;
	_watermark64[i]=0; //

	_watermark260 = gcnew array<unsigned char>(260);
	System::String ^tstr2 = gcnew System::String(HLAE_WATERMARK260);
	for (i=0;i<260-1;i++)
		_watermark260[i] = (char)(tstr2[i]);
	delete tstr2;
	_watermark260[i]=0;
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

void CHlaeDemoFix::AddCommandMapping ( System::String ^srcmap, System::String ^targetmap)
{
	cmd_mapping_s ^mymapping = gcnew cmd_mapping_s;

	for (int i=0;i<64-1;i++)
	{
		if (i>=srcmap->Length)
		{
			mymapping->src[i]=0; // terminate string
			break;
		}

		mymapping->src[i] = (char)(srcmap[i]);
	}

	for (int i=0;i<64-1;i++)
	{
		if (i>=targetmap->Length)
		{
			mymapping->dst[i]=0; // terminate string
			break;
		}

		mymapping->dst[i] = (char)(targetmap[i]);
	}

	_CommandMap.AddFirst(mymapping);
}

void CHlaeDemoFix::ClearCommandMap( void )
{
	_CommandMap.Clear();
}

bool CHlaeDemoFix::Run ( System::String ^infilename, System::String ^outfilename)
{
	_ucHltvFixBell = 2;

	if (_bEnableDirectoryFix)
		return fix_demo(infilename,outfilename);
	else
		return normal_demo(infilename,outfilename);
}

bool CHlaeDemoFix::normal_demo ( System::String ^infilename, System::String ^outfilename )
{
	bool bOK=false;

	System::IO::BinaryReader ^infile;
	System::IO::BinaryWriter ^outfile;

	try
	{
		infile = gcnew System::IO::BinaryReader( System::IO::File::Open( infilename, System::IO::FileMode::Open, System::IO::FileAccess::Read ) );
	} catch (System::Exception ^e)
	{
		System::Console::WriteLine("Exception when opening inputfile: {0}",e);
		return false;
	}

	try
	{
		outfile = gcnew System::IO::BinaryWriter( System::IO::File::Open( outfilename, System::IO::FileMode::Create, System::IO::FileAccess::Write ) );
	}
	catch (System::Exception ^e)
	{
		System::Console::WriteLine("Exception when opening outputfile: {0}",e);
		delete infile;
		return false;
	}

	//wxProgressDialog* tickerdlg = new wxProgressDialog(_T("DemoTools"),_T("Starting ..."),(int)(infile->Length()),NULL,wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME);

	if (
		infile->BaseStream->CanRead && infile->BaseStream->CanSeek
		&& outfile->BaseStream->CanWrite && outfile->BaseStream->CanSeek // seek required(?)
	)
	{
		//wxString tstr;
		hldemo_header_s ^header = gcnew hldemo_header_s; 

		//g_debug.SendMessage(_T("reading demo header ..."),hlaeDEBUG_VERBOSE_LEVEL1);

		if (read_header(infile,header))
		{
			if (_bEnableWaterMarks) watermark_header(header);

			if(!(header->dir_offset))
			{
				//g_debug.SendMessage(_T("Directory entries not present, aborting. Use DemoFix."),hlaeDEBUG_ERROR);
			}
			else if (write_header(outfile,header))
			{

				bool bSearchNewSegment = true && (infile->BaseStream->Position < header->dir_offset);
				int iNumSegments = 0;
				copy_macroblock_e eCopyState=copy_macroblock_e::CPMB_OKSTOP;



				while (bSearchNewSegment)
				{
					//tstr.Printf(_T("Processing segment %i"),iNumSegments);
					//g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					//tstr.Printf(_T("Segment: %i"),iNumSegments);
					//tickerdlg->Update(infile->Tell(),tstr);
					//tickerdlg->TempSet((100*infile->BaseStream->Position)/infile->BaseStream->Length);

					hldemo_macroblock_header_s ^lastheader = gcnew hldemo_macroblock_header_s();
					lastheader->type = 5;
					lastheader->time = 0.0f;
					lastheader->frame = 0;

					eCopyState = copy_macroblock(infile, outfile, lastheader);

					while (eCopyState==copy_macroblock_e::CPMB_OK && (infile->BaseStream->Position < header->dir_offset))
					{
						//if (! ( tickerdlg->Update(infile->BaseStream->Position) ) )
						//{
						//	eCopyState=CPMB_USERABORT;
						//	break;
						//}
						eCopyState = copy_macroblock(infile, outfile, lastheader);
					}
					
					if (eCopyState==copy_macroblock_e::CPMB_OKSTOP)
					{
						// normal stop

						// munch repeated stops:
						long long fpos = infile->BaseStream->Position;
						hldemo_macroblock_header_s ^mbheader = gcnew hldemo_macroblock_header_s();

						while ( (infile->BaseStream->Position < header->dir_offset) && mbheader->read( infile ) && mbheader->type == 5 )
						{
							lastheader = mbheader;
							fpos = infile->BaseStream->Position;
							mbheader->write( outfile );
						}

						// rewind behind last stop:
						infile->BaseStream->Seek( fpos, System::IO::SeekOrigin::Begin );

					} else if (eCopyState==copy_macroblock_e::CPMB_ERROR)
					{
						//g_debug.SendMessage(_T("DemoFix: Found incomplete or unknown message or file end, aborting. Use DemoFix."),hlaeDEBUG_ERROR);
					} else if (eCopyState==copy_macroblock_e::CPMB_USERABORT) {
						// fatal error (user abort)
						//g_debug.SendMessage(_T("DemoFix: User aborted."),hlaeDEBUG_ERROR);
					} else {
						// fatal error
						//g_debug.SendMessage(_T("DemoFix: Cannot recover from last error, failed."),hlaeDEBUG_ERROR);
					}

					if (eCopyState==copy_macroblock_e::CPMB_OKSTOP)
					{
						iNumSegments++;
					}

					bSearchNewSegment = (eCopyState == copy_macroblock_e::CPMB_OKSTOP) && (infile->BaseStream->Position < header->dir_offset);
				}

				// no new segments, copy directory entries (if any):
				if (eCopyState==copy_macroblock_e::CPMB_OKSTOP)
				{
					//tstr.Printf(_T("copying %i directory entries"),iNumSegments);
					//tickerdlg->Update(infile->BaseStream->Position,tstr);
					//g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					if(copy_bytes(infile,outfile,infile->BaseStream->Length - infile->BaseStream->Position))
					{
						// that's it guys.
						//g_debug.SendMessage(_T("DemoTools: Finished."),hlaeDEBUG_INFO);
						//tickerdlg->Update(infile->Length());
						bOK=true;
					};
				}

			} //else g_debug.SendMessage(_T("Failed to write demo header."),hlaeDEBUG_ERROR);

		} //else g_debug.SendMessage(_T("Failed to read demo header."),hlaeDEBUG_ERROR);

		delete header;

	} else {
		//if (!(infile->CanRead && infile->CanSeek)) g_debug.SendMessage(_T("Could not open input file."),hlaeDEBUG_ERROR);
		//if (!(outfile->CanWrite && outfile->CanSeek)) g_debug.SendMessage(_T("Could not open output file."),hlaeDEBUG_ERROR);
	}

	delete infile;
	delete outfile;

	//tickerdlg->Destroy();
	//delete tickerdlg;

	return bOK;
}

bool CHlaeDemoFix::fix_demo ( System::String ^infilename, System::String ^outfilename )
{
	bool bOK=false;

	System::IO::BinaryReader ^infile;
	System::IO::BinaryWriter ^outfile;

	try
	{
		infile = gcnew System::IO::BinaryReader( System::IO::File::Open( infilename, System::IO::FileMode::Open, System::IO::FileAccess::Read ) );
	} catch (System::Exception ^e)
	{
		System::Console::WriteLine("Exception when opening inputfile: {0}",e);
		return false;
	}
	try
	{
		outfile = gcnew System::IO::BinaryWriter( System::IO::File::Open( outfilename, System::IO::FileMode::CreateNew, System::IO::FileAccess::Write ) );
	}
	catch (System::Exception ^e)
	{
		System::Console::WriteLine("Exception when opening outputfile: {0}",e);
		delete infile;
		return false;
	}
	//wxProgressDialog* tickerdlg = new wxProgressDialog(_T("DemoFix"),_T("Starting ..."),(int)(infile->Length()),NULL,wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME);

	if (
		infile->BaseStream->CanRead && infile->BaseStream->CanSeek
		&& outfile->BaseStream->CanWrite && outfile->BaseStream->CanSeek // seek required(?)
	)
	{
		//wxString tstr;
		hldemo_header_s ^header = gcnew hldemo_header_s();

		//g_debug.SendMessage(_T("DemoFix: reading demo header ..."),hlaeDEBUG_VERBOSE_LEVEL1);

		if (read_header(infile,header))
		{
			if (_bEnableWaterMarks) watermark_header(header);

			if(header->dir_offset)
			{
				//g_debug.SendMessage(_T("DemoFix: Directory entries already present, ignoring."),hlaeDEBUG_WARNING);
				header->dir_offset = 0;
			}

			if (write_header(outfile,header))
			{
				bool bSearchNewSegment=true;
				int iNumSegments=0;
				copy_macroblock_e eCopyState;

				System::Collections::Generic::Queue <hldemo_dir_entry_s ^> direntries_que;
				
				while (bSearchNewSegment)
				{
					//tstr.Printf(_T("Scanning for segment %i"),iNumSegments);
					//g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					//tstr.Printf(_T("Segment: %i"),iNumSegments);
					//tickerdlg->Update(infile->Tell(),tstr);

					unsigned int segmentoffset=infile->BaseStream->Position;
					unsigned int totalframes=0;
					float starttime=0;

					hldemo_macroblock_header_s ^lastheader = gcnew hldemo_macroblock_header_s();
					lastheader->type = 5;
					lastheader->time = 0.0f;
					lastheader->frame = 0;

					eCopyState = copy_macroblock(infile, outfile, lastheader);

					if(eCopyState==copy_macroblock_e::CPMB_OK || eCopyState==copy_macroblock_e::CPMB_OKSTOP)
						starttime=lastheader->time;

					while (eCopyState==copy_macroblock_e::CPMB_OK)
					{
						totalframes++;
						//if (! ( tickerdlg->Update(infile->BaseStream->Position) ) )
						//{
						//	eCopyState=CPMB_USERABORT;
						//	break;
						//}
						eCopyState = copy_macroblock(infile, outfile, lastheader);
					}
					
					if (eCopyState==copy_macroblock_e::CPMB_OKSTOP)
					{
						// normal stop

						// munch repeated stops:
						long long fpos = infile->BaseStream->Position;
						hldemo_macroblock_header_s ^mbheader = gcnew hldemo_macroblock_header_s();

						while ( mbheader->read( infile ) && mbheader->type == 5)
						{
							lastheader = mbheader;
							totalframes++;
							fpos = infile->BaseStream->Position;
							mbheader->write( outfile );
						}

						// rewind behind last stop:
						infile->BaseStream->Seek( fpos, System::IO::SeekOrigin::Begin );

					} else if (eCopyState==copy_macroblock_e::CPMB_ERROR)
					{
						// some error, we drop the rest and add the missing stop:
						if (totalframes>0)
						{
							//g_debug.SendMessage(_T("DemoFix: Found incomplete or unknown message or file end. Asuming file end, finishing segment (appending STOP), dropping rest."),hlaeDEBUG_VERBOSE_LEVEL1);
							hldemo_macroblock_header_s ^mbheader = gcnew hldemo_macroblock_header_s();

							mbheader->type = 0x05;
							mbheader->frame = totalframes;
							mbheader->time = lastheader->time;
							mbheader->write( outfile );
							totalframes++;
						} //else g_debug.SendMessage(_T("DemoFix: Found incomplete or unknown message or file end. Asuming file end, ignoring segment (was empty), dropping rest."),hlaeDEBUG_VERBOSE_LEVEL1);
					} else if (eCopyState==copy_macroblock_e::CPMB_USERABORT) {
						// fatal error (user abort)
						//g_debug.SendMessage(_T("DemoFix: User aborted."),hlaeDEBUG_ERROR);
					} else {
						// fatal error
						//g_debug.SendMessage(_T("DemoFix: Cannot recover from last error, failed."),hlaeDEBUG_ERROR);
					}

					if ((eCopyState==copy_macroblock_e::CPMB_OKSTOP || eCopyState==copy_macroblock_e::CPMB_ERROR)&&totalframes>0)
					{
						// add to directory entry
						hldemo_dir_entry_s ^direntry = gcnew hldemo_dir_entry_s();
						direntry->number = iNumSegments;
						if (iNumSegments==0)
						{
							int i=0;
							for each(System::Char achar in System::String("LOADING"))
								direntry->title[i++] = (unsigned char)achar;
							for (;i<direntry->title->Length;i++)
								direntry->title[i]=0; // terminate / fill with zero
						}
						else
						{
							int i=0;
							for each(System::Char achar in System::String("Playback"))
								direntry->title[i++] = (unsigned char)achar;
							for (;i<direntry->title->Length;i++)
								direntry->title[i]=0; // terminate / fill with zero
						}
						direntry->flags = 0;
						direntry->play = 0x0FF;
						direntry->time = lastheader->time - starttime;
						direntry->frames = totalframes;
						direntry->offset = segmentoffset;
						direntry->length = infile->BaseStream->Position - segmentoffset;

						direntries_que.Enqueue( direntry );
						iNumSegments++;
					}

					if (eCopyState!=copy_macroblock_e::CPMB_OKSTOP)
						bSearchNewSegment=false;
				}

				// no new segments, create directory entries (if any):
				if (eCopyState==copy_macroblock_e::CPMB_OKSTOP || eCopyState==copy_macroblock_e::CPMB_ERROR)
				{
					//tstr.Printf(_T("Building %i directory entries"),iNumSegments);
					//tickerdlg->Update(infile->Tell(),tstr);
					unsigned int fdiroffset = outfile->BaseStream->Position; // remember offset of directory entries

					//tstr.Printf(_T("DemoFix: building directroy entries for %i segments ..."),iNumSegments);
					//g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

					outfile->Write( iNumSegments ); // write number of directroy entries;

					// spew out directory entries (if any):
					while (iNumSegments>0)
					{

						hldemo_dir_entry_s ^curentry = direntries_que.Dequeue();
						iNumSegments--;

						curentry->write( outfile );
					}

					// patch dir_offset in header:
					outfile->Seek( hldemo_header_s::size-sizeof(unsigned int), System::IO::SeekOrigin::Begin );
					outfile->Write( fdiroffset );

					// that's it guys.
					//g_debug.SendMessage(_T("DemoFix: Finished."),hlaeDEBUG_INFO);
					//tickerdlg->Update(infile->Length());
					bOK=true;
				}

			} //else g_debug.SendMessage(_T("DemoFix: Failed to write demo header."),hlaeDEBUG_ERROR);

		} //else g_debug.SendMessage(_T("DemoFix: Failed to read demo header."),hlaeDEBUG_ERROR);

		delete header;

	} else {
		//if (!(infile->CanRead && infile->CanSeek)) g_debug.SendMessage(_T("Could not open input file."),hlaeDEBUG_ERROR);
		//if (!(outfile->CanWrite && outfile->CanSeek)) g_debug.SendMessage(_T("Could not open output file."),hlaeDEBUG_ERROR);
	}

	delete infile;
	delete outfile;
	
	//tickerdlg->Destroy();
	//delete tickerdlg;

	return bOK;
}

bool CHlaeDemoFix::read_header( System::IO::BinaryReader ^infile, hldemo_header_s ^header )
{
	int ilen;
	int i;
	bool bOk;

	//wxString tstr;

	if (!(header->read( infile ))) return false;

	bOk=true;
	for (int i=0;i<header->magic->Length;i++)
		bOk = bOk && (header->magic[i] == HLDEMO_MAGIC[i]);
	if (! bOk )
	{
		//g_debug.SendMessage(_T("file identifier invalid"),hlaeDEBUG_ERROR);
		return false;
	}
	//tstr.Printf(_T("File identifier: %s"),wxString(header->magic,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	//iread = sizeof(header->demo_version);
	//if (iread != infile->Read(&(header->demo_version),iread)) return false;
	if (HLDEMO_DEMO_VERSION != header->demo_version)
	{
		//tstr.Printf(_T("Demo version is: %u, but I expected %u. (ignoring)"),header->demo_version,HLDEMO_DEMO_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		//tstr.Printf(_T("Demo version: %u"),header->demo_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	//iread = sizeof(header->network_version);
	//if (iread != infile->Read(&(header->network_version),iread)) return false;
	if (HLDEMO_NETWORK_VERSION != header->network_version)
	{
		//tstr.Printf(_T("Network version is: %u, but I expected %u. (ignoring)"),header->network_version,HLDEMO_NETWORK_VERSION); g_debug.SendMessage(tstr,hlaeDEBUG_WARNING);
	} else {
		//tstr.Printf(_T("Network version: %u"),header->network_version); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);
	}

	ilen=header->map_name->Length;
	bOk=false;
	for (i=0;i<ilen;i++)
		if (header->map_name[i]==0)
		{
			bOk=true;
			break;
		}
	;
	if (!bOk)
	{
		//g_debug.SendMessage(_T("map_name string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->map_name[ilen-1]=0;
	}
	//tstr.Printf(_T("Map name: %s"),wxString(header->map_name,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	ilen=header->game_dll->Length;
	bOk=false;
	for (i=0;i<ilen;i++)
		if (header->game_dll[i]==0)
		{
			bOk=true;
			break;
		}
	;
	if (!bOk)
	{
		//g_debug.SendMessage(_T("game_dll string malformed, forcing term \\0"),hlaeDEBUG_WARNING);
		header->game_dll[ilen-1]=0;
	}
	//tstr.Printf(_T("Game DLL: %s"),wxString(header->game_dll,wxConvISO8859_1)); g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL2);

	//iread = sizeof(header->dir_offset);
	//if (iread != infile->Read(&(header->dir_offset),iread)) return false;
	return true;
}

void CHlaeDemoFix::watermark_header(hldemo_header_s ^header)
{
	bool bMark1=false;
	bool bMark2=false;
	for (int i=0;i<260;i++)
	{
		if (bMark1)
			header->game_dll[i] = _watermark260[i];
		else
			bMark1 = 0 == header->game_dll[i];
		if (bMark2)
			header->map_name[i] = _watermark260[i];
		else
			bMark1 = 0 == header->map_name[i];
	}
}

bool CHlaeDemoFix::write_header( System::IO::BinaryWriter ^outfile, hldemo_header_s ^header )
{
	return header->write( outfile );
}

bool CHlaeDemoFix::copy_bytes( System::IO::BinaryReader ^infile, System::IO::BinaryWriter ^outfile, size_t cbCopy )
{
	if (cbCopy<0)
		return false;

	#define BUFFER_SIZE 1024

	bool bRes = true;
	array<unsigned char> ^pdata;
	int readbytes = BUFFER_SIZE;

	
	while (cbCopy>0)
	{
		try
		{
			if (cbCopy<BUFFER_SIZE) readbytes=(int)cbCopy; 

			pdata = infile->ReadBytes( readbytes );
			outfile->Write( pdata );

			cbCopy -= readbytes;
		} catch ( System::Exception ^e )
		{
			Console::WriteLine( "Generic Exception when copying bytes: {0}", e );
			bRes=false;
			break;
		}
	}

	return bRes;
}

CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_macroblock( System::IO::BinaryReader ^infile, System::IO::BinaryWriter ^outfile, hldemo_macroblock_header_s ^pblock_header )
{
	copy_macroblock_e bReturn=copy_macroblock_e::CPMB_OK;

	unsigned int dwreadbytes;
	
	long long fsize;
	long long fpos;
	long long ftarget;

	hldemo_macroblock_header_s ^macroblock_header=gcnew hldemo_macroblock_header_s();

	// let's define two things using the tools of the devil (macros):

	#define COPY_BYTES(numofbytes) \
		copy_bytes((infile->BaseStream->Seek(fpos, System::IO::SeekOrigin::Begin),infile),outfile,numofbytes)

	#define RETURN_REWIND(returnthis) \
		( \
		fpos < fsize ? infile->BaseStream->Seek(fpos, System::IO::SeekOrigin::Begin), returnthis : copy_macroblock_e::CPMB_FATALERROR \
		)

	#define RETURN_BLOCK(returnthis) \
		( \
		(pblock_header ? (void)(pblock_header=macroblock_header) : (void)0 ), returnthis \
		)

	// #if 0
	// 	#pragma message("warning: CHlaeDemoFix::copy_macroblock_e: DebugMessages enabled, this can be extremely slow!")
	// 	wxString tstr;
	// 	#define DEBUG_MESSAGE(thismsg) \
	// 	tstr.Printf(_T("0x%08x: %s"),(unsigned int)fpos,wxString(_T(thismsg))); \
	// 		g_debug.SendMessage(tstr, hlaeDEBUG_DEBUG);
	// #else
		#define DEBUG_MESSAGE(thismsg)
	// #endif

	// get filesize and backup filepos:
	fsize=infile->BaseStream->Length;
	fpos=infile->BaseStream->Position;

	// read macroblock_header:
	if (! macroblock_header->read( infile ) )
		return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

	// parse macroblock:
	switch (macroblock_header->type)
	{
	case 0:
	case 1:
		DEBUG_MESSAGE("0/1 game data")

		if (infile->BaseStream->Seek(464,System::IO::SeekOrigin::Current) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
		try { infile->ReadUInt32(); } catch ( System::Exception ^) { return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR); }

		if (_bEnableHltvFix)
		{
			if ((ftarget = infile->BaseStream->Position)+dwreadbytes > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
			
			if (copy_gamedata(infile,outfile,dwreadbytes)!= copy_macroblock_e::CPMB_OK) return copy_macroblock_e::CPMB_FATALERROR;
		} else {
			if ((ftarget = infile->BaseStream->Position+dwreadbytes) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		}
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 2:
		DEBUG_MESSAGE("2 unknown empty")

		ftarget = infile->BaseStream->Position;
		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 3:
		DEBUG_MESSAGE("3 client command")

		if (_bEnableDemoCleanUp)
		{
			if((ftarget = infile->BaseStream->Position) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
			if (copy_command(infile,outfile)!=copy_macroblock_e::CPMB_OK) return copy_macroblock_e::CPMB_FATALERROR;
		} else {
			if((ftarget = infile->BaseStream->Position+64) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
			if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		}

		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 4:
		DEBUG_MESSAGE("4 unknown data")

		if((ftarget=infile->BaseStream->Position+32) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 5:
		DEBUG_MESSAGE("5 last in segment")

		ftarget = infile->BaseStream->Position;
		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OKSTOP);
	case 6:
		DEBUG_MESSAGE("6 unknown")

		if((ftarget=infile->BaseStream->Position+84) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 7:
		DEBUG_MESSAGE("7 unknown")

		if((ftarget=infile->BaseStream->Position+8) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 8:
		DEBUG_MESSAGE("8 sound")

		if(infile->BaseStream->Seek(4,System::IO::SeekOrigin::Current) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
		try { dwreadbytes = infile->ReadUInt32(); } catch (System::Exception ^) { return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR); }
		if (infile->BaseStream->Seek(dwreadbytes,System::IO::SeekOrigin::Current) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
		if ((ftarget = infile->BaseStream->Position+16) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	case 9:
		DEBUG_MESSAGE("9 dynamic length data")

		try { dwreadbytes = infile->ReadUInt32(); } catch (System::Exception ^) { return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR); }
		if ((ftarget = infile->BaseStream->Position+dwreadbytes) > fsize) return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);

		if (!COPY_BYTES(ftarget-fpos)) return copy_macroblock_e::CPMB_FATALERROR;
		return RETURN_BLOCK(copy_macroblock_e::CPMB_OK);
	}

	// unsupported block type
	DEBUG_MESSAGE("unknown block type")

	// return OK:
	return RETURN_REWIND(copy_macroblock_e::CPMB_ERROR);
}

CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_command( System::IO::BinaryReader ^infile, System::IO::BinaryWriter ^outfile )
{
	copy_macroblock_e bReturn=copy_macroblock_e::CPMB_OK;
	array<unsigned char> ^stmp;
	
	// read:
	try { stmp = infile->ReadBytes( 64 ); } catch (System::Exception ^)
	{
		//g_debug.SendMessage(_T("failed reading command from demo"),hlaeDEBUG_ERROR);
		return copy_macroblock_e::CPMB_FATALERROR;
	}

	// check in debug mode:
	#ifdef _DEBUG
		//if (strnlen(stmp,64)>=64) g_debug.SendMessage(_T("demo command not null terminated, forcing \\0"),hlaeDEBUG_WARNING);
	#endif

	stmp[63]=0; // force term

	// process mappings:
	for each (cmd_mapping_s ^curmap in _CommandMap)
	{
		bool bEqual = true;
		int ilen = curmap->src->Length;
		for (int i=0;i<ilen;i++)
		{
			bEqual = bEqual && (curmap->src[i] == stmp[i]);
			if (0==stmp[i]) break;
		}
		if (bEqual)
		{
			bool bWaterMark=false;
			for (int i=0;i<stmp->Length;i++)
			{
				if (bWaterMark)
					stmp[i]=_watermark64[i];
				else
					stmp[i]=curmap->dst[i];
				if(0==stmp[i])
					bWaterMark = _bEnableWaterMarks;
			}
			break;
		}
	}

	// write:
	try { outfile->Write( stmp ); } catch (System::Exception ^)
	{
		//g_debug.SendMessage(_T("failed writing command to demo"),hlaeDEBUG_ERROR);
		return copy_macroblock_e::CPMB_FATALERROR;
	}

	return bReturn;
}

void copy_gamedata_ferror(unsigned char cmdcode, size_t initialpos, unsigned int dwreadorg, unsigned int dwreadlast, unsigned int dwreadbytes)
{
	//wxString tstr;

	//tstr.Printf(_T("copy_gamedata failed at 0x%08x in block 0x%08x when parsing cmd %i at 0x%08x"),(unsigned int)initialpos+dwreadorg-dwreadbytes,(unsigned int)initialpos,(unsigned int)cmdcode,(unsigned int)initialpos+dwreadorg-dwreadlast );
	//g_debug.SendMessage(tstr,hlaeDEBUG_ERROR);
}
CHlaeDemoFix::copy_macroblock_e CHlaeDemoFix::copy_gamedata( System::IO::BinaryReader ^infile, System::IO::BinaryWriter ^outfile, unsigned int dwreadbytes )
{
	// for debugging errors:
	//wxString tstr;
	unsigned int dwreadorg = dwreadbytes;
	unsigned int dwreadlast = dwreadbytes;
	size_t initialpos = infile->BaseStream->Position;

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
		try {
			cmdcode = infile->ReadByte();
			outfile->Write( cmdcode );
		} catch (System::Exception ^)
		{
			return PRINT_FERROR(0,copy_macroblock_e::CPMB_FATALERROR);
		}

		dwreadbytes--;

		switch (cmdcode)
		{
		case svc_nop: // 1
			continue;
		
		case svc_time: // 7
			if (dwreadbytes<4) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,4)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=4;
			continue;
		
		case svc_print: // 8
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				try {
					ctmp = infile->ReadByte();
					outfile->Write( ctmp );
				} catch (System::Exception ^)
				{
					return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
				}
				if (ctmp==0) break; // string end, get out of here
			}
			continue;
		
		case svc_serverinfo: // 11
			uitmp = 4 +		4 + 4 + 16;
			if (dwreadbytes<uitmp+1) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,uitmp)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=uitmp;
			
			// now we are at the Holy Grail of maxclients
			try { ctmp = infile->ReadByte(); } catch (System::Exception ^) { return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR); }
			if (ctmp<DEMOFIX_MAXPLAYERS)
			{
				ctmp++; // add a slut slot
				_ucHltvFixBell = 0; // ring the bell happy :)
			} else if (_ucHltvFixBell) _ucHltvFixBell = 1; // ring the bell sad :..(
			try { outfile->Write( ctmp ); } catch (System::Exception ^) { return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR); }

			//tstr.Printf(_T("dem_forcehltv 1 fix: 0x%08x: serverinfo maxplayers: %u -> %u"),(unsigned int)(infile->Tell()),(unsigned int)(ctmp==DEMOFIX_MAXPLAYERS? ctmp : ctmp-1),(unsigned int)ctmp);
			//g_debug.SendMessage(tstr,hlaeDEBUG_VERBOSE_LEVEL1);

			dwreadbytes-=1;
			break; // our work is done here, get us out of here!
		
		case svc_updateuserinfo: // 13
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<5) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,5)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=5;
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				try {
					ctmp = infile->ReadByte();
					outfile->Write( ctmp );
				} catch (System::Exception ^)
				{
					PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
				}
				if (ctmp==0) break; // string end, get out of here
			}
			continue;

		//case svc_deltadescription: // 14
		// i am to lazy to support that now, hope we don't need to parse it before serverinfo : )

		case svc_clientdata: // 15
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<5) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,5)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=5;
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				try {
					ctmp = infile->ReadByte();
					outfile->Write( ctmp );
				} catch (System::Exception ^)
				{
					return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
				}
				if (ctmp==0) break; // string end, get out of here
			}
			continue;

		case svc_spawnstaticsound: // 29
			// deactivated cause s.th. in here is too buggy:
			break; 
			uitmp = 2 +2 +2 +2 +1 +1 +2 +1 +1;
			if (dwreadbytes<uitmp) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,uitmp)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=uitmp;
			continue;

		case svc_roomtype: // 37
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<2) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,2)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=2;
			continue;

		case svc_resourcerequest: // 45
			// deactivated cause s.th. in here is too buggy:
			break; 
			if (dwreadbytes<8) break; // invalid, let copy bytes handle it
			if (!copy_bytes(infile,outfile,8)) return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			dwreadbytes-=8;
			continue;			

		case svc_sendextrainfo: // 54
			// deactivated cause s.th. in here is too buggy:
			break; 
			// Read string:
			while (dwreadbytes>0)
			{
				dwreadbytes--;
				try {
					ctmp = infile->ReadByte();
					outfile->Write( ctmp );
				} catch (System::Exception ^){
					return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
				}
				if (ctmp==0) break; // string end, get out of here
			}
			if (dwreadbytes<1) break; // invalid, let copy bytes handle it
			dwreadbytes--;

			try {
				ctmp = infile->ReadByte();
				outfile->Write( ctmp );
			} catch (System::Exception ^)
			{
				return PRINT_FERROR(cmdcode,copy_macroblock_e::CPMB_FATALERROR);
			}
			continue;
		}

		break; // not handled / unknown, cannot continue
	}

	// copy unhandled data:
	if (dwreadbytes>0 && !copy_bytes(infile,outfile,dwreadbytes)) return PRINT_FERROR(0,copy_macroblock_e::CPMB_FATALERROR);

	return copy_macroblock_e::CPMB_OK;
}