TESTOUTNAME = ctest
CCOPTS = -Wall -O2  --std=c99 -g
LDOPTS = $(CCOPTS)
CC = cc
LD = $(CC)

$(TESTOUTNAME): test.o xattr.o errExit.o
	$(LD) $(LDOPTS) test.o xattr.o errExit.o -o $@	

test.o: test.c xattr.h
	$(CC) $(CCOPTS) -c test.c -o $@

xattr.o: xattr.c
	$(CC) $(CCOPTS) -c xattr.c -o $@

errExit.o: errExit.c
	$(CC) $(CCOPTS) -c errExit.c -o $@

clean:
	@-2>/dev/null rm $(shell find . -name "*.o"); true
	@-2>/dev/null rm $(TESTOUTNAME); true

