/*******************************************************************************
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:      amextran.h
*  
* TITLE:       
*
* DESCRIPTION: This module contains function prototypes for amextran.c
*
* AUTHOR:      B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncamex\amextran.h  $
   
      Rev 1.8   Nov 15 2005 16:37:36   dirby
   Added functionality to block ATM transactions (based on MCC) for
   locally acquired transactions (DE49=608 or DE19=608).
   SCR 17015
   
      Rev 1.7   Jan 15 2004 16:40:34   dirby
   Implemented Online Card Updates (1304/1314).
   SCR 1200
   
      Rev 1.6   Jan 26 2001 14:37:00   dirby
   1.  Added function prototypes for get_originator_info and set_originator_info.
   2.  Updated some comments.
   
      Rev 1.5   Jul 10 2000 15:53:20   dirby
   Removed functionality that retrieved NCF01 from the database during
   every online transaction.  Also removed some functions that were completely
   commented out.  They have never been used, and never will.
   
      Rev 1.4   Mar 16 2000 21:37:48   ddabberu
   modified to support AMEX 99.2 specs
   
      Rev 1.3   Aug 30 1999 15:49:44   egable
   Change function definitions.
   
      Rev 1.2   Jun 15 1999 16:28:24   svelaga
   modified the signature of process_logon_response function
   
      Rev 1.1   Mar 25 1999 14:06:52   egable
   Add new function definition
   
      Rev 1.0   Feb 23 1999 09:17:48   egable
   Initial Release
*
*******************************************************************/

#ifndef AMEXTRAN_H
#define AMEXTRAN_H

BOOLEAN send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT     incoming_translate_request(pPTE_MSG p_msg);
INT     incoming_terminal_request(pAUTH_TX p_auth_tx);
INT     incoming_host_message(pPTE_MSG p_msg);
INT     incoming_host_message_timer_reply(pPTE_MSG p_msg_in);
INT     process_timeout_msg(pPTE_MSG p_msg);
INT     send_response_to_term_app(pAUTH_TX p_auth_tx);
int     alter_transaction_data(pAUTH_TX p_temp_auth_tx, pAUTH_TX p_auth_tx);
INT     send_request_to_host( pAUTH_TX p_auth_tx );
INT     get_term_status(BYTE termid[]);
INT     verify_host_app_status(pAUTH_TX p_auth_tx);
INT     perform_host_logon_request(char request_type[], int source,
                               pAUTH_TX p_auth_tx);
int     perform_op_net_mngmnt_request(char request_type[], pAUTH_TX p_auth_tx);
int     perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
BYTE    match_request_to_response(BYTE request);
void    perform_standin_processing(CHAR rsp_code[3], BYTE gen_status,
                                pAUTH_TX p_auth_tx );
void    get_date( pCHAR time_date, pCHAR date_str );
void    get_time( pCHAR time_date, pCHAR time_str );
void    perform_error_processing(pAUTH_TX p_auth_tx_in);
INT     get_amex_data (pAUTH_TX p_auth_tx);
void    find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest );
void    calc_julian_day(char date_str[], char julian_str[]);
INT     process_incoming_message(pPTE_MSG p_msg);
INT     perform_op_net_mngmnt_request_ncf01_reply( pBYTE pCurrent );
INT     send_network_status_to_monitor(void);
BOOLEAN process_logon_response(CHAR nmi_code[4],pAUTH_TX p_auth_tx);

INT     process_incoming_message_continued( pAUTH_TX p_auth_tx );
INT     process_neg_file( pAUTH_TX p_auth_tx );
INT     process_neg_file_reply( pBYTE pCurrent );
INT     send_request_to_host_timer_reply( pPTE_MSG p_msg_in );
INT     recon_update( pAUTH_TX p_auth_tx );
VOID    match_ISO_to_AMEX_response_code(pAUTH_TX p_auth_tx, pBYTE amex_response_code);
VOID    match_AMEX_to_ISO_response_code(pAUTH_TX p_auth_tx, pBYTE amex_response_code);
INT     process_incoming_message_ncf30_reply(pBYTE pCurrent);
INT     delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx );
VOID    update_auth_tx_with_ncf30_data(NCF30 ncf30, pAUTH_TX p_auth_tx);
INT     get_originator_info( pPTE_MSG p_msg_in, pBYTE orig_queue, pBYTE orig_info );
INT     set_originator_info( pAUTH_TX p_auth_tx, pPTE_MSG p_msg_in );
INT     txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );

#define  LOOKUP_KEY_SIZE         22

/*-------*/
/* 1304  */
/*-------*/
INT  process_file_update_request( pPTE_MSG p_msg );
void process_file_update_response( pAUTH_TX p_auth_tx );
void map_1304_to_tlf01( pAUTH_TX p_auth_tx );
INT  send_response_to_GUI( pAUTH_TX p_auth_tx );
INT  validate_gui_inputs( pCHAR err_msg );
void create_stan( pBYTE stan );
INT  validate_date_yymm( pBYTE date, pCHAR err_buf );

INT  populate_1314_resp_struct( pBYTE    resp_code,
                                pCHAR    err_msg,
                                pAUTH_TX p_auth_tx );

void issue_1304_response( pBYTE    resp_code,
                          pCHAR    err_msg,
                          pAUTH_TX p_auth_tx );

INT insert_db_record( BYTE data_type, pAUTH_TX p_auth_tx );

BOOLEAN ncpos_transfer_response_msg_to_nposa2( pAUTH_TX p_auth_tx);
BOOLEAN ncpos_check_for_cadencieacq_transaction( pAUTH_TX p_auth_tx);
INT clear_request_timer( pCHAR lookup_key, pAUTH_TX p_auth_tx );
void create_request_timer_key( pCHAR timer_key, pAUTH_TX p_auth_tx);
INT set_host_timer( pAUTH_TX p_auth_tx,    pCHAR source_que,
                    WORD     timer_length, pCHAR error_msg );

#endif
