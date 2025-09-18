/**************************************************************************************************
*  
* MODULE:      Txfleet.C
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
* $Log:   N:\PVCS\PTE\Equitable\txcntl\txfleet.c  $
   
      Rev 1.11   Jul 09 2003 14:32:18   dirby
   Corrected bug in function Validate_Fleet_Cards; it was not properly
   incrementing an index in a FOR loop, so product codes listed
   in the cardholder record would never be found, except the first one.
   SCR 1139
   
      Rev 1.10   Jun 05 2001 08:55:48   SYARLAGA
   When clicked  detail of a decline transaction,modified  the response text data to reflect the entire message 
   
      Rev 1.9   Mar 16 2000 17:37:20   iarustam
   Removed debug messages
   
      Rev 1.8   Jan 14 2000 17:28:42   iarustam
   bug fixes
   
      Rev 1.7   Dec 30 1999 13:32:58   ddabberu
   aix_build
   
      Rev 1.6   Jul 12 1999 19:03:08   sdasgupt
    
   
      Rev 1.5   Jun 01 1999 17:14:16   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.4   May 13 1999 14:53:16   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.3   Apr 01 1999 11:20:42   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.2   Mar 12 1999 14:57:30   sdasgupt
   1) updated code so that ADVICE messages
   do not get PIN verification or MAG STRIPE
   verification.
   
   2) Updated checks for empty string fields
   to check for strlen > 0 ....
   
   3) updated to use the field julian_date for
   when max_pin_retries is hit and to clear it
   when on the next day, card holder trys again.
   
   4) uncommented call to check for GUI setting
   to know it card is allowed to get cash_advance
   and other options set in GUI.
   
   GB 3_12_99
   
      Rev 1.1   Feb 23 1999 11:11:36   sdasgupt
    
   
      Rev 1.0   Feb 12 1999 18:54:36   sdasgupt
   Initial version
*******************************************************************************/
#include <stdlib.h>				// for atoi function prototype
#include <math.h>				// for HUGE_VAL definition
#include "txfleet.h"
#include "txcntl.h"
#include "txservice.h"
#include "genutil.h"
#include "ntutils.h"
#include "txutils.h"

extern AUTH_TX Auth_Tx ;



BYTE TxFleet_Validate_Fleet_Cards (void)
{
   /* Check if the product code of the transaction
    * is in the cardholder allowed product codes.
    */
   BYTE  retval = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
   CHAR  strTemp[256] = {0} ;
   INT   index = 0 ;
   INT   allowed_product_code_length;

   allowed_product_code_length = strlen( Auth_Tx.CCF02P_details.product_codes );

   if ((allowed_product_code_length == 0) ||
       (strlen(Auth_Tx.TLF01_details.product_code) == 0) )
   {
      retval = NO_AUTH_ERROR ;
   }
   else
   {
      for (index = 0; index < allowed_product_code_length; index = index+2)
      {
         if ( !strncmp(Auth_Tx.CCF02P_details.product_codes + index,
                       Auth_Tx.TLF01_details.product_code, 2) )
            retval = NO_AUTH_ERROR ;
      }

      if ( retval != NO_AUTH_ERROR )
      {
         /* Txn product code is not in the cardholder allowed product codes */
         sprintf (strTemp, "Fleet card: Inval ProductCode %s\n", 
                  Auth_Tx.TLF01_details.product_code) ;

         txcntl_log_message( 1, 2, strTemp, "TxFleet_Validate_Fleet_Cards", 1 );
         txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_PRODUCT_CODE,
                                                  INVALID_TRANSACTION );
      }
   }
   return( retval );
} /* TxFleet_Validate_Fleet_Cards */

