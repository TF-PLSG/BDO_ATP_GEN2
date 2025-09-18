BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer present in Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

tr31keyblock: tr31keyblock.o tr31keyblock_db.o aes_256.o
	$(CC) -o $(BINDIR)/tr31keyblock \
	tr31keyblock.o tr31keyblock_db.o aes_256.o\
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom -lpthread -lnsl

tr31keyblock.o: tr31keyblock.c
	$(CC) $(CFLAGS) tr31keyblock.c

tr31keyblock_db.o: tr31keyblock_db.c
	$(CC) $(CFLAGS) tr31keyblock_db.c

aes_256.o: aes_256.c
	$(CC) $(CFLAGS) aes_256.c

tr31keyblock_db.c: tr31keyblock_db.pc
	$(PROC) tr31keyblock_db.pc

clean:
	-@rm -f *.o *.lis tr31keyblock_db.c $(BINDIR)/tr31keyblock

