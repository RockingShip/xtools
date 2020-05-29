/*
 * return length of s
 */
strlen(register char *s) {
	register char *t;

	t = s - 1;
	while (*++t);
	return t - s;
}

