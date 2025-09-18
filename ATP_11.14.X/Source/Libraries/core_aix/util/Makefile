#
#	$ make
#	$ make debug
#
#	Use "make" for client releases
#
#	Use "make debug" for debugging purpose. It will build daemon
#	library with a 25 seconds delay to attach gdb debugger, after
#	the daemon process has forked two childs and killed the parents.
#	Process PID changes everytime it forks a child. In order to attach
#	gdb debugger to the final child process before it executes
#	MainProcessor function, a 25 seconds delay is put before it calls
#	MainProcessor().
#
#	Debugger can be attached by following the below steps:
#	$ $ASCENDENT/bin/dcpiso
#	$ ps -ef|grep dcpiso
#	$ gdb
#	gdb> attach <PID>
#	gdb> file <EXE PATH/NAME>			Eg: file dcpiso
#	gdb> break MainProcessor
#	gdb> c
#	Process stopped in MainProcessor:
#	gdb>
#
#	continue debugging the module. Check embedded server docs
#	folder for a list of gdb debugger commands.
#
#



LIBDIR = ../lib
CFLAGS = -c -g -q64 -qcpluscmt -w -I../../include
#$(CC)=gcc
$(CC)=cc
daemon.o: daemon.c 
	$(CC) $(CFLAGS) daemon.c
	@cp daemon.o $(LIBDIR)
	-@rm -f *.o

# output file name is deliberately kept daemon.o and NOT daemon_debug.o
# This is to let same file name be used in Makefile of other modules.
debug: daemon_debug.c
	$(CC) $(CFLAGS) daemon_debug.c -o daemon.o
	@cp daemon.o $(LIBDIR)
	-@rm -f *.o

clean:
	-@rm -f *.o $(LIBDIR)/daemon.o

