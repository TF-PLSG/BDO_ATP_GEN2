/*******************************************************************************
* Copyright (c) 2008, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE: visamain.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module the header file for visamain.c
*
* APPLICATION: 
*
* AUTHOR:  
*
   REVISION HISTORY
   ----------------

   Rev 1.50   Mar 12, 2008   dirby
      Merge Visa Compliance into the Visa Cadencie

      Rev 1.49   Apr 04 2006 16:54:52   dirby
   Increased the size of field 134; added field 140.
   SCR 20160
   
      Rev 1.48   Feb 24 2006 16:26:42   dirby
   Added CVV2 definitions.
   SCR 19670
   
      Rev 1.47   Dec 20 2005 14:33:08   dirby
   Added Visa Online File Maintenance handling.  These are
   0302/0312 transactions.
   SCR 18962
   
      Rev 1.46   Dec 14 2005 12:51:44   dirby
   Changed DE3 to be '11nnnn' for MCC 7995 - QUASI CASH.
   SCR 12418
   
      Rev 1.45   Aug 30 2005 10:26:56   dirby
   Added constant definition for MCC value for ATMs.
   SCR 16625
   
      Rev 1.44   Feb 09 2005 16:43:40   dirby
   Added definitions for various types of POS Entry Modes for EMV.
   SCR 13223
   
      Rev 1.43   Oct 12 2004 15:25:42   dirby
   Added a header field to the HOST_MESSAGE structure to hold
   the Visa header.
   SCR 12755
   
      Rev 1.42   Jul 23 2004 17:24:34   dirby
   Added support for EMV.
   SCR 1388
   
      Rev 1.41   Dec 10 2003 09:43:34   dirby
   Added prototypes and a constant to support finding and validating
   the CAVV result from field 44 in responses.
   SCR 1117
   
      Rev 1.40   Apr 22 2003 18:34:00   dirby
   Added the letter 'v' in front of the host state definitions to
   distinguish them from those defined in app_info.h.
   SCR 1065
   
      Rev 1.39   Mar 13 2003 15:12:42   dirby
   Adding code to handle reject messages from Visa.  If a response
   is rejected, get the original txn from TLF01 and reverse out the
   transaction.  If a request to Visa is rejected, treat it as if Visa
   declined it with a format error response code.
   SCR 1034
   
      Rev 1.38   Sep 16 2002 15:15:32   dirby
   Added definition for system error response code.
   SCR 879
   
      Rev 1.37   Jun 05 2002 15:57:00   dirby
   Added response code definition NO_ACTION_TAKEN.
   SCR 764
   
      Rev 1.36   May 31 2002 16:16:54   dirby
   Added related account type definitions so ncvisa can know
   which account was hit for balances.
   SCR 754
   
      Rev 1.35   May 30 2002 14:49:04   dirby
   Added data structure 'Addl_Amts' to hold balances from DE54
   for balance inquiries and cash withdrawals.
   SCR 750
   
      Rev 1.34   May 29 2002 13:14:02   dirby
   Added definitions for Visa Electron Bin and length of bin.
   SCR 748
   
      Rev 1.33   May 23 2002 16:29:28   dirby
   Renamed structure iso_msg_rec field file_rec to de126 to support DE126.
   SCR 743
   
      Rev 1.32   Apr 24 2002 11:13:12   dirby
   Added response code 57 for blocked txns from Visa.
   SCR 703
   
      Rev 1.31   Apr 19 2002 15:54:28   dirby
   Added definitions for terminal types.  These are based on the
   first position of DE60.
   SCR 702
   
      Rev 1.30   Mar 05 2002 15:02:00   dirby
   Added flags to control flow of MCC Validation.
   SCR 648
   
      Rev 1.29   Feb 01 2002 10:35:48   dirby
   Added definition for Industry Code 800 for Recurring Payment.
   SCR 609
   
      Rev 1.28   Jan 16 2002 15:32:44   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.27   Jan 14 2002 18:22:16   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.26   Jan 07 2002 13:08:12   dirby
   Added definitions of Host States for readability.
   This will be moved to app_info.h in release 401.
   SCR 569 - Send automatic reversal on time outs
   
      Rev 1.25   Dec 10 2001 15:19:22   dirby
   Removed definitions for host states.  These reside in app_info.h now.     SCR 488
   
      Rev 1.24   Oct 23 2001 17:28:14   dirby
   Added macro definition CVV_TEST_FLAG.  It is used to see if
   CVV verification gets performed during the course of a txn.
   SCR 514
   
      Rev 1.23   Oct 23 2001 12:56:48   dirby
   Added definitions of Industry Codes.    SCR 509
   
      Rev 1.22   Oct 01 2001 11:01:56   dirby
   Added definition for response code for invalid_txn.
   
      Rev 1.21   May 23 2001 15:31:24   SYARLAGA
   Added fields in iso_msg_rec
   
      Rev 1.20   Oct 18 2000 15:20:26   dirby
   Added function prototype 'init_txn' for switched in transactions.
   
      Rev 1.19   Aug 28 2000 10:02:56   dirby
   Type cast MAX_STAT_COUNT to unsigned long.
   
      Rev 1.18   Aug 21 2000 16:08:46   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.17   Aug 09 2000 16:53:44   dirby
   Changed definition of MAGNETIC_STRIPE from '90' to '02' to match Hypercom Standards.
   
      Rev 1.16   Jul 30 2000 14:43:06   dirby
   Reworked the Log ON/OFF logic to show accurate status
   on Monitor, and to be a little more efficient.
   
      Rev 1.15   Jun 30 2000 14:07:54   dirby
   Added definitions and prototypes for Visa Electron.
   
      Rev 1.14   Jun 25 2000 01:58:10   dirby
   Added macro definitions for switched in and switched out.
   
      Rev 1.13   Jun 14 2000 17:33:26   dirby
   Added stuff to validate MCC.
   
      Rev 1.12   Jan 04 2000 13:35:56   ddabberu
   updated to process authorization, auth_advice, and reversals.
   changes made in the following functions:
   process_auth_advices(), incoming_other_host_response(), hhutil_db_ok()
   
      Rev 1.11   Dec 30 1999 16:39:06   dirby
   Modified to update transaction control database tables
   upon successful completion of a switched in authorization.
   
      Rev 1.10   Dec 22 1999 11:02:58   dirby
   Modified to separate advice processing from reversals.
   
      Rev 1.9   Dec 20 1999 15:16:08   ddabberu
   Integration Testested  and the following functions were modified. 
   insertTLf01(), reconUpdate(),
   
   
      Rev 1.8   Nov 16 1999 15:29:04   dirby
   Updated to:
   1.  Remove negative file processing,
   2.  Incorporate a new scheme for handling reversals,
   3.  Use the UpdateDS Data Server.
   
      Rev 1.7   Aug 16 1999 16:27:48   egable
   Save the code for field 126.
   
      Rev 1.6   Aug 12 1999 16:48:10   egable
   Define ST2_NCF30_INSERT
   
      Rev 1.5   Jul 29 1999 21:42:14   egable
   Increase the length of field 60 to the max 
   expected.
   
      Rev 1.4   Jun 03 1999 18:54:42   egable
   Change the definition for create_rrn.
   
      Rev 1.3   May 12 1999 16:54:58   egable
   Add src station id to the host message buffer.
   
      Rev 1.2   19 Apr 1999 10:20:04   dirby
   Added function prototype for function 'create_rrn'.
   
      Rev 1.1   Jan 11 1999 09:56:48   egable
   Initial changes for equitable, added  issuing
   paths and several message types.
   
      Rev 1.0   17 Dec 1998 16:25:16   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:32   MSALEH
   initial release
*
*******************************************************************/


/* Protoypes of functions used in hhmain.c */
#ifndef VISAMAIN_H
#define VISAMAIN_H

extern void EndProcess(); 
void h_hdlr_pinnacle_msg_handler(pPTE_MSG p_msg_in);
void write_to_error_log (pCHAR ErrorObject, pCHAR ErrorFunction, pCHAR ErrorMessage);
void manually_build_t_hndlr_msg_consulta();
void manually_build_t_hndlr_msg_resumo();
void create_rrn( pCHAR, pAUTH_TX);
INT  process_auth_advices( pAUTH_TX, INT ,NCF30);
INT  update_database( pAUTH_TX );
LONG validate_mcc( pAUTH_TX p_auth_tx );
INT  parse_track2( pAUTH_TX, pCHAR );
INT  is_txn_visa_electron( pAUTH_TX );
INT  validate_visa_electron( pAUTH_TX, pCHAR );
INT  validate_date_yymm( pBYTE date, pCHAR err_buf );
INT  isnum(pCHAR string);
INT  send_transaction_statistics( pPTE_MSG );
INT  reset_transaction_statistics( pPTE_MSG );
void init_txn( pAUTH_TX );
void write_to_txn_file( void );
void find_cvv2_result_code( BYTE len, pBYTE de44, pBYTE cvv2_result );
void find_carc_flag       ( BYTE len, pBYTE de44, pBYTE carc_result );
void find_cavv_flag       ( BYTE len, pBYTE de44, pBYTE cavv_result );
void get_cavv_result_error( BYTE cavv_result, pCHAR error_msg );

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
#define MSG_ID_AUTH_AUTHORIZATION         "0100"
#define MSG_ID_AUTH_SALE                  "0200"
#define MSG_ID_AUTH_REFUND                "0200"
#define MSG_ID_AUTH_PAYMENT               "0200"
#define MSG_ID_REVERSAL                   "0400"
#define MSG_ID_AUTH_OFFLINE_SALE	      "0220"
#define MSG_ID_AUTH_OFFLINE_REFUND        "0220"
#define MSG_ID_AUTH_SALE_ADJUSTMENT       "0220"
#define MSG_ID_AUTH_REFUND_ADJUSTMENT     "0220"
#define MSG_ID_AUTH_PRE_AUTHORIZATION     "0100"
#define MSG_ID_AUTH_CASH_ADVANCE          "0200"
#define MSG_ID_AUTH_BALANCE_INQUIRY       "0100"
#define MSG_ID_AUTH_VOID_SALE             "0200"
#define MSG_ID_AUTH_VOID_REFUND           "0200"

#define MSG_ID_AUTH_LOGON                 "0800"
#define MSG_ID_AUTH_TEST                  "0800"

#define ST2_INCOMING_HOST               150
#define ST2_INCOMING_TERMINAL           151
#define ST2_TIMEOUT_MSG                 152
#define ST2_INCOMING_OPERATOR           153   
#define ST2_NCF30_INSERT                158

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

#define  COMM_BUF_SZE  1500
#define  NOT_FOUND        0
#define  FOUND            1

#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"

struct iso_msg_rec
   {
        BYTE msgtype_bcd   [2];
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [1];
        BYTE pan_bin       [10];
        BYTE proccode      [3];
        BYTE amount_bin    [6];
        BYTE card_bill_amt [6];
        BYTE gmt_time      [5];
        BYTE conv_rate	   [4];
        BYTE stan_bin      [3];
        BYTE time_bin      [3];
        BYTE date_bin      [2];
        BYTE date_expd     [2];
        BYTE date_settle   [2];
        BYTE sic_number    [2];
        BYTE ctry_code     [2];
        BYTE pan_ctry_cd   [2];
        BYTE posentry      [2];
        BYTE cardseqnum    [2];   /* DE23 */
        BYTE nii           [2];			  
        BYTE pos_condcd    [1];			   
        BYTE pin_captcd    [1];	
		BYTE fee_amount    [9];
        BYTE acq_inst_len  [1];
        BYTE acq_inst_id   [6];
        BYTE fwd_inst_len  [1];			
        BYTE fwd_inst_id   [7];
        BYTE pan_ext_len   [2];		/* DE34 */
        BYTE pan_ext       [255];
        BYTE track2_len    [1];
        BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [2];
        BYTE src           [3];
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
        BYTE cardaccname   [40];
        BYTE addl_rsp_len  [1];
        BYTE addlrspdata   [25];
        BYTE track1_len    [1];
        BYTE track1        [76];
        BYTE addl_data_len [1];
        BYTE addl_data     [256]; /* DE48 */
        BYTE tran_curr_cd  [2];
        BYTE card_curr_cd  [2];
        BYTE pin_block     [8];
        BYTE sec_cntl      [8];
        BYTE addl_len      [1];
        BYTE addlamts      [120];
		BYTE ICCdata_len   [1];
        BYTE ICCdata       [255];
        BYTE PAD_len	   [2];
		BYTE PAD_data      [255];
        BYTE posdata_len   [1];
        BYTE posdata       [14];
        BYTE priv60_len    [1];
        BYTE priv60        [6];
        BYTE priv61_len    [1];
        BYTE priv61        [18];
        BYTE priv62_len    [1];
        BYTE priv62        [58];
        BYTE priv63_len    [1];
        BYTE priv63        [255];
        BYTE nmi_code      [2];
        BYTE action        [3];  /* DE73 */
        BYTE orig_data     [21];
        BYTE file_update   [1];  /* DE91 */
        BYTE resp_ind      [5];
        BYTE repl_amts     [42];
        BYTE msgsec        [8];
        BYTE recv_inst_len [1];
        BYTE recv_inst_id  [7];
        BYTE fname_len     [1];
        BYTE filename      [18]; /* DE101 */
        BYTE acct_id_1_len [1];
        BYTE acct_id_1     [29];
        BYTE acct_id_2_len [1];
        BYTE acct_id_2     [29];
        BYTE trandesc_len  [1];
        BYTE trandesc      [100];
        BYTE issref_len    [1];  /* DE116 */
        BYTE issref        [256];
        BYTE intra_ctry_len[1];  /* DE118 */
        BYTE intra_ctry    [256];
        BYTE org_msg_id    [3];
        BYTE iss_inst_len  [1];
        BYTE iss_inst_id   [12];
        BYTE open_use_len  [1];
        BYTE open_use      [14];
        BYTE addr_verf_len [1];
        BYTE addr_verf     [29];
        BYTE free_text_len [1];
        BYTE free_text     [136];
        BYTE sup_info_len  [1];
        BYTE sup_info      [255];
        BYTE de126_len     [1];
        BYTE de126         [255];
        BYTE file_rec_len  [1];  /* DE127 */
        BYTE file_rec      [255];
        BYTE dest_stat_id  [3];
        BYTE src_stat_id   [3];
        BYTE term_cap_prof [3];  /* DE130 */
        BYTE tvr           [5];  /* DE131 */
        BYTE unpredictable [4];  /* DE132 */
        BYTE tid_serial    [8];  /* DE133 */
        BYTE visa_data_len [1];  /* DE134 */
        BYTE visa_data     [32]; /*   "   */
        BYTE iss_data_len  [1];  /* DE135 */
        BYTE iss_data      [15]; /*   "   */
        BYTE cryptogram    [8];  /* DE136 */
        BYTE app_tran_ctr  [2];  /* DE137 */
        BYTE app_profile   [2];  /* DE138 */
        BYTE arpc_resp     [10]; /* DE139 */
        BYTE iss_auth_len  [1];  /* DE140 */
        BYTE iss_auth_data [255];/*   "   */
        BYTE iss_script_len[1];  /* DE142 */
        BYTE iss_script    [255];/*   "   */
        BYTE iss_result_len[1];  /* DE143 */
        BYTE iss_result    [20]; /*   "   */
        BYTE crypto_type   [1];  /* DE144 */
        BYTE tid_country   [2];  /* DE145 */
        BYTE tid_tran_date [3];  /* DE146 */
        BYTE crypto_amt    [6];  /* DE147 */
        BYTE crypto_curr   [2];  /* DE148 */
        BYTE crypto_cashbk [6];  /* DE149 */
        BYTE header        [30];
 };


/* Merchant Category ranges for txns without track data.
 * Transactions without track data will be declined if
 * MCC is not within these ranges.
 */
#define  MCC_RANGE1_LOW   3000
#define  MCC_RANGE1_HIGH  3768

#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969

/* MCC Validation Control */
#define  MCC_VALIDATION_OFF 0
#define  MCC_VALIDATION_ON  1

/* MCC's */
#define  MCC_ATM         "6011"
#define  MCC_QUASI_CASH  "7995"

/* Response Codes */
#define  APPROVED         "00"
#define  CALL_ISSUER      "01"
#define  DO_NOT_HONOR     "05"
#define  INVALID_TXN      "12"
#define  ACCT_NOT_FOUND   "14"
#define  TIMED_OUT        "19"
#define  NO_ACTION_TAKEN  "21"
#define  FORMAT_ERROR     "30"
#define  EXPIRED_CARD     "54"
#define  NOT_PERMITTED    "57"
#define  EXCEEDS_VELOCITY "61"
#define  HOST_UNAVAILABLE "91"
#define  SYSTEM_ERROR     "96"

/* Transaction Flow */
#define  SWITCHED_IN   0
#define  SWITCHED_OUT  1

/* Visa Electron Service Code from Track2 */
#define  VISA_ELECTRON_SERVICE_CODE   "126"
#define  SERVICE_CODE_SIZE_PLUS_1        4

/* Visa Electron Bin */
#define  VE_BIN       "455968"
#define  VE_BIN_LEN         6

/* Magnetic Stripe Entry Mode ( 1st two digits ) */
#define  MAGNETIC_STRIPE     "02"

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

/* Terminal Types */
#define  ATM_TERMINAL  '2'
#define  POS_TERMINAL  '4'

/* Visa Online File Maintenance Terminal ID (0302 Txns) */
#define  TID_0302  "TID_0302"

#define  ORIG_QUEUE  "ORIGQ"
#define  ORIG_INFO   "ORIGINFO"

/* Miscellaneous */
#define  CVV_TEST_FLAG ' '  /* Space */
#define  CAVV_RESULT_LOC 14
#define  CARC_RESULT_LOC  8
#define  CVV2_RESULT_LOC 10

/*
 * Host States
 */
#define   vONLINE    '0'  /* Online with the host              */
#define   vOFFLINE   '1'  /* Offline with the host             */
#define   vQUERYING  '2'  /* Unknown or busy                   */
#define   vDOWN      '3'  /* Network Controller is not running */
#define   vSTAY      '4'  /* Host state will not change        */

/* DE54 - Structure to hold response balances */
typedef struct
{
   CHAR  acct_type[3];
   CHAR  amt_type [3];
   CHAR  cur_code [4];
   CHAR  sign;
   CHAR  amount   [13];
} Addl_Amts, *pAddl_Amts;

/* Related Account Types */
#define DOLLAR_ACCOUNT_TYPE  '5'
#define PESO_ACCOUNT_TYPE    '6'

/* POS ENTRY MODEs */
#define POS_ENTRY_NORMAL         0
#define POS_ENTRY_EMV_CHIP       1
#define POS_ENTRY_EMV_MAGSTRIPE  2
#define POS_ENTRY_EMV_FALLBACK   3
#define POS_ENTRY_FALLBACK   	 4

#endif
