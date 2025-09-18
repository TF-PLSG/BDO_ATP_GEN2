LIBDIR = ../lib
COREDIR = ../../Libraries/core_aix
CFLAGS = -c -g -q64 -qcpluscmt -I$(COREDIR)/../include -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
CC=cc
libequit.a: genutil.o memmnger.o txdsapi.o txutils.o diskio.o txtrace.o timings.o
	ar -X 64 rv $(LIBDIR)/libequit.a genutil.o memmnger.o txdsapi.o txutils.o diskio.o txtrace.o timings.o

genutil.o: genutil.c
	$(CC) $(CFLAGS) genutil.c

memmnger.o: memmnger.c 
	$(CC) $(CFLAGS) memmnger.c

txdsapi.o: txdsapi.c 
	$(CC) $(CFLAGS) txdsapi.c

txutils.o: txutils.c 
	$(CC) $(CFLAGS) txutils.c

diskio.o: diskio.c
	$(CC) $(CFLAGS) diskio.c

txtrace.o: txtrace.c
	$(CC) $(CFLAGS) txtrace.c

timings.o: timings.c
	$(CC) $(CFLAGS) timings.c

clean:
	-@rm -f *.o *.MAK $(LIBDIR)/libequit.a
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

