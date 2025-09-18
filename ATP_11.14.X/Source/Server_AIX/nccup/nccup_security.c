/***************************************************************************
 *
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup_security.c
 *  
 * TITLE:       CUP Network Controller Security Processor
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
#include "cup.h"
#include "nccup_prototypes.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

/* STRUCTURES */
extern AUTH_TX Auth_Tx;
extern NCF01   Ncf01_I;
extern NCF01   Ncf01_A;
extern NCF30   Ncf30;
extern ATM01   Atm01;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;

/* QUEUES */
extern CHAR nsp_que_name[];

/* VARIABLE*/
extern CHAR    AppName[];
extern CHAR MAC[17];
CHAR Last_Tran_Logging_Filename[256];


/******************************************************************************
 *
 *  NAME:         TRANSLATE_PIN
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void translate_pin()
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   CHAR  buffer[256];
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE];
   
   /* Send both ncf01 and auth_tx to security processor */
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );

   ret_val = build_and_send_request_message( MT_NSP_TRANSLATE_PIN,
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
      nccup_log_message( 1, 3, buffer, "translate_pin",0 );
   }
   return;
}

/******************************************************************************
*
*  NAME:         PROCESS_TRANSLATE_PIN_RESPONSE
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
*  AUTHOR:       Dennis Irby
*
******************************************************************************/
void process_translate_pin_response( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data;
   LONG            data_len;
   FILE 		  *lastmsgptr=NULL_PTR;
   INT 			  Last_Message_len = 0;
   char			  last_tran__msg[256] = {0};

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   /* extract auth_tx structure from nsp reply message */
   memset( &Auth_Tx, 0x00,   Auth_Tx_Size );
   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

 	/*************Last transaction to be process in ************/
 	lastmsgptr = fopen(Last_Tran_Logging_Filename,"w");
 	if(lastmsgptr != NULL_PTR)
 	{
 		sprintf( last_tran__msg,
 			   "Card number=%s STAN=%s TID=%s MID=%s acquirer_id=%s handler_queue=%s \n",
 			   Auth_Tx.TLF01_details.card_num,
 			   Auth_Tx.TLF01_details.sys_trace_audit_num,
 			   Auth_Tx.TLF01_details.terminal_id,
 			   Auth_Tx.TLF01_details.merchant_id,
 			   Auth_Tx.TLF01_details.acquirer_id,
 			   Auth_Tx.TLF01_details.handler_queue);

 		Last_Message_len=strlen(last_tran__msg);

 		fwrite(last_tran__msg,1,Last_Message_len,lastmsgptr);
 		fclose(lastmsgptr);
 	}
   /* Was pin translated? */
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, CUP_RACAL_APPROVED) )
   {
      /* Pin was successfully translated.
       * Continue processing transaction.
       */
      (void)process_request_for_host_after_security();
   }
   else
   {
      /* Pin translation failed, send response to authorizer */
      nccup_log_message( 1, 3, "Pin translation failed.",
                     "process_translate_pin_response",0 );
      perform_error_processing( CUP_SYSTEM_ERROR );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         key_translate
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

void key_translate()
{
   INT   ret_val;
   CHAR  err_buf[100]="";
   CHAR  buffer[256]="";
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE];

   /* Send both ncf01 and auth_tx to security processor */
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );

   ret_val = build_and_send_request_message( MT_NSP_TRANSLATE_ZMK_LMK,
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
	  nccup_log_message( 1, 3, buffer, "key_translate",0 );
   }
   else
   {
		sprintf( buffer,
				"key Transalate request sent to HSM: %s",
				err_buf );
				nccup_log_message( 1, 1, buffer, "key_translate",0 );
   }
}



/******************************************************************************
*
*  NAME:         process_translate_change_response
*
*  DESCRIPTION:  Process a response from the security application for a pin
*                translation.  If the translation failed, stand in for the
*                transaction - decline it and respond to the authorizer.  If
*                successful, update_ncf01 with translated key.
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
void process_key_translate_response( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE              p_data;
   LONG                data_len;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   /* extract auth_tx structure from nsp reply message */
   memset( &Auth_Tx, 0x00,   Auth_Tx_Size );
   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

   /* Was pin translated? */
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, CUP_RACAL_APPROVED) )
   {
      /* Pin was successfully translated.  Store the transalated pin  */
		memcpy(Ncf01_I.retired_cwk,Auth_Tx.TLF01_details.dest_key,strlen(Auth_Tx.TLF01_details.dest_key));
		memcpy(Ncf01_I.kek,MAC,strlen(MAC));
		Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE_RESPONSE;
		memset(Auth_Tx.TLF01_details.response_code,0x00,
											(sizeof(Auth_Tx.TLF01_details.response_code) - 1));
		update_ncf01(&Auth_Tx);
	}
   else
   {
      /* Key transaclate failed  to authorizer */
	 nccup_log_message( 1, 3, "Key transaclate failed.",
                     "process_key_translate_response",0 );
     nccup_process_logoff_after_key_transalation();
     nccup_process_logon_request_after_key_transalation_failed();
   }
   return;
}

