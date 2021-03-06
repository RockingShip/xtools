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
	NAMEMAX = 0x821,	// Size of nametable. 3 bits set. !!! MUST BE PRIME !!!
	PATHMAX = 80,		// Length of filename
	SBUFMAX = 512,		// Size of source buffer
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
	/*
	 * Syntax: opcode reg,immediate(index)
	 *
	 * opcode+0 ea=immediate+regs[index]
	 * opcode+1 ea=memory.byte[immediate+regs[index]]
	 * opcode+2 ea=memory.word[immediate+regs[index]]
	 */
	// reg = reg <operation> ea
	OPC_SVC = 0x04, // reg = reg <systemcall> ea
	OPC_MUL = 0x10, // reg *= ea
	OPC_DIV = 0x14, // reg /= ea
	OPC_MOD = 0x18, // reg %= ea
	OPC_ADD = 0x1c, // reg += ea
	OPC_SUB = 0x20, // reg -= ea
	OPC_LSL = 0x24, // reg <<= ea
	OPC_LSR = 0x28, // reg >>= ea
	OPC_AND = 0x2c, // reg &= ea
	OPC_XOR = 0x30, // reg ^= ea
	OPC_OR = 0x34, // reg |= ea
	OPC_SGT = 0x38, // reg = reg > ea
	OPC_SLT = 0x3c, // reg = reg < ea
	// reg = <operation> ea
	OPC_LD = 0x40, // reg = ea
	OPC_NEG = 0x44, // reg = -ea
	OPC_NOT = 0x48, // reg = ~ea
	// the following opcode block is reserved for flow-control, a concept unknown to untangle
	OPC_ST = 0x4c, // mem[ea] = reg (NOTE: `untangle` considers explicit writing to memory flow-control)
	OPC_JZ = 0x50, // pc = (reg == 0) ? ea : pc+1
	OPC_JNZ = 0x54, // pc = (reg != 0) ? ea : pc+1
	OPC_JSB = 0x58, // mem[--reg] = pc; pc = ea
	OPC_RSB = 0x5c, // pc = mem[reg++]
	// the following are considered convenience instruction to interact with stack
	OPC_PUSH = 0x60, // mem[--reg] = ea
	OPC_PSHR = 0x64, // for (i=0; i<16; ++i) if (ea & (1<<i)) memory.word[--reg] = regs[i]
	OPC_POPR = 0x68, // for (i=15; i>0; --i) if (ea & (1<<i)) regs[i] = memory.word[reg++]
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
char	datbuf[128];		// storage buffer for sto_byte/sto_word
char	inpfn[PATHMAX];		// input filename
char	lisfn[PATHMAX];		// listing filename
char	*lptr;			// Pointer to current character in input buffer
char	nch;			// Next character in line being scanned
char	outfn[PATHMAX];		// output filename
char	sbuf[SBUFMAX];

extern char *stdout;
//exit(int code);
//expression(int lval[]);
//fclose(int hdl);
//fgets(char *str, int siz, int hdl);
//fopen(char *name, char *mode);
//fread(char *buf, int siz, int cnt, int hdl);
//fwrite(char *buf, int siz, int cnt, int hdl);
//strcpy(char *dst, char *src);
//strncpy(char *dst, int n, char *src);

/*
 * Generate error messages
 */
error(char *msg) {
	// Display original line
	fputd(inplnr, stdout);
	fputs(": ", stdout);
	fputs(sbuf, stdout);
	fputs("\n%", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);

	fputs(";% ", lishdl);
	fputs(msg, lishdl);
	fputc('\n', lishdl);

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
				if (curseg == CODESEG) {
					fputc('C', lishdl);
					fput0x(curpos[CODESEG] & 0xffff, 4, lishdl);
				} else if (curseg == DATASEG) {
					fputc('D', lishdl);
					fput0x(curpos[DATASEG] & 0xffff, 4, lishdl);
				} else if (curseg == TEXTSEG) {
					fputc('T', lishdl);
					fput0x(curpos[TEXTSEG] & 0xffff, 4, lishdl);
				} else {
					fputc('U', lishdl);
					fput0x(curpos[UDEFSEG] & 0xffff, 4, lishdl);
				}
				fputs(":\t", lishdl);
				fputs(sbuf, lishdl);
				fputc('\n', lishdl);
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
	fputc(names[hash * NLAST + NCHAR], outhdl);
}

outname(register int hash) {
	register int i;

	i = names[hash * NLAST + NTAB];
	if (i)
		outname(i);
	fputc(names[hash * NLAST + NCHAR], stdout);
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
				fputs("0x", outhdl);
				fput0x(datbuf[i] & 0xff, 2, outhdl);
			}
			fputc('\n', outhdl);
		}

		datlen = 0;
	}
}

/*
 *
 */
sto_byte(char byte) {
	// store lower byte
	if (datlen == 128)
		sto_flush();
	datbuf[datlen++] = byte;
}

/*
 *
 */
sto_word(int word) {
	sto_byte (word);
	sto_byte (word >> 8);
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
				fputs("unimplemented OBJECT cmd: ", stdout);
				fputd(cmd, stdout);
				fputc('\n', stdout);
				exit(1);
				break;
			}
		else {
			switch (cmd) {
			case REL_END:
				fputs("end", outhdl);
				break;
			case REL_ADD:
				fputs("add", outhdl);
				break;
			case REL_SUB:
				fputs("sub", outhdl);
				break;
			case REL_MUL:
				fputs("mul", outhdl);
				break;
			case REL_DIV:
				fputs("div", outhdl);
				break;
			case REL_MOD:
				fputs("mod", outhdl);
				break;
			case REL_LSR:
				fputs("lsr", outhdl);
				break;
			case REL_LSL:
				fputs("lsl", outhdl);
				break;
			case REL_AND:
				fputs("and", outhdl);
				break;
			case REL_OR:
				fputs("or", outhdl);
				break;
			case REL_XOR:
				fputs("xor", outhdl);
				break;
			case REL_SWAP:
				fputs("swap", outhdl);
				break;
			case REL_POPB:
				fputs("popb", outhdl);
				break;
			case REL_POPW:
				fputs("popw", outhdl);
				break;
			case REL_PUSHB:
				fputs("pushb ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_PUSHW:
				fputs("pushw ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_CODEB:
				fputs("codeb ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_CODEW:
				fputs("codew ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_DATAB:
				fputs("datab ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_DATAW:
				fputs("dataw ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_TEXTB:
				fputs("textb ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_TEXTW:
				fputs("textw ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_UDEFB:
				fputs("udefb ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_UDEFW:
				fputs("udefw ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_DSB:
				fputs("dsb ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_CODEORG:
				fputs("codeorg ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_DATAORG:
				fputs("dataorg ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_TEXTORG:
				fputs("textorg ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_UDEFORG:
				fputs("udeforg ", outhdl);
				fputd(val, outhdl);
				break;
			case REL_SYMBOL:
				fputs("symbol ", outhdl);
				foutname(val);
				break;
			case REL_CODEDEF:
				p = &names[val * NLAST];
				fputs("codedef ", outhdl);
				fputd(p[NVALUE], outhdl);
				fputc(',', outhdl);
				foutname(val);
				break;
			case REL_DATADEF:
				p = &names[val * NLAST];
				fputs("datadef ", outhdl);
				fputd(p[NVALUE], outhdl);
				fputc(',', outhdl);
				foutname(val);
				break;
			case REL_TEXTDEF:
				p = &names[val * NLAST];
				fputs("textdef ", outhdl);
				fputd(p[NVALUE], outhdl);
				fputc(',', outhdl);
				foutname(val);
				break;
			case REL_UDEFDEF:
				p = &names[val * NLAST];
				fputs("udefdef ", outhdl);
				fputd(p[NVALUE], outhdl);
				fputc(',', outhdl);
				foutname(val);
				break;
			default:
				fputs("cmd ", outhdl);
				fputd(cmd, outhdl);
				break;
			}
			fputc('\n', outhdl);
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
	if ((ch == '0') && (nch == 'x' || nch == 'X')) {
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
calc(register int left, int opc, register int right) {
	// sign extend
	left |= -(left & (1 << SBIT));
	right |= -(right & (1 << SBIT));

	switch (opc) {
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
	sto_byte(reg);
}

/*
 *
 */
need_imm() {
	int lval[LLAST];

	expression(lval);
	if (lval[LTYPE] == CONSTANT)
		sto_word(lval[LVALUE]);
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
		sto_word(0); // no address

	// test for registers
	blanks();
	if (ch != '(') {
		sto_byte(0); // dummy reg
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
	sto_byte(rreg << 4 | lreg);
	// immediate
	if (lval[LTYPE] == 0) {
		sto_word(0);
	} else if (lval[LTYPE] == CONSTANT) {
		sto_word(lval[LVALUE]);
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
					if (pass == 2) {
						if (size == 1)
							sto_byte(val);
						else
							sto_word(val);
					}
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
						sto_word(lval[LVALUE]);
					else if ((lval[LVALUE] >= -128) && (lval[LVALUE] <= 127))
						sto_byte(lval[LVALUE]);
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
		curpos[curseg] += 4;
		ch = 0; // ignore rest of line
	} else {
		sto_byte(p[NVALUE]);
		need_risc();
		curpos[curseg] += 4;
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
		if (debug && (pass == 2)) {
			fputs("; ", outhdl);
			fput0x(curpos[CODESEG], 4, outhdl);
			fputc(' ', outhdl);
			fputs(lptr, outhdl);
			fputc('\n', outhdl);
		}
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
						fputc('\n', outhdl);
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
						fputc('\n', outhdl);
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

	add_res("svc.a", OPCODE, OPC_SVC + 0);
	add_res("svc.b", OPCODE, OPC_SVC + 1);
	add_res("svc.w", OPCODE, OPC_SVC + 2);
	add_res("mul.a", OPCODE, OPC_MUL + 0);
	add_res("mul.b", OPCODE, OPC_MUL + 1);
	add_res("mul.w", OPCODE, OPC_MUL + 2);
	add_res("div.a", OPCODE, OPC_DIV + 0);
	add_res("div.b", OPCODE, OPC_DIV + 1);
	add_res("div.w", OPCODE, OPC_DIV + 2);
	add_res("mod.a", OPCODE, OPC_MOD + 0);
	add_res("mod.b", OPCODE, OPC_MOD + 1);
	add_res("mod.w", OPCODE, OPC_MOD + 2);
	add_res("add.a", OPCODE, OPC_ADD + 0);
	add_res("add.b", OPCODE, OPC_ADD + 1);
	add_res("add.w", OPCODE, OPC_ADD + 2);
	add_res("sub.a", OPCODE, OPC_SUB + 0);
	add_res("sub.b", OPCODE, OPC_SUB + 1);
	add_res("sub.w", OPCODE, OPC_SUB + 2);
	add_res("lsl.a", OPCODE, OPC_LSL + 0);
	add_res("lsl.b", OPCODE, OPC_LSL + 1);
	add_res("lsl.w", OPCODE, OPC_LSL + 2);
	add_res("lsr.a", OPCODE, OPC_LSR + 0);
	add_res("lsr.b", OPCODE, OPC_LSR + 1);
	add_res("lsr.w", OPCODE, OPC_LSR + 2);
	add_res("and.a", OPCODE, OPC_AND + 0);
	add_res("and.b", OPCODE, OPC_AND + 1);
	add_res("and.w", OPCODE, OPC_AND + 2);
	add_res("xor.a", OPCODE, OPC_XOR + 0);
	add_res("xor.b", OPCODE, OPC_XOR + 1);
	add_res("xor.w", OPCODE, OPC_XOR + 2);
	add_res("or.a", OPCODE, OPC_OR + 0);
	add_res("or.b", OPCODE, OPC_OR + 1);
	add_res("or.w", OPCODE, OPC_OR + 2);
	add_res("sgt.a", OPCODE, OPC_SGT + 0);
	add_res("sgt.b", OPCODE, OPC_SGT + 1);
	add_res("sgt.w", OPCODE, OPC_SGT + 2);
	add_res("slt.a", OPCODE, OPC_SLT + 0);
	add_res("slt.b", OPCODE, OPC_SLT + 1);
	add_res("slt.w", OPCODE, OPC_SLT + 2);
	add_res("ld.a", OPCODE, OPC_LD + 0);
	add_res("ld.b", OPCODE, OPC_LD + 1);
	add_res("ld.w", OPCODE, OPC_LD + 2);
	add_res("neg.a", OPCODE, OPC_NEG + 0);
	add_res("neg.b", OPCODE, OPC_NEG + 1);
	add_res("neg.w", OPCODE, OPC_NEG + 2);
	add_res("not.a", OPCODE, OPC_NOT + 0);
	add_res("not.b", OPCODE, OPC_NOT + 1);
	add_res("not.w", OPCODE, OPC_NOT + 2);
	add_res("st.a", OPCODE, OPC_ST + 0);
	add_res("st.b", OPCODE, OPC_ST + 1);
	add_res("st.w", OPCODE, OPC_ST + 2);
	add_res("jz.a", OPCODE, OPC_JZ + 0);
	add_res("jz.b", OPCODE, OPC_JZ + 1);
	add_res("jz.w", OPCODE, OPC_JZ + 2);
	add_res("jnz.a", OPCODE, OPC_JNZ + 0);
	add_res("jnz.b", OPCODE, OPC_JNZ + 1);
	add_res("jnz.w", OPCODE, OPC_JNZ + 2);
	add_res("jsb.a", OPCODE, OPC_JSB + 0);
	add_res("jsb.b", OPCODE, OPC_JSB + 1);
	add_res("jsb.w", OPCODE, OPC_JSB + 2);
	add_res("rsb.a", OPCODE, OPC_RSB + 0);
	add_res("rsb.b", OPCODE, OPC_RSB + 1);
	add_res("rsb.w", OPCODE, OPC_RSB + 2);
	add_res("push.a", OPCODE, OPC_PUSH + 0);
	add_res("push.b", OPCODE, OPC_PUSH + 1);
	add_res("push.w", OPCODE, OPC_PUSH + 2);
	add_res("pshr.a", OPCODE, OPC_PSHR + 0);
	add_res("pshr.b", OPCODE, OPC_PSHR + 1);
	add_res("pshr.w", OPCODE, OPC_PSHR + 2);
	add_res("popr.a", OPCODE, OPC_POPR + 0);
	add_res("popr.b", OPCODE, OPC_POPR + 1);
	add_res("popr.w", OPCODE, OPC_POPR + 2);

	// reserved words
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
	fputs("X-Assembler, Version ", stdout);
	fputs(getversion(), stdout);
	fputs("\n\n", stdout);

	fputs("usage: xasm <file>[.<ext>]\n", stdout);
	fputs("  -a <file>[.<ext>]]\tListing\n", stdout);
	fputs("  -c <file>[.<ext>]]\tObject output\n", stdout);
	fputs("  -v\t\t\tVerbose\n", stdout);
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
			fputs("Pass 1\n", stdout);

		readline(); // fetch first line
		parse(); // GO !!!
		save_seg_size();

		if (!inphdl)
			fputs("%%missing .END statement\n", stdout);
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
			fputs("Pass 2\n", stdout);

		readline(); // fetch first line
		parse(); // GO !!!

		sto_cmd(REL_END, 0);
	}

	if (lishdl) {
		fputs("code  : 0x", lishdl);
		fput0x(maxpos[CODESEG] & 0xffff, 4, lishdl);
		fputs(" (", lishdl);
		fputd(maxpos[CODESEG] & 0xffff, lishdl);
		fputs(")\n", lishdl);

		fputs("data  : 0x", lishdl);
		fput0x(maxpos[DATASEG] & 0xffff, 4, lishdl);
		fputs(" (", lishdl);
		fputd(maxpos[DATASEG] & 0xffff, lishdl);
		fputs(")\n", lishdl);

		fputs("text  : 0x", lishdl);
		fput0x(maxpos[TEXTSEG] & 0xffff, 4, lishdl);
		fputs(" (", lishdl);
		fputd(maxpos[TEXTSEG] & 0xffff, lishdl);
		fputs(")\n", lishdl);

		fputs("udef  : 0x", lishdl);
		fput0x(maxpos[UDEFSEG] & 0xffff, 4, lishdl);
		fputs(" (", lishdl);
		fputd(maxpos[UDEFSEG] & 0xffff, lishdl);
		fputs(")\n", lishdl);

		j = 0;
		for (i = 0; i < NAMEMAX; ++i) if (names[i * NLAST + NCHAR]) ++j;

		fputs("Names : ", lishdl);
		fputd(j, lishdl);
		fputc('(', lishdl);
		fputd(NAMEMAX, lishdl);
		fputs(")\n", lishdl);
	}

	return errflag;
}
