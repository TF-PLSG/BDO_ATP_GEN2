BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

ncamex: amexmain.o amextran.o amexutil.o amex1304.o echo_timer.o
	$(CC) -o $(BINDIR)/ncpos amexmain.o amextran.o amexutil.o amex1304.o echo_timer.o $(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte -lxcom -lm -lnsl -lpthread
	  cp $(BINDIR)/ncpos $(BINDIR)/ncatm
	  cp $(BINDIR)/ncpos $(BINDIR)/ncatm2
	  cp $(BINDIR)/ncpos $(BINDIR)/ncpos2

amexmain.o: amexmain.c
	$(CC) $(CFLAGS) amexmain.c

amextran.o: amextran.c
	$(CC) $(CFLAGS) amextran.c

amexutil.o: amexutil.c
	$(CC) $(CFLAGS) amexutil.c

amex1304.o: amex1304.c
	$(CC) $(CFLAGS) amex1304.c

echo_timer.o: echo_timer.c
	$(CC) $(CFLAGS) echo_timer.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncpos $(BINDIR)/ncatm
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

