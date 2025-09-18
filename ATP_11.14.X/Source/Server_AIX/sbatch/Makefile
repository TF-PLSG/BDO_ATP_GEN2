BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include  -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc

PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) include=$(XIPCROOT)/include threads=no

sbatch : sbatch.o sbatch_db.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/sbatch sbatch.o sbatch_db.o db_CheckNULL.o \$(LFLAGS) -lequit -lpteipc -lptedb -lpte -lxcom -lpthread -lnsl

sbatch.o : sbatch.c
	$(CC) $(CFLAGS) sbatch.c

sbatch_db.o : sbatch_db.c
	$(CC) $(CFLAGS) sbatch_db.c
	
db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c	

sbatch_db.c : sbatch_db.pc
	$(PROC) sbatch_db.pc

clean :
	-@rm -f *.o *.lis $(BINDIR)/sbatch sbatch_db.c
