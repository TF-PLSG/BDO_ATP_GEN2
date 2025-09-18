BINDIR=../bin
LIBDIR=../lib

ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist` -lm -lnsl

CFLAGS = -c -g -q64 -qcpluscmt -I../../include -I$(XIPCROOT)/include
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads \
         -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib $(PROLDLIBS)  $(ORALLIBS)
CC=cc
PROC=proc include=../../include DEFINE=tid64_t DEFINE=sa_family_t DEFINE=in_addr_t

coreds: dm_config.o coreds.o coredb.o
	$(CC) -o $(BINDIR)/coreds dm_config.o coreds.o coredb.o \
	$(LFLAGS) \
	-lpteipc -lptedb -lpte $(LIBDIR)/daemon.o \
	-lxcom -lpthread

coreds.o: coreds.c
	$(CC) $(CFLAGS) coreds.c

coredb.o: coredb.c
	$(CC) $(CFLAGS) coredb.c

coredb.c: coredb.pc
	$(PROC) coredb.pc

dm_config.o: dm_config.c
	$(CC) $(CFLAGS) dm_config.c

clean:
	-@rm -f *.o *.lis coredb.c $(BINDIR)/coreds

