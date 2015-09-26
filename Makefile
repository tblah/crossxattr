TESTOUTNAME = ctest
CCOPTS = -Wall -O2  --std=c99 -g
LDOPTS = $(CCOPTS)
CC = cc
LD = $(CC)
INSTALL_PATH = /usr/local

all: $(TESTOUTNAME) libxattr.a libxattr.so

$(TESTOUTNAME): test.o xattr.o errExit.o
	$(LD) $(LDOPTS) test.o xattr.o errExit.o -o $@	

test.o: test.c xattr.h
	$(CC) $(CCOPTS) -c test.c -o $@

xattr.o: xattr.c
	$(CC) $(CCOPTS) -fpic -c xattr.c -o $@

libxattr.a: xattr.o errExit.o
	ar rs $@ xattr.o errExit.o

libxattr.so: xattr.o
	$(CC) -shared -o $@ xattr.o errExit.o

errExit.o: errExit.c
	$(CC) $(CCOPTS) -fpic -c errExit.c -o $@

clean:
	@-2>/dev/null rm $(shell find . -name "*.o"); true
	@-2>/dev/null rm $(shell find . -name "*.a"); true
	@-2>/dev/null rm $(shell find . -name "*.so"); true
	@-2>/dev/null rm $(TESTOUTNAME); true

install:
	@-cp libxattr.a $(INSTALL_PATH)/lib/libxattr.a; true
	@-cp libxattr.so $(INSTALL_PATH)/lib/libxattr.so; true
	@-cp xattr.h $(INSTALL_PATH)/include/libxattr.h; true

uninstall:
	@-2>/dev/null rm $(INSTALL_PATH)/lib/libxattr.a; true
	@-2>/dev/null rm $(INSTALL_PATH)/lib/libxattr.so; true
	@-2>/dev/null rm $(INSTALL_PATH)/include/libxattr.h; true
