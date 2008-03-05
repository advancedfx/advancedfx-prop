#!/usr/bin/perl -w

# $Id: dem-hl.pl,v 1.17 2004/03/09 17:34:25 girlich Exp $

use strict;
use IO::Seekable;
use IO::File;
use BitField;

sub read_with_check($$);
sub LittleFloat($);
sub ReadString($);
sub CL_ParseServerMessage($$);

sub parse_message_01_nop($$$);
sub parse_message_04_version($$$);
sub parse_message_07_time($$$);
sub parse_message_08_print($$$);
sub parse_message_11_serverinfo($$$);
sub parse_message_13_updateuserinfo($$$);
sub parse_message_14_deltadescription($$$);
sub parse_message_15_clientdata($$$);
sub parse_message_29_spawnstaticsound($$$);
sub parse_message_37_roomtype($$$);
sub parse_message_45_resourcerequest($$$);
sub parse_message_54_sendextrainfo($$$);

my %parse = (
	 1 => \&parse_message_01_nop,
#	 4 => \&parse_message_04_version,
	 7 => \&parse_message_07_time,
	 8 => \&parse_message_08_print,
	11 => \&parse_message_11_serverinfo,
	13 => \&parse_message_13_updateuserinfo,
	14 => \&parse_message_14_deltadescription,
	15 => \&parse_message_13_updateuserinfo,
	29 => \&parse_message_29_spawnstaticsound,
	37 => \&parse_message_37_roomtype,
	45 => \&parse_message_45_resourcerequest,
	54 => \&parse_message_54_sendextrainfo,
);

my $indent_diff = 1;

my %macroname = (
	"0" => "game data",
	"1" => "game data",
	"3" => "client command",
	"5" => "last in segment",
	"8" => "play sound",
);


(my $release = q$Revision: 1.17 $) =~ s/^[^:]+:\s*(.*?)\s*$/$1/;
(my $date = q$Date: 2004/03/09 17:34:25 $) =~ s/^[^:]+:\s*(.*?)\s*$/$1/;

if (@ARGV != 2) {
	print "Half-Life demo analyser\n";
	print "DEM-HL (c) Uwe Girlich, Release $release, $date\n";
	print "dem-hl.pl dembin demtxt\n";
	exit;
}

my ($name_in, $name_out) = @ARGV;

my $file_in = new IO::File "$name_in", "r";
die "could not open $name_in for reading: $!" unless $file_in;
binmode $file_in;

my $file_out = new IO::File "$name_out", "w";
die "could not open $name_out for writing: $!" unless $file_out;
binmode $file_out;

printf "%s (DEM bin HL) -> %s (DEM text HL)\n",
	$name_in, $name_out;

# file header #################################################################

$file_in->seek(0, SEEK_SET);
my $header_d = read_with_check($file_in, 544);
my ($magick, $demo_version, $network_version, $map_name, $game_dll, $dir_offset) = 
	unpack
	"Z8 V V Z260 Z264 V",
	$header_d;

printf $file_out "// source: binary Half-Life demo %s\n\n", $name_in;
printf $file_out "game \"Half-Life\";\n";
printf $file_out "header {\n";
printf $file_out " magick \"%s\";\n", $magick;
printf $file_out " demo_version %i;\n", $demo_version;
printf $file_out " network_version %i;\n", $network_version;
printf $file_out " map_name \"%s\";\n", $map_name;
printf $file_out " game_dll \"%s\";\n", $game_dll;
printf $file_out " dir_offset %i; // 0x%08x\n", $dir_offset, $dir_offset;
printf $file_out "}\n";

# end of file header ##########################################################


# segment directory ###########################################################


## number of segment directory entries ########################################

$file_in->seek($dir_offset,SEEK_SET);
my $dir_entries_d = read_with_check($file_in, 4);
my ($dir_entries) = unpack 
	"V",
	$dir_entries_d;

printf $file_out "directory {\n";
printf $file_out " entries %i;\n", $dir_entries;

## end of number of segment directory entries #################################

## segment directory entries ##################################################

my @directory;
for (my $i = 0 ; $i < $dir_entries ; $i++ ) {
	my $dir_entry_d = read_with_check($file_in, 92);
	my %segment;
	(
		$segment{"number"},
		$segment{"title"},
		$segment{"flags"},
		$segment{"play"},
		$segment{"time"},
		$segment{"frames"},
		$segment{"offset"},
		$segment{"length"}
	) = unpack
		"V Z64 V V f V V V",
		$dir_entry_d;
	$segment{"time"} = LittleFloat($segment{"time"});

	$segment{"end"} = $segment{"offset"} + $segment{"length"} - 1;

	push @directory, \%segment;

	printf $file_out " entry {\n";
	printf $file_out "  number %i;\n", $segment{"number"};
	printf $file_out "  title \"%s\";\n", $segment{"title"};
	printf $file_out "  flags {\n";
	printf $file_out "   value 0x%08x;\n", $segment{"flags"};
	printf $file_out "   bit_title; // 0x01\n"		if $segment{"flags"} & 0x01;
	printf $file_out "   bit_play; // 0x04\n"		if $segment{"flags"} & 0x04;
	printf $file_out "   bit_fade_in_slow; // 0x08\n"	if $segment{"flags"} & 0x08;
	printf $file_out "   bit_fade_in_fast; // 0x10\n"	if $segment{"flags"} & 0x10;
	printf $file_out "   bit_fade_out_slow; // 0x20\n"	if $segment{"flags"} & 0x20;
	printf $file_out "   bit_fade_out_fast; // 0x40\n"	if $segment{"flags"} & 0x40;
	printf $file_out "  }\n";
	printf $file_out "  play %i;\n", $segment{"play"};
	printf $file_out "  time %fs;\n", $segment{"time"};
	printf $file_out "  frames %i;\n", $segment{"frames"};
	printf $file_out "  offset %i; // 0x%08x\n", $segment{"offset"}, $segment{"offset"};
	printf $file_out "  length %i; // 0x%08x\n", $segment{"length"}, $segment{"length"};
	printf $file_out "  end %i; // 0x%08x\n", $segment{"end"}, $segment{"end"};
	printf $file_out " }\n";
}

# end of segment directory entries ############################################

my $dir_end = $file_in->tell - 1;
printf $file_out " dir_end %i; // %08x\n", $dir_end, $dir_end;
printf $file_out "}\n";

# end of segment directory ####################################################


# go over the segments described in the directory

printf $file_out "segments {\n";

for (@directory) {
	my %segment = %{$_};
	printf $file_out " segment {\n";
	printf $file_out "  number %i\n", $segment{"number"};
	$file_in->seek($segment{"offset"},SEEK_SET);
	for ( ;; ) {
		my %macro;
		$macro{"file_offset"} = $file_in->tell;
		my $macro_d = read_with_check($file_in, 9);
		(
			$macro{"type"},
			$macro{"time"},
			$macro{"frame"}
		) = unpack "C f V", $macro_d;
		$macro{"time"} = LittleFloat($macro{"time"});

		printf $file_out "  macro { // offset=0x%08x\n", $macro{"file_offset"};
		printf $file_out "   type %i; // %s\n", $macro{"type"},
			(exists $macroname{$macro{"type"}} ?
				$macroname{$macro{"type"}} :
				"unknown macro block type");
		printf $file_out "   time %fs;\n", $macro{"time"};
		printf $file_out "   frame %i;\n", $macro{"frame"};
		my $cont = -1;
		# macro block 0, 1 ############################################
		if ($macro{"type"} == 0 || $macro{"type"} == 1) {
			my $uksize;

			if ($network_version == 42) {
				# 1.1.0.1
				$uksize = 560;
			}
			if ($network_version >= 46) {
				# 1.1.1.0
				$uksize = 464;
			}
			
			printf $file_out "   ukdata // offset=0x%08x, len=%d\n",
				$file_in->tell(), $uksize;
			my $data_1h_d = read_with_check($file_in,$uksize);
			# leave it for later

			my $data_1_len_d = read_with_check($file_in,4);
			(
				$macro{"length"}
			) = unpack "V", $data_1_len_d;
			printf $file_out "   gamedata_length %i;\n", $macro{"length"};

			printf $file_out "   gamedata";
			my $data_1_d = read_with_check($file_in,$macro{"length"});
			my @data = unpack "C" . $macro{"length"} , $data_1_d;
			for (@data) { printf $file_out " %02x", $_; }
			printf $file_out ";\n";
			printf $file_out "   gamedata_messages {\n";
			CL_ParseServerMessage($file_out, $data_1_d);
			printf $file_out "   }\n";
			$cont = 1;
		}
		# macro block 2 ###############################################
		if ($macro{"type"} == 2) {
			$cont = 1;
		}
		# macro block 3 ###############################################
		if ($macro{"type"} == 3) {
			my $data_3_d = read_with_check($file_in,64);
			(
				$macro{"text"}
			) = unpack "Z64", $data_3_d;
			printf $file_out "   text \"%s\";\n", $macro{"text"};
			$cont = 1;
		}
		# macro block 4 ###############################################
		if ($macro{"type"} == 4) {
			my $data_4_d = read_with_check($file_in,32);
			my @data = unpack "C32", $data_4_d;
			printf $file_out "   data";
			for (@data) { printf $file_out " %02x", $_; }
			printf $file_out ";\n";
			$cont = 1;
		}
		# macro block 5 ###############################################
		if ($macro{"type"} == 5) {
			$cont = 0; # last in segment
		}
		# macro block 6 ###############################################
		if ($macro{"type"} == 6) {
			# this is just an educated guess
			my $data_6_d = read_with_check($file_in,4+4+4+72);
			(
				$macro{"uk_i1"},
				$macro{"uk_i2"},
				$macro{"uk_f"},
				my @data,
			) = unpack "V V f C72", $data_6_d;
			$macro{"uk_f"} = LittleFloat($macro{"uk_f"});
			printf $file_out "   uk_i1 %d;\n", $macro{"uk_i1"};
			printf $file_out "   uk_i2 %d;\n", $macro{"uk_i2"};
			printf $file_out "   uk_f %f;\n", $macro{"uk_f"};
			printf $file_out "   data";
			for (@data) { printf $file_out " %02x", $_; }
			printf $file_out ";\n";
			$cont = 1;
		}
		# macro block 7 ###############################################
		if ($macro{"type"} == 7) {
			# this is just an educated guess
			my $data_7_d = read_with_check($file_in,8);
			(
				$macro{"uk_i1"},
				$macro{"uk_i2"},
			) = unpack "V V", $data_7_d;
			printf $file_out "   uk_i1 %d;\n", $macro{"uk_i1"};
			printf $file_out "   uk_i2 %d;\n", $macro{"uk_i2"};
			$cont = 1;
		}
		# macro block 8 ###############################################
		if ($macro{"type"} == 8) {
			my $data_8_1 = read_with_check($file_in,8);
			(
				$macro{"uk_i1"},
				$macro{"sound_name_length"}
			) = unpack "V V", $data_8_1;
			printf $file_out "   uk_i1 %d;\n", $macro{"uk_i1"};
			printf $file_out "   sound_name_length %d;\n", $macro{"sound_name_length"};
			my $data_8_2 = read_with_check($file_in,$macro{"sound_name_length"});
			(
				$macro{"sound_name"}
			) = unpack "Z" . $macro{"sound_name_length"}, $data_8_2;
			printf $file_out "   sound_name \"%s\";\n", $macro{"sound_name"};
			my $data_8_3 = read_with_check($file_in,16);
			(
				$macro{"uk_f1"},
				$macro{"uk_f2"},
				$macro{"uk_i2"},
				$macro{"uk_i3"}
			) = unpack "f f V V", $data_8_3;
			$macro{"uk_f1"} = LittleFloat($macro{"uk_f1"});
			$macro{"uk_f2"} = LittleFloat($macro{"uk_f2"});
			printf $file_out "   uk_f1 %d;\n", $macro{"uk_f1"};
			printf $file_out "   uk_f2 %d;\n", $macro{"uk_f2"};
			printf $file_out "   uk_i2 0x%x;\n", $macro{"uk_i2"};
			printf $file_out "   uk_i3 %d;\n", $macro{"uk_i3"};
			$cont = 1;
		}
		# macro block 9 ###############################################
		if ($macro{"type"} == 9) {
			my $data_9_len_d = read_with_check($file_in,4);
			(
				$macro{"length"}
			) = unpack "V", $data_9_len_d;
			my $data_9_d = read_with_check($file_in,$macro{"length"});
			my @data = unpack "C" . $macro{"length"} , $data_9_d;
			printf $file_out "   length %i\n", $macro{"length"};
			printf $file_out "   data";
			for (@data) { printf $file_out " %02x", $_; }
			printf $file_out ";\n";
			$cont = 1;
		}
		#
		printf $file_out "  }\n";
		if ($cont == -1) {
			printf $file_out "// XXX: unknown block type\n";
			$cont = 0;
		}
		last if (!$cont);
	}
	printf $file_out " }\n";
}

printf $file_out "}\n";

$file_in->close;
$file_out->close;


sub read_with_check($$)
{
        my ($infd, $length) = @_;
        my $data;
        my $res = $infd->read($data, $length);
        if ($res != $length) {
                if ($res < 0) {
                        die sprintf
			"could not read 0x%x bytes at 0x%x: $!\n",
                        $length, tell($infd);
                }
                else {
                        die sprintf
			"tried to read 0x%x bytes, got only 0x%x, pos is now 0x%x\n",
			$length, $res, tell($infd);
                }
        }
        return $data;
}


sub LittleFloat($) {
	unpack("f",pack("I",unpack("V",pack("f",$_[0]))));
}


sub ReadString($)
{
	my ($data) = @_;

#	if ($mark == 1) {
#		printf "length=%d data=\"%s\"\n", length($data), $data;
#	}
	$data =~ m|^([^\000]*?)\000(.*)$|s;
	my ($string, $rest) = ($1, $2);
#	if ($mark == 1) {
#		printf "length=%d data=\"%s\"\n", length($data), $data;
#		printf "length=%d rest=\"%s\"\n", length($rest), $rest;
#		$mark = 0;
#	}
	return $string, $rest;
}


sub CL_ParseServerMessage($$) {
	my ($file, $data) = @_;

	my $length = length($data);
	my $indent = 4;

	while (length($data) > 0) {
		(my $msgtype, $data) = unpack("C a*", $data);
		if (exists $parse{$msgtype}) {
			my $ref = $parse{$msgtype};
			$data = &$ref($file, $data, $indent);
		}
		else {
			printf $file "%smessage {\n", " " x $indent; 
			printf $file "%s id %s; // unknown message type\n",
				" " x $indent, $msgtype;
			printf $file "%s data", " " x $indent;
			my @data = unpack (("C" . length($data)), $data);
			for (@data) { printf $file " %02x", $_; }
			printf $file ";\n";
			printf $file "%s}\n", " " x $indent;
			last;
		}
	}
}


sub parse_message_01_nop($$$) {
	my ($file, $data, $indent) = @_;
	printf $file "%snop;\n", " " x $indent;
	return $data;
}


sub parse_message_04_version($$$) {
	my ($file, $data, $indent) = @_;
	my ($version, $rest) = unpack("V a*", $data);
	printf $file "%sversion %d;\n", " " x $indent, $version;
	return $rest;
}


sub parse_message_07_time($$$) {
	my ($file, $data, $indent) = @_;
	my ($time, $rest) = unpack("f a*", $data);
	printf $file "%stime %f;\n", " " x $indent, $time;
	return $rest;
}


sub parse_message_08_print($$$) {
	my ($file, $data, $indent) = @_;
	my ($text, $rest) = ReadString($data);
	printf $file "%sprint \"%s\";\n", " " x $indent, $text;
	return $rest;
}


sub parse_message_11_serverinfo($$$) {
	my ($file, $data, $indent) = @_;
	my @uk_b3;
	(
		my $serverversion,
		my $rest,
	) = unpack("V a*", $data);
	(
		my $uk_i1,
		my $uk_i2,
		$uk_b3[0],
		$uk_b3[1],
		$uk_b3[2],
		$uk_b3[3],
		$uk_b3[4],
		$uk_b3[5],
		$uk_b3[6],
		$uk_b3[7],
		$uk_b3[8],
		$uk_b3[9],
		$uk_b3[10],
		$uk_b3[11],
		$uk_b3[12],
		$uk_b3[13],
		$uk_b3[14],
		$uk_b3[15],
		my $maxclients,
		my $uk_b4,
		my $uk_b5,
		$rest,
	) = unpack("V V C16 C C C a*", $rest);
	(my $gamedir, $rest) = ReadString($rest);
	my $remotehost;
	if ($serverversion >= 46) {
		($remotehost, $rest) = ReadString($rest);
	}
	(my $map1, $rest) = ReadString($rest);
	(my $map2, $rest) = ReadString($rest);
	(my $extraflag, $rest, ) = unpack("C a*", $rest);
	my $extralength;
	my @extradata;
	my @lastdata;
	if ($extraflag > 0) {
		($extralength, $rest, ) = unpack("C a*", $rest);
		$extralength &= 0xff;
		my $format = sprintf "C%d a*", $extralength;
		(@extradata) = unpack($format, $rest);
		$rest = pop @extradata;
		(@lastdata) = unpack("C16 a*", $rest);
		$rest = pop @lastdata;
	}

	if ($maxclients < 1 || $maxclients > 32) {
		die sprintf "Bad maxclients (%u) from server\n", $maxclients;
	}
	printf $file "%sserverinfo {\n", " " x $indent;

	printf $file "%s serverversion %d;\n", " " x $indent,
		$serverversion;
	printf $file "%s uk_i1 %d;\n", " " x $indent,
		$uk_i1;
	printf $file "%s uk_i2 %d;\n", " " x $indent,
		$uk_i2;
	printf $file "%s uk_b3", " " x $indent;
	for (@uk_b3) { printf $file " %02x", $_; }
	printf $file ";\n";
	printf $file "%s maxclients %d;\n", " " x $indent,
		$maxclients;
	printf $file "%s uk_b4 %d;\n", " " x $indent,
		$uk_b4;
	printf $file "%s uk_b5 %d;\n", " " x $indent,
		$uk_b5;
	printf $file "%s gamedir \"%s\";\n", " " x $indent,
		$gamedir;
	if ($serverversion >= 46) {
		printf $file "%s remotehost \"%s\";\n", " " x $indent,
			$remotehost;
	}
	printf $file "%s map1 \"%s\";\n", " " x $indent,
		$map1;
	printf $file "%s map2 \"%s\";\n", " " x $indent,
		$map2;
	printf $file "%s extraflag %d;\n", " " x $indent,
		$extraflag;
	if ($extraflag > 0) {
		printf $file "%s extralength %d;\n", " " x $indent,
		$extralength;
		printf $file "%s extradata", " " x $indent;
		for (@extradata) { printf $file " %02x", $_; }
		printf $file ";\n";
		printf $file "%s lastdata", " " x $indent;
		for (@lastdata) { printf $file " %02x", $_; }
		printf $file ";\n";
	}
	printf $file "%s}\n", " " x $indent;
	return $rest;
}


#my $mark=0;


sub parse_message_13_updateuserinfo($$$) {
	my ($file, $data, $indent) = @_;
	(
		my $slot,
		my $userid,
		my $rest,
	) = unpack ("C V a*", $data);
#	$mark = 1;	
	(my $userinfo, $rest) = ReadString($rest);
	printf $file "%supdateuserinfo {\n", " " x $indent;
	printf $file "%sslot %d;\n", " " x ($indent+$indent_diff), $slot;
	printf $file "%suserid %d;\n", " " x ($indent+$indent_diff), $userid;
	printf $file "%suserinfo \"%s\";\n", " " x ($indent+$indent_diff),
			$userinfo;
	printf $file "%s}\n", " " x $indent;
	return $rest;
}


sub parse_message_14_deltadescription($$$) {
	my ($file, $data, $indent) = @_;
	my ($structure, $rest) = ReadString($data);
	printf $file "%sdeltadescription {\n", " " x $indent;
	printf $file "%sstructure \"%s\";\n", " " x ($indent+$indent_diff), $structure;
	my $bf = BitField->new;
	$bf->set_buffer($rest);
	my $entries = $bf->read_bits(16);
	printf $file "%sentries \"%d\";\n", " " x ($indent+$indent_diff), $entries;
	for (my $entry=0;$entry<$entries;$entry++) {
		printf $file "%sentry {\n", " " x ($indent+$indent_diff);
		my $length = $bf->read_bits(3);
		printf $file "%slength %d\n", " " x ($indent+2*$indent_diff),
			$length;
		printf $file "%sdata ", " " x ($indent+2*$indent_diff);
		for (my $i=0;$i<$length;$i++) {
			my $byte = $bf->read_bits(8);
			printf $file "%02x (%c) ", $byte, chr($byte);
		}
		print $file "\n";
	}
	printf $file "%s}\n", " " x $indent;
	$rest = $bf->get_buffer();
	return $rest;
}



sub parse_message_15_clientdata($$$) {
	my ($file, $data, $indent) = @_;
	(
		my $slot,
		my $userid,
		my $rest,
	) = unpack ("C V a*", $data);
#	$mark = 1;	
	(my $userinfo, $rest) = ReadString($rest);
	printf $file "%sclientdata {\n", " " x $indent;
	printf $file "%sslot %d;\n", " " x ($indent+$indent_diff), $slot;
	printf $file "%suserid %d;\n", " " x ($indent+$indent_diff), $userid;
	printf $file "%suserinfo \"%s\";\n", " " x ($indent+$indent_diff),
			$userinfo;
	printf $file "%s}\n", " " x $indent;
	return $rest;
}


sub parse_message_29_spawnstaticsound($$$) {
	my ($file, $data, $indent) = @_;
	my @org;
	(
		$org[0],
		$org[1],
		$org[2],
		my $sound_num,
		my $vol,
		my $atten,
		my $uk_s1,
		my $uk_b2,
		my $uk_b3,
		my $rest,
	) = unpack ("v v v v C C v C C a*", $data);
	$org[0] /= 8.0; $org[1] /= 8.0; $org[2] /= 8.0;
	$vol /= 255.0;
	$atten /= 64.0;
	printf $file "%sspawnstaticsound {\n", " " x $indent;
	printf $file "%sorg %f %f %f;\n", " " x ($indent+$indent_diff), 
			$org[0], $org[1], $org[2];
	printf $file "%ssound_num %d;\n", " " x ($indent+$indent_diff), 
			$sound_num;
	printf $file "%svol %f;\n", " " x ($indent+$indent_diff), 
			$vol;
	printf $file "%satten %f;\n", " " x ($indent+$indent_diff), 
			$atten;
	printf $file "%suk_s1 %d;\n", " " x ($indent+$indent_diff), 
			$uk_s1;
	printf $file "%suk_b2 %d;\n", " " x ($indent+$indent_diff), 
			$uk_b2;
flush $file;
	printf $file "%suk_b3 %d;\n", " " x ($indent+$indent_diff), 
			$uk_b3;
	printf $file "%s}\n", " " x $indent;
	return $rest;
}


sub parse_message_37_roomtype($$$) {
	my ($file, $data, $indent) = @_;
	my ($room_type, $rest) = unpack("s a*", $data);
	printf $file "%sroomtype %d;\n", " " x $indent, $room_type;
	return $rest;
}


sub parse_message_45_resourcerequest($$$) {
	my ($file, $data, $indent) = @_;
	my ($level, $uk_long, $rest) = unpack("V V a*", $data);
	printf $file "%sresourcerequest {\n", " " x $indent;
	printf $file "%slevel %d;\n", " " x ($indent+$indent_diff), $level;
	printf $file "%suk_long %d;\n", " " x ($indent+$indent_diff), $uk_long;
	printf $file "%s}\n", " " x $indent;
	return $rest;
}



sub parse_message_54_sendextrainfo($$$) {
	my ($file, $data, $indent) = @_;
	my ($text, $rest) = ReadString($data);
	(my $uk_byte, $rest) = unpack("C a*", $rest);
	printf $file "%ssendextrainfo {\n", " " x $indent;
	printf $file "%stext \"%s\";\n", " " x ($indent+$indent_diff), $text;
	printf $file "%suk_byte %d;\n", " " x ($indent+$indent_diff), $uk_byte;
	printf $file "%s}\n", " " x $indent;
	return $rest;
}


