main()
{
	osprint("testing oscalls\n");

	int hdl, ret;
	char buf[4];

	/* create a test file */
osprint("#1\n");
	hdl = fopen("ostest.tmp", "w");
	if (hdl != 6)
		osprint("fopen(w) failed\n");

	/* write some random text */
osprint("#2\n");
	ret = fwrite("abcdefghijklmnopqrstuvwxyz\n", 1, 27, hdl);
	if (ret != 27)
		osprint("fwrite() failed\n");
	
	/* close */
osprint("#3\n");
	ret = fclose(hdl);
	if (ret != 0)
		osprint("fclose() failed\n");
	/* rename file */
osprint("#4\n");
	ret = rename("ostest.tmp", "ostest.pmt");
	if (ret != 0)
		osprint("rename() failed\n");

	/* open test file */
osprint("#5\n");
	hdl = fopen("ostest.pmt", "r");
	if (hdl != 6)
		osprint("fopen(r) #1 failed\n");

        /* position to some nice sub string */ 
osprint("#6\n");
	fseek(hdl, 23, 0);
	ret = ftell(hdl);
	if (ret != 23)
		osprint("fseek() #1 failed\n");
	
	/* read 3 letters */
osprint("#7\n");
	ret = fread(buf, 3, 1, hdl);
	if (ret != 1)
		osprint("fread() #1 failed\n");

	/* compare */
osprint("#8\n");
        if (buf[0] != 'x' || buf[1] != 'y' || buf[2] != 'z')
		osprint("compare #1 failed\n");

        /* position relative to a different spot (using negative constant) */
osprint("#9\n");
	fseek(hdl, -19, 1);
	ret = ftell(hdl);
	if (ret != 7) /* +23(start) +3(read) -19(seekback) */
		osprint("fseek() #2 failed\n");
	
	/* read 3 letters */
osprint("#10\n");
	ret = fread(buf, 1, 3, hdl);
	if (ret != 3)
		osprint("fread() #2 failed\n");

	/* compare */
osprint("#11\n");
        if (buf[0] != 'h' || buf[1] != 'i' || buf[2] != 'j')
		osprint("compare #2 failed\n");

	/* read 3 letters */
osprint("#12\n");
	ret = fread(buf, 3, 1, hdl);
	if (ret != 1)
		osprint("fread() #2 failed\n");

	/* close */
osprint("#13\n");
	ret = fclose(hdl);
	if (ret != 0)
		osprint("fclose() failed\n");

	/* delete file */
osprint("#14\n");
	ret = unlink("ostest.pmt");
	if (ret != 0)
		osprint("rename() failed\n");

	/* final failing reopen */
osprint("#15\n");
	ret = fopen("ostest.pmt", "r");
	if (ret != 0)
		osprint("open deleted false success\n");

	osprint("done\n");
	exit(0);
}
