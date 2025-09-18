BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

nceqit: nceqmain.o nceqtran.o ncequtil.o charcode.o nceq0800.o nceq_bitfields.o nceq_currency.o nceq_database.o nceq_memory.o nceq_security.o nceq_stats.o nceq_timer.o nceq_validation.o
	$(CC) -o $(BINDIR)/nceqit \
	nceqmain.o nceqtran.o ncequtil.o charcode.o nceq0800.o nceq_bitfields.o nceq_currency.o nceq_database.o nceq_memory.o nceq_security.o nceq_stats.o nceq_timer.o nceq_validation.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/nceqit $(BINDIR)/neqit2

nceqmain.o: nceqmain.c
	$(CC) $(CFLAGS) nceqmain.c

nceqtran.o: nceqtran.c
	$(CC) $(CFLAGS) nceqtran.c

ncequtil.o: ncequtil.c
	$(CC) $(CFLAGS) ncequtil.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

nceq0800.o: nceq0800.c
	$(CC) $(CFLAGS) nceq0800.c

nceq_bitfields.o: nceq_bitfields.c
	$(CC) $(CFLAGS) nceq_bitfields.c

nceq_currency.o: nceq_currency.c
	$(CC) $(CFLAGS) nceq_currency.c

nceq_database.o: nceq_database.c
	$(CC) $(CFLAGS) nceq_database.c

nceq_memory.o: nceq_memory.c
	$(CC) $(CFLAGS) nceq_memory.c

nceq_security.o: nceq_security.c
	$(CC) $(CFLAGS) nceq_security.c

nceq_stats.o: nceq_stats.c
	$(CC) $(CFLAGS) nceq_stats.c

nceq_timer.o: nceq_timer.c
	$(CC) $(CFLAGS) nceq_timer.c

nceq_validation.o: nceq_validation.c
	$(CC) $(CFLAGS) nceq_validation.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/nceqit
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

