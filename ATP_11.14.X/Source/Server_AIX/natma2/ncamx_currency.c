/***************************************************************************
 *
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCAMX_CURRENCY.C
 *  
 * TITLE:       Amex Network Controller Currency Conversion Processor
 *  
 * DESCRIPTION: This module contains functions that handle transaction
 *              statistics: number in (RECEIVED) and number out (SENT).
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Sree Velaga
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\ncjcb\ncamx_currency.c  $
   
      Rev 1.0   Aug 25 2003 14:41:20   svelaga
   Initial revision.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "memmnger.h"
#include "ncamx_constants.h"
#include "ncamx_prototypes.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;
extern CRF01   Crf01;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;



/******************************************************************************
 *
 *  NAME:         PERFORM_CURRENCY_CONVERSION
 *
 *  DESCRIPTION:  This function takes an input money amount and converts it
 *                from its currency to that of a destination currency.  The
 *                converted amount is outputed.
 *
 *                This is done using the currency code table in shared
 *                memory.  If for some reason, the shared memory table is
 *                unavailable, then a request is made to the data server.
 *                The input parameter, subtype2, is used for data server
 *                requests.  It tells the network controller where to continue
 *                processing upon return from the data server.
 *
 *                The 2 amounts are expected to be unsigned longs.
 *
 *                The conversion rates are for US Dollars.  The algorithm
 *                to convert from the source to the destination currency
 *                is as follows:
 *
 *                   US Amount   = Txn amount / source conversion rate
 *                   Dest Amount = US Amount * dest conversion rate
 *
 *                ----------------------------------------------------
 *                This can be simplified as follows:
 *
 *                   Dest Amount = (Txn Amt * dest rate) / source rate
 *                ----------------------------------------------------
 *
 *
 *  INPUTS:       amt_in   - Amount to be converted (unsigned long)
 *                cc_in    - Currency code of amt_in
 *                cc_out   - Currency code that amt_in will be converted to
 *
 *  OUTPUTS:      amt_out  - Value of amt_in converted to currency cc_out
 *                err_buf  - Reason for failure
 *
 *  RTRN VALUE:   True if successful,
 *                False if it failed,
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf )
{
   INT     ret_val = true;
   INT     currency_code_size = sizeof(Crf01.primary_key.currency_code);
   XINT    rt_memaccess;
   LONG    rt_record;
   double  d_amt_out;
   double  rate_in;
   double  rate_out;
   Crf01_Size = sizeof(CRF01); // TF Phani
   /* If amount is zero, do not bother converting. */
   if ( amt_in == 0 )
      *amt_out = 0;
   else
   {
      /* If currency codes are the same, do not bother converting. */
      if ( 0 == strcmp(cc_in, cc_out) )
         *amt_out = amt_in;
      else
      {
         /* Source currency code cannot be empty. */
         if ( cc_in[0] == 0x00 )
         {
            ret_val = false;
            strcpy( err_buf,
                   "Source currency code is empty. Cannot convert currency." );
         }

         /* Do no conversion if destination currency code is empty. */
         else if ( cc_out[0] == 0x00 )
         {
            *amt_out = amt_in;
         }
         else
         {
            /* Continue with conversion.
             * Make sure we can access the shared memory table - CRF01.
             */
            rt_memaccess = MemAccess( CRF01_TABLE );
            if ( rt_memaccess < 0)
            {
               sprintf( err_buf,
                       "Unable to access shared memory table %s. Cannot convert currency.",
                        CRF01_TABLE );
               ret_val = false;
            }

            /* We are able to access the table.
             * But no need to here if currency code is in US Dollars.
             */
            else if ( 0 == strcmp(cc_in, USA_CURRENCY_CODE) )
            {
               rate_in = 1.0;
            }

            /* Need to get the 'in' a.k.a. 'source' currency conversion rate. */
            else
            {
			   memset( &Crf01, 0x00, Crf01_Size );
               strcpy( Crf01.primary_key.currency_code, cc_in );
               rt_record = FindMemRecord( (pBYTE)&Crf01, currency_code_size,
                                          Crf01_Size, CRF01_TABLE );
               if ( rt_record == -1L )
               {
                  /* Currency code not found in CRF01 table */
                  sprintf( err_buf,
                         "Currency conversion (CRF01) record not found for currency code '%s'",
                          cc_in );
                  ret_val = false;
               }
               else if ( rt_record < 0L )
               {
                  /* There is some other kind of problem with shared memory. */
                  ret_val = false;
                  sprintf( err_buf,
                          "Shared memory err %d when attempting to get conversion rate for currency code %s",
                           rt_record, cc_in );
               }
               else
               {
                  rate_in = atof( Crf01.conversion_rate );
               }
            }

            /* We have the conversion rate for cc_in (if no error).
             * Now use same method to get it for cc_out (destination curr code).
             */
            if ( true == ret_val )
            {
               /* If it's in US Dollars, set rate to 1. */
               if ( 0 == strcmp(cc_out, USA_CURRENCY_CODE) )
               {
                  rate_out = 1.0;
               }

               /* Need to get the 'out' a.k.a. 'dest' curr conversion rate. */
               else
               {
                  memset( &Crf01, 0x00, Crf01_Size );
                  strcpy( Crf01.primary_key.currency_code, cc_out );
                  rt_record = FindMemRecord( (pBYTE)&Crf01, currency_code_size,
                                             Crf01_Size, CRF01_TABLE );
                  if ( rt_record == -1L )
                  {
                     /* Currency code not found in CRF01 table */
                     sprintf( err_buf,
                         "Currency conversion (CRF01) record not found for currency code '%s'",
                          cc_out );
                     ret_val = false;
                  }
                  else if ( rt_record < 0L )
                  {
                     /* There's some other kind of problem with shared memory.*/
                     ret_val = false;
                     sprintf( err_buf,
                          "Shared memory err %d when attempting to get conversion rate for currency code %s",
                           rt_record, cc_out );
                  }
                  else
                  {
                     rate_out = atof( Crf01.conversion_rate );
                  }
               }
               if ( true == ret_val )
               {
                  /* ------------------------------------------
                   * No errors, we have all the information.
                   * Calculate the converted amount: d_amt_out.
                   * ------------------------------------------
                   */
                  if ( rate_in == 0.0 )
                     d_amt_out = 0.0;
                  else
                     d_amt_out = (double)amt_in * (rate_out / rate_in);

                  /* Round it off and convert to integer: amt_out */
                  *amt_out = (DWORD)(d_amt_out + 0.5);
               }
            }
         }
      }
   }

   return( ret_val );
}
