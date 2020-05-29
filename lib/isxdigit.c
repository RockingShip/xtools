/*
 * return 'true' if c is a hexadecimal digit
 * (0-9, A-F, or a-f)
 */
isxdigit(register int c) {
	return ((c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F') ||
		(c >= '0' && c <= '9'));
}
