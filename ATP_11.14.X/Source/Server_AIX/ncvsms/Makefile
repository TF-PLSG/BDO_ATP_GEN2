BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncvsms: vsmsmain.o echo_timer.o  vsmstran.o vsmsutil.o
	$(CC) -o $(BINDIR)/ncvsms \
	vsmsmain.o echo_timer.o  vsmstran.o vsmsutil.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

vsmsmain.o: vsmsmain.c
	$(CC) $(CFLAGS) vsmsmain.c

echo_timer.o: echo_timer.c
	$(CC) $(CFLAGS) echo_timer.c

vsmstran.o: vsmstran.c
	$(CC) $(CFLAGS) vsmstran.c

vsmsutil.o: vsmsutil.c
	$(CC) $(CFLAGS) vsmsutil.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncvsms
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

