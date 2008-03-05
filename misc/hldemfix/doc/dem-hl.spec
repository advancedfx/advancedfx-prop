/* File format of a HL demo file (for version HL 1.1.0.1) */
/* $Id: dem-hl.spec,v 1.1 2000/08/15 14:44:55 girlich Exp $ */

/* (crude) definition of the file header */

typedef file_header_s {
/*  0*/	char[8]		magic;			/* == "HLDEMO\0\0" */
/*  8*/	uint32		demo_version;		/* == 5 (HL 1.1.0.1) */
/*  c*/	uint32		network_version;	/* == 42 (HL 1.1.0.1) */
/* 10*/	char[0x104]	map_name;		/* eg "c0a0e" */
/*114*/	char[0x108]	game_dll;		/* eg "valve" */
/*21c*/	uint32		dir_offset;		/* absolute directory offset */
} file_header_t;

/* The character arrays are probably both shorter (0x100). */
/* sizeof(file_header_t) == 544 == 0x220 */

/* read the header */

file_header_t	header;

fseek(fp,0,SEEK_SET);
fread(header, 1, sizeof(file_header_t), fp);

/* read number of directory entries (segments) */

uint32	dir_entries_num;

fseek(fp,header.dir_offset, SEEK_SET);
fread(&dir_entries_num, sizeof(dir_entries_num), 1, fp);
/* make sure, that 1 <= dir_entries_num <= 1024 (0x400) */

/* definition of a segment directory entry */

typedef struct dir_entry_s {
/*00*/	uint32		number;	/* 0 ... dir_entries_num - 1 */
/*04*/	char[64]	title;	/* "LOADING", "Playback" */
/*44*/	uint32		flags;	/* see below */
/*48*/	int32		play;	/* CD track for this segment (default: -1) */
/*4c*/	float		time;	/* in seconds */
/*50*/	uint32		frames;	/* number of frames in the segment */
/*54*/	uint32		offset;	/* file offset for this segment */
/*58*/	uint32		length;	/* length of this segment */
} dir_entry_t;
/* sizeof(dir_entry_t) == 92 == 0x5c */

/* possible flags (bitmask) */
#define SEGMENT_FLAG_TITLE		0x01
#define SEGMENT_FLAG_PLAY		0x04
#define SEGMENT_FLAG_FADE_IN_SLOW	0x08
#define SEGMENT_FLAG_FADE_IN_FAST	0x10
#define SEGMENT_FLAG_FADE_OUT_SLOW	0x20
#define SEGMENT_FLAG_FADE_OUT_FAST	0x40

/* calculate the directory size */
uint32 dir_size;
dir_size = dir_entries_num * sizeof(demo_direntry_t);

/* get the necessary memory */
dir_entry_t *dir_entries;
dir_entries = malloc(dir_size);

/* read the directory */
/* no seek here, continue directly after dir_entries_num */
fread(dir_entries, 1, dir_size, fp);



