LIBDIR = ../lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include  -I$(XIPCROOT)/include -DXIPC
#CC=gcc
CC=cc
libpte.a: des.o logger.o ptecopy.o ptedes.o ptehash.o pteinit.o ptellist.o ptemsg.o ptestats.o ptesystm.o ptetime.o ptetimer.o startup.o ntutils.o
	ar -X 64 rv $(LIBDIR)/libpte.a logger.o ptecopy.o ptedes.o ptehash.o pteinit.o ptellist.o ptemsg.o ptestats.o ptesystm.o ptetime.o ptetimer.o startup.o ntutils.o des.o

des.o: des.c 
	$(CC) $(CFLAGS) des.c

logger.o: logger.c 
	$(CC) $(CFLAGS) logger.c

ptecopy.o: ptecopy.c 
	$(CC) $(CFLAGS) ptecopy.c

ptedes.o: ptedes.c 
	$(CC) $(CFLAGS) ptedes.c

ptehash.o: ptehash.c 
	$(CC) $(CFLAGS) ptehash.c

pteinit.o: pteinit.c 
	$(CC) $(CFLAGS) pteinit.c

ptellist.o: ptellist.c 
	$(CC) $(CFLAGS) ptellist.c

ptemsg.o: ptemsg.c 
	$(CC) $(CFLAGS) ptemsg.c

ptestats.o: ptestats.c 
	$(CC) $(CFLAGS) ptestats.c

ptesystm.o: ptesystm.c 
	$(CC) $(CFLAGS) ptesystm.c 

ptetime.o: ptetime.c 
	$(CC) $(CFLAGS) ptetime.c

ptetimer.o: ptetimer.c 
	$(CC) $(CFLAGS) ptetimer.c

startup.o: startup.c 
	$(CC) $(CFLAGS) startup.c

ntutils.o: ntutils.c 
	$(CC) $(CFLAGS) ntutils.c

clean:
	-@rm -f *.o $(LIBDIR)/libpte.a

