#XIPCROOT=/local/xipc
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
BINDIR = ../bin
CFLAGS =  -g -q64 -qcpluscmt -w -I$(XIPCROOT)/include -I$(COREINC) -DAIX_BUILD
LFLAGS = -q64 -L$(CORELIB)/ -lpteipc -lpte -lxcom -lpthread
CC=cc 

xsvrtst: Xcomservertest.c
	$(CC) $(CFLAGS) Xcomservertest.c -o $(BINDIR)/xsvrtst $(CORELIB)/daemon.o $(LFLAGS)

clean:
	-@rm -f *.o $(BINDIR)/xsvrtst 

