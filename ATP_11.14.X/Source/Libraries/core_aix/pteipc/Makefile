LIBDIR = ../lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I../../include  -I$(XIPCROOT)/include -DXIPC 
#$(CC)=gcc
$(CC)=cc
libpteipc.a: cdsapi.o pteipc.o 
	ar -X 64 rv $(LIBDIR)/libpteipc.a cdsapi.o pteipc.o

pteipc.o: pteipc.c 
	$(CC) $(CFLAGS) pteipc.c

cdsapi.o: cdsapi.c
	$(CC) $(CFLAGS) cdsapi.c

clean:
	-@rm -f *.o $(LIBDIR)/libpteipc.a

