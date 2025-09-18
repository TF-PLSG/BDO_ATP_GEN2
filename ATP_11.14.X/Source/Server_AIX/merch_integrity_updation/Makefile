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

merch_integrity_updation: merch_param.o merch_database.o
	$(CC) -o $(BINDIR)/merch_integrity_updation merch_param.o merch_database.o \
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom -lpthread -lnsl

merch_param.o: merch_param.c
	$(CC) $(CFLAGS) merch_param.c

merch_database.o: merch_database.c
	$(CC) $(CFLAGS) merch_database.c

merch_database.c: merch_database.pc
	$(PROC) merch_database.pc

clean:
	-@rm -f *.o *.lis tx_database.c $(BINDIR)/merch_integrity_updation
    
