/*
 * return <0,   0,  >0 a_ording to
 *        s<t, s=t, s>t
 */
strcmp(register char *s, register char *t) {
	while (*s == *t) {
		if (*s == 0)
			return 0;
		++s;
		++t;
	}
	return *s - *t;
}

