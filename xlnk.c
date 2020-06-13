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
	FILEMAX = 50,		// Number of files
	NAMEMAX = 0x821,	// Size of nametable. 3 bits set. !!! MUST BE PRIME !!!
	PATHMAX = 80,		// Length of filename
	STACKMAX = 50,		// Size of linker stack
};

/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,			// Bytes per word
	SBIT = 15,			// Bit number of signed bit
};

/*
 * segment names
 */

enum {
	CODESEG = 1,
	DATASEG,
	TEXTSEG,
	UDEFSEG,
	LASTSEG,
};

/*
 * Definitions for names's
 */

enum {
	NTAB = 0,
	NCHAR,
	NTYPE,
	NMODULE,
	NVALUE,
	NLAST,
};

/*
 * Definitions for files's
 */

enum {
	FFILE = 0,
	FLIB,
	FOFFSET,
	FCODEBASE,
	FCODELEN,
	FCODEPOS,
	FDATABASE,
	FDATAPOS,
	FDATALEN,
	FTEXTBASE,
	FTEXTPOS,
	FTEXTLEN,
	FUDEFBASE,
	FUDEFPOS,
	FUDEFLEN,
	FLAST,
};

/*
 * Definitions for .OLB header
 */

enum {
	LBHNAME = 0,
	LBHFILE,
	LBHLAST,
};

/*
 * Definitions for .OLB filetable
 */

enum {
	LBFNAME = 0,
	LBFLENGTH,
	LBFOFFSET,
	LBFOLDOFS,
	LBFLAST,
};

/*
 * Definitions for .OLB nametable
 */

enum {
	LBNTAB = 0,
	LBNCHAR,
	LBNLIB,
	LBNLAST,
};

/*
 * values for NTYPE
 */

enum {
	CODE = 1,
	DATA,
	TEXT,
	UDEF,
	ABS,
	UNDEF,
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

int	curpos[LASTSEG];	// Position in segment
int	curseg;			// Current segment
int	datlen;			// length of data in datbuf
int	debug;			// Debug   -d specified
int	errflag;		// True if an error has occurred
int	file1[FILEMAX*FLAST];	// files as arguments
int	file1inx;		// Index to next entry
int	file2[FILEMAX*FLAST];	// files processed
int	file2inx;		// Index to next entry
int	inphdl;			// handle for .OBJ/.OLB file
int	lbfile[FILEMAX*LBFLAST];
int	lbhdr[LBHLAST];		// .OLB header
int	lbname[NAMEMAX*LBNLAST];// Library name table
int	lishdl; 		// handle for .MAP file
int	maxpos[LASTSEG];	// Size of segment
int	name[NAMEMAX*NLAST];	// Name table
int	outhdl;			// handle for .IMG file
int	pass;			// Pass number
int	stack[STACKMAX*BPW];	// REL evaluation stack
int	stackinx;		// Poisition in stack
int	stksiz;			// Stksiz  -s specified
int	undef;			// Undef   -u specified
int	verbose;		// Verbose -v specified

char	*line;			// Pointer to current input buffer
char	datbuf[256];		// storage buffer (dynamic size is stored in 8 bits)
char	inpfn[PATHMAX];		// input filename
char	lisfn[PATHMAX];		// listing filename
char	outfn[PATHMAX];		// output filename

extern char *stdout;
//int exit(int code);
//int fclose(int hdl);
//int fgets(char *str, int siz, int hdl);
//int fopen(char *name, char *mode);
//int fputc(int ch, int hdl);
//int fread(char *buf, int siz, int cnt, int hdl);
//int fseek(int hdl, int pos, int whence);
//int fwrite(char *buf, int siz, int cnt, int hdl);
//int strcpy(char *dst, char *src);
//int strlen(char *str);
//int strncpy(char *dst, int n, char *src);

/*
 *
 */
fatal(char *msg) {
	fputs(msg, stdout);
	fputc('\n', stdout);
	exit(1);
}

/*
 *
 */
lbsoutname(register int hash, register char *str) {
	register int i;

	i = lbname[hash * LBNLAST + LBNTAB];
	if (i)
		str = lbsoutname(i, str);
	*str++ = lbname[hash * LBNLAST + LBNCHAR];
	*str = 0;
	return str;
}

/*
 * Get the (unique) hashed value for symbol, return length
 */
lbdohash(register char *ident, int *retval) {
	register int start, hash, tab, len, *p;

	tab = 0;
	len = 0;
	hash = 0;
	while (*ident) {
		start = hash = (hash + *ident * *ident) % lbhdr[LBHNAME];
		while (1) {
			p = &lbname[hash * LBNLAST];
			if ((p[LBNCHAR] == *ident) && (p[LBNTAB] == tab)) {
				tab = hash;
				break; // Inner loop
			} else if (!p[LBNCHAR]) {
				p[LBNCHAR] = *ident;
				p[LBNTAB] = tab;
				tab = hash;
				break; // Inner loop
			} else {
				hash += *ident;
				if (hash >= lbhdr[LBHNAME])
					hash -= lbhdr[LBHNAME];
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
foutname(register int hash) {
	register int i;

	i = name[hash * NLAST + NTAB];
	if (i)
		i = foutname(i); // display and get length string
	else
		i = 0; // nothing displayed yet
	fputc(name[hash * NLAST + NCHAR], lishdl);
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
		start = hash = (hash + *ident * *ident) % NAMEMAX;
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
				if (hash >= NAMEMAX)
					hash -= NAMEMAX;
				if (hash == start)
					fatal("name table overflow");
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
objerr(register int *fp, char *msg, int curseg) {
	fputs(msg, stdout);
	fputs(" at ", stdout);
	if (curseg == CODESEG) {
		fputc('C', stdout);
		fput0x(fp[FCODEPOS], 4, stdout);
	} else if (curseg == DATASEG) {
		fputc('D', stdout);
		fput0x(fp[FDATAPOS], 4, stdout);
	} else if (curseg == TEXTSEG) {
		fputc('T', stdout);
		fput0x(fp[FTEXTPOS], 4, stdout);
	} else {
		fputc('U', stdout);
		fput0x(fp[FUDEFPOS], 4, stdout);
	}
	fputs(" in ", stdout);
	fputs(inpfn, stdout);
	fputc('\n', stdout);
	errflag = 1;
}

/*
 *
 */
read_byte() {
	char arr[1];

	if (fread(arr, 1, 1, inphdl) != 1)
		fatal("missing .END (use -v to discover where)\n");

	// return unsigned
	return arr[0] & 0xff;
}

/*
 *
 */
read_word() {
	char arr[2];
	int w;

	if (fread(arr, 1, 2, inphdl) != 2)
		fatal("missing .END (use -v to discover where)\n");

	// get word
	w = (arr[1] << 8) | (arr[0] & 0xff);
	// sign extend
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

	arr[0] = word;
	arr[1] = word >> 8;

	fwrite(arr, 1, 2, outhdl);
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

	if (verbose) {
		fputs("Loading library ", stdout);
		fputs(inpfn, stdout);
		fputc('\n', stdout);
	}

	inphdl = open_file(inpfn, "r");

	for (i = 0; i < LBHLAST; ++i)
		lbhdr[i] = read_word();
	if (lbhdr[LBHNAME] > NAMEMAX)
		fatal("name table too large in .OLB\n");
	if (lbhdr[LBHFILE] > FILEMAX)
		fatal("file table too large in .OLB\n");
	for (i = 0; i < lbhdr[LBHNAME] * LBNLAST; ++i)
		lbname[i] = read_word();
	if (lbhdr[LBHFILE] > 0)
		for (i = 0; i < lbhdr[LBHFILE] * LBFLAST; ++i)
			lbfile[i] = read_word();

	if (verbose)
		fputs("Loaded\n", stdout);
}

/*
 * Unsigned compare GT
 * Trying all possibilities reveals:
 * unsigned "i>j" can be rewritten as "(j^i)&0x8000 ?  i&0x8000 : (j-i)&0x8000"
 */
unsigned_GT(int i, int j) {
	return ((j ^ i) & (1 << SBIT) ? i & (1 << SBIT) : (j - i) & (1 << SBIT));
}

/*
 *
 */
save_seg_size(register int *fp) {
	if (unsigned_GT(fp[FCODEPOS], fp[FCODELEN]))
		fp[FCODELEN] = fp[FCODEPOS];
	if (unsigned_GT(fp[FDATAPOS], fp[FDATALEN]))
		fp[FDATALEN] = fp[FDATAPOS];
	if (unsigned_GT(fp[FTEXTPOS], fp[FTEXTLEN]))
		fp[FTEXTLEN] = fp[FTEXTPOS];
	if (unsigned_GT(fp[FUDEFPOS], fp[FUDEFLEN]))
		fp[FUDEFLEN] = fp[FUDEFPOS];
}


/*
 *
 */
objmap() {
	register int i, *p, len;

	fputs("                                                CODE       DATA       TEXT       UDEF   \n", lishdl);
	fputs("id         module             library         BASE LEN   BASE LEN   BASE LEN   BASE LEN \n", lishdl);
	fputs("-- -------------------- --------------------  ---- ----  ---- ----  ---- ----  ---- ----\n", lishdl);

	for (i = 0; i < file2inx; ++i) {
		p = &file2[i * FLAST];
		if (i+1 < 10)
			fputc(' ', lishdl);
		fputd(i+1, lishdl);
		fputc(' ', lishdl);
		len = foutname(p[FFILE]);
		while (len++ <= 20)
			fputc(' ', lishdl);
		if (p[FLIB] == -1)
			fputs("                     ", lishdl);
		else {
			len = foutname(p[FLIB]);
			while (len++ <= 20)
				fputc(' ', lishdl);
		}
		fputs(" ", lishdl);
		fput0x(p[FCODEBASE], 4, lishdl);
		fputc(' ', lishdl);
		fput0x(p[FCODELEN], 4, lishdl);

		fputs("  ", lishdl);
		fput0x(p[FDATABASE], 4, lishdl);
		fputc(' ', lishdl);
		fput0x(p[FDATALEN], 4, lishdl);

		fputs("  ", lishdl);
		fput0x(p[FTEXTBASE], 4, lishdl);
		fputc(' ', lishdl);
		fput0x(p[FTEXTLEN], 4, lishdl);

		fputs("  ", lishdl);
		fput0x(p[FUDEFBASE], 4, lishdl);
		fputc(' ', lishdl);
		fput0x(p[FUDEFLEN], 4, lishdl);

		fputc('\n', lishdl);
	}
}

/*
 *
 */
symmap(register int start) {
	register int ch, *p, hash, tab;

	tab = (!start) ? 0 : start;
	for (ch = '!'; ch <= '~'; ++ch) {
		hash = (start + ch * ch) % NAMEMAX;
		while (1) {
			p = &name[hash * NLAST];
			if ((p[NCHAR] == ch) && (p[NTAB] == tab)) {
				if (p[NTYPE]) {
					if (p[NMODULE] + 1 < 10)
						fputc(' ', lishdl);
					fputd(p[NMODULE] + 1, lishdl);
					fputc(' ', lishdl);
					fput0x(p[NVALUE], 4, lishdl);
					fputc(' ', lishdl);
					if (p[NTYPE] == UNDEF)
						fputs("**** ", lishdl);
					else if (p[NTYPE] == ABS)
						fputs("ABS  ", lishdl);
					else if (p[NTYPE] == CODE)
						fputs("CODE ", lishdl);
					else if (p[NTYPE] == DATA)
						fputs("DATA ", lishdl);
					else if (p[NTYPE] == TEXT)
						fputs("TEXT ", lishdl);
					else if (p[NTYPE] == UDEF)
						fputs("UDEF ", lishdl);
					else
						fputs("????", lishdl);
					foutname(hash);
					fputc('\n', lishdl);
				}
				symmap(hash);
				break; // Inner loop
			} else if (!p[NCHAR]) {
				break; // Inner loop
			} else {
				hash += ch;
				if (hash >= NAMEMAX)
					hash -= NAMEMAX;
			}
		}
	}
}

/*
 *
 */
dopass1(int fileid, int libid, int libofs) {
	register int *p, *fp, curseg;
	int symseg, symofs, hash;
	char cmd;

	fp = &file2[file2inx++ * FLAST];
	if (file2inx >= FILEMAX)
		fatal("too many files");
	fp[FFILE] = fileid;
	fp[FLIB] = libid;
	fp[FOFFSET] = libofs;
	fp[FCODEBASE] = fp[FCODEPOS] = fp[FCODELEN] = 0;
	fp[FDATABASE] = fp[FDATAPOS] = fp[FDATALEN] = 0;
	fp[FTEXTBASE] = fp[FTEXTPOS] = fp[FTEXTLEN] = 0;
	fp[FUDEFBASE] = fp[FUDEFPOS] = fp[FUDEFLEN] = 0;

	curseg = CODESEG;
	if (verbose) {
		soutname(fileid, datbuf);
		fputs("processing ", stdout);
		fputs(datbuf, stdout);
		if (libid != -1) {
			soutname(libid, datbuf);
			fputc('(', stdout);
			fputs(datbuf, stdout);
			fputc(')', stdout);
		}
		fputc('\n', stdout);
	}
	while (1) {
		cmd = read_byte();
		if (cmd < 0) {
			datlen = -cmd;
			fread(datbuf, 1, datlen, inphdl);
			if (curseg == CODESEG)
				fp[FCODEPOS] += datlen;
			else if (curseg == DATASEG)
				fp[FDATAPOS] += datlen;
			else if (curseg == TEXTSEG)
				fp[FTEXTPOS] += datlen;
			else
				fp[FUDEFPOS] += datlen;
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
				// skip basic stack operations
				break;
			case REL_PUSHB:
			case REL_CODEB:
			case REL_DATAB:
			case REL_TEXTB:
			case REL_UDEFB:
				// skip byte length stack operations
				read_byte();
				break;
			case REL_PUSHW:
			case REL_CODEW:
			case REL_DATAW:
			case REL_TEXTW:
			case REL_UDEFW:
				// skip BPW length stack operations
				read_word();
				break;
			case REL_SYMBOL:
				// Push symbol value on stack
				datlen = read_byte(); // length
				fread(datbuf, 1, datlen, inphdl); // symbol
				datbuf[datlen] = 0;
				dohash(datbuf, &hash);
				p = &name[hash * NLAST];
				if (!p[NTYPE])
					p[NTYPE] = UNDEF;
				if (debug) {
					fputs("SYMBOL: ", stdout);
					fputs(datbuf, stdout);
					fputc('\n', stdout);
				}
				break;
			case REL_POPB:
				// increase curpos with 1
				if (curseg == CODESEG)
					fp[FCODEPOS] += 1;
				else if (curseg == DATASEG)
					fp[FDATAPOS] += 1;
				else if (curseg == TEXTSEG)
					fp[FTEXTPOS] += 1;
				else
					fp[FUDEFPOS] += 1;
				break;
			case REL_POPW:
				// increase curpos with BPW
				if (curseg == CODESEG)
					fp[FCODEPOS] += BPW;
				else if (curseg == DATASEG)
					fp[FDATAPOS] += BPW;
				else if (curseg == TEXTSEG)
					fp[FTEXTPOS] += BPW;
				else
					fp[FUDEFPOS] += BPW;
				break;
			case REL_DSB:
				// skip specified number of bytes in current segment
				symofs = read_word(); // skipcount
				if (curseg == CODESEG)
					fp[FCODEPOS] += symofs;
				else if (curseg == DATASEG)
					fp[FDATAPOS] += symofs;
				else if (curseg == TEXTSEG)
					fp[FTEXTPOS] += symofs;
				else
					fp[FUDEFPOS] += symofs;
				break;
			case REL_END:
				save_seg_size(fp);
				return;
			case REL_CODEDEF:
			case REL_DATADEF:
			case REL_TEXTDEF:
			case REL_UDEFDEF:
				// symbol definition
				if (cmd == REL_CODEDEF)
					symseg = CODE;
				else if (cmd == REL_DATADEF)
					symseg = DATA;
				else if (cmd == REL_TEXTDEF)
					symseg = TEXT;
				else
					symseg = UDEF;
				symofs = read_word(); // symbol offset
				datlen = read_byte(); // length
				fread(datbuf, 1, datlen, inphdl); // symbol
				datbuf[datlen] = 0;
				dohash(datbuf, &hash);
				p = &name[hash * NLAST];
				if (!p[NTYPE])
					p[NTYPE] = UNDEF;
				if (p[NTYPE] == UNDEF) {
					// new symbol
					p[NTYPE] = symseg;
					p[NMODULE] = file2inx - 1;
					p[NVALUE] = symofs;
				} else {
					fputs("Symbol '", stdout);
					fputs(datbuf, stdout);
					fputs("' doubly defined", stdout);
					objerr(fp, "", symseg);
				}
				if (debug) {
					fputs("SYMDEF: ", stdout);
					fputs(datbuf, stdout);
					fputc(' ', stdout);
					fputd(symseg, stdout);
					fputc(':', stdout);
					fput0x(symofs, 4, stdout);
					fputc('\n', stdout);
				}
				break;
			case REL_CODEORG:
			case REL_DATAORG:
			case REL_TEXTORG:
			case REL_UDEFORG:
				symofs = read_word(); // segment offset
				save_seg_size(fp);
				if (cmd == REL_CODEORG) {
					curseg = CODESEG;
					fp[FCODEPOS] = symofs;
				} else if (cmd == REL_DATAORG) {
					curseg = DATASEG;
					fp[FDATAPOS] = symofs;
				} else if (cmd == REL_TEXTORG) {
					curseg = TEXTSEG;
					fp[FTEXTPOS] = symofs;
				} else {
					curseg = UDEFSEG;
					fp[FUDEFPOS] = symofs;
				}
				if (debug) {
					fputs("ORG: ", stdout);
					fputd(curseg, stdout);
					fputc(':', stdout);
					fput0x(symofs, 4, stdout);
					fputc('\n', stdout);

				}
				break;
			default:
				fputs("\"unknown command ", stdout);
				fput0x(cmd & 0xff, 2, stdout);
				objerr(fp, "", curseg);
				exit(1);
				break;
			}
		}
	}
}

/*
 *
 */
dopass2(register int *fp) {
	register int i, lval, rval;
	int curseg, symofs, hash, val;
	char cmd, cval;

	curseg = CODESEG;
	stackinx = 0;
	fseek(outhdl, fp[FCODEBASE], 0);

	if (verbose) {
		soutname(fp[FFILE], datbuf);
		fputs("processing ", stdout);
		fputs(datbuf, stdout);
		if (fp[FLIB] != -1) {
			soutname(fp[FLIB], datbuf);
			fputc('(', stdout);
			fputs(datbuf, stdout);
			fputc(')', stdout);
		}
		fputc('\n', stdout);
	}

	while (1) {
		cmd = read_byte();
		if (cmd < 0) {
			datlen = -cmd;
			fread(datbuf, 1, datlen, inphdl);
			fwrite(datbuf, 1, datlen, outhdl);
			if (curseg == CODESEG)
				fp[FCODEPOS] += datlen;
			else if (curseg == DATASEG)
				fp[FDATAPOS] += datlen;
			else if (curseg == TEXTSEG)
				fp[FTEXTPOS] += datlen;
			else {
				objerr(fp, "data in UDEF segment", curseg);
				exit(1);
			}
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
					rval = stack[--stackinx];
					lval = stack[--stackinx];
					if (stackinx < 0) objerr(fp, "stack underflow", curseg);
					switch (cmd) {
						case REL_ADD:
							lval += rval;
							break;
						case REL_SUB:
							lval -= rval;
							break;
						case REL_MUL:
							lval *= rval;
							break;
						case REL_DIV:
							lval /= rval;
							break;
						case REL_MOD:
							lval %= rval;
							break;
						case REL_LSR:
							lval >>= rval;
							break;
						case REL_LSL:
							lval <<= rval;
							break;
						case REL_XOR:
							lval ^= rval;
							break;
						case REL_AND:
							lval &= rval;
							break;
						case REL_OR:
							lval |= rval;
							break;
					}
					stack[stackinx++] = lval;
					break;
				case REL_NOT:
				case REL_NEG:
					lval = stack[--stackinx];
					if (stackinx < 0) objerr(fp, "stack underflow", curseg);
					if (cmd == REL_NOT)
						lval = ~lval;
					else
						lval = -lval;
					stack[stackinx++] = lval;
					break;
				case REL_SWAP:
					lval = stack[--stackinx];
					rval = stack[--stackinx];
					if (stackinx < 0) objerr(fp, "stack underflow", curseg);
					stack[stackinx++] = lval;
					stack[stackinx++] = rval;
					break;
				case REL_PUSHB:
				case REL_CODEB:
				case REL_DATAB:
				case REL_TEXTB:
				case REL_UDEFB:
					val = read_byte();
					if (cmd == REL_PUSHB)
						stack[stackinx] = val;
					else if (cmd == REL_CODEB)
						stack[stackinx] = fp[FCODEBASE] + val;
					else if (cmd == REL_DATAB)
						stack[stackinx] = fp[FDATABASE] + val;
					else if (cmd == REL_TEXTB)
						stack[stackinx] = fp[FTEXTBASE] + val;
					else
						stack[stackinx] = fp[FUDEFBASE] + val;
					stackinx += 1;
					if (stackinx >= STACKMAX) objerr(fp, "stack overflow", curseg);
					break;
				case REL_PUSHW:
				case REL_CODEW:
				case REL_DATAW:
				case REL_TEXTW:
				case REL_UDEFW:
					val = read_word();
					if (cmd == REL_PUSHW)
						stack[stackinx] = val;
					else if (cmd == REL_CODEW)
						stack[stackinx] = fp[FCODEBASE] + val;
					else if (cmd == REL_DATAW)
						stack[stackinx] = fp[FDATABASE] + val;
					else if (cmd == REL_TEXTW)
						stack[stackinx] = fp[FTEXTBASE] + val;
					else
						stack[stackinx] = fp[FUDEFBASE] + val;
					stackinx += 1;
					if (stackinx >= STACKMAX) objerr(fp, "stack overflow", curseg);
					break;
				case REL_SYMBOL:
					// Push symbol value on stack
					datlen = read_byte(); // length
					fread(datbuf, 1, datlen, inphdl); // symbol
					datbuf[datlen] = 0;
					dohash(datbuf, &hash);

					stack[stackinx++] = name[hash * NLAST + NVALUE];
					if (stackinx >= STACKMAX) objerr(fp, "stack overflow", curseg);
					break;
				case REL_POPB:
					// pop byte from stack
					stackinx -= 1;
					if (stackinx < 0) objerr(fp, "stack underflow", curseg);
					if ((stack[stackinx] < -128) || (stack[stackinx] > 127))
						objerr(fp, "byte overflow", curseg);
					write_byte(stack[stackinx]);

					// increase curpos with 1
					if (curseg == CODESEG)
						fp[FCODEPOS] += 1;
					else if (curseg == DATASEG)
						fp[FDATAPOS] += 1;
					else if (curseg == TEXTSEG)
						fp[FTEXTPOS] += 1;
					else {
						objerr(fp, "data in UDEF segment", curseg);
						exit(1);
					}
					break;
				case REL_POPW:
					// pop word from stack
					stackinx -= 1;
					if (stackinx < 0) objerr(fp, "stack underflow", curseg);
					write_word(stack[stackinx]);

					// increase curpos with BPW
					if (curseg == CODESEG)
						fp[FCODEPOS] += BPW;
					else if (curseg == DATASEG)
						fp[FDATAPOS] += BPW;
					else if (curseg == TEXTSEG)
						fp[FTEXTPOS] += BPW;
					else {
						objerr(fp, "data in UDEF segment", curseg);
						exit(1);
					}
					break;
				case REL_DSB:
					// skip specified number of bytes in current segment
					symofs = read_word(); // skipcount
					if (curseg == CODESEG) {
						fp[FCODEPOS] += symofs;
						i = symofs;
					} else if (curseg == DATASEG) {
						fp[FDATAPOS] += symofs;
						i = symofs;
					} else if (curseg == TEXTSEG) {
						fp[FTEXTPOS] += symofs;
						i = symofs;
					} else {
						fp[FUDEFPOS] += symofs;
						i = 0;
					}

					datbuf[0] = datbuf[1] = datbuf[2] = datbuf[3] = 0;
					while (i > 4) {
						fwrite(datbuf, 1, 4, outhdl);
						i -= 4;
					}
					while (i > 0) {
						fwrite(datbuf, 1, 1, outhdl);
						i -= 1;
					}
					break;
				case REL_END:
					if (stackinx) {
						fputs("stack not properly released in ", stdout);
						fputs(inpfn, stdout);
						fputc('\n', stdout);
						exit(1);
					}
					return;
				case REL_CODEDEF:
				case REL_DATADEF:
				case REL_TEXTDEF:
				case REL_UDEFDEF:
					// symbol definition (skipped in pass2)
					symofs = read_word() & 0xffff; // symbol offset
					datlen = read_byte() & 0xff; // length
					fread(datbuf, 1, datlen, inphdl); // symbol
					break;
				case REL_CODEORG:
				case REL_DATAORG:
				case REL_TEXTORG:
				case REL_UDEFORG:
					symofs = read_word(); // segment offset
					if (cmd == REL_CODEORG) {
						curseg = CODESEG;
						fp[FCODEPOS] = symofs;
						i = fp[FCODEBASE] + symofs;
					} else if (cmd == REL_DATAORG) {
						curseg = DATASEG;
						fp[FDATAPOS] = symofs;
						i = fp[FDATABASE] + symofs;
					} else if (cmd == REL_TEXTORG) {
						curseg = TEXTSEG;
						fp[FTEXTPOS] = symofs;
						i = fp[FTEXTBASE] + symofs;
					} else {
						curseg = UDEFSEG;
						fp[FUDEFPOS] = symofs;
						i = 0;
					}
					fseek(outhdl, i, 0);
					break;
				default:
					fputs("\"unknown command ", stdout);
					fput0x(cmd & 0xff, 2, stdout);
					objerr(fp, "", curseg);
					exit(1);
					break;
			}
		}
	}
}

/*
 *
 */
doreloc() {
	register int i, curpos, curlen, *p;
	int hash;

	dohash("___CODEBASE", &hash);
	name[hash * NLAST + NVALUE] = 0;
	curpos = 4;  // Reserve space for JMP ___START
	curlen = 0;

	// relocate CODE
	for (i = 0; i < file2inx; ++i) {
		p = &file2[i * FLAST];
		if (p[FFILE] != -1) {
			p[FCODEBASE] = curpos;
			curpos += p[FCODELEN];
			curlen += p[FCODELEN];
		}
	}

	dohash("___CODELEN", &hash);
	name[hash * NLAST + NVALUE] = curlen;
	dohash("___DATABASE", &hash);
	name[hash * NLAST + NVALUE] = curpos;
	curlen = 0;

	// relocate DATA
	for (i = 0; i < file2inx; ++i) {
		p = &file2[i * FLAST];
		if (p[FFILE] != -1) {
			p[FDATABASE] = curpos;
			curpos += p[FDATALEN];
			curlen += p[FDATALEN];
		}
	}

	dohash("___DATALEN", &hash);
	name[hash * NLAST + NVALUE] = curlen;
	dohash("___TEXTBASE", &hash);
	name[hash * NLAST + NVALUE] = curpos;
	curlen = 0;

	// relocate TEXT
	for (i = 0; i < file2inx; ++i) {
		p = &file2[i * FLAST];
		if (p[FFILE] != -1) {
			p[FTEXTBASE] = curpos;
			curpos += p[FTEXTLEN];
			curlen += p[FTEXTLEN];
		}
	}

	dohash("___TEXTLEN", &hash);
	name[hash * NLAST + NVALUE] = curlen;
	dohash("___UDEFBASE", &hash);
	name[hash * NLAST + NVALUE] = curpos;
	curlen = 0;

	// relocate UDEF
	for (i = 0; i < file2inx; ++i) {
		p = &file2[i * FLAST];
		if (p[FFILE] != -1) {
			p[FUDEFBASE] = curpos;
			curpos += p[FUDEFLEN];
			curlen += p[FUDEFLEN];
		}
	}

	dohash("___UDEFLEN", &hash);
	name[hash * NLAST + NVALUE] = curlen;

	// redefine ___STACKLEN
	dohash("___STACKLEN", &hash);
	p = &name[hash * NLAST];
	p[NVALUE] = stksiz;

	// relocate all symbols
	for (i = 0; i < NAMEMAX; ++i) {
		p = &name[i * NLAST];
		if (p[NTYPE] == CODE)
			p[NVALUE] += file2[p[NMODULE] * FLAST + FCODEBASE];
		else if (p[NTYPE] == DATA)
			p[NVALUE] += file2[p[NMODULE] * FLAST + FDATABASE];
		else if (p[NTYPE] == TEXT)
			p[NVALUE] += file2[p[NMODULE] * FLAST + FTEXTBASE];
		else if (p[NTYPE] == UDEF)
			p[NVALUE] += file2[p[NMODULE] * FLAST + FUDEFBASE];
	}
}

/*
 *
 */
process() {
	register int i, j, *fp, *p, len;
	int hash, found;

	pass = 1;
	if (verbose)
		fputs("Pass 1\n", stdout);

	// process pass 1
	for (i = 0; i < file1inx; ++i) {
		fp = &file1[i * FLAST];
		if (fp[FLIB] == -1) {
			// process object
			soutname(fp[FFILE], inpfn);
			inphdl = open_file(inpfn, "r");
			dopass1(fp[FFILE], fp[FLIB], 0);
			fclose(inphdl);
		} else {
			// process library
			soutname(fp[FLIB], inpfn);
			open_olb();
			found = 0;
			while (1) {
				for (j = 0; j < NAMEMAX; ++j) {
					p = &name[j * NLAST];
					if (p[NTYPE] == UNDEF) {
						// found undefined symbol, test if in library
						soutname(j, datbuf);
						lbdohash(datbuf, &hash);
						p = &lbname[hash * LBNLAST];
						if (p[LBNLIB] != -1) {
							// found module containing symbol definition
							p = &lbfile[p[LBNLIB] * LBFLAST];
							fseek(inphdl, p[LBFOFFSET], 0);
							// add module name to linker symboltable
							lbsoutname(p[LBFNAME], datbuf);
							dohash(datbuf, &hash);
							// process
							dopass1(hash, fp[FLIB], p[LBFOFFSET]);
							found = 1;
						}
					}
				}
				// if all symbols tested, test for another load
				if (!found)
					break;
				found = 0;
			}
			fclose(inphdl);
		}
	}

	// relocate modules
	doreloc();

	// test for undefined symbols
	if (!undef) {
		j = 0;
		for (i = 0; i < NAMEMAX; ++i) {
			p = &name[i * NLAST];
			if (p[NTYPE] == UNDEF) {
				if (!j) {
					fputs("Undefined symbols :\n", stdout);
					errflag = 1;
					j = 1;
				}
				outname(i);
				fputc('\n', stdout);
			}
		}
	}

	pass = 2;
	if (verbose)
		fputs("Pass 2\n", stdout);

	// generate prefix "JMP ___START"
	datbuf[0] = 0x50;  // opcode for "jz.a"
	dohash("___START", &hash);
	datbuf[1] = 0; // jz.a r0,imm(r0)
	datbuf[2] = name[hash * NLAST + NVALUE]; // lo
	datbuf[3] = name[hash * NLAST + NVALUE] >> 8; // hi
	fwrite(datbuf, 1, 4, outhdl);

	// process pass 2
	for (i = 0; i < file2inx; ++i) {
		fp = &file2[i * FLAST];
		if (fp[FLIB] == -1) {
			// process object
			soutname(fp[FFILE], inpfn);
			inphdl = open_file(inpfn, "r");
			dopass2(fp);
			fclose(inphdl);
		} else {
			// process library
			soutname(fp[FLIB], inpfn);
			inphdl = open_file(inpfn, "r");
			while (1) {
				fseek(inphdl, fp[FOFFSET], 0);
				dopass2(fp);
				// test if next module comes from same library
				if (((i + 1) >= file2inx) || (file2[(i + 1) * FLAST + FLIB] != fp[FLIB]))
					break;
				// reinit fp and redo loop
				fp = &file2[++i * FLAST];
			}
			fclose(inphdl);
		}
	}
}

/*
 *
 */
add_res(char *sym, int typ) {
	int hash;
	register int *p;

	dohash(sym, &hash);
	p = &name[hash * NLAST];
	p[NTYPE] = typ;
	p[NMODULE] = -1;
	p[NVALUE] = 0;
}

/*
 *
 */
initialize() {
	register int i, *p;
	int hash;

	undef = verbose = debug = 0;
	stksiz = 2000;
	outhdl = lishdl = inphdl = 0;
	inpfn[0] = outfn[0] = 0;
	datlen = 0;
	file1inx = file2inx = 0;

	// reset tables
	for (i = 0; i < NAMEMAX; ++i) {
		p = &name[i * NLAST];
		p[NCHAR] = p[NTYPE] = p[NVALUE] = 0;
	}

	// reserve first entry so it terminates lists
	name[0 * NLAST + NCHAR] = '?';

	// reset positions
	pass = 1;

	// predefined symbols
	add_res("___START", UNDEF);
	add_res("___STACKLEN", ABS);
	add_res("___CODEBASE", ABS);
	add_res("___CODELEN", ABS);
	add_res("___DATABASE", ABS);
	add_res("___DATALEN", ABS);
	add_res("___TEXTBASE", ABS);
	add_res("___TEXTLEN", ABS);
	add_res("___UDEFBASE", ABS);
	add_res("___UDEFLEN", ABS);
}

/*
 * Process commandline
 */
usage() {
	fputs("X-Linker, Version ", stdout);
	fputs(getversion(), stdout);
	fputs("\n\n", stdout);

	fputs("usage: xlnk <file>[.<ext>] ...\n", stdout);
	fputs("  -l <file>[.<ext>]]\tLibrary\n", stdout);
	fputs("  -o <file>[.<ext>]]\tImage output\n", stdout);
	fputs("  -m <file>[.<ext>]]\tMap output\n", stdout);
	fputs("  -s <stksiz>\t\tStack size\n", stdout);
	fputs("  -u\t\t\tIgnore undefined\n", stdout);
	fputs("  -v\t\t\tVerbose\n", stdout);
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
	int hash, *p;

	++argv; // skip argv[0]
	while (*argv) {
		register char *arg;
		arg = *argv++;

		if (*arg != '-') {
			fext(inpfn, arg, ".xo", 0);
			if (!outfn[0])
				fext(outfn, arg, ".img", 1);

			// Insert object into filetable
			if (file1inx >= FILEMAX)
				fatal("too many files");

			p = &file1[file1inx++ * FLAST];
			dohash(inpfn, &hash);
			p[FFILE] = hash;
			p[FLIB] = -1;
			p[FCODEBASE] = p[FCODEPOS] = p[FCODELEN] = 0;
			p[FDATABASE] = p[FDATAPOS] = p[FDATALEN] = 0;
			p[FTEXTBASE] = p[FTEXTPOS] = p[FTEXTLEN] = 0;
			p[FUDEFBASE] = p[FUDEFPOS] = p[FUDEFLEN] = 0;
		} else {
			// Process option
			++arg;
			switch (*arg++) {
			case 'd':
				debug = 1;
				break;
			case 'l':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();
				else
					fext(inpfn, arg, ".xa", 0);

				// Insert file into filetable
				if (file1inx >= FILEMAX)
					fatal("too many files");

				p = &file1[file1inx++ * FLAST];
				dohash(inpfn, &hash);
				p[FFILE] = -1;
				p[FLIB] = hash;
				break;
			case 'm':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();
				else
					fext(lisfn, arg, ".map", 0);
				break;
			case 'o':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();
				else
					fext(outfn, arg, ".img", 0);
				break;
			case 's':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();

				// load value
				stksiz = 0;
				while (*arg >= '0' && *arg <= '9')
					stksiz = stksiz * 10 + *arg++ - '0';
				break;
			case 'u':
				undef = 1;
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

	if (!outfn[0])
		usage();
}

/*
 * Program start
 */
main(int argc, int *argv) {
	register int i, j, *p, len;
	int hash;

	initialize(); // initialize all variables

	startup(argv);       // Process commandline options

	outhdl = open_file(outfn, "w");
	if (lisfn[0])
		lishdl = open_file(lisfn, "w");

	process();           // Start linking

	if (lishdl) {

		fputs("Object statistics : \n", lishdl);
		objmap();
		fputs("\nSymboltable : \n\n", lishdl);
		symmap(0);
	}

	if (lishdl) {
		j = 0;
		for (i = 0; i < NAMEMAX; ++i) if (name[i * NLAST + NCHAR]) ++j;
		fputs("Names        : ", lishdl);
		fputd(j, lishdl);
		fputc('/', lishdl);
		fputd(NAMEMAX, lishdl);
		fputc('\n', lishdl);
	}

	return errflag;
}

