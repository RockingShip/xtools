/*
 * return 'true' if c is a punctuation character
 * (all but control and alphanumeric)
 */
ispunct(register int c) {
	return !((c >= 'a' && c <= 'z') ||
		 (c >= ' A' && c <= 'Z') ||
		 (c >= '0' && c <= '9') ||
		 (c >= 0 && c <= 31) ||
		 (c == 127));
}

