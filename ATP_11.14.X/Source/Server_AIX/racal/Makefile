BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -w -g -q64 -qcpluscmt -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

racal: racal.o tx_sha256_encryption.o hmac.o sha1.o sha2.o
	$(CC) -o $(BINDIR)/racal racal.o tx_sha256_encryption.o hmac.o sha1.o sha2.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lm -lnsl -lpthread

racal.o: racal.c
	$(CC) $(CFLAGS) racal.c

tx_sha256_encryption.o: tx_sha256_encryption.c
	$(CC) $(CFLAGS) tx_sha256_encryption.c

hmac.o: hmac.c
	$(CC) $(CFLAGS) hmac.c

sha1.o: sha1.c
	$(CC) $(CFLAGS) sha1.c
	
sha2.o: sha2.c
	$(CC) $(CFLAGS) sha2.c

clean:
	-@rm -f *.o $(BINDIR)/racal

