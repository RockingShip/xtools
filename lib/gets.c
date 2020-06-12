extern char *stdin;

gets(char *s0) {
	register char *s;
	s = s0;

	while (1) {
		if (fread(s, 1, 1, stdin) != 1) {
			*s = 0;
			return s0;
		}

		if (*s++ == '\n')
			break;
	}

	*s = 0;
	return s0;
}
