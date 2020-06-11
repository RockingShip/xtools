int stdin;
int stdout;
int stderr;

osprint(char *str) {
	asm("ld.a r1,4(r14)");
	asm("svc.a r1,31");
}

fread(char *buf, int siz, int cnt, int hdl) {
	int ctrl[4];

	ctrl[0] = buf;
	ctrl[1] = siz;
	ctrl[2] = cnt;
	ctrl[3] = hdl;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,40");
}

fwrite(int buf, int siz, int cnt, int hdl) {
	int ctrl[4];

	ctrl[0] = buf;
	ctrl[1] = siz;
	ctrl[2] = cnt;
	ctrl[3] = hdl;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,41");
}

fopen(char *name, char *mode) {
	int ctrl[2];

	ctrl[0] = name;
	ctrl[1] = mode;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,42");
}

fclose(int hdl) {
	int ctrl[1];

	ctrl[0] = hdl;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,43");
}

fseek(int hdl, int pos, int whence) {
	int ctrl[3];

	ctrl[0] = hdl;
	ctrl[1] = pos;
	ctrl[2] = whence;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,44");
}

ftell(int hdl) {
	int ctrl[1];

	ctrl[0] = hdl;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,47");
}

unlink(char *name) {
	int ctrl[1];

	ctrl[0] = name;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,45");
}

rename(char *old, char *new) {
	int ctrl[2];

	ctrl[0] = old;
	ctrl[1] = new;

	asm("ld.a r1,0(r15)");
	return asm("svc.a r1,46");
}

exit(int code) {
	int ctrl[1];

	ctrl[0] = code;

	asm("ld.a r1,0(r15)");
	asm("svc.a r1,99");
}

__START(int argc, int *argv)
{
	/* Load the special purpose registers */
	/* r15 REG_SP  initial sp */
	/* r14 REG_AP  frame pointer*/
	/* r13 REG_BPW constant BPW */
	/* r12 REG_1   constant 1 */
	/* r0  REG_0   constant 0 */

	asm("ld.a r13,2");
	asm("ld.a r12,1");
	asm("ld.a r0,0");

	stdin = 0;
	stdout = 1;
	stderr = 2;

	main (argc, argv);
	exit (0);
}
