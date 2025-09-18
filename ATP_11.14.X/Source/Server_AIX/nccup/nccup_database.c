/***************************************************************************
 *
 * Copyright (c) 2002, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup_database.c
 *  
 * TITLE:       CUP  Network Controller Database Processor
 *  
 * DESCRIPTION: This module contains functions that handle requests to
 *              and responses from a data server.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:     Abhishek Verma
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
#include "ptetime.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "cup.h"
#include "nccup_prototypes.h"
#include "tx_dbstruct.h"
#include "ntutils.h"



/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR       updateds_que_name[];
extern CHAR       nsp_que_name[];
extern CHAR       trands_que_name[];
extern CHAR       netds_que_name[];
extern CHAR       oprmon_que_name[];
extern CHAR       applnk_que_name[];
extern CHAR       app_name[];
extern CHAR       authorizer_que_name[];
extern CHAR       timerds_que_name [];

/* Database structures */
extern AUTH_TX    Auth_Tx;
extern NCF01      Ncf01_I;
extern NCF01      Ncf01_A;
extern NCF30      Ncf30;
extern NCF01 Ncf01_Nccup_I;


/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* Miscellaneous */
extern INT  TranFlow;
extern CHAR  static_Rtdcwk_Key[49];
extern CHAR   MAC[17];
extern  NCF01 Nccup3_Nccup_I;

extern INT Key_exchange_enable_Flag;
CHAR Last_Tran_Logging_Filename[256];

/**********************************************************************
 **********************************************************************
 **********************************************************************
 **********                                                  **********
 **********                                                  **********
 **********            REQUESTS  TO  DATA  SERVER            **********
 **********                                                  **********
 **********                                                  **********
 **********************************************************************
 **********************************************************************
 **********************************************************************/




/******************************************************************************
 *
 *  NAME:         INSERT_DB_RECORD
 *
 *  DESCRIPTION:  This function calls the data server to insert a record
 *                into a database table.
 *
 *  INPUTS:       data_type - Indicates which table to insert into
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT insert_db_record( BYTE data_type )
{
   INT    ret_val = true;
   INT    length;
   INT    continue_with_insert = true;
   CHAR   db_table[6]  = "";
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   BYTE   db_buffer[AUTH_TX_SIZE + TLF01_SIZE] = "";

   if ( data_type == TLF01_DATA )
   {
      if ( Auth_Tx.host2_acquirer_cb_code[0] != '*' )
      {
         /* First set tran_finish_time. */
         ptetime_get_strtime( Auth_Tx.TLF01_details.tran_finish_time );
         Auth_Tx.host2_acquirer_cb_code[0] = '*';
         if(Auth_Tx.TLF01_details.tx_key ==  AUTH_REVERSAL_RESPONSE &&
           (0 == strlen(Auth_Tx.TLF01_details.reversal_amount)))
		 {
			// Original transaction is not in DB. Tranlookup need to show without update
			memset(Auth_Tx.TLF01_details.orig_message,0x00,
					(sizeof(Auth_Tx.TLF01_details.orig_message))-1);
		 }

         memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
         memcpy( db_buffer+Auth_Tx_Size, &Auth_Tx.TLF01_details, Tlf01_Size );
         length   = Auth_Tx_Size + Tlf01_Size;
         strcpy( db_table, "TLF01" );
      }
      else
         continue_with_insert = false;
   }

   if ( continue_with_insert == true )
   {
      ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                                ST1_DB_INSERT,
                                                0,
                                                updateds_que_name,
                                                db_buffer,
                                                length,
                                                data_type,
                                                NULL_STR,
                                                err_buf   );
      if ( ret_val == false )
      {
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Unable to insert record into %s.  Error = %s",
                  db_table, err_buf );
         nccup_log_message( 1, 3, Buffer, "insert_db_record",1 );
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         GET_DB_RECORD
 *
 *  DESCRIPTION:  This function makes a call to the data server to select 
 *                a record for given application.
 *
 *  INPUTS:       app_data_type - Type of data (db table) to select
 *                subtype2      - Flag to let us know where to reenter the code
 *                network_type  - 'I' for Issuer, 'A' for Acquirer
 *                Auth_Tx       - (Global)Transaction record to be echoed back
 *                Ncf01         - (Global)Network controller application record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       S. Yarlagadda
 *  
 ******************************************************************************/
INT get_db_record( BYTE app_data_type, BYTE subtype2, pBYTE network_type )
{
   pPTE_MSG  p_msg   = NULL_PTR;
   INT       ret_val;
   INT       buf_len;
   BYTE      buffer[AUTH_TX_SIZE + AUTH_TX_SIZE] = "";
   CHAR      err_buf[200];
   CHAR      error_msg[100];
   NCF30     ncf30;
   


   /* Set up network identifying information for this transaction */
   memcpy( buffer, &Auth_Tx, Auth_Tx_Size );
   if ( NCF30_DATA == app_data_type )
   {
      /* Build primary key to select original transaction. */
      memset( &ncf30, 0x00, sizeof(NCF30) );
      strcpy( ncf30.primary_key.network_id,   AppName );
      strcpy( ncf30.primary_key.network_type, network_type );

      strcpy( ncf30.card_nbr, Auth_Tx.TLF01_details.card_num );
      strcpy( ncf30.tran_amount, Auth_Tx.TLF01_details.total_amount );

      if ( Auth_Tx.TLF01_details.orig_sys_trace_num[0] == 0x00 )
      {
         strcpy( ncf30.sys_trace_audit_nbr,
                 Auth_Tx.TLF01_details.sys_trace_audit_num );
      }
      else
      {
         strcpy( ncf30.sys_trace_audit_nbr,
                 Auth_Tx.TLF01_details.orig_sys_trace_num );
      }

      memcpy( buffer+Auth_Tx_Size, &ncf30, Ncf30_Size );
      buf_len = Auth_Tx_Size + Ncf30_Size;
   }
   
   else if ( NCF01_DATA == app_data_type )
   {
      strcpy( Ncf01_I.primary_key.network_id,   AppName );
      strcpy( Ncf01_I.primary_key.network_type, network_type );

      memcpy( buffer+Auth_Tx_Size, &Ncf01_I, Ncf01_Size );
      buf_len = Auth_Tx_Size + Ncf01_Size;
   }

   /* Build PTE message and send it to data server. */
   ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                             ST1_DB_SELECT,
                                             subtype2,
                                      (pBYTE)netds_que_name,
                                             buffer,
                                             buf_len,
                                             app_data_type,
                                             NULL_STR,
                                             error_msg   );

   if ( ret_val == false )
   {
      nccup_log_message( 1, 3, error_msg, "get_db_record",1 );
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unable to select record from database for table type %d.",
               app_data_type );
      nccup_log_message( 1, 3, err_buf, "get_db_record",1 );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         UPDATE_DATABASE
 *
 *  DESCRIPTION:  This function issues the ALL_DATA command to the data
 *                server.  This will update all records that need to be
 *                updated as a result of a switched in transaction from
 *                a host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT update_database()
{
   INT    ret_val;
   INT    length;
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   BYTE   db_buffer[AUTH_TX_SIZE] = "";

  /*
		We need to change the key for PRE AUTH CANCLE or PREAUTH_CANCLE_REV
  */

  // We need to change the key for PRE AUTH CANCLE
	if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
		(Auth_Tx.TLF01_details.tx_key== AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE))
	{
		Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
		if ( 0 == strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num))
		{
			strcpy(Auth_Tx.TLF01_details.orig_retrieval_ref_num,
				    Auth_Tx.TLF01_details.retrieval_ref_num);
		}
	}
   memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
   length = Auth_Tx_Size;

   ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                             ST1_DB_UPDATE,
                                             ST2_ACQUIRER,
                                      (pBYTE)updateds_que_name,
                                             db_buffer,
                                             Auth_Tx_Size,
                                             ALL_DATA,
                                             NULL_STR,
                                             err_buf   );
   if ( ret_val == false )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Unable to update txn info into database. Error = %s",
               err_buf );
      nccup_log_message( 1, 3, Buffer, "update_database",1 );
   }

   return( ret_val );
}


/**********************************************************************
 **********************************************************************
 **********************************************************************
 **********                                                  **********
 **********                                                  **********
 **********          RESPONSES  FROM  DATA  SERVER           **********
 **********                                                  **********
 **********                                                  **********
 **********************************************************************
 **********************************************************************
 **********************************************************************/



/******************************************************************************
 *
 *  NAME:         PROCESS_DB_REPLY
 *
 *  DESCRIPTION:  This function handles a response from a data server.  It
 *                decides if the data server was a success or a failure and
 *                then calls the appropriate function.
 *
 *  INPUTS:       p_msg - Pointer to the PTE message from the data server
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_db_reply( pPTE_MSG p_msg )
{
   /* Determine if database action was successful or not. */
   if ( PTEMSG_OK == ptemsg_get_result_code(p_msg) )
      process_db_ok( p_msg );
   else
      process_db_error( p_msg );
   return;
}


/******************************************************************************
 *
 *  NAME:         PROCESS_DB_OK
 *
 *  DESCRIPTION:  This function is the handler for processing responses from
 *                a data server.  This function gets called if the data server
 *                responds with PTEMSG_OK, which means it was successful in
 *                performing whatever was requested of it.
 *
 *  INPUTS:       p_msg - Pointer to the PTE message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_db_ok( pPTE_MSG p_msg )
{
   pBYTE         p_data_stream;
   pPTE_MSG_DATA p_msg_data;
   BYTE          subtype1;
   BYTE          app_data_type;
   CHAR          Errorbuf[100]= {0};
   FILE 		  *lastmsgptr=NULL_PTR;
   INT 			  Last_Message_len = 0;
   char			  last_tran__msg[256] = {0};

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg );
   p_data_stream = ptemsg_get_pte_msg_data_data( p_msg_data );
   
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   subtype1 = ptemsg_get_msg_subtype1( p_msg );
    memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
    memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );

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

   if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         /* Database is updated.
          * Send response to host and insert into TLF01.
          */
         TranFlow = SWITCHED_IN;
        /*
			We Need to change the key for PRE_AUTH_CANCLE and PRE_AUTH_CANCLE_REVERSAL
		*/
		// We need to change the key 
		if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
			(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
			(Auth_Tx.TLF01_details.tx_key== AUTH_REVERSAL_RESPONSE))
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE;
			}
         send_response_to_host( SEND_IF_NOT_REVERSAL );
         sprintf(Errorbuf,"Response Send to CUP Host for stan: %s",Auth_Tx.TLF01_details.sys_trace_audit_num);
         nccup_log_message( 2, 1, Errorbuf,"process_db_ok",1 );
	  }
	  else if(NCF01_DATA == app_data_type)
	  {
		  if(CUP_KEY_DESIABLE_FLAG == Auth_Tx.TLF01_details.tx_key )
		  {
			  // Key exchange response is already sent . we can log message and drop it.
			  nccup_log_message( 2, 1, "Key exchange flag is disabled","process_db_ok",0 );
		  }
		  else if(CUP_KEY_ENABLE_FLAG == Auth_Tx.TLF01_details.tx_key )
		  {
			  // Key exchange response is already sent . we can log message and drop it.
			  nccup_log_message( 2, 1, "Key exchange flag is Enabled","process_db_ok",0 );
		  }

	  	// Check we are able to update key and MAC value successful to ncf01 table 
		  else if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key ||
		    AUTH_PIN_CHANGE == Auth_Tx.TLF01_details.tx_key ||
		   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0800",4)))
	  	{
			
			if( 0 == strncmp(AppName,"nccup3",6))
			{
				nccup_log_message( 1, 1, "Updated key in to NCF01 table,Send message to nccup to transalate CUP KEY",
										"process_db_ok",0 );
				// Make sure nccup should not respond this
				send_update_Key_message_to_nccup_ACQ();
			}
			else
			{
				// update status as online for nccup module
				nccup_process_logon_after_key_transalation();
				// Send nccup3 module to so status as online in monitor
				nccup_process_logon_request_after_key_transalation_success();
				return;				
			}
			
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.message_type,"0800",4))
	  	{
			if( 0 == strncmp(AppName,"nccup3",6))
			{
				nccup_log_message( 1, 1, "Updated key in to NCF01 table,Send message to nccup to transalate CUP KEY",
										"process_db_ok",0 );
				 // Make sure nccup should not respond this
				send_update_Key_message_to_nccup_ACQ();
			}
			else
			{
				// update status as online for nccup module
				nccup_process_logon_after_key_transalation();
				// Send nccup3 module to so status as online in monitor
				nccup_process_logon_request_after_key_transalation_success();
				return;
			}
		}
		else
		{
			sprintf(Errorbuf,"NCF01 Operation is Success for message type: %s and ATP key type: %s  ", 
				Auth_Tx.TLF01_details.message_type, Auth_Tx.TLF01_details.tx_key);
			nccup_log_message( 1, 1, Errorbuf,"process_db_ok",0 );
		}
	  }
   }
   else  if ( ST1_DB_SELECT== subtype1 )
   	{
		if (NCF30_DATA == app_data_type)
		{
		
			 if (false == process_incoming_message_ncf30_reply(p_data_stream))
			 {
				 nccup_log_message( 1, 3, "unable to send the request to the authorizor application",
						 	 	 	 	 	 "process_db_ok",1 );
			 }
		}
		else
		{
			nccup_log_message( 1, 3, "Invalid incoming select message type",
										"process_db_ok",1 );
		}
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         PROCESS_DB_ERROR
 *
 *  DESCRIPTION:  This function is the handler for processing responses from
 *                a data server.  This function gets called if the data server
 *                does not respond with PTEMSG_OK, which means it was not
 *                successful in performing whatever was requested of it.
 *
 *  INPUTS:       p_msg - Pointer to the PTE message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_db_error( pPTE_MSG p_msg )
{
   pBYTE         p_data_stream;
   pPTE_MSG_DATA p_msg_data;
   BYTE          app_data_type;
   BYTE          subtype1;
   CHAR          buffer[300] = "";
   CHAR          error_msg[200] = "";

   p_msg_data    = ptemsg_get_pte_msg_data(p_msg);
   p_data_stream = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset( &Auth_Tx,  0x00,          Auth_Tx_Size );
   memcpy( &Auth_Tx,  p_data_stream, Auth_Tx_Size );
   strcpy( error_msg, p_data_stream+ Auth_Tx_Size );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1      = ptemsg_get_msg_subtype1(p_msg);

   if ( ST1_DB_SELECT == subtype1 )
   {
      
      if (NCF30_DATA == app_data_type)
      {
    	  if ( AUTH_ADVICE == Auth_Tx.TLF01_details.tx_key ||
    		   AUTH_ADVICE_REPEAT == Auth_Tx.TLF01_details.tx_key
    		 )
		  {
			 if (false == hhutil_transfer_ctl_to_auth_app(&Auth_Tx))
			 {
				 nccup_log_message( 1, 3, "Unable to send Advice request to txcntl application",
								    "process_db_error",1 );
			 }
		  }
		  else if (false == hhutil_transfer_ctl_to_auth_app(&Auth_Tx))
		  {
			 	 /* Unable to find original txn for a reversal.  Send to Cadencie for Approval. */
				 nccup_log_message( 1, 3, "Unable to send Reversal request to txcntl application",
									"process_db_error",1 );
		  }
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         /* Unable to update database.
          * Txn was approved; decline it and wipe out the approval code.
          * Send response back to host and insert txn into TLF01.
          * Log a system error.
          */
         TranFlow = SWITCHED_IN;
		  /*
			We Need to change the key for PRE_AUTH_CANCLE and PRE_AUTH_CANCLE_REVERSAL
		*/
		// We need to change the key For PRE_AUTH_CANCLE
		if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100",4))&&
			(0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))&&
			(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"nccup",5))&&
			(Auth_Tx.TLF01_details.tx_key== AUTH_REVERSAL_RESPONSE))
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE;
			}
         strcpy( Auth_Tx.TLF01_details.response_code, CUP_SYSTEM_ERROR );
         Auth_Tx.TLF01_details.auth_number[0] = 0x00;
         send_response_to_host( SEND_IF_NOT_REVERSAL );
         sprintf( buffer,
                 "Unable to update DB with txn info. Error = %s",
                  error_msg );
         nccup_log_message( 1, 3, buffer, "process_db_error",1 );
      }
	  else if(NCF01_DATA == app_data_type)
	  {
		if(CUP_KEY_DESIABLE_FLAG == Auth_Tx.TLF01_details.tx_key )
		{
		   nccup_log_message( 2, 1, "Failed to update Key exchange flag as disabled","process_db_error",0 );
		}
		else if(CUP_KEY_ENABLE_FLAG == Auth_Tx.TLF01_details.tx_key )
		{
		   nccup_log_message( 2, 1, "Failed to update Key exchange flag as Enabled","process_db_error",0 );
		}

	  	// Check if we failed to update key and MAC value successful to ncf01 table 
		else if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key ||
		   AUTH_PIN_CHANGE == Auth_Tx.TLF01_details.tx_key ||
		   (0 == strncmp(Auth_Tx.TLF01_details.message_type,"0800",4)))
	  	{
			memcpy(Auth_Tx.TLF01_details.response_code,"96",
				   strlen(Auth_Tx.TLF01_details.response_code));
			Auth_Tx.TLF01_details.tx_key = AUTH_PIN_CHANGE_RESPONSE;
			hhutil_send_net_mngmnt_response(&Auth_Tx);
			nccup_log_message( 1, 3, "Failed to updated key in to NCF01 table",
													"process_db_error",0 );
		}
		else
		{
			sprintf(error_msg,"NCF01 Operation is Failed : %s", Auth_Tx.TLF01_details.tx_key);
			nccup_log_message( 1, 3, error_msg,	"process_db_error",0 );
		}
	  }
   }
   return;
}


/**********************************************************************
 **********************************************************************
 **********************************************************************
 **********                                                  **********
 **********                                                  **********
 **********          INTERACTIVE  WITH  DATA  SERVER         **********
 **********                                                  **********
 **********                                                  **********
 **********************************************************************
 **********************************************************************
 **********************************************************************/


/*****************************************************************************

  Function:    GET_NCF01_WITH_WAIT

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously.

  Author: 
      BGable     6/30/98
  Inputs:
      pncf01    - pointer to the requested ncf01 structure
  Outputs:
      pncf01    - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      true      - No errors
      false     - Error
******************************************************************************/
INT get_ncf01_with_wait( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           ret_val = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[AUTH_TX_SIZE + NCF01_SIZE];
   AUTH_TX       auth_tx;

   memset( buffer,0, sizeof(buffer) );
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   memcpy( buffer, &auth_tx, Auth_Tx_Size );
   memcpy( buffer+sizeof(AUTH_TX),pncf01, Ncf01_Size);

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 NCF01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      ret_val = false;
   }
   else
   {
      if(PTEMSG_OK == (rcode = send_interactive_db_request(p_msg_out, &p_msg_in,
                                                           netds_que_name,
                                                           error_msg )) )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( pncf01, p_data+Auth_Tx_Size, Ncf01_Size );
      }

      if ( PTEMSG_IPC_ERROR != rcode )
         free( p_msg_in );
      else
         ret_val = false;
   }
   return ( ret_val );
}

/*****************************************************************************

  Function:    SEND_INTERACTIVE_DB_REQUEST

  Description: This function will send a request to the dataserver.
               It uses the interactive queue.
  Author: 
      BGable     6/30/98
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
  Outputs:
      error_msg - the errror message from the dataserver
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg )
{
   pBYTE           p_data;
   pPTE_MSG_DATA   p_msg_data;
   CHAR            temp_str[100];
   LONG            retcode;
   BYTE            dberror;

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to NetDS: %s",
               temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+Auth_Tx_Size, sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return dberror;
}

void Get_Key_from_ncf01_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};

	memset( &Ncf01_Nccup_I, 0x00, Ncf01_Size );
    memset( Buffer, 0x00, sizeof(Buffer) );
	// Read data from CUP Issuing link.
    strncpy(Ncf01_Nccup_I.primary_key.network_id, "nccup3",6);
    Ncf01_Nccup_I.primary_key.network_type[0] = 'I';
    ret_val = get_ncf01_with_wait( &Ncf01_Nccup_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on getting key from Nccup issuer: " );
		 strncat( err_buf,	Buffer, 50 );
    }
	 
	 strcpy(static_Rtdcwk_Key, Ncf01_Nccup_I.retired_cwk);
	 strncpy(MAC,Ncf01_Nccup_I.kek,16);
}

/*
 ** This function will read Incoming CUP key from nccup3
 * module when user trigger active active setup
 */

INT nccup_read_cup_key_for_nccup3_active_active_trigger(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[300] = {0};

	memset( &Ncf01_I, 0x00, Ncf01_Size );
    memset( Buffer, 0x00, sizeof(Buffer) );

    strncpy(Ncf01_I.primary_key.network_id, "nccup3",6);
    Ncf01_I.primary_key.network_type[0] = 'I';

    ret_val = get_ncf01_with_wait( &Ncf01_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on reloading key for Active Active setup:" );
		 strncat( err_buf,	Buffer, 50 );
		 nccup_log_message( 1, 3, err_buf,
				 	 	 	 	 "nccup_read_cup_key_for_nccup3_active_active_trigger",0 );
	}
	else
	{
		strcpy(static_Rtdcwk_Key, Ncf01_I.retired_cwk);
		strncpy(MAC,Ncf01_I.kek,16);
		strcpy(  err_buf, "Key Reload successful for Active active:" );
		nccup_log_message( 1, 1, err_buf,
							"nccup_read_cup_key_for_nccup3_active_active_trigger",0 );
	}
	return ret_val;
}


/*
 * This function read CUP transalated key from NCF01 table
 */

INT nccup_Get_key_Transalation_Info_from_nccup_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	CHAR   err_buf1[100] = {0};

	strcpy(  err_buf1, "Read key transalation details from nccup tables " );
	nccup_log_message( 1, 1, err_buf1,
				 	 	 "nccup_Get_key_Transalation_Info_from_nccup_table",0 );

	memset( &Ncf01_I, 0x00, Ncf01_Size );
	memset( Buffer, 0x00, sizeof(Buffer) );
	strncpy(Ncf01_I.primary_key.network_id, "nccup",5);
	Ncf01_I.primary_key.network_type[0] = 'I';
	ret_val = get_ncf01_with_wait( &Ncf01_I, Buffer);
	if ( false == ret_val )
	{
		strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Nccup Issuing: " );
		strncat( err_buf,	Buffer, 50 );
		nccup_log_message( 1, 3, err_buf,
					 	 	 "nccup_Get_key_Transalation_Info_from_nccup_table",0 );
	}
	return ret_val;
}


INT nccup_Key_exchange_enable_Flag_value_from_nccup3_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	CHAR   err_buf1[100] = {0};
	NCF01  Ncf01_nccup_3_I;
	CHAR  prev_src_station[2]={0};

	strcpy(  err_buf1, "Read Key_exchange_enable_Flag value from nccup3 tables " );
	nccup_log_message( 1, 1, err_buf1,
				 	 	 "nccup_Key_exchange_enable_Flag_value_from_nccup3_table",0 );

	memset( &Ncf01_nccup_3_I, 0x00, Ncf01_Size );
	memset( Buffer, 0x00, sizeof(Buffer) );
	strncpy(Ncf01_nccup_3_I.primary_key.network_id, "nccup3",6);
	Ncf01_nccup_3_I.primary_key.network_type[0] = 'I';
	ret_val = get_ncf01_with_wait( &Ncf01_nccup_3_I, Buffer);
	if ( false == ret_val )
	{
		strcpy(  err_buf, "Error on nccup_Key_exchange_enable_Flag_value_from_nccup3_table Nccup Issuing: " );
		strncat( err_buf,	Buffer, 50 );
		nccup_log_message( 1, 3, err_buf,
					 	 	"nccup_Key_exchange_enable_Flag_value_from_nccup3_table",0 );
	}
	else
	{
		strncpy(prev_src_station,Ncf01_nccup_3_I.prev_src_station,1);
		Key_exchange_enable_Flag = atoi(prev_src_station);
	}
	return ret_val;
}
