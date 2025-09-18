BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

nccirs: cirsmain.o cirsrcon.o cirsrevs.o cirstran.o cirsutil.o echo_timer.o
	$(CC) -o $(BINDIR)/nccirs \
	cirsmain.o cirsrcon.o cirsrevs.o cirstran.o cirsutil.o echo_timer.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread -lm -lnsl

cirsmain.o: cirsmain.c
	$(CC) $(CFLAGS) cirsmain.c

cirsrcon.o: cirsrcon.c
	$(CC) $(CFLAGS) cirsrcon.c

cirsrevs.o: cirsrevs.c
	$(CC) $(CFLAGS) cirsrevs.c

cirstran.o: cirstran.c
	$(CC) $(CFLAGS) cirstran.c

cirsutil.o: cirsutil.c
	$(CC) $(CFLAGS) cirsutil.c

echo_timer.o: echo_timer.c
	$(CC) $(CFLAGS) echo_timer.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/nccirs
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

