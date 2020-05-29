/*
 * return 'true' if c is a graphic character
 * (33-126)
 */
isgraph(register int c) {
	return (c >= 33 && c <= 126);
}

