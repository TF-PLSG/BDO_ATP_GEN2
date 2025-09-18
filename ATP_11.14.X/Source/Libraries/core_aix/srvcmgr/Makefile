BINDIR=../bin
LIBDIR=../lib
CPPCMT= 
CFLAGS = -q64 -qcpluscmt -c -g $(CPPCMT) -I../../include -I$(XIPCROOT)/include 
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

srvcmgr:  srvcmgr.o srvc_util.o 
	$(CC) -o $(BINDIR)/srvcmgr srvcmgr.o srvc_util.o \
	$(LFLAGS) $(LIBDIR)/daemon.o -lpteipc -lpte -lxcom -lnsl -lpthread

srvcmgr.o: srvcmgr.c
	$(CC) $(CFLAGS) srvcmgr.c

srvc_util.o: srvc_util.c
	$(CC) $(CFLAGS) srvc_util.c

sm_daemon.o: sm_daemon.c
	$(CC) $(CFLAGS) sm_daemon.c

clean:
	-@rm -f *.o $(BINDIR)/srvcmgr

