BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncdci2: ncdci2_main.o ncdci2_tran.o ncdci2_util.o charcode.o ncdci2_0800.o ncdci2_bitfields.o ncdci2_currency.o ncdci2_database.o ncdci2_memory.o ncdci2_security.o ncdci2_stats.o ncdci2_timer.o ncdci2_validation.o ncdci2_1304.o
	$(CC) -o $(BINDIR)/ncdci2  \
	ncdci2_main.o ncdci2_tran.o ncdci2_util.o charcode.o ncdci2_0800.o ncdci2_bitfields.o ncdci2_currency.o ncdci2_database.o ncdci2_memory.o ncdci2_security.o ncdci2_stats.o ncdci2_timer.o ncdci2_validation.o ncdci2_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

ncdci2_main.o: ncdci2_main.c
	$(CC) $(CFLAGS) ncdci2_main.c

ncdci2_tran.o: ncdci2_tran.c
	$(CC) $(CFLAGS) ncdci2_tran.c

ncdci2_util.o: ncdci2_util.c
	$(CC) $(CFLAGS) ncdci2_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

ncdci2_0800.o: ncdci2_0800.c
	$(CC) $(CFLAGS) ncdci2_0800.c

ncdci2_bitfields.o: ncdci2_bitfields.c
	$(CC) $(CFLAGS) ncdci2_bitfields.c

ncdci2_currency.o: ncdci2_currency.c
	$(CC) $(CFLAGS) ncdci2_currency.c

ncdci2_database.o: ncdci2_database.c
	$(CC) $(CFLAGS) ncdci2_database.c

ncdci2_memory.o: ncdci2_memory.c
	$(CC) $(CFLAGS) ncdci2_memory.c

ncdci2_security.o: ncdci2_security.c
	$(CC) $(CFLAGS) ncdci2_security.c

ncdci2_stats.o: ncdci2_stats.c
	$(CC) $(CFLAGS) ncdci2_stats.c

ncdci2_timer.o: ncdci2_timer.c
	$(CC) $(CFLAGS) ncdci2_timer.c

ncdci2_validation.o: ncdci2_validation.c
	$(CC) $(CFLAGS) ncdci2_validation.c

ncdci2_1304.o: ncdci2_1304.c
	$(CC) $(CFLAGS) ncdci2_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/ncdci2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

