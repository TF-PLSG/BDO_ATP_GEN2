/***************************************************************************
 *
 * Copyright (c) 2002, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_database.c
 *  
 * TITLE:       ncdci  Network Controller Database Processor
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
#include "tx_dbstruct.h"
#include "ntutils.h"
#include "diners.h"
#include "ncdci_prototypes.h"


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
extern NCF30      Ncf30;
extern NCF01 Ncf01_ncdci_I;
extern NCF01_KEYBLOCK     Ncf01_Keyblock;


/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern NCF01 Ncf01_I_key_exchange;
extern INT  Ncf30_Size;
extern INT  Atm01_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

extern NCF01_KEYBLOCK     Ncf01_Keyblock;
extern NCF01_KEYBLOCK     Ncf01_I_key_exchange_Keyblock;
extern NCF01_KEYBLOCK 	Ncf01_ncdci_I_keyblock;

/* Miscellaneous */
extern INT  TranFlow;
extern CHAR  static_Rtdcwk_Key[33];
extern CHAR static_Rtdcwk_Key_Keyblock[33];
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
 *  NAME:         ncdci_insert_db_record
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
INT ncdci_insert_db_record( BYTE data_type )
{
	INT    ret_val = true;
	INT    length = 0;
	INT    continue_with_insert = true;
	CHAR   db_table[6]  = {0};
	CHAR   err_buf[100] = {0};
	CHAR   Buffer[200] = {0};
	BYTE   db_buffer[AUTH_TX_SIZE + TLF01_SIZE] = {0};

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
		{
			continue_with_insert = false;
		}
	}

	if ( continue_with_insert == true )
	{
		ret_val = ncdci_build_and_send_request_message( MT_DB_REQUEST,
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
			ncdci_log_message( 1, 3, Buffer, "ncdci_insert_db_record",1 );
		}
	}

	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_get_db_record
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
INT ncdci_get_db_record( BYTE app_data_type, BYTE subtype2, pBYTE network_type )
{
	pPTE_MSG  p_msg   = NULL_PTR;
	INT       ret_val = false;
	INT       buf_len = 0;
	BYTE      buffer[AUTH_TX_SIZE + AUTH_TX_SIZE] = {0};
	CHAR      err_buf[200] = {0};
	CHAR      error_msg[100] = {0};
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
	ret_val = ncdci_build_and_send_request_message(  MT_DB_REQUEST,
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
		ncdci_log_message( 1, 3, error_msg, "get_db_record" ,1);
		memset( err_buf, 0x00, sizeof(err_buf) );
		sprintf( err_buf,
				"Unable to select record from database for table type %d.",
				app_data_type );
		ncdci_log_message( 1, 3, err_buf, "get_db_record",1 );
	}

	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_update_database
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
INT ncdci_update_database()
{
	INT    ret_val = false;
	INT    length = 0;
	CHAR   err_buf[100] = {0};
	CHAR   Buffer[200] = {0};
	BYTE   db_buffer[AUTH_TX_SIZE] = {0};

	memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
	length = Auth_Tx_Size;

	ret_val = ncdci_build_and_send_request_message( MT_DB_REQUEST,
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
		ncdci_log_message( 1, 3, Buffer, "ncdci_update_database",1 );
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
 *  NAME:         ncdci_process_db_reply
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
void ncdci_process_db_reply( pPTE_MSG p_msg )
{
	/* Determine if database action was successful or not. */
	if ( PTEMSG_OK == ptemsg_get_result_code(p_msg) )
	{
		ncdci_process_db_ok( p_msg );
	}
	else
	{
		ncdci_process_db_error( p_msg );
	}
	return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_process_db_ok
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
void ncdci_process_db_ok( pPTE_MSG p_msg )
{
	pBYTE         p_data_stream = NULL_PTR;
	pPTE_MSG_DATA p_msg_data = NULL_PTR;
	BYTE          subtype1 = 0;
	BYTE          app_data_type = 0;
	CHAR          Errorbuf[100] = {0};

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg );
	p_data_stream = ptemsg_get_pte_msg_data_data( p_msg_data );

	app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
	subtype1 = ptemsg_get_msg_subtype1( p_msg );
	memset( &Auth_Tx, 0x00,          Auth_Tx_Size );
	memcpy( &Auth_Tx, p_data_stream, Auth_Tx_Size );

	if ( ST1_DB_UPDATE == subtype1 )
	{
		if ( ALL_DATA == app_data_type )
		{
			/* Database is updated. Send response to host and insert into TLF01. */
			TranFlow = DINERS_SWITCHED_IN;
			ncdci_send_response_to_host( DINERS_SEND_IF_NOT_REVERSAL );
			ncdci_log_message( 2, 1, "Response Send to ncdci Host",
									 "ncdci_process_db_ok", 1 );
		}
		else if(NCF01_DATA == app_data_type)
		{
			 if((0 == strncmp(AppName,"ncdci3",6)))
			 {
				if(Auth_Tx.TLF01_details.tx_key == AUTH_PIN_CHANGE ||
				   Auth_Tx.TLF01_details.tx_key == AUTH_PIN_CHANGE_RESPONSE)
				{
					ncdci_log_message( 1, 1, "Updated key in to NCF01 table",
									 "ncdci_process_db_ok", 0 );
					ncdci_log_message( 1, 1, "Send message to ncdci to updated key from NCF01",
											"ncdci_process_db_ok", 0 );
					ncdci_send_update_Key_message_to_ncdci_ACQ();
					return;
				}
				else
				{
					sprintf(Errorbuf,"NCF01 Operation is Success for message type: %s and ATP key type: %s  ",
					Auth_Tx.TLF01_details.message_type, Auth_Tx.TLF01_details.tx_key);
					ncdci_log_message( 1, 1, Errorbuf, "ncdci_process_db_ok",0 );
				}
			 }
			 else
			 {
				 ncdci_log_message( 1, 1, "Updated key in to NCF01 table",
				 						  "ncdci_process_db_ok", 0 );
			 }
		}
		else if(NCF01_KEYBLOCK01_DATA == app_data_type)
		{
			 if((0 == strncmp(AppName,"ncdci3",6)))
			 {
				if(Auth_Tx.TLF01_details.tx_key == AUTH_PIN_CHANGE ||
				   Auth_Tx.TLF01_details.tx_key == AUTH_PIN_CHANGE_RESPONSE)
				{
					ncdci_log_message( 1, 1, "Updated key in to NCF01 table",
									 "ncdci_process_db_ok", 0 );
					ncdci_log_message( 1, 1, "Send message to ncdci to updated key from NCF01",
											"ncdci_process_db_ok", 0 );
					ncdci_send_update_Key_message_to_ncdci_ACQ_keyblock();
					return;
				}
				else
				{
					sprintf(Errorbuf,"NCF01 Operation is Success for message type: %s and ATP key type: %s  ",
					Auth_Tx.TLF01_details.message_type, Auth_Tx.TLF01_details.tx_key);
					ncdci_log_message( 1, 1, Errorbuf, "ncdci_process_db_ok",0 );
				}
			 }
			 else
			 {
				 ncdci_log_message( 1, 1, "Updated key in to NCF01 table",
										  "ncdci_process_db_ok", 0 );
			 }
		}
	}

	else  if ( ST1_DB_SELECT== subtype1 )
	{
		if (NCF30_DATA == app_data_type)
		{

			if (false == ncdci_process_incoming_message_ncf30_reply(p_data_stream))
			{
				ncdci_log_message( 1, 1, "Unable to send the request to the authorizer application",
										"ncdci_process_db_ok" , 1);
			}
		}
		else
		{
			ncdci_log_message( 1, 1, "Invalid incoming select message type",
									"ncdci_process_db_ok", 1 );
		}
	}
	return;
}


/******************************************************************************
 *
 *  NAME:         ncplse_process_db_error
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
void ncdci_process_db_error( pPTE_MSG p_msg )
{
	pBYTE         p_data_stream = NULL_PTR;
	pPTE_MSG_DATA p_msg_data = NULL_PTR;
	BYTE          app_data_type = 0;
	BYTE          subtype1 = 0;
	CHAR          buffer[300] = {0};
	CHAR          error_msg[200] = {0};

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
			 if ( AUTH_ADVICE == Auth_Tx.TLF01_details.tx_key|
				  AUTH_ADVICE_REPEAT == Auth_Tx.TLF01_details.tx_key)
			 {
				 if (false == ncdci_hhutil_transfer_ctl_to_auth_app(&Auth_Tx))
				 {
				   ncdci_log_message( 1, 3, "Unable to send Advice request to application",
											   "ncdci_process_db_error",1 );
				 }
			 }
			 else
			 {
				 /* Unable to find original txn for a reversal.  Send to Cadencie for Approval. */
				 if (false == ncdci_hhutil_transfer_ctl_to_auth_app(&Auth_Tx))
				 {
				   ncdci_log_message( 1, 3, "Unable to send Reversal request to application",
											   "ncdci_process_db_error",1 );
				 }
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
			TranFlow = DINERS_SWITCHED_IN;
			strcpy( Auth_Tx.TLF01_details.response_code, DINERS_SYSTEM_ERROR );
			Auth_Tx.TLF01_details.auth_number[0] = 0x00;
			ncdci_send_response_to_host( DINERS_SEND_IF_NOT_REVERSAL );
			sprintf( buffer,
					 "Unable to update DB with txn info. Error = %s",
					 error_msg );
			ncdci_log_message( 1, 3, buffer, "ncdci_process_db_error", 1 );
		}
		else if(NCF01_DATA == app_data_type)
		{
			ncdci_log_message( 1, 1, "Updated key details in to NCF01 table failed",
									 "ncdci_process_db_ok", 0 );
					return;
		}
		else if(NCF01_KEYBLOCK01_DATA == app_data_type)
		{
			ncdci_log_message( 1, 1, "Updated key details in to NCF01 table failed",
									 "ncdci_process_db_ok", 0 );
					return;
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

  Function:    ncdci_get_ncf01_with_wait

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
INT ncdci_get_ncf01_with_wait( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out = NULL_PTR;
   pPTE_MSG	     p_msg_in = NULL_PTR;
   INT           ret_val = true;
   pBYTE         p_data = NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          rcode = 0 ;
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
      if(PTEMSG_OK == (rcode = ncdci_send_interactive_db_request(p_msg_out, &p_msg_in,
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

/*************************************************************************/
INT ncdci_get_ncf01_keyblock_with_wait( pNCF01_KEYBLOCK pncf01_keyblock, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out = NULL_PTR;
   pPTE_MSG	     p_msg_in = NULL_PTR;
   INT           ret_val = true;
   pBYTE         p_data = NULL_PTR;
   pPTE_MSG_DATA p_msg_data = NULL_PTR;
   BYTE          rcode = 0 ;
   BYTE          buffer[AUTH_TX_SIZE + NCF01_SIZE] = {0};
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
      if(PTEMSG_OK == (rcode = ncdci_send_interactive_db_request(p_msg_out, &p_msg_in,
                                                           netds_que_name,
                                                           error_msg )) )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memcpy( pncf01_keyblock, p_data+Auth_Tx_Size, NCF01_KEYBLOCK01_DATA );
      }

      if ( PTEMSG_IPC_ERROR != rcode )
         free( p_msg_in );
      else
         ret_val = false;
   }
   return ( ret_val );
}


/*****************************************************************************

  Function:    ncdci_send_interactive_db_request

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
BYTE ncdci_send_interactive_db_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                                  pCHAR que_name,     pCHAR error_msg )
{
   pBYTE           p_data = NULL_PTR;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   CHAR            temp_str[100] = {0};
   LONG            retcode = 0 ;
   BYTE            dberror = 0;

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

void ncdci_Get_Key_from_ncf01_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};

	memset( &Ncf01_ncdci_I, 0x00, Ncf01_Size );
	memset( Buffer, 0x00, sizeof(Buffer) );

	strncpy(Ncf01_ncdci_I.primary_key.network_id, "ncdci3",6);
	Ncf01_ncdci_I.primary_key.network_type[0] = 'I';
	ret_val = ncdci_get_ncf01_with_wait( &Ncf01_ncdci_I, Buffer);
	if ( false == ret_val )
	{
		strcpy(  err_buf, "Error on getting key from ncdci issuer: " );
		strncat( err_buf,	Buffer, 50 );
	}
	memcpy( Ncf01_I_key_exchange.retired_cwk,Ncf01_ncdci_I.retired_cwk,
			DINERS_DE96_KEY_LEN);
	memcpy( Ncf01_I_key_exchange.cwk,Ncf01_ncdci_I.cwk,
			DINERS_DE96_KEY_LEN);
	memcpy( static_Rtdcwk_Key, Ncf01_ncdci_I.cwk,
			DINERS_DE96_KEY_LEN);
	memcpy( Ncf01_I_key_exchange.prev_src_station,Ncf01_ncdci_I.prev_src_station,
			DINERS_DE96_KEY_CHECK_LEN);
}

void ncdci_Get_Key_from_ncf01_keyblock_table(void)
{
	INT    ret_val = true;
	CHAR   Buffer[200] = {0};
	CHAR   err_buf[100] = {0};

	memset( &Ncf01_Keyblock, 0x00, NCF01_KEYBLOCK01_SIZE );
	memset( Buffer, 0x00, sizeof(Buffer) );

	strncpy(Ncf01_Keyblock.network_id, "ncdci3",6);
	ret_val = ncdci_get_ncf01_keyblock_with_wait( &Ncf01_ncdci_I, Buffer);
	if ( false == ret_val )
	{
		strcpy(  err_buf, "Error on getting key from ncdci issuer: " );
		strncat( err_buf,	Buffer, 50 );
	}
	memcpy( Ncf01_I_key_exchange_Keyblock.iss_rtdcwk_keyblock,
			Ncf01_ncdci_I_keyblock.iss_rtdcwk_keyblock,
			DINERS_DE96_KEY_LEN);

	memcpy( Ncf01_I_key_exchange_Keyblock.iss_cwk_keyblock,
			Ncf01_ncdci_I_keyblock.iss_cwk_keyblock,
			DINERS_DE96_KEY_LEN);

	memcpy( static_Rtdcwk_Key_Keyblock,
			Ncf01_ncdci_I_keyblock.iss_cwk_keyblock,
			DINERS_DE96_KEY_LEN);

	memcpy( Ncf01_I_key_exchange_Keyblock.iss_prev_src_station,
			Ncf01_ncdci_I_keyblock.iss_prev_src_station,
			DINERS_DE96_KEY_CHECK_LEN);
}

