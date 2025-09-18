BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncmcrd: mcrdmain.o mcrdtran.o mcrdutil.o mcrd0800.o ncutils.o
	$(CC) -o $(BINDIR)/ncmcr2 \
	mcrdmain.o mcrdtran.o mcrdutil.o mcrd0800.o ncutils.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

mcrdmain.o: mcrdmain.c
	$(CC) $(CFLAGS) mcrdmain.c

mcrdtran.o: mcrdtran.c
	$(CC) $(CFLAGS) mcrdtran.c

mcrdutil.o: mcrdutil.c
	$(CC) $(CFLAGS) mcrdutil.c

mcrd0800.o: mcrd0800.c
	$(CC) $(CFLAGS) mcrd0800.c

ncutils.o: ncutils.c
	$(CC) $(CFLAGS) ncutils.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncmcr2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

