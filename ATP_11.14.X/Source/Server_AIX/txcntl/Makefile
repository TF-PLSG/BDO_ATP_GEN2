BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

txcntl: txservice.o myutils.o txauthorize.o txcntl.o txdataserver.o txfleet.o tx_sha256_encryption.o hmac.o sha1.o sha2.o
	$(CC) -o $(BINDIR)/txcntl txservice.o myutils.o txauthorize.o txcntl.o txdataserver.o txfleet.o tx_sha256_encryption.o hmac.o sha1.o sha2.o $(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte -lxcom -lm -lpthread -lnsl

txservice.o: txservice.c
	$(CC) $(CFLAGS) txservice.c

myutils.o: myutils.c
	$(CC) $(CFLAGS) myutils.c

txauthorize.o: txauthorize.c
	$(CC) $(CFLAGS) txauthorize.c

txcntl.o: txcntl.c
	$(CC) $(CFLAGS) txcntl.c

txdataserver.o: txdataserver.c
	$(CC) $(CFLAGS) txdataserver.c
	
tx_sha256_encryption.o: tx_sha256_encryption.c
	$(CC) $(CFLAGS) tx_sha256_encryption.c

txfleet.o: txfleet.c
	$(CC) $(CFLAGS) txfleet.c

hmac.o: hmac.c
	$(CC) $(CFLAGS) hmac.c
	
sha1.o: sha1.c
	$(CC) $(CFLAGS) sha1.c
	
sha2.o: sha2.c
	$(CC) $(CFLAGS) sha2.c

clean:
	-@rm -f *.o $(BINDIR)/txcntl

