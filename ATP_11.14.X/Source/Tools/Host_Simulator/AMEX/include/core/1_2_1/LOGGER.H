/******************************************************************************
  
        Module: logger.h
  
         Title: Logger Commnand APIs
  
   Description: API's to send commands to logger

   $Log:   N:\PVCS\PTE\SYSTEM\LOGGER.H  $
   
      Rev 1.1   19 Jan 1998 16:08:18   MSALEH
    
   
      Rev 1.0   Jan 14 1998 11:01:02   drodden
   Created a header and prototypes for logger.c
   Changed the routines to return a BOOLEAN type rather 
   than INT so it is clear what they are returning.
   

****************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include "basictyp.h"


/************************************************************************/
/************************************************************************/
BOOLEAN SendRestartCmd ( pCHAR errmsg );



/************************************************************************/
/************************************************************************/
BOOLEAN SendRefeedCmd ( pCHAR trid, 
                        pCHAR errmsg );

/************************************************************************/
/************************************************************************/
BOOLEAN SendBePrimaryCmd (  pCHAR loggerquename, 
                            pCHAR errmsg );

#endif