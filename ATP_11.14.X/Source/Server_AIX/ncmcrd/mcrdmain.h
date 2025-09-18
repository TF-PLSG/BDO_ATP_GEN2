/******************************************************************************
 *
 *	MCRDMAIN.H
 *
 *  Copyright (c) 2006, Hypercom, Inc.
 *  All Rights Reserved.
 *
 *  TITLE:  MasterCard Host Handler Macros, Constants, and common structures
 *
 *	This header file is used by all of the modules within the
 *  MasterCard host handler application.  It contains global
 *  variables, macros and function prototypes to be used by
 *  any of the application functions.
 *
 *  APPLICATION:  Advanced Transaction Processor (ATP)
 *
 *
 ******************************************************************************/

#ifndef MCRDMAIN_H
#define MCRDMAIN_H   

#define MC11_OCT2023 1

#define MC10_OCT2023 1
#define MC07_OCT2023 1
#define MC01_APR2024 1
#define MC02_APR2024 1
#define MC7_APR2024  1
/* Function Prototypes */

/*******************
 * Local Functions *
 *******************/

/* Main modules */
INT     process_incoming_message(pPTE_MSG p_msg);
INT     incoming_host_message(pPTE_MSG p_msg);
INT     incoming_host_message_timer_reply(pPTE_MSG p_msg_in);
INT     process_switched_in_msg( pPTE_MSG p_msg, pAUTH_TX p_auth_tx );
INT     process_response_of_switched_out_msg( pAUTH_TX p_auth_tx );
INT     clear_request_timer( pAUTH_TX p_auth_tx );
INT     incoming_other_host_response( pAUTH_TX );
INT     incoming_other_host_response_db_reply( pAUTH_TX );
INT     process_timeout_msg(pPTE_MSG p_msg);
INT     security_request( pAUTH_TX );
INT		perform_decrypn_encrypn_request(pAUTH_TX);
INT     security_reply( pPTE_MSG );
INT     security_reply_for_decrypn_encrypn( pPTE_MSG );
INT     incoming_terminal_request( pAUTH_TX );
INT     process_logon_response( pAUTH_TX );
INT     process_file_update_txn( pPTE_MSG p_msg );
INT     process_file_update_response( pAUTH_TX );
INT     process_neg_file( pAUTH_TX );
INT     process_neg_file_reply( pBYTE, BOOLEAN );
INT     incoming_translate_request(pPTE_MSG p_msg);
INT     StartUp();
void    EndProcess(); 

INT     perform_net_mngmnt_request(  CHAR[], pAUTH_TX );
INT     perform_net_mngmnt_response( CHAR[], pAUTH_TX );
INT     send_net_mngmnt_request( pAUTH_TX );
INT     send_net_mngmnt_response( pAUTH_TX );
INT     perform_host_logon_request( CHAR[], INT, pAUTH_TX );
void    process_network_advice( pAUTH_TX );
INT     process_auth_advices( pAUTH_TX, INT , NCF30 );

INT ncmcrd_process_reversal_transactions (pAUTH_TX p_auth_tx, INT status );

INT     perform_standin_processing( CHAR[], BYTE, pAUTH_TX );
void    perform_error_processing( pAUTH_TX, pCHAR );
void    mcrdutil_parse_db_ok(pPTE_MSG p_msg_in);
void    mcrdutil_parse_db_error(pPTE_MSG p_msg_in);
INT     mcrdutil_process_host_request( pAUTH_TX );
void    mcrdutil_get_strtime( pCHAR );
INT     mcrdutil_verify_pin_block( pAUTH_TX );
INT     mcrdutil_parse_host_response_msg();
INT     mcrdutil_send_host_response( pAUTH_TX );
INT     mcrdutil_transfer_ctl_to_auth_app( pAUTH_TX p_auth_tx );
void    mcrdutil_init_txn( pAUTH_TX );
void    mcrdutil_get_response_text( pAUTH_TX );

void    get_file_update_err_msg( pAUTH_TX );
INT	    send_request_to_host( pAUTH_TX );
INT     send_it_back_to_terminal( pCHAR, pAUTH_TX );
INT     alter_transaction_data( pAUTH_TX, pAUTH_TX );
void    build_auth_structure( pAUTH_TX );
INT     send_response_to_term_app( pAUTH_TX );
INT     send_response_to_GUI( pBYTE, pAUTH_TX );
INT     verify_host_app_status( pAUTH_TX );
BYTE    match_request_to_response( BYTE );
void    process_reject( pAUTH_TX );
void    process_0190( pAUTH_TX );
void    process_reversal_advice( pAUTH_TX );
void    process_reversal_advice_db_reply( pBYTE );
void    Determine_Auth_Msg_Type( pAUTH_TX, pAUTH_TX );
INT     verify_processing_code( pAUTH_TX );
INT     process_unsupported_txn( pAUTH_TX );
INT     validate_mcc( pAUTH_TX );
INT     send_transaction_statistics( pPTE_MSG );
INT     reset_transaction_statistics( pPTE_MSG );

pCHAR   crypto_get_key( pBYTE message, BYTE key_type );
void    copy_to_req_resp_buffer( pCHAR, INT );
INT     move_iso_response_to_auth_struct( pAUTH_TX, pBYTE );
BYTE    get_tran_type( pAUTH_TX );
void    build_host_structure( pAUTH_TX );
INT     build_host_request( pAUTH_TX );
INT     get_tran_type_2( pAUTH_TX p_auth_tx );
INT     send_network_status_to_monitor(void);
void    format_date( pCHAR );
BOOLEAN blank_string( pCHAR astr, INT max_len );
INT     perform_data_checks( pAUTH_TX );
INT     route_to_cirrus( pAUTH_TX );
INT     insert_into_TLF01( pAUTH_TX );
INT     get_orig_txn( pAUTH_TX );
INT     delete_ncf30( pNCF30 );
BYTE    get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg );
/*BYTE 	get_bdkkey01_cfg( pBDKKEY01 pbdkkey01, pCHAR error_msg );*/
BYTE    process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                         pCHAR que_name,     pCHAR error_msg );
INT     process_incoming_message_ncf30_reply(pBYTE pCurrent);
INT     delete_orig_ncf30( NCF30 ncf30, pAUTH_TX p_auth_tx );
VOID    update_auth_tx_with_ncf30_data(pNCF30 pncf30, pAUTH_TX p_auth_tx);
INT     update_database( pAUTH_TX );
INT     map_0302_to_tlf01( pAUTH_TX p_auth_tx );
INT     map_0312_to_tlf01( pAUTH_TX p_auth_tx );
void    write_to_txn_file( void );
void build_DE48_TAG37_foracq_txn(pAUTH_TX, int *);
INT     txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg );
void    reject_switched_in_txn( pAUTH_TX p_auth_tx, pBYTE resp_code );
BOOLEAN ncmcrd_check_for_acq_refund_transaction(pAUTH_TX p_auth_tx);
BOOLEAN ncmcrd_transfer_refund_response_msg_to_MC2( pAUTH_TX	 p_auth_tx );
INT insert_tlf01( pAUTH_TX p_auth_tx );
void Read_Incoming_TIMEOUT_Response_Code();
void ncmcrd_read_fai_rc_from_tf_ini();
#ifdef MC7_APR2024
void ncmcrd_read_country_code_from_tf_ini();
#endif
/* Functions external to host handler */
extern INT      process_keychange_response();
extern void     ptetime_get_timestamp( pCHAR );
extern pPTE_MSG crypto_process_request(pCHAR, pCHAR, pBYTE, INT, pBYTE,
                                       INT,    BYTE, LONG *);
extern BOOLEAN  TxUtils_Send_Msg_To_Operator(INT,pBYTE,INT,INT,pBYTE,
                                            BYTE,pBYTE,pBYTE,pBYTE,pBYTE);

void Get_Name_Validation_Parameters_FromTF_INI();

#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT(x)
#endif  



/**********************************
      GLOBAL DECLARATIONS
**********************************/

/* Constants used for message_id in the transaction records */
#define MSG_ID_AUTH_AUTHORIZATION       "0100"
#define MSG_ID_AUTH_SALE                "0200"
#define MSG_ID_AUTH_REFUND              "0200"
#define MSG_ID_AUTH_PAYMENT             "0200"
#define MSG_ID_REVERSAL                 "0400"
#define MSG_ID_AUTH_OFFLINE_SALE        "0220"
#define MSG_ID_AUTH_OFFLINE_REFUND      "0220"
#define MSG_ID_AUTH_SALE_ADJUSTMENT     "0220"
#define MSG_ID_AUTH_REFUND_ADJUSTMENT   "0220"
#define MSG_ID_AUTH_PRE_AUTHORIZATION   "0100"
#define MSG_ID_AUTH_CASH_ADVANCE        "0200"
#define MSG_ID_AUTH_BALANCE_INQUIRY     "0100"
#define MSG_ID_AUTH_VOID_SALE           "0200"
#define MSG_ID_AUTH_VOID_REFUND         "0200"

#define MSG_ID_AUTH_LOGON               "0800"
#define MSG_ID_AUTH_TEST                "0800"

/* These are not used by the data server.  They
 * are used by the host handler to determine
 * the source of the request.
 */
#define ST2_INCOMING_OPERATOR           153
#define ST2_CRYPTO_TRANSLATION          154
#define ST2_CRYPTO_CVC                  155
#define ST2_NEG_FILE                    157
#define ST2_NCF30_INSERT                158

/* Constants used for processing LOGONs */
#define  LOGON_NMI_BIN            "001"
#define  LOGON_NMI_BIN_LOGOFF     "002"
#define  LOGON_SAF_REQUEST        "060"
#define  LOGON_NMI_GROUP_ID       "061"
#define  LOGON_NMI_GROUP_LOGOFF   "062"
#define  ECHO_NMI                 "270"
#define  LOGON_REQUEST            "LOGON"
#define  LOGOFF_REQUEST           "LOGOFF"
#define  LOGOFF_RESPONSE          "LOGOFF RESPONSE"
#define  LOGON_RESPONSE           "LOGON RESPONSE"
#define  ECHO_REQUEST             "ECHO"
#define  ECHO_RESPONSE            "LOGON ECHO RESPONSE"
#define  ECHO_TIMER_KEY           "LOGON ECHO TIMER KEY"
#define  SAF_REQUEST              "SAF REQUEST"
#define  SOURCE_IS_HOST_HANDLER    1

/* Miscellaneous */
#define  NOT_OK  0
#define  OK      1
#define  NO      0
#define  YES     1
#define  NOT_FOUND 0
#define  FOUND     1
#define  LOGON_TIMER_LOOKUP_KEY   "12233322"
#define  ARTIFICIAL_TERMINAL_ID   "12233322"

#define  COMM_BUF_SZE    1500
#define  EXTENDED_BITFIELD 128
#define  NORMAL_BITFIELD    64


/* MCC Validation Control */
#define  MCC_VALIDATION_OFF 0
#define  MCC_VALIDATION_ON  1

/* Merchant Category ranges for txns without track data.
 * Transactions without track data will be declined if
 * MCC is not within these ranges.
 */
#define  MCC_RANGE1_LOW   3000
#define  MCC_RANGE1_HIGH  3768

#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969

#define  AUTO_FUEL_MCC   "5542" /* Automated fuel dispenser Merchant cat code */
#define  ATM_MCC         "6011" /* All ATMs should have this MCC */
#define  MCC_QUASI_CASH  "7995"

#define  SWITCHED_IN   0
#define  SWITCHED_OUT  1

#define CARD_NOT_PRESENT_MC_DE61_SE5 '1'
#define MC03_OCT2023 1
/* Set optional functionality here.
 * Set to YES if you want the functionality,
 * else set to NO.
 */
#define  DO_NEG_FILE_PROCESSING  NO


/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"

/* Response Codes */
#define  APPROVED      "00"
#define  CALL_ISSUER   "01"
#define  DO_NOT_HONOR  "05"
#define  FORMAT_ERROR  "30"
#define  INVALID_PIN   "55"
#define  NOT_PERMITTED "57"

/* Supported processing codes */
#define  PURCHASE           "00"
#define  WITHDRAWAL         "01"
#define  CASH_DISBURSEMENT  "17"
#define  BALANCE_INQUIRY    "30"
#define	 VOID_SALE			"02"  // TF-Ajay 03 Nov,2009 09-76
#define	 SALEPROCCODE    "000000"
#define  MONEY_SEND			"28"	
#define  AUTHORIZATION_REFUND 		"20"


/* E-commerce POS Condition Code */
#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"

/* Verify processing code () uses this value TF-Ajay 03 Nov,2009 */
#define MAX_PROC_CODE  11

/* Data Element 48 subfield definitions */

/*+--------------------------------------------------------------------------+
  | Tag 42                                                                   |
  | Length 07                                                                |
  | 01 = Identifies the electronic commerce level of security                |
  | 03 = Length of data to follow in sub field 01                            |
  |  2 = Security Protocol = Channel encryption                              |
  |  1 = Cardholder certificate not used.                                     |
  |  x = Universal Cardholder Authentication Field (UCAF) indicator          |
  |      This position is variable depending upon the merchant.              |
  |      Where x =                                                           |
  |         0 = UCAF data collection is not supported at Merchant's web site |
  |         1 = UCAF is supported by merchant, but UCAF was not populated    |
  |         2 = UCAF is supported, UCAF is populated; subelement 43 present  |
  |      Since this last value is variable, it is not part of the #define.   |
  +--------------------------------------------------------------------------+*/
#define  DE48_TAG42    		"4207010321"   		// Tag 42 defined above
#define  DE48_TAG41         "41300126"
#define  DE48_TAG43   		"43"           		// Tag 43, fixed length -> 32 AN characters
#define  DE48_TAG43_LEN 	32
#define  DE48_TAG84    		"8402"         		// Tag 84, length of 2
#define  DE48_TAG87    		"8701Y"       		// Tag 87, length of 1, value=Y
#define  DE48_TAG92    		"92"           		// Tag 92 (CVC2) length depends on card type
#define  DE48_TAG87_NP 		"8701P"		   		// For NOT CVC2 Not proceed - Tf Phani
#define  DE48_TAG23    		"2302"         		// Tag 23, length of 2, value=00 for CC tx
#define  DE48_TAG20    		"2001S"        		// Tag 20, length of 1, value=S
#define  DE48_TAG22_SF06    "22050601A"           //april mnd 2023. Value A for now, Since we are not building DE122 in acquiring. Can be 'B' later on.
#define  DE48_TAG63   		"6315"
#define  DE48_TAG65   		"6502"		    	// Tag 65 , length 02 , value == AB A is for TLE Compliant Certified  and B UKPT/DUKPT Compliant Certified.
#define  DE48_TAG67   		"67070103"	    	// Tag 67 , Length 07 , Subfeild 01, length 03 data 000 - 999.
#define  DE48_TAG61    		"610501001"     	// Tag 61, Length 05, Subfiled 5 values 1 indicate Final Authorization, rest subfileds are having default value
#define  DE48_TAG71     	"71040105"      	// Tag 71 , Length 04, Subfiled 01 , Value 05 for MC Secure code AAVS.
#define  DE48_TAG71_MSBS 	"7107013302W"   	// Tag 71 , Length 07, Subfiled 01 , Value 33 for MoneySend Blocking Service
																// Subfiled 02 , Value W for MoneySend Blocking Service
#define DE48_TAG71_SF2		"710302"        	// Tag 71, len 4, SF2
#define DE48_TAG21_SF1      "210301"				// Tag 21 , leg 2 SF1
#define DE48_TAG21_SF1_FOR_MPOS      "210501011"
#define DE48_TAG41_SF1      "41270126"
#define DE48_TAG21      	"21"				// Tag 21 , leg 2 SF1
#define DE48_TAG66	        "66"
#define DE48_TAG50	        "50"
#define DE48_TAG75			"75"
#define DE48_TAG77			"7703"			// Tag 77, Len 3, Value C04 for gaming - repay
#define DE48_TAG80		 	"8002"
#define DE48_TAG87_CVC2_MATCH 'M'
#define DE48_TAG87_CVC2_NO_MATCH 'N'
#define DE48_TAG87_CVC2_NOT_PROCESSED  'P'

#define  DE48_TAG71_LEN      	8
#define  DE48_TAG71_MSBS_LEN    11
#define  DE48_TAG71_SF2_LEN     6
#define  DE48_TAG21_SF1_LEN  	9
#define  DE48_TAG66_LEN 		2
/* These two are set by the switch.  We only get them on
 * switched in transactions.  If present in a request,
 * they must be echoed in the response.
 */
#define  DE48_TAG37    "370704"  //Tag 37, length 07, subelement tag 04
#define  DE48_TAG37_SF05   "37150511"
#define  DE48_TAG88    "8801"
#define  DE48_TAG89    "8901"
#define  DE48_TAG32    "3206"
/* Installement data*/
#define  DE48_TAG95    "95"         // TAG 95
#define  DE48_TAG95_INST_CODE "PHINST"  // Phillipins installement Code

#define  DE48_TAG48    "48"

#define  DE112_TAG009 "009"   
#define  DE112_TYPE_OF_INST "21"
#define  DE112_TAG009_LEN 36
#define  DE48_TAG22    "22"
#define  DE48_TAG57 "57"
#define  DE48_TAG22SF01 "01"
#define  DE48_TAG22SF02 "02"
#define  DE48_TAG22SF03  "03"
#define  DE48_TAG22SF04  "04"
#define  DE48_TAG22SF05  "05"
#define  DE48_TAG22SF06  "06"
#define  DE48_TAG56		 "56"


#define  INVALID_CVC   'Y'

#define MC_INCOMING_TIMEOUT_TRANSACTION_RESPONSE_CODE "01"
/* UCAF Indicator values for DE48, tag 42 */
#define  UCAF_NOT_SUPPORTED                "0"
#define  UCAF_SUPPORTED_BUT_NOT_POPULATED  "1"
#define  UCAF_SUPPORTED_AND_POPULATED      "2"  /* Requires tag 43 */

#define  MCO_LENGTH 3


#define DE108_SE01  "01"
#define DE108_SE02  "02"
#define DE108_SE03  "03"
#define DE108_SE03_SF01 "01"
#define DE108_SE03_SF03 "03"
#define DE108_SE03_SF05 "05"
#define DE108_SE07  "07"
#define DE108_SE08  "08"

#define DE105_SE01  "001"

#endif
