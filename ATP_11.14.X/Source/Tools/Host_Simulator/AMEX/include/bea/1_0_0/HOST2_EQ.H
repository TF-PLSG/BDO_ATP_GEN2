/******************************************************************************
 
 	host2_eq.h
 
    Copyright (c) 1998, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Equitable Host Handler Driver
 
 	This module is the host handler for the Equitable host.  It
    will process all messages destined to, or originated from,
    the Equitable host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\PVCS\PTE\bea\headers\HOST2_EQ.H  $
   
      Rev 1.0   08 Sep 1999 10:53:58   epriyev
   initial BEA load
   
      Rev 1.2   Jun 10 1999 15:17:46   rmalathk
   Added 2 fields: acct_info_flag and data_info_flag
   to the response message structure
   
   
      Rev 1.1   17 Mar 1999 11:05:40   rmalathk
   changes made to the message format
   after reviewing traces from Equitable.
   
      Rev 1.0   24 Feb 1999 16:26:56   rmalathk
   Initial Revision
   
 ******************************************************************************/
#ifndef HOST2_EQUIT_H
#define HOST2_EQUIT_H

#include "basictyp.h"


typedef struct host2_req_eq
{
   BYTE message_id               [2];
   BYTE src_fi_nbr               [4];
   BYTE src_terminal_nbr         [5];
   BYTE src_seq_nbr              [6];
   BYTE message_seq_nbr          [4];
   BYTE transaction_code         [4];
   BYTE trailing_sign;
   BYTE src_terminal_type        [4];
   BYTE src_date                 [6];
   BYTE src_time                 [6];
   BYTE src_business_date        [6];
   BYTE src_aba_nbr              [10];
   BYTE src_branch_nbr           [4];
   BYTE src_receipt_nbr          [8];
   BYTE acceptor_term_id         [8];
   BYTE acceptor_term_name       [25];
   BYTE acceptor_city_name       [13];
   BYTE acceptor_country         [2];
   BYTE local_trans_date         [4];
   BYTE local_trans_time         [6];
   BYTE acceptor_name            [14];
   BYTE acquirer_cb_code         [4];
   BYTE filler                   [52];
   BYTE force_post_indicator;
   BYTE reversal_indicator;   
   BYTE trans_acct_nbr           [16];
   BYTE trans_amount             [9];
   BYTE trans_amount_sign;
   BYTE auth_fi_nbr              [4];
   BYTE auth_fi_branch           [4];
   BYTE auth_fi_aba              [9];
   BYTE business_date            [6];
   BYTE stand_in_auth_type;
   BYTE stand_in_auth_method;
   BYTE stand_in_result_code     [4];
   BYTE pin_verify_flag;
   BYTE track2_flag;
   BYTE encrypted_pin            [16];
   BYTE track2                   [40];
   BYTE card_appl_code           [2];
   BYTE card_member_nbr          [4];
   BYTE card_expire_date         [4];
   BYTE reversal_final_amount    [9];
   BYTE reversal_amount_sign;

} HOST2_EQUIT_REQ, *pHOST2_EQUIT_REQ;



typedef struct host2_admin_req_eq
{
   BYTE message_id               [2];
   BYTE src_fi_nbr               [4];
   BYTE src_terminal_nbr         [5];
   BYTE src_seq_nbr              [6];
   BYTE message_seq_nbr          [4];
   BYTE transaction_code         [4];
   BYTE trailing_sign;
   BYTE src_date                 [6];
   BYTE src_time                 [6];
   BYTE src_business_date        [6];

} HOST2_EQUIT_ADMIN_REQ, *pHOST2_EQUIT_ADMIN_REQ;


typedef struct host2_resp_eq
{
   BYTE message_id               [2];
   BYTE src_fi_nbr               [4];
   BYTE src_terminal_nbr         [5];
   BYTE src_seq_nbr              [6];
   BYTE message_seq_nbr          [4];
   BYTE transaction_code         [4];
   BYTE trailing_sign;
   BYTE on2_result_code          [4];
   BYTE acct_info_flag;                //added 6/10/99 per EB's request
   BYTE data_info_flag;                //added 6/10/99 per EB's request
   BYTE available_balance        [9];
   BYTE available_balance_sign;
   BYTE current_balance          [9];
   BYTE current_balance_sign;
   BYTE auth_code                [6];
   BYTE auth_indicator;
   BYTE transaction_id           [15];
   BYTE validation_code          [4];

} HOST2_EQUIT_RESP, *pHOST2_EQUIT_RESP;



#endif
   
