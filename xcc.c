/*
 * X-C-Compiler/Assembler/Linker/Archiver
 *
 * @date 2020-06-02 15:51:08
 *
 * Known issues:
 * 	- Pointer arithmetic.
 * 	  Only "&arr[n]" is checked to work. All code is written in this style.
 * 	- Advanced pointers like "char ***p".
 * 	  Implementations should work, only declarations are lacking.
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
	SBUFMAX = 256,		// Size of source buffer
	SWMAX = 100,		// Number of switch cases
	SYMMAX = 300,		// Number of identifiers
};

/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,		// Bytes per word
	LOGBPW = 1,		// 1<<LOGBPW == BPW
	SBIT = 15,		// Bit number of signed bit
	REGMAX = 16,		// Number of registers
};

/*
 * Possible values for storage "CLASS"
 */

enum {
	EXTERNAL = 1,	// external global variables
	STATIC,		// static global variables
	GLOBAL,		// global variables
	SP_AUTO,	// local variables
	AP_AUTO,	// procedure arguments
	REGISTER,	// register variables
};

/*
 * Possible values for data "TYPE"
 */

enum {
	ADDRESS = 1,
	MEMORY,
	FUNCTION,
	BRANCH,
};

/*
 * Definitions for lval  (Code generated node)
 */

//#define assert(N) ({ __assert_fail ("0", __FILE__, __LINE__, __PRETTY_FUNCTION__); })
enum {
	LTYPE = 0,
	LPTR,
	LSIZE,
	LNAME,
	LVALUE,
	LREG,
	LTRUE,
	LFALSE,
	LLAST, // LLAST=8 which generates optimized code for "[n*LLAST]"
};

/*
 * Definitions for identifiers
 */

enum {
	ISYM = 0,		// identifier/symbol name
	ICLASS,			// storage class
	ITYPE,			// peephole data type
	IPTR,			// Number of indirects to basetype
	ISIZE,			// size in bytes.
	INAME,			// EA name
	IVALUE,			// EA value/offset
	IREG,			// EA register indirect
	ILAST,			// ILAST=8 which generates optimized code for "[n*ILAST]"
};

/*
 * Definitions for switches
 */

enum {
	SCASE = 0,
	SLABEL,
	SLAST,
};

/*
 * Reserved registers
 */

enum {
	REG_SP = 15,
	REG_AP = 14,
	REG_RETURN = 1,
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
 * Compiler-generated p-codes
 */

enum {
	TOK_MUL = 1,
	TOK_DIV,
	TOK_MOD,
	TOK_ADD,
	TOK_SUB,
	TOK_LSL,
	TOK_LSR,
	TOK_AND,
	TOK_XOR,
	TOK_OR,
	TOK_SGT,
	TOK_SLT,
	TOK_LD,
	TOK_NEG,
	TOK_NOT,
	// the following are considered flow-control, a concept unknown to untangle.
	TOK_ST,
	TOK_JZ,
	TOK_JNZ,
	TOK_JSB,
	TOK_RSB,
	// the following are considered convenience instruction to interact with stack
	TOK_PUSH,
	TOK_PSHR,
	TOK_POPR,
};

/*
 * character properties
 */

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

int	csp;			// stackpointer seen from scope coding
int	currseg;		// Current output segment
int	debug;			// Add debug info to output
int	errflag;		// True if an error has occurred
int	inphdl;			// handle for .C file
int	inplnr;			// Linenumber of .C file
int	lishdl;			// handle for .LIS file
int	maklis;			// Listing -h specified
int	nametab[NAMEMAX];	// Name table
int	nxtlabel;		// Next label number
int	outhdl;			// handle for .ASM file
int	prevseg;		// Previous output segment
int	reglock;		// Register locked by 'register' vars
int	regsum;			// Summary of all used registers
int	reguse;			// Currently used registers
int	regresvd;		// reserved registers
int	sw[SWMAX*SLAST];
int	swinx;			// Position in switch table
int	symidx;			// Next free identifier
int	syms[SYMMAX*ILAST];	// Symbols/identifiers
int	verbose;		// Verbose -v specified
char	ch;			// Current character in line being scanned
char	ctype[256];		// character properties
char	inpfn[PATHMAX];		// input filename
char	*lptr;			// Pointer to current character in input buffer
char	namech[NAMEMAX];
char	nch;			// Next character in line being scanned
char	outfn[PATHMAX];		// output filename
char	sbuf[SBUFMAX];

extern char *stdout;
//exit(int code);
//expression(int lval[]);
//fclose(int hdl);
//fgets(char *str, int siz, int hdl);
//fopen(char *name, char *mode);
//fputc(int ch, int hdl);
//getversion();
//preprocess();
//strcpy(char *dst, char *src);
//strlen(char *str);
//strncpy(char *dst, int n, char *src);

/*
 * Generate error messages
 */
warning(char *msg) {
	// Display original line
	fputd(inplnr, stdout);
	fputs(": ", stdout);
	fputs(sbuf, stdout);
	fputs("\n%", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);

	fputs(";% ", outhdl);
	fputs(msg, outhdl);
	fputc('\n', outhdl);
}

expected(char *lit) {
	// Display original line
	fputd(inplnr, stdout);
	fputs(": ", stdout);
	fputs(sbuf, stdout);
	fputs("\n", stdout);
	fputs(lit, stdout);
	fputs(" expected\n", stdout);

	fputs(";% ", outhdl);
	fputs(lit, outhdl);
	fputc(" expected\n", outhdl);
	errflag = 1;
}

error(char *msg) {
	warning(msg);
	errflag = 1;
}

fatal(char *msg) {
	error(msg);
	exit(1);
}

exprerr() {
	error("Invalid expression");
}

illname() {
	error("illegal symbol name");
}

multidef() {
	error("identifier already defined");
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
			if (maklis) {
				fputs("; ", outhdl);
				fputd(inplnr, outhdl);
				fputc(' ', outhdl);
				fputs(sbuf, outhdl);
				if (sbuf[strlen(sbuf)-1] != '\n')
					fputc('\n', outhdl);
			}
		}
	}
}

/*
 * Get next character from current line
 */
gch() {
	register int c;

	c = ch;
	if (!nch) {
		readline();
		lptr = sbuf;
	} else {
		++lptr;
	}

	// todo: remove all references to lptr and replace next with "nch = *lptr";
	ch = lptr[0];
	nch = ch ? lptr[1] : 0;

	return c;
}

/*
 * Bump/skip next characters in line
 */
bump(int n) {
	while (n) {
		gch();
		--n;
	}
}

/*
 * Skip all spaces until next non-space
 */
blanks() {
	while (1) {
		if (ch == '/' && nch == '/') {
			while (ch && ch != '\n')
				gch();
		}

		if (ch == '/' && nch == '*') {
			while (inphdl && (ch != '*' || nch != '/'))
				gch();
			bump(2);
		}

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

	if (ch != '\\' || nch == 0)
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
	if (!lit[1]) {
		if ((lptr[1] == '=') || (lptr[1] == lptr[0]))
			return 0;
		bump(1);
	} else {
		if (lptr[1] != lit[1])
			return 0;
		if (!lit[2]) {
			if (lptr[2] == '=')
				return 0;
			bump(2);
		} else {
			if (lptr[2] != lit[2])
				return 0;
			bump(3);
		}
	}
	return 1;
}

/*
 *
 */
needtoken(char *lit) {
	if (match(lit))
		return;

	expected(lit);

	// swallow chars until match
	while (inphdl && !match(lit))
		gch();
}

/*
 * semicolon enforcer
 */
semicolon() {
	needtoken(";");
}

/*
 * reconstruct the symbol name
 */
symname(register int tab) {
	register int i;

	i = nametab[tab];
	if (i)
		symname(i);
	fputc(namech[tab], outhdl);
}

/*
 * Get the (unique) hashed value for symbol, return length
 */
dohash(register char *name, int *retval) {
	register int start, hash, tab, len;

	if (!(ctype[*name] & CSYMFIRST))
		return 0; // Not a symbol

	tab = 0;
	len = 0;
	hash = 0;
	while (ctype[*name] & CSYMNEXT) {
		start = hash = (hash + *name * *name) % NAMEMAX;
		while (1) {
			if ((namech[hash] == *name) && (nametab[hash] == tab)) {
				tab = hash;
				break; // Inner loop
			} else if (!namech[hash]) {
				namech[hash] = *name;
				nametab[hash] = tab;
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
 * change to a new segment
 */
toseg(register int newseg) {
	prevseg = currseg;
	if (currseg == newseg)
		return 0;
	if (newseg == CODESEG)
		fputs("\t.code\n", outhdl);
	else if (newseg == DATASEG)
		fputs("\t.data\n", outhdl);
	else if (newseg == TEXTSEG)
		fputs("\t.text\n", outhdl);
	else if (newseg == UDEFSEG)
		fputs("\t.udef\n", outhdl);
	currseg = newseg;
}

/*
 * Generate label definition
 */
genlabel(int lbl) {
	fputc('_', outhdl);
	fputd(lbl, outhdl);
	fputc(':', outhdl);
}

/*
 * Generate a label assignment
 */
genequ(int lbl, int equ) {
	fputc('_', outhdl);
	fputd(lbl, outhdl);
	fputs("=_", outhdl);
	fputd(equ, outhdl);
	fputc('\n', outhdl);
}

/*
 * Generate a assembler statement
 */
genopc(int opc) {
	switch (opc) {
	case TOK_NEG : fputs("\tneg", outhdl); break;
	case TOK_NOT : fputs("\tnot", outhdl); break;
	case TOK_MUL : fputs("\tmul", outhdl); break;
	case TOK_DIV : fputs("\tdiv", outhdl); break;
	case TOK_MOD : fputs("\tmod", outhdl); break;
	case TOK_ADD : fputs("\tadd", outhdl); break;
	case TOK_SUB : fputs("\tsub", outhdl); break;
	case TOK_LSL : fputs("\tlsl", outhdl); break;
	case TOK_LSR : fputs("\tlsr", outhdl); break;
	case TOK_AND : fputs("\tand", outhdl); break;
	case TOK_XOR : fputs("\txor", outhdl); break;
	case TOK_OR  : fputs("\tor", outhdl); break;
	case TOK_SGT: fputs("\tsgt", outhdl); break;
	case TOK_SLT: fputs("\tslt", outhdl); break;
	case TOK_JSB : fputs("\tjsb", outhdl); break;
	case TOK_RSB : fputs("\trsb", outhdl); break;
	case TOK_LD: fputs("\tld", outhdl); break;
	case TOK_ST: fputs("\tst", outhdl); break;
	case TOK_JZ: fputs("\tjz", outhdl); break;
	case TOK_JNZ: fputs("\tjnz", outhdl); break;
	case TOK_PUSH: fputs("\tpush", outhdl); break;
	case TOK_PSHR: fputs("\tpshr", outhdl); break;
	case TOK_POPR: fputs("\tpopr", outhdl); break;
	}
}

gencode(int opc, int size, int lreg, int name, int ofs, int rreg) {
	// sign extend
	ofs |= -(ofs & (1 << SBIT));

	genopc(opc);

	if (size == 0)
		fputs(".a", outhdl);
	else if (size == 1)
		fputs(".b", outhdl);
	else if (size == BPW)
		fputs(".w", outhdl);
	else
		fatal("unimplemented");

	fputs("\tr", outhdl);
	fputd(lreg,outhdl);
	fputc(',', outhdl);
	if (name) {
		fputc('_', outhdl);
		if (name > 0)
			symname(name);
		else
			fputd(-name, outhdl);
	}
	if (ofs > 0) {
		fputc('+', outhdl);
		fputd(ofs, outhdl);
	} else if (ofs < 0)
		fputd(ofs, outhdl);
	if (rreg) {
		fputs("(r", outhdl);
		fputd(rreg, outhdl);
		fputc(')', outhdl);
	}

	// everything zero
	if (!name && !ofs && !rreg)
		fputc('0', outhdl);

	fputc('\n', outhdl);
}

gencode_L(int opc, int reg, int lbl) {
	gencode(opc, 0, reg, -lbl, 0, 0);
}

gencode_lval(int opc, int lreg, int *lval) {
	int name, ofs, rreg, size;
	name = lval[LNAME];
	ofs = lval[LVALUE];
	rreg = lval[LREG];

	// last-second conversion of BRANCH to value
	if (lval[LTYPE] == BRANCH)
		loadlval(lval, -1);

	if (lval[LTYPE] == ADDRESS || lval[LTYPE] == FUNCTION)
		size = 0;
	else if (isWORD(lval))
		size = BPW;
	else
		size = 1;

	gencode(opc, size, lreg, name, ofs, rreg);
}

/*
 * Allocate a free register
 */
allocreg() {
	register int i, mask;

	for (i = 2; i < REGMAX; ++i) {
		mask = 1 << i;
		if (!((regresvd | reguse) & mask)) {
			regsum |= reguse |= mask;
			return i;
		}
	}
	error("out of registers");
	return 2; // First modifiable reg
}

/*
 * Return a register into the free list
 */
freereg(register int reg) {
	register int mask;

	mask = 1 << reg;
	if (!(regresvd & mask)) {
		if (reguse & mask) {
			reguse &= ~mask;
			reglock &= ~mask;
		} else
			error("double register deallocation");
	}
}

/*
 * @date 2020-05-23 17:09:01
 *
 * Test if lval is a constant stored in `lval[LVALUE]`
 */
isConstant(register int lval[]) {
	return (lval[LTYPE] == ADDRESS && lval[LNAME] == 0 && lval[LREG] == 0);
}

/*
 * @date 2020-05-24 14:02:47
 *
 * Test if lval is a register stored in `lval[LREG]`
 */
isRegister(register int lval[]) {
	return (lval[LTYPE] == ADDRESS && lval[LNAME] == 0 && lval[LVALUE] == 0);
}

/*
 * Test if storage is BPW large. "int*" "char*" "int".
 */
isWORD(register int lval[]) {
	return lval[LPTR]  || lval[LSIZE] == BPW;
}

/*
 * Test if "int*".
 */
isINTPTR(register int lval[]) {
	return lval[LPTR]  && lval[LSIZE] == BPW;
}

/*
 * Free all registers assigned to a lval
 */
freelval(register int lval[]) {
	if (!(reglock & (1 << lval[LREG])))
		freereg(lval[LREG]);
}

/*
 * Load a lval into a register
 *
 * If 'reg' == -1 then register is mode is 'register read'. This
 * mode is same as 'reg' == 0 except reserved registers are not
 * copied to 'free' registers
 */
loadlval(register int lval[], register int reg) {
	register int srcreg;

	// Sign extend to fix being called with negative constant when compiled with "-Dint=long"
	reg |= -(reg & (1 << SBIT));

	if (lval[LTYPE] == ADDRESS) {
		// assign resulting register
		if (reg > 0) {
			// loaded into fixed register
			freelval(lval);
		} else if (lval[LNAME] || lval[LVALUE]) {
			// need code to evaluate, result always in new register
			freelval(lval);
			reg = allocreg();
		} else if (reg == 0 && (reglock & (1 << lval[LREG]))) {
			// reserved/locked register to writable register
			reg = allocreg();
		} else if (lval[LNAME] == 0 && lval[LVALUE] == 0) {
			// already in register
			return;
		} else {
			// reuse register
			reg = lval[LREG];
		}

		// generate code
		gencode_lval(TOK_LD, reg, lval);

		// Modify lval
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else if (lval[LTYPE] == MEMORY) {
		freelval(lval);
		if (reg <= 0)
			reg = allocreg();
		gencode_lval(TOK_LD, reg, lval);

		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else if (lval[LTYPE] == BRANCH) {
		int lend;
		lend = ++nxtlabel;

		if (reg <= 0)
			reg = allocreg();
		if (!lval[LFALSE])
			lval[LFALSE] = ++nxtlabel;

		gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		if (lval[LTRUE])
			genlabel(lval[LTRUE]);
		gencode(TOK_LD, 0, reg, 0, 1, 0);
		gencode_L(TOK_JZ, 0, lend);
		genlabel(lval[LFALSE]);
		gencode(TOK_LD, 0, reg, 0, 0, 0);
		genlabel(lend);

		freelval(lval);
		lval[LTYPE] = ADDRESS;
		lval[LPTR] = 0;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else
		fatal("unimplemented");
}

/*
 * Inject assembler into output and catch return value
 */
doasm(register int lval[]) {
	needtoken("(");

	if (!match("\"")) {
		expected("string");
	} else {
		fputc('\t', outhdl);
		while (ch && (ch != '"'))
			fputc(litchar(), outhdl);
		fputc('\n', outhdl);

		gch(); // skip terminator
	}

	blanks();
	needtoken(")");

	// make R1 available
	lval[LTYPE] = ADDRESS;
	lval[LPTR] = 0;
	lval[LSIZE] = BPW;
	lval[LNAME] = 0;
	lval[LVALUE] = 0;
	lval[LREG] = REG_RETURN;
}

/*
 * Get a numerical constant
 */
number(register int *val) {
	register int i, minus;

	i = minus = 0;
	if (!(ctype[ch] & CISDIGIT))
		return 0;
	if (ch == '0' && (nch == 'x' || nch == 'X'))  {
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

	/*
	 * @date 2020-06-09 02:02:25
	 * 0x8000 becomes 32768 (32-bits) and -32768 (16-bits).
	 */
	// sign extend
	i |= -(i & (1 << SBIT));

	*val = i;
	return 1;
}

/*
 * Load a constant value
 */
constant(register int lval[]) {
	lval[LTYPE] = ADDRESS;
	lval[LPTR] = 0;
	lval[LSIZE] = 0;
	lval[LNAME] = 0;
	lval[LVALUE] = 0;
	lval[LREG] = 0;

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

	// start of string
	if (!match("\""))
		return 0;

	int lbl, prevseg;
	lbl = ++nxtlabel;
	prevseg = currseg;

	toseg(TEXTSEG);
	genlabel(lbl);
	fputs("\t.dcb\t\"", outhdl);

	while (ch && (ch != '"')) {
		if (ch == '\\') {
			fputc(ch, outhdl);
			gch();
		}
		fputc(ch, outhdl);
		gch();
	}
	fputs("\",0\n", outhdl);
	gch(); // skip terminator

	toseg(prevseg);

	// Convert to array of char
	lval[LTYPE] = ADDRESS;
	lval[LPTR] = 1;
	lval[LSIZE] = 1;
	lval[LNAME] = -lbl;
	lval[LVALUE] = 0;
	lval[LREG] = 0;

	return 1;
}

/*
 * Load primary expression
 */
primary(register int lval[]) {
	register int *sym, i;
	int sname, len;

	if (match("(")) {  // (expression,...)
		expression(lval);
		needtoken(")");
		return 1;
	}

	// test for "asm()"
	if (amatch("asm")) {
		doasm(lval);
		return 1;
	}

	// load identifier
	if (!(len = dohash(lptr, &sname)))
		return constant(lval);
	bump(len); // Skip identifier

	// identifier. Scan in reverse order of creation.
	for (i = symidx - 1; i >= 0; --i) {
		sym = &syms[i * ILAST];
		if (sym[ISYM] == sname) {
			lval[LTYPE] = sym[ITYPE];
			lval[LPTR] = sym[IPTR];
			lval[LSIZE] = sym[ISIZE];
			lval[LNAME] = sym[INAME];
			lval[LVALUE] = sym[IVALUE];
			lval[LREG] = sym[IREG];

			if (lval[LREG] == REG_SP)
				lval[LVALUE] = lval[LVALUE] - csp;

			return 1;
		}
	}

	// todo: too soon, drop pre-processor first
	// warning("undefined identifier");

	// could be external function names
	lval[LTYPE] = FUNCTION;
	lval[LPTR] = 0;
	lval[LSIZE] = BPW;
	lval[LNAME] = sname;
	lval[LVALUE] = 0;
	lval[LREG] = 0;

	return 1;
}

/*
 * Process a constant evaluation
 */
calc(register int left, int opc, int right) {
	// sign extend
	left |= -(left & (1 << SBIT));
	right |= -(right & (1 << SBIT));

	switch (opc) {
	case TOK_MUL: return (left * right);
	case TOK_DIV: return (left / right);
	case TOK_MOD: return (left % right);
	case TOK_ADD: return (left + right);
	case TOK_SUB: return (left - right);
	case TOK_LSL: return (left << right);
	case TOK_LSR: return (left >> right);
	case TOK_AND: return (left & right);
	case TOK_XOR: return (left ^ right);
	case TOK_OR : return (left | right);
	default: return 0;
	}
}

/*
 * Shared code generation used within expressions
 */
gencode_expr(int tok, register int lval[], register int rval[]) {
	// Generate code
	if (isConstant(lval) && isConstant(rval)) {
		lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
	} else {
		loadlval(lval, 0);

		// Perform operation and release rval
		gencode_lval(tok, lval[LREG], rval);
		freelval(rval);
	}
}

/*
 * Auto increment/decrement
 */
prestep(register int pre, register int lval[]) {
	register int reg, step;

	step = isINTPTR(lval) ? BPW : 1;

	if (isRegister(lval) && (reglock & (1 << lval[LREG]))) {
		gencode(pre, 0, lval[LREG], 0, step, 0);
	} else if (lval[LTYPE] == MEMORY) {
		// load memory into register
		reg = allocreg();
		gencode_lval(TOK_LD, reg, lval);
		// increment/decrement
		gencode(pre, 0, reg, 0, step, 0);
		// store
		gencode_lval(TOK_ST, reg, lval);

		freelval(lval);
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else
		expected("lvalue");
}

/*
 * @date 2020-06-06 00:47:25
 * Note that the return lval needs to be based to the original value or "if (!i++)" will fail when "i=0"
 */
poststep(register int post, register int lval[]) {
	register int reg, step, reg2;

	step = isINTPTR(lval) ? BPW : 1;

	if (isRegister(lval) && (reglock & (1 << lval[LREG]))) {
		reg = allocreg();
		// copy modified original
		gencode_lval(TOK_LD, reg, lval);
		// increment/decrement original
		gencode(post, 0, lval[LREG], 0, step, 0);
		// continue with copy (of original)
		freelval(lval);
		lval[LREG] = reg;
	} else if (lval[LTYPE] == MEMORY) {
		// load memory into register
		reg = allocreg();
		gencode_lval(TOK_LD, reg, lval);
		// increment/decrement into new register
		reg2 = allocreg();
		gencode(TOK_LD, 0, reg2, 0, (post == TOK_ADD) ? step : -step, reg);
		// writeback
		gencode_lval(TOK_ST, reg2, lval);
		freereg(reg2);

		freelval(lval);
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else
		expected("lvalue");
}

/*
 *
 */
expr_postfix(register int lval[]) {
	int rval[LLAST], sav_csp;
	register int argc, reg;

	if (!primary(lval))
		return 0;
	if (match("[")) { // [subscript]
		if (!lval[LPTR])
			error("can't subscript");
		else if (!expression(rval))
			error("need subscript");
		else {
			if (isConstant(rval)) {
				if (lval[LTYPE] == MEMORY)
					loadlval(lval, 0); // load if pointer
				// Subscript is a constant + pointer arithmetic
				lval[LVALUE] += rval[LVALUE] * lval[LSIZE];
			} else {
				// Subscript is a variable/complex-expression
				if (lval[LTYPE] == MEMORY)
					loadlval(lval, 0); // load if pointer

				// pointer arithmetic
				loadlval(rval, 0);
				if (isINTPTR(lval))
					gencode(TOK_LSL, 0, rval[LREG], 0, LOGBPW, 0); // size index

				// merge
				if (!lval[LREG])
					lval[LREG] = rval[LREG];
				else {
					/*
					 * @date 2020-05-23 01:14:18
					 * This is an important location.
					 * having a second indirect register removes the following code.
					 * however, a single register is a much simpler implementation.
					 */
					if ((1 << lval[LREG]) & reglock) {
						// register in lval is locked and needs to be made writable
						// @date 2020-06-10 22:44:40
						loadlval(lval,0);
					}
					gencode_lval(TOK_ADD, lval[LREG], rval);
					freelval(rval);
				}
			}
			// Update data type
			lval[LTYPE] = MEMORY;
			--lval[LPTR]; // deference pointer
		}
		needtoken("]");
	}
	if (match("(")) { // function (...)
		if (lval[LTYPE] != FUNCTION)
			error("Illegal function");

		argc = 0;
		sav_csp = csp;
		blanks();
		while (ch != ')') {
			// Get expression
			expr_assign(rval);
			// Push onto stack
			gencode_lval(TOK_PUSH, REG_SP, rval);
			freelval(rval);
			// increment ARGC
			csp += BPW;
			argc += BPW;

			if (!match(","))
				break;
		}
		needtoken(")");

		// call
		gencode_lval(TOK_JSB, REG_SP, lval);
		freelval(lval);

		// Pop args
		if (argc)
			gencode(TOK_SUB, 0, REG_SP, 0, argc, 0);

		csp = sav_csp;

		lval[LTYPE] = ADDRESS;
		lval[LPTR] = 0;
		lval[LSIZE] = BPW;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = REG_RETURN;
	}

	if (match("++")) {
		poststep(TOK_ADD, lval);
	} else if (match("--")) {
		poststep(TOK_SUB, lval);
	}

	return 1;
}

/*
 *
 */
expr_unary(register int lval[]) {

	if (match("++")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		prestep(TOK_ADD, lval);
	} else if (match("--")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		prestep(TOK_SUB, lval);
	} else if (match("~")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = ~lval[LVALUE];
		else {
			loadlval(lval, -1);
			freelval(lval);
			int reg;
			reg = allocreg();
			gencode_lval(TOK_NOT, reg, lval);
			// continue with new register
			lval[LREG] = reg;
		}
	} else if (match("!")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = !lval[LVALUE];
		else if (lval[LTYPE] == BRANCH) {
			// invert opcode in peephole
			lval[LVALUE] = TOK_JZ + TOK_JNZ - lval[LVALUE];
			// swap labels
			int sav;
			sav = lval[LTRUE];
			lval[LTRUE] = lval[LFALSE];
			lval[LFALSE] = sav;
		} else {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JNZ;
			// keep `LREG`
			lval[LTRUE] = 0;
			lval[LFALSE] = 0;
		}
	} else if (match("-")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = -lval[LVALUE];
		else {
			loadlval(lval, -1);
			freelval(lval);
			int reg;
			reg = allocreg();
			gencode_lval(TOK_NEG, reg, lval);
			// continue with new register
			lval[LREG] = reg;
		}
	} else if (match("+")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
	} else if (match("*")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (!lval[LPTR])
			error("can't dereference");
		else {
			if (lval[LTYPE] == MEMORY)
				loadlval(lval, 0); // load if pointer
			lval[LTYPE] = MEMORY;
			--lval[LPTR]; // deference pointer
		}
	} else if (match("&")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (lval[LTYPE] != MEMORY)
			expected("lvalue");
		else {
			lval[LTYPE] = ADDRESS;
			++lval[LPTR];
		}
	} else {
		if (!expr_postfix(lval))
			return 0;
	}
	return 1;
}

/*
 *
 */
expr_muldiv(int lval[]) {
	// Load lval
	if (!expr_unary(lval))
		return 0;

	while (1) {
		int tok, rval[LLAST];

		if (omatch("*"))
			tok = TOK_MUL;
		else if (omatch("/"))
			tok = TOK_DIV;
		else if (omatch("%"))
			tok = TOK_MOD;
		else
			return 1;

		// Load rval
		if (!expr_unary(rval)) {
			exprerr();
			return 1;
		}

		if (tok != TOK_MUL) {
			gencode_expr(tok, lval, rval);
		} else if (isConstant(lval) && isConstant(rval)) {
			// constant
			lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
		} else if ((isConstant(lval) && lval[LVALUE] == 0) || (isConstant(rval) && rval[LVALUE] == 0)) {
			// zero
			freelval(lval);
			freelval(rval);
			lval[LTYPE] = ADDRESS;
			lval[LNAME] = 0;
			lval[LVALUE] = 0;
			lval[LREG] = 0;
		} else if (isConstant(lval) && lval[LVALUE] == 1) {
			// 1*rval
			lval[LTYPE] = rval[LTYPE];
			lval[LSIZE] = rval[LSIZE];
			lval[LPTR] = rval[LPTR];
			lval[LNAME] = rval[LNAME];
			lval[LVALUE] = rval[LVALUE];
			lval[LREG] = rval[LREG];
		} else if (isConstant(rval) && rval[LVALUE] == 1) {
			// leave `lval[]` untouched
		} else if (isConstant(rval)) {
			// convert to shift
			int shift;
			for (shift=0; shift<BPW*8; shift++) {
				if (rval[LVALUE] == (1<<shift)) {
					loadlval(lval, 0);
					tok = TOK_LSL;
					gencode(tok, 0, lval[LREG], 0, shift, 0);
					break;
				}
			}
			if (tok != TOK_LSL)
				gencode_expr(tok, lval, rval);
		} else {
			gencode_expr(tok, lval, rval);
		}
	}
}

/*
 *
 */
expr_addsub(int lval[]) {
	// Load lval
	if (!expr_muldiv(lval))
		return 0;

	while (1) {
		int tok, rval[LLAST];

		if (omatch("+"))
			tok = TOK_ADD;
		else if (omatch("-"))
			tok = TOK_SUB;
		else
			return 1;

		// Load rval
		if (!expr_muldiv(rval)) {
			exprerr();
			return 1;
		}

		// pointer arithmetic, scale index
		if (isINTPTR(lval)) {
			if (isConstant(rval)) {
				rval[LVALUE] <<= LOGBPW;
			} else {
				loadlval(rval, 0);
				gencode(TOK_LSL, 0, rval[LREG], 0, LOGBPW, 0);
			}
		}

		// Generate code
		if (lval[LTYPE] == ADDRESS && isConstant(rval)) {
			lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
		} else {
			loadlval(lval, 0);

			// Perform operation and release rval
			gencode_lval(tok, lval[LREG], rval);
			freelval(rval);
		}
	}
}

/*
 *
 */
expr_shift(int lval[]) {
	// Load lval
	if (!expr_addsub(lval))
		return 0;

	while (1) {
		int tok, rval[LLAST];

		if (omatch("<<"))
			tok = TOK_LSL;
		else if (omatch(">>"))
			tok = TOK_LSR;
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
expr_rel(int lval[]) {
	int tok, cond, rval[LLAST];

	// Load lval
	if (!expr_shift(lval))
		return 0;

	if (omatch("<=")) {
		tok = TOK_SGT;
		cond = TOK_JNZ;
	} else if (omatch(">=")) {
		tok = TOK_SLT;
		cond = TOK_JNZ;
	} else if (omatch("<")) {
		tok = TOK_SLT;
		cond = TOK_JZ;
	} else if (omatch(">")) {
		tok = TOK_SGT;
		cond = TOK_JZ;
	} else
		return 1;

	// Load rval
	if (!expr_shift(rval)) {
		exprerr();
		return 1;
	}

	// Generate code
	if (isConstant(lval) && isConstant(rval)) {
		if (tok == TOK_SGT)
			lval[LVALUE] = lval[LVALUE] > rval[LVALUE];
		else
			lval[LVALUE] = lval[LVALUE] < rval[LVALUE];
		if (cond == TOK_JNZ)
			lval[LVALUE] = !lval[LVALUE];
	} else {
		loadlval(lval, 0);

		// Compare and release rval
		gencode_lval(tok, lval[LREG], rval);
		freelval(rval);

		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = cond;
		// keep `LREG`
		lval[LTRUE] = 0;
		lval[LFALSE] = 0;
	}
	return 1;
}

/*
 *
 */
expr_equ(int lval[]) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_rel(lval))
		return 0;

	if (omatch("=="))
		tok = TOK_JNZ;
	else if (omatch("!="))
		tok = TOK_JZ;
	else
		return 1;

	// Load rval
	if (!expr_rel(rval)) {
		exprerr();
		return 1;
	}

	// Generate code
	if (isConstant(lval) && isConstant(rval)) {
		lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
	} else {
		loadlval(lval, 0);

		// Compare and release values
		gencode_lval(TOK_XOR, lval[LREG], rval);
		freelval(rval);

		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = tok;
		// keep `LREG`
		lval[LTRUE] = 0;
		lval[LFALSE] = 0;
	}
	return 1;
}

/*
 *
 */
expr_and(int lval[]) {
	// Load lval
	if (!expr_equ(lval))
		return 0;

	while (1) {
		int rval[LLAST];

		if (!omatch("&"))
			return 1;

		// Load rval
		if (!expr_equ(rval)) {
			exprerr();
			return 1;
		}

		// Generate code
		gencode_expr(TOK_AND, lval, rval);
	}

}

/*
 *
 */
expr_xor(int *lval) {
	// Load lval
	if (!expr_and(lval))
		return 0;

	while (1) {
		int rval[LLAST];

		if (!omatch("^"))
			return 1;

		// Load rval
		if (!expr_and(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(TOK_XOR, lval, rval);
	}
}

/*
 *
 */
expr_or(int *lval) {
	// Load lval
	if (!expr_xor(lval))
		return 0;

	while (1) {
		int rval[LLAST];

		if (!omatch("|"))
			return 1;

		// Load rval
		if (!expr_xor(rval)) {
			exprerr();
			return 1;
		}

		gencode_expr(TOK_OR, lval, rval);
	}
}

/*
 *
 */
expr_land(int lval[]) {
	int lfalse;

	// Load lval
	if (!expr_or(lval))
		return 0;

	if (!omatch("&&"))
		return 1;

	// lval must be BRANCH
	if (lval[LTYPE] != BRANCH) {
		loadlval(lval, -1);
		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = TOK_JZ;
		// keep `LREG`
		lval[LTRUE] = 0;
		lval[LFALSE] = 0;
	}
	// adopt LFALSE
	if (lval[LFALSE] == 0)
		lval[LFALSE] = ++nxtlabel; // need an initial final LFALSE
	lfalse = lval[LFALSE];

	while (1) {
		// emit peephole
		gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		freelval(lval);

		// emit LTRUE if referenced
		if (lval[LTRUE])
			genlabel(lval[LTRUE]);

		// Load next lval
		if (!expr_or(lval)) {
			exprerr();
			return 1;
		}

		// lval must be BRANCH
		if (lval[LTYPE] != BRANCH) {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JZ;
			// keep `LREG`
			lval[LTRUE] = 0;
			lval[LFALSE] = 0;
		}

		// cascade jumps
		if (lval[LFALSE])
			genequ(lval[LFALSE], lfalse);
		lval[LFALSE] = lfalse;

		if (!omatch("&&"))
			return 1;
	}
}

/*
 *
 */
expr_lor(int lval[]) {
	int ltrue;

	// Load lval
	if (!expr_land(lval))
		return 0;

	if (!omatch("||"))
		return 1;

	    // lval must be BRANCH
	    if (lval[LTYPE] != BRANCH) {
		    loadlval(lval, -1);
		    // Change lval to "BRANCH"
		    lval[LTYPE] = BRANCH;
		    lval[LVALUE] = TOK_JZ;
		    // keep `LREG`
		    lval[LTRUE] = 0;
		    lval[LFALSE] = 0;
	    }

	// adopt LTRUE
	if (!lval[LTRUE])
		lval[LTRUE] = ++nxtlabel; // need an initial final LTRUE
	ltrue = lval[LTRUE];

	while (1) {
		// emit peephole
		lval[LVALUE] = TOK_JZ + TOK_JNZ - lval[LVALUE];
		gencode_L(lval[LVALUE], lval[LREG], lval[LTRUE]);
		freelval(lval);

		// emit LFALSE if referenced
		if (lval[LFALSE])
			genlabel(lval[LFALSE]);

		// Load next lval
		if (!expr_land(lval)) {
			exprerr();
			return 1;
		}

		// lval must be BRANCH
		if (lval[LTYPE] != BRANCH) {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JZ;
			// keep `LREG`
			lval[LTRUE] = 0;
			lval[LFALSE] = 0;
		}

		// cascade jumps
		if (lval[LTRUE])
			genequ(lval[LTRUE], ltrue);
		lval[LTRUE] = ltrue;

		if (!omatch("||"))
			return 1;
	}
}

/*
 *
 */
expr_ternary(register int lval[]) {
	register int lend, reg;

	if (!expr_lor(lval))
		return 0;
	if (!match("?"))
		return 1;

	// If not a BRANCH then convert it into one
	if (lval[LTYPE] != BRANCH) {
		loadlval(lval, -1);
		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = TOK_JZ;
		// keep `LREG`
		lval[LTRUE] = 0;
		lval[LFALSE] = 0;
	}
	// alloc labels (copy to variable because of `loadlval()`
	int lfalse;
	if (!lval[LFALSE])
		lval[LFALSE] = ++nxtlabel;
	lfalse = lval[LFALSE];

	// condition and jump-when-false
	gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
	freelval(lval);

	// when-true expression
	if (lval[LTRUE])
		genlabel(lval[LTRUE]);
	expression(lval);
	loadlval(lval, 0);

	// save and release final register
	reg = lval[LREG];
	freelval(lval);

	needtoken(":");
	// jump to end
	lend = ++nxtlabel;
	gencode_L(TOK_JZ, 0, lend);

	// when-false expression
	genlabel(lfalse);
	if (!expr_ternary(lval))
		exprerr();

	// get outcome into same register as 'when-true' path
	if (!isRegister(lval) || lval[LREG] != reg) {
		// release `lval[]`, final register should be free
		freelval(lval);

		// reg should be free, allocate it
		if (reguse & (1 << reg))
			fatal("ternary register not free");
		reguse |= 1 << reg;

		// load into final register
		gencode_lval(TOK_LD, reg, lval);

		// Modify lval
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	}

	// common end label
	genlabel(lend);

	return 1;
}

/*
 * Load a numerical expression separated by comma's
 */
expr_assign(register int lval[]) {
	int rval[LLAST];
	register int opc;

	if (!expr_ternary(lval))
		return 0;

	// Test for assignment
	if (omatch("=")) opc = TOK_LD;
	else if (match("*=")) opc = TOK_MUL;
	else if (match("/=")) opc = TOK_DIV;
	else if (match("%=")) opc = TOK_MOD;
	else if (match("+=")) opc = TOK_ADD;
	else if (match("-=")) opc = TOK_SUB;
	else if (match("<<=")) opc = TOK_LSL;
	else if (match(">>=")) opc = TOK_LSR;
	else if (match("&=")) opc = TOK_AND;
	else if (match("^=")) opc = TOK_XOR;
	else if (match("|=")) opc = TOK_OR;
	else
		return 1;

	// test if lval modifiable
	if (!isRegister(lval) && lval[LTYPE] != MEMORY)
		expected("lvalue");

	// Get rval
	if (!expr_assign(rval)) {
		exprerr();
		return 1;
	}

	// pointer arithmetic, scale index
	if ((opc == TOK_ADD || opc == TOK_SUB) && isINTPTR(lval)) {
		if (isConstant(rval)) {
			rval[LVALUE] <<= LOGBPW;
		} else {
			loadlval(rval, 0);
			gencode(TOK_LSL, 0, rval[LREG], 0, LOGBPW, 0);
		}
	}

	if (isRegister(lval)) {
		gencode_lval(opc, lval[LREG], rval);
		freelval(rval);
	} else if (opc == TOK_LD) {
		loadlval(rval, -1); // rval needs to be register
		gencode_lval(TOK_ST, rval[LREG], lval);
		freelval(rval);
	} else {
		int reg;
		reg = allocreg();

		// load lvalue into new register
		gencode_lval(TOK_LD, reg, lval);
		// apply operator
		gencode_lval(opc, reg, rval);
		freelval(rval);
		// writeback
		gencode_lval(TOK_ST, reg, lval);

		// continue with register
		freelval(lval);
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	}

	return 1;
}

/*
 * Load a numerical expression separated by comma's
 */
expression(register int lval[]) {

	if (!expr_assign(lval)) {
		expected("expression");
		return 0;
	}

	while (match(",")) {
		freelval(lval);
		if (!expr_assign(lval)) {
			expected("expression");
			return 1;
		}
	}

	return 1;
}

/*
 * Load a constant expression
 */
constexpr(register int *val) {
	int lval[LLAST];

	if (!expr_ternary(lval))
		return 0;
	if (isConstant(lval)) {
		*val = lval[LVALUE];
		return 1;
	}
	expected("constant expression");
	freelval(lval);
	return 0;
}

/*
 * Dump the switch map and decoding
 */
dumpsw(int swbase, int codlbl, int endlbl) {
	register int lo, hi, i, j, cnt, *ptr;
	int maplbl, deflbl, lbl;

	if (swbase + 1 == swinx) {
		// no cases specified
		ptr = &sw[swbase * SLAST];
		if (ptr[SLABEL])
			gencode_L(TOK_JZ, 0, ptr[SLABEL]);
		return;
	}

	// get lo/hi bounds
	ptr = &sw[(swbase + 1) * SLAST];
	lo = hi = ptr[SCASE];
	for (i = swbase + 1; i < swinx; ++i) {
		ptr = &sw[i * SLAST];
		if (ptr[SCASE] > hi)
			hi = ptr[SCASE];
		if (ptr[SCASE] < lo)
			lo = ptr[SCASE];
	}

	// setup default
	if (!(deflbl = sw[swbase * SLAST + SLABEL]))
		deflbl = endlbl;

	// generate map
	maplbl = ++nxtlabel;
	toseg(DATASEG);
	genlabel(maplbl);
	cnt = 0;
	for (i = lo; i <= hi; ++i) {
		lbl = deflbl;
		for (j = swbase + 1; j < swinx; ++j) {
			ptr = &sw[j * SLAST];
			if (ptr[SCASE] == i) {
				lbl = ptr[SLABEL];
				break;
			}
		}
		if (!cnt++)
			fputs("\t.dcw\t", outhdl);
		else
			fputc(',', outhdl);
		fputc('_', outhdl);
		fputd(lbl, outhdl);
		if (cnt > 15) {
			fputs("\n", outhdl);
			cnt = 0;
		}
	}
	if (cnt)
		fputc('\n', outhdl);
	toseg(prevseg);

	// generate code (use j as reg)
	genlabel(codlbl);
	j = allocreg();

	// bounds check
	gencode(TOK_LD, 0, j, 0, 0, REG_RETURN);
	gencode(TOK_SLT, 0, j, 0, lo, 0);
	gencode_L(TOK_JNZ, j, deflbl);
	gencode(TOK_LD, 0, j, 0, 0, REG_RETURN);
	gencode(TOK_SGT, 0, j, 0, hi, 0);
	gencode_L(TOK_JNZ, j, deflbl);

	// jump
	gencode(TOK_SUB, 0, REG_RETURN, 0, lo, 0);
	gencode(TOK_LSL, 0, REG_RETURN, 0, LOGBPW, 0);
	gencode(TOK_LD, BPW, REG_RETURN, -maplbl, 0, REG_RETURN);
	gencode(TOK_JZ, 0, 0, 0, 0, REG_RETURN);
	freereg(j);
}

/*
 * process one statement
 */
statement(int swbase, int returnlbl, int breaklbl, int contlbl, int breaksp, int contsp) {
	int lval[LLAST], scope, sav_sw;
	register int i, *sym;
	int lbl1, lbl2, lbl3;

	if (match("{")) {
		int last_csp, sav_csp;

		last_csp = sav_csp = csp;
		scope = symidx;
		while (!match("}")) {
			blanks();
			if (!ch) {
				expected("}"); // EOF
				return;
			} else if (amatch("enum")) {
				declenum(scope);
			} else if (amatch("register")) {
				declvar(scope, REGISTER);
			} else if (declvar(scope, SP_AUTO)) {
			} else {
				// allocate locals
				if (csp != last_csp) {
					gencode(TOK_ADD, 0, REG_SP, 0, csp - last_csp, 0);
					last_csp = csp;
				}
				statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
			}
		}
		// unwind stack
		if (csp != sav_csp) {
			gencode(TOK_SUB, 0, REG_SP, 0, csp - sav_csp, 0);
			csp = sav_csp;
		}
		// free local registers
		for (i = scope; i < symidx; ++i) {
			sym = &syms[i * ILAST];
			if (sym[ICLASS] == REGISTER)
				freereg(sym[IREG]);
		}
		symidx = scope;
		return;
	}

	if (amatch("if")) {
		needtoken("(");
		expression(lval);
		needtoken(")");
		if (lval[LTYPE] == BRANCH) {
			if (!lval[LFALSE])
				lval[LFALSE] = ++nxtlabel;
			gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		} else {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JZ;
			// keep `LREG`
			lval[LTRUE] = 0;
			lval[LFALSE] = ++nxtlabel;
			gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		}
		if (lval[LTRUE])
			genlabel(lval[LTRUE]);
		freelval(lval);
		statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
		if (!amatch("else")) {
			// @date 2020-05-19 12:45:53
			//     compare
			// T:  statement
			// F:
			genlabel(lval[LFALSE]);
		} else {
			// @date 2020-05-19 12:45:53
			//     compare
			// T:  statement
			//     jmp L1
			// F:  statement
			// L1:
			lbl1 = ++nxtlabel;
			gencode_L(TOK_JZ, 0, lbl1);
			genlabel(lval[LFALSE]);
			statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
			genlabel(lbl1);
		}
	} else if (amatch("while")) {
		// @date 2020-05-19 12:39:49
		// L1: compare
		//     bcc F
		// T:  statement
		//     jmp L1
		// F:
		lbl1 = ++nxtlabel;
		genlabel(lbl1);
		needtoken("(");
		expression(lval);
		needtoken(")");

		if (lval[LTYPE] == BRANCH) {
			if (!lval[LFALSE])
				lval[LFALSE] = ++nxtlabel;
			gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		} else {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JZ;
			// keep `LREG`
			lval[LTRUE] = 0;
			lval[LFALSE] = ++nxtlabel;
			gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
		}
		if (lval[LTRUE])
			genlabel(lval[LTRUE]);
		freelval(lval);
		statement(swbase, returnlbl, lval[LFALSE], lbl1, csp, csp);
		gencode_L(TOK_JZ, 0, lbl1);
		genlabel(lval[LFALSE]);
	} else if (amatch("do")) {
		// @date 2020-05-19 12:37:46
		// L1: statement
		// L2: compare
		//     bcc L1
		// F:
		lbl1 = ++nxtlabel;
		lbl2 = ++nxtlabel;
		genlabel(lbl1);
		statement(swbase, returnlbl, lbl2, lbl1, csp, csp);
		genlabel(lbl2);
		needtoken("while");
		needtoken("(");
		expression(lval);
		needtoken(")");
		if (lval[LTYPE] == BRANCH) {
			if (lval[LTRUE])
				genequ(lval[LTRUE], lbl1);
			else
				lval[LTRUE] = lbl1;
			gencode_L(TOK_JZ + TOK_JNZ - lval[LVALUE], lval[LREG], lval[LTRUE]);
			if (lval[LFALSE])
				genlabel(lval[LFALSE]);
		} else {
			loadlval(lval, -1);
			// Change lval to "BRANCH"
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_JNZ;
			// keep `LREG`
			lval[LTRUE] = lbl1;
			lval[LFALSE] = 0;
			gencode_L(lval[LVALUE], lval[LREG], lval[LTRUE]);
		}
		freelval(lval);
	} else if (amatch("for")) {
		// @date 2020-05-19 12:37:25
		//     preamble
		// L1: compare
		//     bcc T
		// L2: increment
		//     jmp L1
		// T:  statement
		//     jmp L2
		// F:
		lbl1 = ++nxtlabel;
		lbl2 = ++nxtlabel;
		needtoken("(");
		blanks();
		if (ch != ';') {
			expression(lval);
			freelval(lval);
		}
		needtoken(";");
		genlabel(lbl1);
		blanks();
		if (ch != ';') {
			expression(lval);
			if (lval[LTYPE] == BRANCH) {
				if (!lval[LFALSE])
					lval[LFALSE] = ++nxtlabel;
				if (!lval[LTRUE])
					lval[LTRUE] = ++nxtlabel;
				gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
			} else {
				loadlval(lval, -1);
				// Change lval to "BRANCH"
				lval[LTYPE] = BRANCH;
				lval[LVALUE] = TOK_JZ;
				// keep `LREG`
				lval[LTRUE] = ++nxtlabel;
				lval[LFALSE] = ++nxtlabel;
				gencode_L(lval[LVALUE], lval[LREG], lval[LFALSE]);
			}
			freelval(lval);
		}
		gencode_L(TOK_JZ, 0, lval[LTRUE]);
		needtoken(";");
		genlabel(lbl2);
		blanks();
		if (ch != ')') {
			expression(lval);
			freelval(lval);
		}
		gencode_L(TOK_JZ, 0, lbl1);
		needtoken(")");
		genlabel(lval[LTRUE]);
		statement(swbase, returnlbl, lval[LFALSE], lbl1, csp, csp);
		gencode_L(TOK_JZ, 0, lbl2);
		genlabel(lval[LFALSE]);
	} else if (amatch("switch")) {
		needtoken("(");
		expression(lval);
		needtoken(")");
		loadlval(lval, REG_RETURN);
		lbl1 = ++nxtlabel;
		lbl2 = ++nxtlabel;
		gencode_L(TOK_JZ, 0, lbl1);
		sav_sw = swinx;
		if (swinx >= SWMAX)
			fatal("switch table overflow");
		sw[swinx++ * SLAST + SLABEL] = 0; // enable default
		statement(sav_sw, returnlbl, lbl2, contlbl, csp, contsp);
		gencode_L(TOK_JZ, 0, lbl2);
		dumpsw(sav_sw, lbl1, lbl2);
		genlabel(lbl2);
		swinx = sav_sw;
	} else if (amatch("case")) {
		if (!swbase)
			error("not in switch");
		if (!constexpr(&lbl3))
			expected("case value");
		needtoken(":");
		for (i = swbase + 1; i < swinx; ++i)
			if (sw[i * SLAST + SCASE] == lbl3)
				error("case value already defined");
		lbl1 = ++nxtlabel;
		genlabel(lbl1);
		if (swinx >= SWMAX)
			fatal("switch table overflow");
		sym = &sw[swinx++ * SLAST];
		sym[SCASE] = lbl3;
		sym[SLABEL] = lbl1;
	} else if (amatch("default")) {
		if (!swbase)
			error("not in switch");
		needtoken(":");
		sym = &sw[swbase * SLAST];
		if (sym[SLABEL])
			error("multiple defaults");
		lbl1 = ++nxtlabel;
		genlabel(lbl1);
		sym[SLABEL] = lbl1;
	} else if (amatch("return")) {
		if (!match(";")) {
			// generate a return value in R1
			expression(lval);
			loadlval(lval, REG_RETURN);
			semicolon();
		}
		// release used stack
		if (csp != 0)
			gencode(TOK_SUB, 0, REG_SP, 0, csp, 0);
		gencode_L(TOK_JZ, 0, returnlbl);
	} else if (amatch("break")) {
		if (!breaklbl)
			error("not in block");
		// release used stack for this block
		if (csp != breaksp)
			gencode(TOK_SUB, 0, REG_SP, 0, csp - breaksp, 0);
		gencode_L(TOK_JZ, 0, breaklbl);
		semicolon();
	} else if (amatch("continue")) {
		if (!contlbl)
			error("not in block");
		// release used stack for this block
		if (csp != contsp)
			gencode(TOK_SUB, 0, REG_SP, 0, csp - contsp, 0);
		gencode_L(TOK_JZ, 0, contlbl);
		semicolon();
	} else if (!ch) {
		return; // EOF
	} else if (ch != ';') {
		int r;
		r = reguse;
		expression(lval);
		freelval(lval);
		if (r != reguse)
			warning("register leak");
		semicolon();
	} else
		semicolon();
}

static int classnames[8];
static int typenames[7];

/*
 *
 */
dump_ident(int ident[]) {
	if (!debug)
		return;

	if (!classnames[0]) {
		classnames[0] = "0";
		classnames[1] = "EXTERNAL";
		classnames[2] = "STATIC";
		classnames[3] = "GLOBAL";
		classnames[4] = "SP_AUTO";
		classnames[5] = "AP_AUTO";
		classnames[6] = "REGISTER";
		typenames[0] = "0";
		typenames[1] = "ADDRESS";
		typenames[2] = "MEMORY";
		typenames[3] = "FUNCTION";
		typenames[4] = "BRANCH";
	}

	fputs("; IDENT=", outhdl);
	symname(ident[ISYM]);
	fputs(" CLASS=", outhdl);
	fputs(classnames[ident[ICLASS]], outhdl);
	fputs(" TYPE=", outhdl);
	fputs(typenames[ident[ITYPE]], outhdl);
	fputs(" PTR=", outhdl);
	fputd(ident[IPTR], outhdl);
	fputs(" SIZE=", outhdl);
	fputd(ident[ISIZE], outhdl);
	fputs(" NAME=", outhdl);
	symname(ident[INAME]);
	fputs(" VALUE=", outhdl);
	fputd(ident[IVALUE], outhdl);
	fputs(" REG=", outhdl);
	fputd(ident[IREG], outhdl);
	fputc('\n', outhdl);
}


/*
 *
 */
declenum(int scope) {
	int *sym;
	int seqnr;
	register int i;
	int len, sname;
	int lval[LLAST];

	needtoken("{");

	// get values
	seqnr = 0;
	while (1) {
		blanks();

		if (ch == '}')
			break;

		if (!(len = dohash(lptr, &sname))) {
			illname();
			semicolon();
			return;
		}
		bump(len);

		// add symbol to symboltable
		for (i = scope; i < symidx; ++i) {
			sym = &syms[i * ILAST];
			if (sym[ISYM] == sname) {
				multidef();
				break;
			}
		}

		if (symidx >= SYMMAX)
			fatal("identifier table overflow");
		sym = &syms[symidx++ * ILAST];
		sym[ISYM] = sname;
		sym[ICLASS] = EXTERNAL; // external has no storage
		sym[ITYPE] = ADDRESS;
		sym[IPTR] = 0;
		sym[ISIZE] = 0;
		sym[INAME] = 0;
		sym[IVALUE] = 0;
		sym[IREG] = 0;

		if (match("=")) {
			if (!constexpr(&seqnr))
				expected("constant expected");
		}

		sym[IVALUE] = seqnr++;
		dump_ident(sym);

		if (!match(","))
			break;
	}
	needtoken("}");
	semicolon();
}

/*
 * General global definitions
 */
declvar(int scope, register int clas) {
	int size, sname, len, ptr, type, cnt;
	register int *sym, i;

	// get size of type
	if (amatch("char"))
		size = 1;
	else if (amatch("int"))
		size = 2;
	else
		return 0;

	// scan definitions
	while (1) {
		blanks();

		if (match("*"))
			ptr = 1;
		else
			ptr = 0;
		blanks();

		if (!(len = dohash(lptr, &sname)))
			illname();
		if (len)
			bump(len);

		for (i = scope; i < symidx; ++i) {
			if (syms[i * ILAST + ISYM] == sname) {
				multidef();
				break;
			}
		}

		type = MEMORY;

		cnt = 1; // Number of elements
		if (match("[")) {
			if (clas == REGISTER)
				error("register array not supported");

			type = ADDRESS;
			// add extra indirection to endtype
			++ptr;

			// get number of elements
			if (!constexpr(&cnt))
				cnt = 0;
			else if (cnt < 0)
				warning("warning: negative size");

			// force single dimension
			needtoken("]");
		}

		// add symbol to symboltable
		if (symidx >= SYMMAX)
			fatal("identifier table overflow");
		sym = &syms[symidx++ * ILAST];
		sym[ISYM] = sname;
		sym[ICLASS] = clas;
		sym[ITYPE] = type;
		sym[IPTR] = ptr;
		sym[ISIZE] = size;
		sym[INAME] = sname;
		sym[IVALUE] = 0;
		sym[IREG] = 0;

		// Now generate code
		if (sym[ICLASS] == REGISTER) {
			sym[ITYPE] = ADDRESS;
			sym[INAME] = 0;
			sym[IVALUE] = 0;
			sym[IREG] = allocreg();
			reglock |= (1 << sym[IREG]);
		} else if (sym[ICLASS] == SP_AUTO) {

			sym[INAME] = 0;
			sym[IVALUE] = csp;
			sym[IREG] = REG_SP;

			if (type == ADDRESS) {
				if (ptr <= 1 && size == 1)
					csp += cnt * 1;
				else
					csp += cnt * BPW;
			} else {
				if (!ptr && size == 1)
					csp += cnt * 1;
				else
					csp += cnt * BPW;
			}

		} else if (sym[ICLASS] != EXTERNAL) {
			toseg(UDEFSEG);
			fputc('_', outhdl);
			symname(sname);
			fputc(':', outhdl);

			if (clas != STATIC)
				fputc(':', outhdl);

			if (type == ADDRESS) {
				if (ptr <= 1 && size == 1)
					fputs("\t.dsb\t", outhdl);
				else
					fputs("\t.dsw\t", outhdl);
				fputd(cnt, outhdl);
				fputc('\n', outhdl);
			} else {
				if (!ptr && size == 1)
					fputs("\t.dsb\t1\n", outhdl);
				else
					fputs("\t.dsw\t1\n", outhdl);
			}
		}
		dump_ident(sym);

		// test for more
		if (!match(","))
			break;
	}

	// done
	semicolon();
	return 1;
}

/*
 * Declare/define a procedure argument
 */
declarg(int scope, register int clas, register int argnr) {
	int size, sname, len, ptr, type, reg;
	register int *sym, i;

	// get size of type
	if (amatch("char"))
		size = 1; // char is stored as int on stack
	else if (amatch("int"))
		size = 2;
	else
		return 0;

	// scan definitions
	while (1) {
		blanks();

		if (match("*"))
			ptr = 1;
		else
			ptr = 0;
		blanks();

		if (!(len = dohash(lptr, &sname)))
			illname();
		if (len)
			bump(len);

		for (i = scope; i < symidx; ++i) {
			sym = &syms[i * ILAST];
			if (sym[ISYM] == sname) {
				multidef();
				break;
			}
		}

		type = MEMORY;

		if (match("[")) {
			if (ptr)
				error("array of pointers not supported");
			if (type != MEMORY)
				error("array type not supported");

			ptr = 1; // address of array (passed as argument) is pushed on stack

			// get number of elements
			int cnt;
			if (constexpr(&cnt))
				error("arraysize not allowed");

			// force single dimension
			needtoken("]");
		}

		// add symbol to symboltable
		if (symidx >= SYMMAX)
			fatal("identifier table overflow");
		sym = &syms[symidx++ * ILAST];
		sym[ISYM] = sname;
		sym[ICLASS] = clas;
		sym[ITYPE] = type;
		sym[IPTR] = ptr;
		sym[ISIZE] = size;
		sym[INAME] = 0;
		sym[IVALUE] = argnr;
		sym[IREG] = REG_AP;

		dump_ident(sym);

		// only one
		break;
	}
}

/*
 *
 */
declfunc(int clas) {
	int returnlbl, len, sname, inireg, sav_argc, scope, pshrequ;
	register int *sym, i, numarg;

	returnlbl = ++nxtlabel;
	reguse = regsum = 1 << REG_AP; // reset all registers
	reglock = regresvd | 1 << REG_AP; // locked registers include reserved registers

	csp = 0; // reset stack
	swinx = 1;
	toseg(CODESEG);
	if (verbose) {
		fputs(lptr, outhdl);
		fputc('\n', outhdl);
	}

	// get procedure name
	if (!(len = dohash(lptr, &sname))) {
		fatal("illegal function name or declaration");
		return;
	}
	bump(len);

	scope = symidx;

	for (i = 0; i < symidx; ++i) {
		sym = &syms[i * ILAST];
		if (sym[ISYM] == sname)
			break;
	}

	if (symidx >= SYMMAX)
		fatal("identifier table overflow");
	if (i >= symidx)
		sym = &syms[symidx++ * ILAST];

	// (re)define function
	sym[ISYM] = sname;
	sym[ICLASS] = EXTERNAL;
	sym[ITYPE] = FUNCTION;
	sym[IPTR] = 0;
	sym[ISIZE] = 0;
	sym[INAME] = sname;
	sym[IVALUE] = 0;
	sym[IREG] = 0;
	dump_ident(sym);

	// Generate global label
	fputc('_', outhdl);
	symname(sname);
	fputs("::", outhdl);
	gencode(TOK_LD, 0, REG_RETURN, 0, 0, REG_SP);
	pshrequ = ++nxtlabel;
	gencode(TOK_PSHR, 0, REG_SP, -pshrequ, 0, 0);
	gencode(TOK_LD, 0, REG_AP, 0, 0, REG_RETURN);

	// get parameters
	needtoken("(");
	blanks();

	// now define arguments
	numarg = 0;
	while (ch != ')') {
		if (amatch("register"))
			declarg(scope, REGISTER, numarg++);
		else
			declarg(scope, AP_AUTO, numarg++);

		if (!match(","))
			break;
	}
	needtoken(")");

	if (match(";")) {
		// declaration only
		symidx = scope;
		return;
	}

	if (sym[ICLASS] != EXTERNAL)
		multidef();
	sym[ICLASS] = clas;

	// post-process parameters. syms[scope] is function name
	for (i = scope + 1; i < symidx; ++i) {
		sym = &syms[i * ILAST];

		// tweak AP offsets (
		sym[IVALUE] = (sym[IVALUE] - numarg - 1) * BPW;

		// generate code for register parameters
		if (sym[ICLASS] == REGISTER) {
			int reg;
			reg = allocreg();
			reglock |= (1 << reg);
			gencode(TOK_LD, (sym[ISIZE] == BPW || sym[IPTR]) ? BPW : 1, reg, sym[INAME], sym[IVALUE], sym[IREG]);
			sym[ITYPE] = ADDRESS;
			sym[INAME] = 0;
			sym[IVALUE] = 0;
			sym[IREG] = reg;
			dump_ident(sym);
		}
	}

	// get statement
	inireg = reglock;
	statement(swinx, returnlbl, 0, 0, csp, csp);
	if (csp != 0)
		error("internal error. stack not released");
	if (inireg != reglock)
		error("internal error. registers not unlocked");

	// trailing statements
	genlabel(returnlbl);
	fputc('_', outhdl);
	fputd(pshrequ, outhdl);
	fputc('=', outhdl);
	fputd(regsum, outhdl); // NOTE: immediate, not label
	fputc('\n', outhdl);
	gencode(TOK_POPR, 0, REG_SP, 0, regsum, 0);
	gencode(TOK_RSB, 0, REG_SP, 0, 0, 0);

	symidx = scope;
}

/*
 * process all input text
 *
 * At this level, only static declarations,
 *      defines, includes and function
 *      definitions are legal...
 */
parse() {
	blanks();
	while (inphdl) {
		if (amatch("enum"))
			declenum(0);
		else {
			int clas;
			if (amatch("extern"))
				clas = EXTERNAL;
			else if (amatch("static"))
				clas = STATIC;
			else
				clas = GLOBAL;

			if (amatch("register")) {
				reglock = 0;
				if (declvar(0, REGISTER))
					regresvd |= reglock; // consider global registers are reserved
			} else if (declvar(0, clas)) {
				;
			} else {
				declfunc(clas);
			}
		}
		blanks();
	}
}

/*
 * Initialize all variables
 */
initialize() {
	register int i;

	verbose = maklis = 0;
	outhdl = lishdl = inphdl = 0;
	nxtlabel = 0;
	currseg = 0;
	inpfn[0] = 0;
	inplnr = 0;
	regresvd = ((1 << REG_SP) | (1 << REG_AP) | (1 << REG_RETURN) | (1 << 0));
	debug = 0;

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
	ctype[' '] = CISSPACE;
	ctype['\f'] = CISSPACE;
	ctype['\n'] = CISSPACE;
	ctype['\r'] = CISSPACE;
	ctype['\t'] = CISSPACE;
	ctype['\v'] = CISSPACE;

	// reset table
	for (i = 0; i < NAMEMAX; ++i)
		namech[i] = nametab[i] = 0;
	for (i = 0; i < SYMMAX; ++i)
		syms[i * ILAST + ISYM] = 0;
	for (i = 0; i < SWMAX; ++i)
		sw[i * SLAST + SLABEL] = 0;

	// reserve first entry so it terminates lists
	namech[0] = '?';
}

/*
 * Process commandline
 */
usage() {
	fputs("X-C-Compiler, Version ", stdout);
	fputs(getversion(), stdout);
	fputs("\n\n", stdout);

	fputs("usage: xcc <file>[.<ext>]\n", stdout);
	fputs("  -h\t\t\tInclude high-level source\n", stdout);
	fputs("  -S <file>[.<ext>]]\tAssembler output\n", stdout);
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
			fext(inpfn, arg, ".c", 0);
			if (!outfn[0])
				fext(outfn, arg, ".xs", 1);
		} else {
			// Process option
			++arg;
			switch (*arg++) {
			case 'S':
				if (!*arg && *argv)
					arg = *argv++;
				if (!*arg || *arg == '-')
					usage();
				else
					fext(outfn, arg, ".xs", 0);
				break;
			case 'd':
				debug = 1;
				break;
			case 'h':
				maklis = 1;
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
	register int i, j;

	initialize(); // initialize all variables
	startup(argv); // Process commandline options

	inphdl = open_file(inpfn, "r");
	outhdl = open_file(outfn, "w");

	ch = nch = 0;
	gch(); // load firsty character
	blanks(); // skip initial comments

	toseg(CODESEG); // setup initial segment
	parse(); // GO !!!

	fputs("\t.end\n", outhdl);

	j = 0;
	for (i = 0; i < NAMEMAX; ++i) if (namech[i]) ++j;
	fputs("; Names        : ", outhdl);
	fputd(j, outhdl);
	fputc('/', outhdl);
	fputd(NAMEMAX, outhdl);
	fputc('\n', outhdl);

	fputs("; Identifiers  : ", outhdl);
	fputd(i, outhdl);
	fputc('/', outhdl);
	fputd(SYMMAX, outhdl);
	fputc('\n', outhdl);

	fputs("; Local labels : ", outhdl);
	fputd(nxtlabel, outhdl);
	fputc('\n', outhdl);

	for (i = 1; (i < SWMAX) && sw[i * SLAST + SLABEL]; ++i);
	fputs("; Switch cases : ", outhdl);
	fputd(i - 1, outhdl);
	fputc('/', outhdl);
	fputd(SWMAX, outhdl);
	fputc('\n', outhdl);

	return errflag;
}
