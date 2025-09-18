#XIPCROOT=/local/xipc
BINDIR=../bin
LIBDIR=../lib

#-L$(XIPCROOT)/lib/threads

#CC=gcc
CC=cc
COREDIR = ..
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -q64 -qcpluscmt -w -fPIC -I$(COREINC) -I$(XIPCROOT)/include -DXIPC -DFALSE=0 -DTRUE=1 -DAIX_BUILD

LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib

#CC=gcc
CC=cc
PROC=proc include=../../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

xcom: xcomCmdLine.o 
	$(CC) -o $(BINDIR)/xcomcmd xcomCmdLine.o \
	$(LFLAGS) \
	-lxcom -lpthread 

xcomCmdLine.o: xcomCmdLine.c
	$(CC) $(CFLAGS) xcomCmdLine.c

clean:
	-@rm -f *.o *.lis $(BINDIR)/xcomcmd
	-@rm -f *.o *.lis $(BINDIR)/xipcTest

