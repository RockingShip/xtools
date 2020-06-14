int stdin;
int stdout;
int stderr;

osprint(char *str) {
	int ctrl[1];
	register int *r2;

	r2 = ctrl;
	r2[0] = str;

	asm("svc.a r2,31");
	return r2;
}

fread(char *buf, int siz, int cnt, int hdl) {
	int ctrl[4];
	register int *r2;

	r2 = ctrl;
	r2[0] = buf;
	r2[1] = siz;
	r2[2] = cnt;
	r2[3] = hdl;

	asm("svc.a r2,40");
	return r2;
}

fwrite(int buf, int siz, int cnt, int hdl) {
	int ctrl[4];
	register int *r2;

	r2 = ctrl;
	r2[0] = buf;
	r2[1] = siz;
	r2[2] = cnt;
	r2[3] = hdl;

	asm("svc.a r2,41");
	return r2;
}

fopen(char *name, char *mode) {
	int ctrl[2];
	register int *r2;

	r2 = ctrl;
	r2[0] = name;
	r2[1] = mode;

	asm("svc.a r2,42");
	return r2;
}

fclose(int hdl) {
	int ctrl[1];
	register int *r2;

	r2 = ctrl;
	r2[0] = hdl;

	asm("svc.a r2,43");
	return r2;
}

fseek(int hdl, int pos, int whence) {
	int ctrl[3];
	register int *r2;

	r2 = ctrl;
	r2[0] = hdl;
	r2[1] = pos;
	r2[2] = whence;

	asm("svc.a r2,44");
	return r2;
}

ftell(int hdl) {
	int ctrl[1];
	register int *r2;

	r2 = ctrl;
	r2[0] = hdl;

	asm("svc.a r2,47");
	return r2;
}

unlink(char *name) {
	int ctrl[1];
	register int *r2;

	r2 = ctrl;
	r2[0] = name;

	asm("svc.a r2,45");
	return r2;
}

rename(char *old, char *new) {
	int ctrl[2];
	register int *r2;

	r2 = ctrl;
	r2[0] = old;
	r2[1] = new;

	asm("svc.a r2,46");
	return r2;
}

exit(int code) {
	int ctrl[1];
	register int *r2;

	r2 = ctrl;
	r2[0] = code;

	asm("svc.a r2,99");
}

__START(int argc, int *argv)
{
	stdin = 0;
	stdout = 1;
	stderr = 2;

	main (argc, argv);
	exit (0);
}
