/***************************************************************************
 *
 * Copyright (c) 2001, ThoughtFocus, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci2_security.c
 *  
 * TITLE:       Diners Network Controller Security Processor
 *  
 * DESCRIPTION: This module contains functions that handle transaction
 *              security: request for pin translation and response.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Abhishek Verma
 *
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "diners.h"
#include "ncdci2_prototypes.h"
#include "tx_dbstruct.h"

/*+--------------------+
  | Global Definitions |
  +--------------------+*/

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_I_key_exchange;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;

extern NCF01_KEYBLOCK     Ncf01_Keyblock;
extern NCF01_KEYBLOCK     Ncf01_I_key_exchange_Keyblock;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;

/* QUEUES */
extern CHAR nsp_que_name[];
CHAR masked_Key_value[49] = {0};
CHAR Key_value[49] = {0};

extern INT keyblock_enable_flag_for_DCI;

/******************************************************************************
 *
 *  NAME:         ncdci2_translate_pin
 *
 *  DESCRIPTION:  This function sends an Auth_Tx to the racal service.
 *                The message type indicates to racal to perform a pin
 *                translation.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction Info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_translate_pin()
{
	INT   ret_val = false;
	CHAR  err_buf[100] = {0};
	CHAR  buffer[256] = {0};

	CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};

	memset( buffer1, 0x00, sizeof( buffer1 ) );
	memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
	memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );
	ret_val = ncdci2_build_and_send_request_message( MT_NSP_TRANSLATE_PIN,
													 ST1_NONE,
													 ST2_NONE,
													 nsp_que_name,
													 (pBYTE)buffer1,
													 sizeof(buffer1),
													 0,
													 NULL_STR,
													 err_buf );
	if ( ret_val == false )
	{
		memset( buffer, 0x00, sizeof(buffer) );
		sprintf( buffer,"Unable to translate pin. Err: %s",	err_buf );
		ncdci2_log_message( 1, 3, buffer, "ncdci2_translate_pin",1 );
	}
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci2_translate_pin_keyblock
 *
 *  DESCRIPTION:  This function sends an Auth_Tx to the racal service.
 *                The message type indicates to racal to perform a pin
 *                translation.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction Info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_translate_pin_keyblock()
{
	INT   ret_val = false;
	CHAR  err_buf[100] = {0};
	CHAR  buffer[256] = {0};

	CHAR  buffer2[AUTH_TX_SIZE + NCF01_KEYBLOCK01_SIZE] = {0};

   memset( buffer2, 0x00, sizeof( buffer2 ) );
   memcpy( buffer2, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer2+Auth_Tx_Size, &Ncf01_Keyblock, NCF01_KEYBLOCK01_SIZE );
   ret_val = ncdci2_build_and_send_request_message( MT_NSP_TRANSLATE_PIN_KEYBLOCK,
													ST1_NONE,
													ST2_NONE,
													nsp_que_name,
													(pBYTE)buffer2,
													sizeof(buffer2),
													0,
													NULL_STR,
													err_buf );

	if ( ret_val == false )
	{
		memset( buffer, 0x00, sizeof(buffer) );
		sprintf( buffer,"Unable to translate pin. Err: %s",	err_buf );
		ncdci2_log_message( 1, 3, buffer, "ncdci2_translate_pin_keyblock",1 );
	}
   return;
}


/******************************************************************************
*
*  NAME:         ncdci2_process_translate_pin_response
*
*  DESCRIPTION:  Process a response from the security application for a pin
*                translation.  If the translation failed, stand in for the
*                transaction - decline it and respond to the authorizer.  If
*                successful, continue processing switched out request to host.
*
*  INPUTS:       p_msg - PTE message from security application
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Abhishek Verma
*
******************************************************************************/
void ncdci2_process_translate_pin_response( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data = NULL_PTR;
   LONG            data_len = 0;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   /* extract auth_tx structure from nsp reply message */
   memset( &Auth_Tx, 0x00,   Auth_Tx_Size );
   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

   /* Was pin translated? */
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, DINERS_RACAL_APPROVED) )
   {
      /* Pin was successfully translated.
       * Continue processing transaction.
       */
      (void)ncdci2_process_request_for_host_after_security();
   }
   else
   {
      /* Pin translation failed, send response to authorizer */
      ncdci2_log_message( 2, 3, "Pin translation failed.",
                     "ncdci2_process_translate_pin_response",1 );
      ncdci2_perform_error_processing( DINERS_SYSTEM_ERROR );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci2_key_translate
 *
 *  DESCRIPTION:  This function sends an Auth_Tx to the racal service.
 *                The message type indicates to racal to perform a pin
 *                translation.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction Info
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Kumar Verma
 *
 ******************************************************************************/

void ncdci2_key_translate()
{
   INT   ret_val;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};

   // Copy ZMK defined in the issuing page of NCDCI2
   strcpy(Auth_Tx.TLF01_details.dest_key, Ncf01_I.zmk);
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );

   ret_val = ncdci2_build_and_send_request_message( MT_NSP_TRANSLATE_ZMK_LMK,
											 ST1_NONE,
											 ST2_NONE,
											 nsp_que_name,
											 (pBYTE)buffer1,
											 sizeof(buffer1),
											 0,
											 NULL_STR,
											 err_buf );

 if ( ret_val == false )
   {
	  sprintf( buffer,
			  "Unable to change key. Err: %s",
			   err_buf );
	  ncdci2_log_message( 1, 3, buffer, "ncdci2_key_translate", 0 );
   }
   else
   {
		sprintf( buffer,
				"key Translate request sent to HSM: %s",
				err_buf );
				ncdci2_log_message( 2, 1, buffer, "ncdci2_key_translate", 0 );
   }
}



/******************************************************************************
*
*  NAME:         ncdci2_process_key_translate_response
*
*  DESCRIPTION:  Process a response from the security application for a pin
*                translation.  If the translation failed, stand in for the
*                transaction - decline it and respond to the authorizer.  If
*                successful, ncdci_update_ncf01 with translated key.
*
*  INPUTS:       p_msg - PTE message from security application
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   None
*
*  AUTHOR:      Abhishek Kumar Verma
*
******************************************************************************/
void ncdci2_process_key_translate_response( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE              p_data = NULL_PTR;
   LONG                data_len = 0;
   CHAR                Buffer[512] = {0};

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   /* extract auth_tx structure from nsp reply message */
   memset( &Auth_Tx, 0x00,   Auth_Tx_Size );
   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

   /* Was pin translated? */
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, DINERS_RACAL_APPROVED) )
   {
	   memcpy(Key_value,Auth_Tx.TLF01_details.dest_key,sizeof(Key_value));
	   Mask_Keys(Key_value);
	   	sprintf(Buffer,"Key transaclate Successful. Dest_key : %s ",
	   				masked_Key_value);
	    ncdci2_log_message( 3, 1, Buffer,
	  		                 "ncdci2_process_key_translate_response ", 0 );

	    if (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)
	    {
	    	memcpy(Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,
	    			Auth_Tx.TLF01_details.dest_key,DINERS_KEY_LEN);
	    }
	    else
	    {
	    	memcpy(Ncf01_I_key_exchange.cwk,Auth_Tx.TLF01_details.dest_key,DINERS_KEY_LEN);
	    }
        Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
		ncdci2_perform_host_logon_request(DINERS_NMI_KEYTEST,SOURCE_IS_HOST,&Auth_Tx);
	}
   else
   {
      /* Key transaclate failed  to authorizer */
      ncdci2_log_message( 1, 3, "Key transaclate failed.",
                     	 	 "ncdci2_process_key_translate_response", 0 );
      strncpy( Auth_Tx.TLF01_details.response_code,
    		  DINERS_KEY_TRANSLATE_FAILED_RESP_CODE,
			  DINERS_ISO_RESP_LEN);
      ncdci2_send_response_to_host( DINERS_SEND_IF_KEY_TRAN_FAILED );
   }
   return;
}

void Mask_Keys(char * key_value)
{
	int key_valuelen = 0;
	int nIndex 	= 0;
	memset(masked_Key_value ,0x00, sizeof(masked_Key_value));
	key_valuelen = strlen( key_value );
	if (key_valuelen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_Key_value, key_value, key_valuelen);
		for ( nIndex = key_valuelen-5; nIndex >= key_valuelen-11; nIndex-- )
		{
			masked_Key_value[nIndex] = '*';
		}
	}
}

