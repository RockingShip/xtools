/*
 * strncmp(s,t,n) - Compares two strings for at most n
 *                  characters and returns an integer
 *                  >0, =0, or <0 as s is >t, =t, or <t.
 */
strncmp(register char *s, register char *t, register int n) {
	while (n && (*s == *t)) {
		if (*s == 0)
			return 0;
		++s;
		++t;
		--n;
	}
	if (n)
		return *s - *t;
	return 0;
}

