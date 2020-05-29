fgets(register char *s, register int siz, register int hdl) {
	char *s0;

	s0 = s;

	while (siz > 1) {
		if (fread(s, 1, 1, hdl) != 1)
			return (s == s0) ? 0 : s0;

		if (*s++ == '\n')
			break;
		siz--;
	}

	*s = 0;
	return s0;
}
