
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
#define faircv_RT_HDR_LEN				101


#define  COMM_BUF_SZE            1500

#define INTIALIZE_QUEUES_NO_ERROR 00
#define INTIALIZE_QUEUES_ERROR 01
#define INTIALIZE_EXCEPTION_ERROR 02
#define MAX_INSTANCE_NAME_SIZE   51
#define FIELD67_SIZE 			 5
#define SPACES 					 ' '

#define MAX_EH_EXT_HDR_LEN	1024
#define SCR_HDR_LEN			152


BYTE faircv_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize);

#define MERCHANT_ID_SIZE 16
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20


void faircv_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT faircv_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail);
void faircv_create_Error_Filename();
void faircv_get_error_warning_file_name_path(void);
