BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

falcntl: falcontroller.o falutil.o faltimer.o
	$(CC) -o $(BINDIR)/falcntl falcontroller.o falutil.o faltimer.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte -lxcom -lm -lpthread -lnsl

falcontroller.o: falcontroller.c
	$(CC) $(CFLAGS) falcontroller.c

falutil.o: falutil.c
	$(CC) $(CFLAGS) falutil.c
	
faltimer.o: faltimer.c
	$(CC) $(CFLAGS) faltimer.c

clean:
	-@rm -f *.o $(BINDIR)/falcntl

