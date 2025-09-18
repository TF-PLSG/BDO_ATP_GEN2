BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
# "-x c" option is not supported by xlC, hence removed. Add for gcc if
# required. - Nirbhay
# also removed :-
#	-Wno-error=implicit-function-declaration
CFLAGS = -w \
	 -c -g -q64 -qcpluscmt -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc 

ncjcb: jcbmain.o jcbtran.o jcbutil.o echo_timer.o jcb0302.o jcb_Base64Decoder.o
	$(CC) -o $(BINDIR)/ncjcb \
	jcbmain.o jcbtran.o jcbutil.o echo_timer.o jcb0302.o jcb_Base64Decoder.o\
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread
	cp $(BINDIR)/ncjcb $(BINDIR)/ncjcb3

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
	
jcb_Base64Decoder.o: jcb_Base64Decoder.c
	$(CC) $(CFLAGS) jcb_Base64Decoder.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/ncjcb
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

