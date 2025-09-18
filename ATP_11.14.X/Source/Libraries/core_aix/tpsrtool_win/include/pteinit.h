/******************************************************************************
  
        Module: pininit.h
  
         Title: Pinnacle Startup/Shutdown Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\SYSTEM\PTEINIT.H  $
   
      Rev 1.2   18 Dec 1997 11:31:38   rmalathk
   modified the pinnacle_startup routine to take
   in a service_name as an additional parameter.
   
   
      Rev 1.1   Dec 01 1997 16:48:30   drodden
   Changed the startup to have a single parameter: the
   type of application this is (a enumuerated value).
   Changed the shutdown to not require any parms.
   
   
      Rev 1.0   Oct 23 1997 11:42:18   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#ifndef PININIT_H
#define PININIT_H

#include "basictyp.h"


typedef enum
{
   pteinit_single_instance_app,
   pteinit_multi_instance_app,
   pteinit_gui_app
} PTEINIT_APP_TYPES;



/******************************************************************************
*  function pteinit_pinnacle_startup
*
*  This function performs the startup initialization for a Pinnacle PTE
*  application.  Call it once at the beginning of the program.  If the init
*  completes successfully, it returns true, otherwise it will log any errors
*  and return false.  It it returns false, your application should not continue.
*
*  Parameters:
*     app_type........flag to indicate what type your application is.
*     service_name....for multiple instance apps only, will be ignored for other apps
*
******************************************************************************/
BOOLEAN pteinit_pinnacle_startup ( PTEINIT_APP_TYPES app_type, pCHAR service_name );


/******************************************************************************
*  function pteinit_pinnacle_shutdown
*
*  This function performs the shutdown/exit cleanup for a Pinnacle PTE
*  application.  Call it as the very last thing in your exit_application()
*  routine.
*
*  Parameters: none
*
******************************************************************************/
void pteinit_pinnacle_shutdown ();


#endif


