
/******************************************************************************

     Module:               EQUITDB.H
  
     Title:                ATP Database Structures for Equitable

     Description: 

     Application:          Equitable

     Authours:             Jimmy H
   
     Revision History:     1.0
   
     $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Equitdb.h  $
   
      Rev 1.80   Mar 01 2007 15:39:46   DIrby
   Increased size of SQL01.sqltext from 200 to 400 to accomodate
   repds SQLs.
   SCR 24081
   
      Rev 1.79   Feb 13 2007 08:46:30   DIrby
   Added structure SQL01 for a shared memory table that will hold
   queries being performed by data servers.
   SCR 24081
   
      Rev 1.78   Jan 16 2007 10:29:32   DIrby
   Added field "Multi_Winners_Allowed" to BON02.
   SCR 24046
   
      Rev 1.77   Aug 30 2006 11:10:44   dirby
   Added tran_time to BON03 structure.
   SCR 22835
   
      Rev 1.76   Aug 29 2006 11:46:48   dirby
   Increased size of Cash Bonus Program description from 40 to 120.
   SCR 22835
   
      Rev 1.75   Aug 28 2006 11:49:44   dirby
   Enlarged response text field size for BON02 and BON03 from
   40 to 120 to allow for longer messages on the terminal.  Also
   added a field to contain a text message for cardholders who
   win multiple promos on the same transaction.
   SCR 22835
   
      Rev 1.74   Aug 24 2006 10:51:58   dirby
   Added data structures to support database tables Bon02 and Bon03.
   SCR 22835
   
      Rev 1.73   May 03 2006 16:57:40   dirby
   Moved the EMV_RAW structure here from updatds and repds.
   
      Rev 1.72   Jul 07 2005 11:18:20   dirby
   Changed the size of Fguard Gui List from 50 records to 10.
   TranLookup brings them back 10 at a time.  Also added GUI List
   structure for FG_RAW for batch resends.
   SCR 14421
   
      Rev 1.71   Jun 22 2005 14:35:58   dirby
   Additions to support Fraudguard
   SCR 14421
   
      Rev 1.70   Apr 01 2005 10:54:36   lmitchel
   4_4_1: updates to support countr/currency code limits, unlimited purchase card MCC ranges
   
      Rev 1.69   Mar 10 2005 09:55:40   lmitchel
   Added Country Amt Limits tables structure
   
      Rev 1.68   Jul 13 2004 14:23:28   dirby
   In the EMV structure for auth_tx, renamed the category code field
   to tcc (for transaction category code - tag 9F53).  Also added
   field mcc (for merchant category code - tag 9F15).
   SCR 1287
   
      Rev 1.67   Jul 07 2004 15:57:14   dirby
   Added field 'reload_merchant' to MCF01 - SCR 1287
   Added new EMV structure to Auth_Tx - SCR 1388
   
      Rev 1.66   May 28 2004 13:55:30   dirby
   Removed the MCC fields from the CAF01 data structure.
   SCR 1380
   
      Rev 1.65   Feb 19 2003 13:53:04   dirby
   Changed the number of MCL substructures in the CAF01
   structure from 20 to 50.
   SCR 614
   
      Rev 1.64   Feb 14 2003 16:36:48   dirby
   Increased field length of source_key, dest_key, dcf01_retired_cwk,
   and ncf01_retired_cwk from 17 to 49.
   SCR 955
   
      Rev 1.63   Sep 12 2002 15:49:22   dirby
   Added BON01 list structure for shared memory.
   SCR 804
   
      Rev 1.62   Aug 16 2002 16:29:26   dirby
   Added the following fields to the merchant structure, MCF01:
      deferred_start_date
      deferred_end_date
      group_outlet_code
      group_type
   These are used to support online deferred transactions.
   SCR 255
   
      Rev 1.61   Nov 01 2001 15:22:20   dirby
   Added 'def_gross_amt' and 'monthly_amort_amt' to TLF01.
   Added 'add_on_rate' and 'add_on_term' to ccf02p.
   These are to support RSB Guarantor cards.     SCR 256
   
      Rev 1.60   Oct 16 2001 10:44:24   dirby
   Added fields to BIN01 for a Mod-10 check flag and to block a
   bin based on an range.  SCRs 403 and 361
   
      Rev 1.59   Jun 13 2001 12:04:32   dirby
   Removed Base Product modifications made in
   revisions 1.55 - 1.57  This reverts up back to revision
   1.54, plus the changes made in 1.58  This is done so
   we can compile and use this module in the Equitable
   project, without upgrading to Base product.
   
      Rev 1.58   Jun 13 2001 09:02:08   SYARLAGA
   Added system_date in TLF01_LIST_STRUCTURE
   
      Rev 1.57   Dec 01 2000 18:37:54   rmalathk
   updated for Realpay: UCF03, UCF03L, PCID, TLF01 and batch tables.
   
      Rev 1.55   Nov 29 2000 10:46:58   apinto
   Added/Removed fields for the new product...A.Pinto 11.29.00
   
      Rev 1.54   Jul 31 2000 16:42:02   dirby
   Added odometer and currency code to TLF01_List structure.
   
      Rev 1.53   Jul 31 2000 16:17:54   dirby
   Increased size of ODOMETER in TLF01 from 6 to 7 to match size in
   incoming transaction.
   
   Added system_date to TLF01.  This date should reflect the date a
   transaction is stored into TLF01; this is not necessarily the txn date.
   
      Rev 1.52   Jul 18 2000 12:39:00   dirby
   Added pos_condition_code to TLF01_List_Structure.
   This is for the log_file application; needs it to determine
   a sales_completion.
   
      Rev 1.51   Apr 17 2000 11:12:52   iarustam
    
   
      Rev 1.49   Mar 21 2000 12:21:14   iarustam
   changed the size of report lists from 10 to 20
   
      Rev 1.48   Mar 20 2000 16:50:34   ddabberu
   same commenting problem!!!
   
      Rev 1.47   Mar 15 2000 17:47:16   iarustam
   added ccf01_report_list and paf01_report_list
   
   
      Rev 1.46   Mar 15 2000 12:22:36   iarustam
   added mcf01_report_list for alpha lookup gui
   
      Rev 1.45   Mar 15 2000 11:24:58   iarustam
   changed invoice_number to card_holder_name in the TLF01_LIST_STRUCTURE
   
      Rev 1.43   Dec 20 1999 11:01:34   mbysani
   Decreased the TLF01 Gui list array size
   
      Rev 1.42   Dec 17 1999 15:24:10   dperson
   Added retrieval_ref_num to TLF01_LIST_STRUCTURE
   
   
      Rev 1.41   Nov 17 1999 10:24:04   blee1
   Added CRF01.floor_limit
   
      Rev 1.40   11 Jun 1999 08:41:18   dirby
   Added 'settlement_conv_rate' to auth_tx.
   
      Rev 1.39   May 24 1999 06:36:22   dperson
   Added reversal_amount to tlf01_list
   
      Rev 1.38   May 22 1999 10:55:46   egable
   Increased the size of the source and dest
   station ids in auth_tx.
   
      Rev 1.37   14 May 1999 11:46:10   epriyev
   reinsert previous change from ver. 1.34
   
      Rev 1.36   May 11 1999 15:18:00   egable
   Add incoming_dest_station_id and 
   incoming_src_station_id for Visa network
   handler.
   
      Rev 1.35   May 07 1999 17:30:14   sdasgupt
   Changed the order of cvv_processing field in caf01 to mirror the database as well as db_caf01 in tx_database.pc
   
      Rev 1.33   Apr 15 1999 13:41:26   iarustam
   Changed cvv_length from CAF01 table to 3
   
      Rev 1.32   Apr 12 1999 15:39:22   iarustam
   Changed for tlf01_limits
   
      Rev 1.31   Apr 12 1999 10:50:40   iarustam
   Added reversal_amount ot tlf01_limits
   
      Rev 1.30   Apr 08 1999 10:55:38   iarustam
   Added new field to TLF01 ( orig_auth_number )
   
      Rev 1.29   Mar 30 1999 11:29:26   iarustam
   Added tlf01_limits structure for transaction lookup GUI
   
      Rev 1.28   Mar 22 1999 14:35:40   iarustam
   Added tlf01_gui_list
   
      Rev 1.27   17 Mar 1999 11:44:32   dirby
   Modified the following string lengths in auth_tx:
      acq_inst_id_cd
      forwarding_inst_id_cd
     receiving_inst_id_code
   to all be length of 12.  They were previously various lengths.
   
      Rev 1.26   Mar 16 1999 12:16:42   iarustam
   Added workstation_id to TLF01 and
   voice_category_code to MCF01
   
      Rev 1.25   Mar 16 1999 10:37:50   iarustam
   Added new fields to CAF01
   
      Rev 1.24   Mar 15 1999 08:40:44   iarustam
   Added pvki_location_tr1, pvki_location_tr2,
             pvki_length_tr1, pvki_length_tr2 to PIN_DATA
   
   
      Rev 1.23   Mar 09 1999 13:54:30   iarustam
   Added operator_id field  to tlf01 table
   
      Rev 1.22   Mar 02 1999 15:35:32   egable
   Add host_processing_code to auth_tx.
   
   
      Rev 1.21   Mar 01 1999 16:44:34   egable
   Add length fields to auth_tx for the institution
   id codes.
   
      Rev 1.20   Feb 22 1999 12:58:06   IGOLDFIE
   Added voice_auth_text to TLF01
   
      Rev 1.19   Feb 22 1999 10:53:32   MSALEH
   Remove voice_auth_text from AUTH_TX
   to be placed in tlf01
   
      Rev 1.18   Feb 22 1999 09:11:50   MSALEH
   Added voice_auth_text to AUTH_TX for 
   voice auth gui
   
      Rev 1.17   Feb 19 1999 16:53:16   egable
   Add amex_pos_entry_mode
   
      Rev 1.16   Feb 18 1999 14:41:02   egable
   Add to AUTH_TX, orig_local_time_date and 
   orig_acq_inst_id.
   
      Rev 1.15   Feb 17 1999 16:08:34   IGOLDFIE
   Changed the size of 2 fields in TLF01
   
      Rev 1.14   Feb 17 1999 14:23:00   sdasgupt
   Changed the size of pvki from 2 to 3
   
      Rev 1.13   Feb 17 1999 08:36:34   iarustam
   GUI changes
   
      Rev 1.12   Feb 16 1999 18:24:16   iarustam
   Added acf01_gui_list and 
              ccf04_gui_list
   
      Rev 1.11   Feb 16 1999 14:44:58   MSALEH
   added host2_acquirer_cb_code[3] to auth_Tx
   
      Rev 1.10   Feb 15 1999 14:41:56   sdasgupt
   Added pvki to ccf02p
   Added csck to caf01
   
      Rev 1.9   15 Feb 1999 11:04:22   rmalathk
   Added the following fields to auth_tx:
   src_fi_nbr, src_terminal_nbr, src_seq_nbr
   and message_seq_nbr.
   
      Rev 1.8   Feb 10 1999 16:14:26   IGOLDFIE
   Added actual_amount, reversal_amount to TLF01,
   removed reversal_amount from AUTH_TX
   
      Rev 1.7   Feb 09 1999 17:18:36   IGOLDFIE
   Added 7 new fields to TLF01, changed 3.
   
      Rev 1.6   Feb 09 1999 15:22:40   egable
   Add to auth_tx: reconciliation_amount, 
   local_date_time, function_code, accptor_bus_code,
   reconciliation_cur_code for AMEX.
   
      Rev 1.5   04 Feb 1999 16:59:26   jhapitas
   Added variables to the auth_tx table.
   
      Rev 1.4   Feb 04 1999 10:10:10   IGOLDFIE
   Added TLF01_LIST_STRUCTURE.
   
      Rev 1.3   Feb 03 1999 15:22:48   IGOLDFIE
   Added TLF01_list
   
      Rev 1.2   Feb 03 1999 14:43:16   sdasgupt
   Added #defines for pin verification methods
   
   
      Rev 1.1   03 Feb 1999 10:52:14   jhapitas
   Added new fields
   
      Rev 1.0   01 Feb 1999 14:42:48   jhapitas
   Initial Revision
   
      
   initial release
******************************************************************************/

#ifndef EQUITDB_H
#define EQUITDB_H

#include "basictyp.h"


typedef union 
{
   struct
   {
      BYTE  bankid                              [9];
      BYTE  pvn_location                        [2];
      BYTE  pvn_offset                          [2];
      BYTE  pvn_length                          [2];
      BYTE  comparison_ind                      [2];
      BYTE  partial_pan_offset                  [2];
      BYTE  partial_pan_length                  [2];
   }Identikey;
   struct
   {
      BYTE  bankid                              [17];
      BYTE  pvn_location                        [2];
      BYTE  pvn_offset                          [2];
      BYTE  pvn_length                          [2];
      BYTE  partial_pan_offset                  [2];
      BYTE  partial_pan_length                  [2];
   }EIdentikey;
   struct
   {
      BYTE  conversion_tbl                      [17];
      BYTE  offset_location                     [2];
      BYTE  offset_offset                       [2];
      BYTE  offset_length                       [2];
   }Ibm3624;
   struct
   {
	  BYTE  pin_key								[17];
	  BYTE  decimalization_tbl					[17];
	  BYTE  pin_offset_location				    [3];
	  BYTE  track_offset						[3];
	  BYTE  offset_length						[2];
	}IbmOffset;
   struct
   {
      BYTE  key_left                            [17];
      BYTE  key_right                           [17];
      BYTE  pvv_location_tr1                    [2];
	  BYTE  pvv_location_tr2					[2];
      BYTE  pvv_offset_tr1                      [2];
	  BYTE  pvv_offset_tr2						[2];
      BYTE  pvkey_ind                           [2];
      BYTE  pvki_location_tr1                   [2];
	  BYTE  pvki_location_tr2					[2];
      BYTE  pvki_length_tr1                     [2];
	  BYTE  pvki_length_tr2						[2];
   }Visa;
   struct
   {
      BYTE  bankid                              [9];
      BYTE  partial_pan_offset                  [2];
      BYTE  partial_pan_length                  [2];
      BYTE  pin_verification_key                [17];
      BYTE  pvn_location                        [2];
      BYTE  pvn_offset                          [2];
      BYTE  pvn_length                          [2];
      BYTE  pvn2_type                           [2];
   }DESBiLevel;
   struct
   {
      BYTE  offset_location                     [2];
      BYTE  offset_offset                       [2];
      BYTE  algorithm                           [2];
      BYTE  key_table_location                  [4];
   }Deibold;
   struct
   {
      BYTE  conversion_table                    [16];
      BYTE  offset_location                     [2];
      BYTE  offset_offset                       [2];
      BYTE  offset_length                       [2];
      BYTE  partial_pan_offset                  [2];
      BYTE  partial_pan_length                  [2];
      BYTE  pad_character                       [2];
      BYTE  plen                                [2];
      BYTE  pin_verification_key                [17];
      BYTE  padding_flag                        [2];
      BYTE  counting_flag                       [2];
      BYTE  start_count_position                [2];
      BYTE  select_plen_position                [2];
   }Ncr;
   struct
   {
      BYTE  keya                                [17];
      BYTE  keyb                                [17];
      BYTE  service_code_offset                 [2];
      BYTE  cvv_offset                          [2];
      BYTE  cvv_length                          [2];
   }Cvv;
	
   struct
   {
      BYTE  encryption_key2                     [17];
   }PinBlock;
   struct
   {
      BYTE  No_Key                              [100];
   }None;
}PIN_DATA;



typedef struct
{
   struct
   {
       BYTE transaction_id                  [21]; 
   }primary_key;

    BYTE entry_type                         ;
    BYTE tpdu_id                            [16];
    BYTE issuer_id                          [21];
    BYTE acquirer_id                        [21];
    BYTE tx_key                             ;
    BYTE terminal_id                        [9];
    BYTE terminal_type                      [3];
    BYTE response_text                      [41];
    BYTE card_num_len                       [3];
    BYTE card_num                           [20];
    BYTE exp_date                           [5];
    BYTE total_amount                       [13];
    BYTE add_amounts                        [25];
    BYTE invoice_number                     [9];
    BYTE orig_amount                        [13];
    BYTE auth_number                        [7];
    BYTE processing_code                    [7];
    BYTE sys_trace_audit_num                [7];
    BYTE orig_sys_trace_num                 [7];
    BYTE pos_entry_mode                     [5];
    BYTE nii                                [5];
    BYTE pos_condition_code                 [3];
    BYTE track1                             [77];
    BYTE track2                             [38];
    BYTE retrieval_ref_num                  [13];
    BYTE orig_retrieval_ref_num             [13];
    BYTE merchant_id                        [16];
    BYTE orig_message                       [5];
    BYTE settlement_total                   [17];
    BYTE message_type                       [5];
    BYTE card_holder_name                   [31];
    BYTE general_status                     ;
    BYTE originator_queue                   [17];
    BYTE originator_info                    [33];
    BYTE batch_number                       [7];
    BYTE response_code                      [3];
    BYTE date_yyyymmdd                      [9];
    BYTE time_hhmmss                        [7];
    BYTE num_sales                          [4];
    BYTE sales_amount                       [13];
    BYTE num_refunds                        [4];
    BYTE refund_amount                      [13];
    BYTE num_dbsales                        [4];
    BYTE dbsales_amount                     [13];
    BYTE num_dbrefunds                      [4];
    BYTE dbrefund_amount                    [13];
    BYTE pin_block                          [19];
    BYTE tax_amount                         [13];
    BYTE tip_amount                         [13];
    BYTE tran_start_time                    [17];
    BYTE host_start_time                    [17];
    BYTE host_finish_time                   [17];
    BYTE tran_finish_time                   [17];
    BYTE settle_file_prefix                 [7];
    BYTE resp_source_len                    [3];
    BYTE visa_char_indicator                ;
    BYTE visa_tran_id                       [16];
    BYTE visa_validation_code               [5];
    BYTE mcard_banknet_len                  [4];
    BYTE mcard_banknet                      [51];
    BYTE agent_id_len                       [4];
    BYTE agent_id                           [12];
    BYTE cvc                                ;
    BYTE settlement_date                    [5];

    BYTE category_code                      [5];
    BYTE conversion_rate                    [10];
    BYTE product_code                       [9];
    BYTE odometer                           [8];
    BYTE organization_id                    [5];
    BYTE dynamic_keychange                  ;

    BYTE source_key                         [49];
    BYTE txn_cnt                            [4];
    BYTE dcf01_retired_cwk                  [49];
    BYTE dest_key                           [49];
    BYTE ncf01_retired_cwk                  [49];

    BYTE handler_queue        		        [8];
    BYTE authorizing_host_queue             [8];
    BYTE max_net_consec_tmouts              [4];
    BYTE max_active_txns                    [4];
    BYTE forwarding_institution_id          [12];
    BYTE remote_nsp_type                    [2];

    BYTE kek                                [17];
    BYTE acquiring_id                       [12];
    BYTE outstanding_balance                [13];
    BYTE credit_limit                       [13];

    BYTE nfi_name                           [11];
    BYTE nfi_seq_nbr                        [7];
    BYTE nfi_start_date                     [9];
    BYTE nfi_end_date                       [9];
    BYTE resp_source                        [26];
    struct
    {
      BYTE origin                           [3];
      BYTE auth_1                           [3];
      BYTE auth_2                           [3];

    }dispensation;

   BYTE update_mask                        [2];
   BYTE type_of_data                       [3];
   BYTE down_payment                       [13];
   BYTE period                             [3];
   BYTE interest_rate                      [5];
   BYTE total_interest                     [13];
   BYTE redemption_amount                  [13];
   BYTE vehicle_number                     [9];

   BYTE profile                            [3];
   BYTE nbr_of_prod_codes                  [3];
   struct
   {
      BYTE code                            [7];
      BYTE quantity                        [3];
      BYTE amount                          [13];
   }product_codes[20];

   BYTE visa_merchant_id                   [16];
   BYTE cash_bonus                         [2];
   BYTE deferred_factor                    [8];
   BYTE deferred_term_length               [4];
   BYTE currency_code                      [4];
   BYTE transmission_timestamp             [11];
   BYTE tran_fee_amount                    [9];
   BYTE cvc_data                           [4];

   BYTE verify_mag_stripe                  [2];
   BYTE saf                                [2];
   BYTE security_response_code             [3];
   BYTE service_code                       [4];
   BYTE source_pin_format                  [3];
   BYTE dest_pin_format                    [3];
   BYTE voice_auth                         [2];
   BYTE actual_amount                      [13];
   BYTE reversal_amount                    [13];
   BYTE voice_auth_text                    [241];
   BYTE operator_id                        [21];
   BYTE workstation                        [21];
   BYTE orig_auth_number                   [7];  /* for Voice Reversal */
   BYTE ticket_nbr                         [16]; /* for Offus velocity */
   BYTE system_date                        [9];  /* for Offus velocity */
   BYTE def_gross_amt                      [13];
   BYTE monthly_amort_amt                  [13];

}TLF01, *pTLF01;


/* binn_range table */
typedef struct
{
   struct
   {
      BYTE  length                                 [3];
      BYTE  low                                    [17];
      BYTE  type                                   [2];
   }primary_key; 

   BYTE  high                                      [17];
   BYTE  pan_length                                [3];
   BYTE  auth_ind                                  [2]; /* auth_ind=0 passthru to destination_name */
                                                        /* auth_ind=1 require profile number       */
   BYTE  caf_profile_id                            [5];
   BYTE  destination_name                          [11];
   BYTE  alt_destination_name                      [11]; /* AP SD, Equitable 01-13-99 */
   BYTE  credit_subtype                            [2];  /* AP SD, Equitable 01-13-99 */
   BYTE  description                               [31];
   BYTE  card_type                                 [5];
   BYTE  settle_file_prefix                        [7];
   BYTE  card_family                               [3];  /* AP SD, Equitable 02-03-99 */
   struct
   {  
      BYTE acquirer_id                              [12];
   }   acquirer_blocking[20];      
  
struct
   {  
      BYTE low_range    [6];
   }mcc_low[10];

   struct
   {  
      BYTE high_range    [6];
   }mcc_high[10];

   BYTE  mod_check                 [2];  
}BIN01, *pBIN01;


/* card authorization file */
typedef struct
{
   struct
   {
      BYTE  caf_profile_id                         [5];
      BYTE  type                                   [2];
   }primary_key;

   BYTE  authorization_location                    [2];
   BYTE  authorization_method                      [2];
   BYTE  destination_name                          [11];
   BYTE  iso_nbr                                   [10];
   BYTE  iso_length                                [2];
   BYTE  aba_nbr                                   [9];
   BYTE  name                                      [31];
   BYTE  contact                                   [21];
   BYTE  phone                                     [18];
   BYTE  date_added                                [9];
   BYTE  last_maint                                [9];
   BYTE  changed_by                                [21];
   BYTE  status                                    [3];
   BYTE  max_pin_retries                           [2];
   BYTE  pin_capture_ind                           [2];
   BYTE  track_offset                              [3];
   BYTE  pin_offset_location                       [2];
   BYTE  nsp_process                               [16];
   BYTE  expiration_date_check                     [2];
   BYTE  expiration_date_location                  [2];
   BYTE  merchant_same                             [3];
   struct
   {
      BYTE  day_period                             [3];
      BYTE  nbr_usage                              [2][4];
      BYTE  amt_usage                              [2][13];
   }velocity[3];

   BYTE  pin_verification_method                   [2];  
   PIN_DATA  pin;
   BYTE cvv_processing                              [2]; 
   struct
   {
	   BYTE cvkA									[17] ;
	   BYTE cvkB									[17] ;
   }cvk[6];

   
   BYTE service_code_offset_tr1						[3] ;
   BYTE service_code_offset_tr2						[3] ;
   BYTE cvv_offset_tr1								[3] ;
   BYTE cvv_offset_tr2								[3] ;
   BYTE cvv_length									[3] ;

   BYTE csck										[33]; 
   BYTE floor_limit									[13];
   BYTE country_limits_flag							[2];

}CAF01, *pCAF01;

/*Card Profile record */
typedef struct
{
	
	struct
	{
		BYTE  caf_profile_id                         [5];
		BYTE  type                                   [2];
	}primary_key;
	
	struct
	{
		char  service_code_offset              [2];
		char  icvc_offset                      [2];
		char  pan_seq_nbr_offset               [2];
	}cc_tk[2];
	
	struct
	{
		char  service_code_offset          [2];
		char  icvc_offset                  [2];
		char  pan_seq_nbr_offset           [2];
	}ccless_tk[2];
	
	struct
	{
		char  service_code_offset           [2];
		char  icvc_offset                   [2];
		char  pan_seq_nbr_offset            [2];
		char  atc_offset                    [2];
		char  unpre_nbr_offset              [2];
	}mcless_tk[2];
	
	char  MC_CVC3_code                             [33];
	char  MK_AC_code                               [33];
	
	char  future_key1                              [33];
	char  future_key2                              [33];
	char  future_key3                              [33];
	char  future_key4                              [33];
	char  future_key5                              [33];
	char  temp1                                    [11];
	char  temp2                                    [11];
	char  temp3                                    [11];
	char  temp4                                    [11];
	char  temp5                                    [11];
	
} CAF01_EMV, *pCAF01_EMV;


/* merchant control file */
typedef struct
{
   struct
   {
      BYTE  organization_id                        [5];
      BYTE  merchant_id                            [16];
   }primary_key;

   BYTE  status                                    [2];
   BYTE  name01                                    [31];
   BYTE  name02                                    [31];
   BYTE  address01                                 [31];
   BYTE  address02                                 [31];
   BYTE  city                                      [21];
   BYTE  country                                   [21];
   BYTE  state                                     [3];
   BYTE  zip_code                                  [11];
   BYTE  intl_code                                 [4];
   BYTE  currency_code                             [4];
   BYTE  contact                                   [31]; /* AP FA, Equitable 01-14-99 size change */
   BYTE  phone01                                   [18];
   BYTE  phone02                                   [18];
   BYTE  invoice_batch                             [4];
   BYTE  invoice_subcode                           [3];
   BYTE  process_control_id                        [7];
   BYTE  date_added                                [9];
   BYTE  region_nbr                                [4];  /* AP FA, Equitable 01-14-99 */
   BYTE  division_nbr                              [4];  /* AP FA, Equitable 01-14-99 */
   BYTE  password                                  [8];  /* AP FA, Equitable 01-14-99 */
   BYTE  rps_merchant                              [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  house_card_merchant                       [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  procurement_merchant                      [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  deferred_merchant                         [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  cash_bonus_eligible                       [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  vip_code                                  [5];  /* AP FA, Equitable 01-14-99 */
   BYTE  pos_code                                  [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  vs_tcc_code                               [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  mc_fcc_code                               [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  jcb_sub_code                              [3];  /* AP FA, Equitable 01-14-99 */
   BYTE  mall_code                                 [4];  /* AP FA, Equitable 01-14-99 */
   BYTE  hc_code                                   [4];  /* AP FA, Equitable 01-14-99 */
   BYTE  solicit_code                              [4];  /* AP FA, Equitable 01-14-99 */

   BYTE  block_start_date                          [9];
   BYTE  block_end_date                            [9];
   BYTE  credit_purch_max                          [13];
   BYTE  credit_auth_max                           [13];
   BYTE  credit_return_max                         [13];
   BYTE  credit_cash_adv_max                       [13];
   BYTE  credit_man_entry_max                      [13];
   BYTE  credit_cash_back_max                      [13];
   BYTE  debit_purch_max                           [13];
   BYTE  debit_auth_max                            [13];
   BYTE  debit_return_max                          [13];
   BYTE  debit_cash_adv_max                        [13];
   BYTE  debit_man_entry_max                       [13];
   BYTE  debit_cash_back_max                       [13];

   BYTE  aver_purch_amount                         [13];
   BYTE  max_num_returns                           [5];
   BYTE  amount_returns                            [13];
   BYTE  number_man_entries                        [5];
   BYTE  amount_deposits                           [13];
   BYTE  pan_same                                  [3];

   BYTE  totals_ind                                [2];
   BYTE  statistics_ind                            [2];

   struct
   {
      BYTE  priority_routing                       [10];
   }network[10];

   BYTE  negative_group_id                         [11];
   BYTE  voice_category_code                       [5];
   BYTE  class_code                                [4];
   BYTE  deferred_start_date                       [9];
   BYTE  deferred_end_date                         [9];
   BYTE  group_outlet_code                         [6];
   BYTE  group_type                                [5];
   BYTE  reload_merchant                           [2];
   
}MCF01, *pMCF01;

typedef struct
{
   struct
   {
	   BYTE    merchant_id 						[16];
	   BYTE    card_nbr    						[20];
	   BYTE    max_repeat  						[11];
	   BYTE    repeated    						[11];
	   BYTE    expiry_date 						[9];
	   BYTE	   peso_amt_limit					[13];
	   BYTE	   usd_amt_limit					[13];
   }primary_key;
       BYTE	bypass								[9]; 
	   BYTE	future_use1							[33];
	   BYTE	future_use2							[33];
	   BYTE	future_use3							[33];
	   BYTE	future_use4							[33];
}OMC01, *pOMC01;

typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  primary_acct_nbr                          [20];
   BYTE  caf_profile_id                            [5];
   BYTE  status                                    [3];
   BYTE  pin_offset                                [9];
   BYTE  date_added                                [9];
   BYTE  maint_date                                [9];
   BYTE  changed_by                                [21];
   BYTE  card_class                                [2];
   BYTE  expire_date                               [5];
   BYTE  effect_date                               [9];
   BYTE  percent_deposit                           [3];
   BYTE  percent_avail_allowed                     [3];
   BYTE  credit_line                               [13];
   BYTE  outstanding_auths                         [13];
   BYTE  allowed                                   [17];
   BYTE  currency_code                             [4];
   BYTE  country_code                              [4];
   BYTE  peso_saving_acct                          [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  peso_current_acct                         [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  dollar_saving_acct                        [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  dollar_current_acct                       [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  product_codes                             [21]; /* AP FA, Equitable 01-14-99 */
   BYTE  cash_bonus_eligible                       [2];  /* AP SD, Equitable 01-28-99 */
   BYTE  visa_purchase_card                        [2];  /* AP SD, Equitable 01-28-99 */

   struct
   {
      BYTE  type                                   [3];
      BYTE  nbr                                    [20];
   }related_acct[10];

   BYTE  pin_verification_override                 [2];
   BYTE  pin										[51];
	BYTE  pvki [3];
   BYTE  add_on_rate                               [6];
   BYTE  add_on_term                               [4];
   BYTE  peso_tran_limit						   [13];
   BYTE  dollar_tran_limit						   [13];	

}CCF02P, *pCCF02P;



/* positive related account file */
typedef struct
{
   struct
   {
      BYTE  account_type                           [3];
      BYTE  account_nbr                            [20];
   }primary_key;

   BYTE  status                                    [3];
   BYTE  date_added                                [9];
   BYTE  maint_date                                [9];
   BYTE  changed_by                                [21];
   BYTE  credit_limit                              [13];
   BYTE  available                                 [13];
   BYTE  outstanding                               [13];
   BYTE  ledger_balance                            [13];
   BYTE  cash_limit                                [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  cash_available                            [13]; /* AP FA, Equitable 01-14-99 */
   BYTE  cash_outstanding                          [13]; /* AP FA, Equitable 01-14-99 */

}ACF01, *pACF01;



typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  primary_acct_nbr                          [20];
   BYTE  daily_amt_usage                           [13];

   BYTE  julian_date                               [8];
   BYTE  peso_julian_date                          [8];  /* AP SD 01-28-1999 */
   BYTE  dollar_julian_date                        [8];  /* AP SD 01-28-1999 */

   struct
   {
      BYTE  nbr_usage                              [4];
      BYTE  amt_usage                              [13];
   }peso_usage[14], dollar_usage[14];                    /* AP SD 01-28-1999 remove 1 add 2 */

   BYTE  failed_pin_attempts                       [2];
   BYTE  date_added                                [9];
   BYTE  last_maint                                [9];
   BYTE  status                                    [3];

   struct
   {
      BYTE  category_code                          [5];
      BYTE  usage                                  [4];
   }mcc[20];

   struct
   {
      BYTE  merchant_id                            [16];
      BYTE  nbr_uses                               [4];
   }repeat[20];

}CCF03, *pCCF03;

typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  primary_acct_nbr                          [20];
   BYTE  daily_amt_usage                           [13];

   BYTE  julian_date                               [8];
   BYTE  peso_julian_date                          [8];  /* AP SD 01-28-1999 */
   BYTE  dollar_julian_date                        [8];  /* AP SD 01-28-1999 */

   struct
   {
      BYTE  nbr_usage                              [4];
      BYTE  amt_usage                              [13];
   }peso_usage[14], dollar_usage[14];                    /* AP SD 01-28-1999 remove 1 add 2 */

   BYTE  failed_pin_attempts                       [2];
   BYTE  date_added                                [9];
   BYTE  last_maint                                [9];
   BYTE  status                                    [3];

   struct
   {
      BYTE  category_code                          [5];
      BYTE  usage                                  [4];
   }mcc[20];

   struct
   {
      BYTE  merchant_id                            [16];
      BYTE  nbr_uses                               [4];
   }repeat[20];

}Offus_velocity, *pOffus_velocity;


/* fleet card file*/
typedef struct
{
   struct
   {
      BYTE  caf_profile_id                         [5];
   }primary_key;

   struct
   {
      BYTE  amount                                 [13];
      BYTE  from_time                              [5];
      BYTE  to_time                                [5];
   }velocity[10];

}FAF01, *pFAF01;


/* AP SD 01-28-1999 New table for cash bonus program parameters */
typedef struct
{
   BYTE  minimum_amount                            [13];
   BYTE  program_start_date                        [8]; 
   BYTE  program_end_date                          [8];
   BYTE  transaction_threshold                     [8];
   BYTE  transaction_counter                       [8];
   BYTE  maximum_reward                            [13];
   BYTE  response_message                          [41];

}BON01, *pBON01;


/* EMV Database Table Definition */
typedef struct
{
   BYTE  transaction_id            [21];
   BYTE  tag                       [6];
   BYTE  len                       [4];
   BYTE  emv_data                  [129];
   BYTE  system_date               [9];

} EMV01, *pEMV01;


/* EMV Data Structure for Auth_Tx */
typedef struct
{
   BYTE  currency_code             [5];
   BYTE  issuer_script_1           [129];
   BYTE  issuer_script_2           [129];
   BYTE  interchange_profile       [5];
   BYTE  app_id                    [33];
   BYTE  issuer_auth_data          [33];
   BYTE  tvr                       [11];
   BYTE  tran_date                 [7];
   BYTE  tran_type                 [3];
   BYTE  tran_amount               [13];
   BYTE  other_amount              [13];
   BYTE  version                   [5];
   BYTE  iad                       [65];
   BYTE  country_code              [4];
   BYTE  serial_number             [9];
   BYTE  app_crypto                [17];
   BYTE  crypto_info               [3];
   BYTE  terminal_capabilities     [7];
   BYTE  cvm_results               [7];
   BYTE  terminal_type             [3];
   BYTE  atc                       [5];
   BYTE  unpredictable_number      [9];
   BYTE  tran_sequence_ctr         [9];
   BYTE  tcc                       [2];
   BYTE  mcc                       [5];
   BYTE  pan_sequence_number       [3];
   BYTE  chip_condition_code       [2];
   BYTE  emv_block                 [256]; /* Raw data from TID */
   BYTE  future_field1             [80];
   BYTE  future_field2             [80];
   BYTE  future_field3             [80];

}EMV, *pEMV;


typedef struct
{
   BYTE  transaction_id            [21];
   BYTE  system_date               [9];
   BYTE  DATA_LENGTH               [4];
   BYTE  RAW_DATA                  [300];

} EMV_RAW, *pEMV_RAW;


typedef struct
{
    TLF01 TLF01_details; 
    BIN01 BIN01_details;					
	 CAF01 CAF01_details;
	 MCF01 MCF01_details;
	 CCF02P CCF02P_details;
    ACF01 ACF01_details;
    CCF03 CCF03P_details;
    FAF01 FAF01_details;
    BON01 BON01_details;
    Offus_velocity offus_velocity_details;
    EMV   EMV_details;
    BYTE  ccf03_insert;
    BYTE  offus_velocity_insert;
    BYTE  bin_type;
    BYTE  mac_key                [17];
    BYTE  industry_code          [4];
    BYTE  ncf01_prev_src_station [7];
    BYTE  zip_code               [11];
    BYTE  ch_billing_amt         [13];    /* AP DI, Equitable 01-07-99 */
    BYTE  ch_billing_conv_rate   [9];     /* AP DI, Equitable 01-07-99 */
    BYTE  settlement_conv_rate   [10];    /* AP DI, Equitable 06-11-99 */
    BYTE  conv_date              [5];     /* AP DI, Equitable 01-07-99 */
    BYTE  ch_billing_curr_code   [4];     /* AP DI, Equitable 01-07-99 */
    BYTE  int_net_facility_data  [101];   /* AP DI, Equitable 01-07-99 */
    BYTE  int_net_facility_len   [4];     /* AP DI, Equitable 01-07-99 */
    BYTE  settlement_curr_code   [9];     /* AP DI, Equitable 01-07-99 */
    BYTE acceptor_term_name      [26];    /* AP RM, Equitable 02-03-99 */
    BYTE acceptor_city_name      [14];    /* AP RM, Equitable 02-03-99 */
    BYTE acceptor_country        [3];     /* AP RM, Equitable 02-03-99 */           
    BYTE available_balance       [10];    /* AP RM, Equitable 02-03-99 */
    BYTE available_balance_sign;          /* AP RM, Equitable 02-03-99 */
    BYTE current_balance         [10];    /* AP RM, Equitable 02-03-99 */
    BYTE current_balance_sign;            /* AP RM, Equitable 02-03-99 */
    BYTE pos_pin_capt_code       [3];
    BYTE receiving_inst_id_code  [12];
    BYTE pan_ext_ctry_code       [4];     /* AP BG, Equitable 02-04-99 */
    BYTE pan_extended            [29];    /* AP BG, Equitable 02-04-99 */
    BYTE country_code            [4];     /* AP BG, Equitable 02-04-99 */
    BYTE acct_id_1               [30];    /* AP BG, Equitable 02-04-99 */
    BYTE acct_id_2               [30];    /* AP BG, Equitable 02-04-99 */
    BYTE iss_inst_id_cd          [13];    /* AP BG, Equitable 02-04-99 */
    BYTE iss_inst_id_len         [3];     /* BG, Equitable 03-01-99 */
    BYTE acq_inst_id_cd          [12];    /* AP BG, Equitable 02-04-99 */
    BYTE acq_inst_id_len         [3];     /* BG, Equitable 03-01-99 */
    BYTE forwarding_inst_id_cd   [12];    /* AP BG, Equitable 02-04-99 */
    BYTE forwarding_inst_id_len  [3];     /* BG, Equitable 03-01-99 */
    BYTE additional_pos_information [7];  /* AP BG, Equitable 02-04-99 */
    BYTE reconciliation_amount   [13];    /* AP BG, Equitable 02-09-99 */
    BYTE local_date_time         [13];    /* AP BG, Equitable 02-09-99 */
    BYTE function_code           [4];     /* AP BG, Equitable 02-09-99 */
    BYTE accptor_bus_code        [5];     /* AP BG, Equitable 02-09-99 */
    BYTE reconciliation_cur_code [4];     /* AP BG, Equitable 02-09-99 */
    BYTE effective_date          [5];     /* AP BG, Equitable 02-09-99 */
    BYTE host2_src_fi_nbr        [5];     /* RM equitable 02-15-99 */
    BYTE host2_src_terminal_nbr  [6];     /* RM equitable 02-15-99 */
    BYTE host2_src_seq_nbr       [7];     /* RM equitable 02-15-99 */
    BYTE host2_message_seq_nbr   [5];     /* RM equitable 02-15-99 */
    BYTE host2_acquirer_cb_code  [3];     /* MS Equitable 02-16-99 */
    BYTE orig_local_time_date    [13];    /* BG, Equitable 02-18-99 */
    BYTE orig_acq_inst_id        [14];    /* BG, Equitable 02-18-99 */
    BYTE amex_pos_entry_mode     [13];    /* BG, Equitable 02-19-99 */
    BYTE host_processing_code    [7];     /* BG, Equitable 03-02-99 */
    BYTE advice_repeat_count     [3];     /* BG, Equitable 03-02-99 */
    BYTE incoming_dest_station_id [7];    /* BG, Equitable 05-11-99 - Visa */
    BYTE incoming_src_station_id [7];     /* BG, Equitable 05-11-99 - Visa */
	BYTE future_use_1			 [100];
	BYTE future_use_2			 [100];
	BYTE future_use_3			 [100];
	BYTE future_use_4			 [100];
	BYTE future_use_5			 [100];
	BYTE req_data_len				 [5] ;		
	BYTE req_buffer				 [1500];
	BYTE resp_data_len				 [5] ;		
	BYTE resp_buffer				 [1000];
}AUTH_TX, *pAUTH_TX;


typedef struct
{
   BYTE  num_returned             [5];
   struct
   {
      BYTE  organization_id       [5];
      BYTE  merchant_id           [16];
	  	   		
   }merchant_info[GUI_MAX_LIST_SIZE];

}MCF01_GUI_LIST, *pMCF01_GUI_LIST;


typedef struct
{
   BYTE  num_returned             [5];
   struct
   {
      BYTE  organization_id       [5];
      BYTE  merchant_id           [16];
      BYTE  name01                [31];
      BYTE  name02                [31];
      BYTE  address01             [31];
      BYTE  address02             [31];
      BYTE  city                  [21];
	  	   		
   }merchant_info[GUI_MAX_LIST_SIZE];

}MCF01_REPORT_LIST, *pMCF01_REPORT_LIST;

typedef struct
{
   BYTE  num_returned             [5];
   struct
   {
      BYTE  card_type             [2];
      BYTE  card_nbr              [20];
      BYTE  name_dba              [31];
      BYTE  address1              [31];
      BYTE  address2              [31];
      BYTE  city                  [21];
	  	   		
   }ccf_info[GUI_MAX_LIST_SIZE];

}CCF01_REPORT_LIST, *pCCF01_REPORT_LIST;

typedef struct
{
   BYTE  num_returned             [5];
   struct
   {
      BYTE  primary_acct_nbr      [20];
      BYTE  name_dba              [31];
      BYTE  address1              [31];
      BYTE  address2              [31];
      BYTE  city                  [21];
	  	   		
   }paf_info[GUI_MAX_LIST_SIZE];

}PAF01_REPORT_LIST, *pPAF01_REPORT_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   MCF01 merchant_list    [MCF01_SERVICE_LIST_SIZE];
}MCF01_LIST, *pMCF01_LIST;



typedef struct
{
   BYTE  delete_flag;
   BIN01 bin01_record;
}BIN01_MEM, *pBIN01_MEM;

typedef struct
{
   BYTE  num_returned     [5];
   BIN01 bin_range_list   [GUI_MAX_LIST_SIZE];
}BIN01_GUI_LIST, *pBIN01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   ACF01 acf01_list       [GUI_MAX_LIST_SIZE];
}ACF01_GUI_LIST, *pACF01_GUI_LIST;



typedef struct
{
   BYTE  num_returned     [5];
   BIN01 bin_range_list   [SERVICE_LIST_SIZE];
}BIN01_LIST, *pBIN01_LIST;

typedef struct
{
   BYTE  num_returned  [5];
   BON01 bon_list      [SERVICE_LIST_SIZE];
}BON01_LIST, *pBON01_LIST;


typedef struct
{
   BYTE  delete_flag;
   CAF01 caf01_record;
}CAF01_MEM, *pCAF01_MEM;


typedef struct
{
   BYTE  num_returned     [5];
   struct
   {
	  BYTE  caf_profile_id   [5];
	  BYTE  type			 [2];
   }caf_info[GUI_MAX_LIST_SIZE];

}CAF01_GUI_LIST, *pCAF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   CAF01 caf01_list       [CAF01_SERVICE_LIST_SIZE];
}CAF01_LIST, *pCAF01_LIST;

typedef struct
{
   BYTE  delete_flag;
   FAF01 faf01_record;
}FAF01_MEM, *pFAF01_MEM;

typedef struct
{
   BYTE  num_returned     [5];
   FAF01 faf01_list       [FAF01_SERVICE_LIST_SIZE];
}FAF01_LIST, *pFAF01_LIST;

typedef struct
{
   BYTE  card_type           [2];
   BYTE  card_nbr            [20];
}CCF02P_PK, *pCCF02P_PK;

typedef struct
{
   BYTE   num_returned    [5];
   CCF02P_PK ccf02p_pk    [GUI_MAX_LIST_SIZE];
}CCF02P_GUI_LIST, *pCCF02P_GUI_LIST;

typedef struct
{
	BYTE	merchant_id 					 [16];
	BYTE	card_nbr						 [20];
	BYTE	max_repeat						 [11];
	BYTE	repeated						 [11];
	BYTE	expiry_date 					 [9];
	BYTE	peso_amt_limit			         [13];
	BYTE	usd_amt_limit					 [13];
	BYTE	bypass								[9]; 
	BYTE	future_use1							[33];
	BYTE	future_use2							[33];
	BYTE	future_use3							[33];
	BYTE	future_use4							[33];
}OMC01_PK, *pOMC01_PK;

typedef struct
{
   BYTE   num_returned    [5];
   OMC01_PK omc01_pk    [GUI_MAX_LIST_SIZE];
}OMC01_GUI_LIST, *pOMC01_GUI_LIST;


typedef struct
   {
      BYTE transaction_id                     [21];
      BYTE terminal_id                        [9];
      BYTE  merchant_id                       [16];
      BYTE card_num                           [20];
      BYTE exp_date                           [5];
      BYTE total_amount                       [13];
      BYTE pos_entry_mode                     [5];
      BYTE response_code                      [3];
      BYTE date_yyyymmdd                      [9];
      BYTE time_hhmmss                        [7];
      BYTE tx_key                            ; 
      BYTE auth_number                        [7];
      BYTE processing_code                    [7];
      BYTE sys_trace_audit_num                [7];
      BYTE outstanding_balance                [13];
      BYTE ticket_nbr                         [16];
      BYTE product_code                       [9];
      BYTE deferred_factor                    [8];  
      BYTE deferred_term_length               [4];  
      BYTE category_code                      [5];
      BYTE tran_start_time                    [17];
      BYTE tran_finish_time                   [17];
      BYTE reversal_amount                    [13];
      BYTE retrieval_ref_num                  [13];
      BYTE pos_condition_code                 [3];
      BYTE odometer                           [8];
      BYTE currency_code                      [4];
      BYTE system_date                        [9];
   }TLF01_LIST_STRUCTURE, *pTLF01_LIST_STRUCTURE;

typedef struct
{
   BYTE  num_returned             [5];
   TLF01_LIST_STRUCTURE  tlf01_list_structure[SCH_MAX_LIST_SIZE];

}TLF01_LIST, *pTLF01_LIST;

typedef struct
{
   BYTE  num_returned             [5];
   TLF01 tlf01_structure          [GUI_MAX_LIST_SIZE1];

}TLF01_GUI_LIST, *pTLF01_GUI_LIST;

typedef struct
{
	BYTE transaction_id [21];
	BYTE card_num_min [21];
	BYTE card_num_max [21];
	BYTE date_yyyymmdd_start[9];
	BYTE date_yyyymmdd_end[9];
	BYTE time_hhmmss_start[7];
	BYTE time_hhmmss_end[7];
	BYTE acquirer_id [21];
	BYTE issuer_id [21];
	BYTE merchant_id_min[16];
	BYTE merchant_id_max[16];
	BYTE terminal_id_min[9];
	BYTE terminal_id_max[9];
	BYTE response_code_min[3];
	BYTE response_code_max[3];
	BYTE sys_trace_audit_num_min[9];
	BYTE sys_trace_audit_num_max[9];
	BYTE total_amount_min[13];
	BYTE total_amount_max[13];
	BYTE reversal_amount_min[13];
	BYTE reversal_amount_max[13];
	BYTE auth_number_min[7];
	BYTE auth_number_max[7];
	BYTE retrieval_ref_number_min[13];
	BYTE retrieval_ref_number_max[13];
   BYTE system_date_min[9];
   BYTE system_date_max[9];
}TLF01_LIMITS, *pTLF01_LIMITS;


/* ----------------------------- */
/* Fraudguard Transaction Tables */
/* ----------------------------- */
typedef struct
{
   BYTE  transaction_id         [21];
   BYTE  timestamp              [20];
   BYTE  message_type            [5];
   BYTE  card_num               [20];
   BYTE  processing_code         [7];
   BYTE  amount_peso            [13];
   BYTE  amount_usd             [13];
   BYTE  gmt                    [11];
   BYTE  sys_trace_audit_num     [7];
   BYTE  mcc                     [5];
   BYTE  country_code            [5];
   BYTE  pos_entry_mode          [5];
   BYTE  pos_condition_code      [3];
   BYTE  acquiring_id           [21];
   BYTE  auth_number             [7];
   BYTE  response_code           [3];
   BYTE  terminal_id             [9];
   BYTE  merchant_id            [16];
   BYTE  card_acceptor_name     [26];
   BYTE  card_acceptor_city     [14];
   BYTE  card_acceptor_country   [4];
   BYTE  currency_code_txn       [4];
   BYTE  currency_code_billing   [4];
   BYTE  pos_info               [31];
   BYTE  status                     ;
   BYTE  system_date             [9];
   BYTE  result_code             [6];
   BYTE  application            [21];
   BYTE  rule_domain            [81];
   BYTE  caller_id               [6];
   BYTE  caller_msg_id         [256];
   BYTE  result_text           [256];
   BYTE  product                [31];

} FGUARD, *pFGUARD;


typedef struct
{
   BYTE    num_returned    [5];
   FGUARD  FGUARD_record   [GUI_MAX_LIST_SIZE1];

} FGUARD_GUI_LIST, *pFGUARD_GUI_LIST;


/* ---------------------------------------- */
/* Fraudguard Raw Data Table (XML Messages) */
/* ---------------------------------------- */
typedef struct
{
   BYTE  transaction_id   [21];
   BYTE  system_date       [9];
   BYTE  length            [5];
   BYTE  raw_data       [1500];

} FG_RAW, *pFG_RAW;

typedef struct
{
   BYTE    num_returned  [5];
   FG_RAW  FG_RAW_record [GUI_MAX_LIST_SIZE1];

} FGRAW_GUI_LIST, *pFGRAW_GUI_LIST;



/* --------------------------- */
/* Cash Bonus Promotion Tables */
/* --------------------------- */

/* Data Structure for Cash Bonus Configuration */
typedef struct
{
   BYTE cash_id              [21];
   BYTE tag_id               [2];
   BYTE cash_desc            [121];
   BYTE program_start_date   [9];
   BYTE program_end_date     [9];
   BYTE tran_threshold       [10];
   BYTE tran_ctr             [10];
   BYTE time_period          [5];
   BYTE max_period_winners   [7];
   BYTE period_win_ctr       [7];
   BYTE max_program_winners  [7];
   BYTE program_win_ctr      [7];
   struct
   {
      BYTE low_amount        [13];
      BYTE high_amount       [13];
      BYTE resp_text         [121];
      BYTE win_type          [2];
      BYTE reward_amount     [13];

   }amount_range[5];
   BYTE multi_winner_text    [121];
   BYTE multi_winners_allowed[2];

} BON02, *pBON02;


typedef struct
{
   BYTE  num_returned        [5];
   struct
   {
      BYTE  cash_id          [5];
      BYTE  tag_id           [16];
	  	   		
   }cash_bonus_info[GUI_MAX_LIST_SIZE];

}BON02_GUI_LIST, *pBON02_GUI_LIST;


typedef struct
{
   BYTE  num_returned  [5];
   BON02 bon_list      [SERVICE_LIST_SIZE];
}BON02_LIST, *pBON02_LIST;



/* Data Structure for Cash Bonus Winner History */
typedef struct
{
   BYTE transaction_id       [21];
   BYTE cash_id              [21];
   BYTE tag_id               [2];
   BYTE program_start_date   [9];
   BYTE program_end_date     [9];
   BYTE card_nbr             [20];
   BYTE merchant_id          [16];
   BYTE terminal_id          [9];
   BYTE auth_number          [7];
   BYTE tran_date            [9];
   BYTE tran_time            [7];
   BYTE reward_amount        [13];
   BYTE tran_amount          [13];
   BYTE resp_text            [121];
   BYTE system_date          [9];
   BYTE invoice_number       [9];
   BYTE retrieval_ref_num    [13];
   BYTE card_holder_name     [31];

} BON03, *pBON03;


typedef struct
{
   BYTE  num_returned  [5];
   BON03 bon_list      [SERVICE_LIST_SIZE];
}BON03_LIST, *pBON03_LIST;


/* Data Structures to hold Data Server queries in shared memory. */
typedef struct
{
   BYTE appname[20];
   BYTE sysdate[24];  /* YYYY-MM-DD-hh.mm.ss.jjj */
   BYTE sqltext[400];

} SQL01, *pSQL01;


typedef struct
{
   BYTE  num_returned  [5];
   SQL01 sql01_list    [30];

}SQL01_LIST, *pSQL01_LIST;


#endif
