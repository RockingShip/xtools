/*
 * copy t to s
 */
strcpy(register char *s, register char *t) {
	register char *d;

	d = s;
	while (*s++ = *t++);
	return d;
}

