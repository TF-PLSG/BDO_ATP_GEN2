BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

mega: mega.o
	$(CC) -o $(BINDIR)/mega mega.o \
	$(LFLAGS) -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread -lm -lnsl

mega.o: mega.c
	$(CC) $(CFLAGS) mega.c

clean:
	-@rm -f *.o $(BINDIR)/mega

