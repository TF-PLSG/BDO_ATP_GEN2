
/******************************************************************************

     Module:               DC_DBSTRUCT.H
  
     Title:                ATP Database Structures

     Description: 

     Application:          Equitable

     Authours:             
   
     Revision History:
   
     $Log:   N:\PVCS\PTE\Equitable\App_lib\dc_dbstruct.h  $
   
      Rev 1.15   Jul 07 2004 13:54:20   dirby
   Added the Auto Init fields to the DCF01 data structure.
   SCR 1287
   
      Rev 1.14   Feb 14 2003 16:32:48   dirby
   Increased field size of cwk and retired_cwk from 17 to 49.
   SCR 955
   
      Rev 1.13   Nov 01 2001 15:17:26   dirby
   Added 'def_gross_amt' and 'term' to BCH10/11/20.  This is for RSB cards.     SCR 256
   
      Rev 1.12   Jun 13 2001 11:56:48   dirby
   Removed the changes from the 2 previous version.
   Basically, to this file back to version 1.9 so we can
   compile the Equitable project without some modifications
   made for the Base product.
   
      Rev 1.11   Dec 01 2000 18:37:54   rmalathk
   updated for Realpay: UCF03, UCF03L, PCID, TLF01 and batch tables.
   

      Rev 1.10   Nov 29 2000 10:46:48   apinto

   Added/Removed fields for the new product...A.Pinto 11.29.00

   

      Rev 1.9   Aug 01 2000 08:53:44   dirby
   Increased size of Odometer field in BCH10/11/20 from 6 to 7.
   
      Rev 1.8   Apr 17 2000 11:12:52   iarustam
    
   
      Rev 1.6   Jan 18 2000 16:35:34   ddabberu
   removed c++ style comments
   
      Rev 1.5   Nov 19 1999 16:43:04   blee1
   1) BCH10 & BCH11:
   Add TRANSMISSION_TIMESTAMP [11]
          MCARD_BANKNET                  [51]
   
   2) BCH20
   Make identical to BCH10 and BCH11 with one exception:
   Add PROCESSED [2] as last field.
   
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
   BYTE  cwk                                       [49];
   BYTE  kek                                       [17];
   BYTE  retired_cwk                               [49];
   BYTE  icv                                       [17];

   BYTE  pin_block_format                          [2];
   BYTE  encryption_method                         [2];
   BYTE  pin_key                                   [17];
   BYTE  communication_key                         [17];

   /* Auto Init Fields */
   BYTE  auto_init_activation                      [2];
   BYTE  auto_init_time                            [6]; /* HH:MM */
   BYTE  auto_init_ref_number                      [7];
   BYTE  auto_program_load_activation              [2];
   BYTE  auto_program_load_time                    [6]; /* HH:MM */
   BYTE  auto_program_load_ref_number              [7];
   BYTE  auto_icepac_activation                    [2];
   /* End Auto Init Fields */

}DCF01, *pDCF01;

/* batch header file */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      
   }primary_key;

   BYTE  merchant_id                               [16];
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
   BYTE  open_date                                 [9];
   BYTE  open_time                                 [7];
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

   BYTE  merchant_id                               [16];
   BYTE  processing_code                           [7];
   BYTE  tran_amount                               [13];
   BYTE  response_code                             [3];
   BYTE  auth_number                               [7];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];
   BYTE  transaction_id                            [21];
   BYTE  message_type                              [5];
   BYTE  card_nbr                                  [20];
   BYTE  invoice_nbr                               [9];
   BYTE  settle_file_prefix                        [7];
   BYTE  product_code                              [9];
   BYTE  odometer                                  [8];
   BYTE  sys_trace_audit_num                       [7];
   BYTE  tx_key                                    ;
   BYTE  bin_type                                  ;

   BYTE type_of_data                               [3];
   BYTE down_payment                               [13];
   BYTE period                                     [3];
   BYTE interest_rate                              [5];
   BYTE total_interest                             [13];
   BYTE redemption_amount                          [13];
   BYTE vehicle_number                             [9];
   BYTE pos_entry_mode                             [5];
   BYTE profile                                    [3];
   BYTE nbr_of_prod_codes                          [3];
   struct
   {
      BYTE code                                    [7];
      BYTE quantity                                [3];
      BYTE amount                                  [13];
   }product_codes[20];
   
   BYTE aportionment_flag                          [2];
   BYTE exp_date                                   [5];
   BYTE  open_date                                 [9];
   BYTE  open_time                                 [7];
   BYTE  cash_bonus                                [2];          /* AP SD, Equitable 01-13-99 */
   BYTE  transmission_timestamp                    [11];
   BYTE  mcard_banknet                             [51];
   BYTE ticket_nbr                                 [16];  /* IA 04-11-00 for Offus velocity */
   BYTE def_gross_amt                              [13];
   BYTE term                                       [4];
  
}BCH10, *pBCH10;

/* 0100 transactions similar to bch10 */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      BYTE  retrieval_ref_num                      [13];
   }primary_key;

   BYTE  merchant_id                               [16];
   BYTE  processing_code                           [7];
   BYTE  tran_amount                               [13];
   BYTE  response_code                             [3];
   BYTE  auth_number                               [7];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];
   BYTE  transaction_id                            [21];
   BYTE  message_type                              [5];
   BYTE  card_nbr                                  [20];
   BYTE  invoice_nbr                               [9];
   BYTE  settle_file_prefix                        [7];
   BYTE  product_code                              [9];
   BYTE  odometer                                  [8];
   BYTE  sys_trace_audit_num                       [7];
   BYTE  tx_key                                    ;
   BYTE  bin_type                                  ;
 
   BYTE type_of_data                               [3];
   BYTE down_payment                               [13];
   BYTE period                                     [3];
   BYTE interest_rate                              [5];
   BYTE total_interest                             [13];
   BYTE redemption_amount                          [13];
   BYTE vehicle_number                             [9];
   BYTE pos_entry_mode                             [5];
   BYTE profile                                    [3];
   BYTE nbr_of_prod_codes                          [3];
   struct
   {
      BYTE code                                    [7];
      BYTE quantity                                [3];
      BYTE amount                                  [13];
   }product_codes[20];
   
   BYTE aportionment_flag                          [2];
   BYTE exp_date                                   [5];
   BYTE  open_date                                 [9];
   BYTE  open_time                                 [7];
   BYTE  cash_bonus                                [2];                 /* AP SD, Equitable 01-13-99 */
   BYTE  transmission_timestamp                    [11];
   BYTE  mcard_banknet                             [51];
   BYTE  processed                                 [2];
   BYTE ticket_nbr                                 [16];  /* IA 04-11-00 for Offus velocity */
   BYTE def_gross_amt                              [13];
   BYTE term                                       [4];

}BCH20, *pBCH20;

/* batch record, identical to BCH10 */
typedef struct
{
   struct
   {
      BYTE  device_id                              [9];
      BYTE  batch_nbr                              [7];
      BYTE  retrieval_ref_num                      [13];
   }primary_key;

   BYTE  merchant_id                               [16];
   BYTE  processing_code                           [7];
   BYTE  tran_amount                               [13];
   BYTE  response_code                             [3];
   BYTE  auth_number                               [7];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];
   BYTE  transaction_id                            [21];
   BYTE  message_type                              [5];
   BYTE  card_nbr                                  [20];
   BYTE  invoice_nbr                               [9];
   BYTE  settle_file_prefix                        [7];
   BYTE  product_code                              [9];
   BYTE  odometer                                  [8];
   BYTE  sys_trace_audit_num                       [7];
   BYTE  tx_key                                    ;
   BYTE  bin_type                                  ;
 
   BYTE type_of_data                               [3];
   BYTE down_payment                               [13];
   BYTE period                                     [3];
   BYTE interest_rate                              [5];
   BYTE total_interest                             [13];
   BYTE redemption_amount                          [13];
   BYTE vehicle_number                             [9];
   BYTE pos_entry_mode                             [5];
   BYTE profile                                    [3];
   BYTE nbr_of_prod_codes                          [3];
   struct
   {
      BYTE code                                    [7];
      BYTE quantity                                [3];
      BYTE amount                                  [13];
   }product_codes[20];
   
   BYTE aportionment_flag                          [2];
   BYTE exp_date                                   [5];
   BYTE  open_date                                 [9];
   BYTE  open_time                                 [7];
   BYTE  cash_bonus                                [2];                 /* AP SD, Equitable 01-13-99 */
   BYTE  transmission_timestamp                    [11];
   BYTE  mcard_banknet                             [51];
   BYTE ticket_nbr                                 [16];  /* IA 04-11-00 for Offus velocity */
   BYTE def_gross_amt                              [13];
   BYTE term                                       [4];

}BCH11, *pBCH11;


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

typedef struct
{
	 BYTE transaction_id                     [21];
	 BYTE system_date                        [9];
	 BYTE transaction_send_time				 [18];
	 BYTE mracon_sender_name				 [7];
	 BYTE transaction_number				 [21];
	 BYTE future_1						 	 [51];
	 BYTE future_2						 	 [51];
	 BYTE future_3						 	 [51];
	 BYTE future_4						 	 [51];
	 BYTE future_5						 	 [51];
	 BYTE future_use_1						 	 [101];
	 BYTE future_use_2						 	 [101];
	 BYTE future_use_3						 	 [101];
	 BYTE future_use_4						 	 [101];
	 BYTE future_use_5						 	 [101];

}MRA01,*pMRA01;

#endif
