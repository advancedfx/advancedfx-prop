#ifndef HLAE_DEMO_FIX_H
#define HLAE_DEMO_FIX_H

// Project :  Half-Life Advanced Effects
// File    :  gui/demo_fix.cpp

// Authors : last change / first change / name

// 2008-03-15 / 2008-03-15 / Dominik Tugend

// Comment: classes that help fixing and manipulating demos.

///////////////////////////////////////////////////////////////////////////////

#include <wx/file.h>

// forward declerations:
struct hldemo_header_s;
struct hldemo_macroblock_header_s;


class CHlaeDemoFix
{
public:
	CHlaeDemoFix();
	~CHlaeDemoFix();

	bool fix_demo ( const wxChar* infilename, const wxChar* outfilename);
	// call this to fix a demo that is broken due to missing or malformed directory entries (i.e. the pc crashed during recording)
	// infilename: file to read from
	// outfilename: filte to write to

private:
	enum copy_macroblock_e
	{
		CPMB_OK=0,			// copy ok
		CPMB_OKSTOP,		// copy ok, last block signaled stop
		CPMB_ERROR,			// last block errorenous, no bytes copied
		CPMB_USERABORT,		// user aborted operation
		CPMB_FATALERROR		// fatal error, can not be recovered
	};

	bool read_header(wxFile* infile, hldemo_header_s * header);
	// reads the demo header from a demo file:
	// infile - file to read from, on fail file position is undefined
	// header - pointer on header structure to read to, on fail content is undefined
	// returns: true on success, false on fail

	bool write_header(wxFile* outfile, const hldemo_header_s * header);
	// writes a demo header to outfile.
	// outfile - file to write to, on fail file position is undefined
	// header - header that shall be written
	// returns: true on success, false on fail

	bool copy_bytes(wxFile* infile, wxFile* outfile, size_t cbCopy);
	// copy bytes instantly, so it should be checked before reading if there are enough bytes available!
	// infile - file to read from, on fail file position is undefined
	// outfile - file to write to, on fail file position is undefined
	// cbCopy - number of bytes to copy
	// returns true on success, false on fail

	copy_macroblock_e CHlaeDemoFix::copy_macroblock(wxFile* infile, wxFile* outfile, hldemo_macroblock_header_s* pblock_header = NULL);
	// tires to read a macroblock with best effort
	// infile - file to read from, on CPMB_FATALERROR file position is undefined, otherwise it's the same as before
	// outfile - file to write to, on CPMB_FATALERROR file position is undefined, otherwise it's the same as before
	// pblockheader - if this is not null and the return is CPMB_OK or CPMBO_OKSTOP, the funtion will return a copy of the last read macro_block_header, otherwise the pointed structure stays untouched.
	// returns:
	//	CPMB_OK - block read, no problems
	//	CPMB_OKSTOP - stop block (0x05) read, no problems
	//  CPMB_ERROR - error when reading from file (no more data, malformed block, ...) , successfully rewinded to last valid position (the one when the function was called), suggesting to do no further reads
	//	CPMB_FATALERROR - s.th. went wrong (read error, write error, ...) and it cannot be recovered, the output file shall be asumed to be broken.
};

#endif