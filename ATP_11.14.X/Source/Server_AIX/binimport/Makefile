BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/libclntsh.a $(ORACLE_HOME)/lib/sscoreed.o -lpthread -lnsl -lsql19 -lclient19 -lclntsh $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lclient19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(COREINC) threads=no

binimport: bin02.o
	$(CC) -o $(BINDIR)/binimport bin02.o  \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom


bin02.o: bin02.c
	$(CC) $(CFLAGS) bin02.c

bin02.c: bin02.pc
	$(PROC) bin02.pc

clean:
	-@rm -f *.o *.lis bin02.c $(BINDIR)/binimport

