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
	NAMEMAX = 1511,		// Size of nametable !!! MUST BE PRIME !!!
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
	LLAST,
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
	ILAST,
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
	REG_BPW = 13,
	REG_1 = 12,
	REG_0 = 0,
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
	TOK_ILLEGAL = 1,
	TOK_ADD = 2,
	TOK_SUB = 3,
	TOK_MUL = 4,
	TOK_DIV = 5,
	TOK_MOD = 6,
	TOK_OR = 7,
	TOK_XOR = 8,
	TOK_AND = 9,
	TOK_LSR = 10,
	TOK_LSL = 11,
	TOK_NEG = 12,
	TOK_NOT = 13,
	TOK_BEQ = 14,
	TOK_BNE = 15,
	TOK_BLT = 16,
	TOK_BLE = 17,
	TOK_BGT = 18,
	TOK_BGE = 19,
	TOK_LDB = 20,
	TOK_LDW = 21,
	TOK_LDR = 22,
	TOK_LDA = 23,
	TOK_CMP = 24,
	TOK_TST = 25,
	TOK_STB = 26,
	TOK_STW = 27,
	TOK_JMP = 28,
	TOK_JSB = 29,
	TOK_RSB = 30,
	TOK_PSHR = 31,
	TOK_POPR = 32,
	TOK_PSHB = 33,
	TOK_PSHW = 34,
	TOK_PSHA = 35,
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

int	argcid;			// hash value of reserved word
int	argvid;			// hash value of reserved word
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

//exit(int code);
//expression(int lval[]);
//fclose(int hdl);
//fgets(char *str, int siz, int hdl);
//fopen(char *name, char *mode);
//fprintf(int *, char *, ...);
//fputc(int ch, int hdl);
//getversion();
//preprocess();
//printf(char *, ...);
//strcpy(char *dst, char *src);
//strlen(char *str);
//strncpy(char *dst, int n, char *src);
//toupper(int ch);

/*
 * Generate error messages
 */
warning(char *msg) {
	// Display original line
	printf("%d: %s\n%%%s\n", inplnr, sbuf, msg);
	fprintf(outhdl, ";%% %s\n", msg);
}

expected(char *lit) {
	// Display original line
	printf("%d: %s\n%% %s expected\n", inplnr, sbuf, lit);
	fprintf(outhdl, ";%% %s expected\n", lit);
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
				fprintf(outhdl, "; %d %s", inplnr, sbuf);
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
	fprintf(outhdl, "%c", namech[tab]);
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
	printf("fopen(%s,%s) failed\n", fn, mode);
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
		fprintf(outhdl, "\t.code\n");
	else if (newseg == DATASEG)
		fprintf(outhdl, "\t.data\n");
	else if (newseg == TEXTSEG)
		fprintf(outhdl, "\t.text\n");
	else if (newseg == UDEFSEG)
		fprintf(outhdl, "\t.udef\n");
	currseg = newseg;
}

/*
 * Generate a assembler statement
 */
genopc(int opc) {
	switch (opc) {
	case TOK_ILLEGAL: fprintf(outhdl, "\tillegal\t"); break;
	case TOK_ADD : fprintf(outhdl, "\tadd\t"); break;
	case TOK_SUB : fprintf(outhdl, "\tsub\t"); break;
	case TOK_MUL : fprintf(outhdl, "\tmul\t"); break;
	case TOK_DIV : fprintf(outhdl, "\tdiv\t"); break;
	case TOK_MOD : fprintf(outhdl, "\tmod\t"); break;
	case TOK_OR  : fprintf(outhdl, "\tor\t"); break;
	case TOK_XOR : fprintf(outhdl, "\txor\t"); break;
	case TOK_AND : fprintf(outhdl, "\tand\t"); break;
	case TOK_LSR : fprintf(outhdl, "\tlsr\t"); break;
	case TOK_LSL : fprintf(outhdl, "\tlsl\t"); break;
	case TOK_NEG : fprintf(outhdl, "\tneg\t"); break;
	case TOK_NOT : fprintf(outhdl, "\tnot\t"); break;
	case TOK_BEQ  : fprintf(outhdl, "\tbeq\t"); break;
	case TOK_BNE  : fprintf(outhdl, "\tbne\t"); break;
	case TOK_BLT  : fprintf(outhdl, "\tblt\t"); break;
	case TOK_BLE  : fprintf(outhdl, "\tble\t"); break;
	case TOK_BGT  : fprintf(outhdl, "\tbgt\t"); break;
	case TOK_BGE  : fprintf(outhdl, "\tbge\t"); break;
	case TOK_LDB: fprintf(outhdl, "\tldb\t"); break;
	case TOK_LDW: fprintf(outhdl, "\tldw\t"); break;
	case TOK_LDR: fprintf(outhdl, "\tldr\t"); break;
	case TOK_LDA : fprintf(outhdl, "\tlda\t"); break;
	case TOK_CMP : fprintf(outhdl, "\tcmp\t"); break;
	case TOK_TST : fprintf(outhdl, "\ttst\t"); break;
	case TOK_STB: fprintf(outhdl, "\tstb\t"); break;
	case TOK_STW: fprintf(outhdl, "\tstw\t"); break;
	case TOK_JMP : fprintf(outhdl, "\tjmp\t"); break;
	case TOK_JSB : fprintf(outhdl, "\tjsb\t"); break;
	case TOK_RSB : fprintf(outhdl, "\trsb\t"); break;
	case TOK_PSHB: fprintf(outhdl, "\tpshb\t"); break;
	case TOK_PSHW: fprintf(outhdl, "\tpshw\t"); break;
	case TOK_PSHA: fprintf(outhdl, "\tpsha\t"); break;
	case TOK_PSHR: fprintf(outhdl, "\tpshr\t"); break;
	case TOK_POPR: fprintf(outhdl, "\tpopr\t"); break;
	}
}

gencode(int opc) {
	genopc(opc);

	fprintf(outhdl, "\n");
}

gencode_L(int opc, int lbl) {
	genopc(opc);

	fprintf(outhdl, "_%d\n", lbl);
}

gencode_R(int opc, int lreg, int rreg) {
	// sign extend
	lreg |= -(lreg & (1 << SBIT));

	genopc(opc);

	if (lreg >= 0)
		fprintf(outhdl, "r%d,", lreg);
	fprintf(outhdl, "r%d\n", rreg);
}

gencode_I(int opc, int lreg, int imm) {
	// sign extend
	lreg |= -(lreg & (1 << SBIT));
	imm |= -(imm & (1 << SBIT));

	genopc(opc);

	if (lreg >= 0)
		fprintf(outhdl, "r%d,", lreg);
	fprintf(outhdl, "%d\n", imm);
}

gencode_ADJSP(int imm) {
	// sign extend
	imm |= -(imm & (1 << SBIT));

	if (imm == BPW)
		fprintf(outhdl, "\tadd\tr%d,r%d\n", REG_SP, REG_BPW);
	else if (imm == -BPW)
		fprintf(outhdl, "\tsub\tr%d,r%d\n", REG_SP, REG_BPW);
	else
		fprintf(outhdl, "\tlda\tr%d,%d(r%d)\n", REG_SP, imm, REG_SP);
}

gencode_M(int opc, int lreg, int name, int ofs, int rreg) {
	// sign extend
	lreg |= -(lreg & (1 << SBIT));
	name |= -(name & (1 << SBIT));
	ofs |= -(ofs & (1 << SBIT));

	genopc(opc);

	if (lreg >= 0)
		fprintf(outhdl, "r%d,", lreg);

	if (name) {
		if (name > 0){
			fprintf(outhdl, "_");
			symname(name);
		} else
			fprintf(outhdl, "_%d", -name);
	}

	if (ofs > 0)
		fprintf(outhdl, "+%d", ofs);
	else if (ofs < 0)
		fprintf(outhdl, "%d", ofs);
	if (rreg)
		fprintf(outhdl, "(r%d)", rreg);

	fprintf(outhdl, "\n");
}

gencode_lval(int opc, int lreg, int lval[]) {
	int name, ofs, rreg;
	name = lval[LNAME];
	ofs = lval[LVALUE];
	rreg = lval[LREG];

	// apply any stack adjustments for SP_AUTO
	if (rreg == REG_SP)
		ofs = ofs - csp;

	gencode_M(opc, lreg, name, ofs, rreg);
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
	if (isConstant(lval) || lval[LTYPE] == BRANCH)
		return;
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

	// Sign extend to fix being called with negative constant when copiled with "-Dint=long"
	reg |= -(reg & (1 << SBIT));

	if (lval[LTYPE] == ADDRESS) {
		// try to relocate offset to register
		if (lval[LREG] == 0) {
			if (lval[LVALUE] == 1) {
				lval[LREG] = REG_1;
				lval[LVALUE] = 0;
			} else if (lval[LVALUE] == BPW) {
				lval[LREG] = REG_BPW;
				lval[LVALUE] = 0;
			}
		}

		// assign resulting register
		if (reg > 0) {
			// loaded into fixed register
			freelval(lval);
		} else if (lval[LNAME] || lval[LVALUE]) {
			// need code to evaluate, result always in new register
			freelval(lval);
			reg = allocreg();
		} else if (reg == 0 && ((reglock | regresvd) & (1 << lval[LREG]))) {
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
		if (lval[LNAME] || lval[LVALUE]) {
			gencode_lval(TOK_LDA, reg, lval);
		} else if (lval[LREG] == 0) {
			gencode_R(TOK_LDR, reg, REG_0);
		} else {
			gencode_R(TOK_LDR, reg, lval[LREG]);
		}

		// Modify lval
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else if (lval[LTYPE] == MEMORY) {
		freelval(lval);
		if (reg <= 0)
			reg = allocreg();
		gencode_lval(isWORD(lval) ? TOK_LDW : TOK_LDB, reg, lval);

		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else if (lval[LTYPE] == BRANCH) {
		int lblX;
		lblX = ++nxtlabel;

		if (reg <= 0)
			reg = allocreg();
		if (!lval[LFALSE])
			lval[LFALSE] = ++nxtlabel;
		gencode_L(lval[LVALUE], lval[LFALSE]);
		if (lval[LTRUE])
			fprintf(outhdl, "_%d:", lval[LTRUE]);
		gencode_R(TOK_LDR, reg, REG_1);
		gencode_L(TOK_JMP, lblX);
		fprintf(outhdl, "_%d:", lval[LFALSE]);
		gencode_R(TOK_LDR, reg, REG_0);
		fprintf(outhdl, "_%d:", lblX);

		lval[LTYPE] = ADDRESS;
		lval[LPTR] = 0;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = reg;
	} else
		error("unimplemented");
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
	fprintf(outhdl, "_%d:\t.dcb\t\"", lbl);

	while (ch && (ch != '"')) {
		if (ch == '\\') {
			fputc(ch, outhdl);
			gch();
		}
		fputc(ch, outhdl);
		gch();
	}
	fprintf(outhdl, "\",0\n");
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

			return 1;
		}
	}

	// test for reserved words
	if (sname == argcid) {
		// generate (2(AP)-BPW)/BPW
		lval[LTYPE] = ADDRESS;
		lval[LPTR] = 0;
		lval[LSIZE] = BPW;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = allocreg();

		gencode_M(TOK_LDW, lval[LREG], 0, BPW, REG_AP);
		gencode_R(TOK_SUB, lval[LREG], REG_BPW);
		gencode_R(TOK_DIV, lval[LREG], REG_BPW);
		return 1;
	} else if (sname == argvid) {
		exprerr();
		return 0;
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
 * Get the inverse of an compare
 */
negop(register int op) {
	switch (op) {
	case TOK_BEQ: return TOK_BNE;
	case TOK_BNE: return TOK_BEQ;
	case TOK_BGT: return TOK_BLE;
	case TOK_BLT: return TOK_BGE;
	case TOK_BGE: return TOK_BLT;
	case TOK_BLE: return TOK_BGT;
	default: return op; // No negation
	}
}

/*
 * Process a constant evaluation
 */
calc(register int left, int oper, int right) {
	switch (oper) {
	case TOK_OR : return (left | right);
	case TOK_XOR: return (left ^ right);
	case TOK_AND: return (left & right);
	case TOK_BEQ : return (left == right);
	case TOK_BNE : return (left != right);
	case TOK_BLE : return (left <= right);
	case TOK_BGE : return (left >= right);
	case TOK_BLT : return (left < right);
	case TOK_BGT : return (left > right);
	case TOK_LSR: return (left >> right);
	case TOK_LSL: return (left << right);
	case TOK_ADD: return (left + right);
	case TOK_SUB: return (left - right);
	case TOK_MUL: return (left * right);
	case TOK_DIV: return (left / right);
	case TOK_MOD: return (left % right);
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
		loadlval(rval, -1);

		// Execute operation and release rval
		gencode_R(tok, lval[LREG], rval[LREG]);
		freelval(rval);
	}
}

/*
 * Auto increment/decrement
 */
step(register int pre, register int lval[], register int post) {
	int dest[LLAST];
	register int reg;

	if (!isRegister(lval) && lval[LTYPE] != MEMORY)
		expected("lvalue");

	// Copy lval
	dest[LTYPE] = lval[LTYPE];
	dest[LPTR] = lval[LPTR];
	dest[LSIZE] = lval[LSIZE];
	dest[LNAME] = lval[LNAME];
	dest[LVALUE] = lval[LVALUE];
	dest[LREG] = lval[LREG];

	if (isRegister(lval)) {
		gencode_R((pre | post), lval[LREG], isINTPTR(lval) ? REG_BPW : REG_1);
		if (post) {
			reg = allocreg();
			gencode_R(TOK_LDR, reg, lval[LREG]);
			gencode_R((TOK_ADD + TOK_SUB - post), reg, isINTPTR(lval) ? REG_BPW : REG_1);
			freelval(lval);
			lval[LREG] = reg;
		}
	} else {
		reg = allocreg();
		loadlval(lval, reg);
		gencode_R((pre | post), lval[LREG], isINTPTR(lval) ? REG_BPW : REG_1);
		gencode_lval(isWORD(dest) ? TOK_STW : TOK_STB, lval[LREG], dest);
		if (post) {
			gencode_R((TOK_ADD + TOK_SUB - post), reg, isINTPTR(lval) ? REG_BPW : REG_1);
			lval[LREG] = reg;
		}
	}
}

/*
 *
 */
expr_postfix(register int lval[]) {
	int lval2[LLAST], sav_csp;
	register int argc, reg;

	if (!primary(lval))
		return 0;
	if (match("[")) { // [subscript]
		if (!lval[LPTR])
			error("can't subscript");
		else if (!expression(lval2))
			error("need subscript");
		else {
			if (isConstant(lval2)) {
				if (lval[LTYPE] == MEMORY)
					loadlval(lval, 0); // load if pointer
				// Subscript is a constant
				lval[LVALUE] += lval2[LVALUE] * lval[LSIZE];
			} else {
				// Subscript is a variable/complex-expression
				if (lval[LTYPE] == MEMORY)
					loadlval(lval, 0); // load if pointer
				loadlval(lval2, 0);
				if (lval[LSIZE] == BPW)
					gencode_R(TOK_MUL, lval2[LREG], REG_BPW); // size index
				if (!lval[LREG])
					lval[LREG] = lval2[LREG];
				else {
					/*
					 * @date 2020-05-23 01:14:18
					 * This is an important location.
					 * having a second indirect register removes the following code.
					 * however, a single register is a much simpler implementation.
					 */
					if ((1<<lval[LREG]) & reglock) {
						// register in lval is locked and needs to be made writable
						freelval(lval);
						reg = allocreg();
						gencode_R(TOK_LDR, reg, lval[LREG]);
						lval[LREG] = reg;
					}
					gencode_R(TOK_ADD, lval[LREG], lval2[LREG]);
					freelval(lval2);
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

		argc = BPW;
		sav_csp = csp;
		blanks();
		while (ch != ')') {
			// Get expression
			expr_assign(lval2);
			if (isConstant(lval2)) {
				gencode_I(TOK_PSHA, -1, lval2[LVALUE]);
			} else {
				if (lval2[LTYPE] == BRANCH)
					loadlval(lval2, 0);
				freelval(lval2);
				// Push onto stack
				if (lval2[LTYPE] != MEMORY)
					gencode_lval(TOK_PSHA, -1, lval2);
				else
					gencode_lval(isWORD(lval2) ? TOK_PSHW : TOK_PSHB, -1, lval2);
			}
			// increment ARGC
			csp -= BPW;
			argc += BPW;

			if (!match(","))
				break;
		}
		needtoken(")");
		// Push ARGC
		gencode_I(TOK_PSHA, -1, argc);

		// call
		gencode_lval(TOK_JSB, -1, lval);
		freelval(lval);

		// Pop args
		csp = 0; // `gencode_M` will subtract csp from sp.
		gencode_M(TOK_LDA, REG_SP, 0, argc, REG_SP);

		csp = sav_csp;

		lval[LTYPE] = ADDRESS;
		lval[LPTR] = 0;
		lval[LSIZE] = BPW;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = REG_RETURN;
	}

	if (match("++")) {
		step(0, lval, TOK_ADD);
	} else if (match("--")) {
		step(0, lval, TOK_SUB);
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
		step(TOK_ADD, lval, 0);
	} else if (match("--")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		step(TOK_SUB, lval, 0);
	} else if (match("~")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = ~lval[LVALUE];
		else {
			loadlval(lval, 0);
			gencode_R(TOK_NOT, -1, lval[LREG]);
		}
	} else if (match("!")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = !lval[LVALUE];
		else if (lval[LTYPE] == BRANCH && !lval[LTRUE]) {
			// @date 	2020-05-19 20:21:57
			// bugfix: can only negate opcode if no other opcodes were generated
			lval[LVALUE] = negop(lval[LVALUE]);
		} else {
			// convert CC bits into a BRANCH
			loadlval(lval, 0);
			freelval(lval);
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_BNE;
			lval[LTRUE] = lval[LFALSE] = 0;
		}
	} else if (match("-")) {
		if (!expr_unary(lval)) {
			exprerr();
			return 0;
		}
		if (isConstant(lval))
			lval[LVALUE] = -lval[LVALUE];
		else {
			loadlval(lval, 0);
			gencode_R(TOK_NEG, -1, lval[LREG]);
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

		gencode_expr(tok, lval, rval);
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

		gencode_expr(tok, lval, rval);
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

		if (omatch(">>"))
			tok = TOK_LSR;
		else if (omatch("<<"))
			tok = TOK_LSL;
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
expr_relational(int lval[]) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_shift(lval))
		return 0;

	if (omatch("<="))
		tok = TOK_BLE;
	else if (omatch(">="))
		tok = TOK_BGE;
	else if (omatch("<"))
		tok = TOK_BLT;
	else if (omatch(">"))
		tok = TOK_BGT;
	else
		return 1;

	// Load rval
	if (!expr_shift(rval)) {
		exprerr();
		return 1;
	}

	// Generate code
	if (isConstant(lval) && isConstant(rval)) {
		lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
	} else {
		loadlval(lval, -1);
		loadlval(rval, -1);

		// Compare and release values
		gencode_R(TOK_CMP, lval[LREG], rval[LREG]);
		freelval(lval);
		freelval(rval);

		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = negop(tok);
		lval[LTRUE] = lval[LFALSE] = 0;
	}
	return 1;
}

/*
 *
 */
expr_equality(int lval[]) {
	int tok, rval[LLAST];

	// Load lval
	if (!expr_relational(lval))
		return 0;

	if (omatch("=="))
		tok = TOK_BEQ;
	else if (omatch("!="))
		tok = TOK_BNE;
	else
		return 1;

	// Load rval
	if (!expr_relational(rval)) {
		exprerr();
		return 1;
	}

	// Generate code
	if (isConstant(lval) && isConstant(rval)) {
		lval[LVALUE] = calc(lval[LVALUE], tok, rval[LVALUE]);
	} else {
		loadlval(lval, -1);
		loadlval(rval, -1);

		// Compare and release values
		gencode_R(TOK_CMP, lval[LREG], rval[LREG]);
		freelval(lval);
		freelval(rval);

		// Change lval to "BRANCH"
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = negop(tok);
		lval[LTRUE] = lval[LFALSE] = 0;
	}
	return 1;
}

/*
 *
 */
expr_and(int lval[]) {
	// Load lval
	if (!expr_equality(lval))
		return 0;

	while (1) {
		int rval[LLAST];

		if (!omatch("&"))
			return 1;

		// Load rval
		if (!expr_equality(rval)) {
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
	register int lbl;
	int once;

	// Load lval
	if (!expr_or(lval))
		return 0;

	once = 1;
	while (1) {
		// Locate operation
		if (!omatch("&&"))
			return 1;

		if (once) {
			// One time only: process lval and jump

			// lval must be BRANCH
			if (lval[LTYPE] != BRANCH) {
				loadlval(lval, 0);
				freelval(lval);
				lval[LTYPE] = BRANCH;
				lval[LVALUE] = TOK_BEQ;
				lval[LTRUE] = lval[LFALSE] = 0;
			}

			if (!lval[LFALSE])
				lval[LFALSE] = ++nxtlabel;
			lbl = lval[LFALSE];

			// Mark done
			once = 0;
		}

		// postprocess last lval
		gencode_L(lval[LVALUE], lval[LFALSE]);
		if (lval[LTRUE])
			fprintf(outhdl, "_%d:", lval[LTRUE]);

		// Load next lval
		if (!expr_or(lval)) {
			exprerr();
			return 1;
		}

		// lval must be BRANCH
		if (lval[LTYPE] != BRANCH) {
			loadlval(lval, 0);
			freelval(lval);
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_BEQ;
			lval[LTRUE] = lval[LFALSE] = 0;
		}

		if (lval[LFALSE])
			fprintf(outhdl, "_%d=_%d\n", lval[LFALSE], lbl);
		lval[LFALSE] = lbl;
	}
}

/*
 *
 */
expr_lor(int lval[]) {
	register int lbl;
	int once;

	// Load lval
	if (!expr_land(lval))
		return 0;

	once = 1;
	while (1) {
		// Locate operation
		if (!omatch("||"))
			return 1;

		if (once) {
			// One time only: process lval and jump

			// lval must be BRANCH
			if (lval[LTYPE] != BRANCH) {
				loadlval(lval, 0);
				freelval(lval);
				lval[LTYPE] = BRANCH;
				lval[LVALUE] = TOK_BEQ;
				lval[LTRUE] = lval[LFALSE] = 0;
			}

			if (!lval[LTRUE])
				lval[LTRUE] = ++nxtlabel;
			lbl = lval[LTRUE];

			// Mark done
			once = 0;
		}

		// postprocess last lval
		gencode_L(negop(lval[LVALUE]), lval[LTRUE]);
		if (lval[LFALSE])
			fprintf(outhdl, "_%d:", lval[LFALSE]);

		// Load next lval
		if (!expr_land(lval)) {
			exprerr();
			return 1;
		}

		// lval must be BRANCH
		if (lval[LTYPE] != BRANCH) {
			loadlval(lval, 0);
			freelval(lval);
			lval[LTYPE] = BRANCH;
			lval[LVALUE] = TOK_BEQ;
			lval[LTRUE] = lval[LFALSE] = 0;
		}

		if (lval[LTRUE])
			fprintf(outhdl, "_%d=_%d\n", lval[LTRUE], lbl);
		lval[LTRUE] = lbl;
	}
}

/*
 *
 */
expr_ternary(register int lval[]) {
	register int lbl, reg;

	if (!expr_lor(lval))
		return 0;
	if (!match("?"))
		return 1;

	// If not a BRANCH then convert it into one
	if (lval[LTYPE] != BRANCH) {
		loadlval(lval, 0);
		freelval(lval);
		lval[LTYPE] = BRANCH;
		lval[LVALUE] = TOK_BEQ;
		lval[LTRUE] = lval[LFALSE] = 0;
	}
	// alloc labels (copy to variable because of `loadlval()`
	int lfalse;
	lfalse = lval[LFALSE];
	if (!lfalse)
		lfalse = ++nxtlabel;

	// process 'true' variant
	gencode_L(lval[LVALUE], lfalse);
	if (lval[LTRUE])
		fprintf(outhdl, "_%d:", lval[LTRUE]);
	expression(lval);
	loadlval(lval, reg = allocreg()); // Needed to assign a dest reg

	needtoken(":");
	// jump to end
	lbl = ++nxtlabel;
	gencode_L(TOK_JMP, lbl);

	// process 'false' variant
	fprintf(outhdl, "_%d:", lfalse);
	if (!expr_ternary(lval))
		exprerr();
	else
		loadlval(lval, reg); // Needed for result to occupy same reg

	fprintf(outhdl, "_%d:", lbl);

	// resulting type is undefined, so modify LTYPE
	lval[LTYPE] = ADDRESS;
	lval[LPTR] = 0;

	return 1;
}

/*
 * Load a numerical expression separated by comma's
 */
expr_assign(register int lval[]) {
	int rval[LLAST], dest[LLAST];
	register int oper;

	if (!expr_ternary(lval))
		return 0;

	// Test for assignment
	if (omatch("=")) oper = -1;
	else if (match("|=")) oper = TOK_OR;
	else if (match("^=")) oper = TOK_XOR;
	else if (match("&=")) oper = TOK_AND;
	else if (match("+=")) oper = TOK_ADD;
	else if (match("-=")) oper = TOK_SUB;
	else if (match("*=")) oper = TOK_MUL;
	else if (match("/=")) oper = TOK_DIV;
	else if (match("%=")) oper = TOK_MOD;
	else if (match(">>=")) oper = TOK_LSR;
	else if (match("<<=")) oper = TOK_LSL;
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
	loadlval(rval, -1);

	if (oper == -1) {
		if (isRegister(lval))
			gencode_R(TOK_LDR, lval[LREG], rval[LREG]);
		else
			gencode_lval(isWORD(lval) ? TOK_STW : TOK_STB, rval[LREG], lval);
		// continue with value stored in register `rval[LREG]`
		freelval(lval);
		lval[LTYPE] = ADDRESS;
		lval[LNAME] = 0;
		lval[LVALUE] = 0;
		lval[LREG] = rval[LREG];
	} else {
		// Copy lval
		dest[LTYPE] = lval[LTYPE];
		dest[LPTR] = lval[LPTR];
		dest[LSIZE] = lval[LSIZE];
		dest[LNAME] = lval[LNAME];
		dest[LVALUE] = lval[LVALUE];
		dest[LREG] = lval[LREG];

		// load lval into reg, modify it with rval and copy result into dest
		loadlval(lval, allocreg()); // don't reuse regs for dest
		gencode_R(oper, lval[LREG], rval[LREG]);
		freelval(rval);
		// write-back
		if (isRegister(dest))
			gencode_R(TOK_LDR, dest[LREG], lval[LREG]);
		else
			gencode_lval(isWORD(lval) ? TOK_STW : TOK_STB, lval[LREG], dest);
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
			gencode_L(TOK_JMP, ptr[SLABEL]);
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
	fprintf(outhdl, "_%d:", maplbl);
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
			fprintf(outhdl, "\t.dcw\t_%d", lbl);
		else
			fprintf(outhdl, ",_%d", lbl);
		if (cnt > 15) {
			fprintf(outhdl, "\n");
			cnt = 0;
		}
	}
	if (cnt)
		fprintf(outhdl, "\n");
	toseg(prevseg);

	// generate code (use j as reg)
	fprintf(outhdl, "_%d:", codlbl);
	j = allocreg();
	gencode_I(TOK_LDA, j, lo);
	gencode_R(TOK_SUB, REG_RETURN, j);
	gencode_L(TOK_BLT, deflbl);
	gencode_I(TOK_LDA, j, hi - lo);
	gencode_R(TOK_CMP, REG_RETURN, j);
	gencode_L(TOK_BGT, deflbl);
	gencode_R(TOK_MUL, REG_RETURN, REG_BPW);
	gencode_M(TOK_LDW, j, -maplbl, 0, REG_RETURN);
	gencode_M(TOK_JMP, -1, 0, 0, j);
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
					gencode_ADJSP(csp - last_csp);
					last_csp = csp;
				}
				statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
			}
		}
		// unwind stack
		if (csp != sav_csp) {
			gencode_ADJSP(sav_csp - csp);
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
			gencode_L(lval[LVALUE], lval[LFALSE]);
		} else {
			loadlval(lval, 0);
			lval[LFALSE] = ++nxtlabel;
			lval[LTRUE] = 0;
			gencode_L(TOK_BEQ, lval[LFALSE]);
		}
		if (lval[LTRUE])
			fprintf(outhdl, "_%d:", lval[LTRUE]);
		freelval(lval);
		statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
		if (!amatch("else")) {
			// @date 2020-05-19 12:45:53
			//     compare
			// T:  statement
			// F:
			fprintf(outhdl, "_%d:", lval[LFALSE]);
		} else {
			// @date 2020-05-19 12:45:53
			//     compare
			// T:  statement
			//     jmp L1
			// F:  statement
			// L1:
			lbl1 = ++nxtlabel;
			gencode_L(TOK_JMP, lbl1);
			fprintf(outhdl, "_%d:", lval[LFALSE]);
			statement(swbase, returnlbl, breaklbl, contlbl, breaksp, contsp);
			fprintf(outhdl, "_%d:", lbl1);
		}
	} else if (amatch("while")) {
		// @date 2020-05-19 12:39:49
		// L1: compare
		//     bcc F
		// T:  statement
		//     jmp L1
		// F:
		lbl1 = ++nxtlabel;
		fprintf(outhdl, "_%d:", lbl1);
		needtoken("(");
		expression(lval);
		needtoken(")");

		if (lval[LTYPE] == BRANCH) {
			if (!lval[LFALSE])
				lval[LFALSE] = ++nxtlabel;
			gencode_L(lval[LVALUE], lval[LFALSE]);
		} else {
			loadlval(lval, 0);
			lval[LFALSE] = ++nxtlabel;
			lval[LTRUE] = 0;
			gencode_L(TOK_BEQ, lval[LFALSE]);
		}
		if (lval[LTRUE])
			fprintf(outhdl, "_%d:", lval[LTRUE]);
		freelval(lval);
		statement(swbase, returnlbl, lval[LFALSE], lbl1, csp, csp);
		gencode_L(TOK_JMP, lbl1);
		fprintf(outhdl, "_%d:", lval[LFALSE]);
	} else if (amatch("do")) {
		// @date 2020-05-19 12:37:46
		// L1: statement
		// L2: compare
		//     bcc L1
		// F:
		lbl1 = ++nxtlabel;
		lbl2 = ++nxtlabel;
		fprintf(outhdl, "_%d:", lbl1);
		statement(swbase, returnlbl, lbl2, lbl1, csp, csp);
		fprintf(outhdl, "_%d:", lbl2);
		needtoken("while");
		needtoken("(");
		expression(lval);
		needtoken(")");
		if (lval[LTYPE] == BRANCH) {
			if (lval[LTRUE])
				fprintf(outhdl, "_%d=_%d\n", lval[LTRUE], lbl1);
			else
				lval[LTRUE] = lbl1;
			gencode_L(negop(lval[LVALUE]), lval[LTRUE]);
			if (lval[LFALSE])
				fprintf(outhdl, "_%d:", lval[LFALSE]);
		} else {
			loadlval(lval, 0);
			gencode_L(TOK_BNE, lbl1);
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
		fprintf(outhdl, "_%d:", lbl1);
		blanks();
		if (ch != ';') {
			expression(lval);
			if (lval[LTYPE] == BRANCH) {
				if (!lval[LFALSE])
					lval[LFALSE] = ++nxtlabel;
				if (!lval[LTRUE])
					lval[LTRUE] = ++nxtlabel;
				gencode_L(lval[LVALUE], lval[LFALSE]);
			} else {
				lval[LFALSE] = ++nxtlabel;
				lval[LTRUE] = ++nxtlabel;
				loadlval(lval, 0);
				gencode_L(TOK_BEQ, lval[LFALSE]);
			}
			freelval(lval);
		}
		gencode_L(TOK_JMP, lval[LTRUE]);
		needtoken(";");
		fprintf(outhdl, "_%d:", lbl2);
		blanks();
		if (ch != ')') {
			expression(lval);
			freelval(lval);
		}
		gencode_L(TOK_JMP, lbl1);
		needtoken(")");
		fprintf(outhdl, "_%d:", lval[LTRUE]);
		statement(swbase, returnlbl, lval[LFALSE], lbl1, csp, csp);
		gencode_L(TOK_JMP, lbl2);
		fprintf(outhdl, "_%d:", lval[LFALSE]);
	} else if (amatch("switch")) {
		needtoken("(");
		expression(lval);
		needtoken(")");
		loadlval(lval, REG_RETURN);
		lbl1 = ++nxtlabel;
		lbl2 = ++nxtlabel;
		gencode_L(TOK_JMP, lbl1);
		sav_sw = swinx;
		if (swinx >= SWMAX)
			fatal("switch table overflow");
		sw[swinx++ * SLAST + SLABEL] = 0; // enable default
		statement(sav_sw, returnlbl, lbl2, contlbl, csp, contsp);
		gencode_L(TOK_JMP, lbl2);
		dumpsw(sav_sw, lbl1, lbl2);
		fprintf(outhdl, "_%d:", lbl2);
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
		fprintf(outhdl, "_%d:", lbl1);
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
		fprintf(outhdl, "_%d:", lbl1);
		sym[SLABEL] = lbl1;
	} else if (amatch("return")) {
		if (!match(";")) {
			// generate a return value in R1
			expression(lval);
			loadlval(lval, REG_RETURN);
			semicolon();
		}
		if (csp != 0)
			gencode_ADJSP(- csp);
		gencode_L(TOK_JMP, returnlbl);
	} else if (amatch("break")) {
		if (!breaklbl)
			error("not in block");
		if (csp != breaksp)
			gencode_ADJSP(breaksp - csp);
		gencode_L(TOK_JMP, breaklbl);
		semicolon();
	} else if (amatch("continue")) {
		if (!contlbl)
			error("not in block");
		if (csp != contsp)
			gencode_ADJSP(contsp - csp);
		gencode_L(TOK_JMP, contlbl);
		semicolon();
	} else if (!ch) {
		return; // EOF
	} else if (ch != ';') {
		int r;
		r = reguse;
		expression(lval);
		freelval(lval);
		if (r != reguse)
			fatal("register leak");
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

	fprintf(outhdl, "; IDENT=");
	symname(ident[ISYM]);
	fprintf(outhdl, " CLASS=%s TYPE=%s PTR=%d SIZE=%d NAME=", classnames[ident[ICLASS]], typenames[ident[ITYPE]], ident[IPTR], ident[ISIZE]);
	symname(ident[INAME]);
	fprintf(outhdl, " VALUE=%d REG=%d\n", ident[IVALUE], ident[IREG]);
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

			if (type == ADDRESS) {
				if (ptr <= 1 && size == 1)
					csp -= cnt * 1;
				else
					csp -= cnt * BPW;
			} else {
				if (!ptr && size == 1)
					csp -= cnt * 1;
				else
					csp -= cnt * BPW;
			}

			sym[INAME] = 0;
			sym[IVALUE] = csp;
			sym[IREG] = REG_SP;
		} else if (sym[ICLASS] != EXTERNAL) {
			toseg(UDEFSEG);
			fprintf(outhdl, "_");
			symname(sname);
			fprintf(outhdl, ":");

			if (clas != STATIC)
				fprintf(outhdl, ":");

			if (type == ADDRESS) {
				if (ptr <= 1 && size == 1)
					fprintf(outhdl, "\t.dsb\t%d\n", cnt);
				else
					fprintf(outhdl, "\t.dsw\t%d\n", cnt);
			} else {
				if (!ptr && size == 1)
					fprintf(outhdl, "\t.dsb\t1\n");
				else
					fprintf(outhdl, "\t.dsw\t1\n");
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
		sym[IVALUE] = (-argnr + 1) * BPW;
		sym[IREG] = REG_AP;

		// modify location if chars. Chars are pushed as words, adjust to point to lo-byte
		if (size == 1 && !ptr)
			sym[IVALUE] += BPW - 1;

		dump_ident(sym);

		// only one
		break;
	}
}

/*
 *
 */
declfunc(int clas) {
	int returnlbl, len, sname, inireg, sav_argc, scope, pshrlbl;
	register int *sym, i, numarg;

	returnlbl = ++nxtlabel;
	reguse = regsum = reglock = 1 << REG_AP; // reset all registers
	csp = 0; // reset stack
	swinx = 1;
	toseg(CODESEG);
	if (verbose)
		printf("%s\n", lptr);

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
	fprintf(outhdl, "_");
	symname(sname);
	fprintf(outhdl, "::");
	gencode_R(TOK_LDR, REG_RETURN, REG_SP);
	pshrlbl = ++nxtlabel;
	gencode_M(TOK_PSHR, -1, -pshrlbl, 0, 0);
	gencode_R(TOK_LDR, REG_AP, 1);

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

		// tweak AP offsets
		sym[IVALUE] += numarg * BPW;

		// generate code for register parameters
		if (sym[ICLASS] == REGISTER) {
			int reg;
			reg = allocreg();
			reglock |= (1 << reg);
			gencode_M((sym[ISIZE] == BPW || sym[IPTR]) ? TOK_LDW : TOK_LDB, reg, sym[INAME], sym[IVALUE], sym[IREG]);
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
	fprintf(outhdl, "_%d:\t_%d=%d\n", returnlbl, pshrlbl, regsum);
	gencode_I(TOK_POPR, -1, regsum);
	gencode(TOK_RSB);

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
					regresvd |= reglock;
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
	regresvd = ((1 << REG_SP) | (1 << REG_AP) | (1 << REG_BPW) | (1 << REG_1) | (1 << REG_0) | (1 << REG_RETURN) | (1 << 0));
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

	// reserved words
	dohash("ARGC", &argcid);
	dohash("ARGV", &argvid);
}

/*
 * Process commandline
 */
usage() {
	printf("X-C-Compiler, Version %s\n\n", getversion());

	printf("usage: xcc <file>[.<ext>]\n");
	printf("  -h\t\t\tInclude high-level source\n");
	printf("  -S <file>[.<ext>]]\tAssembler output\n");
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

	fprintf(outhdl, "\t.end\n");

	j = 0;
	for (i = 0; i < NAMEMAX; ++i) if (namech[i]) ++j;
	fprintf(outhdl, "; Names        : %5d/%5d\n", j, NAMEMAX);
	for (i = 0; i < SYMMAX && syms[i * ILAST + ISYM]; ++i);
	fprintf(outhdl, "; Identifiers  : %5d/%5d\n", i, SYMMAX);
	fprintf(outhdl, "; Local labels : %5d\n", nxtlabel);
	for (i = 1; (i < SWMAX) && sw[i * SLAST + SLABEL]; ++i);
	fprintf(outhdl, "; Switch cases : %5d/%5d\n", i - 1, SWMAX);

	return errflag;
}
