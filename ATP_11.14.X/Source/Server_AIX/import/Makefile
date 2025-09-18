BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

import: import.o import_db.o db_CheckNULL.o
	$(CC) -o $(BINDIR)/import import.o import_db.o db_CheckNULL.o\
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
        -lxcom -lpthread
	cp $(BINDIR)/import $(BINDIR)/emvc_tool

import.o: import.c
	$(CC) $(CFLAGS) import.c

import_db.o: import_db.c
	$(CC) $(CFLAGS) import_db.c
	
db_CheckNULL.o: db_CheckNULL.c
	$(CC) $(CFLAGS) db_CheckNULL.c	

import_db.c: import_db.pc
	$(PROC) import_db.pc

clean:
	-@rm -f *.o *.lis import_db.c $(BINDIR)/import

