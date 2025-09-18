BINDIR=../bin
LIBDIR=../lib
CPPCMT= 
CFLAGS = -q64 -qcpluscmt -c -g $(CPPCMT) -I../../include -I$(XIPCROOT)/include 
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

avutl_srvcmgr:  avutl_srvcmgr.o avutl_srvc_util.o 
	$(CC) -o $(BINDIR)/avutl_srvcmgr avutl_srvcmgr.o avutl_srvc_util.o \
	$(LFLAGS) -lpteipc -lpte -lxcom -lnsl -lpthread

avutl_srvcmgr.o: avutl_srvcmgr.c
	$(CC) $(CFLAGS) avutl_srvcmgr.c

avutl_srvc_util.o: avutl_srvc_util.c
	$(CC) $(CFLAGS) avutl_srvc_util.c

clean:
	-@rm -f *.o $(BINDIR)/avutl_srvcmgr

