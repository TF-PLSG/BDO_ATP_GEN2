LIBDIR = ../lib
CFLAGS = -c -g -q64 -qcpluscmt -w -fPIC -I../../include  -I$(XIPCROOT)/include -DXIPC 
#CC=/usr/bin/cc
#GCC=gcc
GCC=cc
staticlib: mymalloc.o
	ar -X 64 rv $(LIBDIR)/libmymalloc.a mymalloc.o

dynamiclib: mymalloc.o
	$(GCC) -shared -W1,soname,libmymalloc.so.1 -o $(LIBDIR)/libmymalloc.so.1.0.1 mymalloc.o
	ln -sf $(LIBDIR)/libmymalloc.so.1.0.1 $(LIBDIR)/libmymalloc.so

mymalloc.o: mymalloc.c
	$(GCC) $(CFLAGS) mymalloc.c

clean:
	-@rm -f *.o
	-@rm -f $(LIBDIR)/libmymalloc.so
	-@rm -f $(LIBDIR)/libmymalloc.so.1.0.1
	-@rm -f $(LIBDIR)/libmymalloc.a

