BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

pingapp: pingmain.o
	$(CC) -o $(BINDIR)/pingapp \
	pingmain.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

pingmain.o: pingmain.c
	$(CC) $(CFLAGS) pingmain.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/pingapp
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

