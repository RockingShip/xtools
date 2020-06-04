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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "config.h"

/*
** Compiler dependent parameters
*/

#define BPW 2
#define SBIT 15
#define MAXFILE 100

/*
** Compiler-generated p-codes
*/

#define _ILLEGAL        0x00
#define _ADD            0x13
#define _SUB            0x12
#define _MUL            0x14
#define _DIV            0x15
#define _MOD            0x16
#define _BOR            0x17
#define _XOR            0x19
#define _BAND           0x18
#define _LSR            0x1B
#define _LSL            0x1A
#define _NEG            0x1D
#define _NOT            0x1C
#define _EQ             0x68
#define _NE             0x67
#define _LT             0x64
#define _LE             0x63
#define _GT             0x66
#define _GE             0x65
#define _MOVB           0x04
#define _MOVW           0x01
#define _MOVR           0x11
#define _LEA            0x03
#define _CMP            0x10
#define _STOB           0x05
#define _STOW           0x02
#define _JMP            0x6F
#define _JSB            0x20
#define _RSB            0x21
#define _PSHR           0x23
#define _POPR           0x24
#define _PSHB           0x25
#define _PSHW           0x26
#define _PSHA           0x27
#define _SVC            0x0A

/*
** Storage
*/

FILE     *handles[MAXFILE];     /* handle/FILE mapping */
uint8_t  image[0x10000];        /* actual image */
char     **inpargv;             /* argv[] for emulated */
uint16_t lowestSP;              /* Lowest encountered SP */
int      monitor;               /* Monitor -m specified */
char     *opc_name[128];        /* texual descriptors of opcodes */
int16_t  regs[16];              /* registers */
int      verbose;               /* Verbose -v specified */

void initialize(void) {
	opc_name[_ADD]  = "ADD";
	opc_name[_SUB]  = "SUB";
	opc_name[_MUL]  = "MUL";
	opc_name[_DIV]  = "DIV";
	opc_name[_MOD]  = "MOD";
	opc_name[_BOR]  = "BOR";
	opc_name[_XOR]  = "XOR";
	opc_name[_BAND] = "AND";
	opc_name[_LSR]  = "LSR";
	opc_name[_LSL]  = "LSL";
	opc_name[_MOVR] = "MOVR";

	opc_name[_ILLEGAL] = "ILLEGAL";
	opc_name[_NEG]     = "NEG";
	opc_name[_NOT]     = "NOT";
	opc_name[_EQ]      = "BEQ";
	opc_name[_NE]      = "BNE";
	opc_name[_LT]      = "BLT";
	opc_name[_LE]      = "BLE";
	opc_name[_GT]      = "BGT";
	opc_name[_GE]      = "BGE";
	opc_name[_MOVB]    = "LODB";
	opc_name[_MOVW]    = "LODW";
	opc_name[_LEA]     = "LODA";
	opc_name[_CMP]     = "CMP";
	opc_name[_STOB]    = "STOB";
	opc_name[_STOW]    = "STOW";
	opc_name[_JMP]     = "JMP";
	opc_name[_JSB]     = "JSB";
	opc_name[_RSB]     = "RSB";
	opc_name[_PSHR]    = "PSHR";
	opc_name[_POPR]    = "POPR";
	opc_name[_PSHB]    = "PSHB";
	opc_name[_PSHW]    = "PSHW";
	opc_name[_PSHA]    = "PSHA";
	opc_name[_SVC]     = "SVC";

	handles[0] = stdin;
	handles[1] = stdout;
	handles[2] = stderr;
}

char *fext(char *path, char *ext, int force) {
	char *p;
	int  baselen;

	baselen = 0;
	for (p  = path; *p; p++) {
		if (*p == '\\' || *p == '/')
			baselen = 0;
		else if (*p == '.')
			baselen = p - path;
	}

	if (!baselen)
		baselen = p - path;
	else if (!force)
		return path;

	p = malloc(baselen + strlen(ext) + 1);
	strncpy(p, path, baselen);
	strcpy(p + baselen, ext);

	return p;
}

/*
** Process commandline
*/
void usage(void) {
	fprintf(stderr, "X-Emulator, Version %s\n\n", PACKAGE_VERSION); /* Print banner */

	fprintf(stderr, "usage: xemu [-m] <file>[.<ext>] [<command>]\n");
	exit(1);
}

void startup(int argc, char **argv) {
	int i, ext;

	argv++; // skip program name
	while (*argv) {
		if (argv[0][0] != '-') {
			inpargv = argv;
			inpargv[0] = fext(inpargv[0], ".img", 0);
			return;
		} else if (argv[0][1] == 'm') {
			monitor = 1;
			argv++;
		} else if (argv[0][1] == 'v') {
			verbose = 1;
			argv++;
		} else {
			usage();
		}
	}

	usage();
}

void shutdown(int code) {
	if (verbose)
		printf("lowestSP=%04x\n", lowestSP);
	exit(code);
}

uint16_t pushArgs(uint16_t sp, char *argv[]) {
	int i, len, strbase, argvbase, argc;
	char *arg;

	// count needed string space
	len = argc = 0;
	for (i = 0; argv[i]; i++)
		len += strlen(argv[argc++]) + 2;

	// align
	len = (len + 1) & ~1;

	// string base
	sp = (sp - len) & 0xffff;
	strbase = sp;
	// argv base
	sp = (sp - (argc + 1) * BPW) & 0xffff;
	argvbase = sp;

	// copy strings
	for (i = 0; i < argc; i++) {
		image[argvbase++] = strbase >> 8;
		image[argvbase++] = strbase;
		for (arg = argv[i]; *arg; arg++)
			image[strbase++] = *arg;
		image[strbase++] = 0;
	}

	// terminator NULL
	image[argvbase++] = 0;
	image[argvbase++] = 0;

	// push argc
	sp -= BPW;
	image[sp + 0] = argc >> 8;
	image[sp + 1] = argc;

	// push argv
	sp -= BPW;
	image[sp + 0] = (sp + 4) >> 8;
	image[sp + 1] = (sp + 4);

	// push size of push-frame
	sp -= BPW;
	image[sp + 0] = 0;
	image[sp + 1] = 3 * BPW;

	// push return address
	sp -= BPW;
	image[sp + 0] = 0;
	image[sp + 1] = 0;

	return sp;
}

/**********************************************************************/
/*                                                                    */
/*    The emulator                                                    */
/*                                                                    */
/**********************************************************************/

void disp_reg(uint16_t pc, int cc) {
	printf("PC:%04x R00:%04x R01:%04x R02:%04x R03:%04x R04:%04x R05:%04x R06:%04x R07:%04x\n",
	       pc, regs[0] & 0xffff, regs[1] & 0xffff, regs[2] & 0xffff, regs[3] & 0xffff,
	       regs[4] & 0xffff, regs[5] & 0xffff, regs[6] & 0xffff, regs[7] & 0xffff);
	printf("CC:%04x R08:%04x R09:%04x R10:%04x R11:%04x R12:%04x R13:%04x R14:%04x R15:%04x\n",
	       cc, regs[8] & 0xffff, regs[9] & 0xffff, regs[10] & 0xffff, regs[11] & 0xffff,
	       regs[12] & 0xffff, regs[13] & 0xffff, regs[14] & 0xffff, regs[15] & 0xffff);
}

void disp_opc(uint16_t pc) {
	switch (image[pc]) {
	case _ADD:
	case _SUB:
	case _MUL:
	case _DIV:
	case _MOD:
	case _BOR:
	case _XOR:
	case _BAND:
	case _LSR:
	case _LSL:
	case _MOVR:
	case _CMP:
		printf("%s R%d,R%d\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2]);
		break;
	case _NEG:
	case _NOT:
		printf("%s R%d\n", opc_name[image[pc + 0]], image[pc + 1]);
		break;
	case _ILLEGAL:
	case _RSB:
		printf("%s\n", opc_name[image[pc + 0]]);
		break;
	case _EQ:
	case _NE:
	case _LT:
	case _LE:
	case _GT:
	case _GE:
	case _JMP:
	case _JSB:
	case _PSHB:
	case _PSHW:
	case _PSHA:
		printf("%s %02x%02x(R%d)\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2], image[pc + 3]);
		break;
	case _MOVB:
	case _MOVW:
	case _LEA:
	case _STOB:
	case _STOW:
		printf("%s R%d,%02x%02x(R%d)\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2], image[pc + 3], image[pc + 4]);
		break;
	case _PSHR:
	case _POPR:
	case _SVC:
		printf("%s %02x%02x\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2]);
		break;
	default:
		printf("OPC_%d\n", image[pc + 0]);
		break;
	}
}

void disp_dump(uint16_t pc, int cc) {
	uint16_t loc;
	uint8_t *cp;
	int i, j;

	disp_reg(pc, cc);
	printf("\nStackdump:\n");
	loc = regs[15];
	for (i = 0; i < 4; i++, loc += 0x10) {
		printf("%04x:", loc);
		cp = &image[loc];
		for (j = 0; j < 8; j++, cp += 2)
			printf(" %02x%02x", cp[0], cp[1]);
		printf("\n");
	}

	printf("\nCodedump:\n");
	loc = pc;
	for (i = 0; i < 4; i++, loc += 0x10) {
		printf("%04x:", loc);
		cp = &image[loc];
		for (j = 0; j < 8; j++, cp++)
			printf(" %02x", cp[0]);
		printf("\n");
	}
}

void do_svc(uint16_t pc, int16_t id, int cc) {
	uint16_t ctrl[10];

	switch (id) {
	case 31: { /* osprint() */
		uint8_t *pb = &image[regs[1] & 0xFFFF]; /* get addr parmblock */
		char *str = &image[pb[0] << 8 | pb[1]]; /* get addr string */

		fputs(str, stdout);
		break;
	}
	case 40: { /* fread() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];
		ctrl[3] = pb[6] << 8 | pb[7];
		char *addr = &image[ctrl[0]];

		if (ctrl[3] < 0 || ctrl[3] >= MAXFILE || !handles[ctrl[3]])
			regs[1] = -1;
		else
			regs[1] = fread(addr, ctrl[1] & 0xffff, ctrl[2] & 0xffff, handles[ctrl[3]]);
		break;
	}
	case 41: { /* fwrite() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];
		ctrl[3] = pb[6] << 8 | pb[7];
		char *addr = &image[ctrl[0]];

		if (ctrl[3] < 0 || ctrl[3] >= MAXFILE || !handles[ctrl[3]])
			regs[1] = -1;
		else
			regs[1] = fwrite(addr, ctrl[1] & 0xffff, ctrl[2] & 0xffff, handles[ctrl[3]]);
		break;
	}
	case 42: { /* fopen() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		char *name = &image[ctrl[0]]; /* get addr string */
		char *mode = &image[ctrl[1]]; /* get addr string */

		int hdl;
		for (hdl = 6; hdl < MAXFILE; hdl++) {
			if (!handles[hdl])
				break;
		}
		if (hdl >= MAXFILE)
			fprintf(stderr, "ERROR: Too many open files\n"), shutdown(1);

		handles[hdl] = fopen(name, mode);
		regs[1] = (handles[hdl] == NULL) ? 0 : hdl;
		break;
	}
	case 43: { /* fclose() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		if (ctrl[0] < 0 || ctrl[0] >= MAXFILE || !handles[ctrl[0]])
			regs[1] = -1;
		else {
			regs[1] = fclose(handles[ctrl[0]]);

			handles[ctrl[0]] = NULL; /* release */
		}
		break;
	}
	case 44: { /* fseek() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];

		long ofs = ctrl[1];

		/* ofs is unsigned for SEEK_SET and signed otherwise */
		if (ctrl[2] == SEEK_SET)
			ofs &= 0xffff;
		else
			ofs |= -(ofs & (1 << 15));

		if (ctrl[0] < 0 || ctrl[0] >= MAXFILE || !handles[ctrl[0]])
			regs[1] = -1;
		else
			regs[1] = fseek(handles[ctrl[0]], ofs, ctrl[2]);
		break;
	}
	case 45: { /* unlink() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		char *name = &image[pb[0] << 8 | pb[1]]; /* get addr string */

		regs[1] = unlink(name);
		break;
	}
	case 46: { /* rename() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		char *oldname = &image[ctrl[0]]; /* get addr string */
		char *newname = &image[ctrl[1]]; /* get addr string */

		regs[1] = rename(oldname, newname);
		break;
	}
	case 47: { /* ftell() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		if (ctrl[0] < 0 || ctrl[0] >= MAXFILE || !handles[ctrl[0]])
			regs[1] = -1;
		else
			regs[1] = ftell(handles[ctrl[0]]);
		break;
	}
	case 90: /* OSINFO */
		switch (regs[0]) {
		case 0x32: { /* Get commandline */
			char **cpp;

			uint8_t *pb = &image[regs[1] & 0xFFFF]; /* get addr parmblock */
			char *args = &image[pb[0] << 8 | pb[1]]; /* get addr string */

			/* concat argv[] except for argv[0] */
			cpp = inpargv + 1;
			if (*cpp)
				strcpy(args, *cpp++);
			while (*cpp) {
				strcat(args, " ");
				strcpy(args, *cpp++);
			}

			break;
		}
		default:
			printf("unimplemented OSINFO call\n");
			disp_opc(pc);
			disp_dump(pc, cc);
			shutdown(ctrl[0]);
			break;
		}
		break;
	case 99: { /* exit() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		shutdown(ctrl[0]);
		break;
	}
	case 100: /* MONITOR ON */
		monitor = 1;
		break;
	case 101: /* MONITOR OFF */
		monitor = 0;
		break;
	default:
		printf("unimplemented SVC call\n");
		disp_opc(pc);
		disp_dump(pc, cc);
		shutdown(1);
	}
}

void run(uint16_t inisp) {

	uint16_t pc;
	int16_t lval;
	int16_t rval;
	uint16_t opc;
	char *cp;
	int cc, i;

	/* initialize */
	pc = 0;
	cc = 0;
	for (i = 0; i < 16; i++)
		regs[i] = 0;
	regs[15] = inisp;
	while (1) {
		if (monitor) {
			disp_reg(pc, cc);
			disp_opc(pc);
		}
		if (!lowestSP || (regs[15] & 0xffff) < lowestSP) {
			lowestSP = regs[15];
			if (monitor)
				printf("lowestSP=%04x\n", lowestSP);
		}

		opc = image[pc++];
		switch (opc) {
		case _ADD:
		case _SUB:
		case _MUL:
		case _DIV:
		case _MOD:
		case _BOR:
		case _XOR:
		case _BAND:
		case _LSR:
		case _LSL:
		case _MOVR:
		case _CMP:
			/* load operands */
			lval = regs[image[pc++] & 0xF];
			rval = regs[image[pc++] & 0xF];
			/* modify operands */
			switch (opc) {
			case _ADD:
				lval += rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _SUB:
				lval -= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _MUL:
				lval *= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _DIV:
				lval /= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _MOD:
				lval %= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _BOR:
				lval |= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _XOR:
				lval ^= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _BAND:
				lval &= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _LSR:
				lval >>= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _LSL:
				lval <<= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _MOVR:
				lval = rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case _CMP:
				lval = lval - rval;
				break;
			}

			if (lval == 0)
				cc = 0;
			else if (lval & (1 << SBIT))
				cc = 1;
			else
				cc = 2;
			break;
		case _NEG:
		case _NOT:
			/* load operands */
			lval = regs[image[pc++] & 0xF];
			/* modify operands */
			if (opc == _NEG)
				lval = -regs[image[pc - 1] & 0xF];
			else if (opc == _NOT)
				lval = ~regs[image[pc - 1] & 0xF];
			regs[image[pc - 1] & 0xF] = lval;

			if (lval == 0)
				cc = 0;
			else if (lval & (1 << SBIT))
				cc = 1;
			else
				cc = 2;
			break;
		case _JMP:
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			pc = rval & 0xffff;
			break;
		case _EQ:
		case _NE:
		case _GT:
		case _GE:
		case _LT:
		case _LE:
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == _EQ) {
				if (cc == 0) pc = rval & 0xffff;
			} else if (opc == _NE) {
				if (cc != 0) pc = rval & 0xffff;
			} else if (opc == _GT) {
				if (cc == 2) pc = rval & 0xffff;
			} else if (opc == _GE) {
				if (cc != 1) pc = rval & 0xffff;
			} else if (opc == _LT) {
				if (cc == 1) pc = rval & 0xffff;
			} else if (opc == _LE) {
				if (cc != 2) pc = rval & 0xffff;
			}
			break;
		case _LEA:
		case _MOVB:
		case _MOVW:
		case _STOB:
		case _STOW:
			/* get lreg */
			lval = image[pc++] & 0xF;
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == _LEA) {
				regs[lval & 0xF] = rval;
			} else if (opc == _MOVB) {
				cp = &image[rval & 0xffff];
				rval = cp[0];
				regs[lval & 0xF] = rval;
			} else if (opc == _MOVW) {
				cp = &image[rval & 0xffff];
				rval = (cp[0] << 8) + (cp[1] & 0xFF);
				regs[lval & 0xF] = rval;
			} else if (opc == _STOB) {
				cp = &image[rval & 0xffff];
				rval = regs[lval & 0xF];
				cp[0] = rval;
			} else if (opc == _STOW) {
				cp = &image[rval & 0xffff];
				rval = regs[lval & 0xF];
				cp[0] = rval >> 8;
				cp[1] = rval;
			}

			/* update CC */
			if (rval > 0) cc = 2; else if (rval < 0) cc = 1; else cc = 0;
			break;
		case _PSHA:
		case _PSHB:
		case _PSHW:
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == _PSHA) {
				regs[15] -= BPW;
				cp = &image[regs[15] & 0xffff]; /* get -(SP) */
				cp[0] = rval >> 8;
				cp[1] = rval;
			} else if (opc == _PSHB) {
				cp = &image[rval & 0xffff];
				rval = cp[0];
				regs[15] -= BPW;
				cp = &image[regs[15] & 0xffff]; /* get -(SP) */
				cp[0] = rval >> 8;
				cp[1] = rval;
			} else if (opc == _PSHW) {
				cp = &image[rval & 0xffff];
				rval = (cp[0] << 8) + (cp[1] & 0xFF);
				regs[15] -= BPW;
				cp = &image[regs[15] & 0xffff]; /* get -(SP) */
				cp[0] = rval >> 8;
				cp[1] = rval;
			}

			/* update CC */
			if (rval > 0) cc = 2; else if (rval < 0) cc = 1; else cc = 0;
			break;
		case _JSB:
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* save old PC */
			regs[15] -= BPW;
			cp = &image[regs[15] & 0xffff]; /* get -(SP) */
			cp[0] = pc >> 8;
			cp[1] = pc;
			/* update PC */
			pc = rval & 0xffff;
			break;
		case _RSB:
			cp = &image[regs[15] & 0xffff]; /* get (SP)+ */
			regs[15] += BPW;
			pc = ((cp[0] << 8) + (cp[1] & 0xFF)) & 0xffff;
			break;
		case _PSHR:
			/* get IMM */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			/* push regs */
			for (i = 0; i < 16; i++) {
				if (rval & 0x0001) {
					regs[15] -= BPW;
					cp = &image[regs[15] & 0xffff]; /* get -(SP) */
					lval = regs[i];
					cp[0] = lval >> 8;
					cp[1] = lval;
				}
				rval >>= 1;
			}
			break;
		case _POPR:
			/* get IMM */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			/* push regs */
			for (i = 15; i >= 0; i--) {
				if (rval & 0x8000) {
					cp = &image[regs[15] & 0xffff]; /* get (SP)+ */
					regs[15] += BPW;
					regs[i] = (cp[0] << 8) + (cp[1] & 0xFF);
				}
				rval <<= 1;
			}
			break;
		case _SVC:
			/* get IMM */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			/* process */
			do_svc(pc - 2, rval, cc);
			break;
		default:
			printf("encountered unimplemented opcode\n");
			disp_opc(pc - 1);
			disp_dump(pc - 1, cc);
			if (verbose)
				printf("lowestSP=%04x\n", lowestSP);
			shutdown(1);
		}
	}
}

/**********************************************************************/
/*                                                                    */
/*    Main                                                            */
/*                                                                    */
/**********************************************************************/

/*
 * Program start
 */
int main(int argc, char **argv) {
	int fd;
	uint16_t inisp;

	setlinebuf(stdout);

	initialize();

	startup(argc, argv);

	/*
	 * Load image
	 */
	fd = open(inpargv[0], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open error on '%s'. error=%s\n", inpargv[0], strerror(errno));
		exit(1);
	}
	int r = read(fd, image, 0x10000);
	close(fd);

	/*
	 * Push argc/argv on stack
	 */
	inisp = pushArgs(inisp, inpargv);

	run(inisp);
	return 0;
}
