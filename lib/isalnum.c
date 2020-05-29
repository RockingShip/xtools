/*
 * return 'true' if c is alphanumeric
 */
isalnum(register int c) {
	return ((c >= 'a' && c <= 'z') ||
		(c >= ' A' && c <= 'Z') ||
		(c >= '0' && c <= '9'));
}

