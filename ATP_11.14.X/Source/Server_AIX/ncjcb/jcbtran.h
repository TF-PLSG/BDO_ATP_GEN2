/*******************************************************************************
* Copyright (c) 2004, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:      jcbtran.h
*  
* TITLE:       
*  
* DESCRIPTION: This module will contain the function prototype for jcbtran.c
*
* APPLICATION: 
*
* AUTHOR:      B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\ncjcb\jcbtran.h  $
   
      Rev 1.16   Oct 07 2004 17:44:18   dirby
   Added prototype for function 'find_cav2'.
   SCR 12462
   
      Rev 1.15   Nov 14 2001 16:30:20   dirby
   Replaced code that creates a lookup key for the timer with a
   function to do the same thing.     SCR 251
   
      Rev 1.14   Oct 28 2000 15:48:24   dirby
   Removed function prototypes that are no longer needed
   as a result of not selecting NCF01 during an online transaction.
   
      Rev 1.13   Aug 18 2000 13:15:06   dirby
   Removed some function prototypes due to functions not being used.
   Added validate_date_yymm and isnum prototypes.
   
      Rev 1.12   Jun 02 2000 10:33:38   gdaniels
   fix field 33 and response back for reversal
   when original not found
   
      Rev 1.11   Jun 01 2000 15:04:54   gdaniels
   fix field90 and reversals
   
      Rev 1.10   Jan 03 2000 14:18:44   ddabberu
   code modifications related to new Auth_advice process
   and AUTHORIZATION .
   
      Rev 1.9   Dec 11 1999 11:25:02   ddabberu
   Integration Test Completed
   
      Rev 1.8   Nov 29 1999 17:08:34   ddabberu
    
   
      Rev 1.7   Nov 22 1999 15:34:32   ddabberu
   added code for reversals and advices
   
      Rev 1.6   Nov 22 1999 15:31:58   ddabberu
    
   
      Rev 1.5   Aug 25 1999 17:02:58   egable
   Update function definitions.
   
      Rev 1.4   Aug 13 1999 14:45:56   egable
   Change the function definition of recon_udpate.
   
      Rev 1.3   Jul 15 1999 13:41:10   egable
   Add function prototype.
   
      Rev 1.2   Apr 20 1999 13:57:24   egable
   Change the process_logon_response definition.
   
      Rev 1.1   Mar 17 1999 17:22:16   egable
   Add currency codes.
   
      Rev 1.0   Feb 23 1999 11:10:36   egable
   Initial release
*
*******************************************************************/

#ifndef JCBTRAN_H
#define JCBTRAN_H

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
void    ncjcb_Compute_CAVV_Value(char * CAVV_value, char * cavv);
void    get_date( pCHAR time_date, pCHAR date_str );
void    get_time( pCHAR time_date, pCHAR time_str );
void    perform_error_processing(pAUTH_TX p_auth_tx_in);
INT     get_jcb_data (pAUTH_TX p_auth_tx);
void    find_cvc_flag( BYTE src_len, pCHAR src,  pCHAR dest );
void    find_cav2    ( BYTE  src_len, pCHAR src,  pCHAR dest, pCHAR flag);
void 	find_avs( BYTE src_len, pCHAR src,  pCHAR flag );
void ncjcb_parse_DE48(BYTE src_len, pCHAR src, pAUTH_TX p_auth_tx);
void find_ECI_CAVV_data( BYTE src_len, pCHAR src,pCHAR dest_eci,pCHAR CAVV_data, pCHAR cavv,pAUTH_TX p_auth_tx);
void    calc_julian_day(char date_str[], char julian_str[]);
INT     process_incoming_message(pPTE_MSG p_msg);
INT     process_incoming_message_ncf01_reply(pBYTE pCurrent);
INT     send_network_status_to_monitor(void);
BOOLEAN process_logon_response(CHAR nmi_code[4], BYTE response_code[3]);
BOOLEAN update_ncf02(pAUTH_TX p_auth_tx);
INT     process_incoming_message_continued( pAUTH_TX p_auth_tx );
BOOLEAN process_reversal_advice(pAUTH_TX p_auth_tx);  /*Durga */
BOOLEAN process_reversal_only(pAUTH_TX p_auth_tx);
BOOLEAN send_response_to_host_with_rc_success(CHAR rsp_code[3], pAUTH_TX p_auth_tx );/*Durga */
BOOLEAN get_orig_data_from_NCF30(pAUTH_TX p_auth_tx,pNCF30 pncf30);/*Durga */
void find_ECI( BYTE src_len, pCHAR src,pCHAR dest_eci,pCHAR CAVV_data);

INT     recon_update( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type );
INT     process_incoming_message_ncf30_reply(pBYTE pCurrent);
INT     delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx );
VOID    update_auth_tx_with_ncf30_data(NCF30 ncf30, pAUTH_TX p_auth_tx);
INT     validate_date_yymm (pBYTE exp_date, BYTE tx_key, pCHAR error_msg);
INT     isnum(pCHAR in_str);
void    create_timer_lookup_key( pAUTH_TX p_auth_tx, pCHAR key );

/* Currency Codes */
#define  USD      "840"
#define  PESO     "608"

#define BASE64_CAVV_LEN "28"
#define CAVV_LEN      "20"



#define JCB_3DS_AUTHENTICATION_RESULT_POS  '0'
#define JCB_SECOND_FACTOR_AUTHENTICATION   '2'
#define JCB_3DS_AUTHENTICATION_RESULT_LEN  2
#define JCB_SECOND_FACTOR_AUTHENTICATION_LEN 2

#endif
