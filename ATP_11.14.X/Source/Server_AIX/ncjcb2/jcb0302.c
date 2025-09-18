/*******************************************************************************
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      jcb0302.c
 *  
 * TITLE:       JCB Online File Update Handler
 *  
 * DESCRIPTION: This module contains functions that handle Online File
 *              Update requests from the ATP GUI, and responses from
 *              the JCB host.
 *
 * APPLICATION: ATP in Equitable
 *
 * AUTHOR:      D. Irby

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
#include "tx_dbstruct.h"
#include "jcbmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "jcb0302.h"
#include "jcbutil.h"
#include "jcbtran.h"

/***********/
/* GLOBALS */
/***********/
extern NCF01               ncf01_i;
extern JCB_ONLINE_UPDATES  Online_Updates;
extern INT                 Txn_Type;

extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* Database structure sizes */
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;
extern CHAR jcb_que_name[]   ;   // ThoughtFocus PHANI

/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_REQUEST
 *
 *  DESCRIPTION:  This function begins the process of sending an 0302 message
 *                to JCB.  The 0302 transaction is built based on information
 *                sent from the GUI, information gathered from NCF01, and some
 *                data that is created here.  Once built, the 0302 is sent to
 *                JCB.
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
   CHAR            err_mesg[512] = {0};
   CHAR            time_str [ 7] = "";
   CHAR            time_date[25] = "";
   CHAR            err_msg  [41] = "";
   AUTH_TX         auth_tx;
   CHAR			   temp[17] = "";

   /* Copy transaction record into data structure. */
   memset( &Online_Updates, 0x00, sizeof(JCB_ONLINE_UPDATES) );
   memset( &auth_tx,        0x00, Auth_Tx_Size            );

   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);


   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy( &Online_Updates,
            p_data_data+(Auth_Tx_Size),
            sizeof(JCB_ONLINE_UPDATES) );

   /* Initialize the transaction. */
   (void)init_txn( &auth_tx );
   Txn_Type = ONLINE_UPDATE;

   /* Get originating information */
   strcpy( auth_tx.TLF01_details.originator_queue,
           ptemsg_get_msg_orig_queue( p_msg ));

   strcpy( auth_tx.TLF01_details.originator_info,
           ptemsg_get_msg_orig_comms_info( p_msg ));
 

   /* Store data into auth_tx for function Build_Host_Structure. */
  if( 0 == strncmp( auth_tx.TLF01_details.originator_queue,"ncjcbA",5));
   {
	   /* This is Response from JCB host for 0302 request from cadience ..TF PHANI*/
	   strcpy(auth_tx.TLF01_details.message_type,"0312");
	   
	 strcpy(auth_tx.TLF01_details.response_text, Online_Updates.resp_text );
  
   /* Response Code */
  
      strcpy(auth_tx.TLF01_details.response_code, Online_Updates.resp_code);
    
   /* Transaction Type */
	 auth_tx.TLF01_details.processing_code[0]=  Online_Updates.txn_type  ;

   /* Action Code */
   
      strcpy(auth_tx.TLF01_details.type_of_data,Online_Updates.action_code );

   /* Purge Date */
   
      strcpy(auth_tx.TLF01_details.product_codes[0].amount,Online_Updates.purge_date );

   /* Stop Regions */
      
      strcpy(auth_tx.TLF01_details.product_codes[1].amount,Online_Updates.stop_regions );
	
	  strcpy(auth_tx.TLF01_details.card_num,     Online_Updates.card_nbr );
	/* Terminal ID ..Now it STAN ..while sending we have used same filed..TF PHANI*/
	   memcpy(auth_tx.TLF01_details.sys_trace_audit_num, Online_Updates.terminal_id, 6);

	  auth_tx.TLF01_details.general_status = GS_TXN_COMPLETED;
	  auth_tx.TLF01_details.tx_key         = AUTH_CARD_UPDATE_RESPONSE;
	 

	    /* Workstation and Operator ID..For JCB2 DE32 and 33 */
	   strcpy(auth_tx.TLF01_details.acquiring_id, Online_Updates.workstation );
	   strcpy(auth_tx.TLF01_details.forwarding_institution_id, Online_Updates.operator_id );
	  
	   //DE 7
	   get_gmt_time(auth_tx.TLF01_details.transmission_timestamp );
	   genutil_format_date(temp);
	   memcpy(auth_tx.TLF01_details.date_yyyymmdd, temp, 8); 
	   memcpy(auth_tx.TLF01_details.time_hhmmss, temp + 8, 6);
	 
		
	  ret_val = verify_host_app_status( &auth_tx );
      if( true != ret_val )
      {
         /* Host is not available.  Send Message to Monitor. */
         sprintf(err_mesg, "JCB2 HOST is not avialbe to Send 0302 response to HOST");
         ncjcb2_log_message( 3, 2, err_mesg , "process_file_update_request" , 0 ,"");
      }
      else
      {
         /* Host is working.  Build PTEMSG's to be sent */
        
		  build_host_structure(&auth_tx);
		   if (true == Build_Host_Request(&auth_tx))
		   {
	
			  if(false == hhutil_send_host_response(&auth_tx))
			  {
			      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			      sprintf(Log_Bufr," Error sending response for the host Term Id:%s ,Merch Id: %s RRN: %s ",
			    		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				ncjcb2_log_message( 3, 3, Log_Bufr , "process_file_update_request" , 0 ,auth_tx.TLF01_details.card_num);
				ret_val = false;
			  }
		   }
		   else
		   {
			  strcpy( auth_tx.TLF01_details.response_text,
					 "Error building response for the host" );
		      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		      sprintf(Log_Bufr," Error building response for the host Term Id:%s ,Merch Id: %s RRN: %s ",
		    		  auth_tx.TLF01_details.terminal_id,
					  auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
			  ncjcb2_log_message( 3, 3, Log_Bufr , "process_file_update_request" , 0 ,auth_tx.TLF01_details.card_num);
			  ret_val = false;
      
		   }
      }
		  
		return (ret_val);

   }
	
    (void)map_0302_to_tlf01( &auth_tx );

   /* Make sure GUI sent valid information. */
   ret_val = validate_gui_inputs( err_msg );
   if ( ret_val == false )
   {
      issue_0302_response( FORMAT_ERROR, &auth_tx, err_msg );
   }
   else
   {
      /* Confirm host is available */
      ret_val = verify_host_app_status( &auth_tx );
      if( true != ret_val )
      {
         /* Host is not available.  Send response to GUI. */
         issue_0302_response( HOST_UNAVAILABLE, &auth_tx, err_msg );
      }
      else
      {
         /* Host is working.  Build PTEMSG's to be sent */
         ret_val = send_request_to_host( &auth_tx );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         MAP_0302_TO_TLF01
 *
 *  DESCRIPTION:  This function copies information stored in the 0302 message
 *                structure into the TLF01 structure for logging purposes.
 *
 *  INPUTS:       (Global) Online_Updates
 *
 *  OUTPUTS:      p_auth_Tx
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void map_0302_to_tlf01( pAUTH_TX p_auth_tx )
{
   CHAR  temp[17] = "";

   /* Create GMT - Transmission Timestamp in Universal time. */
   get_gmt_time( p_auth_tx->TLF01_details.transmission_timestamp );

   /* Create System Trace Audit Number. */
   get_stan( p_auth_tx->TLF01_details.sys_trace_audit_num );

   /* Terminal ID */
   memcpy(p_auth_tx->TLF01_details.terminal_id, Online_Updates.terminal_id, 8);

   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.tx_key         = AUTH_CARD_UPDATE;
   strcpy( p_auth_tx->TLF01_details.message_type, "0302" );

   /* Info from GUI Fields */
   strcpy( p_auth_tx->TLF01_details.card_num,     Online_Updates.card_nbr );
   p_auth_tx->TLF01_details.processing_code[0] =  Online_Updates.txn_type;
   strcpy( p_auth_tx->TLF01_details.type_of_data, Online_Updates.action_code );
   strcpy( p_auth_tx->TLF01_details.product_codes[0].amount,
           Online_Updates.purge_date );

   /* Stop Regions */
   if ( Online_Updates.stop_regions[0] != 0x00 )
   {
      strcpy( p_auth_tx->TLF01_details.product_codes[1].amount,
              Online_Updates.stop_regions );
   }

   /* Workstation and Operator ID */
   strcpy( p_auth_tx->TLF01_details.workstation, Online_Updates.workstation );
   strcpy( p_auth_tx->TLF01_details.operator_id, Online_Updates.operator_id );

   genutil_format_date(temp);
   memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8); 
   memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);

   return;
}


/******************************************************************************
 *
 *  NAME:         VALIDATE_GUI_INPUTS
 *
 *  DESCRIPTION:  This function validates the data received from the GUI as
 *                follows:
 *                   Transaction Type: Must exist
 *                                     Must be numeric
 *                                     Must be 0, 1, 2, or 5
 *
 *                   Card Number     : Must exist
 *                                     Must be numeric
 *
 *                   Action Code     : Must exist if txn type is 1 or 2
 *                                     Can exist for other txn types
 *                                     Must be numeric
 *                                     Must be 01, 04, 05, 07, 41, or 43
 *
 *                   Purge Date      : Must exist if txn type is 1 or 2
 *                                     Can exist for other txn types
 *                                     Must be numeric
 *                                     Must be format YYYYMM, where MM = 01-12
 *
 *                   Stop Regions    : Must be NULL or five 0's and/or 1's
 *                                     Can exist for any txn type
 *                                     Can be a mix of 0's and 1's
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
   INT   i;
   CHAR  temp[3] = "";

   /* Transaction Type */
   if ( Online_Updates.txn_type == 0x00 )
   {
      strcpy( err_msg, "Missing Request Type. Required field." );
   }
   else if ( false == isdigit(Online_Updates.txn_type) )
   {
      sprintf( err_msg,
              "Invalid Request Type %c. Must be 0,1,2,5",
               Online_Updates.txn_type );
   }
   else if ( (Online_Updates.txn_type != '0') &&
             (Online_Updates.txn_type != '1') &&
             (Online_Updates.txn_type != '2') &&
             (Online_Updates.txn_type != '5')  )
   {
      sprintf( err_msg,
              "Invalid Request Type %c. Must be 0,1,2,5",
               Online_Updates.txn_type );
   }

   /* Card Number */
   else if ( 16 != strlen(Online_Updates.card_nbr) )
   {
      sprintf( err_msg,
              "Invalid card number %s",
               Online_Updates.card_nbr );
   }
   else if ( false == isnum(Online_Updates.card_nbr) )
   {
      sprintf( err_msg,
              "Non-numeric card number %s",
               Online_Updates.card_nbr );
   }

   /* Action Code */
   else if ( Online_Updates.action_code[0] == 0x00 )
   {
      if ((Online_Updates.txn_type == '1') || (Online_Updates.txn_type == '2'))
      {
         if ( Online_Updates.txn_type == '1' )
            strcpy( err_msg, "Missing Action Code for ADD" );
         else
            strcpy( err_msg, "Missing Action Code for REVISE" );
      }
      else
         ret_val = true;
   }
   else if ( false == isnum(Online_Updates.action_code) )
   {
      sprintf( err_msg,
              "Non-numeric action code %s",
               Online_Updates.action_code );
   }
   else
   {
      i = atoi(Online_Updates.action_code);
      if ( (i != 1) && (i != 4)  && (i != 5) &&
           (i != 7) && (i != 41) && (i != 43) )
      {
         sprintf( err_msg,
                 "Invalid Action Code %s. <1,4,5,7,41,43>",
                  Online_Updates.action_code );
      }

      /* Purge Date */
      else if ( Online_Updates.purge_date[0] == 0x00 )
      {
         if ( Online_Updates.txn_type == '1' || Online_Updates.txn_type == '2' )
         {
            if ( Online_Updates.txn_type == '1' )
               strcpy( err_msg, "Missing Purge Date for ADD" );
            else
               strcpy( err_msg, "Missing Purge Date for REVISE" );
         }
         else
            ret_val = true;
      }
      else if ( false == isnum(Online_Updates.purge_date) )
      {
         sprintf( err_msg,
                 "Non-numeric purge date %s",
                  Online_Updates.purge_date );
      }
      else
      {
         memcpy( temp, &Online_Updates.purge_date[4], 2 );
         i = atoi( temp );
         if ( (i < 1) || (i > 12) )
         {
            sprintf( err_msg,
                    "Invalid month, %d, in purge date %s",
                     i, Online_Updates.purge_date );
         }

         /* Stop Regions */
         else
         {
            ret_val = true;
            if ( Online_Updates.stop_regions[0] != 0x00 )
            {
               for ( i=0; i<5; i++ )
               {
                  if ( (Online_Updates.stop_regions[i] != '0') &&
                       (Online_Updates.stop_regions[i] != '1')  )
                  {
                     ret_val = false;
                     sprintf( err_msg,
                             "Invalid Stop Regions %s",
                              Online_Updates.stop_regions );
                     break;
                  }
               }
            }
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_FILE_UPDATE_RESPONSE
 *
 *  DESCRIPTION:  This function handles 0312 messages.  These are the responses
 *                to 0302 messages - File Update Requests where card holder
 *                records can be added, deleted, updated, or inquired.
 *
 *  INPUTS:       p_auth_tx - Structure containing most of the transacation data
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
   memset( &Online_Updates, 0x00, sizeof(JCB_ONLINE_UPDATES) );

   /* Populates the GUI structure.
    * Sends it to the GUI.
    * Logs to TLF01.
    */
   issue_0302_response( p_auth_tx->TLF01_details.response_code,
                        p_auth_tx, err_msg );
   return;
}


/******************************************************************************
 *
 *  NAME:         POPULATE_0312_RESP_STRUCT
 *
 *  DESCRIPTION:  This function copies information stored in the 0312 message
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
 *                p_auth_tx - Transaction information
 *                err_msg   - Error message, if any
 *
 *  OUTPUTS:      (Global)Online_Updates - Information to display on GUI
 *
 *  RTRN VALUE:   Always returns true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT populate_0312_resp_struct( pBYTE     resp_code,
                               pAUTH_TX  p_auth_tx,
                               pCHAR     err_msg )
{
   INT   i=0;
   BYTE  buffer[40]="";

   BYTE  resp_text[][2][40] =
         { {"00", "FILE REQUEST SUCCESSFUL"              },
           {"12", "Duplicate message within 24 hours."   },
           {"25", "Unable to locate record on file."     },
           {"27", "File update field edit error."        },
           {"28", "File is temporarily unavailable."     },
           {"29", "File update was not successful."      },
           {"30", "Format Error in 0302 request message."},
           {"63", "Security Violation."                  },
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

   /* Card Number */
   if ( Online_Updates.card_nbr[0] == 0x00 )
      strcpy( Online_Updates.card_nbr, p_auth_tx->TLF01_details.card_num );

   /* Transaction Type */
   if ( Online_Updates.txn_type == 0x00 )
      Online_Updates.txn_type = p_auth_tx->TLF01_details.processing_code[0];

   /* Action Code */
   if ( Online_Updates.action_code[0] == 0x00 )
      strcpy( Online_Updates.action_code, p_auth_tx->TLF01_details.type_of_data );

   /* Purge Date */
   if ( Online_Updates.purge_date[0] == 0x00 )
      strcpy( Online_Updates.purge_date,
              p_auth_tx->TLF01_details.product_codes[0].amount );

   /* Stop Regions */
   if ( Online_Updates.stop_regions[0] == 0x00 )
   {
      strcpy( Online_Updates.stop_regions,
              p_auth_tx->TLF01_details.product_codes[1].amount );
   }

   /* Terminal ID */
   if ( Online_Updates.terminal_id[0] == 0x00 )
      strcpy( Online_Updates.terminal_id, p_auth_tx->TLF01_details.terminal_id );

   return(true);
}

/******************************************************************************
 *
 *  NAME:         SEND_RESPONSE_TO_GUI
 *
 *  DESCRIPTION:  This function builds and sends a message to the applink
 *                queue.  It sends the Online_Updates structure.  Typically
 *                this is a response from the JCB host to an 0302 msg.
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
   BYTE        buffer1[sizeof(AUTH_TX)+sizeof(JCB_ONLINE_UPDATES)];

   /* Build the message to be sent to applink */
   memset( buffer1, 0, sizeof(buffer1) );
   memcpy( buffer1+Auth_Tx_Size,&Online_Updates,sizeof(JCB_ONLINE_UPDATES) );
   ret_val = build_and_send_request_message( MT_INCOMING_GUI, 
                                      ST1_NONE,
                                      ST2_NONE,
                                      p_auth_tx->TLF01_details.originator_queue,
                                      (pBYTE)&buffer1,
                                      sizeof( buffer1 ),
                                      0,
                                      p_auth_tx->TLF01_details.originator_info,
                                      err_msg );

   if ( false == ret_val )
   {
      ncjcb2_log_message( 3, 3, err_msg , "send_response_to_GUI" , 0 ,"");
   }
   return( ret_val );
} /* send_response_to_GUI */

/******************************************************************************
 *
 *  NAME:         ISSUE_0302_RESPONSE
 *
 *  DESCRIPTION:  This function populates response fields and issues
 *                a response to the GUI.
 *
 *  INPUTS:       resp_code - Response code
 *                p_auth_tx - Transaction information
 *                resp_text - Error message, if any
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_0302_response( pBYTE     resp_code,
                          pAUTH_TX  p_auth_tx,
                          pCHAR     err_msg )
{
   populate_0312_resp_struct( resp_code, p_auth_tx, err_msg );
   (void)send_response_to_GUI( p_auth_tx );
   (void)insert_tlf01( p_auth_tx );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_0302_FROM_HOST
 *
 *  DESCRIPTION:  This procedure prepare the buffer to send the file update 
 *                (0302) request to JCB Host.
 *
 *  INPUTS:       host_auth_tx - Transaction request record from host
 *
 *  OUTPUTS:      Auth_Tx - (Global) Txn record is updated with request info
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       TF PHANI
 *
 ******************************************************************************/
void process_0302_from_host(pAUTH_TX p_auth_tx)
{

	BYTE     buffer[sizeof(AUTH_TX) + sizeof(JCB_ONLINE_UPDATES)];
	//JCB_ONLINE_UPDATES FileUpdate;
	INT         ret_val;
    CHAR        err_msg[100];

	memset( buffer, 0, sizeof(buffer) );
	memset( &Online_Updates, 0, sizeof(JCB_ONLINE_UPDATES));
	/* If transaction is coming from Cadience, we have recived STAN, we need to store nad pass as it is to NCJCB
	   No teriminal ID for Cadience 0302 request.
	   Terimnal ID is filled with STAN,we need to take back same in JCB. 
	 */
	memcpy( Online_Updates.terminal_id,p_auth_tx->TLF01_details.sys_trace_audit_num, 6);

	strcpy( Online_Updates.card_nbr,p_auth_tx->TLF01_details.card_num );

	Online_Updates.txn_type = p_auth_tx->TLF01_details.processing_code[0] ;

	strcpy( Online_Updates.action_code,p_auth_tx->TLF01_details.type_of_data );
    
	strcpy(Online_Updates.purge_date, p_auth_tx->TLF01_details.product_codes[0].amount);

	/* Stop Regions */
   
    strcpy( Online_Updates.stop_regions,p_auth_tx->TLF01_details.product_codes[1].amount );
   	/* DE 32 and 33 use these fileds... TF PHANI*/
    strcpy(  Online_Updates.workstation ,p_auth_tx->acq_inst_id_cd );
    strcpy(  Online_Updates.operator_id , p_auth_tx->forwarding_inst_id_cd);


    memcpy( buffer,p_auth_tx, sizeof(AUTH_TX) );
	memcpy( buffer + sizeof(AUTH_TX), &Online_Updates,sizeof(JCB_ONLINE_UPDATES));

	ret_val = build_and_send_request_message( MT_INCOMING_GUI, 
                                      ST1_NONE,
                                      ST2_NONE,
                                      jcb_que_name,  //ncjcbA
                                      (pBYTE)&buffer,
                                      sizeof( buffer ),
                                      0,
                                      NULL_STR,
                                      err_msg );


	if ( false == ret_val )
    {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "process_0302_from_host" , 0 ,p_auth_tx->TLF01_details.card_num);
    }
	

}/* process_1304_from_host */
	

