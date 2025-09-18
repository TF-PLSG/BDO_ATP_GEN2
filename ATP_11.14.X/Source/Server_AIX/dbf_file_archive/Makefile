BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
ORALLIBS= $(ORACLE_HOME)/lib/scorept.o $(ORACLE_HOME)/lib/sscoreed.o -lclntsh -lclient19 -lsql19 $(ORACLE_HOME)/lib/nautab.o $(ORACLE_HOME)/lib/naeet.o $(ORACLE_HOME)/lib/naect.o $(ORACLE_HOME)/lib/naedhs.o -lntcp19 -lvsn19 -lcommon19 -lgeneric19 -lmm -lcore19 -lm `cat $(ORACLE_HOME)/lib/sysliblist`
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR)  -L$(XIPCROOT)/lib \
	 -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/precomp/lib  $(ORALLIBS)

# This object file is no longer part of Oracle 12c for Solaris.
# $(ORACLE_HOME)/rdbms/lib/kpudfo.o

CC=cc
PROC=proc include=../include include=$(XIPCROOT)/include include=$(COREINC) threads=no

dbf_file_archive: dbf.o dbf_db.o
	$(CC) -o $(BINDIR)/dbf_file_archive dbf.o dbf_db.o \
	$(LFLAGS) \
	-lequit -lpteipc -lptedb -lpte \
	-lxcom -lpthread -lnsl

dbf.o: dbf.c
	$(CC) $(CFLAGS) dbf.c

dbf_db.o: dbf_db.c
	$(CC) $(CFLAGS) dbf_db.c

dbf_db.c: dbf_db.pc
	$(PROC) dbf_db.pc

clean:
	-@rm -f *.o *.lis dbf_db.c $(BINDIR)/dbf_file_archive

