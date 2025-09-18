BINDIR=../bin
LIBDIR=../lib
CFLAGS = -c -g -q64 -qcpluscmt -I../../include -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(LIBDIR) -L$(XIPCROOT)/lib/threads
CC=cc

dialog: dialog.o
	$(CC) -o $(BINDIR)/dialog dialog.o $(LFLAGS) $(LIBDIR)/daemon.o -lpteipc -lptecom -lpte -lxcom -lnsl -lpthread
	cp $(BINDIR)/dialog $(BINDIR)/hsteqit
	cp $(BINDIR)/dialog $(BINDIR)/hsteqit2
	cp $(BINDIR)/dialog $(BINDIR)/hstjcb
	cp $(BINDIR)/dialog $(BINDIR)/hstjcb2
	cp $(BINDIR)/dialog $(BINDIR)/hstjcb3
	cp $(BINDIR)/dialog $(BINDIR)/hstvisa
	cp $(BINDIR)/dialog $(BINDIR)/hstvisa2
	cp $(BINDIR)/dialog $(BINDIR)/hstvisa3
	cp $(BINDIR)/dialog $(BINDIR)/hstvsms
	cp $(BINDIR)/dialog $(BINDIR)/hstmcrd
	cp $(BINDIR)/dialog $(BINDIR)/hstmcr2
	cp $(BINDIR)/dialog $(BINDIR)/racalcon
	cp $(BINDIR)/dialog $(BINDIR)/hstacatm2
	cp $(BINDIR)/dialog $(BINDIR)/hstacatm
	cp $(BINDIR)/dialog $(BINDIR)/hstamatm
	cp $(BINDIR)/dialog $(BINDIR)/hstacpos2
	cp $(BINDIR)/dialog $(BINDIR)/hstacpos
	cp $(BINDIR)/dialog $(BINDIR)/hstampos
	cp $(BINDIR)/dialog $(BINDIR)/hstcup
	cp $(BINDIR)/dialog $(BINDIR)/hstcup2
	cp $(BINDIR)/dialog $(BINDIR)/hstcup3
	cp $(BINDIR)/dialog $(BINDIR)/hstiftr
	cp $(BINDIR)/dialog $(BINDIR)/hstdci
	cp $(BINDIR)/dialog $(BINDIR)/hstdci2
	cp $(BINDIR)/dialog $(BINDIR)/hstdci3
	cp $(BINDIR)/dialog $(BINDIR)/hstpul
	cp $(BINDIR)/dialog $(BINDIR)/hstmcr3





dialog.o: dialog.c
	$(CC) $(CFLAGS) dialog.c

clean:
	-@rm -f *.o $(BINDIR)/dialog \
	$(BINDIR)/hstaqatm \
	$(BINDIR)/hstaqatm2 \
	$(BINDIR)/hstaqpos \
	$(BINDIR)/hstaqpos2 \
	$(BINDIR)/hstatm \
	$(BINDIR)/hstcirs \
	$(BINDIR)/hsteqit \
	$(BINDIR)/hstfgrd \
	$(BINDIR)/hstfgrd1 \
	$(BINDIR)/hstjcb \
	$(BINDIR)/hstjcb2 \
	$(BINDIR)/hstpos \
	$(BINDIR)/hstvis2 \
	$(BINDIR)/hstvisa \
	$(BINDIR)/hstvsms \
	$(BINDIR)/racalcon \
