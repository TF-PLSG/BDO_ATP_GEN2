/****************************************************************************************

   Module: srvcmgr.c

   Title: Pinnacle Services Manager

   Description: This is the common header file for Service Manager and Service Monitor.

   $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SRVCINFO.H  $
   
      Rev 1.5   Dec 01 2003 11:30:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:22   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:02   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:22   iarustam
    
   
      Rev 1.0   27 May 1998 15:59:10   rmalathk
   Initial Revision
   
*****************************************************************************************/
#ifndef __SRVCINFO__
#define __SRVCINFO__

#include "basictyp.h"


# define MAX_SERVICES 25 

typedef struct service_inf
{
    CHAR exe_name[100];
    CHAR service_name[12];
    CHAR display_name[25];
    CHAR que_name[12];
    BYTE  disable;
    BYTE  running;
    BYTE  trace_on;
    BYTE  pid[12];
	BYTE  encryption_on;
	BYTE  mra_on;
	BYTE  falcon_on;
} SERVICE_INFO, *pSERVICE_INFO;

#endif

