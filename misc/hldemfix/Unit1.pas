unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls;

type
  TForm1 = class(TForm)
    OpenDialog1: TOpenDialog;
    ProgressBar1: TProgressBar;
    Button1: TButton;
    Memo1: TMemo;
    Label1: TLabel;
    SaveDialog1: TSaveDialog;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

const magick : array [0..7] of byte = ($48,$4C,$44,$45,$4D,$4F,$00,$00);

const // length of special arrays
  demoHeader_magick = 8;
  demoHeader_map_name = 260;
  demoHeader_game_dll = 264;
  demoHeaderSize = demoHeader_magick +4 +4 +demoHeader_map_name
                   +demoHeader_game_dll +4;

type
  PdemoHeader = ^TdemoHeader;
  TdemoHeader = record
    magick : array [0..demoHeader_magick-1] of byte;
    demo_version : cardinal;
    network_version : cardinal;
    map_name : array [0..demoHeader_map_name-1] of byte;
    game_dll : array [0..demoHeader_game_dll-1] of byte;
    dir_offset : cardinal;
  end;

const // length of special arrays
  dirSegment_title = 64;
  dirSegmentSize = 4+dirSegment_title+4+4+4+4+4+4;
type
  PdirSegment = ^TdirSegment;
  TdirSegment = record
    number:cardinal;
    title:array[0..dirSegment_title-1] of byte;
    flags:cardinal;
    play:cardinal;
    time:single;
    frames:cardinal;
    offset:cardinal;
    length:cardinal;
  end;

const
  macroBlockHeaderSize = 1+4+4;
type
  TmacroBlockHeader = record
    btype:byte;
    time:single;
    frame:cardinal;
  end;

type PByteArray = ^TByteArray;
type TbyteArray = array of byte;

function byte_compare(p1,p2:PbyteArray;size:byte):boolean;
var i:byte;
begin
  Result:=True;
  for i:=0 to size-1 do
    if byte(p1^[i])<>byte(p2^[i]) then Result:=False;
end;

function bytestostr(p1:PbyteArray;size:cardinal):string;
var i:cardinal;
begin
  Result:='';
  for i:=0 to size-1 do begin
    if p1^[i]=0 then break
                else Result:=Result+chr(p1^[i]);
  end;

end;

procedure strtobytes(str:string;p1:PbyteArray;size:cardinal);
// asumes string is shorter than size!!!
var i:cardinal;
begin
  fillchar(p1^,size,0);
  for i:=1 to Length(str) do p1^[i-1]:=ord(str[i]);

end;



procedure TForm1.Button1Click(Sender: TObject);
var fn1,fn2:string;
    fh1,fh2:file;
    header:PDemoHeader;
    dirsegment:PdirSegment;
    mbheader:TmacroBlockHeader;
    dir_count:cardinal;
    act_dir:cardinal;
    cont,wantedsave:boolean;
    fpos,fsize,mblen,ptmp,ptmpo,fptmp:cardinal;

    segment1_ofs:cardinal;
    segment2_ofs:cardinal;
    segment2_end:cardinal;
    segment2_frames:cardinal;
    segment2_time:single;

    buff:pointer;

begin
  Button1.Enabled:=false;
  Memo1.Lines.Clear;
  Progressbar1.Position:=0;
  ptmp:=0; ptmpo:=0;
  wantedsave:=false;

  try
    if not (OpenDialog1.Execute) then
      raise EAssertionFailed.Create('OpenDialog Failed');
    fn1:=OpenDialog1.FileName;
    assignfile(fh1,fn1);

    reset(fh1,1);

    fpos:=0;
    fsize:=filesize(fh1);

    try
      New(header);
      blockread(fh1,header^,demoHeaderSize);
      Memo1.Lines.Append('reading file ...');
      if not(byte_compare(@(header^.magick),@magick,demoHeader_magick)) then
      begin
        Memo1.Lines.Append('ERROR: Not a HLDEMO file ...');
        raise EAssertionFailed.Create('File Identifier INVALID');
      end;
      Memo1.Lines.Append('File Identifier: '
        +bytestostr(@(header^.magick),demoHeader_magick));
      Memo1.Lines.Append('Demo Version: '+inttostr(header^.demo_version));
      Memo1.Lines.Append('Network Version: '+inttostr(header^.network_version));
      Memo1.Lines.Append('Map Name: '
        +bytestostr(@(header^.map_name),demoHeader_map_name));
      Memo1.Lines.Append('Game DLL: '
        +bytestostr(@(header^.game_dll),demoHeader_game_dll));

      if (header^.demo_version<>5) or (header^.network_version<>47) then
      begin
        Memo1.Lines.Append
          ('WARNING: only Demo Version 5, Network Version 47 supported');
//        raise EAssertionFailed.Create('Wrong demo or network version');
      end;
// ugly hack
if header^.dir_offset<>0 then begin
   header^.dir_offset:=0;
   Memo1.Lines.Append
          ('WARINING: This demo seems to be already fixed, trying anyways ...');
end;

      if header^.dir_offset=0 then begin
        Memo1.Lines.Append('Demo needs to be fixed, analyzing ...');

        // Loading begins directly after header
        // Play segment begins directly after header and ends before dir_offset
        // so we have to go through the blocks in the loading segment to find
        // the play segment

        segment1_ofs:=filepos(fh1);

//        Memo1.Lines.Append('Searching end of first Segment ...');
        cont:=true;
        while (cont) do begin

          ptmp:=trunc((fpos/fsize)*1024);
          if ptmp<>ptmpo then begin
            ProgressBar1.Position:=ptmp; // header, fsize>0
            Application.ProcessMessages;
            ptmpo:=ptmp;
          end;

          blockread(fh1,mbheader,macroBlockHeaderSize);
          fpos:=filepos(fh1);

          case mbheader.btype of
            0..1: begin // game data
//              Memo1.Lines.Append('macro block 0/1, game data');
              fpos:=filepos(fh1)+464;
              seek(fh1,fpos);
              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen;
              seek(fh1,fpos);
            end;
            2: begin // ?
//              Memo1.Lines.Append('macro block 2, empty');
            end;
            3: begin // text
//              Memo1.Lines.Append('macro block 3, text');
              fpos:=fpos+64;
              seek(fh1,fpos);
            end;
            4: begin // data
//              Memo1.Lines.Append('macro block 4, data');
              fpos:=fpos+32;
              seek(fh1,fpos);
            end;
            5: begin // end found
//              Memo1.Lines.Append('macro block 5, end');
              cont:=false;
            end;
            6: begin // unkown
//              Memo1.Lines.Append('macro block 6');
              fpos:=fpos+84;
              seek(fh1,fpos);
            end;
            7: begin // unkown
//              Memo1.Lines.Append('macro block 7');
              fpos:=fpos+8;
              seek(fh1,fpos);
            end;
            8: begin // sound
//              Memo1.Lines.Append('macro block 8, sound');
              fpos:=fpos+4;
              seek(fh1,fpos);

              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen+16;
              seek(fh1,fpos);
            end;
            9: begin // data dyn length
//              Memo1.Lines.Append('macro block 9, varlength data');
              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen;
              seek(fh1,fpos);
            end;
          else
            Memo1.Lines.Append
            ('ERROR: Invalid Block found, can not continue');
            raise EAssertionFailed.Create('Invalid Block');
          end;
        end; // end while

//        Memo1.Lines.Append('Found.');

//        Memo1.Lines.Append('Searching and skipping repeated stops (HL Bug?)');
        cont:=true;
        ptmp:=0;
        while (cont) do begin

          blockread(fh1,mbheader,macroBlockHeaderSize);

          if mbheader.btype=5 then
          begin // end found
            inc(ptmp);
            fpos:=fpos+macroBlockHeaderSize;
          end else cont:=false;
        end; // end while
//        Memo1.Lines.Append('Skipped '+inttostr(ptmp)+' stops');


(*        Memo1.Lines.Append
          ('Found Second Segment at '+inttohex(fpos,8)+' (hex)');
        Memo1.Lines.Append('Analyzing second Segment ...');
*)                          // to determine frames and time

        seek(fh1,fpos); // we are one blockheader to far
        segment2_ofs:=fpos;
        segment2_frames:=0;
        segment2_time:=0;

        fptmp:=fpos;
try
        cont:=true;
        while (cont) do begin

          ptmp:=trunc((fpos/fsize)*1024);
          if ptmp<>ptmpo then begin
            ProgressBar1.Position:=ptmp; // header, fsize>0
//            Memo1.Lines.Append('last time: '+floattostr(mbheader.time));
//            Memo1.Lines.Append('last frames: '+floattostr(mbheader.frame));
            Application.ProcessMessages;
            ptmpo:=ptmp;
          end;

          blockread(fh1,mbheader,macroBlockHeaderSize);
          fpos:=filepos(fh1);

          case mbheader.btype of
            0..1: begin // game data
//              Memo1.Lines.Append('macro block 0/1, game data');
              fpos:=filepos(fh1)+464;
              seek(fh1,fpos);
              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen;
              seek(fh1,fpos);
            end;
            2: begin // ?
//              Memo1.Lines.Append('macro block 2, empty');
            end;
            3: begin // text
//              Memo1.Lines.Append('macro block 3, text');
              fpos:=fpos+64;
              seek(fh1,fpos);
            end;
            4: begin // data
//              Memo1.Lines.Append('macro block 4, data');
              fpos:=fpos+32;
              seek(fh1,fpos);
            end;
            5: begin // end found
//              Memo1.Lines.Append('macro block 5, end');
              cont:=false;
            end;
            6: begin // unkown
//              Memo1.Lines.Append('macro block 6');
              fpos:=fpos+84;
              seek(fh1,fpos);
            end;
            7: begin // unkown
//              Memo1.Lines.Append('macro block 7');
              fpos:=fpos+8;
              seek(fh1,fpos);
            end;
            8: begin // sound
//              Memo1.Lines.Append('macro block 8, sound');
              fpos:=fpos+4;
              seek(fh1,fpos);

              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen+16;
              seek(fh1,fpos);
            end;
            9: begin // data dyn length
//              Memo1.Lines.Append('macro block 9, varlength data');
              blockread(fh1,mblen,4);
              fpos:=fpos+4+mblen;
              seek(fh1,fpos);
            end;
          else
            Memo1.Lines.Append
            ('ERROR: Invalid Block found, can not continue');
            raise EAssertionFailed.Create('Invalid Block');
          end;

          segment2_frames:=segment2_frames+mbheader.frame;
          segment2_time:=segment2_time+mbheader.time;
          if fpos<=fsize then // I dont know why we can read over without except.
            fptmp:=fpos;

        end; // end while
except
  Memo1.Lines.Append('Found incomplete Message, skipping');
end;

        fpos:=fptmp; // restore last successfull position
(*        Memo1.Lines.Append
          ('Found End of Second Segment: '+inttohex(fpos-1,8)+' (hex)');
*)
        Segment2_end:=fpos-1;

// Does not work
(*        Memo1.Lines.Append
          ('Segment2 frames total: '+inttostr(segment2_frames));
       Memo1.Lines.Append
          ('Segment2 time total: '+floattostr(segment2_time)+' (hex)');
*)

        Memo1.Lines.Append('Trying to create fixed demo ...');

        if not (SaveDialog1.Execute) then
          raise EAssertionFailed.Create('SaveDialog Failed');
          fn2:=SaveDialog1.FileName;

      if fn1=fn2 then
      begin
        Memo1.Lines.Append('ERROR: Please select an other file to save to!');
        raise EAssertionFailed.Create('Lfile = Sfile');
      end;

        wantedsave:=true;
        assignfile(fh2,fn2);
        rewrite(fh2,1);

        // write header with direcotry offset +9 for additonal stop macroblock
        header^.dir_offset:=segment2_end+1+9;
        blockwrite(fh2,header^,demoHeaderSize);

        // write till end
        seek(fh1,demoHeaderSize);
        getmem(buff,10*1024);
        fpos:=demoHeaderSize;
        while(fpos <= segment2_end) do begin
          ptmp:=trunc((fpos/fsize)*1024);
          if ptmp<>ptmpo then begin
            ProgressBar1.Position:=ptmp; // header, fsize>0
            Application.ProcessMessages;
            ptmpo:=ptmp;
          end;

          if fpos+10240 < segment2_end then begin
            blockread(fh1,buff^,10240);
            blockwrite(fh2,buff^,10240);
            fpos:=fpos+10240;
          end else begin
              blockread(fh1,buff^,1);
            blockwrite(fh2,buff^,1);
            fpos:=fpos+1;
          end;
        end;
        freemem(buff,1024);

        // write an additional stop macroblock
        mbheader.btype:=5;
        blockwrite(fh2,mbheader,macroBlockHeaderSize);

        // write number of directories
        dir_count:=2;
        blockwrite(fh2,dir_count,4);

        new(dirSegment);

        dirSegment^.number:=0;
        strtobytes('LOADING',@(dirSegment^.title),dirSegment_title);

        fillchar(dirSegment^.flags,4,0);
        fillchar(dirSegment^.play,4,$FF);
        fillchar(dirSegment^.time,4,0);
        fillchar(dirSegment^.frames,4,0);

        dirSegment^.offset:=DemoHeaderSize;
        dirSegment^.length:=segment2_ofs-DemoHeaderSize;

        blockwrite(fh2,dirSegment^,dirSegmentSize);

        dirSegment^.number:=1;
        strtobytes('Playback',@(dirSegment^.title),dirSegment_title);

        dirSegment^.offset:=segment2_ofs;
        dirSegment^.length:=((segment2_end+1)-segment2_ofs)
                             +macroBlockHeadersize;

        blockwrite(fh2,dirsegment^,dirSegmentSize);


        dispose(dirSegment);

        Memo1.Lines.Append('DONE');
      end else begin
(*        Memo1.Lines.Append('Directory Offset: '
          +inttohex(header^.dir_offset,8)+' (hex)');
*)        Memo1.Lines.Append
          ('INFO: This demo is already fixed or can not be fixed.');
(*       Memo1.Lines.Append('Viewing some direcotry infos ...');
        seek(fh1,header^.dir_offset);
        blockread(fh1,dir_count,4);
        Memo1.Lines.Append('Number of direcotries: '+inttostr(dir_count));

        try
          new(dirsegment);
          for act_dir:=0 to dir_count-1 do begin
            blockread(fh1,dirsegment^,dirSegmentSize);
            Memo1.Lines.Append('Directory Segment #'+
                               inttostr(dirsegment^.number));
            Memo1.Lines.Append('title: '
              +bytestostr(@(dirsegment^.title),dirSegment_title));
            Memo1.Lines.Append('flags: '
              +inttohex(dirsegment^.flags,8)+' (hex)');
            Memo1.Lines.Append('play: '
              +inttohex(dirsegment^.play,8)+' (hex)');
            Memo1.Lines.Append('frames: '+inttostr(dirsegment^.frames));
            Memo1.Lines.Append('time: '+floattostr(dirsegment^.time));
            Memo1.Lines.Append('offset: '
              +inttohex(dirsegment^.offset,8)+' (hex)');
            Memo1.Lines.Append('length: '
              +inttohex(dirsegment^.length,8)+' (hex)');


          end;
        finally
          try
            Dispose(dirsegment);
          except
          end;
        end;
*)
      end;

    except
      Memo1.Lines.Append('ERROR: Could not Handle File');
    end;

    try
      Dispose(header);
    except
    end;

  except
    Memo1.Lines.Append('ERROR: Could not open file(s)');
  end;

  try
    if wantedsave then closefile(fh2);
    closefile(fh1);
  except
  end;

  Button1.Enabled:=true;
end;

end.
