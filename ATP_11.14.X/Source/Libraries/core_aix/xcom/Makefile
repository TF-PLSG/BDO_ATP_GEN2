#XIPCROOT = /local/xipc
COREINC = ../../include
LIBDIR = ../lib
#CFLAGS = -c -g -w -fPIC -I$(COREINC) -I$(XIPCROOT)/include -DXIPC -DFALSE=0 -DTRUE=1 -DSOLARIS_BUILD
CFLAGS = -c -g -q64 -qcpluscmt -qlanglvl=extc99 -fPIC -I$(COREINC) -I$(XIPCROOT)/include -DXIPC -DAIX_BUILD
#$(CC)=/usr/bin/cc
#CC=gcc
CC=cc
staticlib: xcom.o 
	ar -X 64 rv $(LIBDIR)/libxcom.a xcom.o ../mymalloc/mymalloc.o

dynamiclib: xcom1.o
	gcc -X 64 -shared -W1,soname,libxcom.so.1 -o $(LIBDIR)/libxcom.so.1.0.1 xcom1.o ../mymalloc/mymalloc.o
	ln -sf $(LIBDIR)/libxcom.so.1.0.1 $(LIBDIR)/libxcom.so

xcom1.o: xcom.c
	$(CC) $(CFLAGS) xcom.c -o xcom1.o

xcom.o: xcom.c
	$(CC) $(CFLAGS) -DXCOMDEBUG xcom.c

clean:
	-@rm -f *.o
	-@rm -f $(LIBDIR)/libxcom.so
	-@rm -f $(LIBDIR)/libxcom.so.1.0.1
	-@rm -f $(LIBDIR)/libxcom.a

