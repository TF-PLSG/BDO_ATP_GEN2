/*******************************************************************************
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      amex1304.c
 *  
 * TITLE:       American Express Online File Update Handler
 *  
 * DESCRIPTION: This module contains functions that handle Online File
 *              Update requests from the ATP GUI, and responses from
 *              the Amex host.
 *
 * APPLICATION: ATP in Equitable
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\ncamex\amex1304.c  $
   
      Rev 1.0   Jan 15 2004 16:41:44   dirby
   Initial revision.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "amexmain.h"
#include "amextran.h"
#include "amexutil.h"

/***********/
/* GLOBALS */
/***********/
extern AMEX_ONLINE_UPDATES  Online_Updates;
extern NCF01                ncf01;
extern CHAR                 AppName[];
extern CHAR                 updateds_que_name[];

CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_REQUEST
 *
 *  DESCRIPTION:  This function begins the process of sending an 1304 message
 *                to AMEX.  The 1304 transaction is built based on information
 *                sent from the GUI, information gathered from NCF01, and some
 *                data that is created here.  Once built, the 1304 is sent to
 *                AMEX.
 *
 *  INPUTS:       p_msg - This is the incoming message from the GUI.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_file_update_request( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   INT             ret_val;
   CHAR            err_mesg[200] = "";
   CHAR            time_str [ 7] = "";
   CHAR            time_date[25] = "";
   CHAR            err_msg [100] = "";
   CHAR            temp     [17] = "";
   AUTH_TX         auth_tx;


   /* Copy transaction record into data structure. */
   memset( &Online_Updates, 0x00, sizeof(AMEX_ONLINE_UPDATES) );
   memset( &auth_tx,        0x00, sizeof(AUTH_TX)             );

   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);

   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Online_Updates,
            p_data_data+sizeof(AUTH_TX),
            sizeof(AMEX_ONLINE_UPDATES) );

   /* Initialize the transaction. */
   (void)init_txn( &auth_tx );
   strcpy( auth_tx.TLF01_details.dispensation.origin, "LO" );

   strcpy( auth_tx.forwarding_inst_id_cd, ncf01.forwarding_inst_id_code );

   strcpy( auth_tx.TLF01_details.forwarding_institution_id,
           ncf01.forwarding_inst_id_code );

   create_stan( auth_tx.TLF01_details.sys_trace_audit_num );
   auth_tx.TLF01_details.general_status = GS_TXN_NEW;
   auth_tx.TLF01_details.tx_key         = AUTH_CARD_UPDATE;
   strcpy( auth_tx.TLF01_details.message_type, "1304" );
   create_rrn( auth_tx.TLF01_details.retrieval_ref_num );
   strcpy( auth_tx.TLF01_details.tpdu_id, ncf01.tpdu_id );

   /* Set issuer Id for reporting purposes. */
   strncpy( auth_tx.TLF01_details.issuer_id, AppName,
            sizeof(auth_tx.TLF01_details.issuer_id) );

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, auth_tx.TLF01_details.date_yyyymmdd );
   get_time( time_date, auth_tx.TLF01_details.time_hhmmss );

   /* Get originating information */
   strcpy( auth_tx.TLF01_details.originator_queue,
           ptemsg_get_msg_orig_queue( p_msg ));

   strcpy( auth_tx.TLF01_details.originator_info,
           ptemsg_get_msg_orig_comms_info( p_msg ));

   /* Store data into auth_tx for function Build_Host_Structure. */
   (void)map_1304_to_tlf01( &auth_tx );

   /* Make sure GUI sent valid information. */
   ret_val = validate_gui_inputs( err_msg );
   if ( ret_val == false )
   {
      strcpy( auth_tx.accptor_bus_code, FIELD_EDIT_ERROR );
      issue_1304_response( FORMAT_ERROR, err_msg, &auth_tx );
   }
   else
   {
      /* Confirm host is available */
      ret_val = verify_host_app_status( &auth_tx );
      if ( false == ret_val )
      {
         /* Host is not available.  Send response to GUI. */
         strcpy( auth_tx.accptor_bus_code, AMX_HOST_UNAVAILABLE );
         issue_1304_response( HOST_UNAVAILABLE, err_msg, &auth_tx );
      }
      else
      {
         /* Host is working.  Build PTEMSG's to be sent */
         (void)send_request_to_host( &auth_tx );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         MAP_1304_TO_TLF01
 *
 *  DESCRIPTION:  This function copies information stored in the 1304 message
 *                structure into the TLF01 structure for logging purposes.
 *
 *  INPUTS:       (Global) Online_Updates
 *
 *  OUTPUTS:      p_auth_tx
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void map_1304_to_tlf01( pAUTH_TX p_auth_tx )
{
   INT   len;
   BYTE  tempstr[4] = "";

   /* Terminal ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id, Online_Updates.terminal_id, 8 );

   /* Info from GUI Fields OR from nposa2 request */
   len = strlen( Online_Updates.card_nbr );
   sprintf( tempstr, "%02d", len );
   memcpy( p_auth_tx->TLF01_details.card_num_len, tempstr, 2 );
   strcpy( p_auth_tx->TLF01_details.card_num,    Online_Updates.card_nbr );
   strcpy( p_auth_tx->TLF01_details.exp_date,    Online_Updates.exp_date );
   p_auth_tx->TLF01_details.processing_code[0] = Online_Updates.function_code[2];
   strcpy( p_auth_tx->TLF01_details.nii,         Online_Updates.reason_code );

   if(strncmp(p_auth_tx->TLF01_details.originator_queue,"nposa2A",6) == 0)  /* ThoughtFocus Girija Y */
   {
	   memcpy(p_auth_tx->TLF01_details.sys_trace_audit_num,Online_Updates.stan,6);
	   memcpy(p_auth_tx->TLF01_details.date_yyyymmdd,Online_Updates.date_time,8);
       memcpy(p_auth_tx->TLF01_details.time_hhmmss,&Online_Updates.date_time[8],6);
	   memcpy(p_auth_tx->local_date_time,&Online_Updates.date_time[2],12);
	   memcpy(p_auth_tx->TLF01_details.forwarding_institution_id,Online_Updates.fwd_inst_code,
	   sizeof(Online_Updates.fwd_inst_code));
	   memcpy(p_auth_tx->forwarding_inst_id_cd,Online_Updates.fwd_inst_code,
	   sizeof(Online_Updates.fwd_inst_code));
   }
   /* Workstation and Operator ID */
   strcpy( p_auth_tx->TLF01_details.workstation, Online_Updates.workstation );
   strcpy( p_auth_tx->TLF01_details.operator_id, Online_Updates.operator_id );

   return;
}


/******************************************************************************
 *
 *  NAME:         VALIDATE_GUI_INPUTS
 *
 *  DESCRIPTION:  This function validates the data received from the GUI as
 *                follows:
 *                   Card Number     : Must exist
 *                                     Must be numeric
 *
 *                   Expiration Date : Must exist
 *                                     Must be numeric
 *                                     Must in format YYMM
 *
 *                   Request Type    : Must be 301 (Add/Update) or 303 (Delete)
 *
 *                   Msg Reason Code : Must be one of the following:
 *                                     3000, 3001, 3002,
 *                                     3700, 3701, 3702, 3703
 *
 *  INPUTS:       (Global)Online_Updates
 *
 *  OUTPUTS:      err_msg - 40 byte explanation of invalid data
 *
 *  RTRN VALUE:   True if all data is valid, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_gui_inputs( pCHAR err_msg )
{
   INT   ret_val = false;
//   INT   i;
//   INT   in_reason_code;
   CHAR  buffer[200] = "";

   #define NUM_REASON_CODES  8
   INT   reason_codes[NUM_REASON_CODES] = {3000, 3001, 3002,
                                           3700, 3701, 3702, 3703,0000};

   /* Card Number */
   if ( Online_Updates.card_nbr[0] == 0x00 )
   {
      strcpy( err_msg, "Card number is missing" );
   }
   else if ( false == isnum(Online_Updates.card_nbr) )
   {
      sprintf( err_msg,
              "Non-numeric card number %s",
               Online_Updates.card_nbr );
   }

   /* Expiration Date */
   else if ( Online_Updates.exp_date[0] == 0x00 )
   {
      strcpy( err_msg, "Expiry date is missing" );
   }
   else if (  false == validate_date_yymm(Online_Updates.exp_date, buffer) )
   {
      memset( &buffer[199], 0x00, 1 ); /* Prevent overflow */
      sprintf( err_msg, "Expiry err: %s", buffer );
   }

   /* Request Type */
   else if ( (0 != strcmp(Online_Updates.function_code,"301")) &&
             (0 != strcmp(Online_Updates.function_code,"303"))  )
   {
      sprintf( err_msg,
              "Invalid request type (%s): expected 301 or 303",
               Online_Updates.function_code );
   }
   else 
   {
	/* Dont do any thing..Keep Smiling..No need to check reason code..req by Kenneth */
	  
	   ret_val = true;

   }

   /* Message Reason Code */
   
  
  /* else if ( Online_Updates.reason_code[0] == 0x00 )
   {
	    
            strcpy( err_msg, "Action code is missing" );
   }

   else
   {
      in_reason_code = atoi( Online_Updates.reason_code );
      for ( i=0; i<NUM_REASON_CODES; i++ )
      {
         if ( in_reason_code == reason_codes[i] )
         {
            ret_val = true;
            break;
         }
      }

      if ( i == NUM_REASON_CODES )
      {
         sprintf( err_msg,
                 "Invalid Action Code: %s",
                  Online_Updates.reason_code );
      }
   } */



   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_RESPONSE
 *
 *  DESCRIPTION:  This function handles 0314 messages.  These are the responses
 *                to 1304 messages - File Update Requests where card holder
 *                records can be added, deleted, updated, or inquired.
 *
 *  INPUTS:       p_auth_tx - Transacation data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_file_update_response( pAUTH_TX p_auth_tx )
{
   CHAR  err_msg[41] = "";

   /* Initialize the GUI structure. */
   memset( &Online_Updates, 0x00, sizeof(AMEX_ONLINE_UPDATES) );

   /* Populates the GUI structure.
    * Sends it to the GUI.
    * Logs to TLF01.
    */
   issue_1304_response( p_auth_tx->TLF01_details.response_code,
                        err_msg, p_auth_tx );
   return;
}


/******************************************************************************
 *
 *  NAME:         POPULATE_1314_RESP_STRUCT
 *
 *  DESCRIPTION:  This function copies information stored in the 1314 message
 *                structure into the TLF01 structure for logging purposes and
 *                copies it into the GUI data structure in preparation to send
 *                the response to the GUI.
 *
 *                This function could be running due to an error or upon a
 *                successful file update.  If due to a successful update, the
 *                data will be auth_tx.  If due to an error, the data will be
 *                in Online_Updates.  Need to go both ways here to be sure we
 *                log the information correctly, and send it all back to the
 *                GUI.
 *
 *  INPUTS:       resp_code - Response code
 *                err_msg   - Error message, if any
 *
 *  OUTPUTS:      (Global)Online_Updates - Information to display on GUI
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT populate_1314_resp_struct( pBYTE     resp_code,
                               pCHAR     err_msg,
                               pAUTH_TX  p_auth_tx )
{
   INT   i=0;
   BYTE  buffer[40]="";

   BYTE  resp_text[][2][40] =
         { {"00", "FILE REQUEST SUCCESSFUL"              },
           {"12", "Invalid Account"                      },
           {"25", "Unable to locate record on file."     },
           {"27", "File update field edit error."        },
           {"40", "Requested function is not supported." },
           {"63", "Higher priority item on file."        },
           {"80", "Duplicate, old record replaced."      },
           {"91", "Host is unavailable."                 },
           {"EF", ""  /* Keep this as the last field. */ }
         };

   /* Response Text */
   if ( err_msg[0] == 0x00 )
   {
      /* No response text message.  Use the default. */
      do
      {
         if ( 0 == strcmp(resp_text[i][0],resp_code) )
         {
            strcpy( buffer, resp_text[i][1] );
            break;
         }

      } while( 0 != strcmp(resp_text[++i][0], "EF") );
   }
   else
   {
      /* Use the response text message sent by parent function. */
      memcpy( buffer, err_msg, 40 );
   }

   strcpy( p_auth_tx->TLF01_details.response_text, buffer );
   if ( Online_Updates.resp_text[0] == 0x00 )
      strcpy( Online_Updates.resp_text, buffer );

   /* Response Code */
   if ( p_auth_tx->TLF01_details.response_code[0] == 0x00 )
      strcpy( p_auth_tx->TLF01_details.response_code, resp_code );

   if ( Online_Updates.resp_code[0] == 0x00 )
      strcpy( Online_Updates.resp_code, resp_code );

 /* Send as it is when it coming from nposa2 - TF Phani */
   if(0 == strcmp(p_auth_tx->TLF01_details.originator_queue,"nposa2A"))
		strcpy(Online_Updates.resp_code,p_auth_tx->accptor_bus_code);

   /* Amex Response Code - so it gets logged into TLF01. */
   strcpy( p_auth_tx->TLF01_details.auth_number, p_auth_tx->accptor_bus_code );

   /* Card Number */
   if ( Online_Updates.card_nbr[0] == 0x00 )
      strcpy( Online_Updates.card_nbr, p_auth_tx->TLF01_details.card_num );

   /* Expiration Date */
   if ( Online_Updates.exp_date[0] == 0x00 )
      strcpy( Online_Updates.exp_date, p_auth_tx->TLF01_details.exp_date );

   /* Request Type */
   if ( Online_Updates.function_code[0] == 0x00 )
   {
      strcpy( Online_Updates.function_code, "30" );
      Online_Updates.function_code[2] = p_auth_tx->TLF01_details.processing_code[0];
   }

   /* Action Code */
   if ( Online_Updates.reason_code[0] == 0x00 )
      strncpy( Online_Updates.reason_code, p_auth_tx->TLF01_details.nii,4 );

   /* Terminal ID */
   if ( Online_Updates.terminal_id[0] == 0x00 )
      strcpy( Online_Updates.terminal_id, p_auth_tx->TLF01_details.terminal_id );

   /* STAN */
   if ( Online_Updates.stan[0] == 0x00 )
      strcpy( Online_Updates.stan, p_auth_tx->TLF01_details.sys_trace_audit_num );

   /* Operator ID */
   if ( Online_Updates.operator_id[0] == 0x00 )
      strcpy( Online_Updates.operator_id, p_auth_tx->TLF01_details.operator_id );

   /* Workstation */
   if ( Online_Updates.workstation[0] == 0x00 )
      strcpy( Online_Updates.workstation, p_auth_tx->TLF01_details.workstation );

   /* Forwarding Inst ID Code */
   if ( Online_Updates.fwd_inst_code[0] == 0x00 )
      strcpy( Online_Updates.fwd_inst_code,
              p_auth_tx->TLF01_details.forwarding_institution_id );

   /* Date-Time */
   if ( Online_Updates.date_time[0] == 0x00 )
   {
      strcpy( Online_Updates.date_time, p_auth_tx->TLF01_details.date_yyyymmdd);
      strcat( Online_Updates.date_time, p_auth_tx->TLF01_details.time_hhmmss  );
   }

   /* Originator Information */
   if ( Online_Updates.orig_queue[0] == 0x00 )
   {
      strcpy(Online_Updates.orig_queue, p_auth_tx->TLF01_details.originator_queue);
      strcpy(Online_Updates.orig_info,  p_auth_tx->TLF01_details.originator_info );
   }

   return(true);
}

/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_GUI
 *
 *  DESCRIPTION:  This function builds and sends a message to the applink
 *                queue.  It sends the Online_Updates structure.  Typically
 *                this is a response from the Amex host to a 1304 msg.
 *
 *  INPUTS:       p_auth_tx - contains originator information
 *                (Global)Online_Updates
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_response_to_GUI( pAUTH_TX p_auth_tx )
{
   INT         ret_val;
   CHAR        err_msg[512]={0};
   BYTE        buffer1[sizeof(AUTH_TX)+sizeof(AMEX_ONLINE_UPDATES)];

   /* Build the message to be sent to applink */
   memset( buffer1, 0, sizeof(buffer1) );
   memcpy( buffer1+sizeof(AUTH_TX),
          &Online_Updates,
           sizeof(AMEX_ONLINE_UPDATES) );

	   ret_val = build_and_send_request_message( MT_INCOMING_GUI, 
                                      ST1_NONE,
                                      ST2_NONE,
                                      p_auth_tx->TLF01_details.originator_queue,
                                      (pBYTE)&buffer1,
                                      sizeof( buffer1 ),
                                      0,
                                      p_auth_tx->TLF01_details.originator_info,
                                      err_msg );
  // }

   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s",
    		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(err_msg,Log_Bufr);
      ncamex_log_message( 3, 2, err_msg , "send_response_to_GUI" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_val );
} /* send_response_to_GUI */

/******************************************************************************
 *
 *  NAME:         ISSUE_1304_RESPONSE
 *
 *  DESCRIPTION:  This function populates response fields and issues
 *                a response to the GUI.
 *
 *  INPUTS:       resp_code - Response code
 *                resp_text - Error message, if any
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_1304_response( pBYTE     resp_code,
                          pCHAR     err_msg,
                          pAUTH_TX  p_auth_tx )
{
   populate_1314_resp_struct( resp_code, err_msg, p_auth_tx );
   (void)send_response_to_GUI( p_auth_tx );
   (void)insert_db_record( TLF01_DATA, p_auth_tx );
   return;
}


/******************************************************************************
 *
 *  NAME:         INSERT_DB_RECORD
 *
 *  DESCRIPTION:  This function calls the data server to insert a record
 *                into a database table.
 *
 *  INPUTS:       data_type - Indicates which table to insert into
 *                p_auth_tx - Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT insert_db_record( BYTE data_type, pAUTH_TX p_auth_tx )
{
   INT    ret_val = true;
   INT    length;
   INT    continue_with_insert = true;
   CHAR   db_table[6]  = "";
   CHAR   err_buf[512] = {0};
   CHAR   Buffer[512] = {0};
   BYTE   db_buffer[sizeof(AUTH_TX) + sizeof(TLF01)] = "";

   if ( data_type == TLF01_DATA )
   {
      if ( p_auth_tx->host2_acquirer_cb_code[0] != '*' )
      {
         /* First set tran_finish_time. */
         ptetime_get_strtime( p_auth_tx->TLF01_details.tran_finish_time );
         p_auth_tx->host2_acquirer_cb_code[0] = '*';
			/*TF PHANI... Tempary fix for Amex 5CSC, since interst_rate filed has only 4 bytes, we need to fill null at end*/
		 memset(&p_auth_tx->TLF01_details.interest_rate[4],'\0',1);

         memcpy( db_buffer, p_auth_tx, sizeof(AUTH_TX) );
         memcpy( db_buffer+sizeof(AUTH_TX),
                 &(p_auth_tx->TLF01_details),
                 sizeof(TLF01) );

         length   = sizeof(AUTH_TX) + sizeof(TLF01);
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
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncamex_log_message( 3, 3, Buffer , "insert_db_record" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         CREATE_STAN
 *
 *  DESCRIPTION:  This function creates a unique 6-digit system trace audit
 *                number (stan).  It uses the last 6 digits of the time -
 *                number of seconds since 1-1-1970.  This is unique for a
 *                cycle of 11 days.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      stan - Six digit stan
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_stan( pBYTE stan )
{
   INT   len;
   BYTE  time_in_seconds[30]="";

   ptetime_get_strtime( time_in_seconds );
   len = strlen( time_in_seconds );
   if ( len < 10 )
      len = 10;

   /* Time is in format nnnnnnnnn.nnn
    * I want the last 6 n's before the decimal point.
    */
   memcpy( stan, &time_in_seconds[len-10], 6 );
   return;
}

/******************************************************************************
 *
 *  NAME:         validate_date_yymm
 *
 *  DESCRIPTION:  This function validates that a date in the format of YYMM
 *                has a valid year (numeric and greater than 0) and a valid
 *                month (numeric and ranging from 01 to 12).
 *
 *  INPUTS:       date - Date to be validated
 *
 *  OUTPUTS:      err_buf - Indicates what is wrong with the input
 *
 *  RTRN VALUE:   True if good date, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_yymm( pBYTE date, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  iYear;
   INT  iMonth;
   BYTE temp_year[3];
   BYTE temp_month[3];

   memset( temp_year,  0x00, sizeof(temp_year)  );
   memset( temp_month, 0x00, sizeof(temp_month) );

   memcpy( temp_year,  date,   2 );
   memcpy( temp_month, date+2, 2 );

   iYear  = atoi( temp_year  );
   iMonth = atoi( temp_month );

   if ( true != isnum(temp_year) )
      sprintf( err_buf, "Invalid year - not numeric: %s", temp_year );
   else if ( true != isnum(temp_month) )
      sprintf( err_buf, "Invalid month - not numeric: %s", temp_month );
   else if ( (iYear < 0) || (iYear > 60 ) )
      sprintf( err_buf, "Invalid year - not current: %s", temp_year );
   else if ( (iMonth < 1) || (iMonth > 12) )
      sprintf( err_buf, "Invalid month: %s", temp_month );
   else if ( 2 != strlen(temp_year) )
      sprintf( err_buf, "Invalid year - not 2 digits: %s", temp_year );
   else if ( 2 != strlen(temp_month) )
      sprintf( err_buf, "Invalid month - not 2 digits: %s", temp_month );
   else
      ret_val = true;

   return( ret_val );
}

