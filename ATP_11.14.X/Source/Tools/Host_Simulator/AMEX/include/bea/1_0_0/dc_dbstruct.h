
/******************************************************************************

     Module:               ATPDB.H
  
     Title:                ATP Database Structures

     Description: 

     Application:          BP

     Authours:             Last modified by supriya.
   
     Revision History:     1.0
   
     $Log:   N:\PVCS\PTE\bea\headers\dc_dbstruct.h  $
   
      Rev 1.3   Oct 22 1999 17:18:28   iarustam
    
   
      Rev 1.1   Sep 14 1999 16:07:08   svelaga
   update the bch01,bch10,bch11, bch20 and dcf01 to reflect the 
   corresponding database tables.
   
      Rev 1.0   08 Sep 1999 10:54:06   epriyev
   initial BEA load
   
      Rev 1.4   10 Aug 1999 10:57:36   epriyev
   added BCH20 structure
   
      Rev 1.3   May 05 1999 14:04:58   iarustam
   Added bch11_gui_list
   
      Rev 1.2   Feb 09 1999 17:19:42   IGOLDFIE
   CHanged size of BCH01_LIST
   
      Rev 1.1   Feb 03 1999 15:23:22   IGOLDFIE
   Added BCH01_list
   
      Rev 1.0   01 Feb 1999 14:42:56   jhapitas
   Initial Revision
   
      Rev 1.0   Dec 04 1998 14:44:02   MSALEH
   initial release
******************************************************************************/

#ifndef DC_DBSTRUCT_H
#define DC_DBSTRUCT_H

#include "basictyp.h"



/* organization control file */
typedef struct
{
   struct
   {
      BYTE  organization_id                        [5];
   }primary_key;

   BYTE  name01                                    [31];
   BYTE  addr01                                    [31];
   BYTE  addr02                                    [31];
   BYTE  city                                      [21];
   BYTE  state                                     [3];
   BYTE  zip_code                                  [11];
   BYTE  intl_code                                 [4];
   BYTE  contact                                   [21];
   BYTE  phone01                                   [18];
   BYTE  phone02                                   [18];
   BYTE  date_added                                [9];
   BYTE  status                                    [2];
   BYTE  totals_ind                                [2];
   BYTE  statistics_ind                            [2];
}OCF01, *pOCF01;

typedef struct
{
   struct
   {
      BYTE  organization_id                        [5];
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
      BYTE  category_code                          [5];
      BYTE  category_cnt                           [5];
   }decline[20];
}OCF20, *pOCF20;



typedef struct
{
   struct
   {
      BYTE  organization_id                        [5];
      BYTE  merchant_id                            [16];
      BYTE  yyyymmdd                               [9];
   }primary_key;

   BYTE  ytd_total_adds                            [7];
   BYTE  ytd_total_deletes                         [7];
   BYTE  mtd_total_adds                            [7];
   BYTE  mtd_total_deletes                         [7];
   struct
   {
      BYTE  adds                                   [7];
      BYTE  deletes                                [7];
   }daily[31];
}MCF20, *pMCF20;



/* device control file */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
   }primary_key;

   BYTE  short_name                                [10];
   BYTE  phone                                     [18];
   BYTE  industry_code                             [4];
   BYTE  category_code                             [5];
   BYTE  contact                                   [21];
   BYTE  device_type                               [3];
   BYTE  date_added                                [9];
   BYTE  current_bus_date                          [9];
   BYTE  dept_nbr                                  [3];
   BYTE  organization_id                           [5];
   BYTE  merchant_id                               [16];
   BYTE  aba_nbr                                   [10];
   BYTE  status                                    [2];
   BYTE  totals_ind                                [2];
   BYTE  statistics_ind                            [2];
   BYTE  valid_transactions                        [41];  /* 8583 table 'Y' or 'N' */
   BYTE  draft_capture_type                        [2];
   BYTE  amex_charge_desc                          [24]; /* AP FA, Equitable 01-14-99 name change */
   BYTE  diners_charge_desc                        [24]; /* AP FA, Equitable 01-14-99 */
   BYTE  additional_pos_information                [7];
   BYTE  batch_nbr                                 [7];
   BYTE  rental_program                            [4];
   BYTE  nsp_process                               [16];
   BYTE  pin_type                                  [2];
   BYTE  kc                                        [17];
   BYTE  encr_type                                 [2];
   BYTE  pad_char                                  [2];
   BYTE  keyxchange_cnt                            [4];
   BYTE  txn_cnt                                   [4];
   BYTE  cwk                                       [17];
   BYTE  kek                                       [17];
   BYTE  retired_cwk                               [17];
   BYTE  icv                                       [17];

   BYTE  pin_block_format                          [2];
   BYTE  encryption_method                         [2];
   BYTE  pin_key                                   [17];
   BYTE  communication_key                         [17];

   BYTE  currency_code							   [4];
   BYTE  branch_code							   [4];

}DCF01, *pDCF01;

/* batch header file */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      
   }primary_key;

   BYTE  open_date                                 [9];
   BYTE  open_time                                 [7];
   BYTE  merchant_id                               [16];
   BYTE  organization_id                           [5];
   BYTE  total_credit_cnt                          [5];
   BYTE  total_credit_amt                          [13];
   BYTE  total_credit_refund_cnt                   [5];
   BYTE  total_credit_refund_amt                   [13];
   BYTE  total_debit_cnt                           [5];
   BYTE  total_debit_amt                           [13];
   BYTE  total_debit_refund_cnt                    [5];
   BYTE  total_debit_refund_amt                    [13];
   BYTE  total_void_credit_cnt                     [5];
   BYTE  total_void_credit_amt                     [13];
   BYTE  total_void_credit_refund_cnt              [5];
   BYTE  total_void_credit_refund_amt              [13];
   BYTE  release_date                              [9];
   BYTE  release_time                              [7];
   BYTE  batch_status                              [2];      /*R-released;C-closed;A-active*/
   BYTE  visa_merchant_id                          [16];

}BCH01, *pBCH01;

/* batch items record */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      BYTE  retrieval_ref_num                      [13];
   }primary_key;


   BYTE  open_date                                   [9];
   BYTE  open_time                                   [7];

   BYTE  merchant_id                               [16];
   BYTE  organization_id                           [5];
   BYTE  transaction_id                            [21];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];
   BYTE  tran_amount                               [13];
  
   BYTE  message_type                              [5];
   BYTE  processing_code                           [7];
   BYTE  tx_key                                    ;
   BYTE  auth_number                               [7];
   BYTE  card_nbr                                  [20];
   BYTE  primary_acct_nbr                          [20];
  
   BYTE  invoice_nbr                               [9];
   BYTE  settle_file_prefix                        [7];
   BYTE  product_code                              [9];

   BYTE  sys_trace_audit_num                       [7];
   BYTE  bin_type                                   ;
   BYTE card_type             [5];
   BYTE card_family           [3];
   BYTE exp_date                                    [5];

   BYTE pos_entry_mode                             [5];
   BYTE profile                                    [3];

   BYTE currency_code          [4];
   BYTE host_currency_code    [4];
   BYTE conversion_rate       [10];
   BYTE settlement_date        [5];
   BYTE transmission_timestamp [11];
   BYTE mcard_banknet         [51];
   BYTE air_ticket_number     [16];
   BYTE batch_upload_flag      ;

}BCH10,BCH11, *pBCH10, *pBCH11;

/* 0100 transactions similar to bch10 */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      BYTE  retrieval_ref_num                      [13];
   }primary_key;

   BYTE  open_date                                   [9];
   BYTE  open_time                                   [7];

   BYTE  merchant_id                               [16];
   BYTE  organization_id                           [5];
   BYTE  transaction_id                            [21];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];
   BYTE  tran_amount                               [13];
  
   BYTE  message_type                              [5];
   BYTE  processing_code                           [7];
   BYTE  tx_key                                    ;
   BYTE  auth_number                               [7];
   BYTE  card_nbr                                  [20];
   BYTE  primary_acct_nbr                          [20];
  
   BYTE  invoice_nbr                               [9];
   BYTE  settle_file_prefix                        [7];
   BYTE  product_code                              [9];

   BYTE  sys_trace_audit_num                       [7];
   BYTE  bin_type                                  ;
   BYTE card_type             [5];
   BYTE card_family           [3];
   BYTE exp_date                                    [5];

   BYTE pos_entry_mode                             [5];
   BYTE profile                                    [3];

   BYTE currency_code          [4];
   BYTE host_currency_code    [4];
   BYTE conversion_rate       [10];
   BYTE settlement_date        [5];
   BYTE transmission_timestamp [11];
   BYTE mcard_banknet         [51];
   BYTE air_ticket_number     [16];
   BYTE  processed                                 ;

}BCH20, *pBCH20;

/* batch record, identical to BCH10 */

typedef struct
{
   struct
   {
      BYTE low                                     [17];
      BYTE high                                    [17];
   }primary_key;
} NBF01, *pNBF01;

typedef struct
{
   struct
   {
      BYTE negative_group_id                       [11];
   }primary_key;
   
   struct
   {
      BYTE low                                     [17];
      BYTE high                                    [17];
   }bin_range[10];                                          
}NGF01,  *pNGF01;

/* Negative File Info. */
typedef struct
{
   struct
   {
      BYTE  file_name                              [9];
   }primary_key;

   BYTE start_date                                 [9];
   BYTE end_date                                   [9];

}NFI01, *pNFI01;

/* Negative File Info. */
typedef struct
{
   struct
   {
      BYTE  file_name                              [51];
   }primary_key;

   BYTE start_date                                 [9];
   BYTE end_date                                   [9];

}NFI02, *pNFI02;

/* Negative Master File */
typedef struct
{
   struct
   {
      BYTE seq_nbr                                 [7];
   }primary_key;

   BYTE action_code                                [2];
   BYTE format_code                                [2];
   BYTE account_info                               [20];
   BYTE file_name                                  [9];

}NMF01, *pNMF01;

/* Negative Temp File */
typedef struct
{
   struct
   {
     BYTE account_info                             [20];
   }primary_key;

   BYTE format_code                                [2];
   BYTE file_name                                  [9];

}NTF01, *pNTF01;

/* Structure to populate values to retrieve nmf01_list */

typedef struct
{
   BYTE  term_id                                   [9];
   BYTE  merch_id                                  [16];
   BYTE  file_name                                 [11];
   BYTE  seq_nbr                                   [7];
}NMF01_REQUEST, *pNMF01_REQUEST;

/* structure to Download Report Activity  */

typedef struct
{
   BYTE  device_id                                 [9];
   BYTE  merchant_id                               [16];
   BYTE  date_of_request                           [9];
   BYTE  time_of_request                           [7];
   BYTE  request_approved                          [2];
   BYTE  filename_downloaded                       [9];
   BYTE  download_completed                        [2];
}DRF01,  *pDRF01;

/* structure to facilitate Download Report Activity  In addition to DRF01*/

typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
   }primary_key;

   BYTE  merchant_id                               [16];
   BYTE  last_downloaded_filename                  [9];
   BYTE  last_downloaded_date                      [9];
}DLF01,  *pDLF01;



typedef struct
{
   BYTE  num_returned     [5];
   DCF01 dcf01_record     [GUI_MAX_LIST_SIZE];
}DCF01_GUI_LIST, *pDCF01_GUI_LIST;



typedef struct
{
   BYTE  num_returned     [5];
   DCF01 device_list      [DCF01_SERVICE_LIST_SIZE];
}DCF01_LIST, *pDCF01_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   BCH01 batch_record     [GUI_MAX_LIST_SIZE];
}BCH01_GUI_LIST, *pBCH01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BCH10 batch_list       [GUI_MAX_LIST_SIZE];
}BCH10_GUI_LIST, *pBCH10_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BCH11 batch_list       [GUI_MAX_LIST_SIZE];
}BCH11_GUI_LIST, *pBCH11_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BCH10 batch_list       [BATCH_SERVICE_LIST_SIZE];
}BCH10_LIST, *pBCH10_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   BCH11 batch_list       [BATCH_SERVICE_LIST_SIZE];
}BCH11_LIST, *pBCH11_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BCH01 batch_list       [SCH_MAX_LIST_SIZE];
}BCH01_LIST, *pBCH01_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   NBF01 nbf01_record     [SERVICE_LIST_SIZE];
}NBF01_LIST, *pNBF01_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   NBF01 nbf01_record   [GUI_MAX_LIST_SIZE];
}NBF01_GUI_LIST, *pNBF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   NGF01 ngf01_record   [GUI_MAX_LIST_SIZE];
}NGF01_GUI_LIST, *pNGF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE negative_group_id   [GUI_MAX_LIST_SIZE][11];
}NGF01_LIST, *pNGF01_LIST;


typedef struct
{
   BYTE   num_returned     [5];
   NFI01  nfi01_rec        [SERVICE_LIST_SIZE];
}NFI01_LIST, *pNFI01_LIST;


typedef struct
{
   BYTE  num_returned             [5];
   NMF01  nmf01_record             [GUI_MAX_LIST_SIZE];
}NMF01_LIST, *pNMF01_LIST;

typedef struct
{
   NGF01     ngf01_rec;
   struct  
   {
      BYTE   num_returned             [5];
      NMF01  nmf01_record             [SERVICE_LIST_SIZE];
   }nmf01_serv_list;

}NMF01_REPLY, *pNMF01_REPLY;


#endif