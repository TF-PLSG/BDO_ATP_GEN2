/******************************************************************************
 *
 *	CIRSMAIN.H
 *
 *  Copyright (c) 2003, Hypercom, Inc.
 *  All Rights Reserved.
 *
 *  TITLE:  Cirrus Host Handler Macros, Constants, and common structures
 *
 *	This header file is used by all of the modules within the
 *  Cirrus host handler application.  It contains global
 *  variables, macros and function prototypes to be used by
 *  any of the application functions.
 *
 *  APPLICATION:  Advanced Transaction Processor (ATP)
 *
 *  AUTHOR:  Dennis Irby
 *
 *  REVISION HISTORY
 *
 *  $Log:   N:\PVCS\PTE\Equitable\nccirrus\Cirsmain.h  $
   
      Rev 1.29   Apr 02 2003 10:40:46   dirby
   Added transaction flow definitions Switched_In and Switched_Out.
   SCR 1073
   
      Rev 1.28   Apr 01 2003 14:40:16   dirby
   Added function prototype 'process_issuer_reversals' to handle
   0422/0432 transactions.
   SCR 1073
   
      Rev 1.27   Mar 24 2003 14:15:46   dirby
   Removed some definitions that are no longer used.
   SCR 1064
   
      Rev 1.26   Feb 17 2003 13:56:38   dirby
   Increased the size of field 'key' in structure 'key_block_st' from 17 to 49.
   SCR 955
   
      Rev 1.25   Jul 29 2002 11:04:16   dirby
   Added definition for the Echo Request Timer Key.  This key is
   used to time an 0800 echo to the host.  It is not used for Log
   On or Log Off.
   SCR 594
   
      Rev 1.24   Jan 16 2002 16:54:04   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.23   Jan 14 2002 18:10:50   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.22   Dec 11 2001 14:51:16   dirby
   Removed definitions for host states.  These reside in app_info.h now.     SCR 488
   
      Rev 1.21   Sep 29 2000 14:50:42   dirby
   Changed value of LIVE_BUT_BUSY from 2 to 3.  This is so Monitor
   will display 'Querying' instead of 'Timed Out'.
   
      Rev 1.20   Sep 18 2000 16:40:26   dirby
   Added prototype of function ShutDown.
   
      Rev 1.19   Aug 28 2000 10:16:02   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.18   Aug 23 2000 13:37:46   dirby
   Added code to support transaction statistics for Monitor.
   
   
      Rev 1.17   Aug 08 2000 13:00:06   dirby
   Removed prototypes of functions that are no longer used.
   
      Rev 1.16   Jan 31 2000 11:41:30   ddabberu
   Functions
   1.recon_no_atp_totals()
   2.write_no_atp_totals() were added to handle 
   settlement/reconcilation when no batch found
   
   To fix SCR#332
   
      Rev 1.15   Jan 23 2000 16:35:16   dirby
   1.  Changed the value of the echo timer key to be "ECHO_CIRS".
   2.  Switched the values of 2 of the host state constants: 1 = offline, 2 =timed out or busy.
   
      Rev 1.14   Jan 13 2000 11:46:02   ddabberu
   Added Code to do auto_logon on service start_up.
   1. Modifed MainProcessor()
   2. Added hhutil_process_response()
   3. Added hhutil_get_ncf01_cfg()
   
      Rev 1.13   23 Aug 1999 17:08:34   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.12   21 Jun 1999 16:36:32   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.11   28 May 1999 17:58:36   dirby
   Added functionality to fill the response text field in auth_tx.
   
      Rev 1.10   16 Apr 1999 16:18:40   dirby
   Added function 'create_rrn' which will create a retrieval
   reference number that meets 3 special criteria:
   1.  Be numeric
   2.  Be unique
   3.  First 4 characters are YDDD - last digit of year + julian day.
   
      Rev 1.9   09 Apr 1999 15:43:56   dirby
   Added function prototype for 'Rj_with_lead_zeros'.
   
      Rev 1.8   19 Mar 1999 17:12:28   dirby
   Created separate functions to handle shared memory manipulation.
   
      Rev 1.7   18 Mar 1999 15:34:20   dirby
   Created a separate function to send a PTE message to a destination queue.
   This new function, 'send_msg' calls 'pteipc_send' and handles send errors.
   This eliminates lots of repetitive code, especially on the error handling.
   
      Rev 1.6   16 Mar 1999 15:46:50   dirby
   Removed call to function 'perform_error_processing' for
   messages from the host in function cirsutil_parse_db_error.
   
   Changed network_type to BYTE in get_ncf01( ).
   
      Rev 1.5   15 Mar 1999 16:28:16   dirby
   Added code to finish the requirement of pin key changes.
   
      Rev 1.4   12 Mar 1999 16:42:56   dirby
   Added functions to perform pin key exchange.
   Only the code to handle the initial request from the
   host is in place.  Still need to code the response.
   
      Rev 1.3   11 Mar 1999 13:06:00   dirby
   Added 2 prototypes and 1 constant:
      reversal_ncf01_reply
      get_ncf01
      ST2_REVERSAL
   
   
      Rev 1.2   10 Mar 1999 17:00:02   dirby
   Removed prototype of function 'get_tran_type'.
   It is not used in Cirrus.
   
      Rev 1.1   25 Feb 1999 16:44:54   dirby
   Added prototype 'set_host_timer'.
   Added constant 'LOGON_TIMER_LOOKUP_KEY'.
   
      Rev 1.0   19 Feb 1999 16:19:50   dirby
   Initial version
 *
 ******************************************************************************/

#ifndef CIRSMAIN_H
#define CIRSMAIN_H


/* Function Prototypes */

/*******************
 * Local Functions *
 *******************/

/* Main modules */
void    PinnacleMsgHandler( pPTE_MSG p_msg_in );
void    MainProcessor();
void    EndProcess();
void    ShutDown();
INT     process_incoming_message( pPTE_MSG p_msg );
INT     incoming_host_message( pPTE_MSG p_msg );
INT     incoming_host_message_timer_reply( pPTE_MSG p_msg_in );
INT     process_timeout_msg( pPTE_MSG p_msg );
INT     process_incoming_message_continued( pAUTH_TX );
void    cirsutil_parse_db_ok( pPTE_MSG );
void    cirsutil_parse_db_error( pPTE_MSG );

INT     incoming_terminal_request( pAUTH_TX p_auth_tx );
INT     send_response_to_term_app( pAUTH_TX p_auth_tx );
BOOLEAN send_it_back_to_terminal( pCHAR, pAUTH_TX );
INT     send_request_to_host( pAUTH_TX p_auth_tx );
INT     send_network_status_to_monitor(void);
void    perform_standin_processing(  pCHAR, BYTE, pAUTH_TX );
void    perform_error_processing( pAUTH_TX, pCHAR );
INT     perform_host_logon_request(  pCHAR, INT, pAUTH_TX );
BOOLEAN process_logon_response( pAUTH_TX p_auth_tx );
void    process_reject( pAUTH_TX );
BOOLEAN process_network_advice( pAUTH_TX );
void    process_admin_advice( pAUTH_TX );
INT     perform_op_net_mngmnt_request( pCHAR, pAUTH_TX );
INT     perform_op_net_mngmnt_response( pCHAR, pAUTH_TX );
BOOLEAN process_key_change_request( pAUTH_TX );
void    create_key_change_response( pAUTH_TX );
void    process_network_advice_timer_reply( pAUTH_TX );
void    process_unsuccessful_key_change( pAUTH_TX, pCHAR );
INT     security_request( pAUTH_TX, BYTE );
INT     security_reply( pPTE_MSG, BYTE );

INT     cirsutil_send_net_mngmnt_response( pAUTH_TX );
INT     cirsutil_send_net_mngmnt_request( pAUTH_TX );
BOOLEAN cirsutil_send_host_response( pAUTH_TX );
BOOLEAN cirsutil_verify_pin_block( pAUTH_TX );
BOOLEAN cirsutil_transfer_ctl_to_auth_app( pAUTH_TX );
INT     cirsutil_parse_host_response_msg();
INT     build_host_request( pAUTH_TX );
void    build_host_structure( pAUTH_TX );
void    move_iso_response_to_auth_struct( pAUTH_TX );
void    cirsutil_init_txn( pAUTH_TX );
void    cirsutil_get_response_text( pAUTH_TX );
void    cirsutil_error_handler( INT, pBYTE, INT, INT, pBYTE, BYTE,
                               pBYTE,   pBYTE,  pBYTE, pBYTE,  pBYTE );

BOOLEAN set_host_timer( pAUTH_TX );
INT     alter_transaction_data( pAUTH_TX, pAUTH_TX );
INT     verify_host_app_status( pAUTH_TX );
void    calc_julian_day( pCHAR, pCHAR );
void    convert_from_julian_day( char *julian_str, char *date_time );
void    get_date( pCHAR time_date, pCHAR date_str );
void    get_time( pCHAR time_date, pCHAR time_str );
void    create_rrn( pCHAR );
BYTE    match_request_to_response( BYTE request );
void    Determine_Auth_Msg_Type( pAUTH_TX, pAUTH_TX );
void    cirsutil_get_strtime ( pCHAR deststr );
BOOLEAN blank_string(pCHAR astr, INT max_len);
BOOLEAN send_msg( pPTE_MSG, pCHAR, pAUTH_TX );
BOOLEAN send_msg_to_authorizer( pAUTH_TX, BYTE );
BOOLEAN clear_timer( pCHAR lookup_key );
void    copy_to_req_resp_buffer( pCHAR, INT );
void    process_issuer_reversals( pAUTH_TX );
void    process_reconciliation( pAUTH_TX );
INT     recon_update( pAUTH_TX );
INT     recon_initiate( pNCF21 );
INT     recon_close_batch( pBYTE );
INT     recon_totals( pBYTE );
INT     recon_no_atp_totals( pBYTE );

BOOLEAN recon_settlement(pNCF21, pCHAR, pCHAR);
BOOLEAN create_totals_file(pCHAR, pCHAR);
BOOLEAN create_settle_file( pCHAR, pCHAR );
BOOLEAN write_totals( pNCF21, pNCF21, pCHAR, pCHAR );
BOOLEAN write_no_atp_totals( pNCF21,pCHAR, pCHAR );

BOOLEAN write_settle_header( pCHAR, pCHAR );
BOOLEAN write_settle( pNCF21 );
BOOLEAN write_settle_record( pNCF30 );
BOOLEAN create_shared_mem_table( pCHAR );
BOOLEAN write_to_shared_mem_table( pBYTE, pBYTE, pBYTE );
BOOLEAN read_shared_mem_table( pBYTE, pBYTE, pBYTE );
BOOLEAN set_host_state( pBYTE host_state );
BOOLEAN decrement_active_count(void);
BOOLEAN increment_active_count(void);
BOOLEAN increment_timeout_count(void);
BOOLEAN reset_timeout_count(void);
BOOLEAN insert_into_tlf01( pAUTH_TX );
void    Rj_with_lead_zeros( pCHAR, INT );
void    write_to_txn_file( void );



/* These functions are used for Store and Forward. */
BOOLEAN saf_send_request_for_record();
BOOLEAN saf_process_db_response( pBYTE, BYTE );
BOOLEAN saf_remove_from_saf01( pAUTH_TX );
BOOLEAN saf_initiate_timer();
BOOLEAN saf_clear_timer();
BOOLEAN process_out_going_reversal( pAUTH_TX );
BOOLEAN process_reversal_response( pAUTH_TX );
BOOLEAN insert_into_saf01( pAUTH_TX );
void    copy_saf01_into_auth_tx( pSAF01, pAUTH_TX );
BYTE    hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg );  //DD 1/13
BYTE hhutil_process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR que_name, pCHAR error_msg ); //DD 1/13

/* Used for key exchange messages. */
typedef struct
{
   CHAR   key_cycle_number[3];
   CHAR   key[49];
   CHAR   key_check_value[17];
} key_block_st, *p_key_block_st;

BOOLEAN parse_host_key_block( pAUTH_TX, p_key_block_st );

/* Transaction Statistics */
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

/* FUTURE USE FUNCTIONS */
INT      incoming_translate_request(pPTE_MSG p_msg);
INT	     build_auth_record( pAUTH_TX );
void     build_auth_structure( pAUTH_TX );
pPTE_MSG crypto_process_request(pCHAR,pCHAR,pBYTE,INT,pBYTE,INT,BYTE,LONG *);
INT      process_keychange_response();
INT      incoming_other_host_response();
void     cirsutil_send_retry_response_to_host( pCHAR, pAUTH_TX );
void     format_date( pCHAR );


/* EXTERNS */
extern void    ptetime_get_timestamp( pCHAR );
extern BOOLEAN TxUtils_Send_Msg_To_Operator(INT,pBYTE,INT,INT,pBYTE,
                                            BYTE,pBYTE,pBYTE,pBYTE,pBYTE);

/* MACROS */
#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT(x)
#endif  

/**********************************
      GLOBAL DECLARATIONS
**********************************/

/* Constants used for message_id in the transaction records */
#define MSG_ID_AUTH_AUTHORIZATION       "0100"
#define MSG_ID_AUTH_SALE                "0200"
#define MSG_ID_AUTH_REFUND              "0200"
#define MSG_ID_AUTH_PAYMENT             "0200"
#define MSG_ID_REVERSAL                 "0400"
#define MSG_ID_AUTH_OFFLINE_SALE        "0220"
#define MSG_ID_AUTH_OFFLINE_REFUND      "0220"
#define MSG_ID_AUTH_SALE_ADJUSTMENT     "0220"
#define MSG_ID_AUTH_REFUND_ADJUSTMENT   "0220"
#define MSG_ID_AUTH_PRE_AUTHORIZATION   "0100"
#define MSG_ID_AUTH_CASH_ADVANCE        "0200"
#define MSG_ID_AUTH_BALANCE_INQUIRY     "0100"
#define MSG_ID_AUTH_VOID_SALE           "0200"
#define MSG_ID_AUTH_VOID_REFUND         "0200"

#define MSG_ID_AUTH_LOGON               "0800"
#define MSG_ID_AUTH_TEST                "0800"


/* Constants & variables used for processing LOGONs */
#define  LOGON_NMI_BIN            "001"
#define  LOGON_NMI_BIN_LOGOFF     "002"
#define  LOGON_NMI_GROUP_ID       "061"
#define  LOGON_NMI_GROUP_LOGOFF   "062"
#define  ECHO_NMI                 "270"
#define  LOGON_REQUEST            "LOGON"
#define  LOGOFF_REQUEST           "LOGOFF"
#define  LOGOFF_RESPONSE          "LOGOFF RESPONSE"
#define  LOGON_RESPONSE           "LOGON RESPONSE"
#define  ECHO_REQUEST             "ECHO"
#define  ECHO_RESPONSE            "LOGON ECHO RESPONSE"
#define  ECHO_TIMER_KEY           "ECHO_CIRS"
#define  ECHO_REQ_TIMER_KEY       "ECHO_REQ_CIRS"
#define  SOURCE_IS_HOST_HANDLER    1

/* Miscellaneous */

#define  NO  0
#define  YES 1
#define  LOGON_TIMER_LOOKUP_KEY   "12233322"
#define  ARTIFICIAL_TERMINAL_ID   "12233322"
#define  SAF_TIMER_KEY            "SAF"
#define  KEY_CHANGE_REQUEST       "161"

/* Reconciliation Processing */
#define  ACTIVE                   "ACTIVE"
#define  SETTLEMENT_ID_CONSTANT   "1072000326"
#define  TOTALS_FILE_NAME         "cirrus_totals_"
#define  SETTLEMENT_FILE_NAME     "cirrus_"

/* The settlement code, field 66, is always set to '9'.
 * This is for 0530 reconciliation advice responses.
 * The '9' indicates to Cirrus that the reconciliation message, 0520,
 * was received; no balancing was performed.  Reconcilication is the
 * responsibility of the processor.
 */
#define  SETTLEMENT_CODE  "9"

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

#define  COMM_BUF_SZE     1500
#define  EXTENDED_BITFIELD 128
#define  NORMAL_BITFIELD    64

/* Transaction Flow */
#define  SWITCHED_IN    0
#define  SWITCHED_OUT   1

#endif
