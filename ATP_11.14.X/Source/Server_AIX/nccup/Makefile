BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

nccup: nccup_main.o nccup_tran.o nccup_util.o charcode.o nccup_0800.o nccup_bitfields.o nccup_currency.o nccup_database.o nccup_memory.o nccup_security.o nccup_stats.o nccup_timer.o nccup_validation.o nccup_1304.o nccup_Base64Decoder.o
	$(CC) -o $(BINDIR)/nccup  \
	nccup_main.o nccup_tran.o nccup_util.o charcode.o nccup_0800.o nccup_bitfields.o nccup_currency.o nccup_database.o nccup_memory.o nccup_security.o nccup_stats.o nccup_timer.o nccup_validation.o nccup_1304.o nccup_Base64Decoder.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/nccup $(BINDIR)/nccup3

nccup_main.o: nccup_main.c
	$(CC) $(CFLAGS) nccup_main.c

nccup_tran.o: nccup_tran.c
	$(CC) $(CFLAGS) nccup_tran.c

nccup_util.o: nccup_util.c
	$(CC) $(CFLAGS) nccup_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

nccup_0800.o: nccup_0800.c
	$(CC) $(CFLAGS) nccup_0800.c

nccup_bitfields.o: nccup_bitfields.c
	$(CC) $(CFLAGS) nccup_bitfields.c

nccup_currency.o: nccup_currency.c
	$(CC) $(CFLAGS) nccup_currency.c

nccup_database.o: nccup_database.c
	$(CC) $(CFLAGS) nccup_database.c

nccup_memory.o: nccup_memory.c
	$(CC) $(CFLAGS) nccup_memory.c

nccup_security.o: nccup_security.c
	$(CC) $(CFLAGS) nccup_security.c

nccup_stats.o: nccup_stats.c
	$(CC) $(CFLAGS) nccup_stats.c

nccup_timer.o: nccup_timer.c
	$(CC) $(CFLAGS) nccup_timer.c

nccup_validation.o: nccup_validation.c
	$(CC) $(CFLAGS) nccup_validation.c

nccup_1304.o: nccup_1304.c
	$(CC) $(CFLAGS) nccup_1304.c

nccup_Base64Decoder.o: nccup_Base64Decoder.c
	$(CC) $(CFLAGS) nccup_Base64Decoder.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/nccup
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

