/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,			// Bytes per word
	SBIT = 15,			// Bit number of signed bit
};

fput0x(int n, int width, char *hdl) {
	// sign extend for 16/64 bit compatibility
	n |= -(n & (1 << SBIT));

	char numbuf[16], *p;
	p = numbuf + 16;

	// terminator
	*--p = 0;

	do {
		*--p = "0123456789abcdef"[n & 0xf];
		n >>= 4;
		// clear upper nibble to keey unsigned
		n &= ~-(1 << SBIT - 3);
		--width;
	} while (n);

	// leading zeros
	while (width > 0) {
		*--p = '0';
		--width;
	}

	fputs(p, hdl);
}
