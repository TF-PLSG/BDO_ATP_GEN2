/******************************************************************************
  
        Module: console.c
  
         Title: Main routine for console based apps
  
   Description: 

   $Log:   N:\PVCS\PTE\SYSTEM\CONSOLE.C  $
   
      Rev 1.0   19 Dec 1997 11:37:52   rmalathk
   Initial Revision
   
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NTUtils.h"


extern int Start();
extern void Stop ();
char ServiceName[12];


void main( int argc, char *argv[] )
{
	if( argc > 1 )
		strcpy( ServiceName, argv[1] );
	else
		GetAppName( ServiceName );

	if( !Start () )
		printf( "Error starting ServiceManager\n\n" );

	getchar();

	Stop ();
}