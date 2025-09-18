BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

atpnetmon: main.o
	$(CC) -o $(BINDIR)/atpnetmon \
	main.o  \
	$(LFLAGS) -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread \
	-lm -lnsl

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/atpnetmon
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

