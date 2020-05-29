/*
 * Really crappy implementation only intended to be used for cattle.c
 */
static char lastch;
extern int stdin;

scanf(char *fmt, int *num) {
	char ch;
	int found;
	int x;

	if (fmt[0] != '%' || fmt[1] != 'd' || fmt[2] != 0) {
		osprint("% scanf() unimplemented\n");
		exit(1);
	}

	*num = 0;
	found = 0;

	/* skip spaces */
	do {
		/* get next character */
		if (lastch) {
			ch = lastch;
			lastch = 0;
		} else {
			if (fread(&ch, 1, 1, stdin) != 1)
				return 0;
			*num = ch - '0';
		}
	} while (ch <= ' ');

	do {
		/* get next character */
		if (fread(&ch, 1, 1, stdin) != 1)
			break;

		if (ch >= '0' && ch <= '9') {
			*num = *num * 10 + ch - '0';
			found = 1;
		} else {
			lastch = ch;
			return found;
		}
	} while (1);

	return found;
}
