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


extern AUTH_TX auth_tx;				

extern CHAR    ncvisa_que_name [];
extern CHAR		visa1_que_name[];
extern CHAR		authorizer_que_name[];

extern BYTE		ReqRsp_Buffer[]; 
extern INT		buflen;
extern CHAR     emn_contactless_offline_sale_authID[3];
extern CHAR     validate_preauth_validation_processing_code[7];
extern CHAR     CUP_merchant_indicator[8];

extern CHAR  dcpiso_Erro_warning_Filename[256] ;
extern CHAR  dcpiso_module_error_warning_file_name[256];
extern CHAR  dcpiso_error_warning_file_path[256];

UINT Max_File_Size_Defined = 0 ;

extern char mc_timeout_rc[3];
extern char visa_timeout_rc[3];
extern char amex_timeout_rc[3];
extern char jcb_timeout_rc [3];
extern char cup_timeout_rc [3];
extern char dci_timeout_rc [3];

#if BDOR_62_PREAUTH_VOID

extern CHAR declined_rc_for_expired_used_preauths_for_MC[3];
extern CHAR declined_rc_for_expired_used_preauths_for_VISA[3];
extern CHAR declined_rc_for_expired_used_preauths_for_AMEX[3];
extern CHAR declined_rc_for_expired_used_preauths_for_DCI[3];
extern CHAR declined_rc_for_expired_used_preauths_for_JCB[3];
extern CHAR declined_rc_for_expired_used_preauths_for_UPI[3];

extern CHAR declined_rc_for_original_txn_not_present_for_MC[3];
extern CHAR declined_rc_for_original_txn_not_present_for_VISA[3];
extern CHAR declined_rc_for_original_txn_not_present_for_AMEX[3];
extern CHAR declined_rc_for_original_txn_not_present_for_DCI[3];
extern CHAR declined_rc_for_original_txn_not_present_for_JCB[3];
extern CHAR declined_rc_for_original_txn_not_present_for_UPI[3];

extern CHAR  flag_for_sending_partial_reversals_for_MC[2];
extern CHAR  flag_for_sending_partial_reversals_for_VISA[2];
extern CHAR  flag_for_sending_partial_reversals_for_AMEX[2];
extern CHAR  flag_for_sending_partial_reversals_for_DCI[2];
extern CHAR  flag_for_sending_partial_reversals_for_JCB[2];

extern CHAR smcc_id1[5];
extern CHAR smcc_id2[5];
extern CHAR smcc_id3[5];
extern CHAR smcc_id4[5];
extern CHAR smcc_id5[5];
extern CHAR smcc_id6[5];
extern CHAR smcc_id7[5];
extern CHAR smcc_id8[5];
extern CHAR smcc_id9[5];
extern CHAR smcc_id10[5];

#endif

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

	if (strEqual (strResponseCode, TRANSACTION_APPROVED))
	{
		return DEVICE_OK ;
	}
	else if((strEqual (strResponseCode, PIN_TRIES_EXCEEDED)
			|| strEqual (strResponseCode, INCORRECT_PIN)) &&
			false == dcpiso_check_If_VISA_Transaction())
	{
		return DEVICE_OK ;
	}

	/* Voids response should be sent as it is coming from the CUP HOST */
	if ( (auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE))
	{
		if(true == dcpiso_Check_If_CUP_Transaction())
		{
			return DEVICE_OK ;
		}
	}
	return DEVICE_ERROR ;
}

INT dcpiso_Check_If_CUP_Transaction (void)
{
	if (0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(auth_tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == dcpiso_check_If_VISA_Transaction ()) ||
		   (true == dcpiso_check_If_MC_Transaction()))
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
INT dcpiso_check_If_VISA_Transaction (void)
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
INT dcpiso_check_If_MC_Transaction (void)
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
INT dcpiso_check_If_JCB_Transaction (void)
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
INT dcpiso_check_If_AMEX_Transaction (void)
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

INT dcpiso_check_If_Diners_Transaction (void)
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

INT Check_If_Transaction_qualifies_for_Preauth_validation (void)
{
	if(((0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"Y",1)) ||
		(0 == strncmp(auth_tx.MCF01_details.network[7].priority_routing,"y",1))) &&
		(0 == strncmp(auth_tx.TLF01_details.processing_code,validate_preauth_validation_processing_code,
				PROCESSING_CODE_LEN)) &&
	    (0 != strncmp(auth_tx.TLF01_details.auth_number,emn_contactless_offline_sale_authID,2)))
	{
		return true ;
	}
	else
	{
		return false;
	}
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

   #define  NUM_RC  75
   INT   i;
   INT   rc_found;
   CHAR BUFF[100]={0};
   BYTE  valid_rc[NUM_RC][3] =
      {
         "00","01","02","03","04","05","06","07","08","09",
		 "10","11","12","13","14","19","21","25","30","31",
		 "39","41","43","45","46","51","52","53","54","55",
		 "56","57","58","59","61","62","63","65","70","71",
		 "75","76","77","78","79","80","81","82","83","84",
		 "85","86","88","89","91","94","95","96","6P","R0",
		 "R1","R3","N7","1A","N3","N4","15","93","A2","A4",
		 "A5","A6","Z5","5C","9G"
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
             (0 == strcmp(resp_code, "NC")) ||
             (0 == strcmp(resp_code, "Z1"))  )
   {
 	  sprintf(BUFF, "Wrong RC= %c",resp_code);
      strcpy( resp_code, SYSTEM_MALFUNCTION );
	  dcpiso_log_message( 1, 3, BUFF,"Map_Response_Code",1 );
   }
   // TF Abhishek - Visa 2016 April Madated changes .
   //Change response codes "01", "02" in ATP STIP Processing to "05"
   else if((0 == strcmp (auth_tx.TLF01_details.product_codes[1].code,STANDIN)) &&
           (0 == strncmp(auth_tx.TLF01_details.card_num,"4",1)) &&
		   ((0== strncmp(auth_tx.TLF01_details.response_code ,REFER_TO_CARD_ISSUER,2))||
		   (0 == strncmp(auth_tx.TLF01_details.response_code ,RCI_SPECIAL_CONDITION,2))))
	{
			strncpy(auth_tx.TLF01_details.response_code,DO_NOT_HONOR,2);
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
      {
         /* Voids response should be sent as it is coming from the CUP HOST */
		if ( (auth_tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE))
		{
			if(true == dcpiso_Check_If_CUP_Transaction())
			{
				rc_found = true;
				return ;
			}
		}
		 sprintf(BUFF, "could found RC= %c",resp_code);
         strcpy( resp_code, SYSTEM_MALFUNCTION );
		 dcpiso_log_message( 1, 3, BUFF,"Map_Response_Code",1 );
      	}
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

    dcpiso_log_message( 1, 3, strTemp,function,1 );
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

#if BDOR_62_PREAUTH_VOID

void prepare_authtx_for_forwarding_preauth_void_original_processed ()
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_PROCESSED_FOR_VOID_PREAUTH ;

} /* prepare_authTx_for_forwarding_preauth_void_original_processed */


void prepare_authtx_for_forwarding_preauth_void_original_posted (BYTE general_status)
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_POSTED_FOR_VOID_PREAUTH ;

} /* prepare_authtx_for_forwarding_preauth_void_original_posted */

void prepare_authtx_for_forwarding_preauth_void_original_expired (BYTE general_status)
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_EXPIRED_FOR_VOID_PREAUTH ;

} /* prepare_authtx_for_forwarding_preauth_void_original_expired */

void prepare_authtx_for_forwarding_preauth_void_original_voided (BYTE general_status)
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_VOIDED_FOR_VOID_PREAUTH ;

} /* prepare_authtx_for_forwarding_preauth_void_original_voided */

void prepare_authtx_for_forwarding_preauth_void_original_reversed (BYTE general_status)
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_expired_used_preauths_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_REVERSED_FOR_VOID_PREAUTH ;

} /* prepare_authtx_for_forwarding_preauth_void_original_reversed */

void prepare_authtx_for_forwarding_preauth_void_original_not_present (void)
{
	if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_MC,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_VISA,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_AMEX_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_AMEX,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_DCI,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_JCB,
				 ISO_RESPONSE_CODE_LEN) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction())
	{
		strncpy (auth_tx.TLF01_details.response_code,
				 declined_rc_for_original_txn_not_present_for_UPI,
				 ISO_RESPONSE_CODE_LEN) ;
	}

	auth_tx.TLF01_details.general_status = GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH ;

} /* prepare_authtx_for_forwarding_preauth_void_original_reversed */

#endif

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
	ReturnCode = Send_Request_To_Service (  authorizer_que_name,
											MT_AUTH_REQUEST, 
											0, 
											0, 
											0, 
											0, 
											0) ;
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
         {
            ret_val = 1;
      }
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
      case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
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
      dcpiso_log_message( 1, 3, strError,"send_0302_to_network_controller",1 );
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
         dcpiso_log_message( 1, 3, strError,"send_0302_to_network_controller",0 );
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


/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void dcpiso_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  dcpiso_error_warning_file_size[5] = {0};

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
								   dcpiso_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(dcpiso_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  dcpiso_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(dcpiso_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  dcpiso_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(dcpiso_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(dcpiso_error_warning_file_size);
	if(Max_File_Size_Defined <=  0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(dcpiso_error_warning_file_path) > 0) &&
	  (strlen(dcpiso_module_error_warning_file_name)> 0))
   {
	   dcpiso_create_Error_Filename();
   }
}

INT dcpiso_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail)
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
    len = strlen(dcpiso_Erro_warning_Filename);
    path_len = strlen(dcpiso_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&dcpiso_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	dcpiso_create_Error_Filename();
    }
	if((fp = fopen(dcpiso_Erro_warning_Filename,"a+b"))==NULL)
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

void dcpiso_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( dcpiso_Erro_warning_Filename, dcpiso_error_warning_file_path );
    strcat( dcpiso_Erro_warning_Filename, dcpiso_module_error_warning_file_name );
	strcat( dcpiso_Erro_warning_Filename, ".EW." );
    strncat(dcpiso_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(dcpiso_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(dcpiso_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/******************************************************************************
 *
 *  NAME:         dcpiso_log_message
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
void dcpiso_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   dcpiso_Log_error_warning_to_File(text_message,sev,func,details);
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


INT dcpiso_check_If_Tra_qualify_MC_512_handaling (void)
{
	double total_preauth_amount = 0;
	double preauth_amount = 0;
	double total_offline_amount = 0;

	BYTE host_que_name[8] = {0};

	preauth_amount = strtod(auth_tx.TLF01_details.orig_amount, 0);
	total_offline_amount = strtod(auth_tx.TLF01_details.total_amount,0);
	total_preauth_amount = strtod(auth_tx.TLF01_details.product_codes[19].amount,0);

	if(	0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return (false);
	}
	else if((true == dcpiso_check_If_MC_Transaction()) &&
			(0 == strncmp(flag_for_sending_partial_reversals_for_MC ,
					PARTIAL_REVERSAL_FLAG_ON,
					PARTIAL_REVERSAL_FLAG_LEN)))
	{
		/* Lets check what is the Preauth amount & Off line sale amount*/
		if(total_preauth_amount > total_offline_amount)
		{
			/* We need to send reversal to MC*/
			if((0 == strncmp(auth_tx.CAF01_details.destination_name,"ncmcrd",6))||
			   (0 == strncmp(auth_tx.BIN01_details.destination_name,"ncmcrd",6))||
			   (0 == strncmp(auth_tx.TLF01_details.product_codes[5].code,"OFFUS",5)))
			{
				strncpy(host_que_name,"ncmcrdA",7);
			}
			else
			{
				strncpy(host_que_name,"ncmcr2A",7);
			}
			strncpy(auth_tx.TLF01_details.message_type,"0400",4);
			strcpy(auth_tx.TLF01_details.response_text,"Preauth amount > completion amount");
			/* Set a flag for partial reversal*/
			strncpy( auth_tx.TLF01_details.product_codes[3].quantity,"PR",2  );
			auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
			dcpiso_generate_tranasction_id();
			auth_tx.host2_acquirer_cb_code[0] = 0x00;
			Send_partial_reversal_to_host(&host_que_name);

		}
		else
		{
			/* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed*/
		   Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		}
	}

#if AMEX03B_APRIL2024
	else if((true == dcpiso_check_If_AMEX_Transaction()) &&
			(0 == strncmp(flag_for_sending_partial_reversals_for_AMEX ,
					PARTIAL_REVERSAL_FLAG_ON,
					PARTIAL_REVERSAL_FLAG_LEN)))
	{
		/* Lets check what is the Preauth amount & Off line sale amount*/
		if(total_preauth_amount > total_offline_amount)
		{
			/* We need to send reversal to Amex*/
			if((0 == strncmp(auth_tx.CAF01_details.destination_name,"ncposa",6))||
			   (0 == strncmp(auth_tx.BIN01_details.destination_name,"ncposa",6))||
			   (0 == strncmp(auth_tx.TLF01_details.product_codes[5].code,"OFFUS",5)))
			{
				strncpy(host_que_name,"ncposaA",7);
			}
			else
			{
				strncpy(host_que_name,"nposa2A",7);
			}
			strncpy(auth_tx.TLF01_details.message_type,"1420",4);
			strncpy(auth_tx.TLF01_details.processing_code,"024000",6);
			auth_tx.TLF01_details.saf[0] = PARTIAL_REVERSAL;
			strcpy(auth_tx.TLF01_details.response_text,"Preauth amount > completion amount");
			/* Set a flag for partial reversal
			strncpy( auth_tx.TLF01_details.product_codes[3].quantity,"PR",2  );*/
			auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
			dcpiso_generate_tranasction_id();
			auth_tx.host2_acquirer_cb_code[0] = 0x00;
			Send_partial_reversal_to_host(&host_que_name);

		}
		else
		{
			/* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed*/
		   Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		}
	}
#endif

#if BDOR_62_PREAUTH_VOID
	else if((true == dcpiso_check_If_VISA_Transaction())&&
			(0 == strncmp(flag_for_sending_partial_reversals_for_VISA ,
					PARTIAL_REVERSAL_FLAG_ON,
					PARTIAL_REVERSAL_FLAG_LEN)))
	{
		/* Lets check what is the Preauth amount & Off line sale amount*/
		if(total_preauth_amount > total_offline_amount)
		{
			/* We need to send reversal to Visa*/
			if((0 == strncmp(auth_tx.CAF01_details.destination_name,"ncvisa",6))||
			   (0 == strncmp(auth_tx.BIN01_details.destination_name,"ncvisa",6))||
			   (0 == strncmp(auth_tx.TLF01_details.product_codes[5].code,"OFFUS",5)))
			{
				strncpy(host_que_name,"ncvisaA",7);
			}
			else
			{
				strncpy(host_que_name,"ncvis2A",7);
			}
			strncpy(auth_tx.TLF01_details.message_type,"0400",4);
			strcpy(auth_tx.TLF01_details.response_text,"Preauth amount > completion amount");
			/* Set a flag for partial reversal*/
			strncpy( auth_tx.TLF01_details.product_codes[3].quantity,"PR",2  );
			auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
			dcpiso_generate_tranasction_id();
			auth_tx.host2_acquirer_cb_code[0] = 0x00;
			Send_partial_reversal_to_host(&host_que_name);
		}
		else
		{
			/* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed*/
			Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		}
	}
	else if((true == dcpiso_check_If_Diners_Transaction()) &&
			(0 == strncmp(flag_for_sending_partial_reversals_for_DCI ,
					PARTIAL_REVERSAL_FLAG_ON,
					PARTIAL_REVERSAL_FLAG_LEN)))
	{
		/* Lets check what is the Preauth amount & Off line sale amount*/
		if(total_preauth_amount > total_offline_amount)
		{
			/* We need to send reversal to DCI*/
			if((0 == strncmp(auth_tx.CAF01_details.destination_name,"ncdci2",6))||
			   (0 == strncmp(auth_tx.BIN01_details.destination_name,"ncdci2",6))||
			   (0 == strncmp(auth_tx.TLF01_details.product_codes[5].code,"ONUS",4)))
			{
				strncpy(host_que_name,"ncdci2A",7);
			}
			else
			{
				strncpy(host_que_name,"ncdciA",6);
			}
			strncpy(auth_tx.TLF01_details.message_type,"0400",4);
			strcpy(auth_tx.TLF01_details.response_text,"Preauth amount > completion amount");
			/* Set a flag for partial reversal*/
			strncpy( auth_tx.TLF01_details.product_codes[3].quantity,"PR",2  );
			auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
			dcpiso_generate_tranasction_id();
			auth_tx.host2_acquirer_cb_code[0] = 0x00;
			Send_partial_reversal_to_host(&host_que_name);
		}
		else
		{
			/* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed*/
			Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		}
	}
	else if((true == dcpiso_check_If_JCB_Transaction()) &&
			(0 == strncmp(flag_for_sending_partial_reversals_for_JCB ,
					PARTIAL_REVERSAL_FLAG_ON,
					PARTIAL_REVERSAL_FLAG_LEN)))
	{
		/* Lets check what is the Preauth amount & Off line sale amount*/
		if(total_preauth_amount > total_offline_amount)
		{
			/* We need to send reversal to DCI*/
			if((0 == strncmp(auth_tx.CAF01_details.destination_name,"ncjcb2",6))||
			   (0 == strncmp(auth_tx.BIN01_details.destination_name,"ncjcb2",6))||
			   (0 == strncmp(auth_tx.TLF01_details.product_codes[5].code,"ONUS",4)))
			{
				strncpy(host_que_name,"ncjcb2A",7);
			}
			else
			{
				strncpy(host_que_name,"ncjcbA",6);
			}
			strncpy(auth_tx.TLF01_details.message_type,"0400",4);
			strcpy(auth_tx.TLF01_details.response_text,"Preauth amount > completion amount");
			/* Set a flag for partial reversal*/
			strncpy( auth_tx.TLF01_details.product_codes[3].quantity,"PR",2  );
			auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
			dcpiso_generate_tranasction_id();
			auth_tx.host2_acquirer_cb_code[0] = 0x00;
			Send_partial_reversal_to_host(&host_que_name);
		}
		else
		{
			/* Lets update the BCH20 product5_code as procsd to indicate this transaction is processed*/
			Update_BCH20_Details_for_OFFLINE_Sale(ST2_DB_SELECT_BCH20_FOR_PREAUTH);
		}
	}
#endif
	else
	{
		return (false);
	}
}

INT dcpiso_check_If_Tra_is_Offline_Sale (void)
{
	if(((0 == strncmp(auth_tx.TLF01_details.message_type, "0220",4)) ||
	    (0 == strncmp(auth_tx.TLF01_details.message_type, "0230",4)))&&
	  (0 == strncmp(auth_tx.TLF01_details.processing_code,"00",2)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT dcpiso_check_If_Tra_qualify_MC_512_incremental_Preauth(void)
{

	if(	0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return (false);
	}

	else if((true == dcpiso_check_If_MC_Transaction()))
	{
		return (true);
	}
#if AMEX03B_APRIL2024
	else if((true == dcpiso_check_If_AMEX_Transaction()))
	{
		return (true);
	}
#endif

#if BDOR_62_PREAUTH_VOID

	else if((true == dcpiso_check_If_VISA_Transaction()))
	{
		return (true);
	}
	else if((true == dcpiso_check_If_JCB_Transaction()))
	{
		return (true);
	}
	else if((true == dcpiso_check_If_Diners_Transaction()))
	{
		return (true);
	}
#endif
	else
	{
		return (false);
	}
}

INT dcpiso_check_If_Tra_qualify_for_incremental_Preauth(void)
{

	if(	0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return (false);
	}

	else if(true == dcpiso_check_If_MC_Transaction())
	{
		return (true);
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		return (true);
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		return (true);
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}



BYTE Send_partial_reversal_to_host( char *host_que_name)
{
	BYTE ReturnCode = 0 ;
	BYTE strTemp[100]= {0};
	BYTE network_name[8] = {0};

	strncpy(network_name,host_que_name,(sizeof(network_name) -1));
	if ( true == Check_Is_Host_Up(network_name))
    {
		 ReturnCode = Send_Request_To_Service(host_que_name,
				 	 	 	 	 	 	 	  MT_AUTH_REQUEST,
								  			  0,
											  0,
											  0,
											  0,
											  0) ;
	    if (ReturnCode == false)
	    {
	 	    sprintf(strTemp,"error in sending data to %s host",host_que_name) ;
     	    dcpiso_log_message( 1, 3, strTemp," Send_partial_reversal_to_host ", 1 );
			return (ReturnCode) ;
        }
	    else
	    {
			return (ReturnCode) ;
	    }
    }
	else if ((0 == strncmp(host_que_name, "ncmcr2",6))||
			 (0 == strncmp(host_que_name, "ncvis2",6)) ||
			 (0 == strncmp(host_que_name, "nposa2",6)) ||
			 (0 == strncmp(host_que_name, "ncjcb2",6)) ||
			 (0 == strncmp(host_que_name, "ncdci2",6)))
	{
		ReturnCode = Send_Transaction_To_Authorizer();
		if(ReturnCode != DEVICE_OK)
		{
			dcpiso_log_message( 1, 3, "Failed to send partial reversal ", "Send_partial_reversal_to_host",1 );
	    }
		return (ReturnCode);
    }
}

BYTE Check_Is_Host_Up(BYTE* destination_name)
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

	 		if (strEqual (auth_tx.TLF01_details.handler_queue, "VOICE"))
			strcpy (auth_tx.TLF01_details.voice_auth_text, strTemp) ;
			dcpiso_log_message( 1, 2, strTemp, " Check_Is_Host_Up ", 0 );
		}
		else
		{
			if ( strEqual(state, ONLINE) )
				ret_val = true;
	   }
	}

	return( ret_val );
}

void dcpiso_generate_tranasction_id( void)
{

	INT   unique_tran_str_id_len = 0;
	INT   tid_len = 0;
	CHAR  unique_tran_str_id[50] = {0} ;
	CHAR  temp_unique_tran[15] = {0};
	CHAR  date_str [ 9] = {0};
	CHAR  time_date[25] = {0};

	ptetime_get_strtime (unique_tran_str_id) ;
	memset (auth_tx.TLF01_details.primary_key.transaction_id, 0,
		    sizeof (auth_tx.TLF01_details.primary_key.transaction_id)) ;
	tid_len= strlen(auth_tx.TLF01_details.terminal_id);
	if(tid_len == 8)
	{
		memcpy (auth_tx.TLF01_details.primary_key.transaction_id,
			   auth_tx.TLF01_details.terminal_id, sizeof(auth_tx.TLF01_details.terminal_id) - 1) ;

		strncat (auth_tx.TLF01_details.primary_key.transaction_id, unique_tran_str_id+2,12) ;
	}
	else
	{
	   /* We have to now prepare 20 digit unique Transaction ID.
		 unique_tran_str_id will be having 14 digit length
		 now we will make it as 20 by adding YYMMDD in the beginning of it.
		 20 byte will look like
		 YYMMDD"unique_tran_str_id"
		 for example if unique_tran_str_id is "1444752896.334" then it will become
		 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strcpy(temp_unique_tran,unique_tran_str_id);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));
	   ptetime_get_timestamp( time_date );
	   get_date( time_date, date_str );
	   strncpy(unique_tran_str_id, date_str+2, 6); //YYMMDD
	   strcat (unique_tran_str_id,temp_unique_tran);

	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy(auth_tx.TLF01_details.primary_key.transaction_id,
			   auth_tx.TLF01_details.terminal_id, tid_len );
	   strncat( auth_tx.TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+tid_len,
				(unique_tran_str_id_len-tid_len) );
	}

}
void dcpiso_update_TO_rc_basedon_card_brand(CHAR *resp_code)
{

	if( true == dcpiso_check_If_AMEX_Transaction() )
	{
		strncpy (resp_code,amex_timeout_rc,2) ;
	}
	else if(true == dcpiso_Check_If_CUP_Transaction() )
	{
		strncpy (resp_code,cup_timeout_rc,2) ;
	}
	else if(true == dcpiso_check_If_VISA_Transaction())
	{
		strncpy (resp_code,visa_timeout_rc,2) ;
	}
	else if(true == dcpiso_check_If_MC_Transaction())
	{
		strncpy (resp_code,mc_timeout_rc,2) ;
	}
	else if(true == dcpiso_check_If_JCB_Transaction())
	{
		strncpy (resp_code,jcb_timeout_rc,2) ;
	}
	else if(true == dcpiso_check_If_Diners_Transaction())
	{
		strncpy (resp_code,dci_timeout_rc,2) ;
	}

}

INT isLeap(int year)
{
	if (((year % 4   == 0)    &&
         (year % 100 != 0)) ||
         (year % 400 == 0))
	{
		return true;
	}
	return false;
}



int calculate_julian_date(int mm,int dd, int year )
{
	int month[12] ={31,28,31,30,31,30,31,31,30,31,30,31};
	int Julian_date=0;

	for(int i=1;i<mm;i++)
	{
		Julian_date = Julian_date+month[i];
	}

	if (true==isLeap(year))
	{
		if(mm==2 && dd==29)
		{
			Julian_date= Julian_date+dd+1;
		}
		else if(mm> 2)
		{
			Julian_date= Julian_date+dd+1;
		}
	}
	else
	{
		Julian_date= Julian_date+dd;
	}

	return Julian_date;

}

#if OCT_MAN_2024_BLD_2
INT dcpiso_check_if_mid_is_smcc ()
{
	if(	(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id1, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id2, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id3, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id4, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id5, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id6, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id7, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id8, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id9, MID_MCC_LEN))||
		(0 == strncmp(auth_tx.TLF01_details.category_code,smcc_id10,MID_MCC_LEN)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
#endif
