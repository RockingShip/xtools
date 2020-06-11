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
	BPW = 2,		// Bytes per word
	SBIT = 15,		// Bit number of signed bit
	REGMAX = 16,		// Number of registers
	REG_SP = 15,		// Default stack pointer
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
** Storage
*/

FILE     *handles[FILEMAX];     /* handle/FILE mapping */
uint8_t  image[0x10000];        /* actual image */
char     **inpargv;             /* argv[] for emulated */
uint16_t lowestSP;              /* Lowest encountered SP */
int      monitor;               /* Monitor -m specified */
char     *opc_name[256];        /* texual descriptors of opcodes */
int16_t  regs[REGMAX];              /* registers */
int      verbose;               /* Verbose -v specified */

void initialize(void) {

	handles[0] = stdin;
	handles[1] = stdout;
	handles[2] = stderr;

	opc_name[OPC_SVC+0] = "svc.a";
	opc_name[OPC_SVC+1] = "svc.b";
	opc_name[OPC_SVC+2] = "svc.w";
	opc_name[OPC_MUL+0] = "mul.a";
	opc_name[OPC_MUL+1] = "mul.b";
	opc_name[OPC_MUL+2] = "mul.w";
	opc_name[OPC_DIV+0] = "div.a";
	opc_name[OPC_DIV+1] = "div.b";
	opc_name[OPC_DIV+2] = "div.w";
	opc_name[OPC_MOD+0] = "mod.a";
	opc_name[OPC_MOD+1] = "mod.b";
	opc_name[OPC_MOD+2] = "mod.w";
	opc_name[OPC_ADD+0] = "add.a";
	opc_name[OPC_ADD+1] = "add.b";
	opc_name[OPC_ADD+2] = "add.w";
	opc_name[OPC_SUB+0] = "sub.a";
	opc_name[OPC_SUB+1] = "sub.b";
	opc_name[OPC_SUB+2] = "sub.w";
	opc_name[OPC_LSL+0] = "lsl.a";
	opc_name[OPC_LSL+1] = "lsl.b";
	opc_name[OPC_LSL+2] = "lsl.w";
	opc_name[OPC_LSR+0] = "lsr.a";
	opc_name[OPC_LSR+1] = "lsr.b";
	opc_name[OPC_LSR+2] = "lsr.w";
	opc_name[OPC_AND+0] = "and.a";
	opc_name[OPC_AND+1] = "and.b";
	opc_name[OPC_AND+2] = "and.w";
	opc_name[OPC_XOR+0] = "xor.a";
	opc_name[OPC_XOR+1] = "xor.b";
	opc_name[OPC_XOR+2] = "xor.w";
	opc_name[OPC_OR+0] = "or.a";
	opc_name[OPC_OR+1] = "or.b";
	opc_name[OPC_OR+2] = "or.w";
	opc_name[OPC_SGT+0] = "sgt.a";
	opc_name[OPC_SGT+1] = "sgt.b";
	opc_name[OPC_SGT+2] = "sgt.w";
	opc_name[OPC_SLT+0] = "slt.a";
	opc_name[OPC_SLT+1] = "slt.b";
	opc_name[OPC_SLT+2] = "slt.w";
	opc_name[OPC_LD+0] = "ld.a";
	opc_name[OPC_LD+1] = "ld.b";
	opc_name[OPC_LD+2] = "ld.w";
	opc_name[OPC_NEG+0] = "neg.a";
	opc_name[OPC_NEG+1] = "neg.b";
	opc_name[OPC_NEG+2] = "neg.w";
	opc_name[OPC_NOT+0] = "not.a";
	opc_name[OPC_NOT+1] = "not.b";
	opc_name[OPC_NOT+2] = "not.w";
	opc_name[OPC_ST+0] = "st.a";
	opc_name[OPC_ST+1] = "st.b";
	opc_name[OPC_ST+2] = "st.w";
	opc_name[OPC_JZ+0] = "jz.a";
	opc_name[OPC_JZ+1] = "jz.b";
	opc_name[OPC_JZ+2] = "jz.w";
	opc_name[OPC_JNZ+0] = "jnz.a";
	opc_name[OPC_JNZ+1] = "jnz.b";
	opc_name[OPC_JNZ+2] = "jnz.w";
	opc_name[OPC_JSB+0] = "jsb.a";
	opc_name[OPC_JSB+1] = "jsb.b";
	opc_name[OPC_JSB+2] = "jsb.w";
	opc_name[OPC_RSB+0] = "rsb.a";
	opc_name[OPC_RSB+1] = "rsb.b";
	opc_name[OPC_RSB+2] = "rsb.w";
	opc_name[OPC_SVC+0] = "svc.a";
	opc_name[OPC_SVC+1] = "svc.b";
	opc_name[OPC_SVC+2] = "svc.w";
	opc_name[OPC_PUSH+0] = "push.a";
	opc_name[OPC_PUSH+1] = "push.b";
	opc_name[OPC_PUSH+2] = "push.w";
	opc_name[OPC_PSHR+0] = "pshr.a";
	opc_name[OPC_PSHR+1] = "pshr.b";
	opc_name[OPC_PSHR+2] = "pshr.w";
	opc_name[OPC_POPR+0] = "popr.a";
	opc_name[OPC_POPR+1] = "popr.b";
	opc_name[OPC_POPR+2] = "popr.w";
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

void disp_reg(uint16_t pc) {
	printf("pc:%04x r0:%04x r1:%04x r2:%04x r3:%04x r4:%04x r5:%04x r6:%04x r7:%04x r8:%04x r9:%04x r10:%04x r11:%04x r12:%04x r13:%04x r14:%04x r15:%04x\n",
	       pc,
	       regs[0] & 0xffff, regs[1] & 0xffff, regs[2] & 0xffff, regs[3] & 0xffff,
	       regs[4] & 0xffff, regs[5] & 0xffff, regs[6] & 0xffff, regs[7] & 0xffff,
	       regs[8] & 0xffff, regs[9] & 0xffff, regs[10] & 0xffff, regs[11] & 0xffff,
	       regs[12] & 0xffff, regs[13] & 0xffff, regs[14] & 0xffff, regs[15] & 0xffff);
}

void disp_opc(uint16_t pc) {
		printf("%s r%d,%02x%02x", opc_name[image[pc + 0]], image[pc + 1] & 0xf, image[pc + 2], image[pc + 3]);
		if ((image[pc + 1] >> 4) & 0xf)
			printf("(r%d)", (image[pc + 1] >> 4) & 0xf);
		putchar ('\n');
}

void disp_dump(uint16_t pc) {
	uint16_t loc;
	uint8_t *cp;
	int i, j;

	disp_reg(pc);
	printf("\nStackdump:\n");
	loc = regs[REG_SP];
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

void do_svc(uint16_t pc, int lreg, int16_t id) {
	uint16_t ctrl[10];

	switch (id) {
	case 31: { /* osprint() */
		uint8_t *pb = &image[regs[lreg] & 0xFFFF]; /* get addr parmblock */
		char *str = &image[pb[0] << 8 | pb[1]]; /* get addr string */

		fputs(str, stdout);
		break;
	}
	case 40: { /* fread() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];
		ctrl[3] = pb[6] << 8 | pb[7];
		char *addr = &image[ctrl[0]];

		if (ctrl[3] < 0 || ctrl[3] >= FILEMAX || !handles[ctrl[3]])
			regs[lreg] = -1;
		else
			regs[lreg] = fread(addr, ctrl[1] & 0xffff, ctrl[2] & 0xffff, handles[ctrl[3]]);
		break;
	}
	case 41: { /* fwrite() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];
		ctrl[3] = pb[6] << 8 | pb[7];
		char *addr = &image[ctrl[0]];

		if (ctrl[3] < 0 || ctrl[3] >= FILEMAX || !handles[ctrl[3]])
			regs[lreg] = -1;
		else
			regs[lreg] = fwrite(addr, ctrl[1] & 0xffff, ctrl[2] & 0xffff, handles[ctrl[3]]);
		break;
	}
	case 42: { /* fopen() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
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
		regs[lreg] = (handles[hdl] == NULL) ? 0 : hdl;
		break;
	}
	case 43: { /* fclose() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
			regs[lreg] = -1;
		else {
			regs[lreg] = fclose(handles[ctrl[0]]);

			handles[ctrl[0]] = NULL; /* release */
		}
		break;
	}
	case 44: { /* fseek() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		ctrl[2] = pb[4] << 8 | pb[5];

		long ofs = ctrl[1];

		/* ofs is unsigned for SEEK_SET and signed otherwise */
		if (ctrl[2] == SEEK_SET)
			ofs &= 0xffff;
		else
			ofs |= -(ofs & (1 << SBIT));

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
			regs[lreg] = -1;
		else
			regs[lreg] = fseek(handles[ctrl[0]], ofs, ctrl[2]);
		break;
	}
	case 45: { /* unlink() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		char *name = &image[pb[0] << 8 | pb[1]]; /* get addr string */

		regs[lreg] = unlink(name);
		break;
	}
	case 46: { /* rename() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];
		ctrl[1] = pb[2] << 8 | pb[3];
		char *oldname = &image[ctrl[0]]; /* get addr string */
		char *newname = &image[ctrl[1]]; /* get addr string */

		regs[lreg] = rename(oldname, newname);
		break;
	}
	case 47: { /* ftell() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
		ctrl[0] = pb[0] << 8 | pb[1];

		if (ctrl[0] < 0 || ctrl[0] >= FILEMAX || !handles[ctrl[0]])
			regs[lreg] = -1;
		else
			regs[lreg] = ftell(handles[ctrl[0]]);
		break;
	}
	case 90: /* OSINFO */
		switch (regs[0]) {
		case 0x32: { /* Get commandline */
			char **cpp;

			uint8_t *pb = &image[regs[lreg] & 0xFFFF]; /* get addr parmblock */
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
			disp_dump(pc);
			shutdown(ctrl[0]);
			break;
		}
		break;
	case 99: { /* exit() */
		uint8_t *pb = &image[regs[lreg] & 0xffff]; /* get addr parmblock */
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
		disp_dump(pc);
		shutdown(1);
	}
}

void run(uint16_t inisp) {

	uint16_t pc;
	int16_t lval;
	int16_t rval;
	uint16_t opc;
	char *cp;
	int i;
	register int lreg;
	register int16_t ea;


	/* initialize */
	pc = 0;
	for (i = 0; i < REGMAX; i++)
		regs[i] = 0;
	regs[REG_SP] = inisp;
	while (1) {
		if (monitor) {
			disp_reg(pc);
			disp_opc(pc);
		}
		if (!lowestSP || (regs[REG_SP] & 0xffff) < lowestSP) {
			lowestSP = regs[REG_SP];
			if (monitor)
				printf("lowestSP=%04x\n", lowestSP);
		}

		// RISC instructions
		// 32<-----------------------------------------------------<0
		// <immlo.8> <immhi.8> <eareg>.4 <lreg.4> <opcode.6> <size.2>

		opc = image[pc++];
		lreg = image[pc] & 0xF;
		ea = regs[(image[pc++] >> 4) & 0xF];
		ea += image[pc++] << 8;
		ea += image[pc++] & 0xFF;

		if (opc == OPC_ST + 1) {
			cp = &image[ea & 0xffff];
			lval = regs[lreg];
			cp[0] = lval;
		} else if (opc == OPC_ST + 2) {
			cp = &image[ea & 0xffff];
			lval = regs[lreg];
			cp[0] = lval >> 8;
			cp[1] = lval;
		} else {
			if ((opc & 3) == 1) {
				cp = &image[ea & 0xffff];
				ea = cp[0];
			} else if ((opc & 3) == 2) {
				cp = &image[ea & 0xffff];
				ea = (cp[0] << 8) + (cp[1] & 0xFF);
			}

			switch (opc & ~3) {
			case OPC_SVC:
				/* process */
				do_svc(pc - 3, lreg, ea);
				break;
			case OPC_MUL:
				regs[lreg] *= ea;
				break;
			case OPC_DIV:
				regs[lreg] /= ea;
				break;
			case OPC_MOD:
				regs[lreg] %= ea;
				break;
			case OPC_ADD:
				regs[lreg] += ea;
				break;
			case OPC_SUB:
				regs[lreg] -= ea;
				break;
			case OPC_LSL:
				regs[lreg] <<= ea;
				break;
			case OPC_LSR:
				regs[lreg] >>= ea;
				break;
			case OPC_AND:
				regs[lreg] &= ea;
				break;
			case OPC_XOR:
				regs[lreg] ^= ea;
				break;
			case OPC_OR:
				regs[lreg] |= ea;
				break;
			case OPC_SGT:
				regs[lreg] = regs[lreg] > ea;
				break;
			case OPC_SLT:
				regs[lreg] = regs[lreg] < ea;
				break;
			case OPC_LD:
				regs[lreg] = ea;
				break;
			case OPC_NEG:
				regs[lreg] = -ea;
				break;
			case OPC_NOT:
				regs[lreg] = ~ea;
				break;
			case OPC_JZ:
				if (regs[lreg] == 0)
					pc = ea;
				break;
			case OPC_JNZ:
				if (regs[lreg] != 0)
					pc = ea;
				break;
			case OPC_JSB:
				/* save old PC */
				regs[lreg] -= BPW;
				cp = &image[regs[lreg] & 0xffff]; // get EA of -(lreg)
				cp[0] = pc >> 8;
				cp[1] = pc;
				/* update PC */
				pc = ea & 0xffff;
				break;
			case OPC_RSB:
				cp = &image[regs[lreg] & 0xffff]; // get EA of (lreg)+
				regs[lreg] += BPW;
				pc = ((cp[0] << 8) + (cp[1] & 0xFF)) & 0xffff;
				break;
			case OPC_PUSH:
				regs[lreg] -= BPW;
				cp = &image[regs[lreg] & 0xffff]; // get EA of -(lreg)
				cp[0] = ea >> 8;
				cp[1] = ea;
				break;
			case OPC_PSHR:
				/* push regs */
				for (i = 0; i < REGMAX; i++) {
					if (ea & 0x0001) {
						regs[lreg] -= BPW;
						cp = &image[regs[lreg] & 0xffff]; // get EA of -(lreg)
						lval = regs[i];
						cp[0] = lval >> 8;
						cp[1] = lval;
					}
					ea >>= 1;
				}
				break;
			case OPC_POPR:
				/* push regs */
				for (i = REGMAX - 1; i >= 0; i--) {
					if (ea & 0x8000) {
						cp = &image[regs[lreg] & 0xffff]; // get EA of (lreg)+
						regs[lreg] += BPW;
						regs[i] = (cp[0] << 8) + (cp[1] & 0xFF);
					}
					ea <<= 1;
				}
				break;
			default:
				pc -= 3;
				printf("encountered unimplemented opcode\n");
				disp_opc(pc - 1);
				disp_dump(pc - 1);
				if (verbose)
					printf("lowestSP=%04x\n", lowestSP);
				shutdown(1);
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
