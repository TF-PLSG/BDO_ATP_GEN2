/*******************************************************************************
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_1304.c
 *  
 * TITLE:      ncdci  Online File Update Handler
 *  
 * DESCRIPTION: This module contains functions that handle Online File
 *              Update requests from the ATP GUI, and responses from
 *              the ncdci host.
 *
 * APPLICATION: ATP in Equitable
 *
 * AUTHOR:      Abhishek Verma
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
#include "ncdci_bitfields.h"
#include "ncdci_prototypes.h"
/***********/
/* GLOBALS */
/***********/
extern AMEX_ONLINE_UPDATES  Online_Updates;
extern AUTH_TX              Auth_Tx;
extern NCF01                Ncf01_I;


/******************************************************************************
 *
 *  NAME:         ncdci_process_file_update_request
 *
 *  DESCRIPTION:  This function begins the process of sending an 1304 message
 *                to ncdci.  The 1304 transaction is built based on information
 *                sent from the GUI, information gathered from NCF01, and some
 *                data that is created here.  Once built, the 1304 is sent to
 *                ncdci.
 *
 *  INPUTS:       p_msg - This is the incoming message from the GUI.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_process_file_update_request( pPTE_MSG p_msg_in )
{
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data = NULL_PTR;
	CHAR			   err_buf[100] = {0};
	CHAR			   Buffer[200] = {0};
	LONG            data_len = 0;
	LONG            auth_len = 0;
	INT             Auth_Tx_Size_local = 0;
	INT             ret_val;
	CHAR            err_msg[200] = {0};

	/* Copy transaction record into auth_tx structure. */
	Auth_Tx_Size_local = sizeof(Auth_Tx);
	memset( &Auth_Tx, 0x00, Auth_Tx_Size_local);
	p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
	auth_len   = data_len;
	if ( Auth_Tx_Size_local < data_len )
	auth_len = Auth_Tx_Size_local;
	p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
	memcpy( &Auth_Tx, p_data, auth_len );
	ret_val = ncdci_verify_host_status( err_msg );
	if((0 == strncmp(Auth_Tx.TLF01_details.message_type,
					 DINERS_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE,
					 DINERS_MESSAGE_TYPE_SIZE))||
		(0 == strncmp(Auth_Tx.TLF01_details.message_type,
					  DINERS_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE,
					  DINERS_MESSAGE_TYPE_SIZE)))
	{
		ncdci_sendfile_response_to_host( DINERS_SEND_IF_NOT_REVERSAL );
	}
	else
	{
		if ( false == ret_val )
		{
			/* Host is not available.  Send response to GUI. */
			strcpy( Auth_Tx.accptor_bus_code, DINERS_HOST_UNAVAILABLE );
			ncdci_issue_1304_response( DINERS_HOST_UNAVAILABLE_ISO, err_msg );
		}
		else
		{
			/* Host is working.  Build PTEMSG's to be sent */
			(void)ncdci_send_file_request_to_host( DINERS_TIMER_FINANCIAL );
		}
	}
	return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_map_1304_to_tlf01
 *
 *  DESCRIPTION:  This function copies information stored in the 1304 message
 *                structure into the TLF01 structure for logging purposes.
 *
 *  INPUTS:       (Global) Online_Updates
 *
 *  OUTPUTS:      (Global) Auth_Tx
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_map_1304_to_tlf01()
{
   /* Terminal ID */
   memcpy( Auth_Tx.TLF01_details.terminal_id, Online_Updates.terminal_id, 8 );

   /* Info from GUI Fields */
   strcpy( Auth_Tx.TLF01_details.card_num,    Online_Updates.card_nbr );
   strcpy( Auth_Tx.TLF01_details.exp_date,    Online_Updates.exp_date );
   Auth_Tx.TLF01_details.processing_code[0] = Online_Updates.function_code[2];
   strcpy( Auth_Tx.TLF01_details.nii,         Online_Updates.reason_code );

   /* Workstation and Operator ID */
   strcpy( Auth_Tx.TLF01_details.workstation, Online_Updates.workstation );
   strcpy( Auth_Tx.TLF01_details.operator_id, Online_Updates.operator_id );
   /* For online updates we have stored ACTION code in bus code.. Get it back */
   strcpy( Auth_Tx.accptor_bus_code, Online_Updates.resp_code );

   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_validate_gui_inputs
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
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_validate_gui_inputs( pCHAR err_msg )
{
   INT   ret_val = false;
   INT   i = 0;
   INT   in_reason_code = 0;
   CHAR  buffer[200] = {0};

   #define NUM_REASON_CODES  7
   INT   reason_codes[NUM_REASON_CODES] = {3000, 3001, 3002,
                                           3700, 3701, 3702, 3703};

   /* Card Number */
   if ( Online_Updates.card_nbr[0] == 0x00 )
   {
      strcpy( err_msg, "Card number is missing" );
   }
   else if ( false == ncdci_isnum(Online_Updates.card_nbr) )
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
   else if (  false == ncdci_validate_date_yymm(Online_Updates.exp_date, buffer) )
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

   /* Message Reason Code */
   else if ( Online_Updates.reason_code[0] == 0x00 )
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
   }

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci_process_file_update_response
 *
 *  DESCRIPTION:  This function handles 0314 messages.  These are the responses
 *                to 1304 messages - File Update Requests where card holder
 *                records can be added, deleted, updated, or inquired.
 *
 *  INPUTS:       (Global) Auth_Tx - Transacation data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_process_file_update_response()
{
   CHAR  err_msg[41] = {0};

   /* Initialize the GUI structure. */
   memset( &Online_Updates, 0x00, ONLINE_UPDATES_SIZE );

   /* Populates the GUI structure.
    * Sends it to the GUI.
    * Logs to TLF01.
    */
   ncdci_issue_1304_response( Auth_Tx.TLF01_details.response_code, err_msg );
   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_populate_1314_resp_struct
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
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_populate_1314_resp_struct( pBYTE     resp_code,
                               pCHAR     err_msg )
{
   INT   i = 0;
   BYTE  buffer[40] = {0};

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

   strcpy( Auth_Tx.TLF01_details.response_text, buffer );
   if ( Online_Updates.resp_text[0] == 0x00 )
      strcpy( Online_Updates.resp_text, buffer );

   /* Response Code */
   if ( Auth_Tx.TLF01_details.response_code[0] == 0x00 )
      strcpy( Auth_Tx.TLF01_details.response_code, resp_code );

   if ( Online_Updates.resp_code[0] == 0x00 )
      strcpy( Online_Updates.resp_code, resp_code );

   /* ncdci Response Code - so it gets logged into TLF01. */
   strcpy( Auth_Tx.TLF01_details.auth_number, Auth_Tx.accptor_bus_code );

   /* Card Number */
   if ( Online_Updates.card_nbr[0] == 0x00 )
      strcpy( Online_Updates.card_nbr, Auth_Tx.TLF01_details.card_num );

   /* Expiration Date */
   if ( Online_Updates.exp_date[0] == 0x00 )
      strcpy( Online_Updates.exp_date, Auth_Tx.TLF01_details.exp_date );

   /* Request Type */
   if ( Online_Updates.function_code[0] == 0x00 )
   {
      strcpy( Online_Updates.function_code, "30" );
      Online_Updates.function_code[2] = Auth_Tx.TLF01_details.processing_code[0];
   }

   /* Action Code */
   if ( Online_Updates.reason_code[0] == 0x00 )
      strcpy( Online_Updates.reason_code, Auth_Tx.TLF01_details.nii );

   /* Terminal ID */
   if ( Online_Updates.terminal_id[0] == 0x00 )
      strcpy( Online_Updates.terminal_id, Auth_Tx.TLF01_details.terminal_id );

   /* STAN */
   if ( Online_Updates.stan[0] == 0x00 )
      strcpy( Online_Updates.stan, Auth_Tx.TLF01_details.sys_trace_audit_num );

   /* Operator ID */
   if ( Online_Updates.operator_id[0] == 0x00 )
      strcpy( Online_Updates.operator_id, Auth_Tx.TLF01_details.operator_id );

   /* Workstation */
   if ( Online_Updates.workstation[0] == 0x00 )
      strcpy( Online_Updates.workstation, Auth_Tx.TLF01_details.workstation );

   /* Forwarding Inst ID Code */
   if ( Online_Updates.fwd_inst_code[0] == 0x00 )
      strcpy( Online_Updates.fwd_inst_code,
              Auth_Tx.TLF01_details.forwarding_institution_id );

   /* Date-Time */
   if ( Online_Updates.date_time[0] == 0x00 )
   {
      strcpy( Online_Updates.date_time, Auth_Tx.TLF01_details.date_yyyymmdd );
      strcat( Online_Updates.date_time, Auth_Tx.TLF01_details.time_hhmmss   );
   }

   /* Originator Information */
   if ( Online_Updates.orig_queue[0] == 0x00 )
   {
      strcpy(Online_Updates.orig_queue, Auth_Tx.TLF01_details.originator_queue);
      strcpy(Online_Updates.orig_info,  Auth_Tx.TLF01_details.originator_info );
   }

   return(true);
}

/******************************************************************************
 *
 *  NAME:         ncdci_issue_1304_response
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
 *  AUTHOR:      Abhishek Verma
 *
 ******************************************************************************/
void ncdci_issue_1304_response( pBYTE     resp_code,
                          pCHAR     err_msg )
{
	ncdci_populate_1314_resp_struct( resp_code, err_msg );
   (void)ncdci_send_response_to_GUI();
}


/******************************************************************************
 *
 *  NAME:         ncdci_process_file_update_request_1324
 *
 *  DESCRIPTION:  This function begins the process of sending an 1304 message
 *                to Diners.  The 1304 transaction is built based on information
 *                sent from the GUI, information gathered from NCF01, and some
 *                data that is created here.  Once built, the 1304 is sent to
 *                Diners.
 *
 *  INPUTS:       p_msg - This is the incoming message from the GUI.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_process_file_update_request_1324( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   CHAR            err_mesg[200] = {0};
   CHAR            time_str [ 7] = {0};
   CHAR            time_date[25] = {0};
   CHAR            err_msg [100] = {0};
   CHAR            temp     [17] = {0};
   INT             ret_val;
   LONG            data_len;

   /* Copy transaction record into data structure. */
   memset( &Online_Updates, 0x00, ONLINE_UPDATES_SIZE );

   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);

   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Online_Updates,
            p_data_data+AUTH_TX_SIZE,
            ONLINE_UPDATES_SIZE );

   /* Initialize the transaction. */
   (void)ncdci_init_Auth_Tx();
   strcpy( Auth_Tx.TLF01_details.dispensation.origin, "LO" );

   strcpy( Auth_Tx.forwarding_inst_id_cd, Ncf01_I.forwarding_inst_id_code );

   strcpy( Auth_Tx.TLF01_details.forwarding_institution_id,
           Ncf01_I.forwarding_inst_id_code );

   /* Send request to Cadencie Host for 1324 request */
   strncpy( Auth_Tx.TLF01_details.originator_queue,
		    Auth_Tx.TLF01_details.handler_queue,
			strlen(Auth_Tx.TLF01_details.handler_queue));

   Auth_Tx.TLF01_details.general_status = GS_TXN_NEW;
   Auth_Tx.TLF01_details.tx_key         = AUTH_CARD_UPDATE;

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   (void)ncdci_send_file_request_to_host_1324( DINERS_TIMER_FINANCIAL );

   return( ret_val );
}
