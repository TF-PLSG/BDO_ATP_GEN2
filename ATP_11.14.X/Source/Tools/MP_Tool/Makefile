BINDIR=../bin
LIBDIR=../../Server_AIX/lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../../Server_AIX/include  -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient11 -lsql11 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp11 -lvsn11 -lcommon11 -lgeneric11 -lmm -lcore11 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 11g for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc

PROC=proc include=../../Server_AIX/include include=$(XIPCROOT)/include include=$(COREINC) include=$(XIPCROOT)/include threads=no

MPtool : sbatch.o sbatch_db.o
	$(CC) -o $(BINDIR)/MPtool sbatch.o sbatch_db.o $(LFLAGS) -lequit -lpteipc -lptedb -lpte -lxcom -lpthread -lnsl

sbatch.o : sbatch.c
	$(CC) $(CFLAGS) sbatch.c

sbatch_db.o : sbatch_db.c
	$(CC) $(CFLAGS) sbatch_db.c

sbatch_db.c : sbatch_db.pc
	$(PROC) sbatch_db.pc

clean :
	-@rm -f *.o *.lis $(BINDIR)/MPtool sbatch_db.c
