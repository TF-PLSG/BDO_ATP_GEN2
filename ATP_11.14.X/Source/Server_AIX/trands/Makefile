#include $(ORACLE_HOME)/precomp/lib32/env_precomp.mk
BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib $(ORALLIBS)

# This object file is no longer present in Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

trands: tx_ds.o tx_database.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/trands tx_ds.o tx_database.o db_CheckNULL.o \
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom -lpthread -lnsl

tx_ds.o: tx_ds.c
	$(CC) $(CFLAGS) tx_ds.c

tx_database.o: tx_database.c
	$(CC) $(CFLAGS) tx_database.c
	
db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c

tx_database.c: tx_database.pc
	$(PROC) tx_database.pc

clean:
	-@rm -f *.o *.lis tx_database.c $(BINDIR)/trands
    
