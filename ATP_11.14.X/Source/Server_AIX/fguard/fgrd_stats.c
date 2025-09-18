/***************************************************************************
 *
 * Copyright (c) 2005, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      FGRD_STATS.C
 *  
 * TITLE:       Equitable Fraudguard Statistics Processor
 *  
 * DESCRIPTION: This module contains functions that handle transaction
 *              statistics: number in (RECEIVED) and number out (SENT).
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrd_stats.c  $
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
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
#include "fgrd_constants.h"
#include "fgrd_prototypes.h"
#include "txutils.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/******************************************************************************
 *
 *  NAME:         SEND_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will send transaction statistics to Monitor.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to send stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100];

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue, 
                                              (pBYTE)&Monitor_Stats,
                                              sizeof(MONITOR_STATS), 
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      log_message( 2, 2, err_msg, "send_transaction_statistics" );
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         RESET_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this application has
 *                sent to the host.  These include number of approved, timed
 *                out, reversed, and declined txns.  Then this function sends
 *                these newly initialized values to the Monitor.
 *
 *                NOTE: The message being sent to the originator is a response
 *                to a get stats message.  This is the correct response to a
 *                reset stats message.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to reset stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT reset_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100];

   /* Reset the statistics */
   (void)reset_stats( &Txn_Counts );

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue, 
                                              (pBYTE)&Monitor_Stats,
                                              sizeof(MONITOR_STATS), 
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      log_message( 2, 2, err_msg, "reset_transaction_statistics" );
   }
   return( ret_code );
}

