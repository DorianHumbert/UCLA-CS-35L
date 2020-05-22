randmain: randlibsw.so randlibhw.so
	$(CC) $(CFLAGS) -c randcpuid.c -o randcpuid.o
	$(CC) $(CFLAGS) -c randmain.c -o randmain.o
	$(CC) $(CFLAGS) -ldl -Wl,-rpath=$(PWD) randcpuid.o randmain.o -o $@

randlibsw.so: randlibsw.c
	$(CC) $(CFLAGS) -fPIC -c randlibsw.c -o randlibsw.o
	$(CC) $(CFLAGS) -shared randlibsw.o -o $@

randlibhw.so: randlibhw.c
	$(CC) $(CFLAGS) -fPIC -c randlibhw.c -o randlibhw.o
	$(CC) $(CFLAGS) -shared randlibhw.o -o $@ 
