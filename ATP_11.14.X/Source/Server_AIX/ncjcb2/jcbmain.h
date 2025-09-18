/*******************************************************************************
* Copyright (c) 2007, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE: jcbmain.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module the header file for jcbmain.c
*
* APPLICATION: 
*
* AUTHOR:  
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncjcb\jcbmain.h  $
   
      Rev 1.26   Dec 14 2005 14:33:40   dirby
   Changed DE3 to be '00nnnn' for MCC 7995 - QUASI CASH.
   SCR 12318
   
      Rev 1.25   Jun 06 2005 11:25:28   dirby
   Added some POS Entry Mode definitions.
   SCR 15844
   
      Rev 1.24   Feb 22 2005 11:02:46   mbysani
   scr 13560,  EMV Support
   
      Rev 1.23   Oct 07 2004 17:40:40   dirby
   Added constants for field 48 to cull the CAV2 value.
   SCR 12462
   
      Rev 1.22   Jul 09 2004 15:54:58   dirby
   Added Industry Code definitions - mainly for Ecommerce.
   Added CAVV definitions for field 48.
   SCR 1415
   
      Rev 1.21   Apr 15 2002 14:53:10   dirby
   Added definitions for response codes 17 and 68.
   SCR 700
   
      Rev 1.20   Jan 16 2002 16:26:32   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.19   Jan 14 2002 18:17:22   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.18   Nov 13 2001 16:04:00   dirby
   Added support for JCB Online File Update processing - 0302 transactions.     SCR 251
   
      Rev 1.17   Oct 28 2000 15:49:14   dirby
   Added definitions for SWITCHED_IN and SWITCHED_OUT transaction flows.
   
      Rev 1.16   Aug 28 2000 09:51:26   dirby
   Type cast MAX_STAT_COUNT to unsigned long.
   
      Rev 1.15   Aug 22 2000 16:39:20   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.14   Aug 18 2000 13:11:02   dirby
   Added definition of EXPIRED_CARD.
   
      Rev 1.13   Jul 15 2000 23:04:10   dirby
   Added Merchant Category Code 7011 as a MOTO/TE Merchant.
   
      Rev 1.12   Jun 15 2000 12:00:16   dirby
   Added code to validate Merchant Category Code (MCC)
   when there is no track data.
   
      Rev 1.11   Jun 02 2000 10:33:36   gdaniels
   fix field 33 and response back for reversal
   when original not found
   
      Rev 1.10   Jun 01 2000 15:04:54   gdaniels
   fix field90 and reversals
   
      Rev 1.9   Jan 03 2000 14:18:42   ddabberu
   code modifications related to new Auth_advice process
   and AUTHORIZATION .
   
      Rev 1.8   Dec 22 1999 14:05:24   dirby
   Modified to separate advice processing from reversal processing.
   
      Rev 1.7   Dec 11 1999 11:24:58   ddabberu
   Integration Test Completed
   
      Rev 1.6   Nov 29 1999 17:08:32   ddabberu
    
   
      Rev 1.5   Nov 22 1999 15:34:30   ddabberu
   added code for reversals and advices
   
      Rev 1.4   Nov 22 1999 15:31:58   ddabberu
    
   
      Rev 1.3   Aug 12 1999 16:47:42   egable
   Define ST2_NCF30_INSERT
   
      Rev 1.2   19 Apr 1999 10:08:14   dirby
   Added prototype for function 'create_rrn'.
   
      Rev 1.1   16 Apr 1999 10:08:20   dirby
   Changed size of msgtype_bcd from [2] to [4].
   This is to accomodate a message type in EBCDIC format.
   
      Rev 1.0   Feb 23 1999 11:10:34   egable
   Initial release
*
*******************************************************************/


/* Protoypes of functions used in hhmain.c */
#ifndef JCBMAIN_H
#define JCBMAIN_H

extern void EndProcess(); 
void h_hdlr_pinnacle_msg_handler(pPTE_MSG p_msg_in);
void write_to_error_log (pCHAR ErrorObject, pCHAR ErrorFunction, pCHAR ErrorMessage);
void manually_build_t_hndlr_msg_consulta();
void manually_build_t_hndlr_msg_resumo();
void create_rrn( pCHAR );
void write_to_txn_file( void );
INT  process_reversal_responses_to_host( pAUTH_TX );
INT  process_reversal_responses_db_reply( pBYTE pCurrent, INT update_result);
INT  process_auth_advice( pAUTH_TX, INT ,NCF30);
INT  process_file_update_txn( pPTE_MSG p_msg_in );
INT  update_database( pAUTH_TX );
LONG validate_mcc( pAUTH_TX p_auth_tx );
INT  send_transaction_statistics( pPTE_MSG );
INT  reset_transaction_statistics( pPTE_MSG );
INT build_and_send_request_message( BYTE     msgtype,
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

#define ST2_INCOMING_HOST               150    // These are not used by the data server. They
#define ST2_INCOMING_TERMINAL           151    // are used by host handler to determine source
#define ST2_TIMEOUT_MSG                 152    // of the request.
#define ST2_INCOMING_OPERATOR           153   
#define ST2_NCF30_INSERT                158

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1


#define  COMM_BUF_SZE  1500
#define  NOT_FOUND        0
#define  FOUND            1

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

/* POS condition code for Pre-auth and Card verification transaction
 * */
#define POS_CC_PRE_AUTH_CARD_VERFICATION "06"

/* Field 48 Constants */
#define  FIELD48_MAX_LEN  100   

/* CAVV Constants for Ecommerce */
#define  CAVV_TAG_02      "02"
#define  CAVV_TAG_02_LEN  "47"

/* CAV2 Constants for Field 48 */
#define  CAV2_TAG_01      "01"
#define  CAV2_TAG_01_LEN  "03"

#define  DE48_TAG30        "3008"
#define  DE48_TAG31        "3108"

#define  DE48_TAG30_31_LEN   8


#define  SERVICE_CODE_SIZE_PLUS_1        4

#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"

#define PRIMARY_AUTH 		'P'
#define INCREMENTAL_AUTH 	'I'
#define ESTIMATED_AUTH 		'E'

struct iso_msg_rec
   {
        BYTE msgtype_bcd   [4];
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [1];
        BYTE pan_bin       [10];
        BYTE proccode      [3];
        BYTE amount_bin    [6];
        BYTE settle_amt    [6];
        BYTE card_bill_amt [6];
        BYTE gmt_time      [5];
        BYTE conv_rate_stl [4];
        BYTE conv_rate	   [4];
        BYTE stan_bin      [3];
        BYTE time_bin      [3];
        BYTE date_bin      [2];
        BYTE date_expd     [2];
        BYTE date_settle   [2];
        BYTE conv_date	   [2];
        BYTE sic_number    [2];
        BYTE ctry_code     [2];
		BYTE pan_ctry_cd   [2];
        BYTE posentry      [2];
        BYTE card_seq_num  [2];		  
        BYTE nii           [2];			  
        BYTE pos_condcd    [1];			   
        BYTE pin_captcd    [1];
        BYTE tran_fee_amount[9];
        BYTE acq_inst_len  [1];
		BYTE acq_inst_id   [6];
        BYTE fwd_inst_len  [1];			
        BYTE fwd_inst_id   [7];
        BYTE pan_ext_len   [1];
        BYTE pan_ext       [15];
        BYTE track2_len    [1];
		BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [2];
		BYTE src 		   [3];
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
		BYTE cardaccname   [40];
        BYTE addl_rsp_len  [1];
        BYTE addlrspdata   [25];
        BYTE track1_len    [1];
        BYTE track1        [76];
        BYTE addl_data_len [1];
        BYTE addl_data     [225];
        BYTE tran_curr_cd  [3];
        BYTE settle_cur_cd [3];
        BYTE card_curr_cd  [3];
		BYTE pin_block     [8];
        BYTE sec_cntl      [8];
        BYTE addl_len      [1];
        BYTE addlamts      [120];
        BYTE iccdata_len   [2];
        BYTE iccdata       [255];
        BYTE posdata_len   [1];
        BYTE posdata       [14];
        BYTE priv60_len    [1];
        BYTE priv60        [3];
        BYTE priv61_len    [1];
        BYTE priv61        [18];
        BYTE priv62_len    [1];
        BYTE priv62        [58];
        BYTE priv63_len    [1];
        BYTE priv63        [255];
        BYTE nmi_code      [2];
        BYTE action        [3];
        BYTE orig_data     [21];
        BYTE file_update   [1];
        BYTE resp_ind      [5];
        BYTE repl_amts     [42];
        BYTE msgsec        [8];
        BYTE setl_inst_len [1];
        BYTE setl_inst_id  [7];
        BYTE recv_inst_len [1];
        BYTE recv_inst_id  [7];
        BYTE fname_len     [1];
        BYTE filename      [18];
        BYTE acct_id_1_len [1];
        BYTE acct_id_1     [29];
        BYTE acct_id_2_len [1];
        BYTE acct_id_2     [29];
        BYTE trandesc_len  [1];
        BYTE trandesc      [100];
        BYTE intra_ctry_len [1];
        BYTE intra_ctry    [256];
        BYTE record_len    [2];
        BYTE record        [30];
        BYTE iss_inst_len  [1];
        BYTE iss_inst_id   [12];
        BYTE open_use_len  [1];
        BYTE open_use      [14];
        BYTE addr_verf_len [1];
        BYTE addr_verf     [29];
        BYTE free_text_len [1];
        BYTE free_text     [136];
        BYTE access_cd_len [2];
        BYTE access_cd     [255];
		BYTE dest_stat_id  [3];
    	
 };

/* Merchant Category ranges for txns without track data.
 * Transactions without track data will be declined if
 * MCC is not within these ranges.
 */
#define  MCC_RANGE1_LOW   3000
#define  MCC_RANGE1_HIGH  3768

#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969

#define  MCC_MOTO_MERCH_7011 7011  /* This is also a MOTO Merchant. */

#define  MCC_QUASI_CASH  "7995"

/* Response Codes */
#define  APPROVED         "00"
#define  CALL_ISSUER      "01"
#define  CANCELLATION     "17"
#define  TIMED_OUT        "19"
#define  FORMAT_ERROR     "30"
#define  EXPIRED_CARD     "54"
#define  INVALID_PIN      "55"
#define  LATE_RESPONSE    "68"
#define  HOST_UNAVAILABLE "91"

#define  SWITCHED_IN   0
#define  SWITCHED_OUT  1

/* Flags to indicate type of transaction */
#define  ONLINE_UPDATE    0
#define  FINANCIAL        1

/* Constants for Online File Updates - 0302 txns */
#define  TRANSACTION_CODE  "SCPPPSEX"
#define  FILE_NAME         "6332"

/* POS ENTRY MODEs */
#define POS_ENTRY_NORMAL         0
#define POS_ENTRY_EMV_CHIP       1
#define POS_ENTRY_EMV_MAGSTRIPE  2
#define POS_ENTRY_EMV_FALLBACK   3

#endif
