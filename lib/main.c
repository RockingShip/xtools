int stdin;
int stdout;
int stderr;

osprint(char *str) {
	asm("lda r1,4(r14)");
	asm("svc 31");
}

fread(char *buf, int siz, int cnt, int hdl) {
	int ctrl[4];

	ctrl[0] = buf;
	ctrl[1] = siz;
	ctrl[2] = cnt;
	ctrl[3] = hdl;

	asm("lda r1,0(r15)");
	return asm("svc 40");
}

fwrite(int buf, int siz, int cnt, int hdl) {
	int ctrl[4];

	ctrl[0] = buf;
	ctrl[1] = siz;
	ctrl[2] = cnt;
	ctrl[3] = hdl;

	asm("lda r1,0(r15)");
	return asm("svc 41");
}

fopen(char *name, char *mode) {
	int ctrl[2];

	ctrl[0] = name;
	ctrl[1] = mode;

	asm("lda r1,0(r15)");
	return asm("svc 42");
}

fclose(int hdl) {
	int ctrl[1];

	ctrl[0] = hdl;

	asm("lda r1,0(r15)");
	return asm("svc 43");
}

fseek(int hdl, int pos, int whence) {
	int ctrl[3];

	ctrl[0] = hdl;
	ctrl[1] = pos;
	ctrl[2] = whence;

	asm("lda r1,0(r15)");
	return asm("svc 44");
}

ftell(int hdl) {
	int ctrl[1];

	ctrl[0] = hdl;

	asm("lda r1,0(r15)");
	return asm("svc 47");
}

unlink(char *name) {
	int ctrl[1];

	ctrl[0] = name;

	asm("lda r1,0(r15)");
	return asm("svc 45");
}

rename(char *old, char *new) {
	int ctrl[2];

	ctrl[0] = old;
	ctrl[1] = new;

	asm("lda r1,0(r15)");
	return asm("svc 46");
}

exit(int code) {
	int ctrl[1];

	ctrl[0] = code;

	asm("lda r1,0(r15)");
	asm("svc 99");
}

__START(int argc, int *argv)
{
	/* Load the special purpose registers */
	/* r15 REG_SP  initial sp */
	/* r14 REG_AP  frame pointer*/
	/* r13 REG_BPW constant BPW */
	/* r12 REG_1   constant 1 */
	/* r0  REG_0   constant 0 */

	asm("lda r13,2");
	asm("lda r12,1");
	asm("lda r0,0");

	stdin = 0;
	stdout = 1;
	stderr = 2;

	main (argc, argv);
	exit (0);
}
