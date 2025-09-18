/******************************************************************************
 
 	host2_ml.h
 
    Copyright (c) 1998, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  Megalink Host Handler Driver
 
 	This module is the host handler for the Megalink host.  It
    will process all messages destined to, or originated from,
    the Megalink host.

    APPLICATION:  Advanced Transaction Processor (ATP)
 
    REVISION HISTORY
 
    $Log:   N:\PVCS\PTE\bea\headers\HOST2_ML.H  $
   
      Rev 1.0   08 Sep 1999 10:54:00   epriyev
   initial BEA load
   
      Rev 1.1   17 Mar 1999 11:05:42   rmalathk
   changes made to the message format
   after reviewing traces from Equitable.
   
      Rev 1.0   24 Feb 1999 16:26:58   rmalathk
   Initial Revision
   
 ******************************************************************************/
#ifndef HOST2_MLNK_H
#define HOST2_MLNK_H

#include "basictyp.h"


typedef struct host2_req_ml
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
   BYTE src_terminal_type        [4];
   BYTE src_aba_nbr              [10];
   BYTE src_branch_nbr           [4];
   BYTE src_receipt_nbr          [8];
   BYTE terminal_nbr             [8];
   BYTE merchant_nbr             [16];
   BYTE track3_data              [104];
   BYTE force_post_indicator;
   BYTE reversal_indicator;   
   BYTE trans_acct_nbr           [16];
   BYTE trans_amount             [11];
   BYTE trans_amount_sign;
   BYTE auth_fi_nbr              [4];
   BYTE auth_fi_branch           [4];
   BYTE auth_aba_nbr             [9];
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

} HOST2_MLNK_REQ, *pHOST2_MLNK_REQ;


typedef struct host2_admin_req_ml
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

} HOST2_MLNK_ADMIN_REQ, *pHOST2_MLNK_ADMIN_REQ;



typedef struct host2_resp_ml
{
   BYTE message_id               [2];
   BYTE src_fi_nbr               [4];
   BYTE src_terminal_nbr         [5];
   BYTE src_seq_nbr              [6];
   BYTE message_seq_nbr          [4];
   BYTE transaction_code         [4];
   BYTE trailing_sign;
   BYTE on2_result_code          [4];
   BYTE acct_info_flag;
   BYTE data_info_flag;
   BYTE acct_nbr                 [16];
   BYTE available_balance        [11];
   BYTE available_balance_sign;
   BYTE current_balance          [11];
   BYTE current_balance_sign;
   BYTE extra_padding            [6];   // added to accomodate extra 6 bytes found in trace
   BYTE host_passthru            [32];

} HOST2_MLNK_RESP, *pHOST2_MLNK_RESP;



#endif
   
