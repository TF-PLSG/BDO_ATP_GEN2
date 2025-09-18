

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <signal.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "memmnger.h"
#include "txutils.h"
#include "diskio.h"
#include "txtrace.h"
#include "timings.h"

//for logs
#define TESTING 1

#define MAX_EH_EXT_HDR_LEN	1024
#define SCR_HDR_LEN			152

#define LOOKUP_KEY_SIZE					35
#define TRNID_LEN_BEFOREDECIMAL			16
#define TRNID_LEN_AFTERDECIMAL			17
#define TRNDID_AFTERDECIMAL				3
#define UNIQ_ID_LEN_FROM_RSPMSG			31
#define TRANSCN_ID_LEN					19
#define RRN_LEN							12
#define FALCON_RT_HDR_LEN				101

#define   COMM_BUF_SZE            		1500
#define   MERCHANT_ID_SIZE 				16
#define   MAX_FILENAME_SIZE  			80
#define   MAX_INI_INFO_SIZE  			20
#define   SYSTEM_MALFUNCTION            "96"

/* TIMER TYPES */
#define  REQUEST_TIMER  0
#define  ECHO_TIMER     1

/* TIMER APPS */
#define  TIMER_REVERSAL   "Reversal"
#define  TIMER_FINANCIAL  "Financial"
#define  TIMER_08X0       "0800"
#define  TIMER_ECHO       "Echo"
#define RT_RESPONSE  1
#define RT_TIMEOUT   2
#define RT_SYSTEM    3
#define RT_UNKOWN	 -1
#define MAX_TIMER_KEY_SIZE 24
#define MAX_APP_DATA_SIZE  32
#define RT_PASS_A					"Approve                         "
#define RT_PASS_a					"approve                         "
#define	RT_FAIL_D					"Decline                         "
#define	RT_FAIL_d					"decline                         "
#define FT_APPROVED_CODE_LEN		7
#define FT_FAIL_CODE_LEN			7

INT t_secs = 0;
INT t_id = 0;
/*Structure declaration*/
typedef struct
{
		struct {
			char EH_APP_DATA_LEN					[9];
			char EH_EXT_HDR_LEN						[5];
			char EH_TRAN_CODE						[10];
			char EH_SOURCE							[11];
			char EH_DEST							[11];
			char EH_ERROR							[11];
			char EH_FILLER							[2];
			char EH_EXT_HDR							[MAX_EH_EXT_HDR_LEN + 1];
		}Scr_Rsp_Hdr;

		struct {
			char RESPONSE_RECORD_VERSION			[2];
			char SCORE_COUNT						[3];
			char SCORE_NAME_1						[23];
			char ERROR_CODE_1						[5];
			char SCORE_1							[5];
			char REASON_1_1							[5];
			char REASON_1_2							[5];
			char REASON_1_3							[5];
			char SCORE_NAME_2						[23];
			char ERROR_CODE_2						[5];
			char SCORE_2							[5];
			char REASON_2_1							[5];
			char REASON_2_2							[5];
			char REASON_2_3							[5];
			char SCORE_NAME_3						[23];
			char ERROR_CODE_3						[5];
			char SCORE_3							[5];
			char REASON_3_1							[5];
			char REASON_3_2							[5];
			char REASON_3_3							[5];
			char SCORE_NAME_4						[23];
			char ERROR_CODE_4						[5];
			char SCORE_4							[5];
			char REASON_4_1							[5];
			char REASON_4_2							[5];
			char REASON_4_3							[5];
			char SCORE_NAME_5						[23];
			char ERROR_CODE_5						[5];
			char SCORE_5							[5];
			char REASON_5_1							[5];
			char REASON_5_2							[5];
			char REASON_5_3							[5];
			char SCORE_NAME_6						[23];
			char ERROR_CODE_6						[5];
			char SCORE_6							[5];
			char REASON_6_1							[5];
			char REASON_6_2							[5];
			char REASON_6_3							[5];
			char SCORE_NAME_7						[23];
			char ERROR_CODE_7						[5];
			char SCORE_7							[5];
			char REASON_7_1							[5];
			char REASON_7_2							[5];
			char REASON_7_3							[5];
			char SCORE_NAME_8						[23];
			char ERROR_CODE_8						[5];
			char SCORE_8							[5];
			char REASON_8_1							[5];
			char REASON_8_2							[5];
			char REASON_8_3							[5];
			char SEGMENT_ID_1						[9];
			char SEGMENT_ID_2						[9];
			char SEGMENT_ID_3						[9];
			char FILLER1_1							[3];
			char FILLER1_2							[5];
			char FILLER1_3							[3];
			char SEGMENT_ID_4						[9];
			char SEGMENT_ID_5						[9];
			char SEGMENT_ID_6						[9];
			char SEGMENT_ID_7						[9];
			char FILLER2_1							[5];
			char FILLER2_2							[5];
			char SEGMENT_ID_8						[9];
			char FILLER3							[5];
			char DECISION_COUNT						[3];
			char DECISION_TYPE_1					[33];
			char DECISION_CODE_1					[33];
			char DECISION_TYPE_2					[33];
			char DECISION_CODE_2					[33];
			char DECISION_TYPE_3					[33];
			char DECISION_CODE_3					[33];
			char DECISION_TYPE_4					[33];
			char DECISION_CODE_4					[33];
			char DECISION_TYPE_5					[33];
			char DECISION_CODE_5					[33];
			char DECISION_TYPE_6					[33];
			char DECISION_CODE_6					[33];
			char DECISION_TYPE_7					[33];
			char DECISION_CODE_7					[33];
			char DECISION_TYPE_8					[33];
			char DECISION_CODE_8					[33];
			char DECISION_TYPE_9					[33];
			char DECISION_CODE_9					[33];
			char DECISION_TYPE_10					[33];
			char DECISION_CODE_10					[33];
			char SCORING_SERVER_ID					[5];
		}Scr_Rsp_Dat;

}FAI_RSP_MSG;

FTLF01  Ftlf01;
AUTH_TX	Auth_Tx;
FAI_RSP_MSG fai_response_Rsp_msg;

CHAR fai_response_buffer[sizeof(FAI_RSP_MSG)+1]={0};

#define AUTH_TX_SIZE	sizeof(Auth_Tx)
#define FTLF01_SIZE		sizeof(Ftlf01)
#define  FAI_RSP_MSG_SIZE sizeof(FAI_RSP_MSG)

INT  Auth_Tx_Size = AUTH_TX_SIZE;

extern CHAR          ServiceName[12];
extern INT  volatile EndProcessSignalled;
extern INT  volatile MainProcessDone;

CHAR TimerAppData2[MAX_APP_DATA_SIZE];
CHAR txcntl_que_name [MAX_QUE_NAME_SIZE+1] = "txcntlA";
//CHAR falcon_que_name [MAX_QUE_NAME_SIZE+1] = "falconA" ;
CHAR faisnd_que_name [MAX_QUE_NAME_SIZE+1] = "faisndA" ;
CHAR ftimer_que_name [MAX_QUE_NAME_SIZE+1] = "ftimerA" ;
CHAR applnk_que_name [MAX_QUE_NAME_SIZE+1] = "applnkA" ;
CHAR mp_pos_que_name[MAX_QUE_NAME_SIZE+1] 	= "dcpimpA" ;
CHAR mp_nclink_que_name [MAX_QUE_NAME_SIZE+1] 	= "nciftrA";
CHAR fconds_que_name[MAX_QUE_NAME_SIZE+1] 	= "fcondsA";
CHAR falcntl_que_name[MAX_QUE_NAME_SIZE+1] = {0};

CHAR  falcntl_Erro_warning_Filename[256] = {0};
CHAR  falcntl_module_error_warning_file_name[256] = {0};
CHAR  falcntl_error_warning_file_path[256] = {0};
long int Max_File_Size_Defined = 0 ;

CHAR Version[] = "ATP_11.10.6";

void falcntl_create_Error_Filename();
void falcntl_get_error_warning_file_name_path(void);
 INT falcntl_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail);
void falcntl_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details);
void PinnacleMsgHandler(pPTE_MSG p_msg_in);
BOOLEAN FalService_Send_To_Monitor( BYTE mtype, BYTE st1, BYTE st2, pBYTE msg,   INT  len );
INT get_FRT_timeout_for_Falcon( void );
int process_txcntl_request(pPTE_MSG);
int send_request_to_falcon();
void create_request_timer_key( pCHAR timer_key );
INT set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer );
INT validate_date_yyyymmdd( pBYTE date_str);
int falcntl_process_response_message(pPTE_MSG p_msg_in);
int falcntl_process_clear_timer_reply(pPTE_MSG p_msg_in);
void falcntl_process_time_out_message(pPTE_MSG p_cur_msg);
INT send_msg_to_authorizer( BYTE msgtype );
INT insert_db_record( BYTE data_type );
INT Check_Rsp_Msg_for_approval();
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );
INT isnum(pCHAR in_str);
void format_date(pCHAR temp);
void get_date( pCHAR time_date, pCHAR date_str );
void get_time( pCHAR time_date, pCHAR time_str );
BOOLEAN FalService_Send_To_Monitor( BYTE mtype, BYTE st1, BYTE st2,
                                   pBYTE msg,   INT  len );
