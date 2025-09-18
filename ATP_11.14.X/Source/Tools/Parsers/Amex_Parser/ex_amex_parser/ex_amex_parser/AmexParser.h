
#if !defined(AFX_AMEXPARSER_H__10A958AC_8DCB_4F79_99A4_3D251C93C840__INCLUDED_)
#define AFX_AMEXPARSER_H__10A958AC_8DCB_4F79_99A4_3D251C93C840__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

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
   PIN_DATA  pin;
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

//}EMV, *pEMV;
   }EMV;


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
}AUTH_TX, *pAUTH_TX;


#endif // !defined(AFX_AMEXPARSER_H__10A958AC_8DCB_4F79_99A4_3D251C93C840__INCLUDED_)
