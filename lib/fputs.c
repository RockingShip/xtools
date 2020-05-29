fputs(register char *s, register int hdl) {
	char *s0;

	s0 = s;

	while (*s) {
		if (fwrite(s, 1, 1, hdl) != 1)
			return 0;
		s++;
	}

	return s - s0;
}
