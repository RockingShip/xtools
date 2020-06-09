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
	NAMEMAX = 2503,		// Size of nametable !!! MUST BE PRIME !!!
	PATHMAX = 80,		// Length of filename
	SBUFMAX = 256,		// Size of source buffer
};

/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,		// Bytes per word
	SBIT = 15,		// Bit number of signed bit
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
	NVALUE,
	NLAST,
};

/*
 * Definitions for lval's
 */

enum {
	LTYPE = 0,
	LVALUE,
	LLAST,
};

/*
 * values for NTYPE
 */

enum {
	UNDEF = 1,
	ABS,
	CODE,
	DATA,
	TEXT,
	UDEF,
	OPCODE,
	PSEUDO,
	REGISTER,
	LINK,
	POINT,
};

/*
 * values for LTYPE
 */

enum {
	CONSTANT = 1,
	SYMBOL,
	EXPRESSION,
};

/*
 * Compiler-generated p-codes
 */

enum {
	OPC_ILLEGAL = 0x00,
	OPC_ADD = 0x13,
	OPC_SUB = 0x12,
	OPC_MUL = 0x14,
	OPC_DIV = 0x15,
	OPC_MOD = 0x16,
	OPC_OR = 0x17,
	OPC_XOR = 0x19,
	OPC_AND = 0x18,
	OPC_LSR = 0x1B,
	OPC_LSL = 0x1A,
	OPC_NEG = 0x1D,
	OPC_NOT = 0x1C,
	OPC_LDB = 0x04,
	OPC_LDW = 0x01,
	OPC_LDR = 0x11,
	OPC_LEA = 0x03,
	OPC_STB = 0x05,
	OPC_STW = 0x02,
	OPC_JSB = 0x20,
	OPC_RSB = 0x21,
	OPC_PSHR = 0x23,
	OPC_POPR = 0x24,
	OPC_PSHB = 0x25,
	OPC_PSHW = 0x26,
	OPC_PSHA = 0x27,
	OPC_SVC = 0x0A,

	OPC_MULA = 0x40 + 0,
	OPC_MULB = 0x40 + 1,
	OPC_MULW = 0x40 + 2,
	OPC_DIVA = 0x44 + 0,
	OPC_DIVB = 0x44 + 1,
	OPC_DIVW = 0x44 + 2,
	OPC_MODA = 0x48 + 0,
	OPC_MODB = 0x48 + 1,
	OPC_MODW = 0x48 + 2,
	OPC_ADDA = 0x4c + 0,
	OPC_ADDB = 0x4c + 1,
	OPC_ADDW = 0x4c + 2,
	OPC_SUBA = 0x50 + 0,
	OPC_SUBB = 0x50 + 1,
	OPC_SUBW = 0x50 + 2,
	OPC_LSLA = 0x54 + 0,
	OPC_LSLB = 0x54 + 1,
	OPC_LSLW = 0x54 + 2,
	OPC_LSRA = 0x58 + 0,
	OPC_LSRB = 0x58 + 1,
	OPC_LSRW = 0x58 + 2,
OPC_ANDA = 0x5c + 0,
	OPC_XORA = 0x60 + 0,
	OPC_XORB = 0x60 + 1,
	OPC_XORW = 0x60 + 2,
OPC_ORA = 0x64 + 0,
	OPC_SGTA = 0x68 + 0,
	OPC_SGTB = 0x68 + 1,
	OPC_SGTW = 0x68 + 2,
	OPC_SLTA = 0x6c + 0,
	OPC_SLTB = 0x6c + 1,
	OPC_SLTW = 0x6c + 2,
	OPC_LODA = 0x70 + 0,
	OPC_LODB = 0x70 + 1,
	OPC_LODW = 0x70 + 2,
	OPC_STOA = 0x74 + 0,
	OPC_STOB = 0x74 + 1,
	OPC_STOW = 0x74 + 2,
	OPC_JZA = 0x78 + 0,
	OPC_JZB = 0x78 + 1,
	OPC_JZW = 0x78 + 2,
	OPC_JNZA = 0x7c + 0,
	OPC_JNZB = 0x7c + 1,
	OPC_JNZW = 0x7c + 2,
};

/*
 * Pseudo opcodes
 */

enum {
	PSEUDO_CODE = 1,
	PSEUDO_DATA,
	PSEUDO_TEXT,
	PSEUDO_UDEF,
	PSEUDO_ORG,
	PSEUDO_END,
	PSEUDO_DCB,
	PSEUDO_DCW,
	PSEUDO_DSB,
	PSEUDO_DSW,
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

enum {
	CISSPACE = 1 << 0,	// is a space
	CISDIGIT = 1 << 1,	// is a digit
	CISXDIGIT = 1 << 2,	// is a hex digit
	CISUPPER = 1 << 3,	// is lowercase
	CISLOWER = 1 << 4,	// is uppercase
	CSYMFIRST = 1 << 5,	// first character of an identifier name
	CSYMNEXT = 1 << 6,	// next character of an identifier name
};

/*
 * Storage
 */

int	curpos[LASTSEG];	// Position in segment
int	curseg;			// Current segment
int	datlen;			// length of data in datbuf
int	debug;			// Debug   -d specified
int	errflag;		// True if an error has occurred
int	inphdl;			// handle for .ASM file
int	inplnr;			// Linenumber of .C file
int	lishdl;			// handle for .LIS file
int	maxpos[LASTSEG];	// Size in segment
int	names[NAMEMAX * NLAST];	// string name table
int	outhdl;			// handle for .OBJ file
int	pass;			// Pass number
int	verbose;		// Verbose -v specified

char	ch;			// Current character in line being scanned
char	ctype[256];		// character properties
char	datbuf[128];		// storage buffer for sto_data
char	inpfn[PATHMAX];		// input filename
char	lisfn[PATHMAX];		// listing filename
char	*lptr;			// Pointer to current character in input buffer
char	nch;			// Next character in line being scanned
char	outfn[PATHMAX];		// output filename
char	sbuf[SBUFMAX];

//exit(int code);
//expression(int lval[]);
//fclose(int hdl);
//fgets(char *str, int siz, int hdl);
//fopen(char *name, char *mode);
//fprintf(int, char *, ...);
//fread(char *buf, int siz, int cnt, int hdl);
//fwrite(char *buf, int siz, int cnt, int hdl);
//printf(char *, ...);
//strcpy(char *dst, char *src);
//strncpy(char *dst, int n, char *src);
//toupper(int ch);

/*
 * Generate error messages
 */
error(char *msg) {
	// Display original line
	printf("%d: %s\n%%%s\n", inplnr, sbuf, msg);
	if (lishdl)
		fprintf(lishdl, ";%d %%%s\n", inplnr, msg);

	errflag = 1;
}

fatal(char *msg) {
	error(msg);
	exit(1);
}

exprerr() {
	error("Invalid expression");
}

/*
 *
 */
readline() {
	sbuf[0] = 0;
	if (inphdl) {
		if (!fgets(sbuf, SBUFMAX - 1, inphdl)) {
			fclose(inphdl);
			inphdl = 0;
		} else {
			sbuf[SBUFMAX - 1] = 0;
			++inplnr;

			// trim training spaces
			int i;
			i = strlen(sbuf);
			while (i > 0 && (ctype[sbuf[i - 1]] & CISSPACE))
				sbuf[--i] = 0;

			// Copy line to listing
			if (lishdl) {
				if (curseg == CODESEG)
					fprintf(lishdl, "C%04x:\t%s\n", curpos[CODESEG] & 0xffff, sbuf);
				else if (curseg == DATASEG)
					fprintf(lishdl, "D%04x:\t%s\n", curpos[DATASEG] & 0xffff, sbuf);
				else if (curseg == TEXTSEG)
					fprintf(lishdl, "T%04x:\t%s\n", curpos[TEXTSEG] & 0xffff, sbuf);
				else
					fprintf(lishdl, "U%04x:\t%s\n", curpos[UDEFSEG] & 0xffff, sbuf);
			}
		}
	}

	lptr = sbuf;
	ch = lptr[0];
	nch = ch ? lptr[1] : 0;
}

/*
 * Get next character from current line
 */
gch() {
	register int c;

	c = ch;
	if (ch)
		nch = *lptr++;
	else
		nch = 0;

	ch = lptr[0];
	nch = ch ? lptr[1] : 0;

	return c;
}

/*
 * Bump/skip next characters in line
 */
bump(int n) {
	lptr += n;
	ch = lptr[0];
	nch = ch ? lptr[1] : 0;
}

/*
 * Skip all spaces until next non-space
 */
blanks() {
	while (1) {
		if (ch == ';')
			*lptr = ch = 0;

		if (!ch || !(ctype[ch] & CISSPACE))
			break;

		gch();
	}
}

/*
 * Return current literal character and bump lptr
 */
litchar() {
	register int i, oct;

	if ((ch != '\\') || (nch == 0))
		return gch();
	gch();
	switch (ch) {
	case 'b': // bell
		gch();
		return 8;
	case 'f': // form-feed
		gch();
		return 12;
	case 'n': // newline
		gch();
		return 10;
	case 'r': // carriage return
		gch();
		return 13;
	case 't': // horizontal tab
		gch();
		return 9;
	case 'v': // vertical tab
		gch();
		return 11;
	}
	i = 0;
	oct = 0;
	while ((ch >= '0') && (ch <= '7')) {
		oct = (oct << 3) + gch() - '0';
		++i;
	}
	return i ? oct : gch();
}

/*
 * Return 'index' if start next token equals 'lit'
 */
match(register char *lit) {
	register int i;

	blanks();

	i = 0;
	while (lit[i]) {
		if (lptr[i] != lit[i])
			return 0;
		++i;
	}

	bump(i);
	return 1;
}

/*
 * Return 'index' if next token equals 'lit'
 */
amatch(register char *lit) {
	register int i;

	blanks();

	i = 0;
	while (lit[i]) {
		if (lptr[i] != lit[i])
			return 0;
		++i;
	}
	if (ctype[lptr[i]] & CSYMNEXT)
		return 0;

	bump(i);
	return 1;
}

/*
 * Return 'true' if next operator equals 'lit'
 */
omatch(register char *lit) {
	blanks();
	if (lptr[0] != lit[0])
		return 0;
	if (lit[1]) {
		if (lptr[1] != lit[1])
			return 0;
		bump(2);
	} else {
		if ((lptr[1] == '=') || (lptr[1] == lptr[0]))
			return 0;
		bump(1);
	}
	return 1;
}

foutname(register int hash) {
	register int i;

	i = names[hash * NLAST + NTAB];
	if (i)
		foutname(i);
	fprintf(outhdl, "%c", names[hash * NLAST + NCHAR]);
}

outname(register int hash) {
	register int i;

	i = names[hash * NLAST + NTAB];
	if (i)
		outname(i);
	printf("%c", names[hash * NLAST + NCHAR]);
}

lenname(register int hash) {
	register int i;

	i = names[hash * NLAST + NTAB];
	if (i)
		return lenname(i) + 1;
	return 1;
}

/*
 * Get the (unique) hashed value for symbol, return length
 */
dohash(register char *name, int *retval) {
	register int start, hash, tab, len, *p;

	if (~ctype[*name] & CSYMFIRST)
		return 0; // Not a symbol

	tab = 0;
	len = 0;
	hash = 0;
	while (ctype[*name] & CSYMNEXT) {
		start = hash = (hash + *name * *name) % NAMEMAX;
		while (1) {
			p = &names[hash * NLAST];
			if ((p[NCHAR] == *name) && (p[NTAB] == tab)) {
				tab = hash;
				break; // Inner loop
			} else if (!p[NCHAR]) {
				p[NCHAR] = *name;
				p[NTAB] = tab;
				tab = hash;
				break; // Inner loop
			} else {
				hash += *name;
				if (hash >= NAMEMAX)
					hash -= NAMEMAX;
				if (hash == start)
					fatal("name table overflow");
			}
		}
		++name;
		++len;
	}
	*retval = tab;
	return len;
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
 * Open all files
 */
open_file(char *fn, char *mode) {
	int fd;

	fd = fopen(fn, mode);
	if (fd > 0)
		return fd;
	printf("fopen(%s,%s) failed\n", fn, mode);
	exit(1);
}

/*
 *
 */
sto_flush() {
	char cval;
	register int i, j;

	if ((pass == 2) && datlen) {
		if (!debug) {
			cval = -datlen;
			write_byte(cval);
			fwrite(datbuf, 1, datlen, outhdl);
		} else {
			for (i=0; i<datlen; ++i) {
				if (i&15)
					fputc(',', outhdl);
				else if (i)
					fputc('\n', outhdl);
				fprintf(outhdl, "0x%02x", datbuf[i]);
			}
			fputc('\n', outhdl);
		}

		datlen = 0;
	}
}

/*
 *
 */
sto_data(int val, int size) {
	if (size == BPW) {
		// store upper byte
		if (datlen == 128)
			sto_flush();
		datbuf[datlen++] = val >> 8;
	}
	// store lower byte
	if (datlen == 128)
		sto_flush();
	datbuf[datlen++] = val;
}

/*
 *
 */
sto_cmd(int cmd, int val) {
	char cval;
	register int *p;

	sto_flush();
	cval = val;
	if (pass == 2) {
		if (!debug)
			switch (cmd) {
			case REL_END:
			case REL_ADD:
			case REL_SUB:
			case REL_MUL:
			case REL_DIV:
			case REL_MOD:
			case REL_LSR:
			case REL_LSL:
			case REL_AND:
			case REL_OR:
			case REL_XOR:
			case REL_SWAP:
			case REL_POPB:
			case REL_POPW:
				write_byte(cmd);
				break;
			case REL_PUSHB:
			case REL_PUSHW:
				if ((val >= -128) && (val <= 127))
					cmd = REL_PUSHB;
				if (cmd == REL_PUSHB) {
					cval = val;
					write_byte(cmd);
					write_byte(cval);
				} else {
					write_byte(cmd);
					write_word(val);
				}
				break;
			case REL_CODEB:
			case REL_CODEW:
				if ((val >= -128) && (val <= 127))
					cmd = REL_CODEB;
				if (cmd == REL_CODEB) {
					write_byte(cmd);
					write_byte(cval);
				} else {
					write_byte(cmd);
					write_word(val);
				}
				break;
			case REL_DATAB:
			case REL_DATAW:
				if ((val >= -128) && (val <= 127))
					cmd = REL_DATAB;
				if (cmd == REL_DATAB) {
					write_byte(cmd);
					write_byte(cval);
				} else {
					write_byte(cmd);
					write_word(val);
				}
				break;
			case REL_TEXTB:
			case REL_TEXTW:
				if ((val >= -128) && (val <= 127))
					cmd = REL_TEXTB;
				if (cmd == REL_TEXTB) {
					write_byte(cmd);
					write_byte(cval);
				} else {
					write_byte(cmd);
					write_word(val);
				}
				break;
			case REL_UDEFB:
			case REL_UDEFW:
				if ((val >= -128) && (val <= 127))
					cmd = REL_UDEFB;
				if (cmd == REL_UDEFB) {
					write_byte(cmd);
					write_byte(cval);
				} else {
					write_byte(cmd);
					write_word(val);
				}
				break;
			case REL_SYMBOL:
				write_byte(cmd);
				write_byte(lenname(val));
				foutname(val);
				break;
			case REL_CODEDEF:
			case REL_DATADEF:
			case REL_TEXTDEF:
			case REL_UDEFDEF:
				p = &names[val * NLAST];
				write_byte(cmd);
				write_word(p[NVALUE]);
				cval = lenname(val);
				write_byte(cval);
				foutname(val);
				break;
			case REL_CODEORG:
			case REL_DATAORG:
			case REL_TEXTORG:
			case REL_UDEFORG:
			case REL_DSB:
				write_byte(cmd);
				write_word(val);
				break;
			default:
				printf("unimplemented OBJECT cmd: %d\n", cmd);
				exit(1);
				break;
			}
		else {
			switch (cmd) {
			case REL_END: fprintf(outhdl, "end\n", val); break;
			case REL_ADD: fprintf(outhdl, "add\n"); break;
			case REL_SUB: fprintf(outhdl, "sub\n"); break;
			case REL_MUL: fprintf(outhdl, "mul\n"); break;
			case REL_DIV: fprintf(outhdl, "div\n"); break;
			case REL_MOD: fprintf(outhdl, "mod\n"); break;
			case REL_LSR: fprintf(outhdl, "lsr\n"); break;
			case REL_LSL: fprintf(outhdl, "lsl\n"); break;
			case REL_AND: fprintf(outhdl, "and\n"); break;
			case REL_OR:   fprintf(outhdl, "or\n"); break;
			case REL_XOR: fprintf(outhdl, "xor\n"); break;
			case REL_SWAP: fprintf(outhdl, "swap\n"); break;
			case REL_POPB: fprintf(outhdl, "popb\n"); break;
			case REL_POPW: fprintf(outhdl, "popw\n"); break;
			case REL_PUSHB: fprintf(outhdl, "pushb %d\n", val); break;
			case REL_PUSHW: fprintf(outhdl, "pushw %d\n", val); break;
			case REL_CODEB: fprintf(outhdl, "codeb %d\n", val); break;
			case REL_CODEW: fprintf(outhdl, "codew %d\n", val); break;
			case REL_DATAB: fprintf(outhdl, "datab %d\n", val); break;
			case REL_DATAW: fprintf(outhdl, "dataw %d\n", val); break;
			case REL_TEXTB: fprintf(outhdl, "textb %d\n", val); break;
			case REL_TEXTW: fprintf(outhdl, "textw %d\n", val); break;
			case REL_UDEFB: fprintf(outhdl, "udefb %d\n", val); break;
			case REL_UDEFW: fprintf(outhdl, "udefw %d\n", val); break;
			case REL_DSB: fprintf(outhdl, "dsb %d\n", val); break;
			case REL_CODEORG: fprintf(outhdl, "codeorg %d\n", val); break;
			case REL_DATAORG: fprintf(outhdl, "dataorg %d\n", val); break;
			case REL_TEXTORG: fprintf(outhdl, "textorg %d\n", val); break;
			case REL_UDEFORG: fprintf(outhdl, "udeforg %d\n", val); break;
			case REL_SYMBOL:
				fprintf(outhdl, "symbol ");
				foutname(val);
				fprintf(outhdl, "\n");
				break;
			case REL_CODEDEF:
				p = &names[val * NLAST];
				fprintf(outhdl, "codedef %d,", p[NVALUE]);
				foutname(val);
				fprintf(outhdl, "\n", 0);
				break;
			case REL_DATADEF:
				p = &names[val * NLAST];
				fprintf(outhdl, "datadef %d,", p[NVALUE]);
				foutname(val);
				fprintf(outhdl, "\n", 0);
				break;
			case REL_TEXTDEF:
				p = &names[val * NLAST];
				fprintf(outhdl, "textdef %d,", p[NVALUE]);
				foutname(val);
				fprintf(outhdl, "\n", 0);
				break;
			case REL_UDEFDEF:
				p = &names[val * NLAST];
				fprintf(outhdl, "udefdef %d,", p[NVALUE]);
				foutname(val);
				fprintf(outhdl, "\n", 0);
				break;
			default:
				fprintf(outhdl, "cmd: %d\n", cmd);
				break;
			}
		}
	}
}

/*
 *
 */
loadlval(register int lval[]) {
	register int *p;

	if (lval[LTYPE] == CONSTANT) {
		sto_cmd(REL_PUSHW, lval[LVALUE]);
		lval[LTYPE] = EXPRESSION;
	} else if (lval[LTYPE] == SYMBOL) {
		p = &names[lval[LVALUE] * NLAST];
		switch (p[NTYPE]) {
		case ABS:
			sto_cmd(REL_PUSHW, p[NVALUE]);
			break;
		case CODE:
			sto_cmd(REL_CODEW, p[NVALUE]);
			break;
		case DATA:
			sto_cmd(REL_DATAW, p[NVALUE]);
			break;
		case TEXT:
			sto_cmd(REL_TEXTW, p[NVALUE]);
			break;
		case UDEF:
			sto_cmd(REL_UDEFW, p[NVALUE]);
			break;
		default:
			error("Symbol not proper type");
			break;
		}
		lval[LTYPE] = EXPRESSION;
	}
}

/*
 * Get a numerical constant
 */
number(register int *val) {
	register int i, minus;

	i = minus = 0;
	if (~ctype[ch] & CISDIGIT)
		return 0;
	if ((ch == '0') && (toupper(nch) == 'X')) {
		bump(2);
		while (1) {
			if (ctype[ch] & CISDIGIT)
				i = i * 16 + (gch() - '0');
			else if (ctype[ch] & CISLOWER)
				i = i * 16 + (gch() - 'a' + 10);
			else if (ctype[ch] & CISUPPER)
				i = i * 16 + (gch() - 'A' + 10);
			else
				break;
		}
	} else {
		while (ctype[ch] & CISDIGIT)
			i = i * 10 + (gch() - '0');
	}
	*val = i;
	return 1;
}

/*
 * Load a constant value
 */
constant(register int lval[]) {
	lval[LTYPE] = CONSTANT;
	if (number(&lval[LVALUE]))
		return 1;

	// character
	if (match("'")) {
		register int i;

		i = 0;
		while (ch && (ch != '\''))
			i = (i << 8) + litchar();
		gch();

		lval[LVALUE] = i;
		return 1;
	}

	return 0;
}

/*
 * Load primary expression
 */
primary(register int lval[]) {
	register int *p, len;
	int hash;

	if (match("<")) {  // <expression,...>
		expression(lval);
		if (!match(">"))
			error("> expected");
		return 1;
	}

	// test for identifier
	len = dohash(lptr, &hash);
	if (len) {
		bump(len);
		while (1) {
			p = &names[hash * NLAST];
			if (p[NTYPE] != LINK)
				break;
			else
				hash = p[NVALUE];
		}
		if (!p[NTYPE])
			p[NTYPE] = UNDEF; // Initial value

		switch (p[NTYPE]) {
		case UNDEF:
			sto_cmd(REL_SYMBOL, hash);
			lval[LTYPE] = EXPRESSION;
			break;
		case ABS:
		case CODE:
		case DATA:
		case TEXT:
		case UDEF:
			lval[LTYPE] = SYMBOL;
			lval[LVALUE] = hash;
			break;
		case POINT:
			switch (curseg) {
			case CODESEG:
				sto_cmd(REL_CODEW, curpos[CODESEG]);
				break;
			case DATASEG:
				sto_cmd(REL_DATAW, curpos[DATASEG]);
				break;
			case TEXTSEG:
				sto_cmd(REL_TEXTW, curpos[TEXTSEG]);
				break;
			case UDEFSEG:
				sto_cmd(REL_UDEFW, curpos[UDEFSEG]);
				break;
			}
			lval[LTYPE] = EXPRESSION;
			break;
		default:
			error("Invalid expression");
			lval[LTYPE] = CONSTANT;
			break;
		}
		return 1;
	}

	// test for constant
	return constant(lval);
}

/*
 * Process a constant evaluation
 */
calc(register int left, register int right, int oper) {
	switch (oper) {
	case REL_OR :  return (left  |  right);
	case REL_XOR:  return (left  ^  right);
	case REL_AND:  return (left  &  right);
	case REL_LSR:  return (left  >> right);
	case REL_LSL:  return (left  << right);
	case REL_ADD:  return (left  +  right);
	case REL_SUB:  return (left  -  right);
	case REL_MUL:  return (left  *  right);
	case REL_DIV:  return (left  /  right);
	case REL_MOD:  return (left  %  right);
	default:     return 0;
	}
}

/*
 * generic processing for <lval> { <operation> <rval> }
 */
gencode_expr(int tok, register int lval[], register int rval[]) {
	if ((lval[LTYPE] == CONSTANT) && (rval[LTYPE] == CONSTANT)) {
		lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
	} else {
		if ((lval[LTYPE] != EXPRESSION) && (rval[LTYPE] == EXPRESSION)) {
			loadlval(lval);
			sto_cmd(REL_SWAP, 0);
		} else if ((lval[LTYPE] == EXPRESSION) && (rval[LTYPE] != EXPRESSION)) {
			loadlval(rval);
		} else if ((lval[LTYPE] != EXPRESSION) && (rval[LTYPE] != EXPRESSION)) {
			loadlval(lval);
			loadlval(rval);
		}
		sto_cmd(tok, 0);
		lval[LTYPE] = EXPRESSION;
	}
}

/*
 *
 */
expr_unary(register int lval[]) {

	if (match("~")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (lval[LTYPE] == CONSTANT)
			lval[LVALUE] = ~lval[LVALUE];
		else {
			if (lval[LTYPE] != EXPRESSION)
				loadlval(lval);
			sto_cmd(REL_NOT, 0);
		}
	} else if (match("-")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (lval[LTYPE] == CONSTANT)
			lval[LVALUE] = -lval[LVALUE];
		else {
			if (lval[LTYPE] != EXPRESSION)
				loadlval(lval);
			sto_cmd(REL_NEG, 0);
		}
	} else if (match("+")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
	} else if (!primary(lval))
		return 0;
	return 1;
}

/*
 *
 */
expr_muldiv(register int *lval) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_unary(lval))
		return 0;

	while (1) {
		// Locate operation
		if (omatch("*"))
			tok = REL_MUL;
		else if (omatch("/"))
			tok = REL_DIV;
		else if (omatch("%"))
			tok = REL_MOD;
		else
			return 1;

		// Load rval
		if (!expr_unary(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(tok, lval, rval);
	}
}
/*
 *
 */
expr_addsub(register int *lval) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_muldiv(lval))
		return 0;

	while (1) {
		// Locate operation
		if (omatch("+"))
			tok = REL_ADD;
		else if (omatch("-"))
			tok = REL_SUB;
		else
			return 1;

		// Load rval
		if (!expr_muldiv(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(tok, lval, rval);
	}
}

/*
 *
 */
expr_shift(register int lval[]) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_addsub(lval))
		return 0;

	while (1) {
		// Locate operation
		if (omatch("<<"))
			tok = REL_LSL;
		else if (omatch(">>"))
			tok = REL_LSR;
		else
			return 1;

		// Load rval
		if (!expr_addsub(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(tok, lval, rval);
	}
}

/*
 *
 */
expr_and(register int lval[]) {
	int rval[LLAST];

	// Load lval
	if (!expr_shift(lval))
		return 0;

	while (1) {
		// Locate operation
		if (!omatch("&"))
			return 1;

		// Load rval
		if (!expr_shift(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(REL_AND, lval, rval);
	}
}

/*
 *
 */
expr_xor(register int lval[]) {
	int rval[LLAST];

	// Load lval
	if (!expr_and(lval))
		return 0;

	while (1) {
		// Locate operation
		if (!omatch("^"))
			return 1;

		// Load rval
		if (!expr_and(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(REL_XOR, lval, rval);
	}
}

/*
 *
 */
expr_or(register int lval[]) {
	int rval[LLAST];

	// Load lval
	if (!expr_xor(lval))
		return 0;

	while (1) {
		// Locate operation
		if (!omatch("|"))
			return 1;

		// Load rval
		if (!expr_xor(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(REL_OR, lval, rval);
	}
}

/*
 * Load a numerical expression seperated by comma's
 */
expression(register int lval[]) {
	if (!expr_or(lval))
		error("expression required");
}

/*
 * Load a constant expression
 */
constexpr(register int *val) {
	int lval[LLAST];

	if (!expr_or(lval))
		return 0;
	if (lval[LTYPE] == CONSTANT) {
		*val = lval[LVALUE];
		return 1;
	}
	return 0;
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
save_seg_size() {
	if (unsigned_GT(curpos[CODESEG], maxpos[CODESEG]))
		maxpos[CODESEG] = curpos[CODESEG];
	if (unsigned_GT(curpos[DATASEG], maxpos[DATASEG]))
		maxpos[DATASEG] = curpos[DATASEG];
	if (unsigned_GT(curpos[TEXTSEG], maxpos[TEXTSEG]))
		maxpos[TEXTSEG] = curpos[TEXTSEG];
	if (unsigned_GT(curpos[UDEFSEG], maxpos[UDEFSEG]))
		maxpos[UDEFSEG] = curpos[UDEFSEG];
}

/*
 *
 */
need_comma() {
	blanks();
	if (ch == ',')
		gch();
	else
		error("comma expected");
}

/*
 *
 */
need_reg() {
	int hash, reg;
	register int len, *p;

	blanks();
	if (!ch)
		len = 0;
	else
		len = dohash(lptr, &hash);
	if (!len) {
		error("register expected");
		reg = 0;
	} else {
		bump(len);
		while (1) {
			p = &names[hash * NLAST];
			if (p[NTYPE] != LINK)
				break;
			else
				hash = p[NVALUE];
		}
		if (p[NTYPE] != REGISTER) {
			error("register expected");
			reg = 0;
		} else
			reg = p[NVALUE];
	}
	// process reg
	sto_data(reg, 1);
}

/*
 *
 */
need_imm() {
	int lval[LLAST];

	expression(lval);
	if (lval[LTYPE] == CONSTANT)
		sto_data(lval[LVALUE], BPW);
	else {
		loadlval(lval);
		sto_cmd(REL_POPW, 0);
	}
}

/*
 *
 */
need_mem() {
	int lval[LLAST];

	blanks();
	if (ch != '(')
		need_imm();
	else
		sto_data(0, BPW); // no address

	// test for registers
	blanks();
	if (ch != '(') {
		sto_data(0, 1); // dummy reg
	} else {
		gch();
		blanks();
		need_reg();
		blanks();
		if (ch == ')')
			gch();
		else
			error(") expected");
	}
}

/*
 *
 */
need_risc() {
	int lval[LLAST];
	int hash;
	register int len, *p;
	int lreg, imm, rreg;

	/*
	 * Register
	 */
	blanks();
	len = dohash(lptr, &hash);
	if (!len) {
		error("register expected");
		return;
	}
	bump(len);
	while (1) {
		p = &names[hash * NLAST];
		// follow chain
		if (p[NTYPE] != LINK)
			break;
		else
			hash = p[NVALUE];
	}
	if (p[NTYPE] != REGISTER) {
		error("register expected");
		return;
	}
	lreg = p[NVALUE];

	/*
	 * Comma
	 */
	blanks();
	if (ch == ',')
		gch();
	else
		error("comma expected");

	/*
	 * Immediate
	 */
	blanks();
	if (ch != '(')
		expression(lval);
	else
		lval[LTYPE] = 0;

	/*
	 * register-indirect
	 */
	blanks();
	if (ch == '(') {
		gch();
		blanks();
		len = dohash(lptr, &hash);
		if (!len) {
			error("register expected");
			return;
		}
		bump(len);
		while (1) {
			p = &names[hash * NLAST];
			// follow chain
			if (p[NTYPE] != LINK)
				break;
			else
				hash = p[NVALUE];
		}
		if (p[NTYPE] != REGISTER) {
			error("register expected");
			return;
		}
		rreg = p[NVALUE];

		blanks();
		if (ch == ')')
			gch();
		else
			error(") expected");
	} else
		rreg = 0;

	// registers
	sto_data(lreg << 4 | rreg, 1);
	// immediate
	if (lval[LTYPE] == 0) {
		sto_data(0, BPW);
	} else if (lval[LTYPE] == CONSTANT) {
		sto_data(lval[LVALUE], BPW);
	} else {
		loadlval(lval);
		sto_cmd(REL_POPW, 0);
	}
}

do_pseudo(register int p[]) {
	int val, lval[LLAST];
	register int size;

	switch (p[NVALUE]) {
	case PSEUDO_CODE:
		curseg = CODESEG;
		sto_cmd(REL_CODEORG, curpos[CODESEG]);
		break;
	case PSEUDO_DATA:
		curseg = DATASEG;
		sto_cmd(REL_DATAORG, curpos[DATASEG]);
		break;
	case PSEUDO_TEXT:
		curseg = TEXTSEG;
		sto_cmd(REL_TEXTORG, curpos[TEXTSEG]);
		break;
	case PSEUDO_UDEF:
		curseg = UDEFSEG;
		sto_cmd(REL_UDEFORG, curpos[UDEFSEG]);
		break;
	case PSEUDO_DSB:
	case PSEUDO_DSW:
		size = p[NVALUE] == PSEUDO_DSB ? 1 : BPW;
		if (!constexpr(&val)) {
			if (pass == 1)
				error("constant required");
		} else
			curpos[curseg] += val * size;
		sto_cmd(REL_DSB, val * size);
		break;
	case PSEUDO_DCB:
	case PSEUDO_DCW:
		size = p[NVALUE] == PSEUDO_DCB ? 1 : BPW;
		while (1) {
			if (match("\"")) {
				while (ch && (ch != '"')) {
					val = litchar();
					if (pass == 2)
						sto_data(val, size);
					curpos[curseg] += size;
				}
				gch(); // skip terminator
			} else {
				// get an expression
				if (!expr_or(lval))
					break;

				if (pass == 2) {
					if (lval[LTYPE] != CONSTANT) {
						loadlval(lval);
						if (size == 1)
							sto_cmd(REL_POPB, 0);
						else
							sto_cmd(REL_POPW, 0);
					} else if (size == BPW)
						sto_data(lval[LVALUE], BPW);
					else if ((lval[LVALUE] >= -128) && (lval[LVALUE] <= 127))
						sto_data(lval[LVALUE], 1);
					else
						error("constant out of range");
				}
				curpos[curseg] += size;
			}
			if (!match(","))
				break;
		}
		break;
	case PSEUDO_ORG:
		save_seg_size();
		expression(lval);
		if (lval[LTYPE] == CONSTANT) {
			curpos[curseg] = lval[LVALUE];
		} else if (lval[LTYPE] == SYMBOL) {
			p = &names[lval[LVALUE] * NLAST];
			switch (p[NTYPE]) {
			case CODE:
				curseg = CODESEG;
				break;
			case DATA:
				curseg = DATASEG;
				break;
			case TEXT:
				curseg = TEXTSEG;
				break;
			case UDEF:
				curseg = UDEFSEG;
				break;
			}
			curpos[curseg] = p[NVALUE];
		} else
			error("Invalid address");
		if (pass == 2) {
			switch (curseg) {
			case CODESEG:
				sto_cmd(REL_CODEORG, curpos[curseg]);
				break;
			case DATASEG:
				sto_cmd(REL_DATAORG, curpos[curseg]);
				break;
			case TEXTSEG:
				sto_cmd(REL_TEXTORG, curpos[curseg]);
				break;
			case UDEFSEG:
				sto_cmd(REL_UDEFORG, curpos[curseg]);
				break;
			}
		}
		break;
	default:
		error("-> pseudo");
		ch = 0;
		break;
	}
}

do_opcode(register int p[]) {
	if (pass == 1) {
		switch (p[NVALUE]) {
		case OPC_ILLEGAL:
		case OPC_RSB:
			curpos[curseg] += 1;
			break;
		case OPC_PSHR:
		case OPC_POPR:
		case OPC_SVC:
			curpos[curseg] += 3;
			break;
		case OPC_PSHB:
		case OPC_PSHW:
		case OPC_PSHA:
		case OPC_JSB:
			curpos[curseg] += 4;
			break;
		case OPC_NEG:
		case OPC_NOT:
			curpos[curseg] += 2;
			break;
		case OPC_ADD:
		case OPC_SUB:
		case OPC_MUL:
		case OPC_DIV:
		case OPC_MOD:
		case OPC_OR:
		case OPC_XOR:
		case OPC_AND:
		case OPC_LSR:
		case OPC_LSL:
		case OPC_LDR:
			curpos[curseg] += 3;
			break;
		case OPC_LDB:
		case OPC_LDW:
		case OPC_LEA:
		case OPC_STB:
		case OPC_STW:
			curpos[curseg] += 5;
			break;
		case OPC_MULA:
		case OPC_MULB:
		case OPC_MULW:
		case OPC_DIVA:
		case OPC_DIVB:
		case OPC_DIVW:
		case OPC_MODA:
		case OPC_MODB:
		case OPC_MODW:
		case OPC_ADDA:
		case OPC_ADDB:
		case OPC_ADDW:
		case OPC_SUBA:
		case OPC_SUBB:
		case OPC_SUBW:
		case OPC_LSLA:
		case OPC_LSLB:
		case OPC_LSLW:
		case OPC_LSRA:
		case OPC_LSRB:
		case OPC_LSRW:
		case OPC_XORA:
		case OPC_XORB:
		case OPC_XORW:
		case OPC_SGTA:
		case OPC_SGTB:
		case OPC_SGTW:
		case OPC_SLTA:
		case OPC_SLTB:
		case OPC_SLTW:
		case OPC_LODA:
		case OPC_LODB:
		case OPC_LODW:
		case OPC_STOA:
		case OPC_STOB:
		case OPC_STOW:
		case OPC_JZA:
		case OPC_JZB:
		case OPC_JZW:
		case OPC_JNZA:
		case OPC_JNZB:
		case OPC_JNZW:
			curpos[curseg] += 4;
			break;
		default:
			error("unimplemented opcode");
			break;
		}
		ch = 0; // ignore rest of line
	} else {
		sto_data(p[NVALUE], 1);
		switch (p[NVALUE]) {
		case OPC_ILLEGAL:
		case OPC_RSB:
			curpos[curseg] += 1;
			break;
			break;
		case OPC_PSHR:
		case OPC_POPR:
		case OPC_SVC:
			need_imm();
			curpos[curseg] += 3;
			break;
		case OPC_PSHB:
		case OPC_PSHW:
		case OPC_PSHA:
		case OPC_JSB:
			need_mem();
			curpos[curseg] += 4;
			break;
		case OPC_NEG:
		case OPC_NOT:
			need_reg();
			curpos[curseg] += 2;
			break;
		case OPC_ADD:
		case OPC_SUB:
		case OPC_MUL:
		case OPC_DIV:
		case OPC_MOD:
		case OPC_OR:
		case OPC_XOR:
		case OPC_AND:
		case OPC_LSR:
		case OPC_LSL:
		case OPC_LDR:
			need_reg();
			need_comma();
			need_reg();
			curpos[curseg] += 3;
			break;
		case OPC_LDB:
		case OPC_LDW:
		case OPC_LEA:
		case OPC_STB:
		case OPC_STW:
			need_reg();
			need_comma();
			need_mem();
			curpos[curseg] += 5;
			break;
		case OPC_MULA:
		case OPC_MULB:
		case OPC_MULW:
		case OPC_DIVA:
		case OPC_DIVB:
		case OPC_DIVW:
		case OPC_MODA:
		case OPC_MODB:
		case OPC_MODW:
		case OPC_ADDA:
		case OPC_ADDB:
		case OPC_ADDW:
		case OPC_SUBA:
		case OPC_SUBB:
		case OPC_SUBW:
		case OPC_LSLA:
		case OPC_LSLB:
		case OPC_LSLW:
		case OPC_LSRA:
		case OPC_LSRB:
		case OPC_LSRW:
		case OPC_XORA:
		case OPC_XORB:
		case OPC_XORW:
		case OPC_SGTA:
		case OPC_SGTB:
		case OPC_SGTW:
		case OPC_SLTA:
		case OPC_SLTB:
		case OPC_SLTW:
		case OPC_LODA:
		case OPC_LODB:
		case OPC_LODW:
		case OPC_STOA:
		case OPC_STOB:
		case OPC_STOW:
		case OPC_JZA:
		case OPC_JZB:
		case OPC_JZW:
		case OPC_JNZA:
		case OPC_JNZB:
		case OPC_JNZW:
			need_risc();
			curpos[curseg] += 4;
			break;
		default:
			error("unimplemented opcode");
			ch = 0; // ignore rest of line
			break;
		}
	}
}

/*
 *
 */
parse() {
	int i, len, hash, ext;
	register int *p;
	int lval[LLAST];

	while (inphdl) {
		if (debug && (pass == 2))
			fprintf(outhdl, "; %04x %s\n", curpos[CODESEG], lptr);
		while (1) {
			blanks();
			if (!ch)
				break; // end of line
			len = dohash(lptr, &hash);
			if (!len) {
				if (pass == 1)
					error("opcode expected");
				break;
			}
			bump(len);
			p = &names[hash * NLAST];
			if (!p[NTYPE])
				p[NTYPE] = UNDEF;

			switch (p[NTYPE]) {
			case UNDEF:
				if (match(":")) {
					ext = match(":");
					if (verbose && ext) {
						outname(hash);
						printf("\n");
					}
					if (curseg == CODESEG) {
						p[NTYPE] = CODE;
						p[NVALUE] = curpos[CODESEG];
					} else if (curseg == DATASEG) {
						p[NTYPE] = DATA;
						p[NVALUE] = curpos[DATASEG];
					} else if (curseg == TEXTSEG) {
						p[NTYPE] = TEXT;
						p[NVALUE] = curpos[TEXTSEG];
					} else if (curseg == UDEFSEG) {
						p[NTYPE] = UDEF;
						p[NVALUE] = curpos[UDEFSEG];
					} else
						error("unknown segment");
					continue;
				} else if (match("=")) {
					expression(lval);
					if (lval[LTYPE] == CONSTANT) {
						p[NTYPE] = ABS;
						p[NVALUE] = lval[LVALUE];
					} else if (lval[LTYPE] == SYMBOL) {
						p[NTYPE] = LINK;
						p[NVALUE] = lval[LVALUE];

						p = &names[p[NVALUE] * NLAST];
						if (!p[NTYPE])
							p[NTYPE] = UNDEF;	// Initial value
					} else if (pass == 1) {
						p[NTYPE] = ABS;
						p[NVALUE] = 0;
					} else {
						error("use #define");
					}
					break;

				} else {
					if (pass == 1)
						error("unknown opcode");
					ch = 0;
					break;
				}
			case ABS:
			case CODE:
			case DATA:
			case TEXT:
			case UDEF:
				if (match(":")) {
					ext = match(":");
					if (verbose && ext) {
						outname(hash);
						printf("\n");
					}
					if (p[NTYPE] == CODE) {
						if ((curseg != CODESEG) || (p[NVALUE] != curpos[CODESEG]))
							if (pass == 1)
								error("multiply defined");
							else
								error("phase error");
						if (ext && (pass == 2))
							sto_cmd(REL_CODEDEF, hash);
					} else if (p[NTYPE] == DATA) {
						if ((curseg != DATASEG) || (p[NVALUE] != curpos[DATASEG]))
							if (pass == 1)
								error("multiply defined");
							else
								error("phase error");
						if (ext && (pass == 2))
							sto_cmd(REL_DATADEF, hash);
					} else if (p[NTYPE] == TEXT) {
						if ((curseg != TEXTSEG) || (p[NVALUE] != curpos[TEXTSEG]))
							if (pass == 1)
								error("multiply defined");
							else
								error("phase error");
						if (ext && (pass == 2))
							sto_cmd(REL_TEXTDEF, hash);
					} else if (p[NTYPE] == UDEF) {
						if ((curseg != UDEFSEG) || (p[NVALUE] != curpos[UDEFSEG]))
							if (pass == 1)
								error("multiply defined");
							else
								error("phase error");
						if (ext && (pass == 2))
							sto_cmd(REL_UDEFDEF, hash);
					} else {
						error("not implemented");
					}
					continue;
				} else if (match("=")) {
					if (pass == 1)
						error("multiply defined");
					else
						expression(lval);
					break;
				} else {
					error("internal error");
					ch = 0;
					break;
				}
			case OPCODE:
				do_opcode(p);
				break;
			case PSEUDO:
				if (p[NVALUE] == PSEUDO_END)
					return; // don't read next line keeping file open
				do_pseudo(p);
				break;
			case LINK:
				if (pass == 1)
					error("multiply defined");
				ch = 0; // errors displayed in pass 1
				break;
			default:
				if (pass == 1)
					error("opcode expected");
				break;
			}

			// test for junk
			blanks();
			if (ch)
				error("encountered junk");
			break;
		}
		readline();
	}
}

/*
 * Initialize all variables
 */
add_res(char *opc, int typ, int val) {
	int hash;
	register int *p;

	dohash(opc, &hash);
	p = &names[hash * NLAST];
	p[NTYPE] = typ;
	p[NVALUE] = val;
}

/*
 * Initialize all variables
 */
initialize() {
	register int i, *p;

	verbose = debug = 0;
	outhdl = lishdl = inphdl = 0;
	inplnr = 0;
	inpfn[0] = 0;
	datlen = 0;

	// character properties
	for (i = '0'; i <= '9'; ++i)
		ctype[i] = CISDIGIT | CISXDIGIT | CSYMNEXT;
	for (i = 'A'; i <= 'F'; ++i)
		ctype[i] = CISUPPER | CISXDIGIT | CSYMFIRST | CSYMNEXT;
	for (i = 'G'; i <= 'Z'; ++i)
		ctype[i] = CISUPPER | CSYMFIRST | CSYMNEXT;
	for (i = 'a'; i <= 'f'; ++i)
		ctype[i] = CISLOWER | CISXDIGIT | CSYMFIRST | CSYMNEXT;
	for (i = 'g'; i <= 'z'; ++i)
		ctype[i] = CISLOWER | CSYMFIRST | CSYMNEXT;
	ctype['_'] = CSYMFIRST | CSYMNEXT;
	ctype['.'] = CSYMFIRST | CSYMNEXT;
	ctype[' '] = CISSPACE;
	ctype['\f'] = CISSPACE;
	ctype['\n'] = CISSPACE;
	ctype['\r'] = CISSPACE;
	ctype['\t'] = CISSPACE;
	ctype['\v'] = CISSPACE;

	// reset table
	for (i = 0; i < NAMEMAX; ++i) {
		p = &names[i * NLAST];
		p[NCHAR] = p[NTYPE] = 0;
	}

	// reserve first entry so it terminates lists
	names[0 * NLAST + NCHAR] = '?';

	// reset positions
	pass = 1;
	curseg = CODESEG;
	curpos[CODESEG] = curpos[DATASEG] = curpos[TEXTSEG] = curpos[UDEFSEG] = 0;
	maxpos[CODESEG] = maxpos[DATASEG] = maxpos[TEXTSEG] = maxpos[UDEFSEG] = 0;

	add_res("mul.a", OPCODE, OPC_MULA);
	add_res("mul.b", OPCODE, OPC_MULB);
	add_res("mul.w", OPCODE, OPC_MULW);
	add_res("div.a", OPCODE, OPC_DIVA);
	add_res("div.b", OPCODE, OPC_DIVB);
	add_res("div.w", OPCODE, OPC_DIVW);
	add_res("mod.a", OPCODE, OPC_MODA);
	add_res("mod.b", OPCODE, OPC_MODB);
	add_res("mod.w", OPCODE, OPC_MODW);
	add_res("add.a", OPCODE, OPC_ADDA);
	add_res("add.b", OPCODE, OPC_ADDB);
	add_res("add.w", OPCODE, OPC_ADDW);
	add_res("sub.a", OPCODE, OPC_SUBA);
	add_res("sub.b", OPCODE, OPC_SUBB);
	add_res("sub.w", OPCODE, OPC_SUBW);
	add_res("lsl.a", OPCODE, OPC_LSLA);
	add_res("lsl.b", OPCODE, OPC_LSLB);
	add_res("lsl.w", OPCODE, OPC_LSLW);
	add_res("lsr.a", OPCODE, OPC_LSRA);
	add_res("lsr.b", OPCODE, OPC_LSRB);
	add_res("lsr.w", OPCODE, OPC_LSRW);
	add_res("xor.a", OPCODE, OPC_XORA);
	add_res("xor.b", OPCODE, OPC_XORB);
	add_res("xor.w", OPCODE, OPC_XORW);
	add_res("sgt.a", OPCODE, OPC_SGTA);
	add_res("sgt.b", OPCODE, OPC_SGTB);
	add_res("sgt.w", OPCODE, OPC_SGTW);
	add_res("slt.a", OPCODE, OPC_SLTA);
	add_res("slt.b", OPCODE, OPC_SLTB);
	add_res("slt.w", OPCODE, OPC_SLTW);
	add_res("ld.a", OPCODE, OPC_LODA);
	add_res("ld.b", OPCODE, OPC_LODB);
	add_res("ld.w", OPCODE, OPC_LODW);
	add_res("st.a", OPCODE, OPC_STOA);
	add_res("st.b", OPCODE, OPC_STOB);
	add_res("st.w", OPCODE, OPC_STOW);
	add_res("jz.a", OPCODE, OPC_JZA);
	add_res("jz.b", OPCODE, OPC_JZB);
	add_res("jz.w", OPCODE, OPC_JZW);
	add_res("jnz.a", OPCODE, OPC_JNZA);
	add_res("jnz.b", OPCODE, OPC_JNZB);
	add_res("jnz.w", OPCODE, OPC_JNZW);

	// reserved words
	add_res("illegal", OPCODE, OPC_ILLEGAL);
	add_res("add", OPCODE, OPC_ADD);
	add_res("sub", OPCODE, OPC_SUB);
	add_res("mul", OPCODE, OPC_MUL);
	add_res("div", OPCODE, OPC_DIV);
	add_res("mod", OPCODE, OPC_MOD);
	add_res("or", OPCODE, OPC_OR);
	add_res("xor", OPCODE, OPC_XOR);
	add_res("and", OPCODE, OPC_AND);
	add_res("lsr", OPCODE, OPC_LSR);
	add_res("lsl", OPCODE, OPC_LSL);
	add_res("neg", OPCODE, OPC_NEG);
	add_res("not", OPCODE, OPC_NOT);
	add_res("ldb", OPCODE, OPC_LDB);
	add_res("ldw", OPCODE, OPC_LDW);
	add_res("ldr", OPCODE, OPC_LDR);
	add_res("lda", OPCODE, OPC_LEA);
	add_res("stb", OPCODE, OPC_STB);
	add_res("stw", OPCODE, OPC_STW);
	add_res("jsb", OPCODE, OPC_JSB);
	add_res("rsb", OPCODE, OPC_RSB);
	add_res("pshr", OPCODE, OPC_PSHR);
	add_res("popr", OPCODE, OPC_POPR);
	add_res("pshb", OPCODE, OPC_PSHB);
	add_res("pshw", OPCODE, OPC_PSHW);
	add_res("psha", OPCODE, OPC_PSHA);
	add_res("svc", OPCODE, OPC_SVC);
	add_res(".code", PSEUDO, PSEUDO_CODE);
	add_res(".data", PSEUDO, PSEUDO_DATA);
	add_res(".text", PSEUDO, PSEUDO_TEXT);
	add_res(".udef", PSEUDO, PSEUDO_UDEF);
	add_res(".org", PSEUDO, PSEUDO_ORG);
	add_res(".end", PSEUDO, PSEUDO_END);
	add_res(".dcb", PSEUDO, PSEUDO_DCB);
	add_res(".dcw", PSEUDO, PSEUDO_DCW);
	add_res(".dsb", PSEUDO, PSEUDO_DSB);
	add_res(".dsw", PSEUDO, PSEUDO_DSW);
	add_res("r0", REGISTER, 0);
	add_res("r1", REGISTER, 1);
	add_res("r2", REGISTER, 2);
	add_res("r3", REGISTER, 3);
	add_res("r4", REGISTER, 4);
	add_res("r5", REGISTER, 5);
	add_res("r6", REGISTER, 6);
	add_res("r7", REGISTER, 7);
	add_res("r8", REGISTER, 8);
	add_res("r9", REGISTER, 9);
	add_res("r10", REGISTER, 10);
	add_res("r11", REGISTER, 11);
	add_res("r12", REGISTER, 12);
	add_res("r13", REGISTER, 13);
	add_res("r14", REGISTER, 14);
	add_res("r15", REGISTER, 15);
	add_res(".", POINT, 0);
}

/*
 * Process commandline
 */
usage() {
	printf("X-Assembler, Version %s\n\n", getversion());

	printf("usage: xasm <file>[.<ext>]\n");
	printf("  -a <file>[.<ext>]]\tListing\n");
	printf("  -c <file>[.<ext>]]\tObject output\n");
	printf("  -v\t\t\tVerbose\n");
	exit(1);
}

/*
 * Override default/explicit file extension
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
 * Handle program arguments
 */
startup(register int *argv) {
	++argv; // skip argv[0]
	while (*argv) {
		register char *arg;
		arg = *argv++;

		if (*arg != '-') {
			fext(inpfn, arg, ".xs", 0);
			if (!outfn[0])
				fext(outfn, arg, ".xo", 1);
		} else {
			// Process option
			++arg;
			switch (*arg++) {
			case 'a':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					fext(lisfn, inpfn, ".lis", 1);
				else
					fext(lisfn, arg, ".lis", 0);
				break;
			case 'c':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();
				else
					fext(outfn, arg, ".xo", 0);
				break;
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
}

/*
 * Program start
 */
main(int argc, int *argv) {
	register int i, j, *p;

	initialize();        // initialize all variables
	startup(argv);       // Process commandline options

	{
		inphdl = open_file(inpfn, "r");

		inplnr = 0;
		curseg = CODESEG;
		curpos[CODESEG] = curpos[DATASEG] = curpos[TEXTSEG] = curpos[UDEFSEG] = 0;
		datlen = 0;

		pass = 1;
		if (verbose)
			printf("Pass 1\n");

		readline(); // fetch first line
		parse(); // GO !!!
		save_seg_size();

		if (!inphdl)
			printf("%%missing .END statement\n");
		else
			fclose(inphdl);
	}

	{
		inphdl = open_file(inpfn, "r");
		outhdl = open_file(outfn, "w");
		if (lisfn[0])
			lishdl = open_file(lisfn, "w");

		inplnr = 0;
		curseg = CODESEG;
		curpos[CODESEG] = curpos[DATASEG] = curpos[TEXTSEG] = curpos[UDEFSEG] = 0;
		datlen = 0;

		pass = 2;
		if (verbose)
			printf("Pass 2\n");

		readline(); // fetch first line
		parse(); // GO !!!

		sto_cmd(REL_END, 0);
	}

	if (lishdl) {
		fprintf(lishdl, "code  : 0x%04x (%5d)\n", maxpos[CODESEG] & 0xffff, maxpos[CODESEG] & 0xffff);
		fprintf(lishdl, "data  : 0x%04x (%5d)\n", maxpos[DATASEG] & 0xffff, maxpos[DATASEG] & 0xffff);
		fprintf(lishdl, "text  : 0x%04x (%5d)\n", maxpos[TEXTSEG] & 0xffff, maxpos[TEXTSEG] & 0xffff);
		fprintf(lishdl, "udef  : 0x%04x (%5d)\n", maxpos[UDEFSEG] & 0xffff, maxpos[UDEFSEG] & 0xffff);
		j = 0;
		for (i = 0; i < NAMEMAX; ++i) if (names[i * NLAST + NCHAR]) ++j;
		fprintf(lishdl, "Names : %5d(%5d)\n", j, NAMEMAX);
	}

	return errflag;
}
