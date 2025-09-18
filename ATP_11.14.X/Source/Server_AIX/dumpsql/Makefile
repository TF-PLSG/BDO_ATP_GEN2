BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

dumpsql: dumpmain.o dumputil.o
	$(CC) -o $(BINDIR)/dumpsql \
	dumpmain.o dumputil.o \
	$(LFLAGS) -lequit -lpteipc -lpte \
	-lxcom  \
	-lm -lpthread -lnsl

dumpmain.o: dumpmain.c
	$(CC) $(CFLAGS) dumpmain.c

dumputil.o: dumputil.c
	$(CC) $(CFLAGS) dumputil.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/dumpsql
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

