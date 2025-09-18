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

dcpos: dcpos_database_request.o dcpos_database_response.o dcpos_general_utilities.o dcpos_iso_utilities.o dcpos_message_handler.o dcpos_process_incoming_msg.o dcpos_process_outgoing_msg.o dcpos_service_functions.o
	$(CC) -o $(BINDIR)/dcpos \
	dcpos_database_request.o dcpos_database_response.o dcpos_general_utilities.o dcpos_iso_utilities.o dcpos_message_handler.o dcpos_process_incoming_msg.o dcpos_process_outgoing_msg.o dcpos_service_functions.o \
	$(LFLAGS) $(CORELIB)/daemon.o -lequit -lpteipc -lptecom -lpte \
	-lxcom -lpthread \
	-lm -lnsl

lint:
	$(LINT) -Nn10000 -Nd10000 -qcpluscmt -I$(COREINC) -I../include \
	-I$(XIPCROOT)/include -DXIPC  \
	dcpos_database_request.o dcpos_database_response.o dcpos_general_utilities.o dcpos_iso_utilities.o dcpos_message_handler.o dcpos_process_incoming_msg.o dcpos_process_outgoing_msg.o dcpos_service_functions.o \
	> lint.out

dcpos_database_request.o: dcpos_database_request.c
	$(CC) $(CFLAGS) dcpos_database_request.c

dcpos_database_response.o: dcpos_database_response.c
	$(CC) $(CFLAGS) dcpos_database_response.c

dcpos_general_utilities.o: dcpos_general_utilities.c
	$(CC) $(CFLAGS) dcpos_general_utilities.c

dcpos_iso_utilities.o: dcpos_iso_utilities.c
	$(CC) $(CFLAGS) dcpos_iso_utilities.c

dcpos_message_handler.o: dcpos_message_handler.c
	$(CC) $(CFLAGS) dcpos_message_handler.c

dcpos_process_incoming_msg.o: dcpos_process_incoming_msg.c
	$(CC) $(CFLAGS) dcpos_process_incoming_msg.c

dcpos_process_outgoing_msg.o: dcpos_process_outgoing_msg.c
	$(CC) $(CFLAGS) dcpos_process_outgoing_msg.c

dcpos_service_functions.o: dcpos_service_functions.c
	$(CC) $(CFLAGS) dcpos_service_functions.c

clean:
	-@rm -f *.o* *.m* *.p*
	-@ls -1 *.d*|grep -v '\.dsp'|grep -v '\.dsw'|grep -v '\.doc'|xargs rm -f
	-@rm -f $(BINDIR)/dcpos

