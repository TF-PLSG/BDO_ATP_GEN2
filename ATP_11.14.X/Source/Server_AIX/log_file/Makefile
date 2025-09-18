#include $(ORACLE_HOME)/precomp/lib/env_precomp.mk
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

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(COREINC) threads=no

log_file: log_file.o log_db.o
	$(CC) -o $(BINDIR)/log_file log_file.o log_db.o \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom -lpthread

log_file.o: log_file.c
	$(CC) $(CFLAGS) log_file.c

log_db.o: log_db.c
	$(CC) $(CFLAGS) log_db.c

log_db.c: log_db.pc
	$(PROC) log_db.pc

clean:
	-@rm -f *.o *.lis log_db.c $(BINDIR)/log_file

