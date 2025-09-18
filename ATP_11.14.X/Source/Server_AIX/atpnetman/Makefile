BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc

atpnetman: main.o menus.o
	$(CC) -o $(BINDIR)/atpnetman \
	main.o menus.o  \
	$(LFLAGS) -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread \
	-lm -lnsl

main.o: main.c
	$(CC) $(CFLAGS) main.c

menus.o: menus.c
	$(CC) $(CFLAGS) menus.c

clean:
	-@rm -f *.o *.m* $(BINDIR)/atpnetman
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f

