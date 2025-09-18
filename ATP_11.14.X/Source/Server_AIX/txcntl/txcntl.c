/**************************************************************************************************
  
MODULE:      TXCNTL.C
Copyright (c) 2007, Hypercom, Inc.
All Rights Reserved.
 
TITLE:       Equitable - Transaction Controller Main.
 
DESCRIPTION: Main startup and end process routines. 

APPLICATION: Equitable

   
*******************************************************************************/

#include <stdio.h>
#include "pteipc.h"

#include "txtrace.h"

#include "ptesystm.h"
#include "ntutils.h"
#include "txcntl.h"
#include "txcntl_constants.h"
#include "memmnger.h"
#include "txutils.h"
#include "txdataserver.h"
#include "myutils.h"
#include "txauthorize.h"

//new changes - amith
#define VISA7 1
/*****************************************************************************
							CONSTANTS / MACROS
*****************************************************************************/

/*****************************************************************************
							DATA TYPES
*****************************************************************************/

/****************************************************************************
							PROTOTYPES
*****************************************************************************/
static INT get_fraudguard_state_from_ini( void );
static INT get_automatic_resend_state_from_ini( void );
void get_FRT_Decline_RC();
void Txcntl_Get_MC_delcine_rc();
/****************************************************************************
							DATA DECLARATIONS
****************************************************************************/  
extern CHAR ServiceName[12];		/* defined in startup.c */
extern INT  EndProcessSignalled;	/* defined in startup.c */
extern INT  MainProcessDone;		/* defined in startup.c */
extern INT isalphanum(pCHAR string);//TF PHANI

extern char mc_voice_timeout_rc[3];
extern char visa_voice_timeout_rc[3];
extern char amex_voice_timeout_rc[3];
extern char jcb_voice_timeout_rc [3];
extern char cup_voice_timeout_rc [3];
extern char dci_voice_timeout_rc [3];

extern char mc_voice_fai_rc[3];
extern char visa_voice_fai_rc[3];
extern char amex_voice_fai_rc[3];
extern char jcb_voice_fai_rc [3];
extern char cup_voice_fai_rc [3];
extern char dci_voice_fai_rc [3];

// MCC check
//#define EACH_MCC_CODE 250
//#define MAX_MCC_BUFFER 750
MCC_CHECK mcc_check_list[MCC_MAX_CODES];
CHAR MCC_CODES0[EACH_MCC_CODE];
CHAR MCC_CODES1[EACH_MCC_CODE];
CHAR MCC_CODES2[EACH_MCC_CODE];
CHAR MCC_CODES[MAX_MCC_BUFFER];

INT No_of_MCC_codes=0;

MCC_VALUE rpsmcc_list[50];

AUTH_TX     Auth_Tx;
MCC_LIMITS  Mcc_Limits;
PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
COUNTRY_AMT_LIMITS  Country_Amt_Limits ;

NCF01  ncf01_recs[MAX_NCF01_RECS];

OBS01_GUI_LIST		 obs01_list;
OBS01 				 obs01_data;
VISA3DS2_GUI_LIST	 visa3ds2_list;
KEY3DS01 key3ds01;
JCB3DS2_GUI_LIST	 jcb3ds2_list;
AMEX3DS2 amex_3ds2;
DCI3DS2 diners_3ds2;
JCB3DS2 jcb_3ds2;
CUP3DS2 cup_3ds2;

//new table VS-7
#ifdef VISA7
MERCH_INT_PARAMS MERCH_INT_PARAM;
#endif

/* Defined only because it is referenced in genutils.c (atp.lib).
 * Not used in txcntl.
 */
CHAR oprmon_que_name[MAX_QUE_NAME_SIZE+1] = {0} ;  

CHAR tran_dataserver_que_name   [MAX_QUE_NAME_SIZE+1] = "trandsA" ;
CHAR updateds_que_name			[MAX_QUE_NAME_SIZE+1] = "updatdsA";
CHAR device_dataserver_que_name [MAX_QUE_NAME_SIZE+1] = "devdsA" ;
CHAR network_dataserver_que_name[MAX_QUE_NAME_SIZE+1] = "netdsA" ;
CHAR txcntl_que_name            [MAX_QUE_NAME_SIZE+1] = {0} ;
CHAR crypto_que_name            [MAX_QUE_NAME_SIZE+1] = "racalA" ;
CHAR applnk_que_name            [MAX_QUE_NAME_SIZE+1] = "applnkA" ;	
CHAR fguard_que_name            [MAX_QUE_NAME_SIZE+1] = "fguardA" ;
/*CHAR falcon_que_name            [MAX_QUE_NAME_SIZE+1] = "falconA" ;*/
CHAR faisnd_que_name            [MAX_QUE_NAME_SIZE+1] = "faisndA" ;
CHAR nceqit_que_name            [MAX_QUE_NAME_SIZE+1] = "nceqit";
CHAR neqit2_que_name            [MAX_QUE_NAME_SIZE+1] = "neqit2";
CHAR fconds_que_name			[MAX_QUE_NAME_SIZE+1] = "fcondsA";
#ifdef FALCON_CONTROLLER
CHAR falcntl_que_name           [MAX_QUE_NAME_SIZE+1] = "falcntlA" ;
#endif

/* Do not append "A" to these queue names. */
CHAR cirrus_host_que_name[MAX_QUE_NAME_SIZE+1] = "nccirs" ;	
CHAR plus_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncvisa" ;
CHAR vsms_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncvsms" ;
CHAR amex_atm_que_name   [MAX_QUE_NAME_SIZE+1] = "ncatma" ; /* Acquiring */
CHAR diners_atm_que_name   [MAX_QUE_NAME_SIZE+1] = "ncpul" ; /* Diner Pulse  */
CHAR amex_pos_que_name   [MAX_QUE_NAME_SIZE+1] = "ncposa" ; /* Acquiring */
CHAR diners_host_que_name[MAX_QUE_NAME_SIZE+1] = "ncdci" ; /* Diners Acquiring */
CHAR jcb_host_que_name   [MAX_QUE_NAME_SIZE+1] = "ncjcb"  ; 
CHAR cadencie_host_que_name   [MAX_QUE_NAME_SIZE+1] = "ncvis2"  ; 
CHAR cadencie_pos_que_name   [MAX_QUE_NAME_SIZE+1] = "nposa2" ;  // ThoughtFocus Girija Y
CHAR cadencie_atm_que_name   [MAX_QUE_NAME_SIZE+1] = "natma2" ;  //  ThoughtFocus Girija Y	
CHAR cadencie_mc_que_name   [MAX_QUE_NAME_SIZE+1] = "ncmcr2" ;  //  ThoughtFocus Girija Y
CHAR  voice_handler_que_name[]          = "VOICE";
CHAR  ncdci_issuing_link_name[]  		= "ncdci";
CHAR  ncvisa_issuing_link_name[]		= "ncvis3";
CHAR  ncmcrd_issuing_link_name[]		= "ncmcr";

CHAR  txcntl_Erro_warning_Filename[256] = {0};
CHAR  txcntl_module_error_warning_file_name[256] = {0};
CHAR  txcntl_error_warning_file_path[256] = {0};
CHAR  JCB_ACQ_Contactless_Magstrip_RC[3]   = {0};

CHAR FRT_Decline_RC_Acq_MC[3] ={0};
CHAR FRT_Decline_RC_Acq_VISA[3] ={0};
CHAR FRT_Decline_RC_Acq_AMEX[3] ={0};
CHAR FRT_Decline_RC_Acq_JCB[3] ={0};
CHAR FRT_Decline_RC_Acq_UPI[3] ={0};
CHAR FRT_Decline_RC_Acq_DCI[3] ={0};

CHAR FRT_Decline_RC_ISS_MC[3] ={0};
CHAR FRT_Decline_RC_ISS_VISA[3] ={0};
CHAR FRT_Decline_RC_ISS_AMEX[4] ={0};
CHAR FRT_Decline_RC_ISS_JCB[3] ={0};
CHAR FRT_Decline_RC_ISS_UPI[3] ={0};
CHAR FRT_Decline_RC_ISS_DCI[4] ={0};

CHAR FRT_Decline_RC_VOICE_MC[3] ={0};
CHAR FRT_Decline_RC_VOICE_VISA[3] ={0};
CHAR FRT_Decline_RC_VOICE_AMEX[4] ={0};
CHAR FRT_Decline_RC_VOICE_JCB[3] ={0};
CHAR FRT_Decline_RC_VOICE_UPI[3] ={0};
CHAR FRT_Decline_RC_VOICE_DCI[4] ={0};

CHAR FRT_Decline_RC_PLFC[4] ={0};

CHAR mc_3ds2_delcine_rc[3]={0};
CHAR mc_default_rc_for_digital_payment_data[3]={0};
/* Issuer ID for EB Host
 *
 * This is used to determine if a txn originated from Equitable's Host.
 */
CHAR EB_Host_Issuer_ID[MAX_QUE_NAME_SIZE+1] = "nceq";  /*First 4 char to allow for future instances of nceqit*/
CHAR Version[] = "ATP_11.14.0";


INT nFraudGuardState = 0;
INT nAutomaticResendState = 1;  /* ThoughtFocus - Girija Y */
INT nFalconSendFlag  = 0; 
CHAR key[100]={0};
INT nMra_enable_flag   = 0;
INT Nmra_send_flag = 0;
INT Mracon_Log_Enable_Flag = 0;
INT JCB_ACQ_Contactless_Magstrip_Flag = 0;


/*FAlcon send flag, if falg =0 Falcon is not functinal do not send tx to FALCON
					if falg =1 Falcon is functinal ATP can send tx to FALCON
 */

/* Indicator for Authorization Number for ON-US txns.
 * When it changes, UAG01 in the database is updated.
 */
INT  AuthNumberChanged;

char cavv_verification_rc[3]={0};
char Authentication_attempt_result_code[3]={0};
char cav2_validation_flag;
char authentication_attempt_verify_flag;

void Get_JCB3DS2_CAVV_Parameters_FromTF_INI();
/*
 * Global Flag variable for the RPS project
 * */
INT amex_rps_flag = 0;
INT cup_rps_flag  = 0;
INT dci_rps_flag  = 0;
INT jcb_rps_flag  = 0;
INT mc_rps_flag   = 0;
INT visa_rps_flag = 0;

extern INT Falcon_RT_Flag;
extern INT SecondaryExpiryDateValidationFlag;
extern INT Falcon_RT_Ext10_Flag;

char partial_match_rc[3]={0};
char no_match_rc[3]={0};
char name_validation_flag;

INT keyblock_enable_flag_for_MC = 0;
INT keyblock_enable_flag_for_VISA = 0;
INT keyblock_enable_flag_for_AMEX = 0;
INT keyblock_enable_flag_for_JCB = 0;
INT keyblock_enable_flag_for_UPI = 0;
INT keyblock_enable_flag_for_DCI = 0;


/*************************************************************************************
NAME:			EndProcess
DESCRIPTION:	This function gets called by the Stop() function in the library
				Stop() gets triggered when the service gets terminated. This function
				can be used to do some cleanup before the service gets terminated
INPUTS:			None
OUTPUTS:		None
RETURNS:        void
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void EndProcess(void)
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down %s Service, version %s",
            ServiceName, Version );
   txcntl_log_message( 2, 1, Buffer, "EndProcess",0);
   strcat( Buffer, "\n" );
   PRINT( Buffer );

#ifdef _DEBUG
    pteipc_trace_off(); 
#endif
} /* EndProcess */

/*************************************************************************************
NAME:			PinnacleMsgHandler
DESCRIPTION:	This function gets called when the primary thread (MainProcessor) of the
				application gets a message in its queue. When the thread receives a message,
				this function dispatches it to a function depending on the message type
INPUTS:			p_msg_in: Pointer to a PTE_MSG
OUTPUTS:		None
RETURNS:        Void
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void PinnacleMsgHandler(pPTE_MSG p_msg_in)

{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pBYTE			 p_data;
   CHAR    destination_que[7];
   pPTE_MSG p_cur_msg = 0 ;
   pPTE_MSG p_msg_out = 0 ;
   BYTE     MessageType = 0 ;
   BYTE     AppDataType = 0 ; // TF Phani
   pPTE_MSG_DATA pATEMessageData = 0;
   pBYTE	pATEMessageDataData = 0;
   BOOLEAN  bReturnCode = true ;
   BYTE     ReturnCode = 0 ;
   CHAR     strTemp[256] = {0} ;
   INT      first_card_type;
//   AUTH_TX  Auth_Tx;
   INT      len=0;int dest_len = 0;
/* Automatic Resend - ThoughtFocus Girija Y */
   CHAR		cmd[100];
   CHAR	bindir[256];  


   memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
    
   p_cur_msg = p_msg_in ;

	

   MessageType = ptemsg_get_msg_type (p_cur_msg) ;
   switch (MessageType)
   {
		case MT_AUTH_REQUEST: 
			
			/* request from either locally acquired (terminal handler) 
			 or remotely acquired transactions (network handler)
			 or from a ATM host handler
			*/

			ReturnCode = txservice_Process_Auth_Request (p_cur_msg) ;
         if ( ReturnCode != NO_AUTH_ERROR )
         {
            if ( (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR) ||
                 (ReturnCode == SYSTEM_ERROR) )
            {
               if (txservice_If_Tx_Cannot_Be_Declined())
               {
                  txservice_Prepare_AuthTx_For_Forwarding(
                                           Auth_Tx.TLF01_details.general_status,
                                           TRANSACTION_APPROVED);
               }
               else
               {
                  memset( Auth_Tx.TLF01_details.auth_number, 0x00, 7 );
               }


               txservice_Forward_Auth_Tx_To_Originator(
                                            Auth_Tx.TLF01_details.handler_queue,
                                            MT_AUTH_RESPONSE, 0, 0) ;
            }
            else if (ReturnCode == ERROR_BUT_DO_NOT_FORWARD_TO_UPDATE_PROCESSOR)
            {
               memset( Auth_Tx.TLF01_details.auth_number, 0x00, 7 );
               txservice_Forward_Auth_Tx_To_Originator(
                                            Auth_Tx.TLF01_details.handler_queue,
                                            MT_AUTH_RESPONSE, 0, 0) ;
            }
         }
      break;

      case MT_AUTH_RESPONSE: 

         ReturnCode = txservice_Process_Auth_Response (p_cur_msg) ;
         if (ReturnCode != NO_AUTH_ERROR)
         txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                                  MT_AUTH_RESPONSE, 
                                                  0, 
                                                  0) ;
		 
      break ;	
		       
      case MT_DB_REPLY:

#ifdef HCTS2
    	if(ATP_CARD_STATUS_UPDATE_BY_BDO_HOST == ptemsg_get_pte_msg_data_app_data_type(ptemsg_get_pte_msg_data (p_msg_in)))
    		break;
#endif

         ReturnCode = TxDataServer_Process_Db_Reply (p_cur_msg) ;

         /* If the response originates after a call to the Update processor
          * (ALL_DATA) then the ReturnCode is always  UPDATER_NO_ERROR or
          * UPDATER_ERROR. The response code in Auth_Tx could be either
          * TRANSACTION_APPROVED or an error code.
          */
         if (ReturnCode == NO_UPDATER_ERROR || ReturnCode == UPDATER_ERROR)
         {
            /* At this stage the tx is ready to be
             * switched back to the Originator
             */
            txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                                     MT_AUTH_RESPONSE, 
                                                     0, 
                                                     0) ;
         }

         /* XIPC errors are considered to be system errors */
         else if (ReturnCode == SYSTEM_ERROR)
         {
            if (txservice_If_Tx_Cannot_Be_Declined())
            {
               txservice_Prepare_AuthTx_For_Forwarding 
                                       (Auth_Tx.TLF01_details.general_status,
                                        TRANSACTION_APPROVED) ;
            }
            else
            {
            	if (strncmp(Auth_Tx.TLF01_details.response_code, "00", 2) != 0)
            	{
            		if (true == txservice_Check_If_MC_Transaction() &&
            			0 == strncmp(Auth_Tx.TLF01_details.response_code, ACCOUNT_VERIFIED, 2) &&
						Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE)
            		{
            			/* do not clear auth number for auth refund txn */
            		}
            		else
            		{
                        // On error, clear the auth_number field.
                        memset (Auth_Tx.TLF01_details.auth_number, 0,
                                sizeof Auth_Tx.TLF01_details.auth_number) ;
            		}

            	}
            }
            txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue,
                                                     MT_AUTH_RESPONSE, 
                                                     0, 
                                                     0) ;
         }

         /* error situation */
         else if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR)
         {
            /* Transaction has been declined for some reason.
             * If the profile's authorization location is
             * LOCAL/REMOTE OVERRIDE DECLINES, we still have to send
             * it out to the host, as the host could override this decision
             */

            /* LOCAL/REMOTE OVERRIDE DECLINES */
			/* TF PHANI..For STAND IN we still have to send  it out to the host, as the host could override this decision*/
            if ( (strEqual(Auth_Tx.CAF01_details.authorization_location, "D")) || //(strEqual(Auth_Tx.CAF01_details.authorization_location, "S")) ||
                 (true == txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override() )	)
			{
               /* Determine if transaction qualifies for Cash Bonus
                * Program prior to sending it out to a remote host.
                */
               strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
               if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
               {
                  /* It does qualify; set the flag for updatds. */
                  ReturnCode = TxAuthorize_Update_AuthTx_Structures( 0, 1, 0 );
               }
			   dest_len = strlen(Auth_Tx.CAF01_details.destination_name);
							if (Auth_Tx.CAF01_details.destination_name[dest_len - 1] == 'A')
							   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
												(Auth_Tx.CAF01_details.destination_name, 
												 MT_AUTH_REQUEST, 
												 0, 
												 0) ;
							else
							   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
											  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
											   MT_AUTH_REQUEST, 
											   0, 
											   0) ;
               if (bReturnCode == false)
               {
                  sprintf(strTemp,"%s down,Tx sent to %s\n",
                          Auth_Tx.BIN01_details.destination_name,
                          Auth_Tx.TLF01_details.handler_queue) ;

                  if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                      strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

                  txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );

                  /* On error, clear the auth_number field. */
                  memset (Auth_Tx.TLF01_details.auth_number, 0,
                          sizeof Auth_Tx.TLF01_details.auth_number);

                  txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                                           MT_AUTH_RESPONSE, 
                                                           0, 
                                                           0) ;
               }
            }
            else /* LOCAL */
            {
               /* If the tx cannot be declined we still have to approve
                * the transaction even though it is an error situation.
                */

           		if(true==txservice_Check_If_JCB_Transaction() &&
           		(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE) &&
				   (strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"221608",6)==0))
           		{
           			 txservice_Prepare_posdata_for_JCB();
           		}

               if (txservice_If_Tx_Cannot_Be_Declined())
               {

            	  if (Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE &&
            		  true == txservice_Check_If_AMEX_Transaction() &&
            			 0 == strncmp(Auth_Tx.CAF01_details.destination_name,"nposa2A",6))
            	  {
            		  	  		txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
															  MT_AUTH_RESPONSE,
															  0,
															  0) ;
            	  }
            	  else
            	  {
                  txservice_Prepare_AuthTx_For_Forwarding 
                                          (Auth_Tx.TLF01_details.general_status,
                                           TRANSACTION_APPROVED) ;
            		  txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
            		                                                       MT_AUTH_RESPONSE,
            		                                                       0,
            		                                                       0) ;
               }
               }
               else
               {
                   // On error, clear the auth_number field...Do not do if profile is STIP 
                   if(strcmp(Auth_Tx.CAF01_details.authorization_location,"S") == 0 &&
					  strcmp(Auth_Tx.TLF01_details.response_code,"00") == 0		
					  )
				   {
                	   /*Do not clear Auth number*/
				   }
                   else
                   {
                	   if (true == txservice_Check_If_MC_Transaction() &&
							0 == strncmp(Auth_Tx.TLF01_details.response_code, ACCOUNT_VERIFIED, 2) &&
							Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE)
                	   {
                		   /*Do not clear Auth number for Auth Refund*/
                	   }
                	   else
                	   {
						   memset (Auth_Tx.TLF01_details.auth_number, 0,
							   sizeof Auth_Tx.TLF01_details.auth_number) ;
                	   }
                   }

                	   txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
                                                       MT_AUTH_RESPONSE, 
                                                       0, 
                                                       0) ;
               }
            }
         }
         else if (ReturnCode == PIN_ERROR)
         {
            /* In this case, the max_pin_retries has been exceeded, 
             * we still send to updater for updating the failed pin
             * count in the CCF03P table.
             */
            strcpy (strTemp, "Forwarding message to Updater\n") ;

            // On error, clear the auth_number field.
            memset (Auth_Tx.TLF01_details.auth_number, 0,
                    sizeof Auth_Tx.TLF01_details.auth_number);

            bReturnCode = txservice_Forward_Auth_Tx_To_Originator 
                                      (Auth_Tx.TLF01_details.handler_queue,
                                       MT_AUTH_RESPONSE, 
                                       0, 
                                       0) ;
         }
         else if( MID_BIN_BLK_OK == ReturnCode	)
         {
        	 //Do nothing , it might be reversal/advice which might go twice.
         }
         else if (ReturnCode == NO_AUTH_ERROR)
         {
			pATEMessageData			= ptemsg_get_pte_msg_data (p_cur_msg) ;
		    AppDataType = ptemsg_get_pte_msg_data_app_data_type(pATEMessageData);
			/* TF Phani/Nirbhay.. Drop UAG response..No need to forward and process from here  */
			if(AppDataType == UAG01_DATA)
			{   sprintf (strTemp,"Droping UAG01 response %s %s  \n",Auth_Tx.TLF01_details.retrieval_ref_num,Auth_Tx.TLF01_details.sys_trace_audit_num ) ;
				txcntl_log_message( 2, 1,strTemp, "PinnacleMsgHandler", 0 );
			}
			else
            TxDataServer_DBReply_Continue();
         }
      break ;

		case MT_NSP_VERIFY_MAGSTRIPE_RESPONSE:
		case MT_NSP_VERIFY_MAGSTRIPE_RESPONSE_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_VerifyMagstripe_Response (p_cur_msg) ;
			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ||
				 ReturnCode == SYSTEM_ERROR) 
			{
                
                // On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
                memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	
						
				txservice_Forward_Auth_Tx_To_Originator 
									(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
			}
		break ;
		case MT_NSP_VERIFY_ARQC_RESPONSE:
		case MT_NSP_VERIFY_ARQC_RESPONSE_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_ARQC_Response (p_cur_msg) ;
			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ||
				 ReturnCode == SYSTEM_ERROR) 
			{
                
                // On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
                memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	
						
				txservice_Forward_Auth_Tx_To_Originator 
									(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
			}
		break ;
		case MT_NSP_VERIFY_3D_SECURE_RESPONSE:
		case MT_NSP_VERIFY_3D_SECURE_RESPONSE_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_3D_Secure_Response (p_cur_msg) ;
			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ||
				 ReturnCode == SYSTEM_ERROR) 
			{
                
                // On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
                memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	
						
				txservice_Forward_Auth_Tx_To_Originator 
									(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
			}
		break ;
		case MT_NSP_VERIFY_3D_SECURE_RESPONSE_2:
		case MT_NSP_VERIFY_3D_SECURE_RESPONSE_2_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_3D_Secure_Response_2 (p_cur_msg) ;
			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ||
				 ReturnCode == SYSTEM_ERROR)
			{

                // On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
                memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;

				txservice_Forward_Auth_Tx_To_Originator
									(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
			}
		break ;
		case MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE:
		case MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_CNV2_For_CUP_Ecom_Response (p_cur_msg) ;

			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR)
			{
				if((Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) &&
				   (0 == strncmp (Auth_Tx.TLF01_details.response_code, "00", 2))&&
				   ((0 != strncmp( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY, CURRENCY_LEN ))&&
					(0 != strncmp ( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))))
				{
					txservice_Forward_Auth_Tx_To_Originator
					(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
				}
				else
				{
					// On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
					memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;
					txservice_Forward_Auth_Tx_To_Originator
					(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
				}
			}
			else if ( ReturnCode == SYSTEM_ERROR)
			{
				// On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
				memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;
				txservice_Forward_Auth_Tx_To_Originator
				(Auth_Tx.TLF01_details.handler_queue, MT_AUTH_RESPONSE, 0, 0) ;
			}
		break ;
		case MT_NSP_VERIFYPIN_RESPONSE:
		case MT_NSP_VERIFYPIN_RESPONSE_KEYBLOCK:
			ReturnCode = TxAuthorize_Process_VerifyPin_Response (p_cur_msg) ;
			if (ReturnCode == ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ||
				 ReturnCode == SYSTEM_ERROR) 
			{
                strcpy (strTemp,"Forwarding message to Originator\n") ; 
                
                // On error, clear the auth_number field - as per Musab Saleh (April 28 '99)
                memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	
				txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
																	  MT_AUTH_RESPONSE, 
																	  0, 
																	  0) ;
			}
         else if (ReturnCode == PIN_ERROR)
         {
            /* In this case, the pin has not matched, we still send to
             * originator for updating failed pin count in the CCF03P table
             * Also need to make sure no auth number exists.
             */
            memset( Auth_Tx.TLF01_details.auth_number, 0x00,
                    sizeof Auth_Tx.TLF01_details.auth_number);

            strcpy( Auth_Tx.TLF01_details.response_code, INCORRECT_PIN );

            txservice_Forward_Auth_Tx_To_Originator( Auth_Tx.TLF01_details.handler_queue,
                                                     MT_AUTH_RESPONSE,
                                                     0,
                                                     0 );
			}
		break ;

		
		case MT_SYSTEM_REPLY: 
		break ;

		case MT_TIMER_REPLY:
		break ;

		case MT_TIMER_TIMEOUT:
      break ;	

      case MT_SYSTEM_REQUEST:
            ptesystm_system_msg_handler (p_msg_in) ;
            if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
            {
               /* When pinged, display application version on Monitor. */
               sprintf( strTemp,
                       "Pinged -> Transaction Controller Service: %s, version %s",
                        ServiceName, Version );
               txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
			/* Get config parameter for Falcon send flag while ping request Added by praneeth*/
			get_falcon_state_from_ini();
			//txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
            /* Get config parameter for routing of Visa Debits. */
            get_visa_debit_routing();
            sprintf( strTemp, "Routing Visa Debits to %s", vsms_host_que_name );
            txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
            nFraudGuardState = get_fraudguard_state_from_ini();
            memset( strTemp, 0x00, sizeof(strTemp) );
            if ( nFraudGuardState == 0 )
            {
               strcpy( strTemp, "Fraudguard is not active." );
            }
            else
            {
               strcpy( strTemp, "Fraudguard is active for" );
               first_card_type = true;

               /* AMEX */
               if ( (nFraudGuardState & FG_STATE_ACQ_AMEX) ||
                    (nFraudGuardState & FG_STATE_ISS_AMEX)  )
               {
                  first_card_type = false;

                  if ( (nFraudGuardState & FG_STATE_ACQ_AMEX) &&
                       (nFraudGuardState & FG_STATE_ISS_AMEX)  )
                  {
                     strcat( strTemp, " AMEX (A,I)" );
                  }
                  else if ( nFraudGuardState & FG_STATE_ACQ_AMEX )
                     strcat( strTemp, " AMEX (A)" );

                  else if ( nFraudGuardState & FG_STATE_ISS_AMEX )
                     strcat( strTemp, " AMEX (I)" );
               }

               /* JCB */
               if ( (nFraudGuardState & FG_STATE_ACQ_JCB) ||
                    (nFraudGuardState & FG_STATE_ISS_JCB)  )
               {
                  if ( first_card_type == false )
                     strcat( strTemp, "," );
                  else
                     first_card_type = false;

                  if ( (nFraudGuardState & FG_STATE_ACQ_JCB) &&
                       (nFraudGuardState & FG_STATE_ISS_JCB)  )
                  {
                     strcat( strTemp, " JCB (A,I)" );
                  }
                  else if ( nFraudGuardState & FG_STATE_ACQ_JCB )
                     strcat( strTemp, " JCB (A)" );

                  else if ( nFraudGuardState & FG_STATE_ISS_JCB )
                     strcat( strTemp, " JCB (I)" );
               }

               /* MasterCard */
               if ( (nFraudGuardState & FG_STATE_ACQ_MC) ||
                    (nFraudGuardState & FG_STATE_ISS_MC)  )
               {
                  if ( first_card_type == false )
                     strcat( strTemp, "," );
                  else
                     first_card_type = false;

                  if ( (nFraudGuardState & FG_STATE_ACQ_MC) &&
                       (nFraudGuardState & FG_STATE_ISS_MC)  )
                  {
                     strcat( strTemp, " MC (A,I)" );
                  }
                  else if ( nFraudGuardState & FG_STATE_ACQ_MC )
                     strcat( strTemp, " MC (A)" );

                  else if ( nFraudGuardState & FG_STATE_ISS_MC )
                     strcat( strTemp, " MC (I)" );
               }

               /* VISA */
               if ( (nFraudGuardState & FG_STATE_ACQ_VISA) ||
                    (nFraudGuardState & FG_STATE_ISS_VISA)  )
               {
                  if ( first_card_type == false )
                     strcat( strTemp, "," );
                  else
                     first_card_type = false;

                  if ( (nFraudGuardState & FG_STATE_ACQ_VISA) &&
                       (nFraudGuardState & FG_STATE_ISS_VISA)  )
                  {
                     strcat( strTemp, " VISA (A,I)" );
                  }
                  else if ( nFraudGuardState & FG_STATE_ACQ_VISA )
                     strcat( strTemp, " VISA (A)" );

                  else if ( nFraudGuardState & FG_STATE_ISS_VISA )
                     strcat( strTemp, " VISA (I)" );
               }
            }
			txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
			}
		 if( ST1_SRVC_MRAON == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 1;
			send_trace_response(p_msg_in);
			sprintf( strTemp, "MRA enabled for %s, version %s", ServiceName, Version );
            txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
	     }
		 if( ST1_SRVC_MRAOFF == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 0;
			send_trace_response(p_msg_in);
			sprintf( strTemp, "MRA disabled for %s, version %s", ServiceName, Version );
            txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
	     }
		 if( ST1_SRVC_FALCONON == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			nFalconSendFlag = 1;
			send_trace_response(p_msg_in);
			sprintf( strTemp, "FALCON enabled for %s, version %s", ServiceName, Version );
            txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
	     }
		 if( ST1_SRVC_FALCONOFF == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			nFalconSendFlag = 0;
			send_trace_response(p_msg_in);
			sprintf( strTemp, "FALCON disabled for %s, version %s", ServiceName, Version );
            txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
	     }            
		 if(ST1_LOAD_RPS_FLAG == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the RPS processing flag from tf.ini*/
			 	txservice_load_rps_flag_for_all_cardbrands();
			 	send_trace_response(p_msg_in);
				sprintf( strTemp, "RPS flag reloaded for all card brands" );
	            txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_OBS_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 	//txservice_load_OBS_records_for_MC();
			    Txcntl_Get_MC_delcine_rc();
			 	send_trace_response(p_msg_in);
				sprintf( strTemp, "OBS records reloaded for MC" );
	            txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_VISA3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
		 	txservice_load_VISA3DS2_records_for_VISA();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "VISA3DS2 records reloaded for VISA" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_AMEX3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
		 	txservice_load_AMEX3DS2_records_for_AMEX();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "AMEX3DS2 records reloaded for AMEX" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_DINERS3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
		 	txservice_load_DINERS3DS2_records_for_DINERS();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "DINERS3DS2 records reloaded for DINERS" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_JCB3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
		 	txservice_load_JCB3DS2_records_for_JCB();
			 Get_JCB3DS2_CAVV_Parameters_FromTF_INI();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "JCB3DS2 records reloaded for JCB" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }

		 if(ST1_LOAD_CUP3DS2_RECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
		 	txservice_load_CUP3DS2_records_for_CUP();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "CUP3DS2 records reloaded for CUP" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_SECONDARYEXPIRYDATE_FLAG == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			txservice_load_SecondaryExpiryDateValidationflag();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "Secondary expiry date validation flag is loaded with value %d", SecondaryExpiryDateValidationFlag);
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_MID_CPNG_TFINIRECORDS == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			txservice_load_MID_CPNG_records();
		 	send_trace_response(p_msg_in);
		 	sprintf( strTemp, "MIDCpng records reloaded for txcntl" );
		 	txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 if(ST1_LOAD_TO_RC_VALUES ==  ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 txcntl_read_tf_ini_TO_rc(strTemp);
			 send_trace_response(p_msg_in);
			 sprintf( strTemp,
			          "TO rc for card brands, AMEX: %s,DCI:%s,CUP:%s,JCB:%s,MC:%s,VISA:%s",
					  amex_voice_timeout_rc, dci_voice_timeout_rc,cup_voice_timeout_rc,jcb_voice_timeout_rc,mc_voice_timeout_rc,visa_voice_timeout_rc);
			 txcntl_log_message( 1, 1, strTemp,"PinnacleMsgHandler",0 );
		 }
		 else if(ST1_LOAD_RC_FOR_FAI ==  ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 txcntl_read_tf_ini_FAI_rc(strTemp);
			 get_FRT_Decline_RC();
			 send_trace_response(p_msg_in);
			 sprintf( strTemp,
			          "FAI decline rc for card brands, AMEX: %s,DCI:%s,CUP:%s,JCB:%s,MC:%s,VISA:%s",
					  amex_voice_fai_rc, dci_voice_fai_rc,cup_voice_fai_rc,jcb_voice_fai_rc,mc_voice_fai_rc,visa_voice_fai_rc);
			 txcntl_log_message( 1, 1, strTemp,"PinnacleMsgHandler",0 );
		 }
		 else if(ST1_SYS_FALCONRT_ON == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Flag = 1;
			 	txcntl_write_falcon_RT_falg_into_inifile("1");
			 	send_trace_response(p_msg_in);
		 }
		 else  if(ST1_SYS_FALCONRT_OFF == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Flag = 0;
			 	txcntl_write_falcon_RT_falg_into_inifile("0");
			 	send_trace_response(p_msg_in);
		 }
		 else if(ST1_RELOAD_FALCON_FLAG == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from DB*/
			 	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
			 	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
			 	get_FRT_Decline_RC();
			 	txcntl_read_falconrt_ext10_flag_for_transactions();
			 	send_trace_response(p_msg_in);
				sprintf( strTemp, "Falcon Flag reloaded for processing Transactions" );
	            txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
#ifdef HCTS2
		 if(ptemsg_get_msg_subtype1(p_msg_in) == ST1_VERTEXON_CARD_STATUS_FLAG)
		 {
			 txcntl_read_atp_card_activation_parameters();
			 send_trace_response(p_msg_in);
			 txcntl_log_message( 3, 1, "Vertexon card status parameters reloaded", "PinnacleMsgHandler", 0 );
		 }
#endif

		 if(ST1_SYS_FALCONRT_EXT10_ON == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Ext10_Flag = 1;
			 	txcntl_write_falconRT_EXT10_flag_into_inifile("1");
			 	txcntl_read_falconrt_ext10_flag_for_transactions();
			 	send_trace_response(p_msg_in);
		 }
		 else  if(ST1_SYS_FALCONRT_EXT10_OFF == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Ext10_Flag = 0;
			 	txcntl_write_falconRT_EXT10_flag_into_inifile("0");
			 	txcntl_read_falconrt_ext10_flag_for_transactions();
			 	send_trace_response(p_msg_in);
		 }

		 if (ST1_SYS_KEYBLOCK_ON == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read Keyblock Flag from tf.ini */
			/* keyblock_enable_flag_for_MC	= KEYBLOCK_ENABLE;
			 keyblock_enable_flag_for_VISA  = KEYBLOCK_ENABLE;
			 keyblock_enable_flag_for_AMEX  = KEYBLOCK_ENABLE;
			 keyblock_enable_flag_for_JCB   = KEYBLOCK_ENABLE;
			 keyblock_enable_flag_for_UPI   = KEYBLOCK_ENABLE;*/
			 keyblock_enable_flag_for_DCI   = KEYBLOCK_ENABLE;
			 set_keyblock_flag_details("1");
			 get_keyblock_flag_details();
			 send_trace_response(p_msg_in);
			 memset(strTemp, 0x00, sizeof(strTemp));
			 sprintf( strTemp, "Keyblock Flag is set to ON for processing Transactions" );
			 txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
		 else if (ST1_SYS_KEYBLOCK_OFF == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read Keyblock Flag from tf.ini */
			 /*keyblock_enable_flag_for_MC	= KEYBLOCK_DISABLE;
			 keyblock_enable_flag_for_VISA  = KEYBLOCK_DISABLE;
			 keyblock_enable_flag_for_AMEX  = KEYBLOCK_DISABLE;
			 keyblock_enable_flag_for_JCB   = KEYBLOCK_DISABLE;
			 keyblock_enable_flag_for_UPI   = KEYBLOCK_DISABLE;*/
			 keyblock_enable_flag_for_DCI   = KEYBLOCK_DISABLE;
			 set_keyblock_flag_details("0");
			 get_keyblock_flag_details();
			 send_trace_response(p_msg_in);
			 memset(strTemp, 0x00, sizeof(strTemp));
			 sprintf( strTemp, "Keyblock Flag is set to OFF for processing Transactions" );
			 txcntl_log_message( 3, 1,strTemp, "PinnacleMsgHandler", 0 );
		 }
      break ;

      case MT_SYS_MONITOR:

         if ( ST1_MONITOR_UP == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            /* This is a test message from Monitor. Just respond to it. */
            (void)txservice_Send_To_Monitor( MT_SYS_MONITOR,
                                             ST1_MONITOR_UP,
                                             ST2_NONE,
                                             NULL_STR,
                                             0 );
         }
         break;
/*  ThoughtFocus Girija Y - Added Message type to Support Stand-In Tx for Cadencie Link */
	  case MT_LOGON_RESPONSE:

		    if( ( 0 == strncmp(Auth_Tx.TLF01_details.acquirer_id,"nciftr",6)))
			{
		  
				p_msg_data     = ptemsg_get_pte_msg_data (p_cur_msg) ;
				p_data = ptemsg_get_pte_msg_data_data (p_msg_data) ;

				memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
				memcpy (&Auth_Tx, (AUTH_TX*) p_data, sizeof Auth_Tx) ;
				len = strlen(Auth_Tx.TLF01_details.originator_queue);
				memset(destination_que,0x00,sizeof(destination_que));
				memcpy(destination_que,Auth_Tx.TLF01_details.originator_queue,(len - 1));

				GetAscendentBinDirectory(bindir);
				strcat(bindir,"sbatch");
				sprintf(cmd,"%s %s",bindir,destination_que);
				strcat(cmd,"&");
				system(cmd);
				strcpy( strTemp, "Starting Sbatch for MP Stip Transaction");
            	txcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
			}
		  break;

	  case MT_RT_RESPONSE_APPROVED:
		  	 pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
		     pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

		     memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;
		     sprintf(strTemp,"Recievd Approved response from falcon controller ServiceName %s",ServiceName);
		     txcntl_log_message( 2, 1,strTemp,"PinnacleMsgHandler", 1 );
		     dest_len = strlen(Auth_Tx.CAF01_details.destination_name);
			if (Auth_Tx.CAF01_details.destination_name[dest_len - 1] == 'A')
			{
			   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
								(Auth_Tx.CAF01_details.destination_name,
								 MT_AUTH_REQUEST,
								 0,
								 0) ;
			}
			else
			{
			   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
							  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
							   MT_AUTH_REQUEST,
							   0,
							   0) ;
			}

		  break;

	  case MT_RT_RESPONSE_DECLINE:
		  	 pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
		     pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

		     sprintf(strTemp,"Recievd decline response from falcon controller ServiceName %s",ServiceName);
		     txcntl_log_message( 2, 1,strTemp,"PinnacleMsgHandler", 1 );
		     memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;
		     memset( Auth_Tx.TLF01_details.auth_number, 0x00, 7 );

		     /* the transactions declined by FRT will be ATP-STIP procsd */
		     strcpy(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN);
		     strcpy(Auth_Tx.TLF01_details.response_text,"ATP standin, FAI Decline");

		  /*Lets Prepare the RC/AC that Atp need to send to Originator*/
		  if(true == txservice_Check_if_POS_Acq_Transaction())
		  {
			if(true == txservice_Check_If_MC_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_MC,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_VISA_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_VISA,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_AMEX_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_AMEX,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_JCB_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_JCB,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_CUP_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_UPI,
						FRT_DECLINE_RC_LEN);
			}

			else if (true == txservice_Check_If_Diners_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_Acq_DCI,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_if_PLFC_Transaction ())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_PLFC,
						FRT_DECLINE_RC_LEN);
			}
		  }
		  else if(true == txservice_Check_if_Voice_Acq_Transaction())
		  {
			if(true == txservice_Check_If_MC_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_MC,
						FRT_DECLINE_RC_LEN);

			}

			else if(true == txservice_Check_If_VISA_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_VISA,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_AMEX_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_AMEX,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_JCB_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_JCB,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_CUP_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_UPI,
						FRT_DECLINE_RC_LEN);
			}

			else if (true == txservice_Check_If_Diners_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_VOICE_DCI,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_if_PLFC_Transaction ())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_PLFC,
						FRT_DECLINE_RC_LEN);
			}
		  }
		  else if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncmcrd",6)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncmcr3",6)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncvis3",6)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncpos", 5)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"nccup", 5)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncjcb", 5)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"ncdci", 5)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"nceqit",6)) ||
				  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue ,"neqit2",6)))
		  {
			if(true == txservice_Check_If_MC_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_ISS_MC,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_VISA_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_ISS_VISA,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_AMEX_Transaction())
			{
				txcntl_map_amex_action_code_to_iso_repsonse_code(FRT_Decline_RC_ISS_AMEX);
				if(0 == strlen(Auth_Tx.TLF01_details.response_code))
				{
					strncpy(Auth_Tx.TLF01_details.response_code,
							FRT_DECLINE_RC_ISS_AMEX_DEFAULT,
							FRT_DECLINE_RC_LEN);
				}

			}

			else if(true == txservice_Check_If_JCB_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_ISS_JCB,
						FRT_DECLINE_RC_LEN);
			}

			else if(true == txservice_Check_If_CUP_Transaction())
			{
				strncpy(Auth_Tx.TLF01_details.response_code,
						FRT_Decline_RC_ISS_UPI,
						FRT_DECLINE_RC_LEN);
			}

			else if (true == txservice_Check_If_Diners_Transaction())
			{
				txcntl_match_DCI_to_ISO_response_code(FRT_Decline_RC_ISS_DCI);
				if(0 == strlen(Auth_Tx.TLF01_details.response_code))
				{
					strncpy(Auth_Tx.TLF01_details.response_code,
							FRT_DECLINE_RC_ISS_DCI_DEFAULT,
							FRT_DECLINE_RC_LEN);
				}
			}
		  }
		  else if(true == txservice_Check_if_PLFC_Transaction ())
		  {
			  strncpy(Auth_Tx.TLF01_details.response_code,
					  FRT_Decline_RC_PLFC,
					  FRT_DECLINE_RC_LEN);
		  }

		  txservice_Forward_Auth_Tx_To_Originator_for_RT_decline
		  	  	  (Auth_Tx.TLF01_details.handler_queue,
		  	  			  	  	  	  	  	  MT_AUTH_RESPONSE,
											  MT_RT_RESPONSE_DECLINE,
		  		                                0) ;
		  break;

	  case MT_RT_RESPONSE_TIMEOUT:
		  	 pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
		     pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

		     sprintf(strTemp,"Recievd timeout response from falcon controller ServiceName %s",ServiceName);
		     txcntl_log_message( 1, 3,strTemp,"PinnacleMsgHandler", 1 );
		     memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;

		   /*print timeout msg in the log and do as approved */
		   sprintf(strTemp,"Real Time response is timedout");
		   txcntl_log_message( 2, 1,strTemp, "PinnacleMsgHandler", 0 );

		   dest_len = strlen(Auth_Tx.CAF01_details.destination_name);
			if (Auth_Tx.CAF01_details.destination_name[dest_len - 1] == 'A')
			{
			   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
								(Auth_Tx.CAF01_details.destination_name,
								 MT_AUTH_REQUEST,
								 0,
								 0) ;
			}
			else
			{
			   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
							  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
							   MT_AUTH_REQUEST,
							   0,
							   0) ;
			}

		  break;
		case MT_AUTH_RT_EMPTY_RESPONSE:

			if(Falcon_RT_Flag == 1)
			{
				txcntl_log_message(2, 1, "Empty response message received from falcon to txcntl", "PinnacleMsgHandler", 1);
			}
			break;

      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          txcntl_log_message( 1, 3,strTemp, "PinnacleMsgHandler", 0 );
		break ;
    }  /* switch msg_type */
} /* PinnacleMsgHandler */


void Get_JCB3DS2_CAVV_Parameters_FromTF_INI()
{
	   CHAR  filename     [256]={0};
	   CHAR  tmpstr       [256]={0};

	   GetAscendentConfigDirectory(tmpstr);
	   sprintf(filename, "%stf.ini", tmpstr);

	   GetPrivateProfileString(
	    "JCB3DS2_CAVV_PARAMETERS",	            /*  points to section name  */
	    "CAVV_VERIFICATION_RC",           /*  points to key name  */
	    "05",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   strncpy(cavv_verification_rc,tmpstr,2);

	   GetPrivateProfileString(
	    "JCB3DS2_CAVV_PARAMETERS",	            /*  points to section name  */
	    "AUTHENTICATION_ATTEMPT_RESULT_CODE",           /*  points to key name  */
	    "05",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   strncpy(Authentication_attempt_result_code,tmpstr,2);

	   GetPrivateProfileString(
	    "JCB3DS2_CAVV_PARAMETERS",	            /*  points to section name  */
	    "CAV2_VALIDATION_FLAG",           /*  points to key name  */
	    "0",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   cav2_validation_flag=tmpstr[0];

	   GetPrivateProfileString(
	   	   	    "JCB3DS2_CAVV_PARAMETERS",	            /*  points to section name  */
	   	   	    "AUTH_ATTEMPT_VERIFY_FLAG",           /*  points to key name  */
	   	   	    "0",                      /*  points to default string  */
	   	   	    tmpstr,                         /*  points to destination buffer  */
	   	   	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	   	   	    filename                        /*  points to initialization filename  */
	   	   	   );

	   	   authentication_attempt_verify_flag=tmpstr[0];

}
/*************************************************************************************
NAME:			MainProcessor
DESCRIPTION:	This is the primary thread of the application. 
				main (in Console.c) calls StartUp() which creates this thread function 
INPUTS:			None
OUTPUTS:		None
RETURNS:        Void
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
void MainProcessor(void)
{
   pPTE_MSG p_msg = 0 ; 
   LONG     lReturnCode = 0L ;
   CHAR     strBuffer [256] = {0} ;
   CHAR     strTemp [256] = {0} ;
   CHAR     strAppName [MAX_APP_NAME_SIZE+1] = {0} ;
   CHAR     AuthNumber[7];
   BOOLEAN  bReturnCode = TRUE ;
   INT      err_ctr = 0;
   INT      rcode;

   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;

   GetAppName(strAppName) ;
   strcpy (txcntl_que_name, strAppName) ;
   strcat (txcntl_que_name, "A") ;
   /* Display Application version. */
   sprintf(strTemp, "Attempting to start %s, version %s", strAppName, Version);
	txcntl_get_error_warning_file_name_path();
   txcntl_log_message( 2, 1,strTemp, "MainProcessor", 0 );
   strcat( strTemp, "\n" );
   PRINT( strTemp );

	/* Read the TF.INI file looking for section [FRAUDGUARD].
	 * In that section will be an indicator to either have Fraudguard
    * enabled or disabled.  Txcntl should default the state of the
    * Fraudguard functionality to disabled
	 */
//	nFraudGuardState = get_fraudguard_state_from_ini();
	/*Falcon send flag, if falg =0 Falcon is not functinal do not send tx to FALCON
				    	if falg =1 Falcon is functinal ATP can send tx to FALCON
    */
	nFalconSendFlag  = get_falcon_state_from_ini();
	/* Read the TF.INI file looking for section [AUTOMATIC_RESEND].
	 * In that section will be an indicator to either have AUTOMATIC_RESEND
    * enabled or disabled.  Txcntl should default the state of the
    * AUTOMATIC_RESEND functionality to enabled - Girija Y */

	nAutomaticResendState = get_automatic_resend_state_from_ini();
	get_MRACON_state_from_ini();
/*During startup read RPS STIP processing flag*/
	txservice_load_rps_flag_for_all_cardbrands();
	txservice_load_JCB_contactless_magstripe_Flag_RC();
	Totol_numof_MCC = txservice_Get_MCC_Details_For_Rps_Txn();
	Get_Name_Validation_Parameters_FromTF_INI();
	get_keyblock_flag_details ();

	memset(strTemp,0x00,sizeof(strTemp));
	sprintf(strTemp,"Totol_numof_MCC: %d",Totol_numof_MCC);
	txcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );

   #ifdef _DEBUG
      pteipc_trace_on(); 
   #endif

#ifdef _DEBUG   
   if( !pteipc_init_single_instance_app( strAppName, "pte_ipc" ) ) 
#else
   if( !pteipc_init_multiple_instance_app( strAppName, ServiceName,"pte_ipc"))
#endif
   {	
      sprintf (strTemp,
              "%s: MainProcessor: Failed to Create XIPC queues",
               strAppName) ;
      txcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );

      /* signal that we are ready to shut down and quit */
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif     
	  MainProcessDone = 1 ;
      return;
   }
   sprintf(strTemp,
          "Started the Transaction Controller Service: %s",
           ServiceName);
   txcntl_log_message( 0, 1,strTemp, "MainProcessor", 0 );
   /* Get config parameter for routing of Visa Debits. */
   get_visa_debit_routing();
   sprintf( strBuffer, "Routing Visa Debits to %s", vsms_host_que_name );
   txcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );

  /* Initialize the AuthNumber in shared memory. */
   if ( 0 == strcmp(ServiceName, "txcntl") )
   {
      /* Initialize this for a single instance only. */
      if(TxDataServer_initAuthNumber() == false)
	  {
		sprintf(strTemp,"Not able to init Auth Number:TxDataServer_initAuthNumber failed");
		txcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );
	  }
   }

   /* Initialize the NCF01 records that are used to get DE32 for Fraudguard. */
   memset( ncf01_recs, 0x00, sizeof(NCF01)*MAX_NCF01_RECS );
   /* TF- Phani - Read tf.ini for MCC values*/

//	Get_MCC_Allowable_values_From_Ini();
	Get_MCC_Allowable_values_From_DB();
	//txservice_load_OBS_records_for_MC();
	txservice_load_VISA3DS2_records_for_VISA();
	txservice_load_AMEX3DS2_records_for_AMEX();
	txservice_load_DINERS3DS2_records_for_DINERS();
	txservice_load_JCB3DS2_records_for_JCB();
	txservice_load_CUP3DS2_records_for_CUP();
    Get_JCB3DS2_CAVV_Parameters_FromTF_INI();
    txservice_load_MID_CPNG_records();
    txcntl_read_tf_ini_TO_rc(strTemp);
    txcntl_read_falconrt_flag_for_transactions();
 	Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
 	Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
 	get_FRT_Decline_RC();
 	Txcntl_Get_MC_delcine_rc();
 	txservice_load_SecondaryExpiryDateValidationflag();
 	txcntl_read_falconrt_ext10_flag_for_transactions();
 	txcntl_read_flag_for_cup3ds2_transactions_in_stip();
 	
#ifdef VISA7
 	txcntl_read_system_integrity_parameters();
#endif

#ifdef MC12_OCT2023
 	txcntl_read_execssive_delcines_parameter_for_mc();
#endif

#ifdef MC13_OCT2023
 	txcntl_read_mac_validation_flag();
#endif

#ifdef HCTS2
 	txcntl_read_atp_card_activation_parameters();
#endif

   while (!EndProcessSignalled)
   {
      /* You are blocked here waiting for a message on either app queue or
       * control que.  If there is no message on either que for 5 seconds,
       * the blocking call returns.  Application_que_name and control_que_name
       * are populated in pteipc_init_multiple_instance_app (pteipc.c).
       */
      p_msg = pteipc_receive( application_que_name, control_que_name,
                              5, &lReturnCode);

      if(p_msg != NULL_PTR)
      {
         err_ctr = 0;
         PinnacleMsgHandler (p_msg);
         free(p_msg);
      }
      else if(lReturnCode == QUE_ER_TIMEOUT)
      {
         /* Queue has timed out. There is nothing in it. */
         if ( AuthNumberChanged == true )
         {
            /* Auth number has changed since last update,
             * so update UAG01 with the new value.
             */
            memset( AuthNumber, 0x00, sizeof(AuthNumber) );
            rcode = ReadAuthTable( AuthNumber, NOLOCK );
            if ( MEM_OK == rcode )
            {	/* TF PHANI.. Dont update DB, if AuthNumber is NULL  */
				if(isalphanum(AuthNumber))
				{
				   TxDataServer_Update_UAG01( AuthNumber );
				}
				else
				{
					sprintf( strBuffer,"Auth Number is NOT alphanumaric %s : Not Writing to UAG01. ",AuthNumber);
					txcntl_log_message( 2, 2,strTemp, "MainProcessor", 0 );
				}
				   AuthNumberChanged = false;
				
				
            }
            else
            {
               sprintf( strBuffer,
                       "Unable to store Auth Number (%s) in shared memory. "
                       "Xipc err: %d",
                        AuthNumber, rcode );
               txcntl_log_message( 1,2,strTemp, "MainProcessor", 0 );
            }
         }
      }
      else
      {
         pteipc_get_errormsg(lReturnCode, strTemp);
         sprintf( strBuffer,
                 "Error on pteipc_receive %s, error code: %ld",
                  strTemp, lReturnCode );
         txcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );

         /* There is a problem with the queue.  Delay 1 second
          * so as to not flood Applnk's queue and syslog.
          */
         #ifdef WIN32
            Sleep( 1000 );
         #else
            usleep( 1000000 );
         #endif

         /* Inform Operators if service's queue is no longer readable. */
         if ( ++err_ctr > 10 )
         {
            err_ctr = 0;
            sprintf( strBuffer,
                       "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
                     ServiceName );
            txcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );
            #ifdef WIN32
               Sleep (5000);
            #else
               usleep (5000000);
            #endif
         }
      }
   }

   sprintf( strTemp,
           "Shutting down the Transaction Authorization Service: %s",
            ServiceName);
   txcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );

   /* Log out of XIPC */
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif   
	  MainProcessDone = 1 ;
   return;
	
} /* MainProcessor */
/*********************************************************************/


/*********************************************************************/
/*BYTE Parse_MCC_Codes()
{

   
	
   ptrtocomma = MCC_CODES;
   mcc_codes_len = strlen(MCC_CODES);
   if(mcc_codes_len == 0)
   {
	 sprintf(strTemp,
          "NO MCC Codes in INI file ");

   }
    
	while (*ptrtocomma != '\0')
	{
		while(*ptrtocomma != ',')
		{	mcc_list[i][j++] =*ptrtocomma++;
		if(*ptrtocomma == '\0')
			break;
		}
		i++;
		j=0;
		*ptrtocomma++;
		if(*ptrtocomma == NULL)
			break;
	}
	// Check '-' and store in LOW and HIGH
	No_of_MCC_codes = i;

	i=0;j=0;
	while(mcc_list[i] != '\0')
	{
		ptrtohigh = (char *)mcc_list[i];
		while(*ptrtohigh != '-')
		{	//mcc_check_list[i].low[j++] = atoi(*ptrtohigh++);
			temp_MCC[j++] = *ptrtohigh++;
			if(*ptrtohigh == '\0' )
			break;
			if(j>4)
			{
				sprintf(strTemp,
					  "MCC code Length is more than 4 digits  : %d",
					   mcc_check_list[i].low);

			}
		}
		mcc_check_list[i].low = atoi(temp_MCC);
		*ptrtohigh++;j=0;
		memset(temp_MCC,0x00,sizeof(temp_MCC));
		while(*ptrtohigh != ',')
		{	//mcc_check_list[i].high[j++] =atoi(*ptrtohigh++);
			temp_MCC[j++] = *ptrtohigh++;
		 if(*ptrtohigh == '\0')
			break;

			if(j>4)
			{
				sprintf(strTemp,
				  "MCC code Length is more than 4 digits - : %d",
				   mcc_check_list[i].high);
			}

		}
		mcc_check_list[i].high =atoi(temp_MCC);
		i++;j=0;
		memset(temp_MCC,0x00,sizeof(temp_MCC));

		//*ptrtohigh++;

		if(i > No_of_MCC_codes)
			break;

	}

	for (i=0;i<=No_of_MCC_codes ; i++ )
	{
		if(mcc_check_list[i].high == 0)
			mcc_check_list[i].high = mcc_check_list[i].low;

	}


}
*/
/*********************************************************************/


/*********************************************************************/
void Get_MCC_Allowable_values_From_Ini()
{
   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80],strTemp[256]="",temp_MCC[6]="";
   INT   ret_val = true;
   INT mcc_codes_len=0,no_of_items=0;
   INT i=0,j=0;
	CHAR *token=NULL;
	CHAR mcc_list[MCC_MAX_CODES][15]; char *ptrtocomma=NULL;
	char *ptrtohigh=NULL;
   memset( mcc_list, 0x00, sizeof(mcc_list) );
	

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*--------------------------------------*/
   /* Get MCC_CODES0 Service */
   /*--------------------------------------*/
   rc = GetPrivateProfileString(
            "MCC_CODES",                         /* points to section name         */
            "ALLOWABLE_CODES",          /* points to key name             */
            "",                      /* Default string ()        */
             MCC_CODES0,           /* points to destination buffer   */
             sizeof(MCC_CODES0)-1, /* size of destination buffer     */
             filename                      /* points to ini filename         */
   );
   /*--------------------------------------*/
   /* Get MCC_CODES0 Service */
   /*--------------------------------------*/
   rc = GetPrivateProfileString(
            "MCC_CODES",                         /* points to section name         */
            "ALLOWABLE_CODES1",          /* points to key name             */
            "",                      /* Default string ()        */
             MCC_CODES1,           /* points to destination buffer   */
             sizeof(MCC_CODES1)-1, /* size of destination buffer     */
             filename                      /* points to ini filename         */
   );

      /*--------------------------------------*/
   /* Get MCC_CODES0 Service */
   /*--------------------------------------*/
   rc = GetPrivateProfileString(
            "MCC_CODES",                         /* points to section name         */
            "ALLOWABLE_CODES2",          /* points to key name             */
            "",                      /* Default string ()        */
             MCC_CODES2,           /* points to destination buffer   */
             sizeof(MCC_CODES2)-1, /* size of destination buffer     */
             filename                      /* points to ini filename         */
   );
	
   strcpy(MCC_CODES,MCC_CODES0);
   strcat(MCC_CODES,MCC_CODES1);
   strcat(MCC_CODES,MCC_CODES2);
// Read from MCC_CODES
   ptrtocomma = MCC_CODES;
   mcc_codes_len = strlen(MCC_CODES);
   if(mcc_codes_len == 0)
   {
	 sprintf(strTemp,
          "NO MCC Codes in INI file ");
     txcntl_log_message( 1, 1,strTemp, "Get_MCC_Allowable_values_From_Ini", 0 );

   }
    
	while (*ptrtocomma != '\0')
	{
		while(*ptrtocomma != ',')
		{	mcc_list[i][j++] =*ptrtocomma++;
		if(*ptrtocomma == '\0')
			break;
		}
		i++;
		j=0;
		*ptrtocomma++;
		if(*ptrtocomma == NULL)
			break;
	}
	// Check '-' and store in LOW and HIGH
	No_of_MCC_codes = i;

	i=0;j=0;
	while(mcc_list[i] != '\0')
	{
		ptrtohigh = (char *)mcc_list[i];
		while(*ptrtohigh != '-')
		{	//mcc_check_list[i].low[j++] = atoi(*ptrtohigh++);
			temp_MCC[j++] = *ptrtohigh++;
			if(*ptrtohigh == '\0' )
			break;
			if(j>4)
			{
				sprintf(strTemp,
					  "MCC code Length is more than 4 digits  : %d",
					   mcc_check_list[i].low);
			   txcntl_log_message( 1, 1,strTemp, "Get_MCC_Allowable_values_From_Ini", 0 );

			}
		}
		mcc_check_list[i].low = atoi(temp_MCC);
		*ptrtohigh++;j=0;
		memset(temp_MCC,0x00,sizeof(temp_MCC));
		while(*ptrtohigh != ',')
		{	//mcc_check_list[i].high[j++] =atoi(*ptrtohigh++);
			temp_MCC[j++] = *ptrtohigh++;
		 if(*ptrtohigh == '\0')
			break;

			if(j>4)
			{
				sprintf(strTemp,
				  "MCC code Length is more than 4 digits - : %d",
				   mcc_check_list[i].high);
		       txcntl_log_message( 1, 1,strTemp, "Get_MCC_Allowable_values_From_Ini", 0 );

			}

		}
		mcc_check_list[i].high =atoi(temp_MCC);
		i++;j=0;
		memset(temp_MCC,0x00,sizeof(temp_MCC));

		//*ptrtohigh++;

		if(i > No_of_MCC_codes)
			break;

	}

	for (i=0;i<=No_of_MCC_codes ; i++ )
	{
		if(mcc_check_list[i].high == 0)
			mcc_check_list[i].high = mcc_check_list[i].low;

	}
	
	
	

	sprintf(strTemp,
          "NO of MCC code read from INI file is - : %d",
           No_of_MCC_codes);
   txcntl_log_message( 1, 1,strTemp, "Get_MCC_Allowable_values_From_Ini", 0 );
   return;
}

/******************************************************************************
 *
 *  NAME:         get_visa_debit_routing
 *
 *  DESCRIPTION:  This function reads information from the tf ini file to get
 *                the name of the Visa SMS service.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      vsms_host_que_name - (Global) 
 *
 *  RTRN VALUE:   True  - Success
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_visa_debit_routing()
{
   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80];
   INT   ret_val = true;

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*--------------------------------------*/
   /* Get queue name of Vise Debit Service */
   /*--------------------------------------*/
   rc = GetPrivateProfileString(
            "SMS",                         /* points to section name         */
            "VISA_DEBIT_ROUTING",          /* points to key name             */
            "ncvsms",                      /* Default string (ncvsms)        */
             vsms_host_que_name,           /* points to destination buffer   */
             sizeof(vsms_host_que_name)-1, /* size of destination buffer     */
             filename                      /* points to ini filename         */
   );

   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         get_falcon_state_from_ini
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
INT get_falcon_state_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nARState_temp   = 0;
   INT nARState_retval = 1;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FALCON_SECTION",                   /* points to section name       */
             "SEND_TO_FALCON",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFalconSendFlag = atoi(tmpstring);
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


   return(nFalconSendFlag);
}

/******************************************************************************
 *
 *  NAME:         get_fraudguard_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether fraudguard is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Changan Huang
 *
 ******************************************************************************/
INT get_fraudguard_state_from_ini( void )
{
   #define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nFGState_temp   = 0;
   INT nFGState_retval = 0;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /********************************************
    *                                          *
    *  Get Fraudguard ACQUIRING Card Families  *
    *                                          *
    ********************************************/

   /*+------+
     | AMEX |
     +------+*/

   /* See if Fraudguard is turned on for AMEX. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "AMEX",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ACQ_AMEX;

   /*+------+
     | JCB  |
     +------+*/

   /* See if Fraudguard is turned on for JCB. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "JCB",                          /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ACQ_JCB;

   /*+------------+
     | MasterCard |
     +------------+*/

   /* See if Fraudguard is turned on for MC. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "MC",                           /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ACQ_MC;

   /*+------+
     | VISA |
     +------+*/

   /* See if Fraudguard is turned on for VISA. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "VISA",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ACQ_VISA;


   /******************************************
    *                                        *
    *  Get Fraudguard ISSUING Card Families  *
    *                                        *
    ******************************************/

   /*+----------------+
     | AMEX - Issuing |
     +----------------+*/

   /* See if Fraudguard is turned on for AMEX. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "ISS_AMEX",                     /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ISS_AMEX;

   /*+----------------+
     | JCB - Issuing  |
     +----------------+*/

   /* See if Fraudguard is turned on for JCB. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "ISS_JCB",                      /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ISS_JCB;

   /*+----------------------+
     | MasterCard - Issuing |
     +----------------------+*/

   /* See if Fraudguard is turned on for MC. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "ISS_MC",                       /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ISS_MC;

   /*+----------------+
     | VISA - Issuing |
     +----------------+*/

   /* See if Fraudguard is turned on for VISA. */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FRAUDGUARD",                   /* points to section name       */
             "ISS_VISA",                     /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFGState_temp = atoi(tmpstring);
   if ( nFGState_temp == 1 )
      nFGState_retval |= FG_STATE_ISS_VISA;


   return( nFGState_retval );
}
/******************************************************************************
 *
 *  NAME:         get_automatic_resend_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether automatic_resend is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Girija Y - ThoughtFocus 
 *
 ******************************************************************************/
INT get_automatic_resend_state_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nARState_temp   = 0;
   INT nARState_retval = 1;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if Automatic Resend is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "AUTOMATIC_RESEND",                   /* points to section name       */
             "ENABLED",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nARState_temp = atoi(tmpstring);

   return(nARState_temp);
}

INT get_FRT_Decline_RC_Acq_MC( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_MC, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_MC)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_MC,
			   	FRT_DECLINE_RC_ACQ_MC_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_Acq_VISA( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_VISA",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_VISA, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_VISA)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_VISA,
			   	FRT_DECLINE_RC_ACQ_VISA_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_Acq_AMEX( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_AMEX, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_AMEX)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_AMEX,
			   	FRT_DECLINE_RC_ACQ_AMEX_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_Acq_JCB( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_JCB",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_JCB, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_JCB)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_JCB,
			   	FRT_DECLINE_RC_ACQ_JCB_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_Acq_UPI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_UPI",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_UPI, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_UPI)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_UPI,
			   	FRT_DECLINE_RC_ACQ_UPI_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_Acq_DCI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_Acq_DCI, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_Acq_DCI)==0)
   {
	   strncpy( FRT_Decline_RC_Acq_DCI,
			   	FRT_DECLINE_RC_ACQ_DCI_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_MC( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",  /* points to key name           */
             "63",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_MC, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_ISS_MC)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_MC,
			   	FRT_DECLINE_RC_ISS_MC_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_VISA( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_VISA",  /* points to key name           */
             "59",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_VISA, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_ISS_VISA)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_VISA,
			   	FRT_DECLINE_RC_ISS_VISA_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_AMEX( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",  /* points to key name           */
             "107",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_AMEX, tmpstring, FRT_DECLINE_AC_LEN );
   if(strlen(FRT_Decline_RC_ISS_AMEX)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_AMEX,
			   	FRT_DECLINE_RC_ISS_AMEX_DEFAULT,
				FRT_DECLINE_AC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_JCB( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_JCB",  /* points to key name           */
             "59",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_JCB, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_ISS_JCB)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_JCB,
			   	FRT_DECLINE_RC_ISS_JCB_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_UPI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_UPI",  /* points to key name           */
             "59",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_UPI, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_ISS_UPI)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_UPI,
			   	FRT_DECLINE_RC_ISS_UPI_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_ISS_DCI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "SCHEME_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",  /* points to key name           */
             "102",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_ISS_DCI, tmpstring, FRT_DECLINE_AC_LEN );
   if(strlen(FRT_Decline_RC_ISS_DCI)==0)
   {
	   strncpy( FRT_Decline_RC_ISS_DCI,
			   	FRT_DECLINE_RC_ISS_DCI_DEFAULT,
				FRT_DECLINE_AC_LEN);// Default Value
   }
   return( ret_val );
}


INT get_FRT_Decline_RC_VOICE_MC( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_MC, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_MC)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_MC,
			   	FRT_DECLINE_RC_VOICE_MC_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_VOICE_VISA( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_VISA",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_VISA, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_VISA)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_VISA,
			   	FRT_DECLINE_RC_VOICE_VISA_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_VOICE_AMEX( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_AMEX, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_AMEX)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_AMEX,
			   	FRT_DECLINE_RC_VOICE_AMEX_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_VOICE_JCB( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_JCB",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_JCB, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_JCB)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_JCB,
			   	FRT_DECLINE_RC_VOICE_JCB_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_VOICE_UPI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_UPI",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_UPI, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_UPI)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_UPI,
			   	FRT_DECLINE_RC_VOICE_UPI_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_VOICE_DCI( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "VOICE_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_VOICE_DCI, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_VOICE_DCI)==0)
   {
	   strncpy( FRT_Decline_RC_VOICE_DCI,
			   	FRT_DECLINE_RC_VOICE_DCI_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

INT get_FRT_Decline_RC_PLFC( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE_PLFC",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_PLFC, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_PLFC)==0)
   {
	   strncpy( FRT_Decline_RC_PLFC,
			   	FRT_DECLINE_RC_PLFC_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}
void Txcntl_Get_MC_delcine_rc()
{
	   DWORD rc;
	   CHAR filename     [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	   CHAR strTemp[256] ={0};
	   INT  ret_val = true;

	   /* Determine directory and filename of the .ini file. */
	   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   rc = GetPrivateProfileString(
	             "DECLINE_RC_FOR_MC_TXN",     /* points to section name       */
	             "MC_DECLINE_RC_FOR_3DS_2_TXN",  /* points to key name           */
	             "63",                       		/* points to default string     */
	             ini_info,                      /* points to destination buffer */
	             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	             filename                       /* points to ini filename       */
	   );

	   /* Convert entry from a string into a number. */
	   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	   strncpy( tmpstring, ini_info, 2 );
	   strncpy( mc_3ds2_delcine_rc, tmpstring, 2 );
	   if(strlen(mc_3ds2_delcine_rc)==0)
	   {
		   strncpy( mc_3ds2_delcine_rc,DEFAULT_MC_3DS2_DELCINE_RC,2);// Default Value
	   }

	   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   rc = GetPrivateProfileString(
	             "DECLINE_RC_FOR_MC_TXN",     /* points to section name       */
	             "DEFAULT_RC_FOR_ECOM_OBS_TXN",  /* points to key name           */
	             "05",                       		/* points to default string     */
	             ini_info,                      /* points to destination buffer */
	             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	             filename                       /* points to ini filename       */
	   );

	   /* Convert entry from a string into a number. */
	   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	   strncpy( tmpstring, ini_info, 2 );
	   strncpy( mc_default_rc_for_digital_payment_data, tmpstring, 2 );
	   if(strlen(mc_default_rc_for_digital_payment_data)==0)
	   {
		   strncpy( mc_default_rc_for_digital_payment_data,MC_DECLINE_RC,2);// Default Value
	   }

	   sprintf(strTemp,"MC_DECLINE_RC_FOR_3DS_2_TXN (%s) and mc_default_rc_for_digital_payment_data (%s)\n",mc_3ds2_delcine_rc,mc_default_rc_for_digital_payment_data);
	   txcntl_log_message( 1, 1,strTemp, "TxDataServer_Continue_With_Pin_Processing", 1 );
}
void get_FRT_Decline_RC()
{
	 get_FRT_Decline_RC_Acq_MC();
	 get_FRT_Decline_RC_Acq_VISA();
	 get_FRT_Decline_RC_Acq_AMEX();
	 get_FRT_Decline_RC_Acq_JCB();
	 get_FRT_Decline_RC_Acq_UPI();
	 get_FRT_Decline_RC_Acq_DCI();

	 get_FRT_Decline_RC_ISS_MC();
	 get_FRT_Decline_RC_ISS_VISA();
	 get_FRT_Decline_RC_ISS_AMEX();
	 get_FRT_Decline_RC_ISS_JCB();
	 get_FRT_Decline_RC_ISS_UPI();
	 get_FRT_Decline_RC_ISS_DCI();

	 get_FRT_Decline_RC_VOICE_MC();
	 get_FRT_Decline_RC_VOICE_VISA();
	 get_FRT_Decline_RC_VOICE_AMEX();
	 get_FRT_Decline_RC_VOICE_JCB();
	 get_FRT_Decline_RC_VOICE_UPI();
	 get_FRT_Decline_RC_VOICE_DCI();

	 get_FRT_Decline_RC_PLFC();
	 return;
}

void Get_Name_Validation_Parameters_FromTF_INI()
{
	   CHAR  filename     [256]={0};
	   CHAR  tmpstr       [256]={0};

	   GetAscendentConfigDirectory(tmpstr);
	   sprintf(filename, "%stf.ini", tmpstr);

	   GetPrivateProfileString(
	    "NAME_VALIDATION_PARAMETERS",	            /*  points to section name  */
	    "PARTIAL_MATCH_RC",           /*  points to key name  */
	    "00",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   strncpy(partial_match_rc,tmpstr,2);

	   memset(tmpstr,0x00,sizeof(tmpstr));
	   GetPrivateProfileString(
	  	    "NAME_VALIDATION_PARAMETERS",	            /*  points to section name  */
	  	    "NO_MATCH_RC",           /*  points to key name  */
	  	    "63",                      /*  points to default string  */
	  	    tmpstr,                         /*  points to destination buffer  */
	  	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	  	    filename                        /*  points to initialization filename  */
	  	   );

	  	   strncpy(no_match_rc,tmpstr,2);

	  memset(tmpstr,0x00,sizeof(tmpstr));
	   GetPrivateProfileString(
	    "NAME_VALIDATION_PARAMETERS",	            /*  points to section name  */
	    "NAME_VALIDATION_FLAG",           /*  points to key name  */
	    "N",                      /*  points to default string  */
	    tmpstr,                         /*  points to destination buffer  */
	    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
	    filename                        /*  points to initialization filename  */
	   );

	   name_validation_flag=tmpstr[0];

}

void get_keyblock_flag_details( void )
{
	DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );



	/********** MC KeyBlock Value***************/
	keyblock_enable_flag_for_MC = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     	/* points to section name       */
								  "KEY_BLOCK_FLAG_VALUE_MC",  	/* points to key name           */
								  "0",                        	/* points to default string     */
								  ini_info,                   	/* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      	/* size of destination buffer   */
								  filename                    	/* points to ini filename       */
	);

	keyblock_enable_flag_for_MC = atoi(ini_info);


	/********** VISA KeyBlock Value***************/
	keyblock_enable_flag_for_VISA = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     	/* points to section name       */
							 	  "KEY_BLOCK_FLAG_VALUE_VISA",  /* points to key name           */
								  "0",                       	/* points to default string     */
								  ini_info,                   	/* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      	/* size of destination buffer   */
								  filename                    	/* points to ini filename       */
	);

	keyblock_enable_flag_for_VISA = atoi(ini_info);

	/********** AMEX KeyBlock Value***************/
	keyblock_enable_flag_for_AMEX = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     	/* points to section name       */
								  "KEY_BLOCK_FLAG_VALUE_AMEX",  	/* points to key name           */
								  "0",                        	/* points to default string     */
								  ini_info,                   	/* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      	/* size of destination buffer   */
								  filename                    	/* points to ini filename       */
	);

	keyblock_enable_flag_for_AMEX = atoi(ini_info);

	/********** JCB KeyBlock Value***************/
	keyblock_enable_flag_for_JCB = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     	/* points to section name       */
								  "KEY_BLOCK_FLAG_VALUE_JCB",  	/* points to key name           */
								  "0",                        	/* points to default string     */
								  ini_info,                   	/* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      	/* size of destination buffer   */
								  filename                    	/* points to ini filename       */
	 );

	keyblock_enable_flag_for_JCB = atoi(ini_info);

	 /********** UPI KeyBlock Value***************/
	keyblock_enable_flag_for_UPI = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     /* points to section name       */
							 	  "KEY_BLOCK_FLAG_VALUE_UPI",  /* points to key name           */
								  "0",                        /* points to default string     */
								  ini_info,                   /* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      /* size of destination buffer   */
								  filename                    /* points to ini filename       */
	);

	keyblock_enable_flag_for_UPI = atoi(ini_info);

	/********** DCI KeyBlock Value***************/
	keyblock_enable_flag_for_DCI = KEYBLOCK_DISABLE;

	rc = GetPrivateProfileString( "KEY_BLOCK_FLAG_VALUE",     /* points to section name       */
							 	  "KEY_BLOCK_FLAG_VALUE_DCI",  /* points to key name           */
								  "0",                        /* points to default string     */
								  ini_info,                   /* points to destination buffer */
								  MAX_INI_INFO_SIZE - 1,      /* size of destination buffer   */
								  filename                    /* points to ini filename       */
	);

	keyblock_enable_flag_for_DCI = atoi(ini_info);

}

void set_keyblock_flag_details( pCHAR flag )
{
	DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetAscendentConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	/********** MC KeyBlock Value***************/

	 /*rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",
		  "KEY_BLOCK_FLAG_VALUE_MC",
		  flag,
		  filename
	 );*/

	/********** VISA KeyBlock Value***************/

	/*rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",
		  "KEY_BLOCK_FLAG_VALUE_VISA",
		  flag,
		  filename
);*/


	/********** AMEX KeyBlock Value***************/

	/*rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",
		  "KEY_BLOCK_FLAG_VALUE_AMEX",
		  flag,
		  filename
);*/


	/********** JCB KeyBlock Value***************/

	/*rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",
		  "KEY_BLOCK_FLAG_VALUE_JCB",
		  flag,
		  filename
);*/


	 /********** UPI KeyBlock Value***************/

	/*rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",
		  "KEY_BLOCK_FLAG_VALUE_UPI",
		  flag,
		  filename
);*/


	/********** DCI KeyBlock Value***************/

	rc = WritePrivateProfileString(
		  "KEY_BLOCK_FLAG_VALUE",	/* pointer to section name*/
		  "KEY_BLOCK_FLAG_VALUE_DCI",	/* pointer to key name*/
		  flag,							/* pointer to string to add*/
		  filename						/* pointer to initialization filename*/
);

	 sprintf( tmpstr, "Keyblock Flag value changed for processing Transactions:%s",flag );
	 txcntl_log_message( 1, 1,tmpstr, "PinnacleMsgHandler", 0 );
}
