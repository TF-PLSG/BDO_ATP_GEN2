BINDIR=../bin
LIBDIR=../../Server_AIX/lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../../Server_AIX/include \
	 -I$(XIPCROOT)/include -DXIPC

ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient11 -lsql11 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp11 -lclient11 -lvsn11 -lcommon11 -lgeneric11 -lmm -lcore11 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 11g for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../../Server_AIX/include include=$(XIPCROOT)/include include=$(COREINC) threads=no

settle: settle.o settledb.o
	$(CC) -o $(BINDIR)/eftpos_rps_new settle.o settledb.o \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom -lnsl -lpthread

settle.o: settle.c
	$(CC) $(CFLAGS) settle.c

settledb.o: settledb.c
	$(CC) $(CFLAGS) settledb.c

settledb.c: settledb.pc
	$(PROC) settledb.pc

clean:
	-@rm -f *.o *.lis settledb.c $(BINDIR)/eftpos_rps_new

