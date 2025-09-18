/***************************************************************************
 *
 * Copyright (c) 2002, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup2_database.c
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
#include "nccup2_prototypes.h"
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
extern CHAR       oprmon_que_name[];
extern CHAR       applnk_que_name[];
extern CHAR       app_name[];
extern CHAR       authorizer_que_name[];
extern CHAR       timerds_que_name [];

/* Database structures */
extern AUTH_TX    Auth_Tx;
extern NCF01      Ncf01_I;
extern NCF01      Ncf01_A;
extern NCF01 Ncf01_Nccup3_I;
extern NCF01 Ncf01_Nccup_I;
extern NCF01 Ncf01_Nccup_A;
extern NCF30      Ncf30;


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
extern CHAR  security_que_name[];
extern CHAR   MAC[17];


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
         nccup2_log_message( 1, 3, Buffer, "insert_db_record",1 );
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
      nccup2_log_message( 1, 3, error_msg, "get_db_record",1 );
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unable to select record from database for table type %d.",
               app_data_type );
      nccup2_log_message( 1, 3, err_buf, "get_db_record",1 );
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
      nccup2_log_message( 1, 3, Buffer, "update_database",1 );
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
   CHAR          buffer[300] = "";

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
         TranFlow = SWITCHED_IN;
         memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
         memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );
         send_response_to_host( SEND_IF_NOT_REVERSAL );
      }
	  else if(NCF01_DATA == app_data_type)
	  {
	   	 strcpy( buffer,"Updated key in to NCF01 table");
         nccup2_log_message( 2, 1, buffer, "process_db_ok",0 );
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
   pBYTE         p_data_stream, pchain_data;
   pPTE_MSG_DATA p_msg_data;
   pPTE_MSG p_echo_msg;
   BYTE          app_data_type;
   BYTE          subtype1;
   CHAR          buffer[300] = "";
   CHAR          error_msg[512] = "";
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
         strcpy( Auth_Tx.TLF01_details.response_code, CUP_ISO_APPROVED );
         send_response_to_host( SEND_UNCONDITIONALLY );
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
         strcpy( Auth_Tx.TLF01_details.response_code, CUP_SYSTEM_ERROR );
         Auth_Tx.TLF01_details.auth_number[0] = 0x00;
         send_response_to_host( SEND_IF_NOT_REVERSAL );
         sprintf( buffer,
                 "Unable to update DB with txn info. Error = %s",
                  error_msg );
         nccup2_log_message(1, 3, buffer, "process_db_error",1 );
      }
	  else if(NCF01_DATA == app_data_type)
	  {
	   	 sprintf( buffer,
                 "Updated key in to NCF01 table Failed. Error = %s",
                  error_msg );
         nccup2_log_message( 1, 3, buffer, "process_db_error",0 );
	  }
	  else if((strncmp(orig_que,"timerdsA",5)==0 ))
	   {
	  	  	  memset(&Auth_Tx,0x00,data_len);
	  	  	  memcpy(&Auth_Tx,&org_auth_tx,data_len);
	     	/*	TF Phani and Raghu
	     	 * This is request from BDo - Marvin -Falcon RT
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
		 			if( strncmp(org_auth_tx.TLF01_details.response_code,"00",2) ==0 &&
	 					(strncmp(org_auth_tx.TLF01_details.message_type,"0110",4) ==0 ||
	 					strncmp(org_auth_tx.TLF01_details.message_type,"0210",4) ==0))
	 			{
	 				sprintf(error_msg,"CUPTO Sending Reversal for Late resposne subtype1: %d orig_que %s STAN : %s",subtype1,orig_que,org_auth_tx.TLF01_details.sys_trace_audit_num);
	 	             memset(buffer,0x00,sizeof(buffer));
	 	             sprintf(buffer," Term Id:%s ,Merch Id: %s RRN: %s ",
	 	            		org_auth_tx.TLF01_details.terminal_id,org_auth_tx.TLF01_details.merchant_id,org_auth_tx.TLF01_details.retrieval_ref_num);
	 	             strcat(error_msg,buffer);
	 	             nccup2_log_message( 3, 1, error_msg , "mcrdutil_parse_db_error" , 0 );

	 				org_auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
	 				strncpy(org_auth_tx.TLF01_details.orig_message,org_auth_tx.TLF01_details.message_type,4);
	 				strcpy(org_auth_tx.TLF01_details.message_type, "0420");
	 				org_auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
	 				strcpy( org_auth_tx.TLF01_details.orig_sys_trace_num, org_auth_tx.TLF01_details.sys_trace_audit_num);
	 				strcpy( org_auth_tx.TLF01_details.orig_retrieval_ref_num, org_auth_tx.TLF01_details.retrieval_ref_num);
	 				strncpy(org_auth_tx.TLF01_details.product_codes[3].quantity,CUP_TIMEOUT,2);
	 				memset( org_auth_tx.req_data_len, 0x00 , sizeof(org_auth_tx.req_data_len));
	 				memset( org_auth_tx.req_buffer, 0x00 , sizeof(org_auth_tx.req_buffer));
	 				memset( org_auth_tx.resp_data_len, 0x00 , sizeof(org_auth_tx.resp_data_len));
	 				memset( org_auth_tx.resp_buffer, 0x00 , sizeof(org_auth_tx.resp_buffer));
	 				/*TOR : Time out reversal -> it is used to drop the response 410 coming from host
	 				 * for this reversal request sent for CUP timeout
	 				 *
	 				 * */
	 				if(strlen(Ncf01_I.tpdu_id)> 0)
	 				{
	 					strncpy( org_auth_tx.TLF01_details.tpdu_id, Ncf01_I.tpdu_id,sizeof(Ncf01_I.tpdu_id));
	 				}
	 				else
	 				{
	 					strncpy( org_auth_tx.TLF01_details.tpdu_id,"CUP2",4);
	 				}
	 				strcpy(org_auth_tx.TLF01_details.response_text, "LR: Timed out reversal for 0110");
	 				nccup2_send_reversal_to_host_for_late_response();
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
//
BYTE send_interactive_to_racal( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
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
              "Communication Error during request to Racal: %s",
               temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

      strncpy (temp_str, p_data+Auth_Tx_Size, sizeof(temp_str)-1);
      sprintf( error_msg, "Racal Error: %s", temp_str );
   }
   return dberror;
}

void Get_Key_from_ncf01_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = "";
	CHAR   err_buf[100] = "";

	memset( &Ncf01_Nccup3_I, 0x00, Ncf01_Size );
    memset( Buffer, 0x00, sizeof(Buffer) );
	// Read data from CUP Issuing link.
    strncpy(static_Rtdcwk_Key, 0x00,32);
    strncpy(MAC,0x00,16);
    strncpy(Ncf01_Nccup3_I.primary_key.network_id, "nccup3",6);
    Ncf01_Nccup3_I.primary_key.network_type[0] = 'I';
    ret_val = get_ncf01_with_wait( &Ncf01_Nccup3_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on getting key from Nccup3 issuer: " );
		 strncat( err_buf,	Buffer, 50 );
    }
	else
	{
		strncpy(static_Rtdcwk_Key, Ncf01_Nccup3_I.retired_cwk,32);
		strncpy(MAC,Ncf01_Nccup3_I.kek,16);
	}
}

void Get_Pin_Transalation_Info(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = "";
	CHAR   err_buf[100] = "";
	CHAR   err_buf1[100] = "";
	
	// We have to have a copy for CUP Accquring module details so that we can do pin transaction
	strcpy(  err_buf1, "Read Pin transalation details from nccup tables " );
	nccup2_log_message( 1, 1, err_buf1, "Get_Pin_Transalation_Info",0 );
	 memset( &Ncf01_Nccup_I, 0x00, Ncf01_Size );
	 memset( Buffer, 0x00, sizeof(Buffer) );
	 strncpy(Ncf01_Nccup_I.primary_key.network_id, "nccup",5);
	 Ncf01_Nccup_I.primary_key.network_type[0] = 'I';
	 ret_val = get_ncf01_with_wait( &Ncf01_Nccup_I, Buffer);
	 if ( false == ret_val )
	 {
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Nccup Issuing: " );
		 strncat( err_buf,	Buffer, 50 );
		 nccup2_log_message( 1, 3, err_buf, "Get_Pin_Transalation_Info",0 );
	 }	
	 memset( &Ncf01_Nccup_A, 0x00, Ncf01_Size );
	 memset( Buffer, 0x00, sizeof(Buffer) );
	 strncpy(Ncf01_Nccup_A.primary_key.network_id, "nccup",5);
	 Ncf01_Nccup_A.primary_key.network_type[0] = 'A';
	 ret_val = get_ncf01_with_wait( &Ncf01_Nccup_A, Buffer);
	 if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Nccup Acquring: " );
		 strncat( err_buf,	Buffer, 50 );
		 nccup2_log_message( 1, 3, err_buf, "Get_Pin_Transalation_Info",0 );
	}			 

}
INT nccup2_Get_Pin_Transalation_Info_from_nccup_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = "";
	CHAR   err_buf[100] = "";
	CHAR   err_buf1[100] = "";

	// We have to have a copy for CUP Accquring module details so that we can do pin transaction
	strcpy(  err_buf1, "Read Pin transalation details from nccup tables after Active_Active_triger " );
	nccup2_log_message( 1, 1, err_buf1,
						"Get_Pin_Transalation_Info_After_Active_Active_triger",0 );
	memset( &Ncf01_Nccup_I, 0x00, Ncf01_Size );
	memset( Buffer, 0x00, sizeof(Buffer) );
	strncpy(Ncf01_Nccup_I.primary_key.network_id, "nccup",5);
	Ncf01_Nccup_I.primary_key.network_type[0] = 'I';
	ret_val = get_ncf01_with_wait( &Ncf01_Nccup_I, Buffer);
	if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Nccup Issuing: " );
		 strncat( err_buf,	Buffer, 50 );
		 nccup2_log_message( 1, 1, err_buf,
				 	 	 	 "Get_Pin_Transalation_Info_After_Active_Active_triger",0 );
	 }
	 memset( &Ncf01_Nccup_A, 0x00, Ncf01_Size );
	 memset( Buffer, 0x00, sizeof(Buffer) );
	 strncpy(Ncf01_Nccup_A.primary_key.network_id, "nccup",5);
	 Ncf01_Nccup_A.primary_key.network_type[0] = 'A';
	 ret_val = get_ncf01_with_wait( &Ncf01_Nccup_A, Buffer);
	 if ( false == ret_val )
	{
		 strcpy(  err_buf, "Error on Get_Pin_Transalation_Info Nccup Acquring: " );
		 strncat( err_buf,	Buffer, 50 );
		 nccup2_log_message( 1, 3, err_buf,
				 	 	 	 "Get_Pin_Transalation_Info_After_Active_Active_triger",0 );
	}
	 return ret_val;

}

void nccup2_Get_cup_key_Info_from_nccup3_table ()
{
	 LONG			 data_len;
	 CHAR			 ErrorMsg[100]="";
	 CHAR   		 Buffer[100]="";

	memset(static_Rtdcwk_Key,0x00,32);
	memset(MAC,0x00,16);
	Get_Key_from_ncf01_table();
	if ((strlen(static_Rtdcwk_Key)> 0) && (strlen(MAC)> 0))
	{
		strcpy(ErrorMsg,"Sending New key to Cadecncie after Active-Active");
		nccup2_log_message( 1, 1, ErrorMsg,
							"nccup2_Get_cup_key_Info_from_nccup3_table",0 );
		perform_host_key_send();
		memcpy(Ncf01_I.retired_cwk,static_Rtdcwk_Key, strlen(static_Rtdcwk_Key));
		memcpy(Ncf01_I.kek,MAC, strlen(MAC));
		update_ncf01(&Auth_Tx);
	}
	else
	{
		strcpy(ErrorMsg,"Key update failed ");
		nccup2_log_message( 1, 3, ErrorMsg,
							"nccup2_Get_cup_key_Info_from_nccup3_table",0 );
	}
}


void nccup2_process_active_active_trigger_from_nccup3()
{
	INT    ret_val = true;
	char err_buf[200]= {0};

	ret_val= nccup2_Get_Pin_Transalation_Info_from_nccup_table();
	if(ret_val == true)
	{
		nccup2_Get_cup_key_Info_from_nccup3_table();
	}
}
