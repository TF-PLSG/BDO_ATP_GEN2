/******************************************************************************
  
   Module: Update_db.h
  
   Title: Update Dataserver DB Routines Profiles
   

******************************************************************************/

#ifndef _UPDATE_DB
#define _UPDATE_DB

#define VISA7 1

#define MC12_OCT2023 1
#define MC13_OCT2023 1
#define MC06_OCT2023 1
#define MC02_APR2024 1
#define BDOR_62_PREAUTH_VOID 1
/*MP Purchase IT TYPE change*/
#define INVOICE_NUMBER_LEN 					6
#define PURCHASE_ID_TYPE_LEN  				6
#define PURCHASE_ID_DATA_POS				12
#define PURCHASE_ID_LEFTOVER_DATA_POS_1 	24
#define PURCHASE_ID_LEFTOVER_DATA_POS_2		36

#ifdef VISA7
#define RC_CAT_1 1
#define RC_CAT_2 2
#define RC_CAT_3 3
#define RC_CAT_4 4
#define CATEGORY_CODE_REATTEMPT_REACHED "04"

#define ALL_CATEGORY_CODE "1234"
#define	APPROVED "00"
#define HONOR_WITH_ID_RC "08"
#define PARTIAL_APPROVAL_RC "10"
#define ZERO_AMOUNT_RC		"85"
#define PURCHASE_AMOUNT_RC	"87"
#endif

/*PreAuth Void Related defination*/
#define INITIAL_PREAUTH_TRA      						  	"PRIMRY"
#define INCREMENTAL_PREAUTH_TRA      					  	"INCPRA"
#define INITIAL_PREAUTH_TRA_WITH_INCREMENTAL                "PI"
#define INITIAL_PREAUTH_TRA_WITHOUT_INCREMENTAL             "WI"
#define PREAUTH_TRA_LEN_WITH_INCREMENTAL_FLAG  				2
#define PREAUTH_TRA_LEN  								  	6
#define INCREMENTAL_AUTH 'I'
#define PRIMARY_AUTH 'P'

#ifdef MC13_OCT2023
#define MAC_CAT_1 		  1
#define MAC_CAT_2 		  2
#define MAC_CATEGORY_CODE "1111"
#define MAC_NOT_DEFINED	  -1
#endif

#define AMEX03A
#define ESTIMATED_TRANSACTION_KEY "PRIMARY"
INT LOOKBACK_DAYS =0;
char last_date[9]={0};



BYTE db_select_and_update_ncf30_for_Incrementalauthorization(pNCF30,  pCHAR);
BYTE db_select_and_update_ncf30_for_Partialreversalauthorization(pNCF30,  pCHAR);


BYTE db_insert_tlf01 (pTLF01, pCHAR);  

BYTE db_insert_tlf01_stip(pTLF01, pCHAR);       
                        
BYTE db_select_bch01 (pCHAR, pBCH01, pCHAR); 
BYTE db_insert_bch01 (pBCH01, pCHAR);         
BYTE db_update_bch01 (pBCH01, pCHAR);

BYTE db_insert_bch10 (pBCH10, pCHAR);         
BYTE db_delete_bch10_by_stan( pBCH10 bch10_ptr, pCHAR ErrorMsg);
BYTE db_delete_active_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg);
BYTE db_update_bch10( pBCH10, pCHAR);
BYTE db_select_bch10 (pBCH10, pBCH10, pCHAR);
BYTE db_select_active_bch10 (pBCH10, pBCH10, pCHAR);
BYTE db_select_bch10_by_stan (pBCH10,pBCH10,pCHAR );
BYTE db_delete_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg);

BYTE db_insert_bch20( pBCH20 bch20_ptr, pCHAR ErrorMsg);
BYTE db_update_bch20_by_stan( pBCH20 bch20_ptr, pCHAR ErrorMsg);
BYTE db_update_emv_raw_by_bch20_auth_number(pBCH20 bch20_ptr, pCHAR tid,pCHAR ErrorMsg);

BYTE db_update_bon01 (pBON01 bon01_ptr, pCHAR ErrorMsg );
BYTE db_select_bon01 (pBON01 bon01_ptr, pCHAR ErrorMsg );
void bon01_to_db( pBON01 bon01_ptr );
void db_to_bon01( pBON01 bon01_ptr );

INT  perform_cash_bonus_processing( pAUTH_TX, pBON02, pCHAR );
INT  get_cash_bonus_result( pAUTH_TX p_auth_tx );

BYTE db_update_ccf02p( pCCF02P ccf02p_ptr, pCHAR ErrorMsg );
BYTE db_insert_ccf03p( pCCF03 ccf03p_ptr, pCHAR ErrorMsg );
BYTE db_update_ccf03p( pCCF03 ccf03p_ptr, pCHAR ErrorMsg );
BYTE db_update_acf01( pACF01 acf01_ptr, pCHAR ErrorMsg );
BYTE db_update_acf01_for_deferred_account (pAUTH_TX	auth_tx,pACF01 acf01_ptr,pCHAR ErrorMsg);
BYTE db_delete_bch20_by_auth_nbr (pTLF01 , pCHAR  );
BYTE db_delete_bch20_by_stan( pBCH20 bch20_ptr, pCHAR ErrorMsg) ;
BYTE db_insert_offus_velocity( pOffus_velocity , pCHAR ErrorMsg );
BYTE db_update_offus_velocity( pOffus_velocity , pCHAR ErrorMsg );

void ncf21_to_db( pNCF21 ncf21_ptr );
void ncf30_to_db( pNCF30 ncf30_ptr );
void db_to_ncf21 ( pNCF21 ncf21_ptr );
void db_to_ncf30 ( pNCF30 ncf30_ptr );
BYTE db_select_ncf21 (  pCHAR key, pNCF21 ncf21_ptr, pCHAR ErrorMsg );
BYTE db_insert_ncf30( pNCF30 ncf30_ptr, pCHAR ErrorMsg );
BYTE db_insert_ncf21( pNCF21 ncf21_ptr, pCHAR ErrorMsg );
BYTE db_update_ncf21( pNCF21 ncf21_ptr, pCHAR ErrorMsg );
BYTE db_delete_ncf30( pNCF30 ncf30_ptr, pCHAR ErrorMsg) ;
BYTE db_delete_ncf30_DCI( pNCF30 ncf30_ptr, pCHAR ErrorMsg) ;
BYTE db_update_tlf01( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_reload_confirm( pTLF01 tlf01_ptr, pCHAR ErrorMsg );

void get_date( pCHAR time_date, pCHAR date_str );
void create_system_date( pBYTE );
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
void convert_msgtype_to_request( pBYTE msgtype );

BYTE store_emv_records( pAUTH_TX p_auth_tx );
BYTE store_pad_raw_records(pAUTH_TX p_auth_tx);
BYTE db_insert_emv01( pEMV01 emv01_ptr, pCHAR ErrorMsg );
BYTE db_insert_emvraw( pEMV_RAW emvraw_ptr, pCHAR ErrorMsg );
BYTE db_insert_pad_raw( pPAD_RAW pad_raw_ptr, pCHAR ErrorMsg );
BYTE update_chip_condition( pAUTH_TX p_auth_tx );
BYTE db_udpate_chip_condition( pAUTH_TX p_auth_tx, pCHAR ErrorMsg );
BYTE db_process_tc_crypto( pTLF01, pEMV, pBYTE, pCHAR );

BYTE db_insert_fguard( pBYTE fraudguard_ptr, pCHAR ErrorMsg );
BYTE db_update_fguard( pFGUARD fguard_ptr,   pCHAR ErrorMsg );
void db_to_fguard( pFGUARD fguard_ptr );
void fguard_to_db( pFGUARD fguard_ptr );
void fgraw_to_db( pFG_RAW fgraw_ptr );

void bon03_to_db( pBON03 bon03_ptr );
void bon02_to_db( pBON02 bon02_ptr );
void db_to_bon02( pBON02 bon02_ptr );
void ptsn01_to_db( pPTSN01 ptsn01_ptr );
void create_bon03_record( pAUTH_TX p_auth_tx, pBON02 p_bon02, pBON03 p_bon03 );
INT  db_select_bon03( pBYTE  card,      pBYTE cash_id  );
BYTE db_insert_bon03( pBON03 bon03_ptr, pCHAR ErrorMsg );
BYTE db_insert_ptsn01( pPTSN01 ptsn01_ptr, pCHAR ErrorMsg );
BYTE db_update_bon02( pBON02 bon02_ptr, pCHAR ErrorMsg );
INT  db_process_cash_bonus_programs( pAUTH_TX p_auth_tx,
                                     pCHAR    ErrorMsg );

BYTE db_select_cardholder_name( pCHAR p_cardnum,  pCHAR p_cardtype,
                                pCHAR p_cardname, pCHAR ErrorMsg );

#ifdef VISA7
BYTE db_select_merchant_integrity_param (  pCHAR          Primary_key,
                        				  pMERCHANT_INTEGRITY_PARAM   merchant_integrity_param_ptr,
                        				   pCHAR          ErrorMsg );
BYTE db_select_merchant_integrity_param_for_cat1 (  pCHAR          Primary_key,
                        				  pMERCHANT_INTEGRITY_PARAM   merchant_integrity_param_ptr,
                        				   pCHAR          ErrorMsg );
BYTE db_delete_merchant_integrity_param_for_cat_2_3_4( pCHAR Primary_key, pCHAR ErrorMsg );
BYTE db_delete_merchant_integrity_param_for_cat1( pCHAR Primary_key, pCHAR ErrorMsg );
BYTE db_update_merchant_integrity_param( pMERCHANT_INTEGRITY_PARAM merchant_integrity_param_ptr, pCHAR ErrorMsg ,INT );
BYTE db_insert_merchant_integrity_param( pMERCHANT_INTEGRITY_PARAM merchant_integrity_param_ptr, pCHAR ErrorMsg ,INT);
#endif

#ifdef MC12_OCT2023
BYTE db_delete_merchant_integrity_param_for_mc(pCHAR Primary_key, pCHAR ErrorMsg );
BYTE db_insert_merchant_integrity_param( pMERCHANT_INTEGRITY_PARAM merchant_integrity_param_ptr, pCHAR ErrorMsg ,INT);
BYTE db_update_mip_for_mc(pMERCHANT_INTEGRITY_PARAM merchant_integrity_param_ptr, pCHAR ErrorMsg);

BYTE process_merchant_integrity_transactions_for_mc(pAUTH_TX auth_tx);
INT updatds_read_last_updated_date_parameter_for_MC();
INT updatds_read_execssive_delcines_parameter_for_mc();
void prepare_MIP_data_for_insert_MC(pAUTH_TX auth_tx);
void process_delcined_transactions_from_mc_cat1_cat2(pAUTH_TX auth_tx);
void process_delcined_transactions_from_mc_cat1(pAUTH_TX auth_tx);
void process_delcined_transactions_from_mc_cat2(pAUTH_TX auth_tx);

#endif

#ifdef MC13_OCT2023
INT updatds_read_merchant_advice_code_parameters();
#endif

INT  genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len);
INT  hex_to_int (pCHAR buf_ptr, INT buf_len );
BYTE db_look_for_duplicate( pTLF01 tlf01_ptr, pBYTE auth_number, pBYTE rrn );
void rollback_the_update( void );
pCHAR string_reverse( pCHAR instr );
void get_cash_bonus_ini_info(void);

BYTE db_update_tlf01_stip_void( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_tlf01_stip_reversal( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_tlf01_stip_reversal_for_cup_Acq( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_check_tlf01_stip_reversal( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_tlf01_stip_adjust( pTLF01 tlf01_ptr, pCHAR ErrorMsg );

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );

void Create_Db_Statics_And_Db_Oracle_Error_Filename( );

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);

void updateds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);

INT updateds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);

BOOLEAN Updatds_Is_MCRefund_Transaction(pTLF01 pTLF01_details);
INT pin_block_exists(pAUTH_TX p_auth_tx);
INT Check_msg_coming_from_IST(pAUTH_TX p_auth_tx);
INT blank_string(pCHAR astr, INT max_len);

BOOLEAN updatds_check_for_amex_incremental_transaction(pAUTH_TX p_auth_tx);
BOOLEAN updatds_check_for_amex_partial_reversal_transaction(pAUTH_TX p_auth_tx);
/* Cash Bonus Processing Results */
#define  CASH_WINNER     0
#define  NO_CASH_WINNER  1
#define  CASH_PROBLEM    2

/* Cash Bonus Source */
#define  CASH_BONUS_NOT_UPDATED    	0
#define  CASH_BONUS_SHARED_MEMORY  	1
#define  CASH_BONUS_DATABASE       	2
/*Card Type*/
#define CUP_CARD_NUM   			  	"6"
#define CUP_VOID_RESP_FLAG_VAL    	"VR"
#define CUP_VOID_RESP_FLAG_VAL_LEN  2
#define CUP_ORI_STAN_LEN            6

#define PAN_SEQ_NUM                 3
#define MID_MCC_LEN					4

#endif
