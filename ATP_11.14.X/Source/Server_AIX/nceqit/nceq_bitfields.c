/***************************************************************************
*
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      NCEQ_BITFIELDS.C
*  
* TITLE:       Equitable Network Control Data Field Processor
*  
* DESCRIPTION: This module contains Equitable Bank specific functions
*              to build or parse Equitable host data fields.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      D. Irby
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_bitfields.c  $

	  Rev 1.67   Mar 31 2009 00:22:00   Ajay S T
   1)Updated the code to set zeros to field 28 if not present 
   09-25 VISA Mandate changes

		Rev 1.66   Feb 27 2009 17:22:00   Girija Y TF
   1) Changed the code for Fld 37 for POS REVERSAL msg
   08-40 ATP - EQIT INTERFACE

      Rev 1.65   Jan 28 2009 16:12:00   Girija Y TF
   1) Changed the code for Fld 32 for 0422 request msg
   2) Added code for Fld 95 to pass dispensed aomunt from VISA for 0420 msg
   08-40 ATP - EQIT INTERFACE

	Rev 1.64   Jan 22 2009 14:45:00   Girija Y TF
   1) Reverted back changes for Fld 48 for 0422 request msg
   08-40 ATP - EQIT INTERFACE

    Rev 1.63   Jan 20, 2009 19:15:00  Girija Y TF
	Updated version to 4.4.1.19.
	Changed the code to pass msg type 0422 for the fld 25,32,62

	Rev 1.62   Jan 09 2009 10:00:00   Girija Y, TF
   Updated version to 4.4.1.16
   08-40, ATP - EQIT Interface.
   Changed the code for Fld 63 for 220/600 message types.
   Added function gmt_get_time() to convert manila timestamp to GMT.
   Chaged the function move_field_7() for passing GMT timestamp to other hosts.
   
	 Rev 1.61   Jan 06 2009 19:47:00   Girija Y, TF
   Updated version to 4.4.1.14
   08-40, ATP - EQIT Interface.
   Changed the code for Fld 63 to log Monitor message in case of invalid length.
   Changed function move_request_field63() to fix parsing error for Fld 44,63.

      Rev 1.60   Oct 12 2005 14:46:54   dirby
   1.  Modified DE3 to be '22' for Reload Voids.
   2.  Modified DE11 to use orig stan for voids.
   SCR 17660
   
      Rev 1.59   Oct 11 2005 16:28:28   dirby
   Modified field 3 (processing code) to go out as all zeroes for voids.
   This goes along with the requirement to have voids go to the host
   as reversals.
   SCR 17660
   
      Rev 1.58   Nov 18 2004 17:29:04   dirby
   Corrected a bug that was created in the last revision.  Forgot to
   copy the auth code into the host structure.
   SCR 13181
   
      Rev 1.57   Nov 10 2004 15:53:16   dirby
   1.  Modified to set response field 4 to zeroes for Bal Inq's.
   2.  Modified response field 38 to not overwrite an approval code.
       Balance Inquiries were doing this.  Restructured that whole
       function.
   SCR 13065
   
      Rev 1.56   Oct 01 2004 15:23:40   dirby
   Modified request field 11 to use original stan for Reload Voids.
   This is the stan of the Reload Confirm; field 11 must contain same
   value in reversals as the txn being reversed.
   SCR 12626
   
      Rev 1.55   Sep 24 2004 13:34:20   dirby
   Modified to use available balance for reload txns.
   SCR 12503
   
      Rev 1.54   Sep 23 2004 16:53:58   dirby
   Modified to put balance information into tlf01.orig_amount for reloads.
   SCR 12503
   
      Rev 1.53   Jul 14 2004 14:26:56   dirby
   Modified field 3 to support processing codes that start with "21".
   This is for reload transactions.
   SCR 1287
   
      Rev 1.52   Apr 20 2004 14:36:02   dirby
   Modified building request field 90 to not have a subfield for
   currency code for ncmlnk - it is there only for nceqit.
   SCR 1352
   
      Rev 1.51   Apr 01 2004 15:09:38   dirby
   Modified field 49 to be used only for EB host.  Megalink does not
   want it.
   SCR 1352
   
      Rev 1.50   Mar 24 2004 14:35:36   dirby
   Added building request fields 36, 102, and 103.  Modified request
   fields 42, 48, and 128 to act differently for Megalink versus EB Host.
   SCR 1352
   
      Rev 1.49   Jan 05 2004 17:16:26   dirby
   Modified building response field 38 to set it to all zeroes if the
   auth number is missing or is all blanks.
   SCR 1296
   
      Rev 1.48   Oct 14 2003 01:28:24   dirby
   Modified 'build_response_field_48' to get current amount for
   balance inquiries from either cash_outstanding or from
   cash_available.  Network controllers populate this in one place
   while txcntl (for On-Us) populates it in a different place.
   SCR 1083
   
      Rev 1.47   Sep 29 2003 13:51:42   dirby
   Added support for Fee Collection and Funds Disbursement for
   Acquirers.
   SCR 1083
   
      Rev 1.46   Sep 11 2003 15:47:52   dirby
   Removed an unused local variable.
   SCR 1186
   
      Rev 1.45   Sep 09 2003 17:41:08   svelaga
   Fixed parsing issue with Field 48 for Back office adjustment.
   
      Rev 1.44   Aug 29 2003 14:31:20   svelaga
   Field 62 bit map size is increased to 8 bytes.
   
      Rev 1.43   Aug 28 2003 14:30:18   svelaga
   Fixed bugs/issues with 0600 message.
   
      Rev 1.42   Aug 26 2003 16:22:58   svelaga
   Administrative text message logic fixed.
   Version bumped to 4.2.0.11
   
      Rev 1.41   Aug 25 2003 13:48:02   svelaga
   Fixed
   1. Added support for Field 62
   2. No check for valid NMI code for Administrative text message.
   3. Backoffice adjustment and Representment are distinguised based on
      Field 25 ( pos condition code ) instead of processing code.
   
      Rev 1.40   Aug 12 2003 12:09:28   svelaga
   filed 25: correction- echo from request.
   
      Rev 1.39   Aug 06 2003 10:38:12   svelaga
   Version 4.2.0.9:  Support for more POSCC added.
   
      Rev 1.38   Aug 05 2003 10:29:04   svelaga
   VISASMS Specification Changes: New Private field 62 and some changes to Private field 63.
   
      Rev 1.37   Jul 31 2003 16:44:04   svelaga
   Visa SMS QA bug fixes.
   
      Rev 1.36   Jul 31 2003 10:38:06   svelaga
   Retreival reference num ( move_field_37 ) problem fixed.
   
      Rev 1.35   Jul 30 2003 15:24:24   svelaga
   Removed nulls from response message.
   
      Rev 1.34   Jul 30 2003 14:45:48   svelaga
   Administrative message ( 0620 ) message added.
   
      Rev 1.33   Jul 28 2003 08:55:56   svelaga
   Build response procedurtes for Field 44, 63 are updated.
   
      Rev 1.32   Jul 25 2003 17:32:52   svelaga
   SMS projects changes -
   1. Support for BACKOFFICE_ADJUSTMENT, REPRESENTMENT transactions.
   2. Added to parse and build some extra fields like DE48, DE63,
   DE18,DE19 etc...
   
      Rev 1.31   May 14 2003 14:42:30   lmitchel
   Version 4.2.0.5 SCR1127:  Modified build_response_field_48 additional data.
   First balance field changed to contain acf01.cash_outstanding instead of
   acf01.cash_available.
   
      Rev 1.30   Apr 23 2003 15:34:18   dirby
   Modified function 'build_response_field_102' to set the Cash
   Advance Type (DE102.1), if visa, based on processing code.
   This allows a different value for savings, current/checking, and
   credit.
   SCR 1101
   
      Rev 1.29   Apr 08 2003 15:15:14   dirby
   Modified build_response_field_102 so that is a transaction does
   not go out to a host due to host offline, causing the authorizing
   host queue field to not get populated, then we can still build
   this field based on card family.
   SCR 1086
   
      Rev 1.28   Apr 03 2003 14:15:32   dirby
   Modified response field 102 to always populate it for ATMs.
   Used to only populate it for approvals.
   SCR 1082
   
      Rev 1.27   Dec 23 2002 11:44:32   dirby
   Modified field 95 for switched in to set reversal amount to txn
   amount if amount dispensed (actual amount) is zero or less.
   Found and fixed this bug while working SCR 845.
   
      Rev 1.26   Sep 17 2002 15:29:18   dirby
   In function 'build_response_field_102' changed the authorizing 
   host comparisons from 4 characters to 3.  This occurs where
   I am trying to determine card type in order to populate the field.
   The reason for the change is to allow multiple copies of a network
   handler.  For example, make a copy of ncvisa and call it ncvis2.
   Cannot call it ncvisa2 because the authorizing host field in TLF01
   does not allow that many characters.
   SCR 877
   
      Rev 1.25   Aug 19 2002 11:11:24   dirby
   Modified DE41 functions to not put any restrictions on content.
   SCR 876
   
      Rev 1.24   Jul 05 2002 13:10:56   dirby
   Modified DE48 to copy currency code for balance info into
   auth_tx.host2_src_fi_nbr.  This is so ncvisa can pick it out of
   there for DE54 currency code.  Need to do this because this
   currency code might be different than the txn currency code.
   SCR 823
   
      Rev 1.23   Jun 25 2002 16:48:44   dirby
   Modified to copy balance information into TLF01 fields so Equitable
   can report on them.  Balances copy in as:
     Current Balance    --> TLF01.outstanding_balance
     Available Balance --> TLF01.credit_limit
   SCR 796
   
      Rev 1.22   Jun 25 2002 12:45:02   dirby
   Modified response field 48 to check for the existence of current
   and available balance information.  Previously, it only checked
   available, so if current existed, but available did not, no balance
   info was passed in field 48.
   SCR 798
   
      Rev 1.21   Jun 24 2002 11:16:20   dirby
   Modified DE38 to allow spaces.
   SCR 797
   
   Modified incoming track2 from host to remove the end sentinel
   if it exists.  Some ATMs send it, some do not.  If it is not removed,
   and if the txn goes to Visa, Visa will reject it with an 0027 reject code.
   SCR 790
   
      Rev 1.20   Jun 20 2002 16:10:06   dirby
   Modified DE102 to space fill the Visa fields if they do not exist.
   SCR 794
   
      Rev 1.19   Jun 17 2002 14:07:14   dirby
   Allow spaces in DE41 request to host.
   SCR 791
   
      Rev 1.18   May 15 2002 12:08:52   dirby
   Removed an extraneous semicolon from function move_field_95.
   Found and fixed this while working on SCR 725
   
      Rev 1.17   Apr 21 2002 16:08:56   dirby
   Modified to set DE38 (auth number) to all zeroes on declined
   responses.
   SCR 706
   
      Rev 1.16   Apr 15 2002 18:02:58   dirby
   Modified to set DE38 to all zeroes if response code is a time out.
   SCR 700
   
      Rev 1.15   Apr 15 2002 16:09:40   dirby
   Made changes necessary to save transmission timestamp
   and convert it to GMT from local Manila time.  Need to save
   original value for the response back to EB Host.
   SCR 700
   
      Rev 1.14   Apr 02 2002 16:26:30   dirby
   Corrected a bug in function 'move_field_52'.  The pin was being
   converted to ASCII, but the EBCDIC pin was being copied to
   auth_tx.  Corrected this to copy the ASCII pin to auth_tx.
   SCR 679
   
      Rev 1.13   Mar 06 2002 15:28:00   dirby
   Added currency code to DE90 for reversal responses.
   SCR 650
   
      Rev 1.12   Mar 06 2002 14:57:06   dirby
   Added currency code to DE90.
   SCR 650
   
      Rev 1.11   Oct 11 2001 16:59:08   dirby
   Corrected a simple bug in build_request_field_38.   SCR 510
   
      Rev 1.10   Oct 10 2001 17:30:14   dirby
   1.  Modified DE32 to not overwrite acquiring_id if there is an existing value.  SCR 512
   2.  Set auth_number to all zeroes if one does not exist.  This is for reversals.
   3.  Pass the response code through when building a reversal
        request, instead of always sending approved.  SCR 510
   4.  Set original message type to 0200 in DE90.  SCR 511
   5.  Copy balance into total_amount for balance inquiries.  This
        is so the balance will be displayed on the terminal.  SCR 513
   
      Rev 1.9   Oct 08 2001 13:04:54   dirby
   Added a check to function 'build_request_field_48' to only copy
   the card holder billing amount if it exists.  SCR 492
   
      Rev 1.8   Oct 02 2001 13:33:24   dirby
   1.  Changed build_request_field_35 to check for '?' at end of track1.
   If it is not there, need to copy last character into track2.
   2.  Corrected some indentation issues in build_request_field 4.
   SCR 446
   
      Rev 1.7   Sep 27 2001 14:11:40   dirby
   Enhance the code the creates track 2 data from track 1 to include
   service code, PVV, and CVV information.
   SCR 446
   
      Rev 1.6   Sep 27 2001 09:55:54   dirby
   Modified build_request_field_32 so that the first byte of
   HOST_MESSAGE.acq_inst_id is not overwritten with a null.  SCR 447
   Not related to this SCR, but fixed while working the SCR.
   
      Rev 1.5   Aug 30 2001 09:16:40   SYARLAGA
   Modified  move_field_41 function.
   
      Rev 1.4   Aug 29 2001 09:27:58   SYARLAGA
   Modified  build_request_field_3 function.
   
      Rev 1.3   Aug 27 2001 13:54:58   SYARLAGA
   modified   build_request_field_4,   build_request_field_48 and   build_request_field_49 functions.
   
      Rev 1.2   Aug 20 2001 17:09:18   dirby
   1.  Modified building DE32 to use value that is configured in
   NCF01.  If a value is not configured, then set it based on txn
   origin: 00 = local, 95 = international.  SCR 438.
   
   2.  Modified to only verify DE7 from host if txn is switched in.
   SCR 439.
   
      Rev 1.1   Aug 13 2001 17:19:38   SYARLAGA
   Modified move_field_52 and build_request_field_52 functions.
   
      Rev 1.0   Aug 06 2001 09:47:56   dirby
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
#include "ptetime.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "nceq_constants.h"
#include "nceq_prototypes.h"
#include "nceq_bitfields.h"



/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     TranFlow;
       INT     NetworkType;

/* Network: nceqit or ncmlnk */
extern INT     Network;

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

extern const struct isobit BITTAB[128+1];
INT second_Bitmap;

/*****************************************************************************


  Function:			gmt_get_time

  Description:		This function will convert from ascii to EBCDIC or from 
					EBCDIC to ascii depending on conv_type.

  Author:			Girija Y TF, Jan 08 2009
      
  Inputs:           Pointer to string unsigned string
				
  Outputs:          String converted from Manila timestamp to GMT 
					
  Return values:    pointer to BYTE
/******************************************************************************/
INT gmt_get_time(pBYTE mmdd_hhmmss)
{
	CHAR	time_hh[3];
 	INT		gmt_int =0 ;
	INT		gmt_day;
	INT		gmt_month;
	INT		gmt_time;
	BYTE    day[3] ;
	BYTE    month[3] ;
	BYTE    time_hhmmss[7];
	BYTE    current_year[5];
	INT		year_yyyy;
	CHAR	str_day[2];
	CHAR	str_month[2];
	CHAR	time_date[25];
	CHAR	date_str[9];
	CHAR	time_mmss[5];
	INT		gmt_mmss;
	CHAR	error_buf[125];
	CHAR	str_date[5];
	CHAR	str_time[7];
	INT		rval;

	memset(str_day,0x00,sizeof(str_day));
	memset(str_month,0x00,sizeof(str_month));
	memset(time_date,0x00,sizeof(time_date));
	memset(date_str,0x00,sizeof(date_str));
	memset(time_mmss,0x00,sizeof(time_mmss));
	memset(time_hh,0x00,sizeof(time_hh));

	memcpy(month,mmdd_hhmmss,2);
	memcpy(day,mmdd_hhmmss+2,2);
	memcpy(time_hh,mmdd_hhmmss+4,2);
	memcpy(time_hhmmss,mmdd_hhmmss+4,6);
	memcpy(time_mmss,mmdd_hhmmss+6,4);
	
	/* Get current date and time */
	ptetime_get_timestamp( time_date );
    get_date( time_date, date_str );
	memcpy(current_year,date_str,4);


	gmt_day = atoi(day);
	gmt_month=atoi(month);
	gmt_time = atoi(time_hh);
	year_yyyy = atoi(current_year);
	gmt_mmss = atoi(time_mmss);

	/* Convert local manila time to GMT */
	gmt_int = gmt_time - 8;

	sprintf(str_date,"%02d%02d",gmt_month,gmt_day);
	rval = validate_date_mmdd( str_date );

    if ( rval == true )
    {
       sprintf(str_time,"%02d%04d",gmt_int,gmt_mmss);
	   rval = validate_time(str_time);
	   if ( rval == false )
	   {
			sprintf(error_buf,"Invalid timestamp %s",str_time);
			nceqit_log_message(1,2,error_buf,"gmt_get_time",0);
	   }
    }
	
	/* In case of negative time, go back to previous date */ 
	if( gmt_int < 0 )
	{
		gmt_int +=24;
		(gmt_day)--;
		if( gmt_day <= 0 )
		{
			(gmt_month)-- ;
			if(gmt_month <= 0 )
			{
				gmt_month = 12;
				(year_yyyy)--;
			} /* Make month as Dec */

			/* Check for leap year */
			if( ((year_yyyy % 400) == 0) ||
				(((year_yyyy % 4) == 0 ) && ((year_yyyy % 100) != 0)))
			{
				if( gmt_month == 2)
				{
					gmt_day = 29;
				}
				else if ( (gmt_month == 4) || (gmt_month == 6) || (gmt_month == 9) || (gmt_month == 11) )
				{
					gmt_day = 30;
				}
				else 
				{
					gmt_day = 31;
				}
			} /*End of if */
			else
			{
				if( gmt_month == 2)
				{
					gmt_day = 28;
				}
				else if ( (gmt_month == 4) || (gmt_month == 6) || (gmt_month == 9) || (gmt_month == 11) )
				{
					gmt_day = 30;
				}
				else 
				{
					gmt_day = 31;
				}
			} /* End of else */
		}

		sprintf(mmdd_hhmmss,"%02d%02d%02d%04d",gmt_month,gmt_day,gmt_int,gmt_mmss);
	}
	else
	{
		sprintf(mmdd_hhmmss,"%02d%02d%02d%04d",gmt_month,gmt_day,gmt_int,gmt_mmss);
	}

	return(rval);
}
/*****************************************************************************/
/*****************************************************************************

  Function:    ascii_ebcdic_conv

  Description: This function will convert from ascii to EBCDIC or from 
               EBCDIC to ascii depending on conv_type.
  Author: 
      unknown
  Inputs:
      conv_char - the character to convert
      conv_type - A2E - convert from ascii to EBCDIC
                  E2A - convert from EBCDIC to ascii
  Outputs:
      None
  Return values:
      BYTE - the converted character
******************************************************************************/
static  BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{
   INT            i;
   INT    found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           !     "     #     $     %     &     '  |
        |   (     )     *     +     ,     -     .     /     :     ;  |
        |   <     =     >     ?     @     \     _     `     {     |  |
        |   }     ~                                                  |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
         0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61, 0x7A, 0x5E,
         0x4C, 0x7E, 0x6E, 0x6F, 0x7C, 0xE0, 0x6D, 0x79, 0xC0, 0x4F,
         0xD0, 0xA1, 0x5F
      };

   static unsigned char ascii_chart[] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
         0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B,
         0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x5C, 0x5F, 0x60, 0x7B, 0x7C,
         0x7D, 0x7E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      //the next line is modified by farad 8/13/98
	  for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      //the next line is modified by farad 8/13/98
      for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   
} /* asc_to_bcd */
/******************************************************************************
 *
 *  NAME:         build_request_field_2
 *
 *  DESCRIPTION:  This function will move the card number to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits.  While the card number is
 *                variable length, this field is always the same length.
 *                Need to left justify and zero fill the field.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_2( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.TLF01_details.card_num );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build request message to host. No card number." );
      }
      else
      {
         if ( isalphanum(Auth_Tx.TLF01_details.card_num) )
         {
            sprintf( HOST_MESSAGE.pan_len, "%02d", len  );
            memset( HOST_MESSAGE.pan, '0', MAX_DE2_SIZE );
            memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
         }
         else
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                 "Unable to build request msg to host. Invalid card number: %s",
                  Auth_Tx.TLF01_details.card_num );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_3
 *
 *  DESCRIPTION:  This function will move the processing code to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf      - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_3( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE proc_code[7]="";

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.TLF01_details.processing_code) )
      {
         memcpy( proc_code, Auth_Tx.TLF01_details.processing_code, 6 );
         
         if ( Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY )
            memcpy( proc_code, "30", 2 );

         else if ( (proc_code[1]== '0') && (NetworkType == ISSUER))
            memcpy( proc_code, "01", 2 );

         else if ( Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD )
            memcpy( proc_code, "22", 2 );

         else if ( is_void() )
            memset( proc_code, '0', 6 );

         memcpy( HOST_MESSAGE.proccode, proc_code, DE3_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid processing code: %s",
                Auth_Tx.TLF01_details.processing_code );
      }
   }
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
   memcpy(Auth_Tx.TLF01_details.product_codes[17].amount, proc_code, DE3_SIZE );
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_4
 *
 *  DESCRIPTION:  This function will move the amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_4( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.TLF01_details.total_amount) )
      {
         if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
                    DE4_SIZE );
         }
         else if ( 0 == strcmp(Auth_Tx.reconciliation_cur_code,
                               Auth_Tx.TLF01_details.currency_code) )
         {
            memcpy( HOST_MESSAGE.amount,
                    Auth_Tx.TLF01_details.total_amount,
                    DE4_SIZE );
         }
         else
         {
            memcpy(HOST_MESSAGE.amount,Auth_Tx.reconciliation_amount,DE4_SIZE);
         }
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid amount: %s",
                Auth_Tx.TLF01_details.total_amount );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_5
 *
 *  DESCRIPTION:  This function will move the settelment amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_5( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.TLF01_details.settlement_total) )
      {
         {
            memcpy( HOST_MESSAGE.setl_amount,
                    Auth_Tx.TLF01_details.settlement_total,
                    DE5_SIZE );
         }
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid settelment amount: %s",
                Auth_Tx.TLF01_details.settlement_total );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date and time
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is expressed in local time.  Indicates the time
 *                at the message sender's last node before leaving the sender's
 *                domain.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  date_str [ 9] = "";
   CHAR  time_str [ 7] = "";
   CHAR  time_date[25] = "";

   if ( *result_flag == 0 )
   {
      /* If this is a reversal, need to make sure we have the original
       * timestamp.  It should exist in 'orig_local_time_date', but
       * it might exist in 'transmission_timestamp'.
       *
       * If this is not a reversal, value may or may not exist.
       */
      if ( is_reversal() )
      {
         /* It is a reversal.  Original timestamp must exist. */
         if ( 0x00 == Auth_Tx.orig_local_time_date[0] )
         {
            if ( 0x00 == Auth_Tx.TLF01_details.transmission_timestamp[0] )
            {
               /* Original timestamp does not exist. Error! */
               ret_val = false;
               *result_flag = 1;
               sprintf( err_buf,
                     "Cannot build request msg (%s) for host. Missing a timestamp.",
                      Auth_Tx.TLF01_details.message_type );
            }
            else
            {
               /* Need to move timestamp to 'original' field. */
               memcpy( Auth_Tx.orig_local_time_date,
                       Auth_Tx.TLF01_details.transmission_timestamp,
                       DE7_SIZE );

               memset( Auth_Tx.TLF01_details.transmission_timestamp,
                       0x00,
                       sizeof(Auth_Tx.TLF01_details.transmission_timestamp));
            }
         }
      }

      /* Create timestamp if it does not exist. */
      if ( true == ret_val )
      {
         if ( 0x00 == Auth_Tx.TLF01_details.transmission_timestamp[0] )
         {
            /* Need to create transmission time stamp. Get local time. */
            ptetime_get_timestamp( time_date );
            get_date( time_date, date_str );
            get_time( time_date, time_str );

            /* Put it in MMDDhhmms format */
            memcpy( Auth_Tx.TLF01_details.transmission_timestamp, date_str+4,4);
            memcpy( Auth_Tx.TLF01_details.transmission_timestamp+4, time_str,6);

         }

         memcpy( HOST_MESSAGE.gmt_time,
                 Auth_Tx.TLF01_details.transmission_timestamp,
                 DE7_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_9
 *
 *  DESCRIPTION:  This function will move the settelment conversion rate to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  conversion rate is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_9( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isalphanum(Auth_Tx.settlement_conv_rate) )
      {
         {
            memcpy( HOST_MESSAGE.setl_conv_rate,
                    Auth_Tx.settlement_conv_rate,
                    DE9_SIZE );
         }
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid settelment conversion rate: %s",
                Auth_Tx.settlement_conv_rate );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_11( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   CHAR stan[7] = "";

   if ( *result_flag == 0 )
   {
      if ( is_void() )
         strcpy( stan, Auth_Tx.TLF01_details.orig_sys_trace_num );
      else
         strcpy( stan, Auth_Tx.TLF01_details.sys_trace_audit_num );

      if ( isnum(stan) )
      {
         memcpy( HOST_MESSAGE.stan, stan, DE11_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid STAN: %s",
                stan );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local time at the card aceptors location.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";

   if ( *result_flag == 0 )
   {
		// If its a transaction coming from DCPISO (POS), copy time of original Sale txn from
		// field 7 (timestamp) to field 12. nirbhay/Girija GB  : BDO production issue wrong Field 12 for reversals

      if ( //is_reversal() &&	// nirbhay/Girija GB : BDO production issue wrong Field 12 for reversals
		   strncmp(Auth_Tx.TLF01_details.handler_queue, "dcpiso", 6) == 0 &&
		   Auth_Tx.TLF01_details.transmission_timestamp[4] != 0
		 )
      {						// nirbhay : BDO production issue wrong Field 12 for reversals
		  memcpy( Auth_Tx.TLF01_details.time_hhmmss,
						Auth_Tx.TLF01_details.transmission_timestamp+4, 6);
	  }						// nirbhay : BDO production issue wrong Field 12 for reversals
	  else					// nirbhay : BDO production issue wrong Field 12 for reversals
	  {						// nirbhay : BDO production issue wrong Field 12 for reversals
		  if ( 0x00 == Auth_Tx.TLF01_details.time_hhmmss[0] )
		  {
			 /* Need to create transaction time. Get local time. */
			 ptetime_get_timestamp( time_date );
			 get_time( time_date, Auth_Tx.TLF01_details.time_hhmmss );
		  }

		  else if (!validate_time(Auth_Tx.TLF01_details.time_hhmmss))
		  {
			 ret_val = false;
			 *result_flag = 1;
			 sprintf( err_buf,
				   "Cannot build request msg for host. Invalid transaction time: %s",
					Auth_Tx.TLF01_details.time_hhmmss );
		  }
	  }						// nirbhay : BDO production issue wrong Field 12 for reversals

	  if ( true == ret_val )
	  {
		 memcpy( HOST_MESSAGE.time,
				 Auth_Tx.TLF01_details.time_hhmmss,
				 DE12_SIZE );
	  }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_13
 *
 *  DESCRIPTION:  This function will move the transaction date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is the local MMDD at the card aceptors location.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";

   if ( *result_flag == 0 )
   {
      if ( 0x00 == Auth_Tx.TLF01_details.date_yyyymmdd[0] )
      {
         /* Need to create transaction date. Get local date/time. */
         ptetime_get_timestamp( time_date );
         get_date( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
      }

      else if (!validate_date_yyyymmdd(Auth_Tx.TLF01_details.date_yyyymmdd,err_buf))
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid transaction date: %s",
                Auth_Tx.TLF01_details.date_yyyymmdd );
      }

      if ( true == ret_val )
      {
         memcpy( HOST_MESSAGE.date,
                 Auth_Tx.TLF01_details.date_yyyymmdd+4,
                 DE13_SIZE );
      }
   }
   return( ret_val );
}
//TF PHANI

INT build_request_field_14( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BOOLEAN ret_code = 0;
   CHAR  time_date[25] = "";

   if ( *result_flag == 0 )
   {
	   ret_code = Isallzeros(Auth_Tx.TLF01_details.exp_date,4);
	   if(ret_code == true )
	   {
		   memset(Auth_Tx.TLF01_details.exp_date,0x00,sizeof(Auth_Tx.TLF01_details.exp_date)-1);
	       return( ret_val );
	   }
	   else if(Auth_Tx.TLF01_details.exp_date[0] != 0x00)
       {
         memcpy( HOST_MESSAGE.exp_date,
                 Auth_Tx.TLF01_details.exp_date,
                 DE14_SIZE );
       }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_15
 *
 *  DESCRIPTION:  This function will move the settlement date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is format MMDD.  Interchange settlement date
 *                for PLUS transactions.  Actual business date for the day
 *                that the acquirer transmitted the transaction to MegaLink.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_15( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";
   CHAR  date_str[9]   = "";

   if ( *result_flag == 0 )
   {
      if ( (0x00 == Auth_Tx.TLF01_details.settlement_date[0]) ||
           (0 == strcmp("0000",Auth_Tx.TLF01_details.settlement_date)) )
      {
         /* Need to create transaction date. Get local date/time. */
         ptetime_get_timestamp( time_date );
         get_date( time_date, date_str );
         memcpy( Auth_Tx.TLF01_details.settlement_date, date_str+4, 4 );
      }

      else if (!validate_date_mmdd(Auth_Tx.TLF01_details.settlement_date))
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid settlement date: %s",
                Auth_Tx.TLF01_details.settlement_date );
      }

      if ( true == ret_val )
      {
         memcpy( HOST_MESSAGE.date_settle,
                 Auth_Tx.TLF01_details.settlement_date,
                 DE15_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_16
 *
 *  DESCRIPTION:  This function will move the conversion date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is format MMDD.  
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_16( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25] = "";
   CHAR  date_str[9]   = "";

   if ( *result_flag == 0 )
   {
	  if( Auth_Tx.conv_date[0] != 0x00 )
	  {
		  if (!validate_date_mmdd(Auth_Tx.conv_date))
		  {
			 ret_val = false;
			 *result_flag = 1;
			 sprintf( err_buf,
				   "Cannot build request msg for host. Invalid conversion date: %s",
					Auth_Tx.conv_date );
		  }

		  if ( true == ret_val )
		  {
			 memcpy( HOST_MESSAGE.date_conv,
					 Auth_Tx.conv_date,
					 DE16_SIZE );
		  }
	  }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_18
 *
 *  DESCRIPTION:  This function will move the merchant's type
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_18( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.TLF01_details.category_code) )
      {
         memcpy( HOST_MESSAGE.merch_type,
                 Auth_Tx.TLF01_details.category_code,
                 DE18_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid merchant type: %s",
                Auth_Tx.TLF01_details.category_code);
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_19
 *
 *  DESCRIPTION:  This function will move the Acquiring Institution ID Code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
/* NEW */
INT build_request_field_19( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.country_code) )
      {
         memcpy( HOST_MESSAGE.acq_inst_cnt_code,
                 Auth_Tx.country_code,
                 DE19_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid Acquiring Institution ID Code: %s",
                Auth_Tx.country_code);
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_20
 *
 *  DESCRIPTION:  This function will move the PAN Extended, Country Code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
/* NEW */
INT build_request_field_20( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.pan_ext_ctry_code) )
      {
         memcpy( HOST_MESSAGE.fwd_inst_cnt_code,
                 Auth_Tx.pan_ext_ctry_code,
                 DE21_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid PAN Extended, Country Code: %s",
                Auth_Tx.pan_ext_ctry_code);
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_21
 *
 *  DESCRIPTION:  This function will move the Forward institution country code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_request_field_21( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.pan_ext_ctry_code) )
      {
         memcpy( HOST_MESSAGE.fwd_inst_cnt_code,
                 Auth_Tx.country_code,
                 DE21_SIZE );
      }
      else
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid forward institution country code: %s",
                Auth_Tx.country_code);
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_22
 *
 *  DESCRIPTION:  This function will move the POS entry mode
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;

   if ( *result_flag == 0 )
   {
      if ( 0x00 == Auth_Tx.TLF01_details.pos_entry_mode[0] )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf, "POS Entry Mode is missing. Decline txn." );
      }
      else if ( false == isnum(Auth_Tx.TLF01_details.pos_entry_mode) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
               "Cannot build request msg for host. Invalid POS Entry Mode: %s",
                Auth_Tx.TLF01_details.pos_entry_mode );
      }

      /* Determine format of POS Entry Mode. */
      else
      {
         len = strlen( Auth_Tx.TLF01_details.pos_entry_mode );
         if ( len == DE22_SIZE )
         {
            /* Format is ok; just copy it over. */
            memcpy( HOST_MESSAGE.posentry,
                    Auth_Tx.TLF01_details.pos_entry_mode, DE22_SIZE );
         }
         else
         {
            /* Convert from Hypercom ISO format 0nnn to EB format nnn. */

            memcpy( HOST_MESSAGE.posentry, 
                    Auth_Tx.TLF01_details.pos_entry_mode+1, DE22_SIZE );

        	 if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",3)== 0 &&
        	    (strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3)== 0 ||
        	     strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"008",3)== 0	)
        						 )
			 {         /*Fall back  Tx from POS*/
				 memcpy( HOST_MESSAGE.posentry,
									"90", 2 );
			 }
         }
      }
   }
   memcpy(Auth_Tx.TLF01_details.product_codes[9].code,HOST_MESSAGE.posentry,DE22_SIZE);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_23
 *
 *  DESCRIPTION:  This function will move Pan seq nbr.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
/* NEW */
INT build_request_field_23( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( isnum(Auth_Tx.host2_message_seq_nbr) )
      {
       strncpy(HOST_MESSAGE.cardseqnum,&Auth_Tx.host2_message_seq_nbr[1],3);
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_25
 *
 *  DESCRIPTION:  This function will move the POS condition code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	  /* Copy pos condition code for Charge Back request - Girija Y, TF */
	   if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0422",4))
	   {
		   memcpy( HOST_MESSAGE.pos_condcd,Auth_Tx.TLF01_details.pos_condition_code,
			       sizeof(HOST_MESSAGE.pos_condcd));
	   }
	  // RPS INDICATOR - 09 for RPS tx - TF Phani
	   else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",3))
		 {
			memcpy( HOST_MESSAGE.pos_condcd, Auth_Tx.TLF01_details.pos_condition_code, DE25_SIZE );
		 }
	  else if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
	  {
		   memcpy( HOST_MESSAGE.pos_condcd, sRPS_POSCC, DE25_SIZE );
	  }
	  // TF Phani - For 59
	  else if((0 == strncmp("59",Auth_Tx.TLF01_details.pos_condition_code,2)))
	  {
		   memcpy( HOST_MESSAGE.pos_condcd,Auth_Tx.TLF01_details.pos_condition_code,
			       sizeof(HOST_MESSAGE.pos_condcd));
	  }	
      /* Set Condition Code based on ATM or POS. */
      else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE   ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)  )
      {
         memcpy( HOST_MESSAGE.pos_condcd, sATM_POSCC, DE25_SIZE );
      }

      else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_SALE   ) ||
                (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION)  )
      {
         memcpy( HOST_MESSAGE.pos_condcd, sNORMAL_POSCC, DE25_SIZE );
      }

      else if ( (0 == strncmp("01",Auth_Tx.TLF01_details.processing_code,2)) ||
                (0 == strncmp("30",Auth_Tx.TLF01_details.processing_code,2))  )
      {
         /* This is a Cash Advance or Balance Inquiry = ATM */
         memcpy( HOST_MESSAGE.pos_condcd, sATM_POSCC, DE25_SIZE );
      }
      else
      {
         memcpy( HOST_MESSAGE.pos_condcd, Auth_Tx.TLF01_details.pos_condition_code, DE25_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_28
 *
 *  DESCRIPTION:  This function will move the transaction fee amount
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_28( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	   if( Auth_Tx.TLF01_details.tran_fee_amount[0] != 0x00 )
	   {
		   if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3", 6))
		   {
			   if((Auth_Tx.TLF01_details.tx_key==AUTH_REVERSAL) ||
				  (Auth_Tx.TLF01_details.tx_key==AUTH_REVERSAL_REPEAT) ||
				  (Auth_Tx.TLF01_details.tx_key==AUTH_REVERSAL_ADVICE))
			   {
				   HOST_MESSAGE.tran_fee_amt[0]='C';
			   }
			   else
			   {
				   HOST_MESSAGE.tran_fee_amt[0]='D';
			   }

			   memcpy(&HOST_MESSAGE.tran_fee_amt[1],Auth_Tx.TLF01_details.tran_fee_amount,DE28_SIZE);
		   }
		   else
		   {
			   memcpy(HOST_MESSAGE.tran_fee_amt,Auth_Tx.TLF01_details.tran_fee_amount,DE28_SIZE);
		   }
	   }

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_32
 *
 *  DESCRIPTION:  This function will move the acquiring institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_32( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
	  /* Copy acquiring institution ID for Charge Back request - Girija Y, TF */
	  /* Pass "95" as acquirer id for 0422 msg, Jan 28, 2009 */
	  if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0422",4))
	  {
		   len = 2;
		   memcpy( HOST_MESSAGE.acq_inst_id,INTL_ATM_POS,DE32_SIZE);
	  }
	  else if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')/* RPS tx '00'-TF Phani*/
	  {
		  len = 2;
		   memcpy( HOST_MESSAGE.acq_inst_id,LOCAL_POS,DE32_SIZE);
	  }
      else if ( Ncf01_I.acquirer_inst_id_code[0] == 0x00 )
      {
         /* An Acquirer ID is not configured.  Base it on origin. */
         len = 2;
         if(0== strcmp(REMOTE_ORIGIN,Auth_Tx.TLF01_details.dispensation.origin))
            memcpy( HOST_MESSAGE.acq_inst_id, INTL_ATM_POS, DE32_SIZE );
         else
            memcpy( HOST_MESSAGE.acq_inst_id, LOCAL_POS,    DE32_SIZE );
      }
      else
      {
         len = strlen( Ncf01_I.acquirer_inst_id_code );
         memcpy( HOST_MESSAGE.acq_inst_id, Ncf01_I.acquirer_inst_id_code, len );
      }

      /* Get the length. */
      sprintf( temp_len, "%02d", len );
      memcpy( HOST_MESSAGE.acq_inst_len, temp_len, 2 );

      /* Store value in TLF01 if there is not already a value. */
      if ( Auth_Tx.TLF01_details.acquiring_id[0] == 0x00 )
      {
         memcpy( Auth_Tx.TLF01_details.acquiring_id,
                 HOST_MESSAGE.acq_inst_id,
                 len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_33
 *
 *  DESCRIPTION:  This function will move the forwarding institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_33( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
      if ( Auth_Tx.forwarding_inst_id_cd [0] != 0x00 )
      {
		  /* Get the length. */
		  len = strlen( Auth_Tx.forwarding_inst_id_cd );
          sprintf( temp_len, "%02d", len );
          memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, 2 );
		  memcpy( HOST_MESSAGE.fwd_inst_id,Auth_Tx.forwarding_inst_id_cd,
			      DE33_SIZE);
         
      }
      /* Store value in TLF01 if there is not already a value. */
      if ( Auth_Tx.TLF01_details.forwarding_institution_id[0] == 0x00 )
      {
         memcpy( Auth_Tx.TLF01_details.forwarding_institution_id,
                 HOST_MESSAGE.fwd_inst_id,
                 len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_35
 *
 *  DESCRIPTION:  This function will move the track2 data
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *                This function will create track2 data from track1 data if
 *                there is no track2, but track1 does exist.  If there is no
 *                track1, then track2 is created from card number and expiry
 *                date.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_35( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   INT   i;
   INT   empty_data;
   pBYTE trk2_ptr = NULL_PTR;
   pBYTE FS2_ptr  = NULL_PTR;
   pBYTE FS1_ptr  = NULL_PTR;

   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y' ||  Auth_Tx.TLF01_details.track2[0] == 0x00 )// If tx is RPS do not populate Track2 - TF Phani
   { 
	   return( ret_val );
   }

	if ( Auth_Tx.TLF01_details.track2[0] == 0x00  && 
		(Auth_Tx.TLF01_details.tx_key == AUTH_OCT_TX || Auth_Tx.TLF01_details.tx_key == AUTH_FAST_FUND))
	{
		return( ret_val );
	}
   if ( *result_flag == 0 )
   {
      /* Track data is required, even for reversals.
       * If it does not exist, create it:
       * card num + FS + expiry date + service code + PVV [+ CVV]
       */

      /* Initialize to all spaces. */
      memset( HOST_MESSAGE.track2, ' ', MAX_DE35_SIZE );

      if ( Auth_Tx.TLF01_details.track2[0] == 0x00 )
      {
         if ( Auth_Tx.TLF01_details.track1[0] == 0x00 )
         {
            len = strlen(Auth_Tx.TLF01_details.card_num);
            memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.card_num, len );
            memset(&HOST_MESSAGE.track2[len++], TRK2_FS, 1 );
            memcpy(&HOST_MESSAGE.track2[len], Auth_Tx.TLF01_details.exp_date,4);
         }
         else
         {
            /*--------------------------------*/
            /* Use Track 1 to create Track 2. */
            /*--------------------------------*/

            /* Set all the pointers we need. */
            trk2_ptr = HOST_MESSAGE.track2;
            FS1_ptr = strchr( Auth_Tx.TLF01_details.track1, TRK1_FS );
            FS2_ptr = strchr( FS1_ptr+1, TRK1_FS );
            if ( (FS1_ptr == NULL_PTR) || (FS2_ptr == NULL_PTR) )
            {
               ret_val = false;
               *result_flag = 1;
               sprintf( err_buf,
                "Unable to build track2. Missing field separator in track1: %s",
                 Auth_Tx.TLF01_details.track1 );
            }
            else
            {
               /* Get card number from track 1. */
               len = FS1_ptr - Auth_Tx.TLF01_details.track1 - 1;
               memcpy( trk2_ptr, Auth_Tx.TLF01_details.track1+1, len );
               trk2_ptr += len;

               /* Set track2 field separator. */
               *trk2_ptr = TRK2_FS;
               trk2_ptr++;

               /* Get length of data after 2nd field separator in track1. */
               len = strlen( Auth_Tx.TLF01_details.track1 );
               len = &Auth_Tx.TLF01_details.track1[len-1] - FS2_ptr;

               if ( len <= 13 )
               {
                  /* There is no CVV data, so do a straight copy. 
                   * Exclude the '?' at the end if it is there.
                   * Apparently, it might not always be there.
                   */
                  if ( *(FS2_ptr+len) == '?' )
                     memcpy( trk2_ptr, FS2_ptr+1, len-1 );
                  else
                     memcpy( trk2_ptr, FS2_ptr+1, len   );
               }
               else
               {
                  /* There's some optional data. Is it all zeroes and spaces? */
                  empty_data = true;
                  for ( i=1; i<len; i++ )
                  {
                     if ( (FS2_ptr[i] != ' ') && (FS2_ptr[i] != '0') )
                     {
                        empty_data = false;
                        break;
                     }
                  }
                  if ( empty_data == true )
                  {
                     /* The optional data is all zeroes and spaces.
                      * Do not use them.
                      */
                     memcpy( trk2_ptr, FS2_ptr+1, 12 );
                  }
                  else
                  {
                     /* '12' is the length of data: exp, service, and PVV. */
                     memcpy( trk2_ptr, FS2_ptr+1, 12 );

                     /* Copy the CVV data. */
                     trk2_ptr += 12;
                     memcpy( trk2_ptr, FS2_ptr+15, 3 );
                  }
               }
            }
         }
         /* Copy track 2 into Auth_Tx for storage into the database. */
         memcpy(Auth_Tx.TLF01_details.track2,HOST_MESSAGE.track2,MAX_DE35_SIZE);
      }
      else
      {
         len = strlen( Auth_Tx.TLF01_details.track2 );
         memcpy( HOST_MESSAGE.track2, Auth_Tx.TLF01_details.track2, len );
      }

      /* Set track 2 length for the host. */
      memcpy( HOST_MESSAGE.track2_len, "37", 2 );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_36
 *
 *  DESCRIPTION:  This function will move the track 3 data to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is alphanumeric, variable length.
 *
 *                This field is only for Megalink, not for EB Host.  It
 *                is space filled; need to do this because Megalink host
 *                expects this field, even though there is no data.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_36( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( Network == NCMLNK )
   {
      if ( *result_flag == 0 )
      {
         sprintf( HOST_MESSAGE.track3_len, "%03d", DE36_SIZE );
         memset( HOST_MESSAGE.track3, 0x20, DE36_SIZE );
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_37
 *
 *  DESCRIPTION:  This function will move the RRN value to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFIED BY:  Girija Y - TF
 ******************************************************************************/
INT build_request_field_37( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;


      if ( *result_flag == 0 )
      {
		  /* Send original RRN for POS reversals, Girija Y TF, Feb 27, 2009 */
		 if (strncmp(Auth_Tx.TLF01_details.dispensation.origin, "LO",2) == 0)
		 {
			 if( Auth_Tx.TLF01_details.orig_retrieval_ref_num[0] != 0x00 )
			 {
				memcpy( HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.orig_retrieval_ref_num,
						sizeof(HOST_MESSAGE.rrn) );
			 }
			 else
			 {
				 memcpy( HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,
						 sizeof(HOST_MESSAGE.rrn) );
			 }

		 }

         else if( Auth_Tx.TLF01_details.retrieval_ref_num[0] != 0x00 )
		 {
			memcpy( HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,
				    sizeof(HOST_MESSAGE.rrn) );
		 }
      }

   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_38( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( false == isalphanum_space(Auth_Tx.TLF01_details.auth_number) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Unable to build request msg to host. Auth num invalid (%s)",
                  Auth_Tx.TLF01_details.auth_number );
      }
      else if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
      {
         /* The only request message this field is in is reversals.
          * If there is no auth number, need to set it to all zeroes.
          */
         memset( HOST_MESSAGE.authcode, '0', DE38_SIZE );
      }
      else
      { 
         memcpy( HOST_MESSAGE.authcode,
                 Auth_Tx.TLF01_details.auth_number,
                 DE38_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_39( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	  if(0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6))
	  {
		  if ( Auth_Tx.TLF01_details.response_code[0] != 0x00 )
		  {
			  memcpy( HOST_MESSAGE.responsecd,
					  Auth_Tx.TLF01_details.response_code,
					  DE39_SIZE );
		  }
	  }
	  else
	  {
		  if ( Auth_Tx.TLF01_details.response_code[0] == 0x00 )
		  {
			  memcpy( HOST_MESSAGE.responsecd, APPROVED, DE39_SIZE );
		  }
		  else
		  {
			  memcpy( HOST_MESSAGE.responsecd,
                 Auth_Tx.TLF01_details.response_code,
                 DE39_SIZE );
		  }
	  }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric with spaces,
 *                fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_41( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.termid,
              Auth_Tx.TLF01_details.terminal_id,
              DE41_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_42
 *
 *  DESCRIPTION:  This function will move the merchant Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_42( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( Network == NCEQIT )
   {
      if ( *result_flag == 0 )
      {
         memcpy( HOST_MESSAGE.cardaccid,
                 Auth_Tx.TLF01_details.merchant_id,
                 DE42_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_43
 *
 *  DESCRIPTION:  This function will move the merchant Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
******************************************************************************/
INT build_request_field_43( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
   
	   if ( blank_string((pCHAR)Auth_Tx.acceptor_term_name,
		   sizeof(Auth_Tx.acceptor_term_name)-1) )
		   strcpy( Auth_Tx.acceptor_term_name,Auth_Tx.MCF01_details.name01);
	   if ( blank_string((pCHAR)Auth_Tx.acceptor_city_name,
		   sizeof(Auth_Tx.acceptor_city_name)-1) )
		   strncpy( Auth_Tx.acceptor_city_name,Auth_Tx.MCF01_details.city,13);
	   if ( blank_string((pCHAR)Auth_Tx.acceptor_country,
		   sizeof(Auth_Tx.acceptor_country)-1) )
		   strcpy( Auth_Tx.acceptor_country, "PH" );

	    memset(HOST_MESSAGE.cardaccname,0x20,DE43_SIZE);

		memcpy( HOST_MESSAGE.cardaccname,Auth_Tx.acceptor_term_name,25);
		memcpy( HOST_MESSAGE.cardaccname+25,Auth_Tx.acceptor_city_name,13);
		memcpy( HOST_MESSAGE.cardaccname+38,Auth_Tx.acceptor_country,2);
   }

   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_44
 *
 *  DESCRIPTION:  This function will move the merchant Id
 *               
 *               
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Phani 
 *
******************************************************************************/
INT build_request_field_44( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT	de44_len=0,temp_len=0;
   BYTE strLen[5]={0};

  de44_len = strlen(Auth_Tx.TLF01_details.resp_source);
   if ( *result_flag == 0 && de44_len > 0 && de44_len <= 25 &&
   		strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",4)== 0)
   {
      memset(HOST_MESSAGE.addlrspdata,0x20,sizeof(HOST_MESSAGE.addlrspdata));
	   
		memcpy( &HOST_MESSAGE.addlrspdata[0],
   		&Auth_Tx.TLF01_details.resp_source[0],
		de44_len);
				
	    sprintf( strLen, "%02d", de44_len );

	   memcpy( HOST_MESSAGE.addl_rsp_len, strLen, 2);
    
   
	  
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_45
 *
 *  DESCRIPTION:  This function will move the track1
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, variable length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Phani
 *
 ******************************************************************************/
INT build_request_field_45( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR temp_len[3]="";

   if ( *result_flag == 0 )
   {
      if(Auth_Tx.TLF01_details.track1[0] != 0x00)
      {
         /* Track 1 is present. Do not send track 2. */
         memset(HOST_MESSAGE.track2_len, 0x00, sizeof(HOST_MESSAGE.track2_len));

         len = strlen( Auth_Tx.TLF01_details.track1 );
         memcpy( HOST_MESSAGE.track1, Auth_Tx.TLF01_details.track1, len );

         sprintf( temp_len, "%02d", len );
         memcpy( HOST_MESSAGE.track1_len, temp_len, 2 );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_48
 *
 *  DESCRIPTION:  This function will move the additional data
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_48( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
//   INT	len = 0;  /* Girija Y TF, Jan 21, 2009 */
//   CHAR str_len[3];

   if ( *result_flag == 0 )
   {
/*	   if( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0422",4)) 
	   {
		   len = strlen(Auth_Tx.TLF01_details.voice_auth_text);
		   if (( len > 0 ) && (len < 240 ))
		   {
				memcpy( HOST_MESSAGE.addl_data,Auth_Tx.TLF01_details.voice_auth_text,
						len );
				sprintf(str_len,"%003d",len);
				memcpy( HOST_MESSAGE.addl_data_len,str_len,3);
		   }
		   else
		   {
			   sprintf(err_buf,"Invalid length for Field 48");
			   nceqit_log_message(3,3,err_buf,"build_request_field_48");
		   }
	   } */
	   /* Dont pass zeros for the transactions coming from POS, Girija Y, TF */
	  // if( 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",6)) 
	   //{
		  /* It is always the same length. */
		  memcpy( HOST_MESSAGE.addl_data_len, "038", 3 );

		  /* Always space fill for 0200 requests to Megalink.
		   * For requests to EB Host or other Megalink requests, zero fill
		   * unless there is a card holder billing amount. Then put that
		   * amount here.  This amount is DE6 from Visa.
		   */
		  if ( ((Auth_Tx.TLF01_details.message_type[1] == '2')  ||
				(Auth_Tx.TLF01_details.message_type[1] == '1')) &&
			   ( Network == NCMLNK ) )
		  {
			 /* Always space fill for Megalink 0200 messages. */
			 memset( HOST_MESSAGE.addl_data, ' ', 38 );
		  }
		  else
		  {
			 memset( HOST_MESSAGE.addl_data, '0', 38 );

			 if ( Auth_Tx.ch_billing_amt[0] != 0x00 )
				memcpy( HOST_MESSAGE.addl_data,Auth_Tx.ch_billing_amt,12);

			 if( Auth_Tx.ch_billing_curr_code[0] != 0x00 )
				  memcpy( &HOST_MESSAGE.addl_data[12],Auth_Tx.ch_billing_curr_code,3);

		  }
	   //}
	   
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_49
 *
 *  DESCRIPTION:  This function will move the currency code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_49( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( Network == NCEQIT )
   {
      if ( *result_flag == 0 )
      {
         if ( false == isalphanum(Auth_Tx.TLF01_details.currency_code) )
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                 "Unable to build request msg to host. Invalid currency code (%s)",
                  Auth_Tx.TLF01_details.currency_code );
         }
         else
         {
            if ( 0 == strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code) )
            {
               memcpy( HOST_MESSAGE.tran_curr_cd,
                       Auth_Tx.TLF01_details.currency_code,
                       DE49_SIZE );
            }
            else if(0 == strcmp(Auth_Tx.reconciliation_cur_code,
                                Auth_Tx.TLF01_details.currency_code) )
            {
               memcpy( HOST_MESSAGE.tran_curr_cd,
                       Auth_Tx.TLF01_details.currency_code,
                       DE49_SIZE );
            }
            else
            {
               memcpy( HOST_MESSAGE.tran_curr_cd,
                       Auth_Tx.reconciliation_cur_code,
                       DE49_SIZE );
            }
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_52
 *
 *  DESCRIPTION:  This function will move the pin block
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_52( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;
   CHAR tempstr[20];


   if ( *result_flag == 0 )
   {
	   /* Dont pass zeros if PIN BLOCK does not exist,commented below, Girija Y, TF */
      /* Field is mandatory. Zero fill it if it does not exist. */
 /*     if ( Auth_Tx.TLF01_details.pin_block[0] == 0x00 ) 
      {
		 {
			memset( HOST_MESSAGE.pinblock, 0xf0, DE52_SIZE );
		 }
      }*/
      if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 ) 
      {
         /* Left justify, padded with "F"s. */
         memset( HOST_MESSAGE.pinblock, 255, DE52_SIZE );
         memset(tempstr, 0, sizeof(tempstr));

         convert_to_ebcdic( Auth_Tx.TLF01_details.pin_block,
                            &tempstr[0],
                            strlen(Auth_Tx.TLF01_details.pin_block));

         len = strlen( tempstr );
         if ( len > DE52_SIZE )
         {
            ret_val = false;
            *result_flag = 1;
            strcpy( err_buf, "Unable to build request msg for host. PIN too long." );
         }
         else
         {
            memcpy( HOST_MESSAGE.pinblock, tempstr, strlen(tempstr) );
         }

		 
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_55
 *
 *  DESCRIPTION:  This function will move the ICC data
 *                
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
INT build_request_field_55( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  temp_len;
   CHAR tempstr[20];
   BYTE emv_out_len[4]={0};
   BYTE Bin_len[2]={0};
   INT tag_len=0;


   if ( *result_flag == 0 )
   {

		if(Auth_Tx.EMV_details.emv_block[0] != 0x00 && Auth_Tx.EMV_details.emv_block[1] != 0x00 )
		{
			
			/* If transaction from POS. Len is 2 bytes and followed by Data. This has
			 * convert in correct format for IST */
			if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",3)== 0)
			{
				temp_len = genutil_bcd_to_int( Auth_Tx.EMV_details.emv_block , 2 );

				if(temp_len > 0 && temp_len < 255)
				{// Dat set ID
					HOST_MESSAGE.ICCdata[0]=0x01;
					HOST_MESSAGE.ICCdata[1]=0x00;
					// Date set Len should converted to BCD format
					sprintf( emv_out_len, "%01x", temp_len );
					genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)Bin_len,&tag_len);
					memcpy(&HOST_MESSAGE.ICCdata[2],Bin_len,1);
					// Copy EMV data
					memcpy(&HOST_MESSAGE.ICCdata[3],&Auth_Tx.EMV_details.emv_block[2],temp_len);

					sprintf( emv_out_len, "%01x", temp_len+3 );
					genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)Bin_len,&tag_len);
					memcpy(HOST_MESSAGE.ICCdata_len,Bin_len,1);

				}
			}
			else
			{	/* Transactions coming from Card brands. VISA/AMEX/MC*/
			memcpy(HOST_MESSAGE.ICCdata_len,Auth_Tx.EMV_details.emv_block,1);
			temp_len = HOST_MESSAGE.ICCdata_len[0];

			if(temp_len > 0 && temp_len < 255)
			memcpy(HOST_MESSAGE.ICCdata,&Auth_Tx.EMV_details.emv_block[1],temp_len);
		}
		}
   
   }
   return( ret_val );
}

//build_request_field_60
/******************************************************************************
 *
 *  NAME:         build_request_field_60
 *
 *  DESCRIPTION:  This function will move the private field 62 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani - TF
 *
 ******************************************************************************/
INT build_request_field_60( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT   len_temp;
   INT 	ProdCode_len=0;

   if ( *result_flag == 0 )
   {	/* Fixed len 12*/
		memset(HOST_MESSAGE.priv60, ' ',12);
		len_temp = strlen(Auth_Tx.TLF01_details.nfi_name);
	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",3)== 0)
		{
			if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3)== 0)
			{	/*Fall back case from POS*/
				strncpy(Auth_Tx.TLF01_details.nfi_name,"0520000000",10);
				memcpy(HOST_MESSAGE.priv60,"052000000000",12);
			}else
			{
				strncpy(Auth_Tx.TLF01_details.nfi_name,"0500000000",10);
				memcpy(HOST_MESSAGE.priv60,"050000000000",12);
			}
		}
		else
		{
			// For all the other brands except visa, DE60 is build as "0000000000"
			if(len_temp == 0)
			{
				len_temp = 10;
				strncpy(Auth_Tx.TLF01_details.nfi_name,"0000000000",10);
				memcpy(HOST_MESSAGE.priv60,Auth_Tx.TLF01_details.nfi_name,len_temp);
			}
		/* For visa, we will use Auth_Tx.TLF01_details.product_codes[16].amount( complete 12 bytes), instead of nfi_name( 10 bytes)
		 * Prod issue: 7/2/2021
		 * Flow: issuing to IST
		*/
			if ( Auth_Tx.TLF01_details.card_num[0]=='4' ||
			   (0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA)))
			{
				ProdCode_len = strlen(Auth_Tx.TLF01_details.product_codes[16].amount);
				memcpy(HOST_MESSAGE.priv60,Auth_Tx.TLF01_details.product_codes[16].amount,ProdCode_len);
			}

		}
		
	
		
		
   }// Log in  TLF01
   strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,HOST_MESSAGE.priv60,12);

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_62
 *
 *  DESCRIPTION:  This function will move the private field 62 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
 ******************************************************************************/
INT build_request_field_62( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE   F62BitMap[8];
   INT    F62Len,F62Len_offset;
   INT    i; 
   BYTE   visa_tran[17];
   BYTE   visa_tran_bcd[8];
   CHAR   len_temp[4];

   if ( *result_flag == 0 )
   {
	  if (Auth_Tx.TLF01_details.tx_key < AUTH_INVALID)  
   {
     
         /* Initialize bitmap and field length. */
         memset( F62BitMap, 0x00, 8 );
         F62Len = 8;   /* Length of bitmap */

		 F62Len_offset = 16;



            /* FIELD 62.1 - AUTH CHARACTERISTICS INDICATOR */
            i = 1;
            if(Auth_Tx.TLF01_details.visa_char_indicator == 0x00)
              HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'Y', A2E );
			

            else if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
                      (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT)  )
            {
               if ( Auth_Tx.TLF01_details.visa_char_indicator == 'E' )
               {
                  /* Only send this field if E for these 2 txns types. */
                 HOST_MESSAGE.priv62[F62Len] = 
                 ascii_ebcdic_conv( Auth_Tx.TLF01_details.visa_char_indicator,
                                     A2E );
				
               }
               else
                  i = 0;
            }
            else
            //   HOST_MESSAGE.priv62[F62Len] = 
            //   ascii_ebcdic_conv( Auth_Tx.TLF01_details.visa_char_indicator,
                               //    A2E );
		
			   HOST_MESSAGE.priv62[F62Len_offset] = Auth_Tx.TLF01_details.visa_char_indicator;

            if ( i == 1 )
            {
               F62BitMap[0] |= 0x80;      /* Set bit 1 */
               F62Len += 1;
			   F62Len_offset += 1;
            }

            /* FIELD 62.2 - VISA TRANSACTION ID */
            if(Auth_Tx.TLF01_details.visa_tran_id[0] !=  0x00)
            {
               memset(visa_tran, 0x00, sizeof(visa_tran) );
               memset(visa_tran_bcd, 0x00, sizeof(visa_tran_bcd) );

               visa_tran[0] = '0'; /* Right justify the tran id. */
               memcpy(visa_tran+1, Auth_Tx.TLF01_details.visa_tran_id,15);

               F62BitMap[0] |= 0x40;     /* Set bit 2 */
           //    asc_to_bcd( visa_tran, 16, visa_tran_bcd );
           //    memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran, 16 );
			    memcpy(HOST_MESSAGE.priv62+F62Len_offset,
			 	    Auth_Tx.TLF01_details.visa_tran_id,15);
               F62Len += 8;
			   F62Len_offset += 15;
            }
		
            //VISA4 - April Mandates 2024
            /* FIELD 62.4 - MARKET SPECIFIC DATA */
            if(Auth_Tx.TLF01_details.product_codes[15].quantity[0] != 0x00)
            {
            	HOST_MESSAGE.priv62[F62Len_offset] = Auth_Tx.TLF01_details.product_codes[15].quantity[0];
            	F62BitMap[0] |= 0x10;      /* Set bit 4 */
            	F62Len += 1;
            	F62Len_offset += 1;
            }

            if ( F62Len > 8 )
			{
				hex_to_ascii(F62BitMap,HOST_MESSAGE.priv62,8);
				sprintf(len_temp,"%003d",F62Len_offset);
	            memcpy(HOST_MESSAGE.priv62_len,len_temp,3);
			//	memcpy( &HOST_MESSAGE.priv62, F62BitMap, 8 );
			//	HOST_MESSAGE.priv62_len[0] = F62Len;
			}
       }

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_63
 *
 *  DESCRIPTION:  This function will move the private field 63 
 *                to the Host Message structure according to Equitable
 *                formatting specifications.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y
 *
 ******************************************************************************/
INT build_request_field_63( pINT result_flag, pCHAR err_buf )
{
   
   INT    ret_val = true;
   INT    F63Len_offset = 0;
   CHAR   strLen[3]; 
   BYTE   field63_bitmap_hex[4]={0};
   BYTE   field63_bitmap_ascii[7]={0};
   BYTE   field63_bitmap_ebcdic[7]={0};
   INT    bitMapsize = 3;
   INT    len = 0;

   

   if ( *result_flag == 0 )
   {
	  
	  /* Initialize bitmap and field length. */

	  memset( field63_bitmap_hex,    0x00, sizeof(field63_bitmap_hex)   );
      memset( field63_bitmap_ebcdic, 0x00, sizeof(field63_bitmap_ebcdic));
      memset( field63_bitmap_ascii,  0x00, sizeof(field63_bitmap_ascii) );

	  F63Len_offset = 6;

      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT ) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_SALE ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY ) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION )||  
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM) ||
		    (Auth_Tx.TLF01_details.tx_key == AUTH_OCT_TX )||  
		   (Auth_Tx.TLF01_details.tx_key == AUTH_FAST_FUND))
	  {
		 
			 /* Echo field 63.1 only. Network ID */
			 if( Auth_Tx.TLF01_details.product_codes[10].code[0] != 0x00 )
			 {
   				 memcpy(&HOST_MESSAGE.priv63[F63Len_offset],
			 			Auth_Tx.TLF01_details.product_codes[10].code,4);
				 field63_bitmap_hex[0] |= 0x80;      /* Set bit 1 */
				 F63Len_offset += 4;

			 }
	  }
	  
		  /* Plus Proprietary Member Center ID field 63.3 */
		   if ( (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ) ||
           (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT ) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER) ||  
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM))
		  {
			
			if( Auth_Tx.TLF01_details.product_codes[11].code[0] != 0x00 )
			{
					 memcpy(&HOST_MESSAGE.priv63[F63Len_offset],
						   Auth_Tx.TLF01_details.product_codes[11].code,4);
					 field63_bitmap_hex[0] |= 0x20;      /* Set bit 3 */
					 F63Len_offset+= 4;

			}
				
		   }

		  /* Plus Proprietary Member Center ID field 63.5 */
		  if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT))
		  {
			  if(Auth_Tx.TLF01_details.product_codes[6].amount[0] != 0x00 )
			  {
				  memcpy(&HOST_MESSAGE.priv63[F63Len_offset], 
						 Auth_Tx.TLF01_details.product_codes[6].amount,4);
				  field63_bitmap_hex[0] |= 0x08;      /* Set bit 5 */
				  F63Len_offset+= 4;

			  }
			  
		  }
		  /* 63.13 */
		  if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
		   (Auth_Tx.TLF01_details.tx_key == AUTH_REPRESENTMENT) )
		  {
			  memcpy(&HOST_MESSAGE.priv63[F63Len_offset], 
						 Auth_Tx.TLF01_details.product_codes[4].amount,6);
				  field63_bitmap_hex[0] |= 0x08;      /* Set bit 13 */
				  F63Len_offset+= 4;

		  }
		  

	  hex_to_ebcdic(field63_bitmap_hex,field63_bitmap_ebcdic,bitMapsize);
      convert_to_ascii( field63_bitmap_ebcdic, field63_bitmap_ascii, (2*bitMapsize));

	  if( F63Len_offset > 6 )
	  {
		memcpy(HOST_MESSAGE.priv63,field63_bitmap_ascii,(2*bitMapsize));
		
		sprintf( strLen, "%02d", F63Len_offset );
		memcpy( HOST_MESSAGE.priv63_len, strLen, 2);
	  }

   }
   
     
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_request_field_90
 *
 *  DESCRIPTION:  This function will move the original data elements
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_90( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Original message type - always 0200. */
      memcpy( HOST_MESSAGE.orig_data, "0200", 4 );

      /* Original System Trace Audit Number */
      if ( DE11_SIZE != strlen(Auth_Tx.TLF01_details.orig_sys_trace_num) )
      {
         ret_val = false;
         sprintf( err_buf,
                 "Invalid orig sys trace audit num (%s) for reversal request.",
                  Auth_Tx.TLF01_details.orig_sys_trace_num );
      }
      else if ( false == isnum(Auth_Tx.TLF01_details.orig_sys_trace_num) )
      {
         ret_val = false;
         sprintf( err_buf,
                 "Invalid orig sys trace audit num (%s) for reversal request.",
                  Auth_Tx.TLF01_details.orig_sys_trace_num );
      }
      else
      {
         memcpy( HOST_MESSAGE.orig_data+4,
                 Auth_Tx.TLF01_details.orig_sys_trace_num,
                 DE11_SIZE);
      }

      /* Original Transmission Timestamp */
      if ( DE7_SIZE != strlen(Auth_Tx.orig_local_time_date) )
      {
         ret_val = false;
         sprintf( err_buf,
                 "Invalid orig time stamp (%s) for reversal request.",
                  Auth_Tx.orig_local_time_date );
      }
      else if ( false == isnum(Auth_Tx.orig_local_time_date) )
      {
         ret_val = false;
         sprintf( err_buf,
                 "Invalid orig time stamp (%s) for reversal request.",
                  Auth_Tx.orig_local_time_date );
      }
      else
      {
         memcpy( HOST_MESSAGE.orig_data+10,
                 Auth_Tx.orig_local_time_date,
                 DE7_SIZE);
      }

      /* Original Acquiring Institution ID Code */
      if ( DE32_SIZE != strlen(Auth_Tx.orig_acq_inst_id) &&
    		  Auth_Tx.orig_acq_inst_id[0] == '\0' )
      {
         /* It does not exist; let's use the same as in DE32. */
         memcpy( Auth_Tx.orig_acq_inst_id,  HOST_MESSAGE.acq_inst_id, DE32_SIZE);
         memcpy( HOST_MESSAGE.orig_data+20, HOST_MESSAGE.acq_inst_id, DE32_SIZE);
      }
      else
      {
         memcpy( HOST_MESSAGE.orig_data+20, Auth_Tx.orig_acq_inst_id, DE32_SIZE);
      }

      if ( false == isnum(Auth_Tx.orig_acq_inst_id) )
      {
         ret_val = false;
         sprintf( err_buf,
                 "Invalid orig Acquiring ID (%s) for reversal request.",
                  Auth_Tx.orig_acq_inst_id );
      }

      if ( Network == NCEQIT )
      {
         /* Original Currency Code */
         (void)build_request_field_49( result_flag, err_buf );
         memcpy( HOST_MESSAGE.orig_data+22, HOST_MESSAGE.tran_curr_cd, 3 );

         /* Filler */
         memset( HOST_MESSAGE.orig_data+25, '0', 17 );
      }
      else
      {
         /* Filler - Megalink does not have currency code */
         memset( HOST_MESSAGE.orig_data+22, '0', 20 );
      }

      if ( false == ret_val )
         *result_flag = 1;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_95
 *
 *  DESCRIPTION:  This function will move the replacement amount
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This is the dispensed amount.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_95( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( false == isnum(Auth_Tx.TLF01_details.actual_amount) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Invalid dispensed amount (%s) - DE95 for reversal request.",
                  Auth_Tx.TLF01_details.actual_amount );
      }
      else
      {
         /* Actual amount: 12; need to stretch it to 13 positions; 1st byte=0 */
         if ( 0 == atol(Auth_Tx.TLF01_details.actual_amount) )
            memset(HOST_MESSAGE.replace_amt, '0', DE95_SIZE );
         else
         {

            memset(HOST_MESSAGE.replace_amt, '0', 1 );
			/* For partial reversals ( 0102 msg ) pass 2 aomunts */
			/* Girija Y, TF Jan 28, 2009 */
            if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis",5) &&
               Auth_Tx.TLF01_details.add_amounts[0] != 0x00 )
            {
				memcpy(HOST_MESSAGE.replace_amt+1,Auth_Tx.TLF01_details.actual_amount, 12);
				memcpy(HOST_MESSAGE.replace_amt+13,Auth_Tx.TLF01_details.add_amounts, 12);
				memset(HOST_MESSAGE.replace_amt+25, '0', DE95_SIZE-25 ); /* Filler */
            }
            else if( Auth_Tx.TLF01_details.add_amounts[0] != 0x00 )
			{
				memcpy(HOST_MESSAGE.replace_amt+1,Auth_Tx.TLF01_details.add_amounts, 12);
				memcpy(HOST_MESSAGE.replace_amt+13,Auth_Tx.TLF01_details.actual_amount, 12);
				memset(HOST_MESSAGE.replace_amt+25, '0', DE95_SIZE-25 ); /* Filler */
			}
			else
			{
				memcpy(HOST_MESSAGE.replace_amt+1,Auth_Tx.TLF01_details.actual_amount,12);
				memset(HOST_MESSAGE.replace_amt+13, '0', DE95_SIZE-13 ); /* Filler */
			}

         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_102
 *
 *  DESCRIPTION:  This function will move the 'from' acct data to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is alphanumeric, variable length.
 *
 *                This field is only for Megalink, not for EB Host.  It
 *                is zero filled; need to do this because Megalink host
 *                expects this field, even though there is no data.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_102( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( Network == NCMLNK )
   {
      if ( *result_flag == 0 )
      {
         sprintf( HOST_MESSAGE.acct1_len, "%02d", DE102_MEGALINK_SIZE );
         memset(  HOST_MESSAGE.acct1,      0x30,  DE102_MEGALINK_SIZE );
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_request_field_103
 *
 *  DESCRIPTION:  This function will move the 'to' acct data to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is alphanumeric, variable length.
 *
 *                This field is only for Megalink, not for EB Host.  It
 *                is zero filled; need to do this because Megalink host
 *                expects this field, even though there is no data.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_103( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( Network == NCMLNK )
   {
      if ( *result_flag == 0 )
      {
         sprintf( HOST_MESSAGE.acct2_len, "%02d", DE103_MEGALINK_SIZE );
         memset(  HOST_MESSAGE.acct2,      0x30,  DE103_MEGALINK_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_104
 *
 *  DESCRIPTION:  This function will move the 'to' acct data to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is alphanumeric, variable length.
 *
 *                This field is only for Megalink, not for EB Host.  It
 *                is zero filled; need to do this because Megalink host
 *                expects this field, even though there is no data.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
/* DE104 DATA SET ID and TAG length value */
/*
DE104 data will be in below format:

Total length -dataset ID -dataset length(hex) - TAG -length -value

only value is in EBCDIC format, rest all in ascii format we need to convet in to EBCDIC format.

*/


INT build_request_field_104( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   CHAR   strLen[4]={0},data_set1[2]={0},tag_id[2]={0};
	BYTE    data_set_length[2]={0},tag_set_len[2]={0};
    int org_len=0,i=0,data_len=0,tag_len=0,DE_104_len=0; //BYTE field104_ebcdic[512],field104_ascii[512];
   BYTE temp_string[100]="";
   BYTE de_104_data[256]={0};
   CHAR chk_data_set[2] = {0}, chk_tag_id[2]= {0};
   INT d56_t81 = false;
   BYTE d56_len[2] = {0};

   org_len = Auth_Tx.future_use_2[0];

   if (org_len > 0)
   {

		if ( org_len < 99 )
		{
			memcpy(de_104_data,&Auth_Tx.future_use_2[1],org_len);
		}
		else if ( org_len < 198)
		{
		   memcpy(de_104_data, &Auth_Tx.future_use_2[1], sizeof(Auth_Tx.future_use_2) -2);
		   memcpy(de_104_data+98, Auth_Tx.future_use_3, org_len-98);
		}
		else if (org_len < 297)
		{
		   memcpy(de_104_data, &Auth_Tx.future_use_2[1], sizeof(Auth_Tx.future_use_2) -2);
		   memcpy(de_104_data+98, Auth_Tx.future_use_3, sizeof(Auth_Tx.future_use_3) -1);
		   memcpy(de_104_data+197, Auth_Tx.future_use_4, org_len-197);
		}
   }



   if ( *result_flag == 0 && org_len >0)
      {
			i =0;DE_104_len =0;
		while (org_len > i)
		{//Get the first data set ID - 1 byte
			strncpy(data_set1,&de_104_data[i],1);
			 hex_to_ebcdic(data_set1,temp_string,1);
			 hex_to_ascii(data_set1, chk_data_set, 1);
			 memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],1);
			 i++;DE_104_len +=1;
		// Get 2 byte length - 2 byte
			memcpy(data_set_length,&de_104_data[i],2);
			hex_to_ebcdic(data_set_length,temp_string,2);
			memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],2);
			i += 2;DE_104_len +=2;
		// Based on length get the TAG and TAG length information	
			data_len = data_set_length[0];
			data_len += data_set_length[1];
		
			do
			{// Parse each TAG and copy the data
				
				strncpy(tag_id,&de_104_data[i],1);
				 hex_to_ebcdic(tag_id,temp_string,1);
				 hex_to_ascii(tag_id, chk_tag_id, 1);
				 if (0 == strncmp(chk_data_set, "56", 2 ) &&
				 	 0 == strncmp(chk_tag_id, "81", 2))
				 {
					 d56_t81 = true;
				 }

				 if (d56_t81 == true)
				 {
					 //strncpy(&HOST_MESSAGE.trandesc[DE_104_len],temp_string,2);
					  i += 1;data_len -= 1;
					 //tag_len = atoi(&Auth_Tx.TLF01_details.voice_auth_text[i]);
					 strncpy(tag_set_len,&de_104_data[i],1);
					 tag_len = tag_set_len[0];
					 hex_to_ebcdic(tag_set_len,temp_string,1);
					 //strncpy(&HOST_MESSAGE.trandesc[DE_104_len],temp_string,2);
					 i += 1;data_len -= 1;
					 //strncpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],tag_len);
					 i +=tag_len ; data_len -= tag_len;

					 d56_len[1] = data_len;
					 hex_to_ebcdic(d56_len,temp_string,2);
					 memcpy(&HOST_MESSAGE.trandesc[DE_104_len-2],d56_len,2);
					 d56_t81 = false;
				 }
				 else
				 {
					 memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],1);
					  i += 1;DE_104_len +=1;data_len -= 1;
					 /* data set 5C has 2 byte tagids, copy the next byte after "1FXX" */
					 if (0 == strncmp(chk_data_set, "5C", 2) &&
						 0 == strncmp(chk_tag_id, "1F", 2))
					 {
						 memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],1);
						 i += 1;DE_104_len +=1;data_len -= 1;
					 }
				 //tag_len = atoi(&Auth_Tx.TLF01_details.voice_auth_text[i]);
				 strncpy(tag_set_len,&de_104_data[i],1);
				 tag_len = tag_set_len[0];
				 hex_to_ebcdic(tag_set_len,temp_string,1);
					 memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],1);
					 i += 1;DE_104_len +=1;data_len -= 1;
					 memcpy(&HOST_MESSAGE.trandesc[DE_104_len],&de_104_data[i],tag_len);
				 i +=tag_len ; DE_104_len +=tag_len;data_len -= tag_len;

				 }

			}while (data_len > 0);
			i += data_len;
		}

		sprintf( strLen, "%03d", DE_104_len );
		memcpy( HOST_MESSAGE.trandesc_len, strLen, 3);
		
      }
   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_115
 *
 *  DESCRIPTION:  This function will move the 'to' acct data to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  It is alphanumeric, variable length.
 *
 *                This field is only for Megalink, not for EB Host.  It
 *                is zero filled; need to do this because Megalink host
 *                expects this field, even though there is no data.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/

INT build_request_field_115( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT len;
   len = strlen(Auth_Tx.TLF01_details.dcf01_retired_cwk);
   if ( *result_flag == 0 && len > 0)
      {
         sprintf( HOST_MESSAGE.addl_trace_len, "%02d", len );
         memcpy(  HOST_MESSAGE.addl_trace,  Auth_Tx.TLF01_details.dcf01_retired_cwk, len  );
      }
   
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_126
 *
 *  DESCRIPTION:  This function will move the message authentication code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *                This field is mandatory, but not used (Huh?). Space fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_126( pINT result_flag, pCHAR err_buf )
{
	#define BCD_BITMAP_LEN	 8
	#define ASC_BITMAP_LEN  16
	#define ASCII_BITMAP 	"00C0000000000000"
	#define MIN_126_LEN		14

	 INT	 i             		= 0;
	 INT	 cps_bit	  		= 0;
	 INT	 cps_byte	   		= 0;
	 INT	 TotalLen	   		= 0;
	 INT	 inptr		   		= BCD_BITMAP_LEN;	/* Point to inbuf, past bitmap. */
	 INT	 outptr 	   		= ASC_BITMAP_LEN;	/* Point to outstring, past bitmap */
	 BYTE	 bcd_BitMap[8] 		= "";
	 BYTE	 asc_BitMap[17]		= "";
	 BYTE	 inbuf[255]    		= "";
	 BYTE	 tempstr[58]   		= {0};
	 BYTE	 outstring[255]		= "";
	 BYTE    RPS_Indicator 		= 0;
     INT  	 ret_val 	   		= true;
     INT  	 temp_len      		= 0;
     BYTE 	 temp_msg[250] 		= {0};
     BYTE 	 de126_bitmap[17]	= {0};
     BYTE 	 ascii_126_9[41] 	= {0};
     BYTE    ascii_126_8[41]={0};
     BYTE    ascii_126_19[2]={0};
     BYTE    ascii_126_20[2]={0};
     BYTE 	 ascii_126_10[7] 	= {0};
     INT     de126_len		 	= 0 ;

     TotalLen=(INT)Auth_Tx.TLF01_details.voice_auth_text[0];

     if ( *result_flag == 0 &&  TotalLen >= MIN_126_LEN  && TotalLen < sizeof(HOST_MESSAGE.de126) )
     { /* Added MIN_126_LEN (An additional check)check to avoid the wrong data sending*/
    	 memcpy( inbuf, &Auth_Tx.TLF01_details.voice_auth_text[1], TotalLen );

    	 /* Copy bitmap into local variable. */
    	 memcpy( bcd_BitMap, &Auth_Tx.TLF01_details.voice_auth_text[1], BCD_BITMAP_LEN );


    	 /* Convert bitmap to ASCII. */
    	 bcd_to_asc( bcd_BitMap, BCD_BITMAP_LEN, asc_BitMap, ASC_BITMAP_LEN );
    	 memcpy( outstring, asc_BitMap, ASC_BITMAP_LEN );

    	 /* Parse the bitmap for the first 2 bytes only - others are unused. */
    	 for( cps_byte=1; cps_byte<=3; cps_byte++ )
    	 {
    		 for( cps_bit=1; cps_bit<=8; cps_bit++ )
    		 {
    			 if ( bcd_BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
    			 {
    				 /* The bit is set. */
    				 switch( cps_byte )
    				 {
    				 	 case 1: /* Byte 1 */
    				 		 switch( cps_bit )
    				 		 {
    				 		 	 case 1:
    				 		 		 /* Customer Name */
    				 		 		 memset(tempstr, 0x00, sizeof(tempstr) );
    				 		 		 for( i=0; i<25; i++ )
    				 		 		 {
    				 		 			 tempstr[i] =
    				 		 					 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
    				 		 		 }
    				 		 		 strcat( outstring+outptr, tempstr );
    				 		 		 inptr  += 25;
    				 		 		 outptr += 25;
    				 		 		 break;
    				 	  case 2:
							   /* Customer Address */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<57; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   inptr  += 57;
							   outptr += 57;
							   break;
						  case 3:
							   /* Biller Address */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<57; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   inptr  += 57;
							   outptr += 57;
							   break;
						   case 4:
							   /* Biller Telephone Number */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<18; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   inptr  += 18;
							   outptr += 18;
							   break;
						    case 5:
							   /* Process by Date */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<8; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   inptr  += 8;
							   outptr += 16;
							   break;
						    case 6:
							   /* Cardholder Certificate Serial Number */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   genutil_bin_to_ascii( &inbuf[inptr], 1,
													 tempstr, sizeof(tempstr));
							   strncat( outstring+outptr, tempstr, 2 );
							   inptr++;
							   outptr += 2;
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
							   strcat( outstring+outptr, tempstr );
							   inptr  += 16;
							   outptr += 32;
							   break;
						    case 7:
							   /* Merchant Certificate Serial Number */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   genutil_bin_to_ascii( &inbuf[inptr], 1,
													 tempstr, sizeof(tempstr));
							   strncat( outstring+outptr, tempstr, 2 );
							   inptr++;
							   outptr += 2;
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
							   strcat( outstring+outptr, tempstr );
							   inptr  += 16;
							   outptr += 32;
							   break;
						     case 8:
							   /* Transaction ID (VSEC) */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   genutil_bin_to_ascii( &inbuf[inptr], 20,
													 tempstr, sizeof(tempstr));
							   strcat( outstring+outptr, tempstr );
							   strncpy(ascii_126_8,tempstr,40);
							   inptr  += 20;
							   outptr += 40;
							   break;
					 }
					 break;
			  case 2: /* Byte 2 */
					 switch( cps_bit+8 )
					 {
						case  9:
							   /* TranStain (VSEC) CAVV DATA for 3D secure*/
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   genutil_bin_to_ascii( &inbuf[inptr], 20,
													 tempstr, sizeof(tempstr));
							   strcat( outstring+outptr, tempstr );

							   strncpy(ascii_126_9,tempstr,40);

							   inptr  += 20;
							   outptr += 40;

							   break;
						case 10:
							   /* CVV2 Request Data */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<6; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   strncpy(ascii_126_10,tempstr,6);//Asciic format
							   inptr  += 6;
							   outptr += 12;

							   break;
						case 11:
							   /* IGOTS Transaction Description */
							   memset(tempstr, 0x00, sizeof(tempstr) );
							   for( i=0; i<2; i++ )
							   {
								  tempstr[i] =
									 ascii_ebcdic_conv( inbuf[inptr+i], E2A );
							   }
							   strcat( outstring+outptr, tempstr );
							   inptr  += 2;
							   outptr += 2;
							   break;
						case 12:
							   /* Service Development Indicator */
							   strncat( outstring+outptr, &inbuf[inptr], 6 );

							   // Increment pointers as we are parsing for other fields now.
							   inptr  += 3;
							   outptr += 6;
							   break;

						case 13:
								 memset(tempstr, 0x00, sizeof(tempstr) );
								 RPS_Indicator = tempstr[0] = ascii_ebcdic_conv( inbuf[inptr], E2A );
								 strncat( outstring+outptr, &inbuf[inptr], 1 );
								 inptr  += 1;
							     outptr += 1;
							   break;
					 }
					 break;
					 case 3: /* Byte 3 */
					 switch( cps_bit+16 )
					 {
					 case  19:
					    memset(tempstr, 0x00, sizeof(tempstr) );
					    tempstr[0] = ascii_ebcdic_conv( inbuf[inptr], E2A );
					    strncat( outstring+outptr, tempstr,1);
					    strncpy(ascii_126_19,tempstr,1);//Asciic format
                        inptr  += 1;
                        outptr += 1;
					    break;
					 case  20:
					    memset(tempstr, 0x00, sizeof(tempstr) );
					    tempstr[0] = ascii_ebcdic_conv( inbuf[inptr], E2A );
					    strncat( outstring+outptr, tempstr,1);
					    strncpy(ascii_126_20,tempstr,1);//Asciic format
                        inptr  += 1;
                        outptr += 1;
					    break;
					 }
					break;
			  default: break;
		   }  /* End SWITCH based on BYTE */
		}  /* End IF bit is set */
	 }	/* End LOOP on BIT */
  }  /* End LOOP on BYTE */

/* IST requires only 126.9 and 126.10 dont send other fileds*/
 // LLL  3byte EBCDIC length
 // 8 BYTE bitmap in EBCD format
 // 126.9 in EBCDIC format - 20 bytes binary 40 bytes EBCDIC
 // 126.10 in EBCDIC format - 6 bytes EBCDIC
   
	//convert_to_ebcdic(ASCII_BITMAP,de126_bitmap,16);

  	  /* If 126.9 and 126.10 both present then only copy both else check for each 126.9 and 126.10,
  	   *  send which ever is present.
  	   *  If both are not their , don't send DE126, memset DE126 so that it won't be sent.  */

    	 int data_pos_126 = 16;
		 char Bit_map_126[17]= {0};
		 strncpy(Bit_map_126, "0000000000000000",16);
		 memset(HOST_MESSAGE.de126, 0x00, sizeof(HOST_MESSAGE.de126));

		 if (strlen(ascii_126_8) > 0 )
		 {
			 // Set the bitMap in Bit_map_126
			 Bit_map_126[1] |= 0x01;
			 // Copy the bitmap in 1st 16 byte
			 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
			 memcpy(  HOST_MESSAGE.de126+16, ascii_126_8, 40  );
			 data_pos_126 = 40 +16;

		}
		 if (strlen(ascii_126_9) > 0 )
		 {
			 // Set the bitMap in Bit_map_126
			 Bit_map_126[2] ^= 0x08;
			 // Copy the bitmap in 1st 16 byte
			 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
			 memcpy(  HOST_MESSAGE.de126+ data_pos_126,    ascii_126_9, 40  );
			 data_pos_126 = data_pos_126 + 40;

		 }
		 if (strlen(ascii_126_10) > 0 )
		 {
			 // Set the bitMap in Bit_map_126
			 if (strlen(ascii_126_9) > 0)
				 Bit_map_126[2] ^= 0x7B;
			 else
				 Bit_map_126[2] ^= 0x04;
			 // Copy the bitmap in 1st 16 byte
			 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
			 memcpy(  HOST_MESSAGE.de126+ data_pos_126,    ascii_126_10, 6  );
			 data_pos_126 = data_pos_126 + 6;

		 }

		 if (0 != strncmp(Auth_Tx.TLF01_details.currency_code, "608", 2))
		 {
			 if (strlen(ascii_126_19) > 0 )
			 {
				 // Set the bitMap in Bit_map_126
				 Bit_map_126[4] |= 0x02;
				 // Copy the bitmap in 1st 16 byte
				 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
				 memcpy(  HOST_MESSAGE.de126+ data_pos_126,    ascii_126_19, 1  );
				 data_pos_126 = data_pos_126 + 1;
			 }
			 else if (strlen(ascii_126_19) == 0 )
			 {
				 // Set the bitMap in Bit_map_126
				 Bit_map_126[4] |= 0x02;
				 // Copy the bitmap in 1st 16 byte
				 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
				 memcpy(  HOST_MESSAGE.de126+ data_pos_126, " ", 1  );
				 data_pos_126 = data_pos_126 + 1;
			 }
		 }


		 if (strlen(ascii_126_20) > 0 )
		 {
			 // Set the bitMap in Bit_map_126
			 Bit_map_126[4] |= 0x01;
			 // Copy the bitmap in 1st 16 byte
			 memcpy(HOST_MESSAGE.de126, Bit_map_126, 16);
			 memcpy(  HOST_MESSAGE.de126+ data_pos_126,    ascii_126_20, 1  );
			 data_pos_126 = data_pos_126 + 1;

		 }

		 sprintf( HOST_MESSAGE.de126_len, "%03d", data_pos_126 );

		 if (data_pos_126 > 0)
			 second_Bitmap = true;

    /*if(strlen(ascii_126_9) > 0 && strlen(ascii_126_10) > 0 &&
    		strlen(ascii_126_8) > 0 && strlen(ascii_126_20) > 0)
    {
		strncpy(  HOST_MESSAGE.de126,	"01C0100000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_8, 40  );
		memcpy(  HOST_MESSAGE.de126+56,	ascii_126_9, 40  );
		memcpy(  HOST_MESSAGE.de126+96,	ascii_126_10,6  );
		memcpy(  HOST_MESSAGE.de126+102,ascii_126_20,1  );
		de126_len = strlen(HOST_MESSAGE.de126);

    }
    else if(strlen(ascii_126_9) > 0 && strlen(ascii_126_10) > 0 && strlen(ascii_126_20) > 0)
    {
		strncpy(  HOST_MESSAGE.de126,	"00C0100000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_9, 40  );
		memcpy(  HOST_MESSAGE.de126+56,	ascii_126_10,6  );
		memcpy(  HOST_MESSAGE.de126+62,ascii_126_20,1  );
		de126_len = strlen(HOST_MESSAGE.de126);

    }
    else if(strlen(ascii_126_9) > 0 && strlen(ascii_126_10) > 0 && strlen(ascii_126_8) > 0)
    {
		strncpy(  HOST_MESSAGE.de126,	"01C0000000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_8, 40  );
		memcpy(  HOST_MESSAGE.de126+56,	ascii_126_9,40  );
		memcpy(  HOST_MESSAGE.de126+96,ascii_126_10,1  );
		de126_len = strlen(HOST_MESSAGE.de126);

    }
    else if(strlen(ascii_126_9) > 0 && strlen(ascii_126_10) > 0)
	{
		strncpy(  HOST_MESSAGE.de126,	"00C0000000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_9, 40  );
		memcpy(  HOST_MESSAGE.de126+56,	ascii_126_10, 6  );
		de126_len = 62 ;
	}
	else if(strlen(ascii_126_9) > 0 && strlen(ascii_126_9) < 41)
	{
		strncpy(  HOST_MESSAGE.de126,	"0080000000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_9, 40  );
		de126_len = 56 ;
	}
	else if(strlen(ascii_126_10) > 0 && strlen(ascii_126_10) < 7)
	{
		strncpy(  HOST_MESSAGE.de126,	"0040000000000000" ,16  );
		memcpy(  HOST_MESSAGE.de126+16,	ascii_126_10, 6  );
		de126_len = 22 ;
	}
	else
	{
		de126_len = 0;
		memset(HOST_MESSAGE.de126,0x00,sizeof(HOST_MESSAGE.de126));
		return ret_val;
	}*/



	}


	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_request_field_128
 *
 *  DESCRIPTION:  This function will move the message authentication code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *                This field is mandatory, but not used (Huh?). Space fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_request_field_128( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( Network == NCMLNK )
      {
         memset( HOST_MESSAGE.msg_authc, 0x30, DE128_SIZE );
      }
      else
      {
         /* This field is not supported for EB Host.  Space fill it. */
         memset( HOST_MESSAGE.msg_authc, ' ', DE128_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_2
 *
 *  DESCRIPTION:  This function will move the card number to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits. This is an echo from the request.
 *                Therefore, there is no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_2( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.TLF01_details.card_num );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build response message to host. No card number." );
      }
      else
      {
         sprintf( HOST_MESSAGE.pan_len, "%02d", len );
         memset( HOST_MESSAGE.pan, '0', MAX_DE2_SIZE );
         memcpy( HOST_MESSAGE.pan, Auth_Tx.TLF01_details.card_num, len );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_3
 *
 *  DESCRIPTION:  This function will move the processing code to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed length.
 *                This is an echo from the request. Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_3( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.proccode,
              Auth_Tx.TLF01_details.processing_code,
              DE3_SIZE );
      /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
      memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,
              Auth_Tx.TLF01_details.processing_code,
              DE3_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_4
 *
 *  DESCRIPTION:  This function will move the amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *                This is an echo from the request. Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_4( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE )
      {
         /* EQ Host expects zero in DE4 for Balance Inquiry. */
         memcpy( Auth_Tx.TLF01_details.total_amount, "000000000000", 12 );
      }
      memcpy( HOST_MESSAGE.amount,
              Auth_Tx.TLF01_details.total_amount,
              DE4_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_5
 *
 *  DESCRIPTION:  This function will move the settlement amount to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_5( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

     
   if ( *result_flag == 0 )
   {
	   if(Auth_Tx.reconciliation_amount[0] != 0x00)
	   {
			memcpy( HOST_MESSAGE.setl_amount,
				  Auth_Tx.reconciliation_amount,
					DE5_SIZE );
	   }
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_response_field_7
 *
 *  DESCRIPTION:  This function will move the transmission date and time
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_7( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Original timestamp is stored in product codes[19].
       * This is so it can be overwritten into GMT format.
       * Reference function 'move_field_7'.
       */
      memcpy( HOST_MESSAGE.gmt_time,
              Auth_Tx.TLF01_details.product_codes[19].amount,
              DE7_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_9
 *
 *  DESCRIPTION:  This function will move the settlement conversion rate
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_9( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      
	  if(Auth_Tx.settlement_conv_rate[0] != 0x00)
		memcpy( HOST_MESSAGE.setl_conv_rate,
              Auth_Tx.settlement_conv_rate,
              DE9_SIZE );
	  
   }

   return( ret_val );

}



/******************************************************************************
 *
 *  NAME:         build_response_field_11
 *
 *  DESCRIPTION:  This function will move the system trace audit number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_11( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.stan,
              Auth_Tx.TLF01_details.sys_trace_audit_num,
              DE11_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_15
 *
 *  DESCRIPTION:  This function will move the settlement date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_15( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25];
   CHAR  date_str[9];

   if ( *result_flag == 0 )
   {
      /* Is settlement date null or all zeroes? */
      if ( (Auth_Tx.TLF01_details.settlement_date[0] == 0x00) ||
           (0 == strcmp(Auth_Tx.TLF01_details.settlement_date,"0000")) )
      {
         /* It does not exist; use txn date. */
         if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
         {
            /* Txn date does not exist; use today's date. */
            memset( time_date, 0x00, sizeof(time_date) );
            memset( date_str,  0x00, sizeof(date_str ) );
            ptetime_get_timestamp( time_date );
            get_date( time_date, date_str );
            memcpy( Auth_Tx.TLF01_details.settlement_date, date_str+4, DE15_SIZE );
         }
         else
         {
            memcpy( HOST_MESSAGE.date_settle,
                    Auth_Tx.TLF01_details.date_yyyymmdd+4,
                    DE15_SIZE );
         }
      }
      else
      {
         memcpy( HOST_MESSAGE.date_settle,
                 Auth_Tx.TLF01_details.settlement_date,
                 DE15_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_16
 *
 *  DESCRIPTION:  This function will move the conversion date
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *                This field is format MMDD.  Interchange conversion date
 *                for PLUS transactions.  Actual business date for the day
 *                that the acquirer transmitted the transaction to MegaLink.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_16( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  time_date[25];
   CHAR  date_str[9];

   if ( *result_flag == 0 )
   {
      /* Is settlement date null or all zeroes? */
      if ( (Auth_Tx.conv_date[0] == 0x00) ||
           (0 == strcmp(Auth_Tx.conv_date,"0000")) )
      {
         /* It does not exist; use txn date. */
         if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
         {
            /* Txn date does not exist; use today's date. */
            memset( time_date, 0x00, sizeof(time_date) );
            memset( date_str,  0x00, sizeof(date_str ) );
            ptetime_get_timestamp( time_date );
            get_date( time_date, date_str );
            memcpy( Auth_Tx.conv_date, date_str+4, DE16_SIZE );
         }
         else
         {
            memcpy( HOST_MESSAGE.date_conv,
                    Auth_Tx.TLF01_details.date_yyyymmdd+4,
                    DE16_SIZE );
         }
      }
      else
      {
         memcpy( HOST_MESSAGE.date_conv,
                 Auth_Tx.conv_date,
                 DE16_SIZE );
      }
   }
   return( ret_val );

   
}

/******************************************************************************
 *
 *  NAME:         build_response_field_19
 *
 *  DESCRIPTION:  This function will move the Acquirer institution ID Country code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_19( pINT result_flag, pCHAR err_buf )
{
  INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(Auth_Tx.country_code[0] != 0x00)
			memcpy( HOST_MESSAGE.acq_inst_cnt_code,
              Auth_Tx.country_code,
              DE19_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_20
 *
 *  DESCRIPTION:  This function will move the Pan extended country code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_20( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(Auth_Tx.pan_ext_ctry_code[0] != 0x00)
			memcpy( HOST_MESSAGE.coun_code,
                 Auth_Tx.pan_ext_ctry_code,
                 DE20_SIZE );
   }
   return( ret_val );

 
}

/******************************************************************************
 *
 *  NAME:         build_response_field_21
 *
 *  DESCRIPTION:  This function will move the Forwarding institution country code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_21( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if(Auth_Tx.country_code[0] != 0x00)
			memcpy( HOST_MESSAGE.fwd_inst_cnt_code,
					     Auth_Tx.country_code,
						DE21_SIZE );


   }
   return( ret_val );

 
}

/******************************************************************************
 *
 *  NAME:         build_response_field_23
 *
 *  DESCRIPTION:  This function will move the Card seq nbr
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Phani
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_23( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
	   if ( isnum(Auth_Tx.host2_message_seq_nbr) )
      {
       strncpy(HOST_MESSAGE.cardseqnum,&Auth_Tx.host2_message_seq_nbr[1],3);
      }

   }
   return( ret_val );

 
}

/******************************************************************************
 *
 *  NAME:         build_response_field_25
 *
 *  DESCRIPTION:  This function will move the POS condition code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {

      /* Set Condition Code based on ATM or POS. */
      memcpy( HOST_MESSAGE.pos_condcd, Auth_Tx.TLF01_details.pos_condition_code, DE25_SIZE );
     
     
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_response_field_32
 *
 *  DESCRIPTION:  This function will move the acquiring institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length. This is an echo from the request.  Therefore, there
 *                is no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_32( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.acq_inst_id_cd );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build response message to host. No acquiring ID." );
      }
      else
      {
         sprintf( HOST_MESSAGE.acq_inst_len, "%02d", len );
         strcpy( HOST_MESSAGE.acq_inst_id, Auth_Tx.acq_inst_id_cd );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_37
 *
 *  DESCRIPTION:  This function will move the retrieval reference number
				  to the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_37( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.TLF01_details.retrieval_ref_num );
      if ( len == 0 )
      {
         ret_val = false;
         *result_flag = 1;
         strcpy( err_buf,
                "Unable to build response message to host. No RRN." );
      }
      else
      {
         memcpy(HOST_MESSAGE.rrn,Auth_Tx.TLF01_details.retrieval_ref_num,DE37_SIZE);
      }
   }
   return( ret_val );

}


/******************************************************************************
 *
 *  NAME:         build_response_field_38
 *
 *  DESCRIPTION:  This function will move the authorization number
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_38( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Since EB Host is fixed ISO, it requires an auth code.
       * If txn is declined or if there is no auth code, set
       * it to all zeroes.
       */
      if ( 0 == strcmp(Auth_Tx.TLF01_details.response_code, APPROVED) )
      {
         /* APPROVED */
         if ( (blank_string(Auth_Tx.TLF01_details.auth_number,6)) ||
              (Auth_Tx.TLF01_details.auth_number[0] == 0x00) )
         {
            /* Approval code is blank or missing. */
            if ( (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE) ||
                 (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) )
            {
               /* Use STAN for these, not sure why. */
               memcpy( Auth_Tx.TLF01_details.auth_number,
                       Auth_Tx.TLF01_details.sys_trace_audit_num,
                       DE38_SIZE );
            }
            else
            {
               /* Approved without an approval code. */
               strcpy( Auth_Tx.TLF01_details.auth_number, "000000" );
            }
         }
      }
      else
      {
         /* Not approved, so zero it out. */
    	 /* BDOPS2024-24, approval code should not be zerod for decline cases */
    	 memset( Auth_Tx.TLF01_details.auth_number, 0x00, sizeof(Auth_Tx.TLF01_details.auth_number));
      }

      /* Verify the approval code. */
      if ( false == isalphanum_space(Auth_Tx.TLF01_details.auth_number) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Unable to build response msg to host. Auth num invalid (%s)",
                  Auth_Tx.TLF01_details.auth_number );
      }
      else
      {
         memcpy( HOST_MESSAGE.authcode,
                 Auth_Tx.TLF01_details.auth_number,
                 DE38_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_39
 *
 *  DESCRIPTION:  This function will move the response code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, fixed
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_39( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      if ( false == isalphanum(Auth_Tx.TLF01_details.response_code) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Unable to build response msg to host. Invalid response code (%s)",
                  Auth_Tx.TLF01_details.response_code );
      }
      else if (0 == strncmp(Auth_Tx.TLF01_details.acquirer_id, "ncvsms", 5) &&
    		  Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE &&
			  Auth_Tx.TLF01_details.response_code == NULL_STR)
      {
    	  strncpy(Auth_Tx.TLF01_details.response_code, TO, 2);
      }
      else
      {
         memcpy( HOST_MESSAGE.responsecd,
                 Auth_Tx.TLF01_details.response_code,
                 DE39_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_41
 *
 *  DESCRIPTION:  This function will move the terminal Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_41( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.termid,
              Auth_Tx.TLF01_details.terminal_id,
              DE41_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_42
 *
 *  DESCRIPTION:  This function will move the merchant Id
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_42( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT i = 0;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.cardaccid,
              Auth_Tx.TLF01_details.merchant_id,
              DE42_SIZE );

	  for(i=0;i<DE42_SIZE;i++)
		  if(HOST_MESSAGE.cardaccid[i] == 0x00)
			  HOST_MESSAGE.cardaccid[i] = 0x20;
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_response_field_44
 *
 *  DESCRIPTION:  This function will move the Original response code in field 44
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
/* NEW */
INT build_response_field_44( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len = 0;
   CHAR strLen[3] = {0};

   if ( *result_flag == 0 )
   {

	   memset(HOST_MESSAGE.addlrspdata,0x20,sizeof(HOST_MESSAGE.addlrspdata));
	   
	   len = 10; 
	   
	   /* 44.11 Original response code. Rest of the fields are set to blank */

	   memcpy( &HOST_MESSAGE.addlrspdata[10],
              Auth_Tx.TLF01_details.response_code,
              2 );
              
       len += 2;
    
	   sprintf( strLen, "%02d", len );

	   memcpy( HOST_MESSAGE.addl_rsp_len, strLen, 2);
    
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_SMS_response_field_48
 *
 *  DESCRIPTION:  This function will move the SMS related additional field data
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_SMS_response_field_48( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val = true;
   INT     i;
   pCHAR   p_text = NULL;

   if ( *result_flag == 0 )
   {
      /* It is always the same length. */
      memcpy( HOST_MESSAGE.addl_data_len, "059", 3 );

	 
	  if(Auth_Tx.MCF01_details.vs_tcc_code[0] == 'P')
	  {
			sprintf( HOST_MESSAGE.addl_data,
                   "%c%9s%4s",
                    Auth_Tx.MCF01_details.vs_tcc_code[0],  
                     Auth_Tx.MCF01_details.negative_group_id,
                     Auth_Tx.MCF01_details.group_type
					 ); 
			for( i=59; HOST_MESSAGE.addl_data[i]==0x00; i-- )
				HOST_MESSAGE.addl_data[i] = ' ';
	  }

	  else
  	  for( i=59; HOST_MESSAGE.addl_data[i]==0x00; i-- )
			HOST_MESSAGE.addl_data[i] = ' ';

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_SMS_Admin_response_field_48
 *
 *  DESCRIPTION:  This function will move the SMS related additional field data
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_SMS_Admin_response_field_48( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val = true;
   pCHAR   p_text = NULL;
   CHAR   strLen[4] = {0};
   INT	  F48Len = 0;

  
	  

   if ( *result_flag == 0 )
   {
      /* It is always the same length. */


  
	  F48Len = strlen(Auth_Tx.TLF01_details.voice_auth_text) + 1;
      sprintf( strLen, "%03d", F48Len );
	  memcpy( HOST_MESSAGE.addl_data_len, strLen, 3);

	 
  	  sprintf( HOST_MESSAGE.addl_data,
                   "%c%s",
                    Auth_Tx.MCF01_details.vs_tcc_code[0],  
                     Auth_Tx.TLF01_details.voice_auth_text); 

	  /*
	  for( i=51; HOST_MESSAGE.addl_data[i]==0x00; i-- )
				HOST_MESSAGE.addl_data[i] = ' ';
				*/

   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         build_response_field_48
 *
 *  DESCRIPTION:  This function will move the balance inquiry data
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *                For rejected transactions, this field contains a message.
 *                If response code starts with 'N', message is left justified,
 *                space filled.  If response is numeric and has a response
 *                description, forward the message to the acquirer.
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_48( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val 				= true;
   INT     i			 		= 0;
   BYTE    sign_current  		= 0;
   BYTE    sign_available		= 0;
   CHAR    current_amount[14] 	= "";
   pCHAR   p_text 				= NULL;
   BYTE    temp_addl_amt[39]	= {0};
   BYTE    temp_acct_type[3]	= {0};
   INT     Pul_addl_amt_len		=  0;
   CHAR    currency_code[4]     = {0};
   CHAR    avail_cash_limit[13] = {0};

   if ( *result_flag == 0 )
   {
      /* It is always the same length. */
      memcpy( HOST_MESSAGE.addl_data_len, "038", 3 );

      /* Is transaction approved? */
      if ( 0 == strcmp(APPROVED, Auth_Tx.TLF01_details.response_code) )
      {
         /* YES, it is approved. Supply balance information if it exists. */

    	  	  /* 1002608C000000000000 */
          if(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncpul",5) )
          {
        	  strncpy(temp_addl_amt,"00000000",8);
        	  Pul_addl_amt_len =  8 ;
        	  strncat(temp_addl_amt,Auth_Tx.TLF01_details.mcard_banknet+4,3);
        	  Pul_addl_amt_len = Pul_addl_amt_len + 3;
        	  strncpy(temp_acct_type,Auth_Tx.TLF01_details.mcard_banknet,2);
        	  Pul_addl_amt_len = Pul_addl_amt_len + 2;
        	  if(0 == strncmp(temp_acct_type,"10",2) )
        	  {
        		  temp_addl_amt[Pul_addl_amt_len]='D';
        	  }
        	  else if (0 == strncmp(temp_acct_type,"30",2) )
        	  {
        		  temp_addl_amt[Pul_addl_amt_len]='C';
        	  }
        	  else
        	  {
        		  temp_addl_amt[Pul_addl_amt_len]='0';
        	  }
        	  strncat(temp_addl_amt,Auth_Tx.TLF01_details.mcard_banknet+7,12);
        	  Pul_addl_amt_len = Pul_addl_amt_len + 12;
        	  strncpy( HOST_MESSAGE.addl_data, temp_addl_amt, Pul_addl_amt_len );

          }
          else if(  0  == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncjcb",5)  &&
        		  '\0' != Auth_Tx.future_use_1[0]										   &&
				  (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE))
          {
        	  strncpy(temp_acct_type,&Auth_Tx.future_use_1[3],2);
        	  if(0 == strncmp(temp_acct_type,"01",2))
        	  {
        		  strncpy(currency_code,&Auth_Tx.future_use_1[5],3);
        		  sign_current = Auth_Tx.future_use_1[8];
        		  strncpy(current_amount,&Auth_Tx.future_use_1[9],12);
        	  }
        	  memset(temp_acct_type,0x00,sizeof(temp_acct_type));
        	  strncpy(temp_acct_type,&Auth_Tx.future_use_1[23],2);
        	  if(0 == strncmp(temp_acct_type,"02",2))
        	  {
        		  if('\0' == currency_code[0])
        		  {
        			  strncpy(currency_code,&Auth_Tx.future_use_1[5],3);
        		  }
        		  sign_available = Auth_Tx.future_use_1[28];
        		  strncpy(avail_cash_limit,&Auth_Tx.future_use_1[29],12);
        	  }
        	  else
        	  {
        		  sign_available = 'D';
        		  memset(avail_cash_limit,'0',12);
        	  }
        	  if('\0' != currency_code[0])
        	  {

                  sprintf( HOST_MESSAGE.addl_data,
                          "%7.7s%3.3s%c%013s%c%013s",
                          "0000000",  /* Zero fill first 7 positions. */
                           currency_code,
                           sign_current,
                           current_amount,
                           sign_available,
                           avail_cash_limit ); /* Available Cash Limit */
        	  }
        	  else
        	  {
        		  memset( HOST_MESSAGE.addl_data_len, 0x00, 3 );
        	  }

          }
          else if ( (Auth_Tx.ACF01_details.cash_available[0]   != 0x00) ||
              (Auth_Tx.ACF01_details.cash_limit[0]       != 0x00) ||
              (Auth_Tx.ACF01_details.cash_outstanding[0] != 0x00)  )
         {
            /* Yes, balance info exists. Populate the field. */
            if ( Auth_Tx.current_balance_sign == '-' )
               sign_current = 'D';
            else if ( Auth_Tx.current_balance_sign == '+' )
               sign_current = 'C';
            else
               sign_current = '0';

            if ( Auth_Tx.available_balance_sign == '-' )
               sign_available = 'D';
            else if ( Auth_Tx.available_balance_sign == '+' )
               sign_available = 'C';
            else
               sign_available = '0';

            /* The Current balance comes from 2 different places:
             * 1.  cash_outstanding = txcntl
             * 2.  cash_available   = network controllers
             */
            if ( Auth_Tx.ACF01_details.cash_outstanding[0] != 0x00 )
               strcpy( current_amount, Auth_Tx.ACF01_details.cash_outstanding );
            else
               strcpy( current_amount, Auth_Tx.ACF01_details.cash_available );

            sprintf( HOST_MESSAGE.addl_data,
                    "%7.7s%3.3s%c%013s%c%013s",
                    "0000000",  /* Zero fill first 7 positions. */
                     Auth_Tx.TLF01_details.currency_code,
                     sign_current,
                     current_amount,
                     sign_available,
                     Auth_Tx.ACF01_details.cash_limit ); /* Available Cash Limit */

            /* ----------------------------------------------------- *
             * Copy the balance information into these fields for    *
             * Equitable's proprietary reports. (Issuer perspective) *
             * ----------------------------------------------------- */

            /* Current Balance */
            sprintf( Auth_Tx.TLF01_details.outstanding_balance, "%012s",
                     current_amount );

            if ( sign_current == 'D' )
               Auth_Tx.TLF01_details.outstanding_balance[0] = 'D';

            /* Available Balance */
            sprintf( Auth_Tx.TLF01_details.credit_limit, "%012s",
                     Auth_Tx.ACF01_details.cash_limit );

            if ( sign_available == 'D' )
               Auth_Tx.TLF01_details.credit_limit[0] = 'D';
         }
         else
         {
            /* No, balance info does not exist.  Zero fill it. */
            memset( HOST_MESSAGE.addl_data, '0', 38 );
         }
      }
      else
      {
         /* NO, it is not approved.
          * Send response text.
          * Left justify if response code starts with 'N'.
          */
         if ( Auth_Tx.TLF01_details.response_code[0] == 'N' )
         {
            /* Remove all leading spaces and nulls. */
            p_text = Auth_Tx.TLF01_details.response_text;
            while( (*p_text == ' ') || (*p_text == 0x00) )
               p_text++;

            strncpy( HOST_MESSAGE.addl_data, p_text, 38 );
         }

         /* Add trailing space if necessary to ensure length of 38. */
         for( i=37; HOST_MESSAGE.addl_data[i]==0x00; i-- )
            HOST_MESSAGE.addl_data[i] = ' ';
      }
   }
   return( ret_val );
}


INT build_response_field_49( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if (*result_flag == 0)
   {
	  memcpy( HOST_MESSAGE.tran_curr_cd,
			  Auth_Tx.TLF01_details.currency_code,
			  DE49_SIZE );
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         build_response_field_55
 *
 *  DESCRIPTION:  This function will move the ICC data
 *                
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
INT build_response_field_55( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  temp_len=0;INT emv_len_in=0;
   CHAR tempstr[20];
   INT	 tag_len;
   BYTE	emv_out_len[3] = "";

   if ( *result_flag == 0 )
   {
		 if ( (Auth_Tx.EMV_details.future_field1[0] != 0x00) ||
              (Auth_Tx.EMV_details.future_field1[1] != 0x00)  )
         {
			/*2 byte header for DE55*/
			   HOST_MESSAGE.ICCdata[0]=0x01;
			   HOST_MESSAGE.ICCdata[1]=0x00;

			emv_len_in = genutil_bcd_to_int( Auth_Tx.EMV_details.future_field1, 2 );
			sprintf( emv_out_len, "%02X", emv_len_in );
			genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)tempstr,&tag_len);
			memcpy(&HOST_MESSAGE.ICCdata[2],tempstr,1);
			memcpy(&HOST_MESSAGE.ICCdata[3],Auth_Tx.EMV_details.future_field1+2,
					sizeof(Auth_Tx.EMV_details.future_field1)-3
					);
			if ( emv_len_in > (sizeof(Auth_Tx.EMV_details.future_field1)-3) )
			{
				/* Append the EMV data from future field 2. */
				memcpy( HOST_MESSAGE.ICCdata+3
					+ sizeof(Auth_Tx.EMV_details.future_field1)-3,
					 Auth_Tx.EMV_details.future_field2,
					 sizeof(Auth_Tx.EMV_details.future_field2)-1 );

				if ( emv_len_in > ((sizeof(Auth_Tx.EMV_details.future_field1)-3)
					   +(sizeof(Auth_Tx.EMV_details.future_field2)-1)) )
				{
				/* Append the EMV data from future field 2. */
				   memcpy( HOST_MESSAGE.ICCdata+3
					   + sizeof(Auth_Tx.EMV_details.future_field1)-3
					   + sizeof(Auth_Tx.EMV_details.future_field2)-1,
						Auth_Tx.EMV_details.future_field3,
						sizeof(Auth_Tx.EMV_details.future_field3)-1 );
				}
			}
			sprintf( emv_out_len, "%02X", emv_len_in+3 );
			genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)tempstr,&tag_len);
			memcpy(HOST_MESSAGE.ICCdata_len,tempstr,1);
		}
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2) == 0)
	{
		nceqit_log_message( 2, 1, "Response doesn't have DE55, hence not Sending DE55 in Response to IST", "build_response_field_55" ,1);
	}
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_response_field_62
 *
 *  DESCRIPTION:  This function will move the Private field 62
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_62( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val = true;
   INT    bitMapsize = 8;
   BYTE   field62_bitmap_hex[9]={0};
   BYTE   field62_bitmap_ascii[17]={0};
   BYTE   field62_bitmap_ebcdic[17]={0};
   INT    F62Len = 0;
   CHAR   strLen[4];


   memset(HOST_MESSAGE.priv62,0x20,sizeof(HOST_MESSAGE.priv62));

   if ( *result_flag == 0 )
   {

	   memset( field62_bitmap_hex, 0x00, sizeof(field62_bitmap_hex));
	   memset( field62_bitmap_ebcdic, 0x00, sizeof(field62_bitmap_ebcdic));
   	   memset( field62_bitmap_ascii, 0x00, sizeof(field62_bitmap_ascii));
	
	   
	   field62_bitmap_hex[0] |= 0xC0;      /* Set bit 1 , 2*/
	   field62_bitmap_hex[1] |= 0x00;	  

	   hex_to_ebcdic(field62_bitmap_hex,field62_bitmap_ebcdic,bitMapsize);
	   convert_to_ascii( field62_bitmap_ebcdic, field62_bitmap_ascii, (2*bitMapsize));
       memcpy(HOST_MESSAGE.priv62,field62_bitmap_ascii,(2*bitMapsize));
       
       F62Len += (2*bitMapsize);
       
       /* 62.1 Authorization character */
	   if(Auth_Tx.TLF01_details.visa_char_indicator != 0x00)
		   HOST_MESSAGE.priv62[F62Len] = Auth_Tx.TLF01_details.visa_char_indicator;

	   F62Len ++;
       
	   /* 62.2 Transaction identifier */
	   if(Auth_Tx.TLF01_details.visa_tran_id[0] != 0x00)
			memcpy( HOST_MESSAGE.priv62 + F62Len, Auth_Tx.TLF01_details.visa_tran_id,15 );

	   F62Len += 15;


	   
	   /* 62.23 Product ID */
	  /* if ( Auth_Tx.TLF01_details.product_codes[7].code[0] != 0x00)
	   {
		   memcpy( HOST_MESSAGE.priv62 + F62Len, Auth_Tx.TLF01_details.product_codes[7].code, 2);
		   F62Len += 2;
		   field62_bitmap_hex[2] |= 0x02;
	   }*/

	   
	   sprintf( strLen, "%03d", F62Len );

	   memcpy( HOST_MESSAGE.priv62_len, strLen, 3);

   }


   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         build_response_field_63
 *
 *  DESCRIPTION:  This function will move the Private field 63
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_63( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val = true;
   INT    bitMapsize = 3;
   BYTE   field63_bitmap_hex[4]={0};
   BYTE   field63_bitmap_ascii[7]={0};
   BYTE   field63_bitmap_ebcdic[7]={0};
   INT    F63Len = 0;
   CHAR   strLen[3];



   if ( *result_flag == 0 )
   {
      memset( field63_bitmap_hex,    0x00, sizeof(field63_bitmap_hex)   );
      memset( field63_bitmap_ebcdic, 0x00, sizeof(field63_bitmap_ebcdic));
      memset( field63_bitmap_ascii,  0x00, sizeof(field63_bitmap_ascii) );

      switch( Auth_Tx.TLF01_details.tx_key )
      {
         case AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE:
         case AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE:
		 case AUTH_SALE_RESPONSE:
		 case AUTH_REVERSAL_RESPONSE:
		 case AUTH_CASH_ADVANCE_RESPONSE:
		 case AUTH_BALANCE_INQUIRY_RESPONSE:

             /* Set only bit 1 */
             field63_bitmap_hex[0] |= 0x80;
             break;

         default:

             /* Set bits 1, 5, and 13 */
             field63_bitmap_hex[0] |= 0x88;
             field63_bitmap_hex[1] |= 0x08;
             break;
      }

      hex_to_ebcdic(field63_bitmap_hex,field63_bitmap_ebcdic,bitMapsize);
      convert_to_ascii( field63_bitmap_ebcdic, field63_bitmap_ascii, (2*bitMapsize));
      memcpy(HOST_MESSAGE.priv63,field63_bitmap_ascii,(2*bitMapsize));

      F63Len += (2*bitMapsize);

      /* 63.1 Network identification code */
      if(Auth_Tx.MCF01_details.vip_code[0] != 0x00)
	  {
         memcpy( HOST_MESSAGE.priv63 + F63Len, Auth_Tx.MCF01_details.vip_code,4 );
		 F63Len += 4;
	  }

      if ( (Auth_Tx.TLF01_details.tx_key != AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE) &&
           (Auth_Tx.TLF01_details.tx_key != AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE) && 
		   (Auth_Tx.TLF01_details.tx_key != AUTH_SALE_RESPONSE) &&
		   (Auth_Tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE) &&
		   (Auth_Tx.TLF01_details.tx_key != AUTH_CASH_ADVANCE_RESPONSE) &&
		   (Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY_RESPONSE))
      {
         /* 63.5 PLUS Proprietary Member Center ID */
         if(Auth_Tx.MCF01_details.process_control_id[0] != 0x00)
		 {
            memcpy( HOST_MESSAGE.priv63 + F63Len,
                    Auth_Tx.MCF01_details.process_control_id, 6 );
		    F63Len += 6;
		 }
      }
      sprintf( strLen, "%02d", F63Len );
      memcpy( HOST_MESSAGE.priv63_len, strLen, 2);
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_admin_response_field_63
 *
 *  DESCRIPTION:  This function will move the Private field 63 for adminstrative
 *                text message to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *
 *                If transaction is approved, and not an inquiry, zero fill.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_admin_response_field_63( pINT result_flag, pCHAR err_buf )
{
   INT     ret_val = true;
   INT    bitMapsize = 3;
   BYTE   field63_bitmap_hex[4]={0};
   BYTE   field63_bitmap_ascii[7]={0};
   BYTE   field63_bitmap_ebcdic[7]={0};
   INT    F63Len = 0;
   CHAR   strLen[4];


   memset(HOST_MESSAGE.priv63,0x20,sizeof(HOST_MESSAGE.priv63));

   if ( *result_flag == 0 )
   {

	   memset( field63_bitmap_hex, 0x00, sizeof(field63_bitmap_hex));
	   memset( field63_bitmap_ebcdic, 0x00, sizeof(field63_bitmap_ebcdic));
   	   memset( field63_bitmap_ascii, 0x00, sizeof(field63_bitmap_ascii));
	
	   
	   field63_bitmap_hex[0] |= 0x88;      /* Set bit 1 , 5*/
       

	   hex_to_ebcdic(field63_bitmap_hex,field63_bitmap_ebcdic,bitMapsize);
	   convert_to_ascii( field63_bitmap_ebcdic, field63_bitmap_ascii, (2*bitMapsize));
       memcpy(HOST_MESSAGE.priv63,field63_bitmap_ascii,(2*bitMapsize));
       
       F63Len += (2*bitMapsize);
       
       /* 63.1 Network identification code */
	   if(Auth_Tx.MCF01_details.vip_code[0] != 0x00)
		memcpy( HOST_MESSAGE.priv63 + F63Len, Auth_Tx.MCF01_details.vip_code,4 );
       
       F63Len += 4;
       
	   /* 63.5 PLUS Proprietary Member Center ID */
	   if(Auth_Tx.MCF01_details.process_control_id[0] != 0x00)
			memcpy( HOST_MESSAGE.priv63 + F63Len, Auth_Tx.MCF01_details.process_control_id,6 );

	   F63Len += 6;
	   
	   
	   sprintf( strLen, "%03d", F63Len );

	   memcpy( HOST_MESSAGE.priv63_len, strLen, 3);

   }


   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         build_response_field_70

 *
 *  DESCRIPTION:  This function will move the network management information code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_70( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.nmi_code,
              NMI_Code,
              DE70_SIZE);
      
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_admin_response_field_70

 *
 *  DESCRIPTION:  This function will move the network management information code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_admin_response_field_70( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.nmi_code,
              Auth_Tx.TLF01_details.service_code,
              DE70_SIZE);
      
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_90
 *
 *  DESCRIPTION:  This function will move the original data elements
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_90( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.orig_data,
              Auth_Tx.TLF01_details.orig_message,
              4);

      memcpy( HOST_MESSAGE.orig_data+4,
              Auth_Tx.TLF01_details.orig_sys_trace_num,
              6);

      /* This is the original transmission timestamp from DE7.
       * Reference function 'move_field_90' to
       * see where this value comes from.
       */
      memcpy( HOST_MESSAGE.orig_data+10,
              Auth_Tx.TLF01_details.product_codes[18].amount, 10);

      memcpy( HOST_MESSAGE.orig_data+20, Auth_Tx.orig_acq_inst_id, 2);

      memcpy( HOST_MESSAGE.orig_data+22, Auth_Tx.TLF01_details.currency_code,3);

      memset( HOST_MESSAGE.orig_data+25, '0', 17 );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_95
 *
 *  DESCRIPTION:  This function will move the replacement amount
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is numeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *                This is the dispensed amount.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_95( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      memset(HOST_MESSAGE.replace_amt, '0', 1 );
      memcpy(HOST_MESSAGE.replace_amt+1,Auth_Tx.TLF01_details.actual_amount, 12);
      memset(HOST_MESSAGE.replace_amt+13, '0', DE95_SIZE-13 );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_100
 *
 *  DESCRIPTION:  This function will move the Receiving institution ID
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length. This is an echo from the request.  Therefore, there
 *                is no reason to validate the data here.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_100( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   INT  len;

   if ( *result_flag == 0 )
   {
      len = strlen( Auth_Tx.receiving_inst_id_code );
      if ( len > 0 )
      {
         sprintf( HOST_MESSAGE.rcvid_len, "%02d", len );
         strcpy( HOST_MESSAGE.rcvid, Auth_Tx.receiving_inst_id_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_102
 *
 *  DESCRIPTION:  This function will move the account-1 identification info
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *                This is an echo from the request.  Therefore, there is
 *                no reason to validate the data here.
 *
 *                This field is always present.  If transaction is not an
 *                ATM (DE25 = 02), then space fill.  Else populate as follows:
 *                   1  Cash Advance Type
 *                   1  Auth Indicator for Visa CPS, else space fill
 *                   15 Transaction ID for Visa CPS, else space fill
 *                   4  Validation Code for Visa CPS, else space fill
 *                   7  Filler of spaces
 *
 *                Cash Advance Type indicates where a transactions was
 *                switched, e.g. Visa Cirrus, JCB as follows:
 *                   A - ECN Visa
 *                   B - Off us Visa
 *                   C - Off us MasterCard
 *                   D - ECN MasterCard
 *                   E - Cirrus
 *                   F - Off us PLUS CASH ADVANCE
 *                   G - Off us PLUS SALE (Plus does not do sale, only ATM)
 *                   H - ECN JCB
 *                   I - Off us JCB
 *                   J - ECN Amex
 *                   K - Off us Amex
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_102( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;
   BYTE tx_key;

   if ( *result_flag == 0 )
   {
      memcpy( HOST_MESSAGE.acct1_len, "28", 2 );
      if ( 0 == strcmp(Auth_Tx.TLF01_details.pos_condition_code, sATM_POSCC) )
      {
         /*+--------------------------------------+
           | This is an ATM.  Populate the field. |
           +--------------------------------------+*/

         /* First determine if ON-US or OFF-US. */
         if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.auth_2,"LA") )
         {
            /*+------------------------------------+
              | This is ON-US; locally authorized. |
              +------------------------------------+*/

            /* There is no Visa CPS info for ON-US; space fill those fields.*/
            memset( HOST_MESSAGE.acct1, ' ', DE102_SIZE );

            /* Determine card type based on card family. */
            if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA) )
               HOST_MESSAGE.acct1[0] = ECN_VISA;
            else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX) )
               HOST_MESSAGE.acct1[0] = ECN_AMEX;
            else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) )
               HOST_MESSAGE.acct1[0] = ECN_MC;
            else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) )
               HOST_MESSAGE.acct1[0] = ECN_JCB;
            else if (0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_CUP))
            	HOST_MESSAGE.acct1[0] = ECN_CUP;
            else
            {
               ret_val = false;
               *result_flag = 1;
               sprintf( err_buf,
                       "Unable to build response field 102. Unknown ON-US card family %s.",
                        Auth_Tx.BIN01_details.card_family );
            }
         }
         else
         {
            /*+--------------------------------------+
              | This is OFF-US; remotely authorized. |
              +--------------------------------------+*/

            /* Determine card type based on card family. */
            if ( 0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncv",3))
            {
               /* Use processing code to determine Cash Advance Type. */
               switch ( Auth_Tx.TLF01_details.processing_code[2] )
               {
                  case PROC_CODE_SAVINGS:

                     HOST_MESSAGE.acct1[0] = OFF_US_PLUS_SA;
                     break;

                  case PROC_CODE_CURRENT:

                     HOST_MESSAGE.acct1[0] = OFF_US_PLUS_CA;
                     break;

                  case PROC_CODE_CREDIT:

                     HOST_MESSAGE.acct1[0] = OFF_US_VISA;
                     break;

                  default:

                     /* For unspecified processing code, base
                      * the value on transaction type.
                      */
                     tx_key = Auth_Tx.TLF01_details.tx_key;
                     switch ( tx_key )
                     {
                        case AUTH_SALE: 
                        case AUTH_AUTHORIZATION: 
                                      HOST_MESSAGE.acct1[0] = OFF_US_VISA;
                                      break;
                        case AUTH_CASH_ADVANCE: 
                        case AUTH_BALANCE_INQUIRY:
                        default:
                                      HOST_MESSAGE.acct1[0] = OFF_US_PLUS_CA;
                                      break;
                     }
                     break;
               }

               if ( Auth_Tx.TLF01_details.visa_char_indicator != 0x00 )
                  HOST_MESSAGE.acct1[1] = Auth_Tx.TLF01_details.visa_char_indicator;
               else
                  HOST_MESSAGE.acct1[1] = ' ';

               if ( Auth_Tx.TLF01_details.visa_tran_id[0] != 0x00 )
                  memcpy( &HOST_MESSAGE.acct1[2],
                          Auth_Tx.TLF01_details.visa_tran_id,
                          15 );
               else
                  memset( &HOST_MESSAGE.acct1[2], ' ', 15 );

               if ( Auth_Tx.TLF01_details.visa_validation_code[0] != 0x00 )
                  memcpy( &HOST_MESSAGE.acct1[17],
                          Auth_Tx.TLF01_details.visa_validation_code,
                          4 );
               else
                  memset( &HOST_MESSAGE.acct1[17], ' ', 4 );

               memset( &HOST_MESSAGE.acct1[21], ' ', 7 );  /* Filler */
            }
            else
            {
               memset( HOST_MESSAGE.acct1, ' ', DE102_SIZE );
               if ( NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"jcb"))
                  HOST_MESSAGE.acct1[0] = OFF_US_JCB;
               else if ( NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"cir"))
                  HOST_MESSAGE.acct1[0] = CIRRUS;
               else if ( NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"mcr"))
                  HOST_MESSAGE.acct1[0] = OFF_US_MC;
               else if ( NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"dci"))
                  HOST_MESSAGE.acct1[0] = ON_US_DCI;
               else if (NULL_PTR != strstr(Auth_Tx.TLF01_details.authorizing_host_queue,"cup"))
            	   HOST_MESSAGE.acct1[0] = OFF_US_CUP;


               /* If we get here, it means the transaction did not go
                * to the host.  Network status is probably offline.
                */
               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) )
                  HOST_MESSAGE.acct1[0] = OFF_US_JCB;

               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) )
                  HOST_MESSAGE.acct1[0] = CIRRUS;

               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX) )
                  HOST_MESSAGE.acct1[0] = OFF_US_AMEX;

               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_DINERS) )
                   HOST_MESSAGE.acct1[0] = OFF_US_PUL;

               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_CUP))
            	   HOST_MESSAGE.acct1[0] = OFF_US_CUP;

               else if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA) )
               {
                  /* Use processing code to determine Cash Advance Type. */
                  switch ( Auth_Tx.TLF01_details.processing_code[2] )
                  {
                     case PROC_CODE_SAVINGS:

                        HOST_MESSAGE.acct1[0] = OFF_US_PLUS_SA;
                        break;

                     case PROC_CODE_CURRENT:

                        HOST_MESSAGE.acct1[0] = OFF_US_PLUS_CA;
                        break;

                     case PROC_CODE_CREDIT:

                        HOST_MESSAGE.acct1[0] = OFF_US_VISA;
                        break;

                     default:

                        /* For unspecified processing code, base
                         * the value on transaction type.
                         */
                        tx_key = Auth_Tx.TLF01_details.tx_key;
                        switch ( tx_key )
                        {
                           case AUTH_SALE: 
                           case AUTH_AUTHORIZATION: 
                                      HOST_MESSAGE.acct1[0] = OFF_US_VISA;
                                      break;
                           case AUTH_CASH_ADVANCE: 
                           case AUTH_BALANCE_INQUIRY:
                           default:
                                      HOST_MESSAGE.acct1[0] = OFF_US_PLUS_CA;
                                      break;
                        }
                        break;
                  }
               }
               else
               {
                  ret_val = false;
                  *result_flag = 1;
                  sprintf( err_buf,
                       "Unable to build response field 102. Unknown OFF-US card authorizer %s.",
                        Auth_Tx.TLF01_details.authorizing_host_queue );
               }
            }
         }
      }
      else
      {
         /* This is not an ATM.  Space fill the field. */
         memset( HOST_MESSAGE.acct1, ' ', DE102_SIZE );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_103
 *
 *  DESCRIPTION:  This function will move the account-2 identification info
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alpha numeric, variable
 *                length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_103( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      /* This field is always space filled. */
      sprintf( HOST_MESSAGE.acct2_len, "%02d", DE103_SIZE );
      memset( HOST_MESSAGE.acct2, ' ', DE103_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_response_field_128
 *
 *  DESCRIPTION:  This function will move the message authentication code
 *                to the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_response_field_128( pINT result_flag, pCHAR err_buf )
{
   INT  ret_val = true;

   if ( *result_flag == 0 )
   {
      /* This field is not supported.  Space fill it. */
      memset( HOST_MESSAGE.msg_authc, ' ', DE128_SIZE );
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_2
 *
 *  DESCRIPTION:  This function will move the card number from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Card number is numeric, variable
 *                length up to 19 digits.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_2( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;

   if ( *result_flag == 0 )
   {
      /* First get the length of the card number. */
      memcpy( Auth_Tx.TLF01_details.card_num_len,
              HOST_MESSAGE.pan_len, DE2_LEN_SIZE );
      len = atoi(Auth_Tx.TLF01_details.card_num_len);

      if ( len <= MAX_DE2_SIZE )
      {
         memcpy( Auth_Tx.TLF01_details.card_num, HOST_MESSAGE.pan, len );
         if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.card_num)))
         {
            *result_flag = 1;
            sprintf( err_buf, "Non-numeric pan (field 2): %s,",
                     Auth_Tx.TLF01_details.card_num );
         }
      }
      else
      {
         sprintf( err_buf, "Invalid length of card number: %d,", len );
         ret_val = false;
         *result_flag = 1;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_3
 *
 *  DESCRIPTION:  This function will move the processing code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Processing code is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_3( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
        memcpy( Auth_Tx.TLF01_details.processing_code,
              HOST_MESSAGE.proccode, DE3_SIZE );
		if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
			strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
		{
			// do nothing as you copied it in while sendig the request
		}
		else
		{
		      /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
		      memcpy( Auth_Tx.TLF01_details.product_codes[17].amount,
		              HOST_MESSAGE.proccode, DE3_SIZE );
		}

      if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.processing_code)))
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric processing code: %s,",
                  Auth_Tx.TLF01_details.processing_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_4
 *
 *  DESCRIPTION:  This function will move the amount from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_4( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.total_amount,
              HOST_MESSAGE.amount, DE4_SIZE );
      if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.total_amount)))
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric amount (field 4): %s,",
                  Auth_Tx.TLF01_details.total_amount );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_7
 *
 *  DESCRIPTION:  This function will move the transmission time from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Transmission time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_7( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Only do this for a request from the host.
       * Ignore for responses, as it may not be there.
       */
      if ( TranFlow == SWITCHED_IN )
      {
         memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
                 HOST_MESSAGE.gmt_time, DE7_SIZE );
         ret_val = isnum(Auth_Tx.TLF01_details.transmission_timestamp);
         if ( ret_val == false )
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-numeric Transmission Timestamp (field 7): %s,",
                     Auth_Tx.TLF01_details.transmission_timestamp );
         }
         else if ( false == validate_date_time(Auth_Tx.TLF01_details.transmission_timestamp))
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                    "Invalid Transmission Timestamp (field 7): %s,",
                     Auth_Tx.TLF01_details.transmission_timestamp );
         }
         else
         {
            /* This date-time is expressed in local (Manila) time.
             * Save it for the response, then convert it to GMT
             * for Visa, JCB, and Cirrus.
             */
            memcpy( Auth_Tx.TLF01_details.product_codes[19].amount,
                    Auth_Tx.TLF01_details.transmission_timestamp,
                    DE7_SIZE );
//            pintime_get_gmt( gmt_timedate );                /* YYYYMMDDhhmmss */
			

//            memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
//                    gmt_timedate+4, 10 );                   /*     MMDDhhmmss */

			/* Girija Y TF, Jan 07 2009, EQIT Interface */

			ret_val = gmt_get_time(Auth_Tx.TLF01_details.transmission_timestamp);
			
         }
      }

	  /* For ChargeBack response need to pass Timestamp, Girija Y TF, Jan 21, 2009 */
	  else if ( TranFlow == SWITCHED_OUT ) 
	  {
		  if( 0 == strncmp(Auth_Tx.TLF01_details.message_type, "0432",4))
		  {
				memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
						HOST_MESSAGE.gmt_time, DE7_SIZE );
				ret_val = isnum(Auth_Tx.TLF01_details.transmission_timestamp);
				if ( ret_val == false )
				{
					*result_flag = 1;
					sprintf( err_buf,
							"Non-numeric Transmission Timestamp (field 7): %s,",
							Auth_Tx.TLF01_details.transmission_timestamp );
				}
				else if ( false == validate_date_time(Auth_Tx.TLF01_details.transmission_timestamp))
				{
					ret_val = false;
					*result_flag = 1;
					sprintf( err_buf,
							"Invalid Transmission Timestamp (field 7): %s,",
							Auth_Tx.TLF01_details.transmission_timestamp );
				}
		  }
	  }

		  
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_11
 *
 *  DESCRIPTION:  This function will move the system trace number from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_11( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.sys_trace_audit_num,
              HOST_MESSAGE.stan, DE11_SIZE );
      ret_val = isnum(Auth_Tx.TLF01_details.sys_trace_audit_num);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric System Trace Audit Number (field 11): %s,",
                  Auth_Tx.TLF01_details.sys_trace_audit_num );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_12
 *
 *  DESCRIPTION:  This function will move the transaction time from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Transaction time is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_12( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.time_hhmmss,
              HOST_MESSAGE.time, DE12_SIZE );
      ret_val = isnum(Auth_Tx.TLF01_details.time_hhmmss);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric transaction time (field 12): %s,",
                  Auth_Tx.TLF01_details.time_hhmmss );
      }
      else if ( false == validate_time(Auth_Tx.TLF01_details.time_hhmmss))
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Invalid transaction time (field 12): %s,",
                  Auth_Tx.TLF01_details.time_hhmmss );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_13
 *
 *  DESCRIPTION:  This function will move the transaction date from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Transaction date is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_13( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  date_mmdd[5]="";
   CHAR  time_date[25];


   if ( *result_flag == 0 )
   {
      memcpy( date_mmdd, HOST_MESSAGE.date, DE13_SIZE );
      ret_val = isnum( date_mmdd );
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric transaction date (field 13): %s,",
                  date_mmdd );
      }
      else if ( false == validate_date_mmdd(date_mmdd) )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Invalid transaction date (field 13): %s,",
                  date_mmdd );
      }
      else
      {
         /* Date is valid.  Prepend the year. */
         ptetime_get_timestamp( time_date );
         get_date( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
         memcpy( Auth_Tx.TLF01_details.date_yyyymmdd+4, date_mmdd, DE13_SIZE );

         /* Make sure there is no end-of-year rollover problem,
          * Jan 1st and Dec 31st only.
          */
         if ( (0 == strcmp(date_mmdd, "0101")) ||
              (0 == strcmp(date_mmdd, "1231"))  )
         {
            check_end_of_year_rollover( date_mmdd,
                                        Auth_Tx.TLF01_details.date_yyyymmdd );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_15
 *
 *  DESCRIPTION:  This function will move the settlement date from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Settlement date is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_15( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  date_str[9];
   CHAR  time_date[25];

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.settlement_date,
              HOST_MESSAGE.date_settle, DE15_SIZE );

      /* Is settlement date null or all zeroes? */
      if ( (Auth_Tx.TLF01_details.settlement_date[0] == 0x00) ||
           (0 == strcmp(Auth_Tx.TLF01_details.settlement_date,"0000")) )
      {
         /* It does not exist; use txn date. */
         if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
         {
            /* Txn date does not exist; use today's date. */
            memset( time_date, 0x00, sizeof(time_date) );
            memset( date_str,  0x00, sizeof(date_str ) );
            ptetime_get_timestamp( time_date );
            get_date( time_date, date_str );
            memcpy( Auth_Tx.TLF01_details.settlement_date, date_str+4, DE15_SIZE );
         }
         else
         {
            memcpy( Auth_Tx.TLF01_details.settlement_date,
                    Auth_Tx.TLF01_details.date_yyyymmdd+4,
                    DE15_SIZE );
         }
      }
      else
      {
         ret_val = isnum(Auth_Tx.TLF01_details.settlement_date);
         if ( ret_val == false )
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-numeric settlement date (field 15): %s,",
                     Auth_Tx.TLF01_details.settlement_date );
         }
         else if ( false == validate_date_mmdd(Auth_Tx.TLF01_details.settlement_date))
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                    "Invalid settlement date (field 15): %s,",
                     Auth_Tx.TLF01_details.settlement_date);
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_18
 *
 *  DESCRIPTION:  This function will move the merchant type from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_18( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	   /* We are using MCF01_details.number_man_entries as placeholder for merch_type*/
      memcpy( Auth_Tx.MCF01_details.number_man_entries,
              HOST_MESSAGE.merch_type, DE18_SIZE );
      ret_val = isnum(Auth_Tx.MCF01_details.number_man_entries);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric Merchant type (field 18): %s,",
                  Auth_Tx.MCF01_details.number_man_entries );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_19
 *
 *  DESCRIPTION:  This function will move the Acquirer institution country code 
 *				  from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_19( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	   /* change from acq_inst_id_cd to corresponding acquiring  institution country code */
      memcpy( Auth_Tx.acceptor_country,
              HOST_MESSAGE.acq_inst_cnt_code, DE19_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_21
 *
 *  DESCRIPTION:  This function will move the forwarding institution country code 
 *				  from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_21( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
	   /* change from acq_inst_id_cd to corresponding forwarding  institution country code */
      memcpy( Auth_Tx.country_code,
              HOST_MESSAGE.fwd_inst_cnt_code, DE21_SIZE );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_22
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  POS Entry Mode is numeric, fixed
 *                length.  For Equitable host, it will always be the
 *                same value.
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_22( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  pos_entry_mode[5]="";

   if ( *result_flag == 0 )
   {
      memcpy( pos_entry_mode, HOST_MESSAGE.posentry, DE22_SIZE );
      ret_val = isnum(pos_entry_mode);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric POS Entry Mode (field 22): %s,",
                  pos_entry_mode );
      }
      else
      {
         /* Prepend a leading zero. */
         Auth_Tx.TLF01_details.pos_entry_mode[0] = '0';
         memcpy( Auth_Tx.TLF01_details.pos_entry_mode+1, pos_entry_mode, 3 );
      }
   }
	if( strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 ||
		strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",3) == 0	)
	{
		// do nothing as you copied it in while sendig the request
	}
	else
	{
		memcpy( Auth_Tx.TLF01_details.product_codes[9].code,pos_entry_mode,3);
	}
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_23
 *
 *  DESCRIPTION:  This function will move the POS Entry Mode from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  POS Entry Mode is numeric, fixed
 *                length.  For Equitable host, it will always be the
 *                same value.
 *
 *                Prepend '0' to put it into Hypercom ISO format.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/

INT move_field_23( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   BYTE  pos_entry_mode[5]="";

   if ( *result_flag == 0 )
   {
	   if(strlen(HOST_MESSAGE.cardseqnum) > 0)	
	   {
	   	Auth_Tx.host2_message_seq_nbr[0]=0x30;
	   	strncpy( &Auth_Tx.host2_message_seq_nbr[1],HOST_MESSAGE.cardseqnum, 3 );
	   	}
      
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_25
 *
 *  DESCRIPTION:  This function will move the POS Condition Code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  POS Condition Code is numeric, fixed
 *                length.  For Equitable host, it will always one of two
 *                values.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_25( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   itemp;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.pos_condition_code,
              HOST_MESSAGE.pos_condcd, DE25_SIZE );
      ret_val = isnum(Auth_Tx.TLF01_details.pos_condition_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric POS Condition Code (field 25): %s,",
                  Auth_Tx.TLF01_details.pos_condition_code );
      }
      else
      {
         itemp = atoi( Auth_Tx.TLF01_details.pos_condition_code );
         if ( (itemp != ATM_POSCC) && (itemp != POS_POSCC) && (itemp != NORMAL_POSCC) &&
			  (itemp != ADJ_POSCC1) && (itemp != ADJ_POSCC2) && (itemp !=  ADJ_POSCC3)
			)

         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                    "Invalid POS Condition Code: %s, expected %02d or %02d or %02d or %02d or %02d or %02d",
                     Auth_Tx.TLF01_details.pos_condition_code,
                     ATM_POSCC, POS_POSCC,NORMAL_POSCC,ADJ_POSCC1,ADJ_POSCC2,ADJ_POSCC3 );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_28
 *
 *  DESCRIPTION:  This function will move the transaction fee amount from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Amount is numeric, fixed length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_28( pINT result_flag, pCHAR err_buf )
{
 /*  INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.tran_fee_amount,   HOST_MESSAGE.amount, DE28_SIZE );
      if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.tran_fee_amount)))
      {
        result_flag = 1;
         sprintf( err_buf, "Non-numeric amount (field 28): %s,",
                Auth_Tx.TLF01_details.tran_fee_amount ); 
	   }
   }
   return( ret_val ); */

/* if value present in the field 28 copy it otherwise set with all zeros*/
   INT   ret_val = true;
   if ( *result_flag == 0 )
   {
	   if(HOST_MESSAGE.tran_fee_amt[0] != 0x00 )
	   {
		  memcpy( Auth_Tx.TLF01_details.tran_fee_amount,&HOST_MESSAGE.tran_fee_amt[1], DE28_SIZE );
			if (false ==(ret_val = isnum(Auth_Tx.TLF01_details.tran_fee_amount)))
			{
				*result_flag = 1;
				sprintf( err_buf, "Non-numeric amount (field 28): %s,",
					 Auth_Tx.TLF01_details.tran_fee_amount ); 
			}
	   }
	   else
	   {
		   memset( Auth_Tx.TLF01_details.tran_fee_amount, '0', DE28_SIZE );
	   }
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         move_field_32
 *
 *  DESCRIPTION:  This function will move the Acquiring Inst ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Acquiring Inst ID is alphanumeric, variable
 *                length up to 2 digits. (From the requirements, it will
 *                always be 2 alphanumeric characters).
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_32( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  len_str[3]="";

   if ( *result_flag == 0 )
   {
      /* First get the length. */
      memcpy( len_str, HOST_MESSAGE.acq_inst_len, 2 );
      len = atoi( len_str );
      if ( len > 0 )
      {
         /* Now copy the Id into Auth_Tx and check it. */
         memcpy( Auth_Tx.acq_inst_id_cd, HOST_MESSAGE.acq_inst_id, len );
         if (false ==(ret_val = isalphanum(Auth_Tx.acq_inst_id_cd)))
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-alphanumeric acquiring inst. Id (field 32): %s,",
                     Auth_Tx.acq_inst_id_cd );
         }

         /* Copy it into TLF01 for database reference. */
         memcpy( Auth_Tx.TLF01_details.acquiring_id,
                 HOST_MESSAGE.acq_inst_id,
                 len );
      }
      else
      {
         *result_flag = 1;
         sprintf( err_buf, "Invalid length of acquiring inst. Id: %c%c,",
                  HOST_MESSAGE.acq_inst_len[0],
                  HOST_MESSAGE.acq_inst_len[1] );
         ret_val = false;
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_field_33
 *
 *  DESCRIPTION:  This function will move the Forwarding Inst ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Acquiring Inst ID is alphanumeric, variable
 *                length up to 2 digits. (From the requirements, it will
 *                always be 2 alphanumeric characters).
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_33( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  len_str[3]="";

   if ( *result_flag == 0 )
   {
      /* First get the length. */
      memcpy( len_str, HOST_MESSAGE.fwd_inst_len, 2 );
      len = atoi( len_str );
      if ( len > 0 )
      {
         /* Now copy the Id into Auth_Tx and check it. */
         memcpy( Auth_Tx.forwarding_inst_id_cd, HOST_MESSAGE.fwd_inst_id, len );
         if (false ==(ret_val = isalphanum(Auth_Tx.forwarding_inst_id_cd)))
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Non-alphanumeric acquiring inst. Id (field 33): %s,",
                     Auth_Tx.forwarding_inst_id_cd );
         }

      }
      else
      {
         *result_flag = 1;
         sprintf( err_buf, "Invalid length of forwarding inst. Id: %c%c,",
                  HOST_MESSAGE.fwd_inst_len[0],
                  HOST_MESSAGE.fwd_inst_id[1] );
         ret_val = false;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_35
 *
 *  DESCRIPTION:  This function will move the Track 2 Data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Track 2 Data is alphanumeric, variable
 *                length up to 37 digits.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_35( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   CHAR  track2_len[4]="";

   if ( *result_flag == 0 )
   {
      /* Get the length and check it. */
      memcpy( track2_len, HOST_MESSAGE.track2_len, DE35_LEN_SIZE );
      len = atoi(track2_len);
      if ( false==(ret_val=isnum(track2_len)))
      {
         *result_flag = 1;
         sprintf( err_buf, "Track 2 length is not numeric: %s,", track2_len );
      }
      else if ( len > MAX_DE35_SIZE )
      {
         ret_val = false;
         *result_flag = 1;
         sprintf( err_buf,
                 "Track 2 is longer than max allowed, len=%d,",
                  len );
     }
     else
     {
         /* The length checks out ok.  Get the track 2 data. */
         memcpy( Auth_Tx.TLF01_details.track2,
                 HOST_MESSAGE.track2, len );
		//if(1 == len % 2 )
		//{
		//strcat(Auth_Tx.TLF01_details.track2,"0");
		//}
         /* Make sure the end sentinel is removed. Most ATM's
          * do not send it, but some do.  We need to remove it.
          */
         (void)remove_track2_end_sentinel( Auth_Tx.TLF01_details.track2 );

         /* Parse track 2 to get card number and expiration date.
          * This will overwrite the pan in field 2.
          * Customer requirement: If track 2 exists, do not use
          * card number from field 2; get it from track 2.
          */
         if (false==(ret_val=parse_track2() ))
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Track 2 has invalid format: %s, no field separator,",
                     Auth_Tx.TLF01_details.track2 );
         }
         else
         {
            /* Validate the card number from track 2. */
            len = atoi( Auth_Tx.TLF01_details.card_num_len );
            if ( len > MAX_DE2_SIZE )
            {
               *result_flag = 1;
               sprintf( err_buf,
                       "Card number from track 2 is too long: %s. Max size is %d,",
                        Auth_Tx.TLF01_details.card_num, MAX_DE2_SIZE );
               ret_val = false;
            }
            else if (false==(ret_val=isnum(Auth_Tx.TLF01_details.card_num)))
            {
               *result_flag = 1;
               sprintf( err_buf,
                       "Card number from track 2 is not numeric: %s,",
                        Auth_Tx.TLF01_details.card_num );
            }
            else
            {
               /* Card number is ok.  Validate expiration date from track 2. */
               ret_val = validate_date_yymm( Auth_Tx.TLF01_details.exp_date,
                                             err_buf );

               /* Trim trailing spaces */
               if ( true == ret_val )
                  trim_trailing_spaces( Auth_Tx.TLF01_details.track2 );
            }
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_37
 *
 *  DESCRIPTION:  This function will move the retrieval reference number from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  System trace number is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_37( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.retrieval_ref_num,
              HOST_MESSAGE.rrn, DE37_SIZE );
      ret_val = isnum(Auth_Tx.TLF01_details.retrieval_ref_num);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric Retrieval Refence Number (field 37): %s,",
                  Auth_Tx.TLF01_details.retrieval_ref_num );
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_38
 *
 *  DESCRIPTION:  This function will move the Authorization Number from Host
 *                Message structure according to Equitable formatting
 *                specifications.  Auth Number is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_38( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.auth_number,
              HOST_MESSAGE.authcode, DE38_SIZE );
      ret_val = isalphanum_space(Auth_Tx.TLF01_details.auth_number);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in Auth Number (field 38) not allowed: %s,",
                  Auth_Tx.TLF01_details.auth_number );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_39
 *
 *  DESCRIPTION:  This function will move the Response Code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Response Code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_39( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.response_code,
              HOST_MESSAGE.responsecd, DE39_SIZE );
      ret_val = isalphanum(Auth_Tx.TLF01_details.response_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in response code (field 39) not allowed: %s,",
                  Auth_Tx.TLF01_details.response_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_41
 *
 *  DESCRIPTION:  This function will move the terminal ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Terminal ID is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_41( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.terminal_id,
              HOST_MESSAGE.termid, DE41_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_42
 *
 *  DESCRIPTION:  This function will move the Merchant ID from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Merchant ID is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_42( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.merchant_id,
              HOST_MESSAGE.cardaccid, DE42_SIZE );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_43
 *
 *  DESCRIPTION:  This function will move the Card acceptor name/location 
 *				  from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Merchant ID is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_43( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
		memcpy( Auth_Tx.acceptor_term_name, HOST_MESSAGE.cardaccname,    25);
		memcpy( Auth_Tx.acceptor_city_name, HOST_MESSAGE.cardaccname+25, 13);
		memcpy( Auth_Tx.acceptor_country,   HOST_MESSAGE.cardaccname+38, 2);

		if (strlen(Auth_Tx.TLF01_details.card_holder_name) == 0)
		{
			memcpy( Auth_Tx.TLF01_details.card_holder_name, HOST_MESSAGE.cardaccname,    25);
			memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk, HOST_MESSAGE.cardaccname+25, 13);
			memcpy( Auth_Tx.TLF01_details.ncf01_retired_cwk+13,   HOST_MESSAGE.cardaccname+38, 2);
		}


   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         move_field_44
 *
 *  DESCRIPTION:  This function will move the CVV result code 
 *				  from the Host
 *                Message structure according to Equitable formatting
 *                specifications.It is numeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Girija Y - TF
 *
/******************************************************************************/
INT move_field_44( pINT result_flag, pCHAR err_buf )
{
	INT   ret_val = true;
	INT   de44_len = 0;
	char  de44_len_asc[3] = {0};

	if ( *result_flag == 0 )
	{
		strncpy(de44_len_asc,HOST_MESSAGE.addl_rsp_len,2);
		de44_len = atoi(de44_len_asc) ;
		if( de44_len > 0 && de44_len <= MAX_DE44_RESPONSE_LEN )
		{
			memcpy( Auth_Tx.TLF01_details.resp_source, HOST_MESSAGE.addlrspdata, strlen(HOST_MESSAGE.addlrspdata));
		}
		else
		{
			nceqit_log_message( 2, 1, "Either DE44 not present or length of DE44 is greater than 25", "move_field_44" ,1);
		}
	}

	return (true);
}
/******************************************************************************
 *
 *  NAME:         move_request_field_48
 *
 *  DESCRIPTION:  This function will move the Additional Data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Additional Data is alphanumeric, variable
 *                length up to 38 digits.  (Per Equitable requirements, this
 *                field will always be zeroes.)
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *                usage        - Indicates field format
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_request_field_48( pINT result_flag, pCHAR err_buf, INT usage )
{
   INT   ret_val = true;
   INT   len;
   INT   i;
   BYTE  tempstr[4]="";

   if ( *result_flag == 0 )
   {
      /* Get length of data. */
      memcpy( tempstr, HOST_MESSAGE.addl_data_len, 3 );
      len = atoi( tempstr );

      /* Check to see if field is all zeroes.  If yes, ignore this field. */
      for( i=0; i<len; i++ )
      {
         if ( tempstr[i] != '0' )
            break;
      }
	  
      if(len > 1)
      {
         if ( usage == USAGE_NONE )
         {
            /* Usage is based on an Identifier. */
            if(HOST_MESSAGE.addl_data[0] == 'P')
            {
               /* For plus transactions only */

               /* Identifier */
               Auth_Tx.MCF01_details.vs_tcc_code[0] = HOST_MESSAGE.addl_data[0];

               /* Time and Trace number */
               memcpy( Auth_Tx.MCF01_details.negative_group_id,
                      &HOST_MESSAGE.addl_data[1], 9 );

               /* Local Date */
               memcpy( Auth_Tx.MCF01_details.group_type,
                      &HOST_MESSAGE.addl_data[10], 4 );
            }
            else if(HOST_MESSAGE.addl_data[0] == 'V')
            {
               /* For VISA transactions only */

               /* Identifier*/
               Auth_Tx.MCF01_details.vs_tcc_code[0] = HOST_MESSAGE.addl_data[0];

               /* Usage code */
               Auth_Tx.MCF01_details.pos_code[0] = HOST_MESSAGE.addl_data[1];

               /* Document indicator */
               Auth_Tx.MCF01_details.mall_code[0] = HOST_MESSAGE.addl_data[2];

               /* chargeback reference number */
               memcpy(Auth_Tx.MCF01_details.password,&HOST_MESSAGE.addl_data[3],6);

               /* Message text : length 50 */
               memcpy(Auth_Tx.MCF01_details.name01,&HOST_MESSAGE.addl_data[9],30);
               memcpy(Auth_Tx.MCF01_details.name02,&HOST_MESSAGE.addl_data[39],20);
            }
         }
         else if ( usage == USAGE_5 )
         {
            /* Usage 5 - Explanation for message (max length is 50). */
            memcpy(Auth_Tx.MCF01_details.name01,&HOST_MESSAGE.addl_data,30);
            memcpy(Auth_Tx.MCF01_details.name02,&HOST_MESSAGE.addl_data[30],20);
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_admin_request_field_48
 *
 *  DESCRIPTION:  This function will move the Administrative text message 
 *				  Additional Data from the Host Message structure according 
 *				  to Equitable formatting specifications.  
 *				  Additional Data is alphanumeric.
 *
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_admin_request_field_48( pINT result_flag, pCHAR err_buf )
{

   INT   ret_val = true;
   INT   len;
   INT   i;
   BYTE  tempstr[4]="";

   if ( *result_flag == 0 )
   {
      /* Get length of data. */
      memcpy( tempstr, HOST_MESSAGE.addl_data_len, 3 );
      len = atoi( tempstr );

      /* Check to see if field is all zeroes.  If yes, ignore this field. */
      for( i=0; i<len; i++ )
      {
         if ( tempstr[i] != '0' )
            break;
      }
      
	  
	  if(len > 1)
	  {
		  	  
		/* Identifier*/
			Auth_Tx.MCF01_details.vs_tcc_code[0] = HOST_MESSAGE.addl_data[0];
		if(len < (sizeof(Auth_Tx.TLF01_details.voice_auth_text) - 1))
		/* Unformatted text */
			memcpy(Auth_Tx.TLF01_details.voice_auth_text,&HOST_MESSAGE.addl_data[1],len);
		else 
			memcpy(Auth_Tx.TLF01_details.voice_auth_text,&HOST_MESSAGE.addl_data[1],sizeof(Auth_Tx.TLF01_details.voice_auth_text) -1 );
			

		
	  }
	  
        
   }
   
   return( ret_val );
   
}
/******************************************************************************
 *
 *  NAME:         move_response_field_48
 *
 *  DESCRIPTION:  This function will move the Additional Data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Additional Data is alphanumeric, variable
 *                length up to 38 digits.
 *
 *                CONTENTS:
 *                1.  If this data is not provided on txns sent to the switch
 *                    system, this field is all zeroes.
 *                2.  Rejected txn: If response code start with 'N', data is
 *                    left justified, space filled.  If response is numeric
 *                    and with response description, forward message to the
 *                    acquirer.
 *                3.  Cash Advance and Inquiries:
 *                     1-7   filler of zeroes
 *                     8-10  balance currency code
 *                       11  sign byte: C = positive, D = negative
 *                    12-24  current balance, right justified, zero filled
 *                       25  sign byte: C = positive, D = negative
 *                    26-38  available balance, right justified, zero filled
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_response_field_48( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   len;
   INT   i;
   LONG  balance;
   BYTE  tempstr[4]="";

   if ( *result_flag == 0 )
   {
      /* Get length of data. */
      memcpy( tempstr, HOST_MESSAGE.addl_data_len, 3 );
      len = atoi( tempstr );

      /* Check to see if field is all zeroes.  If yes, ignore this field. */
      for( i=0; i<len; i++ )
      {
         if ( tempstr[i] != '0' )
            break;
      }
      if ( i < len )
      {
         /* Field is not all zeroes. */
         if ( 0 == strcmp(APPROVED,Auth_Tx.TLF01_details.response_code) )
         {
            /* Copy the balances to Auth_Tx. */
            Auth_Tx.current_balance_sign = HOST_MESSAGE.addl_data[10];
            memcpy( Auth_Tx.current_balance, &HOST_MESSAGE.addl_data[15], 9 );

            Auth_Tx.available_balance_sign = HOST_MESSAGE.addl_data[24];
            memcpy( Auth_Tx.available_balance, &HOST_MESSAGE.addl_data[29], 9 );

            /* Copy the currency code of the balance information. */
            memcpy( Auth_Tx.host2_src_fi_nbr, &HOST_MESSAGE.addl_data[7], 3 );

            /* Also copy available amount to DE4 so
             * it gets displayed on the device.  Since
             * the field sizes are different, first convert
             * to an integer.
             */
            balance = atol( Auth_Tx.available_balance );
            if ( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,"21",2) )
            {
               /* For Reload transactions, do not overwrite original amount
                * in total_amount because it needs to be stored in BCH10.
                * Dcpiso will pick available balance up out of orig_amount
                * and send it back to terminal in field 4.
                */
               sprintf( Auth_Tx.TLF01_details.orig_amount, "%012d", balance );
            }
            else
            {
               sprintf( Auth_Tx.TLF01_details.total_amount, "%012d", balance );
            }

            /* ------------------------------------------------------- *
             * Copy the balance information into these fields for      *
             * Equitable's proprietary reports. (Acquirer perspective) *
             * ------------------------------------------------------- */

            /* Current Balance */
            sprintf( Auth_Tx.TLF01_details.outstanding_balance, "%012s",
                     Auth_Tx.current_balance );

            if ( Auth_Tx.current_balance_sign == 'D' )
               Auth_Tx.TLF01_details.outstanding_balance[0] = 'D';

            /* Available Balance */
            sprintf( Auth_Tx.TLF01_details.credit_limit, "%012s",
                     Auth_Tx.available_balance );

            if ( Auth_Tx.available_balance_sign == 'D' )
               Auth_Tx.TLF01_details.credit_limit[0] = 'D';
         }
         else
         {
            /* Txn NOT approved.  Forward response text. */
            if ( len > sizeof(Auth_Tx.TLF01_details.response_code)-1 )
               len = sizeof(Auth_Tx.TLF01_details.response_code)-1;

            memcpy( Auth_Tx.TLF01_details.response_text,
                    HOST_MESSAGE.addl_data,
                    len );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_49
 *
 *  DESCRIPTION:  This function will move the currency code from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Currency code is alphanumeric, fixed
 *                length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_49( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.currency_code,
              HOST_MESSAGE.tran_curr_cd, DE49_SIZE );
      ret_val = isalphanum(Auth_Tx.TLF01_details.currency_code);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Special characters in Currency Code (field 49) not allowed: %s,",
                  Auth_Tx.TLF01_details.currency_code );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_52
 *
 *  DESCRIPTION:  This function will move the PIN Block from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  PIN Block is alphanumeric, fixed
 *                length.  This is an encrypted pin, could be padded
 *                with "F"s, left justified.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_52( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  tempstr[20] = "";

   if ( *result_flag == 0 )
   {
      convert_to_ascii( HOST_MESSAGE.pinblock,
                        tempstr,
                        sizeof(HOST_MESSAGE.pinblock) );

      memcpy( Auth_Tx.TLF01_details.pin_block, tempstr, DE52_SIZE );
   }
   return( ret_val );
}
// Phani
void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;
   if(emv_raw[dataIndex] == 0x01)
   {
		dataIndex=dataIndex+3;
   }
   else if(emv_raw[dataIndex] == 0x00)
   {
	   dataIndex=dataIndex+3;
   }

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
            if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
                        memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         move_field_55
 *
 *  DESCRIPTION:  This function will move the EMV Block from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  EMV Block is alphanumeric,
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Phani
 *
 ******************************************************************************/

INT move_field_55( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   CHAR  tempstr[20] = "";
   CHAR  templen [6] = {0} ;INT temp_len=0;	 
   BYTE  ICCdata [256] = {0};
	  temp_len = HOST_MESSAGE.ICCdata_len[0];

   if ( *result_flag == 0 )
   {
     if ( temp_len > 0 && temp_len < 255 )
	{	
	  memcpy(Auth_Tx.EMV_details.emv_block,HOST_MESSAGE.ICCdata_len,1);
	  if ( temp_len > 0) 
	  {     memset(ICCdata, 0, sizeof(ICCdata)); 
	  		memcpy(ICCdata, HOST_MESSAGE.ICCdata, temp_len);    
			memcpy( Auth_Tx.EMV_details.emv_block+1,HOST_MESSAGE.ICCdata,temp_len);
			genutil_VISA_EMV_To_Auth_Tx( ICCdata, temp_len, &Auth_Tx);   
	  }
	}
   }

   /*For Amex ATM txn, PAN sequence issue fix */
	if(strlen(Auth_Tx.EMV_details.pan_sequence_number) == 0)
	{
		strncpy(Auth_Tx.EMV_details.pan_sequence_number,&Auth_Tx.host2_message_seq_nbr[2],2);
	}

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_request_field_62
 *
 *  DESCRIPTION:  This function will move the Private field 62 from the Host
 *                Message structure according to Equitable formatting
 *                specifications. 
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_request_field_62( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   INT     Len;
   INT     bfidx;    /* Index to bitfields for this filed 63*/
   INT     TotalLen;
   INT     BitMap_Len = 8;
   BYTE    TempStr[51] = {0};
   BYTE    BitMap_hex[9]="";
   BYTE    templenstr[9]="";
   BYTE	   tempbitmap[17]={0};


   if ( *result_flag == 0 )
   {
		/* Get length of field. */

	     memcpy( templenstr, HOST_MESSAGE.priv62_len, 3);
		 TotalLen = atoi( templenstr );

		/* Added code for error handlings, Girija Y TF, Jan 21, 2009 */
		/* Copy bitmap into local variable. */
		if ( ( TotalLen > 0 ) && ( TotalLen < 100 ) )
		{
			memset( BitMap_hex, 0x00, sizeof(BitMap_hex) );
	        convert_to_ebcdic( HOST_MESSAGE.priv62, tempbitmap, 16 );
			ebcdic_to_hex(tempbitmap,BitMap_hex, 16 );

	
			memset( TempStr, 0x00, sizeof(TempStr) );

			if( TotalLen > 16 )
			{
				memcpy( TempStr, &HOST_MESSAGE.priv62[16],
							  TotalLen - 16 );
			}
			else
			{
				sprintf(err_buf,"Invalid length for field 62,Len = %d",TotalLen);
				nceqit_log_message(1,2,err_buf,"move_request_field_62()",0);
				return (false);
			}

	   
			if(TempStr[0] != 0x00)
			{
				Len = 0;

				for( bfidx = 1; bfidx <= 27; bfidx++ )
   				{
      				if ( BitMap_hex[BITTAB[bfidx].byte_no] &
           					BITTAB[bfidx].bit_mask )
      				{
					
						/* The bit is set. */
            
						switch( bfidx )
						{
							case 1:  
								/* Authorization character indicator*/

									Auth_Tx.TLF01_details.visa_char_indicator = TempStr[Len];
									Len ++;

								break;
							case 2:
								/* transaction identifier */
								strncpy( Auth_Tx.TLF01_details.visa_tran_id,
												   &TempStr[Len],15);
								Len += 15;
								break;

							case 27:
								/* Routing table unique indicator*/
								strncpy( Auth_Tx.TLF01_details.product_codes[8].amount,&TempStr[Len],7);
								break;
							default:
								Len++;
								break;
					  }
            		}
				}
			}
				 
		}
   }
   return ( ret_val );
}




/******************************************************************************
 *
 *  NAME:         move_request_field_63
 *
 *  DESCRIPTION:  This function will move the Private field 63 from the Host
 *                Message structure according to Equitable formatting
 *                specifications. 
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_request_field_63( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   Len;
   INT   bfidx;    /* Index to bitfields for this filed 63*/
   INT   TotalLen;
   INT   BitMap_Len = 3;
   BYTE  TempStr[51] = {0};
   BYTE  BitMap_hex[4]="";
   BYTE  templenstr[4]="";
   BYTE  tempbitmap[7]={0};
   INT   StrLen ;


   if ( *result_flag == 0 )
   {

      /* Get length of field. */
      memcpy( templenstr, HOST_MESSAGE.priv63_len, 2 );
      TotalLen = atoi( templenstr );

      /* Copy bitmap into local variable. */
      if ( ( TotalLen > 0 ) && ( TotalLen < 100 ) )
      {
         memset( BitMap_hex, 0x00, sizeof(BitMap_hex) );

         convert_to_ebcdic( HOST_MESSAGE.priv63, tempbitmap, 6 );
         ebcdic_to_hex(tempbitmap,BitMap_hex, 6 );

         memset( TempStr, 0x00, sizeof(TempStr) );
		 /* Validate the length of field, not to copy Fld 63 if the bitmap is not present */
		 /* Jan 06 2008, EQIT Interfaces, Girija Y TF */
		 if( TotalLen > 6 )
		 {
			memcpy( TempStr, &HOST_MESSAGE.priv63[6], TotalLen - 6 );
			StrLen = 6;
		 }
		 else
		 {
			 sprintf(err_buf,"Invalid length for field 63,Len = %d",TotalLen);
			 nceqit_log_message(1,2,err_buf,"move_request_field_63()",0);
			 return (false);
		 }
		 

         /*******************
          Since we don't have any corresponding fields present, we are
          using MCF01 fields.
          ********************/
         if(TempStr[0] != 0x00)
         {
            Len = 0;
            for( bfidx = 1; bfidx < 14; bfidx++ )
            {
               if ( BitMap_hex[BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask )
               {
                  /* The bit is set. */

                  switch( bfidx )
                  {
                     /* Network Identification code*/
                     case 1:  
						    /* Validate length */
						    /* Girija Y TF, Jan 07 2009, EQIT Interface */
						    if(( StrLen + 4 ) <= TotalLen )
							{
								strncpy(Auth_Tx.MCF01_details.vip_code,&TempStr[Len],4);
								strncpy(Auth_Tx.TLF01_details.product_codes[10].code,&TempStr[Len],4);
								Len += 4;
								StrLen += 4;
							}
						break;

                     case 2:
                        Len += 4;
						StrLen += 4;
                        break;

                     /* Message Reason Code */
                     case 3:
						 /* Validate length */
						 /* Girija Y TF, Jan 07 2009, EQIT Interface */
						 if( (StrLen + 4 ) <= TotalLen )
						 {
							strncpy( Auth_Tx.MCF01_details.max_num_returns,
                                &TempStr[Len],4 );
							Len += 4;
							StrLen += 4;
						 }
                        break;

                     case 4:
                        Len += 4;
						StrLen += 4;
                        break;

                     /* Plus proprietary member center ID */
                     case 5:
						 /* Validate length */
						 /* Girija Y TF, Jan 07 2009, EQIT Interface */
						 if( (StrLen + 6 ) <= TotalLen )
						 {
							strncpy( Auth_Tx.MCF01_details.process_control_id,
                                &TempStr[Len],6 );
							Len += 6;
							StrLen += 6;
						 }
                        break;

                     /* Number of decimal positions in the amount fields*/
                     case 13:
						 /* Validate length */
						 /* Girija Y TF, Jan 07 2009, EQIT Interface */
						 if( (StrLen + 6 ) <= TotalLen )
						 {
							strncpy( Auth_Tx.MCF01_details.deferred_start_date,
                                &TempStr[Len],6 );
							Len += 6;
							StrLen += 6;
						 }
                        break;

                     default:
                        Len++;
						StrLen += 4;
                        break;
                  }
               }
            }
         }
      }
   }
   return ( ret_val );
}

/******************************************************************************/
/******************************************************************************
 *
 *  NAME:         move_field_70
 *
 *  DESCRIPTION:  This function will move the Network Information Management
 *                code (NMI code) from the Host Message structure according
 *                to Equitable formatting specifications.  NMI Code is
 *                numeric, fixed length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_70( pINT result_flag, pCHAR err_buf )
{
   #define MAX_NMI_CODES  20
   INT   ret_val = true;
   INT   i;

   CHAR  supported_nmi_codes[MAX_NMI_CODES][4] =
                   { "001","002","101","201","301","360",
					 "265","266","267","268","272","274",
					 "281","282","283","284","285","286",
					 "287","288"};

   if ( *result_flag == 0 )
   {
      memcpy( NMI_Code,
              HOST_MESSAGE.nmi_code, DE70_SIZE );
      ret_val = isnum( NMI_Code );
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric NMI Code (field 70): %s,",
                  NMI_Code );
      }
      else
      { 
		 if( (strncmp(Auth_Tx.TLF01_details.message_type ,"0600",4) == 0) ||
			 (strncmp(Auth_Tx.TLF01_details.message_type ,"0620",4) == 0) ||
			 (strncmp(Auth_Tx.TLF01_details.message_type ,"0610",4) == 0) ||
			 (strncmp(Auth_Tx.TLF01_details.message_type ,"0630",4) == 0) )
		 {

			 ret_val = true;
		 }
		 else
		 {
			 ret_val = false;
			 for( i=0; i<MAX_NMI_CODES; i++ )
			 {
				if ( 0 == strcmp( NMI_Code, supported_nmi_codes[i] ) )
				{
				   ret_val = true;
				   break;
				}
			 }
			 if ( ret_val == false )
			 {
				*result_flag = 1;
				sprintf( err_buf,
						"Unsupported NMI Code \"%s\" (field 70), MsgType = %s,",
						 NMI_Code,
						 Auth_Tx.TLF01_details.message_type );
			 }
		 }
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         move_admin_field_70
 *
 *  DESCRIPTION:  This function will move the Network Information Management
 *                code (NMI code) from the Host Message structure according
 *                to Equitable formatting specifications.  NMI Code is
 *                numeric, fixed length.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_admin_field_70( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.service_code,
              HOST_MESSAGE.nmi_code, DE70_SIZE );
      
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_90
 *
 *  DESCRIPTION:  This function will move the original data from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Original data exists only in reversals
 *                Spec says it is numeric, fixed length.  However, it contains
 *                fields from an original 0200 transaction.  Fields 7, 11, and
 *                32. Fields 7 & 11 are numeric, while field 32 is alphanumeric.
 *                So validate accordingly.  Message type of original txn is
 *                also included, as follows:
 *
 *                   Original Message Type              - Pos 1-4
 *                   Original System Trace Num (DE11)   - Pos 5-10
 *                   Original Transmission Time (DE7)   - Pos 11-20
 *                   Original Acquiring Inst ID (DE32)  - Pos 21-22
 *                   Filler of zeroes                   - Pos 23-42
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_90( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;

   if ( *result_flag == 0 )
   {
      /* Get and validate Original Message Type */
      memcpy( Auth_Tx.TLF01_details.orig_message, HOST_MESSAGE.orig_data, 4 );

      ret_val = isnum( Auth_Tx.TLF01_details.orig_message );
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Reversal (%s) contains invalid original msg type %s, (DE90),",
                  Auth_Tx.TLF01_details.message_type,
                  Auth_Tx.TLF01_details.orig_message );
      }
      else
      {
         /* Get and validate original system trace audit number. */
         memcpy( Auth_Tx.TLF01_details.orig_sys_trace_num,
                 HOST_MESSAGE.orig_data+4, DE11_SIZE );
         ret_val = isnum( Auth_Tx.TLF01_details.orig_sys_trace_num );
         if ( ret_val == false )
         {
            *result_flag = 1;
            sprintf( err_buf,
                    "Reversal (%s) contains invalid original stan %s, (DE90),",
                     Auth_Tx.TLF01_details.message_type,
                     Auth_Tx.TLF01_details.orig_sys_trace_num );
         }
         else
         {
            /* Get and validate original transmission timestamp.
             * Store in product codes[18] in order to save for
             * the response.  The value from NCF30 will be sent
             * to the authorizer for processing.
             */
            memcpy( Auth_Tx.TLF01_details.product_codes[18].amount,
                    HOST_MESSAGE.orig_data+10, DE7_SIZE );
            ret_val = isnum( Auth_Tx.TLF01_details.product_codes[18].amount );
            if ( ret_val == false )
            {
               *result_flag = 1;
               sprintf( err_buf,
                 "Reversal (%s) contains invalid original transmission time %s, (DE90),",
                  Auth_Tx.TLF01_details.message_type,
                  Auth_Tx.TLF01_details.product_codes[18].amount );
            }
            else
            {
               /* Get and validate original acquiring institution Id. */
               memcpy( Auth_Tx.orig_acq_inst_id,
                       HOST_MESSAGE.orig_data+20, DE32_SIZE );
               ret_val = isalphanum( Auth_Tx.orig_acq_inst_id );
               if ( ret_val == false )
               {
                  *result_flag = 1;
                  sprintf( err_buf,
                        "Reversal (%s) contains invalid orig acq inst id %s, (DE90),",
                        Auth_Tx.TLF01_details.message_type,
                        Auth_Tx.orig_acq_inst_id );
               }
               else
               {
                  /* Get original currency code. */
                  memcpy( Auth_Tx.TLF01_details.currency_code,
                          HOST_MESSAGE.orig_data+22, DE49_SIZE );
               }
            }
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_95
 *
 *  DESCRIPTION:  This function will move the Replacement Amount from the Host
 *                Message structure according to Equitable formatting
 *                specifications.  Replacement Amount is numeric, fixed
 *                length.  It is of length 42, but only the first 13 bytes
 *                are used.  They are the amount dispensed.  Everything else
 *                should be zeroes
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_95( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   LONG  total_amount;
   LONG  actual_amount;
   LONG  reversal_amount;

   if ( *result_flag == 0 )
   {
      memcpy( Auth_Tx.TLF01_details.actual_amount,
              HOST_MESSAGE.replace_amt+1, 12 );
      ret_val = isnum(Auth_Tx.TLF01_details.actual_amount);
      if ( ret_val == false )
      {
         *result_flag = 1;
         sprintf( err_buf,
                 "Non-numeric Replacement Amount (field 95): %s,",
                  Auth_Tx.TLF01_details.actual_amount );
      }
      else
      {
         /* Verify amount dispensed is a valid amount.
          * It must be less than or equal to the total amount.
          */
         total_amount  = atol( Auth_Tx.TLF01_details.total_amount );
         actual_amount = atol( Auth_Tx.TLF01_details.actual_amount );

         if ( actual_amount > total_amount )
         {
            ret_val = false;
            *result_flag = 1;
            sprintf( err_buf,
                 "Reversal: Amt dispensed (%ld) cannot be greater than txn amount (%ld),",
                  actual_amount, total_amount );
         }
         else
         {
            if ( actual_amount > 0 )
               reversal_amount = total_amount - actual_amount;
            else
               reversal_amount = total_amount;

            sprintf(Auth_Tx.TLF01_details.reversal_amount,"%012d",reversal_amount);
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_100
 *
 *  DESCRIPTION:  This function will move the Receiving Institution identification code
 *                from the Host Message structure according to Equitable
 *                formatting specifications.  .
 *
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_100( pINT result_flag, pCHAR err_buf )
{
   INT    ret_val    = true;
   INT    len;
   CHAR   tempstr[3] = "";
   pBYTE  p_host_ptr = NULL;

   if ( *result_flag == 0 )
   {
      
        memcpy( tempstr, HOST_MESSAGE.rcvid_len, 2 );
        len = atoi(tempstr);
        memcpy( Auth_Tx.receiving_inst_id_code, HOST_MESSAGE.rcvid, len );
      
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         move_field_102
 *
 *  DESCRIPTION:  This function will move the account-1 identification info
 *                from the Host Message structure according to Equitable
 *                formatting specifications.  It is alphanumeric, fixed length.
 *
 *                This field is always present.  If transaction is not an
 *                ATM (DE25 = 02), then zero fill.  Else populate as follows:
 *                   1  Cash Advance Type
 *                   1  Auth Indicator for Visa CPS, else space filled
 *                   15 Transaction ID for Visa CPS, else space filled
 *                   4  Validation Code for Visa CPS, else space filled
 *                   7  Filler of spaces
 *
 *                Cash Advance Type indicates where a transactions was
 *                switched, e.g. Visa Cirrus, JCB as follows:
 *                   A - ECN Visa
 *                   B - Off us Visa
 *                   C - Off us MasterCard
 *                   D - ECN MasterCard
 *                   E - Cirrus
 *                   F - Off us PLUS CASH ADVANCE
 *                   G - Off us PLUS SALE (Plus does not do sale, only ATM)
 *                   H - ECN JCB
 *                   I - Off us JCB
 *                   J - ECN Amex
 *                   K - Off us Amex
 *
 *  INPUTS:       result_flag - If 1, an error occurred else where
 *                Auth_Tx - (Global)Structure populated from Host Message
 *
 *  OUTPUTS:      err_buf - Contains text to describe a reason for failure
 *                result_flag  - Set to 1 if an error occurs
 *                Host_Message - Equitable specific structure (Global)
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_102( pINT result_flag, pCHAR err_buf )
{
   INT    ret_val    = true;
   INT    len;
   CHAR   tempstr[3] = "";
   pBYTE  p_host_ptr = NULL;

   if ( *result_flag == 0 )
   {
      /* See if field is zero or space filled. */
      if ( (HOST_MESSAGE.acct1[0] != ' ') && (HOST_MESSAGE.acct1[0] != '0') )
      {
         /* It has data.  Skip over the Cash Advance Type; do not need it.
          * If the Visa CPS fields are not space filled, move them to Auth_Tx.
          */
         p_host_ptr = HOST_MESSAGE.acct1+1;
         if ( *p_host_ptr != ' ' )
            Auth_Tx.TLF01_details.visa_char_indicator = *p_host_ptr;

         p_host_ptr++;
         if ( *p_host_ptr != ' ' )
            memcpy( Auth_Tx.TLF01_details.visa_tran_id, p_host_ptr, 15 );

         p_host_ptr += 15;
         if ( *p_host_ptr != ' ' )
            memcpy( Auth_Tx.TLF01_details.visa_validation_code, p_host_ptr, 4 );

         /* At this point, we do not know where the txn request came from.
          * Save this whole field as account identification used for the
          * "from" account.  Need it for locally acquired ATM transactions.
          * The data copied into Visa CPS fields above may not be Visa CPS
          * data.  We do not know yet.  So save it both ways: above and here.
          */
         memcpy( tempstr, HOST_MESSAGE.acct1_len, 2 );
         len = atoi(tempstr);
         memcpy( Auth_Tx.acct_id_1, HOST_MESSAGE.acct1, len );

      }
   }
   return( ret_val );
}
// Phani
INT move_field_104( pINT result_flag, pCHAR err_buf )
{
   INT  len;
   BYTE tempstr[4]="";

   if ( *result_flag == 0 )
   {
      memcpy( tempstr, HOST_MESSAGE.trandesc_len, 3 );
   //   convert_to_ascii( tempstr, HOST_MESSAGE.trandesc_len, 3 );
      len = atoi(tempstr);
	 /* sprintf( &Auth_Tx.TLF01_details.voice_auth_text[0],"%1x", len );
      memcpy( &Auth_Tx.TLF01_details.voice_auth_text[1], HOST_MESSAGE.trandesc, len );*/


      Auth_Tx.future_use_2[0]= len;
      memcpy( &Auth_Tx.future_use_2[1], HOST_MESSAGE.trandesc,len);
    /*  if(len > 98)
      {
			memcpy( &Auth_Tx.future_use_2[1], HOST_MESSAGE.trandesc,98);
			if( (len-99) > 99)
			{
				memcpy( &Auth_Tx.future_use_3[1], HOST_MESSAGE.trandesc+98 ,99);
				memcpy( &Auth_Tx.future_use_4[1], HOST_MESSAGE.trandesc+98+99 ,58);
			}
			else
			{
				memcpy( &Auth_Tx.future_use_3[1], HOST_MESSAGE.trandesc+98,(len-98));
			}
		}
      else
      {
    	  memcpy( &Auth_Tx.future_use_2[1], HOST_MESSAGE.trandesc,len);
      }*/
   }
   return( true );
}

/******************************************************************************
 *
 *  NAME:         move_field_103
 *
 *  DESCRIPTION:  This function will move the account identification-2
 *                from the Host Message structure according to Equitable
 *                formatting specifications.  Additional Data is alphanumeric,
 *                variable length up to 28 digits.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_103( pINT result_flag, pCHAR err_buf )
{
   INT  len;
   CHAR tempstr[3]="";

   if ( *result_flag == 0 )
   {
      memcpy( tempstr, HOST_MESSAGE.acct2_len, 2 );
      len = atoi(tempstr);
      memcpy( Auth_Tx.acct_id_2, HOST_MESSAGE.acct2, len );
   }
   return( true );
}

INT move_field_126( pINT result_flag, pCHAR err_buf )
{
   INT   ret_val = true;
   INT   Len;
   INT   bfidx;    /* Index to bitfields for this filed 63*/
   INT   TotalLen;
   INT   BitMap_Len = 8;
   BYTE  TempStr[255] = {0};
   BYTE  BitMap_hex[17]="";
   BYTE  templenstr[4]="";
   BYTE  tempbitmap[17]={0};
   BYTE  buffer[50] = {0};
   INT   StrLen = 0;
   BYTE  tempstr[100]={0};

   if ( *result_flag == 0 )
   {
	   /* Get length of field. */
	   //memcpy( templenstr, HOST_MESSAGE.de126_len, 3);
	   convert_to_ascii( HOST_MESSAGE.de126_len, templenstr, 3 );
	   TotalLen = atoi( templenstr );

	   if ((TotalLen > 0) && (TotalLen < DE126_SIZE))
	   {
	         memset( BitMap_hex, 0x00, sizeof(BitMap_hex) );

	         //convert_to_ebcdic( HOST_MESSAGE.de126, tempbitmap, 16 );
	         memcpy(tempbitmap, HOST_MESSAGE.de126, 16);
	         ebcdic_to_hex(tempbitmap,BitMap_hex, 16 );

			 if( TotalLen > 16 )
			 {
				memcpy( TempStr, &HOST_MESSAGE.de126[16], TotalLen - 16 );
				StrLen = 16;
			 }
			 else
			 {
				 sprintf(err_buf,"Invalid length for field 126,Len = %d",TotalLen);
				 nceqit_log_message(1,2,err_buf,"move_request_field_126()",0);
				 return (false);
			 }

			 if ((TotalLen - 16) > 0)
			 {
		            Len = 0;
		            for( bfidx = 1; bfidx < 25; bfidx++ )
		            {
		               if ( BitMap_hex[BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask )
		               {
		                  /* The bit is set. */

		                  switch( bfidx )
		                  {
		                  case 8:
		                	  /* F126.8,Tran ID */
						  memset(buffer, 0x00, sizeof(buffer));
						  memset(tempstr, 0x00, sizeof(tempstr));
		                  //convert_to_ebcdic( &HOST_MESSAGE.de126[16+Len], buffer, 20 );
		                  memcpy(buffer, &TempStr[Len], 20);
		                  genutil_bin_to_ascii( buffer, 20, tempstr, sizeof(tempstr));
						  strncpy(Auth_Tx.TLF01_details.operator_id, tempstr, 20);
						  strncpy(Auth_Tx.TLF01_details.workstation, tempstr+20, 20);
						  StrLen += 40;
						  Len += 20;
						  break;

						  case 9:
							  /* F126.9, CAVV data */
						  memset(buffer, 0x00, sizeof(buffer));
						  memset(tempstr, 0x00, sizeof(tempstr));
						  //convert_to_ebcdic( &HOST_MESSAGE.de126[16+Len], buffer, 20 );
		                  memcpy(buffer, &TempStr[Len], 20);
						  genutil_bin_to_ascii( buffer, 20, tempstr, sizeof(tempstr));
						  strncpy(Auth_Tx.TLF01_details.dcf01_retired_cwk, tempstr, 40);
						  strncpy(Auth_Tx.TLF01_details.product_codes[0].code, tempstr, 2);
						  StrLen += 40;
						  Len += 20;
						  break;

						  case 10:
							  /* F126.10, CVV2 data */
						  memset(tempstr, 0x00, sizeof(tempstr));
						  convert_to_ascii( &TempStr[Len], tempstr, 6 );
						  strncpy(Auth_Tx.TLF01_details.product_codes[15].code, tempstr, 6);
						  StrLen += 6;
						  Len += 6;
						  break;

						  case 19:
						 /* F126.19, Dynamic Currency Conversion opt in - '1' */
							  memset(tempstr, 0x00, sizeof(tempstr));
							  convert_to_ascii( &TempStr[Len], tempstr, 1 );
						  Auth_Tx.TLF01_details.product_codes[0].code[3] = tempstr[0];
						  StrLen += 1;
						  Len += 1;
						  break;

						  case 20:
							  /* F126.20, 3D-Secure version */
							  memset(tempstr, 0x00, sizeof(tempstr));
							  convert_to_ascii( &TempStr[Len], tempstr, 1 );
						  Auth_Tx.TLF01_details.product_codes[0].code[2] = tempstr[0];
						  StrLen += 1;
						  Len += 1;
						  break;

						default:
						break;
		                  }
		               }
		            }
			 }

	   }
   }
   return (true);
}
/******************************************************************************
 *
 *  NAME:         move_field_128
 *
 *  DESCRIPTION:  This function will move the message authentication code
 *                from the Host Message structure according to Equitable
 *                formatting specifications.  Additional Data is alphanumeric,
 *                variable length up to 38 digits.  (Per Equitable requirements,
 *                this field will always be zeroes.
 *
 *  INPUTS:       Host_Message - Equitable specific structure (Global)
 *                result_flag  - If 1, an error occurred else where
 *
 *  OUTPUTS:      err_buf     - Contains text to describe a reason for failure
 *                Auth_Tx     - (Global)Structure populated from Host Message
 *                result_flag - Set to 1 if an error occurs
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_field_128( pINT result_flag, pCHAR err_buf )
{
   return( true );
}

