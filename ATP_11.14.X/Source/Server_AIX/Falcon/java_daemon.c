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
#include <unistd.h>
#include "ntutils.h"


//#define JAVA_HOME       "/usr/java6_64"

/*void java_signal_handler( int signal )
{
   printf("recived kill signal\n");
   javaEndProcessSignalled = 1;
   sleep( 1 );
}*/

void daemon_start( int ignsigcld )

{
	char errBuf[256];
	register int childpid, fd;



   /* Shutdown upon `kill` command */

  /* if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
	{
	   signal( SIGTERM, java_signal_handler );
	   printf("Kill signal has been Set\n");
	  
	}*/



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

      sprintf(errBuf, "Unable to fork first child.  Exiting.\n");

      //LogEvent(errBuf, ERROR_MSG);

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

      sprintf(errBuf, "Unable to fork second child.  Exiting.\n");

      //LogEvent(errBuf, ERROR_MSG);

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

    char tmpstr[256],buffer[500],java_buff[500];
	char *java_hm;
	char *bin_path;
	CHAR  	Version[] = "ATP_11.1.0";

	daemon_start( 1 );
	
	java_hm=getenv("ASCENDENT_JAVA_HOME_FALCON");
	strcpy(java_buff,java_hm);
	strcat(java_buff,"/bin/java"); 
	//printf("java_path=%s\n",java_buff);

	bin_path=getenv( "ASCENDENTROOT" );
	strcpy(buffer,bin_path);
	strcat(buffer,"/bin/falcon.jar");
	//printf("bin_path=%s",buffer);
    //execl("/usr/java6_64/bin/java", "java", "-jar", "/home/veena/ascendent/bin/falcon.jar",  argv[1],0);
//	execl(java_buff, "java", " -Xms10m -Xmx10m -jar", buffer,  argv[1],0);
//   execl(java_buff, "java","-Xms10m","-Xmx256m", "-jar", buffer,  argv[1],0);
//-Dcom.ibm.CORBA.Debug.Output=/dev/null
	execl(java_buff, "java","-Xms10m","-Xmx256m", "-jar", "-Dcom.ibm.CORBA.Debug.Output=/dev/null",buffer,  argv[1],0);

   return;

}



    
