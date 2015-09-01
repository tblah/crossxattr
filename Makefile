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
	$(CC) $(CCOPTS) -shared -c xattr.c -o $@

errExit.o: errExit.c
	$(CC) $(CCOPTS) -c errExit.c -o $@

clean:
	@-2>/dev/null rm $(shell find . -name "*.o"); true
	@-2>/dev/null rm $(TESTOUTNAME); true

install:
	@-2>/dev/null mkdir /usr/share/crossattr; true
	@-2>/dev/null cp xattr.o /usr/share/crossattr/xattr.o; true
	@-2>/dev/null cp xattr.h /usr/share/crossattr/xattr.h; true

uninstall:
	@-2>/dev/null rm /usr/share/crossattr/*; true
	@-2>/dev/null rmdir /usr/share/crossattr; true
