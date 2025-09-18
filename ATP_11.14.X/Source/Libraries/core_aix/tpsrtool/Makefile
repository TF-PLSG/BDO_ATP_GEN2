BINDIR=../bin
LIBDIR=../lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I../../include -I$(XIPCROOT)/include -DXIPC -DBDO_SOLARIS_PORT
LFLAGS = -q64 -L$(LIBDIR)

#-L$(XIPCROOT)/lib/threads

#CC=gcc
CC=cc
tpsrtool: tpsrtool.o convert.o notify.o common.o tcp.o
	$(CC) -o $(BINDIR)/tpsrtool tpsrtool.o convert.o notify.o \
	common.o tcp.o $(LFLAGS) -lpte \
	-lnsl -lpthread

#-lpteipc -lxcom

tpsrtool.o: tpsrtool.c
	$(CC) $(CFLAGS) tpsrtool.c

convert.o: convert.c
	$(CC) $(CFLAGS) convert.c

notify.o: notify.c
	$(CC) $(CFLAGS) notify.c

common.o: common.c
	$(CC) $(CFLAGS) common.c

tcp.o: tcp.c
	$(CC) $(CFLAGS) tcp.c

clean:
	-@rm -f *.o
	-@rm -f $(BINDIR)/tpsrtool

