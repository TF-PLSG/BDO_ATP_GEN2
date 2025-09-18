BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -w -q64 -qcpluscmt -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncjcb: jcbmain.o jcbtran.o jcbutil.o echo_timer.o jcb0302.o
	$(CC) -o $(BINDIR)/ncjcb2 \
	jcbmain.o jcbtran.o jcbutil.o echo_timer.o jcb0302.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

jcbmain.o: jcbmain.c
	$(CC) $(CFLAGS) jcbmain.c

jcbtran.o: jcbtran.c
	$(CC) $(CFLAGS) jcbtran.c

jcbutil.o: jcbutil.c
	$(CC) $(CFLAGS) jcbutil.c

echo_timer.o: echo_timer.c
	$(CC) $(CFLAGS) echo_timer.c

jcb0302.o: jcb0302.c
	$(CC) $(CFLAGS) jcb0302.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncjcb2
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

