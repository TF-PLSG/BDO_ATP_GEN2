BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC

ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lclient19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 11g for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

falconarc: falconarc.o falcon_db.o
	$(CC) -o $(BINDIR)/falcon_archive falconarc.o falcon_db.o \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom -lnsl -lpthread

falconarc.o: falconarc.c
	$(CC) $(CFLAGS) falconarc.c

falcon_db.o: falcon_db.c
	$(CC) $(CFLAGS) falcon_db.c

falcon_db.c: falcon_db.pc
	$(PROC) falcon_db.pc

clean:
	-@rm -f *.o *.lis falcon_db.c $(BINDIR)/falcon_archive
