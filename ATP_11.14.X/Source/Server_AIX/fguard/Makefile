BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

fguard: fgrdmain.o fgrdtran.o fgrdutil.o fgrd_stats.o fgrd_memory.o fgrd_database.o
	$(CC) -o $(BINDIR)/fguard \
	fgrdmain.o fgrdtran.o fgrdutil.o fgrd_stats.o fgrd_memory.o fgrd_database.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom \
	-lm -lpthread -lnsl

fgrdmain.o: fgrdmain.c
	$(CC) $(CFLAGS) fgrdmain.c

fgrdtran.o: fgrdtran.c
	$(CC) $(CFLAGS) fgrdtran.c

fgrdutil.o: fgrdutil.c
	$(CC) $(CFLAGS) fgrdutil.c

fgrd_stats.o: fgrd_stats.c
	$(CC) $(CFLAGS) fgrd_stats.c

fgrd_memory.o: fgrd_memory.c
	$(CC) $(CFLAGS) fgrd_memory.c

fgrd_database.o: fgrd_database.c
	$(CC) $(CFLAGS) fgrd_database.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/fguard
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

