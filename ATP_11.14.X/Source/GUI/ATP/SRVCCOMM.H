/****************************************************************************************

   Module: srvccomm.h

   Title: Pinnacle Services Monitor

   $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SRVCCOMM.H  $
   
      Rev 1.5   Dec 01 2003 11:30:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:20   iarustam
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
   
      Rev 1.1   Jan 22 1999 11:00:02   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:20   iarustam
    
   
      Rev 1.0   27 May 1998 16:54:22   rmalathk
   Initial Revision
   
*****************************************************************************************/
#ifndef SRVCCOMM
#define SRVCCOMM

#include "srvcinfo.h"

BYTE srvccomm_get_service_table( SERVICE_INFO services_table[], pINT num_services,
                                   pCHAR err_msg );
BYTE srvccomm_control_service( pCHAR service_name, BYTE action, pCHAR error_msg );

BYTE srvccomm_process_all( BYTE action, pCHAR error_msg );

BYTE srvccomm_confirm_status( pCHAR error_msg );

BYTE srvccomm_ping_service( pCHAR que_name, pCHAR error_msg );

INT srvccomm_change_connection( pCHAR machine_id, pCHAR instance );

INT srvccomm_disconnect();
#endif


