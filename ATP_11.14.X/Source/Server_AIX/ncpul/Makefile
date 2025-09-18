BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncpul: ncpul_main.o ncpul_tran.o ncpul_util.o charcode.o ncpul_0800.o ncpul_bitfields.o ncpul_currency.o ncpul_database.o ncpul_memory.o ncpul_security.o ncpul_stats.o ncpul_timer.o ncpul_validation.o ncpul_1304.o
	$(CC) -o $(BINDIR)/ncpul  \
	ncpul_main.o ncpul_tran.o ncpul_util.o charcode.o ncpul_0800.o ncpul_bitfields.o ncpul_currency.o ncpul_database.o ncpul_memory.o ncpul_security.o ncpul_stats.o ncpul_timer.o ncpul_validation.o ncpul_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	#cp $(BINDIR)/ncpul $(BINDIR)/ncpul3

ncpul_main.o: ncpul_main.c
	$(CC) $(CFLAGS) ncpul_main.c

ncpul_tran.o: ncpul_tran.c
	$(CC) $(CFLAGS) ncpul_tran.c

ncpul_util.o: ncpul_util.c
	$(CC) $(CFLAGS) ncpul_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

ncpul_0800.o: ncpul_0800.c
	$(CC) $(CFLAGS) ncpul_0800.c

ncpul_bitfields.o: ncpul_bitfields.c
	$(CC) $(CFLAGS) ncpul_bitfields.c

ncpul_currency.o: ncpul_currency.c
	$(CC) $(CFLAGS) ncpul_currency.c

ncpul_database.o: ncpul_database.c
	$(CC) $(CFLAGS) ncpul_database.c

ncpul_memory.o: ncpul_memory.c
	$(CC) $(CFLAGS) ncpul_memory.c

ncpul_security.o: ncpul_security.c
	$(CC) $(CFLAGS) ncpul_security.c

ncpul_stats.o: ncpul_stats.c
	$(CC) $(CFLAGS) ncpul_stats.c

ncpul_timer.o: ncpul_timer.c
	$(CC) $(CFLAGS) ncpul_timer.c

ncpul_validation.o: ncpul_validation.c
	$(CC) $(CFLAGS) ncpul_validation.c

ncpul_1304.o: ncpul_1304.c
	$(CC) $(CFLAGS) ncpul_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/ncpul
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

