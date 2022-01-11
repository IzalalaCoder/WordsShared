dist:
	$(MAKE) -C main clean
	tar -zcf "$(CURDIR).tar.gz" hashtable/* holdall/* main/* makefile
