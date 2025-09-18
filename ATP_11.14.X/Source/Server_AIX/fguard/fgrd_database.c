/***************************************************************************
 *
 * Copyright (c) 2005, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      FGRD_DATABASE.C
 *  
 * TITLE:       Equitable Fraudguard Database Processor
 *  
 * DESCRIPTION: This module contains functions that handle requests to
 *              and responses from a data server.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrd_database.c  $
   
      Rev 1.2   Jul 19 2005 11:33:14   dirby
   Added code to make sure garbage is not sent to the data server
   when doing a batch resend of all failed transactions.  In this case,
   no transaction Id is sent to the data server.
   SCR 14421
   
      Rev 1.1   Jul 15 2005 09:49:02   dirby
   Added transaction Id to an error message to identify the transaction.
   SCR 14421
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
   Initial revision.
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
#include "fgrd_constants.h"
#include "fgrd_prototypes.h"
#include "tx_dbstruct.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];

/* QUEUES */
extern CHAR       updateds_que_name[];
extern CHAR       trands_que_name[];
extern CHAR       netds_que_name[];
extern CHAR       applnk_que_name[];
extern CHAR       app_name[];
extern CHAR       authorizer_que_name[];
extern CHAR       timerds_que_name [];


/* Database structures */
extern AUTH_TX   Auth_Tx;
extern CRF01     Crf01;
extern FG01      Fg01;
extern FGUARD    Fguard;
extern FG_RAW    Fgraw;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Crf01_Size;
extern INT  Fguard_Size;
extern INT  Fgraw_Size;
extern INT  Fg01_Size;

/* Miscellaneous */
extern INT  TranFlow;


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
   CHAR   db_table[14]  = "";
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   BYTE   db_buffer[sizeof(AUTH_TX) + sizeof(FGUARD) + sizeof(FG_RAW)] = "";

   if ( data_type == FGUARD_DATA )
   {
      memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
      memcpy( db_buffer+Auth_Tx_Size, &Fguard, Fguard_Size );
      memcpy( db_buffer+Auth_Tx_Size+Fguard_Size, &Fgraw, Fgraw_Size );

      length   = Auth_Tx_Size + Fguard_Size + Fgraw_Size;
      strcpy( db_table, "FGUARD+FGRAW" );
   }

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
      log_message( 3, 2, Buffer, "insert_db_record" );
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
 *                db_keys       - Keys to database records
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *  
 ******************************************************************************/
INT get_db_record( BYTE app_data_type, BYTE subtype2, pBYTE db_keys )
{
   pPTE_MSG  p_msg   = NULL_PTR;
   INT       len;
   INT       ret_val;
   INT       buf_len;
   BYTE      buffer[sizeof(AUTH_TX) + sizeof(FGRAW_GUI_LIST)] = "";
   CHAR      err_buf[200];
   CHAR      error_msg[100];
   CHAR      table[10] = "";


   if ( FGRAW_DATA == app_data_type )
   {
      /* Build primary key to select original transaction. */
      len = strlen( db_keys );
      if ( len == 0 )
         len = 10;  /* Send 10 add'l nulls to prevent garbage at data server */
      memcpy( buffer+Auth_Tx_Size, db_keys, len );
      buf_len = Auth_Tx_Size + len;
      strcpy( table, "FG_RAW" );
   }

   /* Build PTE message and send it to data server. */
   ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                             ST1_DB_SELECT,
                                             subtype2,
                                      (pBYTE)trands_que_name,
                                             buffer,
                                             buf_len,
                                             app_data_type,
                                             NULL_STR,
                                             error_msg   );

   if ( ret_val == false )
   {
      log_message( 3, 3, error_msg, "get_db_record" );
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unable to select record from database for table %s. Subtype2 %d",
               table, subtype2 );
      log_message( 3, 3, err_buf, "get_db_record" );
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         UPDATE_DATABASE
 *
 *  DESCRIPTION:  This function issues a command to the data server to
 *                update a record in FGUARD and insert into FG_RAW.
 *
 *  INPUTS:       Fguard        - (Global) XML Transaction Structure
 *                ReqRsp_Buffer - (Global) Raw data message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT update_database( BYTE data_type )
{
   INT    ret_val;
   INT    length;
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   CHAR   db_table[10] = "";
   BYTE   db_buffer[sizeof(AUTH_TX) + sizeof(FGUARD) + sizeof(FG_RAW)] = "";

   if ( data_type == FGUARD_DATA )
   {
      memcpy( db_buffer, &Auth_Tx, Auth_Tx_Size );
      memcpy( db_buffer+Auth_Tx_Size, &Fguard, Fguard_Size );
      memcpy( db_buffer+Auth_Tx_Size+Fguard_Size, &Fgraw, Fgraw_Size );

      length   = Auth_Tx_Size + Fguard_Size + Fgraw_Size;
      strcpy( db_table, "FGUARD" );
   }

   ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                             ST1_DB_UPDATE,
                                             ST2_NONE,
                                      (pBYTE)updateds_que_name,
                                             db_buffer,
                                             length,
                                             FGUARD_DATA,
                                             NULL_STR,
                                             err_buf   );
   if ( ret_val == false )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Unable to update Fguard info into database. Error = %s",
               err_buf );
      log_message( 3, 2, Buffer, "update_database" );
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
   BYTE          subtype2;
   BYTE          app_data_type;

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg );
   p_data_stream = ptemsg_get_pte_msg_data_data( p_msg_data );
   
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   subtype1 = ptemsg_get_msg_subtype1( p_msg );
   subtype2 = ptemsg_get_msg_subtype2( p_msg );

   if ( ST1_DB_SELECT == subtype1 )
   {
      if ( FGRAW_DATA == app_data_type )
      {
         /* Retrieved one or more FG_RAW records for resending. */
         (void)continue_resend_process( p_data_stream, subtype2 );
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
   BYTE          subtype2;
   CHAR          buffer[300] = "";
   CHAR          error_msg[200] = "";

   p_msg_data    = ptemsg_get_pte_msg_data(p_msg);
   p_data_stream = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset( &Auth_Tx,  0x00,          Auth_Tx_Size );
   memcpy( &Auth_Tx,  p_data_stream, Auth_Tx_Size );
   strcpy( error_msg, p_data_stream+ Auth_Tx_Size );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1      = ptemsg_get_msg_subtype1(p_msg);
   subtype2      = ptemsg_get_msg_subtype2(p_msg);

   if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( FGUARD_DATA == app_data_type )
      {
         sprintf( buffer,
                 "Err updating FGUARD (tranid: %s): %s",
                  Auth_Tx.TLF01_details.primary_key.transaction_id,
                  error_msg );
         log_message( 3, 2, buffer, "process_db_error" );
      }
   }
   else if ( ST1_DB_INSERT == subtype1 )
   {
      if ( FGUARD_DATA == app_data_type )
      {
         sprintf( buffer,
                 "Error inserting FGUARD or FG_RAW: %s",
                  error_msg );
         log_message( 3, 2, buffer, "process_db_error" );
      }
   }
   else if ( ST1_DB_SELECT == subtype1 )
   {
      if ( FGRAW_DATA == app_data_type )
      {
         sprintf( buffer,
                 "Error selecting FG_RAW (subtype2 %d) for resend: %s",
                  subtype2, error_msg );
         log_message( 3, 2, buffer, "process_db_error" );
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

  Function:    GET_FG01_WITH_WAIT

  Description: This function will send a request to the dataserver to select 
               FG01.  This is done syncronously.

  Author:      D Irby

  Inputs:      pncf01    - pointer to the requested ncf01 structure

  Outputs:     pncf01    - pointer to the returned ncf01 structure
               error_msg - error message returned by the data server

  Return values:  true      - No errors
                  false     - Error
******************************************************************************/
INT get_fg01_with_wait( pFG01 pfg01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   INT           ret_val = true;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX) + sizeof(FG01)];
   AUTH_TX       auth_tx;

   memset( buffer,0, sizeof(buffer) );
   memset( &auth_tx, 0x00, Auth_Tx_Size );

   memcpy( buffer, &auth_tx, Auth_Tx_Size );
   memcpy( buffer+Auth_Tx_Size,pfg01, Fg01_Size);

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 FG01_DATA );
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

         memcpy( pfg01, p_data+Auth_Tx_Size, Fg01_Size );
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

