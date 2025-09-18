#XIPCROOT=/local/xipc
COREDIR = ../../Libraries/core_aix
COREINC = $(COREDIR)/../include
CORELIB = $(COREDIR)/lib
BINDIR = ../bin

#CFLAGS = -lrt -g -mt -I$(XIPCROOT)/include -I$(COREINC) -DSOLARIS_BUILD 
CFLAGS = -g -q64 -qcpluscmt -I$(XIPCROOT)/include -I$(COREINC) -DAIX_BUILD 

# mtmalloc is the multi threaded library for high
# performance malloc/free API's. Normal malloc library is not suitable
# for multi threaded applications.
#
# - mymalloc() will be used if memory is to be allocated from shared memory.
# - malloc() will be used if memory is to be allocated from process memory.
LFLAGS = -L$(CORELIB) -lpthread -lmymalloc 
#CC= /usr/bin/cc 
CC=cc 

#.PHONY: debug
debug: xcomdaemon
	-@cp ./xcomdaemon $(BINDIR)/

#.PHONY: release
release: xcomdaemon1
	mv xcomdaemon1 xcomdaemon
	-@cp ./xcomdaemon $(BINDIR)/

xcomdaemon1: hash1.o queue1.o shm_mem1.o xcomdaemon.c init.o
	$(CC) $(CFLAGS) xcomdaemon.c hash1.o queue1.o shm_mem1.o $(LFLAGS) init.o -o xcomdaemon1

hash1.o : hash.c
	$(CC) -c $(CFLAGS) hash.c -o hash1.o 

queue1.o: queue.c 
	$(CC) -c $(CFLAGS) queue.c -o queue1.o 

shm_mem1.o: shm_mem.c 
	$(CC) -c $(CFLAGS) shm_mem.c -o shm_mem1.o 
	-@cp ./xcomdaemon $(BINDIR)/


xcomdaemon: hash.o queue.o shm_mem.o xcomdaemon.c init.o
	$(CC) $(CFLAGS) -DXCOMDEBUG xcomdaemon.c hash.o queue.o shm_mem.o $(LFLAGS) init.o -o xcomdaemon

hash.o: hash.c 
	$(CC) -c $(CFLAGS) -DXCOMDEBUG hash.c -o hash.o

queue.o: queue.c 
	$(CC) -c $(CFLAGS) -DXCOMDEBUG queue.c -o queue.o 

shm_mem.o: shm_mem.c 
	$(CC) -c $(CFLAGS) -DXCOMDEBUG shm_mem.c -o shm_mem.o 

init.o: init.c
	$(CC) -c -g -w $(CFLAGS) -I$(COREINC) init.c -o init.o

clean:
	-@rm -f ./*.o ./xcomdaemon $(BINDIR)/xcomdaemon
