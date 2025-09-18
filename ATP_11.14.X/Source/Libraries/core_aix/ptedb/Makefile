LIBDIR = ../lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include  -I$(XIPCROOT)/include
#CC=gcc
CC=cc
PROC=proc include=../../include
libptedb.a: dstimer.o dbcommon.o
	ar -X 64 rv $(LIBDIR)/libptedb.a dstimer.o dbcommon.o

dstimer.o: dstimer.c 
	$(CC) $(CFLAGS) dstimer.c

dbcommon.o: dbcommon.c
	$(CC) $(CFLAGS) dbcommon.c

dbcommon.c: dbcommon.pc
	$(PROC) dbcommon.pc

clean:
	-@rm -f *.o *.lis dbcommon.c $(LIBDIR)/libptedb.a

