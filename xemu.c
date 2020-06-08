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
 * Internal storage limits
 */

enum {
	FILEMAX = 100,			// Number of files
};

/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,			// Bytes per word
	SBIT = 15,			// Bit number of signed bit
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
	OPC_BEQ = 0x38,
	OPC_BNE = 0x37,
	OPC_BLT = 0x34,
	OPC_BLE = 0x33,
	OPC_BGT = 0x36,
	OPC_BGE = 0x35,
	OPC_LDB = 0x04,
	OPC_LDW = 0x01,
	OPC_LDR = 0x11,
	OPC_LEA = 0x03,
	OPC_CMP = 0x10,
	OPC_STB = 0x05,
	OPC_STW = 0x02,
	OPC_JMP = 0x3F,
	OPC_JSB = 0x20,
	OPC_RSB = 0x21,
	OPC_PSHR = 0x23,
	OPC_POPR = 0x24,
	OPC_PSHB = 0x25,
	OPC_PSHW = 0x26,
	OPC_PSHA = 0x27,
	OPC_SVC = 0x0A,

	OPC_ADDA = 0x4c+0,
	OPC_ADDB = 0x4c+1,
	OPC_ADDW = 0x4c+2,
	OPC_SUBA = 0x50+0,
	OPC_SUBB = 0x50+1,
	OPC_SUBW = 0x50+2,
	OPC_LODA  = 0x70+0,
	OPC_LODB  = 0x70+1,
	OPC_LODW  = 0x70+2,
	OPC_STOA  = 0x74+0,
	OPC_STOB  = 0x74+1,
	OPC_STOW  = 0x74+2,
	OPC_JEQA  = 0x78+0,
	OPC_JEQB  = 0x78+1,
	OPC_JEQW  = 0x78+2,
	OPC_JNEA  = 0x7c+0,
	OPC_JNEB  = 0x7c+1,
	OPC_JNEW  = 0x7c+2,
};

/*
** Storage
*/

FILE     *handles[FILEMAX];     /* handle/FILE mapping */
uint8_t  image[0x10000];        /* actual image */
char     **inpargv;             /* argv[] for emulated */
uint16_t lowestSP;              /* Lowest encountered SP */
int      monitor;               /* Monitor -m specified */
char     *opc_name[128];        /* texual descriptors of opcodes */
int16_t  regs[16];              /* registers */
int      verbose;               /* Verbose -v specified */

void initialize(void) {
	opc_name[OPC_ADD] = "add";
	opc_name[OPC_SUB] = "sub";
	opc_name[OPC_MUL] = "mul";
	opc_name[OPC_DIV] = "div";
	opc_name[OPC_MOD] = "mod";
	opc_name[OPC_OR] = "or";
	opc_name[OPC_XOR] = "xor";
	opc_name[OPC_AND] = "and";
	opc_name[OPC_LSR] = "lsr";
	opc_name[OPC_LSL] = "lsl";
	opc_name[OPC_LDR] = "ldr";

	opc_name[OPC_ILLEGAL] = "illegal";
	opc_name[OPC_NEG] = "neg";
	opc_name[OPC_NOT] = "not";
	opc_name[OPC_BEQ] = "beq";
	opc_name[OPC_BNE] = "bne";
	opc_name[OPC_BLT] = "blt";
	opc_name[OPC_BLE] = "ble";
	opc_name[OPC_BGT] = "bgt";
	opc_name[OPC_BGE] = "bge";
	opc_name[OPC_LDB] = "ldb";
	opc_name[OPC_LDW] = "ldw";
	opc_name[OPC_LEA] = "loda";
	opc_name[OPC_CMP] = "cmp";
	opc_name[OPC_STB] = "stb";
	opc_name[OPC_STW] = "stw";
	opc_name[OPC_JMP] = "jmp";
	opc_name[OPC_JSB] = "jsb";
	opc_name[OPC_RSB] = "rsb";
	opc_name[OPC_PSHR] = "pshr";
	opc_name[OPC_POPR] = "popr";
	opc_name[OPC_PSHB] = "pshb";
	opc_name[OPC_PSHW] = "pshw";
	opc_name[OPC_PSHA] = "psha";
	opc_name[OPC_SVC] = "svc";

	handles[0] = stdin;
	handles[1] = stdout;
	handles[2] = stderr;

	opc_name[OPC_ADDA] = "add.a";
	opc_name[OPC_ADDB] = "add.b";
	opc_name[OPC_ADDW] = "add.w";
	opc_name[OPC_SUBA] = "sub.a";
	opc_name[OPC_SUBB] = "sub.b";
	opc_name[OPC_SUBW] = "sub.w";
	opc_name[OPC_LODA] = "ld.a";
	opc_name[OPC_LODB] = "ld.b";
	opc_name[OPC_LODW] = "ld.w";
	opc_name[OPC_STOA] = "st.a";
	opc_name[OPC_STOB] = "st.b";
	opc_name[OPC_STOW] = "st.w";
	opc_name[OPC_JEQA] = "jeq.a";
	opc_name[OPC_JEQB] = "jeq.b";
	opc_name[OPC_JEQW] = "jeq.w";
	opc_name[OPC_JNEA] = "jne.a";
	opc_name[OPC_JNEB] = "jne.b";
	opc_name[OPC_JNEW] = "jne.w";
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
	case OPC_CMP:
		printf("%s R%d,R%d\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2]);
		break;
	case OPC_NEG:
	case OPC_NOT:
		printf("%s R%d\n", opc_name[image[pc + 0]], image[pc + 1]);
		break;
	case OPC_ILLEGAL:
	case OPC_RSB:
		printf("%s\n", opc_name[image[pc + 0]]);
		break;
	case OPC_BEQ:
	case OPC_BNE:
	case OPC_BLT:
	case OPC_BLE:
	case OPC_BGT:
	case OPC_BGE:
	case OPC_JMP:
	case OPC_JSB:
	case OPC_PSHB:
	case OPC_PSHW:
	case OPC_PSHA:
		printf("%s %02x%02x(R%d)\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2], image[pc + 3]);
		break;
	case OPC_LDB:
	case OPC_LDW:
	case OPC_LEA:
	case OPC_STB:
	case OPC_STW:
		printf("%s R%d,%02x%02x(R%d)\n", opc_name[image[pc + 0]], image[pc + 1], image[pc + 2], image[pc + 3], image[pc + 4]);
		break;
	case OPC_PSHR:
	case OPC_POPR:
	case OPC_SVC:
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

		if (ctrl[3] < 0 || ctrl[3] >= FILEMAX || !handles[ctrl[3]])
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

		if (ctrl[3] < 0 || ctrl[3] >= FILEMAX || !handles[ctrl[3]])
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
		for (hdl = 6; hdl < FILEMAX; hdl++) {
			if (!handles[hdl])
				break;
		}
		if (hdl >= FILEMAX)
			fprintf(stderr, "ERROR: Too many open files\n"), shutdown(1);

		handles[hdl] = fopen(name, mode);
		regs[1] = (handles[hdl] == NULL) ? 0 : hdl;
		break;
	}
	case 43: { /* fclose() */
		uint8_t *pb = &image[regs[1] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
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

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
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

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
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
		case OPC_CMP:
			/* load operands */
			lval = regs[image[pc++] & 0xF];
			rval = regs[image[pc++] & 0xF];
			/* modify operands */
			switch (opc) {
			case OPC_ADD:
				lval += rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_SUB:
				lval -= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_MUL:
				lval *= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_DIV:
				lval /= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_MOD:
				lval %= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_OR:
				lval |= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_XOR:
				lval ^= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_AND:
				lval &= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_LSR:
				lval >>= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_LSL:
				lval <<= rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_LDR:
				lval = rval;
				regs[image[pc - 2] & 0xF] = lval;
				break;
			case OPC_CMP:
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
		case OPC_NEG:
		case OPC_NOT:
			/* load operands */
			lval = regs[image[pc++] & 0xF];
			/* modify operands */
			if (opc == OPC_NEG)
				lval = -regs[image[pc - 1] & 0xF];
			else if (opc == OPC_NOT)
				lval = ~regs[image[pc - 1] & 0xF];
			regs[image[pc - 1] & 0xF] = lval;

			if (lval == 0)
				cc = 0;
			else if (lval & (1 << SBIT))
				cc = 1;
			else
				cc = 2;
			break;
		case OPC_JMP:
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			pc = rval & 0xffff;
			break;
		case OPC_BEQ:
		case OPC_BNE:
		case OPC_BGT:
		case OPC_BGE:
		case OPC_BLT:
		case OPC_BLE:
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == OPC_BEQ) {
				if (cc == 0) pc = rval & 0xffff;
			} else if (opc == OPC_BNE) {
				if (cc != 0) pc = rval & 0xffff;
			} else if (opc == OPC_BGT) {
				if (cc == 2) pc = rval & 0xffff;
			} else if (opc == OPC_BGE) {
				if (cc != 1) pc = rval & 0xffff;
			} else if (opc == OPC_BLT) {
				if (cc == 1) pc = rval & 0xffff;
			} else if (opc == OPC_BLE) {
				if (cc != 2) pc = rval & 0xffff;
			}
			break;
		case OPC_LEA:
		case OPC_LDB:
		case OPC_LDW:
		case OPC_STB:
		case OPC_STW:
			/* get lreg */
			lval = image[pc++] & 0xF;
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == OPC_LEA) {
				regs[lval & 0xF] = rval;
			} else if (opc == OPC_LDB) {
				cp = &image[rval & 0xffff];
				rval = cp[0];
				regs[lval & 0xF] = rval;
			} else if (opc == OPC_LDW) {
				cp = &image[rval & 0xffff];
				rval = (cp[0] << 8) + (cp[1] & 0xFF);
				regs[lval & 0xF] = rval;
			} else if (opc == OPC_STB) {
				cp = &image[rval & 0xffff];
				rval = regs[lval & 0xF];
				cp[0] = rval;
			} else if (opc == OPC_STW) {
				cp = &image[rval & 0xffff];
				rval = regs[lval & 0xF];
				cp[0] = rval >> 8;
				cp[1] = rval;
			}

			/* update CC */
			if (rval > 0) cc = 2; else if (rval < 0) cc = 1; else cc = 0;
			break;
		case OPC_PSHA:
		case OPC_PSHB:
		case OPC_PSHW:
			/* get EA */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			rval += regs[image[pc++] & 0xF];
			/* process */
			if (opc == OPC_PSHA) {
				regs[15] -= BPW;
				cp = &image[regs[15] & 0xffff]; /* get -(SP) */
				cp[0] = rval >> 8;
				cp[1] = rval;
			} else if (opc == OPC_PSHB) {
				cp = &image[rval & 0xffff];
				rval = cp[0];
				regs[15] -= BPW;
				cp = &image[regs[15] & 0xffff]; /* get -(SP) */
				cp[0] = rval >> 8;
				cp[1] = rval;
			} else if (opc == OPC_PSHW) {
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
		case OPC_JSB:
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
		case OPC_RSB:
			cp = &image[regs[15] & 0xffff]; /* get (SP)+ */
			regs[15] += BPW;
			pc = ((cp[0] << 8) + (cp[1] & 0xFF)) & 0xffff;
			break;
		case OPC_PSHR:
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
		case OPC_POPR:
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
		case OPC_SVC:
			/* get IMM */
			rval = image[pc++] << 8;
			rval += image[pc++] & 0xFF;
			/* process */
			do_svc(pc - 2, rval, cc);
			break;
		case OPC_STOB: {
			// risc
			register int lreg, ea;
			lreg = (image[pc] & 0xF0) >> 4;
			ea = regs[image[pc++] & 0xF];
			ea += image[pc++] << 8;
			ea += image[pc++] & 0xFF;

			cp = &image[ea & 0xffff];
			lval = regs[lreg];
			cp[0] = lval;

			/* update CC */
			if (lval > 0) cc = 2; else if (lval < 0) cc = 1; else cc = 0;
			break;
		}
		case OPC_STOW: {
			// risc
			register int lreg, ea;
			lreg = (image[pc] & 0xF0) >> 4;
			ea = regs[image[pc++] & 0xF];
			ea += image[pc++] << 8;
			ea += image[pc++] & 0xFF;

			cp = &image[ea & 0xffff];
			lval = regs[lreg];
			cp[0] = lval >> 8;
			cp[1] = lval;

			/* update CC */
			if (lval > 0) cc = 2; else if (lval < 0) cc = 1; else cc = 0;
			break;
		}
		default: {
			register int lreg, ea;

			// RISC instructions
			// 32<-----------------------------------------------------<0
			// <immlo.8> <immhi.8> <eareg>.4 <lreg.4> <opcode.6> <size.2>

			lreg = (image[pc] & 0xF0) >> 4;
			ea = regs[image[pc++] & 0xF];
			ea += image[pc++] << 8;
			ea += image[pc++] & 0xFF;

			if ((opc & 3) == 1) {
				cp = &image[ea & 0xffff];
				ea = cp[0];
			} else if ((opc & 3) == 2) {
				cp = &image[ea & 0xffff];
				ea = (cp[0] << 8) + (cp[1] & 0xFF);
			}

			switch (opc) {
			case OPC_ADDA:
			case OPC_ADDB:
			case OPC_ADDW:
				lval = regs[lreg] += ea;
				break;
			case OPC_SUBA:
			case OPC_SUBB:
			case OPC_SUBW:
				lval = regs[lreg] -= ea;
				break;
			case OPC_LODA:
			case OPC_LODB:
			case OPC_LODW:
				lval = regs[lreg] = ea;
				break;
			case OPC_JEQA:
			case OPC_JEQB:
			case OPC_JEQW:
				lval = regs[lreg];
				if (lval == 0)
					pc = ea;
				break;
			case OPC_JNEA:
			case OPC_JNEB:
			case OPC_JNEW:
				lval = regs[lreg];
				if (lval != 0)
					pc = ea;
				break;
			default:
				pc -= 3;
				printf("encountered unimplemented opcode\n");
				disp_opc(pc - 1);
				disp_dump(pc - 1, cc);
				if (verbose)
					printf("lowestSP=%04x\n", lowestSP);
				shutdown(1);
			}

			if (lval == 0)
				cc = 0;
			else if (lval & (1 << SBIT))
				cc = 1;
			else
				cc = 2;

		}
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
