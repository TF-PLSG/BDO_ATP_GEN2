BINDIR=../bin
LIBDIR=../lib
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I$(COREINC) -I../include \
	 -I$(XIPCROOT)/include -DXIPC 
LFLAGS = -q64 -L$(CORELIB) -L$(LIBDIR) -L$(XIPCROOT)/lib
CC=cc
LINT=lint

dcpiso: batch_upload.o database_request.o database_response.o general_utilities.o iso_utilities.o message_handler.o process_incoming_msg.o process_outgoing_msg.o service_functions.o
	$(CC) -o $(BINDIR)/dcpiso \
	batch_upload.o database_request.o database_response.o general_utilities.o iso_utilities.o message_handler.o process_incoming_msg.o process_outgoing_msg.o service_functions.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread \
	-lm -lnsl

lint:
	$(LINT) -Nn10000 -Nd10000 -qcpluscmt -I$(COREINC) -I../include \
	-I$(XIPCROOT)/include -DXIPC  \
	batch_upload.o database_request.o database_response.o general_utilities.o iso_utilities.o message_handler.o process_incoming_msg.o process_outgoing_msg.o service_functions.o \
	> lint.out

batch_upload.o: batch_upload.c
	$(CC) $(CFLAGS) batch_upload.c

database_request.o: database_request.c
	$(CC) $(CFLAGS) database_request.c

database_response.o: database_response.c
	$(CC) $(CFLAGS) database_response.c

general_utilities.o: general_utilities.c
	$(CC) $(CFLAGS) general_utilities.c

iso_utilities.o: iso_utilities.c
	$(CC) $(CFLAGS) iso_utilities.c

message_handler.o: message_handler.c
	$(CC) $(CFLAGS) message_handler.c

process_incoming_msg.o: process_incoming_msg.c
	$(CC) $(CFLAGS) process_incoming_msg.c

process_outgoing_msg.o: process_outgoing_msg.c
	$(CC) $(CFLAGS) process_outgoing_msg.c

service_functions.o: service_functions.c
	$(CC) $(CFLAGS) service_functions.c

clean:
	-@rm -f *.o* *.m* *.p*
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f
	-@rm -f $(BINDIR)/dcpiso

