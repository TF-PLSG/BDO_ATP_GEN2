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
int err_code;
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
      syslog(LOG_ERR, errBuf);
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
      syslog(LOG_ERR, errBuf);
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
        strcpy( ServiceName, "xcomdaemon" );

    InitEventLogging();

    sprintf( tmpstr, "Starting Ascendent Service: %s\n", ServiceName );
    syslog(LOG_INFO, tmpstr);
    daemon_start( 1 );

    EndProcessSignalled = 0;
    MainProcessor();

    sprintf( tmpstr, "Stopping Ascendent Service: %s\n", ServiceName );
    syslog(LOG_INFO, tmpstr);
    EndProcess();

    ShutDownEventLogging();
 
    return;
}

    
