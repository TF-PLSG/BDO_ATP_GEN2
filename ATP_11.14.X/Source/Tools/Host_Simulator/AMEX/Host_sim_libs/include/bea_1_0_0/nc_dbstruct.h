
/******************************************************************************

     Module:               NC_DBSTRUCT.H
  
     Title:                ATP Database Structures

     Description: 

     Application:          ATP

     Authors:              Last modified by Dirby.

     Revision History:     1.0
   
     $Log:   N:\PVCS\PTE\bea\headers\nc_dbstruct.h  $
   
      Rev 1.1   23 Sep 1999 12:56:16   dirby
   Wrong version was taken from Equitable to be the initial version
   for BEA.  Modified by removing some NCF30 fields and the SAF01
   table.
   
      Rev 1.0   08 Sep 1999 10:54:08   epriyev
   initial BEA load
   
      Rev 1.8   Jun 21 1999 14:49:48   farad
   Added the neg02 table to the list.
   
      Rev 1.7   Apr 09 1999 10:43:04   iarustam
   Added ncf01_list for transaction lookup GUI
   
      Rev 1.6   Mar 20 1999 08:56:06   iarustam
   Changes for GUI lists
   
      Rev 1.5   17 Mar 1999 13:43:26   dirby
   Added 'originator_queue' and 'originator_info' to MCC_GUI_Inputs.
   
      Rev 1.4   17 Mar 1999 11:42:08   dirby
   Added 'receiving_inst_id_code' to NCF01.
   
      Rev 1.3   18 Feb 1999 15:59:14   dirby
   Increased size of NCF21 amounts from 15 to 16.
   
      Rev 1.2   15 Feb 1999 16:36:34   dirby
   Rearranged neg01 structure so the 'keys'
   are together at the start of the structure.
   
      Rev 1.1   12 Feb 1999 08:54:02   dirby
    
   
      Rev 1.0   Dec 04 1998 14:44:02   MSALEH
   initial release
******************************************************************************/

#ifndef NC_DBSTRUCT_H
#define NC_DBSTRUCT_H

#include "basictyp.h"


/* network control file */
typedef struct
{
   struct
   {
      BYTE network_id                              [11];
      BYTE network_type                            [2];
   } primary_key;

   BYTE  name                                      [30];
   BYTE  status                                    [2];
   BYTE  totals_ind                                [2];
   BYTE  statistics_ind                            [2];
   BYTE  acquirer_inst_id_code                     [12];
   BYTE  forwarding_inst_id_code                   [12];
   BYTE  receiving_inst_id_code                    [12];
   BYTE  remote_nsp_type                           [2];
   BYTE  pin_type                                  [2];
   BYTE  encr_type                                 [2];
   BYTE  pad_char                                  [2];
   BYTE  cwk                                       [17];
   BYTE  kek                                       [17];
   BYTE  retired_cwk                               [17];
   BYTE  auto_logon                                [2];
   BYTE  echo_interval                             [4];
   BYTE  max_consecutive_timeouts                  [4];
   BYTE  max_active_txns                           [4];
   BYTE  que_name                                  [12];
   BYTE  local_nsp_type                            [8];
   BYTE  request_timer                             [4];
   BYTE  idle_line_timer                           [4];
   BYTE  advice_repeat_counter                     [3];
   BYTE  admin_message_timer                       [4];
   BYTE  store_forward_processing                  [2];
   BYTE  currency_code_purchase                    [4];
   BYTE  currency_code_cash                        [4];
   BYTE  country_code                              [4];
   BYTE  station_id1                               [7];
   BYTE  station_id2                               [7];
   BYTE  prev_src_station                          [7];
   BYTE  tpdu_id                                   [16];
   struct
   {
      BYTE  identifier                             [7];
   }logon_bin[10];
   struct
   {
      BYTE  processing_code                        [7];
   }allowed[20];
   BYTE zmk                                        [33];

} NCF01, *pNCF01;


typedef struct
{
   BYTE  num_returned             [5];
   NCF01 ncf01_record             [GUI_MAX_LIST_SIZE];

} NCF01_GUI_LIST, *pNCF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned             [5];
   NCF01 ncf01_record             [SERVICE_LIST_SIZE];

} NCF01_LIST, *pNCF01_LIST;

/* network control file */
typedef struct
{
   struct
   {
      BYTE network_id                              [11];
      BYTE network_type                            [2];
   } primary_key;

   BYTE  credit_cnt                                [5];
   BYTE  credit_reversal_cnt                       [5];
   BYTE  debit_cnt                                 [5];
   BYTE  debit_reversal_cnt                        [5];
   BYTE  transfer_cnt                              [5];
   BYTE  transfer_reversal_cnt                     [5];
   BYTE  inquiries_cnt                             [5];
   BYTE  authorizations_cnt                        [5];
   BYTE  credit_proc_fee_amt                       [13];
   BYTE  credit_tran_fee_amt                       [13];
   BYTE  debit_proc_fee_amt                        [13];
   BYTE  debit_tran_fee_amt                        [13];
   BYTE  credit_amt                                [13];
   BYTE  credit_reversal_amt                       [13];
   BYTE  debit_amt                                 [13];
   BYTE  debit_reversal_amt                        [13];
   BYTE  net_settlement_amt                        [13];

} NCF02, *pNCF02;


typedef struct
{
   BYTE  num_returned             [5];
   NCF02 ncf02_record             [GUI_MAX_LIST_SIZE];

} NCF02_GUI_LIST, *pNCF02_GUI_LIST;


/* Network Control File*/
typedef struct
{
   struct
   {
      BYTE  network_id                             [10];
      BYTE  yyyymmdd                               [9];
   }primary_key;

   BYTE  ytd_total_tran_cnt                        [13];
   BYTE  ytd_total_tran_amt                        [13];
   BYTE  mtd_total_tran_cnt                        [13];
   BYTE  mtd_total_tran_amt                        [13];
   BYTE  tran_cnt                                  [7];
   BYTE  tran_amt                                  [13];
   BYTE  tran_approved_cnt                         [7];
   BYTE  tran_declined_cnt                         [7];
   struct
   {
      BYTE category_code                           [5];
      BYTE category_cnt                            [5];
   }decline[20];

} NCF20, *pNCF20;


/* Network Control Reconciliation Totals */
typedef struct
{
   struct
   {
      BYTE  network_id              [11];
      BYTE  network_type             [2];
      BYTE  batch_number             [7];
   }primary_key;

   BYTE  open_date                   [9];
   BYTE  open_time                   [7];
   BYTE  release_date                [9];
   BYTE  release_time                [7];
   BYTE  credit_cnt                 [11];
   BYTE  credit_amt                 [17];
   BYTE  credit_rev_cnt             [11];
   BYTE  credit_rev_amt             [17];
   BYTE  debit_cnt                  [11];
   BYTE  debit_amt                  [17];
   BYTE  debit_rev_cnt              [11];
   BYTE  debit_rev_amt              [17];
   BYTE  total_amt                  [17];
   BYTE  total_amt_type              [2];

} NCF21, *pNCF21;


/* Network Control Reconciliation Details */
typedef struct
{
   struct
   {
      BYTE  network_id              [11];
      BYTE  network_type             [2];
      BYTE  batch_number             [7];
      BYTE  retrieval_ref_num       [13];
   }primary_key;

   BYTE  open_date                   [9];
   BYTE  open_time                   [7];
   BYTE  merchant_id                [16];
   BYTE  transaction_id             [21];
   BYTE  device_id                   [9];
   BYTE  card_nbr                   [20];
   BYTE  message_type                [5];
   BYTE  processing_code             [7];
   BYTE  tran_amount                [13];
   BYTE  auth_number                 [7];
   BYTE  response_code               [3];
   BYTE  tran_date                   [9];
   BYTE  tran_time                   [7];
   BYTE  sys_trace_audit_nbr         [7];
   BYTE  tx_key                         ;
   BYTE  invoice_nbr                 [9];
   BYTE  network_data               [13];

} NCF30, *pNCF30;

typedef struct
{
   BYTE  num_returned             [5];
   NCF30 batch_list               [BATCH_SERVICE_LIST_SIZE];

} NCF30_BATCH_LIST, *pNCF30_BATCH_LIST;


/* Network Control Negative File */
typedef struct
{
   BYTE  card_nbr     [17];
   BYTE  card_type     [2];  /* 'V' for Visa, 'M' for MCard */
   BYTE  card_exten    [4];
   BYTE  card_status   [2];

} NEG01, *pNEG01;

/* Network Control Negative back up File */
typedef struct
{
   BYTE  card_nbr     [17];
   BYTE  card_type     [2];  /* 'V' for Visa, 'M' for MCard */
   BYTE  card_exten    [4];
   BYTE  card_status   [2];

} NEG02, *pNEG02;


/* Megalink ATM File */
typedef struct 
{
   struct
   {
      BYTE bkcb_name  [3];
      BYTE termnumr   [9];
   }primary_key;
 
   BYTE bourder       [3];
   BYTE mnemonic      [5];
   BYTE brchname     [16];
   BYTE termsite      [4];
   BYTE termaddr     [51];
   BYTE termdist     [26];
   BYTE termcimu     [26];
   BYTE termprov     [26];
   BYTE termregn      [7];
   BYTE machtype     [21];
   BYTE termodel     [11];
   BYTE dateinst      [9];
   BYTE termloc      [13];
   BYTE termtype      [6];
   BYTE refnum        [7];
   BYTE linktype      [2];

} ATM01, *pATM01;


/* Structure to communicate information between GUI and NCMCRD
 * for file update requests.
 */
typedef struct
{
   CHAR  msg_type            [7];
   BYTE  choice;
   CHAR  card_nbr           [20];
   BYTE  reason;
   CHAR  product_code        [4];
   CHAR  activity_date       [7];
   CHAR  activity_time       [5];
   CHAR  vip_limit_amt      [13];
   CHAR  vip_currency_code   [4];
   INT   number_of_regions;
   CHAR  region1_purge_date  [8];
   CHAR  region2_purge_date  [8];
   CHAR  region3_purge_date  [8];
   CHAR  region4_purge_date  [8];
   CHAR  region5_purge_date  [8];
   BYTE  originator_queue   [17];
   BYTE  originator_info    [33];
} MCC_GUI_Inputs, *pMCC_GUI_Inputs;

typedef struct
{
   CHAR  response_code       [3];
   CHAR  err_msg           [200];
   CHAR  inquiry_record   [2000];
} MCC_GUI_Response, *pMCC_GUI_Response;


#endif