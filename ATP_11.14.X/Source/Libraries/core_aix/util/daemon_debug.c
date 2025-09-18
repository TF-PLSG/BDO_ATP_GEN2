/*

    NIRBHAY: Use this file when debugging using GNU GDB debugger.

    If any program calls fork() system call, it creates a new process.
    In our case, we run ATP modules as daemon processes that
    forks a child and kill the parent. When debugging with GDB, we
    actually wants to debug the child and not the parent. If GDB is
    attached from start, it doesn't automatically switches to the
    child (created by fork). Instead it will terminate as the parent
    is killed in the daemon_start() function below. There is no good
    way of switching to the forked child from gdb.

    For this reason, main() function below is tweaked a little to
    allow us to debug any ATP modules. A sleep() is added after the
    multiple forking is completed and final resulting child is ready
    to call MainProcessor().

    All ATP modules linking to this file will get 25 seconds after
    launching the module to attach gdb to the final child process.
    Use the following steps:
    - execute the module Eg:- $ASCENDENT/bin/dcpiso
    - ps -ef|grep dcpiso => note down the PID of dcpiso. By this time
    the process would have forked two childs and killed the parents. So
    the PID that you see will be the PID of resulting final process to
    debug. It will sleep for 25 seconds before calling MainProcessor.
    Attach the process to debugger by:
    - gdb
    - attach <PID>
    - file <PATH OF EXE>
    ...continuing debugging.

*/

#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#include <errno.h>
/*
#include <sys/fcntl.h>
*/
#include <termios.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <pthread.h>

#include "ntutils.h"

#define PERMS 0666

INT EndProcessSignalled;
INT MainProcessDone;

CHAR        ServiceName[12];
extern CHAR ExeName[MAX_APP_NAME_SIZE]; 

extern void MainProcessor();
extern void EndProcess();

void signal_handler( int signal )
{
   EndProcessSignalled = 1;
   sleep( 2 );
}

void daemon_start( int ignsigcld )
{

   char errBuf[256];
   register int childpid, fd;

   /* Shutdown upon `kill` command */
   if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
	    signal( SIGTERM, signal_handler );

   /* Use SIGUSR1 as a backdoor shutdown 
   signal( SIGUSR1, signal_handler ); */

   if( getppid() == 1 )
		goto out;

#ifdef SIGTTOU
   signal( SIGTTOU, SIG_IGN );
#endif

#ifdef SIGTTIN
   signal( SIGTTIN, SIG_IGN );
#endif

#ifdef SIGTSTP
   signal( SIGTSTP, SIG_IGN );
#endif

   /**
    * Ensure we're not process group leader.  This is a
    * pre-requisite to calling setsid() below:
    */
   if ( (childpid = fork() ) < 0)
   {
      sprintf(errBuf, "%s: Unable to fork first child.  Exiting.\n", ExeName);
      LogEvent(errBuf, ERROR_MSG);
      exit(-1);
   }
   else if (childpid != 0)
      exit(0);  /* parent goes bye-bye */

   setsid();

   /* Immune from pgrp leader death: */
   signal( SIGHUP, SIG_IGN );

   /**
    * Ensure we're not a session leader, and, therefore, 
    * can't re-acquire a controlling tty.
    */
   if ( (childpid = fork() ) < 0)
   {
      sprintf(errBuf, "%s: Unable to fork second child.  Exiting.\n", ExeName);
      LogEvent(errBuf, ERROR_MSG);
      exit(-1);
   }
   else if (childpid != 0)
      exit(0);  /* first child goes bye-bye */

out:

   /* Not interested in child death: */
   if( ignsigcld ) signal( SIGCLD, SIG_IGN ); 
}


void main( int argc, char *argv[] )
{
    char tmpstr[256];

    strcpy( ExeName, argv[0] );

    if( argc > 1 )
        strcpy( ServiceName, argv[1] );
    else
        GetAppName( ServiceName );

    InitEventLogging();

    sprintf( tmpstr, "Starting Ascendent Service: %s\n", ServiceName );
    LogEvent(tmpstr, INFO_MSG);

    // DELETE THE NEXT TWO LINES.
    //Give 20 seconds for user to connect the debugger.
    //sleep(20);

    daemon_start( 1 );

    //Give 25 seconds for user to connect the debugger.
    sleep(25);

    EndProcessSignalled = 0;
    MainProcessor();

    sprintf( tmpstr, "Stopping Ascendent Service: %s\n", ServiceName );
    LogEvent(tmpstr, INFO_MSG);
    EndProcess();

    ShutDownEventLogging();
 
    return;
}

    
