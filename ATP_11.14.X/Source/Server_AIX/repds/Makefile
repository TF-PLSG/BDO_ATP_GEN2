BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer present in Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

repds: rep_ds.o rep_database.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/repds rep_ds.o rep_database.o db_CheckNULL.o \
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom -lnsl -lpthread

rep_ds.o: rep_ds.c
	$(CC) $(CFLAGS) rep_ds.c

rep_database.o: rep_database.c
	$(CC) $(CFLAGS) rep_database.c

db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c	

rep_database.c: rep_database.pc
	$(PROC) rep_database.pc

clean:
	-@rm -f *.o *.lis rep_database.c $(BINDIR)/repds

