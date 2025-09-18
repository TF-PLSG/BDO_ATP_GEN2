BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lpthread -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lnls19 -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(COREINC) threads=no

cleanup: eqcleanup.o 
	$(CC) -o $(BINDIR)/cleanup eqcleanup.o \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom

eqcleanup.o: eqcleanup.c
	$(CC) $(CFLAGS) eqcleanup.c

eqcleanup.c: eqcleanup.pc
	$(PROC) eqcleanup.pc

clean:
	-@rm -f *.o *.lis eqcleanup.c $(BINDIR)/cleanup

