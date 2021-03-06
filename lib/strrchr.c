/*
 * strrchr(s,c) - Search s for rightmost occurrance of c.
 *  s      = Pointer to string to be searched.
 * c      = Character to search for.
 * Returns pointer to rightmost c or NULL.
 */
strrchr(register char *s, register char *c) {
	register char *ptr;

	ptr = 0;
	while (*s) {
		if (*s == c)
			ptr = s;
		++s;
	}
	return ptr;
}

