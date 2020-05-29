/*
 * copy n characters from sour to dest (null padding)
 */
strncpy(register char *dest, register char *sour, register int n) {
	register char *d;

	d = dest;
	while (n-- > 0) {
		if (*d++ = *sour++)
			continue;
		while (n-- > 0)
			*d++ = 0;
	}
	return dest;
}

