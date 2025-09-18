/*******************************************************************************
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE: amexmain.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module the header file for amexmain.c
*
* APPLICATION: 
*
* AUTHOR:  
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\ncamex\amexmain.h  $
   
      Rev 1.17   Sep 27 2004 18:17:12   dirby
   Changed size of shortest message from 44 to 42.  This is because
   Amex, for a sign on, sends a 1 byte field in DE33.
   
      Rev 1.16   Jan 15 2004 16:40:34   dirby
   Implemented Online Card Updates (1304/1314).
   SCR 1200
   
      Rev 1.15   Jan 16 2002 15:13:24   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.14   Jan 14 2002 18:08:12   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.13   Mar 02 2001 10:55:40   dirby
   Changed input parameters to function 'init_txn'
   so a pPTE_MSG is not needed.
   
      Rev 1.12   Jan 26 2001 14:35:46   dirby
   1.  Added PTE Message parameter to 'init_txn'.
   2.  Added definitions for host states: ONLINE, OFFLINE, BUSY.
   
      Rev 1.11   Aug 28 2000 10:07:32   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.10   Aug 23 2000 12:46:32   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.9   Aug 01 2000 13:41:28   dirby
   Added function 'validate_host_msg' to weed out garbage
   messages that might find their way through the dialog manager.
   
      Rev 1.8   Jul 10 2000 14:20:32   dirby
   Added function prototype for init_txn.
   
      Rev 1.7   Mar 16 2000 21:37:46   ddabberu
   modified to support AMEX 99.2 specs
   
      Rev 1.6   Dec 22 1999 17:15:30   dirby
   Added input parameter to process_auth_advice for NCF30.
   
      Rev 1.5   Dec 22 1999 10:00:06   dirby
   Modified to separate out 'advice' processing from the 'reversal' processing.
   
      Rev 1.4   Nov 17 1999 14:21:28   dirby
   Modified code for the following reasons:
   1.  Use Update Data Server for updating non-network tables.
   2.  Implement new reversal strategy.
   
      Rev 1.3   Aug 10 1999 16:07:52   egable
   Add a new #define.
   
      Rev 1.2   19 Apr 1999 09:50:30   dirby
   Added function prototype 'create_rrn'.
   
      Rev 1.1   Feb 26 1999 14:34:00   egable
   Correct the size of the card acceptor name 
   and location.
   
      Rev 1.0   Feb 23 1999 09:17:46   egable
   Initial Release
*
*******************************************************************/


/* Protoypes of functions used in hhmain.c */
#ifndef AMEXMAIN_H
#define AMEXMAIN_H

#define DE22_SIZE      12 // Size of POS Entry Mode.

#define AMX1_APR2024 1

extern void EndProcess(); 
void h_hdlr_pinnacle_msg_handler(pPTE_MSG p_msg_in);
void write_to_error_log (pCHAR ErrorObject, pCHAR ErrorFunction, pCHAR ErrorMessage);
void manually_build_t_hndlr_msg_consulta();
void manually_build_t_hndlr_msg_resumo();
void create_rrn( pCHAR );
INT  update_database( pAUTH_TX );
INT  process_auth_advices( pAUTH_TX, INT, NCF30 );
INT  init_txn( pAUTH_TX );
INT  validate_host_msg();
INT  send_transaction_statistics( pPTE_MSG );
INT  reset_transaction_statistics( pPTE_MSG );
void write_to_txn_file( void );
INT  build_and_send_request_message( BYTE     msgtype,
								    BYTE     subtype1, 
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );


#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT(x)
#endif  

/**********************************
      GLOBAL DECLARATIONS
**********************************/
/* ISTMO constants used for message_id in the transaction records */
#define MSG_ID_AUTH_AUTHORIZATION		"0100"
#define MSG_ID_AUTH_SALE				   "0200"
#define MSG_ID_AUTH_REFUND				   "0200"
#define MSG_ID_AUTH_PAYMENT				"0200"
#define MSG_ID_REVERSAL				   	"0400"
#define MSG_ID_AUTH_OFFLINE_SALE	      "0220"
#define MSG_ID_AUTH_OFFLINE_REFUND		"0220"
#define MSG_ID_AUTH_SALE_ADJUSTMENT		"0220"
#define MSG_ID_AUTH_REFUND_ADJUSTMENT	"0220"
#define MSG_ID_AUTH_PRE_AUTHORIZATION	"0100"
#define MSG_ID_AUTH_CASH_ADVANCE		   "0200"
#define MSG_ID_AUTH_BALANCE_INQUIRY		"0100"
#define MSG_ID_AUTH_VOID_SALE			   "0200"
#define MSG_ID_AUTH_VOID_REFUND			"0200"

#define MSG_ID_AUTH_LOGON				   "0800"
#define MSG_ID_AUTH_TEST				   "0800"

#define ST2_INCOMING_HOST               150
#define ST2_INCOMING_TERMINAL           151
#define ST2_TIMEOUT_MSG                 152
#define ST2_INCOMING_OPERATOR           153   
#define ST2_NCF30_INSERT                158

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1


#define  COMM_BUF_SZE  1500

#define  NOT_FOUND  0
#define  FOUND      1

#define  SHORTEST_MSG_LEN   42

#define   FORMAT_ERROR           "30"
#define   HOST_UNAVAILABLE       "91"
#define   FIELD_EDIT_ERROR      "304"
#define   AMX_HOST_UNAVAILABLE  "912"

struct iso_msg_rec
   {
        BYTE msgtype       [4];
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [2];
        BYTE pan_bin       [19];
        BYTE proccode      [6];
        BYTE amount_bin    [12];
        BYTE reconcil_amt  [12];
        BYTE gmt_time      [10];
        BYTE stan_bin      [6];
        BYTE time_bin      [12];
        BYTE date_bin      [4];
        BYTE date_expd     [4];
        BYTE ctry_code     [3];
        BYTE posentry      [12];
        BYTE func_cd       [3];
        BYTE nii           [2];			  
        BYTE reasoncd      [4];			   
        BYTE accpt_bus_cd  [4];			   
        BYTE orig_amt      [24];
        BYTE tran_fee_amt  [8];
		BYTE tranid_len	   [2];
		BYTE tranid		   [40];
        BYTE acq_inst_len  [2];
		BYTE acq_inst_id   [11];
        BYTE fwd_inst_len  [2];			
        BYTE fwd_inst_id   [11];
        BYTE track2_len    [2];
		BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [3];
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
        BYTE cardaccname_len   [2];
        BYTE cardaccname       [99];
        BYTE address_verif_len [2];
        BYTE address_verif     [99];
        BYTE track1_len    [2];
        BYTE track1        [76];
        BYTE addl_data_len [3];
        BYTE addl_data     [350];
        BYTE tran_curr_cd  [3];
        BYTE recn_curr_cd  [3];
        BYTE card_curr_cd  [3];
		BYTE pin_block     [8];
        BYTE sec_cntl_len  [2];
        BYTE sec_cntl      [8];
        BYTE addl_len      [3];
        BYTE addlamts      [120];
		BYTE IccData_len   [3];
		BYTE IccData	   [264];
        BYTE orig_data_len [2];
        BYTE orig_data     [35];
		BYTE priv60_len   [3];
		BYTE priv60 	  [212];
		BYTE priv61_len	  [3];
		BYTE priv61		  [99];
		BYTE priv62_len	  [3];
		BYTE priv62		  [99];
		BYTE priv63_len	  [3];
		BYTE priv63		  [99];
 };


#endif
