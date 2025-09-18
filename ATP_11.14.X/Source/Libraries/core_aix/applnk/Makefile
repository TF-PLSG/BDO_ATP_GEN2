BINDIR=../bin
LIBDIR=../lib
APPLIB=../../../Server_AIX/lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include -I$(XIPCROOT)/include 
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads -L$(APPLIB)
CC=cc

applnk: applnk.o notify.o
	$(CC) -o $(BINDIR)/applnk applnk.o notify.o $(LIBDIR)/daemon.o $(LFLAGS) -lptecom -lpte -lpteipc -lxcom -lnsl -lpthread -lequit

applnk.o: applnk.c
	$(CC) $(CFLAGS) applnk.c

notify.o: notify.c
	$(CC) $(CFLAGS) notify.c

clean:
	-@rm -f *.o $(BINDIR)/applnk

