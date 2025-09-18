/***************************************************************************
 *
 * Copyright (c) 2002, ThoughtFocus, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci2_database.c
 *  
 * TITLE:       Diners  Network Controller Database Processor
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
#include "diners.h"
#include "ncdci2_prototypes.h"
#include "tx_dbstruct.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR       updateds_que_name[];
extern CHAR       nsp_que_name[];
extern CHAR       trands_que_name[];
extern CHAR       netds_que_name[];
extern CHAR       devds_que_name[];
extern CHAR       oprmon_que_name[];
extern CHAR       applnk_que_name[];
extern CHAR       app_name[];
extern CHAR       authorizer_que_name[];
extern CHAR       timerds_que_name [];

/* Database structures */
extern AUTH_TX    Auth_Tx;
extern NCF01      Ncf01_I;
extern NCF01      Ncf01_A;
extern NCF01 Ncf01_Ncdci3_I;
extern NCF01 Ncf01_Ncdci_I;
extern NCF01 Ncf01_Ncdci_A;
extern NCF30      Ncf30;

extern NCF01_KEYBLOCK 		Ncf01_Keyblock_Ncdci3_I;
extern NCF01_KEYBLOCK 		Ncf01_Keyblock;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* Miscellaneous */
extern INT  TranFlow;
extern CHAR  static_Rtdcwk_Key[39];
extern CHAR   static_Rtdcwk_Key_keyblock[33];
extern CHAR  security_que_name[];
extern CHAR   MAC[17];
extern CHAR   MAC_Keyblock[17];

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
 *  NAME:         ncdci2_insert_db_record
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci2_insert_db_record( BYTE data_type )
{
	CHAR   db_table[6]  = {0};
	CHAR   err_buf[100] = {0};
	CHAR   Buffer[200] = {0};
	BYTE   db_buffer[AUTH_TX_SIZE + TLF01_SIZE] = {0};
   INT    ret_val = true;
   INT    length = 0;
   INT    continue_with_insert = true;


   if ( data_type == TLF01_DATA )
   {
      if ( Auth_Tx.host2_acquirer_cb_code[0] != '*' )
      {
         /* First set tran_finish_time. */
         ptetime_get_strtime( Auth_Tx.TLF01_details.tran_finish_time );
         Auth_Tx.host2_acquirer_cb_code[0] = '*';

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
      ret_val = ncdci2_build_and_send_request_message( MT_DB_REQUEST,
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
         ncdci2_log_message( 1, 3, Buffer, "ncdci2_insert_db_record",1 );
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_get_db_record
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
INT ncdci2_get_db_record( BYTE app_data_type, BYTE subtype2, pBYTE network_type )
{
   pPTE_MSG  p_msg   = NULL_PTR;
   BYTE      buffer[AUTH_TX_SIZE + AUTH_TX_SIZE] = {0};
   CHAR      err_buf[200] = {0};
   CHAR      error_msg[100] = {0};
   INT       ret_val = 0;
   INT       buf_len = 0;
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
   else if ( NCF01_KEYBLOCK01_DATA == app_data_type )
   {
	 strcpy( Ncf01_Keyblock.network_id,   AppName );

	 memcpy( buffer+Auth_Tx_Size, &Ncf01_Keyblock, NCF01_KEYBLOCK01_SIZE );
	 buf_len = Auth_Tx_Size + NCF01_KEYBLOCK01_SIZE;
   }

   /* Build PTE message and send it to data server. */
   ret_val = ncdci2_build_and_send_request_message( MT_DB_REQUEST,
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
	  ncdci2_log_message( 1, 3, error_msg, "get_db_record" ,1);
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unable to select record from database for table type %d.",
               app_data_type );
      ncdci2_log_message( 2, 3, err_buf, "ncdci2_get_db_record", 1 );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_update_database
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci2_update_database()
{
	   CHAR   err_buf[100] = {0};
	   CHAR   Buffer[200] = {0};
	   BYTE   db_buffer[AUTH_TX_SIZE] = {0};
	   INT    ret_val;
	   INT    length;


   memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
   length = Auth_Tx_Size;

   ret_val = ncdci2_build_and_send_request_message( MT_DB_REQUEST,
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
      ncdci2_log_message( 1, 3, Buffer, "ncdci2_update_database",1 );
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
 *  NAME:         ncdci2_process_db_reply
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_process_db_reply( pPTE_MSG p_msg )
{
   /* Determine if database action was successful or not. */
   if ( PTEMSG_OK == ptemsg_get_result_code(p_msg) )
      ncdci2_process_db_ok( p_msg );
   else
      ncdci2_process_db_error( p_msg );
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci2_process_db_ok
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_process_db_ok( pPTE_MSG p_msg )
{
   pBYTE         p_data_stream = NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          subtype1 = 0;
   BYTE          app_data_type = 0;
   CHAR          buffer[300] = {0};

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg );
   p_data_stream = ptemsg_get_pte_msg_data_data( p_msg_data );
   
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   subtype1 = ptemsg_get_msg_subtype1( p_msg );

   if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         /* Database is updated.
          * Send response to host and insert into TLF01.
          */
    	  if(true == ncdci2_check_for_cadencieacq_transaction() &&
    		 0 ==strncmp( Auth_Tx.TLF01_details.response_code, DINERS_TIMEOUT,2 ))
    	  {
    		  // ncdci2 is offline dont send the response to cadencei drop it.
    		  return;
    	  }
         TranFlow = DINERS_SWITCHED_IN;
         memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
         memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );
         ncdci2_send_response_to_host( DINERS_SEND_IF_NOT_REVERSAL );
      }
	  else if(NCF01_DATA == app_data_type)
	  {
		  memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
		  memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );
		 if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		 {
			 strcpy( buffer,"Updated key in to NCF01 table");
			 ncdci2_log_message( 2, 1, buffer, "ncdci2_process_db_ok",0 );
			 // Send response to Cadencie.
			 ncdci2_perform_host_logon_request(DINERS_NMI_KEYCHANGE,SOURCE_IS_HOST,&Auth_Tx);
		 }
		 else
		 {
			 strcpy( buffer,"Updated NCF01 table ");
			 ncdci2_log_message( 2, 1, buffer, "ncdci2_process_db_ok",0 );
		 }
	  }
	  else if(NCF01_KEYBLOCK01_DATA == app_data_type)
	  {
		  memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
		  memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );
		 if(AUTH_PIN_CHANGE_RESPONSE == Auth_Tx.TLF01_details.tx_key)
		 {
			 strcpy( buffer,"Updated key in to NCF01 table");
			 ncdci2_log_message( 2, 1, buffer, "ncdci2_process_db_ok",0 );
			 ncdci2_perform_host_logon_request(DINERS_NMI_KEYCHANGE,SOURCE_IS_HOST,&Auth_Tx);
		 }
		 else
		 {
			 strcpy( buffer,"Updated NCF01 table ");
			 ncdci2_log_message( 2, 1, buffer, "ncdci2_process_db_ok",0 );
		 }
	  }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci2_process_db_error
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci2_process_db_error( pPTE_MSG p_msg )
{
   pBYTE         p_data_stream ,pchain_data= NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   pPTE_MSG p_echo_msg;
   BYTE          app_data_type = 0;
   BYTE          subtype1 = 0;
   CHAR          buffer[300] = {0};
   CHAR          error_msg[512] = {0};
   AUTH_TX org_auth_tx;
   pBYTE orig_que;
   LONG           data_len;

   p_msg_data    = ptemsg_get_pte_msg_data(p_msg);
   p_data_stream = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset( &Auth_Tx,  0x00,          Auth_Tx_Size );
   memcpy( &Auth_Tx,  p_data_stream, Auth_Tx_Size );
   strcpy( error_msg, p_data_stream+ Auth_Tx_Size );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1      = ptemsg_get_msg_subtype1(p_msg);
   orig_que 	 = ptemsg_get_msg_orig_queue(p_msg);
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	pchain_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );
	if ( data_len == Auth_Tx_Size )
	{
		/* Get auth_tx record that was built from host response data. */
		memset( &org_auth_tx, 0x00,   Auth_Tx_Size );
		memcpy( &org_auth_tx, pchain_data, Auth_Tx_Size );
	}



   if ( ST1_DB_SELECT == subtype1 )
   {
      
      if (NCF30_DATA == app_data_type)
      {
         /* Unable to find original txn for a reversal.  So approve it. */
         strcpy( Auth_Tx.TLF01_details.response_code, DINERS_ISO_APPROVED );
         ncdci2_send_response_to_host( DINERS_SEND_UNCONDITIONALLY );
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
         TranFlow = DINERS_SWITCHED_IN;
         strcpy( Auth_Tx.TLF01_details.response_code, DINERS_SYSTEM_ERROR );
         Auth_Tx.TLF01_details.auth_number[0] = 0x00;
         ncdci2_send_response_to_host( DINERS_SEND_IF_NOT_REVERSAL );
         sprintf( buffer,
                 "Unable to update DB with txn info. Error = %s",
                  error_msg );
         ncdci2_log_message( 1, 3, buffer, "ncdci2_process_db_error",1 );
      }
	  else if(NCF01_DATA == app_data_type)
	  {
	   	 sprintf( buffer,
                 "Updated key in to NCF01 table Failed. Error = %s",
                  error_msg );
         ncdci2_log_message( 1, 3, buffer, "ncdci2_process_db_error",1 );
         ncdci2_send_response_to_host( DINERS_SEND_IF_KEY_TRAN_FAILED );
	  }
	  else if(NCF01_KEYBLOCK01_DATA == app_data_type)
	  {
		 sprintf( buffer,
				   "Updated key in to NCF01 table Failed. Error = %s",
					error_msg );
		   ncdci2_log_message( 1, 3, buffer, "ncdci2_process_db_error",1 );
		   ncdci2_send_response_to_host( DINERS_SEND_IF_KEY_TRAN_FAILED );
	  }
	  else if((strncmp(orig_que,"timerdsA",5)==0 ))
	  {
		  memset(&Auth_Tx,0x00,data_len);
		 memcpy(&Auth_Tx,&org_auth_tx,data_len);
	      	/*	TF Phani and Raghu
	      	 * This is request from BDo - Marvin - MC time out - APR mandated changed 2017
	      	 * In this scenario , where ATP sends TO to cadenceie, But cadencie replied with approval
	      	 * for timed out trnsaction.    	 *
	      	 * 	1. ATP -> cadeince : sends 100
	      	 * 	2. ATP timed out
	      	 * 	3. ATP sends reversal to Cadencie, however there is no approved tx for this. This will be unsloicated reversal.
	      	 * 	4. ATP start processing as STIP, repsonds to orginator
	      	 * 	5  Cadencie responds with 410 with 00 or 19. AS unslocited reversal.
	      	 * 	5. Cadencie responds with 0110
	      	 * 	6. ATP should sends 0400 to cadencie, since this tx alreday processed as STIP.
	      	 * 	7. Cadencie responds with 0410. Just ingnore. No action to take.
	      	 * */

	      	  // Test

	  		/*	this part moved above inorder to isolate the log printing.
	  		 * p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	  			p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	  			p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	  			data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	  			if ( data_len == Auth_Tx_Size )
	  			{
	  				//Get auth_tx record that was built from host response data.
	  				memset( &org_auth_tx, 0x00,   Auth_Tx_Size );
	  				memcpy( &org_auth_tx, p_data, Auth_Tx_Size );
	  			}*/
	  			  //Test
	  			if( strncmp(Auth_Tx.TLF01_details.response_code,"00",2) ==0 &&
	  					(strncmp(Auth_Tx.TLF01_details.message_type,"1110",4) ==0 ||
	  					strncmp(Auth_Tx.TLF01_details.message_type,"0210",4) ==0))
	  			{
	  				sprintf(error_msg,"DCITO Sending Reversal for Late resposne subtype1: %d  orig_que %s STAN : %s",subtype1,orig_que,Auth_Tx.TLF01_details.sys_trace_audit_num);
	  	             memset(buffer,0x00,sizeof(buffer));
	  	             sprintf(buffer," Term Id:%s ,Merch Id: %s RRN: %s ",
	  	            		org_auth_tx.TLF01_details.terminal_id,Auth_Tx.TLF01_details.merchant_id,Auth_Tx.TLF01_details.retrieval_ref_num);
	  	             strcat(error_msg,buffer);
	  	             ncdci2_log_message( 3, 1, error_msg , "ncdci2_process_db_error" , 0 );

	  	           Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
	  				strncpy(Auth_Tx.TLF01_details.orig_message,Auth_Tx.TLF01_details.message_type,4);
	  				strcpy(Auth_Tx.TLF01_details.message_type, "0420");
	  				Auth_Tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
	  				strcpy( Auth_Tx.TLF01_details.orig_sys_trace_num, Auth_Tx.TLF01_details.sys_trace_audit_num);
	  				strcpy( Auth_Tx.TLF01_details.orig_retrieval_ref_num, Auth_Tx.TLF01_details.retrieval_ref_num);
	  				strncpy(Auth_Tx.TLF01_details.product_codes[3].quantity,DINERS_TIMEOUT,2);
	  				memset( Auth_Tx.req_data_len, 0x00 , sizeof(Auth_Tx.req_data_len));
	  				memset( Auth_Tx.req_buffer, 0x00 , sizeof(Auth_Tx.req_buffer));
	  				memset( Auth_Tx.resp_data_len, 0x00 , sizeof(Auth_Tx.resp_data_len));
	  				memset( Auth_Tx.resp_buffer, 0x00 , sizeof(Auth_Tx.resp_buffer));
	  				/*TOR : Time out reversal -> it is used to drop the response 410 coming from host
	  				 * for this reversal request sent for mc timeout
	  				 *
	  				 * */
	  				if(strlen(Ncf01_I.tpdu_id)> 0)
	  				{
	  					strncpy( Auth_Tx.TLF01_details.tpdu_id, Ncf01_I.tpdu_id,sizeof(Ncf01_I.tpdu_id));
	  				}
	  				else
	  				{
	  					strncpy( Auth_Tx.TLF01_details.tpdu_id,"DCI2",4);
	  				}
	  				strcpy(Auth_Tx.TLF01_details.response_text, "LR: Timed out reversal for 0110");

	  				ncdci2_send_reversal_to_host_for_late_response();
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

  Function:    ncdci2_get_ncf01_with_wait

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously.

  Author: 
      Abhishek Verma
  Inputs:
      pncf01    - pointer to the requested ncf01 structure
  Outputs:
      pncf01    - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      true      - No errors
      false     - Error
******************************************************************************/
INT ncdci2_get_ncf01_with_wait( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out = NULL_PTR;
   pPTE_MSG	     p_msg_in = NULL_PTR;
   INT           ret_val = true;
   pBYTE         p_data = NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          rcode = true;
   BYTE          buffer[AUTH_TX_SIZE + NCF01_SIZE] = {0};
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
      if(PTEMSG_OK == (rcode = ncdci2_send_interactive_db_request(p_msg_out, &p_msg_in,
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

/******************************************************************************/
INT ncdci2_get_ncf01_keyblock_with_wait( pNCF01_KEYBLOCK pncf01_keyblock, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out = NULL_PTR;
   pPTE_MSG	     p_msg_in = NULL_PTR;
   INT           ret_val = true;
   pBYTE         p_data = NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          rcode = true;
   BYTE          buffer[AUTH_TX_SIZE + NCF01_KEYBLOCK01_SIZE] = {0};
   AUTH_TX       auth_tx;

   memset( buffer,0, sizeof(buffer) );
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   memcpy( buffer, &auth_tx, Auth_Tx_Size );
   memcpy( buffer+sizeof(AUTH_TX),pncf01_keyblock, NCF01_KEYBLOCK01_SIZE);

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
								 NCF01_KEYBLOCK01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message." );
      ret_val = false;
   }
   else
   {
      if(PTEMSG_OK == (rcode = ncdci2_send_interactive_db_request(p_msg_out, &p_msg_in,
                                                           netds_que_name,
                                                           error_msg )) )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( pncf01_keyblock, p_data+Auth_Tx_Size, NCF01_KEYBLOCK01_SIZE );
      }

      if ( PTEMSG_IPC_ERROR != rcode )
         free( p_msg_in );
      else
         ret_val = false;
   }
   return ( ret_val );
}
/*******************************************************************************/

/*****************************************************************************

  Function:    ncdci2_send_interactive_db_request

  Description: This function will send a request to the dataserver.
               It uses the interactive queue.
  Author: 
      Abhishek Verma
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
BYTE ncdci2_send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg )
{
   pBYTE           p_data = NULL_PTR;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   CHAR            temp_str[100] = {0};
   BYTE            dberror = 0;
   LONG            retcode = 0;


   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to NetDS: %s",
               temp_str );
      return (PTEMSG_IPC_ERROR);
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+Auth_Tx_Size, sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return (dberror);
}

BYTE send_interactive_to_racal( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg )
{
   pBYTE           p_data = NULL_PTR;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   CHAR            temp_str[100] = {0};
   BYTE            dberror = 0;
   LONG            retcode = 0;


   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to Racal: %s",
               temp_str );
      return (PTEMSG_IPC_ERROR);
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+Auth_Tx_Size, sizeof(temp_str)-1);
      sprintf( error_msg, "Racal Error: %s", temp_str );
   }
   return (dberror);
}

void ncdci2_Get_Key_from_ncf01_table(void)
{

	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	INT    ret_val = true;

	memset( &Ncf01_Ncdci3_I, 0x00, Ncf01_Size );
    memset( Buffer, 0x00, sizeof(Buffer) );

    strncpy(Ncf01_Ncdci3_I.primary_key.network_id, "ncdci3",6);
    Ncf01_Ncdci3_I.primary_key.network_type[0] = 'I';
    ret_val = ncdci2_get_ncf01_with_wait( &Ncf01_Ncdci3_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on getting key from Ncdci3 issuer: " );
		 strncat( err_buf,	Buffer, 50 );
    }

	 strncpy(static_Rtdcwk_Key, Ncf01_Ncdci3_I.retired_cwk,32);
	 strncpy(MAC,Ncf01_Ncdci3_I.kek,16);
}

void ncdci2_Get_Key_from_ncf01_keyblock_table(void)
{

	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	INT    ret_val = true;

	memset( &Ncf01_Keyblock_Ncdci3_I, 0x00, KEYBLOCK_ENABLE );
    memset( Buffer, 0x00, sizeof(Buffer) );

    strncpy(Ncf01_Keyblock_Ncdci3_I.network_id, "ncdci3",6);

    ret_val = ncdci2_get_ncf01_keyblock_with_wait( &Ncf01_Keyblock_Ncdci3_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on getting key from Ncdci3 issuer: " );
		 strncat( err_buf,	Buffer, 50 );
    }

	 strncpy(static_Rtdcwk_Key_keyblock, Ncf01_Keyblock_Ncdci3_I.iss_rtdcwk_keyblock,32);
	 strncpy(MAC_Keyblock,Ncf01_Keyblock_Ncdci3_I.acq_kek_keyblock,16);
}

void ncdci2_Get_Pin_Transalation_Info(void)
{

	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	CHAR   err_buf1[100] = {0};
	INT    ret_val = true;
	
	// We have to have a copy for Diners Accquring module details so that we can do pin transaction
	 memset( &Ncf01_Ncdci_I, 0x00, Ncf01_Size );
	 memset( Buffer, 0x00, sizeof(Buffer) );
	 strncpy(Ncf01_Ncdci_I.primary_key.network_id, "ncdci",5);
	 Ncf01_Ncdci_I.primary_key.network_type[0] = 'I';
	 ret_val = ncdci2_get_ncf01_with_wait( &Ncf01_Ncdci_I, Buffer);
	 if ( false == ret_val )
	 {
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Ncdci Issuing: " );
		 strncat( err_buf,	Buffer, 50 );
		 ncdci2_log_message( 2, 3, err_buf, "ncdci2_Get_Pin_Transalation_Info",0 );
	 }	
	 memset( &Ncf01_Ncdci_A, 0x00, Ncf01_Size );
	 memset( Buffer, 0x00, sizeof(Buffer) );
	 strncpy(Ncf01_Ncdci_A.primary_key.network_id, "ncdci",5);
	 Ncf01_Ncdci_A.primary_key.network_type[0] = 'A';
	 ret_val = ncdci2_get_ncf01_with_wait( &Ncf01_Ncdci_A, Buffer);
	 if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Ncdci Acquring: " );
		 strncat( err_buf,	Buffer, 50 );
		 ncdci2_log_message( 2, 3, err_buf, "ncdci2_Get_Pin_Transalation_Info",0 );
	}			 

}

void ncdci2_Get_Pin_Transalation_Info_keyblock(void)
{
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};
	CHAR   err_buf1[100] = {0};
	INT    ret_val = true;

	memset( &Ncf01_Ncdci_I, 0x00, Ncf01_Size );
	memset( Buffer, 0x00, sizeof(Buffer) );
	strncpy(Ncf01_Keyblock_Ncdci3_I.network_id, "ncdci",5);

	ret_val = ncdci2_get_ncf01_keyblock_with_wait( &Ncf01_Keyblock_Ncdci3_I, Buffer);
	if ( false == ret_val )
	{
		strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Ncdci Issuing: " );
		strncat( err_buf,	Buffer, 50 );
		ncdci2_log_message( 2, 3, err_buf, "ncdci2_Get_Pin_Transalation_Info_keyblock",0 );
	}
}



/*************************************************************************/
