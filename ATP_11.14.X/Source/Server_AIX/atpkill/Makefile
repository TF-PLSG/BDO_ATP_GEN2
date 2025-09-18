#XIPCROOT=/local/xipc
BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix/
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

atpkill: killmain.o
	$(CC) -o $(BINDIR)/atpkill \
	killmain.o \
	$(LFLAGS) -lequit -lpteipc -lpte \
	-lxcom -lpthread \
	-lm -lnsl

killmain.o: killmain.c
	$(CC) $(CFLAGS) killmain.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/atpkill
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

