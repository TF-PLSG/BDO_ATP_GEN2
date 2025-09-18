/**************************************************************************************
 
MODULE:      myutils.h
Copyright (c) 2005, Hypercom, Inc.
All Rights Reserved. 

TITLE:       Utility file.
 
DESCRIPTION:  

APPLICATION: TXCNTL

AUTHOR:  Sanjoy Dasgupta

REVISION HISTORY:


$Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\txcntl\myutils.h  $
   
      Rev 1.9   Dec 09 2005 15:39:16   dirby
   Changed strEqual from a function to a Macro - more efficient.
   SCR 18700
   
      Rev 1.8   Dec 30 1999 13:31:54   ddabberu
   aix-build
   
      Rev 1.7   Jul 12 1999 19:03:00   sdasgupt
    
   
      Rev 1.6   Jun 01 1999 17:14:12   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.5   May 13 1999 14:53:12   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.4   Apr 29 1999 15:35:08   sdasgupt
   Bug fixes
   
      Rev 1.3   Apr 15 1999 15:33:36   sdasgupt
   1. Voice-auth Overrides
   2. Voice-auth Reversals
   3. Added Voice-auth error messages
   4. Fixed Deferred balance account update
   5. Fixed error message with the deferred min amount
   6. Fixed bug where a Cash Advance transaction was ignoring the Merchant Capabilites screen
   
      Rev 1.2   Apr 01 1999 11:20:38   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.1   Feb 23 1999 11:11:28   sdasgupt
    
   
      Rev 1.0   Feb 12 1999 18:54:26   sdasgupt
   Initial version
****************************************************************************************/
#include "txcntl.h"
void strTrim (char* strSource, char* strDestination) ;
int Convert_Amt_To_Auth_Tx_String_Format 
                              (double nAmount, char* strAmount) ;
int Convert_Amt_To_Auth_Tx_String_Format_CF
                              (double nAmount, char* strAmount) ;
void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character) ;

void create_rrn (pCHAR rrn) ;
void get_date (pCHAR time_date, pCHAR date_str) ;
void get_time (pCHAR time_date, pCHAR time_str) ;
void calc_julian_day (pCHAR date_str, pCHAR julian_str) ;
BYTE FieldIsNumeric (char str[]) ;
