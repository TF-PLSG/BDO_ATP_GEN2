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
	char tmpstr[256] = {0};
	char buffer[1500] = {0};
	char buffer1[1500] = {0};
	char java_buff[500] = {0};

	char javaAppName[128] = {0};
	char javaServiceName[128] = {0};

	char *java_hm;
	char *bin_path;
	CHAR  	Version[] = "ATP_11.6.0";

	int 	PID = 0;
	int 	service_name_len = 0;

	daemon_start( 1 );
	
	java_hm=getenv("ASCENDENT_JAVA_HOME_FALCON");
	strcpy(java_buff,java_hm);
	strcat(java_buff,"/bin/java"); 

	bin_path=getenv( "ASCENDENTROOT" );
	strcpy(buffer,bin_path);
	strcpy(buffer1,bin_path);
	strcat(buffer,"/bin/faisnd.jar");
	strcat(buffer1,"/bin/boss-falcon_0.0.5-SNAPSHOT.jar");

	sprintf(javaAppName, "-Dservice.app.name=faisnd");
	sprintf(javaServiceName, "-Dservice.service.name=%s\0", argv[1]);

	service_name_len = strlen(argv[1]);

	if (service_name_len == 6 &&
	(0 == strncmp(argv[1],"faisnd",6)))
	{
		PID = fork();
		if(0 == PID)
		{
			/*child process should call boos Falcom jar*/
			execl(java_buff, "java","-Xmx1024m","-XX:+UseG1GC","-XX:MaxGCPauseMillis=200",
					"-XX:InitiatingHeapOccupancyPercent=45", "-XX:ConcGCThreads=2", "-jar", buffer1,  argv[1],0);
		}
		if (PID > 0)
		{
			execl(java_buff, "java","-Xms256m","-Xmx512m","-XX:+UseG1GC","-XX:MaxGCPauseMillis=200",
					"-XX:InitiatingHeapOccupancyPercent=45", "-XX:ConcGCThreads=2",
					"-Dcom.ibm.CORBA.Debug.Output=/dev/null",
					javaAppName, javaServiceName,  "-jar" ,buffer,0);
		}
	}
	else
	{
		execl(java_buff, "java","-Xms256m","-Xmx512m", "-Dcom.ibm.CORBA.Debug.Output=/dev/null", "-XX:NativeMemoryTracking=detail", "-XX:+UnlockDiagnosticVMOptions", "-XX:+PrintNMTStatistics", javaAppName, javaServiceName,  "-jar" ,buffer,0);
	}
	return;
}

    
