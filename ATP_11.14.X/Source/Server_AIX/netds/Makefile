BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lclient19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer present in Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

netds: nc_ds.o nc_database.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/netds nc_ds.o nc_database.o db_CheckNULL.o \
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom -lnsl -lpthread


nc_ds.o: nc_ds.c
	$(CC) $(CFLAGS) nc_ds.c

nc_database.o: nc_database.c
	$(CC) $(CFLAGS) nc_database.c
	
db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c

nc_database.c: nc_database.pc
	$(PROC) nc_database.pc

clean:
	-@rm -f *.o *.lis nc_database.c $(BINDIR)/netds

