
#define RT_RESPONSE  1
#define RT_TIMEOUT   2
#define RT_SYSTEM    3
#define RT_UNKOWN	 -1

#define RSP_APP_DATA_LEN				8
#define RSP_EXT_HDR_LEN					4
#define RSP_TRAN_CODE_LEN				9
#define RSP_SOURCE_LEN					10
#define RSP_DEST_LEN					10
#define RSP_ERROR_LEN					10
#define RSP_FILLER_LEN					1
#define RSP_HDR_SPACES_LEN				17
#define RSP_TR_RQ_TIME					RSP_HDR_SPACES_LEN+6

#define RSP_RCRD_VER_LEN				1
#define RSP_SCR_CNT_LEN					2
#define RSP_SCORE_NAME_LEN				22
#define RSP_ERROR_CODE_LEN				4
#define RSP_SCORE_LEN					4
#define RSP_REASON_LEN					4
#define RSP_SEGMENT_ID_LEN				8
#define RSP_FILLER1_1_3_LEN				2
#define RSP_FILLER1_2_LEN				4
#define RSP_FILLER2_LEN					4
#define RSP_FILLER3_LEN					4
#define RSP_DECISION_COUNT_LEN			2
#define RSP_DECISION_TYPE_LEN			32
#define RSP_DECISION_CODE_LEN			32
#define RSP_SCORING_SERVER_ID_LEN		4

#define RT_PASS_A					"Approve                         "
#define RT_PASS_a					"approve                         "
#define	RT_FAIL_D					"Decline                         "
#define	RT_FAIL_d					"decline                         "
#define FT_APPROVED_CODE_LEN		7
#define FT_FAIL_CODE_LEN			7

#define LOOKUP_KEY_SIZE					35
#define TRNID_LEN_BEFOREDECIMAL			16
#define TRNID_LEN_AFTERDECIMAL			17
#define TRNDID_AFTERDECIMAL				3
#define UNIQ_ID_LEN_FROM_RSPMSG			31
#define TRANSCN_ID_LEN					19
#define RRN_LEN							12
#define FALCON_RT_HDR_LEN				101


#define  COMM_BUF_SZE            1500

/* TIMER TYPES */
#define  REQUEST_TIMER  0
#define  ECHO_TIMER     1

/* TIMER APPS */
#define  TIMER_REVERSAL   "Reversal"
#define  TIMER_FINANCIAL  "Financial"
#define  TIMER_08X0       "0800"
#define  TIMER_ECHO       "Echo"

INT FALCON_read_CUP_merchant_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_1_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_2_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_3_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_4_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_5_indicator(void);
INT FALCON_read_CUP_MP_Bin_range_len_1(void);
INT FALCON_read_CUP_MP_Bin_range_len_2(void);
INT FALCON_read_CUP_MP_Bin_range_len_3(void);
INT FALCON_read_CUP_MP_Bin_range_len_4(void);
INT FALCON_read_CUP_MP_Bin_range_len_5(void);

BYTE Falcon_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize);

void FALCON_build_request_HeaderFile();
void FALCON_build_request_field_1( );
void FALCON_build_request_field_2( );
void FALCON_build_request_field_3( );
void FALCON_build_request_field_4( );
void FALCON_build_request_field_5( );
void FALCON_build_request_field_6( );
void FALCON_build_request_field_7( );
void FALCON_build_request_field_8( );
void FALCON_build_request_field_9( );
void FALCON_build_request_field_10( );
void FALCON_build_request_field_11( );
void FALCON_build_request_field_12( );
void FALCON_build_request_field_13( );
void FALCON_build_request_field_14( );
void FALCON_build_request_field_15( );
void FALCON_build_request_field_16( );
void FALCON_build_request_field_17( );
void FALCON_build_request_field_18( );
void FALCON_build_request_field_19( );
void FALCON_build_request_field_20( );
void FALCON_build_request_field_21( );
void FALCON_build_request_field_22( );
void FALCON_build_request_field_23( );
void FALCON_build_request_field_24( );
void FALCON_build_request_field_25( );
void FALCON_build_request_field_26( );
void FALCON_build_request_field_27( );
void FALCON_build_request_field_28( );
void FALCON_build_request_field_29( );
void FALCON_build_request_field_30( );
void FALCON_build_request_field_31( );
void FALCON_build_request_field_32( );
void FALCON_build_request_field_33( );
void FALCON_build_request_field_34( );
void FALCON_build_request_field_35( );
void FALCON_build_request_field_36( );
void FALCON_build_request_field_37();
void FALCON_build_request_field_38( );
void FALCON_build_request_field_39( );
void FALCON_build_request_field_40( );
void FALCON_build_request_field_41( );
void FALCON_build_request_field_42( );
void FALCON_build_request_field_43( );
void FALCON_build_request_field_44( );
void FALCON_build_request_field_45( );
void FALCON_build_request_field_46( );
void FALCON_build_request_field_47( );
void FALCON_build_request_field_48( );
void FALCON_build_request_field_49( );
void FALCON_build_request_field_50( );
void FALCON_build_request_field_51( );
void FALCON_build_request_field_52( );
void FALCON_build_request_field_53( );
void FALCON_build_request_field_54( );
void FALCON_build_request_field_55( );
void FALCON_build_request_field_56( );
void FALCON_build_request_field_57( );
void FALCON_build_request_field_58( );
void FALCON_build_request_field_59( );
void FALCON_build_request_field_60( );
void FALCON_build_request_field_61( );
void FALCON_build_request_field_62( );
void FALCON_build_request_field_63( );
void FALCON_build_request_field_64( );
void FALCON_build_request_field_65( );
void FALCON_build_request_field_66( );
void FALCON_build_request_field_67( );
void FALCON_build_request_field_68( );
void FALCON_build_request_field_69( );
void FALCON_build_request_field_70( );
void FALCON_build_request_field_71( );
void FALCON_build_request_field_72( );
void FALCON_build_request_field_73( );
void FALCON_build_request_field_74( );
void FALCON_build_request_field_75( );
void FALCON_build_request_field_76( );
void FALCON_build_request_field_77( );
void FALCON_build_request_field_78( );
void FALCON_build_request_field_79( );
void FALCON_build_request_field_80( );
void FALCON_build_request_field_81( );
void FALCON_build_request_field_82( );
void FALCON_build_request_field_83( );
void FALCON_build_request_field_84( );
void FALCON_build_request_field_85( );
void FALCON_build_request_field_86( );
void FALCON_build_request_field_87( );
void FALCON_build_request_field_88( );
void FALCON_build_request_field_89( );
void FALCON_build_request_field_90( );
void FALCON_build_request_field_91( );
void FALCON_build_request_field_92( );
void FALCON_build_request_field_93( );
void FALCON_build_request_field_94( );
void FALCON_build_request_field_95( );
void FALCON_build_request_field_96( );
void FALCON_build_request_field_97( );
void FALCON_build_request_field_98( );
void FALCON_build_request_field_99( );
void FALCON_build_request_field_100( );
void FALCON_build_request_field_101( );
void FALCON_build_request_field_102( );
void FALCON_build_request_field_103( );
void FALCON_build_request_field_104( );
void FALCON_build_request_field_105( );
void FALCON_build_request_field_106( );
void FALCON_build_request_field_107( );
void FALCON_build_request_field_108( );
void FALCON_build_request_field_109( );
void FALCON_build_request_field_110( );
void FALCON_build_request_field_111( );
void FALCON_build_request_field_112( );
void FALCON_build_request_field_113( );
void FALCON_build_request_field_114( );
void FALCON_build_request_field_115( );
void FALCON_build_request_field_116( );
void FALCON_build_request_field_117( );
void FALCON_build_request_field_118( );
void FALCON_build_request_field_119( );
void FALCON_build_request_field_120( );
void FALCON_build_request_field_121( );
void FALCON_build_request_field_122( );
void FALCON_build_request_field_123( );
void FALCON_build_request_field_124( );
void FALCON_build_request_field_125( );
void FALCON_build_request_field_126( );
void FALCON_build_request_field_127( );
void FALCON_build_request_field_128( );
void FALCON_build_request_field_129( );
void FALCON_build_request_field_130( );
void FALCON_build_request_field_131( );
void FALCON_build_request_field_132( );
void FALCON_build_request_field_133( );
void FALCON_build_request_field_134( );
void FALCON_build_request_field_135( );
void FALCON_build_request_field_136( );
void FALCON_build_request_field_137( );
void FALCON_build_request_field_138( );
void FALCON_build_request_field_139( );
void FALCON_build_request_field_140( );
void FALCON_build_request_field_141( );
void FALCON_build_request_field_142( );
void FALCON_build_request_field_143( );
void FALCON_build_request_field_144( );
void FALCON_build_request_field_145( );
void FALCON_build_request_field_145();
#define MERCHANT_ID_SIZE 16
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

void format_date(pCHAR temp);
INT validate_date_yyyymmdd( pBYTE date_str);

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount);

void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character);

void falcon_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT falcon_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail);
void falcon_create_Error_Filename();
void falcon_get_error_warning_file_name_path(void);

void RT_FALCON_build_request_field_1();
void RT_FALCON_build_request_field_2( );
void RT_FALCON_build_request_field_3( );
void RT_FALCON_build_request_field_4( );
void RT_FALCON_build_request_field_5( );
void RT_FALCON_build_request_field_6( );
void RT_FALCON_build_request_field_7( );
void RT_FALCON_build_request_field_8( );
void RT_FALCON_build_request_field_9( );
void RT_FALCON_build_request_field_10( );
void RT_FALCON_build_request_field_11( );
void RT_FALCON_build_request_field_12( );
void RT_FALCON_build_request_field_13( );
void RT_FALCON_build_request_field_14( );
void RT_FALCON_build_request_field_15( );
void RT_FALCON_build_request_field_16( );
void RT_FALCON_build_request_field_17( );
void RT_FALCON_build_request_field_18( );
void RT_FALCON_build_request_field_19( );
void RT_FALCON_build_request_field_20( );
void RT_FALCON_build_request_field_21( );
void RT_FALCON_build_request_field_22( );
void RT_FALCON_build_request_field_23( );
void RT_FALCON_build_request_field_24( );
void RT_FALCON_build_request_field_25( );
void RT_FALCON_build_request_field_26( );
void RT_FALCON_build_request_field_27( );
void RT_FALCON_build_request_field_28( );
void RT_FALCON_build_request_field_29( );
void RT_FALCON_build_request_field_30( );
void RT_FALCON_build_request_field_31( );
void RT_FALCON_build_request_field_32( );
void RT_FALCON_build_request_field_33( );
void RT_FALCON_build_request_field_34( );
void RT_FALCON_build_request_field_35( );
void RT_FALCON_build_request_field_36( );
void RT_FALCON_build_request_field_37( );
void RT_FALCON_build_request_field_38( );
void RT_FALCON_build_request_field_39( );
void RT_FALCON_build_request_field_40( );
void RT_FALCON_build_request_field_41( );
void RT_FALCON_build_request_field_42( );
void RT_FALCON_build_request_field_43( );
void RT_FALCON_build_request_field_44( );
void RT_FALCON_build_request_field_45( );
void RT_FALCON_build_request_field_46( );
void RT_FALCON_build_request_field_47( );
void RT_FALCON_build_request_field_48( );
void RT_FALCON_build_request_field_49( );
void RT_FALCON_build_request_field_50( );
void RT_FALCON_build_request_field_51( );
void RT_FALCON_build_request_field_52( );
void RT_FALCON_build_request_field_53( );
void RT_FALCON_build_request_field_54( );
void RT_FALCON_build_request_field_55( );
void RT_FALCON_build_request_field_56( );
void RT_FALCON_build_request_field_57( );
void RT_FALCON_build_request_field_58( );
void RT_FALCON_build_request_field_59( );
void RT_FALCON_build_request_field_60( );
void RT_FALCON_build_request_field_61( );
void RT_FALCON_build_request_field_62( );
void RT_FALCON_build_request_field_63( );
void RT_FALCON_build_request_field_64( );
void RT_FALCON_build_request_field_65( );
void RT_FALCON_build_request_field_66( );
void RT_FALCON_build_request_field_67( );
void RT_FALCON_build_request_field_68( );
void RT_FALCON_build_request_field_69( );
void RT_FALCON_build_request_field_70( );
void RT_FALCON_build_request_field_71( );
void RT_FALCON_build_request_field_72( );
void RT_FALCON_build_request_field_73( );
void RT_FALCON_build_request_field_74( );
void RT_FALCON_build_request_field_75( );
void RT_FALCON_build_request_field_76( );
void RT_FALCON_build_request_field_77( );
void RT_FALCON_build_request_field_78( );
void RT_FALCON_build_request_field_79( );
void RT_FALCON_build_request_field_80( );
void RT_FALCON_build_request_field_81( );
void RT_FALCON_build_request_field_82( );
void RT_FALCON_build_request_field_83( );
void RT_FALCON_build_request_field_84( );
void RT_FALCON_build_request_field_85( );
void RT_FALCON_build_request_field_86( );
void RT_FALCON_build_request_field_87( );
void RT_FALCON_build_request_field_88( );
void RT_FALCON_build_request_field_89( );
void RT_FALCON_build_request_field_90( );
void RT_FALCON_build_request_field_91( );
void RT_FALCON_build_request_field_92( );
void RT_FALCON_build_request_field_93( );
void RT_FALCON_build_request_field_94( );
void RT_FALCON_build_request_field_95( );
void RT_FALCON_build_request_field_96( );
void RT_FALCON_build_request_field_97( );
void RT_FALCON_build_request_field_98( );
void RT_FALCON_build_request_field_99( );
void RT_FALCON_build_request_field_100( );
void RT_FALCON_build_request_field_101( );
void RT_FALCON_build_request_field_102( );
void RT_FALCON_build_request_field_103( );
void RT_FALCON_build_request_field_104( );
void RT_FALCON_build_request_field_105( );
void RT_FALCON_build_request_field_106( );
void RT_FALCON_build_request_field_107( );
void RT_FALCON_build_request_field_108( );
void RT_FALCON_build_request_field_109( );
void RT_FALCON_build_request_field_110( );
void RT_FALCON_build_request_field_111( );
void RT_FALCON_build_request_field_112( );
void RT_FALCON_build_request_field_113( );
void RT_FALCON_build_request_field_114( );
void RT_FALCON_build_request_field_115( );
void RT_FALCON_build_request_field_116( );
void RT_FALCON_build_request_field_117( );
void RT_FALCON_build_request_field_118( );
void RT_FALCON_build_request_field_119( );
void RT_FALCON_build_request_field_120( );
void RT_FALCON_build_request_field_121( );
void RT_FALCON_build_request_field_122( );
void RT_FALCON_build_request_field_123( );
void RT_FALCON_build_request_field_124( );
void RT_FALCON_build_request_field_125( );
void RT_FALCON_build_request_field_126( );
void RT_FALCON_build_request_field_127( );
void RT_FALCON_build_request_field_128( );
void RT_FALCON_build_request_field_129( );
void RT_FALCON_build_request_field_130( );
void RT_FALCON_build_request_field_131( );
void RT_FALCON_build_request_field_132( );
void RT_FALCON_build_request_field_133( );
void RT_FALCON_build_request_field_134( );
void RT_FALCON_build_request_field_135( );
void RT_FALCON_build_request_field_136( );
void RT_FALCON_build_request_field_137( );
void RT_FALCON_build_request_field_138( );
void RT_FALCON_build_request_field_139( );
void RT_FALCON_build_request_field_140( );
void RT_FALCON_build_request_field_141( );
void RT_FALCON_build_request_field_142( );
void RT_FALCON_build_request_field_143( );
void RT_FALCON_build_request_field_144( );
void RT_FALCON_build_request_field_145( );

/*AMEX ACQ POS data changes*/

#define AMEX_MAGSTRIP_POS_DATA_WITH_PIN						"560101254041"
#define AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN					"560101254041"

#define AMEX_CHIP_POS_DATA_WITH_PIN							"560101554041"
#define AMEX_CHIP_POS_DATA_WITHOUT_PIN						"560101554041"

#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN				"56010X554041"
#define AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN			"56010X554041"

#define AMEX_FALLBACK_POS_DATA_WITH_PIN						"560101954041"
#define AMEX_FALLBACK_POS_DATA_WITHOUT_PIN					"560101954041"

#define AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN		"56010X254041"

#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN				"560100664041"
#define AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN			"560100664041"

#define AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA					"100SS0S00110"
#define AMEX_RPS_ASIAPAY_RPS_POS_DATA						"600090100110"
#define AMEX_BDO_PAY_POS_DATA								"660400600130"

#define AMEX_POS_DATA_LEN									12

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

#define POS_ENTRY_NORMAL         0
#define POS_ENTRY_EMV_CHIP       1
#define POS_ENTRY_EMV_MAGSTRIPE  2
#define POS_ENTRY_EMV_FALLBACK   3
#define POS_ENTRY_FALLBACK   	 4
#define  SERVICE_CODE_SIZE_PLUS_1        4
#define  ECOM_POS_CC   "59"
#define  ECOM_POS_ENTRYMODE "001"
#define  ECOM_POS_CC_VISA   "51"

#define ID_MOBILE_POS_ACCP_DEVICES 9 //Identification of Mobile POS Acceptance Devices
#define  AUTO_FUEL_MCC   "5542" /* Automated fuel dispenser Merchant cat code */
#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969

/*Handler Queue */
#define DINERS_POS_HANDLER_MODULE_COMP_LEN     		3
#define DINERS_ISSUING_MODULE_COMP_LEN  			5
#define DINERS_VOICE_MODULE_COMP_LEN    			5
#define DINERS_SBATCH_MODULE_COMP_LEN    			4
#define DINERS_CANDENCIE_LINK_QUEUE_NAME_LEN        6
#define IST_MODULE_COMP_LEN  						5

/*Transaction type*/
#define DINERS_RPS_INDUSTRY_CODE 			      "800"
#define DINERS_RPS_INDUSTRY_CODE_LEN 			   3

/*Diners EMV Capability Indicator*/
#define DINERS_EMV_CAPABILITY_INDICATOR     			"DC"
#define DINERS_DEVICE_EMV_CAPABILITY_LEN    			2
#define DEVICE_EMV_CAPABILITY_POS_FOR_DINERS    		4

#define DINERS_DE22_SIZE      						12
/*DE 22 Point Of Service Data Code  */
/*=============================================================================================*/
/*ISO BDO POS TERMINAL*/
#define DINERS_BDO_POS_ENTRY_MODE_LEN										3
#define DINERS_BDO_POS_ENTRY_MODE_UNKNOW									"000"
#define DINERS_BDO_POS_ENTRY_MODE_MANUAL									"001"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1									"002"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP2									"006"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP3									"090"
#define DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP4									"008"
#define DINERS_BDO_POS_ENTRY_MODE_ICC_EMV									"005"
#define DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV							"007"
#define DINERS_BDO_POS_ENTRY_MODE_FALLBACK									"080"
#define DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE						"091"
#define DINERS_BDO_POS_ENTRY_MODE_ECOM_INCOMING_TRN							"081"
#define DINERS_BDO_POS_ENTRY_MODE_WITH_PIN									"1"
#define DINERS_BDO_POS_ENTRY_MODE_WITHOUT_PIN								"2"
#define DINERS_BDO_POS_ENTRY_MODE_PIN_LEN									1
#define DINERS_BDO_POS_ENTRY_MODE_FOR_RPS									"0012"
#define DINERS_BDO_POS_ENTRY_MODE_FOR_RPS_LEN								4

/***********************************************************************************/
/*POS 1*/
/***********************************************************************************/
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_UNKNOW						'0'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MANUAL						'1'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP					'2'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_BARCODE					'3'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_OCT						'4'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_ICC						'5'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_KET_ENTERED				'6'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_CONTACTLESS				'8'
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_HYPERTERMINAL				'9'
/***********************************************************************************/
/***********************************************************************************/
/*POS 2*/
#define DINERS_DE22_POST_2_CARDHOLDER_AUTHENTICATION_CAPABILITY_PIN		'1'
/***********************************************************************************/
/***********************************************************************************/
/*POS 4*/
#define DINERS_DE22_POST_4_OPERATING_ENV_UNKNOWN							'9'
/***********************************************************************************/

/***********************************************************************************/
/*POS 5*/
#define DINERS_DE22_POST_5_INTERNET											'9'
/***********************************************************************************/
/*POS 6*/
/***********************************************************************************/
#define DINERS_DE22_POST_6_CARD_NOT_PRESENT								'0'
#define DINERS_DE22_POST_6_CARD_PRESENT									'1'
#define DINERS_DE22_POST_6_CARD_PRESENT_UNKNOW							'8'
/***********************************************************************************/
/*POS 7*/
/***********************************************************************************/
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_UNSPECIFIED					'0'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MANUAL						'1'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_READ				'2'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_BAR_CODE					'3'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_OCR							'4'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_ICC							'5'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_KEY_ENTERED					'6'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_FALLBACK				'9'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CHIP_CARD_DATA_RFID			'S'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_MAGSTRIP_DATA_RFID			'T'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_CONTACTLESS_CHANGED_TO_ICC	'U'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_ISO_USE			'7'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_RESERVED_NATION_USE			'8'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_M_COMMERCE					'V'
#define DINERS_DE22_POST_7_CARD_DATA_INPUT_MODE_STORED_CARD_ACCT_INFO		'B'
/***********************************************************************************/

/*MAGSTIPR POS DATA FOR DCI*/
/***********************************************************************************/
#define DINERS_DE22_POST_1_CARD_INPUT_CAPABILITY_MAGSTIP_STR				"2"
#define DINERS_DE22_POST_1_
/***********************************************************************************/

/*POS DATA MAPPING FOR ACQUIRED TRANSACTION*/
/***********************************************************************************/

/*Magstrip Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN 						"960101215140"
#define DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN 					"960101255140"
#define DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN 					"960101215140"
#define DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN  				"960101255140"
/***********************************************************************************/

/*Chip Dip Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN  						"960101511140"
#define DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN   					"960101551140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN   				"960101511140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN  				"960101551140"
/***********************************************************************************/

/*Fall Back Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN   					"960101915140"
#define DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN   					"960101955140"
#define DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN   				"960101915140"
#define DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN   				"960101955140"
/***********************************************************************************/

/*Contactless Magstrip Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN   		"960101T15940"
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN   		"960101T65140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN   	"960101T15940"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN	"960101T65140"
/***********************************************************************************/

/*Contactless CHIP Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN				"960101S11940"
#define DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN			"960101S51140"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN			"960101S11940"
#define DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN		"960101S51140"
/***********************************************************************************/
/*Manual Keyed in, RPS , ECOM, Voice Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM							"660490695110"
#define DCI_POS_DATA_DC_PRESENT_RPS										"160040B95110"
#define DCI_POS_DATA_DC_PRESENT_ASIAPAY_RPS								"160040195110"
#define DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN							"960101664140"
#define DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN						"960101664140"
#define DCI_POS_DATA_DC_PRESENT_VOICE									"660100665110"
#define DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM						"660490695110"
#define DCI_POS_DATA_DC_NOT_PRESENT_RPS									"160040B95110"
#define DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_RPS							"160040195110"
#define DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN						"960101664140"
#define DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN					"960101664140"
#define DCI_POS_DATA_DC_NOT_PRESENT_VOICE								"660100665110"
#define DCI_POS_DATA_FOR_BDO_PAY										"160000195130"
/***********************************************************************************/
/*Manual Keyed in, RPS , ECOM, Voice Acquired transaction*/
/***********************************************************************************/
#define DCI_POS_DATA_DC_PRESENT_UNKNOWN									"06093S8065110"
#define DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN								"06093S8065110"
/***********************************************************************************/
/***********************************************************************************/

/*DE 25 Message Reason Code  */
/*=============================================================================================*/
#define DINERS_DE25_MESG_REASON_CODE_REVERSAL_REQUEST						"4005"
#define DINERS_DE25_MESG_REASON_CODE_ADVICE_AUTHORIZATION					"4006"
#define DINERS_DE25_MESG_REASON_CODE_TEMP_NEG_FILE_ADD_ADVICE				"1411"
#define DINERS_DE25_MESG_REASON_CODE_NEGT_FILE_HIT_ADVICE					"1412"
#define DINERS_DE25_MESG_REASON_CODE_ISSUER_UNAVAILABLE						"0066"
#define DINERS_DE25_MESG_REASON_CODE_CAVV_VERIFICATION_FAILED				"0069"
#define DINERS_DE25_MESG_REASON_CODE_COUNTRY_BLOCK_BY_ISSUER				"1378"
#define DINERS_DE25_MESG_REASON_CODE_TRN_BLOCK_SUSPECTED_FRAUD				"1379"
#define DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION                     		"59"
#define DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN       		2
#define DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION                     		"08"
#define DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN				2
#define DINERS_DE25_RECURRING_TRANSACTION                     				"08"
#define DINERS_DE25_RECURRING_TRANSACTION_INDICATOR_LEN      		 		2
#define DINERS_DE25_BDO_PAY_TRANSACTION                     				"05"
#define DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN      		 			2
/*=============================================================================================*/


#define CUP_DE23_SIZE 3
#define CUP_DE22_SIZE 3
#define CUP_DE25_SIZE 2
#define CUP_VOICE_MODULE_COMP_LEN			5

/*CUP MOTO Changes*/
#define CUP_MOTO_INDUSTRY_CODE 				"500"
#define INDUSTRY_CODE_CODE	 				4
#define CUP_MOTO_TRN_INITIATION_MODE        "3"
#define CUP_CARD_NOT_PRESENT_TRAN			"7"

#define CUP_MID_INDICATOR_DEFAULT 			"7240608"
#define CUP_MID_INDICATOR_LEN     			7
#define CUP_RPS_POS_ENTRY_MODE 				"0012"
#define CUP_RPS_POS_ENTRY_MODE_LEN 			4
#define CUP_ISO_RPS_POS_CONDITION_CODE 		"08"
#define CUP_UPI_RPS_POS_CONDITION_CODE 		"28"
#define CUP_BDO_PAY_POS_CONDITION_CODE      "05"
#define CUP_BDO_PAY_POS_ENTRY_MODE      	"0012"
#define CUP_BDO_POS_ENTRY_MODE_LEN 			4
