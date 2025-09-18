BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncdci: ncdci_main.o ncdci_tran.o ncdci_util.o charcode.o ncdci_0800.o ncdci_bitfields.o ncdci_currency.o ncdci_database.o ncdci_memory.o ncdci_security.o ncdci_stats.o ncdci_timer.o ncdci_validation.o ncdci_1304.o
	$(CC) -o $(BINDIR)/ncdci  \
	ncdci_main.o ncdci_tran.o ncdci_util.o charcode.o ncdci_0800.o ncdci_bitfields.o ncdci_currency.o ncdci_database.o ncdci_memory.o ncdci_security.o ncdci_stats.o ncdci_timer.o ncdci_validation.o ncdci_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/ncdci $(BINDIR)/ncdci3

ncdci_main.o: ncdci_main.c
	$(CC) $(CFLAGS) ncdci_main.c

ncdci_tran.o: ncdci_tran.c
	$(CC) $(CFLAGS) ncdci_tran.c

ncdci_util.o: ncdci_util.c
	$(CC) $(CFLAGS) ncdci_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

ncdci_0800.o: ncdci_0800.c
	$(CC) $(CFLAGS) ncdci_0800.c

ncdci_bitfields.o: ncdci_bitfields.c
	$(CC) $(CFLAGS) ncdci_bitfields.c

ncdci_currency.o: ncdci_currency.c
	$(CC) $(CFLAGS) ncdci_currency.c

ncdci_database.o: ncdci_database.c
	$(CC) $(CFLAGS) ncdci_database.c

ncdci_memory.o: ncdci_memory.c
	$(CC) $(CFLAGS) ncdci_memory.c

ncdci_security.o: ncdci_security.c
	$(CC) $(CFLAGS) ncdci_security.c

ncdci_stats.o: ncdci_stats.c
	$(CC) $(CFLAGS) ncdci_stats.c

ncdci_timer.o: ncdci_timer.c
	$(CC) $(CFLAGS) ncdci_timer.c

ncdci_validation.o: ncdci_validation.c
	$(CC) $(CFLAGS) ncdci_validation.c

ncdci_1304.o: ncdci_1304.c
	$(CC) $(CFLAGS) ncdci_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/ncdci
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

