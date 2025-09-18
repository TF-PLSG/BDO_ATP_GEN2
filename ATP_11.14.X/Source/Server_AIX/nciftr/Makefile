BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

nciftr: nciftr_main.o nciftr_tran.o nciftr_util.o charcode.o nciftr_0800.o nciftr_bitfields.o nciftr_currency.o nciftr_database.o nciftr_memory.o nciftr_security.o nciftr_stats.o nciftr_timer.o nciftr_validation.o nciftr_1304.o
	$(CC) -o $(BINDIR)/nciftr  \
	nciftr_main.o nciftr_tran.o nciftr_util.o charcode.o nciftr_0800.o nciftr_bitfields.o nciftr_currency.o nciftr_database.o nciftr_memory.o nciftr_security.o nciftr_stats.o nciftr_timer.o nciftr_validation.o nciftr_1304.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

nciftr_main.o: nciftr_main.c
	$(CC) $(CFLAGS) nciftr_main.c

nciftr_tran.o: nciftr_tran.c
	$(CC) $(CFLAGS) nciftr_tran.c

nciftr_util.o: nciftr_util.c
	$(CC) $(CFLAGS) nciftr_util.c

charcode.o: charcode.c
	$(CC) $(CFLAGS) charcode.c

nciftr_0800.o: nciftr_0800.c
	$(CC) $(CFLAGS) nciftr_0800.c

nciftr_bitfields.o: nciftr_bitfields.c
	$(CC) $(CFLAGS) nciftr_bitfields.c

nciftr_currency.o: nciftr_currency.c
	$(CC) $(CFLAGS) nciftr_currency.c

nciftr_database.o: nciftr_database.c
	$(CC) $(CFLAGS) nciftr_database.c

nciftr_memory.o: nciftr_memory.c
	$(CC) $(CFLAGS) nciftr_memory.c

nciftr_security.o: nciftr_security.c
	$(CC) $(CFLAGS) nciftr_security.c

nciftr_stats.o: nciftr_stats.c
	$(CC) $(CFLAGS) nciftr_stats.c

nciftr_timer.o: nciftr_timer.c
	$(CC) $(CFLAGS) nciftr_timer.c

nciftr_validation.o: nciftr_validation.c
	$(CC) $(CFLAGS) nciftr_validation.c

nciftr_1304.o: nciftr_1304.c
	$(CC) $(CFLAGS) nciftr_1304.c


clean:
	-@rm -f *.o *.m* $(BINDIR)/nciftr
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

