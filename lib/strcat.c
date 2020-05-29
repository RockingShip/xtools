/*
 * concatenate t to end of s
 * s must be large enough
 */
strcat(register char *s, register char *t) {
	register char *d;

	d = s;
	--s;
	while (*++s);
	while (*s++ = *t++);
	return d;
}

