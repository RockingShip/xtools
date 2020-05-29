char *p, *q, *r, s[50000];
int i,j,k,l;

main() {

	for (r = s, i = 10000; i--; r++)
		for (j = i + 3211, k = 4; k--; *r++ = 48 + j % 10, j /= 10);
	for (; puts((k = r - s) ? q = r - 5 : "?"), k && k - 5;)
		for (scanf("%d", &k), p = s; p - r;) {
			for (i = j = 0; j - 16; p[j % 4] |= l = !(p[j % 4] - q[j / 4]) ? i++, 64 : 0, j |= l / 17, j++);
			for (j = 0; j - 5; i += !((*p++ &= 63) - q[j++]) * 9);
			for (; k - i + 9 && j--; *--p = *--r);
		}

}