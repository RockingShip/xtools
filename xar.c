/*
 * X-C-Compiler/Assembler/Linker/Archiver
 */

/*
 *	This file is part of XTools, X-C-Compiler/Assembler/Linker/Archiver.
 *	Copyright (C) 2020, xyzzy@rockingship.org
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Internal storage limits
 */

enum {
	FILEMAX = 50,		// Size of filetable
	NAMEMAX = 0x821,	// Size of nametable. 3 bits set. !!! MUST BE PRIME !!!
	PATHMAX = 80,		// length of filename
};

/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,		// Bytes per word
	SBIT = 15,		// Bit number of signed bit
};

/*
 * Definitions for names's
 */

enum {
	NTAB = 0,
	NCHAR,
	NLIB,
	NLAST,
};

/*
 * Definitions for files's
 */

enum {
	FNAME = 0,
	FLENGTH,
	FOFFSET,
	FOLDOFS,
	FLAST,
};

/*
 * Definitions for .OLB header
 */

enum {
	HNAME = 0,
	HFILE,
	HLAST,
};

/*
 * User commands
 */

enum {
	ADDCMD = 1,
	DELCMD,
	CRECMD,
	LISCMD,
	EXTCMD,
};

/*
 * Object commands
 */

enum {
	REL_END = 1,

	REL_ADD = 2,
	REL_SUB = 3,
	REL_MUL = 4,
	REL_DIV = 5,
	REL_MOD = 6,
	REL_LSR = 7,
	REL_LSL = 8,
	REL_XOR = 9,
	REL_AND = 10,
	REL_OR = 11,
	REL_NOT = 12,
	REL_NEG = 13,
	REL_SWAP = 14,

	REL_CODEB = 16,
	REL_CODEW = 17,
	REL_CODEDEF = 18,
	REL_CODEORG = 19,
	REL_DATAB = 20,
	REL_DATAW = 21,
	REL_DATADEF = 22,
	REL_DATAORG = 23,
	REL_TEXTB = 24,
	REL_TEXTW = 25,
	REL_TEXTDEF = 26,
	REL_TEXTORG = 27,
	REL_UDEFB = 28,
	REL_UDEFW = 29,
	REL_UDEFDEF = 30,
	REL_UDEFORG = 31,

	REL_SYMBOL = 32,
	REL_PUSHB = 33,
	REL_PUSHW = 34,
	REL_POPW = 35,
	REL_POPB = 36,
	REL_DSB = 37,
};

/*
 * Storage
 */

int	datlen;			// length of datbuf
int	debug;			// Debug   -d specified
int	errflag;		// Error occurred
int	file[FILEMAX*NLAST];
int	name[NAMEMAX*NLAST];
int	objhdl;			// handle for .OBJ file
int	olbhdl;			// handle for source .OLB file
int	olbhdr[HLAST];		// .OLB header
int	outhdl;			// handle for destination .OLB file
int	usercmd;		// user command
int	verbose;		// Verbose -v specified

char	bakfn[PATHMAX];		// .BAK filename
char	datbuf[512];		// internal scratch buffer
char	modn[PATHMAX];		// name of module
char	objfn[PATHMAX];		// .OBJ filename
char	olbfn[PATHMAX];		// source .OLB filename
char	outfn[PATHMAX];		// destination .OLB filename

extern char *stdout;
//exit(int code);
//fclose(int hdl);
//fopen(char *name, char *mode);
//fread(char *buf, int siz, int cnt, int hdl);
//fseek(int hdl, int pos, int whence);
//fwrite(char *buf, int siz, int cnt, int hdl);
//rename(char *old, char *new);
//strcpy(char *dst, char *src);
//strncpy(char *dst, int n, char *src);
//unlink(char *name);

/*
 *
 */
error(char *msg) {
	fputs("%s", stdout);
	errflag = 1;
}

/*
 *
 */
fatal(char *msg) {
	error(msg);
	exit(0);
}

/*
 *
 */
outname(register int hash) {
	register int i;

	i = name[hash * NLAST + NTAB];
	if (i)
		i = outname(i); // display and get length string
	else
		i = 0; // nothing displayed yet
	fputc(name[hash * NLAST + NCHAR], stdout);
	return i + 1; // Increment length
}

/*
 *
 */
soutname(register int hash, register char *str) {
	register int i;

	i = name[hash * NLAST + NTAB];
	if (i)
		str = soutname(i, str);
	*str++ = name[hash * NLAST + NCHAR];
	*str = 0;
	return str;
}

/*
 * Get the (unique) hashed value for symbol, return length
 */
dohash(register char *ident, int *retval) {
	register int start, hash, tab, len, *p;

	tab = 0;
	len = 0;
	hash = 0;
	while (*ident) {
		start = hash = (hash + *ident * *ident) % olbhdr[HNAME];
		while (1) {
			p = &name[hash * NLAST];
			if ((p[NCHAR] == *ident) && (p[NTAB] == tab)) {
				tab = hash;
				break; // Inner loop
			} else if (!p[NCHAR]) {
				p[NCHAR] = *ident;
				p[NTAB] = tab;
				tab = hash;
				break; // Inner loop
			} else {
				hash += *ident;
				if (hash >= olbhdr[HNAME])
					hash -= olbhdr[HNAME];
				if (hash == start) {
					fputs("name table overflow\n", stdout);
					exit(1);
				}
			}
		}
		++ident;
		++len;
	}
	*retval = tab;
	return len;
}

/*
 *
 */
read_byte() {
	char arr[1];

	if (fread(arr, 1, 1, objhdl) != 1) {
		fputs("missing .END (use -v to discover where)\n", stdout);
		exit(1);
	}

	// return unsigned
	return arr[0] & 0xff;
}

/*
 *
 */
read_word() {
	char arr[2];
	int w;

	if (fread(arr, 1, 2, objhdl) != 2) {
		fputs("missing .END (use -v to discover where)\n", stdout);
		exit(1);
	}

	// return signed
	w = arr[0] << 8 | (arr[1] & 0xff);
	w |= -(w & (1 << SBIT));
	return w;
}

/*
 *
 */
write_byte(char byte) {
	fwrite(&byte, 1, 1, outhdl);
}

/*
 *
 */
write_word(int word) {
	char arr[2];

	arr[0] = word >> 8;
	arr[1] = word;

	fwrite(arr, 1, 2, outhdl);
}

/*
 *
 */
read_word_olb() {
	char arr[2];
	int w;

	if (fread(arr, 1, 2, olbhdl) != 2) {
		fputs("missing .END (use -v to discover where)\n", stdout);
		exit(1);
	}

	// return signed
	w = arr[0] << 8 | (arr[1] & 0xff);
	w |= -(w & (1 << SBIT));
	return w;
}

/*
 * Open all files
 */
open_file(char *fn, char *mode) {
	int fd;

	fd = fopen(fn, mode);
	if (fd > 0)
		return fd;
	fputs("fopen(", stdout);
	fputs(fn, stdout);
	fputc(',', stdout);
	fputs(mode, stdout);
	fputs(") failed\n", stdout);
	exit(1);
}

/*
 * Open the .OLB file, test the header, and load the tables
 */
open_olb() {
	register int i, *p;

	// open, allow failure
	olbhdl = fopen(olbfn, "r");

	if (!olbhdl) {
		if (verbose) {
			fputs("Creating library ", stdout);
			fputs(olbfn, stdout);
			fputc('\n', stdout);
		}

		// init header for empty archive
		olbhdr[HNAME] = NAMEMAX;
		// set all symbols to 'not in library'
		for (i = 0; i < NAMEMAX; ++i)
			name[i * NLAST + NLIB] = -1;
		return;
	}

	if (verbose) {
		fputs("Loading library ", stdout);
		fputs(olbfn, stdout);
		fputc(' ', stdout);
		fputd(olbhdl, stdout);
		fputc('\n', stdout);
	}

	for (i = 0; i < HLAST; ++i)
		olbhdr[i] = read_word_olb();
	if (olbhdr[HNAME] > NAMEMAX)
		fatal("name table too large in .OLB\n");
	if (olbhdr[HFILE] > FILEMAX)
		fatal("file table too large in .OLB\n");
	for (i = 0; i < olbhdr[HNAME] * NLAST; ++i)
		name[i] = read_word_olb();
	if (olbhdr[HFILE] > 0)
		for (i = 0; i < olbhdr[HFILE] * FLAST; ++i)
			file[i] = read_word_olb();

	// duplicate offset fields
	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		p[FOLDOFS] = p[FOFFSET];
	}
}

/*
 *
 */
copy_obj(register int hdl, register int ofs, register int len) {
	register int i, tmplen;

	fseek(hdl, ofs, 0);
	while (len > 0) {
		tmplen = len;
		if (tmplen > 512)
			tmplen = 512;
		if ((i = fread(datbuf, 1, tmplen, hdl)) != tmplen) {
			fputs("error reading .OLB during copy\n", stdout);
			exit(1);
		}
		if ((i = fwrite(datbuf, 1, tmplen, outhdl)) != tmplen) {
			fputs("error writinging .OLB during copy\n", stdout);
			exit(1);
		}
		len -= tmplen;
	}
}

/*
 *
 */
objmap() {
	register int i, *p, len;

	fputs("id        module        BASE LEN\n", stdout);
	fputs("-- -------------------- ---- ----\n", stdout);

	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		if (i+1 < 10)
			fputc(' ', stdout);
		fputd(i+1, stdout);
		fputc(' ', stdout);
		len = outname(p[FNAME]);
		while (len++ <= 20)
			fputc(' ', stdout);
		fput0x(p[FOFFSET], 4, stdout);
		fputc(' ', stdout);
		fput0x(p[FLENGTH], 4, stdout);
		fputc('\n', stdout);
	}
}

/*
 *
 */
symmap(register int start) {
	register int ch, *p, hash, tab;

	tab = (!start) ? 0 : start;
	for (ch = '!'; ch <= '~'; ++ch) {
		hash = (start + ch * ch) % olbhdr[HNAME];
		while (1) {
			p = &name[hash * NLAST];
			if ((p[NCHAR] == ch) && (p[NTAB] == tab)) {
				if (p[NLIB] != -1) {
					if (p[NLIB] + 1 < 10)
						fputc(' ', stdout);
					fputd(p[NLIB] + 1, stdout);
					fputc(' ', stdout);
					outname(hash);
					fputc('\n', stdout);
				}
				symmap(hash);
				break; // Inner loop
			} else if (!p[NCHAR]) {
				break; // Inner loop
			} else {
				hash += ch;
				if (hash >= olbhdr[HNAME])
					hash -= olbhdr[HNAME];
			}
		}
	}
}

/*
 *
 */
do_lis() {
	open_olb();
	fputs("Object statistics : \n", stdout);
	objmap();
	fputs("\nSymboltable : \n\n", stdout);
	symmap(0);
	fclose(olbhdl);
}

/*
 *
 */
do_cre() {
	register int i;

	// initialize header
	olbhdr[HNAME] = NAMEMAX;
	olbhdr[HFILE] = 0;
	// Initialize nametable
	for (i = 0; i < NAMEMAX; ++i)
		name[i * NLAST + NLIB] = -1;

	// open outputfile
	unlink(outfn);
	outhdl = open_file(outfn, "w");

	// Writeout
	for (i = 0; i < HLAST; ++i)
		write_word(olbhdr[i]);
	for (i = 0; i < olbhdr[HNAME] * NLAST; ++i)
		write_word(name[i]);

	// close and rename
	fclose(outhdl);
	unlink(bakfn);
	rename(olbfn, bakfn);
	rename(outfn, olbfn);
}

/*
 *
 */
do_add() {
	char cmd;
	register int objlen, olblen, *p, i;
	int error, hash, objinx;

	// open inputfile
	open_olb();

	if (verbose) {
		fputs("Loading module ", stdout);
		fputs(objfn, stdout);
		fputc('\n', stdout);
	}

	// first delete any existing occurrences
	dohash(modn, &hash);
	for (objinx = 0; objinx < olbhdr[HFILE]; ++objinx)
		if (file[objinx * FLAST + FNAME] == hash)
			break;
	if (objinx < olbhdr[HFILE]) {
		// found existing entry, overwrite
		for (i = 0; i < olbhdr[HNAME]; ++i) {
			p = &name[i * NLAST];
			if (p[NLIB] == objinx)
				p[NLIB] = -1;
		}
		// update file entry
		p = &file[objinx * FLAST];
		p[FOLDOFS] = 0;
	} else {
		objinx = olbhdr[HFILE]++;
		// create new object at end of list
		if (objinx >= FILEMAX) {
			fputs("Too many modules in library\n", stdout);
			exit(1);
		}
		// update file entry
		p = &file[objinx * FLAST];
		dohash(modn, &p[FNAME]);
		p[FOLDOFS] = 0;
	}

	// open objectfile
	objhdl = open_file(objfn, "r");

	// read object, calc length and insert all found symbols
	error = 0;
	objlen = 0;
	cmd = -1;
	while (cmd != REL_END) {
		cmd = read_byte();
		if (cmd < 0) {
			datlen = -cmd;
			fread(datbuf, 1, datlen, objhdl);
			objlen += datlen + 1;
		} else {
			switch (cmd) {
			case REL_ADD:
			case REL_SUB:
			case REL_MUL:
			case REL_DIV:
			case REL_MOD:
			case REL_LSR:
			case REL_LSL:
			case REL_XOR:
			case REL_AND:
			case REL_OR:
			case REL_NOT:
			case REL_NEG:
			case REL_SWAP:
			case REL_POPB:
			case REL_POPW:
				objlen += 1;
				break;
			case REL_PUSHB:
			case REL_CODEB:
			case REL_DATAB:
			case REL_TEXTB:
			case REL_UDEFB:
				read_byte();
				objlen += 2;
				break;
			case REL_PUSHW:
			case REL_CODEW:
			case REL_DATAW:
			case REL_TEXTW:
			case REL_UDEFW:
				read_word();
				objlen += BPW + 1;
				break;
			case REL_SYMBOL:
				// Push symbol value on stack
				datlen = read_byte(); // length
				fread(datbuf, 1, datlen, objhdl); // symbol
				objlen += 2 + datlen;
				break;
			case REL_DSB:
				// skip specified number of bytes in current segment
				read_word(); // skipcount
				objlen += 1 + BPW;
				break;
			case REL_END:
				objlen += 1;
				break;
			case REL_CODEDEF:
			case REL_DATADEF:
			case REL_TEXTDEF:
			case REL_UDEFDEF:
				// symbol definition
				read_word(); // symbol offset
				datlen = read_byte(); // length
				fread(datbuf, 1, datlen, objhdl); // symbol
				objlen += 1 + BPW + 1 + datlen;

				// locate symbol in table and insert
				datbuf[datlen] = 0;
				dohash(datbuf, &hash);
				p = &name[hash * NLAST];
				if (p[NLIB] != -1) {
					// get name of library already containing symbol
					fputs("Symbol '", stdout);
					fputs(datbuf, stdout);
					fputs("' already defined in module ", stdout);
					soutname(file[p[NLIB] * FLAST + FNAME], datbuf);
					fputs(datbuf, stdout);
					fputc('\n', stdout);
					++error;
				} else {
					// new symbol
					p[NLIB] = objinx;
					if (debug) {
						fputs("SYMDEF: ", stdout);
						fputs(datbuf, stdout);
						fputc('\n', stdout);
					}
				}
				break;
			case REL_CODEORG:
			case REL_DATAORG:
			case REL_TEXTORG:
			case REL_UDEFORG:
				read_word(); // segment offset
				objlen += 1 + BPW;
				break;
			default:
				fputs("\"unknown command ", stdout);
				fput0x(cmd & 0xff, 2, stdout);
				exit(1);
				break;
			}
		}
	}

	// save length of module
	file[objinx * FLAST + FLENGTH] = objlen;

	// generate new offsets for existing modules
	olblen = (HLAST * BPW) +
		 (olbhdr[HNAME] * NLAST * BPW) +
		 (olbhdr[HFILE] * FLAST * BPW);
	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		p[FOFFSET] = olblen;
		olblen += p[FLENGTH];
	}

	// if no errors occurred then generate new library
	if (error) {
		fputs("module not inserted\n", stdout);
		exit(1);
	}
	if (debug) {
		fputs("module length: ", stdout);
		fputd(objlen, stdout);
		fputc('\n', stdout);
	}

	// build new library
	unlink(outfn);
	outhdl = open_file(outfn, "w");

	// Writeout
	for (i = 0; i < HLAST; ++i)
		write_word(olbhdr[i]);
	for (i = 0; i < olbhdr[HNAME] * NLAST; ++i)
		write_word(name[i]);
	for (i = 0; i < olbhdr[HFILE] * FLAST; ++i)
		write_word(file[i]);

	// copy objects and append new object
	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		if (p[FOLDOFS])
			copy_obj(olbhdl, p[FOLDOFS], p[FLENGTH]);
		else
			copy_obj(objhdl, 0, p[FLENGTH]);
	}

	// close and rename
	fclose(outhdl);
	unlink(bakfn);
	rename(olbfn, bakfn);
	rename(outfn, olbfn);
}

/*
 *
 */
do_del() {
	char cmd;
	register int olblen, *p, i;
	int error, hash, objinx;

	// open inputfile
	open_olb();

	// locate module in filetable
	dohash(modn, &hash);
	for (objinx = 0; objinx < olbhdr[HFILE]; ++objinx)
		if (file[objinx * FLAST + FNAME] == hash)
			break;
	if (objinx >= olbhdr[HFILE]) {
		fputs("module not found\n", stdout);
		exit(1);
	}

	// remove all symbol references
	for (i = 0; i < olbhdr[HNAME]; ++i) {
		p = &name[i * NLAST];
		if (p[NLIB] == objinx)
			p[NLIB] = -1;
		else if (p[NLIB] > objinx)
			--p[NLIB];
	}

	// remove fileentry
	for (i = objinx; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		p[FNAME] = p[FNAME + FLAST];
		p[FOLDOFS] = p[FOLDOFS + FLAST];
		p[FLENGTH] = p[FLENGTH + FLAST];
	}
	--olbhdr[HFILE];

	// generate new offsets for remaining modules
	olblen = (HLAST * BPW) +
		 (olbhdr[HNAME] * NLAST * BPW) +
		 (olbhdr[HFILE] * FLAST * BPW);
	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		p[FOFFSET] = olblen;
		olblen += p[FLENGTH];
	}

	// build new library
	unlink(outfn);
	outhdl = open_file(outfn, "w");

	// Writeout
	for (i = 0; i < HLAST; ++i)
		write_word(olbhdr[i]);
	for (i = 0; i < olbhdr[HNAME] * NLAST; ++i)
		write_word(name[i]);
	for (i = 0; i < olbhdr[HFILE] * FLAST; ++i)
		write_word(file[i]);

	// copy objects and append new object
	for (i = 0; i < olbhdr[HFILE]; ++i) {
		p = &file[i * FLAST];
		copy_obj(olbhdl, p[FOLDOFS], p[FLENGTH]);
	}

	// close and rename
	fclose(outhdl);
	unlink(bakfn);
	rename(olbfn, bakfn);
	rename(outfn, olbfn);
}

/*
 *
 */
do_ext() {
	char cmd;
	register int olblen, *p, i;
	int error, hash, objinx;

	// open inputfile
	open_olb();

	// locate module in filetable
	dohash(modn, &hash);
	for (objinx = 0; objinx < olbhdr[HFILE]; ++objinx) {
		p = &file[objinx * FLAST];
		if (p[FNAME] == hash)
			break;
	}
	if (objinx >= olbhdr[HFILE]) {
		fputs("module not found\n", stdout);
		exit(1);
	}

	// open object file as outhdl (used by copy_obj)
	outhdl = open_file(objfn, "w");
	copy_obj(olbhdl, p[FOFFSET], p[FLENGTH]);

	fclose(outhdl);
	fclose(olbhdl);
}

/*
 * Initialize all variables
 */
initialize() {
	register int i, *p;

	verbose = debug = usercmd = 0;
	objhdl = olbhdl = outhdl = 0;

	objfn[0] = olbfn[0] = outfn[0] = 0;

	// reset tables
	for (i = 0; i < NAMEMAX; ++i) {
		p = &name[i * NLAST];
		p[NCHAR] = p[NLIB] = 0;
	}

	// reserve first entry so it terminates lists
	name[0 * NLAST + NCHAR] = '?';
}

/*
 * Process commandline
 */
usage() {
	fputs("X-Archiver, Version ", stdout);
	fputs(getversion(), stdout);
	fputs("\n\n", stdout);

	fputs("usage: xar (a|c|d|l|x) <library>[.<ext>] [<object>[.<ext>] ... \n", stdout);
	fputs("  a  Add a module\n", stdout);
	fputs("  c  Create a new library\n", stdout);
	fputs("  d  Delete a module\n", stdout);
	fputs("  t  List the library\n", stdout);
	fputs("  x  Extract a module\n", stdout);
	fputs("  -v Verbose\n", stdout);
	exit(1);
}

/*
 *
 */
fext(char *out, char *path, char *ext, int force) {
	char *p;
	int baselen;

	baselen = 0;
	for (p = path; *p; ++p) {
		if (*p == '\\' || *p == '/')
			baselen = 0;
		else if (*p == '.')
			baselen = p - path;
	}

	if (baselen && !force)
		strcpy(out, path);
	else {
		if (!baselen)
			baselen = p - path;
		strncpy(out, path, baselen);
		strcpy(out + baselen, ext);
	}
}

/*
 *
 */
startup(register int *argv) {
	++argv; // skip argv[0]
	while (*argv) {
		register char *arg;
		arg = *argv++;


		if (*arg != '-') {
			if (!usercmd) {
				switch (*arg) {
				case 'a':
					usercmd = ADDCMD;
					break;
				case 'c':
					usercmd = CRECMD;
					break;
				case 'd':
					usercmd = DELCMD;
					break;
				case 't':
					usercmd = LISCMD;
					break;
				case 'x':
					usercmd = EXTCMD;
					break;
				default:
					usage();
				}
				if (*++arg)
					usage(); // one letter commands only
			} else if (!olbfn[0]) {
				fext(olbfn, arg, ".xa", 0);
				fext(bakfn, arg, ".bak", 1);
				fext(outfn, arg, ".tmp", 1);
			} else if (!objfn[0]) {
				fext(objfn, arg, ".xo", 0);
				fext(modn, arg, "", 1);
			} else
				usage();
		} else {
			// Process option
			++arg;
			switch (*arg++) {
			case 'd':
				debug = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				usage();
				break;
			}
		}
	}

	// filename MUST be supplied
	if (!outfn[0])
		usage();
	// command MUST be supplied
	if (!usercmd)
		usage();
	// commands add/del/extract needs object name
	if ((usercmd == ADDCMD) || (usercmd == DELCMD) || (usercmd == EXTCMD))
		if (!outfn[0])
			usage();
}

/*
 * Program start
 */
main(int argc, int *argv) {
	register int i, j, *p;

	initialize(); // initialize all variables
	startup(argv); // Process commandline options
	if (debug) {
		fputs("Library  : '", stdout);
		fputs(olbfn, stdout);
		fputs("'\n", stdout);
		fputs("Object   : '", stdout);
		fputs(objfn, stdout);
		fputs("'\n", stdout);
	}

	switch (usercmd) {
	case ADDCMD:
		do_add();
		break;
	case CRECMD:
		do_cre();
		break;
	case DELCMD:
		do_del();
		break;
	case LISCMD:
		do_lis();
		break;
	case EXTCMD:
		do_ext();
		break;
	}

	if (debug) {
		j = 0;
		for (i = 0; i < olbhdr[HNAME]; ++i) if (name[i * NLAST + NCHAR]) ++j;

		fputs("Names        : ", stdout);
		fputd(j, stdout);
		fputc('/', stdout);
		fputd(olbhdr[HNAME], stdout);
		fputc('\n', stdout);
	}

	return 0;
}
