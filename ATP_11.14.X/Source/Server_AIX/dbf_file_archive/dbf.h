/*******************************************************************************
*  
* MODULE:           settle.h
*
* Copyright (c) 2006, Hypercom, Inc. All Rights Reserved.   
*  
* TITLE:            Visa Outgoing Center Transaction files creator . 
*  
* DESCRIPTION:      This module allows to create Settlement files
*                   for particular day and time.  
*
* APPLICATION:      cd_settle_file.exe
*
* AUTHOR:           Irene Goldfild
*
* REVISION HISTORY:
*
* 
   initial release
*
*******************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  


#define DBF_FILE_INI "dbf_file.ini"
#define SWIPE_CARD   "0022"
#define MANUAL_ENTRY "0012"

#define EXCEPTION_BASE_NAME  "no_exceptions"
#define EXCEPTION_DAYS_BACK  45

#define FILLERCHAR		' '
#define SEPERATORCHAR	'|'
/*** Common structure for current terminal batch ***/

#define CURRENCY_CODE_LEN 3

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount) ;
void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character) ;

// DBF TLF01
typedef struct
{
   char transaction_id                     [21]; 
   char entry_type                         ;
   char tpdu_id                            [16];
   char issuer_id                          [21];
   char acquirer_id                        [21];
   char tx_key                             ;
   char terminal_id                        [9];
   char terminal_type                      [3];
   char response_text                      [41];
   char card_num_len                       [3];
   char card_num                           [65];
   char exp_date                           [5];
   char total_amount                       [13];
   char add_amounts                        [25];
   char invoice_number                     [9];
   char orig_amount                        [13];
   char auth_number                        [7];
   char processing_code                    [7];
   char sys_trace_audit_num                [7];
   char orig_sys_trace_num                 [7];
   char pos_entry_mode                     [5];
   char nii                                [5];
   char pos_condition_code                 [3];
   char track1                             [77];
   char track2                             [38];
   char retrieval_ref_num                  [13];
   char orig_retrieval_ref_num             [13];
   char merchant_id                        [16];
   char orig_message                       [5];
   char settlement_total                   [17];
   char message_type                       [5];
   char card_holder_name                   [31];
   char general_status                     ;
   char originator_queue                   [17];
   char originator_info                    [33];
   char batch_number                       [7];
   char response_code                      [3];
   char date_yyyymmdd                      [9];
   char time_hhmmss                        [7];
   char num_sales                          [4];
   char sales_amount                       [13];
   char num_refunds                        [4];
   char refund_amount                      [13];
   char num_dbsales                        [4];
   char dbsales_amount                     [13];
   char num_dbrefunds                      [4];
   char dbrefund_amount                    [13];
   char pin_block                          [19];
   char tax_amount                         [13];
   char tip_amount                         [13];
   char tran_start_time                    [17];
   char host_start_time                    [17];
   char host_finish_time                   [17];
   char tran_finish_time                   [17];
   char settle_file_prefix                 [7];
   char resp_source_len                    [3];
   char visa_char_indicator                ;
   char visa_tran_id                       [16];
   char visa_validation_code               [5];
   char mcard_banknet_len                  [4];
   char mcard_banknet                      [51];
   char agent_id_len                       [4];
   char agent_id                           [12];
   char cvc                                ;
   char settlement_date                    [5];
   char category_code                      [5];
   char conversion_rate                    [10];
   char product_code                       [9];
   char odometer                           [8];
   char organization_id                    [5];
   char dynamic_keychange                  ;
   char source_key                          [49];
   char txn_cnt                            [4];
   char dcf01_retired_cwk                  [49];
   char dest_key                           [49];
   char ncf01_retired_cwk                  [49];
   char handler_queue        		       [8];
   char authorizing_host_queue		   	   [8];
   char max_net_consec_tmouts              [4];
   char max_active_txns                    [4];
   char forwarding_institution_id          [12];
   char remote_nsp_type                    [2];
   char kek                                [17]; 
   char acquiring_id                       [12];
   char outstanding_balance                [13]; 
   char credit_limit                       [13];
   char nfi_name                           [11];
   char nfi_seq_nbr                        [7];
   char nfi_start_date                     [9];
   char nfi_end_date                       [9];
   char resp_source                        [26];
   char origin                             [3];
   char auth_1                             [3];
   char auth_2                             [3];
   char update_mask                        [2];
   char type_of_data                       [3];
   char down_payment                       [13];
   char period                             [3];
   char interest_rate                      [5];
   char total_interest                     [13];
   char redemption_amount                  [13];
   char vehicle_number                     [9];
   char profile                                    [3];
   char nbr_of_prod_codes                          [3];
   char product1_code                              [7];
   char product1_quantity                          [3];
   char product1_amount                            [13];
   char product2_code                              [7];
   char product2_quantity                          [3];
   char product2_amount                            [13];
   char product3_code                              [7];
   char product3_quantity                          [3];
   char product3_amount                            [13];
   char product4_code                              [7];
   char product4_quantity                          [3];
   char product4_amount                            [13];
   char product5_code                              [7];
   char product5_quantity                          [3];
   char product5_amount                            [13];
   char product6_code                              [7];
   char product6_quantity                          [3];
   char product6_amount                            [13];
   char product7_code                              [7];
   char product7_quantity                          [3];
   char product7_amount                            [13];
   char product8_code                              [7];
   char product8_quantity                          [3];
   char product8_amount                            [13];
   char product9_code                              [7];
   char product9_quantity                          [3];
   char product9_amount                            [13];
   char product10_code                              [7];
   char product10_quantity                          [3];
   char product10_amount                            [13];
   char product11_code                              [7];
   char product11_quantity                          [3];
   char product11_amount                            [13];
   char product12_code                              [7];
   char product12_quantity                          [3];
   char product12_amount                            [13];
   char product13_code                              [7];
   char product13_quantity                          [3];
   char product13_amount                            [13];
   char product14_code                              [7];
   char product14_quantity                          [3];
   char product14_amount                            [13];
   char product15_code                              [7];
   char product15_quantity                          [3];
   char product15_amount                            [13];
   char product16_code                              [7];
   char product16_quantity                          [3];
   char product16_amount                            [13];
   char product17_code                              [7];
   char product17_quantity                          [3];
   char product17_amount                            [13];
   char product18_code                              [7];
   char product18_quantity                          [3];
   char product18_amount                            [13];
   char product19_code                              [7];
   char product19_quantity                          [3];
   char product19_amount                            [13];
   char product20_code                              [7];
   char product20_quantity                          [3];
   char product20_amount                            [13];
   char visa_merchant_id                            [16];
   char cash_bonus                                  [2];  /* added 01-29-99 by IG */
   char deferred_factor                             [8];  /* added 01-29-99 by IG */
   char deferred_term_length                        [4];  /* added 01-29-99 by IG */
   char currency_code                               [4];  /* added 01-29-99 by IG */
   char transmission_timestamp                      [11]; /* added 01-29-99 by IG */
   char tran_fee_amount                             [9];  /* added 01-29-99 by IG */
   char cvc_data                                    [4];  /* added 01-29-99 by IG */
   char verify_mag_stripe                           [2];  /* added 02-09-99 by IG */
   char saf                                         [2];  /* added 02-09-99 by IG */
   char security_response_code                      [3];  /* added 02-09-99 by IG */
   char service_code                                [4];  /* added 02-09-99 by IG */
   char source_pin_format                           [3];  /* added 02-09-99 by IG */
   char dest_pin_format                             [3];  /* added 02-09-99 by IG */
   char voice_auth                                  [2];  /* added 02-09-99 by IG */
   char actual_amount                               [13]; /* AP IG, Equitable 02-10-99 */
   char reversal_amount                             [13]; /* AP IG, Equitable 02-10-99 */
   char voice_auth_text                             [241]; /* IG 02-22-99 */ 
   char operator_id                                 [21];  /* IA 03-09-99 */
   char workstation                                 [21];  /* IA 03-16-99 */
   char orig_auth_number                            [7];   /* IA 04-08-99 */
   char ticket_nbr									[16];
   char system_date                                 [9];
   char def_gross_amt                               [13];
   char monthly_amort_amt                           [13];
   char purge_date									[9];
}DBF_TLF01, *pDBF_TLF01;


// WORKING_DBF TLF01
typedef struct
{
   char transaction_id                     [21];			//1		//1 
   char entry_type                         ;				//2  	//2 
   char acquirer_id                        [21];			//5  	//3 
   char terminal_id                        [9];				//7  	//4 
   char response_text                      [41];			//9  	//5 
   char card_num_len                       [3];				//10 	//6 
   char card_num                           [65];			//11 	//7 
   char exp_date                           [5];				//12 	//8 
   char total_amount                       [13];			//13 	//9 
   char orig_amount                        [13];			//16 	//10
   char auth_number                        [7];				//17 	//11
   char processing_code                    [7];				//18 	//12
   char sys_trace_audit_num                [7];				//19 	//13
   char orig_sys_trace_num                 [7];				//20 	//14
   char pos_entry_mode                     [5];				//21 	//15
   char pos_condition_code                 [3];				//23 	//16
   char retrieval_ref_num                  [13];			//26 	//17
   char orig_retrieval_ref_num             [13];			//27 	//18
   char merchant_id                        [16];			//28 	//19
   char orig_message                       [5];				//29 	//20
   char settlement_total                   [17];			//30 	//21
   char message_type                       [5];				//31 	//22
   char card_holder_name                   [31];			//32 	//23
   char general_status                     [2];				//33 	//24
   char batch_number                       [7];				//36 	//25
   char response_code                      [3];				//37 	//26
   char date_yyyymmdd                      [9];				//38 	//27
   char time_hhmmss                        [7];				//39 	//28
   char tran_start_time                    [17];			//51 	//29
   char host_start_time                    [17];			//52 	//30
   char host_finish_time                   [17];			//53 	//31
   char tran_finish_time                   [17];			//54 	//32
   char agent_id                           [12];			//63 	//33
   char cvc                                ;				//64 	//34
   char settlement_date                    [5];				//65 	//35
   char category_code                      [5];				//66 	//36
   char conversion_rate                    [10];			//67 	//37
   char product_code                       [9];				//68 	//38
   char acquiring_id                       [12];			//84 	//39
   char outstanding_balance                [13]; 			//85 	//40
   char credit_limit                       [13];			//86 	//41
   char origin                             [3];				//92 	//42
   char interest_rate                      [5];				//99 	//43
   char total_interest                     [13];			//100	//44
   char profile                            [3];				//103	//45
   char nbr_of_prod_codes                  [3];				//104	//46
   char product1_code                      [7];				//105	//47
   char product1_quantity                  [3];				//106	//48
   char product1_amount                    [13];			//107	//49
   char visa_merchant_id                   [16];			//165	//50
   char deferred_factor                    [8];  			//167	//51
   char deferred_term_length               [4];  			//168	//52
   char currency_code                      [4];  			//169	//53
   char transmission_timestamp             [11]; 			//170	//54
   char cvc_data                           [4];  			//172	//55
   char verify_mag_stripe                  [2];  			//173	//56
   char security_response_code             [3];  			//175	//57
   char service_code                       [4];  			//176	//58
   char voice_auth                         [2];  			//179	//59
   char actual_amount                      [13]; 			//180	//60
   char reversal_amount                    [13]; 			//181	//61
   char voice_auth_text                    [241];			//182	//62
   char operator_id                        [21]; 			//183	//63
   char orig_auth_number                   [7];  			//185	//64
   char system_date                        [9];				//187	//65
   char monthly_amort_amt                  [13];			//189	//66
}WORKING_DBF_TLF01;


/*typedef struct
{
	char tran_id      				[21];
	char sys_date                	[9];
	char card_brand              	[11];
	char cadencie_link           	[11];
	char tran_req_sent_time      	[11];
	char tran_resp_recv_time     	[11];
	char tran_req_sent_mode    		[3];
	char tran_resp_recv_mode      	[3];
	char response_text           	[101];
	char decision_code_1_value   	[11];
	char decision_code_2_value   	[11];
	char decision_code_3_value   	[11];
	char decision_code_4_value   	[11];
	char decision_code_5_value   	[11];
	char decision_code_6_value   	[11];
	char decision_code_7_value   	[11];
	char decision_code_8_value   	[11];
	char decision_code_9_value  	[11];
	char decision_code_10_value  	[11];
	char raw_data    				[1201];
}FTLF01, *pFTLF01;*/
//
/*** Function Prototypes ***/
void MainProcessor(void);
void EndProcess(void);
BOOLEAN get_currency_code(void);
void returnCurrentTime(char *currentTime);

void    get_date( pCHAR time_date, pCHAR date_str );

INT     db_get_except_record_cnt( pCHAR ErrorMsg );
INT     db_process_bch10_records( pCHAR ErrorMsg );
INT     db_process_bch10_except_records( pCHAR ErrorMsg );
INT     db_insert_bch11_delete_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg );
BYTE    db_select_curr_code( pCHAR   curr_code, pCHAR  device_type, 
                             pDCF01  dcf01_ptr, pCHAR  ErrorMsg );

BYTE db_get_bon03_service_list( pCHAR       tran_id,
                                pBON03_LIST bon03_list,
                                pCHAR       ErrorMsg );

BOOLEAN CreateDBF(char *filename);

BOOLEAN WriteDBFHeader();

BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** fptr, char *header);
BOOLEAN WriteChar(char c);
BOOLEAN WriteTLF01TableIntoDBF(DBF_TLF01 *pte_tlf01);

BOOLEAN CreateTSV(char *filename);
BOOLEAN WriteTSVHeader();
BOOLEAN WriteTSVFooter(long lTotalRecords);
BOOLEAN WriteTLF01TableIntoTSV(DBF_TLF01 *pte_tlf01, FTLF01 *pte_ftlf01,PTSN01 *pte_ptsn01,char *Local_kek);

BOOLEAN dbf_decrypt_file(char *filename);
double Char2Hex(char *lpHex);
void date_to_string(pCHAR datestr);

BOOLEAN GetArchiveServiceName  ( pCHAR app_srvc_name );
BOOLEAN GetArchiveUserId       ( pCHAR app_user_id );
BOOLEAN GetArchivePassword     ( pCHAR app_password );
BYTE dbcommon_connect_Archds    ( pCHAR username, pCHAR password,  pCHAR alias, pCHAR database, pCHAR ErrorMsg );
void RemoveChar(char *string, int len, char ch);
void RemoveNonAlplaNumSpaceSpecail(char *string, int len);
void PrepareTranType (DBF_TLF01 *pte_tlf01, CHAR* TRAN_TYPR);
INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename( );
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
void dbf_arch_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);
INT  dbf_arch_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);


INT isalphanum(pCHAR string);
