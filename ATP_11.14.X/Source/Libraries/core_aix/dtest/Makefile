BINDIR=../bin
LIBDIR=../lib
COREDIR=../lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include -I$(XIPCROOT)/include -DXIPC
LFLAGS = -q64 -L$(LIBDIR)
#CC=gcc
CC=cc
dialog: dialog.o
	$(CC) -o $(BINDIR)/dtest $(LFLAGS) dialog.o $(COREDIR)/daemon.o -lpteipc -lptecom -lpte -lxcom -lnsl -lpthread

dialog.o: dialog.c
	$(CC) $(CFLAGS) dialog.c

clean:
	-@rm *.o
	-@rm $(BINDIR)/dtest

