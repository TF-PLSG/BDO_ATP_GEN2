BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncvis2: visamain.o echo_timer.o  visatran.o visautil.o
	$(CC) -o $(BINDIR)/ncvis2 \
	visamain.o echo_timer.o  visatran.o visautil.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

visamain.o: visamain.c
	$(CC) $(CFLAGS) visamain.c

echo_timer.o: echo_timer.c
	$(CC) $(CFLAGS) echo_timer.c

visatran.o: visatran.c
	$(CC) $(CFLAGS) visatran.c

visautil.o: visautil.c
	$(CC) $(CFLAGS) visautil.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncvis2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

