/*
 * return 'true' if c is a white-space character
 */
isspace(register int c) {
	/* first check gives quick exit in most cases */
	return (c <= ' ' && (c == ' ' || (c <= 13 && c >= 9)));
}

