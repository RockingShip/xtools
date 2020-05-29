puts(register char *s) {
	register int ret;

	ret = osprint(s);
	osprint("\n");
	return ret + 1;
}
