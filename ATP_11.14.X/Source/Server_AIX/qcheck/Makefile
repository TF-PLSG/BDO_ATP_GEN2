BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC -DBDO_SOLARIS_PORT
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

qcheck: qcheck.o 
	$(CC) -o $(BINDIR)/qcheck \
	qcheck.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/qcheck $(BINDIR)/qcheck2
qcheck.o: qcheck.c
	$(CC) $(CFLAGS) qcheck.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/qcheck
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

