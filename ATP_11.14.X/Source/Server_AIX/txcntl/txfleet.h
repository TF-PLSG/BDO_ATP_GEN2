/**************************************************************************************************
*  
* MODULE:      Txfleet.h
*  
* TITLE:       
*  
* DESCRIPTION:  
*
* APPLICATION: TXCNTL
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
*
* $Log:   N:\PVCS\PTE\Equitable\txcntl\txfleet.h  $
   
      Rev 1.5   Dec 30 1999 13:32:58   ddabberu
   aix_build
   
      Rev 1.4   Jun 01 1999 17:14:16   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.3   May 13 1999 14:53:18   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.2   Apr 01 1999 11:20:42   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.1   Feb 23 1999 11:11:38   sdasgupt
    
   
      Rev 1.0   Feb 12 1999 18:54:36   sdasgupt
   Initial version
*******************************************************************************/
#ifndef _TxFleet
#define _TxFleet

#include <stdio.h>
#include <string.h>
#include "basictyp.h"


BYTE TxFleet_Validate_Fleet_Cards (void) ;

#endif
