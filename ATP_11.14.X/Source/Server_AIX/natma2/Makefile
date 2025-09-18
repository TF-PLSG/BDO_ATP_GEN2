BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

nposa2: ncamx_main.o ncamx_tran.o ncamx_util.o charcode.o ncamx_0800.o ncamx_bitfields.o ncamx_currency.o ncamx_database.o ncamx_memory.o ncamx_security.o ncamx_stats.o ncamx_timer.o ncamx_validation.o ncamx_1304.o
	$(CC) -o $(BINDIR)/natma2  \
	ncamx_main.o ncamx_tran.o ncamx_util.o charcode.o ncamx_0800.o ncamx_bitfields.o ncamx_currency.o ncamx_database.o ncamx_memory.o ncamx_security.o ncamx_stats.o ncamx_timer.o ncamx_validation.o ncamx_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/natma2 $(BINDIR)/nposa2

ncamx_main.o: ncamx_main.c
	$(CC) $(CFLAGS) ncamx_main.c

ncamx_tran.o: ncamx_tran.c
	$(CC) $(CFLAGS) ncamx_tran.c

ncamx_util.o: ncamx_util.c
	$(CC) $(CFLAGS) ncamx_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

ncamx_0800.o: ncamx_0800.c
	$(CC) $(CFLAGS) ncamx_0800.c

ncamx_bitfields.o: ncamx_bitfields.c
	$(CC) $(CFLAGS) ncamx_bitfields.c

ncamx_currency.o: ncamx_currency.c
	$(CC) $(CFLAGS) ncamx_currency.c

ncamx_database.o: ncamx_database.c
	$(CC) $(CFLAGS) ncamx_database.c

ncamx_memory.o: ncamx_memory.c
	$(CC) $(CFLAGS) ncamx_memory.c

ncamx_security.o: ncamx_security.c
	$(CC) $(CFLAGS) ncamx_security.c

ncamx_stats.o: ncamx_stats.c
	$(CC) $(CFLAGS) ncamx_stats.c

ncamx_timer.o: ncamx_timer.c
	$(CC) $(CFLAGS) ncamx_timer.c

ncamx_validation.o: ncamx_validation.c
	$(CC) $(CFLAGS) ncamx_validation.c

ncamx_1304.o: ncamx_1304.c
	$(CC) $(CFLAGS) ncamx_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/natma2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

