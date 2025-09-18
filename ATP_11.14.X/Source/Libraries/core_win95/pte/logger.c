/***************************************************************************

         Module: logger.c

          Title: Logger Command APIs

    Description: API's to send restart/refeed commands to logger
    
    $Log:   N:\PVCS\PTE\CORE\PTE_LIB\LOGGER.C  $
   
      Rev 1.10   Oct 15 1999 11:12:20   MSALEH
   Move SSL_SUPPORT to preprocessot definition
   section of project files
   
      Rev 1.9   Sep 28 1999 14:40:46   MSALEH
   AIX Modifications
   
      Rev 1.8   May 19 1998 09:56:08   drodden
   Changed to use the new logger private message types/subtypes.
   
   
      Rev 1.7   17 Feb 1998 15:59:32   MSALEH
   1) define a new function to get the logger
   queue name from the registry
   2) use the function when sending refeed/restart
   transactions
   
      Rev 1.6   Jan 29 1998 14:35:40   drodden
   Remove an unreferenced variable from SendBePrimaryCmd to
   get rid of compiler warnings.
   
   
      Rev 1.5   19 Jan 1998 16:08:18   MSALEH
   added SendBePrimaryCmd(loggerquename, errmsg)
   
      Rev 1.4   Jan 16 1998 08:14:34   drodden
   Changed to use the st2_none defined value rather than null_ptr.
   Changed to use strlen(trid)+1 to include the null terminator.
   
   
      Rev 1.3   Jan 14 1998 11:00:58   drodden
   Created a header and prototypes for logger.c
   Changed the routines to return a BOOLEAN type rather 
   than INT so it is clear what they are returning.
   
   
      Rev 1.2   10 Jan 2000 14:07:26   MSALEH
   switch order or queues to be proper, 
   from (application queue) to (logger queue)

****************************************************************************/

#include <stdio.h>

#include "logger.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ntutils.h"

/************************************************************************/
/************************************************************************/
BOOLEAN SendRestartCmd ( pCHAR errmsg )
{
   LONG        RetCode;
   CHAR        logger_que_name[255];

   pPTE_MSG    pPteMsg;
   pPTE_MSG    p_msg_in;

   GetLoggerQueName(logger_que_name);

   pPteMsg = ptemsg_build_msg( MT_LOGGER_REPLY, 
                               ST1_LOG_RESTART,
                               ST2_NONE,
                               logger_que_name,
                               application_que_name,
                               NULL_PTR,
                               0,
                               0);
  
   if ( pPteMsg != NULL_PTR )
   {
      p_msg_in = pteipc_send_receive( pPteMsg, logger_que_name, application_que_name, &RetCode );
      
      if ( p_msg_in == NULL_PTR )
      {
         pteipc_get_errormsg(RetCode, errmsg);
         return ( false );
      }

      free(p_msg_in);
      return ( true );
   }
   else
   {
      sprintf( errmsg, "Unable to build message for restart command to logger" );
      return ( false );
   }
}



/************************************************************************/
/************************************************************************/
BOOLEAN SendRefeedCmd ( pCHAR trid, 
                        pCHAR errmsg )
{
   LONG        RetCode;
   CHAR        logger_que_name[255];
   pPTE_MSG    pPteMsg;

   GetLoggerQueName(logger_que_name);

   pPteMsg = ptemsg_build_msg( MT_LOGGER_REPLY, 
                               ST1_LOG_REFEED,
                               ST2_NONE,
                               logger_que_name,
                               application_que_name,
                               trid,
                               strlen(trid)+1,
                               0);
  
   if ( pPteMsg != NULL_PTR )
   {
      RetCode = pteipc_send( pPteMsg, logger_que_name );
      
      if ( RetCode < 0 )
      {
         pteipc_get_errormsg(RetCode, errmsg);
         return ( false );
      }
    
      return ( true );
   }
   else
   {
      sprintf( errmsg, "Unable to build message for refeed command to logger" );
      return ( false );
   }
}


/************************************************************************/
/************************************************************************/
BOOLEAN SendBePrimaryCmd ( pCHAR loggerquename, pCHAR errmsg )
{
   LONG        RetCode;
   pPTE_MSG    pPteMsg;


   pPteMsg = ptemsg_build_msg( MT_PRIVATE_LOGGER_REPLY, 
                               ST1_PRV_BE_PRIMARY,
                               ST2_NONE,
                               loggerquename,
                               application_que_name,
                               NULL_PTR,
                               0,
                               0);
  
   if ( pPteMsg != NULL_PTR )
   {
      RetCode = pteipc_send( pPteMsg, loggerquename );
      
      if ( RetCode < 0 )
      {
         pteipc_get_errormsg(RetCode, errmsg);
         return ( false );
      }
    
      return ( true );
   }
   else
   {
      sprintf( errmsg, "Unable to build message for refeed command to logger" );
      return ( false );
   }
}




