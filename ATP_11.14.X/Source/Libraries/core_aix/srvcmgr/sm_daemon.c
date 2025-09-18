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

#include "ntutils.h"
#include "pteipc.h"

#define PERMS 0666

INT EndProcessSignalled;
INT MainProcessDone;

CHAR ServiceName[12];
extern CHAR ExeName[100]; 

extern void MainProcessor();
extern void EndProcess();

void signal_handler( int signal )
{

   /*printf( "in signal handler\n" );*/
   EndProcessSignalled = 1;
   pteipc_sleep(2000);
}


void daemon_start( int ignsigcld )
{

   register int childpid, fd;

   /* Shutdown upon `kill` command */
   if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
	    signal( SIGTERM, signal_handler );

   /* Use SIGUSR1 as a backdoor shutdown */
   signal( SIGUSR1, signal_handler );

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
      printf("%s: Unable to fork first child.\n", ExeName);
   else if (childpid != 0)
      exit(0);  /* parent goes bye-bye */

/*
   if( setpgrp() == -1 )
	printf( "can't change process group\n" );
*/

   setsid();

   /* Immune from pgrp leader death: */
   signal( SIGHUP, SIG_IGN );

   /**
    * Ensure we're not a session leader, and, therefore, 
    * can't re-acquire a controlling tty.
    */
   if ( (childpid = fork() ) < 0)
      printf("%s: Unable to fork second child.\n", ExeName);
   else if (childpid != 0)
      exit(0);  /* first child goes bye-bye */

out:
   /*for( fd = 0; fd < NOFILE; fd++ ) close( fd );*/

   /*chdir( "/" );*/

   /*umask( 0 );*/

   /* Not interested in child death: */
   if( ignsigcld ) signal( SIGCLD, SIG_IGN ); 
}


void main( int argc, char *argv[] )
{
    strcpy( ExeName, argv[0] );

    if( argc > 1 )
        strcpy( ServiceName, argv[1] );
    else
        GetAppName( ServiceName );

    InitEventLogging();

    printf( "Starting Ascendent Service: %s\n", ServiceName );
    daemon_start( 1 );

    EndProcessSignalled = 0;
    MainProcessor();

    printf( "Stopping Ascendent Service: %s\n", ServiceName );
    EndProcess();

    ShutDownEventLogging();
 
    return;
}

    
