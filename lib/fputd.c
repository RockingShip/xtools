/*
 * Machine dependent parmeters
 */

enum {
	BPW = 2,			// Bytes per word
	SBIT = 15,			// Bit number of signed bit
};

fputd(int n, char *hdl)
{
	// sign extend for 16/64 bit compatibility
	n |= -(n & (1 << SBIT));

	// note: 0x8000 equals -32768 but also +32768 (i.e. for 16bits ints)
	char numbuf[16], *p;
	p = numbuf + 16;

	// terminator
	*--p = 0;

	if (n >= 0) {
		do {
			*--p = (n % 10) + '0';
			n /= 10;
		} while (n);

	} else {
		do {
			*--p = -(n % 10) + '0';
			n /= 10;
		} while (n);
		*--p = '-';
	}

	fputs(p, hdl);
}
