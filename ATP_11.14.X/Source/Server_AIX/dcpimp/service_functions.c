/**************************************************************************************************
*  
* MODULE:      service_functions.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: General routines
*
* APPLICATION: DCPISO
*
* AUTHOR:  Sanjoy Dasgupta
*
* 
      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "pteipc.h"
#include "global_definitions.h"  
#include "txutils.h"
#include "ntutils.h"
#include "genutil.h"
#include "process_incoming_msg.h"
#include "process_outgoing_msg.h"
#include "database_request.h"
#include "general_utilities.h"
#include "service_functions.h"
#include "diners.h"
#include "app_info.h"


extern AUTH_TX auth_tx;				

extern CHAR     ncvisa_que_name [];
extern CHAR	   visa1_que_name[];
extern CHAR	   authorizer_que_name[];
extern CHAR     nciftr_que_name [];
extern CHAR     nciftr_module_name [];
extern CHAR     faisnd_que_name[];
#ifdef FALCON_CONTROLLER
extern CHAR 	falcntl_que_name[];
#endif
CHAR fconds_que_name[]= "fcondsA";

extern BYTE		ReqRsp_Buffer[]; 
extern INT		buflen;
extern INT        nFalconSendFlag;
extern PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
CHAR	ServiceCode[4];
extern CHAR CUP_merchant_indicator[];
extern CHAR  CUP_MP_Bin_range_1[11];
extern CHAR  CUP_MP_Bin_range_2[11];
extern CHAR  CUP_MP_Bin_range_3[11];
extern CHAR  CUP_MP_Bin_range_4[11];
extern CHAR  CUP_MP_Bin_range_5[11];
extern INT   CUP_MP_Bin_range_len_1;
extern INT   CUP_MP_Bin_range_len_2;
extern INT   CUP_MP_Bin_range_len_3;
extern INT   CUP_MP_Bin_range_len_4;
extern INT   CUP_MP_Bin_range_len_5;
extern CHAR  dcpimp_Erro_warning_Filename[256] ;
extern CHAR  dcpimp_module_error_warning_file_name[256];
extern CHAR  dcpimp_error_warning_file_path[256];
BYTE p_buffer[MAX_DATASERVER_BUFFER_LENGTH] ;

UINT Max_File_Size_Defined = 0 ;

FRULE01_GUI_LIST amex_frual01;
FRULE01_GUI_LIST dci_frual01;
FRULE01_GUI_LIST cup_frual01;
FRULE01_GUI_LIST jcb_frual01;
FRULE01_GUI_LIST mc_frual01;
FRULE01_GUI_LIST visa_frual01;
FRULE01_GUI_LIST pl_frual01;

FCLF01 amex_fclf01;
FCLF01 dci_fclf01;
FCLF01 cup_fclf01;
FCLF01 jcb_fclf01;
FCLF01 mc_fclf01;
FCLF01 visa_fclf01;
FCLF01 iftr_fclf01;

BYTE Check_Transaction_Tx_Key (void)
{
   BYTE tx_key;
   BYTE retCode = DEVICE_OK;
   CHAR strError[512];

   tx_key = auth_tx.TLF01_details.tx_key ;
   if ( tx_key <= AUTH_INVALID )
   {
      memset( strError, 0x00, sizeof(strError) );
      sprintf( strError,
              "Invalid tx key %d received from Transaction Controller\n",
               auth_tx.TLF01_details.tx_key) ;

      retCode = Generate_Usage_Error_Message (1, strError,
                                             "Check_Transaction_Tx_Key", 0,
                                              INVALID_TRANSACTION );
   }
   return( retCode );
}

BYTE Check_If_Transaction_Has_Valid_Response_Code (void)
{
	char strResponseCode[10] = {0} ;
	strcpy (strResponseCode, auth_tx.TLF01_details.response_code) ;

	if (strEqual (strResponseCode, TRANSACTION_APPROVED) ||
		strEqual (strResponseCode, PIN_TRIES_EXCEEDED) ||
		strEqual (strResponseCode, INITIATE_BATCH_UPLOAD) ||
		strEqual (strResponseCode, INVALID_AMOUNT)|| 
		strEqual (strResponseCode, INCORRECT_PIN)||
		strEqual (strResponseCode, REFER_TO_CARD_ISSUER) ||
		strEqual (strResponseCode, TIME_OUT))
	{
		return DEVICE_OK ;
	}
	return DEVICE_ERROR ;
}

void Map_Response_Code( pBYTE resp_code )
{
   /* This mapping is necessary for the terminal because
    * some of these responses are not approvals, but they
    * print a sales slip on the terminal.  Since the terminal
    * prints a sales slip, the merchants mistake it as an
    * approval transaction and will release the goods to
    * the customer.
    */

   #define  NUM_RC  53
   INT   i;
   INT   rc_found;
   BYTE  valid_rc[NUM_RC][3] =
      {
         "00","01","02","03","04","05","06","07","08","09",
		 "10","12","13","14","19","21","25","30","31","41",
		 "43","46","51","52","53","54","55","56","57","58",
		 "61","62","63","65","70","71","75","76","77","78",
		 "79","80","81","82","83","84","85","86","89","91",
		 "94","95","96"
      };
       

   if ( 0 == strcmp(resp_code, "21") )
   {
      strcpy( resp_code, REENTER_TRANSACTION );
   }
   else if ( 0 == strcmp(resp_code, "70") )
   {
      strcpy( resp_code, REENTER_TRANSACTION );
   }   
   else if ( (0 == strcmp(resp_code, "71")) ||
			(0 == strcmp(resp_code, "86"))  )
   {
      strcpy( resp_code, "05" );
   }
   else if ( (0 == strcmp(resp_code, "77")) ||
             (0 == strcmp(resp_code, "88")) ||
             (0 == strcmp(resp_code, "NC")) ||
             (0 == strcmp(resp_code, "Z1"))  )
   {
      strcpy( resp_code, SYSTEM_MALFUNCTION );
   }
   else
   {
      for( i=0,rc_found=false; i<NUM_RC; i++ )
      {
         if ( 0 == strcmp(resp_code,valid_rc[i]) )
         {
            rc_found = true;
            break;
         }
      }
      if ( rc_found == false )
         strcpy( resp_code, SYSTEM_MALFUNCTION );
   }
   return;
}

BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code)
{
    if (strEqual (auth_tx.TLF01_details.handler_queue, "VOICE"))
        strcpy (auth_tx.TLF01_details.voice_auth_text, strTemp) ;

    strncpy(auth_tx.TLF01_details.response_text,strTemp,
        		(sizeof(auth_tx.TLF01_details.response_text)-1));

	dcpimp_log_message( 1, 2, strTemp,function, 1 );
	Prepare_AuthTx_For_Forwarding (general_status,
								   response_code) ;	
				
	return DEVICE_ERROR ;
}

void Get_Bin_Type_From_Processing_Code (BYTE* pnAccountType)
{
	/* third digit of the processing code determines the bin type */
	if (auth_tx.TLF01_details.processing_code[2] == '0' || 
		auth_tx.TLF01_details.processing_code[2] == '3' ||
		auth_tx.TLF01_details.processing_code[2] == '4') /* universal type treated as CREDIT */
		*pnAccountType = 'C' ;
	else 
		*pnAccountType = 'D' ;

} /* Get_Bin_Type_From_Processing_Code */


void Prepare_AuthTx_For_Forwarding (BYTE general_status, BYTE* strResponseCode) 
{
	strcpy (auth_tx.TLF01_details.response_code, strResponseCode) ; 
	auth_tx.TLF01_details.general_status = general_status ;
    
} /* Prepare_AuthTx_For_Terminal_Handler_Response */

BYTE Perform_Auth_Tx_Checks (void)
{
	BYTE retCode = 0 ;
	char strError[1024] = {0} ;

	if (auth_tx.TLF01_details.tx_key == AUTH_INVALID)
	{
		sprintf (strError, "Invalid transaction %d - invalid tx key", auth_tx.TLF01_details.tx_key) ;
	}
	else if (!IsFieldNull (auth_tx.TLF01_details.card_num) && !IsFieldNumeric (auth_tx.TLF01_details.card_num))
	{
			sprintf (strError, "Invalid card number %s - not numeric", auth_tx.TLF01_details.card_num ) ;
	}
	else if (!IsFieldNull (auth_tx.TLF01_details.total_amount) && !IsFieldNumeric (auth_tx.TLF01_details.total_amount))
	{
		sprintf (strError, "Amount field %s in transaction is not numeric", auth_tx.TLF01_details.total_amount) ;
	}
	else if (IsFieldNull (auth_tx.TLF01_details.originator_queue))
	{
		strcpy (strError, "Originator que is not populated in transaction") ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.terminal_id, 
														sizeof auth_tx.TLF01_details.terminal_id))
	{
		sprintf (strError, "Invalid terminal id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.terminal_id,
							strlen (auth_tx.TLF01_details.terminal_id), 
							sizeof auth_tx.TLF01_details.terminal_id) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num,
														sizeof auth_tx.TLF01_details.card_num))
	{
		sprintf (strError, "Invalid card number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num,
							strlen (auth_tx.TLF01_details.card_num), 
							sizeof auth_tx.TLF01_details.card_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.sys_trace_audit_num,
														sizeof auth_tx.TLF01_details.sys_trace_audit_num))
	{
		sprintf (strError, "Invalid sys_trace_audit_num %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.sys_trace_audit_num,				
							strlen (auth_tx.TLF01_details.sys_trace_audit_num), 
							sizeof auth_tx.TLF01_details.sys_trace_audit_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.primary_key.transaction_id,
														sizeof auth_tx.TLF01_details.primary_key.transaction_id))
	{
		sprintf (strError, "Invalid transaction_id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.primary_key.transaction_id,
							strlen (auth_tx.TLF01_details.primary_key.transaction_id), 
							sizeof auth_tx.TLF01_details.primary_key.transaction_id) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.retrieval_ref_num,
														sizeof auth_tx.TLF01_details.retrieval_ref_num))
	{
		sprintf (strError, "Invalid calculated retrieval_ref_num %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.retrieval_ref_num,
							strlen (auth_tx.TLF01_details.retrieval_ref_num), 
							sizeof auth_tx.TLF01_details.retrieval_ref_num) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.message_type,
														sizeof auth_tx.TLF01_details.message_type))
	{
		sprintf (strError, "Invalid message_type %s length is %d, datastructure length is %d",
							auth_tx.TLF01_details.message_type,
							strlen (auth_tx.TLF01_details.message_type), 
							sizeof auth_tx.TLF01_details.message_type) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.processing_code,
														sizeof auth_tx.TLF01_details.processing_code))
	{
		sprintf (strError, "Invalid processing_code %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.processing_code,
							strlen (auth_tx.TLF01_details.processing_code), 
							sizeof auth_tx.TLF01_details.processing_code) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.date_yyyymmdd,
														sizeof auth_tx.TLF01_details.date_yyyymmdd))
	{
		sprintf (strError, "Invalid date_yyyymmdd %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.date_yyyymmdd,
							strlen (auth_tx.TLF01_details.date_yyyymmdd), 
							sizeof auth_tx.TLF01_details.date_yyyymmdd) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.total_amount,
														sizeof auth_tx.TLF01_details.total_amount))
	{
		sprintf (strError, "Invalid total_amount %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.total_amount,
							strlen (auth_tx.TLF01_details.total_amount), 
							sizeof auth_tx.TLF01_details.total_amount) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pos_entry_mode,
														sizeof auth_tx.TLF01_details.pos_entry_mode))
	{
		sprintf (strError, "Invalid pos_entry_mode %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pos_entry_mode,
							strlen (auth_tx.TLF01_details.pos_entry_mode), 
							sizeof auth_tx.TLF01_details.pos_entry_mode) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.nii,
														sizeof auth_tx.TLF01_details.nii))
	{
		sprintf (strError, "Invalid nii %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.nii,
							strlen (auth_tx.TLF01_details.nii), 
							sizeof auth_tx.TLF01_details.nii) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num_len,
														sizeof auth_tx.TLF01_details.card_num_len))
	{
		sprintf (strError, "Invalid card_num_len %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num_len,
							strlen (auth_tx.TLF01_details.card_num_len), 
							sizeof auth_tx.TLF01_details.card_num_len) ;
	}
	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.merchant_id,
														sizeof auth_tx.TLF01_details.merchant_id))
	{
		sprintf (strError, "Invalid merchant_id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.merchant_id,
							strlen (auth_tx.TLF01_details.merchant_id), 
							sizeof auth_tx.TLF01_details.merchant_id) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.handler_queue, 
														sizeof auth_tx.TLF01_details.handler_queue))
	{
		sprintf (strError, "Invalid handler que %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.handler_queue,
							strlen (auth_tx.TLF01_details.handler_queue), 
							sizeof auth_tx.TLF01_details.handler_queue) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.card_num_len,
														sizeof auth_tx.TLF01_details.card_num_len))
	{
		sprintf (strError, "Invalid card num length %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.card_num_len,
							strlen (auth_tx.TLF01_details.card_num_len), 
							sizeof auth_tx.TLF01_details.card_num_len) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pos_condition_code,
														sizeof auth_tx.TLF01_details.pos_condition_code))
	{
		sprintf (strError, "Invalid pos_condition_code %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pos_condition_code,
							strlen (auth_tx.TLF01_details.pos_condition_code), 
							sizeof auth_tx.TLF01_details.pos_condition_code) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.exp_date,
														sizeof auth_tx.TLF01_details.exp_date))
	{
		sprintf (strError, "Invalid expiry date %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.exp_date,
							strlen (auth_tx.TLF01_details.exp_date), 
							sizeof auth_tx.TLF01_details.exp_date) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.track1,
														sizeof auth_tx.TLF01_details.track1))
	{
		sprintf (strError, "Invalid track1 %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.track1,
							strlen (auth_tx.TLF01_details.track1), 
							sizeof auth_tx.TLF01_details.track1) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.exp_date,
														sizeof auth_tx.TLF01_details.exp_date))
	{
		sprintf (strError, "Invalid expiry date %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.exp_date,
							strlen (auth_tx.TLF01_details.exp_date), 
							sizeof auth_tx.TLF01_details.exp_date) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.pin_block,
														sizeof auth_tx.TLF01_details.pin_block))
	{
		sprintf (strError, "Invalid pin block %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.pin_block,
							strlen (auth_tx.TLF01_details.pin_block), 
							sizeof auth_tx.TLF01_details.pin_block) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.orig_retrieval_ref_num,
														sizeof auth_tx.TLF01_details.orig_retrieval_ref_num))
	{
		sprintf (strError, "Invalid transaction retrieval reference number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.orig_retrieval_ref_num,
							strlen (auth_tx.TLF01_details.orig_retrieval_ref_num), 
							sizeof auth_tx.TLF01_details.orig_retrieval_ref_num) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.auth_number,
														sizeof auth_tx.TLF01_details.auth_number))
	{
		sprintf (strError, "Invalid auth_number %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.auth_number,
							strlen (auth_tx.TLF01_details.auth_number), 
							sizeof auth_tx.TLF01_details.auth_number) ;
	}

	else if (IsFieldLengthGreaterThanDatabaseLength (	auth_tx.TLF01_details.visa_merchant_id,
														sizeof auth_tx.TLF01_details.visa_merchant_id))
	{
		sprintf (strError, "Invalid visa merchant id %s, length is %d, datastructure length is %d",
							auth_tx.TLF01_details.visa_merchant_id,
							strlen (auth_tx.TLF01_details.visa_merchant_id), 
							sizeof auth_tx.TLF01_details.visa_merchant_id) ;
	}

	

	if (!IsFieldNull (strError))
	{
		retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, 
											strError,
											"Perform_Auth_Tx_Checks",
											GS_TXN_SYSTEM_ERROR,
											INVALID_TRANSACTION) ;
		return retCode ;
	}

	return DEVICE_OK ;
} /* Perform_Auth_Tx_Checks */

BYTE Send_Transaction_To_Authorizer (void)
{
	BYTE ReturnCode = 0 ;
	CHAR strTemp[100]={0};
	CHAR  gmt_time[20]={0};

    if(0 == strncmp(auth_tx.TLF01_details.product_codes[2].quantity,"1",1))
    {
   	
   	  if ( true == Check_Is_Host_Up(nciftr_module_name))
   	  {	
   		 /*ncitr host is online*/
		 ReturnCode = Send_Request_To_Service(nciftr_que_name, 
								  			  MT_AUTH_REQUEST, 
								  			  0, 
											  0,
											  0,
											  0,
											  0) ;
	    if (ReturnCode == false)
	    {
	 	    sprintf(strTemp,"error in sending data to nciftr host\n") ;
     	    dcpimp_log_message( 1, 3, strTemp," Send_Transaction_To_Authorizer ", 1 );
			return ReturnCode ;
        }
	    else
	    {
			return ReturnCode ;
	    }
   	 }
	 else
	 {
	 	// Check if it is reversal message , then drop it , we will response it.
	 	if (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL)
	 	{
			return DEVICE_OK;
	 	}
		
		strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
		sprintf(strTemp,"In standin processing, network_controller: nciftr") ;
     	dcpimp_log_message( 1, 2, strTemp," Send_Transaction_To_Authorizer ", 1 );

		//Copy last 6 digit of transaction id as Authid
		// We are not storing . from transaction ID
		strncpy(auth_tx.TLF01_details.auth_number, auth_tx.TLF01_details.primary_key.transaction_id +13 , 3);
		strncat(auth_tx.TLF01_details.auth_number,auth_tx.TLF01_details.primary_key.transaction_id +17,3);
		if ( 0 == strcmp( NULL_STR,auth_tx.TLF01_details.transmission_timestamp ) )
		{
			pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
			strncpy( auth_tx.TLF01_details.transmission_timestamp, &gmt_time[4], 10 );  /* MMDDhhmmss */
			
		}
#ifdef HCTS3
		if(auth_tx.BIN01_details.primary_key.low[0] != '\0' &&
		    auth_tx.BIN01_details.caf_profile_id[0] != '\0' &&
		          auth_tx.BIN01_details.auth_ind[0] != '\0')
		{
			//BIN record already fetched and verified the auth profile details, it is Onus
			ReturnCode = Process_When_Bin_Range_Found();
		}
		else
#endif
		{
			// check if the CARD TYPE of the PAN is among those allowed in the CARD TYPE of the Merchant/MID.
			ReturnCode = Retrieve_BIN01_Record ();
		}
		//Get_MGF01_Details();
		
		return ReturnCode;
	 }
    }
	else
	{
	  	
   
		ReturnCode = Send_Request_To_Service (  authorizer_que_name,
												MT_AUTH_REQUEST, 
												0, 
												0, 
												0, 
												0, 
												0) ;
	 }
	return ReturnCode ;
}


BYTE If_Transaction_Cannot_Be_Declined (void)
{
   BYTE  ret_val = 0;

   if ( If_Transaction_Is_A_Advice() )
      ret_val = 1;
   else
   {
      if ( CREDIT_BIN == Get_Bin_Type(auth_tx.TLF01_details.processing_code) )
      {
         if ( (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL) ||
              (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE) )
            ret_val = 1;
      }
   }
   return( ret_val );
} /* If_Transaction_Cannot_Be_Declined */

BYTE If_Transaction_Is_A_Advice (void)
{
    char strMessageType [20] = {0} ;
    char strFirstTwoBytesOfMessageType [20] = {0} ;
	char strLastTwoBytesOfMessageType [20] = {0} ;
	
    strcpy (strMessageType, auth_tx.TLF01_details.message_type) ;
	strncpy (strFirstTwoBytesOfMessageType, strMessageType, 2) ;
	strncpy (strLastTwoBytesOfMessageType, strMessageType+2, 2) ;

    /* Non Reversal Advices eg Offline Sale or Auth_Advice or Auth Advice Repeat */
	if (strEqual (strLastTwoBytesOfMessageType, "20") ||
		 strEqual (strLastTwoBytesOfMessageType, "21") ||
		 strEqual (strLastTwoBytesOfMessageType, "01")) 
	{
        return 1 ;
	}
	else
	{
		return 0 ;
	}
} /* If_Transaction_Is_A_Advice */

BYTE IsFieldLengthGreaterThanDatabaseLength (unsigned char field[], unsigned int field_datastructure_length)
{
	/* We subtract 1 from <field_datastructure_length> because the datastructure field is always defined to
	   be 1 more than the actual field length (to accomodate the NULL character at the end 
	*/
	if (strlen (field) > (field_datastructure_length - 1))
	{
		return 1 ;
	}
	return 0 ;
		
} /* IsFieldLengthGreaterThanDatabaseLength */

BYTE PopulateAuthTxTimeField (void)
{
	
	CHAR temp[50] = {0} ;

	genutil_format_date (temp) ;
	
	/* create time of transaction and store it in the auth_tx table */
	memcpy(auth_tx.TLF01_details.time_hhmmss, temp + 8, 6) ;

	return 1 ;
} /* CalculateTransactionTimeField */  

BYTE PopulateAuthTxDateField (void)
{
	CHAR temp[50] = {0} ;

	genutil_format_date (temp) ;
	
	
	/* create date of transaction and store it in the auth_tx table */
	memcpy (auth_tx.TLF01_details.date_yyyymmdd, temp, 8) ;

	return 1 ;
} /* CalculateTransactionDateField */  

BYTE Get_Bin_Type( pBYTE proc_code )
{
   BYTE  bin_type;

   /* Third digit of the processing code determines the bin type */
	if ((proc_code[2] == '0') || (proc_code[2] == '3') || (proc_code[2] == '4'))
		bin_type = CREDIT_BIN;
	else 
		bin_type = DEBIT_BIN;

   return( bin_type );

} /* Get_Bin_Type_From_Processing_Code */


BYTE txn_is_financial()
{
   BYTE  ret_val;

   switch( auth_tx.TLF01_details.tx_key )
   {
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE:
      case AUTH_AUTHORIZATION_RESPONSE:
      case AUTH_PRE_AUTHORIZATION_RESPONSE:
      case AUTH_CARD_VERIFICATION_RESPONSE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:		
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
      case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE	:	            

      case AUTH_SALE_RESPONSE:
      case AUTH_CASH_ADVANCE_RESPONSE:
      case AUTH_SALES_COMPLETION_RESPONSE:
      case AUTH_OFFLINE_SALE_RESPONSE:
      case AUTH_QUASI_CASH_RESPONSE:

      case AUTH_REFUND_RESPONSE:
      case AUTH_OFFLINE_REFUND_RESPONSE:           

      case AUTH_VOID_SALE_RESPONSE:
      case AUTH_VOID_REFUND_RESPONSE:
      case AUTH_VOID_CASH_ADVANCE_RESPONSE:
      case AUTH_OFFLINE_VOID_SALE_RESPONSE:
      case AUTH_OFFLINE_VOID_REFUND_RESPONSE:

      case AUTH_SALE_ADJUSTMENT_RESPONSE: 
      case AUTH_REFUND_ADJUSTMENT_RESPONSE: 

      case AUTH_REVERSAL_RESPONSE:

      /* These reload txns are financial (prepaid).  Reload is listed
       * here so the batch tables get updated.  Reload confirm is listed
       * here to update the card holder account balances.  Reload voids
       * are listed here to update account balances and batch tables.
       */
      case AUTH_RELOAD_RESPONSE:
      case AUTH_RELOAD_CONFIRM_RESPONSE:
      case AUTH_VOID_RELOAD_RESPONSE:
               ret_val = true;
               break;

      default: ret_val = false;
               break;
   }
   return( ret_val );
}

BYTE send_0302_to_network_controller( pPTE_MSG p_msg )
{
   INT       len;
   BYTE      retval              = DEVICE_OK;
   pPTE_MSG  p_msg_out           = NULL_PTR;
   LONG      retCode             = 0L;
   CHAR      strError[512]       = "";
   BYTE      originator_queue[17]= "";
   BYTE      originator_info[33] = "";

   strcpy( originator_queue, ptemsg_get_msg_orig_queue(p_msg) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info(p_msg) );

   /* Append originator Q to end of ReqRsp_Buffer: Tag, Length, Data. */
   len = strlen( ORIG_QUEUE );
   memcpy( ReqRsp_Buffer+buflen, ORIG_QUEUE, len );
   buflen += len;

   len = strlen( originator_queue );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_queue, len );
   buflen += len;

   /* Append originator Info to end of ReqRsp_Buffer: Tag, Length, Data. */
   len = strlen( ORIG_INFO );
   memcpy( ReqRsp_Buffer+buflen, ORIG_INFO, len );
   buflen += len;

   len = strlen( originator_info );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_info, len );
   buflen += len;

   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 ncvisa_que_name,
                                 application_que_name,
                                 ReqRsp_Buffer,
                                 buflen,
                                 0 );

   if ( p_msg_out == NULL_PTR )
   {
      sprintf( strError,
              "Insufficient Memory to build ncvisa request message. Size: %d",
               buflen );
      dcpimp_log_message( 1, 2, strError,"send_0302_to_network_controller", 0 );
      retval = DEVICE_ERROR;
   }
   else
   {
      ptemsg_set_orig_comms_info( p_msg_out, originator_info );
      retCode = pteipc_send( p_msg_out, ncvisa_que_name );
      free (p_msg_out);

      if ( retCode < 0 )
      {
         pteipc_get_errormsg (retCode, strError);
         sprintf( strError,
                 "Error on pteipc_send to que %s: %s",
                  originator_queue, strError );
         dcpimp_log_message( 1, 2, strError,"send_0302_to_network_controller", 0 );
         retval = DEVICE_ERROR;
      }
   }
   return( retval );
} /* send_0302_to_network_controller */

void process_0312( pPTE_MSG p_msg_in )
{
   INT             i;
   INT             len;
   INT             temp_buflen;
   pBYTE           p_data     = NULL_PTR;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   pCHAR           p_rec      = NULL_PTR;
   BYTE            retCode    = 0;
   LONG            dataLength = 0L;
   pBYTE           orig_dialog      = NULL_PTR;
   pBYTE           orig_dialog_info = NULL_PTR;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );
   p_rec      = (pCHAR)p_data;

   dataLength = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   retCode    = Copy_Message_Data_To_Global_Buffer( p_rec, dataLength );

   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
   strcpy( auth_tx.TLF01_details.originator_queue, ptemsg_get_msg_orig_queue(p_msg_in) );
   strcpy( auth_tx.TLF01_details.originator_info,  ptemsg_get_msg_orig_comms_info(p_msg_in) );

   /* Find the original dialog queue. It is appended to ReqRsp_Buffer. */
   orig_dialog = ReqRsp_Buffer;
   len = strlen( ORIG_QUEUE );
   for ( i=0; i<buflen; i++ )
   {
      if ( 0 == strncmp(orig_dialog,ORIG_QUEUE,len) )
         break;
      else
         orig_dialog++;
   }
   temp_buflen = orig_dialog - ReqRsp_Buffer; // Len without orig info & Q.
   orig_dialog += len;
   len = orig_dialog[0];
   if ( len < sizeof( auth_tx.TLF01_details.originator_queue ) )
      memcpy( auth_tx.TLF01_details.originator_queue, orig_dialog+1, len );

   /* Find the original dialog info. It is appended to ReqRsp_Buffer. */
   orig_dialog_info = orig_dialog;
   len = strlen( ORIG_INFO );
   for ( i=0; i<20; i++ )
   {
      /* Orig Info should immediately follow orig Q,
       * so just check like the first 20 bytes after orig Q.
       */
      if ( 0 == strncmp(orig_dialog_info,ORIG_INFO,len) )
         break;
      else
         orig_dialog_info++;
   }
   orig_dialog_info += strlen( ORIG_INFO );
   len = orig_dialog_info[0];
   if ( len < sizeof( auth_tx.TLF01_details.originator_info ) )
      strcpy( auth_tx.TLF01_details.originator_info, orig_dialog_info+1 );

   /* The network controller logs txn into TLF01.  No need to do it here. */
   buflen = temp_buflen;
   (void)Send_Response_To_Terminal();
   return;
}


/* SCR 22830 - ThoughtFocus 
 * This function checks if the request is from a terminal or others.
 * Currently only e-commerce transactions gets checked (pos_condition_code=59),
 * every other transactions is considered to be from the terminal.
 */

BOOLEAN IsOriginatorATerminal( pBYTE pos_condition_code )
{
   INT     ipos_condition_code = 0;
   BOOLEAN retval = true;

   ipos_condition_code = atoi(pos_condition_code);

   switch (ipos_condition_code)
   {
      case 59:  retval = false; break;
      default:  break;
   }
   return retval;
}



/***********************************************************************

NAME:        Validate_Expiry_Date

DESCRIPTION:  Determines if a date is EXPIRED, VALID or INVALID (20/20 format)
              The algorithm uses a +/- 20 factor
              Any date less than 20 years before the system date or 
              more than 20 years from the system date is treated as INVALID

              The maximum allowed card date is (system year + 20) and
              card month = system month

INPUTS:       strExpiryDate - Pointer to the date string in the format "YYMM"

OUTPUTS:      None

RETURNS:      false  if the date is EXPIRED or INVALID
              true   if date is VALID

AUTHOR:       Abhihek Kumar Verma

*************************************************************************/
BYTE Validate_Expiry_Date (BYTE* strExpiryDate)
{
   /* The Expiry date has to be in the format YYMM */
   BYTE strSystemDateTime[50] = {0} ;
   BYTE strTemp[256] = {0} ;
   BYTE ValidExpiryDate = true ;
   BYTE ReturnCode = 0 ;

   INT nSystemYY = 0 ;        // 00 to 99
   INT nSystemMM = 0 ;        // 00 to 12
   INT nCardYY   = 0 ;        // 00 to 99
   INT nCardMM   = 0 ;        // 00 to 12

   INT nCardRangeLowYY  = 0 ;
   INT nCardRangeHighYY = 0 ;
	

   if (strlen(strExpiryDate) == 0)
   {
      strcpy (strTemp, "Invalid Expiry Date-Date cannot be NULL") ;
	  dcpimp_log_message( 2, 2, strTemp," Validate_Expiry_Date ", 1 );
      return ReturnCode ;
   }

   /* Get the System Date in the format yyyymmdd */
   genutil_format_date (strSystemDateTime) ;

   /* Get the month portion of the expiry date (yymm) */
   nCardMM = atoi(strExpiryDate + 2) ;

   /* Get the year portion of the expiry date (yymm) */
   memcpy (strTemp, strExpiryDate, 2) ; 
   nCardYY = atoi(strTemp) ;

   /* Get the year portion of the expiry date (yyyymmdd) */
   /* If the current year is 1999, nSystemYY will be 99 and if 2000, nSystemYY will be 0 */
   memcpy (strTemp, strSystemDateTime, 4) ;
   nSystemYY = atoi(strTemp) % 100 ;

   /* Get the month portion of the expiry date (yyyymmdd) */
   memset (strTemp, 0, sizeof strTemp) ;
   memcpy (strTemp, strSystemDateTime+4, 2) ;
   nSystemMM = atoi(strTemp) ;

   /* System Date between 2000 and 2020 */
   if (nSystemYY >= 0 && nSystemYY <= 20)
      nSystemYY += 100 ;

   if (nSystemYY >= 80 && nSystemYY <= 120 && nCardYY >= 0 && nCardYY <= 40)
      nCardYY += 100 ;

   /* Define the boundary for the +/- 20 year check */
   nCardRangeLowYY = nSystemYY - 20 ;
   nCardRangeHighYY = nSystemYY + 20 ;

   /* Date is invalid if date on card is less than 20 years from system date
      or more than 20 years from the system date */
   if (nCardYY < nCardRangeLowYY || nCardYY > nCardRangeHighYY)
   {
      ValidExpiryDate = false ; /* Outside the window - INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeLowYY && (nCardMM < nSystemMM))
   {
      ValidExpiryDate = false ; /* INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeHighYY && (nCardMM > nSystemMM))
   {
      /* card month is greater than system month */
      ValidExpiryDate = false ; /* INVALID CARD */
   }

   /* All cards within the +/- 20 year boundary and with years less than the system
   date are termed EXPIRED (these are not INVALID cards). For our purposes all INVALID and
   EXPIRED cards are treated as INVALID */

   if (ValidExpiryDate) /* At this time, all cards are within the +/- 20 year boundary */
   {
      if (nCardYY < nSystemYY)
      {
         ValidExpiryDate = false ; /* EXPIRED CARD */
      }
      else if (nCardYY == nSystemYY && (nCardMM < nSystemMM))
      {
         ValidExpiryDate = false ; /* EXPIRED CARD */
      }
   }

   if (ValidExpiryDate == false)
   {
      sprintf( strTemp, "Invalid Expiry Date (%s)", strExpiryDate );
	  dcpimp_log_message( 2, 2, strTemp," Validate_Expiry_Date ", 1 );
      return ReturnCode ;
   }
   /* At this stage, all cards are valid */
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Validate_Expiry_Date */


BYTE If_Transaction_Occurred_On_Same_Day (void)
{
    char strSystemDateTime[50] = {0} ;
    char strCurrentDate[50] = {0} ;
    LONG CurrentDate = 0L ;
    LONG DateWhenAPesoTransactionLastOccurred = 0L ;
    LONG DateWhenADollarTransactionLastOccurred = 0L ;
    LONG DateWhenATransactionLastOccurred = 0L ;
    LONG DaysElapsedSinceATransactionLastOccurred = 0L ;

    /* Get date in yyyymmdd format */
    genutil_format_date ((char*)strSystemDateTime) ;
	memcpy (strCurrentDate, strSystemDateTime, 8) ; 
	
	/* Get Current Date in Julian */
	CurrentDate = Txutils_Calculate_Julian_Date (strCurrentDate) ;

    /* peso_julian_date is the date when a peso tx last occurred */
	DateWhenAPesoTransactionLastOccurred = atol (auth_tx.CCF03P_details.peso_julian_date) ;
	/* dollar_julian_date is the date when a dollar tx last occurred */
	DateWhenADollarTransactionLastOccurred = atol (auth_tx.CCF03P_details.dollar_julian_date) ;

	/* Regardless of peso or dollar, determine when the latest transaction date for the cardholder */
	if (DateWhenAPesoTransactionLastOccurred > DateWhenADollarTransactionLastOccurred)
		DateWhenATransactionLastOccurred = DateWhenAPesoTransactionLastOccurred ;
	else 
		DateWhenATransactionLastOccurred = DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, get diff between the current date and when 
	// a transaction was last occurred for the cardholder (could be 0)
	DaysElapsedSinceATransactionLastOccurred = CurrentDate - DateWhenATransactionLastOccurred ;
    if (DaysElapsedSinceATransactionLastOccurred == 0)
        return 1 ;
    else 
        return 0 ;
} /* TxAuthorize_If_Transaction_Occurred_On_Same_Day */

BYTE Update_Private_Ccf03p_Structure ()
{
    memset (&private_ccf03p_struct, 0, sizeof private_ccf03p_struct) ;

	private_ccf03p_struct.peso_julian_date = atol (auth_tx.CCF03P_details.peso_julian_date) ;
    private_ccf03p_struct.dollar_julian_date = atol (auth_tx.CCF03P_details.dollar_julian_date) ;

    Populate_Usage_Bucket (PESO) ;
    Populate_Usage_Bucket (DOLLAR) ;
    Populate_Merchant_Usage_Bucket () ;
    return NO_AUTH_ERROR ;
} 

BYTE Populate_Usage_Bucket (int context)
{
	int index = 0 ;

   if (context == PESO)
	{
		// Copy the array from CCF03P into card_usage converting 
		// the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.peso_usage[index].nbr_usage = 
				atoi (auth_tx.CCF03P_details.peso_usage[index].nbr_usage) ;
			private_ccf03p_struct.peso_usage[index].amt_usage = 
				(strtod (auth_tx.CCF03P_details.peso_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	else if (context == DOLLAR)
	{
		// Copy the array from CCF03P into card_usage converting the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.dollar_usage[index].nbr_usage = 
				atoi (auth_tx.CCF03P_details.dollar_usage[index].nbr_usage) ;
			private_ccf03p_struct.dollar_usage[index].amt_usage = 
				(strtod (auth_tx.CCF03P_details.dollar_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	return NO_AUTH_ERROR ;
}

BYTE Populate_Merchant_Usage_Bucket (void)
{
	int index = 0 ;
   
	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.repeat[index].merchant_id, 
			    auth_tx.CCF03P_details.repeat[index].merchant_id) ;
		private_ccf03p_struct.repeat[index].nbr_uses = 
			atoi (auth_tx.CCF03P_details.repeat[index].nbr_uses) ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE Update_CCF03_Structure( INT update_date )
{
   BYTE  strTemp[50];
   BYTE  strAmount[20]="";

   /* Update the date only if the input indicates it. */
   if ( update_date == true )
   {
     
      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.peso_usage[13].nbr_usage, strTemp, 10) ;
      strcpy (auth_tx.CCF03P_details.peso_usage[13].nbr_usage, strTemp) ;

	  // We are not storing amount in database as decimal format. Convert_Amt_To_Auth_Tx_String_Format function will multiply by 100 so device it by 100
	  private_ccf03p_struct.peso_usage[13].amt_usage = private_ccf03p_struct.peso_usage[13].amt_usage ;
	  memset (strAmount, 0, sizeof (strAmount)) ;
      Convert_Amt_To_Auth_Tx_String_Format 
              (private_ccf03p_struct.peso_usage[13].amt_usage, strAmount);
	   strcpy (auth_tx.CCF03P_details.peso_usage[13].amt_usage, strAmount);
   }

	strcpy (auth_tx.CCF03P_details.repeat[18].merchant_id, 
	      private_ccf03p_struct.repeat[18].merchant_id) ;

	memset (strTemp, 0, sizeof (strTemp)) ;
	itoa (private_ccf03p_struct.repeat[18].nbr_uses, strTemp, 10) ;
	strcpy (auth_tx.CCF03P_details.repeat[18].nbr_uses, strTemp) ;


   // Set CCF03P_MASK in the update_mask field of TLF01
   auth_tx.TLF01_details.update_mask[0] |= CCF03P_MASK ;
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_CCF03_Structure */

void  Init_CCF03_Structure (void )
{
   BYTE  strTemp[50];
   BYTE  strAmount[50] = {0} ;
   INT   nIndex = 0 ;


   memset( strTemp, 0x00, sizeof(strTemp) );
   ltoa( private_ccf03p_struct.peso_julian_date, strTemp, 10 );
   strcpy( auth_tx.CCF03P_details.peso_julian_date, strTemp );

   memset( strTemp, 0x00, sizeof(strTemp) );
   ltoa( private_ccf03p_struct.dollar_julian_date, strTemp, 10 );
   strcpy( auth_tx.CCF03P_details.dollar_julian_date, strTemp );
  
   memset (strTemp, 0, sizeof (strTemp)) ;
   itoa (private_ccf03p_struct.failed_pin_attempts, strTemp, 10) ;
   strcpy (auth_tx.CCF03P_details.failed_pin_attempts, strTemp) ;

   Convert_Amt_To_Auth_Tx_String_Format( private_ccf03p_struct.daily_amt_usage,
                                         strAmount) ;
   strcpy (auth_tx.CCF03P_details.daily_amt_usage, strAmount) ;

   for (nIndex = 0 ; nIndex <= 13 ; nIndex++)
   {
      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.peso_usage[nIndex].nbr_usage, strTemp, 10) ;
      strcpy (auth_tx.CCF03P_details.peso_usage[nIndex].nbr_usage, strTemp) ;

      // Multiply the number by 100 and get rid of the decimal point
      memset (strAmount, 0, sizeof (strAmount)) ;
      Convert_Amt_To_Auth_Tx_String_Format 
             (private_ccf03p_struct.peso_usage[nIndex].amt_usage, strAmount) ;

      strcpy (auth_tx.CCF03P_details.peso_usage[nIndex].amt_usage, strAmount) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.dollar_usage[nIndex].nbr_usage, strTemp, 10) ;
      strcpy (auth_tx.CCF03P_details.dollar_usage[nIndex].nbr_usage, strTemp) ;

      // Multiply the number by 100 and get rid of the decimal point
      memset (strAmount, 0, sizeof (strAmount)) ;
      Convert_Amt_To_Auth_Tx_String_Format 
              (private_ccf03p_struct.dollar_usage[nIndex].amt_usage, strAmount);

      strcpy (auth_tx.CCF03P_details.dollar_usage[nIndex].amt_usage, strAmount);
   }

   for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
   {
      strcpy (auth_tx.CCF03P_details.mcc[nIndex].category_code, 
              private_ccf03p_struct.mcc[nIndex].category_code) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.mcc[nIndex].usage, strTemp, 10) ;
      strcpy (auth_tx.CCF03P_details.mcc[nIndex].usage, strTemp) ;
   }

   for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
   {
      strcpy (auth_tx.CCF03P_details.repeat[nIndex].merchant_id, 
              private_ccf03p_struct.repeat[nIndex].merchant_id) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.repeat[nIndex].nbr_uses, strTemp, 10) ;
      strcpy (auth_tx.CCF03P_details.repeat[nIndex].nbr_uses, strTemp) ;
   }
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
INT parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
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


INT dcpimp_Check_If_CUP_MP_Transaction (void)
{
	if ((0 == strncmp(auth_tx.TLF01_details.card_num,CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1))||
		(0 == strncmp(auth_tx.TLF01_details.card_num,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2))||
		(0 == strncmp(auth_tx.TLF01_details.card_num,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3))||
		(0 == strncmp(auth_tx.TLF01_details.card_num,CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4))||
		(0 == strncmp(auth_tx.TLF01_details.card_num,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT  dcpimp_check_If_CUP_Retail_Transaction(void)
{
	if (0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(auth_tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == dcpimp_check_If_VISA_Transaction ()) ||
		   (true == dcpimp_check_If_MC_Transaction()))
		{
			return true ;
		}
		else
		{
			strncpy(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2);
			return true ;
		}
	}
	else
	{
		return false;
	}
}
INT dcpimp_check_If_VISA_Transaction (void)
{
	if (auth_tx.TLF01_details.card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT dcpimp_check_If_MC_Transaction (void)
{

	char MC_bin_series[7]   = {0};
    int  MC_Bin_series_low  = 222100;
    int  MC_Bin_series_high = 272099;
    int  MC_bin_value 		= 0;
    int  MC_Bin_serice_len  = 6 ;

    strncpy(MC_bin_series,auth_tx.TLF01_details.card_num,MC_Bin_serice_len);
    MC_bin_value = atoi(MC_bin_series);

    if(	auth_tx.TLF01_details.card_num[0]=='5')
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
INT dcpimp_check_If_JCB_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   (auth_tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT dcpimp_check_If_AMEX_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   ((auth_tx.TLF01_details.card_num[1] == '4') ||
		(auth_tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT dcpimp_check_If_Diners_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   ((auth_tx.TLF01_details.card_num[1] == '0') ||
		(auth_tx.TLF01_details.card_num[1] == '6') ||
		(auth_tx.TLF01_details.card_num[1] == '8') ||
		(auth_tx.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((auth_tx.TLF01_details.card_num[0]=='6') &&
			 ((auth_tx.TLF01_details.card_num[1] == '0') ||
			 (auth_tx.TLF01_details.card_num[1] == '4') ||
			 (auth_tx.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
/******************************************************************************/
void dcpimp_Prepare_Pos_Data(void)
{

	if(true == dcpimp_check_If_AMEX_Transaction())
	{
		dcpimp_prepare_posdata_for_Amex();
	}
	else if(true == dcpimp_check_If_JCB_Transaction())
	{
		dcpimp_prepare_posdata_for_JBC();
	}
	else if(true == dcpimp_check_If_VISA_Transaction())
	{
		dcpimp_prepare_posdata_for_VISA();
	}
	else if(true == dcpimp_check_If_MC_Transaction())
	{
		dcpimp_prepare_posdata_for_MC();
	}
	else if(true == dcpimp_check_If_Diners_Transaction())
	{
		dcpimp_Prepare_posdata_for_DINERS();
	}
	else if(true == dcpimp_Check_If_CUP_MP_Transaction())
	{
		dcpimp_prepare_posdata_for_CUP();
	}

}


void dcpimp_prepare_posdata_for_Amex( )

{

	CHAR temp_entry_mode[13]={0};

	if((0 == strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp(auth_tx.TLF01_details.handler_queue,"VOICE",5)))
	{
		if(strlen(auth_tx.TLF01_details.pos_entry_mode) > 3)
		{
			strncpy(temp_entry_mode,&auth_tx.TLF01_details.pos_entry_mode[1],3);
		}
		else
		{
			strcpy(temp_entry_mode,auth_tx.TLF01_details.pos_entry_mode);
		}
		if(strncmp(temp_entry_mode,"05",2) == 0)
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"510101554041",DE22_SIZE_AMEX);
		}
		else if(strncmp(temp_entry_mode,"07",2) == 0 && strncmp(auth_tx.TLF01_details.pos_condition_code,"59",2) != 0)
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"51010X554041",DE22_SIZE_AMEX);
		}
		else if(strncmp(temp_entry_mode,"91",2) == 0)
		{
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"51010X254041",DE22_SIZE_AMEX);
		}
		else if(strncmp(temp_entry_mode,"80",2) == 0)
		{
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"510101954041",DE22_SIZE_AMEX);
		}
		/* For voice transactions always send this */
		else if(auth_tx.TLF01_details.voice_auth[0] == 'Y')
		{
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"600001S00100",DE22_SIZE_AMEX);
			 /* For position seven, need to know if 4DBC/4CSC was entered.
					* Security control information is coming in
					* auth_Tx.tlf01_details.interest_rate.
					*/
			if ( auth_tx.TLF01_details.interest_rate[0] == 0x00 )
			{
			 /* It was not entered, so change 'S' to '6'. */
				auth_tx.TLF01_details.product_codes[16].amount[6] = '6';
			}
		}

		// else if(strncmp(temp_entry_mode,"012",3) == 0 && (strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2) == 0))
		else if(strncmp(auth_tx.TLF01_details.pos_condition_code,"59",2) == 0) // For AMEX testing
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"6000S0S00100",DE22_SIZE_AMEX); /* For E-commerce tx*/
		}
		else if(strncmp(temp_entry_mode,"01",2) == 0)
		{
			 if((strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2) == 0) &&
			 (temp_entry_mode[2] == '2') && (0 == strcmp(auth_tx.industry_code,"800")) )
			 {
				strncpy(auth_tx.TLF01_details.product_codes[16].amount,"600090600100",DE22_SIZE_AMEX);              /* Changed the position 5 of POS Entry mode form '2' to '9'*/                                                                                                                                                                                                        /* Girija Y, March 02, 2009 */
			 }
			 else
			 {
				strncpy(auth_tx.TLF01_details.product_codes[16].amount,"600101600100",DE22_SIZE_AMEX);
			 }
		}
		else if(strncmp(temp_entry_mode,"02",2) == 0)
		{
			if(strncmp(auth_tx.TLF01_details.dispensation.origin,"RO",2) == 0)
			{
				strncpy(auth_tx.TLF01_details.product_codes[16].amount,"210201254141",DE22_SIZE_AMEX);
			}
			else
			{
				 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"210101254140",DE22_SIZE_AMEX);
			}
			 /*For fall back , Terminal may be Chip, update term type*/
			if(strncmp(auth_tx.TLF01_details.terminal_type,"07",2)== 0)
				//if(strncmp(Auth_Tx.TLF01_details.terminal_type,AMEX_EMV_CAPABILITY_INDICATOR,AMEX_DEVICE_EMV_CAPABILITY_LEN)== 0)
			{
				auth_tx.TLF01_details.product_codes[16].amount[0] = '5';
			}
		}
		else if(strncmp(temp_entry_mode,"06",2) == 0)
		{
		  if(auth_tx.TLF01_details.tx_key == AUTH_REFUND)
		  {
			 /* According to EB: Credit refund POS entry mode, since
						* the 1120 message does not contain field 53, you need
						* to always to use the pos entry mode for 02X, 01X. This is
						* a limitation on the terminal software.  Thus, for a credit
						* refund transaction, if ATP receives a value of 06X, ATP
						* needs to use a pos mode of 02X, and if ATP receives a
						* value of 07X, ATP needs to use a pos entry mode of 01X.
						*/
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"260101254140",DE22_SIZE_AMEX);
		  }
		  else
		  {
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"260101W54140",DE22_SIZE_AMEX);
		  }
		}
		else if(strncmp(temp_entry_mode,"07",2) == 0)
		{
			// if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND)
			//             {
			/* According to EB: Credit refund POS entry mode, since
						* the 1120 message does not contain field 53, you need
						* to always to use the pos entry mode for 02X, 01X. This is
						* a limitation on the terminal software.  Thus, for a credit
						* refund transaction, if ATP receives a value of 06X, ATP
						* needs to use a pos mode of 02X, and if ATP receives a
						* value of 07X, ATP needs to use a pos entry mode of 01X.
						*/
			//   strncpy(Auth_Tx.TLF01_details.product_codes[16].amount,"660101654140",DE22_SIZE_AMEX);
			//   }
		 if((strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2) == 0)
		  && (0 == strcmp(auth_tx.industry_code,"800")))  /* Changed the position 5 of POS Entry mode form '2' to '9'*/                                                                                                                                                                                                                                                                   /* Girija Y, March 02, 2009 */
		  {
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"600090S04100",DE22_SIZE_AMEX);
		  }
		  else
		  {
			 strncpy(auth_tx.TLF01_details.product_codes[16].amount,"610101S54140",DE22_SIZE_AMEX);
		  }
		}
		if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3))
		{
			 if( temp_entry_mode[2] !='1')
			 {
				 if((0 == strncmp(auth_tx.TLF01_details.pos_condition_code,"59",2))||
					(0 == strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2)))
				 {
					 // Dont change the Bit 2
				 }
				 else
				 {
					 memset(&auth_tx.TLF01_details.product_codes[16].amount[1],0x36,1);
				 }
			 }
		}
	}
}
INT dcpimp_check_CUP_emv_capability_device(void)
{
	if( 0 == strncmp(auth_tx.TLF01_details.terminal_type + DEVICE_EMV_CAPABILITY_POS_FOR_CUP,
					  CUP_EMV_CAPABILITY_INDICATOR,CUP_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

void dcpimp_prepare_posdata_for_CUP(void)
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
	#define DE23_SIZE 3
	#define DE22_SIZE 3

    if( 0 == strncmp(auth_tx.TLF01_details.handler_queue,"nccup",5))
	{

	}
	else
	{
		/*Prepare feild 60.2*/
		memcpy(auth_tx.TLF01_details.product_codes[16].amount ,"0",1);
		//Prepare PAN Scequence Number
		if (strlen(auth_tx.host2_message_seq_nbr) > 0)
		{
			  /* Card Seq. num came in field 23, but not in DE55. */
			  memcpy(temp_str,auth_tx.host2_message_seq_nbr+1,DE23_SIZE);
			  itemp = atoi(temp_str);
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num,temp_str1,DE23_SIZE );
		}else if (strlen(auth_tx.EMV_details.pan_sequence_number)> 0)
		{
			  memcpy(temp_str,auth_tx.EMV_details.pan_sequence_number,DE23_SIZE);
			  itemp = atoi( temp_str );
			  sprintf( temp_str1, "%03d", itemp );
			  memcpy( HOSTMESSAGE_card_seq_num, temp_str1,DE23_SIZE );
		}

		//Prepare POS ENTRY MODE
		strncpy(HOSTMESSAGE_posentry,auth_tx.TLF01_details.pos_entry_mode +1 ,DE22_SIZE);
		strncpy(temp_pos,HOSTMESSAGE_posentry,2);
		temp_posentry=atoi(temp_pos);
		switch(temp_posentry)
		{
			case 02:
			case 90:
			case 06:
				//Prepare feild 60.2.2
				if( true == dcpimp_check_CUP_emv_capability_device())
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount+ 5,"5",1);
				}
				else
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 5,"2",1);
				}
				//Prepare feild 60.2.3
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 05:
			case 95:
			case 97:
			case 80:
				if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"VOICE",5))
				{
					//Prepare feild 60.2.2
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"0",1);
					//Prepare feild 60.2.3
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
				else
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"5",1);
					//Prepare feild 60.2.3
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
				}
			break;

			case 07:
			case 91:
			case 96:
			case 98:
				//Prepare feild 60.2.2
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"6",1);
				//Prepare feild 60.2.3
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;

			case 01:
				//Prepare feild 60.2.2
				if((0 == strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2)) ||
				   (0 == strncmp(auth_tx.TLF01_details.pos_condition_code,"59",2)))
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 5,"0",1);
				}
				else if( true == dcpimp_check_CUP_emv_capability_device())
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 5,"5",1);
				}
				else
				{
					memcpy(auth_tx.TLF01_details.product_codes[16].amount + 5,"2",1);
				}
				//Prepare feild 60.2.3
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 6,"0",1);
			break;

			default:
				//Prepare feild 60.2.2
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"0",1);
				//Prepare feild 60.2.3
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			break;
		}

		/*Prepare feild 60.2.4 Reserved filled in with 0*/
		memcpy(auth_tx.TLF01_details.product_codes[16].amount + 3,"0",1);

		//Prepare feild 60.2.2 for Void and Reversal for Magstip Trasaction
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
				// Do nothing Its EMV Trasaction
			}
		else if(auth_tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_CANCLE ||
				auth_tx.TLF01_details.tx_key == AUTH_REVERSAL ||
				auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||
				auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE
			)
		{
			// Its Magstrip Transaction
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"2",1);
		}

		// Prepare field 60.2.3 for Reversal and Fallback reversal
		if(auth_tx.TLF01_details.tx_key == AUTH_REVERSAL ||
				auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE )
		{
			if( (true == dcpimp_check_CUP_emv_capability_device())&&
				(auth_tx.TLF01_details.pos_entry_mode[1]== '8'))
			{
				// Its fallback , Check if DE 23 is not present
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"5",1);
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"2",1);
			}
			else
			{
				// Normal  Reversal
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
			}
		}
		//Prepare filed 60.2.3 for POS EMV fallback transaction
		else if( (true == dcpimp_check_CUP_emv_capability_device())&&
				 (auth_tx.TLF01_details.pos_entry_mode[1]== '8'))
		{
			// Its fallback , Check if DE 23 is not present
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 1,"5",1);
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 2,"0",1);
		}

		/*Prepare feild 60.2.5 Transaction Channel Type*/
		if( 0 == strncmp(auth_tx.TLF01_details.handler_queue,"VOICE",5))
		{
			if(0 ==strncmp(auth_tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 4,"08", 2);//Mobile phone Voice transaction
			}
			else
			{
				memcpy(auth_tx.TLF01_details.product_codes[16].amount + 4,"07", 2);//PC Voice transaction
			}
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 10,"3", 1);
		}
		else
		{
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 4,"03", 2);//POS-Point Of Sale 03
			/*Prepare feild 60.2.9 Interactive mode indicator*/
			memcpy(auth_tx.TLF01_details.product_codes[16].amount + 10,"0", 1);
		}

		/*Prepare feild 60.2.6 Signature-based Credit Card Network Identifier*/
		memcpy(auth_tx.TLF01_details.product_codes[16].amount + 6,"0", 1);
		/*Prepare feild 60.2.7 Chip Card Authentication Reliability Indicato*/
		memcpy(auth_tx.TLF01_details.product_codes[16].amount + 7,"0", 1);
		/*Prepare feild 60.2.8	Electronic Commerce Identification*/
		memcpy(auth_tx.TLF01_details.product_codes[16].amount + 8,"00", 2);
	}
}


void dcpimp_prepare_posdata_for_VISA(void)
{
	CHAR   	tempstr [256]; 
	BYTE 	emv_chip_code;
	INT  	pos_entry_type = POS_ENTRY_NORMAL;
	CHAR   	temp_str[100];
	BYTE 	eCommerce_Indcator[3] ;
	INT    	RPS_flag = 0;
	INT    	iIndustry;
	INT  	emv_txn;
	INT  	emv_len;
	CHAR    temp_len[5]="";
	CHAR    error_str[100]="";

	iIndustry = atoi( auth_tx.industry_code );

   /* Determine if txn is EMV (Smart Card). */
   if ( (auth_tx.EMV_details.emv_block[0] == 0x00) &&
        (auth_tx.EMV_details.emv_block[1] == 0x00)  )
      emv_txn = false;
   else
   {
      emv_txn = true;
      if(strcmp(auth_tx.TLF01_details.dispensation.origin,"RO")==0)
		  emv_len = auth_tx.EMV_details.emv_block[0];
		  else
		  emv_len = genutil_bcd_to_int(auth_tx.EMV_details.emv_block, 2 );
   }
   
    /* Set the POS Entry Mode flag for use in other fields. */
      if ( emv_txn == true )
         pos_entry_type = POS_ENTRY_EMV_CHIP;

      else if ( 0 == strncmp("080",auth_tx.TLF01_details.pos_entry_mode,3) )
         pos_entry_type = POS_ENTRY_EMV_MAGSTRIPE;

      else if ( 0 == strncmp("008",auth_tx.TLF01_details.pos_entry_mode,3) )
         pos_entry_type = POS_ENTRY_EMV_FALLBACK;

	  /* FIELD 60: ADDITIONAL POS INFORMATION */
	 memset( tempstr, 0x00, sizeof(tempstr) ); 
	 memcpy(tempstr,auth_tx.TLF01_details.nfi_name,sizeof(auth_tx.TLF01_details.nfi_name)); 
	 if(!(IsFieldNull(auth_tx.TLF01_details.nfi_name))) 
	 {		
	 	strncpy(auth_tx.TLF01_details.product_codes[16].amount,auth_tx.TLF01_details.nfi_name,
		   	   (sizeof(auth_tx.TLF01_details.nfi_name)-1));
	 }	
	 else 
	 {				 
	  if ( auth_tx.TLF01_details.track2[0] != 0x00 )
	  {
		 /* Determine chip condition code (if relevant) for DE60.3 */
		 (void)parse_track2(&auth_tx,error_str); /* Gets service code */
		 if ( (ServiceCode[0] == '2') || (ServiceCode[0] == '6') )
		 {
			if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
			{
			   emv_chip_code = auth_tx.EMV_details.chip_condition_code[0];
			   auth_tx.EMV_details.chip_condition_code[0] = '1';
			}
			else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
			{
			   emv_chip_code = '2';
			   auth_tx.EMV_details.chip_condition_code[0] = '2';
			}
			else if ( pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE )
			{
			   emv_chip_code = auth_tx.EMV_details.chip_condition_code[0];
			}
			else
			{
			   emv_chip_code = '0';
			}

			/* Make sure chip code is not 0 for Service Code 2 or 6. */
			if ( (emv_chip_code != '1') && (emv_chip_code != '2') )
			   emv_chip_code = '1';

			if ( auth_tx.EMV_details.chip_condition_code[0] == 0x00 )
			   auth_tx.EMV_details.chip_condition_code[0] = '1';
		 }
		 else
		 {
			emv_chip_code = '0';  /* Always for Service Code not 2 or 6 */
			if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
			{
			   auth_tx.EMV_details.chip_condition_code[0] = '1';
			}
			else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
			{
			   auth_tx.EMV_details.chip_condition_code[0] = '2';
			}
			else if ( pos_entry_type != POS_ENTRY_EMV_MAGSTRIPE )
			{
			   auth_tx.EMV_details.chip_condition_code[0] = '0';
			}
		 }
	  }

	   if ( iIndustry == ECOMMERCE )  /* Verified by visa VbV */
	   {
		 memset( tempstr, 0x00, sizeof(tempstr) );
		 if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
			strcpy( tempstr, "55200000" );
		 else
			strcpy( tempstr, "50000000" );

		 if ( eCommerce_Indcator[0] == 0x00 )
		 {
			if ( auth_tx.TLF01_details.message_type[1] == '4' )
			{
			   /* This is a reversal. Need to echo ECI.
				* It gets stored in settlement date (TLF01);
				* It is stored in BCH10 in product code 20.
				*/
			   strcpy(eCommerce_Indcator,auth_tx.TLF01_details.settlement_date);
			}
			else
			{
			   /* Not a reversal. Use default value. */
			   strcpy( eCommerce_Indcator, "07" );
			}
		 }
		 strncat( tempstr, eCommerce_Indcator, 2 );

		 /* Save ECI for reversals. */
		 strcpy( auth_tx.TLF01_details.settlement_date, eCommerce_Indcator );

		 if ( auth_tx.TLF01_details.terminal_type[1] == '7' )
			tempstr[1] = '5'; /* Terminal has Chip Read capability. */
		 memcpy(auth_tx.TLF01_details.product_codes[16].amount,auth_tx.additional_pos_information,
		 		sizeof(auth_tx.additional_pos_information));	

	   }
	  else if ( emv_txn == true ||	pos_entry_type == POS_ENTRY_EMV_FALLBACK ||
				pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE)
	  {
		 memset( tempstr, 0x00, sizeof(tempstr) );
		 strcpy( tempstr, "05000020" );

		 /* 60.3 = Chip Condition Code */
		 if ( emv_chip_code == 0x00 )
		 {
			/* EB wants this field to default to zero. */
			emv_chip_code = '0';
			auth_tx.EMV_details.chip_condition_code[0] = '0';
		 }
		 /* tempstr[2] = emv_chip_code; EB wants 60.3 set to 0. */
		 memcpy(auth_tx.additional_pos_information,tempstr,	sizeof(auth_tx.additional_pos_information));
		 
		 memcpy(auth_tx.TLF01_details.product_codes[16].amount,auth_tx.additional_pos_information,
			sizeof(auth_tx.additional_pos_information));

	  }
	  else if ('Y' != auth_tx.TLF01_details.voice_auth[0])
	  {
		 if (0 == strcmp(auth_tx.TLF01_details.terminal_type,"01"))
		 {
			/* Unattended gas pump */
			auth_tx.additional_pos_information[0] = '3';
		 }
		// else if ( iIndustry == ATM ) 
		 //{
			/* ATM */
			//Auth_Tx.additional_pos_information[0] = ATM_TERMINAL;
		// }
		 else
		 {
			/* Unspecified */
			auth_tx.additional_pos_information[0] = '0';
		 }

		 /* Ignore the following for Ecommerce. */
		 if ( iIndustry != ECOMMERCE )
		 {
			if ( RPS_flag == 1 ) //RPS
			{
			   /* RPS Transaction - Do not send field 60 for non-USA. */
			   auth_tx.additional_pos_information[0] = 0x00;
			}
			else if (auth_tx.TLF01_details.pos_entry_mode[2] == '2')
			{
			   /* Magnetic stripe read capability */
			   auth_tx.additional_pos_information[1] = '2';
			}
			//else if ( iIndustry == ATM ) /* ATM */
			//{
			   /*  magnetic stripe read capability */
			  // Auth_Tx.additional_pos_information[1] = '2';
			//}
			else if (auth_tx.TLF01_details.pos_entry_mode[2] == '1')
			{
			   /* Magnetic stripe read capability */
			   auth_tx.additional_pos_information[1] = '2';
			}
			else if ( pos_entry_type != POS_ENTRY_NORMAL )
			{
			   auth_tx.additional_pos_information[1] = '5';
			   auth_tx.additional_pos_information[2] = emv_chip_code;
			}
			else 
			{
			   /* No magnetic stripe read capability */
			   auth_tx.additional_pos_information[1] = '1';
			}

			if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
			   memcpy( auth_tx.additional_pos_information, "0520", 4 );
		 }

		 if ( auth_tx.TLF01_details.terminal_type[1] == '7' )
		 {
			/* Terminal has Chip Read capability. */
			auth_tx.additional_pos_information[1] = '5';
		 }
		 memcpy(auth_tx.TLF01_details.product_codes[16].amount,auth_tx.additional_pos_information,
		 		sizeof(auth_tx.additional_pos_information));
		 	
		 /* Store here for Fraudguard. */
		 memcpy( auth_tx.TLF01_details.nfi_name,
				 auth_tx.additional_pos_information,
				 sizeof(auth_tx.additional_pos_information) );
	  }
	} //Neetu

}

void dcpimp_prepare_posdata_for_MC(void)
{
   BOOLEAN e_commerce;
   INT iIndustry;
   CHAR    temp_str3[4];
   INT     itemp;
   
   iIndustry = atoi( auth_tx.industry_code );
	
   /* Some fields need to know if this txn is an e-commerce txn. */
   if( (iIndustry == 700) ||
       (0==strcmp(auth_tx.TLF01_details.pos_condition_code, ECOM_POS_CC)) )
   {
      e_commerce = true;
   }
   else
      e_commerce = false;
	
   if ( e_commerce )
   {
			  strncpy( auth_tx.TLF01_details.product_codes[16].amount, "1025100006000608",11 );
	
			  /* If merchant supports UCAF (see DE48, tag 42), then
			   * set position four to '5' and position ten to '6'.
			   * It doesn't matter if DE48.43 is populated.
			   * Right now, no Equitable merchants support UCAF, so
			   * leave these values as they are set above.
			   */
		   }
	
		   else if ( ('Y' == auth_tx.TLF01_details.voice_auth[0]) ||
					 ('y' == auth_tx.TLF01_details.voice_auth[0])   )
			  strncpy( auth_tx.TLF01_details.product_codes[16].amount, "2030000000100608" ,11);
	
		   else
		   {
			  switch( iIndustry )
			  {
				 case 100:
				 case 101:
				 case 200:
				 case 300:
				 case 400:
				 case 900:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "0000010000700608", 11 ); break;
				 case 902:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "0000010000700608", 11 ); break;
				 case 500:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "2032100000000608", 11 ); break;
				 case 600:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "1000010001700608", 11 ); break;
				 case 800:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "2034100000100608", 11 ); break;
				 default:	strncpy( auth_tx.TLF01_details.product_codes[16].amount, "0000000000700608", 11 ); break;   
			  }
		   }
	
			/*	Subfield 10—Cardholder-Activated Terminal Level
				Identification of Mobile POS Acceptance Devices
				Set Feild 61.10 to 9 to indicate Cardholder-Activated Terminal Level Indicator
				is mPOS Acceptance Device
			*/
			if(auth_tx.TLF01_details.product_codes[1].quantity == 'ID_MOBILE_POS_ACCP_DEVICES')
			{
				strncpy( auth_tx.TLF01_details.product_codes[16].amount[9], auth_tx.TLF01_details.product_codes[1].quantity,1 );
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
		   memset( temp_str3, 0x00, sizeof(temp_str3) );
		   memcpy( temp_str3, auth_tx.TLF01_details.pos_entry_mode, 3 );
	
		   if ( (0 == strncmp(temp_str3, "90", 2)) ||
				(0 == strncmp(temp_str3, "05", 2)) ||
				(0 == strncmp(temp_str3, "01", 2)) ||
				(0 == strncmp(temp_str3, "80", 2))
			  )
		   {
			  /* Mag stripe, key entry, and EMV-Compatible chip reader */
				if( ((auth_tx.TLF01_details.product_codes[16].amount[3] == '0') && (auth_tx.TLF01_details.product_codes[16].amount[4] == '1'))) 
				{
					// To avoid 8.h edit error
					auth_tx.TLF01_details.product_codes[16].amount[10]  = '7';
				}
				else
				{
					auth_tx.TLF01_details.product_codes[16].amount[10]  = '8';
				}
			}
		   else if ( (0 == strcmp (temp_str3, "072"  )) ||
					 (0 == strcmp (temp_str3, "912"  ))
				   )
		   {
			  /* Contactless M/Chip */
			  auth_tx.TLF01_details.product_codes[16].amount[10]  = '3';
		   }
	
		   if(auth_tx.MCF01_details.rps_merchant[0]=='Y')
		   {
			   auth_tx.TLF01_details.product_codes[16].amount[10]  = '0';  /* ThoughtFocus - Chandra - For Recurring Payment transaction, subfield 11 should be 0.	*/
		   }
	
		   //strncpy( &HOST_MESSAGE.priv61[16], Auth_Tx.MCF01_details.zip_code,10);
	
		   // Edit error 8.f check
		  if ( (0 == strncmp(temp_str3, "90", 2)) || 
			   (0 == strncmp(temp_str3, "02", 2)))
		  {
			   if(auth_tx.TLF01_details.product_codes[16].amount[10]=='6')
			   {
				   auth_tx.TLF01_details.product_codes[16].amount[10]  = '7';
			   }
		  }
	   
		  // Edit error 8.i check
		  if ( (0 == strncmp(temp_str3, "90", 2)) || 
			   (0 == strncmp(temp_str3, "05", 2)) ||
			   (0 == strncmp(temp_str3, "02", 2)) ||
			   (0 == strncmp(temp_str3, "80", 2))
			 )
		  {
	   
			   auth_tx.TLF01_details.product_codes[16].amount[4]  = '0';
		  }
	   
		  // Edit error 8.e check
		  if ( (0 == strncmp(temp_str3, "00", 2)))
		  {
			   if((auth_tx.TLF01_details.product_codes[16].amount[10]=='2')||
				  (auth_tx.TLF01_details.product_codes[16].amount[10]=='5')||
				  (auth_tx.TLF01_details.product_codes[16].amount[10]=='7')||
				  (auth_tx.TLF01_details.product_codes[16].amount[10]=='8'))
			   {
				   auth_tx.TLF01_details.product_codes[16].amount[10]  = '0';
			   }
		  }
		
		  // Edit error 8.d check
		  if ( (0 == strncmp(temp_str3, "00", 2))||
			   (0 == strncmp(temp_str3, "01", 2)))
		  {
			   if(auth_tx.TLF01_details.product_codes[16].amount[10]=='1')
			   {
	   
				   auth_tx.TLF01_details.product_codes[16].amount[0]='2';
				   auth_tx.TLF01_details.product_codes[16].amount[2]='3';
			   }
		  }
		  // Edit error 3.d and 3.e
		  if((0 == strncmp(temp_str3, "80", 2))||
			 (0 == strncmp(temp_str3, "90", 2))||
			 (0 == strncmp(temp_str3, "90", 2)))
		  {
			   // Check if DE45 and DE35 is present or not
			   if((strlen(auth_tx.TLF01_details.track2) > 0)||
				  (strlen(auth_tx.TLF01_details.track1) > 0))
			   {
				   auth_tx.TLF01_details.product_codes[16].amount[3]  = '0';
				   if(auth_tx.TLF01_details.product_codes[16].amount[4]	== '1')
				   {
					   if(0 == strncmp(temp_str3, "80", 2))
					   {
						   auth_tx.TLF01_details.product_codes[16].amount[4]  = '5';
					   }
					   else if(0 == strncmp(temp_str3, "90", 2))
					   {
						   auth_tx.TLF01_details.product_codes[16].amount[4]  = '2';
					   }
					   else if(0 == strncmp(temp_str3, "91", 2))
					   {
						   auth_tx.TLF01_details.product_codes[16].amount[4]  = '4';
					   }
				   }
			   }
		  }
	
		   /* Blank fill whatever zip code does not fill. */
		   itemp = strlen( auth_tx.MCF01_details.zip_code );
		   //memset( &HOST_MESSAGE.priv61[16+itemp], 0x20, 10 - itemp );
	
		   /* For automated fuel dispenser transactions, subfield 10 must be '2' */
		   if ( 0 == strncmp( AUTO_FUEL_MCC, auth_tx.TLF01_details.category_code,4 ))
		   {
			  /* Industry code 600 should take care of this,
			   * but just in case, do it here, else MasterCard charges a fee.
			   */
			  memset( &auth_tx.TLF01_details.product_codes[16].amount[9], 0x32, 1 );
		   }
	
   if(0 == strncmp(temp_str3, "005", 3) )
   { //Chip
	   strncpy( auth_tx.TLF01_details.product_codes[16].amount, "000001000080",12 );
   }
		   // copy POS data in AUTH->acceptor_term_name
		   strcpy(auth_tx.acceptor_term_name, auth_tx.TLF01_details.product_codes[16].amount);

}

void dcpimp_prepare_posdata_for_JBC(void)
{

	CHAR   temp_str[50];
	INT    iIndustry;
	INT    mcc;
	CHAR   tempstr [256];
	CHAR   num_cc[4] = "";

	iIndustry = atoi( auth_tx.industry_code );
	
	/* FIELD 61: POINT OF SERVICE INFORMATION */
	   /* Default Values */
	   memset( temp_str, 0x00, sizeof(temp_str) );
	   strcpy( temp_str, "222" );
	
	   /* Subfield 61.1 - Mail/Telephone Order Transaction Sign */
	   if ( iIndustry == MAIL_PHONE )
		  temp_str[0] = '1';
	
	   /* Subfield 61.2 - Recurring Transaction Sign */
	   else if ( iIndustry == RPS )
	   {
		  temp_str[1] = '1';
	
		  /* Check for MOTO Merchant */
		  mcc = atoi( auth_tx.TLF01_details.category_code );
		  if ( (MCC_RANGE2_LOW <= mcc)	&&	(mcc <= MCC_RANGE2_HIGH) )
			 temp_str[0] = '1';
	   }
	
	   /* Subfield 61.3 - Pre Authorization Sign */
	   if ( (auth_tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION) ||
			(auth_tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION) ||
			(auth_tx.mac_key[0]			 == AUTH_CARD_VERIFICATION)  )
	   {
		  /* Note: Dcpiso changes Card Verifies to Auths. The original tx_key is stored in mac_key.   */
		  temp_str[2] = '1';
	   }
	
	   /* Subfield 61.4 - Numeric Country Code where terminal is located. */
	   memset( tempstr, 0x00, sizeof(tempstr) );
	   memcpy( tempstr, auth_tx.MCF01_details.intl_code, 2 );
	
	   /* Convert 2-char alpha country code to 3-char numeric country code. */
	   genutil_get_country_code( A2N, tempstr, num_cc );
	   if ( num_cc[0] == 0x00 )
	   {
		  /* Default to Peso (608) for Equitable. Normally we would default to 900 for an unknown value.  */
		  strcpy( num_cc, "608" );
	   }
	   memcpy( temp_str+3, num_cc, 3 );
	
	  // HOST_MESSAGE.priv61_len[0] = 6;
	   memcpy( auth_tx.TLF01_details.product_codes[16].amount, temp_str, 6 );
}

INT dcpimp_check_DINERS_emv_capability_device(void)
{
	if( 0 == strncmp(auth_tx.TLF01_details.terminal_type ,
					 DINERS_EMV_CAPABILITY_INDICATOR,
					 DINERS_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN dcpimp_check_is_Diners_Magstrip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN dcpimp_check_is_Diners_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN dcpimp_check_is_Diners_Fallback_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN dcpimp_check_is_Diners_Contactless_Mastripe_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN dcpimp_check_is_Diners_Contactless_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
							DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
							DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

BOOLEAN dcpimp_check_is_Diners_Manual_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx.TLF01_details.pos_entry_mode,
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


/*******************************************************************************
 * NAME:        dcpimp_Prepare_posdata_for_DINERS
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
void dcpimp_Prepare_posdata_for_DINERS ()
{

	if(true == dcpimp_check_DINERS_emv_capability_device ())
	{
		if(true == dcpimp_check_is_Diners_Magstrip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101255140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Chip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101551140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Fallback_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101955140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101T65140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101S1140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Manual_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101664140",DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"06093S8065110",DINERS_DE22_SIZE);
		}
	}
	else
	{
		if(true == dcpimp_check_is_Diners_Magstrip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"260101255140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Chip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101551140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Fallback_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101955140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101T65140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Contactless_Chip_pos_transaction())
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"960101S1140",DINERS_DE22_SIZE);
		}
		else if(true == dcpimp_check_is_Diners_Manual_pos_transaction())
		{
				strncpy(auth_tx.TLF01_details.product_codes[16].amount,"260101664140",DINERS_DE22_SIZE);
		}
		else
		{
			strncpy(auth_tx.TLF01_details.product_codes[16].amount,"06093S8065110",DINERS_DE22_SIZE);
		}
	}
}



/*************************************************************************************
       NAME:           returnNumMP()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character , it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumMP(pCHAR input)
{
   INT cnt ;
   int count = 0;
   cnt = strlen(input);
   BYTE temp_str[3]="";
   strcpy(temp_str,input);
   while( count < cnt)
   {
		if((!isdigit(input[count])))
		{
			input[count] = '\0';
		}
		else
		{
			strcpy(input,temp_str+count);
			count = cnt;
		}
		count++;
   }
} /* end of returnNumdot*/




/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void dcpimp_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  dcpimp_error_warning_file_size[5] = {0};

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
								   dcpimp_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(dcpimp_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  dcpimp_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(dcpimp_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  dcpimp_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(dcpimp_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(dcpimp_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(dcpimp_error_warning_file_path) > 0) &&
	  (strlen(dcpimp_module_error_warning_file_name)> 0))
   {
	   dcpimp_create_Error_Filename();
   }
}

INT dcpimp_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;


	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(dcpimp_Erro_warning_Filename);
    path_len = strlen(dcpimp_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&dcpimp_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	dcpimp_create_Error_Filename();
    }
	if((fp = fopen(dcpimp_Erro_warning_Filename,"a+b"))==NULL)
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
		if (strlen(auth_tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( auth_tx.TLF01_details.card_num );
			strncpy( tempcard, auth_tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
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
		fclose(fp);
		return  0;
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

void dcpimp_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( dcpimp_Erro_warning_Filename, dcpimp_error_warning_file_path );
    strcat( dcpimp_Erro_warning_Filename, dcpimp_module_error_warning_file_name );
	strcat( dcpimp_Erro_warning_Filename, ".EW." );
    strncat(dcpimp_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(dcpimp_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(dcpimp_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         dcpimp_log_message
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
void dcpimp_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   dcpimp_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   if(details == 1)
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
			   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 auth_tx.TLF01_details.terminal_id,
										 auth_tx.TLF01_details.card_num,
										 auth_tx.TLF01_details.merchant_id );
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
BYTE dcpimp_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,
                                               pPTE_MSG *p_msg_in,
                                               pCHAR     que_name,
                                               pCHAR     error_msg )
{
   pBYTE           p_data;
   pPTE_MSG_DATA   p_msg_data;
   CHAR            temp_str[100] = {0};
   LONG            retcode;
   BYTE            dberror;

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to %s: %s",
               que_name, temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+sizeof(AUTH_TX), sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return dberror;
}

void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"AMEX",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for Amex" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &amex_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for AMEX\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for AMEX NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX", 0 );
	}
}

void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"CUP",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for CUP" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &cup_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for CUP\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for CUP NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_CUP", 0 );
	}

}

void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"DCI",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for DCI" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &dci_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for DCI\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for DCI NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_DCI", 0 );
	}

}

void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"JCB",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for JCB" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &jcb_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for JCB\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for JCB NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_JCB", 0 );
	}

}
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_MC()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"MC",2);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for MC" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &mc_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for mc\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for mc NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_MC", 0 );
	}

}
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"VISA",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for Visa" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &visa_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for Visa\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for Visa NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_VISA", 0 );
	}

}
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR()
{


	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FCLF01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX),"MP/PL/FC",8);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
					 ST1_DB_SELECT,
					 "",
					 NULL_PTR,
					 interactive_que_name,
					 (pBYTE) buffer,
					 sizeof( buffer),
					 FCLF01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FCLF01 for MP/PL/FC" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &iftr_fclf01, p_data+sizeof(AUTH_TX), sizeof(FCLF01) );
			free( p_msg_in );
			strcpy(error_msg , " fetched fclf01  details for  MP/PL/FC\n");
		}
		else
		{
			strcpy(error_msg , " fclf01  details for  MP/PL/FC NOT FOUND\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR", 0 );
	}



}

void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX)+sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"AMEX",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for Amex" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &amex_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );
		}
		if(atoi(amex_frual01.num_returned)  > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for AMEX\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01 the details for AMEX not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX", 0 );
	}

}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"CUP",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for CUP" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &cup_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );
		}
		if(atoi(cup_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for cup\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01 the details for CUP not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_CUP", 0 );
	}


}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI()
{
	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"DCI",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for DCI" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &dci_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(dci_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for DCI\n");
		}
		else
		{
			strcpy(error_msg , " fraul01 the details for DCI not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_DCI", 0 );
	}


}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"JCB",3);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for JCB" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &jcb_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(jcb_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for JCB\n");
		}
		else
		{
			strcpy(error_msg , " fraul01 the details for JCB not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_JCB", 0 );
	}


}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_MC()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"MC",2);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for MC" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &mc_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(mc_frual01.num_returned)>0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for MC\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01 the details for MC not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_MC", 0 );
	}


}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA()
{

	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"VISA",4);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for VISA" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &visa_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(visa_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for VISA\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01 the details for VISA not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_VISA", 0 );
	}


}
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_PL()
{


	pPTE_MSG		p_msg_out	= NULL_PTR;
	pPTE_MSG		p_msg_in	= NULL_PTR;
	BYTE			rcode		= 0;
	pBYTE         	p_data		= NULL_PTR;
    pPTE_MSG_DATA	p_msg_data	= NULL_PTR;
    BYTE            buffer[sizeof(AUTH_TX) + sizeof(FRULE01)] = {0};
    CHAR          	error_msg[256]  = {0} ;

    memcpy(buffer+sizeof(AUTH_TX)+20,"MP/PL/FC",8);
	p_msg_in  = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_GUI_LIST,
								  ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  FRULE01_DATA );

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message, while fetching FRULE01 for MP/PL/FC" );
	}
	else
	{
		rcode = dcpimp_Send_Interactive_DB_Request( p_msg_out,
										  	  	  	  	  &p_msg_in,
														  fconds_que_name,
														  error_msg );
		if ( rcode == PTEMSG_OK )
		{
			p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
			p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

			memcpy( &pl_frual01, p_data+sizeof(AUTH_TX), sizeof(FRULE01_GUI_LIST) );
			free( p_msg_in );

		}
		if(atoi(pl_frual01.num_returned) > 0)
		{
			strcpy(error_msg , " Fetched FRULE01 the details for  MP/PL/FC\n");
		}
		else
		{
			strcpy(error_msg , " FRULE01 the details for  MP/PL/FC not found\n");
		}
		dcpimp_log_message( 1, 1,error_msg, "Get_Falcon_RT_Configurerule_From_DB_For_PL", 0 );
	}



}

/*
 * Reading FCLF01 tabel for all tables
 * */
void dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands()
{
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX();
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP();
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI();
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB();
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_MC();
	//dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA();
	dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR();
}
/*
 *Reading FRUAL01 tabel for all tables
 * */
void dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands()
{
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX();
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP();
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI();
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB();
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_MC();
	//dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA();
	dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_PL();
}

/******************************************************************************
 *
 *  NAME:         CREATE_SYSTEM_DATE
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
void create_system_date( pBYTE system_date )
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

INT Dcpimp_If_Tx_Qualifies_For_RealTime()
{

	if (true == Dcpimp_check_link_for_NCIFTR() &&
		true == Dcpimp_verify_fraud_rules_for_NCIFTR())
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Dcpimp_check_link_for_NCIFTR(void)
{
	if ( ('1' == iftr_fclf01.fraud_control_enable_flag[0]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Dcpimp_verify_fraud_rules_for_NCIFTR(void)
{
	INT i = 0;
	long int txn_amt = 0;
	long int blng_amt = 0;
	BYTE hex_txn_amt[20] = {0};
	BYTE hex_blng_amt[20] = {0};
	INT rulematch = false;

	if ( ('0' == iftr_fclf01.fraud_rule_enable_flag[0] ))
	{
		return true;
	}

	else
	{
		for (i=0; i<10; i++)
		{

			if(pl_frual01.frule01_list[i].fraud_rule[0] != '\0')
			{
				if (0 != strlen(pl_frual01.frule01_list[i].fraud_rule))
				{
				if (0 == strncmp(auth_tx.TLF01_details.currency_code,
						pl_frual01.frule01_list[i].acq_country_code, 3))
				{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].billing_amt))
				{
					strncpy(hex_txn_amt, auth_tx.TLF01_details.total_amount, 12);
					txn_amt =  atoi(hex_txn_amt);

					strncpy(hex_blng_amt, pl_frual01.frule01_list[i].billing_amt, 12);
					blng_amt =  atoi(hex_blng_amt);

					if (txn_amt >= blng_amt)
					{
						return true;
					}
				}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_1))
				{
					if ((0 == strcmp(auth_tx.TLF01_details.category_code,
									pl_frual01.frule01_list[i].mcc_1)))
					{
						return true;
					}
					}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_2))
				{
					if ((0 == strcmp(auth_tx.TLF01_details.category_code,
									 pl_frual01.frule01_list[i].mcc_2)))
					{
						return true;
					}
					}

				if (0 != strlen(pl_frual01.frule01_list[i].mcc_3))
					{
						if ((0 == strcmp(auth_tx.TLF01_details.category_code,
									pl_frual01.frule01_list[i].mcc_3)))
							{
						return true;
							}
						}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_1))
						{
							if ((0 == strncmp(auth_tx.TLF01_details.pos_entry_mode,
									 pl_frual01.frule01_list[i].pos_mode_1, 3)))
							{
						return true;
					}
							}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_2))
				{
					if ((0 == strncmp(auth_tx.TLF01_details.pos_entry_mode,
									 pl_frual01.frule01_list[i].pos_mode_2, 3)))
							{
						return true;
							}
						}

				if (0 != strlen(pl_frual01.frule01_list[i].pos_mode_3))
				{
					if ((0 == strncmp(auth_tx.TLF01_details.pos_entry_mode,
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


/*************************************************************************************
 * NAME:		Dcpimp_Send_Request_To_RT_Falcon_Service
 * DESCRIPTION:	Send AUTH_TX structure to RT Falcon service
 * INPUTS:		None
 * OUTPUTS:		None
 * RETURNS:     true   if forwarded; false  otherwise
 * AUTHOR:		Phani
 * MODIFICATIONS:
 ************************************************************************************/
BOOLEAN Dcpimp_Send_Request_To_RT_Falcon_Service ( void )
{
	BOOLEAN  retval = true;
	INT      nBufferSize;
	LONG     lReturnCode  = 0L;
	BYTE     ReturnCode   = 0;
	pPTE_MSG p_msg_out    = 0;
	CHAR     strTemp[256] = {0};
	CHAR     strError[512]= "";

	/*For MP transactions Acquiring ID will be NCIFTR*/
	strcpy( auth_tx.TLF01_details.acquiring_id, "NCIFTR");

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &auth_tx, sizeof(AUTH_TX ) );
	nBufferSize = sizeof( AUTH_TX );

#ifdef FALCON_CONTROLLER
	p_msg_out = ptemsg_build_msg( MT_AUTH_RT_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  falcntl_que_name,
								  nciftr_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", falcntl_que_name );

		sprintf( strError, "Error on pteipc_send to que %s: %s",
				falcntl_que_name, strError );

		dcpimp_log_message( 1, 2, strError,"Dcpimp_Send_Request_To_RT_Falcon_Service", 0 );
		retval = DEVICE_ERROR;
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, falcntl_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,falcntl_que_name);

			pteipc_get_errormsg (lReturnCode, strTemp);
			sprintf( strError,"Error on pteipc_send to que %s: %s",
					falcntl_que_name, strError );
			dcpimp_log_message( 1, 2, strError,"Dcpimp_Send_Request_To_RT_Falcon_Service", 0 );
			retval = DEVICE_ERROR;
		}
		else
		free( p_msg_out );
	}
#else
	p_msg_out = ptemsg_build_msg( MT_AUTH_RT_REQUEST,
								  ST1_NONE,
								  ST2_NONE,
								  faisnd_que_name,
								  nciftr_que_name,
								  (pBYTE) p_buffer,
								  nBufferSize,
								  0 );
	if (p_msg_out == NULL)
	{
		sprintf( strTemp,"Mem allocate Err build msg for %s", faisnd_que_name );

		sprintf( strError, "Error on pteipc_send to que %s: %s",
				faisnd_que_name, strError );

		dcpimp_log_message( 1, 2, strError,"Dcpimp_Send_Request_To_RT_Falcon_Service", 0 );
		retval = DEVICE_ERROR;
	}
	else
	{
		lReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
		if (lReturnCode != 0)
		{
			sprintf(strTemp,"Err %ld sending msg to %s", lReturnCode,faisnd_que_name);

			pteipc_get_errormsg (lReturnCode, strTemp);
			sprintf( strError,"Error on pteipc_send to que %s: %s",
					faisnd_que_name, strError );
			dcpimp_log_message( 1, 2, strError,"Dcpimp_Send_Request_To_RT_Falcon_Service", 0 );
			retval = DEVICE_ERROR;
		}
		else
		{
			/*   sprintf( strTemp,"Succusfully send request to FALCON %s", faisnd_que_name );
			ReturnCode = TxService_Generate_Usage_Error_Message
			(1,strTemp, "Dcpimp_Send_Request_To_RT_Falcon_Service",
			GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION );*/

		}
		free( p_msg_out );
	}
#endif

	return( retval );
} /* Dcpimp_Send_Request_To_RT_Falcon_Service */
