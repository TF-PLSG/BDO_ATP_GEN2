
/******************************************************************************

     Module:               NC_DBSTRUCT.H
  
     Title:                ATP Database Structures

     Description: 

     Application:          ATP

     Authors:              Last modified by Dirby.

     Revision History:     1.0
   
     $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\nc_dbstruct.h  $
   
      Rev 1.27   Jun 22 2005 14:35:58   dirby
   Additions to support Fraudguard
   SCR 14421
   
      Rev 1.26   Nov 17 2003 12:03:06   dirby
   Added data structure AMEX_ONLINE_UPDATES to be used with
   1302/1312 transactions initiated at the GUI and sent to Amex
   through ncamaq.
   SCR 1200
   
      Rev 1.25   Oct 10 2003 08:40:54   dirby
   Changed number_of_regions from INT to CHAR in the
   MCC_ONLINE_UPDATES structure.  This is to avoid issues
   when the Windows GUI sends the structure to the AIX server.
   I was seeing a strange conversion of integer values, preventing
   inserts into TLF01.
   SCR 1224
   
      Rev 1.24   Oct 08 2003 17:19:40   dirby
   Added terminal ID to the MasterCard 0302 structure.  This is so
   it can be logged into TLF01 by ncmcrd.
   SCR 1224
   
      Rev 1.23   Mar 03 2003 11:06:24   dirby
   Merged the 2 MCC Gui structures into a single structure.  Turns
   out that most of the request information is echoed in the response.
   SCR 891
   
      Rev 1.22   Feb 14 2003 16:40:14   dirby
   Increased the length of fields cwk and retired_cwk from 16 to 49.
   Increased the length of field zmk from 33 to 49.
   SCR 955
   
      Rev 1.21   Jul 29 2002 16:44:36   dirby
   Added Operator ID and Workstation ID to the MasterCard structure
   for Online Updates.  This is so they get logged into TLF01.
   SCR 575
   
      Rev 1.20   Jan 09 2002 17:35:44   dirby
   Added operator_id and workstation to JCB_ONLINE_UPDATES structure, so they
   get logged into TLF01.  This affects ncjcb and ATP.exe.
   SCR 574
   
      Rev 1.19   Nov 02 2001 13:07:06   dirby
   Added a data structure used for communication between GUI
   and the JCB Network Controller.   Used for JCB Online File
   Updates.    SCR 251
   
      Rev 1.18   Nov 01 2001 15:19:18   dirby
   Added 'neg_access_code' to NCF01.  This is in support of JCB Online File Updates.     SCR 251
   
      Rev 1.17   Jun 13 2001 12:08:22   dirby
   Reverted back to revision 1.15.  Revision 1.16 is not
   compatible with the current Equitable product.
   
      Rev 1.16   Nov 30 2000 15:20:16   apinto
   Changed SAF01 to be same as TLF01.
   
      Rev 1.15   Feb 14 2000 15:50:30   iarustam
   added new field to the mcc_gui_imput
   
      Rev 1.13   23 Aug 1999 17:02:00   dirby
   Added tran_fee_amount to NCF30.
   
      Rev 1.12   20 Aug 1999 13:45:20   dirby
   Added reversal amount to SAF01.
   
      Rev 1.11   18 Aug 1999 14:54:44   dirby
   Added acquiring_inst_id_code and forwarding_inst_id_code to SAF01.
   
      Rev 1.10   18 Aug 1999 13:57:44   dirby
   Added currency_code, acquiring_inst_id_code,
   settlement_date, and transmission_timestamp
   to NCF30.
   
      Rev 1.9   17 Aug 1999 10:20:06   dirby
   Added SAF01 data structure for Store and Forward processing.
   
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
   BYTE  cwk                                       [49];
   BYTE  kek                                       [17];
   BYTE  retired_cwk                               [49];
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
   BYTE zmk                                        [49];
   BYTE  neg_access_code                           [7];

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
   BYTE  currency_code               [4];
   BYTE  acquiring_inst_id_code     [12];
   BYTE  settlement_date             [5];
   BYTE  transmission_timestamp     [11];
   BYTE  tran_fee_amount            [10];

} NCF30, *pNCF30;

typedef struct
{
   BYTE  num_returned             [5];
   NCF30 batch_list               [BATCH_SERVICE_LIST_SIZE];

} NCF30_BATCH_LIST, *pNCF30_BATCH_LIST;


typedef struct
{
	char 	network_id            [11];
	char 	iss_cwk_keyblock	  [129];
	char 	iss_zmk_keyblock      [129];
	char 	iss_rtdcwk_keyblock   [129];
	char 	iss_kek_keyblock      [129];
	char 	acq_cwk_keyblock	  [129];
	char 	acq_zmk_keyblock      [129];
	char 	acq_kek_keyblock      [129];
	char 	iss_prev_src_station  [129];
	char 	future_field1		  [129];
	char 	future_field2		  [129];
	char 	future_field3		  [129];
	char 	future_field4		  [129];
	char 	future_field5		  [129];

}NCF01_KEYBLOCK, *pNCF01_KEYBLOCK;

typedef struct
{
   BYTE  			num_returned				[5];
   NCF01_KEYBLOCK	ncf01_keyblock_record		[GUI_MAX_LIST_SIZE];

} NCF01_KEYBLOCK_GUI_LIST, *pNCF01_KEYBLOCK_GUI_LIST;


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


/* Store and Forward table */
typedef struct
{
   struct
   {
      BYTE  transaction_id          [21];
   }primary_key;

   BYTE  card_num                   [20];
   BYTE  tran_amount                [13];
   BYTE  device_id                   [9];
   BYTE  message_type                [5];
   BYTE  processing_code             [7];
   BYTE  sys_trace_audit_nbr         [7];
   BYTE  tran_date                   [9];
   BYTE  tran_time                   [7];
   BYTE  tx_key                         ;
   BYTE  retrieval_ref_num          [13];
   BYTE  merchant_id                [16];
   BYTE  auth_number                 [7];
   BYTE  response_code               [3];
   BYTE  invoice_nbr                 [9];
   BYTE  network_data               [52];
   BYTE  currency_code               [4];
   BYTE  transmission_timestamp     [11];
   BYTE  exp_date                    [5];
   BYTE  settlement_amount          [13];
   BYTE  settlement_date             [5];
   BYTE  settlement_conv_rate        [9];
   BYTE  settlement_curr_code        [4];
   BYTE  billing_amount             [13];
   BYTE  billing_conv_rate           [9];
   BYTE  billing_curr_code           [4];
   BYTE  tran_fee_amount             [9];
   BYTE  handler_queue               [8];
   BYTE  acquiring_inst_id_code     [12];
   BYTE  forwarding_inst_id_code    [12];
   BYTE  reversal_amount            [13];

} SAF01, *pSAF01;


/* Structures and definitions to communicate information
 * between GUI and NCMCRD for file update requests.
 */
#define CHOICE_ADD     'A'
#define CHOICE_UPDATE  'U'
#define CHOICE_DELETE  'D'
#define CHOICE_INQUIRE 'I'
#define MCC_ERR_LINE_LEN 80

typedef struct
{
   BYTE  indicator;
   CHAR  purge_date      [7];
   CHAR  chargeback_date [7];

} MCC_REGION, *pMCC_REGION;

typedef struct
{
   CHAR        msg_type            [7];
   BYTE        choice;
   CHAR        card_nbr           [20];
   BYTE        reason;
   CHAR        product_code        [4];
   CHAR        activity_date       [7];
   CHAR        activity_time       [5];
   CHAR        vip_limit_amt      [13];
   CHAR        vip_currency_code   [4];
   CHAR        number_of_regions   [3];
   MCC_REGION  region_us;
   MCC_REGION  region_canada;
   MCC_REGION  region_sa;
   MCC_REGION  region_asia;
   MCC_REGION  region_europe;
   MCC_REGION  region_africa;
   BYTE        terminal_id         [9];
   BYTE        originator_queue   [17];
   BYTE        originator_info    [33];
   BYTE        operator_id        [21];
   BYTE        workstation        [21];
   CHAR        response_code       [3];
   CHAR        err_msg1           [MCC_ERR_LINE_LEN];
   CHAR        err_msg2           [MCC_ERR_LINE_LEN];
   CHAR        err_msg3           [MCC_ERR_LINE_LEN];

} MCC_ONLINE_UPDATES, *pMCC_ONLINE_UPDATES;

/* Structure to communicate information between GUI and NCJCB
 * for file update requests.
 */
typedef struct
{
   BYTE  card_nbr[20];
   BYTE  txn_type;
   BYTE  action_code[3];
   BYTE  purge_date[7];
   BYTE  stop_regions[6];
   BYTE  resp_code[3];
   BYTE  resp_text[40];
   BYTE  orig_queue[17];
   BYTE  orig_info[33];
   BYTE  terminal_id[9];
   BYTE  operator_id[21];
   BYTE  workstation[21];

} JCB_ONLINE_UPDATES, *pJCB_ONLINE_UPDATES;


/* Structure to communicate information between
 * GUI and NCAMAQ for file update requests.
 */
typedef struct
{
   BYTE  card_nbr[20];
   BYTE  exp_date[5];
   BYTE  function_code[4];
   BYTE  reason_code[5];
   BYTE  resp_code[4];
   BYTE  resp_text[60];
   BYTE  stan[7];
   BYTE  orig_queue[17];
   BYTE  orig_info[33];
   BYTE  terminal_id[9];
   BYTE  operator_id[21];
   BYTE  workstation[21];
   BYTE  fwd_inst_code[12];
   BYTE  date_time[15];     /* YYYYMMDDhhmmss */
} AMEX_ONLINE_UPDATES, *pAMEX_ONLINE_UPDATES;


/* ----------------------------------- */
/* Fraudguard GUI Configuration Record */
/* ----------------------------------- */
typedef struct
{
   BYTE  name           [11];
   BYTE  type            [2];
   BYTE  caller_id       [6];
   BYTE  source         [31];
   BYTE  username       [31];
   BYTE  application    [21];
   BYTE  rule_domain    [81];
   BYTE  product        [31];
   BYTE  priority        [5];
   BYTE  tpdu_id        [12];
   BYTE  queue_name     [16];
   BYTE  request_timer   [4];
   BYTE  echo_timer      [4];

} FG01, *pFG01;


/* ------------------- */
/* Fraudguard GUI List */
/* ------------------- */
typedef struct
{
   BYTE  num_returned   [5];
   FG01  FG01_record    [GUI_MAX_LIST_SIZE];

} FG01_GUI_LIST, *pFG01_GUI_LIST;

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
	 BYTE date_yyyymmdd							 [9];
}MRA01,*pMRA01;

typedef struct
{
	BYTE on_behalf_service[3];
	BYTE on_behalf_result [2];
	BYTE description      [41];
	BYTE response_code    [3];
}OBS01,*pOBS01;

typedef struct
{
   BYTE  num_returned             [5];
   OBS01 obs01_record             [GUI_MAX_LIST_SIZE];

} OBS01_GUI_LIST, *pOBS01_GUI_LIST;

typedef struct
{

	BYTE cavv 			  [2];
	BYTE description      [41];
	BYTE response_code    [3];
}VISA3DS2,*pVISA3DS2;

typedef struct
{
   BYTE  num_returned             [5];
   VISA3DS2 visa3ds2_record       [GUI_MAX_LIST_SIZE];

} VISA3DS2_GUI_LIST, *pVISA3DS2_GUI_LIST;
#endif
