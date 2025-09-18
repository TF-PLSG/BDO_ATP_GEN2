/******************************************************************************
 
 	CIRSRCON.C
 
    Copyright (c) 2003, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Cirrus Host Handler Reconciliation Processor
 
    This module is a part of the Cirrus Host Handler.  It handles reconciliation
	processing with the Cirrus host.
 
    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\PVCS\PTE\Equitable\nccirrus\Cirsrcon.c  $
   
      Rev 1.15   Mar 24 2003 14:20:00   dirby
   Modified to set RRN to the same value as the orig RRN and also
   modified to not call ALL_DATA, but call NCF21_DATA instead
   in function recon_update.  The RRN was changed because it
   must be the same value as the one that goes to Cirrus.  No
   longer calling ALL_DATA because then TLF01 does not get properly inserted.
   SCR 1064
   
      Rev 1.14   Jul 03 2002 17:29:24   dirby
   1.  Modified to use pre-calculated sizes instead of always recalculating a structure size.
   2.  Remove use of read_timer.
   3.  Provide details of errors from timerds.
   4.  Added code to display time outs and reversals on Monitor.
   5.  Version 4.0.3.1
   SCR 621
   
      Rev 1.13   Jan 14 2002 18:10:42   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.12   Aug 23 2000 13:38:22   dirby
   Removed function 'close_file' because it is now in the ATP library.
   
      Rev 1.11   Feb 25 2000 10:36:06   ddabberu
   write_totals()  modified to fix scr #433
   
      Rev 1.10   Feb 14 2000 14:56:24   svelaga
   Modified the logic to maintain the transaction
   integrity.
   
      Rev 1.9   Feb 09 2000 09:32:42   ddabberu
   recon_update
   modified for totals, rrn
   
      Rev 1.8   Feb 03 2000 09:57:48   ddabberu
   1.modified create_totals_file() and create_settle_file()
   to add time stamp to the file name.
   2. Modified write_totals to adjust formatting
   
      Rev 1.7   Jan 31 2000 11:41:30   ddabberu
   Functions
   1.recon_no_atp_totals()
   2.write_no_atp_totals() were added to handle 
   settlement/reconcilation when no batch found
   
   To fix SCR#332
   
      Rev 1.6   Jan 27 2000 13:10:22   ddabberu
   updated write_totals to fix SCR#327
   
      Rev 1.5   23 Aug 1999 17:08:36   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.4   21 Jun 1999 16:36:32   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.3   26 Mar 1999 16:37:50   dirby
   Added 'tx_key' to recon_update processing.
   
      Rev 1.2   18 Mar 1999 15:34:22   dirby
   Created a separate function to send a PTE message to a destination queue.
   This new function, 'send_msg' calls 'pteipc_send' and handles send errors.
   This eliminates lots of repetitive code, especially on the error handling.
   
      Rev 1.1   04 Mar 1999 16:47:10   dirby
   Updated for AIX port.
   
      Rev 1.0   19 Feb 1999 16:19:52   dirby
   Initial version
   
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <math.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_database.h"
#include "nc_dbstruct.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "memmnger.h"
#include "txdsapi.h"
#include "txutils.h"
#include "cirsmain.h"
#include "diskio.h"
#include "cirsutil.h"

/* Global Variables */
extern CHAR  AppName[8];

/* Database structures */
extern NCF01             ncf01;
extern NCF21             ncf21;
extern NCF30             ncf30;
extern TLF01             tlf01;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf21_Size;
extern INT    Ncf30_Size;
extern INT    Saf01_Size;
extern INT    Auth_Tx_Size;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];

/* Shared Memory Table Name */
extern CHAR  SHARED_MEMORY_TBL[15];

/* Settlement file pointers */
extern FILE  *Totals_fp;
extern FILE  **pTotals_fp;
extern FILE  *Settle_fp;
extern FILE  **pSettle_fp;

/* Queue definitions */
extern CHAR  cur_app_name [];
extern CHAR	 updateds_que_name[];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  oprmon_que_name[];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  security_que_name[];



/******************************************************************************
 *
 *  NAME:         RECON_UPDATE
 *
 *  DESCRIPTION:  This function sends auth_tx to the data server.
 *                Its purpose is to update totals and keep a running count.
 *                It also stores the transaction into NCF30.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT recon_update( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)];
   BOOLEAN         ret_code = false;

   /* Make sure the reversal RRN is the same as that in the orig txn. */
   if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
   {
      memcpy( p_auth_tx->TLF01_details.retrieval_ref_num,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              bf37_len );
   }

   /* Update NCF21 and insert into NCF30. */
   memset( buffer1, 0x00,      Auth_Tx_Size );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_UPDATE, 
                             ST2_ISSUER,
                             updateds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             Auth_Tx_Size, 
                             NCF21_DATA );
   if ( NULL_PTR == p_msg  )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build data server message",
                             0, WARN_MSG, "recon_update", 1,
                             WARNING_ERROR,p_auth_tx->TLF01_details.terminal_id,
                             p_auth_tx->TLF01_details.card_num,
                             p_auth_tx->TLF01_details.merchant_id, "529");
   }
   else
   {
      /* Send message data server. */
      ret_code = send_msg( p_msg, updateds_que_name, p_auth_tx );
      free( p_msg );
   }
   return( (INT)ret_code );
}

/******************************************************************************
 *
 *  NAME:         RECON_INITIATE
 *
 *  DESCRIPTION:  This function sends ncf21 to the data server.
 *                The data server will begin reconciliation with the host.
 *
 *  INPUTS:       p_ncf21_host_totals - Totals from Cirrus
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT recon_initiate( pNCF21 p_ncf21_host_totals )
{
   pPTE_MSG   p_msg  = NULL_PTR;
   BYTE       buffer1[sizeof(AUTH_TX)+sizeof(NCF21)];
   BOOLEAN    ret_code = false;
   NCF21      ncf21_atp_totals;

   /* Build message to get ATP-generated totals from data server. */
   strcpy( ncf21_atp_totals.primary_key.network_id,    AppName );
   strcpy( ncf21_atp_totals.primary_key.network_type, "I"      );
   strcpy( ncf21_atp_totals.primary_key.batch_number,  ACTIVE  );

   /* Use Auth_Tx_Size, not Ncf21_Size for data server purposes. */
   memset( buffer1, 0x00,   sizeof(buffer1) );
   memcpy( buffer1, p_ncf21_host_totals,  Ncf21_Size );
   memcpy( buffer1+Auth_Tx_Size, &ncf21_atp_totals, Ncf21_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_SELECT, 
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF21_DATA );
   if ( NULL_PTR == p_msg  )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build data server message",
                             1, ALERT_MSG, "recon_initiate", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,"538");
   }
   else
   {
      /* Send message data server. */
      ret_code = send_msg( p_msg, netds_que_name, (pAUTH_TX)NULL_PTR );
      free( p_msg );
   }
   return( (INT)ret_code );
}

/******************************************************************************
 *
 *  NAME:         RECON_CLOSE_BATCH
 *
 *  DESCRIPTION:  This function sends a message to the data server to
 *                close the active batch.
 *
 *  INPUTS:       pCurrent - String of data from the data server.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT recon_close_batch( pBYTE pCurrent )
{
   pPTE_MSG   p_msg  = NULL_PTR;
   BYTE       buffer1[sizeof(AUTH_TX)+sizeof(NCF21)];
   BOOLEAN    ret_code = false;


   memset( buffer1, 0x00,     sizeof(buffer1) );
   memcpy( buffer1, pCurrent, sizeof(buffer1)   );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_UPDATE, 
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF21_DATA );
   if ( NULL_PTR == p_msg  )
   {
      cirsutil_error_handler(1,
                             "Insufficient Memory to build data server message",
                             1, ALERT_MSG, "recon_close_batch", 4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,"542");
   }
   else
   {
      /* Send message data server. */
      ret_code = send_msg( p_msg, netds_que_name, (pAUTH_TX)NULL_PTR );
      free( p_msg );
   }
   return( (INT)ret_code );
}

/******************************************************************************
 *
 *  NAME:         RECON_TOTALS
 *
 *  DESCRIPTION:  This function writes Cirrus batch totals to a file, and 
 *                then sends a message to get the detail transactions.
 *
 *  INPUTS:       pCurrent - String of data from the data server.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT recon_totals( pBYTE pCurrent )
{
   BYTE       buffer1[sizeof(AUTH_TX)+sizeof(NCF21)];
   CHAR       date_str [ 9] = "";
   CHAR       time_str [ 7] = "";
   NCF21      ncf21_host_totals;
   NCF21      ncf21_atp_totals;

   /* Get NCF21 record containing ATP generated totals. */
   memset( buffer1, 0x00, sizeof(buffer1) );
   memcpy( &ncf21_host_totals, pCurrent,              Ncf21_Size );
   memcpy( &ncf21_atp_totals,  pCurrent+Auth_Tx_Size, Ncf21_Size );

   /* Create Totals Filename */
   if ( true == create_totals_file(date_str, time_str) )
   {

      /* Write totals into file */
      if ( true == write_totals(&ncf21_host_totals,&ncf21_atp_totals,
                                date_str,time_str) )
      {
         (void)close_file( pTotals_fp );
      }
      else
         cirsutil_error_handler(1,
                          "Unable to write Cirrus Reconciliation Totals file.",
                          1, WARN_MSG, "recon_totals", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"548");
   }
   else
   {
      cirsutil_error_handler(1,
                          "Unable to create Cirrus Reconciliation Totals file.",
                          1, WARN_MSG, "recon_totals", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"547");
   }

   /* Perform batch processing - create settlement file. */
   (void)recon_settlement( &ncf21_atp_totals, date_str, time_str );
   return( true );
}
/******************************************************************************
 *
 *  NAME:         RECON_TOTALS
 *
 *  DESCRIPTION:  This function writes Cirrus batch totals to a file, and 
 *                then sends a message to get the detail transactions.
 *
 *  INPUTS:       pCurrent - String of data from the data server.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT recon_no_atp_totals( pBYTE pCurrent )
{
   BYTE       buffer1[sizeof(AUTH_TX)+sizeof(NCF21)];
   CHAR       date_str [ 9] = "";
   CHAR       time_str [ 7] = "";
   NCF21      ncf21_host_totals;

   /* Get NCF21 record containing ATP generated totals. */
   memset( buffer1, 0x00, sizeof(buffer1) );
   memcpy( &ncf21_host_totals, pCurrent, Ncf21_Size );

   /* Create Totals Filename */
   if ( true == create_totals_file(date_str, time_str) )
   {

      /* Write totals into file */
      if ( true == write_no_atp_totals(&ncf21_host_totals,date_str,time_str) )
      {
         (void)close_file( pTotals_fp );
      }
      else
         cirsutil_error_handler(1,
                          "Unable to write Cirrus Reconciliation Totals file.",
                          1, WARN_MSG, "recon_totals", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"548");
   }
   else
   {
      cirsutil_error_handler(1,
                          "Unable to create Cirrus Reconciliation Totals file.",
                          1, WARN_MSG, "recon_totals", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"547");
   }

   return( true );
}

/******************************************************************************
 *
 *  NAME:         CREATE_TOTALS_FILE
 *
 *  DESCRIPTION:  This function creates the filename and the file on disk
 *                where the reconciliation totals are written.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      date_str - system date yyyymmdd
 *                time_str - system time hhmmss
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN create_totals_file( pCHAR date_str, pCHAR time_str )
{
   CHAR       time_date[25]    = "";
   CHAR       totals_file[300] = "";


   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Get location to put file. */
   GetAscendentLogDirectory( totals_file );

   /* Create File name */
   strcat( totals_file, TOTALS_FILE_NAME );
   strcat( totals_file, date_str);
   strcat( totals_file, time_str);
   /* Open file. If it already exists, append to it. */
   return( open_file( pTotals_fp, totals_file, "a" ) );
}

/******************************************************************************
 *
 *  NAME:         WRITE_TOTALS
 *
 *  DESCRIPTION:  This function writes the 'totals' information into a file
 *                already opened.
 *
 *  INPUTS:       p_host - NCF21 structure containing totals that came from
 *                         Cirrus in a 0520 transaction
 *                p_atp  - NCF21 structure containing totals from the database
 *                date_str - Current system date - yyyymmdd
 *                time_str - Current system time - hhmmss
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN write_totals( pNCF21 p_host,pNCF21 p_atp,pCHAR date_str,pCHAR time_str )
{
   CHAR  data_string[85];
   INT   i;

   /* First put in some blank lines and titles. */
   memset( data_string, 0x00, sizeof(data_string) );
   strcpy( data_string, "\n\n\n" );
   if ( false == write_file( pTotals_fp, data_string ) )
      return( false );

   memset( data_string, ' ', 5 );
   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   (void)write_file( pTotals_fp, data_string );

   strcpy( data_string, "        CIRRUS  RECONCILIATION TOTALS\n\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Store date and time in file. */
   sprintf( data_string,
      "        Date of Reconciliation: %c%c-%c%c-%c%c%c%c   %c%c:%c%c:%c%c",
      date_str[6],date_str[7],date_str[4],date_str[5],
      date_str[0],date_str[1],date_str[2],date_str[3],
      time_str[0],time_str[1],time_str[2],time_str[3],time_str[4],time_str[5] );

   (void)write_file( pTotals_fp, data_string );

   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   strcat( data_string, "\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Write the column headings. */
   memset( data_string, ' ', 38 );
   strcpy( &data_string[33], "              CIRRUS                  ATP  " );
   (void)write_file( pTotals_fp, data_string );
   strcpy( &data_string[33], "     ---------------      ---------------\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Credit count and amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,"        Credit Count                %12d     %12d\n",
      atoi(p_host->credit_cnt), atoi(p_atp->credit_cnt) );
   /*strcat( data_string, "        Credit Count" );
   strcat( data_string, "                       " );
   strcat( data_string, p_host->credit_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->credit_cnt  );
   */ 
  (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Amount                %16.2f     %16.2f\n",
      atof(p_host->credit_amt)/100, atof(p_atp->credit_amt)/100 );
   (void)write_file( pTotals_fp, data_string );

   /* Credit reversal count and amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Reversal Count       %12d     %12d\n",
      atoi(p_host->credit_rev_cnt), atoi(p_atp->credit_rev_cnt) );
   (void)write_file( pTotals_fp, data_string );

   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Credit Reversal Count" );
   strcat( data_string, "              " );
   strcat( data_string, p_host->credit_rev_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->credit_rev_cnt  );
   (void)write_file( pTotals_fp, data_string );
   */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Reversal Amount       %16.2f     %16.2f\n",
      atof(p_host->credit_rev_amt)/100, atof(p_atp->credit_rev_amt)/100 );
   (void)write_file( pTotals_fp, data_string );

   /* Debit count and amount */
   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Debit Count" );
   strcat( data_string, "                        " );
   strcat( data_string, p_host->debit_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->debit_cnt  );
   (void)write_file( pTotals_fp, data_string );
  */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Count                 %12d     %12d\n",
      atoi(p_host->debit_cnt), atoi(p_atp->debit_cnt) );
   (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Amount                 %16.2f     %16.2f\n",
      atof(p_host->debit_amt)/100, atof(p_atp->debit_amt)/100 );
   (void)write_file( pTotals_fp, data_string );

   /* Debit reversal count and amount */
   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Debit Reversal Count" );
   strcat( data_string, "               " );
   strcat( data_string, p_host->debit_rev_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->debit_rev_cnt  );
   (void)write_file( pTotals_fp, data_string );
   */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Reversal Count        %12d     %12d\n",
      atoi(p_host->debit_rev_cnt), atoi(p_atp->debit_rev_cnt) );
   (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Reversal Amount        %16.2f     %16.2f\n",
      atof(p_host->debit_rev_amt)/100, atof(p_atp->debit_rev_amt)/100 );
   (void)write_file( pTotals_fp, data_string );

   /* TOTAL amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Net Settlement Amount        %16.2f(%s)  %16.2f(%s)\n",
      atof(p_host->total_amt)/100, p_host->total_amt_type,
      atof(p_atp->total_amt)/100,  p_atp->total_amt_type );
   (void)write_file( pTotals_fp, data_string );

   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   return( write_file( pTotals_fp, data_string ));

}

/******************************************************************************
 *
 *  NAME:         WRITE_NO_ATP_TOTALS
 *
 *  DESCRIPTION:  This function writes the 'totals' information into a file
 *                already opened.
 *
 *  INPUTS:       p_host - NCF21 structure containing totals that came from
 *                         Cirrus in a 0520 transaction
 *                p_atp  - NCF21 structure containing totals from the database
 *                date_str - Current system date - yyyymmdd
 *                time_str - Current system time - hhmmss
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN write_no_atp_totals( pNCF21 p_host,pCHAR date_str,pCHAR time_str )
{
   CHAR  data_string[85];
   INT   i;

   /* First put in some blank lines and titles. */
   memset( data_string, 0x00, sizeof(data_string) );
   strcpy( data_string, "\n\n\n" );
   if ( false == write_file( pTotals_fp, data_string ) )
      return( false );

   memset( data_string, ' ', 5 );
   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   (void)write_file( pTotals_fp, data_string );

   strcpy( data_string, "        CIRRUS  RECONCILIATION TOTALS\n\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Store date and time in file. */
   sprintf( data_string,
      "        Date of Reconciliation: %c%c-%c%c-%c%c%c%c   %c%c:%c%c:%c%c",
      date_str[6],date_str[7],date_str[4],date_str[5],
      date_str[0],date_str[1],date_str[2],date_str[3],
      time_str[0],time_str[1],time_str[2],time_str[3],time_str[4],time_str[5] );

   (void)write_file( pTotals_fp, data_string );

   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   strcat( data_string, "\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Write the column headings. */
   memset( data_string, ' ', 38 );
   strcpy( &data_string[33], "              CIRRUS                  ATP  " );
   (void)write_file( pTotals_fp, data_string );
   strcpy( &data_string[33], "     ---------------      ---------------\n" );
   (void)write_file( pTotals_fp, data_string );

   /* Credit count and amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,"        Credit Count                %12d     %12d\n",
      atoi(p_host->credit_cnt), 0 );
   /*strcat( data_string, "        Credit Count" );
   strcat( data_string, "                       " );
   strcat( data_string, p_host->credit_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->credit_cnt  );
   */ 
  (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Amount                %16.2f     %16.2f\n",
      atof(p_host->credit_amt)/100, 0 );
   (void)write_file( pTotals_fp, data_string );

   /* Credit reversal count and amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Reversal Count       %12d     %12d\n",
      atoi(p_host->credit_rev_cnt), 0 );
   (void)write_file( pTotals_fp, data_string );

   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Credit Reversal Count" );
   strcat( data_string, "              " );
   strcat( data_string, p_host->credit_rev_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->credit_rev_cnt  );
   (void)write_file( pTotals_fp, data_string );
   */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Credit Reversal Amount       %16.2f     %16.2f\n",
      atof(p_host->credit_rev_amt)/100, 0);
   (void)write_file( pTotals_fp, data_string );

   /* Debit count and amount */
   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Debit Count" );
   strcat( data_string, "                        " );
   strcat( data_string, p_host->debit_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->debit_cnt  );
   (void)write_file( pTotals_fp, data_string );
  */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Count                 %12d     %12d\n",
      atoi(p_host->debit_cnt), 0 );
   (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Amount                 %16.2f     %16.2f\n",
      atof(p_host->debit_amt)/100, 0);
   (void)write_file( pTotals_fp, data_string );

   /* Debit reversal count and amount */
   /*
   memset( data_string, 0x00, sizeof(data_string) );
   strcat( data_string, "        Debit Reversal Count" );
   strcat( data_string, "               " );
   strcat( data_string, p_host->debit_rev_cnt );
   strcat( data_string, "                 " );
   strcat( data_string, p_atp->debit_rev_cnt  );
   (void)write_file( pTotals_fp, data_string );
   */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Reversal Count       %12d     %12d\n",
      atoi(p_host->debit_rev_cnt), 0 );
   (void)write_file( pTotals_fp, data_string );

   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Debit Reversal Amount        %16.2f     %16.2f\n",
      atof(p_host->debit_rev_amt)/100, 0);
   (void)write_file( pTotals_fp, data_string );

   /* TOTAL amount */
   memset( data_string, 0x00, sizeof(data_string) );
   sprintf( data_string,
      "        Net Settlement Amount        %16.2f(%s)  %16.2f(%s)\n",
      atof(p_host->total_amt)/100, p_host->total_amt_type,
      0,  0 );
   (void)write_file( pTotals_fp, data_string );

   for( i=5; i<=80; i++ )
      data_string[i] = '-';
   return( write_file( pTotals_fp, data_string ));

}

/******************************************************************************
 *
 *  NAME:         RECON_SETTLEMENT
 *
 *  DESCRIPTION:  This function creates a settlement file.
 *
 *  INPUTS:       p_ncf21  - NCF21 structure containing batch number
 *                date_str - Local date yyyymmdd
 *                time_str - Local time hhmmss
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN recon_settlement( pNCF21 p_ncf21, pCHAR date_str, pCHAR time_str )
{
   BOOLEAN  ret_code = true;
   /* Create Settlement Filename */
   if ( true == create_settle_file(date_str, time_str) )
   {
      /* Write settlement file header */
      if ( true == write_settle_header( date_str, time_str ) )
      {
         /* Get transactions and write them to the file. */
         if ( true == write_settle(p_ncf21) )
         {
            (void)close_file( pSettle_fp );
         }
         else
         {
            cirsutil_error_handler(1,
                          "Unable to write to Cirrus settlement file.",
                          1, WARN_MSG, "recon_settlement", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"549");
            ret_code = false;
         }
      }
      else
      {
         cirsutil_error_handler(1,
                       "Unable to write to Cirrus settlement file.",
                       1, WARN_MSG, "recon_settlement", 3,
                       WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"550");
         ret_code = false;
      }
   }
   else
   {
      cirsutil_error_handler(1,
                          "Unable to create Cirrus Settlement file.",
                          1, WARN_MSG, "recon_settlement", 3,
                          WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"551");
      ret_code = false;
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         CREATE_SETTLE_FILE
 *
 *  DESCRIPTION:  This function creates the filename and the file on disk
 *                where the reconciliation transactions are written.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      date_str - system date yyyymmdd
 *                time_str - system time hhmmss
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN create_settle_file( pCHAR date_str, pCHAR time_str )
{
   CHAR     settle_file[300] = "";

   /* Get location to put file. */
   GetAscendentLogDirectory( settle_file );

   /* Create File name */
   strcat( settle_file, SETTLEMENT_FILE_NAME );
   strcat( settle_file, date_str);
   strcat( settle_file, time_str);
   /* Open file. If it already exists, append to it. */
   return( open_file( pSettle_fp, settle_file, "a" ) );
}

/******************************************************************************
 *
 *  NAME:         WRITE_SETTLE_HEADER
 *
 *  DESCRIPTION:  This function writes titles and date into settlement file.
 *
 *  INPUTS:       date_str - Current system date - yyyymmdd
 *                time_str - Current system time - hhmmss
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN write_settle_header( pCHAR date_str, pCHAR time_str )
{
   CHAR  data_string[81];

   /* First put in some blank lines and titles. */
   strcpy( data_string, "\n\n\n        CIRRUS  SETTLEMENT  FILE\n" );
   if ( false == write_file( pSettle_fp, data_string ) )
      return( false );

   /* Store date and time in file. */
   sprintf( data_string,
      "        Date of Reconciliation: %c%c-%c%c-%c%c%c%c   %c%c:%c%c:%c%c\n",
      date_str[6],date_str[7],date_str[4],date_str[5],
      date_str[0],date_str[1],date_str[2],date_str[3],
      time_str[0],time_str[1],time_str[2],time_str[3],time_str[4],time_str[5] );

   return( write_file( pSettle_fp, data_string ) );
}

/******************************************************************************
 *
 *  NAME:         WRITE_SETTLE
 *
 *  DESCRIPTION:  This function writes transactions into settlement file.
 *
 *  INPUTS:       p_ncf21 - NCF21 structure containing key information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN write_settle( pNCF21 p_ncf21 )
{
   NCF30_BATCH_LIST    Ncf30List;
   NCF30               ncf30;
   CHAR                err_msg[100];
   CHAR                data_str[93];
   INT                 max_tran, count_tran, next_portion = 0;
   INT                 total_trans = 0;
   INT                 i;

   /* Initial setting to get detail records from NCF30 */
   memset( &ncf30, 0x00, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id,   p_ncf21->primary_key.network_id   );
   strcpy( ncf30.primary_key.network_type, p_ncf21->primary_key.network_type );
   strcpy( ncf30.primary_key.batch_number, p_ncf21->primary_key.batch_number );
   strcpy( ncf30.primary_key.retrieval_ref_num, " " );
   strcpy( ncf30.open_date, p_ncf21->open_date );
   strcpy( ncf30.open_time, p_ncf21->open_time );

   /* If batch has more than max records, we get them in a next list */
   while ( 0 == next_portion )
   {
      /* Get list of detail records */
      if ( PTEMSG_OK != 
              txdsapi_select_ncf30_service_list(&ncf30, &Ncf30List, err_msg))
      {
         cirsutil_error_handler(1, err_msg, 1, ERROR_MSG, "write_settle",
                        4, FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR, "552" );

         return (false);
      }

      /* Returned number of detail transactions */
       max_tran = atoi(Ncf30List.num_returned);
       total_trans += max_tran;
       if (max_tran < BATCH_SERVICE_LIST_SIZE)
           next_portion = 1;

      /* Loop for detail transactions */
      for (count_tran = 0; count_tran < max_tran; count_tran++)
      {
         memset(&ncf30, 0x00, Ncf30_Size);
         memcpy(&ncf30, &Ncf30List.batch_list[count_tran], Ncf30_Size);

         /* Write a transaction into the settlement file. */
         if ( false == write_settle_record( &ncf30 ) )
         {
            cirsutil_error_handler(1,
                       "Unable to write to Cirrus settlement file.",
                       1, WARN_MSG, "recon_settlement", 3,
                       WARNING_ERROR, NULL_STR, NULL_STR, NULL_STR,"553");
            return( false );
         }
      }/**for end **/

   }/*while end **/

   memset( data_str, 0x00, sizeof(data_str) );
   strcpy(data_str, "\n\n     Number of transactions = %d" );
   sprintf( data_str, data_str, total_trans );
   (void)write_file( pSettle_fp, data_str );

   memset( data_str, ' ', sizeof(data_str) );
   for(i=0; i<=90; i++ )
      data_str[i] = '-';
   return( write_file( pSettle_fp, data_str ) );
}

/******************************************************************************
 *
 *  NAME:         WRITE_SETTLE_RECORD
 *
 *  DESCRIPTION:  This function formats an NCF30 record into settlement file
 *                format and writes it to the settlement file.
 *
 *  INPUTS:       p_ncf30 - NCF30 structure
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN write_settle_record( pNCF30 p_ncf30 )
{
   CHAR  data_str[93];

   memset( data_str, 0x00, sizeof(data_str) );

   sprintf(data_str,"%-19s%6s%012s%6s%06s%04s%012s%06s%08s%-12s",
                  p_ncf30->card_nbr,
                  p_ncf30->processing_code,
                  p_ncf30->tran_amount,
                  p_ncf30->sys_trace_audit_nbr,
                  p_ncf30->tran_time,
                  p_ncf30->tran_date+4,
                  p_ncf30->primary_key.retrieval_ref_num,
                  p_ncf30->auth_number,
                  p_ncf30->device_id,
                  p_ncf30->network_data);

   return( write_file( pSettle_fp, data_str ) );
}


