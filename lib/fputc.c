fputc(char c, register int hdl) {
	if (fwrite(&c, 1, 1, hdl) != 1)
		return -1;

	return c;
}
