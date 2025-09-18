BINDIR=../bin
LIBDIR=../lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include -I$(XIPCROOT)/include 
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

archlnk: archlnk.o notify.o
	$(CC) -o $(BINDIR)/archlnk archlnk.o notify.o $(LIBDIR)/daemon.o $(LFLAGS) -lptecom -lpte -lpteipc -lxcom -lnsl -lpthread

archlnk.o: archlnk.c
	$(CC) $(CFLAGS) archlnk.c

notify.o: notify.c
	$(CC) $(CFLAGS) notify.c

clean:
	-@rm -f *.o $(BINDIR)/archlnk

