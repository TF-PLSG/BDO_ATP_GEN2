

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "ptemsg.h"
#include "pteipc.h"
#include "txtrace.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "txutils.h"
#include "genutil.h"
#include "basictyp.h"
#include "app_info.h"
#include "ptetime.h"


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

      /* LogEvent(errBuf, ERROR_MSG); */

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

      /* LogEvent(errBuf, ERROR_MSG); */

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
    char buffer[500] = {0};
    char buffer1[500] = {0};
    char java_buff[500] = {0};
	char *java_hm;
	char *bin_path;
	/* char 	version_message_buffer[500] = {0}; */
	CHAR  	Version[] = "ATP_11.1.0";
	int 	PID = 0;
	int 	service_name_len = 0;
	
	daemon_start( 1 );

	java_hm=getenv("ASCENDENT_JAVA_HOME_MRACON");
	strcpy(java_buff,java_hm);
	strcat(java_buff,"/bin/java"); 

	bin_path=getenv( "ASCENDENTROOT" );
	strcpy(buffer,bin_path);
	strcpy(buffer1,bin_path);
	strcat(buffer,"/bin/mracon.jar");
	strcat(buffer1,"/bin/boss-0.0.5-SNAPSHOT.jar");

	/*sprintf( version_message_buffer,
	         "Starting MRACON Service with version %s",
	         Version );*/


	service_name_len = strlen(argv[1]);
	printf("service lenght %d \n",service_name_len);
	if (service_name_len == 6 &&
		(0 == strncmp(argv[1],"mracon",6)))
	{

		PID = fork();
		if(0 == PID)
	{
		/*child process should call boos jar*/
			/* execl("/usr/java8_64/bin/java", "java", "-jar", buffer1,  argv[1],0); */
			execl(java_buff, "java","-Xmx1024m", "-jar", buffer1,  argv[1],0);
		}
		if (PID > 0)
		{
			printf("Run mracon jar \n");
			/* execl("/usr/java8_64/bin/java", "java", "-jar", buffer, argv[1],0); */
			 execl(java_buff, "java","-Xms10m","-Xmx256m", "-jar", "-Dcom.ibm.CORBA.Debug.Output=/dev/null",buffer,  argv[1],0);
			/* execl(java_buff, "java", "-jar", buffer, argv[1],0); */
		}
	}
	else
	{
		execl(java_buff, "java", "-jar", buffer, argv[1],0);
	}
   return;

}
