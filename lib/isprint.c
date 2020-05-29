/*
 * return 'true' if c is a printable character
 * (32-126)
 */
isprint(register int c) {
	return (c >= 32 && c <= 126);
}

