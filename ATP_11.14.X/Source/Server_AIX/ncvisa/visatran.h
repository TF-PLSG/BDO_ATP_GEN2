/*******************************************************************************
* Copyright (c) 2008, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:      visatran.h
*  
* TITLE:       
*  
* DESCRIPTION: This module will contain the function prototype for visatran.c
*
* APPLICATION: 
*
* AUTHOR:      B. Gable
*
   REVISION HISTORY
   ----------------

   Rev 1.17    Feb 5, 2008   dirby
      Added function prototype "is_txn_reversal".
   SCR 25045

      Rev 1.16   Dec 20 2005 14:33:06   dirby
   Added Visa Online File Maintenance handling.  These are
   0302/0312 transactions.
   SCR 18962
   
      Rev 1.15   Apr 29 2004 15:35:08   dirby
   Removed a prototype of a function that is no longer used.
   SCR 1382
   
      Rev 1.14   Apr 22 2003 18:38:34   dirby
   Added function prototype 'set_host_state'.  It is used to set a
   single host state value based on the combination of Acquirer
   and Issuer states.
   SCR 1065
   
      Rev 1.13   Mar 13 2003 15:13:00   dirby
   Adding code to handle reject messages from Visa.  If a response
   is rejected, get the original txn from TLF01 and reverse out the
   transaction.  If a request to Visa is rejected, treat it as if Visa
   declined it with a format error response code.
   SCR 1034
   
      Rev 1.12   Sep 16 2002 15:24:14   dirby
   Added function prototypes for functions that process authorization
   repeats.     SCR 879
   
      Rev 1.11   Apr 24 2002 11:18:06   dirby
   Added function prototype 'txn_is_blocked'
   SCR 703
   
      Rev 1.10   Dec 30 1999 16:38:08   dirby
   Changed input in 'delete_orig_ncf30' to a pointer.
   
      Rev 1.9   Dec 20 1999 15:16:12   ddabberu
   Integration Testested  and the following functions were modified. 
   insertTLf01(), reconUpdate(),
   
   
      Rev 1.8   Nov 16 1999 15:29:06   dirby
   Updated to:
   1.  Remove negative file processing,
   2.  Incorporate a new scheme for handling reversals,
   3.  Use the UpdateDS Data Server.
   
      Rev 1.7   Aug 25 1999 17:06:16   egable
   Update function definitions.
   
      Rev 1.6   Aug 13 1999 14:56:54   egable
   Change the function definition of recon_update
   
      Rev 1.5   Apr 20 1999 16:01:44   egable
   Change definition of process_logon_response.
   
      Rev 1.4   Mar 26 1999 15:23:32   egable
   Add new function definition.
   
      Rev 1.3   Mar 15 1999 13:37:56   egable
   Make corrections from unit test of acquiring 
   path.
   
      Rev 1.2   Feb 23 1999 09:27:52   egable
   Add new function definitions.
   
      Rev 1.1   Feb 02 1999 15:59:38   egable
   Changes to message flow, headers and
   'if' statements.  This is the basis for JCB.
   
   
      Rev 1.0   17 Dec 1998 16:25:18   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:34   MSALEH
   initial release
*
*******************************************************************/

#ifndef VISATRAN_H
#define VISATRAN_H

BOOLEAN send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx );
INT     incoming_terminal_request(pAUTH_TX p_auth_tx);
INT     incoming_host_message(pPTE_MSG p_msg);
INT     incoming_host_message_ncf01_reply(pBYTE pCurrent);
INT     incoming_host_message_timer_reply(pPTE_MSG p_msg_in);
INT     process_timeout_msg(pPTE_MSG p_msg);
INT     process_timeout_msg_ncf01_reply(pBYTE pCurrent);
INT     send_response_to_term_app(pAUTH_TX p_auth_tx);
int     alter_transaction_data(pAUTH_TX p_temp_auth_tx, pAUTH_TX p_auth_tx);
INT     send_request_to_host( pAUTH_TX p_auth_tx );
INT     get_term_status(BYTE termid[]);
INT     verify_host_app_status(pAUTH_TX p_auth_tx);
INT     perform_host_logon_request(char request_type[], int source,
                               pAUTH_TX p_auth_tx, BYTE subtype2);
INT     perform_op_net_mngmnt_request( CHAR request_type[],
                                       pAUTH_TX p_auth_tx,
                                       pBYTE host_state, BYTE subtype2,
                                       pBYTE acq_state, pBYTE iss_state);
int     perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx);
BYTE    match_request_to_response(BYTE request);
void    perform_standin_processing(CHAR rsp_code[3], BYTE gen_status,
                                pAUTH_TX p_auth_tx );
void    get_date( pCHAR time_date, pCHAR date_str );
void    get_time( pCHAR time_date, pCHAR time_str );
void    perform_error_processing(pAUTH_TX p_auth_tx_in);
INT     get_visa_data (pAUTH_TX p_auth_tx);
void    find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest );
void    calc_julian_day(char date_str[], char julian_str[]);
INT     process_incoming_message(pPTE_MSG p_msg);
INT     process_incoming_message_ncf01_reply(pBYTE pCurrent);
INT     perform_op_net_mngmnt_request_ncf01_reply( pBYTE pCurrent );
INT     send_network_status_to_monitor(void);
BOOLEAN process_logon_response(CHAR nmi_code[], CHAR response_code[]);
INT     process_incoming_message_continued( pAUTH_TX p_auth_tx );
INT     recon_update( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type );
INT     process_incoming_message_ncf30_reply(pBYTE pCurrent);
INT     delete_orig_ncf30( pNCF30 pncf30, pAUTH_TX p_auth_tx );
VOID    update_auth_tx_with_ncf30_data(pNCF30 pncf30, pAUTH_TX p_auth_tx);
INT     txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
BYTE    process_auth_repeat_part1( pAUTH_TX p_auth_tx );
BYTE    process_auth_repeat_part2( pAUTH_TX, INT, pNCF30 );
void    increment_tran_id( pAUTH_TX p_auth_tx );
INT     set_host_state( BYTE  acq_state, BYTE  iss_state,
                        pBYTE timeouts,  pBYTE active_txns );
INT     process_0302_request( pPTE_MSG p_msg );
void    process_0312_response( pAUTH_TX p_auth_tx );
void    issue_0302_response( pBYTE resp_code, pAUTH_TX p_auth_tx );
void    decrement_active_txn_ctr( INT time_out_flag );
INT     is_txn_reversal( BYTE tx_key );
BOOLEAN send_response_to_parvbh	( pAUTH_TX p_auth_tx );
#endif
