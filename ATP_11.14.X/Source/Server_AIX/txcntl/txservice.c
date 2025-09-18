/************************************************************************************
 
MODULE:      txservice.C
Copyright (c) 2007, Hypercom, Inc.
All Rights Reserved. 

TITLE:       Equitable - Process Incoming Request.
 
DESCRIPTION:  

APPLICATION: TXCNTL

AUTHOR:  Sanjoy Dasgupta

REVISION HISTORY:

  
 
**************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pteipc.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "txcntl.h"
#include "txservice.h"
#include "txdataserver.h"
#include "txcntl_constants.h"
#include "genutil.h"
#include "txfleet.h"
#include "txauthorize.h"
#include "txutils.h"
#include "memmnger.h"
#include "myutils.h"
#include "diners.h"
#include "cup.h"
#include "hmac.h"

#define NOT_BLOCKED 0
#define EMPTY       1	
#define BLOCKED     2

#define RESPONSE_CODE_SIZE 2
#define TIMED_OUT "19"

/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

#define VISA7 1
#define VISA2 1

extern AUTH_TX Auth_Tx ;
extern NCF01   ncf01_recs[];
extern PRIVATE_CCF03P_STRUCT private_ccf03p_struct;

#ifdef VISA7
extern MERCH_INT_PARAMS MERCH_INT_PARAM;
#endif

extern CHAR tran_dataserver_que_name [] ;
extern CHAR updateds_que_name[];
extern CHAR network_dataserver_que_name [] ;
extern CHAR fconds_que_name[];

extern CHAR txcntl_que_name [] ;
extern CHAR crypto_que_name [] ;
extern CHAR cirrus_host_que_name[] ;
extern CHAR plus_host_que_name[] ;
extern CHAR vsms_host_que_name[] ;
extern CHAR amex_atm_que_name[];
extern CHAR amex_pos_que_name[];
extern CHAR diners_host_que_name[];
extern CHAR diners_atm_que_name[];
extern CHAR jcb_host_que_name[];
extern CHAR applnk_que_name [] ;
extern CHAR EB_Host_Issuer_ID[];
extern CHAR cadencie_host_que_name[] ; // nirbhay ThoughtFocus
extern CHAR cadencie_pos_que_name[] ; // ThoughtFocus Girija Y
extern CHAR cadencie_atm_que_name[] ; // ThoughtFocus Girija Y
extern CHAR cadencie_mc_que_name[] ; // ThoughtFocus Girija Y
extern CHAR device_dataserver_que_name[] ;
extern CHAR ncdci_issuing_link_name[];
extern CHAR ncvisa_issuing_link_name[];
extern CHAR ncmcrd_issuing_link_name[];
extern INT isalphanum(pCHAR string);//TF PHANI


extern CHAR fguard_que_name [];
extern CHAR faisnd_que_name [];
#ifdef FALCON_CONTROLLER
extern CHAR falcntl_que_name [];
#endif
extern CHAR nceqit_que_name [];
extern CHAR neqit2_que_name [];
extern INT nFraudGuardState ;
extern INT nFalconSendFlag;
extern CHAR key[];

INT  ServiceCodeFlag = 0;
CHAR    mrads_que_name[]       = "mradsA" ;
CHAR    mracon_que_name[]       = "mraconA" ;

extern CHAR  voice_handler_que_name[] ;
// Allocate a buffer to pass to the data server
BYTE p_buffer[MAX_DATASERVER_BUFFER_LENGTH] ;
extern CHAR  txcntl_Erro_warning_Filename[256] ;
extern CHAR  txcntl_module_error_warning_file_name[256];
extern CHAR  txcntl_error_warning_file_path[256];
extern CHAR  JCB_ACQ_Contactless_Magstrip_RC[3];

double nCCF02P_Incremented_Outstanding_Auths ;
double nACF01_Incremented_Outstanding ;
double nACF01_Incremented_Cash_Outstanding ;
double nACF01_Decremented_Available ;
double nACF01_Decremented_Cash_Available  ;

UINT Max_File_Size_Defined = 0 ;

extern CHAR ServiceName[12];

CHAR	ServiceCode[4];
CHAR	pos_rc[3] = {0};
CHAR	pos_res_txt[40] = {0};

extern INT amex_rps_flag ;
extern INT cup_rps_flag  ;
extern INT dci_rps_flag  ;
extern INT jcb_rps_flag  ;
extern INT mc_rps_flag   ;
extern INT visa_rps_flag ;
extern INT nMra_enable_flag  ;
extern INT Nmra_send_flag ;
extern Mracon_Log_Enable_Flag;
extern INT JCB_ACQ_Contactless_Magstrip_Flag;

extern MCC_VALUE rpsmcc_list[50];
extern OBS01_GUI_LIST obs01_list;
extern VISA3DS2_GUI_LIST visa3ds2_list;
extern JCB3DS2_GUI_LIST jcb3ds2_list;
extern AMEX3DS2 amex_3ds2;
extern DCI3DS2 diners_3ds2;
extern JCB3DS2 jcb_3ds2;
extern CUP3DS2 cup_3ds2;

extern KEY3DS01 key3ds01;

extern FRULE01_GUI_LIST amex_frual01;
extern FRULE01_GUI_LIST dci_frual01;
extern FRULE01_GUI_LIST cup_frual01;
extern FRULE01_GUI_LIST jcb_frual01;
extern FRULE01_GUI_LIST mc_frual01;
extern FRULE01_GUI_LIST visa_frual01;
extern FRULE01_GUI_LIST pl_frual01;

extern FCLF01 amex_fclf01;
extern FCLF01 dci_fclf01;
extern FCLF01 cup_fclf01;
extern FCLF01 jcb_fclf01;
extern FCLF01 mc_fclf01;
extern FCLF01 visa_fclf01;
extern FCLF01 iftr_fclf01;

INT MCC_RPS_flag ;
extern exception_cardnum_flag;
extern CR01 cr01;
char mc_voice_timeout_rc[3]={0};
char visa_voice_timeout_rc[3]={0};
char amex_voice_timeout_rc[3]={0};
char jcb_voice_timeout_rc [3]={0};
char cup_voice_timeout_rc [3]={0};
char dci_voice_timeout_rc [3]={0};

char mc_voice_fai_rc[3]={0};
char visa_voice_fai_rc[3]={0};
char amex_voice_fai_rc[3]={0};
char jcb_voice_fai_rc [3]={0};
char cup_voice_fai_rc [3]={0};
char dci_voice_fai_rc [3]={0};

#ifdef HCTS2
BYTE card_activation_flag;
BYTE card_updating_status [5]      =  {0};
BYTE card_current_status [51][5]   =  {0};
INT  total_card_current_status     =  0;
BYTE visa_vertexon_codes [101][4]  =  {0};
BYTE mc_vertexon_codes   [101][4]  =  {0};
BYTE jcb_vertexon_codes  [101][4]  =  {0};
BYTE amex_vertexon_codes [101][4]  =  {0};
BYTE dci_vertexon_codes  [101][4]  =  {0};
BYTE upi_vertexon_codes  [101][4]  =  {0};
BYTE mp_in_vertexon_codes  [101][4]  =  {0};
BYTE pl_ft_vertexon_codes  [101][4]  =  {0};
void txcntl_read_atp_card_activation_parameters();
void txcntl_read_vertexon_codes();
BOOLEAN txservice_verify_onus_online_transaction();
#endif

INT Falcon_RT_Flag=0;
INT Falcon_RT_Ext10_Flag = 0;
INT SecondaryExpiryDateValidationFlag=0;
INT Cup3ds2_Flag=0;

extern OBS01 obs01_data;

extern INT keyblock_enable_flag_for_MC ;
extern INT keyblock_enable_flag_for_VISA ;
extern INT keyblock_enable_flag_for_AMEX ;
extern INT keyblock_enable_flag_for_JCB ;
extern INT keyblock_enable_flag_for_UPI ;
extern INT keyblock_enable_flag_for_DCI ;
/*************************************************************************************
NAME:			txservice_Eqit_POS_Field63
DESCRIPTION:	Passing Fld 63 ( Network ID, Message Reason Code) for the Tx coming from POS.
                This value is passed only when Tx is locally originated and destination is "nceqit"
INPUTS:			pAUTH_TX
OUTPUTS:		None
RETURNS:        NO_AUTH_ERROR 
AUTHOR:         Girija Y TF, Jan 02 2009, ATP - EQIT Changes
**************************************************************************************/
void txservice_Eqit_POS_Field63(pAUTH_TX p_auth_tx,pBYTE destination_name)
{
	INT len;
	len = strlen(EB_Host_Issuer_ID);

    if( (LOCAL_ORIGIN == txservice_GetTransactionOrigin()) && 
		( 0 == strncmp(destination_name,EB_Host_Issuer_ID,len))) 
	{
		memcpy(p_auth_tx->TLF01_details.product_codes[10].code,"0000",4);

		if( ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
			  (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE) )
					
		{
			memcpy(p_auth_tx->TLF01_details.product_codes[11].code, "2501",4);
		}
		
		/* Message reason code for REVERSALS */
		else if( 0 == memcmp(p_auth_tx->TLF01_details.message_type,"0400",4 ) )
		{
			memcpy(p_auth_tx->TLF01_details.product_codes[11].code, "2503",4);
		}
			
		else  
		{
			memcpy(p_auth_tx->TLF01_details.product_codes[11].code, "2502",4);
		}	
	}
}

/*************************************************************************************
NAME:			txservice_Process_Auth_Response
DESCRIPTION:	This is where the control is transferred when a MT_AUTH_RESPONSE message 
				is received from the host handler
INPUTS:			pPTE_MSG p_msg_in: The PTE message
OUTPUTS:		None
RETURNS:        NO_AUTH_ERROR 
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	Ravikumar Nageshaiah TF rel 08-15
***************************************************************************************/
BYTE txservice_Process_Auth_Response (pPTE_MSG p_msg_in)
{
	BOOLEAN ReturnCode = true ;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	char strTemp[256] = {0} ;
	char strMessage[512]={0} ; 
	BYTE   processing_code [7];
	char p_buffer2[10]={0};
	CHAR amex_response_code[4] = {0};
	
	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	
	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/

	sprintf (strTemp,"Received msg from host-RespCode:%s  ",
		      Auth_Tx.TLF01_details.response_code) ;
/* TF - Phani - Other services may not understand These tx keys, change to orginal*/
	if(true==txservice_Check_If_VISA_Transaction() &&
		(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE) &&
		(0==strncmp(Auth_Tx.TLF01_details.acquirer_id,"nceqit",6) ||
		 0==strncmp(Auth_Tx.TLF01_details.acquirer_id,"neqit2",6)) ||
		 0==strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncvis2",6))
	{

	}
	else if(true==txservice_Check_If_AMEX_Transaction() &&
			0==strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5) &&
			0==strncmp(Auth_Tx.function_code,"401",3))
	{/*AMEX03A dont change the key*/

	}
	else
	{
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE)
		{
			Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
		
			memcpy( processing_code,
					Auth_Tx.TLF01_details.processing_code,
					sizeof(processing_code) );

			if(strncmp(processing_code,"02",2) == 0)
				Auth_Tx.TLF01_details.tx_key 	= AUTH_VOID_SALE_RESPONSE;

			if(Auth_Tx.TLF01_details.saf[0] == 'P' || Auth_Tx.TLF01_details.saf[0] == 'I')
				Auth_Tx.TLF01_details.tx_key 	= AUTH_REVERSAL_RESPONSE;
		}
	}

    /*The logic added for stand-in processing - Ravikumar K N - ThoughtFocus*/
	if ((strcmp(Auth_Tx.TLF01_details.response_code,"00") == 0) &&
		(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0))
	{
		if((strncmp(Auth_Tx.CAF01_details.destination_name,"natma2A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncjcb2A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncmcr2A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"nposa2A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncvis2A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncvis3A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncjcb3A",5) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"neqit2",5) == 0)  ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"nccup2",6) == 0)  ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncdci2",6) == 0)  ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncvis4A",5) == 0) )
		{

			// Lets prepare POS data for ON US OFFLINE sale transaction.
			if(((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
			    ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))) &&
			    ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0220",4)) ||
				(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0230",4))))
			{
				txservice_Prepare_Pos_Data();
			}

			/*In case of Realtime we will have ACF01 details but accout has not been updated
			 * It should be handled like ACF01 recrds have not been updated*/
			 if (0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S") &&
				true == Falcon_RT_Flag  &&
				true == txservice_If_Tx_Qualifies_For_RealTime() &&
				false == txservice_Check_If_IST_Message ())
			{
				 ReturnCode = txserviceCadience_Perform_Balance_Update_For_RT_Tx();
				 if (ReturnCode == NO_AUTH_ERROR)
				 {
					return ReturnCode;
				 }

			}
				/* At this point of time we might have done all validation and Balnce update also
				 we need to check whether we done or not ..TF PHANI */
			else if(FieldIsNumeric(Auth_Tx.ACF01_details.primary_key.account_nbr))
			{
			/* Check if account has bee updated properly */

				if(Auth_Tx.TLF01_details.update_mask[0] & ACF01_MASK)
				{
					txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
					return GS_TXN_COMPLETED;
				}
				else
				{	/* Update ACF01 structure variables..TF PHANI */
					/* At this point, STIP transaction is rejected by ATP, but HOST (network controller) approved
					   This can happen if card holder accounts are not synchronized from issuer.
					   Request By kenneth: 
					*/
					ReturnCode = TxAuthorize_STIP_Update_ACF01_Structure();
					ReturnCode = TxAuthorize_Update_AuthTx_Structures( 1, 0, 1 );
					/* Log message to monitor ,TF PHANI*/
				 sprintf (strMessage, "Transaction Approved by ,network_controller: %s But we are not able to do balance update for,Auth Location %s,Auth Method %s case,STAN %s",
					  Auth_Tx.TLF01_details.authorizing_host_queue,Auth_Tx.CAF01_details.authorization_location,Auth_Tx.CAF01_details.authorization_method,Auth_Tx.TLF01_details.sys_trace_audit_num);
				 txcntl_log_message( 1, 3,strMessage, "txservice_Process_Auth_Response", 1 );
				 return GS_TXN_COMPLETED;
				}
			}
			else
			{					
				//return GS_TXN_COMPLETED;
				ReturnCode = txserviceCadience_Perform_Balance_Update();
				if (ReturnCode == NO_AUTH_ERROR)     
					return ReturnCode; 
			}
		}
	}

	if (true == txservice_check_if_txn_is_cup3ds2_issuing_txn() &&
		Cup3ds2_Flag == 1)
	{
		ReturnCode = txservice_perform_standin_processing();
				return ReturnCode;
	}

	if ((strcmp(Auth_Tx.TLF01_details.response_code,"01") == 0) &&
		(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) &&
		(txservice_Is_Host_Up(Auth_Tx.CAF01_details.destination_name) == false ||
				strcmp(Auth_Tx.TLF01_details.product_codes[3].quantity, TIMED_OUT) == 0)
		)
	{	/* Above three condition should satisify to proecess STIP
			1. Response code should be 01 with HOST as OFFLINE
			2. Profile should be STIP
			3. HOST should be offline, Since some time HOST also can return with 01 response code for various reasons- Kenneth
		*/

		if( true == txservice_is_txn_should_blocked_for_processingCode())
		{
			/* Transaction blocked and sending response to originator*/
			return GS_TXN_COMPLETED;
		}

		if( true == txservice_is_txn_should_blocked_for_MCC())
		{
			/* Transaction blocked and sending response to originator*/
			return GS_TXN_COMPLETED;
		}

		ReturnCode = txservice_perform_standin_processing();
			if (ReturnCode == NO_AUTH_ERROR)   
				return ReturnCode;      
	}
	/*Let us check if its MC advice message (0120) and Cadecnie had not responded*/
	else if ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0130",4))&&
			 (true == txservice_Check_If_MC_Transaction()) &&
			 (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",02)) &&
			 (0 == strncmp(Auth_Tx.CAF01_details.authorization_location,"S",1)) &&
			 (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "sbat",4)))
	{
		/* Cadencie did not not responded for 0120, Marked as STANDIN so that we will be able to store in tlf01_stip table*/
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		strncpy(Auth_Tx.TLF01_details.response_code,TRANSACTION_APPROVED,2);
		return GS_TXN_COMPLETED;
	}

	// POS data is empty for OFFLINE OFF US sale transaction
	else if(((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	       ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))) &&
	       ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0220",4)) ||
	    	(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0230",4))))
	{
		txservice_Prepare_Pos_Data();
	}

	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit2",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",6)) &&
		true == txservice_Is_Host_Up(Auth_Tx.TLF01_details.acquirer_id) &&
		(Auth_Tx.TLF01_details.response_code == NULL_STR) &&
	    Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE)
	{
		strncpy(Auth_Tx.TLF01_details.response_code,"19",2);
	}

	if ((strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) &&
		(txservice_Is_Host_Up(Auth_Tx.CAF01_details.destination_name) == false) &&
		true  == txservice_Check_If_AMEX_Transaction() &&
		true == txservice_Check_If_POS_Online_PIN_transction())
	{
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
			/* Make trnsaction key as request because we are doing STAND IN..
				 * TF PHANI*/
			if( Auth_Tx.TLF01_details.tx_key > AUTH_INVALID &&
				Auth_Tx.TLF01_details.tx_key  < AUTH_TX_KEY_MAX
				)
			{
				Auth_Tx.TLF01_details.tx_key -= 100;
			}

			if(0 == strlen(Auth_Tx.TLF01_details.product_codes[12].code))
			 {
				match_ISO_to_AMEX_response_code(amex_response_code);
				memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
					amex_response_code,
					sizeof(amex_response_code));
			 }
	}

	if ((strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) &&
		((strncmp(Auth_Tx.TLF01_details.response_code, "19", 2) == 0) 	||
		 (strncmp(Auth_Tx.TLF01_details.response_code, "00", 2) != 0))	&&
	   ((strncmp(Auth_Tx.CAF01_details.destination_name,"ncvis2",6) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncmcr2",6) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"nposa2",6) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncdci2",6) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"nccup2",6) == 0) ||
		(strncmp(Auth_Tx.CAF01_details.destination_name,"ncjcb2",6) == 0)))
	{
			/* set the flag to 1 to read customer number from PAF01 */
			Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 1;
			ReturnCode = TxDataServer_Get_PAF01_Details();
			return NO_AUTH_ERROR;
	}

	if ((strncmp(Auth_Tx.TLF01_details.card_num, "7",1) == 0) &&
		(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) &&
		(strncmp(Auth_Tx.TLF01_details.handler_queue, "dcpimp", 6) == 0) &&
		(strncmp(Auth_Tx.CAF01_details.destination_name,"nciftr",6) == 0))
	{
		/* For PL/FC, set the flag to 1 to read customer number from PAF01 */
		Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 1;
		ReturnCode = TxDataServer_Get_PAF01_Details();
		return NO_AUTH_ERROR;
	}
	return GS_TXN_COMPLETED;
	
} /* txservice_Process_Auth_Response */


/******************************************************************************
NAME:			txservice_perform_standin_processing

DESCRIPTION:This is the entry point into the Transaction Controller

INPUTS:		pPTE_MSG p_msg_in: The PTE message dispatched from the Pinnacle
                               Message Handler

OUTPUTS:		None

RETURNS:    NO_AUTH_ERROR: When there is a problem reading the bin table from
                           shared memory and a call was made to the data server
							      Also when there is no problem with the function

				SYSTEM_ERROR:  Whenever any XIPC error occurred

            TRUE:          If transaction was authorized

AUTHOR:     Ravikumar Nageshaiah 
******************************************************************************/

BYTE txservice_perform_standin_processing(void)
{
	BYTE ReturnCode = 0;
	BYTE strMessage[256] = {0};
	CHAR gmt_time[20] = {0};
	CHAR strTemp[512] = {0};
	CHAR  date_str [ 9] = {0};
	CHAR  time_str [ 7] = {0};
	CHAR  time_date[25] = {0};

	if(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0)
	{
	strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
	}
	/* Req buffer should reset */
		memset(Auth_Tx.req_buffer,0x00, sizeof(Auth_Tx.req_buffer));
		memset(Auth_Tx.req_data_len,0x00,sizeof(Auth_Tx.req_data_len));

	if( (true == txservice_Is_Refund_Transaction() 				 &&
		 0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S"))  &&
		(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
		 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)||
		 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis",5) ))
	{/* will include all ncmcrd and ncmcr3*/

		(void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
		if(true == txservice_Check_If_MC_Transaction())
		{
			sprintf(Auth_Tx.TLF01_details.response_text,"MC Refund txn Processed in STIP");
			strncpy(Auth_Tx.TLF01_details.response_code,MC_REFUND_RC,2);
			txcntl_log_message( 1, 3,Auth_Tx.TLF01_details.response_text,"txservice_perform_standin_processing", 1 );
			return NO_UPDATER_ERROR;
		}
		else if(true == txservice_Check_If_VISA_Transaction())
		{
			sprintf(Auth_Tx.TLF01_details.response_text,"VISA  Refund txn Processed in STIP");
			strncpy(Auth_Tx.TLF01_details.response_code,VISA_REFUND_RC,2);
			txcntl_log_message( 1, 3,Auth_Tx.TLF01_details.response_text,"txservice_perform_standin_processing", 1 );
			return NO_UPDATER_ERROR;
		}
		/* AMEX02 Amex code committed for not responding immediately as per the 11.8 AMEX mandate 2024 requirement
		else if(true == txservice_Check_If_AMEX_Transaction())
		{
			sprintf(Auth_Tx.TLF01_details.response_text,"AMEX  Refund txn Processed in STIP");
			strncpy(Auth_Tx.TLF01_details.response_code,AMEX_REFUND_RC,2);
			txcntl_log_message( 1, 3,Auth_Tx.TLF01_details.response_text,"txservice_perform_standin_processing", 1 );
			return NO_UPDATER_ERROR;
		}*/
	}
	if( 0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S")  &&
		0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) &&
		(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
		 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)) &&
		 Auth_Tx.TLF01_details.remote_nsp_type[0]=='P')
		 {
			memset(Auth_Tx.TLF01_details.response_text,0x00,sizeof(Auth_Tx.TLF01_details.response_text));
			strncpy(Auth_Tx.TLF01_details.response_code,MC_DECLINE_RC,2);
			sprintf(strTemp,"Declining the MC transaction for TCC:%s",Auth_Tx.TLF01_details.remote_nsp_type[0]);
			txcntl_log_message( 1, 3,strTemp,"txservice_perform_standin_processing", 1 );
			return NO_UPDATER_ERROR;

		 }
	if((true == txservice_Check_If_CUP_Transaction()) &&
		(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE) )
	{
		Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
		/* We are storing so that we can put RC= 05 for AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE
		 when atp did not received DE51(ch_billing_curr_code) or DE 50 (settlemet currency) is
		 not peso or dollar*/
		strncpy(Auth_Tx.TLF01_details.product_codes[0].quantity,"20",2);
	}
	if((true == txservice_Check_If_CUP_Transaction()) &&  0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S")  &&
		0==strncmp(Auth_Tx.TLF01_details.message_type,"0200",4) &&
		0==strncmp(Auth_Tx.TLF01_details.product_codes[17].amount,"014000",6))
	{
		sprintf(strTemp,"Declining the ATM CUP transaction with RC %s,"CUP_ATM_CASH_ADVANCE_RC);
		strncpy(Auth_Tx.TLF01_details.response_code,CUP_ATM_CASH_ADVANCE_RC,2);
		txcntl_log_message( 1, 3,strTemp,"txservice_perform_standin_processing", 1 );
		return NO_UPDATER_ERROR;

	}
	
	if(true == txservice_Check_If_Diners_Transaction())
	{
		if(Auth_Tx.TLF01_details.message_type[1] == '4' ||
		   Auth_Tx.TLF01_details.message_type[2] == '2' )
		{
			// No need to check if DE55 is present.
		}
		else if( ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
						 DINERS_BDO_POS_ENTRY_MODE_LEN))||
			 (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
						  DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
						  DINERS_BDO_POS_ENTRY_MODE_LEN))) &&
			(false == txservice_Check_If_EMV_Trn()))
		{
			// Its EMV tramsaction and there is no DE55.
			strcpy(strTemp, "Invalid EMV transaction,No DE55 present");
			strncpy(Auth_Tx.TLF01_details.security_response_code,
					DINERS_RACAL_DECLINE_RESPONSE,
					DINERS_RACAL_RESPONSE_LEN);
			ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
						 	 	 	 	 	 	 	 	 	 	  "txservice_perform_standin_processing",
																  GS_TXN_TRANSACTION_NOT_ALLOWED,
																  DO_NOT_HONOR) ;
			 return ReturnCode;
		}
	}

	if(true == txservice_check_for_DCI_refund_txn())
	{ // check for PC 3rd and 4th bye, DCI supports "00" and  "30" only
		if(0 != strncmp(Auth_Tx.TLF01_details.processing_code,"2000",4) &&
		   0 != strncmp(Auth_Tx.TLF01_details.processing_code,"2030",4)	)
		{
			strncpy(Auth_Tx.TLF01_details.response_code,DO_NOT_HONOR,2);
			strncpy(Auth_Tx.TLF01_details.response_text, "Invalid processing code in refund Txn",40);
			return NO_UPDATER_ERROR;
		}
	}
	/* Since we have not Approved Tx , Clear Auth_number, NC may put some value TF-Phani 29-Jul-2009*/
	if( Auth_Tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE &&
	    Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_REVERSAL_ADVICE_RESPONSE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_REVERSAL &&
		Auth_Tx.TLF01_details.tx_key != AUTH_REVERSAL_REPEAT_RESPONSE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_ADVICE_RESPONSE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_PRE_AUTHORIZATION_CANCLE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE &&
		Auth_Tx.TLF01_details.tx_key != AUTH_VOID_PRE_AUTHORIZATION &&
		Auth_Tx.TLF01_details.tx_key != AUTH_VOID_PRE_AUTHORIZATION_RESPONSE &&
		(0 != strncmp(Auth_Tx.TLF01_details.message_type,"0220",4)) &&
		(0 != strncmp(Auth_Tx.TLF01_details.message_type,"0230",4))
	   )
	{
		memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
	}

	if(!(strEqual(Auth_Tx.BIN01_details.auth_ind,sREMOTE_PROCESSING_INDICATOR))) 
	{
        sprintf (strMessage, "In standin processing, network_controller: %s",
             Auth_Tx.TLF01_details.authorizing_host_queue);

        txcntl_log_message( 1, 2,strMessage, "txservice_perform_standin_processing", 1 );
		  /* Current Greenwich Mean Time for the transmission-to-host time. */
		  if ( 0 == strcmp( NULL_STR,
						   Auth_Tx.TLF01_details.transmission_timestamp ) )
		  {
			  if(true == txservice_Check_If_CUP_Transaction())
			  {
				   memset( Auth_Tx.TLF01_details.transmission_timestamp, 0x00, CUP_DE7_SIZE );
				   memset( Auth_Tx.TLF01_details.time_hhmmss, 0x00, CUP_DE12_SIZE );
				   ptetime_get_timestamp( time_date );
				   get_date( time_date, date_str );
				   get_time( time_date, time_str );

				   /* Store MMDDhhmms format in transmission_timestamp */
				   /* Store MMDD in transmission_timestamp */
				   memcpy( Auth_Tx.TLF01_details.transmission_timestamp,
						   date_str + CUP_MMDD_START_POS,
						   CUP_MMDD_LEN);
				   /* Store hhmmss in transmission_timestamp after storing DDMM*/
				   memcpy( Auth_Tx.TLF01_details.transmission_timestamp + 4,
						   time_str, CUP_HHMMSS_LEN);
				   /* Lets Populate DE12 as well*/
				   memcpy( Auth_Tx.TLF01_details.time_hhmmss , time_str, CUP_DE12_SIZE);
			  }
			  else
			  {
				pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
			    strncpy( Auth_Tx.TLF01_details.transmission_timestamp,
					 &gmt_time[4], 10 );  /* MMDDhhmmss */
			  }
		   }

		  if(0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))
		  {
			  // We have to consider only incoming authorization message 0100/1100
			 // if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))||
			 //    (0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100",4)))
			  if(	Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
					Auth_Tx.TLF01_details.tx_key == AUTH_SALE ||
					Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ||
					Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE ||
					Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
					Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE
			  )
			  {
				  //Check if DE55 is present or not
				  if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
						  (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
				  {
					  if(false == txservice_ATP_STIP_Processing_for_Incoming_Issuing_Transactions())
					  {
						  /* Transaction is failed to qualify for EMV issuing validation
						   * RC = 12
						   * Response text =â€œInvalid transactionâ€�
						   * */
						  memset(strMessage,0x00,sizeof(strMessage));
						  memset(Auth_Tx.TLF01_details.response_text,0x00,
								  sizeof(Auth_Tx.TLF01_details.response_text));
						  strcpy (strMessage, "Invalid transaction") ;
						  memcpy( Auth_Tx.TLF01_details.response_text, strMessage,
								  sizeof(Auth_Tx.TLF01_details.response_text)-1 );
						  strncpy(Auth_Tx.TLF01_details.response_code,INVALID_TRANSACTION,2);

						  ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strMessage,
																					   "txservice_perform_standin_processing",
																						GS_TXN_INVALID_TRACK2_SERVICE_CODE,
																						INVALID_TRANSACTION) ;

						  return ReturnCode ;
					  }
				  }
			  }
		  }

    	/*	Prepare POS data only for acquired transaction */
		  if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
		  	 (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
		  {
			  txservice_Prepare_Pos_Data();
		  }
		
		if ((Auth_Tx.BIN01_details.caf_profile_id == NULL) && 
			(strlen(Auth_Tx.BIN01_details.pan_length) == 0)) 
		{
			//Get the BIN record for the card 
			//Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ
			ReturnCode = TxDataServer_Get_BIN01_Details (SHARED_MEMORY_READ) ;

			if (ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
			{
				// bin range is not found in shared memory
				// if bin type is CREDIT, return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR
				// if bin type is DEBIT, search the BIN02 table
				ReturnCode = txservice_Process_When_Bin_Range_Not_Found () ;
			}
			else if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY)
			{
				if(Auth_Tx.BIN01_details.auth_ind == 0)
				{
					//Do not perform standin processing for the profile that requires the 
					//remote host be available for authorization. return to the calling function
					//and return the error to the user
					return NO_AUTH_ERROR;
				}
				// Now that we have got the BIN01 profile in AUTH_TX structure
				// get the CAF01 details for this card. 
				ReturnCode = TxDataServer_Get_Bin_Level_Profile();
			}
			else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
			{
				// if problems with reading shared memory, read the 
				// bin table from the database
				strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
				ReturnCode = TxDataServer_Get_BIN01_Details (DATABASE_READ) ;
				if (ReturnCode == false)
					return SYSTEM_ERROR;
			}
		}
		if ((Auth_Tx.BIN01_details.caf_profile_id != NULL) &&
			(Auth_Tx.CAF01_details.primary_key.caf_profile_id == NULL))
		{
			BYTE ReturnCode = 0 ;
			if(Auth_Tx.BIN01_details.auth_ind == 0)
			{
				//Do not perform standin processing for the profile that requires the 
				//remote host be available for authorization. return to the calling function
				//and return the error to the user
				return NO_AUTH_ERROR;
			}
			/* Retrieve the CAF01 record using the the caf_profile_id in the bin table 
			(Auth_Tx.BIN01_details.caf_profile_id) */
			/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
			ReturnCode = TxDataServer_Get_CAF01_Details (SHARED_MEMORY_READ, 
														Auth_Tx.BIN01_details.caf_profile_id, 
														Auth_Tx.BIN01_details.primary_key.type,
														ST2_DB_SELECT_CAF01_BIN01) ;
																		
			if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
			{
				strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
				ReturnCode = TxDataServer_Get_CAF01_Details (DATABASE_READ, 
															Auth_Tx.BIN01_details.caf_profile_id, 
															Auth_Tx.BIN01_details.primary_key.type,
															ST2_DB_SELECT_CAF01_BIN01) ;
				if (ReturnCode == false)
					return SYSTEM_ERROR;
			}
					
		}
		if ((Auth_Tx.BIN01_details.caf_profile_id != NULL) &&
				(Auth_Tx.CAF01_details.primary_key.caf_profile_id != NULL))
		{

			/**** At this point we have both BIN01 and CAF01 records with us. 
			Lets go ahead and validate the profile for standin processing ****/
			if ((strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) ||
				(strcmp(Auth_Tx.CAF01_details.authorization_location,"s") == 0))
			{
				memset(Auth_Tx.TLF01_details.response_code,0x00,sizeof(Auth_Tx.TLF01_details.response_code));

				if(0 == strncmp(Auth_Tx.TLF01_details.response_text,"DE48 SE84:",10) &&
				   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) ||
					0 == strncmp(Auth_Tx.TLF01_details.message_type,"0130",4)) &&
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrdA",7) || 0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3A",7)))
				{
					/* dont do anything for this case*/
				}
				else
				{
					memset(Auth_Tx.TLF01_details.response_text,0x00,sizeof(Auth_Tx.TLF01_details.response_text));
				}


				/* TF Phani/Ajay.. Response text should be Approved , If tx coming from POS
				if(0 != strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
				}
				else
				{
					strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED");
				}
				Auth_Tx.TLF01_details.general_status = GS_TXN_COMPLETED;*/
				strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
				/* At this point of time we might have done all validation and Balnce update also
				 we need to check whether we done or not ..TF PHANI */

				/*In case of Realtime we will have ACF01 details but accout has not been updated
				 * It should be handled like ACF01 recrds have not been updated*/
				 if (0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S") &&
				    true == Falcon_RT_Flag  &&
				    true == txservice_If_Tx_Qualifies_For_RealTime() &&
				    false == txservice_Check_If_IST_Message ())
				{
					if( Auth_Tx.TLF01_details.tx_key > AUTH_INVALID &&
						Auth_Tx.TLF01_details.tx_key  < AUTH_TX_KEY_MAX
						)
					{
						Auth_Tx.TLF01_details.tx_key -= 100;
					}
					else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE)
					{/* This might be BIN01 read error case..we have all BIN details..We can proceed for balance update */

					}
					else
					{  // Error case.. Drop tx..TF PHANI
						sprintf (strMessage, "In standin processing, network_controller: %s,Wrong TX-Key : %d",
						Auth_Tx.TLF01_details.authorizing_host_queue,Auth_Tx.TLF01_details.tx_key);
						txcntl_log_message( 1, 3,strMessage, "txservice_perform_standin_processing", 1 );
						memcpy(Auth_Tx.TLF01_details.response_code,"12",sizeof(Auth_Tx.TLF01_details.response_code));
						memcpy(Auth_Tx.TLF01_details.response_text,"Invalid TX-Key: Pl Check tran type",sizeof(Auth_Tx.TLF01_details.response_text));

							return SYSTEM_ERROR;
					}
					ReturnCode = txserviceCadience_Perform_Balance_Update_For_RT_Tx();

				}
				else if(FieldIsNumeric(Auth_Tx.ACF01_details.primary_key.account_nbr))
				{
				/* Check if account has been updated properly */

						if(Auth_Tx.TLF01_details.update_mask[0] & ACF01_MASK)
						{
							txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
							return GS_TXN_COMPLETED;
						}
						else
						{
							
							ReturnCode=txserviceCadience_Continue_With_ACF01_For_Balance_Update();

						}
				}
				else
				{
						/* Make trnsaction key as request because we are doing STAND IN.. TF PHANI*/
					if( Auth_Tx.TLF01_details.tx_key > AUTH_INVALID && 
						Auth_Tx.TLF01_details.tx_key  < AUTH_TX_KEY_MAX
						) 
					{
						Auth_Tx.TLF01_details.tx_key -= 100;
					}
					else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE)
					{/* This might be BIN01 read error case..we have all BIN details..We can proceed for balance update */
					 
					}
					else
					{  // Error case.. Drop tx..TF PHANI
						sprintf (strMessage, "In standin processing, network_controller: %s,Wrong TX-Key : %d",
						Auth_Tx.TLF01_details.authorizing_host_queue,Auth_Tx.TLF01_details.tx_key);
						txcntl_log_message( 1, 3,strMessage, "txservice_perform_standin_processing", 1 );
						memcpy(Auth_Tx.TLF01_details.response_code,"12",sizeof(Auth_Tx.TLF01_details.response_code));
						memcpy(Auth_Tx.TLF01_details.response_text,"Invalid TX-Key: Pl Check tran type",sizeof(Auth_Tx.TLF01_details.response_text));

							return SYSTEM_ERROR;
					}
					/* TF Phani: During offline state, for AMEX and JCB cards are doing VOICE or on local POS 
					dollar transactions, ATP is rejecting this with a â€œDOLLAR ACCOUNT NOT FOUNDâ€�.
					ATP should have converted this to PESO and debited the PESO account instead.
					--------------------------------------------------------------------------
					Check, if we have to change the currency code.  
					*/
					 /* if ( (strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)) &&
						   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)) ||
							(strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX))) )
					   {
						  strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
						  strcpy( Auth_Tx.TLF01_details.host_start_time, "0.000" );
					   }*/
					ReturnCode = txserviceCadience_Perform_Balance_Update();
					
				}

				if (false == ReturnCode || ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR)
				return ReturnCode;
			}
		}
	}

return NO_AUTH_ERROR;

}/*end of perform_standin_processing - Ravikumar K . N - ThoughtFocus*/





/******************************************************************************
NAME:			txservice_Process_Auth_Request

DESCRIPTION:This is the entry point into the Transaction Controller

INPUTS:		pPTE_MSG p_msg_in: The PTE message dispatched from the Pinnacle
                               Message Handler

OUTPUTS:		None

RETURNS:    NO_AUTH_ERROR: When there is a problem reading the bin table from
                           shared memory and a call was made to the data server
							      Also when there is no problem with the function

				SYSTEM_ERROR:  Whenever any XIPC error occurred

            TRUE:          If transaction was authorized

AUTHOR:     Sanjoy Dasgupta
*******************************************************************************/
BYTE txservice_Process_Auth_Request (pPTE_MSG p_msg_in)
{
   BOOLEAN bReturnCode = true ;
   pBYTE pATEMessageDataData = 0 ;
   pPTE_MSG_DATA pATEMessageData = 0 ;
	
   char strTemp[256] = {0} ;
   char strResponseCode[256] = {0} ;
   BYTE strRRN[15] = {0} ;
   BYTE strDate_yyyymmdd[10] = {0} ;
   BYTE strTime_hhmmss[10] = {0} ;
   BYTE strAuthNumber[10] = {0} ;
   BYTE TransactionOrigin = 0 ;
   BYTE card_digit;

   int   ReturnCode = 0 ;
   BYTE  AccountType = 0 ; //Determines if card is CREDIT, DEBIT, FLEET, LOYALTY

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   /*****************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;
   /*****************************************************************/

   if ( (JCB_ACQ_Contactless_Magstrip_Flag == 1) &&
		(true == txservice_Check_If_JCB_Transaction ()) &&
	    (true == txservice_Check_If_JCB_Contactless_Magstrip_Transaction ())&&
	    (true == txservice_Check_if_POS_Acq_Transaction()))
      {
	     if(0 != strncmp(JCB_ACQ_Contactless_Magstrip_RC, "00", 2))
	     {
	    	 strncpy (strTemp, JCB_ACQ_Contactless_Magstrip_RC, 2) ;
	    	 strcpy(Auth_Tx.TLF01_details.response_text,"Decline JCB ACQ Contactless Magstrip ");
	    	 txservice_Prepare_AuthTx_For_Forwarding(GS_TXN_TRANSACTION_NOT_ALLOWED,
	    			 	 	 	 	 	 	 	 	 strTemp) ;
   			 return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	     }
      }

	/* Return the transaction if it is an ADVICE and the response code is 
    * not NULL, not 00 and not spaces - This is to accommodate Rod Snyder's
    * request.
    */
   if (txservice_If_Tx_Is_Treated_Like_A_Advice () &&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6)))
   {
      memcpy (strResponseCode, Auth_Tx.TLF01_details.response_code, 2) ;
      if(true != txservice_Check_If_DCI_issuing_transaction())
      {
		  if (strResponseCode[0] != 0 && !strEqual (strResponseCode, "  ") &&
			  !strEqual (strResponseCode, "00"))
		  {
			 strcpy (strTemp, "Received ADVICE msg with non 00RespCode\n") ;
			 txservice_Prepare_AuthTx_For_Forwarding(GS_TXN_TRANSACTION_NOT_ALLOWED,
					 strResponseCode) ;
			 //Send incoming advice transaction transaction to Falcon
			 if( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))||
			     (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
			 {
				 return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
			 }
			 else
			 {
				// txservice_Send_Request_To_Falcon_Service();
			     //return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
			 }

		  }
      }
   }

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
   {
      /* For voice authorized transactions, we store the orig_comms_info
       * so as to be able to return to applink when we are done with the
       * transaction.
       */
      strcpy( Auth_Tx.TLF01_details.originator_info,
              ptemsg_get_msg_orig_comms_info (p_msg_in)) ;

      /* Set Flag to '1' to not build DE126 for VOICE in sbatch */
      Auth_Tx.TLF01_details.product_codes[4].quantity[0] = '1';
   }

   /* Perform semantic checks on the transaction */
   ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   ReturnCode = txservice_Filter_Transactions () ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   /* Clear the response code field, if txn is not a reversal or advice. */
   if ( false == txservice_If_Tx_Is_Treated_Like_A_Reversal() &&
		false == txservice_If_Tx_Is_Treated_Like_A_Advice())
   {
	   if(true != txservice_Check_If_DCI_issuing_transaction())
	   {
		   memset( Auth_Tx.TLF01_details.response_code, 0x00,
				   sizeof Auth_Tx.TLF01_details.response_code );
	   }
   }

   strcpy (Auth_Tx.TLF01_details.dispensation.auth_1, "LA") ;
   strcpy (Auth_Tx.TLF01_details.dispensation.auth_2, "LA") ;

   /* Determine if the transaction was locally or remotely acquired */
   TransactionOrigin = txservice_GetTransactionOrigin () ;

   /* It was decided that we populate the date, time, and the RRN if the 
    * transaction originated LOCALLY - for REMOTELY ACQUIRED transactions, we
    * do not touch these fields, except for Cancellations from Voice.
    */
   if ( (TransactionOrigin == LOCAL_ORIGIN) ||
        ((strEqual(Auth_Tx.TLF01_details.handler_queue, "VOICE")) &&
         (txservice_If_Tx_Is_Treated_Like_A_Reversal())         )  )
   {
      txservice_Create_Date_Time_RRN_For_Auth_Tx( strDate_yyyymmdd,
                                                  strTime_hhmmss,
                                                  strRRN );

      if (strlen (Auth_Tx.TLF01_details.date_yyyymmdd) == 0)
         strcpy (Auth_Tx.TLF01_details.date_yyyymmdd, strDate_yyyymmdd) ;

      if (strlen (Auth_Tx.TLF01_details.time_hhmmss) == 0)
         strcpy (Auth_Tx.TLF01_details.time_hhmmss, strTime_hhmmss) ;

      if (strlen (Auth_Tx.TLF01_details.retrieval_ref_num) == 0)
         strcpy (Auth_Tx.TLF01_details.retrieval_ref_num, strRRN) ;
   }

   /* Copy one of these value to TLF01 for extraction by log_file app. */
   if ( Auth_Tx.TLF01_details.card_num[0] != 0x00 )
      card_digit = Auth_Tx.TLF01_details.card_num[0];
   else if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
      card_digit = Auth_Tx.TLF01_details.track2[0];
   else
      card_digit = Auth_Tx.TLF01_details.track1[0];

   if ( '4' == card_digit )
   {
      Auth_Tx.TLF01_details.product_codes[19].code[0] =
                                           Auth_Tx.MCF01_details.vs_tcc_code[0];
   }
   else if ( '5' == card_digit )
   {
      Auth_Tx.TLF01_details.product_codes[19].code[0] =
                                           Auth_Tx.MCF01_details.mc_fcc_code[0];
   }
   else
   {
      Auth_Tx.TLF01_details.product_codes[19].code[0] = ' ';
   }

   /* Figure out if the card is a DEBIT or a CREDIT from the processing code */
   /* The returned AccountType will be either 'C' or a 'D' */
   txservice_Get_Bin_Type_From_Processing_Code (&AccountType) ;

   /* If Credit, need to check to see if it is a deferred. */
   if ( AccountType == TYPE_CREDIT )
   {
      if ( txservice_If_Tx_Is_Treated_Like_A_Deferred() )
      {
         /* Yes it is. Use the Deferred bin type. */
         AccountType = TYPE_DEFERRED;
      }
   }

   /* Set the bin_type in Auth_Tx - used later in grouping txns in the batch */
   Auth_Tx.bin_type = AccountType ;

   /* Get the BIN record for the card */
   /* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
   ReturnCode = TxDataServer_Get_BIN01_Details (SHARED_MEMORY_READ) ;

   if (ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
   {
      // bin range is not found in shared memory
      // if bin type is CREDIT, return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR
      // if bin type is DEBIT, search the BIN02 table
      ReturnCode = txservice_Process_When_Bin_Range_Not_Found () ;
      return ReturnCode ;
   }
   else if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY)
   {
      // network, return NO_AUTH_ERROR
      // if the auth_ind is 1 start the local processing, return NO_AUTH_ERROR
      // if system error return SYSTEM_ERROR

	  /* EQIT Interface Changes, Jan 02 2009, Girija Y, TF */
      /* Added code to pass network Id to eqit */

      txservice_Eqit_POS_Field63(&Auth_Tx,Auth_Tx.BIN01_details.destination_name);

#ifdef MC13_OCT2023
	if( Auth_Tx.MCF01_details.division_nbr[0] == 'C'     &&
		txcntl_check_if_txn_falls_under_mac_validation() &&
		(true == txservice_Check_If_MC_Transaction()) 	 &&
		txcntl_check_if_cnp_txn_flag_is_enabled_with_config_pos_codes()
	  )
	{
		ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(MAC_CATEGORY_CODE);
	}
	else
#endif
#ifdef VISA7
	if( Auth_Tx.MCF01_details.division_nbr[0] == 'C' &&
		txcntl_check_if_txn_falls_under_merchant_ingerity_validation() &&
		(true == txservice_Check_If_VISA_Transaction() ||true == txservice_Check_If_MC_Transaction()))
	{
		if( true == txservice_Check_If_VISA_Transaction() &&
		   (cat1_MID_integrity_validation_flag == 'Y'     ||
			cat2_MID_integrity_validation_flag == 'Y'     ||
			cat3_MID_integrity_validation_flag == 'Y'     ||
			cat4_MID_integrity_validation_flag == 'Y'    ))
		{
			ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(VISA_CATEGORY_CODE);
		}
	#ifdef MC12_OCT2023
		else if(true == txservice_Check_If_MC_Transaction() &&  excessive_declines_flag == 'Y')
		{
			ReturnCode = TxDataServer_Get_Merchant_Integrity_Param_Details(MC_CATEGORY_CODE);
		}
		else
		{
			ReturnCode = txservice_Process_When_Bin_Range_Found () ;
		}
	#endif
	}
#endif
	else
	{
		ReturnCode = txservice_Process_When_Bin_Range_Found () ;
	}


      return ReturnCode ;
   }
   else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
   {
      // if problems with reading shared memory, read the 
      // bin table from the database

	  /* EQIT Interface Changes, Jan 02 2009, Girija Y, TF */
      /* Added code to pass network Id to eqit */

      txservice_Eqit_POS_Field63(&Auth_Tx,Auth_Tx.BIN01_details.destination_name);

      ReturnCode = TxDataServer_Get_BIN01_Details (DATABASE_READ) ;
      return ReturnCode ;
   }

   return NO_AUTH_ERROR ;
} /* txservice_Process_Auth_Request */


INT txcntl_check_if_txn_falls_under_merchant_ingerity_validation()
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6) &&
	    (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0200",4) ||
	    (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4)   &&
	     (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"30",2)   ||
		 0 == strncmp(Auth_Tx.TLF01_details.processing_code,"38",2)))) ||
		(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) &&
		 0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4)))
		{
			return 1;
		}
	else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4)  &&
		    (Auth_Tx.mac_key[0] == AUTH_PRE_AUTHORIZATION			 ||
						Auth_Tx.mac_key[0] == AUTH_CARD_VERIFICATION)))
	{
		return 1;
	}
		return 0;
}

#ifdef MC13_OCT2023
INT txcntl_check_if_txn_falls_under_mac_validation()
{
	if( ( ((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,       "dcpiso",                6)) ||
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,       "dcpos",                 5)))    &&
		  (((0 == strncmp(Auth_Tx.TLF01_details.message_type,       "0200",                  4)) &&
		    (0 == strncmp(Auth_Tx.TLF01_details.processing_code,    "00",                    2)))  ||
		   ((0 == strncmp(Auth_Tx.TLF01_details.message_type,       "0100",                  4)) &&
		    ((0 == strncmp(Auth_Tx.TLF01_details.processing_code,   "30",                    2)) ||
		     (0 == strncmp(Auth_Tx.TLF01_details.processing_code,   "38",                    2)))))   &&
		  (((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,     mac_pos_entry_mode_cat1, 3)) &&
		    (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, mac_pos_cond_code_cat1,  2)))  ||
		   ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,     mac_pos_entry_mode_cat2, 3)) &&
		    (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, mac_pos_cond_code_cat2,  2)))) ) ||
	    ( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE", 5)) &&
		  (0 == strncmp(Auth_Tx.TLF01_details.message_type,  "0100",  4)) )
	  )
	{
		return 1;
	}

	return 0;
}

INT txcntl_check_if_cnp_txn_flag_is_enabled_with_config_pos_codes()
{
	if((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,     mac_pos_entry_mode_cat1, 3)) &&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, mac_pos_cond_code_cat1,  2)) &&
	   mac_cat1_validation_flag == 'Y')
	{
		return 1;
	}
	else
	if((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,     mac_pos_entry_mode_cat2, 3)) &&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, mac_pos_cond_code_cat2,  2)) &&
	   mac_cat2_validation_flag == 'Y')
	{
		return 1;
	}
	else
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE", 5)) &&
		mac_cat1_validation_flag == 'Y' )
	{
		//POS codes will be hard coded for VOICE and it falls under CAT1 as it is Manual Key-in
		return 1;
	}

	return 0;
}
#endif

BYTE txservice_Process_When_Bin_Range_Not_Found (void)
{
   BYTE AccountType = 0 ;
   BYTE ReturnCode = 0 ;
   char strTemp [256] = {0} ;
   char tlf01_issuerid[5] = {0};
   INT  len;

   AccountType = Auth_Tx.bin_type;


   /*SCR1443 08/10/04- Equitable reported issue where POS debit transactions are routed to 
     Cirrus and should not be.  Only transaction from nceqit which are ATM transactions will search bin02
	 if bin01 search fails.  If the bin02 search fails, the ATM transactions can be 
	 sent to Cirrus.  All other transactions that fail bin01 search should decline. 

	 Equitable confirmed the routing rules as follows:
	 Send txn to Cirrus when the following conditions are met:
     -  Record not found in Bin01
     -  Record not found in Bin02
     -  Card number starts with 3, 4, 5, or 6
     -  Transaction is ATM (issuer = nceqit)
	*/
  // strncpy(tlf01_issuerid, Auth_Tx.TLF01_details.issuer_id, 4);
  // len = strlen( EB_Host_Issuer_ID );

   /*if ( 0 == strncmp(EB_Host_Issuer_ID, tlf01_issuerid, len) )
   {
      // This is a transaction from Equitable's host.
      ReturnCode = TxDataServer_Get_BIN02_Details () ;
   }
   else*/ if ( (TYPE_CREDIT == AccountType) || (TYPE_DEBIT == AccountType) || (TYPE_DEFERRED == AccountType) )
   {
      if ( TYPE_CREDIT == AccountType )
         strcpy (strTemp, "Unsupported card; bin not found (credit)") ;
	  else 
	  if ( TYPE_DEBIT == AccountType )
		  strcpy (strTemp, "Unsupported card; bin not found (debit)") ;
      else
         strcpy (strTemp, "Unsupported card; bin not found-Deferred") ;

      if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
          strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

      txcntl_log_message( 1, 3,strTemp, "txservice_Process_When_Bin_Range_Not_Found", 1 );
      txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_BIN_NOT_FOUND,REFER_TO_CARD_ISSUER) ;
      ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
   }

   return ReturnCode ;
} /* txservice_Process_When_Bin_Range_Not_Found */

BYTE txservice_Process_When_Bin_Range_Found (void) 
{
   BYTE    ReturnCode        = NO_AUTH_ERROR ;
   BYTE    TransactionOrigin = 0 ;
   BYTE    resp_code[3];
   BYTE    gs;
   CHAR    strTemp[256]      = {0} ;

   TransactionOrigin = txservice_GetTransactionOrigin () ;
   strncpy(Auth_Tx.TLF01_details.product_codes[2].code,Auth_Tx.BIN01_details.primary_key.low,6);// BIN LOW

   /* If this is a deferred txn, need to set bin type back to credit now.
    * Otherwise, we will not be able to find CAF and Card records.
    */
   if ( Auth_Tx.BIN01_details.primary_key.type[0] == TYPE_DEFERRED )
      Auth_Tx.BIN01_details.primary_key.type[0] = TYPE_CREDIT;

   /* Determine, for later calculations, if this is
    * an RSB (Robinson Savings Bank) card.  It will be
    * treated similar, but not exactly, like a deferred
    * transaction.  Want to flag it here in case of error
    * so it is logged into TLF01 as an RSB card.
    */
   if ( Auth_Tx.BIN01_details.credit_subtype[0] == RSB_CARD )
      strcpy( Auth_Tx.TLF01_details.type_of_data, RSB_FLAG );

   /* Filter Visa Electron (VE) cards; Equitable requirements.
    *
    * VE is not allowed for non-card swipe transactions.  Also
    * VE is not allowed for Voice or Deferreds. It also is not
    * allowed for authorization type messages. It only pertains
    * to 0200 and related type transactions. Need to be careful
    * here with 0100 transactions, because we can get 0100 debit
    * transactions from the networks.
    *
    * Allow manual txn for VE Prepaid Cash Reload txns (SCR 13514).
    */
   if ( (Auth_Tx.BIN01_details.credit_subtype[0] == 'E') ||
        (Auth_Tx.BIN01_details.credit_subtype[0] == 'P'))  
   {
      /* This is a Visa Electron (VE) transaction. */
      memset( resp_code, 0x00, 3 );

	/* Do not valide for VE card for RPS trasnction Dt: 21 Sep 2010- Kenneth : TF Phani*/
	  if(TransactionOrigin == LOCAL_ORIGIN && Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
	  {// SKIP validations
	  }
	  else
	  {
	      if (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"21",2))
	      {
	         /* This is a VE Cash Reload (Prepaid). */
	         if ( Auth_Tx.BIN01_details.credit_subtype[0] != 'P' )
	         {
	            /* Credit Subtype not set properly for Prepaid. Block it. */
	            strcpy( resp_code, TRANSACTION_NOT_PERMITTED );
	            strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                   "VE Prepaid: Invalid Credit Subtype" );
	         }
	      }
	      else if ( Auth_Tx.TLF01_details.pos_entry_mode[2] != '2' )
	      {
	         /* This is a manually entered transaction. */
	         if ( false == txservice_If_Tx_Is_Treated_Like_A_Reversal() )
	         {
	            /* It is not a reversal. */
	            if ( Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD )
	            {
	               /* Cannot allow a manual VE Prepaid if not a Cash
	                * Reload except for cash reload voids.  Block it.
	                */
	               strcpy( resp_code, TRANSACTION_NOT_PERMITTED );
	               strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                      "VE cards not allowed for manual txns" );
	            }
	         }
	      }

	      if ( 0 == strcmp(Auth_Tx.TLF01_details.handler_queue, "VOICE") )
	      {
	         /* This is a Voice transaction - block it. */
	         strcpy( resp_code, TRANSACTION_NOT_PERMITTED );
	         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                "VE cards not allowed for Voice Auths" );
	      }
	      else if ( txservice_If_Tx_Is_A_Deferred_Purchase_Type() )
	      {
	         /* This is a Deferred transaction - block it. */
	         strcpy( resp_code, TRANSACTION_NOT_PERMITTED );
	         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                "Deferred Txns not allowed for VE cards" );
	      }
	      else if ( (0 == strcmp(Auth_Tx.industry_code,"700")) ||
	                ( (TransactionOrigin == LOCAL_ORIGIN) &&
	                  (Auth_Tx.int_net_facility_len[0] != 0x00))  )
	      {
	         /* E-commerce txns are not allowed for VE cards.
	          * This 'if' checks for e-commerce 2 ways: Industry code,
	          * local txn with data in int_net_facility_len (which is
	          * where dcpiso puts the e-commerce info).
	          */
	         strcpy( resp_code, TRANSACTION_NOT_PERMITTED );
	         strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                "E-commerce Txns not allowed for VE cards" );
	      }

	      /* Let debit transactions through, but not 0100 local credits. */
	      else if ( Auth_Tx.BIN01_details.primary_key.type[0] != TYPE_DEBIT )
	      {
	         /* This is not a debit; if from a local device, do not
	          * allow if this is an authorization type transaction.
	          */
	         if ( TransactionOrigin == LOCAL_ORIGIN )
	         {
	            /* This is a credit transaction from a local device (not from
	             * a network).  Since this is a VE card, block it for auth
	             * transactions.
	             */
	            if ( Auth_Tx.TLF01_details.message_type[1] == '1' )
	            {
	               /* Block it except for Balance Inquiry. */
	               /* Allow this txn for Prepaid. */
	               if ( Auth_Tx.BIN01_details.credit_subtype[0] != 'P' )
	               {
	                  /* This is a VE txn of the non-Prepaid type.
	                   * For 0100's, only allow Balance Inquiry. But
	                   * Balance Inquiry must be a swipe with pin.
	                   */
	                  if ( Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY )
	                  {
	                     /* This is a balance inquiry; must be swipe with pin. */
	                     if ( (Auth_Tx.TLF01_details.pos_entry_mode[2] != '2' ) ||
	                          (Auth_Tx.TLF01_details.pin_block     [0] == 0x00)  )
	                     {
	                        /* Either pin is missing, or not a swipe. Block Txn. */
	                        strcpy( resp_code, INVALID_TRANSACTION );
	                        strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                               "VE cards not allowed for auth trans (b).");
	                     }
	                  }
	                  else
	                  {
	                     /* Not a balance inquiry, so block it. */
	                     strcpy( resp_code, INVALID_TRANSACTION );
	                     strcpy( Auth_Tx.TLF01_details.voice_auth_text,
	                            "VE cards not allowed for auth trans (a).");
	                  }
	               }
	            }
	         }
	      }

	      if ( resp_code[0] != 0x00 )
	      {
	         gs = GS_TXN_TRANSACTION_NOT_ALLOWED;
	         txcntl_log_message( 1, 2,Auth_Tx.TLF01_details.voice_auth_text,
	        		 	 	 	 "txservice_Process_When_Bin_Range_Found", 1 );
	         txservice_Prepare_AuthTx_For_Forwarding( gs, resp_code ) ;
	         ReturnCode = ERROR_BUT_DO_NOT_FORWARD_TO_UPDATE_PROCESSOR ;
	      }
	  }/* End of RPS if else*/
   }/* End of VE card if  */

   if ( ReturnCode == NO_AUTH_ERROR )
   {
      /* If the transaction is locally acquired,before switching out 
       * do the merchant velocity checks, if any check fails,
       * the function returns ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR,
       * else NO_AUTH_ERROR
       */
      if (TransactionOrigin == LOCAL_ORIGIN)
      {
         if ( (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE)   ||
              (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND) ||
              (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE) ||
			  (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD))
         {
            ReturnCode = txservice_Continue_Processing_When_Bin_Range_Found () ;
         }
         else
         {
            ReturnCode = TxAuthorize_Perform_Merchant_Velocity_Checks () ;
            if (ReturnCode == NO_AUTH_ERROR)
            {
               if ( false == TxDataServer_Get_MCT01_Details() )
                  ReturnCode = SYSTEM_ERROR ;
            }
			//TF Praneeth process tx while OMC
			else if(Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
			{
				ReturnCode = TxDataServer_Get_OMC01_Details(ONUS);
				return ReturnCode ;
			}
			//end of TF praneeth
         }
      }
      else 
      {
         ReturnCode = txservice_Continue_Processing_When_Bin_Range_Found () ;
      }
   }
   return( ReturnCode );
} /* txservice_Process_When_Bin_Range_Found */

BYTE txservice_Continue_Processing_When_Bin_Range_Found (void) 
{
   CHAR    strTemp[256] = {0} ;
   BYTE    ReturnCode = 0 ;
   INT     AuthIndicator = 0 ;
   INT     i = 0;
   INT     acquirer_result,mcc_result ; 
   BOOLEAN bin_blocked  ;
   char strResponseCode[256] = {0} ;

	
   /* Perform MOD-10 Check if configured to do so. */
   if ( Auth_Tx.BIN01_details.mod_check[0] & MOD10_ON )
   {
	  if (true ==  txservice_Check_If_VISA_Transaction() &&
			  true == txservice_Check_If_VISA_issuing_transaction() &&
			  0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4) &&
			  0==strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))
	  {
		  //Do nothing for the invalid card number for refund transaction
	  }
	  else if(true==txservice_check_for_AMEX_refund_txn())
	  {
		  //Do nothing for the invalid card number for refund transaction
	  }
	  else if ( !ValidateCardNumber() )
      {
         sprintf( strTemp,
                 "Card number (%s) failed Mod 10 check.",
                  Auth_Tx.TLF01_details.card_num );
         ReturnCode = txservice_Generate_Usage_Error_Message (0,strTemp, 
                           "txservice_Continue_Processing_When_Bin_Range_Found",
                            GS_TXN_TRANSACTION_NOT_ALLOWED, 
                            INVALID_ACCOUNT_NUMBER) ;
         return ReturnCode ;
      }
   }

   acquirer_result = validate_acquirer_id();
   if(BLOCKED == acquirer_result)
   {
	   sprintf( strTemp,
			    "Tx blocked. Acquirer %s",
			    Auth_Tx.TLF01_details.acquiring_id );
	   if(true == txservice_Check_If_MC_Transaction())
	   {
		   ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
		   	                             "txservice_Continue_Processing_When_Bin_Range_Found",
		   	                              GS_TXN_TRANSACTION_NOT_ALLOWED,
										  DO_NOT_HONOR) ;
	   }
	   else
	   {
		   ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
										 "txservice_Continue_Processing_When_Bin_Range_Found",
										  GS_TXN_TRANSACTION_NOT_ALLOWED,
										  TRANSACTION_NOT_PERMITTED) ;
	   }
	   return ReturnCode ;
   }

   mcc_result = validate_mcc_range();
   if(BLOCKED == mcc_result)
   {
		sprintf( strTemp,
			  "Tx blocked. MCC %s",
			   Auth_Tx.TLF01_details.category_code );
		if(true == txservice_Check_If_MC_Transaction())
		{
		   ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
										 "txservice_Continue_Processing_When_Bin_Range_Found",
										  GS_TXN_TRANSACTION_NOT_ALLOWED,
										  INVALID_MERCHANT) ;
		}
		else
		{
			ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp,
										 "txservice_Continue_Processing_When_Bin_Range_Found",
										  GS_TXN_TRANSACTION_NOT_ALLOWED,
										  TRANSACTION_NOT_PERMITTED) ;
		}
	  return ReturnCode ;
  }


   if (txservice_If_Tx_Is_Treated_Like_A_Advice () &&
   	(0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6)))
   	{
   		memcpy (strResponseCode, Auth_Tx.TLF01_details.response_code, 2) ;
   		if(true != txservice_Check_If_DCI_issuing_transaction())
   		{
   			if (strResponseCode[0] != 0 && !strEqual (strResponseCode, "  ") &&
   				!strEqual (strResponseCode, "00"))
   			{
   				strcpy (strTemp, "Received ADVICE msg with non 00RespCode\n") ;
   				txservice_Prepare_AuthTx_For_Forwarding(GS_TXN_TRANSACTION_NOT_ALLOWED,
   						strResponseCode) ;
   				//Send incoming advice transaction transaction to Falcon
   				if( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))||
   						(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
   				{
   					return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
   				}
   				else
   				{
   					if(true == txservice_Check_If_IST_Message())
   					{
   						// Let it flow down

   					}
   					else
   					{
   						txservice_Send_Request_To_Falcon_Service();
   						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;

   					}



   				}

   			}
   		}
   	}


   if(((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
       (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))) )
	  {
	    // Do nothing ,Let is fall to normal flow.Its issuing transaction without MID
	  }

   /*
    * Let check Merchant ID is blocked for this bin or not.
    * We shall do this check its issuing transaction coming for Schema
    *
    * */
   else if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"neqi",4))||
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nceq",4))||
		   (0 == strncmp(Auth_Tx.BIN01_details.destination_name,"neqi",4))||
		   (0 == strncmp(Auth_Tx.BIN01_details.destination_name,"nceq",4)))
   {
	   // Do nothing , Let is fall to normal flow
   }
   else if(0 == strlen(Auth_Tx.TLF01_details.merchant_id))
   {
	   // Do nothing ,Let is fall to normal flow.Its issuing transaction without MID
   }
   else
   {
	   /* Its Issuing transaction.
	    *  Lets get the Blocked MID list assosiated with this and
	    *  see if this MID is present in that list or not.*/
	   ReturnCode = TxDataServer_Get_MID_Blockling_Lisgt_Assoisated_with_Bin();
	   if (SYSTEM_ERROR == ReturnCode)
	   {
		   // We have failure in getting bin table details
		   // lets log error message and allow this transaction to continue
		   sprintf (strTemp, "Failed to send MID BIN BLOCK detail request for BIN  %s",
		            Auth_Tx.BIN01_details.primary_key.low) ;
		   txcntl_log_message( 1, 3,strTemp,"txservice_Process_When_Bin_Range_Found", 1 );
	   }
	   else
	   {
		   return MID_BIN_BLK_OK;
	   }
   }


   if (Auth_Tx.BIN01_details.auth_ind[0] == 0 || 
       (Auth_Tx.BIN01_details.auth_ind[0] != '1' &&
        Auth_Tx.BIN01_details.auth_ind[0] != '0') )
   {
      sprintf (strTemp, "Invalid auth_indicator value %s in BIN\n", 
               Auth_Tx.BIN01_details.auth_ind) ;
      if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
      strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
      txcntl_log_message( 1, 2,strTemp,"txservice_Process_When_Bin_Range_Found", 1 );
      txservice_Prepare_AuthTx_For_Forwarding (GS_INVALID_AUTH_INDICATOR,REFER_TO_CARD_ISSUER) ;
      return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
   }

   /* Switch out to the network if the auth_ind is 0 */
   AuthIndicator = atoi (Auth_Tx.BIN01_details.auth_ind) ;
   if (AuthIndicator == REMOTE_PROCESSING_INDICATOR) /* 0 */
   {
      if (txservice_If_Tx_Is_A_Deferred_Purchase_Type ())
      {
         /* Do not switch the transaction out to the networks
          * for deferred transactions.
          */
         strcpy (strTemp, "Cannot send Def Purchase to remote n/w") ;
         ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp, 
                           "txservice_Continue_Processing_When_Bin_Range_Found",
                            GS_TXN_TRANSACTION_NOT_ALLOWED, 
                            REFER_TO_CARD_ISSUER) ;
         return ReturnCode ;
      }

		if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) ||
				0 == strncmp(Auth_Tx.TLF01_details.message_type,"0130",4)||
				0 == strncmp(Auth_Tx.TLF01_details.message_type,"1120",4))
      {
			if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6))
      {

      ReturnCode = 
            txservice_Process_Bin_Range_Switch_Out (BIN01_DATA, 
                                   Auth_Tx.BIN01_details.destination_name,
                                   Auth_Tx.BIN01_details.alt_destination_name) ;
      return ReturnCode ;
			}
			else
			{
				//drop the advice transaction flowing to IST for other transactions
			}
      }
      else
      {

    	  // support for other types of transactions.
      ReturnCode =
            txservice_Process_Bin_Range_Switch_Out (BIN01_DATA,
                                   Auth_Tx.BIN01_details.destination_name,
                                   Auth_Tx.BIN01_details.alt_destination_name) ;
      return ReturnCode ;
      }


   }
   else if (AuthIndicator == LOCAL_PROCESSING_INDICATOR) /* 1 */
   {
      /* Start local processing */
      ReturnCode = txservice_Start_Local_Processing () ;
      return ReturnCode ;
   } 

   return NO_AUTH_ERROR ;
} /* txservice_Continue_Processing_When_Bin_Range_Found */

BYTE txservice_Process_Bin_Range_Switch_Out (BYTE  context,
                                             pBYTE destination_name,
                                             pBYTE alt_destination_name) 
{
   BOOLEAN  bReturnCode = true ;
   BOOLEAN  bValidDestinationName = false ;
   BYTE     strMessage[256] = {0} ;
   BYTE     ReturnCode;
   BYTE     AccountType;
   INT      txn_went_out = false;

   CHAR     remote_host_que_name    [MAX_QUE_NAME_SIZE+1] = {0} ;
   CHAR     alternate_host_que_name [MAX_QUE_NAME_SIZE+1] = {0} ;
   SAF01    sSAF01;
   CHAR     strTemp[256] = {0} ;
   INT      TransactionOrigin = 0 ;
   INT		len = 0;

   /*Implement Diners routing for PULSE
    *
    * */
   if ( 0 == strncmp(destination_name, diners_host_que_name,5) )
     {
        /* This is destined for Diners network. Check if it is from IST send to PULSE */
         if ( strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",4)== 0 ||
        	  strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit",4)== 0
         )
        {
           /* DCI debit must go to the PULSE ATM network. */
			if(0 == strncmp(destination_name, "ncdci2",6))
			{
				// Let the transaction go to Cadencie. Route only offus pulse
			}else
			{
			   strcpy( destination_name, diners_atm_que_name );
			   strcpy( alt_destination_name, diners_atm_que_name );
			}
        }
     }


   /* Implement Visa SMS routing; if anything is going to Visa,
    * send it to Visa SMS if a pin exists.  SMS is the ATM host
    * for Visa.
    */
   if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 )
   {
      /* A pin does exist.  Is the destination ncvisa? */
      if ( 0 == strcmp(destination_name, plus_host_que_name) )
         strcpy( destination_name, vsms_host_que_name );

      if (alt_destination_name != NULL_PTR)
         if ( 0 == strcmp(alt_destination_name, plus_host_que_name) )
            strcpy( alt_destination_name, vsms_host_que_name );
   }

   /* Implement Amex ATM & POS routing.  Amex bins can go to either
    * the POS network or the ATM network.  The difference is in the
    * transaction type.  Debits go to the ATM network (ncatma);
    * Credits go to the POS network (ncposa).
    */
   if ( 0 == strcmp(destination_name, amex_pos_que_name) )
   {
      /* This is destined for Amex POS network. */
      txservice_Get_Bin_Type_From_Processing_Code( &AccountType );
      if ( (AccountType == TYPE_DEBIT) ||
           (Auth_Tx.TLF01_details.pin_block[0] != 0x00) )
      {
         /* Amex debits must go to the ATM network. */
         strcpy( destination_name, amex_atm_que_name );
      }
   }

/********************** ThoughtFocus Girija Y *********************************/

   /* Implement Amex ATM & POS routing.  Amex bins can go to either
    * the POS network or the ATM network.  The difference is in the
    * transaction type.  Debits go to the ATM network (natma2);
    * Credits go to the POS network (nposa2).
    */
   if ( 0 == strcmp(destination_name, cadencie_pos_que_name) )
   {
      /* This is destined for Amex POS network. */
      txservice_Get_Bin_Type_From_Processing_Code( &AccountType );
      if (AccountType == TYPE_CREDIT)
      {
    	  /* do nothing for credit flow with PIN */
      }
      else if ( (AccountType == TYPE_DEBIT) ||
           (Auth_Tx.TLF01_details.pin_block[0] != 0x00) )
      {
         /* Amex debits must go to the ATM network. */
         strcpy( destination_name, cadencie_atm_que_name );
      }
	  /* TF Phani .. Reversal may not have PIN BLOCk ..Double check once */

	  if(0 == strcmp(Auth_Tx.TLF01_details.handler_queue,"ncatmA"))
	  {
		  /* Destination should be  natma2 */
		  if(0 == strcmp(destination_name,cadencie_pos_que_name))
		  {
			strcpy( destination_name, cadencie_atm_que_name );
		  }
	  }
   }

   memset (&sSAF01, 0, sizeof (SAF01) ) ;
   strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
   strcpy( Auth_Tx.TLF01_details.authorizing_host_queue, destination_name );

   /* Check to see if this is a Visa Electron transaction.
    * If it is, need to verify CVV, if not already verified.
    * Flag used to determine if it has been verified is
    * Auth_Tx.effective_date.  Use this field to avoid
    * modifying Auth_Tx.  This field is not used for Visa txns;
    * it is an Amex field.  No need to check reversals.
    */
   if ( (!txservice_If_Tx_Is_Treated_Like_A_Reversal() ) &&
        (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE) &&
        (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_CASH_ADVANCE) &&
		(Auth_Tx.MCF01_details.rps_merchant[0] !='Y')) 
		/* RPS Tx- Don't validate For ELECTRON and PREPAID	*/
   {
      if ( ( (Auth_Tx.BIN01_details.credit_subtype[0] == 'E') ||
             (Auth_Tx.BIN01_details.credit_subtype[0] == 'P')  ) &&
           (Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD)    &&
           (Auth_Tx.TLF01_details.pos_entry_mode[2] == '2')       )
      {
         if ( Auth_Tx.effective_date[0] != 1 )
         {
            /* This is a Visa Electron whose CVV has not been checked yet.
             * Validate the Service Code for Visa Electron.
             */
            if   ( true == txservice_is_txn_visa_electron( Auth_Tx ) )
            {
               /* It is Visa Electron.  Validate Expiration Date. */
               ReturnCode = 
                 TxAuthorize_Validate_Expiry_Date(Auth_Tx.TLF01_details.exp_date);

               if (ReturnCode != NO_AUTH_ERROR)
                  return ReturnCode ;

               /* Go validate CVV with racal. */
               Auth_Tx.effective_date[0] = 1;   /* CVV Flag for the return */

        	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
        		   (true == txservice_Check_If_MC_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																				   MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				   0, 0) ;
        	   }
        	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
        			    (true == txservice_Check_If_VISA_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																				   MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				   0, 0) ;
        	   }
        	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
        				(true == txservice_Check_If_AMEX_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																				  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				  0, 0) ;
        	   }
        	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
        				(true == txservice_Check_If_JCB_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																				   MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				   0, 0) ;
        	   }
        	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
        			    (true == txservice_Check_If_CUP_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																				   MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				   0, 0) ;
        	   }
        	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
        			    (true == txservice_Check_If_Diners_Transaction()))
        	   {
        		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
        				  	  	  	  	  	  	  	  	  	  	  	  	  	  	   MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																				   0, 0) ;
        	   }
        	   else
        	   {
        		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
                                                        				   MT_NSP_VERIFY_MAGSTRIPE,
																		   0, 0) ;
        	   }

               if (false == bReturnCode)
               {
                  return (SYSTEM_ERROR) ;
               }
               else
               {
            	   return (NO_AUTH_ERROR) ;
               }
            }
            else
            {
               /* Invalid Service Code for Visa Electron transaction. */
               strcpy (strTemp, "Invalid service code for Visa Electron") ;
               txcntl_log_message( 1, 2,strTemp,"txservice_Process_Bin_Range_Switch_Out", 1 );
               /* Set error response code based on origin. */
               TransactionOrigin = txservice_GetTransactionOrigin () ;
               if (TransactionOrigin == LOCAL_ORIGIN )
                  txservice_Prepare_AuthTx_For_Forwarding 
                     (GS_TXN_INVALID_TRACK2_SERVICE_CODE,RCI_SPECIAL_CONDITION);
               else
                  txservice_Prepare_AuthTx_For_Forwarding 
                     (GS_TXN_INVALID_TRACK2_SERVICE_CODE,DO_NOT_HONOR);

               return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
            }
         }
      }
   }


/*   09/21/04 LM: SCR1497/12497 - if origin is remote origin and from ncequit (ATM) and destination  = ncjcb,
				  check for selected jcb bin ranges that are meant to go to nccirrus.  If found, route these 
				  ATM transactions to Cirrus.  These selected BIN ranges are hard-coded here in
				  txservice_is_selected_jcb_bin_for_cirrus until a proper design is approved to route by 
				  transaction type.
*/
   len = strlen( EB_Host_Issuer_ID );

   if ( strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO") &&
	    0 == strcmp (destination_name, jcb_host_que_name)  &&
	    0 == strncmp(EB_Host_Issuer_ID, Auth_Tx.TLF01_details.issuer_id, len) )
   {
	   if( txservice_is_selected_jcb_bin_for_cirrus() )
	   {
		  strcpy( destination_name, cirrus_host_que_name );

	   }

   }
/*END 09/21/04 LM*/

   if (destination_name != NULL_PTR)
   {
      bValidDestinationName = true ;
      strcpy (remote_host_que_name, destination_name) ;
      strcat (remote_host_que_name, "A") ;
   }
   else
      bValidDestinationName = false ; // que name empty so error 


   if (alt_destination_name != NULL_STR)
   {
      bValidDestinationName = true ;
      strcpy (alternate_host_que_name, alt_destination_name) ;
      strcat (alternate_host_que_name, "A") ;
   }

   if (!bValidDestinationName)
   {
      // Invalid remote and alternate destination names
      strcpy (strTemp, "Cannot switch card out-destination NULL") ;
      if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
          strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
      txcntl_log_message( 1, 2,strTemp,"txservice_Process_Bin_Range_Switch_Out", 1 );
      txservice_Prepare_AuthTx_For_Forwarding 
                           (GS_TXN_INVALID_DESTINATION,DESTINATION_UNAVAILABLE);
      return SYSTEM_ERROR ;
   }	

   /* Determine if transaction qualifies for Cash Bonus
    * Program prior to sending it out to a remote host.
    */
   if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
   {
      /* It does qualify; set the flag for updatds. */
      ReturnCode = TxAuthorize_Update_AuthTx_Structures( 0, 1, 0 );
   }

   if (context == BIN01_DATA)
   {
	  /*Check if transaction qualifying for real time*/
	  if (0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S") &&
		  true == Falcon_RT_Flag  &&
		  true == txservice_If_Tx_Qualifies_For_RealTime() &&
		  false == txservice_Check_If_IST_Message ())
	  {
		  /*Lets Get CCF02P details */
		   ReturnCode = TxDataServer_Get_CCF02_Details_For_RT ();
		   if (ReturnCode == NO_AUTH_ERROR)
		   {
				return NO_AUTH_ERROR;
		   }
		   else
		   {
			   txcntl_log_message( 2, 3, "Failed to retrieve CCF02P details for FRT transaction", "txservice_Process_Bin_Range_Switch_Out", 1 );
			   ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
			   if(false == ReturnCode)
			   {
				   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "txservice_Process_Bin_Range_Switch_Out", 1 );
				   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
											  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
											   MT_AUTH_REQUEST,
											   0,
											   0) ;
				   return ReturnCode ;
			   }
			   else
			   {
				   return ReturnCode ;
			   }
		   }
	  }
	  else if (txservice_Is_Host_Up (destination_name))
      {

         strcpy (Auth_Tx.TLF01_details.authorizing_host_queue,
                 remote_host_que_name) ;

         bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                                   (remote_host_que_name, MT_AUTH_REQUEST, 0,0);
         if (false == bReturnCode)
            return SYSTEM_ERROR ;

      }
      else if (txservice_Is_Host_Up (alt_destination_name))
      {
         strcpy (Auth_Tx.TLF01_details.authorizing_host_queue,
                 alternate_host_que_name) ;

         bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                                (alternate_host_que_name, MT_AUTH_REQUEST, 0,0);
         if (false == bReturnCode)
            return SYSTEM_ERROR ;

      }
      else
      {
         //if ( 0 == strncmp(destination_name, "ncmc", 4) )
         if ( 0 == strcmp(destination_name, "ncmcr" )) // TF Phani/Girija Y
         {
            /* This is intended for MasterCard, but it is not up.
             * If it is a debit, send it to Cirrus.
             */
            if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 )
            {
               strcpy( remote_host_que_name, cirrus_host_que_name );
               strcat( remote_host_que_name, "A" );
               if (txservice_Is_Host_Up (cirrus_host_que_name))
               {
                  strcpy( Auth_Tx.TLF01_details.authorizing_host_queue,
                          remote_host_que_name );

                  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                                   (remote_host_que_name, MT_AUTH_REQUEST, 0,0);
                  if (false == bReturnCode)
                     return SYSTEM_ERROR ;

                  txn_went_out = true;
               }
               else
               {
                  /* Do this so it follow the SAF path below. */
                  strcpy( destination_name, cirrus_host_que_name );
               }
            }
         }
		 else if ( 0 == strcmp(destination_name, "ncmcr2" )) 
		 {
			if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 )
            {
               strcpy( remote_host_que_name, cadencie_mc_que_name );
               strcat( remote_host_que_name, "A" );
               if (txservice_Is_Host_Up (cadencie_mc_que_name))
               {
                  strcpy( Auth_Tx.TLF01_details.authorizing_host_queue,
                          remote_host_que_name );

                  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                                   (remote_host_que_name, MT_AUTH_REQUEST, 0,0);
                  if (false == bReturnCode)
                     return SYSTEM_ERROR ;

                  txn_went_out = true;
               }
               else
               {
                  /* Do this so it follow the SAF path below. */
                  strcpy( destination_name, cadencie_mc_que_name );
               }
            }
		 }

         if ( txn_went_out == false )
         {
            /* If the host is "nccirs" and it is down, and if the transaction is
             * an AUTH_REVERSAL, set saf = 'Y' , send it back to the originator
             * with a "00" response code.
             */
            if  (strEqual (destination_name, "nccirs") &&
                (Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL))
            {
               strcpy (Auth_Tx.TLF01_details.saf, "Y") ;
               txservice_Send_Request_To_Service ( network_dataserver_que_name, 
                                                   MT_DB_REQUEST,ST1_DB_INSERT, 
                                                   0, SAF01_DATA, 
                                                   (pBYTE) &sSAF01, sizeof(SAF01));

               txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_HOST_NOT_AVAILABLE,
                                                        TRANSACTION_APPROVED) ;

               txservice_Forward_Auth_Tx_To_Originator
                                             (Auth_Tx.TLF01_details.handler_queue, 
                                              MT_AUTH_RESPONSE, 0, 0) ;
            }
            else
            {
                /*The following code added by Ravikumar K N, ThoughtFocus for rel 08-16 
                Cadience changes to support standin processing. If the profile is set to support standin processing
                just forward the request it to the network controller and the response from the network controller
                will be handled in transaction controller to perform standin processing */
                if(!(strEqual(Auth_Tx.BIN01_details.auth_ind,sREMOTE_PROCESSING_INDICATOR)) &&
                	(Auth_Tx.CAF01_details.primary_key.caf_profile_id != NULL) &&
                	(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0))
                {

                        strcpy (Auth_Tx.TLF01_details.authorizing_host_queue,
                 				remote_host_que_name) ;
                
                		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                                   (remote_host_que_name, MT_AUTH_REQUEST, 0,0);
                		if (false == bReturnCode)
                		{
                			return SYSTEM_ERROR ;
                		}
                }
                else
                {
        			if(((0==strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3",6)) &&
        					(0==strncmp(Auth_Tx.TLF01_details.message_type, "0100",4)) ||
        					(0==strncmp(Auth_Tx.TLF01_details.message_type, "0400",4) &&
        					AUTH_REVERSAL_ADVICE != Auth_Tx.TLF01_details.tx_key) )
        					&& true == txservice_Check_If_IST_Message ())
        			{
        				/* host is down */
        				sprintf (strMessage, "Remote host %s is down", destination_name) ;
        				strcpy( Auth_Tx.TLF01_details.response_text, " IST HOST IS OFFLINE" );

                   if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                           strcpy (Auth_Tx.TLF01_details.voice_auth_text, strMessage) ;
                
                   txcntl_log_message( 1, 2,strMessage,"txservice_Process_Bin_Range_Switch_Out", 1 );
        				txservice_Prepare_AuthTx_For_Forwarding
                                  (GS_TXN_HOST_NOT_AVAILABLE, BDO_VISA_DEBITHOST_UNAVAILABLE) ;

        				return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
        			}

        			else
        			{
        				/* host is down */
        				sprintf (strMessage, "Remote host %s is down", destination_name) ;

        				if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
        					strcpy (Auth_Tx.TLF01_details.voice_auth_text, strMessage) ;

        				txcntl_log_message( 1, 2,strMessage,"txservice_Process_Bin_Range_Switch_Out", 1 );
                   txservice_Prepare_AuthTx_For_Forwarding
                                 (GS_TXN_HOST_NOT_AVAILABLE, DESTINATION_UNAVAILABLE) ;
                
        				if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3",6) &&
        				    0 == strncmp(Auth_Tx.TLF01_details.message_type, "0120",4) &&
        				     AUTH_ADVICE == Auth_Tx.TLF01_details.tx_key &&
							 true == txservice_Check_If_IST_Message ())
						{
        					   if (strlen(Auth_Tx.CAF01_details.destination_name) > 0)
        						   strncpy(Auth_Tx.TLF01_details.acquirer_id, Auth_Tx.CAF01_details.destination_name, 6);
        					   else
        						   strncpy(Auth_Tx.TLF01_details.acquirer_id, Auth_Tx.TLF01_details.authorizing_host_queue, 6);

        	               	   strcpy( Auth_Tx.TLF01_details.response_text, " IST HOST IS OFFLINE" );

        	                   bReturnCode = txservice_Send_Request_To_Service (updateds_que_name,MT_DB_REQUEST,
																									ST1_DB_UPDATE,
																												0,
																									   TLF01_DATA,
																												0,
																												0) ;

						}
                   return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
                }
            }
         }
      }
      }
   } /* If context is BIN01_DATA */
   else if (context == BIN02_DATA)
   {
      if (txservice_Is_Host_Up (destination_name))
      {
         strcpy (Auth_Tx.TLF01_details.authorizing_host_queue,
                 remote_host_que_name) ;

         bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                               (remote_host_que_name, MT_AUTH_REQUEST, 0, 0) ;
         if (false == bReturnCode)
         {
            /* XIPC error sending to remote host and alternate host is down */
            sprintf (strMessage, "XIPC err sending to remote host %s", 
                     remote_host_que_name) ;
            if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
               strcpy (Auth_Tx.TLF01_details.voice_auth_text, strMessage);

            txcntl_log_message( 1, 2,strMessage,"txservice_Process_Bin_Range_Switch_Out", 1 );
            txservice_Prepare_AuthTx_For_Forwarding
                                      (GS_TXN_SYSTEM_ERROR,SYSTEM_MALFUNCTION) ;
            return SYSTEM_ERROR ;
         }
      }
      else
      {
		 /*The following code added by Ravikumar K N, ThoughtFocus for rel 08-16 
		Cadience changes to support standin processing. If the profile is set to support standin processing
		just forward the request it to the network controller and the response from the network controller
		will be handled in transaction controller to perform standin processing */
		if(!(strEqual(Auth_Tx.BIN01_details.auth_ind,sREMOTE_PROCESSING_INDICATOR)) &&
			(Auth_Tx.CAF01_details.primary_key.caf_profile_id != NULL) &&
			(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0))
		{
		        strcpy (Auth_Tx.TLF01_details.authorizing_host_queue,
		 				remote_host_que_name) ;

					bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
		                   (remote_host_que_name, MT_AUTH_REQUEST, 0,0);
					if (false == bReturnCode)
				return SYSTEM_ERROR ;
		}
		else
		{
         /* host is down */
         sprintf (strMessage, "Remote host %s is down", destination_name) ;

         if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strMessage) ;

         txcntl_log_message( 1, 2,strMessage,"txservice_Process_Bin_Range_Switch_Out", 1 );
         txservice_Prepare_AuthTx_For_Forwarding
                          (GS_TXN_HOST_NOT_AVAILABLE, DESTINATION_UNAVAILABLE) ;

         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}

      }
   } /* else if context is BIN02_DATA */

   return NO_AUTH_ERROR ;

} /* txservice_Process_Bin_Range_Switch_Out */

BYTE txservice_Start_Local_Processing (void)
{
	BYTE    ReturnCode;
	BYTE    TransactionOrigin = 0 ;
   double  total_amount = 0.0;

    /* Clear the response code field */
   if ( (true == txservice_If_Tx_Is_Treated_Like_A_Advice ()) &&
	    (true != txservice_Check_If_DCI_issuing_transaction()))
	 {
	   memset (Auth_Tx.TLF01_details.response_code,0, sizeof Auth_Tx.TLF01_details.response_code) ;
	 }

   /* This is for Equitable only.  For JCB and AMEX cards (ON-US), if the
    * currency is in US Dollars, it must be converted to Phillippino Peso
    * prior to processing it.  It will be coverted during the authorization
    * phase, so for now, set currency code to PESO so the right account is
    * hit. Set host_start_time (this field is not used for local processing)
    * to 0.000 to flag the authorizer that it still needs to be converted.
    */
   if ( (strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)) &&
		(!strEqual(Auth_Tx.TLF01_details.handler_queue, "dcpisoA"))&&
       ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)) ||
        (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX))) )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
      strcpy( Auth_Tx.TLF01_details.host_start_time, "0.000" );
   }

   /* For Amex cards, create a random numeric auth id
    * if one does not already exist.  Auth Id must be
    * maintained for reversals and Voice overrides.
    */
   if ( strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) &&
		(true == txservice_If_Tx_Is_Treated_Like_A_Reversal () ||
		 true == txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override ()))
   {
      /* Does Auth Id exist already? */

      if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
      {
         /* No. Generate a numeric authorization id */
         (void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
      }
	}
   ReturnCode = TxDataServer_Continue_With_Transaction_Processing () ;

	return( ReturnCode );
} /* txservice_Start_Local_Processing */
	


/*************************************************************************************
NAME:			txservice_Forward_Auth_Tx_To_Updater
DESCRIPTION:	Sends the message to the transaction data server (the background updater)
INPUTS:			CHAR* que_name:		the destination que name
				BYTE message_type:	the pte message type
				BYTE message_subtype_1:	the pte message subtype
				BYTE AppDataType :		the pte message appdatatype
OUTPUTS:		None
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN txservice_Forward_Auth_Tx_To_Updater (CHAR* que_name, 
                                              BYTE message_type, 
											  BYTE message_subtype_1, 
											  BYTE AppDataType) 
{
	BOOLEAN bReturnCode = true ;
	txservice_Replace_Tx_Key_In_Auth_Tx () ;
	bReturnCode = txservice_Send_Request_To_Service (que_name,message_type, 
		                                              message_subtype_1, 0, 
													AppDataType, 0, 0) ;
	return bReturnCode ;
} /* txservice_Forward_Auth_Tx_To_Updater */

/************************************************************************************* 
DESCRIPTION:	Sends the message to the remote destination (host handler)	
INPUTS:			CHAR* que_name:		the destination que name		
				BYTE message_type:	the pte message type
				BYTE message_subtype_1:	the pte message subtype
				BYTE AppDataType :		the pte message appdatatype
OUTPUTS:		None
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN txservice_Forward_Auth_Tx_To_Remote_Host (CHAR* que_name,
                                                  BYTE  message_type, 
                                                  BYTE  message_subtype_1, 
                                                  BYTE  AppDataType) 
{
   BOOLEAN bReturnCode = true ;
   INT     len		   = 0;

   /* RPS PROJECT (RT 1454442_MCC ATP STIP Processing for RPS and COF Transactions - BR_DRAFT_ - v1.0.pdf)
    * 2018- Raghu
    * */
	if(Auth_Tx.CAF01_details.authorization_location[0] == 'S')
	{
		MCC_RPS_flag = txservice_Check_MCC_availablity();
		/* RPS STIP condition for Acquiring transactions*/
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6) == 0)
		{
			if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0 &&
			  strncmp(Auth_Tx.TLF01_details.message_type,"0200",4)      == 0 )
			{
				bReturnCode = txservice_Check_RPS_Flag_For_ACQ_Txn();
				if(true == bReturnCode)
				{
					if(Auth_Tx.TLF01_details.product_codes[6].quantity[0] == 0x00)
					{
						Auth_Tx.TLF01_details.tx_key += 100;
						Auth_Tx.TLF01_details.product_codes[6].quantity[0] = 'R';
						Auth_Tx.TLF01_details.product_codes[6].quantity[1] = '\0';
						bReturnCode = txservice_perform_standin_processing();
						if (bReturnCode == NO_AUTH_ERROR)
						{
							txcntl_log_message( 1, 1,"Txn qualified for RPS STIP", "txservice_Forward_Auth_Tx_To_Remote_Host", 1 );
							return true;
						}
					}
				}
			}
		}/* RPS STIP condition for Incoming transaction */
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"005",3) != 0 &&
		        strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"002",3) != 0)
		{
			if(strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0)
			{
			bReturnCode = txservice_Check_If_Transaction_Qualifies_For_RPS_STIP();
			if(bReturnCode == true)
			{
				if(Auth_Tx.TLF01_details.product_codes[6].quantity[0] == 0x00)
				{
					Auth_Tx.TLF01_details.tx_key += 100;
					Auth_Tx.TLF01_details.product_codes[6].quantity[0] = 'R';
					Auth_Tx.TLF01_details.product_codes[6].quantity[1] = '\0';
					/* Display acquirer_id in Tranlookup */
					strncpy(Auth_Tx.TLF01_details.acquirer_id,Auth_Tx.CAF01_details.destination_name, 6);
					txcntl_log_message( 2, 1,"Txn qualified for RPS STIP", "txservice_Forward_Auth_Tx_To_Remote_Host", 1 );
					bReturnCode = txservice_perform_standin_processing();
					if (bReturnCode == NO_AUTH_ERROR)
					{
						return true;
					}
				}
			}
		}
	}
	}
   /* Implement Visa SMS routing; if anything is going to Visa,
    * send it to Visa SMS if a pin exists.  SMS is the ATM host
    * for Visa.
    */
   if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 )
   {
      /* A pin does exist.  Is the destination ncvisa? */
      //len = strlen( plus_host_que_name );
      len = strlen( que_name );// nirbhay: Lenth was not correct strncmp was giving result 0 so was going to ncvsms

      /* Ignore the 'A' at the end of 'que_name' */
      if ( 0 == strncmp(que_name, plus_host_que_name, len-1))
      {
         /* Yes, change it to Visa SMS. */
         strcpy( que_name, vsms_host_que_name );
         strcat( que_name, "A" );
      }
   }

   /*
	All Cup cards should be send to cup host of BDO host even bin range is configure to other host.
   */

   
   if ((0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))&&
   	   (0 != strncmp(que_name,"nccup",5)))
   	{
		if(0 != strncmp(Auth_Tx.CAF01_details.authorization_location,"S",1))
		{
			strncpy(que_name, "nccupA",6);
			que_name[6]=0X00;
		}
   	}


   bReturnCode = txservice_Send_Request_To_Service (que_name, message_type, 
                                                    message_subtype_1, 
                                                    0, AppDataType, 0, 0) ;
	return bReturnCode ;
} /* txservice_Forward_Auth_Tx_To_Remote_Host */

/*************************************************************************************
NAME:			txservice_Forward_Auth_Tx_To_Originator
DESCRIPTION:	Sends the message to the requester (usually the terminal handler)
INPUTS:			CHAR* que_name:		the destination que name		
				BYTE message_type:	the pte message type
				BYTE message_subtype_1:	the pte message subtype
				BYTE AppDataType :		the pte message appdatatype
OUTPUTS:		None
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN txservice_Forward_Auth_Tx_To_Originator (CHAR* que_name, BYTE message_type, 
																BYTE message_subtype_1, 
																BYTE AppDataType)
{
	BOOLEAN bReturnCode = true ;
	BYTE Temp_Que_Name[256] = {0} ;
	CHAR time_date[50]={0};
	CHAR msgBuffer[1023]={0};
	INT retVal			= 0;

	txservice_Replace_Tx_Key_In_Auth_Tx () ;

	/* For voice authorized transactions, send to MRACON */
	if (strEqual (que_name, "VOICE")) 
	{
		strcpy (Temp_Que_Name, applnk_que_name) ;
		AppDataType = AUTH_TX_DATA ;
		txservice_Check_and_update_rc_if_Voicetxn_is_TO();
		bReturnCode = txservice_Send_Request_To_Service (Temp_Que_Name,message_type,
														 message_subtype_1,
				                                         0,AppDataType, 0, 0) ;
		if(nMra_enable_flag == 1)
		{
			/* Send to MRA if tx is VOICE*/

			if(Nmra_send_flag == 1)
			{
				//if(strcmp(Auth_Tx.TLF01_details.batch_number,"ACTIVE") == 0)
				//strcpy(Auth_Tx.TLF01_details.batch_number,"");
				bReturnCode = txservice_Send_Request_To_Service (	mrads_que_name,
											  MT_DB_REQUEST,
											  ST1_DB_INSERT,
											  ST2_NONE,
											  MRA_AUTH_TBL_DATA,
											  (pBYTE) &Auth_Tx.TLF01_details,
											  sizeof Auth_Tx.TLF01_details) ;
			}
			else if(Nmra_send_flag == 0)
			{
				txcntl_log_message( 2, 1,"Send trx to MRACON", "txservice_Forward_Auth_Tx_To_Originator", 1 );
				txservice_Send_Request_To_Mracon_Service();
			}
		}
	}
	else
	{
		if( true == txservice_Check_If_AMEX_Transaction() &&
		   (Auth_Tx.EMV_details.emv_block[0] != 0x00 	  ||
		    Auth_Tx.EMV_details.emv_block[1] != 0x00 ))
		{
				if( 0 != strncmp(Auth_Tx.TLF01_details.response_code,"00",2))
				{
					if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))
					{
						Auth_Tx.EMV_details.future_field1[13]= 0x35;
					}
					else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5))
					{
						Auth_Tx.EMV_details.future_field1[10]= 0x35;
					}
				}
		}

		//product_codes[13].quantity, we are checking in updatds for merch_integrity_param
		//response codes handling
		if( true == txservice_Check_If_VISA_Transaction() &&
			0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6) &&
			0 != strncmp(Auth_Tx.TLF01_details.response_code,"00",2))
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[13].quantity,Auth_Tx.TLF01_details.response_code,2);
		}

		/* Leela - AprilMandated 2025 AMEX-1,With Action code 115 mapped with RC-12
		 * To display "Invalid transaction" in Response text field in Transaction details*/
		if( (true==txservice_Check_If_AMEX_Transaction()) &&
			(0==strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6)) &&
			(0==strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncposa",6)) &&
			(0==strcmp(Auth_Tx.TLF01_details.product_codes[12].code,"115")))
		{
			strcpy(Auth_Tx.TLF01_details.response_text,"Invalid Transaction");
		}
		bReturnCode = txservice_Send_Request_To_Service (que_name,message_type,
														 message_subtype_1,
				                                         0,AppDataType, 0, 0);
	
	}

#ifdef HCTS2
	//pre condition check

	if(txservice_verify_onus_online_transaction() == true)
	{
		TxDataserver_update_atp_card_status_based_on_vertexon_rc();
	}
#endif

	if(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0 && nFalconSendFlag == 1)//IF tx is BDO card sent it to FAI...TF Phani
	{
		/* Falcon requirement to support sending EXT10 data to falcon
		 * for declined responses from Vertexon/STIP */
		if (true == Falcon_RT_Flag  &&
			true == txservice_If_Tx_Qualifies_For_RealTime() &&
			false == txservice_Check_If_IST_Message())
		{
			if (true == Falcon_RT_Ext10_Flag  &&
				false == txservice_check_auth_rc_for_falcon_ext10())
			{
				txservice_Send_EXT10_Request_To_RT_Falcon_Service();
			}

		}
		else
		{
			txservice_Send_Request_To_Falcon_Service();
		}

	}

	return bReturnCode ;
	
} /* txservice_Forward_Auth_Tx_To_Originator */

BOOLEAN txservice_verify_onus_online_transaction()
{
	if(
		(strncmp(Auth_Tx.CAF01_details.authorization_location, "S",1) == 0) &&
		(strcmp(Auth_Tx.TLF01_details.product_codes[1].code, STANDIN) != 0)
	  )
	{
		return true;
	}

	return false;
}

BOOLEAN txservice_Forward_Auth_Tx_To_Originator_for_RT_decline (CHAR* que_name, BYTE message_type,
																BYTE message_subtype_1,
																BYTE AppDataType)
{
	BOOLEAN bReturnCode = true ;
	BYTE Temp_Que_Name[256] = {0} ;
	CHAR time_date[50]={0};
	CHAR msgBuffer[1023]={0};
	INT retVal			= 0;

	txservice_Replace_Tx_Key_In_Auth_Tx () ;

	/* For voice authorized transactions, send to MRACON */
	if (strEqual (que_name, "VOICE"))
	{
		strcpy (Temp_Que_Name, applnk_que_name) ;
		AppDataType = AUTH_TX_DATA ;
		bReturnCode = txservice_Send_Request_To_Service (Temp_Que_Name,message_type,
														 message_subtype_1,
				                                         0,AppDataType, 0, 0) ;
		if(nMra_enable_flag == 1)
		{
			/* Send to MRA if tx is VOICE*/

			if(Nmra_send_flag == 1)
			{
				//if(strcmp(Auth_Tx.TLF01_details.batch_number,"ACTIVE") == 0)
				//strcpy(Auth_Tx.TLF01_details.batch_number,"");
				bReturnCode = txservice_Send_Request_To_Service (	mrads_que_name,
											  MT_DB_REQUEST,
											  ST1_DB_INSERT,
											  ST2_NONE,
											  MRA_AUTH_TBL_DATA,
											  (pBYTE) &Auth_Tx.TLF01_details,
											  sizeof Auth_Tx.TLF01_details) ;
			}
			else if(Nmra_send_flag == 0)
			{
				txcntl_log_message( 2, 1,"Send trx to MRACON", "txservice_Forward_Auth_Tx_To_Originator", 1 );
				txservice_Send_Request_To_Mracon_Service();
			}
		}
	}
	else
	{
		if( true == txservice_Check_If_AMEX_Transaction() &&
		   (Auth_Tx.EMV_details.emv_block[0] != 0x00 	  ||
		    Auth_Tx.EMV_details.emv_block[1] != 0x00 ))
		{
				if( 0 != strncmp(Auth_Tx.TLF01_details.response_code,"00",2))
				{
					if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))
					{
						Auth_Tx.EMV_details.future_field1[13]= 0x35;
					}
					else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5))
					{
						Auth_Tx.EMV_details.future_field1[10]= 0x35;
					}
				}
		}
		bReturnCode = txservice_Send_Request_To_Service (que_name,message_type,
														 message_subtype_1,
				                                         0,AppDataType, 0, 0);
	}

	/*if(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0 && nFalconSendFlag == 1)//IF tx is BDO card sent it to FAI...TF Phani
	{
		txservice_Send_Request_To_Falcon_Service();
	}*/
	return bReturnCode ;

} /* txservice_Forward_Auth_Tx_To_Originator_for_RT_decline */


/******************************************************************************
 *
 *  NAME:         get_MRACON_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether falcon is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Phani  - ThoughtFocus
 *
 ******************************************************************************/
void get_MRACON_state_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nARState_temp   = 0;
   INT nARState_retval = 1;
   INT Nmra_falg = 0;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
             "MRA_FAI_SEND_FLAG",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Nmra_send_flag = atoi(tmpstring);
    /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
             "SEND_TO_MRADB",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nMra_enable_flag = atoi(tmpstring);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

     GetPrivateProfileString(
               "MRA_SECTION",                   /* points to section name       */
               "MRACON_LOG_ENABLE_FLAG",       /* points to key name           */
               "0",                            /* points to default string     */
               ini_info,                       /* points to destination buffer */
               MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
               filename                        /* points to ini filename       */
               );

     /* Convert entry from a string into a number. */
     memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
     strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

     Mracon_Log_Enable_Flag = atoi(tmpstring);



   //return(Nmra_send_flag);
}
/******************************************************************************/

BOOLEAN txservice_Send_Request_To_Mracon_Service ( void )
{
   BOOLEAN  retval = true;
   INT      nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   memset( p_buffer, 0x00,     sizeof(p_buffer) );
   memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );

   txcntl_log_message( 2, 1,"Inside txservice_Send_Request_To_Mracon_Service","txservice_Send_Request_To_Mracon_Service", 1 );
   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
                                 mracon_que_name,
                                 txcntl_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize,
                                 0 );
   if (p_msg_out == NULL)
   {
	   if(Mracon_Log_Enable_Flag == 1)
	   {
		   sprintf( strTemp,"Mem allocate Err build msg for %s", mracon_que_name );
		   txcntl_log_message( 1, 3,strTemp,"txservice_Send_Request_To_Mracon_Service", 1 );
	   }
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, mracon_que_name) ;
      if (lReturnCode != 0)
      {
    	  if(Mracon_Log_Enable_Flag == 1)
    	  {
			 sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,mracon_que_name);
			 txcntl_log_message( 1, 3,strTemp,"txservice_Send_Request_To_Mracon_Service", 1 );
    	  }
      }
      else
      {
    	  txcntl_log_message( 2, 1,"Trx posted to MRACON", "txservice_Send_Request_To_Mracon_Service", 1 );
      }
   	  free( p_msg_out );
   }
   return( retval );
} /* txservice_Send_Request_To_Mracon_Service */


/*************************************************************************************
NAME:			txservice_Prepare_AuthTx_For_Forwarding
DESCRIPTION:	Inserts parameters in TFL01	
INPUTS:			BYTE general_status:   
				BYTE* responseCode:		ISO response code 
OUTPUTS:		None
RETURNS:        void
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void txservice_Prepare_AuthTx_For_Forwarding (BYTE general_status, BYTE* strResponseCode) 
{   
	CHAR amex_response_code[4] = {0};
	CHAR dci_response_code [4] = {0};
//	strncpy (Auth_Tx.TLF01_details.response_code, strResponseCode, sizeof (Auth_Tx.TLF01_details.response_code, strResponseCode) -1) ; 
	strncpy (Auth_Tx.TLF01_details.response_code, strResponseCode, RESPONSE_CODE_SIZE) ; 

	if(true == txservice_Check_If_CUP_Transaction())
	{
		// We need to change the key 
		if((0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))&&
			(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
			(0 == strncmp(Auth_Tx.BIN01_details.card_type,"CUP",3))&&
			(Auth_Tx.TLF01_details.tx_key== AUTH_REVERSAL))
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE;
			}
	}

	if(true == txservice_Check_If_MC_Transaction())
	{
		// We need to change the key for MC purchase reversal
		if((0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))&&
			(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
			(0 == strncmp(Auth_Tx.BIN01_details.card_type,"MC",3))&&
			(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
			 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)) &&
			(Auth_Tx.TLF01_details.tx_key== AUTH_REVERSAL))
			{/* will include all ncmcrd and ncmcr3*/
				Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE;
			}
	}

	Auth_Tx.TLF01_details.general_status = general_status ;
	/* TF PHANI/Ajay Fill Repsonse text with proper message*/
	if(strcmp(strResponseCode,"00") == 0 && strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[6].quantity[0] == 'R')
		{
			strcpy(Auth_Tx.TLF01_details.response_text,"ATP STIP Processed RPS");
		}
		else if(0 != strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA"))
		{
			if(0 == strncmp(Auth_Tx.TLF01_details.response_text,"DE48 SE84:",10) &&
			  (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) ||
			   0 == strncmp(Auth_Tx.TLF01_details.message_type,"0130",4)) &&
			  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrdA",7) || 0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3A",7)))
			{
				/* dont copy anything for this case*/
			}
			else
			{
				strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
			}

		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.response_text,"");
		}
	}

	// Only for STIP transaction we shall copy the Action code here
	if(true  == txservice_Check_If_AMEX_Transaction())
	{
		 if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))
		 {
			 if(0 == strlen(Auth_Tx.TLF01_details.product_codes[12].code))
			 {
			 match_ISO_to_AMEX_response_code(amex_response_code);
			memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
					amex_response_code,
					sizeof(amex_response_code));
			 }
		 }
	}
	/* Copy action code to support for EXT10 request */
	if(Falcon_RT_Ext10_Flag == 1 &&
		true  == txservice_Check_If_Diners_Transaction())
	{
		if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))
		{
			if(0 == strlen(Auth_Tx.TLF01_details.product_codes[12].code))
			{
			 match_ISO_to_Diners_response_code(dci_response_code);
			 memcpy( Auth_Tx.TLF01_details.product_codes[12].code,
					 dci_response_code,
					sizeof(dci_response_code));
			}
		}
	}
    
} /* txservice_Prepare_AuthTx_For_Terminal_Handler_Response */

/******************************************************************************
NAME:        txservice_Generate_Numeric_Auth_Number
DESCRIPTION: Gets a numeric auth number from the database.
INPUTS:      None
OUTPUTS:     strAuthNumber - Authorization number
RETURNS:     void
AUTHOR:      Sanjoy Dasgupta
*******************************************************************************/
void txservice_Generate_Numeric_Auth_Number (BYTE* strAuthNumber) 
{
   INT   retval;
   CHAR  errbuf[100] = "";
   CHAR  Buffer[200];
   UAG01 uag01;

   memset( &uag01, 0x00, sizeof(UAG01) );
   retval = TxDataServer_Get_Uag01_With_Wait( &uag01,
                                              ST2_DB_SELECT_NUMERIC_AUTH_ID,
                                              errbuf );
   if ( false == retval )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer, "Err with auth id: %s", errbuf );

      txcntl_log_message( 1, 2,Buffer,"txservice_Generate_Numeric_Auth_Number", 1 );
   }
   else
   {
      /* We successfully got a numeric auth number from UAG01. */
      strcpy( strAuthNumber, uag01.auth_id );
   }
   return;
} /* txservice_Generate_Numeric_Auth_Number */



/*************************************************************************************
NAME:			txservice_Create_Date_Time_RRN_For_Auth_Tx
DESCRIPTION:	Generates a date, time, and retrieval reference number based on the 
				system date and time	
INPUTS:			None
OUTPUTS:		BYTE* pstrDate_yyyymmdd:  The system date in the format yyyymmdd
				BYTE* pstrTime_hhmmss:    The system time in the format hhmmss
				BYTE* pstrRRN:			  The retrieval reference number
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void txservice_Create_Date_Time_RRN_For_Auth_Tx (BYTE* pstrDate_yyyymmdd, 
																 BYTE* pstrTime_hhmmss, 
																 BYTE* pstrRRN)
{
	BYTE strRRN[50] = {0} ;
    BYTE strTest[50] = {0} ;
    char local_time[15] = {0} ;
    BYTE strSystemDateTime[50] = {0} ;
   
	genutil_format_date (strSystemDateTime) ;
	
	memcpy (pstrDate_yyyymmdd, strSystemDateTime, 8) ; 
	pstrDate_yyyymmdd[8] = 0 ;
	memcpy (pstrTime_hhmmss, strSystemDateTime + 8, 6) ;
	pstrTime_hhmmss[6] = 0 ;
	
    create_rrn (pstrRRN) ;
} /* txservice_Create_Date_Time_RRN_For_Auth_Tx */


BYTE txservice_Is_Host_Up (BYTE* destination_name)
{
   XINT  ReturnCode ;
   CHAR  strTemp [256] = {0} ;
   BYTE  tmouts[100] = {0}, txns[100] = {0}, state[100] = {0} ;
   BYTE  ret_val = false;int i;

   if (destination_name[0] != 0)
   {
      for( i = 0; destination_name[i] != '\0'; i++);   
	  destination_name[--i];	
	  if(destination_name[i] == 'A')
	  {
		  destination_name[i] = '\0';
	  }
	  strcpy (strTemp, destination_name) ;
      strcat (strTemp, "Table") ;

      ReturnCode = ReadGenericTable (strTemp, tmouts, txns, state) ;
      if (ReturnCode == MEMACCESSERROR)
      {
         memset (strTemp, 0, sizeof strTemp) ;
         sprintf (strTemp, 
                 "Err accessing shared memory (%s)\n",
                  destination_name) ;

         if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
         txcntl_log_message( 1, 3,strTemp,"txservice_Is_Host_Up", 1 );
      }
      else
      {
         /* Forward request to network controller
          * if the network controller is Online.
          */

		  if ( strEqual(state, ONLINE) )
            ret_val = true;
      }
   }
   return( ret_val );

} /* txservice_Is_Host_Up */


void txservice_Get_Bin_Type_From_Processing_Code (BYTE* pnAccountType)
{
	/* third digit of the processing code determines the bin type */
	if (Auth_Tx.TLF01_details.processing_code[2] == '0' || 
		Auth_Tx.TLF01_details.processing_code[2] == '3' ||
      Auth_Tx.TLF01_details.processing_code[2] == '4')
		*pnAccountType = 'C' ;
	else 
		*pnAccountType = 'D' ;
	if(pnAccountType == 'D')
	{
		// if tx coming from VISA and no PIN block , it might be credit
		if( strlen(Auth_Tx.TLF01_details.pin_block) > 0 )
		{//dont change Account type
		}
		else if( strncmp (Auth_Tx.TLF01_details.handler_queue, "ncvis",5) == 0)
			*pnAccountType = 'C' ; // it could be swipe tx
			
	}

} /* Get_Bin_Type_From_Processing_Code */

BYTE txservice_If_Tx_Is_Treated_Like_A_Deferred()
{
   BYTE  ret_val;

   switch( Auth_Tx.TLF01_details.tx_key )
   {
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
         ret_val = true;
         break;

      case AUTH_REVERSAL:
      case AUTH_REVERSAL_RESPONSE:

      case AUTH_VOID_SALE:
      case AUTH_VOID_SALE_RESPONSE:
         if ( Auth_Tx.TLF01_details.deferred_term_length[0] == 0x00 )
            ret_val = false;
         else
            ret_val = true;
         break;
      default:
         ret_val = false;
         break;
   }
   return( ret_val );
}


/*************************************************************************************
NAME:			txservice_Send_Request_To_Service
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BOOLEAN txservice_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
										   BYTE nSubType2, BYTE nAppDataType, 
										   pBYTE p_data,   INT nDataSize)
{
	LONG lReturnCode = 0L ;
    BYTE ReturnCode = 0 ;
	pPTE_MSG p_msg_out = 0 ;
	INT nBufferSize = 0 ;
	char strTemp[256] = {0} ;
    CHAR logger_que_name[256] = {0} ;
	CHAR orig_comms_info [MAX_ORIG_INFO_SIZE] = {0} ; 


	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof Auth_Tx) ;
	
	GetLoggerQueName (logger_que_name) ;

	if (p_data != 0)
		memcpy (p_buffer + sizeof Auth_Tx, p_data, nDataSize) ;
		
	nBufferSize = sizeof Auth_Tx + nDataSize ;

	p_msg_out = ptemsg_build_msg (  nType, 
								    nSubType1, 
								    nSubType2, 
								    que_name, 
								    txcntl_que_name, 
								    (pBYTE) p_buffer, 
								    nBufferSize, 
								    nAppDataType) ;

    if (p_msg_out == NULL)
    {
        if(strcmp(que_name,mrads_que_name) == 0)			
			return true;	
			sprintf (strTemp,"Mem allocate Err build msg for %s", que_name) ;
                
		ReturnCode = txservice_Generate_Usage_Error_Message 
													(1,strTemp, "txservice_Send_Request_To_Service",
													GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
		return false ;
    }

	if (strEqual (que_name, applnk_que_name))
	{
		/* This is to ensure that for a voice authorization, the tx is sent to applink 
		   when the transaction is terminated */
		
		ptemsg_set_orig_comms_info (p_msg_out, Auth_Tx.TLF01_details.originator_info) ;
		sprintf (strTemp, "Sending message to %s, originator_info is %s, New Auth Number is %s\n", 
			     que_name, 
			     ptemsg_get_msg_orig_comms_info (p_msg_out),
                 Auth_Tx.TLF01_details.auth_number) ;
        
	}
	else 
		sprintf (strTemp, "Sending message to %s\n", que_name) ;
	    lReturnCode = pteipc_send (p_msg_out, que_name) ;

	if (lReturnCode != 0)
	{
		sprintf (strTemp,"Err %ld sending msg to %s", lReturnCode, que_name) ;
        if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

        free (p_msg_out) ;
		
        if(strcmp(que_name,mrads_que_name) == 0)			
		{	memset (Auth_Tx.TLF01_details.voice_auth_text, 0x00,sizeof(Auth_Tx.TLF01_details.voice_auth_text)) ;
			return true;	
        }
        ReturnCode = txservice_Generate_Usage_Error_Message 
													(1,strTemp, "txservice_Send_Request_To_Service",
													GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
		return false ;
		
	}

											
	free (p_msg_out) ;
	return true ;
} /* txservice_Send_Request_To_Service */

/*************************************************************************************
NAME:			txservice_Get_Original_Transaction_Type
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void txservice_Get_Original_Transaction_Type (  BYTE* strMessageType, 
											    BYTE* strProcessingCode, 
												BYTE* pstrTxKey) 
{
	BYTE strTxType[7] = {0} ;
	
	memcpy (strTxType, strMessageType, 4) ;
	memcpy (strTxType+4, strProcessingCode, 2) ;

	strcpy(pstrTxKey, strTxType) ;
} /* txservice_Get_Original_Transaction_Type */

BYTE txservice_GetTransactionOrigin (void)
{
   BYTE  retval;

   if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
      retval = REMOTE_ORIGIN;
   else
      retval = LOCAL_ORIGIN;

   return( retval );
} /* txservice_GetTransactionOrigin (void) */

BYTE txservice_Get_Card_SubType (void) 
{
	return (Auth_Tx.BIN01_details.credit_subtype[0]) ;
} /* txservice_Get_Card_SubType */

BYTE txservice_Verify_Currency_Code (void)
{
/*   #define  TURKISH_LIRA_LIMIT  2500*/

//   LONG  tx_amt;
   BYTE  retval       = NO_AUTH_ERROR;
   CHAR  strTemp[256] = {0} ;

   /*
		For CUP we will not do any Currency validation if it is coming from CUP and ATP is not doing stip 
    */
    if ( (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))||
    	 (true == txservice_Check_If_DCI_issuing_transaction()))
	{
		   return( retval );
    }
    else if ( (0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))&&
		  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
		  (0 == strncmp(Auth_Tx.ch_billing_curr_code,PESO_CURRENCY,3)||
		  (0 == strncmp(Auth_Tx.ch_billing_curr_code,USDOLLAR_CURRENCY,3))))
	{
			   return( retval );
    }
    else if((0 != strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)) &&
    		((true == txservice_Check_If_AMEX_issuing_transaction()) ||
			 (true == txservice_Check_If_JCB_issuing_transaction()))
		   )
    {
    	return( retval );
    }
    else if (0 != strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN) &&
			 (true == txservice_Check_If_VISA_Transaction()) &&
			 (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nceqit", 6) ||
			  0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "neqit2", 6)))
    {
    	/* April Mandt 2025: OTH1 requirement */
    	return( retval );
    }
    /*else if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)) ||
    		(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)) &&
			(0 == strncmp(Auth_Tx.TLF01_details.currency_code,SIERRA_LEONE_CURRENCY,3)))
    {

    	 return( retval );

    }*/
#if VISA2
    /*else if (true == txservice_Check_If_VISA_issuing_transaction() &&
    		 true == txservice_Check_If_MC_issuing_transaction() &&
    		strEqual (Auth_Tx.TLF01_details.nfi_seq_nbr, ICE_KRONA_CURRENCY))
    {
    			return( retval );
    }*/
#endif
   else if (	!strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) && 
		!strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)
		//&& !strEqual(Auth_Tx.TLF01_details.originator_queue, plus_host_que_name) && 		// nirbhay ThoughtFocus: Cadencie link (BDO range) changes
       )	// nirbhay ThoughtFocus: Cadencie link (BDO range) changes : Allow third currency to Cadencie if billing currency is PESO or USD
   {
      
      sprintf( strTemp,
              "Invalid Currency code %s in Tx\n", 
               Auth_Tx.TLF01_details.currency_code );

      if ( strEqual(Auth_Tx.TLF01_details.handler_queue, "VOICE") )
         strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp);

      txcntl_log_message( 1, 3,strTemp,"txservice_Verify_Currency_Code", 1 );
      txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_INVALID_CURRENCY_CODE,
                                               REFER_TO_CARD_ISSUER);

      retval = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;

   }

   /* Implement a temporary 'quick' fix for SCR 13929.
    * For currency codes of 792 or 949 (Turkish Lira), decline amounts
    * over 25 USD.  NC's put original currency code in nfi_seq_nbr.
    * Do this only for non-pin-based transactions.
    */

   /* 4/8/05 Removing this temporary section upon implementation of Country Amount Limits Table SCR13929*//*
   else
   {
      if ( Auth_Tx.TLF01_details.pin_block[0] == 0x00 )
      {
         // There is no pin, so check the currency code. 
         if ( (0 == strcmp("792",Auth_Tx.TLF01_details.nfi_seq_nbr)) ||
              (0 == strcmp("949",Auth_Tx.TLF01_details.nfi_seq_nbr))  )
         {
            // It is in Turkish Lira. Check the amount.  USD amount might
             * be in settlement_total (MC and Amex) or ch_billing_amt (Visa).
             * JCB accepts only Peso and USD currencies.
             //
            if ( 0x00 != Auth_Tx.TLF01_details.settlement_total[0] )
            {
               // Use settlement_total from MC or Amex. 
               tx_amt = atol( Auth_Tx.TLF01_details.settlement_total );
            }
            else
            {
               // Use Card Holder Billing amount from Visa. 
               tx_amt = atol( Auth_Tx.ch_billing_amt );
            }

            if ( tx_amt > TURKISH_LIRA_LIMIT )
            {
               // Amount is over 25 USD. Decline it. 
               sprintf( strTemp,
                       "Currency code %s, amount %0.2f exceeds %0.2f limit",
                        Auth_Tx.TLF01_details.nfi_seq_nbr,
                        tx_amt / 100.0,
                        TURKISH_LIRA_LIMIT/100.0 );

               if ( strEqual(Auth_Tx.TLF01_details.handler_queue, "VOICE") )
                  strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp);


               txservice_Prepare_AuthTx_For_Forwarding(
                                            GS_TXN_INVALID_TRANSACTION_AMOUNT,
                                            REFER_TO_CARD_ISSUER
                                                );

               retval = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
            }
         }
      }
   }*/
   return( retval );

} /* txservice_Verify_Currency_Code */

BYTE txservice_Perform_PTE_Message_Checks (pPTE_MSG p_pte_msg)
{
    char strTemp[256] = {0} ;

    if (p_pte_msg->msg_version != PTEMSG_MSG_VERSION)
        strcpy (strTemp, "Invalid PTE Message Version") ;

    if (p_pte_msg->num_chained_data != 1)
        strcpy (strTemp, "Invalid PTE Msg field<num_changed_data>") ;

    if (p_pte_msg->num_chained_messages != 1)
        strcpy (strTemp, "Invalid PTE Msg field<num_changed_msg>") ;

    if (strTemp[0] != 0)
    {
        if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
        txcntl_log_message( 1, 3,strTemp,"txservice_Perform_Auth_Tx_Checks", 1 );
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
												REFER_TO_CARD_ISSUER) ;
		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
    }

    return NO_AUTH_ERROR ;
}


BYTE txservice_Perform_Auth_Tx_Checks (void)
{
   #define  DATE_LEN  9
	BYTE ReturnCode = NO_AUTH_ERROR ;
	char strTemp[256] = {0} ;
	int TransactionOrigin = 0 ;
   CHAR start_date[DATE_LEN];
   CHAR end_date[DATE_LEN];
   CHAR txn_date[DATE_LEN];
   CHAR time_date[25];

	//  : commented as 302 msg is routed directly from ncvis2 to ncvisa.
	// -------------------------------------------------------
	//if(!(strncmp(Auth_Tx.TLF01_details.handler_queue, cadencie_host_que_name, 6) == 0 &&	// nirbhay Thoughtfocus: cadencie changes
	//	strcmp(Auth_Tx.TLF01_details.message_type, "0302") == 0)
	//  )
	//{

	   ReturnCode = txservice_Verify_Currency_Code () ;
		if (ReturnCode != NO_AUTH_ERROR)
			return ReturnCode ;
	//}

    TransactionOrigin = txservice_GetTransactionOrigin () ;
	if (TransactionOrigin != LOCAL_ORIGIN && TransactionOrigin != REMOTE_ORIGIN)
	{
        strcpy (strTemp, "Invalid dispensation origin for tx") ;
	}

	else if (Auth_Tx.TLF01_details.tx_key == 0 || Auth_Tx.TLF01_details.tx_key == AUTH_INVALID)
	{
        strcpy (strTemp, "Invalid tx_key for the transaction") ;
	}    
    else if (strlen(Auth_Tx.TLF01_details.card_num)  == 0)
	{
        strcpy (strTemp, "Invalid card number for the transaction") ;
	}

    else if (!FieldIsNumeric (Auth_Tx.TLF01_details.card_num))
    {
        strcpy (strTemp, "Invalid card number for the transaction") ;
    }

    else if (strlen (Auth_Tx.TLF01_details.card_num)  > 19)
    {
        strcpy (strTemp, "Invalid card number for the transaction") ;
    }    

    else if (strlen(Auth_Tx.TLF01_details.processing_code)  != 6)
	{
        strcpy (strTemp, "Invalid processing code for transaction") ;
	}

    else if (!FieldIsNumeric (Auth_Tx.TLF01_details.processing_code))
    {
        strcpy (strTemp, "Invalid processing code for transaction") ;
    }

    else if (strlen(Auth_Tx.TLF01_details.primary_key.transaction_id)  == 0)
	{
        strcpy (strTemp, "Invalid transaction_id for transaction") ;
	}

	
	else if (strlen(Auth_Tx.TLF01_details.handler_queue)  == 0)
	{
        strcpy (strTemp, "Invalid handler queue for transaction") ;
	}

    else if ((strlen (Auth_Tx.TLF01_details.total_amount) == 0) &&
            (strlen (Auth_Tx.TLF01_details.reversal_amount) == 0))
    {
    	if(!(AUTH_BALANCE_INQUIRY == Auth_Tx.TLF01_details.tx_key || 
			 AUTH_BALANCE_INQUIRY_RESPONSE== Auth_Tx.TLF01_details.tx_key ))
    	{
        	strcpy (strTemp, "Invalid amount for the transaction") ;
    	}
    }
    else if ( (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) ) &&

		   	( (Auth_Tx.MCF01_details.deferred_merchant[0]   == 'Y') || 
			     (Auth_Tx.MCF01_details.house_card_merchant[0] == 'Y') )  )
   {	
			sprintf (strTemp, "Inval CurrencyCode%s,def/houseCard Tx", 
					Auth_Tx.TLF01_details.currency_code) ;
            
		
	}
    else if (strlen (Auth_Tx.TLF01_details.category_code) == 0 &&
       !txservice_If_Tx_Is_Treated_Like_A_Reversal () )
    {
        strcpy (strTemp, "Tx does not have a category code") ;
    }
    
    else if (TxAuthorize_Is_Transaction_Manually_Entered () &&
             !TxAuthorize_Is_Merchant_A_MOTO_TE_Merchant () &&
             !strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
    {
        /* Decline manually keyed-in transactions from all merchants other than
           MOTO and T&E Merchants, except VOICE AUTH transactions */
        strcpy (strTemp, "Merchant cannot do manually keyed-in Tx") ;
    }

   else if (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION )
   {
      if (!strEqual (Auth_Tx.MCF01_details.deferred_merchant, "Y"))
      {
         strcpy (strTemp, "Only def merchants can do def_purchases") ;
      }
      else if (strlen(Auth_Tx.TLF01_details.deferred_term_length) == 0)
      {
         strcpy (strTemp, "Null term length for def merchant") ;
      }
      else if (atoi(Auth_Tx.TLF01_details.deferred_term_length) == 0)
      {
         sprintf (strTemp, "Invalid term length (%s) for def merchant", Auth_Tx.TLF01_details.deferred_term_length) ;
      }
	  else if (TransactionOrigin == REMOTE_ORIGIN)
      {
         strcpy (strTemp, "Deferred Tx cannot have a remote origin") ;
      }
      else
      {
         /* Check the Deferred Promotional date range. */
         memset( start_date, 0x00, DATE_LEN );
         memset( end_date,   0x00, DATE_LEN );
         memset( txn_date,   0x00, DATE_LEN );

         if ( Auth_Tx.TLF01_details.date_yyyymmdd[0] == 0x00 )
         {
            /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
            ptetime_get_timestamp( time_date );

            /* Reformat to get date "YYYYMMDD". */
            get_date( time_date, Auth_Tx.TLF01_details.date_yyyymmdd );
         }

         memcpy( txn_date,  Auth_Tx.TLF01_details.date_yyyymmdd,      DATE_LEN);
         memcpy( start_date,Auth_Tx.MCF01_details.deferred_start_date,DATE_LEN);
         memcpy( end_date,  Auth_Tx.MCF01_details.deferred_end_date,  DATE_LEN);

         if ( (0 > strcmp(txn_date,start_date)) ||
              (0 < strcmp(txn_date,end_date  ))  )
         {
            strcpy( strTemp, "Deferred Promotion is not active" );
         }
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD || 
	        Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD_REPEAT ||
	        Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD_CONFIRM)
   {

      if (!strEqual (Auth_Tx.MCF01_details.reload_merchant, "Y"))
         strcpy (strTemp, "Only Reload merchants can do reload transactions") ;
      
   }




	if (strTemp[0] != 0)
    {
        if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
        txcntl_log_message( 1, 3,strTemp,"txservice_Perform_Auth_Tx_Checks", 1 );
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
												REFER_TO_CARD_ISSUER) ;
		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
    }

	return NO_AUTH_ERROR ;
} /* txservice_Perform_Auth_Tx_Checks */

BYTE txservice_If_Tx_Requires_Pin_Validation (void)
{
   if (  strcmp ( Auth_Tx.CAF01_details.pin_verification_method,"J") == 0 )
      return false;

   if(true == txservice_check_for_DCI_refund_txn() || true == txservice_check_for_AMEX_refund_txn())
   {
	   return false;
   }
   /*AMEX02*/


	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_SALE:
		case AUTH_AUTHORIZATION:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_CASH_ADVANCE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
		case AUTH_QUASI_CASH:
		case AUTH_BALANCE_INQUIRY:
			return true ;
		default:
			return false ;
	}
} /* txservice_If_Transaction_Requires_Pin_Validation */

BYTE txservice_If_Tx_Requires_CVV_Validation (void)
{
  if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))
  {
	  if (strlen(Auth_Tx.TLF01_details.cvc_data) > 0)
	  {
		  /* For POS Trxns */
		   switch (Auth_Tx.TLF01_details.tx_key)
		   {
			  case AUTH_SALE:
			  case AUTH_AUTHORIZATION:
			  case AUTH_PRE_AUTHORIZATION:
			  case AUTH_CASH_ADVANCE:
			  case AUTH_CASH_ADVANCE_AUTHORIZATION:
			  case AUTH_QUASI_CASH:
			  case AUTH_BALANCE_INQUIRY:
			  case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
			  case AUTH_CARD_VERIFICATION:
				 return true ;
			  default:
				 return false ;
		   }
	  }
	  else
	  {
		  return false ;
	  }
  }
  else
  {
	  /* For Voice and Incoming Trxns */
	   switch (Auth_Tx.TLF01_details.tx_key)
	   {
	      case AUTH_SALE:
	      case AUTH_AUTHORIZATION:
	      case AUTH_PRE_AUTHORIZATION:
	      case AUTH_CASH_ADVANCE:
	      case AUTH_CASH_ADVANCE_AUTHORIZATION:
	      case AUTH_QUASI_CASH:
	      case AUTH_BALANCE_INQUIRY:
	      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
	      case AUTH_CARD_VERIFICATION:
	         return true ;
	      default:
	         return false ;
	   }
  }

} /* txservice_If_Transaction_Requires_Pin_Validation */

BYTE txservice_If_Tx_Is_Treated_Like_A_Adjustment (void)
{
   switch (Auth_Tx.TLF01_details.tx_key)
   {
      case AUTH_SALE_ADJUSTMENT:
      case AUTH_REFUND_ADJUSTMENT: 
         return true ;
      default:
         return false ;
   }
}

BYTE txservice_If_Tx_Is_Treated_Like_A_Reversal (void)
{
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_REVERSAL:
		case AUTH_ATM_CONFIRM:
		case AUTH_REVERSAL_ADVICE:
		case AUTH_REVERSAL_REPEAT:
		case AUTH_REVERSAL_ADVICE_REPEAT:
		case AUTH_REVERSAL_RESPONSE:
		case AUTH_REVERSAL_ADVICE_RESPONSE:
		case AUTH_REVERSAL_REPEAT_RESPONSE:
		case AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE:
		  	return true ;
		default:
			return false ;
	}
} /* txservice_If_Transaction_Is_A_Reversal */

BYTE txservice_If_Tx_Is_Treated_Like_A_Offline_Void (void)
{
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_OFFLINE_VOID_SALE:
			if ( strcmp ( Auth_Tx.TLF01_details.orig_retrieval_ref_num, "" ) != 0 )
   		  		return true ;
		break ;	/* Added by SDG */

		case AUTH_OFFLINE_VOID_REFUND:
			return true ;
		break ;
		default:
			return false ;
		break ;
	}
	return false ;	/* Added by SDG */
} /* txservice_If_Tx_Is_Treated_Like_A_Offline_Void */

BYTE txservice_If_Tx_Qualifies_For_Expiry_Date_Checking (void)
{
   BYTE  retval = false;
/* TF-Phani : STIP-RPS transctions hsould by pass this check*/
   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y' && 
   	  strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0) 
   	{
   	 return( retval );
   	}
   if(true == txservice_check_for_DCI_refund_txn() ||
	  'R'  == Auth_Tx.TLF01_details.product_codes[6].quantity[0] ||
	  true == txservice_check_for_AMEX_refund_txn()) /*AMEX02-Mandate2024*/
   {
	   return false;
   }
   if((Auth_Tx.CAF01_details.expiration_date_check[0] == CHECK_EXPIRY_YES) ||
      (Auth_Tx.CAF01_details.expiration_date_check[0] == CHECK_EXPIRY_EXPANDED))
   {
      switch( Auth_Tx.TLF01_details.tx_key )
      {
         case AUTH_SALE:
         case AUTH_PRE_AUTHORIZATION:
         case AUTH_AUTHORIZATION:
         case AUTH_QUASI_CASH:
         case AUTH_CASH_ADVANCE:
         case AUTH_CASH_ADVANCE_AUTHORIZATION:
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
         case AUTH_BALANCE_INQUIRY:
         case AUTH_RELOAD_CONFIRM:
            retval = true;
      }
   }
   return( retval );
} /* txservice_If_Tx_Qualifies_For_Expiry_Date_Checking*/

BYTE txservice_If_Tx_Qualifies_For_Authorization_Checks (void)
{
   switch (Auth_Tx.TLF01_details.tx_key)
   {
      case AUTH_OFFLINE_SALE:
      case AUTH_ADVICE:
      case AUTH_SALE:
      case AUTH_QUASI_CASH:
      case AUTH_CARD_VERIFICATION:
      case AUTH_AUTHORIZATION:
      case AUTH_PRE_AUTHORIZATION:
      case AUTH_CASH_ADVANCE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
      case AUTH_REVERSAL:
      case AUTH_ATM_CONFIRM:
      case AUTH_REVERSAL_ADVICE:
      case AUTH_ADVICE_REPEAT:
      case AUTH_REVERSAL_REPEAT:
      case AUTH_AUTHORIZATION_OVERRIDE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
      case AUTH_VOID_SALE:
      case AUTH_VOID_REFUND:
      case AUTH_OFFLINE_VOID_SALE:
      case AUTH_OFFLINE_VOID_REFUND:
      case AUTH_SALE_ADJUSTMENT:
      case AUTH_REFUND_ADJUSTMENT:
      case AUTH_REFUND:
      case AUTH_BACKOFFICE_ADJUSTMENT: 
      case AUTH_REPRESENTMENT:
      case AUTH_CASH_DISBURSEMENT_ADJUSTMENT:
      case AUTH_ADMINISTRATIVE_TEXT: 
      case AUTH_VOID_RELOAD:
      case AUTH_RELOAD_CONFIRM:
         return true ;

      default:
         return false ;
   }
}


BYTE txservice_If_Tx_Qualifies_For_Card_Velocity_Checking (void)
{
   BYTE  retval;

   if ( (strEqual(Auth_Tx.TLF01_details.handler_queue, "VOICE")) &&
        (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX)) &&
       ((strEqual(Auth_Tx.BIN01_details.auth_ind,sREMOTE_PROCESSING_INDICATOR)) ||
        (strEqual(Auth_Tx.CAF01_details.authorization_location,REMOTE_AUTHORIZATION))))
   {
      /* Equitable requirement for Amex Voice txns to not check
       * offus frequency.  These get routed to ncposa.  SCR 12498.
       */
      retval = false;
   }
   else
   {
      switch (Auth_Tx.TLF01_details.tx_key)
      {
         case AUTH_OFFLINE_SALE:
         case AUTH_ADVICE:
         case AUTH_SALE:
         case AUTH_QUASI_CASH:
         case AUTH_AUTHORIZATION:
         case AUTH_PRE_AUTHORIZATION:
         case AUTH_CASH_ADVANCE:
         case AUTH_CASH_ADVANCE_AUTHORIZATION:
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
         case AUTH_REVERSAL:
         case AUTH_VOID_SALE:
         case AUTH_VOID_CASH_ADVANCE:

         case AUTH_ATM_CONFIRM:
         case AUTH_REVERSAL_ADVICE:
         case AUTH_ADVICE_REPEAT:
         case AUTH_REVERSAL_REPEAT:
         case AUTH_AUTHORIZATION_OVERRIDE:
         case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:

         case AUTH_RELOAD:
         case AUTH_RELOAD_REPEAT:
         case AUTH_RELOAD_CONFIRM:
            retval = true;
            break;
         default:
            retval = false;
            break;
      }
   }
   return( retval );
}

BYTE txservice_If_Tx_Qualifies_For_Fleet_Card_Checks (void)
{
   BYTE ReturnCode = txservice_Get_Card_SubType () ;
   if (ReturnCode != 'F')
      return false ;

 	switch (Auth_Tx.TLF01_details.tx_key)
   {
      case AUTH_SALE:
      case AUTH_QUASI_CASH:
      case AUTH_AUTHORIZATION:
      case AUTH_PRE_AUTHORIZATION:
      case AUTH_CASH_ADVANCE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION:
      case AUTH_AUTHORIZATION_OVERRIDE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
         return true ;
      default:
         return false ;
   }
}


/******************************************************************************
 *
 *  NAME:         txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing
 *
 *  DESCRIPTION:  This function determines if a transaction qualifies for the
 *                Cash Bonus Program.  There are 3 qualifiers: Bin, Merch, Card.
 *                
 *                If bin, merch, and card are tagged, qualifies.
 *                If bin and merch - switched out qualifies.
 *                If bin tagged, but merch or card are not, not qualified
 *                If bin not tagged, not qualified
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction qualifies, else false
 *
 *  AUTHOR:       Sanjoy Dasgupta
 *
 ******************************************************************************/
BYTE txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing()
{
   BYTE  ret_val = false;

   /* Determine if Cash Bonus flag is set on the Bin record. */
   if ( Auth_Tx.BIN01_details.mod_check[0] & CASH_ON )
   {
      /* Bin record is tagged. Check the Merchant record. */
      if ( Auth_Tx.MCF01_details.cash_bonus_eligible[0] == 'Y' )
      {
         /* Merchant record is tagged.  Continue checking. */
         if ( true == txservice_CashBonusTxn() )
         {
            /* This is a txn that is eligible for cash bonus. */
            if ( LOCAL_ORIGIN == txservice_GetTransactionOrigin() )
            {
               /* Transaction is of local origin.  This is good. */
               if ( 0 == strcmp(Auth_Tx.TLF01_details.dispensation.auth_2, "RA") )
               {
                  /* Transaction will be switched out.
                   * With Bin and Merchant records tagged,
                   * this transaction is eligible for the Cash Bonus award.
                   */
                  ret_val = true;
               }
               else
               {
                  if ( Auth_Tx.CCF02P_details.cash_bonus_eligible[0] == 'Y' )
                  {
                     /* This is a locally processed transaction.
                      * All the flags are properly set, so it
                      * qualifies for the Cash Bonus award.
                      */
                     ret_val = true;
                  }
               }
            }
         }
      }
   }
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         txservice_CashBonusTxn
 *
 *  DESCRIPTION:  This function determines if a transaction is a cash bonus
 *                eligible transaction.  Some are not, such as Cash Advance.
 *
 *  INPUTS:       Auth_Tx - (Global) Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction is a cash bonus txn, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE txservice_CashBonusTxn()
{
   BYTE  ret_val = false;
   BYTE  tx_key;

   if ( Auth_Tx.TLF01_details.voice_auth[0] != 'Y' )
   {
      tx_key = Auth_Tx.TLF01_details.tx_key;
      if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
      {
         /* Reversal: Let it qualify for some reversals to undo
          *           the transaction and winner counters.
          */
         if ( 0 == strcmp(Auth_Tx.TLF01_details.orig_message,"0200"))
         {
            if ( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,"00",2) )
               tx_key = AUTH_SALE;
            else if (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"11",2))
               tx_key = AUTH_QUASI_CASH;
         }
      }

      switch( tx_key )
      {
         case AUTH_SALE:
         case AUTH_QUASI_CASH:
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:

         case AUTH_SALE_RESPONSE:
         case AUTH_QUASI_CASH_RESPONSE:
         case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE:
            ret_val = true;
            break;

         default:
            ret_val = false;
            break;
      }
   }
   return( ret_val );
}


BYTE txservice_If_Tx_Qualifies_For_Balance_Account_Checks (void)
{
	if (strEqual (Auth_Tx.CAF01_details.authorization_method, POSITIVE_FILE)) 
	{
		// Balance checking is not required
		return false ;
	}
	if(true == txservice_check_for_ASI_txn())
	{
		// Balance checking is not required for MC ASI transaction
		return false;
	}
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_OFFLINE_SALE:
      case AUTH_OFFLINE_VOID_REFUND:
		case AUTH_ADVICE:
		case AUTH_SALE:
		case AUTH_QUASI_CASH:
		case AUTH_AUTHORIZATION:
      case AUTH_CARD_VERIFICATION:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_CASH_ADVANCE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
		case AUTH_REVERSAL:
		case AUTH_ATM_CONFIRM:
		case AUTH_REVERSAL_ADVICE:
		case AUTH_ADVICE_REPEAT:
		case AUTH_REVERSAL_REPEAT:
      case AUTH_AUTHORIZATION_OVERRIDE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
      case AUTH_VOID_SALE:
		case AUTH_SALE_ADJUSTMENT:
      case AUTH_REFUND_ADJUSTMENT:
      case AUTH_REFUND:
      case AUTH_VOID_REFUND:
		  case AUTH_VOID_RELOAD:
	  case AUTH_RELOAD_CONFIRM:

         return true ;

      case AUTH_OFFLINE_VOID_SALE:
			if ( strcmp ( Auth_Tx.TLF01_details.orig_retrieval_ref_num, "" ) == 0 )
      return false ;
         else
      return true;
		break ;	/* Added by IA for offline voids that never made to the system */

      default:
			return false ;
	}
   			return false ;

}

BYTE txservice_If_Tx_Cannot_Be_Declined (void)
{
	if (txservice_If_Tx_Is_Treated_Like_A_Offline_Sale () ||
		txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice () ||
		txservice_If_Tx_Is_Treated_Like_A_Advice () ||
		txservice_If_Tx_Is_Treated_Like_A_Reversal () ||
		txservice_If_Tx_Is_Treated_Like_A_Adjustment () ||
		txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override ())
	{
		return true ;
	}
	return false ;
}

BYTE txservice_If_Tx_Is_Treated_Like_A_Offline_Sale (void)
{
	if (strEqual (Auth_Tx.TLF01_details.message_type, "0221"))
		/* REPEAT of a OFFLINE_SALE */
		return true ;
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_OFFLINE_SALE:
		case AUTH_ADVICE:
		case AUTH_ADVICE_REPEAT:
		  	return true ;
		default:
			return false ;
	}
} /* txservice_If_Tx_Is_Treated_Like_A_Offline_Sale */

BYTE txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override (void)
{
   switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_AUTHORIZATION_OVERRIDE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
		  	return true ;
		default:
			return false ;
	}
}

BYTE txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice (void)
{
	char strMessageType [20] = {0} ;
	char strFirstTwoBytesOfMessageType [20] = {0} ;
	char strLastTwoBytesOfMessageType [20] = {0} ;

	strcpy (strMessageType, Auth_Tx.TLF01_details.message_type) ;
	strncpy (strFirstTwoBytesOfMessageType, strMessageType, 2) ;
	strncpy (strLastTwoBytesOfMessageType, strMessageType+2, 2) ;
	
   if (!strEqual (strFirstTwoBytesOfMessageType, "04"))
	{  /* Non Reversal Advices eg Offline Sale or Auth_Advice or Auth Advice Repeat */
		if (strEqual (strLastTwoBytesOfMessageType, "20") ||
			 strEqual (strLastTwoBytesOfMessageType, "21") ||
			 strEqual (strLastTwoBytesOfMessageType, "01")) 
		{
            return true ;
		}
		else
		{
			return false ;
		}

	}
	else 
	{
		return false ;
	}
} /* txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice */

BYTE txservice_If_Tx_Is_Treated_Like_A_Advice (void)
{
    char strMessageType [20] = {0} ;
    char strFirstTwoBytesOfMessageType [20] = {0} ;
	char strLastTwoBytesOfMessageType [20] = {0} ;
	
    strcpy (strMessageType, Auth_Tx.TLF01_details.message_type) ;
	strncpy (strFirstTwoBytesOfMessageType, strMessageType, 2) ;
	strncpy (strLastTwoBytesOfMessageType, strMessageType+2, 2) ;

	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)  ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)) &&
	   (0 == strncmp(strLastTwoBytesOfMessageType, "20",2)))
	{/* will include all ncmcrd and ncmcr3*/
		return false;
	}
	
    /* Non Reversal Advices eg Offline Sale or Auth_Advice or Auth Advice Repeat */
	if (strEqual (strLastTwoBytesOfMessageType, "20") ||
		 strEqual (strLastTwoBytesOfMessageType, "21") ||
		 strEqual (strLastTwoBytesOfMessageType, "01")) 
	{
        return true ;
	}
	else
	{
		return false ;
	}
} /* txservice_If_Tx_Is_Treated_Like_A_Advice */

BYTE txservice_If_Tx_Is_A_Deferred_Purchase_Type (void)
{
   BYTE ret_val;
   CHAR strProcessingCode [20] = {0} ;

   strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;
   if (strEqual (strProcessingCode, "70"))
      ret_val = true;
   else if ( txservice_If_Tx_Is_Treated_Like_A_Deferred() )
      ret_val = true;
   else
      ret_val = false;

   return( ret_val );
} /* txservice_If_Tx_Is_A_Deferred_Purchase_Type */


BYTE txservice_If_Reversed_Tx_Is_A_Sale_Type_Tx (void)
{
	char strProcessingCode [20] = {0} ;
	strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;
      /* Authorization, Sale, Offline_Sale, Quasi Cash, Auth_Advice */
	if (strEqual (strProcessingCode, "00") || 
		 /* Pre_Authorization */
		 strEqual (strProcessingCode, "30") ||
       /* Quasi Cash ?? */
       strEqual (strProcessingCode, "18") ||
       strEqual (strProcessingCode, "11") ||
		 /* Refund IA */
       strEqual (strProcessingCode, "20") ||
		 /* Cash_Advance */
		 strEqual (strProcessingCode, "01") ||
		 strEqual (strProcessingCode, "17") ||
       /* Deferred Purchase */
       strEqual (strProcessingCode, "70") ||
       /*Account funding transaction*/
	   strEqual (strProcessingCode, "10") ||
       /*Original Credit transaction*/
	   strEqual (strProcessingCode, "26"))

		 return true ;
	else 
		return false ;
} /* txservice_If_Reversed_Tx_Is_A_Sale_Type_Tx */


BYTE txservice_Tx_Qualifies_To_Be_Completed_Before_Authorization (void)
{
	 /* Void sale txn comes from the Mastercard host(Swithced in) 
	    send that txn into cadencie, don't process here (09-76) 11 Nov, 2009 TF-Ajay */

	if((Auth_Tx.TLF01_details.tx_key==AUTH_VOID_SALE))
	{
		if(true == txservice_Check_If_MC_Transaction())
		{  /*retrun false so this txn will not proces before authorization.
	   	   It will forward to network controller */

			return false;
		}
	}
	else
	{
		switch (Auth_Tx.TLF01_details.tx_key)
		{
		case AUTH_VOID_SALE:			/* remotely acquired AUTH_VOID_SALE */
		case AUTH_VOID_REFUND:		    /* remotely acquired AUTH_VOID_REFUND */
	/*	case AUTH_OFFLINE_VOID_SALE: 
		case AUTH_OFFLINE_VOID_REFUND:*/
			
	/*	case AUTH_REFUND:*/
	/*	case AUTH_OFFLINE_REFUND: */ /* TF PHANI- BDO want to porcess OFFLINE REFUND*/
	/*	case AUTH_SALE_ADJUSTMENT:
		case AUTH_REFUND_ADJUSTMENT: */
		case AUTH_VOID_CASH_ADVANCE: 
		case AUTH_SALE_CASH:
		case AUTH_CHECK_VERIFICATION:
	/*	case AUTH_CARD_VERIFICATION:*/
		case AUTH_PAYMENT:
		case AUTH_PAYMENT_ADJUSTMENT:
		case AUTH_SETTLE:
		case AUTH_BATCH_UPLOAD:
		case AUTH_SETTLE_TRAILER:
		case AUTH_STATISTICS:
		case AUTH_LOGON:
		case AUTH_TEST:
		/*case AUTH_VOID_PRE_AUTHORIZATION:*/
		case AUTH_INITIALIZATION:
		case AUTH_PLEASE_WAIT_ADVICE:
		case AUTH_VOID_PAYMENT:
		case AUTH_REGISTRATION:
		case AUTH_CARD_UPDATE:
		case AUTH_RECONCILE_ADVICE:
		case AUTH_NETWORK_ADVICE:
		/* case AUTH_BALANCE_INQUIRY: */
			return true ;

		/* All other transaction types would fall under the default case
			including remotely acquired AUTH_REVERSAL transactions
		*/
		
		default:
			return false ;
		break ;

		} /* switch */
	}/* else */
} /* txservice_Tx_Qualifies_To_Be_Completed_Before_Authorization */



/*************************************************************************************
NAME:			txservice_Replace_Tx_Key_In_Auth_Tx
DESCRIPTION:	Contains logic for changing the tx key depending on the transaction
INPUTS:			None
OUTPUTS:		None
RETURNS:        void
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void txservice_Replace_Tx_Key_In_Auth_Tx (void)
{
   char strTemp [200] = "";

	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_SALE:					
			Auth_Tx.TLF01_details.tx_key = AUTH_SALE_RESPONSE ;					
		break ;
      case AUTH_OFFLINE_VOID_SALE:
			Auth_Tx.TLF01_details.tx_key = AUTH_OFFLINE_VOID_SALE_RESPONSE ;					
		break ;
		case AUTH_REFUND:				
			Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_RESPONSE ;				
		break ;
		case AUTH_CASH_ADVANCE:			
			Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_RESPONSE ;			
		break ;
		case AUTH_PAYMENT:
			Auth_Tx.TLF01_details.tx_key = AUTH_PAYMENT_RESPONSE ;	
		break ;
		case AUTH_OFFLINE_SALE:			
			Auth_Tx.TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE ;			
		break ; 
		case AUTH_OFFLINE_REFUND:		
			Auth_Tx.TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE ;		
      break ;
      case AUTH_OFFLINE_VOID_REFUND:
         Auth_Tx.TLF01_details.tx_key = AUTH_OFFLINE_VOID_REFUND_RESPONSE ;
		break ;
		case AUTH_SALE_ADJUSTMENT:		
			Auth_Tx.TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE ;		
		break ;
		case AUTH_REFUND_ADJUSTMENT:	
			Auth_Tx.TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE ;	
		break ;
		case AUTH_PAYMENT_ADJUSTMENT:
			Auth_Tx.TLF01_details.tx_key = AUTH_PAYMENT_ADJUSTMENT_RESPONSE ;	
		break ;
		case AUTH_PRE_AUTHORIZATION:	
			Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_RESPONSE ;	
		break ;
		case AUTH_AUTHORIZATION:		
			Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE ;		
		break ;	
		case AUTH_BALANCE_INQUIRY:
			Auth_Tx.TLF01_details.tx_key = AUTH_BALANCE_INQUIRY_RESPONSE ;
		break ;
		case AUTH_VOID_SALE:			
			Auth_Tx.TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE ;			
		break ;
		case AUTH_VOID_REFUND:			
			Auth_Tx.TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE ;			
		break ;
		case AUTH_SETTLE:
			Auth_Tx.TLF01_details.tx_key = AUTH_SETTLE_RESPONSE ;		
		break ;
		case AUTH_BATCH_UPLOAD:
			Auth_Tx.TLF01_details.tx_key = AUTH_BATCH_UPLOAD_RESPONSE ;	
		break ;
		case AUTH_SETTLE_TRAILER:
			Auth_Tx.TLF01_details.tx_key = AUTH_SETTLE_TRAILER_RESPONSE ;	
		break ;
		case AUTH_STATISTICS:
			Auth_Tx.TLF01_details.tx_key = AUTH_STATISTICS_RESPONSE ;	
		break ;
		case AUTH_REVERSAL:				
			Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE ;				
		break ;
		case AUTH_LOGON:
			Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE ;
		break ;
		case AUTH_TEST:
			Auth_Tx.TLF01_details.tx_key = AUTH_TEST_RESPONSE ;
		break ;
		case AUTH_PIN_CHANGE:
			Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE ;
		break ;
		case AUTH_VOID_CASH_ADVANCE:	
			Auth_Tx.TLF01_details.tx_key = AUTH_VOID_CASH_ADVANCE_RESPONSE ;	
		break ;
		case AUTH_SALE_CASH:			
			Auth_Tx.TLF01_details.tx_key = AUTH_SALE_CASH_RESPONSE ;			
		break ;
		case AUTH_CHECK_VERIFICATION:	
			Auth_Tx.TLF01_details.tx_key = AUTH_CHECK_VERIFICATION_RESPONSE ;	
		break ;  
		case AUTH_CARD_VERIFICATION:	
			Auth_Tx.TLF01_details.tx_key = AUTH_CARD_VERIFICATION_RESPONSE ;	
		break ;  
		case AUTH_VOID_PRE_AUTHORIZATION:
			Auth_Tx.TLF01_details.tx_key = AUTH_VOID_PRE_AUTHORIZATION_RESPONSE ;
		break ;
		case AUTH_SALES_COMPLETION:		
			Auth_Tx.TLF01_details.tx_key = AUTH_SALES_COMPLETION_RESPONSE ;		
		break ;
		case AUTH_INITIALIZATION:
			Auth_Tx.TLF01_details.tx_key = AUTH_INITIALIZATION_RESPONSE ;
		break ;
		case AUTH_PLEASE_WAIT_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_PLEASE_WAIT_ADVICE_RESPONSE ;
		break ;
		case AUTH_ADMIN_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_ADMIN_ADVICE_RESPONSE ;
		break ;
		case AUTH_VOID_PAYMENT:
			Auth_Tx.TLF01_details.tx_key = AUTH_VOID_PAYMENT_RESPONSE ;
		break ;
		case AUTH_REGISTRATION:
			Auth_Tx.TLF01_details.tx_key = AUTH_REGISTRATION_RESPONSE ;
		break ;
		case AUTH_QUASI_CASH:
			Auth_Tx.TLF01_details.tx_key = AUTH_QUASI_CASH_RESPONSE ;
		break ;
		case AUTH_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_RESPONSE ;
		break ;
		case AUTH_REVERSAL_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE ;
		break ;
		case AUTH_CARD_UPDATE:
			Auth_Tx.TLF01_details.tx_key = AUTH_CARD_UPDATE_RESPONSE ;
		break ;
		case AUTH_RECONCILE_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_RECONCILE_ADVICE_RESPONSE ;
		break ;
		case AUTH_NETWORK_ADVICE:
			Auth_Tx.TLF01_details.tx_key = AUTH_NETWORK_ADVICE_RESPONSE ;
		break ;
		case AUTH_ADVICE_REPEAT:
			Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_REPEAT_RESPONSE ;
		break ;
		case AUTH_REVERSAL_REPEAT:
			Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT_RESPONSE ;
		break ;
		case AUTH_CASH_ADVANCE_AUTHORIZATION:	
			Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE ;	
		break ; 
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:	
			Auth_Tx.TLF01_details.tx_key = AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE ;	
		break ; 
      case AUTH_AUTHORIZATION_OVERRIDE:
         Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_OVERRIDE_RESPONSE ;
      break ;
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
         Auth_Tx.TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE ;
      break ;
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
         Auth_Tx.TLF01_details.tx_key = AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE ;
      break ;
      case AUTH_ATM_CONFIRM:
         Auth_Tx.TLF01_details.tx_key = AUTH_ATM_CONFIRM_RESPONSE ;
      break ;
      case AUTH_BACKOFFICE_ADJUSTMENT:
         Auth_Tx.TLF01_details.tx_key = AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE ;
      break;
      case AUTH_REPRESENTMENT:
         Auth_Tx.TLF01_details.tx_key = AUTH_REPRESENTMENT_RESPONSE ;
      break;
      case AUTH_CASH_DISBURSEMENT_ADJUSTMENT:
         Auth_Tx.TLF01_details.tx_key = AUTH_CASH_DISBURSEMENT_ADJUSTMENT_RESPONSE ;
      break;
      case AUTH_ADMINISTRATIVE_TEXT:
         Auth_Tx.TLF01_details.tx_key = AUTH_ADMINISTRATIVE_TEXT_RESPONSE ;
      break;
      case AUTH_FEE_COLLECTION_ACQUIRER:
         Auth_Tx.TLF01_details.tx_key = AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE ;
      break;
      case AUTH_FUNDS_DISBURSEMENT_ACQUIRER:
         Auth_Tx.TLF01_details.tx_key = AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE ;
      break;

	  case AUTH_RELOAD:
         Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_RESPONSE ;
      break;
	  case AUTH_RELOAD_REPEAT:
         Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_REPEAT_RESPONSE ;
      break;
	  case AUTH_RELOAD_CONFIRM:
         Auth_Tx.TLF01_details.tx_key = AUTH_RELOAD_CONFIRM_RESPONSE ;
      break;
	  case AUTH_VOID_RELOAD:
         Auth_Tx.TLF01_details.tx_key = AUTH_VOID_RELOAD_RESPONSE ;
      break;
	 case AUTH_OCT_TX:
         Auth_Tx.TLF01_details.tx_key = AUTH_OCT_TX_RESPONSE ;
      break;
	  case AUTH_FAST_FUND:
         Auth_Tx.TLF01_details.tx_key = AUTH_FAST_FUND_RESPONSE ;
      break;
	  case AUTH_MONEY_SEND:
         Auth_Tx.TLF01_details.tx_key = AUTH_MONEY_SEND_RESPONSE ;
      break;
	  case AUTH_PRE_AUTHORIZATION_CANCLE:
		Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE;
		break;
	
	  case AUTH_SALE_RESPONSE :	
      case AUTH_OFFLINE_VOID_SALE_RESPONSE :
	  case AUTH_REFUND_RESPONSE :				
	  case AUTH_CASH_ADVANCE_RESPONSE :			
	  case AUTH_PAYMENT_RESPONSE :	
	  case AUTH_OFFLINE_SALE_RESPONSE :			
	  case AUTH_OFFLINE_REFUND_RESPONSE :		
	  case AUTH_SALE_ADJUSTMENT_RESPONSE :		
	  case AUTH_REFUND_ADJUSTMENT_RESPONSE :	
	  case AUTH_PAYMENT_ADJUSTMENT_RESPONSE :	
	  case AUTH_PRE_AUTHORIZATION_RESPONSE :	
	  case AUTH_AUTHORIZATION_RESPONSE :		
	  case AUTH_BALANCE_INQUIRY_RESPONSE :
	  case AUTH_VOID_SALE_RESPONSE :			
	  case AUTH_VOID_REFUND_RESPONSE :			
	  case AUTH_SETTLE_RESPONSE :		
	  case AUTH_BATCH_UPLOAD_RESPONSE :	
	  case AUTH_SETTLE_TRAILER_RESPONSE :	
	  case AUTH_STATISTICS_RESPONSE :	
	  case AUTH_REVERSAL_RESPONSE :				
	  case AUTH_LOGON_RESPONSE :
	  case AUTH_TEST_RESPONSE :
	  case AUTH_PIN_CHANGE_RESPONSE :
	  case AUTH_VOID_CASH_ADVANCE_RESPONSE :	
	  case AUTH_SALE_CASH_RESPONSE :			
	  case AUTH_CHECK_VERIFICATION_RESPONSE :	
	  case AUTH_CARD_VERIFICATION_RESPONSE :	
	  case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE :
	  case AUTH_SALES_COMPLETION_RESPONSE :		
	  case AUTH_INITIALIZATION_RESPONSE :
	  case AUTH_PLEASE_WAIT_ADVICE_RESPONSE :
	  case AUTH_ADMIN_ADVICE_RESPONSE :
	  case AUTH_VOID_PAYMENT_RESPONSE :
	  case AUTH_REGISTRATION_RESPONSE :
	  case AUTH_QUASI_CASH_RESPONSE :
	  case AUTH_ADVICE_RESPONSE :
	  case AUTH_REVERSAL_ADVICE_RESPONSE :
	  case AUTH_CARD_UPDATE_RESPONSE :
	  case AUTH_RECONCILE_ADVICE_RESPONSE :
	  case AUTH_NETWORK_ADVICE_RESPONSE :
	  case AUTH_ADVICE_REPEAT_RESPONSE :
	  case AUTH_REVERSAL_REPEAT_RESPONSE :
	  case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE :	
	  case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE :
      case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
      case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
      case AUTH_ATM_CONFIRM_RESPONSE:
      case AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE:
      case AUTH_REPRESENTMENT_RESPONSE:
      case AUTH_CASH_DISBURSEMENT_ADJUSTMENT_RESPONSE:
      case AUTH_ADMINISTRATIVE_TEXT_RESPONSE:
      case AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE:
      case AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE:
	  case AUTH_RELOAD_RESPONSE:
	  case AUTH_RELOAD_REPEAT_RESPONSE:
	  case AUTH_RELOAD_CONFIRM_RESPONSE:
	  case AUTH_VOID_RELOAD_RESPONSE:
	  case AUTH_FAST_FUND_RESPONSE:
	  case AUTH_OCT_TX_RESPONSE:
	  case AUTH_MONEY_SEND_RESPONSE:
  	  case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE:
		break ;
		
		default:
         sprintf( strTemp,
                 "Invalid Tx Key (%d). Can't set to resp",
                  Auth_Tx.TLF01_details.tx_key );

		   txservice_Generate_Usage_Error_Message (2,strTemp,
																"txservice_Replace_Tx_Key_In_Auth_Tx",
																GS_TXN_TRANSACTION_NOT_ALLOWED,
																INVALID_TRANSACTION) ;	
         txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
												  REFER_TO_CARD_ISSUER) ;
				
		break;
	} // switch
} /* txservice_Replace_Tx_Key_In_Auth_Tx */

BYTE txservice_Filter_Transactions (void)
{
    char strTemp[256] = {0} ;
    switch (Auth_Tx.TLF01_details.tx_key)
	{
        case AUTH_SALE :					
        case AUTH_REFUND :				
        case AUTH_CASH_ADVANCE :			
        case AUTH_OFFLINE_SALE :			
        
        case AUTH_OFFLINE_REFUND :		
        case AUTH_SALE_ADJUSTMENT :		
        case AUTH_REFUND_ADJUSTMENT :	
        case AUTH_PRE_AUTHORIZATION :	
        
        case AUTH_AUTHORIZATION :
        case AUTH_CARD_VERIFICATION:
        case AUTH_BALANCE_INQUIRY :
        case AUTH_VOID_SALE :			
        case AUTH_VOID_REFUND :	
		case AUTH_OFFLINE_VOID_SALE:
        case AUTH_OFFLINE_VOID_REFUND:	
        case AUTH_REVERSAL :				
        case AUTH_PIN_CHANGE :
        case AUTH_VOID_CASH_ADVANCE :	
               	
            
        case AUTH_SALES_COMPLETION :		
        case AUTH_ADMIN_ADVICE :
        case AUTH_QUASI_CASH :
        case AUTH_ADVICE :
        case AUTH_REVERSAL_ADVICE :
        case AUTH_ATM_CONFIRM :
        
        case AUTH_RECONCILE_ADVICE :
        case AUTH_NETWORK_ADVICE :
        case AUTH_ADVICE_REPEAT :
        case AUTH_REVERSAL_REPEAT :
        case AUTH_REVERSAL_ADVICE_RESPONSE:
        case AUTH_CASH_ADVANCE_AUTHORIZATION :	
        case AUTH_DEFERRED_PURCHASE_AUTHORIZATION :
        case AUTH_AUTHORIZATION_OVERRIDE:
        case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
        case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
		case AUTH_BACKOFFICE_ADJUSTMENT:
		case AUTH_REPRESENTMENT:
		case AUTH_CASH_DISBURSEMENT_ADJUSTMENT:	
		case AUTH_ADMINISTRATIVE_TEXT:

		case AUTH_RELOAD : 
		case AUTH_RELOAD_REPEAT :
		case AUTH_RELOAD_CONFIRM  :
		case AUTH_VOID_RELOAD :
	    case AUTH_OCT_TX :
		case AUTH_FAST_FUND :
		case AUTH_MONEY_SEND :
        case AUTH_PRE_AUTHORIZATION_CANCLE:
        case AUTH_VOID_PRE_AUTHORIZATION:
            return NO_AUTH_ERROR ;

		// : commented as 302 msg is routed directly from ncvis2 to ncvisa.
		// ------------------------------------------------------
		// 302 msg routing from cadencie (ncvis2) to VISA (ncvisa)
		//case AUTH_CARD_UPDATE :
		//
		//	if(	strcmp(Auth_Tx.TLF01_details.message_type, "0302") == 0 &&
		//		strncmp(Auth_Tx.TLF01_details.handler_queue, cadencie_host_que_name, 6) == 0
		//	)
		//		return NO_AUTH_ERROR ;

			//else
			// FALL THROUGH TO DEFAULT CASE
        default:
            strcpy (strTemp, "Transaction not allowed\n") ;
            txcntl_log_message( 1, 3,strTemp,"txservice_Perform_Auth_Tx_Checks", 1 );
		    txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
												REFER_TO_CARD_ISSUER) ;
		    return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
    }
}

BYTE txservice_Generate_Usage_Error_Message( INT   SystemMonitor,
                                             pBYTE strTemp,
                                             pBYTE function,
                                             BYTE  general_status,
                                             pBYTE response_code )
{
   if ( strEqual(Auth_Tx.TLF01_details.handler_queue, "VOICE") )
      strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp );

   if(0== strncmp(Auth_Tx.TLF01_details.response_text,"DE48 SE84:",10) &&
	 (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) ||
	  0 == strncmp(Auth_Tx.TLF01_details.message_type,"0130",4)) &&
	  (0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) || 0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)))
   {
/* dont do anythig for this case*/
   }
   else
   {
	   memcpy( Auth_Tx.TLF01_details.response_text,
	   	              strTemp,
	   	              sizeof(Auth_Tx.TLF01_details.response_text)-1 );
   }


   txcntl_log_message( SystemMonitor, 3,strTemp,function, 1 );
   txservice_Prepare_AuthTx_For_Forwarding( general_status,	response_code );

   return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
}
/* TF PHANI.. Function to generate information message for STIP transctions*/
BYTE txserviceCadience_Generate_Usage_Error_Message( INT   SystemMonitor,
                                             pBYTE strTemp,
                                             pBYTE function,
                                             BYTE  general_status,
                                             pBYTE response_code )
{
	txcntl_log_message( 1, 3,strTemp,function, 1 );
    return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
}

/********************************************************************* *********
*
*  NAME:         RJ_WITH_LEAD_ZEROS
*
*  DESCRIPTION:  This procedure right justifies a string, padding 
leading
*                zeros.
*
*  INPUTS:       str      - Pointer to string to be right justified *                FieldLen - Length of str
*
*  OUTPUTS:      str - Contents of str is modified. *
*  RTRN VALUE:   None
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:

********************************************************************** ********/
void Rj_with_lead_zeros( pCHAR str, INT FieldLen ) 
{
   int i;
   int dif;
   int strlength;

   /* Convert any leading blanks to zero. */ 
   for( i=0; i< FieldLen; i++ )
   {
      if ( 0x20 == str[i] )
      memset( &str[i], 0x30, 1 );
      else
      break;
   }

   /* Remove any trailing blanks. */
   for( i=FieldLen-1; i>=0; i-- )
   {
      if ( 0x20 == str[i] )
      memset( &str[i], 0x00, 1 );
      else
      break;
   }

   /* Right justify, then prepend leading zeros. */ strlength = strlen( str );
   dif = FieldLen - strlength;

   if ( strlength < FieldLen )
   {
      for( i=FieldLen-1; i>=0; i-- )
      {
         if ( i >= dif )
         strncpy( &str[i], &str[i-dif], 1 );  /* Shift string to 
         the right */
         else
         memset( &str[i], 0x30, 1 );          /* Prepend leading 
         zeros     */
      }
   }
}

/******************************************************************************
 *
 *  NAME:         IS_TXN_VISA_ELECTRON
 *
 *  DESCRIPTION:  This function determines if a transaction is a Visa
 *                Electron.  It uses the Service Code from Track2 to make
 *                this determination.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing transaction info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction is a Visa Electron txn, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT txservice_is_txn_visa_electron( )
{
   INT   ret_val = false;
   CHAR  error_msg[100];
   CHAR  service_code[4] = "";

   /* Need to get the Service Code from the track 2 information. */
   if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
   {
      memset( error_msg, 0x00, sizeof(error_msg) );
      (void)txservice_get_service_code_from_track2( service_code, error_msg );
      if ( 0 == strcmp( service_code, VISA_ELECTRON_SERVICE_CODE ) )
         ret_val = true;
   }
   else if ( Auth_Tx.TLF01_details.track1[0] != 0x00 )
   {
      memset( error_msg, 0x00, sizeof(error_msg) );
      (void)txservice_get_service_code_from_track1( service_code, error_msg );
      if ( 0 == strcmp( service_code, VISA_ELECTRON_SERVICE_CODE ) )
         ret_val = true;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         txservice_GET_SERVICE_CODE_FROM_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 to find the service code.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing transaction info
 *
 *  OUTPUTS:      service_code - Service Code from Track 2
 *                error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT txservice_get_service_code_from_track2( pCHAR service_code, pCHAR error_msg )
{
   INT    ret_val = false;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator = NULL_PTR;

   if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = Auth_Tx.TLF01_details.track2;
      for( track2_idx=0; track2_idx <= MAX_CARD_NUM_LEN;
           track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( track2_idx > MAX_CARD_NUM_LEN )
      {
         strcpy( error_msg,
                "Unable to parse track2.  Cannot find field separator.");
      }
      else
      {
         /* Get the service code. */
         memcpy( service_code, field_separator+5, 3 );
         ret_val = true;
      }

      /* Put Card Number into Auth_Tx. */
      if ( Auth_Tx.TLF01_details.card_num[0] == 0x00 )
      {
         card_num_len = field_separator - Auth_Tx.TLF01_details.track2;

         /* If track2 length is odd, ignore leading zero on card number. */
         if ( 1 == (strlen(Auth_Tx.TLF01_details.track2) % 2) )
         {
            memcpy (Auth_Tx.TLF01_details.card_num,
                    Auth_Tx.TLF01_details.track2+1, (--card_num_len));
         }
         else
         {
            memcpy( Auth_Tx.TLF01_details.card_num,
                    Auth_Tx.TLF01_details.track2, card_num_len);
         }
         sprintf( Auth_Tx.TLF01_details.card_num_len,"%02d",card_num_len);
      }

      /* Get the expiration date. */
      if ( Auth_Tx.TLF01_details.exp_date[0] == 0x00 )
      {
         memcpy( Auth_Tx.TLF01_details.exp_date, field_separator+1, 4 );
      }
   }
   return( ret_val );
}
/********************************************************************
*
 *  NAME:         txservice_GET_SERVICE_CODE_FROM_TRACK1
 *
 *  DESCRIPTION:  This function parses track1 to find the service code.
 *
 *  INPUTS:       Auth_Tx - (Global)Structure containing transaction info
 *
 *  OUTPUTS:      service_code - Service Code from Track 1
 *                error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track1 is successfully parsed, else false
 *
 *  AUTHOR:       Sailaja Yarlagadda.
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT txservice_get_service_code_from_track1( pCHAR service_code, pCHAR error_msg )
{
   INT    ret_val = false;
   INT    card_num_len;
   CHAR   *field_separator,*field_separator1;
   
   if ( Auth_Tx.TLF01_details.track1[0] != 0x00 )
   {
      /* Locate the field separator. */
      
	  field_separator=strchr(Auth_Tx.TLF01_details.track1,'^');
	  if((field_separator==NULL) ||((((int)field_separator) - ((int)Auth_Tx.TLF01_details.track1)) > MAX_CARD_NUM_LEN))
	  {
		  strcpy( error_msg,
                "Unable to parse track1.  Cannot find field separator.");
	  }
	  else
	  {
		  field_separator1=strchr(field_separator+1,'^');
		  
		  if((field_separator1==NULL )||((field_separator1-(field_separator+1))>MAX_CARD_HOLDER_LEN))
		  {
             strcpy( error_msg,
                "Unable to parse track1.  Cannot find field separator.");
		  }
	  }
		  if(*field_separator1 == '^')	  
		  {		  
            memcpy( service_code, field_separator1+5, 3 );
   
         ret_val = true;
		  }

      /* Put Card Number into Auth_Tx. */
      if ( Auth_Tx.TLF01_details.card_num[0] == 0x00 )
      {
         card_num_len = ((int)field_separator) - ((int)Auth_Tx.TLF01_details.track1);

         /* If track1 length is odd, ignore leading zero on card number. */
         if ( 1 == (strlen(Auth_Tx.TLF01_details.track1) % 2) )
         {
            memcpy (Auth_Tx.TLF01_details.card_num,
                    Auth_Tx.TLF01_details.track1+1, (--card_num_len));
         }
         else
         {
            memcpy( Auth_Tx.TLF01_details.card_num,
                    Auth_Tx.TLF01_details.track1, card_num_len);
         }
         sprintf( Auth_Tx.TLF01_details.card_num_len,"%02d",card_num_len);
      }

      /* Get the expiration date. */
      if ( Auth_Tx.TLF01_details.exp_date[0] == 0x00 )
      {
         memcpy( Auth_Tx.TLF01_details.exp_date, field_separator1+1, 4 );
      }
   }
   return( ret_val );
}



/********************************************************************/

BYTE txservice_Switch_to_Cirrus(void)
{
	BYTE AccountType = 0 ;
	BYTE ReturnCode = 0 ;
	char strTemp [256] = {0} ;

/*	 Equitable confirmed the routing rules as follows:
	 Send txn to Cirrus when the following conditions are met:
     -  Record not found in Bin01
     -  Record not found in Bin02
     -  Card number starts with 3, 4, 5, or 6
     -  Transaction is ATM (MCC = 6011)
*/
   if ( strchr ("3456", Auth_Tx.TLF01_details.card_num[0]) != 0 &&
	    strEqual(Auth_Tx.TLF01_details.category_code, MCC_ATM) ) 
   {
      ReturnCode = txservice_Process_Bin_Range_Switch_Out (BIN02_DATA,
                                                           cirrus_host_que_name,
                                                           "") ;
   }
   else
   {
      strcpy (strTemp, "Cannot switch debit to PLUS or CIRRUS") ;
      ReturnCode = txservice_Generate_Usage_Error_Message (1,strTemp, 
                                                          "txservice_Switch_to_Cirrus",
                                                           GS_TXN_BIN_NOT_FOUND, 
                                                           REFER_TO_CARD_ISSUER) ;
   }

	return ReturnCode ;
} /* txservice_Switch_to_Cirrus */
/************************************************************************************************/

INT validate_acquirer_id(void)
{
   CHAR  strTemp[256] = {0} ;
   BYTE  ReturnCode = 0 ;    
   INT   i;
   INT   ret_val = EMPTY;

   if ( 0x00 != Auth_Tx.BIN01_details.acquirer_blocking[0].acquirer_id[0] )
   {
      /* We have some Acquirer IDs in Bin01 to block. */
      if ( 0x00 != Auth_Tx.TLF01_details.acquiring_id[0] )
      {
         /* The transaction contains an Acquirer ID. */
         if( REMOTE_ORIGIN == txservice_GetTransactionOrigin() )
         {
            /* Block transactions that originated remotely if the Acquirer ID
             * in the transaction is in the list of Acquirer IDs in Bin01 to
             * be blocked.
             */
            ret_val = NOT_BLOCKED;
            for ( i = 0; i <= 20; i++ )
            {
               if(strcmp(Auth_Tx.BIN01_details.acquirer_blocking[i].acquirer_id,
                         Auth_Tx.TLF01_details.acquiring_id)==0 )
               {
                  /* Acquirer ID in txn is in the BIN01 list. Block it. */
                  ret_val = BLOCKED ;
                  break;
               }
            }
         }
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         validate_mcc_range
 *
 *  DESCRIPTION:  This function verifies whether the merchant category code 
 *                resides within a specified range or not.  Validation is
 *                performed if mcc resides within the range.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *                
 *  RTRN VALUE:   EMPTY       = There are no MCC ranges being blocked
 *                BLOCKED     = MCC falls within a range that is being blocked
 *                NOT_BLOCKED = MCC is not being blocked
 *
 *  AUTHOR:       Sailaja Yarlagadda.
 *
 ******************************************************************************/
INT validate_mcc_range(void) 
{
   CHAR  strTemp[256] = {0} ;
   BYTE  ReturnCode   = 0 ; 
   INT   i;
   INT   ret_val = EMPTY;
   
   if (true == txservice_If_Tx_Is_Treated_Like_A_Reversal())
   {
		return NOT_BLOCKED;
   }

   if ( (strlen( Auth_Tx.BIN01_details.mcc_low[0].low_range   ) != 0) &&
        (strlen( Auth_Tx.BIN01_details.mcc_high[0].high_range ) != 0) )
   {
      if( REMOTE_ORIGIN == txservice_GetTransactionOrigin() )
      {
         ret_val = NOT_BLOCKED;
         for ( i = 0; i <= 10; i++ )
         {
            if (( strcmp(Auth_Tx.BIN01_details.mcc_low[i].low_range,  Auth_Tx.TLF01_details.category_code) <= 0 ) &&
                ( strcmp(Auth_Tx.BIN01_details.mcc_high[i].high_range,Auth_Tx.TLF01_details.category_code) >= 0 ) )
            {
               ret_val = BLOCKED;
               break;
            }
         }
      }
   }
   return( ret_val );
} 

/*INT validate_Bin_Range_ForChip_Fallback_Txn(void)
{
   CHAR  strTemp[256] = {0} ;
   BYTE  ReturnCode   = 0 ;
   INT   i;
   INT   ret_val = EMPTY;

   if (true == txservice_If_Tx_Is_Treated_Like_A_Reversal())
   {
		return NOT_BLOCKED;
   }

   if ( (strlen( Auth_Tx.BIN01_details.primary_key.low  ) != 0) &&
        (strlen( Auth_Tx.BIN01_details.high ) != 0) )
   {
        ret_val = NOT_BLOCKED;

           if (( strcmp(Auth_Tx.BIN01_details.primary_key.low,  Auth_Tx.TLF01_details.product_codes[2].code) <= 0 ) &&
                ( strcmp(Auth_Tx.BIN01_details.high,Auth_Tx.TLF01_details.category_code) >= 0 ) )
            {
               ret_val = BLOCKED;
               break;
            }


   }
   return( ret_val );
}*/
/*****************************************************************************/


BOOLEAN ValidateCardNumber()
{
   CHAR     pCardNbr[20]="";
   INT      nLastDigit;
   INT      len;
   BOOLEAN  ret_val = TRUE;

   len = strlen( Auth_Tx.TLF01_details.card_num );
   memcpy( pCardNbr, Auth_Tx.TLF01_details.card_num, len - 1 );

   /* Pass the card number (minus the last digit) to the Mod10 algorithm. */
   nLastDigit = GetMod10CheckDigit( pCardNbr, len-1 );

   /* Compare the calculated check digit
    * with the last digit of the card number.
    */
   if ( nLastDigit != atoi(&Auth_Tx.TLF01_details.card_num[len-1]) )
   {
      /* Check digit does not match. */
      ret_val = FALSE;
   }
   return( ret_val );
}

/***********************************************************************/
INT GetMod10CheckDigit( pCHAR input_str, INT input_len )
{
   INT  i;
   INT  dble   = 0;
   INT  result = 0;

   for( i = input_len - 1; i >= 0; i-- )
   {
      dble = (input_str[i] - 48 ) * 2;
      result = (dble >= 10) ? ( result + 1 + (dble - 10) ): (result + dble);
      i--;
      if(i>=0)
         result += (input_str[i] - 48 );
   }

   dble = result / 10 ;

   i = ((dble + 1) * 10);
   result = i - result;
   if( result > 9 )
       result = 0;

   return( result );
}


/*************************************************************************************************/
/* This function searches the defined set of BIN ranges and returns true if cardnumber is found
   within one of these ranges*/

INT txservice_is_selected_jcb_bin_for_cirrus()  
{

	if ( (strncmp(Auth_Tx.TLF01_details.card_num, "356200", 6) >= 0) &&	/*from 356200 to 356219*/
         (strncmp(Auth_Tx.TLF01_details.card_num, "356219", 6) <= 0) )
		 return true;
	else
	if ( (strncmp(Auth_Tx.TLF01_details.card_num, "356734", 6) >= 0) &&	/*from 356734 to 356734*/
         (strncmp(Auth_Tx.TLF01_details.card_num, "356734", 6) <= 0) )
		 return true;
    else
	if ( (strncmp(Auth_Tx.TLF01_details.card_num, "356754", 6) >= 0) &&	/*from 356754 to 356754*/
         (strncmp(Auth_Tx.TLF01_details.card_num, "356754", 6) <= 0) )
		 return true;
	else
	if ( (strncmp(Auth_Tx.TLF01_details.card_num, "358800", 6) >= 0) &&	/*from 358800 to 358999*/
         (strncmp(Auth_Tx.TLF01_details.card_num, "358999", 6) <= 0) )
		 return true;
	else
		return false;
}

/******************************************************************************
 *	NAME:        txservice_If_Txn_FraudGuard_Routing_Needed
 *
 *	DESCRIPTION: Determine if a transaction needs to go to Fraudguard
 *
 *	INPUTS:      fg_state - Contains indicators as to which card families
 *                         are enabled for Fraudguard - from TF.ini file.
 *	OUTPUTS:     None
 *
 *	RETURNS:     True if txn should be routed to Fraudguard
 *
 * AUTHOR:      Changan Huang
 ******************************************************************************/
BYTE txservice_If_Txn_FraudGuard_Routing_Needed( INT fg_state ) 
{
   BYTE  retval = false;
   BYTE  card_family_enabled = false;

   /* Fraudguard eligibility is different depending on if Equitable
    * is an Acquirer or an Issuer.  Can tell if this is an ECN card
    * (Issuer) by the presence or lack of a card record in Auth_Tx.
    */
   if ( Auth_Tx.CCF02P_details.primary_key.card_nbr[0] == 0x00 )
   {
      /*+----------+
        | ACQUIRER |
        +----------+*/

      /* Determine if Fraudguard functionality is enabled for card family. */
      switch( Auth_Tx.BIN01_details.card_family[1] )
      {
         case CARD_FAMILY_VISA:  if ( fg_state & FG_STATE_ACQ_VISA )
                                    card_family_enabled = true;
                                 break;

         case CARD_FAMILY_MC:    if ( fg_state & FG_STATE_ACQ_MC )
                                    card_family_enabled = true;
                                 break;

         case CARD_FAMILY_JCB:   if ( fg_state & FG_STATE_ACQ_JCB )
                                    card_family_enabled = true;
                                 break;

         case CARD_FAMILY_AMEX:  if ( fg_state & FG_STATE_ACQ_AMEX )
                                    card_family_enabled = true;
                                 break;
     }

      /* Fraudguard is currently for Locally Acquired transactions only. */
      if ( card_family_enabled == true )
      {
         if ( LOCAL_ORIGIN != txservice_GetTransactionOrigin() )
         {
            /* Remote origin; do not route to FG. */
            card_family_enabled = false;
         }
      }

      /* Filtering requirements:	
       * 1) Message type must be 0100/0110 or 0200/0210
       * 2) Must be a credit - no pin block (field TLF01_details.pin_block will be null.)
       * 3) TLF01_details.general_status cannot be GS_TXN_TIMEDOUT or GS_TXN_COMPLETED_OFF.
       * 4) Transaction is not a balance inquiry (tx_key != AUTH_BALANCE_INQUIRY_RESPONSE)
       */
      if ( card_family_enabled == true )
      {
                /* Condition 1 */
         if ( ( ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0100") ) ||
                ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0110") ) ||
                ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0200") ) ||
                ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0210") ) 
              ) &&

                /* Condition 2 */
              ( strcmp((const char *)Auth_Tx.TLF01_details.pin_block, "") == 0 ) &&

                /* Condition 3 */
              ( Auth_Tx.TLF01_details.general_status != GS_TXN_TIMEDOUT           &&
                Auth_Tx.TLF01_details.general_status != GS_TXN_COMPLETED_OFF      &&
                Auth_Tx.TLF01_details.general_status != GS_TXN_HOST_NOT_AVAILABLE &&
                Auth_Tx.TLF01_details.general_status != GS_TXN_DROPPED )          &&

                /* Condition 4 */
              ( Auth_Tx.TLF01_details.tx_key != AUTH_BALANCE_INQUIRY_RESPONSE )
	         )
         {
            retval = true;
         }
      }
   }
   else
   {
      /*+--------+
        | ISSUER |
        +--------+*/

      /* Bin Record must show Fraudguard eligibility.
       * If bit is set, bin is NOT eligible for Fraudguard.
       */
      if ( !(Auth_Tx.BIN01_details.mod_check[0] & FGUARD_ON) )
      {
         /* Determine if Fraudguard functionality is enabled for card family. */
         switch( Auth_Tx.BIN01_details.card_family[1] )
         {
            case CARD_FAMILY_VISA:  if ( fg_state & FG_STATE_ISS_VISA )
                                       card_family_enabled = true;
                                    break;

            case CARD_FAMILY_MC:    if ( fg_state & FG_STATE_ISS_MC )
                                       card_family_enabled = true;
                                    break;

            case CARD_FAMILY_JCB:   if ( fg_state & FG_STATE_ISS_JCB )
                                       card_family_enabled = true;
                                    break;

            case CARD_FAMILY_AMEX:  if ( fg_state & FG_STATE_ISS_AMEX )
                                       card_family_enabled = true;
                                    break;
         }

         if ( card_family_enabled )
         {
            /* Filtering requirements:
             * 1) Message type must be 0100/0110 or 0200/0210
             * 2) No system errors
             * 3) Txn is not a void
             * 4) Can be credit or debit
             * 5) Can be a balance inquiry
             * 6) Can be locally or remotely acquired
             * 7) Can be deferred
             */

                   /* Condition 1 */
            if ( ( ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0100") ) ||
                   ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0110") ) ||
                   ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0200") ) ||
                   ( 0 == strcmp(Auth_Tx.TLF01_details.message_type,"0210") ) 
                 ) &&

                   /* Condition 2 */
                 ( Auth_Tx.TLF01_details.general_status != GS_TXN_TIMEDOUT           &&
                   Auth_Tx.TLF01_details.general_status != GS_TXN_COMPLETED_OFF      &&
                   Auth_Tx.TLF01_details.general_status != GS_TXN_HOST_NOT_AVAILABLE &&
                   Auth_Tx.TLF01_details.general_status != GS_TXN_DROPPED )          &&

                   /* Condition 3 */
                 ( Auth_Tx.TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE       &&
                   Auth_Tx.TLF01_details.tx_key != AUTH_VOID_REFUND_RESPONSE     &&
                   Auth_Tx.TLF01_details.tx_key != AUTH_VOID_PAYMENT_RESPONSE    &&
                   Auth_Tx.TLF01_details.tx_key != AUTH_VOID_RELOAD_RESPONSE
                 )
	            )
            {
               retval = true;
            }
         }
      }
   }
   return( retval );
}

/*************************************************************************************
 * NAME:		txservice_Send_Request_To_Falcon_Service
 * DESCRIPTION:	Send AUTH_TX structure to Falcon service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Phani
 * MODIFICATIONS:	
 ************************************************************************************/
BOOLEAN txservice_Send_Request_To_Falcon_Service ( void )
{
   BOOLEAN  retval = true;
   INT      nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   /**
    No Need send Fleet cart transaction to Falcon
    */


   if(true == txservice_Check_If_Fleet_Transaction())
   {
	   return( retval );
   }

   /*Debit transaction should not be send to Falcon as well*/
   if (true == txservice_Check_If_IST_Message())
   {
 		 return( retval );
   }

   /* First, need to make sure Acquirer Id (DE32) is populated.
    * It will not be populated it txn failed velocity checks.
    * In that case, need to get it from NCF01.
    */
   if ( 0x00 == Auth_Tx.TLF01_details.acquiring_id[0] )
   {
      (void)txservice_Get_Network_Acquirer_ID();
   }

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );

   //as of now only RT transactions are supported by falcon controller(falcntl).
/*#ifdef FALCON_CONTROLLER
   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
								 falcntl_que_name,
                                 txcntl_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize, 
                                 0 );
   if (p_msg_out == NULL)
   {
      sprintf( strTemp,"Mem allocate Err build msg for %s", falcntl_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message 
                                      (1,strTemp, "txservice_Send_Request_To_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
      retval = false;
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, falcntl_que_name) ;
      if (lReturnCode != 0)
      {
         sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,falcntl_que_name);

         ReturnCode = txservice_Generate_Usage_Error_Message 
                                      (1,strTemp, "txservice_Send_Request_To_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
         retval = false;
      }
      free( p_msg_out );
   }
#else*/
   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
								 faisnd_que_name,
                                 txcntl_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize,
                                 0 );
   if (p_msg_out == NULL)
   {
      sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_Request_To_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
      retval = false;
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
      if (lReturnCode != 0)
      {
         sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);

         ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_Request_To_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
         retval = false;
      }else
	  {
	/*   sprintf( strTemp,"Succusfully send request to FALCON %s", faisnd_que_name );
	  ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_Request_To_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );*/

      }
      free( p_msg_out );
   }
//#endif

   return( retval );
} /* txservice_Send_Request_To_Service */

/*************************************************************************************
 * NAME:		txservice_Send_EXT10_Request_To_RT_Falcon_Service
 * DESCRIPTION:	Send EXT10 structure to RT Falcon service for declined rc from cadencie/stip
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Phani
 * MODIFICATIONS:
 ************************************************************************************/

BOOLEAN txservice_Send_EXT10_Request_To_RT_Falcon_Service ( void )
{
   BOOLEAN  retval = true;
   INT      nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   /**
    No Need send Fleet cart transaction to Falcon
    */


   if(true == txservice_Check_If_Fleet_Transaction())
   {
	   return( retval );
   }

   /*Debit transaction should not be send to Falcon as well*/
   if (true == txservice_Check_If_IST_Message())
   {
 		 return( retval );
   }

   /* First, need to make sure Acquirer Id (DE32) is populated.
    * It will not be populated it txn failed velocity checks.
    * In that case, need to get it from NCF01.
    */
   if ( 0x00 == Auth_Tx.TLF01_details.acquiring_id[0] )
   {
      (void)txservice_Get_Network_Acquirer_ID();
   }

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );


   p_msg_out = ptemsg_build_msg( MT_AUTH_EXT10_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
								 faisnd_que_name,
                                 txcntl_que_name,
								 (pBYTE) p_buffer,
                                 nBufferSize,
                                 0 );
   if (p_msg_out == NULL)
   {
      sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_EXT10_Request_To_RT_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
      retval = false;
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
      if (lReturnCode != 0)
      {
         sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);

         ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_EXT10_Request_To_RT_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
         retval = false;
      }else
	  {
	/*   sprintf( strTemp,"Succusfully send request to FALCON %s", faisnd_que_name );
	  ReturnCode = txservice_Generate_Usage_Error_Message
                                      (1,strTemp, "txservice_Send_EXT10_Request_To_RT_Falcon_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );*/

      }
      free( p_msg_out );
   }


   return( retval );
} /* txservice_Send_EXT10_Request_To_RT_Falcon_Service */


/*************************************************************************************
 * NAME:		txservice_Send_Request_To_RT_Falcon_Service
 * DESCRIPTION:	Send AUTH_TX structure to RT Falcon service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Phani
 * MODIFICATIONS:
 ************************************************************************************/
BOOLEAN txservice_Send_Request_To_RT_Falcon_Service ( void )
{
	BOOLEAN  retval = true;
	INT      nBufferSize;
	LONG     lReturnCode  = 0L;
	BYTE     ReturnCode   = 0;
	pPTE_MSG p_msg_out    = 0;
	CHAR     strTemp[256] = {0};

	/* First, need to make sure Acquirer Id (DE32) is populated.
	* It will not be populated it txn failed velocity checks.
	* In that case, need to get it from NCF01.
	*/
	if ( 0x00 == Auth_Tx.TLF01_details.acquiring_id[0] )
	{
		(void)txservice_Get_Network_Acquirer_ID();
	}

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX ) );
	nBufferSize = sizeof( AUTH_TX );

#ifdef FALCON_CONTROLLER
	p_msg_out = ptemsg_build_msg( MT_AUTH_RT_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  falcntl_que_name,
								  txcntl_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", falcntl_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message
				(1,strTemp, "txservice_Send_Request_To_RT_Falcon_Service",
						GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
		retval = false;
	}
	else
	{

		lReturnCode = pteipc_send (p_msg_out, falcntl_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,falcntl_que_name);

			ReturnCode = txservice_Generate_Usage_Error_Message
					(1,strTemp, "txservice_Send_Request_To_RT_Falcon_Service",
							GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
			retval = false;
		}
		sprintf(strTemp,"Sent the request to falcon controller ServiceName %s", ServiceName);
		txcntl_log_message( 2, 1,strTemp,"txservice_Send_Request_To_RT_Falcon_Service", 1 );
		free( p_msg_out );
	}
#else
	p_msg_out = ptemsg_build_msg( MT_AUTH_RT_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  faisnd_que_name,
								  txcntl_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message
				(1,strTemp, "txservice_Send_Request_To_RT_Falcon_Service",
						GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
		retval = false;
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
		if (lReturnCode == QUE_OFFLINE)
		{
			sprintf(strTemp,"Err %ld sending msg to %s\n", lReturnCode,faisnd_que_name);
			txcntl_log_message( 1, 3,strTemp,"txservice_Send_Request_To_RT_Falcon_Service", 1 );

			sprintf(strTemp, "%s queue is down\n", faisnd_que_name) ;
			txcntl_log_message( 1, 3,strTemp,"txservice_Send_Request_To_RT_Falcon_Service", 1 );

			//If the txn is an RT and falcon is down, send it to remotehost for authorization
			retval = false;
		}
		else if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);

			ReturnCode = txservice_Generate_Usage_Error_Message
					(1,strTemp, "txservice_Send_Request_To_RT_Falcon_Service",
							GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
			retval = false;
		}
		else
		{
			/*   sprintf( strTemp,"Succusfully send request to FALCON %s", faisnd_que_name );
			ReturnCode = txservice_Generate_Usage_Error_Message
			(1,strTemp, "txservice_Send_Request_To_RT_Falcon_Service",
			GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );*/

		}
		free( p_msg_out );
	}
#endif

	return( retval );
} /* txservice_Send_Request_To_RT_Falcon_Service */



/*************************************************************************************
 * NAME:		txservice_Send_Request_To_FraudGuard_Service
 * DESCRIPTION:	Send AUTH_TX structure to FraudGuard service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Changan Huang
 * MODIFICATIONS:	
 ************************************************************************************/
BOOLEAN txservice_Send_Request_To_FraudGuard_Service ( void )
{
   BOOLEAN  retval = true;
   INT      nBufferSize;
   LONG     lReturnCode  = 0L;
   BYTE     ReturnCode   = 0;
   pPTE_MSG p_msg_out    = 0;
   CHAR     strTemp[256] = {0};

   /* First, need to make sure Acquirer Id (DE32) is populated.
    * It will not be populated it txn failed velocity checks.
    * In that case, need to get it from NCF01.
    */
   if ( 0x00 == Auth_Tx.TLF01_details.acquiring_id[0] )
   {
      (void)txservice_Get_Network_Acquirer_ID();
   }

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX ) );
   nBufferSize = sizeof( AUTH_TX );

   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 ST1_NONE,
                                 ST2_NONE,
                                 fguard_que_name,
                                 txcntl_que_name,
                         (pBYTE) p_buffer,
                                 nBufferSize, 
                                 0 );
   if (p_msg_out == NULL)
   {
      sprintf( strTemp,"Mem allocate Err build msg for %s", fguard_que_name );

		ReturnCode = txservice_Generate_Usage_Error_Message 
                                      (1,strTemp, "txservice_Send_Request_To_FraudGuard_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
      retval = false;
   }
   else
   {
      lReturnCode = pteipc_send (p_msg_out, fguard_que_name) ;
      if (lReturnCode != 0)
      {
         sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,fguard_que_name);

         ReturnCode = txservice_Generate_Usage_Error_Message 
                                      (1,strTemp, "txservice_Send_Request_To_FraudGuard_Service",
                                      GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );
         retval = false;
      }
      free( p_msg_out );
   }
   return( retval );
} /* txservice_Send_Request_To_Service */


/******************************************************************************
 *
 *  NAME:         txservice_Get_Network_Acquirer_ID
 *
 *  DESCRIPTION:  This function looks in the array of NCF01 records to see 
 *                if we have the acquirer Id for this authorizing network.
 *                If yes, use it. If not, go to the database to get it.
 *
 *  INPUTS:       Auth_Tx - (Global) Contains name of authorizing network
 *
 *  OUTPUTS:      Auth_Tx - (Global) Contains Acquirer Id
 *                
 *  RTRN VALUE:   True if Acquirer Id is obtained, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT txservice_Get_Network_Acquirer_ID()
{
   INT   retval = false;
   INT   idx = 0;
   INT   network_exists = false;
   CHAR  network[11] = "";
   CHAR  net_type[2] = "I";
   CHAR  errbuf[100];
   CHAR  Buffer[200];
   NCF01 ncf01;

   /* 1.  Determine name of network. */
   if ( 0x00 != Auth_Tx.CAF01_details.destination_name[0] )
      strcpy( network, Auth_Tx.CAF01_details.destination_name );

   else if ( 0x00 != Auth_Tx.BIN01_details.destination_name[0] )
      strcpy( network, Auth_Tx.BIN01_details.destination_name );

   else if ( 0x00 != Auth_Tx.BIN01_details.alt_destination_name[0] )
      strcpy( network, Auth_Tx.BIN01_details.alt_destination_name );

   if ( 0x00 != network[0] )
   {
      /* 2.  See if we already have Acquirer Id for this network. */
      while( 0x00 != ncf01_recs[idx].primary_key.network_id[0] )
      {
         if ( 0 == strcmp(network, ncf01_recs[idx].primary_key.network_id))
         {
            network_exists = true;
            break;
         }
         idx++;
      }

      if ( network_exists )
      {
         /* Got it!! */
         retval = true;
         strcpy( Auth_Tx.TLF01_details.acquiring_id,
                 ncf01_recs[idx].acquirer_inst_id_code );
      }
      else
      {
         /* 3. Don't have it. Go to database to get it. */
         memset( &ncf01, 0x00, sizeof(NCF01) );
         strcpy( ncf01.primary_key.network_id,   network  );
         strcpy( ncf01.primary_key.network_type, net_type );

         memset( errbuf, 0x00, sizeof(errbuf) );
         retval = TxDataServer_Get_Ncf01_With_Wait( &ncf01, errbuf );
         if ( false == retval )
         {
            memset( Buffer, 0x00, sizeof(Buffer) );
            sprintf( Buffer,
                    "Err select %s,%s from NCF01 for FG: %s",
                     network, net_type, errbuf );
            txcntl_log_message( 1, 3,Buffer,"txservice_Get_Network_Acquirer_ID", 1 );
         }
         else
         {
            /* 4. We successfully got NCF01. Add it to array and Auth_Tx. */
            memcpy( &ncf01_recs[idx], &ncf01, sizeof(NCF01) );
            strcpy( Auth_Tx.TLF01_details.acquiring_id,
                    ncf01.acquirer_inst_id_code );
         }
      }
   }
   return( retval );
}


/******************************************************************************
 *
 *  NAME:         txservice_Set_Last_Txn_Date_To_Today
 *
 *  DESCRIPTION:  This function updates either the Peso or Dollar julian
 *                date in CCF03P (ONUS Velocity) to today.
 *
 *  INPUTS:       Auth_Tx - (Global) Contains txn currency code
 *
 *  OUTPUTS:      Auth_Tx - (Global) Contains CCF03P data structure
 *                private_ccf03p_struct - (Global) Contains CCF03P Julian date. 
 *                
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void txservice_Set_Last_Txn_Date_To_Today()
{
   BYTE  strTemp[50] = {0};
   CHAR  strSystemDateTime[50] = {0};
   BYTE  strCurrentDate[9] = {0};
   LONG  CurrentDate  = 0L;

   /* Get Current Date in YYYYMMDD */
	genutil_format_date( (pCHAR)strSystemDateTime );
	memcpy( strCurrentDate, strSystemDateTime, 8 );

	/* Put it in Julian Day format, or number of days since Jan 1, 1970. */
   CurrentDate = Txutils_Calculate_Julian_Date( strCurrentDate );

   /* Update private structure that later updates CCF03P. */
   ltoa( CurrentDate, strTemp, 10 );
   if ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) )
   {
      strcpy( Auth_Tx.CCF03P_details.peso_julian_date, strTemp );
      private_ccf03p_struct.peso_julian_date = CurrentDate;
   }
   else if ( strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) )
   {
      strcpy( Auth_Tx.CCF03P_details.dollar_julian_date, strTemp );
      private_ccf03p_struct.dollar_julian_date = CurrentDate ;
   }
   return;
}

/*******************************************************************************
 * NAME:        txservice_Send_To_Monitor
 *
 * DESCRIPTION: Send any PTE message to Monitor
 *
 * INPUTS:      mtype - PTE Message Type
 *              st1   - Subtype 1
 *              st2   - Subtype 2
 *              msg   - Pointer to message to be sent
 *              len   - Length of message to be sent
 *
 * OUTPUTS:     None
 *
 * RETURNS:     true   if forwarded; false  otherwise
 *
 * AUTHOR:      D. Irby
 ******************************************************************************/
BOOLEAN txservice_Send_To_Monitor( BYTE mtype, BYTE st1, BYTE st2,
                                  pBYTE msg,   INT  len )
{
   BOOLEAN  retval = true;
   pPTE_MSG p_msg = 0;
   CHAR     ConnectionId[MAX_ORIG_INFO_SIZE+1] = "";
   CHAR     err_mesg[200] = "";
   CHAR     Buff[256];
   LONG     ret_code;
   XINT     ReturnCode = 0;


   p_msg = ptemsg_build_msg( mtype,
                             st1, 
                             st2,
                             applnk_que_name, 
                             application_que_name,
                             msg,
                             len, 
                             0 );

   if ( p_msg != NULL_PTR )
   {
      memset( ConnectionId, 0x00, sizeof(ConnectionId) );
      ReturnCode = ReadMemMonitorTable( ConnectionId );
      if ( ConnectionId[0] != 0 )
      {
         ptemsg_set_orig_comms_info( p_msg, ConnectionId );
         ret_code = pteipc_send( p_msg, applnk_que_name );
         if ( 0 > ret_code )
         {
            pteipc_get_errormsg( ret_code, err_mesg );
            sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     applnk_que_name, err_mesg );
            txcntl_log_message( 1, 3,Buff,"txservice_Send_To_Monitor", 1 );
            retval = false;
         }
      }
      free( p_msg );
   }
   else
   {
      retval = false;
      txcntl_log_message( 1, 3,"Insufficient Memory to build response to Monitor",
    		  	  	  	  	  "txservice_Send_To_Monitor", 1 );
   }
   return( retval );
}



/*******************************************************************************
 * NAME:        txservice_Perform_OMD_GlobalParamterBypass
 *
 * DESCRIPTION: This function is called for a merchant when OMD functinality are provided for it
 *				and Global Parameter Capping Bypass flag enable for it. This Function update the
 * 				private_ccf03p_struct struncture memeber based on Currency type and alos check
 *				MCC capping
 *
 * INPUTS:      BYTE MessageSubType2
 *			 		ONUS/OFFUS
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     true   if forwarded; false  otherwise
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

BYTE txservice_Perform_OMD_GlobalParamterBypass(BYTE MessageSubType2)
{
	double nConvertedTxAmount = 0.0;
	int CurrencyType = 0;
	BYTE ReturnCode = NO_AUTH_ERROR;
	BYTE strTemp[1000] = {0};

	// Set OMD flag 
	strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"OMD");
	
	// Get the Transaction amount
	nConvertedTxAmount = (strtod (Auth_Tx.TLF01_details.product_codes[15].amount,    0) / 100.0);
	
	// Clearing voice authorisation test as error is overriden 
	strcpy(Auth_Tx.TLF01_details.voice_auth_text,"");

	if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
	{
		CurrencyType = PESO ;
		
		//Since it is overriden the counts are incremented
		private_ccf03p_struct.peso_usage[0].nbr_usage += 1 ;

        /* Cap private_ccf03p_struct.peso_usage[0].nbr_usage to 999 */
        if (private_ccf03p_struct.peso_usage[0].nbr_usage > 999)
            private_ccf03p_struct.peso_usage[0].nbr_usage = 999 ;

        private_ccf03p_struct.peso_usage[0].amt_usage += nConvertedTxAmount ;

        if (private_ccf03p_struct.peso_usage[0].amt_usage > 9999999999.99) 
            private_ccf03p_struct.peso_usage[0].amt_usage = 9999999999.99 ;
	}
	else if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
	{
		CurrencyType = DOLLAR ;
		
		// Since it is overriden the counts are incremented
		private_ccf03p_struct.dollar_usage[0].nbr_usage += 1 ;

		/* Cap private_ccf03p_struct.dollar_usage[0].nbr_usage to 999 */
		if (private_ccf03p_struct.dollar_usage[0].nbr_usage > 999)
		{
			private_ccf03p_struct.dollar_usage[0].nbr_usage = 999 ;

		}
		
		private_ccf03p_struct.dollar_usage[0].amt_usage += nConvertedTxAmount ;

		if (private_ccf03p_struct.dollar_usage[0].amt_usage > 9999999999.99)
		{
			  private_ccf03p_struct.dollar_usage[0].amt_usage = 9999999999.99 ;
		}
	}

	// Check if the card has been used at the same merchant more than 
	// <MCF01.pan_same> times in a 24 hour period
	// The merchant usage bucket has to be cleared on a new transaction day

   if (Auth_Tx.CAF01_details.merchant_same[0] != 0)
   {
      ReturnCode = TxAuthorize_Validate_Merchant_Repeat_Checks () ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
   }

	// Check for MCC capping
	ReturnCode = TxAuthorize_Validate_Category_Count_Checks(CurrencyType);
	if (ReturnCode != NO_AUTH_ERROR)
	{
		if (Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
		{
			if( false == TxDataServer_Get_OMC01_Details(MessageSubType2))
			{
				ReturnCode = SYSTEM_ERROR ;
			}
			else
			{
				return NO_AUTH_ERROR;
			}
		}
		else
		{
			return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}
	}
	else
	{
		/* Determine if transaction qualifies for Cash Bonus
		* Program prior to sending it out to a remote host.
		*/
		// Need to add a check to see for ONUSE and OFF US card.
		if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[6].code,"OFFUS"))
		{
			strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
												
			if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
			{
				/* It does qualify; set the flag for updatds. */
				ReturnCode = TxAuthorize_Update_AuthTx_Structures( 0, 1, 0 );
			}

			ReturnCode = TxAuthorize_Update_Offus_Velocity_Structure ();
			if (ReturnCode != NO_AUTH_ERROR)
			{
				return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
			}
		}

		ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
					  (strcat(Auth_Tx.CAF01_details.destination_name, "A"), 
							MT_AUTH_REQUEST, 
							0, 
							0) ;
		if (ReturnCode == false)
		{
			sprintf( strTemp,
					  "%s down,Tx sent to %s\n",
					   Auth_Tx.BIN01_details.destination_name,
					   Auth_Tx.TLF01_details.handler_queue) ;

			 if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
			 {
				  strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
			 }

			 txcntl_log_message( 1, 3,strTemp,"txservice_Perform_OMD_GlobalParamterBypass", 1 );
			 memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;
			 txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue,
													   MT_AUTH_RESPONSE, 
													   0, 
													   0) ;
			  return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}
		else
		{
			return NO_AUTH_ERROR;
		}
	}
}


/*******************************************************************************
 * NAME:        txservice_Perform_OMD_MCCBypass
 *
 * DESCRIPTION: This function is called for a merchant when OMD functinality are provided for it
 *				and MCC Capping Bypass flag enable for it. 
 *
 * INPUTS:      BYTE MessageSubType2
 *			 		ONUS/OFFUS
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     true   if forwarded; false  otherwise
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/


BYTE txservice_Perform_OMD_MCCBypass(void)
{
	int CurrencyType = 0;
	BYTE ReturnCode = NO_AUTH_ERROR;
	BYTE strTemp[1000] = {0};

	// Set OMD flag 
	strcpy(Auth_Tx.TLF01_details.product_codes[18].code,"OMD");

	// Clearing voice authorisation test as error is overriden 
	strcpy(Auth_Tx.TLF01_details.voice_auth_text,"");

	/* Determine if transaction qualifies for Cash Bonus
	* Program prior to sending it out to a remote host.
	*/
	// Need to add a check to see for ONUSE and OFF US card.
	if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[6].code,"OFFUS"))
	{
		strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
											
		if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
		{
			/* It does qualify; set the flag for updatds. */
			ReturnCode = TxAuthorize_Update_AuthTx_Structures( 0, 1, 0 );
		}

		ReturnCode = TxAuthorize_Update_Offus_Velocity_Structure ();
		if (ReturnCode != NO_AUTH_ERROR)
		{
			return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}
	}

	ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
				  (strcat(Auth_Tx.CAF01_details.destination_name, "A"), 
						MT_AUTH_REQUEST, 
						0, 
						0) ;
	if (ReturnCode == false)
	{
		sprintf( strTemp,
				  "%s down,Tx sent to %s\n",
				   Auth_Tx.BIN01_details.destination_name,
				   Auth_Tx.TLF01_details.handler_queue) ;

		 if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
		 {
			  strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;
		 }

		 txcntl_log_message( 1, 3,strTemp,"txservice_Perform_OMD_MCCBypass", 1 );
		 memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;
		 txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue,
												   MT_AUTH_RESPONSE, 
												   0, 
												   0) ;
		  return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}
	else
	{
		return NO_AUTH_ERROR;
	}
}

INT txservice_check_CUP_emv_capability_device(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.terminal_type ,
					  CUP_EMV_CAPABILITY_INDICATOR,CUP_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_check_DINERS_emv_capability_device(void)
{
	/*BDO have discarded this functionality so always return fasle*/
	return false;
	if( 0 == strncmp(Auth_Tx.TLF01_details.terminal_type ,
					 DINERS_EMV_CAPABILITY_INDICATOR,DINERS_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_check_is_Diners_Magstrip_pos_transaction(void)
{
	if(  (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP2,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP3,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)) ||
		 (0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
						 DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP4,
						 DINERS_BDO_POS_ENTRY_MODE_LEN)))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_is_Diners_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_is_Diners_Fallback_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_is_Diners_Contactless_Mastripe_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_is_Diners_Contactless_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_is_voice_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   voice_handler_que_name,
					   DINERS_VOICE_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}
BOOLEAN txservice_check_is_Diners_Manual_pos_transaction(void)
{
	if( 0 ==  strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_MANUAL,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_is_Diners_Asiapay_Ecommerce_transaction (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_check_is_Diners_RPS_Transaction(void)
{
	if( 0 == strncmp(Auth_Tx.industry_code, DINERS_RPS_INDUSTRY_CODE,
					 DINERS_RPS_INDUSTRY_CODE_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_check_is_Diners_Asiapay_Recurring (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_check_is_Diners_BDO_PAY (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_BDO_PAY_TRANSACTION,
					 DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_check_is_voice_transaction(void)
{
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,voice_handler_que_name,VOICE_HANDLE_QUEUE_NAME) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*******************************************************************************
 * NAME:        txservice_Prepare_Pos_Data
 *
 * DESCRIPTION: This function identify function that needs to be called 
 *                         to prepare the POS Data
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_Pos_Data(void)
{

	if(true == txservice_Check_If_AMEX_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_Amex();
		}
		/* Offline sale correct POS DATA for displaying in tranlookup-Leela */
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_Amex_OfflineSale();
		}
	}

	else if(true == txservice_Check_If_JCB_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_JCB();
		}
		/* Offline sale correct POS DATA for displaying in tranlookup-Leela */
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_JCB_OfflineSale();
		}
	}

	else if(true == txservice_Check_If_VISA_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_VISA();
		}
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_VISA_OfflineSale();
		}
	}

	else if(true == txservice_Check_If_MC_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_MC();
		}
		/* Offline sale correct POS DATA for displaying in tranlookup-Leela */
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_MC_OfflineSale();
		}
	}

	else if(true == txservice_Check_If_CUP_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_CUP();
		}
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_CUP_OfflineSale();
		}
	}

	else if (true == txservice_Check_If_Diners_Transaction())
	{
		if((0 == strlen(Auth_Tx.TLF01_details.product_codes[16].amount)))
		{
			txservice_Prepare_posdata_for_DINERS();
		}
		if( ((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0))&&
			(0 == strlen(Auth_Tx.TLF01_details.product_codes[5].amount)))
		{
			txservice_Prepare_posdata_for_DINERS_OfflineSale();
		}
	}

}
/*Offline Sale POS Data for displaying in tranlookup- Leela*/
void txservice_Prepare_posdata_for_Amex_OfflineSale(void )

{
	/*Offline sale-Manual Key PEM*/
	if(((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))&&
		((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
	   ( (0 == strncmp("001",Auth_Tx.TLF01_details.product_codes[14].code,3)) &&
		 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
    {/*Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
			should be PEM-51's*/
	   /*Magstipre*/
	   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101254041",12 );
	   }
	   /*chip contact*/
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101554041",12 );
	   }
	   /*contactless*/
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "56010X554041",12 );
	   }
	   /*fallback*/
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101954041",12 );
	   }
	   /*contactless magstripe*/
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "56010X254041",12 );
	   }
	   /*Manual Keyin*/
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560110664041",12 );
	   }
   }
}
/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_Amex
 *
 * DESCRIPTION: This function prepare POS data for AMEX deurring STIP
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_posdata_for_Amex(void )

{
	INT	 ret_val = true;
	CHAR  temp_entry_mode[4] = {0};
	memset(Auth_Tx.TLF01_details.product_codes[16].amount,0x30,12);

	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode) > 3)
	{
		strncpy(temp_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],3);
	}
	else
	{
		strcpy(temp_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode);
	}

	/*Manual Key PEM and Original PEM with Sale Completion POS Data-Leela*/
	/*if(((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))&&
		(strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
	   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
		 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
    {Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
			should be PEM-51's
	   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101254041",12 );
	   }
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101554041",12 );
	   }
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "56010X554041",12 );
	   }
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560101954041",12 );
	   }
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "56010X254041",12 );
	   }
	   Manual Keyin
	   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
	   {
		   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "560110664041",12 );
	   }
   }*/

#if BDOR_68_REQ_1_AMEX    /* MobilePOS-NFC Terminal for Tap2Phone Transactions */
	if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin())
	{
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
				AMEX_POS_DATA_LEN );
	}
	else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin())
	{
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				AMEX_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
				AMEX_POS_DATA_LEN );
	}
#endif
#if BDOR_68_REQ_3_AMEX /* Swiftpass BDO Pay Credit Transactions */
   else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
   {
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
				AMEX_POS_DATA_LEN );
   }
   else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
   {
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
				AMEX_POS_DATA_LEN );
   }
#endif
#if BDOR_68_REQ_5_AMEX /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

   else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
   {
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
			    AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
			    AMEX_POS_DATA_LEN );
   }
#endif
	else if(strncmp(temp_entry_mode,"02",2) == 0 ||
	   strncmp(temp_entry_mode,"06",2) == 0 ||
	   strncmp(temp_entry_mode,"08",2) == 0 ||
	   strncmp(temp_entry_mode,"90",2) == 0 )
	{
		/*Mag stripe*/
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_MAGSTRIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"05",2) == 0 )
	{
		/* EMV - Contact*/
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_CHIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_CHIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"80",2) == 0 )
	{	/* POS 80X */
		if(temp_entry_mode[2] == '1')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_FALLBACK_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_FALLBACK_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
		}
	}

	else if( strncmp(temp_entry_mode,"01",2) == 0 )
	{	/*Manual - VOICE- ECOM  - Recurring - RPS*/
			/*VOICE*/
		if(Auth_Tx.TLF01_details.voice_auth[0] == 'Y')
		{/* AMEX01-2024 Mandate BR, previous POS data: 600001S00100 changed POS data:660431S00130 */
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"660430600130",AMEX_POS_DATA_LEN);

			/* For position seven, need to know if 4DBC/4CSC was entered.
			* Security control information is coming in
			* auth_Tx.tlf01_details.interest_rate.
			*/
			if ( Auth_Tx.TLF01_details.interest_rate[0] == 0x00 )
			{
				/* It was not entered, so change 'S' to '6'. */
				Auth_Tx.TLF01_details.product_codes[16].amount[6] = '6';
			}
		}

		else if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0)
		{
			/* For E-commerce tx*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				   AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA,AMEX_POS_DATA_LEN);
			/*if(Auth_Tx.TLF01_details.interest_rate[0] != 0x00 )
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"100SS0S00100",AMEX_POS_DATA_LEN);
			}*/
		}
		else if(0 == strcmp(Auth_Tx.industry_code,"800") &&
				(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
				)
		{
			/*RPS*/
			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					AMEX_RPS_ASIAPAY_RPS_POS_DATA,AMEX_POS_DATA_LEN);
		}
		else if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2))
		{
			//BDO PAY
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					AMEX_BDO_PAY_POS_DATA,AMEX_POS_DATA_LEN);
		}
		else
		{
			/*Manual entry*/
			if(temp_entry_mode[2] == '1')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
			}
			else
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_POS_MANUAL_KEYIN_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
			}
		}
	}

	else if( strncmp(temp_entry_mode,"07",2) == 0)
	{
		 /*  07X with out POS, Special case */
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND)
		{
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"660101654140",AMEX_POS_DATA_LEN);
		}
		else if((strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0)
				&& (0 == strcmp(Auth_Tx.industry_code,"800")) )
		{
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"600090S04100",AMEX_POS_DATA_LEN);
		}
		else
		{
			if(temp_entry_mode[2] == '1')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,AMEX_POS_DATA_LEN);
			}
			else
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					   AMEX_CONTACTLESS_CHIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
			}
		}
	}
	else if(strncmp(temp_entry_mode,"91",2) == 0)
	{
		/*Special case for COntact less */
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
			   AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN,AMEX_POS_DATA_LEN);
	}
	else
	{	 /* POS 00X */
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"010110054140",AMEX_POS_DATA_LEN);
	}
}

/*Sale Completion POS Data for displaying in tranlookup-Leela*/
void txservice_Prepare_posdata_for_CUP_OfflineSale(void)
{
    if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
	   if(((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
			   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
				 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
	   {
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "05000300000",11 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "06000300000",11 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "05200300000",11 );
		   }
		   //Tap2phone
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0831",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "06002300000",11 );
		   }
	   }
	}
}
/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_CUP
 *
 * DESCRIPTION: This function prepare POS data for CUP deurring STIP
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_posdata_for_CUP(void)
{
	/*
		Refer build_request_field_60 in nccup for more info
	*/
	INT	ret_val = true;
	INT	len = 0;
	INT	len_60_3 = 0;
	CHAR temp_len[4] = {0};
	BYTE temp_pos[4] = {0};
	INT temp_posentry = 0;
	CHAR HOSTMESSAGE_posentry[4] = {0};
	CHAR HOSTMESSAGE_card_seq_num[4] = {0};
	CHAR    temp_str[4] = {0};
	CHAR    temp_str1[4] = {0};
	INT     itemp;
	CHAR  team_field_23[4] = {0};
	INT    temp_len_field_23 = 0;

    if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))
	{
		
	}
	else 
	{
		/*Prepare feild 60.2.1*/
		if((true == txservice_Check_If_CUP_RPS_TRN()) ||
		   (true == txservice_Check_If_CUP_MOTO_TRN()))
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,"9",1);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount ,"0",1);
		}
		/*Prepare PAN Scequence Number*/
		if (strlen(Auth_Tx.host2_message_seq_nbr) > 0)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy(temp_str,Auth_Tx.host2_message_seq_nbr+1,CUP_DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num,temp_str1,CUP_DE23_SIZE );
		}
		else if (strlen(Auth_Tx.EMV_details.pan_sequence_number)> 0)
		{
			  memcpy(temp_str,Auth_Tx.EMV_details.pan_sequence_number,CUP_DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num, temp_str1,CUP_DE23_SIZE );
		}
		
		/*Prepare POS ENTRY MODE */
		strncpy(HOSTMESSAGE_posentry,Auth_Tx.TLF01_details.pos_entry_mode +1 ,CUP_DE22_SIZE);
		strncpy(temp_pos,HOSTMESSAGE_posentry,2);
		temp_posentry=atoi(temp_pos);

		switch(temp_posentry)
		{
			case 02:
			case 90:
			case 06:
				/*Prepare feild 60.2.2 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount+ 1,"5",1);
				/*Prepare feild 60.2.3 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 05:
			case 95:
			case 97:
			case 80:
				if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
				{
					/*Prepare feild 60.2.2 */
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
					/*Prepare feild 60.2.3 */
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
				else
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					/*Prepare feild 60.2.3 */
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
			break;

			case 07:
			case 91:
			case 96:
			case 98:
				/*Prepare feild 60.2.2 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"6",1);
				/*Prepare feild 60.2.3 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 01:
				/*Prepare feild 60.2.2 */
				if((0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)) ||
				   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)))
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				}
				else if(true == txservice_Check_If_CUP_MOTO_TRN())
				{
					if( 0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
									   voice_handler_que_name,
									   CUP_VOICE_MODULE_COMP_LEN) )
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1 ,"0",1);
					}
					else
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					}
				}
				else if (true == txservice_Check_If_CUP_RPS_TRN()||
						(true == txservice_Check_If_CUP_BDO_PAY()))
				{
					memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				}
				else
				{
					if (0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
										   voice_handler_que_name,
										   CUP_VOICE_MODULE_COMP_LEN))
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
					}
					else
					{
						memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					}
				}
				/*Prepare feild 60.2.3 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			default:
				/*Prepare feild 60.2.2 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				/*Prepare feild 60.2.3 */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;
		}

		/*Prepare feild 60.2.4 Reserved filled in with 0*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 3,"0",1);

		/*Prepare feild 60.2.2 for Void and Reversal for Magstip Trasaction */
		if( temp_posentry == 05 ||
			temp_posentry == 12 ||
			temp_posentry == 01 ||
			temp_posentry == 80 || 
			temp_posentry == 95 || 
			temp_posentry == 97 ||
			temp_posentry == 07 || 
			temp_posentry == 91 ||
			temp_posentry == 96 ||
			temp_posentry == 98)
			{
				/* Do nothing Its EMV Trasaction */
			}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE
			)
		{
			/* Its Magstrip Transaction */
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"2",1);
		}

		/* Prepare field 60.2.3 for Reversal and Fallback reversal */
		if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE )
		{
			if(Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
			{
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"2",1);
			}
			else
			{ 
				/* Normal  Reversal */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			}
		}
		/*Prepare filed 60.2.3 for POS EMV fallback transaction */
		else if( Auth_Tx.TLF01_details.pos_entry_mode[1]== '8')
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 1,"5",1);
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 2,"2",1);
		}

		/*Prepare feild 60.2.5 Transaction Channel Type*/
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)) 
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				/*Mobile phone Voice transaction */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);
			}
			else if(true == txservice_Check_If_CUP_MOTO_TRN())
			{
				/*MOTO Voice transaction */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);
			}
			else
			{
				/*PC Voice transaction */
				memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);
			}
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}
		else if (true == txservice_Check_If_CUP_RPS_TRN())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"14", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}
		else if (true == txservice_Check_If_CUP_BDO_PAY())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"08", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}
		else
		{
			/*POS-Point Of Sale 03 */
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 4,"03", 2);
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}

		/*Prepare feild 60.2.6 Signature-based Credit Card Network Identifier*/
		if(true == txservice_Check_If_CUP_MOTO_TRN())
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 6,"1", 1);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 6,"0", 1);
		}
		/*Prepare feild 60.2.7 Chip Card Authentication Reliability Indicato*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 7,"0", 1);
		/*Prepare feild 60.2.8	Electronic Commerce Identification*/
		memcpy(Auth_Tx.TLF01_details.product_codes[16].amount + 8,"00", 2);

#if BDOR_68_REQ_1_UPI /*MobilePOS NFC Tap2Phone Trx*/
		if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				   UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
				   UPI_POS_DATA_LEN);
		}
		else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				   UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
				   UPI_POS_DATA_LEN);
		}
#endif
#if BDOR_68_REQ_3_UPI /* Swiftpass BDO Pay Credit Transactions */
	   else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
	   {
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  UPI_POS_DATA_LEN);
	   }
	   else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
	   {
		   memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
				  UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  UPI_POS_DATA_LEN);
	   }
#endif
#if BDOR_68_REQ_5_UPI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

		else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
		{
			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					 UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					 UPI_POS_DATA_LEN);
	   }
#endif
	}

/*    if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
    	Manual Key PEM and Original PEM with Sale Completion POS Data-Leela
	   if((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
			   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
				 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
	   {Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
				should be PEM-51's
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "05000300000",11 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "06000300000",11 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "05200300000",11 );
		   }
		   Tap2phone
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0831",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "06002300000",11 );
		   }
	   }
	}*/
}

/*Sale completion POS Data for displaying in tranlookup-Leela*/
void txservice_Prepare_posdata_for_VISA_OfflineSale(void)
{
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
	   /*Manual Key PEM and Original PEM with Sale Completion POS Data-Leela*/
	   if(((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
			   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
				 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
	   {
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "050000100010",12 );
		   }
		   /*Chip fallback*/
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "052000000010",12 );
		   }
		   /*Contactless Magstripe*/
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "912000100010",12 );
		   }
	   }
	}
}

/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_VISA
 *
 * DESCRIPTION: This function prepare POS data for CUP deurring STIP
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_posdata_for_VISA(void)
{
	CHAR   	tempstr [256] = {0};
	BYTE 	emv_chip_code;
	INT  	pos_entry_type = POS_ENTRY_NORMAL;
	CHAR   	temp_str[100] = {0};
	BYTE 	eCommerce_Indcator[3] ={0} ;
	INT    	RPS_flag = 0;
	INT    	iIndustry;
	INT  	emv_txn;
	INT  	emv_len;
	CHAR    temp_len[5] = {0};
	CHAR    error_str[100] = {0};

	strncpy(eCommerce_Indcator,Auth_Tx.TLF01_details.profile,2);
	iIndustry = atoi( Auth_Tx.industry_code );
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
	   /* Determine if txn is EMV (Smart Card). */
	   if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
			(Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
		  emv_txn = false;
	   else
	   {
		  emv_txn = true;
		  if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0)
			  emv_len = Auth_Tx.EMV_details.emv_block[0];
			  else
			  emv_len = genutil_bcd_to_int(Auth_Tx.EMV_details.emv_block, 2 );
	   }

		/* Set the POS Entry Mode flag for use in other fields. */
		  if ( emv_txn == true )
			 pos_entry_type = POS_ENTRY_EMV_CHIP;
	   else if( 0 == strncmp("0802",Auth_Tx.TLF01_details.pos_entry_mode,4) || (0 == strncmp("0801",Auth_Tx.TLF01_details.pos_entry_mode,4) ))
			  pos_entry_type = POS_ENTRY_FALLBACK;
		  else if ( 0 == strncmp("080",Auth_Tx.TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_MAGSTRIPE;

		  else if ( 0 == strncmp("008",Auth_Tx.TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_FALLBACK;

		  /* FIELD 60: ADDITIONAL POS INFORMATION */
		 memset( tempstr, 0x00, sizeof(tempstr) );
		 memcpy(tempstr,Auth_Tx.TLF01_details.nfi_name,sizeof(Auth_Tx.TLF01_details.nfi_name));
		 if(!(txservice_IsFieldNull(Auth_Tx.TLF01_details.nfi_name)))
		 {
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,Auth_Tx.TLF01_details.nfi_name,
				   (sizeof(Auth_Tx.TLF01_details.nfi_name)-1));
		 }
		 else
		 {
		  if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
		  {
			 /* Determine chip condition code (if relevant) for DE60.3 */
			 (void)txservice_Parse_track2(&Auth_Tx,error_str); /* Gets service code */
			 if ( (ServiceCode[0] == '2') || (ServiceCode[0] == '6') )
			 {
				if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
				{
				   emv_chip_code = Auth_Tx.EMV_details.chip_condition_code[0];
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
				{
				   emv_chip_code = '2';
				   Auth_Tx.EMV_details.chip_condition_code[0] = '2';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE )
				{
				   emv_chip_code = Auth_Tx.EMV_details.chip_condition_code[0];
				}
				else
				{
				   emv_chip_code = '0';
				}

				/* Make sure chip code is not 0 for Service Code 2 or 6. */
				if ( (emv_chip_code != '1') && (emv_chip_code != '2') )
				   emv_chip_code = '1';

				if ( Auth_Tx.EMV_details.chip_condition_code[0] == 0x00 )
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
			 }
			 else
			 {
				emv_chip_code = '0';  /* Always for Service Code not 2 or 6 */
				if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '1';
				}
				else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '2';
				}
				else if ( pos_entry_type != POS_ENTRY_EMV_MAGSTRIPE )
				{
				   Auth_Tx.EMV_details.chip_condition_code[0] = '0';
				}
			 }
		  }

		   if ( iIndustry == ECOMMERCE || (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC,2)))  /* Verified by visa VbV */
		   {
			 memset( tempstr, 0x00, sizeof(tempstr) );
			strncpy( tempstr, "500000000040",12 );

			 if ( eCommerce_Indcator[0] == 0x00 )
			 {
				if ( Auth_Tx.TLF01_details.message_type[1] == '4' )
				{
				   /* This is a reversal. Need to echo ECI.
					* It gets stored in settlement date (TLF01);
					* It is stored in BCH10 in product code 20.
					*/
				   strcpy(eCommerce_Indcator,Auth_Tx.TLF01_details.settlement_date);
				}
				else
				{
				   /* Not a reversal. Use default value. */
				   strcpy( eCommerce_Indcator, "07" );
				}
			 }

			 strncpy( tempstr+8, eCommerce_Indcator,2);

			//strncat( tempstr, eCommerce_Indcator, 2 );

			 /* Save ECI for reversals. */
			 strcpy( Auth_Tx.TLF01_details.settlement_date, eCommerce_Indcator );

			 if ( Auth_Tx.TLF01_details.terminal_type[1] == '7' )
				tempstr[1] = '5'; /* Terminal has Chip Read capability. */

			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,tempstr,
					strlen(tempstr));

		   }
		  else if ( emv_txn == true || ( 0 == strncmp("005",Auth_Tx.TLF01_details.pos_entry_mode,3) )
				  || ( 0 == strncmp("007",Auth_Tx.TLF01_details.pos_entry_mode,3) ))
		  {
			 memset( tempstr, 0x00, sizeof(tempstr) );
			  if( 0 == strncmp("0912",Auth_Tx.TLF01_details.pos_entry_mode,4))
			  {
				  strncpy( tempstr, "912000100010" ,12);
			  }
			  else if ((pos_entry_type == POS_ENTRY_EMV_CHIP) ||
					   (0 == strncmp("007",Auth_Tx.TLF01_details.pos_entry_mode,3))||
				       (0 == strncmp("005",Auth_Tx.TLF01_details.pos_entry_mode,3)))
			  {

				 strncpy( tempstr, "050000100010" ,12);
			  }

			 /* 60.3 = Chip Condition Code */
			 if ( emv_chip_code == 0x00 )
			 {
				/* EB wants this field to default to zero. */
				emv_chip_code = '0';
				Auth_Tx.EMV_details.chip_condition_code[0] = '0';
			 }

			 memcpy(Auth_Tx.TLF01_details.product_codes[16].amount,tempstr,
			sizeof(Auth_Tx.TLF01_details.product_codes[16].amount)-1);

		  }

	   /*Manual Key PEM and Original PEM with Sale Completion POS Data-Leela*/
		   /*else if((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
				   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
					 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
		   {Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
					should be PEM-51's
			   when PEM-Mag,Chip Contact, Contactless,MK
			   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
				   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
				   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
				   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
			   {
				   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "050000100010",12 );
			   }
			   Chip fallback
			   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
			   {
				   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "052000000010",12 );
			   }
			   Contactless Magstripe
			   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0))
			   {
				   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "912000100010",12 );
			   }
		   }*/
		  else if ('Y' != Auth_Tx.TLF01_details.voice_auth[0])
		  {
			 if (0 == strcmp(Auth_Tx.TLF01_details.terminal_type,"01"))
			 {
				/* Unattended gas pump */
				 Auth_Tx.TLF01_details.product_codes[16].amount[0] = '3';
			 }
			 else
			 {
				/* Unspecified */
				 Auth_Tx.TLF01_details.product_codes[16].amount[0] = '0';
			 }

			 /* Ignore the following for Ecommerce. */
			 if ( iIndustry != ECOMMERCE )
			 {
				if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0012",4) &&
					 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)) //RPS
				{
					RPS_flag = 1;
				   strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,"010000000000",12);
				}
				else if (Auth_Tx.TLF01_details.pos_entry_mode[2] == '2')
				{
					if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))
					{
					 /* EMV capability */
						strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,"050000100010",12);  ///code change same as ncvisa
					}
				}

				else if ((Auth_Tx.TLF01_details.pos_entry_mode[2] == '1') &&  (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2)))
				{
					if (Auth_Tx.TLF01_details.pos_entry_mode[1] == '9')
				   /* Magnetic stripe read capability */
						strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "912000100010", 12 );
					else
						/* Manual key in */
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "050000100010", 12 );
				}
#if BDOR_68_REQ_1_VISA
				else if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin()))
				{
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
							 VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
							 VISA_POS_DATA_LEN );
				}
				else if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin()))
				{
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
							 VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
							 VISA_POS_DATA_LEN );
				}
#endif

				else if ( pos_entry_type != POS_ENTRY_NORMAL )
				{
					Auth_Tx.TLF01_details.product_codes[16].amount[1] = '5';
					Auth_Tx.TLF01_details.product_codes[16].amount[2] = emv_chip_code;
				}

				else if((Auth_Tx.TLF01_details.pos_entry_mode[2] == '1') &&
				(0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2)))
				{
							   /* BDO PAY QR*/
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "010000000000", 12 );
				}
				else
				{
				   /* No magnetic stripe read capability */
					Auth_Tx.TLF01_details.product_codes[16].amount[1] = '1';
				}

				if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK ||
					 pos_entry_type == POS_ENTRY_FALLBACK ||
					 pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE)
				 {
					strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "052000000010", 12 );
					}

				 }
			 }
#if BDOR_68_REQ_3_VISA /*Swiftpass BDO Pay Credit Transactions*/

			if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
			{
				strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
						 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
						 VISA_POS_DATA_LEN );
			}
			else if (true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
			{
				strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
						 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
						 VISA_POS_DATA_LEN );
			}
#endif
#if BDOR_68_REQ_5_VISA /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

			else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
			{
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
						VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						VISA_POS_DATA_LEN );
		   }
#endif
		   	 /* Oct Mandate changes 2021 */
		  	 else if ('Y' == Auth_Tx.TLF01_details.voice_auth[0])
		 	 {
		 		strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "010000000000", 12 );
		 	 }


			 if ( Auth_Tx.TLF01_details.terminal_type[1] == '7' && RPS_flag == 0)
			 {
				/* Terminal has Chip Read capability. */
				 Auth_Tx.TLF01_details.product_codes[16].amount[1] = '5';
			 }

			 /* Store here for Fraudguard. */
			 memcpy( Auth_Tx.TLF01_details.nfi_name,
					 Auth_Tx.TLF01_details.product_codes[16].amount,
			 					 sizeof(Auth_Tx.TLF01_details.nfi_name)-1 );
		  }
		}
	}


void txservice_Prepare_posdata_for_MC_OfflineSale(void)
{
   CHAR    temp_str3[4] = {0};
   INT     itemp;

   memset( temp_str3, 0x00, sizeof(temp_str3) );

   if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
   	  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
   {
	   /* Its Acq transaction */
	   memcpy( temp_str3, Auth_Tx.TLF01_details.pos_entry_mode +1, 3 );

	   /*Sale Completion POS Data-Leela*/
	   if(((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
		   (0 == strncmp(temp_str3, "01", 2) && 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))
		   )
	   {
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "000001000030",12 );
		   }
		   /*Manual Keyin*/
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "000111000030",12 );
		   }
	   }
   }
 }
/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_MC
 *
 * DESCRIPTION: This function prepare POS data for MC deurring STIP
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_posdata_for_MC(void)
{
   BOOLEAN e_commerce;
   INT iIndustry;
   CHAR    temp_str3[4] = {0};
   INT     itemp;
   
   memset( temp_str3, 0x00, sizeof(temp_str3) );

   if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
   	  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
   {
	   /* Its Acq transaction */
	   iIndustry = atoi( Auth_Tx.industry_code );
	   memcpy( temp_str3, Auth_Tx.TLF01_details.pos_entry_mode +1, 3 );

	   /* Some fields need to know if this txn is an e-commerce txn. */
	   if( (iIndustry == 700) ||
		   (0==strcmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC)) )
	   {
		   e_commerce = true;
	   }
	   else
		   e_commerce = false;
	
	   if ( e_commerce )
	   {
			  strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "102510000660",12);
	   }
	
	   else if ( ('Y' == Auth_Tx.TLF01_details.voice_auth[0]) ||
				 ('y' == Auth_Tx.TLF01_details.voice_auth[0])   )
		  strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "20311000006" ,11);

	   /*Manual Key PEM and Original PEM with Sale Completion POS Data-Leela*/
	   /*else if((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
			   (0 == strncmp(temp_str3, "01", 2) && 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))
			   )
	   {Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
	    		should be PEM-51's
		   when PEM-Mag,Chip Contact, Contactless, Fallback,Contactless magstripe
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "000001000030",12 );
		   }
		   Manual Keyin
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "000111000030",12 );
		   }
	   }*/
	   else
	   {
		  //RPS 203110000010
		   if (Auth_Tx.MCF01_details.rps_merchant[0]=='Y' || iIndustry == 800 ||
				   (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "203410000000",12 );
		   }

		   else if(0 == strncmp(temp_str3, "01", 2) && 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2) )
		   { // Manual Keyed in
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000111000030",12 );
		   }
		   /*else if (0 == strncmp(temp_str3, "01", 2) )
		   { // this can be removed
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "00011000008",11 );
		   }*/
		   else if(0 == strncmp(temp_str3, "91", 2) ||
			       0 == strncmp(temp_str3, "07", 2))
		   {// Contact less magstripe and chip
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
		   else if(0 == strncmp(temp_str3, "02", 2) ||
			       0 == strncmp(temp_str3, "05", 2) ||
				   0 == strncmp(temp_str3, "80", 2) )
		   { // Magstripe swipe, Chip, Chip Fallbak
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
		   else if(0 == strncmp(temp_str3, "01", 2) &&
			   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2)))
		   {
			   // BDO PAY QR
			   strcpy( Auth_Tx.TLF01_details.product_codes[16].amount, "203110000100" );
		   }
		   else if(iIndustry == 500 )
		   {	// MOTO
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "20321000000",11 );
		   }
		   else
		   {// chip, contact, mag stripe
			   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount, "000001000030",12 );
		   }
	   }
	   /*
			TF - Abhishek For Card Ver/ Pre-Auth and Processing code 38, 30, DE61 Subfield 7
			shoud be 4. This is done for April 2016 mandated changes.
			In dcpiso we are storing original key for card verification in auth_tx.mac_key[0] filed.
		*/
		if( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"30",2))||
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"38",2))||
			( Auth_Tx.mac_key[0] == AUTH_CARD_VERIFICATION))
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[6]  = '4';
			}


	   /* ----------------------------------------------------- */
	   /* POS Card Data Terminal Input Capability = Field 61.11 */
	   /* ----------------------------------------------------- */

	   /********************************************************************
		*
		*  Set field 61.11 based on field 22 as follows. The
		*  value of this field is NOT dependent on terminal type.
		*
		*  +----------------+-------------+-----------------------------+
		*  | POS Entry Mode | Field 61.11 | Description (61.11) 		|
		*  +----------------+-------------+-----------------------------+
		*  |	05X 		|	  8 	  | Mag stripe, key entry, chip |
		*  |	01X 		|	  8 	  | 							|
		*  |	80X 		|	  8 	  | 							|
		*  |	90X 		|	  8 	  | 							|
		*  |	072 		|	  3 	  | Contactless M/Chip			|
		*  |	912 		|	  3 	  | 							|
		*  +----------------+-------------+-----------------------------+
		*
		********************************************************************
		*/

	   if ( (0 == strncmp(temp_str3, "90", 2)) ||
			(0 == strncmp(temp_str3, "05", 2)) ||
			(0 == strncmp(temp_str3, "01", 2)) ||
			(0 == strncmp(temp_str3, "80", 2))
		  )
	   {
		  /* Mag stripe, key entry, and EMV-Compatible chip reader */
			if( ((Auth_Tx.TLF01_details.product_codes[16].amount[3] == '0') &&
				 (Auth_Tx.TLF01_details.product_codes[16].amount[4] == '1')))
			{
				// To avoid 8.h edit error
				Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '7';
			}
			/*
			else if( (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, "59",2))||
					   ('Y' == Auth_Tx.TLF01_details.voice_auth[0])||
					   ('y' == Auth_Tx.TLF01_details.voice_auth[0])||
					   (iIndustry == 800) ||
					   (Auth_Tx.MCF01_details.rps_merchant[0]=='Y'))
			{
				  // Do nothing
			}*/
			/*can remove 2019 raghu
			else
			{
				if(0 == strncmp(temp_str3, "01", 2) && 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))
				{
				  // Do nothing
				}
				else
				{
				  Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '8';
				}

			}*/
	   }/*can remove 2019 raghu
	   else if ( (0 == strcmp (temp_str3, "072"  )) ||
	             (0 == strcmp (temp_str3, "912"  )) ||
				 (0 == strncmp(temp_str3, "07",2 )) ||
				 (0 == strncmp(temp_str3, "911",3 ))
	           )
	   {
	      // Contactless M/Chip
		   Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '3';
	   }
	   else if (  (0 == strncmp (temp_str3, "91",2  ))    )
	   {
		 // Contactless M/Chip
		   Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '4';
	   }*/
	   // Check for Ecomerce and RPS , 61.11 should be "0" .. TF Phani
	   	if(0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
	   	{
	   		Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '1';
	   	}

	   	// Edit error 8.f check
	   	/* can remove
	   if ( (0 == strncmp(temp_str3, "90", 2)) ||
			(0 == strncmp(temp_str3, "02", 2)))
	   {
			if(Auth_Tx.TLF01_details.product_codes[16].amount[10]=='6')
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '7';
			}
	   }*/

	   // Edit error 8.i check
	   	/* can remove
	   if ( (0 == strncmp(temp_str3, "90", 2)) ||
			(0 == strncmp(temp_str3, "05", 2)) ||
			(0 == strncmp(temp_str3, "02", 2)) ||
			(0 == strncmp(temp_str3, "80", 2))
		  )
	   {

		   Auth_Tx.TLF01_details.product_codes[16].amount[4]  = '0';
	   }
*/
	   // Edit error 8.e check
	   if ( (0 == strncmp(temp_str3, "00", 2)))
	   {
			if((Auth_Tx.TLF01_details.product_codes[16].amount[10]=='2')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='5')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='7')||
			   (Auth_Tx.TLF01_details.product_codes[16].amount[10]=='8'))
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[10]  = '0';
			}
	   }

	   // Edit error 8.d check
	   if ( (0 == strncmp(temp_str3, "00", 2))||
			(0 == strncmp(temp_str3, "01", 2)))
	   {
			if(Auth_Tx.TLF01_details.product_codes[16].amount[10]=='1')
			{

				Auth_Tx.TLF01_details.product_codes[16].amount[0]='2';
				Auth_Tx.TLF01_details.product_codes[16].amount[2]='3';
			}
	   }
#if BDOR_68_REQ_1_MC
	if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin( )))
	 {
		Auth_Tx.TLF01_details.product_codes[16].amount[9] = MC_CARDHOLDER_ACTIVATED_TERMINAL_LEVEL;
	 }
	else if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin( )))
	 {
		Auth_Tx.TLF01_details.product_codes[16].amount[9] = MC_CARDHOLDER_ACTIVATED_TERMINAL_LEVEL;
	 }
#endif

#if BDOR_68_REQ_3_MC   /* Swiftpass BDO Pay Credit */
	else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
	{
		strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
				 MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				 MC_POS_DATA_LEN );
	}
#endif
#if BDOR_68_REQ_5_MC /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

    else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
    {
	   strncpy( Auth_Tx.TLF01_details.product_codes[16].amount,
			    MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				MC_POS_DATA_LEN );
   }
#endif
	   // Edit error 3.d and 3.e
	   /* can remove
	   if((0 == strncmp(temp_str3, "80", 2))||
		  (0 == strncmp(temp_str3, "90", 2))||
		  (0 == strncmp(temp_str3, "91", 2)))
	   {
			// Check if DE45 and DE35 is present or not
			if((strlen(Auth_Tx.TLF01_details.track2) > 0)||
			   (strlen(Auth_Tx.TLF01_details.track1) > 0))
			{
				Auth_Tx.TLF01_details.product_codes[16].amount[3]  = '0';
				if(Auth_Tx.TLF01_details.product_codes[16].amount[4]  == '1')
				{
					Auth_Tx.TLF01_details.product_codes[16].amount[4]  = '0';
				}
			}
	   }
*/
	   /* For automated fuel dispenser transactions, subfield 10 must be '2' */
	   if ( 0 == strncmp( AUTO_FUEL_MCC, Auth_Tx.TLF01_details.category_code,4 ))
	   {
		  /* Industry code 600 should take care of this,
		   * but just in case, do it here, else MasterCard charges a fee.
		   */
		  memset( &Auth_Tx.TLF01_details.product_codes[16].amount[9], 0x32, 1 );
	   }
	   Auth_Tx.TLF01_details.product_codes[16].amount[12]=0x00;
   }
 }
/*Sale Completion POS Data- Leela*/
void txservice_Prepare_posdata_for_JCB_OfflineSale(void)
{

	CHAR   temp_str[50] = {0};
	INT    iIndustry;
	INT    mcc;
	CHAR   tempstr [256] = {0};
	CHAR   num_cc[4] = {0};

	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
	   /* Sale Completion POS Data-Leela*/
	   if(((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
			   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
				 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
	   {
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "222608",6 );
		   }
	   }
     }
}
/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_JCB
 *
 * DESCRIPTION: This function prepare POS data for JCB deurring STIP
 *
 * INPUTS:      None
 *   
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

void txservice_Prepare_posdata_for_JCB(void)
{

	CHAR   temp_str[50] = {0};
	INT    iIndustry;
	INT    mcc;
	CHAR   tempstr [256] = {0};
	CHAR   num_cc[4] = {0};


	/* all the data is in auth_tx.int_net_facility_data */
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		iIndustry = atoi( Auth_Tx.industry_code );
	
		if((txservice_Is_AsiaPay_Transaction() == true))
		{
			iIndustry=ECOMMERCE;
		}

		/* FIELD 61: POINT OF SERVICE INFORMATION */
	   /* Default Values */
	   memset( temp_str, 0x00, sizeof(temp_str) );
	   strcpy( temp_str, "222" );
	
	   /* Subfield 61.1 - Mail/Telephone Order Transaction Sign */
	   if ( iIndustry == MAIL_PHONE )
	   {
		  temp_str[0] = '2';
	   }
	   /* Subfield 61.2 - Recurring Transaction Sign */
	   //else if ( iIndustry == RPS || strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0 )
	   else if ((strncmp(Auth_Tx.TLF01_details.message_type,"0100", 4) == 0 ||
			   	   strncmp(Auth_Tx.TLF01_details.message_type,"0200", 4) == 0) &&
			   (strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2) == 0) &&
			   Auth_Tx.MCF01_details.rps_merchant[0] == 'Y')
	   {
		  temp_str[0] = '2' ;
		  temp_str[1] = '1';
	
		  /* Check for MOTO Merchant */
		  mcc = atoi( Auth_Tx.TLF01_details.category_code );
		  if ( (MCC_RANGE2_LOW <= mcc)	&&	(mcc <= MCC_RANGE2_HIGH) )
			 temp_str[0] = '1';
	   }
	   //else if(iIndustry == ECOMMERCE || strncmp(Auth_Tx.TLF01_details.pos_condition_code,ECOM_POS_CC,2) == 0 )
	   else if((strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0) ||
			   (strncmp(Auth_Tx.TLF01_details.pos_condition_code,"05",2) ==0)) //bdoo PAY QR and ECOm
	   { // If it JCB RPS transaction send 10 in firt 2 bytes of POS data, HA issue fix, April 2019 Raised by Anna.
		   temp_str[0] = '1' ;

	   }
	   /*Manual Key PEM and Original PEM with Sale Completion POS Data-Leela*/
	   /*if((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
			   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
				 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
	   {Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
				should be PEM-51's
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"002",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"005",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"007",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"080",3) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0)||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "222608",6 );
		   }
	   }*/
	   /* Subfield 61.3 - Pre Authorization Sign */
	   if ( (Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)||
			(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION_RESPONSE) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION) ||
			(Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION_RESPONSE)||
			(Auth_Tx.mac_key[0]			 == AUTH_CARD_VERIFICATION)  )
	   {
		  /* Note: Dcpiso changes Card Verifies to Auths. The original tx_key is stored in mac_key.   */
		  temp_str[2] = '1';
	   }
#if BDOR_68_REQ_1_JCB
      if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin(  )))
      {
    	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
    			  JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
				  JCB_POS_DATA_LEN );
      }
      else if ((true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin(  )))
      {
    	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
    			  JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,
				  JCB_POS_DATA_LEN );
      }
#endif

#if BDOR_68_REQ_3_JCB   /* Swiftpass BDO Pay Credit */
      else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
      {
    	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
    			  JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  JCB_POS_DATA_LEN );
      }
      else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
      {
    	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
    			  JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				  JCB_POS_DATA_LEN );
      }

#endif
#if BDOR_68_REQ_5_JCB /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

     else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
	{
    	  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount,
    			  JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				  JCB_POS_DATA_LEN );

	}
#endif
     else
     {
		   /* Subfield 61.4 - Numeric Country Code where terminal is located. */
		   memset( tempstr, 0x00, sizeof(tempstr) );
		   memcpy( tempstr, Auth_Tx.MCF01_details.intl_code, 2 );

		   /* Convert 2-char alpha country code to 3-char numeric country code. */
		   //genutil_get_country_code( A2N, tempstr, num_cc );
		   /*if ( num_cc[0] == 0x00 )
		   {
			  //Default to Peso (608) for Equitable. Normally we would default to 900 for an unknown value.
			  strcpy( num_cc, "608" );
		   }*/
			  strcpy( num_cc, "608" );
		   memcpy( temp_str+3, num_cc, 3 );

		  memcpy( Auth_Tx.TLF01_details.product_codes[16].amount, temp_str, 6 );
     }
	}
}

/*Sale Completion POS DATA for displaying in Tranlookup -Leela*/

txservice_Prepare_posdata_for_DINERS_OfflineSale ()
{
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		if(((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0)||(strncmp(Auth_Tx.TLF01_details.message_type,"0230", 4) == 0)) &&
		   ( (0 == strncmp("001",Auth_Tx.TLF01_details.product_codes[14].code,3)) &&
			 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
		{
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0022",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101255140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0052",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101551140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0072",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101S51140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0802",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101955140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101T65140",12 );
		   }
		   //Manual Keyin
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "660101664140",12 );
		   }
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0021",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101255141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0051",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101551141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0071",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101S51141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0801",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101955141",12 );
		   }
	   }
	}

}



/*******************************************************************************
 * NAME:        txservice_Prepare_posdata_for_DINERS
 *
 * DESCRIPTION: This function prepare POS data for DINERS durring STIP
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/
txservice_Prepare_posdata_for_DINERS ()
{

	if(true == txservice_check_DINERS_emv_capability_device ())
	{
		if(true == txservice_check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Chip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Fallback_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_is_voice_transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_VOICE,
					DINERS_DE22_SIZE);
		}
#if BDOR_68_REQ_1_DCI
		/* MobilePOS-NFC Terminal for Tap2Phone Transactions*/
		else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
#endif

#if BDOR_68_REQ_3_DCI
		/*Swiftpass BDO Pay Credit Transactions With Pin*/
		else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
#endif
#if BDOR_68_REQ_5_DCI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

		else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DINERS_DE22_SIZE);
		}
#endif
		else if(true == txservice_check_is_Diners_Manual_pos_transaction())
		{
			if(true == txservice_check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_RPS_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_Asiapay_Recurring())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_RPS	,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_BDO_PAY())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY	,
						DINERS_DE22_SIZE);
			}
			else if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
	else
	{
		if(true == txservice_check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Chip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN ,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Fallback_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == txservice_check_is_voice_transaction())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_VOICE,
					DINERS_DE22_SIZE);
		}
#if BDOR_68_REQ_1_DCI
		/* MobilePOS-NFC Terminal for Tap2Phone Transactions*/
		else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else if(true == txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
#endif

#if BDOR_68_REQ_3_DCI
		/*Swiftpass BDO Pay Credit Transactions With Pin*/
		else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITH_PIN,
					DINERS_DE22_SIZE);
		}
		else if(true == txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA_WITHOUT_PIN,
					DINERS_DE22_SIZE);
		}
#endif
#if BDOR_68_REQ_5_DCI /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

		else if(true == txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx ())
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DINERS_DE22_SIZE);
		}
#endif
		else if(true == txservice_check_is_Diners_Manual_pos_transaction())
		{
			if(true == txservice_check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_RPS_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_Asiapay_Recurring())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == txservice_check_is_Diners_BDO_PAY())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY	,
						DINERS_DE22_SIZE);
			}
			else if(true == txservice_Check_If_DCI_pin_block_exists())
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}

/*	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		Manual Key PEM and Original PEM with Sale Completion POS Data-Leela
		if((strncmp(Auth_Tx.TLF01_details.message_type,"0220", 4) == 0) &&
		   ( (0 == strncmp("001",Auth_Tx.TLF01_details.pos_entry_mode,3)) &&
			 (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,"00",2))))
		{Eg: when pre-auth's PEM 51 and Offline sale's PEM is MK-12 then Sale completion POS Data
				should be PEM-51's
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0022",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101255140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0052",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101551140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0072",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101S51140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0802",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101955140",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0912",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101T65140",12 );
		   }
		   Manual Keyin
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"001",3) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "660101664140",12 );
		   }
		   if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0021",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101255141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0051",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101551141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0071",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101S51141",12 );
		   }
		   else if((strncmp(Auth_Tx.TLF01_details.product_codes[10].code,"0801",4) == 0))
		   {
			   strncpy( Auth_Tx.TLF01_details.product_codes[5].amount, "960101955141",12 );
		   }
	   }
	}*/

}



/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It compares to anything in the card number field in auth_tx.
 *                It compares the expiration date to anything in the exp date
 *                field in auth_tx.  If empty, it copies them into those fields.
 *                It also copies the service code into a global variable in
 *                order to check for a Visa Electron transaction.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT txservice_Parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   INT    bf2_max_len=19;
   pBYTE  field_separator   = NULL_PTR;

   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = p_auth_tx->TLF01_details.track2;
      for(track2_idx=0;track2_idx <= bf2_max_len;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( track2_idx > bf2_max_len )
      {
         ret_val = false;
         sprintf( error_msg,
                 "Unable to parse track2. No field separator. Track2: %s",
                  p_auth_tx->TLF01_details.track2 );
      }
      else
      {
         /* Put Card Number into auth_tx. */
         if ( p_auth_tx->TLF01_details.card_num[0] == 0x00 )
         {
            card_num_len = field_separator - p_auth_tx->TLF01_details.track2;

           /* If track2 length is odd, ignore leading zero on card number. */
               memcpy( p_auth_tx->TLF01_details.card_num,
                       p_auth_tx->TLF01_details.track2, card_num_len);
            
            sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",card_num_len);
         }

         /* Get the expiration date. */
         if ( p_auth_tx->TLF01_details.exp_date[0] == 0x00 )
         {
            memcpy( p_auth_tx->TLF01_details.exp_date, field_separator+1, 4 );
         }

         /* Get the service code. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, field_separator+5, 3 );
      }
   }
   return( ret_val );
}


BYTE txservice_IsFieldNull (char str[])
{
	if (strlen (str) == 0)
		return 1 ;
	return 0 ;
}


/**************************************************************************
NAME:			txserviceCadience_Perform_Balance_Account_Checks
DESCRIPTION:	Determines if the balance account limits have been exceeded	
INPUTS:			The transaction amount				
OUTPUTS:		None
RETURNS:        false  if limit is exceeded
                true   if limit is not exceeded
AUTHOR:         Ravikumar Nageshaiah, Initial Release 08-16 
                Cadience Standin processing changes.
MODIFIED BY:	
**************************************************************************/
BYTE txserviceCadience_Perform_Balance_Account_Checks (double nConvertedTxAmount) 
{
   double nACF01_Available        = 0.0 ;
   double nACF01_Cash_Available   = 0.0 ;
   double nACF01_Outstanding      = 0.0 ;
   double nACF01_Cash_Outstanding = 0.0 ;
   double nACF01_Credit_Limit     = 0.0 ;
   double nACF01_Cash_Limit       = 0.0 ;
   double nTLF01_orig_amount      = 0.0 ;
   double nMCF01_floor_limit	  = 0.0;

   BYTE strTemp[256] = {0} ;
   BYTE strTxType[7] = {0} ;
   BYTE strProcessingCode[20] = {0} ;
   BYTE ReturnCode = 0 ;


   /* Reset the global variables */
   nCCF02P_Incremented_Outstanding_Auths = 0.0 ;
   nACF01_Incremented_Outstanding        = 0.0 ;
   nACF01_Incremented_Cash_Outstanding   = 0.0 ;
   nACF01_Decremented_Available          = 0.0 ;
   nACF01_Decremented_Cash_Available     = 0.0 ;

   /* Populate with values from the ACF01 record */
   nACF01_Available        = (strtod (Auth_Tx.ACF01_details.available, 0) / 100.0) ;
   nACF01_Outstanding      = (strtod (Auth_Tx.ACF01_details.outstanding, 0) / 100.0) ;
   nACF01_Cash_Available   = (strtod (Auth_Tx.ACF01_details.cash_available, 0) / 100.0) ;
   nACF01_Cash_Outstanding = (strtod (Auth_Tx.ACF01_details.cash_outstanding, 0) / 100.0) ;
   nACF01_Credit_Limit     = (strtod (Auth_Tx.ACF01_details.credit_limit, 0) / 100.0) ;
   nACF01_Cash_Limit       = (strtod (Auth_Tx.ACF01_details.cash_limit, 0) / 100.0) ;

	if(nACF01_Available == 0)
	{
		// We are seeing avaiable as Zero, lets check if we have retrive details correctly or not
		//Cash
		if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE               ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH )
		{
		   if(nACF01_Cash_Limit <= 0)
		   {
			   txcntl_log_message( 2, 3,"Failed to read ACF01 details for Cash",
								   "txserviceCadience_Perform_Balance_Account_Checks", 1 );
		   }
		   else
		   {
			   nACF01_Available = nACF01_Cash_Limit - nACF01_Cash_Outstanding;
		   }
		}
		// Credit
		else if(nACF01_Credit_Limit <= 0)
		{
		   txcntl_log_message( 2, 3,"Failed to read ACF01 details",
				   	   	   	   "txserviceCadience_Perform_Balance_Account_Checks", 1 );
		}
		else
		{
		   nACF01_Available = nACF01_Credit_Limit - nACF01_Outstanding;
		}
	}
   
   /* If tx is CASH_ADVANCE or QUASI_CASH,
    * then follow the algorithm in the specs.
    */
   if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE||
        Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE||
        Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH_RESPONSE)
   {
      if (nConvertedTxAmount <= nACF01_Cash_Available)
      {
         if (nConvertedTxAmount <= nACF01_Available)
         {
            // transaction is approved
            nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
            nACF01_Decremented_Cash_Available   = nACF01_Cash_Available - nConvertedTxAmount ;
            nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
            nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ; 
         }
         else
         {
			 /* Even though available amt  is less, ATP should update the account details */
            nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
            nACF01_Decremented_Cash_Available   = nACF01_Cash_Available - nConvertedTxAmount ;
            nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
            nACF01_Decremented_Available        = 0.00;

			 sprintf (strTemp,"Bal_Chk fail-Avail:%-9.2fReq:%-9.2f ", 
                     nACF01_Available, nConvertedTxAmount) ;
			txcntl_log_message( 2, 1, strTemp, "txserviceCadience_Perform_Balance_Account_Checks", 1 );
			return false;
		
         }
      }
      else
      {
		  /* Even though available amt  is less, ATP should update the account details */
		  if (nConvertedTxAmount <= nACF01_Available)
		  {
            nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
            nACF01_Decremented_Cash_Available   = 0.00 ;
            nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
            nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount;
		  }
		  else
		  {
			nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
            nACF01_Decremented_Cash_Available   = 0.00 ;
            nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
            nACF01_Decremented_Available        = 0.00;
		  }

          sprintf (strTemp,"CashAdv fail-Avail:%-9.2fReq:%-9.2f", 
                  nACF01_Cash_Available, nConvertedTxAmount) ;
 			txcntl_log_message( 2, 1, strTemp, "txserviceCadience_Perform_Balance_Account_Checks", 1 );
			return false;
      }
   }
   /*else if ( (strcmp ( Auth_Tx.TLF01_details.response_code, "00" ) != 0) &&
             (strcmp ( Auth_Tx.TLF01_details.message_type, "0130" )== 0 ) )
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
      nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
   }*/
   else if ( Auth_Tx.TLF01_details.tx_key == AUTH_REFUND_RESPONSE)
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
      nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
   }
   else if (txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response()       || 
            Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE||          
            Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE||          
            txserviceCadience_If_Tx_Is_Treated_Like_A_Offline_Void_Response() )
   {
      /* Transaction is AUTH_REVERSAL, or ATM_CONFIRM, or REVERSAL_ADVICE */
      strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;

      /* If the previous tx was a SALE, OFFLINE_SALE, AUTHORIZATION or a
       * PRE_AUTHORIZATION, we have to undo the previous transaction from
       * the Oustanding and the Available.
       */
	  /* SCR 12502 - ThoughtFocus 
	   * REVERSAL of an AUTH_VOID_REFUND, AUTH_OFFLINE_VOID_REFUND, AUTH_REFUND_ADJUSTMENT, AUTH_VOID_RELOAD
	   */
	  if  ( txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response() &&
		    strEqual (strProcessingCode, "22")
		  )
	  {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
	  }
	  /* SCR 12502 - ThoughtFocus */ 
   /* if  ( strEqual (strProcessingCode, "20") ) */ /* AUTH_REFUND */
	  else if  ( strEqual (strProcessingCode, "20") )  /* REVERSAL of AUTH_REFUND/AUTH_VOID_CASH_ADVANCE */
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else if  ( strEqual (strProcessingCode, "22") )  /* AUTH_VOID_REFUND/AUTH_OFFLINE_REFUND*/
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }

      /* Deferred Purchase including Deferred Voids
       * ------------------------------------------
       * This IF statement looks for a deferred purchase OR void of deferred,
       * AND not an RSB card.  Need this 'and' because RSB is very similar to
       * a deferred purchase, but there is no deferred record. So an RSB card
       * will fall through to the next 'else if'.
       */
      else if ( ( (strEqual(strProcessingCode, "70"))  ||
                  (Auth_Tx.TLF01_details.deferred_term_length[0] != 0x00) ) &&
                (  Auth_Tx.BIN01_details.credit_subtype[0] != RSB_CARD)      )
      {
         if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, PESO_ACCOUNT_TYPE))
         {
            /* SECOND PASS THROUGH HERE
             * ------------------------
             * This is the PESO account for the deferred customer.
             * Calculate the monthly amortized amount.  It is compared
             * to the Peso balance account.
             *
             * The amortized amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the PESO account.
             */
            txserviceCadience_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
         }
         else if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, DEFERRED_ACCOUNT_TYPE))
         {
            /* FIRST PASS THROUGH HERE
             * -----------------------
             * This is the DEFERRED account for the deferred customer.
             * Calculate the gross deferred amount.  It will be compared to
             * the deferred balance account.  Later, 2nd pass, the monthly
             * amortization amount will be compared to the Peso balance.
             *
             * The gross amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the DEFERRED account
             */
            txserviceCadience_Calculate_Deferred_Gross_Amount (&nConvertedTxAmount) ;
         }
         sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
         nConvertedTxAmount = strtod ( strTemp, 0 );

         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
#ifdef AMEX03A
      else if(true == txservice_Check_If_AMEX_issuing_transaction() &&
    		  true == txservice_check_for_AMEX_partial_reversal_transaction())
      {
    	  nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
    	  nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
    	  /* Not done any changes to the cash part, just copy back the data read from ACF01 back to ACF01*/
          nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
          nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
#endif

	  /* SCR 12502 -ThoughtFocus
	   * REVERSAL of a AUTH_VOID_SALE or AUTH_OFFLINE_VOID_SALE or AUTH_SALE_ADJUSTMENT
	   */
	  else if ( txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response() &&
		  strEqual (strProcessingCode, "02")) 
	  {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
	  }

      else if  (strEqual (strProcessingCode, "00") || /* AUTH_SALE              */ 
                                                      /* or AUTH_OFFLINE        */
                                                      /* or AUTH_AUTHORIZATION  */
           strEqual (strProcessingCode, "30") ||      /* AUTH_PRE_AUTHORIZATION */
           strEqual (strProcessingCode, "02"))        /* AUTH_VOID_SALE         */
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
      else if (strEqual (strProcessingCode, "01") || /* AUTH_CASH_ADVANCE */
               strEqual (strProcessingCode, "17") ||
               strEqual (strProcessingCode, "11") || /* AUTH_QUASI_CASH */
               strEqual (strProcessingCode, "18")  ) /* AUTH_QUASI_CASH */
      {
         /* If the previous tx was a QUASI_CASH or CASH_ADVANCE,
          * we need to undo it from purchase and cash accounts.
          */

         /* Undo Purchase Account */
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;

         /* Undo Cash Account */
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
   }

   else if (txserviceCadience_If_Tx_Is_Treated_Like_A_Voice_Auth_Override_Response ())
   {
      /* AUTH_AUTHORIZATION_OVERRIDE, AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE,
         AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE */
      if (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE) 
      {
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available   - nConvertedTxAmount ;
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ; 
         nACF01_Decremented_Available        = nACF01_Available  - nConvertedTxAmount ;
      }
      else if (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE_RESPONSE||
               Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE) 
      {
         if (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE)
         {
            txserviceCadience_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
            sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
            nConvertedTxAmount = strtod ( strTemp, 0 );
         }

         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
   }
   else if ( txserviceCadience_If_Tx_Is_Treated_Like_A_Adjustment_Response () )
   {
      strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;
      if  ( strEqual (strProcessingCode, "22") )  /* AUTH_REFUND */
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
   }

   else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION_RESPONSE)
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
      nACF01_Decremented_Available        = nACF01_Available ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;

      if (nConvertedTxAmount > nACF01_Available)
      {
       
		nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
        nACF01_Decremented_Available        = 0.00;

		  sprintf (strTemp,"Bal_Acc_Chk fail-Avail:%9.2fReq:%9.2f", 
                  nACF01_Available, nConvertedTxAmount) ;
		txcntl_log_message( 2, 1, strTemp, "txserviceCadience_Perform_Balance_Account_Checks", 1 );
		return false;
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE||
            Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
            Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE||
            Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE||
		    Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
   {
      if (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE)
      {
         if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, PESO_ACCOUNT_TYPE))
         {
            /* SECOND PASS THROUGH HERE
             * ------------------------
             * This is the PESO account for the deferred customer.
             * Calculate the monthly amortized amount.  It is compared
             * to the Peso balance account.
             *
             * The amortized amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the PESO account
             */
            txserviceCadience_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
         }
         else if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, DEFERRED_ACCOUNT_TYPE))
         {
            /* FIRST PASS THROUGH HERE
             * -----------------------
             * This is the DEFERRED account for the deferred customer.
             * Calculate the gross deferred amount.  It will be compared to
             * the deferred balance account.  Later, 2nd pass, the monthly
             * amortization amount will be compared to the Peso balance.
             *
             * The gross amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the DEFERRED account
             */
            txserviceCadience_Calculate_Deferred_Gross_Amount (&nConvertedTxAmount) ;
         }
         sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
         nConvertedTxAmount = strtod ( strTemp, 0 );
      }

      if (nConvertedTxAmount <= nACF01_Available)
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else
      {
		   /* ALLOW if tx amount is less than FLOOR LIMIT */
		  nMCF01_floor_limit = (strtod (Auth_Tx.MCF01_details.network[0].priority_routing, 0) / 100.0) ;
		  if(nConvertedTxAmount <= nMCF01_floor_limit)
		  {
			 nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
			 nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
			 nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
			 nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;

			return NO_AUTH_ERROR ;
		  }
          nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
          nACF01_Decremented_Available        = 0.00 ;
		  sprintf (strTemp,
                 "BalChk fail-Avail:%-9.2fReq:%-9.2f", 
                  nACF01_Available, nConvertedTxAmount) ;
		 txcntl_log_message( 2, 1, strTemp, "txserviceCadience_Perform_Balance_Account_Checks", 1 );
		 	/*TF PHANI ..Error handling for insuufcient funds */
		 ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp,
                                 "TxAuthorize_Perform_Balance_Account_Checks",
                                  GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED, 
                                  NO_SUFFICIENT_FUNDS) ;
		return false;
		
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD_CONFIRM_RESPONSE)
   {
	     nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD_RESPONSE)
   {
	     nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      
   }

   /* Make sure Available is not greater than credit limit. */
   if ( nACF01_Decremented_Available > nACF01_Credit_Limit )
      nACF01_Decremented_Available = nACF01_Credit_Limit;

   if ( nACF01_Decremented_Cash_Available > nACF01_Cash_Limit )
      nACF01_Decremented_Cash_Available = nACF01_Cash_Limit;

   return NO_AUTH_ERROR ;
} /* txservice_Cadience_Balance_Account_Checks */

BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response (void)
{
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_REVERSAL_RESPONSE:
		case AUTH_ATM_CONFIRM_RESPONSE:
		case AUTH_REVERSAL_ADVICE_RESPONSE:
		case AUTH_REVERSAL_REPEAT_RESPONSE:
		  	return true ;
		default:
			return false ;
	}
} /* txservice_If_Transaction_Is_A_Reversal_Response */

BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Offline_Void_Response(void)
{
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_OFFLINE_VOID_SALE_RESPONSE:
			if ( strcmp ( Auth_Tx.TLF01_details.orig_retrieval_ref_num, "" ) != 0 )
   		  		return true ;
		break ;	/* Added by SDG */

		case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
			return true ;
		break ;
		default:
			return false ;
		break ;
	}
	return false ;	
} /* txservice_If_Tx_Is_Treated_Like_A_Offline_Void_Response */

BYTE txserviceCadience_Calculate_Deferred_Gross_Amount (double* pConvertedTxAmount) 
{
   double  nDeferredFactor = 0.0 ;
   double  nAmount         = 0.0 ;
   int     nDeferredMonths = 0 ;

   nDeferredFactor = (strtod(Auth_Tx.TLF01_details.deferred_factor, 0) / 1000000.0);
   nDeferredMonths = atoi(Auth_Tx.TLF01_details.deferred_term_length);

   if (txserviceCadience_If_Tx_Is_Treated_Like_A_Reversal_Response())
      nAmount = (strtod (Auth_Tx.TLF01_details.reversal_amount, 0) / 100.0) ;
   else
      nAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

   /* *pConvertedTxAmount = nAmount + (nAmount * nDeferredFactor) ;*/
   *pConvertedTxAmount = nAmount * nDeferredFactor * nDeferredMonths ;

	return NO_AUTH_ERROR ;
}

BYTE txserviceCadience_Calculate_Deferred_Monthly_Amortized_Amount (double* pConvertedTxAmount) 
{
   double  nGrossDeferredAmount = 0.0 ;
   int     nDeferredMonths      = 0 ;

   txserviceCadience_Calculate_Deferred_Gross_Amount (&nGrossDeferredAmount)  ;

   nDeferredMonths = atoi (Auth_Tx.TLF01_details.deferred_term_length) ;

   if (nDeferredMonths == 0) 
      nDeferredMonths = 1 ;

   *pConvertedTxAmount = nGrossDeferredAmount / ((double)nDeferredMonths) ;

   /* Update values in TLF01 record. */
   sprintf( Auth_Tx.TLF01_details.def_gross_amt,
           "%012.0f",
            nGrossDeferredAmount*100.0 );

   sprintf( Auth_Tx.TLF01_details.monthly_amort_amt,
           "%012.0f",
            *pConvertedTxAmount*100.0 );

	return NO_AUTH_ERROR ;
}

BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Adjustment_Response (void)
{
   switch (Auth_Tx.TLF01_details.tx_key)
   {
      case AUTH_SALE_ADJUSTMENT_RESPONSE:
      case AUTH_REFUND_ADJUSTMENT_RESPONSE: 
         return true ;
      default:
         return false ;
   }
}
BYTE txserviceCadience_If_Tx_Is_Treated_Like_A_Voice_Auth_Override_Response (void)
{
   switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
		  	return true ;
		default:
			return false ;
	}
}

BYTE txserviceCadience_Update_ACF01_Structure (void)
{
	BYTE strTemp[50] = {0} ;
	BYTE strAmount[50] = {0} ;
	int nIndex = 0 ;
	BYTE tmp_update_mask[2];
	int ReturnCode = 0;

	tmp_update_mask[0] = Auth_Tx.TLF01_details.update_mask[0];
	/* Clear the update mask */
    Auth_Tx.TLF01_details.update_mask[0] = 0x00;
    if (nACF01_Incremented_Outstanding <= 0.0) nACF01_Incremented_Outstanding = 0.0 ;
    if (nACF01_Incremented_Cash_Outstanding <= 0.0) nACF01_Incremented_Cash_Outstanding = 0.0 ;
    if (nACF01_Decremented_Available <= 0.0) nACF01_Decremented_Available = 0.0 ;
    if (nACF01_Decremented_Cash_Available <= 0.0) nACF01_Decremented_Cash_Available = 0.0 ;

    /* We have a limit of 10 digits and 2 decimals for amount fields */
    if (nACF01_Incremented_Outstanding > 9999999999.99) nACF01_Incremented_Outstanding = 9999999999.99 ;
    if (nACF01_Incremented_Cash_Outstanding > 9999999999.99) nACF01_Incremented_Cash_Outstanding = 9999999999.99 ;

    memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nACF01_Incremented_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Incremented_Cash_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Available, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.available, strAmount) ;

    /* special case - this is because TLF01 contains only <outstanding_balance> */
    strcpy (Auth_Tx.TLF01_details.outstanding_balance, Auth_Tx.ACF01_details.available) ;
    /* end special case */

    strcpy (Auth_Tx.TLF01_details.credit_limit, Auth_Tx.ACF01_details.credit_limit) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Cash_Available, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_available, strAmount) ;
	/* Set ACF01_MASK in the update_mask field of TLF01 */
	 Auth_Tx.TLF01_details.update_mask[0] |= ACF01_MASK ;

   /* If currency was converted from USD to Peso for JCB or AMEX,
    * need to set it back to USD for logging into TLF01. The flag
    * that determines this is host_start_time set to 0.000.
    */
   if ( strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP) )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code,   PESO_CURRENCY );
     // strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
   }
   else if ( strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP) )
   {
      strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
     // strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
   }

   /* For INC and ACQ online transactions approved by Vertexon */
   if ((strncmp(Auth_Tx.TLF01_details.product_codes[4].code, "ISSUE", 5) == 0 ||
		strncmp(Auth_Tx.TLF01_details.product_codes[4].code, "ACQUR", 5) == 0) &&
		   Auth_Tx.pan_extended[0] == '\0')
   {
   	Auth_Tx.TLF01_details.product_codes[9].quantity[0] = 1;
   	ReturnCode = TxDataServer_Get_PAF01_Details();
   	return NO_AUTH_ERROR ;
   }
   else
   {	 txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue,
                                          MT_AUTH_RESPONSE, 
                                          0, 
                                          0) ;
   }
   return NO_AUTH_ERROR ;
} /* txserviceCadience_Update_ACF01_Structure */

/**************************************************************************
NAME:			txserviceCadience_Perform_Balance_Update
DESCRIPTION:	Determines if the balance account limits have been exceeded	
INPUTS:			The transaction amount				
OUTPUTS:		None
RETURNS:        false  if limit is exceeded
                true   if limit is not exceeded
AUTHOR:         Ravikumar Nageshaiah
MODIFIED BY:	
**************************************************************************/
BYTE txserviceCadience_Perform_Balance_Update(void)
{
	BYTE ReturnCode = 0;
//	BYTE bReturnCode;
	double nAuthTxTotalAmount = 0.0;
	BYTE MessageSubType2 = 0 ;
	static int RetryCountCCF02P =0;
	static int RetryCountACF01 = 0;
	char strTemp[256] = {0} ;
//	char Buffer[250];
	/*  Need to Check 3rd Currency code coming from Host- TF Phani*/
	if(	!strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) && 
		!strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)
       )
    {	
    	if(true == txservice_Check_If_CUP_Transaction())
		{
    		/*
			  	  ATP must authorize in PESO or USD.
			  	  Let us check for the 3rd Currency.
			 	  If field 49 is neither PESO and USD
			 	  use field 6 for the amount field, and 51 as currency
			 	  if Field 51 is not dollor or PESO then
			 	  use field 5 for the amount field, and 51 as currency
			 	  if Field 50 is not dollor or PESO then drop it.
		 	 	  This is for switched in, or Issuing, transactions only.
		 	 	 Its only for CUP
    		 */
			// Check Field 51 is PESO or DOLLOR. Its Card Holder billing Currency rate.
			if(strEqual (Auth_Tx.ch_billing_curr_code, PESO_CURRENCY) || 
				strEqual (Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY))
	   		 {	
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 51 is PESO
				if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 51 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}
		
			// Check Field 50 is PESO or DOLLOR . Its Settlement Currency Code
			else if(strEqual (Auth_Tx.settlement_curr_code, PESO_CURRENCY) || 
				strEqual (Auth_Tx.settlement_curr_code, USDOLLAR_CURRENCY))
	   		 {	
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 50 is PESO
				if ( 0 == strncmp( Auth_Tx.settlement_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.TLF01_details.settlement_total );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 50 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.settlement_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.TLF01_details.settlement_total );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}

			else if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))
			{
				if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)
				{
					//Let us check if its AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE
					//strncpy(Auth_Tx.TLF01_details.product_codes[0].quantity,"20",2);
					if(0 == strncmp(Auth_Tx.TLF01_details.product_codes[0].quantity,"20",2))
					{
						// We need to see if we have original transaction is present in DB or not.
						if(0 == strlen(Auth_Tx.TLF01_details.reversal_amount))
						{
							strcpy (strTemp, "Currency Not supported for Tx") ;
							memcpy( Auth_Tx.TLF01_details.response_text,  strTemp,
								    sizeof(Auth_Tx.TLF01_details.response_text)-1 );
							strncpy(Auth_Tx.TLF01_details.response_code,CUP_DO_NOT_HONOR,
									CUP_RESPONSEP_CODE_LEN);

							ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp,
																						"txserviceCadience_Perform_Balance_Update",
																						GS_TXN_INVALID_CURRENCY_CODE,
																						DO_NOT_HONOR) ;

							return ReturnCode ;
						}
						else
						{
							strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
						}
					}
					else if(strlen(Auth_Tx.TLF01_details.reversal_amount) > 0)
					{
						strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
					}
					else
					{
						strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
						strncpy(Auth_Tx.TLF01_details.response_code,CUP_APPROVED,
								CUP_RESPONSEP_CODE_LEN);
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
					}

				}
				else if((true == txservice_Check_If_CUP_ECOM ())&&
						 Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
				{
					// Its CUP ECOM Balance inquiry transaction transaction
					ReturnCode = TxDataServer_Get_CCF02_Details ();
					if (ReturnCode == NO_AUTH_ERROR)
					{
						return NO_AUTH_ERROR;
					}
				}
				else
				{
					strcpy (strTemp, "Currency Not supported for Tx") ;
					memcpy( Auth_Tx.TLF01_details.response_text,
				           strTemp,
				           sizeof(Auth_Tx.TLF01_details.response_text)-1 );
					strncpy(Auth_Tx.TLF01_details.response_code,"05",2);

					ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp,
																				"txserviceCadience_Perform_Balance_Update",
																				GS_TXN_INVALID_CURRENCY_CODE,
																				DO_NOT_HONOR) ;

					return ReturnCode ;
				}

			}
			else
			{
				return GS_TXN_COMPLETED;
			}
		}
    	else if(true == txservice_Check_If_Diners_Transaction())
		{
    		/*
			  	  ATP must authorize in PESO or USD.
			  	  Let us check for the 3rd Currency.
			 	  If field 49 is neither PESO and USD
			 	  use field 6 for the amount field, and 51 as currency
			 	  This is for switched in, or Issuing, transactions only.
		 	 	 Its only for CUP
    		 */
			// Check Field 51 is PESO or DOLLOR. Its Card Holder billing Currency rate.
			if(strEqual (Auth_Tx.ch_billing_curr_code, PESO_CURRENCY) ||
				strEqual (Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY))
	   		 {
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 51 is PESO
				if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 51 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}
			else
			{
				return GS_TXN_COMPLETED;
			}
		}
		else
		{
			return GS_TXN_COMPLETED;
		}
	}
	if (!(strEqual(Auth_Tx.CCF02P_details.primary_key.card_nbr,Auth_Tx.TLF01_details.card_num)))
	{
		ReturnCode = TxDataServer_Get_CCF02_Details ();
		if (ReturnCode == NO_AUTH_ERROR)
		return NO_AUTH_ERROR;
	

    }
    else if ((strEqual(Auth_Tx.CCF02P_details.primary_key.card_nbr,Auth_Tx.TLF01_details.card_num)))
    {
    	// here we have CCF02P data. Get the ACF01 data from the DS calls 
    	ReturnCode = TxDataServer_Get_ACF01_Details (Auth_Tx.CCF02P_details.related_acct[9].type,
                                                  Auth_Tx.CCF02P_details.related_acct[9].nbr,
                                                  MessageSubType2) ;
    	if (ReturnCode == NO_AUTH_ERROR)  
    		return NO_AUTH_ERROR;
	
    }
	return NO_AUTH_ERROR;
}

BYTE txserviceCadience_Continue_With_CCF02P_For_Balance_Update(void)
{
//	char Buffer[250];
	BYTE ReturnCode = 0;
	BYTE bReturnCode;
	double nAuthTxTotalAmount = 0.0;
	BYTE MessageSubType2 = 0 ;
	char strRelatedAccountType[50] = {0} ;
    char strProcessingCode[20] = {0} ;
	int nIndex = 0 ;
	int nIndex1 = 0 ;
	char strTemp[256] = {0} ;
	BOOLEAN bTypeFound = false ; 

	/* If Deferred, or void/reversal/override of a deferred... */
      if ( (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION) ||
           (strEqual(strProcessingCode, "70"))                                    ||
           (Auth_Tx.TLF01_details.deferred_term_length[0] != 0x00)                 )
      {
         strcpy (strRelatedAccountType, DEFERRED_ACCOUNT_TYPE) ;
      }
		
		else if (Auth_Tx.MCF01_details.house_card_merchant[0] == 'Y')
			strcpy (strRelatedAccountType, COUNTRY_CLUB_ACCOUNT_TYPE) ;
		
		else if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
			strcpy (strRelatedAccountType, DOLLAR_ACCOUNT_TYPE) ;
		
		else if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
			strcpy (strRelatedAccountType, PESO_ACCOUNT_TYPE) ;

		
		if (strlen(strRelatedAccountType) == 0)
		{
			/******/
			strcpy (strTemp, "RelatedAcc cannot be determined from Tx") ;
			ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp, 
																"Continue_With_CCF02P_Processing",
																GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																REFER_TO_CARD_ISSUER) ;
			return ReturnCode ;
			/******/
		}

		/* Search the related_acct array for the card holder to check if the 
		   the specific related account is present. if not, then it is an error */

		for (nIndex = 0 ; nIndex < 9 && !bTypeFound; nIndex++)
			if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex].type, strRelatedAccountType))
				bTypeFound = true ;

		
	
		/* if the type wasn't found or the nbr was NULL,  return */
		if (!bTypeFound) 
		{
			/*****/
         if ( cPESO_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Peso " );

         else if ( cDOLLAR_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Dollar " );

         else if ( cDEFERRED_ACCT_TYPE == strRelatedAccountType[1] )
            strcpy( strTemp, "Deferred " );

         else
            strcpy( strTemp, "House " );

		 /* The txn currency is USD. if Dollar account not found, convert into peso. debit from the peso account 
		   OR The txn currency is Peso. If Peso account not found debit from the Dollar account if found. 
		   Otherwise log error message. 19-OCt-2009*/
			 if( (!strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP)) &&
				(!strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP)) 
				)
				{
					if(strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)) 
					{
						strcpy(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY);
						strcpy(Auth_Tx.TLF01_details.host_start_time, PESO_COMP);
					}
					else if(strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
					{
						strcpy(Auth_Tx.TLF01_details.currency_code,USDOLLAR_CURRENCY);
						strcpy(Auth_Tx.TLF01_details.host_start_time, USD_COMP);
					}
					ReturnCode = txserviceCadience_Continue_With_CCF02P_For_Balance_Update () ;
						return ReturnCode ;
		 				
				}
				else
				{   
					if(strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP) ||
					   strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP)	
					  )
					  strcpy( strTemp, "Peso and Dollar " );	

					strcat( strTemp, "account not found in card record" );
					ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp, 
																		"Continue_With_CCF02P_Processing",
																		GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																		REFER_TO_CARD_ISSUER) ;
					return ReturnCode ;
				}
			/*****/
		}

		if (strlen(Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr) == 0)
		{
			/*****/
			strcpy (strTemp, "RelatedAccNo invalid for relatedacctype") ;
			ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp, 
																  "Continue_With_CCF02P_Processing",
																  GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																  REFER_TO_CARD_ISSUER) ;
			return ReturnCode ;
			/*****/
		}

		if (strEqual (strRelatedAccountType, DEFERRED_ACCOUNT_TYPE))
		{
			MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_DEFERRED ;
			bTypeFound = false ;

			for (nIndex1 = 0 ; nIndex1 < 9 && !bTypeFound; nIndex1++)
				if (strEqual(Auth_Tx.CCF02P_details.related_acct[nIndex1].type, PESO_ACCOUNT_TYPE))
					bTypeFound = true ;

			if (!bTypeFound) 
			{
				/* If a card holder has a deferred account type, then
					the card holder also has to have a Peso account */
				/*****/
				strcpy (strTemp, "PesoAcc notfound in related for defered") ;
				ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp, 
																      "Continue_With_CCF02P_Processing",
																      GS_TXN_INVALID_CCF02_RELATED_ACCT, 
																      REFER_TO_CARD_ISSUER) ;
				return ReturnCode ;
				/*****/
			
			}
		}
		else
			MessageSubType2 = BALANCE_ACCOUNT_TYPE_IS_OTHER ;      

         strcpy(Auth_Tx.CCF02P_details.related_acct[9].type, Auth_Tx.CCF02P_details.related_acct[nIndex-1].type);
         strcpy(Auth_Tx.CCF02P_details.related_acct[9].nbr,  Auth_Tx.CCF02P_details.related_acct[nIndex-1].nbr);
    
    	// here we have CCF02P data. Get the ACF01 data from the DS calls 
    	bReturnCode = TxDataServer_Get_ACF01_Details (Auth_Tx.CCF02P_details.related_acct[9].type,
                                                  Auth_Tx.CCF02P_details.related_acct[9].nbr,
                                                  MessageSubType2) ; // TF PHANI.
	        	if (bReturnCode == true)
    		           return NO_AUTH_ERROR;
				else 
					return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
   

	return NO_AUTH_ERROR;

}



BYTE txserviceCadience_Continue_With_ACF01_For_Balance_Update(void)
{

	BYTE ReturnCode = 0;
   double nAuthTxTotalAmount = 0.0;
   double nAuthTxRevAmount   = 0.0;
   double nAuthTxOrigAmount  = 0.0;
	BYTE MessageSubType2 = 0 ;
	char strTemp[256] = {0} ;
	BYTE strAmount[50]={0};
	
    
//	if(strEqual(Auth_Tx.ACF01_details.primary_key.account_nbr,Auth_Tx.TLF01_details.card_num))
	if(FieldIsNumeric(Auth_Tx.ACF01_details.primary_key.account_nbr))
    {
        /* Check if we have changed currency code , Them we need to Convert amount*/
		 if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP)     ) &&
        ( strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) ) 
        
        )
   		{
   			if ( !TxAuthorizeCadience_Convert_Peso_to_USD(&nAuthTxTotalAmount,
												&nAuthTxRevAmount,
												&nAuthTxOrigAmount) )
			{
				 sprintf(strTemp, "During balance update,Unable to Convert currency from Peso to Dollor");
				 ReturnCode=txserviceCadience_Generate_Usage_Error_Message(1,strTemp,
											 "Continue_With_ACF01_For_Balance_Update",
											 GS_TXN_INVALID_ACF01_ACCOUNT,
											 INVALID_ACCOUNT_NUMBER);
				return ReturnCode;

			}
			
			/*We have changed currency code, We have to update Earmarked amount*/
			memset (strAmount, 0, sizeof (strAmount)) ;
			Convert_Amt_To_Auth_Tx_String_Format(nAuthTxTotalAmount, strAmount) ;
    		LeftFill_String_With_Character (sizeof Auth_Tx.TLF01_details.product_codes[9].amount-1, strAmount,'0') ;
			strcpy (Auth_Tx.TLF01_details.product_codes[9].amount, strAmount) ;
   		}	
	   else if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP) ) &&
			( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) 
			)
   		{
   			if ( !TxAuthorizeCadience_Convert_USD_to_Peso(&nAuthTxTotalAmount,
												&nAuthTxRevAmount,
												&nAuthTxOrigAmount) )
			{
				 sprintf(strTemp, "During balance update,Unable to Convert currency from Dollor to Peso");
				 ReturnCode=txserviceCadience_Generate_Usage_Error_Message(1,strTemp,
											 "Continue_With_ACF01_For_Balance_Update",
											 GS_TXN_INVALID_ACF01_ACCOUNT,
											 INVALID_ACCOUNT_NUMBER);
				return ReturnCode;

			}
			/*We have changed currency code, We have to update Earmarked amount*/
			memset (strAmount, 0, sizeof (strAmount)) ;
			Convert_Amt_To_Auth_Tx_String_Format(nAuthTxTotalAmount, strAmount) ;
    		LeftFill_String_With_Character (sizeof Auth_Tx.TLF01_details.product_codes[9].amount-1, strAmount,'0') ;
			strcpy (Auth_Tx.TLF01_details.product_codes[9].amount, strAmount) ;
			
   		}// Tf Phani
   		else if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
        ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
        ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
	         (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
	   {
	      TxAuthorizeCadience_Convert_USD_to_Peso(&nAuthTxTotalAmount,
	                                            &nAuthTxRevAmount,
	                                            &nAuthTxOrigAmount);

		  /*We have changed currency code, We have to update Earmarked amount*/
		  memset (strAmount, 0, sizeof (strAmount)) ;
		  Convert_Amt_To_Auth_Tx_String_Format(nAuthTxTotalAmount, strAmount) ;
    	  LeftFill_String_With_Character (sizeof Auth_Tx.TLF01_details.product_codes[9].amount-1, strAmount,'0') ;
		  strcpy (Auth_Tx.TLF01_details.product_codes[9].amount, strAmount) ;	        
	   }
	   else
	   {// at this point we have CCF02  and ACF01 data
			nAuthTxTotalAmount = (strtod (Auth_Tx.TLF01_details.total_amount,    0) / 100.0);
		}
		 if(false == txservice_check_for_ASI_txn())
		 {
    	ReturnCode = txserviceCadience_Perform_Balance_Account_Checks(nAuthTxTotalAmount);
		 }
    	if (NO_AUTH_ERROR == ReturnCode)
    		(void)txserviceCadience_Update_ACF01_Structure();
		else
			return ReturnCode;
        
    }
	else
	{

		txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                          MT_AUTH_RESPONSE, 
                                          0, 
                                          0) ;

		 sprintf(strTemp, "Account Num is not numeric");
		 ReturnCode=txserviceCadience_Generate_Usage_Error_Message(1,strTemp,
		                             "Continue_With_ACF01_For_Balance_Update",
									 GS_TXN_INVALID_ACF01_ACCOUNT,
									 INVALID_ACCOUNT_NUMBER);
		return ReturnCode;
	}
/* Removed the else case and moved to different function txserviceCadience_Continue_With_ACF01_For_Not_Found */

	return NO_AUTH_ERROR;
}// End of code changes for Cadience Standin processing Ravikumar K N - ThoughtFocus Technologies

BYTE txserviceCadience_Continue_With_ACF01_For_Not_Found(void)
{
	char Buffer[250];
	BYTE ReturnCode = 0;
	double nAuthTxTotalAmount = 0.0;
	BYTE MessageSubType2 = 0 ;
	
    

        memset(&Buffer, 0, sizeof(Buffer));
/* Copy response_code,general status and response_text in Auth_Tx structure while sending to Originator - Girija Y */
		strcpy(Auth_Tx.TLF01_details.response_code,"14");	
		strcpy(Auth_Tx.TLF01_details.response_text,"Invalid account number");
		Auth_Tx.TLF01_details.general_status = GS_TXN_DROPPED;

        sprintf( Buffer,
    			"txserviceCadience_Continue_With_ACF01_For_Balance_Update: The ACF01 record does not \
                exists for the card number: %s, transaction id: %s transaction amount: %s, STAN: %s, ",
    				Auth_Tx.TLF01_details.card_num, Auth_Tx.TLF01_details.primary_key.transaction_id,
    				Auth_Tx.TLF01_details.total_amount, Auth_Tx.TLF01_details.sys_trace_audit_num);
    			txcntl_log_message( 2, 1, Buffer, "txserviceCadience_Continue_With_ACF01_For_Not_Found", 0 );
    			printf( "%s\n", Buffer );
    			txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
    														     MT_AUTH_RESPONSE, 
    														     0, 
    														     0);
				return GS_TXN_ACF01_NOT_FOUND;


} /* Phani, Girija Y - ThoughtFocus */


INT txservice_Check_If_CUP_Transaction (void)
{
	if ((0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(Auth_Tx.BIN01_details.card_family,CARD_CUP,CARD_TYPE_LEN)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_Diners_Transaction (void)
{
	if ((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_DINERS)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT txservice_Check_If_VISA_Transaction (void)
{
	if ( Auth_Tx.TLF01_details.card_num[0]=='4' ||
		(0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA)||
		0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_DUAL)))

	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT txservice_Check_If_MC_Transaction (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,Auth_Tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	Auth_Tx.TLF01_details.card_num[0]=='5'||
		(0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD)))
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT txservice_Check_If_JCB_Transaction (void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   (Auth_Tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else if(0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT txservice_Check_If_AMEX_Transaction (void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '4') ||
		(Auth_Tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else if(0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

/**************************************************************************
NAME:			txservice_Check_If_MC_DE_48_Tag_63_Trace_ID_Present
DESCRIPTION:	This function checks for Mastercard DE48 filed is contaning
				tag 63 or not
INPUTS:			DE 48 len and DE 48 data
OUTPUTS:		None
RETURNS:        true  if DE48 filed is contains tag 63
                false if DE48 filed is not contains tag 63
AUTHOR:         Abhishe Verma
**************************************************************************/

INT txservice_Check_If_MC_DE_48_Tag_63_Trace_ID_Present( int MC_DE48_len, char *src )
{
   INT  ret_val = false;
   INT  sub_field_len;
   INT  MainIndex;
   INT  TotalLen;
   CHAR tag[3] = {0};
   CHAR len[3] = {0};

   TotalLen = MC_DE48_len;

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<MC_FIELD48_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;
	 /* Other subfields are variable length. */
	 memset( len, 0x00, sizeof(len) );
	 memcpy( len, &src[MainIndex], 2 );
	 sub_field_len = atoi( len );
	 MainIndex += 2;

	 /* Get the data from these subfields. */
	if ( 0 == strncmp(tag, MC_DE48_TAG63, 2) )
	 {
		return true;
	 }
	 MainIndex += sub_field_len;
   }
   return( ret_val );
}  /* End txservice_Check_If_MC_DE_48_Tag_63_Trace_ID_Present */


/*******************************************************************************
 * NAME:        txservice_ATP_STIP_Processing_for_Incoming_Issuing_Transactions
 *
 * DESCRIPTION: This function identify function that needs to be called
 *                         to prepare the POS Data
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/

INT txservice_ATP_STIP_Processing_for_Incoming_Issuing_Transactions(void)
{
	INT ret_code = true;

	if(true == txservice_Check_If_AMEX_Transaction())
	{
		ret_code = txservice_Check_If_AMEX_ISSUING_EMV_transaction_qualifies_For_processing();
	}
	else if(true == txservice_Check_If_VISA_Transaction())
	{
		ret_code = txservice_Check_If_VISA_ISSUING_EMV_transaction_qualifies_For_processing();
	}

	else if(true == txservice_Check_If_MC_Transaction())
	{
		ret_code = txservice_Check_If_MC_ISSUING_EMV_transaction_qualifies_For_processing();
	}

	else if(true == txservice_Check_If_CUP_Transaction())
	{
		ret_code = txservice_Check_If_CUP_ISSUING_EMV_transaction_qualifies_For_processing();
	}

	return ret_code;
}


/**************************************************************************
NAME:			txservice_Check_If_AMEX_ISSUING_EMV_transaction_qualifies_For_processing
DESCRIPTION:	This function checks for Amex Issuing EMV transaction qualifies For processing

INPUTS:			None
OUTPUTS:		None
RETURNS:        true  if transaction details are alline with Pos Entry mode, service code and
			    terminal capability
                false if transaction details are not alline with Pos Entry mode, service code and
			    terminal capability
AUTHOR:         Abhishe Verma
**************************************************************************/

INT txservice_Check_If_AMEX_ISSUING_EMV_transaction_qualifies_For_processing()
{
	CHAR service_code[4] = {0};
	CHAR error_msg[256] = {0};
	CHAR POS_terminal_capability[2] = {0};
	CHAR POS_entry_mode[2] = {0};

	/****************************************************************************************
	 *
	|______________________|______________|___________|________________________|____________________________________|
	|Bit 22 Position 7     | Card Service |Chip data  |POS Terminal Capability | ATP STIP Handling/				    |
	|					   | Code		  |Field 55   |(Bit 22 Position 1)     | Response							|
	|______________________|______________|___________|________________________|____________________________________|
	|2 â€“ magnetic stripe   |              |  Not      |5 â€“ ICC (intergrated    |Field 39 = 12						|
	|	 read      	       |	201	  	  |  present  |circuit card)/EMV       |Response test="Invalid transaction" |
	|W â€“ magnetic stripe   |    202       |           |                        |                                    |
	|read with PCSC        |              |           |                        |                                    |
	|______________________|______________|           |                        |                                    |
	|0 â€“ unspecified       | Not          |           |                        |                                    |
	|6 â€“ key entered       | present      |           |                        |                                    |
	|S â€“ PAN key entry     |			  |	          |                        |                                    |
	|______________________|______________|___________|________________________|____________________________________|

	*/

	if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
	{
		// Let us get the service code .
		if((true == txservice_get_service_code(  service_code,  error_msg )))
		{
			strncpy(Auth_Tx.TLF01_details.service_code,service_code,(sizeof(service_code) -1));
		}
	}
	else
	{
		strncpy(service_code,Auth_Tx.TLF01_details.service_code,(sizeof(service_code) -1));
	}

	// Bit 22 Positio 1 indicates POS Terminal capability
	strncpy(POS_terminal_capability,Auth_Tx.TLF01_details.product_codes[16].amount,1);

	// Bit 22 Position 7 indicate Pos Entry mode
	strncpy(POS_entry_mode,Auth_Tx.TLF01_details.product_codes[16].amount + 6,1);

	if ( (0 == strncmp(service_code,"201",3)) || ( 0 == strncmp(service_code,"202",3)))
	{
		if(POS_terminal_capability[0] == '5')
		{
			if((POS_entry_mode[0] == '2') ||(POS_entry_mode[0] == 'W'))
			{

				return false;
			 }
		}
	}
	else if( 0 == strlen(service_code))
	{
		if(POS_terminal_capability[0] == '5')
		{
			if( POS_entry_mode[0] == '0' ||
			    POS_entry_mode[0] == '5'||
				POS_entry_mode[0] == '9')
			{
				return false;
			 }
		}
	}
    return true;
}

/**************************************************************************
NAME:			txservice_Check_If_VISA_ISSUING_EMV_transaction_qualifies_For_processing
DESCRIPTION:	This function checks for Visa Issuing EMV transaction qualifies For processing

INPUTS:			None
OUTPUTS:		None
RETURNS:        true  if transaction details are alline with Pos Entry mode, service code and
			    terminal capability
                false if transaction details are not alline with Pos Entry mode, service code and
			    terminal capability
AUTHOR:         Abhishe Verma
**************************************************************************/

INT txservice_Check_If_VISA_ISSUING_EMV_transaction_qualifies_For_processing()
{
	CHAR service_code[4] = {0};
	CHAR error_msg[256] = {0};
	CHAR POS_terminal_capability[2] = {0};
	CHAR POS_entry_mode[5] = {0};
	CHAR chip_condition_code[2] = {0};

	/****************************************************************************************
	 * 01 â€“ manual
	 *
	|______________________|______________|___________|__________________________|____________________________________|
	|	Field 22		   | Card Service |  Chip data|POS Terminal Capability   | ATP STIP Handling/				  |
	|					   | Code		  |Field 55   |(F60.2.2)& Chip Condition | Response							  |
	|                      |              |           | Code (F60.2.3)           |                                    |
	|______________________|______________|___________|__________________________|____________________________________|
	|02 â€“ magnetic stripe  |              |  Not      |F60.2.2 = 5, 8 (chip 	 |Field 39 = 12						  |
	|	  read             |	201	  	  |  present  |capable/EMV;capable to    |Response test="Invalid transaction" |
	|90 â€“ magnetic stripe  |    202       |           |read Visa PayWave/EMV     |                                    |
	|     read 		       |			  |           |contactless)              |                                    |
	|______________________|______________|			  |F60.2.3 = 0 or not present|                                    |
	|01 â€“ manual           |  Not         |           |                          |                                    |
	|                      |  present     |           |                          |                                    |
	|______________________|______________|___________|__________________________|____________________________________|
	*/

	if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
	{
		// Let us get the service code.
		if((true == txservice_get_service_code(  service_code,  error_msg )))
		{
			strncpy(Auth_Tx.TLF01_details.service_code,service_code,(sizeof(service_code) -1));
		}
	}
	else
	{
		strncpy(service_code,Auth_Tx.TLF01_details.service_code,(sizeof(service_code) -1));
	}

	// Bit 22 indicate Pos Entry mode
	strncpy(POS_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,3);

	// Bit 60 Positio 2 indicates POS Terminal capability
	strncpy(POS_terminal_capability,Auth_Tx.TLF01_details.product_codes[16].amount +1 ,1);

	// Bit 60.2.3 Chip Condition code
	strncpy(chip_condition_code,Auth_Tx.TLF01_details.product_codes[16].amount+2,1);
	if ( 0 ==strlen(chip_condition_code))
	{
		strncpy(chip_condition_code,"0",1);
	}

	if ( (0 == strncmp(service_code,"201",3)) || (0 == strncmp(service_code,"202",3)))
	{
		if(((POS_terminal_capability[0] == '5')|| (POS_terminal_capability[0] == '8')) &&
			(0 == strncmp(chip_condition_code,"0",1)))
		{
			if( (0 == strncmp(POS_entry_mode,"002",3))||(0 == strncmp(POS_entry_mode,"02",2))||
				(0 == strncmp(POS_entry_mode,"009",3))||(0 == strncmp(POS_entry_mode,"09", 2))||
				(0 == strncmp(POS_entry_mode,"90", 2)))
				{
					return false;
				}
		}
	}
	else if( 0 == strlen(service_code))
	{
		// Service code is not present

		if(((POS_terminal_capability[0] == '5')|| (POS_terminal_capability[0] == '8')) &&
		(0 == strncmp(chip_condition_code,"0",1)))
		{
			if( 0 == strncmp(POS_entry_mode,"000",3))
			{
				return false;
			}
		}

	}

	return true;
}

/**************************************************************************
NAME:			txservice_Check_If_MC_ISSUING_EMV_transaction_qualifies_For_processing
DESCRIPTION:	This function checks for MC Issuing EMV transaction qualifies For processing

INPUTS:			None
OUTPUTS:		None
RETURNS:        true  if transaction details are alline with Pos Entry mode, service code and
			    terminal capability
                false if transaction details are not alline with Pos Entry mode, service code and
			    terminal capability
AUTHOR:         Abhishe Verma
**************************************************************************/

INT txservice_Check_If_MC_ISSUING_EMV_transaction_qualifies_For_processing()
{

	CHAR service_code[4] = {0};
	CHAR error_msg[256] = {0};
	CHAR POS_terminal_capability[2] = {0};
	CHAR POS_entry_mode[5] = {0};

	/****************************************************************************************
	 *
	|_______________________|______________|___________|________________________|____________________________________|
	|Bit 22 Position 7      | Card Service |Chip data  |POS Terminal Capability | ATP STIP Handling/				 |
	|					    | Code		   |Field 55   |(Bit 22 Position 1)     | Response							 |
	|_______________________|______________|___________|________________________|____________________________________|
	|02 â€“ PAN auto entry via|              |  Not      |3 â€“ contactless M/Chip; |Field 39 = 12						 |
	|      magnetic stripe	|	 201	   |  present  |  supports EMV/magstripe|Response test="Invalid transaction" |
	|90 â€“ magnetic stripe   |    202       |           |  contactless           |                                    |
	|     read              |	           |		   |5 â€“ EMV and magstripe   |                                    |
	|                       |              |           |    capable             |                                    |
	|                       |              |           |8 â€“ EMV,magstripe reader|                                    |
	|                       |              |		   |	, and key entry     |                                    |
	|                       |              |           |9 â€“ EMV only            |                                    |
	|                       |              |           |                        |                                    |
	|_______________________|______________|           |                        |                                    |
	| 00 â€“ unspecified      |  Not         |           |                        |                                    |
	|                       |  Present     |           |                        |                                    |
	|_______________________|______________|___________|________________________|____________________________________|

	*/

	if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
	{
		// Let us get the service code.
		if((true == txservice_get_service_code(  service_code,  error_msg )))
		{
			strncpy(Auth_Tx.TLF01_details.service_code,service_code,(sizeof(service_code) -1));
		}
	}
	else
	{
		strncpy(service_code,Auth_Tx.TLF01_details.service_code,(sizeof(service_code) -1));
	}
	// Bit 22 indicate Pos Entry mode
	strncpy(POS_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,4);

	// Bit 61 subfield 11 indicates POS Terminal capability
	strncpy(POS_terminal_capability,Auth_Tx.TLF01_details.product_codes[16].amount+10,1);

	if ( (0 == strncmp(service_code,"201",3)) || (0 == strncmp(service_code,"202",3)))
    {
	    if((POS_terminal_capability[0] == '3')||
		   (POS_terminal_capability[0] == '5')||
		   (POS_terminal_capability[0] == '8')||
		   (POS_terminal_capability[0] == '9'))
		 {
			 if((0 == strncmp(POS_entry_mode,"002",3))||(0 == strncmp(POS_entry_mode,"02",2))||
				(0 == strncmp(POS_entry_mode,"009",3))||(0 == strncmp(POS_entry_mode,"90", 2))||
				(0 == strncmp(POS_entry_mode,"09",2)))
				 {
					 return false;
				 }
		 }
	}
	else if(0 ==strlen(service_code))
	{
		if((POS_terminal_capability[0] == '3')||
		   (POS_terminal_capability[0] == '5')||
		   (POS_terminal_capability[0] == '8')||
		   (POS_terminal_capability[0] == '9'))
		 {
			 if(0 == strncmp(POS_entry_mode,"000",3))
			 {
				 return false;
			 }
		 }
	}

    return true;
}

/**************************************************************************
NAME:			txservice_Check_If_CUP_ISSUING_EMV_transaction_qualifies_For_processing
DESCRIPTION:	This function checks for CUP Issuing EMV transaction qualifies For processing

INPUTS:			None
OUTPUTS:		None
RETURNS:        true  if transaction details are alline with Pos Entry mode, service code and
			    terminal capability
                false if transaction details are not alline with Pos Entry mode, service code and
			    terminal capability
AUTHOR:         Abhishe Verma
**************************************************************************/

INT txservice_Check_If_CUP_ISSUING_EMV_transaction_qualifies_For_processing()
{
	CHAR service_code[4] = {0};
	CHAR error_msg[256] = {0};
	CHAR POS_terminal_capability[2] = {0};
	CHAR chip_condition_code[2] = {0};
	CHAR POS_entry_mode[5] = {0};

	/****************************************************************************************
	 *
	|______________________|______________|___________|__________________________|____________________________________|
	|	Field 22		   | Card Service |  Chip data|POS Terminal Capability   | ATP STIP Handling/				  |
	|					   | Code		  |Field 55   |(F60.2.2)& Chip Condition | Response							  |
	|                      |              |           | Code (F60.2.3)           |                                    |
	|______________________|______________|___________|__________________________|____________________________________|
	|02 â€“ magnetic stripe  |              |  Not      |F60.2.2 = 5, 6 (EMV/chip	 |Field 39 = 12						  |
	|	   read            |	201	  	  |  present  |capable,optional magnetic |Response test="Invalid transaction" |
	|90 â€“ magnetic stripe  |    202       |           |stripe; cannot read       |                                    |
	|     read		       |			  |           |contactless IC/EMV card)  |                                    |
	|                      |			  |	          |F60.2.3 = 0 or not present|                                    |
	|______________________|______________|           |                          |                                    |
	|00 â€“ unspecified      |   Not        |           |                          |                                    |
	|                      |   present    |           |                          |                                    |
	|______________________|______________|___________|__________________________|____________________________________|

	*/

	if(0 == strlen(Auth_Tx.TLF01_details.service_code) )
	{
		// Let us get the service code.
		if((true == txservice_get_service_code(  service_code,  error_msg )))
		{
			strncpy(Auth_Tx.TLF01_details.service_code,service_code,(sizeof(service_code) -1));
		}
	}
	else
	{
		strncpy(service_code,Auth_Tx.TLF01_details.service_code,(sizeof(service_code) -1));
	}
	// Bit 22 indicate Pos Entry mode
	strncpy(POS_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,4);

	// Bit 60.2.2 POS Terminal capability
	strncpy(POS_terminal_capability,Auth_Tx.TLF01_details.product_codes[16].amount+1,1);

	// Bit 60.2.3 Chip Condition code
	strncpy(chip_condition_code,Auth_Tx.TLF01_details.product_codes[16].amount+2,1);
	if ( 0 ==strlen(chip_condition_code))
	{
		strncpy(chip_condition_code,"0",1);
	}

	if ( (0 == strncmp(service_code,"201",3)) || (0 == strncmp(service_code,"202",3)))
	{
		if(((POS_terminal_capability[0] == '5')|| (POS_terminal_capability[0] == '6')) &&
			(0 == strncmp(chip_condition_code,"0",1)))
		 {
			 if((0 == strncmp(POS_entry_mode,"002",3))||(0 == strncmp(POS_entry_mode,"02",2))||
				(0 == strncmp(POS_entry_mode,"009",3))||(0 == strncmp(POS_entry_mode,"09",2))||
				(0 == strncmp(POS_entry_mode,"90",2)))
				 {
					 return false;
				 }
		 }
	}
	else if (0 == strlen(service_code))
	{
		if(((POS_terminal_capability[0] == '5')|| (POS_terminal_capability[0] == '6')) &&
			(0 == strncmp(chip_condition_code,"0",1)))
		 {
			 if(0 == strncmp(POS_entry_mode,"000",3))
			 {
				 return false;
			 }
		 }
	}
    return true;
}


INT txservice_get_service_code( pCHAR service_code, pCHAR error_msg )
{
	pBYTE  field_separator = NULL_PTR;
	pBYTE  field_separator1 = NULL_PTR;
	INT    ret_val = false;
	INT    card_num_len;
	INT    track2_idx;

	if ( Auth_Tx.TLF01_details.track2[0] != 0x00 )
	{
		/* Locate the field separator. */
		field_separator = Auth_Tx.TLF01_details.track2;
		for( track2_idx=0; track2_idx <= MAX_CARD_NUM_LEN;
		track2_idx++,field_separator++)
		{
			if ( !isdigit( *field_separator ) )
				break;
		}

		if ( track2_idx > MAX_CARD_NUM_LEN )
		{
			strcpy( error_msg,"Unable to parse track2.  Cannot find field separator.");
		}
		else
		{
			/* Get the service code. */
			memcpy( service_code, field_separator+5, 3 );
			ret_val = true;
		}
	}

	else if ( Auth_Tx.TLF01_details.track1[0] != 0x00 )
	{
		/* Locate the field separator. */

		field_separator=strchr(Auth_Tx.TLF01_details.track1,'^');
		if((field_separator==NULL) ||((((int)field_separator) - ((int)Auth_Tx.TLF01_details.track1)) > MAX_CARD_NUM_LEN))
		{
			strcpy( error_msg,"Unable to parse track1.  Cannot find field separator.");
		}
		else
		{
			field_separator1=strchr(field_separator+1,'^');

			if((field_separator1==NULL )||((field_separator1-(field_separator+1))>MAX_CARD_HOLDER_LEN))
			{
				strcpy( error_msg,"Unable to parse track1.  Cannot find field separator.");
			}
		}
		if(*field_separator1 == '^')
		{
			memcpy( service_code, field_separator1+5, 3 );

			ret_val = true;
		}

	}
	return( ret_val );
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void txcntl_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  txcntl_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   txcntl_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(txcntl_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  txcntl_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(txcntl_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  txcntl_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(txcntl_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(txcntl_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(txcntl_error_warning_file_path) > 0) &&
	  (strlen(txcntl_module_error_warning_file_name)> 0))
   {
	   txcntl_create_Error_Filename();
   }
}

INT txcntl_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR  Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	CHAR  tempcard[21] = {0};
	CHAR  masked_cardnum[21] = {0};
	char  *cardIndex = NULL ;
	FILE  *fp;
	UINT  file_size = 0 ;
	char  buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(txcntl_Erro_warning_Filename);
    path_len = strlen(txcntl_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strncmp(&txcntl_Erro_warning_Filename[len-4], current_mmdd,4) )
    {
		/* Now create the new filename.*/
    	txcntl_create_Error_Filename();
    }
	if((fp = fopen(txcntl_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fseek (fp, 0, SEEK_END);
		file_size=ftell (fp);
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void txcntl_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( txcntl_Erro_warning_Filename, txcntl_error_warning_file_path );
    strcat( txcntl_Erro_warning_Filename, txcntl_module_error_warning_file_name );
	strcat( txcntl_Erro_warning_Filename, ".EW." );
    strncat(txcntl_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(txcntl_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(txcntl_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         txcntl_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void txcntl_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;
   CHAR  appname[512] = {0};

   GetAppName (appname) ;
   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   if (dest == 2)
   {
	   eventlog_flag = 1;
   }
   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   txcntl_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   if(details == 1)
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
			   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 Auth_Tx.TLF01_details.terminal_id,
										 Auth_Tx.TLF01_details.card_num,
										 Auth_Tx.TLF01_details.merchant_id );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
   }

   return;
}

BOOLEAN txservice_Check_If_DCI_issuing_transaction(void)
{
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,ncdci_issuing_link_name,DINERS_ISSUING_MODULE_COMP_LEN) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 *
 *  NAME:         txservice_Check_If_DCI_pin_block_exists
 *
 *  DESCRIPTION:  This function determines if a pin block exists.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if pin exists, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT txservice_Check_If_DCI_pin_block_exists()
{
   INT  ret_val = true;

   if ( 0x00 == Auth_Tx.TLF01_details.pin_block[0] &&
		0x00 == Auth_Tx.TLF01_details.pin_block[1] )
   {
      ret_val = false;
   }
   else if (txservice_Check_If_blank_string(Auth_Tx.TLF01_details.pin_block,
                         sizeof(Auth_Tx.TLF01_details.pin_block)-1) )
   {
      ret_val = false;
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         txservice_Check_If_blank_string
 *
 *  DESCRIPTION:  This function determines if a string consists of all blanks.
 *
 *  INPUTS:       max_len - Number of bytes to check as blank or not
 *                astr    - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if 'astr' is all blank, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT txservice_Check_If_blank_string(pCHAR astr, INT max_len)
{
   INT  i = 0;
   INT  ret_val = true;

   if (0 == strlen(astr))
      ret_val = true;

   else
   {
      for(i = 0;i < max_len; i++)
      {
         if (astr[i] != ' ')
         {
            ret_val = false;
            break;
         }
      }
   }
   return( ret_val );
}

INT txservice_Check_If_EMV_Trn()
{
	if ( (Auth_Tx.EMV_details.emv_block[0] == 0x00) &&
		 (Auth_Tx.EMV_details.emv_block[1] == 0x00)  )
	{
		return false;
	}
	else
	{
		return true;
	}
}

INT txservice_Check_If_ECOM_TRAN(void)
{
	if ( ((strcmp(Auth_Tx.industry_code,"700")== 0) &&
		  (strcmp(Auth_Tx.TLF01_details.nfi_end_date,"1") == 0))||
		 (true == txservice_Check_If_CUP_ECOM()) ||
		 (true == txservice_Check_If_CUP_3DS2()) ||
		 (true == txservice_Check_If_DCI_ECOM_Productbuy()))
	{
		return true;
	} /* length check of int_net_facility_len to handle 3D secure transaction without CAVV
	and with CVV/CVV2 */
	else if( ((Auth_Tx.TLF01_details.handler_queue,"dcpiso",6)   &&
			    strlen(Auth_Tx.int_net_facility_len) != 0 		 &&
				txservice_Is_AsiaPay_Transaction() == true)      &&
			 ((true == txservice_Check_If_JCB_Transaction()) 	 ||
			  (true == txservice_Check_If_AMEX_Transaction()) 	 ||
			  (true == txservice_Check_If_MC_Transaction()) 	 ||
			  (true == txservice_Check_If_VISA_Transaction()))  )
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CUP_ECOM(void)
{
	if( true == txservice_Check_If_CUP_Transaction())
	{
		if ( 0 == strncmp( Auth_Tx.TLF01_details.product_codes[3].quantity,
						   CUP_ECOM_TERM_TYPE,
						   CUP_DE60_TERM_TYPR_LEN))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CAVV_Key_Indicator_set_for_BDO_CAVV(void)
{
	char CAVV_indicator [3] = {0};

	strncpy(CAVV_indicator,
			Auth_Tx.future_use_4 + DINERS_DE122_CAVV_KEY_INDICATOR_POS,
			DINERS_DE122_CAVV_KEY_INDICATOR_LEN);
	if( (0 == strncmp(CAVV_indicator,
					  DINERS_DE122_CAVV_KEY_SET_1_VALUE,
					  DINERS_DE122_CAVV_KEY_INDICATOR_LEN))||
		(0 == strncmp(CAVV_indicator,
					  DINERS_DE122_CAVV_KEY_SET_2_VALUE,
					  DINERS_DE122_CAVV_KEY_INDICATOR_LEN)))
	{
		return true;
	}
	else
	{
		return false;
	}

}


INT txservice_Check_If_DCI_ECOM_Productbuy(void)
{
	if(true ==  txservice_Check_If_Diners_Transaction ())
	{
		if( DINERS_DE22_POST_5_INTERNET == Auth_Tx.TLF01_details.product_codes[16].amount[4] &&
			(true == txservice_Check_If_CAVV_Key_Indicator_set_for_BDO_CAVV()))
		{
			return true;
		}
		else if(true == txservice_Check_If_DCI_Asiapay_Ecommerce_transaction())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_DCI_Asiapay_Ecommerce_transaction (void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
					 DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					 DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_Check_from_DCI_Protectbuy_3DS2_txn(void)
{
	if ( 0 != strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, "00", 2) &&
		(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncdci", 5) ||
		 0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncdci3", 6)) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_DE122_AuthnResultCode_value_to_directly_decline_txn(void)
{
	 if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DCI_3DS2_CAVV_AUTHN_NOT_PERFORMED, 2) ||
		 0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DCI_3DS2_CAVV_AUTHN_FAILED, 2))
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}

BOOLEAN txservice_Check_DE122_AuthnResultCode_value_to_skip_CAVV_validation(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DCI_3DS2_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION, 2) ||
		0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DCI_3DS2_CAVV_AUTHN_ATTEMPT_ACS_NOT_AVAILABLE, 2))
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }

}

BOOLEAN txservice_DCI_get_CAVV_from_DESEncrypn(pBYTE block)
{
	//BYTE block = "29547599606445197993099000000000";
	BYTE blockA[100];
	BYTE blockB[100];
	BYTE keyA [100];
	BYTE keyB [100];
	char gen_CAVV[4];
	INT buffer_len;
	INT i, j, k;
	BYTE buf_copy[100];
	char HexBuffer[100]={0};
	char HexBuffer1[100]={0};
	char HexBuffer2[100]={0};
	char HexBuffer3[100]={0};
	char HexBuffer4[100]={0};
	char HexBuffer5[100]={0};
	BYTE block1[100] = {0};
	BYTE block2[100] = {0};

	memset(blockA, 0x00, sizeof(blockA));
	memset(blockB, 0x00, sizeof(blockB));
	memset(keyA, 0x00, sizeof(keyA));
	memset(keyB, 0x00, sizeof(keyB));

	strncpy(blockA, block, 64);
	hexchar2word(block1,blockA);
	strncpy(blockB, block+64, 64);
	hexchar2word(block2,blockB);
	string2hexString(block2,HexBuffer5);

	static WORD hard_key [16]   = {0};
	static WORD hard_key1[16]   = {0};

	strncpy(keyA, Auth_Tx.CAF01_details.csck, 32);
	strncat(keyA+32, Auth_Tx.CAF01_details.cvk[3].cvkA, 16);
	strncat(keyA+48, Auth_Tx.CAF01_details.cvk[3].cvkB, 16);
	hexchar2word(hard_key, keyA);
	hexchar2word(hard_key1, keyA);

		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer);


		buffer_len = strlen(block1);

		for (i=0; i< strlen(block1); i++)
		{
			block1[i]^= block2[i];
		}
		string2hexString(block1,HexBuffer1);
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer2);
		buffer_len = strlen(block1);

		i = des_decryption_ex1(block1, buffer_len, hard_key1, 0);
		string2hexString(block1,HexBuffer3);
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer4);
		buffer_len = strlen(block1);

		memset(buf_copy, 0x00, sizeof(buf_copy));

		byte2hexchar(buf_copy, block1, buffer_len);

		memset(block1, 0x00, sizeof(block1));
		GetOnlyNumber(buf_copy, block1);

		strncpy(gen_CAVV,block1, 3);
		if (0 == strncmp(gen_CAVV,&Auth_Tx.TLF01_details.product_codes[3].amount[7], 3))
		{
			return true;
		}
		else
		{
			return false;
		}
}

void txservice_DCI_prepare_data_3DS2(char* Input_data)
{
	char append[128] = {0};
	memset(append, '0', sizeof(append));

	//card num
	strcpy(Input_data, Auth_Tx.TLF01_details.card_num);
	//4 digits ATN
	strncat(Input_data, &Auth_Tx.TLF01_details.dcf01_retired_cwk[12], 4);
	//Authn Res code
	strncat(Input_data, &Auth_Tx.TLF01_details.dcf01_retired_cwk[2], 2);
	//Scnd Fctr Authn Res code
	strncat(Input_data, &Auth_Tx.TLF01_details.dcf01_retired_cwk[4], 2);
	//Append 0's to make 128 bit
	strncat(Input_data, append, 106);

}

BOOLEAN txservice_Check_DCI_3DS2_self_validation(void)
{
	char Input_data[129] = {0};

	txservice_DCI_prepare_data_3DS2(&Input_data);
	if (true == txservice_DCI_get_CAVV_from_DESEncrypn(Input_data))
	{
		return true;
	}
	else
	{
		return false;
	}
}
	/* DCI req 1 Oct Mndt 2022 */
BOOLEAN txservice_Check_DCI_3DS2_SF_Authn(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_STAT_PASS_CODE, 2)||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_SMS_OTP, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_RDR_OTP, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_APP_OTP, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_OTP, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_KBA, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_OOB_BIOMETRIC, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_OOB_APP_LOGIN, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_OOB, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_OTHER_AUTHN, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_PUSH_CFRM, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_DECOUPLED, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_WEB_AUTHN, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_SPC, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_CARD_BEHVRL_BIOMETRIC, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_RBA, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_ATMPT_SERVER, 2) ||
			0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+4, DINERS_DE122_SECOND_FAC_AUTH_3DS2_FRCTNLES_FLOW_RBA_2, 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}
BOOLEAN txservice_Check_DCI_3DS2_txn_for_fully_authentication(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+0, DINERS_DE122_AUTH_DATA_TYPE_PRODUCTBUY_VALUE, 1) &&
		0 == strncmp(Auth_Tx.TLF01_details.profile, DINERS_DE122_ECI_SEC_ECOM_TRAN_VALUE, DINERS_DE122_ECI_VALUE_LEN) &&
		0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk+2, DCI_3DS2_CAVV_SUCCESSFULL_AUTHN, DINERS_DE122_3D_SEC_AUTH_RESULT_CODE_VALUE_LEN) &&
		true == txservice_Check_DCI_3DS2_SF_Authn() &&
		true == txservice_Check_If_CAVV_Key_Indicator_set_for_BDO_CAVV())
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_DCI_3DS2_txn_for_authentication_attempt(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.profile, DINERS_DE122_ECI_NON_AUTH_SEC_ECOM_TRAN_WITH_MID_CAPABLE_VALUE, DINERS_DE122_ECI_VALUE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CUP_ECOM_AUTH_MODE(void)
{
	if ( 0 == strncmp(Auth_Tx.TLF01_details.profile,
					  CUP_ECOM_AUTH_MODE,
					  CUP_ECOM_MODE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CUP_ECOM_NON_AUTH_MODE(void)
{
	if ( (0 == strncmp(Auth_Tx.TLF01_details.profile,
					   CUP_ECOM_NON_AUTH_MODE,
					   CUP_ECOM_MODE_LEN)) ||
		 (0 == strncmp(Auth_Tx.TLF01_details.profile,
					   CUP_ECOM_DEFAULT_MODE,
					   CUP_ECOM_MODE_LEN)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CUP_Account_Varification_Trn(void)
{
	if ( Auth_Tx.TLF01_details.resp_source[2] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Validate_CUP_ECOM_TRAN(void)
{
	char  mobile_num[31] = {0};
	BYTE  ReturnCode = 0 ;
	char  strTemp [252] = {0};

	// Check if transaction has valid authenticate mode.
	if(false == txservice_Check_If_CUP_ECOM_AUTH_MODE() &&
	   false == txservice_Check_If_CUP_ECOM_NON_AUTH_MODE())
	{
		 sprintf (strTemp, "Invalid ECI value received from UPI %s",
				 	 	 	 Auth_Tx.TLF01_details.profile) ;
		 ReturnCode = txservice_Generate_Usage_Error_Message (1, strTemp,
															  "txservice_Validate_CUP_ECOM_TRAN",
															  GS_TXN_TRANSACTION_NOT_ALLOWED,
															  DO_NOT_HONOR );
		 return ReturnCode ;

	}


	if( !ValidateCardNumber())
	{
		 sprintf (strTemp, "Invalid card number as Card number (%s) failed Mod-10 check for UPI ECOM transaction",
						Auth_Tx.TLF01_details.card_num) ;
		 ReturnCode = txservice_Generate_Usage_Error_Message (1, strTemp,
															  "txservice_Validate_CUP_ECOM_TRAN",
															  GS_TXN_TRANSACTION_NOT_ALLOWED,
															  INVALID_ACCOUNT_NUMBER );
		 return ReturnCode ;
	}

	if(true == txservice_Check_If_Validate_EXPIRY_NUM_FOR_CUP_ECOM_TRAN())
	{
		if(0 != strncmp(Auth_Tx.CCF02P_details.expire_date,
						Auth_Tx.TLF01_details.exp_date,
						CUP_DE14_SIZE))
		{
			// Expiry date failed drop transaction processing and response back to CUP
			 ReturnCode = txservice_Generate_Usage_Error_Message (1, "Expiry date Mismatch",
																  "txservice_Validate_CUP_ECOM_TRAN",
																  GS_TXN_INVALID_EXPIRY_DATE,
																  EXPIRED_CARD );
			 return ReturnCode ;
		}
	}
	else if (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
	{
		// Expiry date is optional in case of balance inquiry.
	}
	else if (true == 	txservice_Check_If_CUP_Account_Varification_Trn())
	{
		// Its Account varification tranasction
	}
	else
	{
		// Expiry date is missing in transaction
		 ReturnCode = txservice_Generate_Usage_Error_Message (1, "Expiry date is not Present in Transaction",
															  "txservice_Validate_CUP_ECOM_TRAN",
															  GS_TXN_INVALID_EXPIRY_DATE,
															  EXPIRED_CARD );
		 return ReturnCode ;
	}

	if(true == txservice_Check_If_CUP_ECOM_AUTH_MODE() ||
	   Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
	{
		if(true == txservice_Check_If_CUP_Mobile_number_Assoisated_with_Card(&mobile_num))
		{
			if(true == txservice_Check_If_Validate_Mobile_num_FOR_CUP_ECOM_TRAN())
			{
				if(false == txservice_Validate_coustmer_mobile_number(&mobile_num))
				{

					// Mobile number validation failed Wrong mobile number
					ReturnCode = txservice_Generate_Usage_Error_Message (1, "Wrong Mobile number",
																		 "txservice_Validate_CUP_ECOM_TRAN",
																		 GS_TXN_NO_WRONG_MOBILE_NUM_IN_TRAN,
																		 CUP_MOBILE_VALIDATION_FALED );
					return ReturnCode ;
				}
			}
			else if(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
			{
				// Mobile number not present in transaction
				ReturnCode = txservice_Generate_Usage_Error_Message (1, "Mobile no missing in BAL INQ TRN",
																	 "txservice_Validate_CUP_ECOM_TRAN",
																	 GS_TXN_NO_WRONG_MOBILE_NUM_IN_TRAN,
																	 CUP_BALANCE_INQ_WITHOUT_MOBILE );
				return ReturnCode ;

			}
			else if (true == 	txservice_Check_If_CUP_Account_Varification_Trn())
			{
				// Its Account varification tranasction
			}
			else
			{
				// Mobile number not present in transaction
				ReturnCode = txservice_Generate_Usage_Error_Message (1, "Trn missing Mobile number validation",
																	 "txservice_Validate_CUP_ECOM_TRAN",
																	 GS_TXN_NO_WRONG_MOBILE_NUM_IN_TRAN,
																	 CUP_MOBILE_VALIDATION_FALED );
				return ReturnCode ;
			}

		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
		{
			if(true == txservice_Check_If_Validate_Mobile_num_FOR_CUP_ECOM_TRAN())
			{
				// Card number is not link with any mobile number
				ReturnCode = txservice_Generate_Usage_Error_Message (1, "Card has no mobile number",
																	 "txservice_Validate_CUP_ECOM_TRAN",
																	 GS_TXN_NO_MOBILE_NUM_LINK_WITH_CARD,
																	 CUP_CARD_HAS_NO_MOBILE_NUM );
				return ReturnCode ;
			}
			else if (true == 	txservice_Check_If_CUP_Account_Varification_Trn())
			{
				// Its Account varification tranasction
			}
			else
			{
				// Mobile number not present in transaction
				ReturnCode = txservice_Generate_Usage_Error_Message (1, "Mobile no not present in BAL INQ TRN",
																	 "txservice_Validate_CUP_ECOM_TRAN",
																	 GS_TXN_NO_WRONG_MOBILE_NUM_IN_TRAN,
																	 CUP_BALANCE_INQ_WITHOUT_MOBILE );
				return ReturnCode ;
			}
		}
		else
		{
			// Card number is not link with any mobile number
			ReturnCode = txservice_Generate_Usage_Error_Message (1, "Card has no mobile phone number",
																 "txservice_Validate_CUP_ECOM_TRAN",
																 GS_TXN_NO_MOBILE_NUM_LINK_WITH_CARD,
																 CUP_CARD_HAS_NO_MOBILE_NUM );
			return ReturnCode ;
		}
	}


	if(true == txservice_Check_If_Validate_CVN2_FOR_CUP_ECOM_TRAN())
	{
		ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
		return ReturnCode ;
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
	{
		if((0 != strncmp( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY, CURRENCY_LEN ))&&
		   (0 != strncmp( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY, CURRENCY_LEN )))
		{
			// We Ã§an't get card details so we have to approve this transaction
			strncpy(Auth_Tx.TLF01_details.response_code,CUP_ISO_APPROVED,CUP_RESPONSEP_CODE_LEN);
			if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
			{
				/* No. Generate a numeric authorization id */
				(void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
			}
			strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
			return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
		}
		else
		{
			// CVN2 is optional in case of Balance inquiry Transaction.
			ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
		}
	}
	else if(true == txservice_Check_If_CUP_ECOM_NON_AUTH_MODE())
	{
		// CVN2 is optional in case of Non AUTH mode and Balance inquiry Transaction.
		ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
	}
	else if (true == 	txservice_Check_If_CUP_Account_Varification_Trn())
	{
		// Its Account varification tranasction
		ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
	}
	else
	{
		 ReturnCode = txservice_Generate_Usage_Error_Message (1, "CVN2 missing in CUP AUTH_MODE ECOM TRN",
															  "txservice_Validate_CUP_ECOM_TRAN",
															  GS_TXN_TRANSACTION_NOT_ALLOWED,
															  INVALID_ACCOUNT_NUMBER );
		 return ReturnCode ;
	}
	return NO_AUTH_ERROR ;
}

INT txservice_Check_If_Validate_EXPIRY_NUM_FOR_CUP_ECOM_TRAN(void)
{
	if(Auth_Tx.TLF01_details.resp_source[CUP_DE61_CARD_EXPIRY_VER_POS] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_Check_If_Validate_CVN2_FOR_CUP_ECOM_TRAN(void)
{
	if(Auth_Tx.TLF01_details.resp_source[CUP_DE61_CVN2_VER_POS] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_Validate_CVN2_FOR_CUP_Non_ECOM_TRAN(void)
{
	if(Auth_Tx.TLF01_details.resp_source[CUP_DE61_CVN2_VER_POS] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_Validate_Mobile_num_FOR_CUP_ECOM_TRAN(void)
{
	if(Auth_Tx.TLF01_details.resp_source[CUP_DE61_MOBILE_NUM_VER_POS] == '1')
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_CUP_Mobile_number_Assoisated_with_Card(char * mobile_num)
{
	INT   mobile_len     = 0;
	BYTE  temp_card_mobile_num[31] = {0};
	BYTE  str_mobile_num[31] = {0};

	strncpy(temp_card_mobile_num,
			Auth_Tx.CCF02P_details.pin + CUP_USER_MOBILE_NUM_POS_IN_CCF02P,
			CUP_ECOM_MAX_MOBILE_NUM_LEN);
	// Lets trip the space.
	strTrim (temp_card_mobile_num, str_mobile_num) ;
	mobile_len = strlen(str_mobile_num);
	if(mobile_len == 0  || mobile_len > CUP_ECOM_MAX_MOBILE_NUM_LEN)
	{
      //There is no mobile number associated with Card or invalid number
		return false;
	}
	else
	{
		strncpy(mobile_num,str_mobile_num,mobile_len);
		return true;
	}
}

INT txservice_Validate_coustmer_mobile_number(char * mobile_num)
{
	INT   mobile_len     = 0;
	INT   len     = 0;
	CHAR  temp_mobile_num [21] = {0};
	BYTE  temp_mobile_num_len [4] = {0};
	BYTE  temp_len[4] = {0};
	INT   length = 0;
	INT   leading_Zero_len = 0;
	BYTE  Leader_Zero_trim_mobile_num [21]= {0};

	memcpy(temp_len,Auth_Tx.TLF01_details.product_codes[13].code, CUP_DE61_LEN_SIZE);
	len=atoi(temp_len);
	if(len >CUP_DE_61_AM_USAGE_MOBILE_NUM_LEN_POS)
	{
		strncpy(temp_mobile_num_len,
				Auth_Tx.future_use_2 + CUP_DE_61_AM_USAGE_MOBILE_NUM_LEN_POS,
			    CUP_DE_61_AM_USAGE_MOBILE_NUM_LEN);
		len = atoi(temp_mobile_num_len);
		if(len > 0 && len <= CUP_ECOM_MAX_MOBILE_NUM_LEN )
		{
			strncpy(temp_mobile_num,
				   Auth_Tx.future_use_2 + CUP_DE_61_AM_USAGE_MOBILE_NUM_POS,
				   len);
			// Lets compare mobile number present in CCF02P table
			mobile_len = strlen(mobile_num);
			if(mobile_len != len)
			{
				// lets check if Mobile number store in ATP  is having leading Zero
				while (mobile_num[length] != '\0')
				{
					 if(mobile_num[length] == '0')
					 {
						 leading_Zero_len++;
						 length++;
					 }
					 else
					 {
						 break;
					 }
				}
				strncpy( Leader_Zero_trim_mobile_num,
						 mobile_num +leading_Zero_len,
						 mobile_len - leading_Zero_len);

				// Lets check the lenth
				if ( leading_Zero_len != (mobile_len - len))
				{
					 // Length missmatch
				return false;
			}
				else if(0 == strncmp(Leader_Zero_trim_mobile_num,
									 temp_mobile_num,len))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if(0 == strncmp(mobile_num,temp_mobile_num,mobile_len))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}
	else
	{
		return false;
	}
}
BOOLEAN txservice_If_Tx_qualify_for_EMV_validation()
{
	if( Auth_Tx.TLF01_details.message_type[1] == '4' )
	{
		return false;
	}
	else if(Auth_Tx.TLF01_details.message_type[2] == '2')
	{
		return false;
	}
	else if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0 &&
			strncmp(Auth_Tx.TLF01_details.processing_code,"02",2) == 0 )
	{
		return false;
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE	||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL					||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE			||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT			||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT		||
			Auth_Tx.TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL		||
			Auth_Tx.TLF01_details.tx_key == AUTH_ISSUER_REVERSAL_ADVICE		||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE					||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND				||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE			||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PAYMENT				||
			Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE			||
			Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND		||
			Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void txservice_load_rps_flag_for_all_cardbrands()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  rps_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",   /* points to section name         */
									  "AMEX",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   rps_flag,              		/* points to destination buffer   */
									   sizeof(rps_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   amex_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",
									  "CUP",
									   "0",
									   rps_flag,
									   sizeof(rps_flag)-1,
									   filename
							 	   );
	   cup_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",
									  "DCI",
									   "0",
									   rps_flag,
									   sizeof(rps_flag)-1,
									   filename
							 	   );
	   dci_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",
									  "JCB",
									   "0",
									   rps_flag,
									   sizeof(rps_flag)-1,
									   filename
							 	   );
	   jcb_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",
									  "MC",
									   "0",
									   rps_flag,
									   sizeof(rps_flag)-1,
									   filename
							 	   );
	   mc_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );

	   rc = GetPrivateProfileString(  "LOAD_RPS_PROCESSING_FLAG",
									  "VISA",
									   "0",
									   rps_flag,
									   sizeof(rps_flag)-1,
									   filename
							 	   );
	   visa_rps_flag = atoi(rps_flag);
	   memset( rps_flag, 0x00, sizeof(rps_flag) );
}

void txservice_load_MID_CPNG_records()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  tmpstring	[20] = {0};
	   CHAR  tmp_ini    [40] = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "MID_RESPONSE_CODE",   /* points to section name         */
									  "ATP_POS_RESPONSE_CODE",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   tmp_ini,              		/* points to destination buffer   */
									   sizeof(tmp_ini)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   memset ( tmpstring, 0x00, MAX_INI_INFO_SIZE );
	   strncpy( tmpstring, tmp_ini, MAX_INI_INFO_SIZE-1 );
	   strncpy( pos_rc, tmpstring, sizeof(pos_rc)-1);

	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "MID_RESPONSE_CODE",   /* points to section name         */
									  "RESPONSE_TEXT",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   tmp_ini,              		/* points to destination buffer   */
									   sizeof(tmp_ini)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   strncpy( pos_res_txt, tmp_ini, sizeof(tmp_ini)-1);
}

BOOLEAN txservice_load_OBS_records_for_MC()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(OBS01_GUI_LIST)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &obs01_list,   0x00, sizeof(OBS01_GUI_LIST  ) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	 ST1_DB_GET_GUI_LIST,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 OBS01_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													network_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &obs01_list, p_data+sizeof(AUTH_TX), sizeof(OBS01_GUI_LIST) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_read_OBS_record_for_MC()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i       = 0;
	   INT           retcode = 0;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode = 0;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(OBS01)] = {0};
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &obs01_data, 0x00, sizeof (obs01_data));

	   memcpy(buffer+sizeof(AUTH_TX),Auth_Tx.TLF01_details.product_codes[19].quantity,2);
	   memcpy(buffer+sizeof(AUTH_TX)+3,Auth_Tx.TLF01_details.product_codes[16].quantity,1);
	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			                         ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 OBS01_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													network_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
				 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
				 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

				 memcpy( &obs01_data, p_data+sizeof(AUTH_TX), sizeof(OBS01) );
				 free( p_msg_in );
			}
	   }
	   return PTEMSG_OK;
}

BOOLEAN txservice_load_VISA3DS2_records_for_VISA()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(VISA3DS2_GUI_LIST)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &visa3ds2_list,   0x00, sizeof(VISA3DS2_GUI_LIST  ) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	 ST1_DB_GET_GUI_LIST,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 VISA3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													network_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &visa3ds2_list, p_data+sizeof(AUTH_TX), sizeof(VISA3DS2_GUI_LIST) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_load_AMEX3DS2_records_for_AMEX()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(AMEX3DS2)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &amex_3ds2,   0x00, sizeof(AMEX3DS2  ) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	  ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 AMEX3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &amex_3ds2, p_data+sizeof(AUTH_TX), sizeof(AMEX3DS2) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_load_DINERS3DS2_records_for_DINERS()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(DCI3DS2)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &diners_3ds2,   0x00, sizeof(DCI3DS2) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	  ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
	                                 DINERS3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &diners_3ds2, p_data+sizeof(AUTH_TX), sizeof(DCI3DS2) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_load_JCB3DS2_records_for_JCB()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(JCB3DS2_GUI_LIST)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &jcb3ds2_list,   0x00, sizeof(JCB3DS2_GUI_LIST) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	 ST1_DB_GET_GUI_LIST,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
									 JCB3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &jcb3ds2_list, p_data+sizeof(AUTH_TX), sizeof(JCB3DS2_GUI_LIST) );
			 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_load_CUP3DS2_records_for_CUP()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(CUP3DS2)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   /* Initialize the global variables. */
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &cup_3ds2,   0x00, sizeof(CUP3DS2) );

	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	  ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
									 CUP3DS2_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                &p_msg_in,
													tran_dataserver_que_name,
	                                                 error_msg );
			if ( rcode == PTEMSG_OK )
			{
			 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			 memcpy( &cup_3ds2, p_data+sizeof(AUTH_TX), sizeof(CUP3DS2) );
			 free( p_msg_in );
			}
	   }
	   return true;
}



BOOLEAN txservice_Check_If_Transaction_Qualifies_For_RPS_STIP()
{

	if(true == txservice_Check_If_AMEX_Transaction() && amex_rps_flag == 1 && MCC_RPS_flag == 1)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == '9')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_JCB_Transaction() && jcb_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_VISA_Transaction() && visa_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		if(strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"02",2) == 0)// check here
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_MC_Transaction() && mc_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		if(0 == strncmp(Auth_Tx.TLF01_details.category_code,"5542",4) &&
		  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)))
		{/* will include all ncmcrd and ncmcr3*/
			return false;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4')// check here)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_CUP_Transaction() && cup_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"28",2) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (true == txservice_Check_If_Diners_Transaction() && dci_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == '4' )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

BOOLEAN txservice_Check_RPS_Flag_For_ACQ_Txn()
{
	if(true == txservice_Check_If_AMEX_Transaction() && amex_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else if(true == txservice_Check_If_JCB_Transaction() && jcb_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else if(true == txservice_Check_If_VISA_Transaction() && visa_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else if(true == txservice_Check_If_MC_Transaction() && mc_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else if(true == txservice_Check_If_CUP_Transaction() && cup_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else if (true == txservice_Check_If_Diners_Transaction() && dci_rps_flag == 1 && MCC_RPS_flag== 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN  txservice_Check_If_CUP_RPS_TRN(void)
{
	if((0 == strncmp(Auth_Tx.TLF01_details.merchant_id,
			   	   	 CUP_MID_INDICATOR_DEFAULT,
					 CUP_MID_INDICATOR_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			   	   	 CUP_RPS_POS_ENTRY_MODE,
					 CUP_RPS_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	   	 CUP_ISO_RPS_POS_CONDITION_CODE,
					 CUP_DE25_SIZE))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					  "dcp",3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_If_CUP_BDO_PAY(void)
{
	if((0 == strncmp( Auth_Tx.TLF01_details.pos_entry_mode,
					  CUP_BDO_PAY_POS_ENTRY_MODE,
					  CUP_BDO_POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.pos_condition_code,
			   	      CUP_BDO_PAY_POS_CONDITION_CODE,
					  CUP_DE25_SIZE))&&
	   (0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
				  	  "dcp",3)))
		{
			return true;
		}
		else
		{
			return false;
		}
}

BOOLEAN txservice_Check_If_CUP_MOTO_TRN(void)
{
	if((0 == strncmp(Auth_Tx.industry_code,CUP_MOTO_INDUSTRY_CODE,INDUSTRY_CODE_CODE))&&
	   ((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3))||
	    (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001",3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_if_POS_Acq_Transaction(void)
{
   if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpiso",6))
   {
	   return true;
   }
   else
   {
	   return false;
   }
}

BOOLEAN txservice_Check_if_Voice_Acq_Transaction(void)
{
   if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
   {
	   return true;
   }
   else
   {
	   return false;
   }
}
BOOLEAN txservice_Check_if_PLFC_Transaction(void)
{
   if( 0 == strncmp(Auth_Tx.TLF01_details.card_num ,"7",1))
   {
	   return true;
   }
   else
   {
	   return false;
   }
}

BOOLEAN txservice_Check_If_JCB_Contactless_Magstrip_Transaction(void)
{

	if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"091", 3))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_If_POS_Online_PIN_transction(void)
{
	/* Check if txn has F52, F64 and F22 pos 3 is '1' */
	if (true == txservice_Check_if_POS_Acq_Transaction() &&
		true == txservice_Check_If_DCI_pin_block_exists() &&
		'\0' != Auth_Tx.TLF01_details.product_codes[4].amount[0] &&
		 '1' == Auth_Tx.TLF01_details.pos_entry_mode[3] )
	{
		return true;
	}
	else
	{
		return false;
	}

}

void match_ISO_to_AMEX_response_code( pBYTE amex_response_code)
{
   #define NB 42

   INT  i;
   CHAR iso_response_codes [NB][2] =  {"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "12",  "13",  "14",  "15",  "19",  "21",  "25",  "28",
                                       "39",  "41",  "43",  "51",  "52",  "53",  "54",  "55",
                                       "57",  "58",  "61",  "62",  "63",  "65",  "75",  "76",
                                       "77",  "80",  "81",  "82",  "85",  "86",  "91",  "92",
                                       "93",  "96"};
   CHAR auth_auth_amex [NB][3]     =  {"000", "100", "100", "100", "200", "125", "100", "100",
                                       "111", "110", "100", "100", "100", "100", "100", "100",
                                       "100", "100", "100", "110", "100", "100", "101", "117",
                                       "100", "100", "100", "100", "100", "100", "106", "100",
                                       "100", "100", "100", "100", "000", "100", "100", "100",
                                       "100", "100"};
   CHAR auth_advice_amex [NB][3]   =  {"900", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "900", "904", "904", "904",
                                       "904", "904"};
   CHAR auth_logon_amex [NB][3]    =  {"800", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "899", "899", "899", "899",
                                       "899", "899", "899", "899", "800", "899", "912", "899",
                                       "899", "899"};
   CHAR auth_cash_amex [NB][3]     =  {"000", "100", "100", "100", "100", "107", "100", "100",
                                       "100", "100", "100", "100", "100", "100", "100", "100",
                                       "100", "290", "290", "121", "100", "100", "100", "117",
                                       "100", "100", "121", "100", "100", "100", "106", "100",
                                       "100", "100", "100", "100", "000", "100", "100", "100",
                                       "100", "100"};
  CHAR auth_reversal_amex [NB][3]  =  {"400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400"};

   if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_AUTHORIZATION_RESPONSE)
   {
      strcpy( amex_response_code, "100" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_auth_amex[i], 3);
            break;
         }
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_ADVICE_RESPONSE)
   {
      strcpy( amex_response_code, "904" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_advice_amex[i], 3);
            break;
         }
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_LOGON_RESPONSE)
   {
      strcpy( amex_response_code, "899" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_logon_amex[i], 3);
            break;
         }
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_REVERSAL_ADVICE_RESPONSE)
   {
      strcpy( amex_response_code, "400" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_reversal_amex[i], 3);
            break;
         }
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_CASH_ADVANCE_RESPONSE)
   {
      strcpy( amex_response_code, "100" ); /* Default value */
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_cash_amex[i], 3);
            break;
         }
      }
   }

   if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_SALE_RESPONSE)
   {
	      strcpy( amex_response_code, "100" ); /* Default value */
	      for (i=0; i<NB; i++)
	      {
	         if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
	         {
	            strncpy(amex_response_code, auth_auth_amex[i], 3);
	            break;
	         }
	      }
   }

   if (Auth_Tx.TLF01_details.tx_key+100 == AUTH_AUTHORIZATION_RESPONSE)
   {
	   if ((Auth_Tx.TLF01_details.pin_block[0] != '\0') &&
		   (0 == strncmp(Auth_Tx.TLF01_details.response_code, "05", 2)))
	   {
		   strncpy(amex_response_code, "100", 3);
	   }
   }
}

VOID match_ISO_to_Diners_response_code(  pBYTE diners_response_code)
{
	#define NB 86

	INT  i;
	CHAR iso_response_codes [NB][2] =
								{"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
								"08",  "09",  "10",  "11",  "12",  "13",  "14",  "15",
								"16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",
								"24",  "25",  "26",  "27",  "28",  "29",  "30",  "31",
								"32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",
								"40",  "41",  "42",  "43",  "44",  "51",  "52",  "53",
								"54",  "55",  "58",  "59",  "60",  "61",  "62",  "63",
								"64",  "65",  "66",  "68",  "75",  "76",  "77",  "78",
								"79",  "80",  "81",  "82",  "84",  "85",  "86",  "87",
								"88",  "89",  "90",  "91",  "92",  "93",  "94",  "95",
								"96",  "97",  "98",  "NO",  "N7",  "00"};
	CHAR DCI_response_code [NB][3] =
								{"081", "182", "182", "109", "200", "100", "100", "200",
								"001", "100", "081", "081", "100", "110", "100", "100",
								"081", "100", "909", "100", "909", "100", "909", "100",
								"   ", "100", "   ", "   ", "100", "   ", "904", "905",
								"100", "101", "102", "100", "204", "203", "206", "100",
								"115", "208", "100", "209", "100", "100", "100", "100",
								"101", "117", "100", "210", "203", "100", "104", "100",
								"100", "100", "205", "100", "106", "100", "100", "100",
								"100", "307", "909", "100", "100", "081", "909", "100",
								"909", "100", "910", "100", "909", "124", "909", "909",
								"909", "100", "100", "909", "100", "400"};

	strcpy( diners_response_code, "100" ); /* Default value */
	for (i=0; i<NB; i++)
	{
		if (0 == strncmp(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2))
		{
			strncpy(diners_response_code, DCI_response_code[i], 3);
			break;
		}
	}
}

BOOLEAN txservice_Is_AsiaPay_Transaction()
{
	if((0==strncmp(Auth_Tx.TLF01_details.message_type, "0200",4)) &&
	   (0==strncmp(Auth_Tx.TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Refund_Transaction()
{
	if((0==strncmp(Auth_Tx.TLF01_details.message_type, "0100",4)||
		(0==strncmp(Auth_Tx.TLF01_details.message_type, "1100",4))) &&
	   (0==strncmp(Auth_Tx.TLF01_details.processing_code, "20",2)))
	{
		return true;
	}
	return false;
}
BYTE txservice_Is_Incoming_stip_reversal_txn()
{
	if(  0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)	&&
		 0 != strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) &&
		'4' == Auth_Tx.TLF01_details.message_type[1])
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_validate_mcc()
{
	CHAR  BLOCKED_MCC[][5] = { "6010", "6011","6012", "6051", "done" };
	int i = 0;
	BOOLEAN ret_val = false;
	CHAR error_msg[256]= {0};

    while( BLOCKED_MCC[i][0] != 'd' )
    {
       if ( 0 == strcmp(Auth_Tx.TLF01_details.category_code,BLOCKED_MCC[i]) )
       {
          /* Need to block this transaction. */
          ret_val = true ;
          sprintf( error_msg,"Txn blocked for MCC value(%s)",Auth_Tx.TLF01_details.category_code);
          strncpy(Auth_Tx.TLF01_details.response_text,error_msg,sizeof(Auth_Tx.TLF01_details.response_text)-1);
          txcntl_log_message( 3, 3,error_msg, "txservice_validate_mcc", 1 );
          break;
       }
       i++;
    }
	return ret_val;
}

/******************************************************************************
 *
 *  NAME:         get_MRACON_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether falcon is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Phani  - ThoughtFocus
 *
 ******************************************************************************/
void txservice_load_JCB_contactless_magstripe_Flag_RC( void )
{
   #define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   GetPrivateProfileString(
             "JCB_CONTACTLESS_MAGSTRIPE_SEC",    /* points to section name       */
             "JCB_CONTACTLESS_MAGSTRIPE_FLAG",   /* points to key name           */
             "0",                                /* points to default string     */
             ini_info,                           /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,              /* size of destination buffer   */
             filename                            /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   JCB_ACQ_Contactless_Magstrip_Flag = atoi(tmpstring);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   GetPrivateProfileString(
             "JCB_CONTACTLESS_MAGSTRIPE_SEC",    /* points to section name       */
             "JCB_CONTACTLESS_MAGSTRIPE_RC",   /* points to key name           */
             "0",                                /* points to default string     */
             ini_info,                           /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,              /* size of destination buffer   */
             filename                            /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy (JCB_ACQ_Contactless_Magstrip_RC, tmpstring, 2);
}
/******************************************************************************/


BYTE txservice_is_txn_should_blocked_for_processingCode()
{
	char strTemp[512] = {0};

	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6) ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6)) &&
		0 == strncmp(Auth_Tx.TLF01_details.processing_code,"28",2))
	{/* will include all ncmcrd and ncmcr3*/
		memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);

		if(true == txservice_verify_MCC_blocked_for_send_fund_txn())
		{
			strcpy(Auth_Tx.TLF01_details.response_text,"Transaction not permitted to terminal");
			strncpy(Auth_Tx.TLF01_details.response_code,TRANSACTION_NOT_PERMITTED,2);
		}
		else
		{
		strcpy(Auth_Tx.TLF01_details.response_text,"Processing code not supproted in STIP");
		strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_FOR_CARD,2);
		}

		return true;
	}


	if (true == txservice_is_txn_AFT_for_partial_approval())
	{
		memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(Auth_Tx.TLF01_details.response_text,"AFT txn not supproted in STIP");
		strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_FOR_CARD,2);
		return true;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
	   0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4)  &&
	   0 == strncmp(Auth_Tx.TLF01_details.processing_code, AUTH_AFT,2))
	{
		memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(Auth_Tx.TLF01_details.response_text,"Credit CashTx not allowed at this time");
		strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_CASH_ADV,2);
		return true;
	}

	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
	   0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4)  &&
	   0 == strncmp(Auth_Tx.TLF01_details.processing_code, AUTH_OCT,2))
	{
		memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(Auth_Tx.TLF01_details.response_text,"Credit CashAdvTx not allowed for card");
		strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_CASH_ADV,2);
		return true;
	}

	if ((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit2",6) ||
		 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",6)) &&
		 0 == strncmp(Auth_Tx.TLF01_details.acquirer_id, "nccup2", 6) &&
		 0 == strncmp(Auth_Tx.TLF01_details.processing_code, AUTH_CASH_ADV, 2))
	{
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		strcpy(Auth_Tx.TLF01_details.response_text,"CashAdvTx not supported for card");
		sprintf(strTemp, "CashAdvTx not supported for card");
		txcntl_log_message(3, 1, strTemp, "txservice_is_txn_should_blocked_for_processingCode", 1);
		strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_FOR_CARD,2);
		return true;
	}


	return false;
}

BYTE txservice_is_txn_AFT_for_partial_approval()
{
	/*APRMNDT 2024,VS - 10: F60.10 - Addtnl Auth Indcr 1 or 3 in AFT for partial approval */
	if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6) &&
		0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4)  &&
		0 == strncmp(Auth_Tx.TLF01_details.processing_code, AUTH_AFT,2) &&
		(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[15].quantity[1], "1",1) ||
		 0 == strncmp(&Auth_Tx.TLF01_details.product_codes[15].quantity[1], "3",1)))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BYTE txservice_is_txn_should_blocked_for_MCC()
{
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis3",6)) &&
	    true == txservice_validate_mcc())
	{/* will include all ncmcrd and ncmcr3*/
		 memset(Auth_Tx.TLF01_details.auth_number,0x00,sizeof(Auth_Tx.TLF01_details.auth_number));
		 strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		 strncpy(Auth_Tx.TLF01_details.response_code,TXN_NOT_PERMITTED_FOR_CARD,2);
		 return true;
	}
	return false;
}

BYTE txservice_verify_MCC_blocked_for_send_fund_txn()
{
	if(0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4) &&
	  ( 0 == strcmp(Auth_Tx.TLF01_details.category_code, "7994") ||
		0 == strcmp(Auth_Tx.TLF01_details.category_code, "7995") ||
		0 == strcmp(Auth_Tx.TLF01_details.category_code, "9406")) &&
	    0 == strcmp(Auth_Tx.TLF01_details.remote_nsp_type[0], 'P' ) &&
	    0 == strncmp(Auth_Tx.TLF01_details.product_codes[13].code, "C04",3))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BYTE txservice_check_for_DCI_refund_txn()
{
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci",5)  ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci3",6)) &&
		(0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100",4) ||
		0 == strncmp(Auth_Tx.TLF01_details.message_type,"1110",4)) &&
		0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))
	{
			return true;
	}

	return false;

}

BYTE txservice_check_for_AMEX_refund_txn()
{
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5) &&
		(0 == strncmp(Auth_Tx.TLF01_details.message_type,"1100",4) ||
		0 == strncmp(Auth_Tx.TLF01_details.message_type,"1110",4)) &&
		0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))
	{
			return true;
	}

	return false;

}

INT txservice_Get_MCC_Details_For_Rps_Txn()
{
	INT numofmcc = 0;
	INT index= 0;
	DWORD rc;
	CHAR  filename   [80] = {0};
	CHAR  tmpstr     [80] = {0};
	CHAR  rps_flag   [2]  = {0};

	/* Get path to the tf.ini file. */
	memset( tmpstr, 0x00, sizeof(tmpstr) );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	numofmcc = GetPrivateProfileList( filename,
	                                        "MCC_FOR_RPS_TXN",
	                                       (pCHAR)&rpsmcc_list,
	                                        sizeof(rpsmcc_list[0]) );

	for(index = 0 ; index<numofmcc;index++ )
	{
		rpsmcc_list[index].Intmcc = atoi(rpsmcc_list[index].MCCstring);
	}
	return numofmcc;

}

INT txservice_Check_MCC_availablity()
{
	INT txn_mcc_value=0;
	int index = 0;
	CHAR error_msg[512]={0};

	txn_mcc_value = atoi(Auth_Tx.TLF01_details.category_code);

	for(index = 0 ;index< Totol_numof_MCC; index++)
	{
		if(txn_mcc_value == rpsmcc_list[index].Intmcc)
		{
	        sprintf( error_msg,"Txn blocked for MCC value(%s)",Auth_Tx.TLF01_details.category_code);
	        strncpy(Auth_Tx.TLF01_details.response_text,error_msg,sizeof(Auth_Tx.TLF01_details.response_text)-1);
	        txcntl_log_message( 3, 3,error_msg, "txservice_validate_mcc", 1 );
			return 1;
		}
	}

	return 0;
}
BOOLEAN txservice_Check_If_VISA_issuing_transaction(void)
{
	if(  0 == strncmp(Auth_Tx.TLF01_details.handler_queue,ncvisa_issuing_link_name,6) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Check_CAVVResultCode_value_to_directly_decline_txn()
{
	if(Auth_Tx.TLF01_details.cvc == VISA_CAVV_FAILED_RESULT_CODE1 				  	||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_FAILED_RESULT_CODE2                 	||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_AUTH_RESULT_INVALID					||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_AUTH_FAILED  							||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_FAILED_VALIDATION_ATMP 				||
	   Auth_Tx.TLF01_details.cvc == VISA_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION 	||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_WAS_NOT_VALIDATE_ATTEMPT 				||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_WAS_NOT_VALIDATED)
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_Check_CAVVResultCode_value_to_skip_CAVV_validation()
{
	if(Auth_Tx.TLF01_details.cvc == VISA_CAVV_SUCCESS_RESULT_CODE1 ||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_SUCCESS_RESULT_CODE2 ||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_PASS_VALIDATION_AUTH ||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_PASS_VALIDATION_ATTEMPT ||
	   Auth_Tx.TLF01_details.cvc == VISA_CAVV_PASS_VALIDATION_INFO)
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_Is_Txn_Qualify_For_Self_CAVV_Validation()
{
	if((0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"06",2)) &&
	   (Auth_Tx.TLF01_details.cvc != '7' && Auth_Tx.TLF01_details.cvc != '8' &&
		Auth_Tx.TLF01_details.cvc != '9' && Auth_Tx.TLF01_details.cvc != 'A'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_DAF_with_CAVV_validation()
{
	if( ((0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
		0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "06", 2) &&
		true == txservice_verify_CAVV_KeyIndicator_from_Issuer()) ||
		true == txservice_VISA_to_Support_DAF_for_MIT_transactions() ) &&
		(0 != strncmp(Auth_Tx.TLF01_details.cvc, '7', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, '8', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, '9', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, 'A', 1)))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_Is_Txn_Qualify_for_DAF_without_CAVV_validation()
{
	if( ((0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
		0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "06", 2) &&
		true == txservice_verify_CAVV_KeyIndicator_from_Visa()) ||
		true == txservice_VISA_to_Support_DAF_for_MIT_transactions() ) &&
		(0 != strncmp(Auth_Tx.TLF01_details.cvc, '7', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, '8', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, '9', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, 'A', 1)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Visa_Secure_Smart_Attempts_Service()
{
	if( (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
		  0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[2], "89", 2) &&
		 (Auth_Tx.TLF01_details.product_codes[0].code[2] == 'S') &&
		  true == txservice_verify_CAVV_KeyIndicator_from_Visa()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Smart_Service_with_passed_CAVV_validation()
{
	if( (0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "00", 2)) &&
		(Auth_Tx.TLF01_details.cvc == '8'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Smart_Service_with_failed_CAVV_validation()
{
	/* OMC2024 VS-12: Do not verify the Auth Result code for DE44.13 = '7' */
	if( Auth_Tx.TLF01_details.cvc == '7' )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Frictionless_Flow_using_FIDO_standards()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
			  0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[2], "90", 2) &&
			 (Auth_Tx.TLF01_details.product_codes[0].code[2] == 'Q'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Frictionless_Flow_Visa_secure_services()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
			  0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[2], "86", 2) &&
			 (Auth_Tx.TLF01_details.product_codes[0].code[2] == 'R'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Visa_Data_Only_transactions()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"07",2) &&
			  0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "03", 2) &&
			  0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[2], "91", 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Visa_Data_Only_with_CAVV_passed_valdn()
{
	if ((true == txservice_verify_CAVV_KeyIndicator_from_Issuer() ||
		 true == txservice_verify_CAVV_KeyIndicator_from_Visa() ) &&
		Auth_Tx.TLF01_details.cvc == VISA_CAVV_PASS_VALIDATION_INFO	)
	{
		return true;
	}
	else
	{
		return false;
	}
}
BOOLEAN txservice_Is_Txn_Qualify_for_Visa_Data_Only_with_CAVV_failed_valdn()
{
	if ((true == txservice_verify_CAVV_KeyIndicator_from_Issuer() ||
		 true == txservice_verify_CAVV_KeyIndicator_from_Visa() ) &&
		(Auth_Tx.TLF01_details.cvc == VISA_CAVV_AUTH_FAILED ||
		 Auth_Tx.TLF01_details.cvc == VISA_CAVV_FAILED_RESULT_CODE1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_Visa_IDX_transactions()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"07",2) &&
		Auth_Tx.TLF01_details.dcf01_retired_cwk[0] != '\0')
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_Is_Txn_Qualify_for_nonDAF_with_CAVV_validation()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
		0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "00", 2) &&
		false == txservice_VISA_to_Support_DAF_for_MIT_transactions() &&
		(0 != strncmp(Auth_Tx.TLF01_details.cvc, '7', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, '8', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, '9', 1) &&
		 0 != strncmp(Auth_Tx.TLF01_details.cvc, 'A', 1)))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_Is_Txn_Qualify_for_nonDAF_without_CAVV_validation()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"06",2) &&
		(0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "07", 2) ||
		 0 == strncmp(Auth_Tx.TLF01_details.dcf01_retired_cwk, "08", 2)) &&
		 false == txservice_VISA_to_Support_DAF_for_MIT_transactions() &&
		 (0 != strncmp(Auth_Tx.TLF01_details.cvc, '7', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, '8', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, '9', 1) &&
			0 != strncmp(Auth_Tx.TLF01_details.cvc, 'A', 1)))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_VISA_to_Support_DAF_for_MIT_transactions()
{
	/* APR MNDT 2024, VS-7: Check DAF Indicator 01, 06 */
	if (0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode, "0010", 2) &&
		(0 == strncmp(Auth_Tx.TLF01_details.product_codes[7].quantity, "C", 1) ||
		 0 == strncmp(Auth_Tx.TLF01_details.product_codes[7].quantity, "I", 1) ||
		 0 == strncmp(Auth_Tx.TLF01_details.product_codes[7].quantity, "R", 1)) &&
		(0 == strncmp(Auth_Tx.TLF01_details.product_codes[8].code, "01", 2) ||
		 0 == strncmp(&Auth_Tx.TLF01_details.product_codes[8].code[2], "06", 2)) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN txservice_Is_Txn_Qualify_For_Account_Verification()
{
	double trans_amt = 0.0;
	trans_amt = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

	if (0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4) &&
		0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,ECOM_POS_CC_VISA,2) &&
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"05",2) &&
		(0 == trans_amt) &&
		0 != strlen(Auth_Tx.TLF01_details.dcf01_retired_cwk))
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_verify_CAVV_KeyIndicator_from_Issuer()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "01", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "02", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "03", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "04", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "05", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "06", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "07", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "08", 2))
	{
		return true;
	}
	else
		return false;
}

BOOLEAN txservice_verify_CAVV_KeyIndicator_from_Visa()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "10", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "11", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "12", 2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.dcf01_retired_cwk[4], "13", 2) )
	{
		return true;
	}
	else
		return false;

}
BOOLEAN txservice_verify_VISA_self_validation()
{
	/*char Supplemental_output[33] = "0";

	TxDataServer_Sha256_Encryption( &Supplemental_output);
	if (true == txservice_VISA_get_CAVV_from_DESEncrypn(Supplemental_output))
		return true;
	else
		return false;*/
	return true;
}

BOOLEAN txservice_VISA_get_CAVV_from_DESEncrypn(pBYTE block)
{
	//BYTE block = "29547599606445197993099000000000";
	BYTE blockA[50];
	BYTE blockB[50];
	BYTE keyA [50];
	BYTE keyB [50];
	char gen_CAVV[4];
	INT buffer_len;
	INT i, j, k;
	BYTE buf_copy[50];
	char HexBuffer[50]={0};
	char HexBuffer1[50]={0};
	char HexBuffer2[50]={0};
	char HexBuffer3[50]={0};
	char HexBuffer4[50]={0};
	char HexBuffer5[50]={0};
	BYTE block1[20] = {0};
	BYTE block2[20] = {0};

	memset(blockA, 0x00, sizeof(blockA));
	memset(blockB, 0x00, sizeof(blockB));
	memset(keyA, 0x00, sizeof(keyA));
	memset(keyB, 0x00, sizeof(keyB));


	strncpy(blockA, block, 16);
	hexchar2word(block1,blockA);
	strncpy(blockB, block+16, 16);
	hexchar2word(block2,blockB);
	string2hexString(block2,HexBuffer5);
	static WORD hard_key [4]   = {0};
	static WORD hard_key1[4]   = {0};

	strncpy(keyA, Auth_Tx.CAF01_details.csck, 16);
	hexchar2word(hard_key, keyA);
	strncpy(keyB, Auth_Tx.CAF01_details.csck+16, 16);
	hexchar2word(hard_key1, keyB);

		buffer_len = strlen(block1);
		
		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer);


		buffer_len = strlen(block1);

		for (i=0; i< strlen(block1); i++)
		{
			block1[i]^= block2[i];
		}
		string2hexString(block1,HexBuffer1);
		buffer_len = strlen(block1);
		
		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer2);
		buffer_len = strlen(block1);
		
		i = des_decryption_ex1(block1, buffer_len, hard_key1, 0);
		string2hexString(block1,HexBuffer3);
		buffer_len = strlen(block1);
		
		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer4);
		buffer_len = strlen(block1);

		memset(buf_copy, 0x00, sizeof(buf_copy));

		byte2hexchar(buf_copy, block1, buffer_len);

		buffer_len = strlen(buf_copy);

		memset(block1, 0x00, sizeof(block1));
		GetOnlyNumber(buf_copy, block1);
		strncpy(gen_CAVV,block1, 3);

		/* Compare generated CAVV with CAVV received from VISA */
		if (0 == strncmp(gen_CAVV,&Auth_Tx.TLF01_details.dcf01_retired_cwk[7], 3))
		{
			return true;
		}
		else
		{
			return false;
		}

}


BOOLEAN txservice_Check_AVV_IVV_value_to_skip_CAVV_validation()
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kJ" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kE" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kL" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kN" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kF" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kK" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kM" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kC" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kD" ,2))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_control_bytes_validation()
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kA" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kB" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kG" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kH" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kO" ,2) ||
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kP" ,2) )
	{
		return true;
	}
	return false;

}

BOOLEAN txservice_MC_check_control_byte_and_SLI_combo_for_AAV_validation()
{

	if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_FULLYAUTHENTICATED,2) &&
	  (0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kA" ,2) ||
	   0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kB" ,2)))
	{
		return true;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_RECURRING_MIT,2) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kO" ,2)   ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kP" ,2)))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_MC_check_control_byte_and_SLI_combo_for_CVC2_and_BAU_validation()
{
	if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_FULLYAUTHENTICATED,2) &&
	  (0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kC" ,2)   ||
	   0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kD" ,2)))
	{
		return true;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_MCIDENTITYCHECK_INSIGHTS,2)  &&
		   (0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kX" ,2)    ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kY" ,2)    ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kZ" ,2)))
	{
		return true;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_AUTHENTICATED_ACQUIRERSCAEXEMPTION,2)  &&
		   (0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kN" ,2)    ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kU" ,2)    ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kV" ,2)    ||
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kW" ,2) ))
	{
		return true;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_ATTEMPT_AUTHENTICATION,2)  &&
			0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kE" ,2))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_MC_check_control_byte_and_SLI_combo_for_declining_3DS2()
{
	if(0 == strncmp(Auth_Tx.TLF01_details.profile,ECI_SLI_ATTEMPT_AUTHENTICATION,2)  &&
	   0 == strncmp(Auth_Tx.TLF01_details.product_codes[5].quantity, "kF" ,2))
	{
			return true;
	}
	return false;
}
BOOLEAN txservice_MC_check_for_OBS_validation()
{
	if(('\0' != Auth_Tx.TLF01_details.product_codes[19].quantity[0] &&
	    '\0' != Auth_Tx.TLF01_details.product_codes[16].quantity[0]  )&&
		( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[19].quantity,"05",2) ||
	      0 == strncmp(Auth_Tx.TLF01_details.product_codes[19].quantity,"06",2) ))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_MC_check_for_OBS_Digital_Payment_Data_Validation()
{
	if(('\0' != Auth_Tx.TLF01_details.product_codes[19].quantity[0] &&
	   '\0' != Auth_Tx.TLF01_details.product_codes[16].quantity[0]  )&&
	 ( 0 == strncmp(Auth_Tx.TLF01_details.product_codes[19].quantity,"54",2) ||
	  'D' == Auth_Tx.TLF01_details.product_codes[16].quantity[0] ))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_self_AAV_Validation_For_MC()
{
	static BYTE key_hexa[128], data_hexa[128]="";
	unsigned int   i,key_len, data_len, mac_len = 0;
	unsigned char   mac[128] = {0};
	BYTE ReturnCode=0;
	BYTE   key[128],data[128]={0};
	INT de48_retcode= 0;
	INT ucaf_data_64_len =0;
	BYTE UCAF_data[33]={0};
	INT ucaf_data_len,astr_len = 0;
	char cvv_value[9] = {0};
	BYTE UCAF_data_ascii[41]={0},UCAF_data_bin[41]={0};
	char error_msg[512]={0};
	BYTE ascii_mac[65]={0}, generated_IAV[9]={0};
	int De48len=0;
		   	BYTE DE48_data[256]={0};
	mac_len =32;

	txcntl_log_message( 3, 1,"txn qualified for self validtion", "txservice_self_AAV_Validation_For_MC", 1 );

	txservice_get_3DS2_key_for_MC();
	memcpy(key,key3ds01.key3ds,sizeof(key));

//	sprintf( error_msg,"Key used for MC 3DS 2.0 (%s)",key);
	//txcntl_log_message( 2, 3,error_msg, "txservice_self_AAV_Validation_For_MC", 1 );

	/*data preparation */
	txservice_prepare_data_for_MC_hmac_verification(&data);

	memset(error_msg,0x00,sizeof(error_msg));
    sprintf( error_msg,"Data used for MC 3DS 2.0 (%s)",data);
    txcntl_log_message( 3, 3,error_msg, "txservice_self_AAV_Validation_For_MC", 1 );


	/*converting data and key into hexa format*/
	key_len = strlen(key)/2;
	data_len = strlen(data)/2;
	for(i=0;i<key_len;i++)
	{
		hexchar2byte(key_hexa+i,key+(2*i));
	}
	for(i=0;i<data_len;i++)
	{
		hexchar2byte(data_hexa+i,data+(2*i));
	}
/*Below link can be used for HMAC-SHA256 MAC generation
 * https://www.liavaag.org/English/SHA-Generator/HMAC/
 * */
	hmac_sha(HMAC_SHA256,key_hexa,key_len,
		data_hexa,data_len,&mac,mac_len);

	genutil_bin_to_ascii (mac, mac_len, ascii_mac, astr_len);
	printf("MAC converted from bin to ASCII:\n %s \n",ascii_mac);

	sprintf( error_msg,"hash value generated by ATP (%s)",ascii_mac);
	txcntl_log_message( 3, 3,error_msg, "txservice_self_AAV_Validation_For_MC", 1 );

	strncpy(generated_IAV,ascii_mac,8);

	printf("CAVV generated by ATP:(%s)\n",generated_IAV);

	if(Auth_Tx.EMV_details.issuer_script_2[0] == '\0')
	{
		De48len = Auth_Tx.future_use_3[0];
		memcpy(DE48_data,&Auth_Tx.future_use_3[1],De48len);
	}
	else
	{
		De48len = strlen(Auth_Tx.EMV_details.issuer_script_2);
		memcpy(DE48_data,Auth_Tx.EMV_details.issuer_script_2,strlen(Auth_Tx.EMV_details.issuer_script_2));
	}
	de48_retcode = parse_field_48( De48len,&DE48_data,&UCAF_data );

	/*Extract the IAV from the transaction*/
	//de48_retcode = parse_field_48( strlen(Auth_Tx.EMV_details.issuer_script_2),&Auth_Tx.EMV_details.issuer_script_2,&UCAF_data );
	ucaf_data_64_len =strlen(UCAF_data);
	ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
	genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
	//printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
	strncpy(cvv_value,&UCAF_data_ascii[4],8);

	//printf("IAV parsed from transaction (%s), IAV generated from the transaction (%s)\n",cvv_value,generated_IAV);
	memset(error_msg,0x00,sizeof(error_msg));
	sprintf( error_msg,"CAVV value from txn:(%s) and CAVV value genetrated by ATP:(%s)",cvv_value,generated_IAV);
	txcntl_log_message( 3, 3,error_msg, "txservice_self_AAV_Validation_For_MC", 1 );



	/*if(0 == strncmp(cvv_value,mac,8))
	{
		txcntl_log_message( 3, 1,"AAv matched for the txn", "txservice_self_AAV_Validation_For_MC", 1 );
		return true;
	}
	else
	{
		return false;
	}*/

	/*Test code*/
	memset(Auth_Tx.EMV_details.future_field1,0x00,sizeof(Auth_Tx.EMV_details.future_field1));
	memcpy(Auth_Tx.EMV_details.future_field1,data,strlen(data));

	Auth_Tx.EMV_details.future_field1[strlen(data)]='\0';
	memset(Auth_Tx.EMV_details.future_field2,0x00,sizeof(Auth_Tx.EMV_details.future_field2));
	memcpy(Auth_Tx.EMV_details.future_field2,key,strlen(key));

	ReturnCode =TxAuthorize_Process_Security(MT_NSP_VERIFY_3D_SECURE_2);
	return NO_AUTH_ERROR;
}

void txservice_prepare_data_for_MC_hmac_verification(char * data)
{
	#define Total_card_len_for_hmac 20
	#define padding_char_for_card 'F'
	#define Total_Amount_Len 4
	#define padding_char_for_amount '0'
	int index,count,i,k=0;
	unsigned char card_num[21], Ds_txn_id, currency[4],
	txn_amt[13],merch_name[41] ={0};
	double nAuthTxAmount = 0;
	double log_amount;
	double Default_amount= 6553.6;
	int txn_amount, final_amount_for_hex = 0;
	char hexa_txn_amount[5] = {0};
	int amount_padding_len=0;
	char final_hexa_txn_amount[5]={0};
	char temp_hexa_txn_amount[5]={0};
	char DS_txn_id[33]="",temp_DS_txn_id[37]="";
	char *ptr_for_txn_amt= NULL;
	char error_msg[512]={0};
	int default_check_cmt = 14000;
	char merchant_name[41]={0};
	char merchant_name_hash[100]={0};
	char ascii_merch[100]={0};
	int ascii_merch_len=0;
	int merch_len=0;
	memset(data,0x00,128);
	/*Card Number*/
		memcpy(card_num,Auth_Tx.TLF01_details.card_num,sizeof(Auth_Tx.TLF01_details.card_num));
		for(index=strlen(card_num);index<Total_card_len_for_hmac;index++)
		{
			card_num[index] = padding_char_for_card;
		}
		card_num[index]= '\0';
		strncpy(data,card_num,strlen(card_num));

	memset(error_msg,0x00,sizeof(error_msg));
	for(index=5;index<10;index++)
	{
		card_num[index]='X';
	}
	sprintf( error_msg,"Final card number:%s",card_num);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );

	/*DS txn Id*/

		/* Remove the '-' and concatenate the remaining data*/
		strncpy(temp_DS_txn_id,Auth_Tx.TLF01_details.workstation,20);
		strncat(temp_DS_txn_id,Auth_Tx.TLF01_details.operator_id,16);
		i=0;
		while(i < strlen(temp_DS_txn_id))
		{
		if(temp_DS_txn_id[i] == '-' ||
		   temp_DS_txn_id[i] == '_')
			{
				i++;
			}
			else
			{
				if(isalpha(temp_DS_txn_id[i]) && islower(temp_DS_txn_id[i]))
				{
					DS_txn_id[k]=toupper(temp_DS_txn_id[i]);
				}
				else
				{
				DS_txn_id[k]=temp_DS_txn_id[i];
				}
				i++;
				k++;
			}
		}
		strncat(data,DS_txn_id,strlen(DS_txn_id));

	memset(error_msg,0x00,sizeof(error_msg));
	sprintf( error_msg,"Final DS txn id:%s",DS_txn_id);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );

	/* Currency*/
		strncat(data,"0",1);
		strncat(data,Auth_Tx.TLF01_details.nfi_seq_nbr,3);

	memset(error_msg,0x00,sizeof(error_msg));
	sprintf( error_msg,"Final Currency code:0%s",Auth_Tx.TLF01_details.nfi_seq_nbr);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );

	/* Transaction amount */
	final_amount_for_hex = strtod (Auth_Tx.TLF01_details.total_amount,0);
	//printf("final_amount_for_hex value after strtod:(%f)-(%d)\n",final_amount_for_hex);

	if(final_amount_for_hex  > default_check_cmt )
	{
			nAuthTxAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0);
			log_amount = log10(nAuthTxAmount);
			final_amount_for_hex = log_amount * Default_amount;
	}

		/*conevrt the amount into hexa*/
	sprintf(hexa_txn_amount, "%x", final_amount_for_hex);
	sprintf(temp_hexa_txn_amount,"%04s",hexa_txn_amount);

	//printf("After log conversion final_amount_for_hex(%d) , hex converted amount(%s)\n",final_amount_for_hex,hexa_txn_amount);
	i=0;k=0;
	while(i < strlen(temp_hexa_txn_amount))
	{
		if(isalpha(temp_hexa_txn_amount[i]) && islower(temp_hexa_txn_amount[i]))
		{
			final_hexa_txn_amount[k]=toupper(temp_hexa_txn_amount[i]);
		}
		else
		{
			final_hexa_txn_amount[k]=temp_hexa_txn_amount[i];
		}
	//	printf("final amount in hex(%X)\n",final_hexa_txn_amount[k]);
		i++;
		k++;
	}

//	printf("Final amount for the data (%s)\n",final_hexa_txn_amount);
	strncat(data,final_hexa_txn_amount,Total_Amount_Len);

/*	memset(error_msg,0x00,sizeof(error_msg));
	sprintf( error_msg,"Final amount:%s",hexa_txn_amount);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );

	strncpy(merchant_name,Auth_Tx.TLF01_details.card_holder_name,strlen(Auth_Tx.TLF01_details.card_holder_name));
	strncat(merchant_name,Auth_Tx.TLF01_details.ncf01_retired_cwk,strlen(Auth_Tx.TLF01_details.ncf01_retired_cwk));

	merch_len=strlen(merchant_name);
	sha256_MC(&merchant_name_hash,merchant_name,merch_len);

	genutil_bin_to_ascii (merchant_name_hash, strlen(merchant_name_hash), ascii_merch, ascii_merch_len);
	printf("merchant_name_hash converted from bin to ASCII:\n %s \n",ascii_merch_len);

	sprintf( error_msg,"hash value generated by ATP (%s)",ascii_merch);
	txcntl_log_message( 3, 3,error_msg, "txservice_self_AAV_Validation_For_MC", 1 );
	memset(error_msg,0x00,sizeof(error_msg));
	sprintf( error_msg,"Merchant name(%s) Hash of merchant name:%s",merchant_name,ascii_merch);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );
	for (i=0;i<=merch_len;i++)
	{
		printf("%X", ascii_merch[i]);
	}
	printf("\n");
	strncat(data,ascii_merch,8);*/

	printf("Data prepared %s\n",data);

	sprintf( error_msg,"Data prepared:%s",data);
	txcntl_log_message( 3, 3,error_msg, "txservice_prepare_data_for_MC_hmac_verification", 1 );

	/*  Adding merchant name rulled out from the discussion */

}

BOOLEAN txservice_Check_AEVVResultCode_value_to_directly_decline_txn()
{
	if (Auth_Tx.TLF01_details.cvc == AMEX_AEVV_AUTHN_NOT_PERFORMED 		||
		Auth_Tx.TLF01_details.cvc == AMEX_AEVV_AUTHN_REJECT_BY_ISS
		)
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_get_3DS2_key_for_AMEX()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(KEY3DS01)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   // Initialize the global variables.
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &key3ds01,   0x00, sizeof(KEY3DS01  ) );


	   memcpy(key3ds01.caf_profile_id,Auth_Tx.CAF01_details.primary_key.caf_profile_id,sizeof(key3ds01.caf_profile_id));
	   memcpy (buffer, &Auth_Tx, sizeof Auth_Tx) ;
	   memcpy (buffer + sizeof Auth_Tx, &key3ds01, sizeof key3ds01) ;
	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	 ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
									 KEY3DS01_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                        &p_msg_in,
															tran_dataserver_que_name,
	                                                         error_msg );
			if ( rcode == PTEMSG_OK )
			{
				 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
				 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

				 memcpy( &key3ds01, p_data+sizeof(AUTH_TX), sizeof(key3ds01) );
				 free( p_msg_in );
			}
	   }
	   return true;
}

BOOLEAN txservice_AMEX_3DS2_self_validation()
{
	/*slef validation command not available, decline as of now */
	if (true == txservice_AMEX_get_AV_from_3DES2keyEncrypn())
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOLEAN txservice_AMEX_get_AV_from_3DES2keyEncrypn()
{

	/*unsigned short block1[20] = {0x99,0x12,0x59,0x87,0x65,0x43,0x21,0x00};
	unsigned short block2[20] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};*/

	BYTE accnt_blk1[50] = {0};
	BYTE accnt_blk2[50] = {0};
	BYTE bufferA[50]    = {0};
	BYTE bufferB[50]    = {0};
	BYTE block1[50]      = {0};
	BYTE block2[20]      = {0};
	BYTE data_blk[100]   = {0};
	BYTE hard_key1[50]   = {0};
	BYTE hard_key2[8]   = {0};
	BYTE key1[50]		= {0};
	BYTE key2[16]       = {0};
	char gen_AV_OP[4]   = {0};
	char Fst_AV_op[17]  = {0};
    char Alpha_data[17] = {0};
	char ext_op[17]     = {0};
	INT buffer_len;
	INT i=0, j=0, k=0;
	BYTE TrpDES_op[50] = {0};
	BYTE Trpl_DES_res[50]  = {0};

	//create an 32 bit Account Block
	memset(data_blk, 0x00, sizeof(data_blk));
	memset(accnt_blk1, 0x00, sizeof(accnt_blk1));
	memset(accnt_blk2, 0x00, sizeof(accnt_blk2));

	/* Step 1 start */
	//Add last 4 digits of ATN
	strncpy(accnt_blk1, Auth_Tx.TLF01_details.dcf01_retired_cwk+26, 4);
	//Add 3 to 14 digits of PAN
	strncpy(accnt_blk1+4, Auth_Tx.TLF01_details.card_num+2, 12);
	/*Step1 Ends */

	/* Step2 start */
	//Add service code "000"
	strncpy(accnt_blk2, "000", 3);
	//Add 13 zeros to it
	strncpy(accnt_blk2, "0000000000000", 16);
	/* Step2 end */

	/*step 3 start */
	strncpy(data_blk, accnt_blk1, 16 );
	strncat(data_blk, accnt_blk2, 16);
	/*step 3 end */

	//Copy Account Block to buffer
	//strncpy(bufferA, data_blk, 16);
	//strncpy(bufferB, data_blk + 16, 16);

	for (i = 0,j =0; i<32, j<16;)
	{
		hexchar2byte(&block1[j], &data_blk[i]);
		j = j +1;
		i = i +2;
	}
	//hexchar2byte(block1, data_blk);
	//hexchar2word(block2, bufferB);

	/* Step 4 start */

	txservice_get_3DS2_key_for_AMEX();
	memcpy(key1, key3ds01.key3ds, sizeof(key1));

	/*strncpy(key1, &Auth_Tx.CAF01_details.cvk[3].cvkA, 16);
	strncat(key1, &Auth_Tx.CAF01_details.cvk[3].cvkB, 16);*/

	for (i = 0,j =0; i<48, j<24;)
	{
		hexchar2byte(&hard_key1[j], &key1[i]);
		j = j +1;
		i = i +2;
	}
	//hexchar2byte(hard_key1, key1);

	//hexchar2word(hard_key2, key2);


	/*static WORD hard_key1 [8]   = {0x1234, 0x5678, 0x90AB, 0xCDEF};
	static WORD hard_key2 [8]   = {0x1234, 0x5678, 0x90AB, 0xCDEF};*/


		buffer_len = 16;
		printf("Buf len is %d\n", buffer_len);

		add_log ("\n\n*****Simple Encryption*****", block1, buffer_len);
		i = des_encryption(block1, buffer_len, hard_key1, 0);
		add_log ("after encryption ", block1, i);

		/*memcpy(block2, block1, buffer_len);

		add_log ("\n\n*****Simple Encryption*****", block1, buffer_len);
		i = des_encryption_ex1(block2, buffer_len, hard_key2, 0);
		add_log ("after encryption ", block1, i);

		memcpy(block1+4, block2, 16);
		memcpy(hard_key1+4, hard_key2, 16);*/

		add_log ("\n\n*****Simple Encryption*****", block1, buffer_len);
		i = des_decryption(block1, buffer_len, hard_key1, 0);
		add_log ("after encryption ", block1, i);

		add_log ("\n\n*****Simple Encryption*****", block1, buffer_len);
		i = des_encryption(block1, buffer_len, hard_key1, 0);
		add_log ("after encryption ", block1, i);

		/* Step 4 end */

		byte2hexchar(TrpDES_op, block1, buffer_len);
		//byte2hexchar(Trpl_DES_res, block2, sizeof(block2));

		/*strncpy(TrpDES_op, block1, 8);
		strncpy(TrpDES_op+16, block2, 8);
		add_log ("Final encryption ", TrpDES_op, 16);*/

		strncpy (Trpl_DES_res, TrpDES_op+16, 16);

		ExtractOnlyNumber(Trpl_DES_res, Fst_AV_op);
		if(strlen(Fst_AV_op) < 12)
		{
			for (i = 0; i< 16; i++)
			{
				if (Trpl_DES_res[i] >= 'A' && Trpl_DES_res[i] <= 'F' && i<5)
						Alpha_data[k++] = (Trpl_DES_res[i]-'A')+'1';
				else if (Trpl_DES_res[i] >= 'A' && Trpl_DES_res[i] <= 'D' && i>=5 && i<=15)
						Alpha_data[k++]  = (Trpl_DES_res[i]-'A')+'6';
				else if (Trpl_DES_res[i] >= 'E' && Trpl_DES_res[i] <= 'F' && i>=5 && i<=15)
						Alpha_data[k++]  = (Trpl_DES_res[i]-'E') + '0';


			}
			Alpha_data[k]= '\0';
			strcat(Fst_AV_op, Alpha_data);
		}

		//Copy final AV output
		strncpy(gen_AV_OP, Fst_AV_op+9, 3);

		/* Compare generated CAVV with CAVV received from VISA */
		if (0 == strncmp(gen_AV_OP, &Auth_Tx.TLF01_details.dcf01_retired_cwk[7], 3))
		{
			//Set this flag to build DE61
			Auth_Tx.current_balance_sign = 1;
			return true;
		}
		else
		{
			//Set this flag to build DE61
			Auth_Tx.current_balance_sign = 0;
			return false;
		}
}

BOOLEAN txservice_check_from_Amex_3DS2_txn()
{
	/* ATP will not do validation for the 3DS version and restore it when required.*/
	/*if(strncmp(Auth_Tx.TLF01_details.workstation,"02", 2) == 0 &&
	   strncmp(Auth_Tx.TLF01_details.handler_queue, "ncposA",5) == 0)
	{
		return true;
	}*/

	if(strncmp(Auth_Tx.TLF01_details.handler_queue, "ncposA",5) == 0)
	{
		return true;
	}
	return false;

}
BOOLEAN txservice_check_txn_for_fully_authentication()
{

	if (0 == strncmp(Auth_Tx.TLF01_details.profile,"05",2) &&
			Auth_Tx.TLF01_details.cvc == AMEX_AEVV_FULLY_AUTHENTICATION	)
	{
		return true;
	}
	return false;
}
BOOLEAN txservice_check_txn_for_authentication_attempt()
{
	if ( 0 == strncmp(Auth_Tx.TLF01_details.profile,"06",2) &&
		(Auth_Tx.TLF01_details.cvc == AMEX_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION ||
		 Auth_Tx.TLF01_details.cvc == AMEX_AEVV_AUTN_ATTEMPT_ACS_NOT_AVAILABLE))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_txn_for_CUP_authentication_attempt()
{
	if (0 == strncmp(Auth_Tx.TLF01_details.profile,"06",2) &&
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[1].amount[CUP_AV_AUTHENTICATION_RESULT_POS],"06",CUP_AV_AUTHENTICATION_RESULT_LEN) &&
		0 == strncmp(Auth_Tx.TLF01_details.product_codes[1].amount[CUP_AV_GENERATOR_INDICATOR_POS],'2',CUP_AV_AUTHENTICATION_RESULT_LEN) &&
		(Auth_Tx.TLF01_details.resp_source[10]=='2'))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_txn_for_JCB_authentication_attempt()
{
	if ((0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"07",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"08",2))&&
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"09",2))
	{
		return true;
	}
	return false;
}

BOOLEAN txservice_check_txn_for_JCB_Secure_auth_result_code()
{
	if (0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"02",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"03",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"04",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"05",2) ||
		0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[0],"09",2))
	{
		return true;
	}
	return false;
}
BOOLEAN txservice_Check_AEVVResultCode_value_to_skip_AEVV_validation()
{
	if(Auth_Tx.TLF01_details.cvc == AMEX_ISS_NOT_PARTICIPATING_IN_CAVV_VALIDATION		||
	   Auth_Tx.TLF01_details.cvc == AMEX_AEVV_AUTN_ATTEMPT_ACS_NOT_AVAILABLE
	  )
	{
		return true;
	}
	return false;
}


BOOLEAN txservice_Is_Txn_Qualify_For_Self_AV_Validation()
{
	if(( 0 == strncmp( Auth_Tx.TLF01_details.profile,
			CUP_UPI3DS_AUTH_MODE_ISSUER,
		    CUP_DE60_TERM_TYPR_LEN)) &&
			(0 == strncmp(&Auth_Tx.TLF01_details.resp_source[10],"1",1)) &&
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[1].amount[4],"1",1)) &&
			(Auth_Tx.TLF01_details.resp_source[10]!='2') &&
			cup_3ds2.av_validation_flag[0]=='Y')
		{
			return true;
		}
		return false;
}

BOOLEAN txservice_Is_Txn_Qualify_For_Self_JCB_CAVV_Validation()
{
	if(( 0 == strncmp( &Auth_Tx.TLF01_details.product_codes[3].amount[0], "00", 2)) &&
			((0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"01",2)) ||
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"02",2))  ||
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"03",2))   ||
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"04",2)) ||
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[2],"05",2))) &&
			(0 == strncmp(&Auth_Tx.TLF01_details.product_codes[3].amount[4],"01",2))  )
		{
			return true;
		}
		return false;
}

BOOLEAN txservice_verify_JCB_self_validation()
{
	/*char Supplemental_output[33] = "0";
	char buffer[512]={0};
	TxDataServer_JCB_Input( &Supplemental_output);
	sprintf (buffer, "Supplemental Data is %s",Supplemental_output);
	txcntl_log_message( 2,1,buffer, "txservice_verify_JCB_self_validation", 0 );
	if(true == txservice_JCB_get_CAVV_from_DESEncrypn(Supplemental_output))
	{
		return true;
	}
	else
	{
		return false;
	}*/

	return true;

}

BOOLEAN txservice_JCB_get_CAVV_from_DESEncrypn(pBYTE block)
{
	//BYTE block = "29547599606445197993099000000000";
	BYTE blockA[50];
	BYTE blockB[50];
	BYTE keyA [50];
	BYTE keyB [50];
	char gen_CAVV[4];
	INT buffer_len;
	INT i, j, k;
	BYTE buf_copy[50];
	char HexBuffer[50]={0};
	char HexBuffer1[50]={0};
	char HexBuffer2[50]={0};
	char HexBuffer3[50]={0};
	char HexBuffer4[50]={0};
	char HexBuffer5[50]={0};
	char HexBuffer6[50]={0};
	char HexBuffer7[50]={0};
	BYTE block1[20] = {0};
	BYTE block2[20] = {0};
	char buffer[512]={0};


	memset(blockA, 0x00, sizeof(blockA));
	memset(blockB, 0x00, sizeof(blockB));
	memset(keyA, 0x00, sizeof(keyA));
	memset(keyB, 0x00, sizeof(keyB));


	strncpy(blockA, block, 16);
	hexchar2word(block1,blockA);
	strncpy(blockB, block+16, 16);
	hexchar2word(block2,blockB);
	string2hexString(block2,HexBuffer5);
	static WORD hard_key [4]   = {0};
	static WORD hard_key1[4]   = {0};


	strncpy(keyA, Auth_Tx.CAF01_details.csck, 16);
	sprintf (buffer, "KEY A Value %s",keyA);
	txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );

	hexchar2word(hard_key, keyA);
	string2hexString(hard_key,HexBuffer6);
	sprintf (buffer, "hard_key Value %s",hard_key);
	txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );

	strncpy(keyB, Auth_Tx.CAF01_details.csck+16, 16);
	sprintf (buffer, "KEY B Value %s",keyB);
	txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0);

	hexchar2word(hard_key1, keyB);
	string2hexString(hard_key1,HexBuffer7);
	sprintf (buffer, "hard_key1 Value %s",hard_key1);
	txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0);

		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer);
		sprintf (buffer, "des_encryption_ex1_1 %s",HexBuffer);
		txcntl_log_message( 2, 3,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );


		buffer_len = strlen(block1);

		for (i=0; i< strlen(block1); i++)
		{
			block1[i]^= block2[i];
		}
		string2hexString(block1,HexBuffer1);
		sprintf (buffer, "Block 1 XOR %s",HexBuffer1);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer2);
		sprintf (buffer, "des_encryption_ex1_2 %s",HexBuffer2);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );
		buffer_len = strlen(block1);

		i = des_decryption_ex1(block1, buffer_len, hard_key1, 0);
		string2hexString(block1,HexBuffer3);
		sprintf (buffer, "des_decryption_ex1_3 %s",HexBuffer3);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer4);
		sprintf (buffer, "des_encryption_ex1_4 %s",HexBuffer4);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );
		buffer_len = strlen(block1);

		memset(buf_copy, 0x00, sizeof(buf_copy));

		byte2hexchar(buf_copy, block1, buffer_len);
		sprintf (buffer, " buf_copy %s",buf_copy);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );

		buffer_len = strlen(buf_copy);

		memset(block1, 0x00, sizeof(block1));
		GetOnlyNumber(buf_copy, block1);
		sprintf (buffer, " CAVV Values block1 %s",block1);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );

		strncpy(gen_CAVV,block1, 3);
		sprintf (buffer, " Gen_CAVV %s",gen_CAVV);
		txcntl_log_message( 2, 1,buffer, "txservice_JCB_get_CAVV_from_DESEncrypn", 0 );

		if (0 == strncmp(gen_CAVV,&Auth_Tx.TLF01_details.product_codes[3].amount[7], 3))
		{
			return true;
		}
		else
		{
			return false;
		}


}

void TxDataServer_JCB_Input(char * Supplemental_output)
{
	strcpy(Supplemental_output,Auth_Tx.TLF01_details.card_num);
	strncat(Supplemental_output,&Auth_Tx.TLF01_details.product_codes[3].amount[10],2);
	strncat(Supplemental_output,&Auth_Tx.TLF01_details.product_codes[6].amount[0],2);
	strncat(Supplemental_output,&Auth_Tx.TLF01_details.product_codes[3].amount[0],2);
	strncat(Supplemental_output,&Auth_Tx.TLF01_details.product_codes[3].amount[2],2);
	strcat(Supplemental_output,JCB_PADDED_ZEROES);
}

INT txservice_Check_If_CUP_3DS2(void)
{
	if( true == txservice_Check_If_CUP_Transaction())
	{
		if (( 0 == strncmp( Auth_Tx.TLF01_details.product_codes[3].quantity,
							CUP_UPI3DS_AUTH_MODE_ISSUER,
						    CUP_DE60_TERM_TYPR_LEN))     ||
			( 0 == strncmp( Auth_Tx.TLF01_details.product_codes[3].quantity,
								CUP_UPI3DS_AUTH_MODE_UPI_ATTEMPT,
					        CUP_DE60_TERM_TYPR_LEN)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}



BOOLEAN txservice_check_for_ASI_txn()
{
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) 	||
		0 == strncmp(Auth_Tx.TLF01_details.message_type,"0110",4))	&&
	   (MC_ASI_TXN_IDENTIFIER_VALUE == Auth_Tx.TLF01_details.product_codes[16].amount[6]))
	{
		return true;
	}
	return false;
}

INT txservice_Check_If_IST_Message(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nceqit",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"neqit2",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"nceqit",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"neqit2",6) ||
		0 == strncmp(Auth_Tx.BIN01_details.destination_name,"nceqit",6)||
		0 == strncmp(Auth_Tx.BIN01_details.destination_name,"neqit2",6)	||
		0 == strncmp(Auth_Tx.BIN01_details.alt_destination_name,"nceqit",6) ||
		0 == strncmp(Auth_Tx.BIN01_details.alt_destination_name,"neqit2",6))

	{

		/*Check if this Cash Advanc transaction coming from IST*/
		if( ((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0200",4))) &&
			((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncmcr2",6)) ||
			(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncvis2",6)) ||
			(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"natma2",6)) ||
			(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncjcb2",6)) ||
			(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"nccup2",6)) ||
			(0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncdci2",6))))
		{
			/* In case of Real time this is a valid credit transaction so we are returning as false
			 * to indicate its not a debit transaction. Cash advance transaction coming from IST will
			 * Qualify for Real time .
			 */
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_Fleet_Transaction(void)
{
	if(0 == strncmp(Auth_Tx.TLF01_details.card_num, "7",1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT parse_field_48( int src_len, char *src, pBYTE ucaf_data )
{
   #define  FIELD48_MAX_LEN  100

   INT  ret_val = 0;
   INT  sub_field_len;
   INT  MainIndex;
   INT  TotalLen;
   CHAR tag[3];
   CHAR len[3];


   TotalLen =  src_len ;

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;

 //     /* Get length of subfield. */
 //     if ( 0 == strcmp(tag, DE48_TAG43) )
 //     {
 //        /* Subfield 43 is fixed length. */
 //        sub_field_len = DE48_TAG43_LEN;

 //        /* Currently, do not need the data from this subfield. */
 //     }
//      else
 //     {
         /* Other subfields are variable length. */
         memset( len, 0x00, sizeof(len) );
         memcpy( len, &src[MainIndex], 2 );
         sub_field_len = atoi( len );
         MainIndex += 2;

         /* Get the data from these subfields. */
		if ( 0 == strncmp(tag, DE48_TAG43, 2) )
		 {
			memcpy(ucaf_data,&src[MainIndex],sub_field_len);
		 }

 //     }
      MainIndex += sub_field_len;
   }
   return( ret_val );
}  /* End parse_field_48 */

BOOLEAN txservice_get_3DS2_key_for_MC()
{
	   pPTE_MSG	     p_msg_out;
	   pPTE_MSG	     p_msg_in;
	   INT           i;
	   INT           retcode;
	   INT           retval = true;
	   pBYTE         p_data;
	   pPTE_MSG_DATA p_msg_data;
	   BYTE          rcode;
	   BYTE          buffer[sizeof(AUTH_TX) + sizeof(KEY3DS01)];
	   AUTH_TX       auth_tx;
	   CHAR          error_msg[100]      = "",strTemp[100];

	   // Initialize the global variables.
	   memset( buffer,   0x00, sizeof(buffer ) );
	   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
	   memset( &key3ds01,   0x00, sizeof(KEY3DS01  ) );


	   memcpy(key3ds01.caf_profile_id,Auth_Tx.CAF01_details.primary_key.caf_profile_id,sizeof(key3ds01.caf_profile_id));
	   memcpy (buffer, &Auth_Tx, sizeof Auth_Tx) ;
	   memcpy (buffer + sizeof Auth_Tx, &key3ds01, sizeof key3ds01) ;
	   p_msg_in  = NULL_PTR;
	   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
			   	   	   	   	   	   	 ST1_DB_SELECT,
	                                 "",
	                                 NULL_PTR,
	                                 interactive_que_name,
	                                 (pBYTE) buffer,
	                                 sizeof( buffer),
									 KEY3DS01_DATA );
	   if( NULL_PTR == p_msg_out )
	   {
	      strcpy( error_msg, "Insufficient memory to build request message." );
	      retval = false;
	   }
	   else
	   {
		   rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
	                                                        &p_msg_in,
															tran_dataserver_que_name,
	                                                         error_msg );
			if ( rcode == PTEMSG_OK )
			{
				 p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
				 p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

				 memcpy( &key3ds01, p_data+sizeof(AUTH_TX), sizeof(key3ds01) );
				 free( p_msg_in );
			}
	   }
	   return true;
}
BOOLEAN txservice_CUP_get_AVN_from_DESEncrypn(pBYTE block)
{
	//BYTE block = "29547599606445197993099000000000";
	BYTE blockA[50];
	BYTE blockB[50];
	BYTE keyA [50];
	BYTE keyB [50];
	char gen_CAVV[4];
	INT buffer_len;
	INT i, j, k;
	BYTE buf_copy[50];
	char HexBuffer[50]={0};
	char HexBuffer1[50]={0};
	char HexBuffer2[50]={0};
	char HexBuffer3[50]={0};
	char HexBuffer4[50]={0};
	char HexBuffer5[50]={0};
	BYTE block1[20] = {0};
	BYTE block2[20] = {0};

	memset(blockA, 0x00, sizeof(blockA));
	memset(blockB, 0x00, sizeof(blockB));
	memset(keyA, 0x00, sizeof(keyA));
	memset(keyB, 0x00, sizeof(keyB));

	strncpy(blockA, block, 16);
	hexchar2word(block1,blockA);
	strncpy(blockB, block+16, 16);
	hexchar2word(block2,blockB);
	string2hexString(block2,HexBuffer5);
	static WORD hard_key [4]   = {0};
	static WORD hard_key1[4]   = {0};

	strncpy(keyA, Auth_Tx.CAF01_details.csck, 16);
	hexchar2word(hard_key, keyA);
	strncpy(keyB, Auth_Tx.CAF01_details.csck+16, 16);
	hexchar2word(hard_key1, keyB);

		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer);


		buffer_len = strlen(block1);

		for (i=0; i< strlen(block1); i++)
		{
			block1[i]^= block2[i];
		}
		string2hexString(block1,HexBuffer1);
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer2);
		buffer_len = strlen(block1);

		i = des_decryption_ex1(block1, buffer_len, hard_key1, 0);
		string2hexString(block1,HexBuffer3);
		buffer_len = strlen(block1);

		i = des_encryption_ex1(block1, buffer_len, hard_key, 0);
		string2hexString(block1,HexBuffer4);
		buffer_len = strlen(block1);

		memset(buf_copy, 0x00, sizeof(buf_copy));

		byte2hexchar(buf_copy, block1, buffer_len);

		buffer_len = strlen(buf_copy);

		memset(block1, 0x00, sizeof(block1));
		GetOnlyNumber(buf_copy, block1);
		strncpy(gen_CAVV,block1, 3);

		/* Compare generated CAVV with CAVV received from VISA */
		if (0 == strncmp(gen_CAVV,&Auth_Tx.TLF01_details.product_codes[1].amount[5], 3))
		{
			return true;
		}
		else
		{
			return false;
		}

}


INT txservice_Check_If_MC_issuing_transaction(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
	{
		return true;
	}
	else
	{
		return false;
	}


}

INT txservice_If_Tx_Qualifies_For_RealTime()
{

	if (true == txservice_check_RTlink_for_card() &&
		true == txservice_check_fraud_rules_for_card() &&
		true == txservice_check_if_transaction_qualifies_for_Realtime())
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT txservice_check_if_transaction_qualifies_for_Realtime()
{
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_CASH_ADVANCE:
		case AUTH_BALANCE_INQUIRY:
		case AUTH_MONEY_SEND:
		case AUTH_AUTHORIZATION:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_SALE:
		case AUTH_CARD_VERIFICATION:
		case AUTH_PAYMENT:
		case AUTH_QUASI_CASH:
		case AUTH_OCT_TX:
		case AUTH_FAST_FUND:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
		case AUTH_CASH_ADVANCE_RESPONSE:
		case AUTH_BALANCE_INQUIRY_RESPONSE:
		case AUTH_MONEY_SEND_RESPONSE:
		case AUTH_AUTHORIZATION_RESPONSE:
		case AUTH_PRE_AUTHORIZATION_RESPONSE:
		case AUTH_SALE_RESPONSE:
		case AUTH_CARD_VERIFICATION_RESPONSE:
		case AUTH_PAYMENT_RESPONSE:
		case AUTH_QUASI_CASH_RESPONSE:
		case AUTH_OCT_TX_RESPONSE:
		case AUTH_FAST_FUND_RESPONSE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE:
			return true ;

		default:
			return false ;
	}
}


INT txservice_check_fraud_rules_for_card()
{
	if (true == txservice_Check_If_VISA_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_VISA())
			return true;
	}
	else if (true == txservice_Check_If_MC_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_MC())
			return true;
	}
	else if (true == txservice_Check_If_AMEX_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_AMEX())
			return true;
	}
	else if (true == txservice_Check_If_Diners_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_DCI())
			return true;
	}
	else if (true == txservice_Check_If_CUP_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_CUP())
			return true;
	}
	else if (true == txservice_Check_If_JCB_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_JCB())
			return true;
	}
	else if (true == txservice_Check_if_PLFC_Transaction())
	{
		if (true == txservice_verify_fraud_rules_for_PLFC())
			return true;
	}

	return false;

}


INT txservice_verify_fraud_rules_for_VISA()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT rulematch = false;

	if (visa_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{

			if(visa_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{

				if (0 != strlen(visa_frual01.frule01_list[i].acq_country_code))
				{
				if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						visa_frual01.frule01_list[i].acq_country_code, 3))
				{
						return true;
					}
				}

				if (0 != strlen(visa_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, visa_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
				}
				if (0 != strlen(visa_frual01.frule01_list[i].mcc_1))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 visa_frual01.frule01_list[i].mcc_1 )))
					{
						return true;
					}
					}
				if (0 != strlen(visa_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 visa_frual01.frule01_list[i].mcc_2 )))
					{
						return true;
					}
					}
				if (0 != strlen(visa_frual01.frule01_list[i].mcc_3))
					{
						if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								visa_frual01.frule01_list[i].mcc_3)))
							{
						return true;
							}
						}
				if (0 != strlen(visa_frual01.frule01_list[i].pos_mode_1))
						{
							if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 visa_frual01.frule01_list[i].pos_mode_1, 3 )))
							{
						return true;
					}
							}
				if (0 != strlen(visa_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 visa_frual01.frule01_list[i].pos_mode_2, 3)))
							{
						return true;
							}
						}
				if (0 != strlen(visa_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 visa_frual01.frule01_list[i].pos_mode_3, 3)))
					{
						return true;
					}
				}
				}

			}
		}
		return false;
   }


INT txservice_verify_fraud_rules_for_MC()
{
	INT i = 0;
	double txn_amt = 0.0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT rulematch = false;

	if (mc_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{

			if(mc_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(mc_frual01.frule01_list[i].acq_country_code))
				{
				if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						mc_frual01.frule01_list[i].acq_country_code, 3))
				{
						return true;
					}
				}

				if (0 != strlen(mc_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, mc_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
				}

				if (0 != strlen(mc_frual01.frule01_list[i].mcc_1))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 mc_frual01.frule01_list[i].mcc_1 )))
					{
						return true;
					}
				}

				if (0 != strlen(mc_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 mc_frual01.frule01_list[i].mcc_2 )))
					{
						return true;
					}
					}

				if (0 != strlen(mc_frual01.frule01_list[i].mcc_3))
					{
						if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								mc_frual01.frule01_list[i].mcc_3)))
						{
						return true;
						}
						}

				if (0 != strlen(mc_frual01.frule01_list[i].pos_mode_1))
						{
							if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 mc_frual01.frule01_list[i].pos_mode_1, 3 )))
							{
						return true;
							}
				}

				if (0 != strlen(mc_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 mc_frual01.frule01_list[i].pos_mode_2, 3)))
							{
						return true;
							}
						}

				if (0 != strlen(mc_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 mc_frual01.frule01_list[i].pos_mode_3, 3)))
				{
					return true;
				}
			}
		}
		}
		return false;
	}
}

INT txservice_verify_fraud_rules_for_AMEX()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT rulematch = false;

	if (amex_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{
			if(amex_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(amex_frual01.frule01_list[i].acq_country_code))
				{
					if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						amex_frual01.frule01_list[i].acq_country_code, 3))
					{
						return true;
					}
					else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos", 5)&&
							0 == strncmp(Auth_Tx.TLF01_details.host_start_time,"0.000",4))
					{
						if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY, 3))
						{
							return true;
						}
					}
					else if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos", 5))&&
							(0 != strcmp(NULL_STR, Auth_Tx.reconciliation_cur_code)) &&
							(0 != strcmp(NULL_STR, Auth_Tx.reconciliation_amount)) &&
							(0 != strcmp(NULL_STR, Auth_Tx.TLF01_details.conversion_rate)))
					{
						if (0 == strncmp(Auth_Tx.reconciliation_cur_code,USDOLLAR_CURRENCY, 3))
						{
							return true;
						}
					}
				}

				if (0 != strlen(amex_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, amex_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
				}

				if (0 != strlen(amex_frual01.frule01_list[i].mcc_1))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 amex_frual01.frule01_list[i].mcc_1 )))
					{
						return true;
					}
					}

				if (0 != strlen(amex_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 amex_frual01.frule01_list[i].mcc_2 )))
					{
						return true;
					}
					}

				if (0 != strlen(amex_frual01.frule01_list[i].mcc_3))
					{
						if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								amex_frual01.frule01_list[i].mcc_3)))
						{
						return true;
						}
						}

				if (0 != strlen(amex_frual01.frule01_list[i].pos_mode_1 ))
						{
							if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 amex_frual01.frule01_list[i].pos_mode_1, 3 )))
							{
						return true;
					}
							}

				if (0 != strlen(amex_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 amex_frual01.frule01_list[i].pos_mode_2, 3)))
							{
						return true;
							}
						}

				if (0 != strlen(amex_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 amex_frual01.frule01_list[i].pos_mode_3, 3)))
				{
					return true;
				}
			}
		}
		}
		return false;
	}
}

INT txservice_verify_fraud_rules_for_DCI()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT  rulematch = false;

	if (dci_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{
			if(dci_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(dci_frual01.frule01_list[i].acq_country_code))
				{
				if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						dci_frual01.frule01_list[i].acq_country_code, 3))
				{
						return true;
					}
				}

				if (0 != strlen(dci_frual01.frule01_list[i].billing_amt))
				{
						strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
						txn_amt =  atoi(hex_txn_amt);

						strncpy(hex_blng_amt, dci_frual01.frule01_list[i].billing_amt, 12);
						blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
						{
						return true;
					}
						}

				if (0 != strlen(dci_frual01.frule01_list[i].mcc_1 ))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 dci_frual01.frule01_list[i].mcc_1  )))
					{
						return true;
					}
				}

				if (0 != strlen(dci_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 dci_frual01.frule01_list[i].mcc_2)))
						{
						return true;
					}
						}

				if (0 != strlen(dci_frual01.frule01_list[i].mcc_3))
						{
							if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									dci_frual01.frule01_list[i].mcc_3)))
							{
						return true;
							}
							}

				if (0 != strlen(dci_frual01.frule01_list[i].pos_mode_1 ))
							{
								if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 dci_frual01.frule01_list[i].pos_mode_1, 3  )))
								{
						return true;
					}
								}

				if (0 != strlen(dci_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 dci_frual01.frule01_list[i].pos_mode_2, 3)))
								{
						return true;
								}
							}

				if (0 != strlen(dci_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 dci_frual01.frule01_list[i].pos_mode_3, 3)))
				{
					return true;
				}
			}
		}
		}
		return false;
	}
}

INT txservice_verify_fraud_rules_for_CUP()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT  rulematch = false;

	if (cup_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{
			if(cup_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(cup_frual01.frule01_list[i].acq_country_code))
				{
				if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						cup_frual01.frule01_list[i].acq_country_code, 3))
				{
						return true;
					}
				}

				if (0 != strlen(cup_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, cup_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
					}

				if (0 != strlen(cup_frual01.frule01_list[i].mcc_1 ))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 cup_frual01.frule01_list[i].mcc_1  )))
					{
						return true;
					}
					}

				if (0 != strlen(cup_frual01.frule01_list[i].mcc_2))
					{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 cup_frual01.frule01_list[i].mcc_2)))
					{
						return true;
					}
				}

				if (0 != strlen(cup_frual01.frule01_list[i].mcc_3))
				{
						if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								cup_frual01.frule01_list[i].mcc_3)))
						{
						return true;
						}
						}

				if (0 != strlen(cup_frual01.frule01_list[i].pos_mode_1))
						{
							if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 cup_frual01.frule01_list[i].pos_mode_1, 3)))
							{
						return true;
							}
				}

				if (0 != strlen(cup_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 cup_frual01.frule01_list[i].pos_mode_2, 3)))
							{
						return true;
							}
						}

				if (0 != strlen(cup_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 cup_frual01.frule01_list[i].pos_mode_3, 3)))
				{
					return true;
				}
			}

			}
		}
		return false;
	}
}

INT txservice_verify_fraud_rules_for_JCB()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT  rulematch = false;

	if (jcb_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
	}
	else
	{
		for (i=0; i<10; i++)
		{
			if(jcb_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(jcb_frual01.frule01_list[i].acq_country_code))
				{
					if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
						jcb_frual01.frule01_list[i].acq_country_code, 3))
					{
						return true;
					}
					else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncjcb3", 6)&&
							0 == strncmp(Auth_Tx.TLF01_details.host_start_time,"0.000",4))
					{
						if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,PESO_CURRENCY, 3))
						{
							return true;
						}
					}
				}

				if (0 != strlen(jcb_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, jcb_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
				}

				if (0 != strlen(jcb_frual01.frule01_list[i].mcc_1 ))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 jcb_frual01.frule01_list[i].mcc_1 )))
					{
						return true;
					}
					}

				if (0 != strlen(jcb_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
									 jcb_frual01.frule01_list[i].mcc_2)))
					{
						return true;
					}
					}

				if (0 != strlen(jcb_frual01.frule01_list[i].mcc_3))
					{
						if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								jcb_frual01.frule01_list[i].mcc_3)))
						{
						return true;
					}
				}

				if (0 != strlen(jcb_frual01.frule01_list[i].pos_mode_1 ))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
								jcb_frual01.frule01_list[i].pos_mode_1, 3 )))
					{
						return true;
						}
				}

				if (0 != strlen(jcb_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									 jcb_frual01.frule01_list[i].pos_mode_2, 3)))
						{
						return true;
					}
						}

				if (0 != strlen(jcb_frual01.frule01_list[i].pos_mode_3))
						{
							if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
									jcb_frual01.frule01_list[i].pos_mode_3, 3)))
							{
						return true;
					}
				}


			}
		}
		return false;
	}
}

INT txservice_verify_fraud_rules_for_PLFC()
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT  rulematch = false;

	if (iftr_fclf01.fraud_rule_enable_flag[0] == '0')
	{
		return true;
							}
							else
							{
		for (i=0; i<10; i++)
		{
			if(pl_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(pl_frual01.frule01_list[i].acq_country_code))
				{
					if (0 == strncmp(Auth_Tx.TLF01_details.currency_code,
								pl_frual01.frule01_list[i].acq_country_code, 3))
					{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, Auth_Tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, pl_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
							}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_1 ))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								pl_frual01.frule01_list[i].mcc_1 )))
					{
						return true;
					}
						}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								pl_frual01.frule01_list[i].mcc_2)))
					{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_3))
				{
					if ((0 == strcmp(Auth_Tx.TLF01_details.category_code,
								pl_frual01.frule01_list[i].mcc_3)))
					{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_1 ))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
								pl_frual01.frule01_list[i].pos_mode_1, 3 )))
				{
					return true;
				}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
								pl_frual01.frule01_list[i].pos_mode_2, 3)))
					{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
								pl_frual01.frule01_list[i].pos_mode_3, 3)))
					{
						return true;
					}
				}


			}
		}
		return false;
	}
}

INT txservice_check_RTlink_for_card()
{
	if (true == txservice_Check_If_VISA_Transaction())
	{
		if (true == txservice_check_link_for_VISA())
			return true;
	}
	else if (true == txservice_Check_If_MC_Transaction())
	{
		if (true == txservice_check_link_for_MC())
			return true;
	}
	else if (true == txservice_Check_If_AMEX_Transaction())
	{
		if (true == txservice_check_link_for_AMEX())
			return true;
	}
	else if (true == txservice_Check_If_Diners_Transaction())
	{
		if (true == txservice_check_link_for_DCI())
			return true;
	}
	else if (true == txservice_Check_If_CUP_Transaction())
	{
		if (true == txservice_check_link_for_CUP())
			return true;
	}
	else if (true == txservice_Check_If_JCB_Transaction())
	{
		if (true == txservice_check_link_for_JCB())
			return true;
	}
	else if (true == txservice_Check_if_PLFC_Transaction())
	{
		if (true == txservice_check_link_for_PLFC())
			return true;
	}

	return false;
}

INT txservice_check_link_for_VISA()
{
	if((true  == txservice_Check_if_POS_Acq_Transaction()) &&
	   (0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncvis2",6)) &&
	   ('1' == visa_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncvis2",6)) &&
			('1' == visa_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name, "ncvis2",6)) &&
			('1' == visa_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_MC()
{
	if( (true == txservice_Check_if_POS_Acq_Transaction()) &&
		(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncmcr2",6)) &&
		('1' == mc_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncmcr2",6)) &&
			('1' == mc_fclf01.fraud_control_enable_flag[0]))

	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name, "ncmcr2",6)) &&
			('1' == mc_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_AMEX()
{
	if( (true == txservice_Check_if_POS_Acq_Transaction()) &&
		(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nposa2",6)) &&
		('1' == amex_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "nposa2",6)) &&
			('1' == amex_fclf01.fraud_control_enable_flag[0]))

	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "natma2",6)) &&
			('1' == amex_fclf01.fraud_control_enable_flag[0]))

	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nposa2",6)) &&
			('1' == amex_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_DCI()
{
	if( (true == txservice_Check_if_POS_Acq_Transaction()) &&
		(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncdci2",6))&&
		('1' == dci_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncdci2",6)) &&
		   ('1' == dci_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncdci2",6)) &&
			('1' == dci_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_CUP()
{
	if((true == txservice_Check_if_POS_Acq_Transaction()) &&
	   (0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nccup2",6)) &&
	   ('1' == cup_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "nccup2",6)) &&
			('1' == cup_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nccup2",6)) &&
			('1' == cup_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_JCB()
{
	if((true == txservice_Check_if_POS_Acq_Transaction()) &&
	   (0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncjcb2",6)) &&
	   ('1' == jcb_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "ncjcb2",6)) &&
			('1' == jcb_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"ncjcb2",6)) &&
			('1' == jcb_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_check_link_for_PLFC()
{
	if((true == txservice_Check_if_POS_Acq_Transaction() ||
		0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpimp",6)) &&
	   (0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nciftr",6)) &&
	   ('1' == iftr_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((0 == strncmp(Auth_Tx.TLF01_details.authorizing_host_queue, "nciftr",6)) &&
			('1' == iftr_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else if((true == txservice_Check_if_Voice_Acq_Transaction()) &&
			(0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nciftr",6)) &&
			('1' == iftr_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT txservice_Check_If_JCB_issuing_transaction(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncjcb",5) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncjcb3",6))
	{
		return true;
	}
	else
	{
		return false;
	}


}
INT txservice_Check_If_AMEX_issuing_transaction(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5))
	{
		return true;
	}
	else
	{
		return false;
	}


}
INT txservice_Check_If_DCI_Transaction(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci",5) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncdci3",6))
	{
		return true;
	}
	else
	{
		return false;
	}


}

INT txservice_Check_If_CUP_issuing_Transaction(void)
{
	if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5) ||
	    0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup3",6))
	{
		return true;
	}
	else
	{
		return false;
	}


}

INT txservice_check_if_txn_is_cup3ds2_issuing_txn()
{
	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "nccup", 5) &&
	   0 == strncmp( Auth_Tx.TLF01_details.product_codes[3].quantity, CUP_ECOM_TERM_TYPE, CUP_DE60_TERM_TYPR_LEN) &&
       0 == strncmp( Auth_Tx.TLF01_details.profile, CUP_UPI3DS_AUTH_MODE_ISSUER, CUP_DE60_TERM_TYPR_LEN) &&
	   0 == strncmp( Auth_Tx.future_use_2 + CUP_DE61_AM_USAGE_TAG_POS, CUP_DE61_AM_USAGE_TAG, CUP_DE_61_AM_USAGE_TAG_LEN) &&
	   0 == strncmp( Auth_Tx.TLF01_details.resp_source[AVN_INDICATOR_POS],'1',1) &&
	   0 == strncmp( Auth_Tx.TLF01_details.product_codes[12].amount, "TF", 2) &&
	   Auth_Tx.TLF01_details.product_codes[13].amount[0] != '\0')
	{
		return (true);
	}
	else
	{
		return (false);
	}

}

BOOLEAN txservice_check_if_txn_is_IST_transaction(void)
{
	if(  0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
					   nceqit_que_name, IST_MODULE_COMP_LEN)||
		 0 == strncmp( Auth_Tx.TLF01_details.handler_queue,
				 	   neqit2_que_name,IST_MODULE_COMP_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}

}

#ifdef VISA7
INT txcntl_read_system_integrity_parameters( void )
{
    DWORD rc;
    CHAR filename     [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
    CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
    CHAR rc_data[256] ={0};
    INT  ret_val = true;
    /* Determine directory and filename of the .ini file. */

    cat1_MID_integrity_validation_flag='\0',cat2_MID_integrity_validation_flag='\0';
    cat3_MID_integrity_validation_flag='\0',cat4_MID_integrity_validation_flag='\0';

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY1",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    txcntl_log_message(2,2,"system_integrity_parameters CATEGORY1 not read","txcntl_read_system_integrity_parameters", 0);
    }
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat1_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY2",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );
    if(rc != 1)
    {
	    txcntl_log_message(2,2,"system_integrity_parameters CATEGORY2 not read","txcntl_read_system_integrity_parameters", 0);
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat2_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY3",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    txcntl_log_message(2,2,"system_integrity_parameters CATEGORY3 not read","txcntl_read_system_integrity_parameters", 0);
    }
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat3_MID_integrity_validation_flag = tmpstring[0];

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "MERCHANT_INTEGRITY_VISA",
             "CATEGORY4",
             "Y",
             ini_info,
             MAX_INI_INFO_SIZE - 1,
             filename
    );

    if(rc != 1)
    {
	    txcntl_log_message(2,2,"system_integrity_parameters CATEGORY4 not read","txcntl_read_system_integrity_parameters", 0);
    }

    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    cat4_MID_integrity_validation_flag = tmpstring[0];

    return( ret_val );
}
#endif
#ifdef MC12_OCT2023
INT txcntl_read_execssive_delcines_parameter_for_mc()
{
    DWORD rc;
    CHAR filename     [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
    CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
    CHAR rc_data[256] ={0};
    INT  ret_val = true;
    INT index=0,rc_index=0;

    excessive_declines_flag='\0'; cat1_excessive_declines_validation_flag='\0'; cat2_excessive_declines_validation_flag='\0';

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "EXCESSIVE_DECLINES_FLAG",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   txcntl_log_message(2,2,"EXCESSIVE_DECLINES_FLAG not read","txcntl_read_execssive_delcines_parameter_for_mc",0);
	}

	excessive_declines_flag = ini_info[0];

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "CATEGORY1",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   txcntl_log_message(2,2,"CATEGORY1 validation flag not read","txcntl_read_execssive_delcines_parameter_for_mc",0);
	}

	cat1_excessive_declines_validation_flag=ini_info[0];

    rc = GetPrivateProfileString(
                "MERCHANT_INTEGRITY_MC",
                "CATEGORY2",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename
       	   	   	   	   	   	   );

	if(rc != 1)
	{
	   txcntl_log_message(2,2,"CATEGORY1 validation flag not read","txcntl_read_execssive_delcines_parameter_for_mc",0);
	}

	cat2_excessive_declines_validation_flag=ini_info[0];

	return ret_val;
}
#endif

#ifdef MC13_OCT2023
INT txcntl_read_mac_validation_flag()
{
	INT  ret_val = true;
    DWORD rc;
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    mac_cat1_validation_flag='\0';
    mac_cat2_validation_flag='\0';
    memset(mac_pos_entry_mode_cat1, 0x00, sizeof(mac_pos_entry_mode_cat1));
    memset(mac_pos_cond_code_cat1,  0x00, sizeof(mac_pos_cond_code_cat1));
    memset(mac_pos_entry_mode_cat2, 0x00, sizeof(mac_pos_entry_mode_cat2));
    memset(mac_pos_cond_code_cat2,  0x00, sizeof(mac_pos_cond_code_cat2));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, MERCHANT_INTEGRITY_INI_FILE);

    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "CATEGORY1",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   txcntl_log_message(2, 2, "CATEGORY1 not read", "txcntl_read_mac_validation_flag", 0);
	}

	mac_cat1_validation_flag = ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "CATEGORY2",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "CATEGORY2 not read", "txcntl_read_mac_validation_flag", 0);
	}

	mac_cat2_validation_flag = ini_info[0];

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_ENTRY_MODE_CAT1",
                "0012",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "POS_ENTRY_MODE_CAT1 validation flag not read",
								 "txcntl_read_mac_validation_flag", 0);
	}

	strncpy(mac_pos_entry_mode_cat1 ,ini_info, sizeof(mac_pos_entry_mode_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_COND_CODE_CAT1",
                "00",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "POS_COND_CODE_CAT1 validation flag not read",
								 "txcntl_read_mac_validation_flag", 0);
	}

	strncpy(mac_pos_cond_code_cat1 ,ini_info, sizeof(mac_pos_cond_code_cat1)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_ENTRY_MODE_CAT2",
                "0012",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "POS_ENTRY_MODE_CAT2 validation flag not read",
								 "txcntl_read_mac_validation_flag", 0);
	}

	strncpy(mac_pos_entry_mode_cat2 ,ini_info, sizeof(mac_pos_entry_mode_cat2)-1);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                "MC_MERCHANT_ADVICE_CODE",
                "POS_COND_CODE_CAT2",
                "08",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "POS_COND_CODE_CAT2 validation flag not read",
								 "txcntl_read_mac_validation_flag", 0);
	}

	strncpy(mac_pos_cond_code_cat2 ,ini_info, sizeof(mac_pos_cond_code_cat2)-1);

	return ret_val;
}
#endif

#ifdef HCTS2
void txcntl_read_atp_card_activation_parameters()
{
    int ind, index, statusIndex;
	DWORD rc;
    CHAR statusBuffer[256] = {0};
    CHAR dataBuffer[51] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    card_activation_flag='\0';
    memset(card_updating_status, 0x00, sizeof(card_updating_status));
    memset(card_current_status,  0x00, sizeof(card_current_status));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "CARD_ACTIVATION",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	   txcntl_log_message(2, 2, "CARD_ACTIVATION not read", "txcntl_read_atp_card_activation_parameters", 0);
	}

	card_activation_flag = ini_info[0];
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );


	rc = GetPrivateProfileStringSpaces(
	                "ATP_CARD_ACTIVATION",
	                "UPDATING_STATUS",
	                "  ",
	                ini_info,
	                MAX_INI_INFO_SIZE - 1,
	                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "UPDATING_STATUS not read", "txcntl_read_atp_card_activation_parameters", 0);
	}

	strncpy(card_updating_status, ini_info, 2);
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );


	rc = GetPrivateProfileString(
	                "ATP_CARD_ACTIVATION",
	                "CURRENT_STATUS",
	                "",
	                ini_info,
	                MAX_INI_INFO_SIZE - 1,
	                filename);

	if(rc != 1)
	{
		txcntl_log_message(2, 2, "CURRENT_STATUS not read", "txcntl_read_atp_card_activation_parameters", 0);
	}

	strncpy(statusBuffer, ini_info, strlen(ini_info));
	memset(dataBuffer, 0x00, sizeof(dataBuffer));
    ind = 0, index = 0; statusIndex = 0;
    if( card_activation_flag == 'Y' &&
        strlen(statusBuffer) > 0    &&
		statusBuffer[(strlen(statusBuffer)-1)] == ';'
	  )
    {
		while(statusBuffer[index] != ';')
		{
			if(statusBuffer[index] != ',')
			{
				card_current_status[statusIndex][ind] = statusBuffer[index];
				ind++;
			}
			else
			{
				statusIndex++;
				ind = 0;
			}
			index++;
		}
		total_card_current_status = statusIndex+1;
    }
    else
    {
    	txcntl_log_message(2, 2, "CURRENT_STATUS contains invalid data", "txcntl_read_atp_card_activation_parameters", 0);
    }

    if( card_activation_flag == 'Y')
    	txcntl_read_vertexon_codes();
}

void txcntl_read_vertexon_codes()
{
	#define MAX_INI_INFO_SIZE 256

    int ind, index, codeIndex;
	DWORD rc;
	CHAR codesCount[3] = {0};
    CHAR codesBuffer[256] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    memset(visa_vertexon_codes,  0x00, sizeof(visa_vertexon_codes));
    memset(mc_vertexon_codes,    0x00, sizeof(mc_vertexon_codes));
    memset(jcb_vertexon_codes,   0x00, sizeof(jcb_vertexon_codes));
    memset(amex_vertexon_codes,  0x00, sizeof(amex_vertexon_codes));
    memset(dci_vertexon_codes,   0x00, sizeof(dci_vertexon_codes));
    memset(upi_vertexon_codes,   0x00, sizeof(upi_vertexon_codes));
    memset(mp_in_vertexon_codes, 0x00, sizeof(mp_in_vertexon_codes));
    memset(pl_ft_vertexon_codes, 0x00, sizeof(pl_ft_vertexon_codes));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    //VISA codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "VISA_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "VISA_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				visa_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(visa_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "VISA_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //MC codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "MC_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "MC_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mc_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mc_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "MC_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //JCB codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "JCB_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "JCB_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				jcb_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(jcb_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "JCB_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //AMEX codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "AMEX_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "AMEX_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				amex_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(amex_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "AMEX_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //DCI codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "DCI_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "DCI_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				dci_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(dci_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "DCI_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //UPI codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "UPI_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "UPI_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				upi_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(upi_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "UPI_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //MP/Installment codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "MP_IN_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "MP_IN_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mp_in_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mp_in_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "MP_IN_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //Pvt. Label/Fleet codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "PL_FT_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "PL_TF_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				pl_ft_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(pl_ft_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "PL_TF_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }
}
#endif

INT txcntl_read_tf_ini_TO_rc( pCHAR err_buf )
{
   DWORD rc;
   CHAR filename     [256]= {0};
   CHAR tmpstr       [256]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);


	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_MC",  /* points to key name           */
			"19",                       /* points to default string     */
			 ini_info,                      /* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
			 filename                       /* points to ini filename       */
	);

	strncpy(mc_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_VISA",  /* points to key name           */
		   "19",                       /* points to default string     */
			ini_info,                      /* points to destination buffer */
			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
			filename                       /* points to ini filename       */
	);

	strncpy(visa_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_JCB",  /* points to key name           */
		  "19",                       /* points to default string     */
		   ini_info,                      /* points to destination buffer */
		   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		   filename                       /* points to ini filename       */
	);

	strncpy(jcb_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_AMEX",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(amex_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_CUP",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(cup_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_TIMEOUT_RC",                   /* points to section name       */
			"VOICE_TIMEOUT_RC_FOR_DCI",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(dci_voice_timeout_rc,ini_info,2);
   return( ret_val );
}


void txservice_Check_and_update_rc_if_Voicetxn_is_TO()
{
	if(strncmp(Auth_Tx.TLF01_details.response_code,"19",2)==0)
	{

		if( true == txservice_Check_If_AMEX_Transaction() )
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
					amex_voice_timeout_rc,
					sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}
		else if(true == txservice_Check_If_CUP_Transaction() )
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
					cup_voice_timeout_rc,
					sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}
		else if(true == txservice_Check_If_VISA_Transaction())
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
								visa_voice_timeout_rc,
								sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}
		else if(true == txservice_Check_If_MC_Transaction())
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
								mc_voice_timeout_rc,
								sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}
		else if(true == txservice_Check_If_JCB_Transaction())
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
								jcb_voice_timeout_rc,
								sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}
		else if(true == txservice_Check_If_Diners_Transaction())
		{
			memcpy (Auth_Tx.TLF01_details.response_code,
								dci_voice_timeout_rc,
								sizeof(Auth_Tx.TLF01_details.response_code)) ;
		}

	}
}

INT txcntl_read_tf_ini_FAI_rc( pCHAR err_buf )
{
   DWORD rc;
   CHAR filename     [256]= {0};
   CHAR tmpstr       [256]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);


	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",  /* points to key name           */
			"19",                       /* points to default string     */
			 ini_info,                      /* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
			 filename                       /* points to ini filename       */
	);

	strncpy(mc_voice_fai_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_VISA",  /* points to key name           */
		   "19",                       /* points to default string     */
			ini_info,                      /* points to destination buffer */
			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
			filename                       /* points to ini filename       */
	);

	strncpy(visa_voice_fai_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_JCB",  /* points to key name           */
		  "19",                       /* points to default string     */
		   ini_info,                      /* points to destination buffer */
		   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		   filename                       /* points to ini filename       */
	);

	strncpy(jcb_voice_timeout_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(amex_voice_fai_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_CUP",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(cup_voice_fai_rc,ini_info,2);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			 "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
			"VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",  /* points to key name           */
		 "19",                       /* points to default string     */
		  ini_info,                      /* points to destination buffer */
		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
		  filename                       /* points to ini filename       */
	);

	strncpy(dci_voice_fai_rc,ini_info,2);
   return( ret_val );
}

void txcntl_read_falconrt_flag_for_transactions()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  reload_falcon_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "FALCON_RT_FLAG",   /* points to section name         */
									  "TXCNTL_FALCONRT_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   reload_falcon_flag,              		/* points to destination buffer   */
									   sizeof(reload_falcon_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   Falcon_RT_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );
}

void txservice_load_SecondaryExpiryDateValidationflag()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  Validation_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "SECONDARY_EXPIRY_DATE_VALIDATION_FLAG",   /* points to section name         */
									  "EXPIRY_DATE_VALIDATION_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   Validation_flag,              		/* points to destination buffer   */
									   sizeof(Validation_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   SecondaryExpiryDateValidationFlag = atoi(Validation_flag);

	   memset( Validation_flag, 0x00, sizeof(Validation_flag) );
}

DWORD txcntl_write_falcon_RT_falg_into_inifile(pCHAR fai_rt_value)
{
	   DWORD rc;
	   CHAR filename     [256]= {0};
	   CHAR tmpstr       [256]= {0};
	   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	   INT  ret_val = true;
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);

	 rc= WritePrivateProfileString(
		  "FALCON_RT_FLAG",	/* pointer to section name*/
		  "TXCNTL_FALCONRT_FLAG",	/* pointer to key name*/
		  fai_rt_value ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );

	 sprintf( tmpstr, "Falcon RT Flag value changed for processing Transactions:%s",fai_rt_value );
	 txcntl_log_message( 3, 1,tmpstr, "PinnacleMsgHandler", 0 );
	 return rc;
}

DWORD txcntl_write_falconRT_EXT10_flag_into_inifile(pCHAR fai_ext_value)
{
	   DWORD rc;
	   CHAR filename     [256]= {0};
	   CHAR tmpstr       [256]= {0};
	   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	   INT  ret_val = true;
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);

	 rc= WritePrivateProfileString(
		  "FALCON_RT_FLAG",	/* pointer to section name*/
		  "TXCNTL_FALCONRT_EXT10_FLAG",	/* pointer to key name*/
		  fai_ext_value ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );

	 sprintf( tmpstr, "FalconRT EXT10 Flag value changed for processing Transactions:%s",fai_ext_value );
	 txcntl_log_message( 3, 1,tmpstr, "PinnacleMsgHandler", 0 );
	 return rc;
}

/******************************************************************************
 *
 *  NAME:         txservice_CREATE_SYSTEM_DATE
 *
 *  DESCRIPTION:  This procedure creates the current system date into
 *                format YYYYMMDD.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       None
 *  OUTPUTS:      system_date  : Format = "YYYY0M0D"
 *  RTRN VALUE:   None
 *
 ******************************************************************************/
void txservice_create_system_date( pBYTE system_date )
{
   CHAR  time_date[25] = "";

   memset( system_date,  0x00, 9 );

   /* Get local time - Format = "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( time_date );

   /* Format it. */
   strncpy( system_date,  time_date,    4 );
   strncat( system_date, &time_date[5], 2 );
   strncat( system_date, &time_date[8], 2 );
   return;
}


INT txcntl_map_amex_action_code_to_iso_repsonse_code(pCHAR amex_response_code )
{
   #define  NUMBER_OF_ACTION_CODES   31
   #define   AMX_INVALID_TXN         "12"

   INT     ret_val = true;
   INT     i;
   INT     found = false;

   CHAR    iso_response_codes[NUMBER_OF_ACTION_CODES][3] =
                             {"00",  "00",  "00",  "05",  "54",  "62",  "75",  "01",
                              "13",  "12",  "51",  "55",  "51",  "05",  "05",  "43",
                              "62",  "00",  "00",  "00",  "30",  "91",  "91",  "94",
                              "40",  "00",  "40",  "25",  "80",  "27",  "63"};

   CHAR    action_codes_amex[NUMBER_OF_ACTION_CODES][4] =
                             {"000", "001", "002", "100", "101", "104", "106", "107",
                              "110", "111", "116", "117", "121", "122", "125", "200",
                              "290", "400", "800", "900", "904", "911", "912", "913",
                              "115", "300", "301", "302", "303", "304", "380"};


   if(amex_response_code[0] != 0x00)
   {
      for (i=0; i<NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(amex_response_code, action_codes_amex[i], 3))
         {
            strncpy(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2);
            found = true;
            break;
         }
      }
   }

   if ( found == false )
   {
      strncpy(Auth_Tx.TLF01_details.response_code,AMX_INVALID_TXN,2);
   }
   /*copy amex action into product12 code*/
   strncpy(Auth_Tx.TLF01_details.product_codes[12].code, amex_response_code, 3);
   return ret_val;
}

void txcntl_match_DCI_to_ISO_response_code( pBYTE dci_response_code)
{
   INT  i;

   CHAR    iso_response_codes[DINERS_NUMBER_OF_ACTION_CODES][2] =
                             {"08",  "00",  "05",  "54",  "43", "05",  "75",
							  "03",  "13" , "12",  "55",  "05", "43",  "43",
							  "43",  "43",  "43",  "43",  "43", "43",  "80",
                              "30",  "31",  "  ",  "  ",  "  ", "  ",  "  ",
							  "88",  "92",  "94",  "95",  "96", "  ",  "  ",
							  "00",  "00",  "00",  "00",  "00", "00",  "00",
							  "05",  "05",  "05",  "05",  "05", "05",  "05",
							  "05",  "05",  "05",  "05",  "05", "05",  "05",
							  "54",  "54",  "54",  "54",  "54", "54",  "54",
							  "00",  "00",  "01"};

   CHAR    auth_advice_dci[DINERS_NUMBER_OF_ACTION_CODES][3] =
                             {"001", "081", "100", "101", "102", "104", "106",
							  "109", "110", "115", "117", "124", "200", "203",
							  "204", "205", "206", "208", "209", "210", "307",
                              "904", "905", "909", "909", "909", "909", "909",
							  "909", "909", "909", "909", "909", "909", "910",
							  "082", "083", "084", "085", "086", "087", "090",
							  "102", "104", "109", "115", "118", "119", "124",
							  "129", "140", "141", "163", "180", "181", "182",
							  "201", "202", "203", "204", "205", "206", "207",
							  "400", "800", "186"};



      for (i=0; i<DINERS_NUMBER_OF_ACTION_CODES; i++)
      {
         if (0 == strncmp(dci_response_code, auth_advice_dci[i], 3))
         {
            strncpy(Auth_Tx.TLF01_details.response_code, iso_response_codes[i], 2);
            break;
         }
      }
      /*copy dci action into product12 code*/
      strncpy(Auth_Tx.TLF01_details.product_codes[12].code, dci_response_code, 3);
}


BOOLEAN txservice_Check_If_Transaction_Is_Incoming_RPS_STIP()
{

	if(true == txservice_Check_If_AMEX_Transaction())
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == '9')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_JCB_Transaction())
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_VISA_Transaction())
	{
		if(strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[8],"02",2) == 0)// check here
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_MC_Transaction())
	{

		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4')// check here)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_CUP_Transaction())
	{
		if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"28",2) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (true == txservice_Check_If_Diners_Transaction())
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == '4' )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}
BOOLEAN txservice_Check_If_Transaction_Is_Incoming_COF()
{

	if(true == txservice_Check_If_AMEX_Transaction())
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[6] == 'A')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_JCB_Transaction())
	{
		if(0==strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"10",2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_VISA_Transaction())
	{
		if(0==strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"10",2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(true == txservice_Check_If_MC_Transaction())
	{

		if(0==strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"10",2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (true == txservice_Check_If_Diners_Transaction())
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[4] == '4' )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

/**************************************************************************
NAME:			txserviceCadience_Perform_Balance_Update_For_RT_Tx
DESCRIPTION:	Determines if the balance account limits have been exceeded
INPUTS:			The transaction amount
OUTPUTS:		None
RETURNS:        false  if limit is exceeded
                true   if limit is not exceeded
AUTHOR:         Abhishek Verma
MODIFIED BY:
**************************************************************************/
BYTE txserviceCadience_Perform_Balance_Update_For_RT_Tx(void)
{
	BYTE ReturnCode = 0;
//	BYTE bReturnCode;
	double nAuthTxTotalAmount = 0.0;
	BYTE MessageSubType2 = 0 ;
	static int RetryCountCCF02P =0;
	static int RetryCountACF01 = 0;
	char strTemp[256] = {0} ;
//	char Buffer[250];
	/*  Need to Check 3rd Currency code coming from Host- TF Phani*/
	if(	!strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) &&
		!strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY)
       )
    {
    	if(true == txservice_Check_If_CUP_Transaction())
		{
    		/*
			  	  ATP must authorize in PESO or USD.
			  	  Let us check for the 3rd Currency.
			 	  If field 49 is neither PESO and USD
			 	  use field 6 for the amount field, and 51 as currency
			 	  if Field 51 is not dollor or PESO then
			 	  use field 5 for the amount field, and 51 as currency
			 	  if Field 50 is not dollor or PESO then drop it.
		 	 	  This is for switched in, or Issuing, transactions only.
		 	 	 Its only for CUP
    		 */
			// Check Field 51 is PESO or DOLLOR. Its Card Holder billing Currency rate.
			if(strEqual (Auth_Tx.ch_billing_curr_code, PESO_CURRENCY) ||
				strEqual (Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY))
	   		 {
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 51 is PESO
				if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 51 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}

			// Check Field 50 is PESO or DOLLOR . Its Settlement Currency Code
			else if(strEqual (Auth_Tx.settlement_curr_code, PESO_CURRENCY) ||
				strEqual (Auth_Tx.settlement_curr_code, USDOLLAR_CURRENCY))
	   		 {
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 50 is PESO
				if ( 0 == strncmp( Auth_Tx.settlement_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.TLF01_details.settlement_total );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 50 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.settlement_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.TLF01_details.settlement_total );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}
			else if(0 == strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN))
			{
				if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL)
				{
					//Let us check if its AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE
					//strncpy(Auth_Tx.TLF01_details.product_codes[0].quantity,"20",2);
					if(0 == strncmp(Auth_Tx.TLF01_details.product_codes[0].quantity,"20",2))
					{
						// We need to see if we have original transaction is present in DB or not.
						if(0 == strlen(Auth_Tx.TLF01_details.reversal_amount))
						{
							strcpy (strTemp, "Currency Not supported for Tx") ;
							memcpy( Auth_Tx.TLF01_details.response_text,  strTemp,
									sizeof(Auth_Tx.TLF01_details.response_text)-1 );
							strncpy(Auth_Tx.TLF01_details.response_code,CUP_DO_NOT_HONOR,
									CUP_RESPONSEP_CODE_LEN);

							ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp,
																						"txserviceCadience_Perform_Balance_Update_For_RT_Tx",
																						GS_TXN_INVALID_CURRENCY_CODE,
																						DO_NOT_HONOR) ;

							return ReturnCode ;
						}
						else
						{
							strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
						}
					}
					else if(strlen(Auth_Tx.TLF01_details.reversal_amount) > 0)
					{
						strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
					}
					else
					{
						strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
						strncpy(Auth_Tx.TLF01_details.response_code,CUP_APPROVED,
								CUP_RESPONSEP_CODE_LEN);
						return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
					}

				}
				else if((true == txservice_Check_If_CUP_ECOM ())&&
						 Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
				{
					// Its CUP ECOM Balance inquiry transaction transaction
					ReturnCode = TxDataServer_Get_CCF02_Details ();
					if (ReturnCode == NO_AUTH_ERROR)
					{
						return NO_AUTH_ERROR;
					}
				}
				else
				{
					strcpy (strTemp, "Currency Not supported for Tx") ;
					memcpy( Auth_Tx.TLF01_details.response_text,
						   strTemp,
						   sizeof(Auth_Tx.TLF01_details.response_text)-1 );
					strncpy(Auth_Tx.TLF01_details.response_code,"05",2);

					ReturnCode = txserviceCadience_Generate_Usage_Error_Message (1,strTemp,
																				"txserviceCadience_Perform_Balance_Update_For_RT_Tx",
																				GS_TXN_INVALID_CURRENCY_CODE,
																				DO_NOT_HONOR) ;

					return ReturnCode ;
				}

			}
			else
			{
				return GS_TXN_COMPLETED;
			}
		}
    	else if(true == txservice_Check_If_Diners_Transaction())
		{
    		/*
			  	  ATP must authorize in PESO or USD.
			  	  Let us check for the 3rd Currency.
			 	  If field 49 is neither PESO and USD
			 	  use field 6 for the amount field, and 51 as currency
			 	  This is for switched in, or Issuing, transactions only.
		 	 	 Its only for CUP
    		 */
			// Check Field 51 is PESO or DOLLOR. Its Card Holder billing Currency rate.
			if(strEqual (Auth_Tx.ch_billing_curr_code, PESO_CURRENCY) ||
				strEqual (Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY))
	   		 {
				/* Save original currency code and amount. */
	      		strcpy(Auth_Tx.reconciliation_cur_code,Auth_Tx.TLF01_details.currency_code );
	      		strcpy(Auth_Tx.TLF01_details.orig_amount,Auth_Tx.TLF01_details.total_amount );

				//Check if field 51 is PESO
				if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, PESO_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
	      		}
				//Check if field 51 is DOLLAR
				else if ( 0 == strncmp( Auth_Tx.ch_billing_curr_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))
	      		{
	         		strcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.ch_billing_amt );
			        strcpy( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY );
	      		}
			}
			else
			{
				return GS_TXN_COMPLETED;
			}
		}
		else
		{
			return GS_TXN_COMPLETED;
		}
	}

	/*We have CCF02P & ACF01 records with US but transaction is not validated based
	  on card details and balance details. We need to process the transaction and
	  Authorized based on card capability and Balance
	  Lets get CCF02 details again */

	  ReturnCode = TxDataServer_Get_CCF02_Details ();
	  if (ReturnCode == NO_AUTH_ERROR)
	  {
		  return NO_AUTH_ERROR;
	  }

	return NO_AUTH_ERROR;
} /*txserviceCadience_Perform_Balance_Update_For_RT_Tx*/

#ifdef MC12_OCT2023
BYTE txservice_Merchant_Integrity_Param_Check_for_MC()
{
	BYTE ReturnCode   = NO_AUTH_ERROR;
	CHAR strTemp[256] = {0};
	CHAR last_response_code[3] ={0};

	if(	(MERCH_INT_PARAM.c1_block[0] == 'Y' && cat1_excessive_declines_validation_flag == 'Y') ||
		(MERCH_INT_PARAM.c2_block[0] == 'Y' && cat2_excessive_declines_validation_flag == 'Y') )
	{
		if(MERCH_INT_PARAM.c1_block[0] == 'Y')
		{
			strcpy(Auth_Tx.TLF01_details.response_text, "Decline: 24-hour maximum decline reached");
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.response_text, "Decline : 30-day maximum decline reached");
		}

		/*
		 * add the logic to validate the last declined RC, numeric only, should not be spaces
		 * */
		if(isalnum(MERCH_INT_PARAM.last_declined_rc[0])  &&
		   isalnum(MERCH_INT_PARAM.last_declined_rc[1])  &&
		   2 == strlen(MERCH_INT_PARAM.last_declined_rc))
		{
			strncpy(last_response_code,MERCH_INT_PARAM.last_declined_rc,2);
		}
		else
		{
			strncpy(last_response_code,CATEGORY_CODE_REATTEMPT_REACHED,2);
		}

		sprintf(strTemp, "%s, Decline the TXN with RC:%s", Auth_Tx.TLF01_details.response_text,last_response_code);
		//printf("%s\n", strTemp);
        txcntl_log_message( 3, 3, strTemp, "txservice_Merchant_Integrity_Param_Check_for_MC", 1 );
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,last_response_code) ;

		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}
	else
	{
		ReturnCode = txservice_Process_When_Bin_Range_Found () ;
	}

	return ReturnCode;
}
#endif
#ifdef VISA7
BYTE txservice_Merchant_Integrity_Param_Check()
{
	BYTE ReturnCode   = NO_AUTH_ERROR;
	CHAR strTemp[256] = {0};
	CHAR last_response_code[3] ={0};

	if(	(MERCH_INT_PARAM.c1_block[0] == 'Y' && cat1_MID_integrity_validation_flag == 'Y') ||
		(MERCH_INT_PARAM.c2_block[0] == 'Y' && cat2_MID_integrity_validation_flag == 'Y') ||
		(MERCH_INT_PARAM.c3_block[0] == 'Y' && cat3_MID_integrity_validation_flag == 'Y') ||
		(MERCH_INT_PARAM.c4_block[0] == 'Y' && cat4_MID_integrity_validation_flag == 'Y'))
	{
		if(MERCH_INT_PARAM.c1_block[0] == 'Y')
			strcpy(Auth_Tx.TLF01_details.response_text, "Decline : category 1 re-attempt reached");
		else if((MERCH_INT_PARAM.c2_block[0] == 'Y') || (MERCH_INT_PARAM.c3_block[0] == 'Y'))
			strcpy(Auth_Tx.TLF01_details.response_text, "Decline:category 2,3 re-attempt reached");
		else
			strcpy(Auth_Tx.TLF01_details.response_text, "Decline : category 4 re-attempt reached");

		/*
		 * add the logic to validate the last declined RC, numeric only, should not be spaces
		 * */
		if(isalnum(MERCH_INT_PARAM.last_declined_rc[0])  &&
		   isalnum(MERCH_INT_PARAM.last_declined_rc[1])  &&
		   2 == strlen(MERCH_INT_PARAM.last_declined_rc))
		{
			strncpy(last_response_code,MERCH_INT_PARAM.last_declined_rc,2);
		}
		else
		{
			strncpy(last_response_code,CATEGORY_CODE_REATTEMPT_REACHED,2);
		}

		sprintf(strTemp, "%s, Decline the TXN with RC:%s", Auth_Tx.TLF01_details.response_text,last_response_code);
		//printf("%s\n", strTemp);
        txcntl_log_message( 3, 3, strTemp, "txservice_Merchant_Integrity_Param_Check", 1 );
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,last_response_code) ;

		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}
	else
	{
		ReturnCode = txservice_Process_When_Bin_Range_Found () ;
	}

	return ReturnCode;
}
#endif

#ifdef MC13_OCT2023
BYTE txservice_Merchant_Integrity_Param_Check_for_MAC()
{
	BYTE ReturnCode   = NO_AUTH_ERROR;
	CHAR strTemp[256] = {0};
	CHAR last_response_code[3] ={0};

	if(	(MERCH_INT_PARAM.c1_block[0] == 'Y' || MERCH_INT_PARAM.c2_block[0] == 'Y')
		//&& (0 == strncmp(MERCH_INT_PARAM.reattempts_date, Auth_Tx.TLF01_details.total_amount, 12)) -- check with BDO
	  )
	{
		strcpy(Auth_Tx.TLF01_details.response_text, "Alw attempt for CNP MAC already reached");
		memset(Auth_Tx.TLF01_details.product_codes[0].amount, 0, sizeof(Auth_Tx.TLF01_details.product_codes[0].amount));
		sprintf(Auth_Tx.TLF01_details.product_codes[0].amount, "DE48 SE84:%s", MERCH_INT_PARAM.first_reattempt_date);

		/*
		 * add the logic to validate the last declined RC, numeric only, should not be spaces
		 * */
		if(isalnum(MERCH_INT_PARAM.last_declined_rc[0])  &&
		   isalnum(MERCH_INT_PARAM.last_declined_rc[1])  &&
		   2 == strlen(MERCH_INT_PARAM.last_declined_rc))
		{
			strncpy(last_response_code, MERCH_INT_PARAM.last_declined_rc, 2);
		}
		else
		{
			strncpy(last_response_code, CATEGORY_CODE_REATTEMPT_REACHED, 2);
		}

		sprintf(strTemp, "%s, Decline the TXN with RC:%s", Auth_Tx.TLF01_details.response_text, last_response_code);
		//printf("%s\n", strTemp);
        txcntl_log_message( 3, 3, strTemp, "txservice_Merchant_Integrity_Param_Check_for_MAC", 1 );
		txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED, last_response_code) ;

		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}
	else
	{
		ReturnCode = txservice_Process_When_Bin_Range_Found () ;
	}

	return ReturnCode;
}
#endif

void txcntl_read_falconrt_ext10_flag_for_transactions()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  reload_falcon_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "FALCON_RT_FLAG",   /* points to section name         */
									  "TXCNTL_FALCONRT_EXT10_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   reload_falcon_flag,              		/* points to destination buffer   */
									   sizeof(reload_falcon_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   Falcon_RT_Ext10_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );
	   if (Falcon_RT_Ext10_Flag == 1)
	   {
		   txcntl_read_vertexon_codes_for_falcon_ext10();
	   }
}

void txcntl_read_vertexon_codes_for_falcon_ext10()
{
	#define MAX_INI_INFO_SIZE 256

    int ind, index, codeIndex;
	DWORD rc;
	CHAR codesCount[3] = {0};
    CHAR codesBuffer[256] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    memset(visa_vertexon_codes,  0x00, sizeof(visa_vertexon_codes));
    memset(mc_vertexon_codes,    0x00, sizeof(mc_vertexon_codes));
    memset(jcb_vertexon_codes,   0x00, sizeof(jcb_vertexon_codes));
    memset(amex_vertexon_codes,  0x00, sizeof(amex_vertexon_codes));
    memset(dci_vertexon_codes,   0x00, sizeof(dci_vertexon_codes));
    memset(upi_vertexon_codes,   0x00, sizeof(upi_vertexon_codes));
    memset(mp_in_vertexon_codes, 0x00, sizeof(mp_in_vertexon_codes));
    memset(pl_ft_vertexon_codes, 0x00, sizeof(pl_ft_vertexon_codes));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    //VISA codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "VISA_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "VISA_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				visa_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(visa_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "VISA_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //MC codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "MC_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "MC_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mc_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mc_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "MC_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //JCB codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "JCB_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "JCB_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				jcb_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(jcb_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "JCB_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //AMEX codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "AMEX_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "AMEX_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

    ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				amex_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(amex_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "AMEX_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //DCI codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "DINERS_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "DCI_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				dci_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(dci_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "DCI_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

    //UPI codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "UPI_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "UPI_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				upi_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(upi_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "UPI_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }

#if 0
    //MP/Installment codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "MP_IN_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "MP_IN_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mp_in_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mp_in_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "MP_IN_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }
#endif
    //Pvt. Label/Fleet codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "PL_FT_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
	    txcntl_log_message(2, 2, "PL_TF_CODES not read", "txcntl_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				pl_ft_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(pl_ft_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	txcntl_log_message(2, 2, "PL_TF_CODES contains invalid data", "txcntl_read_vertexon_codes", 0);
    }
}


BOOLEAN txservice_check_auth_rc_for_falcon_ext10()
{
	int i=0, codesCount = 0;
	BOOLEAN bReturnCode = false ;

	char tempStatus[51] = {0};
	char buffer[100] = {0};


		//validate the response/action codes from Vertexon
		if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncmcr2") == 0)
		{
			sprintf(buffer, "ncmcr2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(mc_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(mc_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncvis2") == 0)
		{
			sprintf(buffer, "ncvis2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(visa_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(visa_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		else if((strcmp(Auth_Tx.TLF01_details.acquirer_id, "nposa2") == 0) ||
				(strcmp(Auth_Tx.TLF01_details.acquirer_id, "natma2") == 0)) //check with BDO
		{
			sprintf(buffer, "nposa2/natma2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(amex_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(amex_vertexon_codes[i], Auth_Tx.TLF01_details.product_codes[12].code, 3) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncjcb2") == 0)
		{
			sprintf(buffer, "ncjcb2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(jcb_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(jcb_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "ncdci2") == 0)
		{
			sprintf(buffer, "ncdci2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(dci_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(dci_vertexon_codes[i], Auth_Tx.TLF01_details.product_codes[12].code, 3) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "nccup2") == 0)
		{
			sprintf(buffer, "nccup2 checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			codesCount = atoi(upi_vertexon_codes[0]);
			for(i=1; i<=codesCount; i++)
			{
				if(strncmp(upi_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
				{
					bReturnCode = true;
					break;
				}
			}
		}
		//also handled in dcpimp module
		else if(strcmp(Auth_Tx.TLF01_details.acquirer_id, "nciftr") == 0)
		{
			sprintf(buffer, "nciftr checking for configured RC with response RC = [%s]", Auth_Tx.TLF01_details.response_code);
			txcntl_log_message(2, 1, buffer, "txservice_check_auth_rc_for_falcon_ext10", 0);

			if((strncmp(Auth_Tx.TLF01_details.card_num, "7", 1) != 0) ||
			   (strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity, "1", 1) != 1))
			{
				codesCount = atoi(pl_ft_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(pl_ft_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						bReturnCode = true;
						break;
					}
				}
			}
			else
			{
				codesCount = atoi(mp_in_vertexon_codes[0]);
				for(i=1; i<=codesCount; i++)
				{
					if(strncmp(mp_in_vertexon_codes[i], Auth_Tx.TLF01_details.response_code, 2) == 0)
					{
						bReturnCode = true;
						break;
					}
				}
			}
		}



	return bReturnCode ;
}

void txcntl_read_flag_for_cup3ds2_transactions_in_stip()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  stip_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "UPI_3DS2_MODE",   /* points to section name         */
									  "ATP_STIP_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   stip_flag,              		/* points to destination buffer   */
									   sizeof(stip_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
		if(rc != 1)
		{
			txcntl_log_message(2, 2, "UPI_3DS2_MODE not read", "txcntl_read_cup3ds2_flag_for_transactions_in_stip", 0);
		}
		Cup3ds2_Flag = atoi(stip_flag);

	   memset( stip_flag, 0x00, sizeof(stip_flag) );
}

BOOLEAN txservice_check_for_AMEX_partial_reversal_transaction()
{
	if( (0 == strncmp(Auth_Tx.TLF01_details.message_type,"1420",4) ||
		 0 == strncmp(Auth_Tx.TLF01_details.message_type,"1430",4)) &&
		 0 == strncmp(Auth_Tx.function_code,"401",3) )
	{
		return true;
	}
	return false;
}

INT txservice_check_if_trx_is_bdo_pos_acq_trx(  )
{
	 if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,
			 	 	  BDO_POS_ACQ_TXN_HANDLER_QUEUE,
					  BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


/*MobilePOS_NFC_Tap2Phone Function*/
INT txservice_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
{
	 if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					 POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
{
	if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			 	 	POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_SALE_COMPLETION_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
{
	if((true == txservice_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin()) &&
	   (true == txservice_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == txservice_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_is_acq_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
{
	if((true == txservice_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin()) &&
	   (true == txservice_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == txservice_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/***********************************************************/


/*Swiftpass BDOPay Credit Trx Function*/

INT txservice_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(  )
{
	 if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( )
{
	if(0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}



INT txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_with_pin( )
{
	if((true == txservice_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin()) &&
	   (true == txservice_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == txservice_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_is_acq_Swiftpass_BDO_Pay_credit_trx_without_pin( )
{
	if((true == txservice_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin()) &&
	   (true == txservice_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == txservice_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/***********************************************************/


/*E-commerce Entry Mode from Swiftpass-BDO Pay Transactions*/

INT txservice_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(Auth_Tx.TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx(  )
{
	if((0 == strncmp(Auth_Tx.TLF01_details.pos_entry_mode,
					 POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM,
					 POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code,
			   	   	 POS_ECOM_CONDITION_CODE,
					 POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT txservice_check_if_trx_is_acq_ecommerce_swiftpass_BDOPay_trx(  )
{
	if((true == txservice_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx()) &&
	   (true == txservice_check_if_trx_is_bdo_pos_acq_trx())&&
	   (true == txservice_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx()))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


/*************************************************************************************
DESCRIPTION:	Sends the message to the remote destination (host handler)
INPUTS:			CHAR* que_name:		the destination que name
				BYTE message_type:	the pte message type
				BYTE message_subtype_1:	the pte message subtype
				BYTE AppDataType :		the pte message appdatatype
OUTPUTS:		None
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:
*************************************************************************************/
BOOLEAN txservice_Forward_Auth_Tx_To_Remote_Host_keyblock (CHAR* que_name,
                                                  BYTE  message_type,
                                                  BYTE  message_subtype_1,
                                                  BYTE  AppDataType)
{
	pPTE_MSG_DATA 	p_msg_data;
	pPTE_MSG	    p_msg_out;
	pPTE_MSG	    p_msg_in;
	pBYTE         	p_data;

	BOOLEAN 		bReturnCode = true ;
	INT     		len		   = 0;
	INT     		retcode;
	INT 			Auth_Tx_size = 0;
	BYTE          	rcode;
	AUTH_TX       	auth_tx;
	CHAR          	error_msg[100]      = {0};
	CAF01_KEYBLOCK caf01_keyblock;
	BYTE          	buffer[sizeof(AUTH_TX) + sizeof(caf01_keyblock)];
	BYTE          	buffer_keyblock[sizeof(caf01_keyblock)];

	/* Initialize the global variables. */
	Auth_Tx_size = sizeof(AUTH_TX);
	memset( buffer,   0x00, sizeof(buffer ) );
	memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );

	strncpy(caf01_keyblock.primary_key.Caf_Profile_id,Auth_Tx.CAF01_details.primary_key.caf_profile_id, 4 );
	strncpy(caf01_keyblock.primary_key.Type,Auth_Tx.CAF01_details.primary_key.type, 1 );

	memcpy( buffer, &Auth_Tx, Auth_Tx_size );
	memcpy( buffer + Auth_Tx_size, &caf01_keyblock, sizeof(caf01_keyblock) );

	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( 	MT_DB_REQUEST,
									ST1_DB_SELECT,
									"",
									NULL_PTR,
									interactive_que_name,
									(pBYTE) buffer,
									sizeof( buffer),
									CAF01_KEYBLOCK_DATA );
	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message." );
		bReturnCode = false;
	}
	else
	{
		rcode = TxDataServer_Send_Interactive_DB_Request(p_msg_out,
														 &p_msg_in,
														 tran_dataserver_que_name,
														 error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &caf01_keyblock, p_data+sizeof(AUTH_TX), sizeof(caf01_keyblock) );
			free( p_msg_in );
		}
	}

	/* Implement Visa SMS routing; if anything is going to Visa,
	* send it to Visa SMS if a pin exists.  SMS is the ATM host
	* for Visa.
	*/
	if ( Auth_Tx.TLF01_details.pin_block[0] != 0x00 )
	{
		/* A pin does exist.  Is the destination ncvisa? */
		len = strlen( que_name );

		/* Ignore the 'A' at the end of 'que_name' */
		if ( 0 == strncmp(que_name, plus_host_que_name, len-1))
		{
			/* Yes, change it to Visa SMS. */
			strcpy( que_name, vsms_host_que_name );
			strcat( que_name, "A" );
		}
	}

    if ( ((strlen(caf01_keyblock.Cvk_Mag_Keyblock)> 0) ||
		  (strlen(caf01_keyblock.Cvk_Acq_3DS_Keyblock)> 0) ||
		  (strlen(caf01_keyblock.Cvk_Pin_Verification)> 0) ||
		  (strlen(caf01_keyblock.Cvk_Mkac_Keyblock)> 0 )||
		  (strlen(caf01_keyblock.Csck_Iss_3ds_Keyblock)> 0) ||
		  (strlen(caf01_keyblock.Csck_Mag_Keyblock)> 0) ||
		  (strlen(caf01_keyblock.Dci_3ds_Keyblock)> 0) ||
		  (strlen(caf01_keyblock.Cvk_3ds_Cak_Keyblock)> 0)) &&
		 (false == txservice_check_if_txn_is_IST_transaction()))
    {
		memcpy( buffer_keyblock , &caf01_keyblock, sizeof(caf01_keyblock) );

    	bReturnCode = txservice_Send_Request_To_Service (que_name, message_type,
        												 message_subtype_1,
    													 0, AppDataType,
    													 (pBYTE)buffer_keyblock ,
    													 sizeof( buffer_keyblock)) ;
    }
    else
    {
    	sprintf (error_msg,"Could not able to get the Keyblock profil %s non keyblock validation will be done",
    			caf01_keyblock.primary_key.Caf_Profile_id);
    	txcntl_log_message( 2, 3, error_msg, "txservice_Forward_Auth_Tx_To_Remote_Host_keyblock", 1 );

    	if (message_type == MT_NSP_VERIFYPIN_KEYBLOCK)
    	{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    		    			  	  	  	  	  	  	  	  	  	  	  	  	 MT_NSP_VERIFYPIN,
    																		 0, 0) ;
    	}
    	else if (message_type == MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK)
    	{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY_MAGSTRIPE,
																			 0, 0) ;
    	}
    	else if (message_type == MT_NSP_VERIFY_ARQC_KEYBLOCK)
		{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY_ARQC,
    																		 0, 0) ;
		}
    	else if (message_type == MT_NSP_VERIFY_3D_SECURE_KEYBLOCK)
		{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY_3D_SECURE,
    																		 0, 0) ;
		}
    	else if (message_type == MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK)
		{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY_CUP_ECOM_CVN2,
    																		 0, 0) ;
		}
    	else if (message_type == MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK)
		{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY_3D_SECURE_2,
    																		 0, 0) ;
		}
    	else if (message_type == MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK)
		{
    		bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
    																		 MT_NSP_VERIFY3DS2_MAGSTRIPE,
    																		 0, 0) ;
		}

    }
	return (bReturnCode) ;
} /* txservice_Forward_Auth_Tx_To_Remote_Host_keyblock */
