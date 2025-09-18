BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC -DBOOL=int
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

hostgen: convert.o dbs_sett.o
	$(CC) -o $(BINDIR)/hostgen \
	convert.o dbs_sett.o \
	$(LFLAGS) -lpteipc -lptecom -lpte \
	-lxcom -lm -lpthread -lnsl

convert.o: convert.c
	$(CC) $(CFLAGS) convert.c

dbs_sett.o: dbs_sett.c
	$(CC) $(CFLAGS) dbs_sett.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/hostgen
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

