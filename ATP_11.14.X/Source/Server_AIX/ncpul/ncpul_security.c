/***************************************************************************
 *
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncpul_security.c
 *  
 * TITLE:       ncpul Network Controller Security Processor
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
#include "pulse.h"
#include "ncpul_prototypes.h"

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
extern CHAR static_Rtdcwk_Key[39];
extern CHAR MAC[17];



/******************************************************************************
 *
 *  NAME:         ncpul_translate_pin
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
void ncpul_translate_pin()
{
   INT   ret_val = false;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};
   
   /* Send both ncf01 and auth_tx to security processor */
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );

   ret_val = ncpul_build_and_send_request_message( MT_NSP_TRANSLATE_PIN,
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
      ncpul_log_message( 3, 3, buffer, "translate_pin",0 );
   }
   return;
}

/******************************************************************************
*
*  NAME:         ncpul_process_translate_pin_response
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
void ncpul_process_translate_pin_response( pPTE_MSG p_msg_in )
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
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, PULSE_RACAL_APPROVED) )
   {
      /* Pin was successfully translated.
       * Continue processing transaction.
       */
      (void)ncpul_process_request_for_host_after_security();
   }
   else
   {
      /* Pin translation failed, send response to authorizer */
      ncpul_log_message( 3, 3, "Pin translation failed.",
                     "ncpul_process_translate_pin_response",0 );
      ncpul_perform_error_processing( PULSE_SECURITY_FAILURE );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ncpul_key_translate
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

void ncpul_key_translate()
{
   INT   ret_val;
   CHAR  err_buf[100] = {0};
   CHAR  buffer[256] = {0};
   CHAR  buffer1[AUTH_TX_SIZE + NCF01_SIZE] = {0};

   /* Send both ncf01 and auth_tx to security processor */
   memset( buffer1, 0x00, sizeof( buffer1 ) );
   memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );

   ret_val = ncpul_build_and_send_request_message( MT_NSP_TRANSLATE_ZMK_LMK,
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
	  ncpul_log_message( 3, 3, buffer, "ncpul_key_translate",0 );
   }
   else
   {
		sprintf( buffer,"key Transalate request sent to HSM: %s",err_buf );
		ncpul_log_message( 3, 1, buffer, "ncpul_key_translate",0 );
   }
}



/******************************************************************************
*
*  NAME:         ncpul_process_key_translate_response
*
*  DESCRIPTION:  Process a response from the security application for a pin
*                translation.  If the translation failed, stand in for the
*                transaction - decline it and respond to the authorizer.  If
*                successful, ncpul_update_ncf01 with translated key.
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
void ncpul_process_key_translate_response( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE              p_data = NULL_PTR;
   LONG                data_len = 0;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   /* extract auth_tx structure from nsp reply message */
   memset( &Auth_Tx, 0x00,   Auth_Tx_Size );
   memcpy( &Auth_Tx, p_data, Auth_Tx_Size );

   /* Was pin translated? */
   if( 0 == strcmp(Auth_Tx.TLF01_details.security_response_code, PULSE_RACAL_APPROVED) )
   {
      /* Pin was successfully translated.  Store the transalated pin  */
	   memcpy(Ncf01_I.cwk,0x00,	sizeof(Ncf01_I.cwk));
		memcpy(Ncf01_I.cwk,Auth_Tx.TLF01_details.dest_key,
				strlen(Auth_Tx.TLF01_details.dest_key));
		Auth_Tx.TLF01_details.tx_key=AUTH_PIN_CHANGE_RESPONSE;
		ncpul_update_ncf01(&Auth_Tx);
	}
   else
   {
      /* Key transaclate failed  to authorizer */
      ncpul_log_message( 3, 3, "Key transaclate failed.",
                     "process_key_translate_response",0 );
      ncpul_perform_error_processing( PULSE_SECURITY_FAILURE );
   }
   return;
}

