BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

nccup2: nccup2_main.o nccup2_tran.o nccup2_util.o charcode.o nccup2_0800.o nccup2_bitfields.o nccup2_currency.o nccup2_database.o nccup2_memory.o nccup2_security.o nccup2_stats.o nccup2_timer.o nccup2_validation.o nccup2_1304.o
	$(CC) -o $(BINDIR)/nccup2  \
	nccup2_main.o nccup2_tran.o nccup2_util.o charcode.o nccup2_0800.o nccup2_bitfields.o nccup2_currency.o nccup2_database.o nccup2_memory.o nccup2_security.o nccup2_stats.o nccup2_timer.o nccup2_validation.o nccup2_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

nccup2_main.o: nccup2_main.c
	$(CC) $(CFLAGS) nccup2_main.c

nccup2_tran.o: nccup2_tran.c
	$(CC) $(CFLAGS) nccup2_tran.c

nccup2_util.o: nccup2_util.c
	$(CC) $(CFLAGS) nccup2_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

nccup2_0800.o: nccup2_0800.c
	$(CC) $(CFLAGS) nccup2_0800.c

nccup2_bitfields.o: nccup2_bitfields.c
	$(CC) $(CFLAGS) nccup2_bitfields.c

nccup2_currency.o: nccup2_currency.c
	$(CC) $(CFLAGS) nccup2_currency.c

nccup2_database.o: nccup2_database.c
	$(CC) $(CFLAGS) nccup2_database.c

nccup2_memory.o: nccup2_memory.c
	$(CC) $(CFLAGS) nccup2_memory.c

nccup2_security.o: nccup2_security.c
	$(CC) $(CFLAGS) nccup2_security.c

nccup2_stats.o: nccup2_stats.c
	$(CC) $(CFLAGS) nccup2_stats.c

nccup2_timer.o: nccup2_timer.c
	$(CC) $(CFLAGS) nccup2_timer.c

nccup2_validation.o: nccup2_validation.c
	$(CC) $(CFLAGS) nccup2_validation.c

nccup2_1304.o: nccup2_1304.c
	$(CC) $(CFLAGS) nccup2_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/nccup2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

