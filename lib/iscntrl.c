/*
 * return 'true' if c is a control character
 * (0-31 or 127)
 */
iscntrl(register int c) {
	return ((c >= 0 && c <= 31) || (c == 127));
}

