/***************************************************************************
 *
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_security.c
 *  
 * TITLE:       ncdci Network Controller Security Processor
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
#include "ncdci_prototypes.h"
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

/* VARIABLE*/
extern CHAR    AppName[];
extern CHAR static_Rtdcwk_Key[33];
extern CHAR MAC[17];
CHAR masked_Key_value[49]={0};
CHAR Key_value_DestKey[49]={0};
extern CHAR static_Rtdcwk_Key_Keyblock[33];

extern CHAR   MAC_Keyblock[17] ;

/******************************************************************************
 *
 *  NAME:         ncdci_translate_pin
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
void ncdci_translate_pin()
{
   INT   ret_val = false;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};
   
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );
   ret_val = ncdci_build_and_send_request_message( MT_NSP_TRANSLATE_PIN,
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
      sprintf( buffer,
              "Unable to translate pin. Err: %s",
               err_buf );
      ncdci_log_message( 1, 3, buffer, "ncdci_translate_pin",1 );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_translate_pin_keyblock
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
void ncdci_translate_pin_keyblock()
{
   INT   ret_val = false;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};

   CHAR  buffer2[AUTH_TX_SIZE + NCF01_KEYBLOCK01_SIZE] = {0};

   memset( buffer2, 0x00, sizeof( buffer2 ) );
   memcpy( buffer2, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer2+Auth_Tx_Size, &Ncf01_Keyblock, NCF01_KEYBLOCK01_SIZE );
   ret_val = ncdci_build_and_send_request_message(  MT_NSP_TRANSLATE_PIN_KEYBLOCK,
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
      sprintf( buffer,
              "Unable to translate pin. Err: %s",
               err_buf );
      ncdci_log_message( 1, 3, buffer, "ncdci_translate_pin_keyblock",1 );
   }
   return;
}

/******************************************************************************
*
*  NAME:         ncdci_process_translate_pin_response
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
void ncdci_process_translate_pin_response( pPTE_MSG p_msg_in )
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
      (void)ncdci_process_request_for_host_after_security();
   }
   else
   {
      /* Pin translation failed, send response to authorizer */
      ncdci_log_message( 1, 3, "Pin translation failed.",
                     "ncdci_process_translate_pin_response", 1 );
      ncdci_perform_error_processing( DINERS_SECURITY_FAILURE );
   }
   return;
}

/******************************************************************************
*
*  NAME:         ncdci_process_translate_pin_response_Keyblock
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
void ncdci_process_translate_pin_response_Keyblock( pPTE_MSG p_msg_in )
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
      (void)ncdci_process_request_for_host_after_security();
   }
   else
   {
      /* Pin translation failed, send response to authorizer */
      ncdci_log_message( 1, 3, "Pin translation failed.",
                     "ncdci_process_translate_pin_response_Keyblock", 1 );
      ncdci_perform_error_processing( DINERS_SECURITY_FAILURE );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ncdci_key_generate_req
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

void ncdci_key_generate_req()
{
   INT   ret_val;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};

   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );
   ret_val = ncdci_build_and_send_request_message(MT_NSP_DINERS_GENERATE_KEY,
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
			  "Unable to send request for key generation. Err: %s",
			   err_buf );
	  ncdci_log_message( 1, 3, buffer, "ncdci_key_generate_req", 0 );
   }
   else
   {
		sprintf( buffer,
				"key generation request sent to HSM: %s",
				err_buf );
				ncdci_log_message( 1, 1, buffer, "ncdci_key_generate_req", 0 );
   }
}

/******************************************************************************
 *
 *  NAME:         ncdci_key_generate_req_keyblock
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

void ncdci_key_generate_req_keyblock()
{
   INT   ret_val;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};

   CHAR  buffer2[AUTH_TX_SIZE + NCF01_KEYBLOCK01_SIZE] = {0};

   memset( buffer2, 0x00, sizeof( buffer2 ) );
   memcpy( buffer2, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer2+Auth_Tx_Size, &Ncf01_Keyblock, NCF01_KEYBLOCK01_SIZE );
   ret_val = ncdci_build_and_send_request_message(MT_NSP_DINERS_GENERATE_KEY_KEYBLOCK,
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
		sprintf( buffer,
				"Unable to send request for key generation. Err: %s",
				err_buf );
		ncdci_log_message( 1, 3, buffer, "ncdci_key_generate_req_keyblock", 0 );
	}
	else
	{
		sprintf( buffer,
				"key generation request sent to HSM: %s",
				err_buf );
		ncdci_log_message( 1, 1, buffer, "ncdci_key_generate_req_keyblock", 0 );
	}
}




/******************************************************************************
*
*  NAME:         ncdci_key_generate_response
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
void ncdci_key_generate_response( pPTE_MSG p_msg_in )
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
	   if( 0 == strncmp(AppName,"ncdci3",6))
	   {
		   memcpy(Key_value_DestKey,Auth_Tx.TLF01_details.dest_key,sizeof(Key_value_DestKey));
		   Mask_Keys(Key_value_DestKey);
		   sprintf(Buffer,"Key transaclate Successful. Dest_key : %s , Source_key :%s, kek :%s",masked_Key_value,Auth_Tx.TLF01_details.source_key,Auth_Tx.TLF01_details.kek);
		   ncdci_log_message( 3, 1, Buffer,
		                        "ncdci_process_key_translate_response ", 0 );

		   memcpy(Ncf01_I_key_exchange.cwk,Auth_Tx.TLF01_details.dest_key,strlen(Auth_Tx.TLF01_details.dest_key));
		   memcpy(Ncf01_I_key_exchange.retired_cwk,Auth_Tx.TLF01_details.source_key,strlen(Auth_Tx.TLF01_details.source_key));
		   memcpy(Ncf01_I_key_exchange.prev_src_station,Auth_Tx.TLF01_details.kek,strlen(Auth_Tx.TLF01_details.kek));
		   Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE;
		   ncdci_issue_keytest_req_08X4(DINERS_NMI_KEYTEST);
	   }
	}
   else
   {
      /* Key transaclate failed  to authorizer */
      ncdci_log_message( 1, 3, "Key transaclate failed.",
                     "ncdci_process_key_translate_response", 0 );
   }
   return;
}

/******************************************************************************
*
*  NAME:         ncdci_key_generate_response_keyblock
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
void ncdci_key_generate_response_keyblock( pPTE_MSG p_msg_in )
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
	   if( 0 == strncmp(AppName,"ncdci3",6))
	   {
		   memcpy(Key_value_DestKey,Auth_Tx.TLF01_details.dest_key,sizeof(Key_value_DestKey));
		   Mask_Keys(Key_value_DestKey);
		   sprintf(Buffer,"Key transaclate Successful. Dest_key : %s , Source_key :%s, kek :%s",masked_Key_value,Auth_Tx.TLF01_details.source_key,Auth_Tx.TLF01_details.kek);
		   ncdci_log_message( 3, 1, Buffer,
		                        "ncdci_process_key_translate_response ", 0 );

		   /* Pin was successfully translated.  Store the transalated pin  */
		   memcpy(Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,Auth_Tx.TLF01_details.dest_key,strlen(Auth_Tx.TLF01_details.dest_key));
		   memcpy(Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock,Auth_Tx.TLF01_details.source_key,strlen(Auth_Tx.TLF01_details.source_key));
		   memcpy(Ncf01_I_key_exchange_Keyblock.iss_kek_keyblock,Auth_Tx.TLF01_details.kek,strlen(Auth_Tx.TLF01_details.kek));
		   Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE;
		   ncdci_issue_keytest_req_08X4(DINERS_NMI_KEYTEST);
	   }
	}
   else
   {
      /* Key transaclate failed  to authorizer */
      ncdci_log_message( 1, 3, "Key transaclate failed.",
                     "ncdci_process_key_translate_response", 0 );
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

