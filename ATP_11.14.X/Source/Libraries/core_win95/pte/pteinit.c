/******************************************************************************
  
        Module: pteinit.c
  
         Title: Pinnacle Startup/Shutdown Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTEINIT.C  $
   
      Rev 1.3   Feb 01 2000 10:12:32   MSALEH
   changes app_name variable size to MAX_APP_NAME_SIZE
   
      Rev 1.2   18 Dec 1997 11:31:38   rmalathk
   modified the pinnacle_startup routine to take
   in a service_name as an additional parameter.
   
   
      Rev 1.1   Dec 01 1997 16:48:32   drodden
   Changed the startup to have a single parameter: the
   type of application this is (a enumuerated value).
   Changed the shutdown to not require any parms.
   
   
      Rev 1.0   Oct 23 1997 11:42:16   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#include <stdlib.h>

#include "pteinit.h"
#include "pteipc.h"
#include "ntutils.h"



PRIVATE PTEINIT_APP_TYPES app_type_saved;



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
BOOLEAN pteinit_pinnacle_startup ( PTEINIT_APP_TYPES app_type, pCHAR service_name )
{
   CHAR  app_name [MAX_APP_NAME_SIZE];
   
   
   GetAppName (app_name);
   app_type_saved = app_type;
   
   /*
    * Based on the app type, set up the ipc communications.
    */
   switch (app_type)
   {
      case pteinit_single_instance_app :
         if (pteipc_init_single_instance_app (app_name, "pte_ipc") == false)
            return (false);
         break;

      case pteinit_multi_instance_app :
         if (pteipc_init_multiple_instance_app (app_name, service_name, "pte_ipc") == false)
            return (false);
         break;

      case pteinit_gui_app :
         if (pteipc_init_gui_app (app_name, "pte_ipc") == false)
            return (false);
         break;

      default :
         if (pteipc_init_single_instance_app (app_name, "pte_ipc") == false)
            return (false);
         break;
   }
   
   return (true);
}



/******************************************************************************
*  function pteinit_pinnacle_shutdown
*
*  This function performs the shutdown/exit cleanup for a Pinnacle PTE
*  application.  Call it as the very last thing in your exit_application()
*  routine.
*
*  Parameters:
*     app_name...........name of your application.
*     multi_instance.....same boolean flag that was passed in to the startup.
*
******************************************************************************/
void pteinit_pinnacle_shutdown ()
{
   /*
    * Based on the app type, clean up the ipc communications.
    */
   switch (app_type_saved)
   {
      case pteinit_single_instance_app :
         pteipc_shutdown_single_instance_app ();
         break;

      case pteinit_multi_instance_app :
         pteipc_shutdown_multiple_instance_app ();
         break;

      case pteinit_gui_app :
         pteipc_shutdown_gui_app ();
         break;

      default :
         pteipc_shutdown_single_instance_app ();
         break;
   }
}




