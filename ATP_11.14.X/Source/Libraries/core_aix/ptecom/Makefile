LIBDIR = ../lib
#CPPCMT = -qcpluscmt
CPPCMT = 

CFLAGS = -q64 -qcpluscmt -c $(CPPCMT) -g -I../../include  -I$(XIPCROOT)/include -DXIPC -DBDO_SOLARIS_PORT
CFLAGS_LOW_PRIORITY = -c $(CPPCMT) -g -I../../include  -I$(XIPCROOT)/include -DXIPC -DBDO_SOLARIS_PORT -DLOW_PRIORITY

#CC=gcc
CC=cc
libptecom.a: common.o notify.o convert.o tcp.o
	ar -X 64 rv $(LIBDIR)/libptecom.a common.o notify.o convert.o tcp.o

common.o: common.c 
	$(CC) $(CFLAGS) common.c

convert.o: convert.c 
	$(CC) $(CFLAGS) convert.c 

notify.o: notify.c
	$(CC) $(CFLAGS) notify.c

tcp.o: tcp.c
	$(CC) $(CFLAGS) tcp.c

serial.o: serial.c
	$(CC) $(CFLAGS) serial.c

####################################################
# Default priority for transactional XIPC messages
# will be changed to 2 inside ATP. High priority
# control/system messages, ping requests etc. will
# use priority 3.
#
# Here we will build ptecom so that it will use a
# lower priority 1, when dialog sends an incoming
# transaction to device controllers.
#
#
####################################################
lowprt: common.o notify_low convert.o tcp.o
	ar -X 64 rv $(LIBDIR)/libptecom.a common.o notify.o convert.o tcp.o

notify_low: notify.c
	$(CC) $(CFLAGS_LOW_PRIORITY) notify.c

clean:
	-@rm -f *.o $(LIBDIR)/libptecom.a

