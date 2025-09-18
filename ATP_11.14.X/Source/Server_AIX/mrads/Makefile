BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient12 -lsql12 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp12 -lvsn12 -lcommon12 -lgeneric12 -lmm -lcore12 -lm `cat $(ORACLE_HOME)/lib/sysliblist` -lm -lnsl
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

#This library is not present in 12c
#$(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

mrads: mra_ds.o mra_database.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/mrads mra_ds.o mra_database.o db_CheckNULL.o \
	$(LFLAGS) \
	$(CORELIB)/daemon.o -lequit -lpteipc -lptedb -lpte \
	-lxcom

mra_ds.o: mra_ds.c
	$(CC) $(CFLAGS) mra_ds.c

mra_database.o: mra_database.c
	$(CC) $(CFLAGS) mra_database.c

db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c

mra_database.c: mra_database.pc
	$(PROC) mra_database.pc

clean:
	-@rm -f *.o *.lis $(BINDIR)/mrads mra_database.c

