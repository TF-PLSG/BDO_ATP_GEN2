/***************************************************************************
 *
 * Copyright (c) 2003, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nciftr_0800.C
 *  
 * TITLE:       CUP Network Controller Network Management Processor
 *  
 * DESCRIPTION: This module contains functions that handle network
 *              management requests and responses, and
 *              switched out.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Abhishek Verma
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
#include "memmnger.h"
#include "nc_dbstruct.h"
#include "nciftr_constants.h"
#include "nciftr_prototypes.h"
#include "nciftr_bitfields.h"
#include "txutils.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];
extern CHAR    NMI_Code[];
extern INT     buflen;
extern BYTE    ReqRsp_Buffer[];
extern INT     TranFlow;

/* QUEUES */
extern CHAR    dialog_que_name[];
extern CHAR    applnk_que_name[];

/* Database structures */
extern AUTH_TX    Auth_Tx;
extern NCF01      Ncf01_I;

/* STRUCTURE SIZES */
extern INT  Auth_Tx_Size;
extern INT  Ncf01_Size;
extern INT  Ncf30_Size;
extern INT  Tlf01_Size;
extern INT  Crf01_Size;

/* SHARED MEMORY */
extern BYTE    current_state[2];
extern BYTE    net_consec_tmouts_ctr[4];
extern BYTE    active_txns_ctr[4];

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Echo Timer Info */
extern INT    Echo_Timer_Flag;
extern INT    Echo_Timer_Count;

extern CHAR    TimerAppData2[];
extern CHAR  authorizer_que_name[];
extern CHAR   trmp_stand[7];

/******************************************************************************
 *
 *  NAME:         PROCESS_MONITOR_REQUEST
 *
 *  DESCRIPTION:  This function is called when a network management request
 *                is made on the ATP Monitor: Log ON/OFF/Echo, etc.  It will
 *                call the function that issues the request to the host,
 *                passing it the appropriate network management type.
 *
 *  INPUTS:       p_msg - PTE message from Monitor
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_monitor_request( pPTE_MSG p_msg )
{
   INT   nmi_type;
   BYTE  nm_type;
   CHAR  err_buf[100];
   INT   send_request = true;

   nm_type = ptemsg_get_msg_subtype1( p_msg );
   if ( nm_type == ST1_LOGON )
      nmi_type = LOGON;
   else if ( nm_type == ST1_LOGOFF )
      nmi_type = LOGOFF;
   else if ( nm_type == ST1_ECHOTEST )
      nmi_type = ECHO;
   else if ( nm_type = ST1_NETWORK_QUERY )
   {
      process_monitor_query( p_msg );
      send_request = false;
   }
   else
   {
      send_request = false;
      memset( err_buf, 0x00, sizeof(err_buf) );
      sprintf( err_buf,
              "Unsupported request from Monitor. Subtype1 = %c",
               nm_type );
      nciftr_log_message( 1, 2, err_buf, "process_monitor_request",0 );
   }

   if ( send_request == true )
   {
      (void)issue_Dummy_200( nmi_type );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         issue_Dummy_200
 *
 *  DESCRIPTION:  This function sends a network management request to the host.
 *                It does whatever it needs to for each particular type of
 *                network management request.  This includes displaying an
 *                informational message on Monitor.
 *
 *  INPUTS:       nm_type - Type of 08X0 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_Dummy_200( INT nm_type )
{
   INT   ret_val = true;
   INT   seconds;
   CHAR  echo_req_timer_key[LOOKUP_KEY_SIZE]="";

   CHAR  err_buf[100]="";
   CHAR  buffer[100]  = "";

   /* Read shared memory to get current state of host. */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   Auth_Tx.TLF01_details.tx_key = AUTH_LOGON;
   strcpy( Auth_Tx.TLF01_details.message_type, "0100" );// Client requested for 0100 message type for dumy logon 
   TranFlow = SWITCHED_OUT;

   if ( nm_type == ECHO )
   {
      /* Before sending Echo request, make sure  a bunch of them have not timed out  */
      Echo_Timer_Count++;

      /* Don't let this value get too high. */
      if ( Echo_Timer_Count > 999 )
         Echo_Timer_Count = 1;

      /* Set the NMI code in field 70 for building the transaction. */
      strcpy( NMI_Code, NMI_ECHO );

      /* Set Timer for Echoes. */
      seconds = atoi(Ncf01_I.request_timer);
      strcpy( TimerAppData2, TIMER_08X0 );
      (void)create_echo_req_timer_key( echo_req_timer_key );
      (void)set_timer( echo_req_timer_key, seconds, REQUEST_TIMER );
   }
   else if ( nm_type == LOGON )
   {
      if ( 0 != strcmp( ONLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG ON request to host." );
         nciftr_log_message( 1, 1, err_buf, "issue_Dummy_200",0 );
         strcpy( NMI_Code, NMI_LOGON );
      }
      else
      {
         /* Already online.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already online.  Log ON request not sent." );
         nciftr_log_message( 1, 1, err_buf, "issue_Dummy_200",0 );
         ret_val = false;
      }
   }
   else if ( nm_type == LOGOFF )
   {
      if ( 0 != strcmp( OFFLINE, current_state ) )
      {
         strcpy( err_buf, "Sending LOG OFF request to host." );
         nciftr_log_message( 1, 1, err_buf, "issue_Dummy_200",0 );
		 ret_val = false;
         //strcpy( NMI_Code, NMI_LOGOFF );
		 /*
			Once we received a log off request from User throgh monitor, we will set state as OFFLINE
		 */
		 if ( false == set_host_state( OFFLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to offline. " );
            strcat( buffer, err_buf );
            nciftr_log_message( 1, 1, buffer, "issue_Dummy_200",0 );
         }
		 else
		 {
			strcpy( buffer, "nciftr Module is now OFFLINE. " );
            strcat( buffer, err_buf );
            nciftr_log_message( 1, 1, buffer, "issue_Dummy_200",0 );
		 }
      }
      else
      {
         /* Already offline.  Display this fact, then do not send request. */
         strcpy( err_buf, "Already offline.  Log OFF request not sent." );
         nciftr_log_message( 1, 1, err_buf, "issue_Dummy_200",0 );
         ret_val = false;
      }
   }
   else
   {
      /* Received a network management type that is not supported here. */
      sprintf( err_buf,
              "Received request for unsupported 08X0 request. Type = %d",
               nm_type );
      nciftr_log_message( 1, 1, err_buf, "issue_Dummy_200",0 );
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Issue the 08X0 request. */

      /* Step 1.  Build the data fields. */
      if ( false == build_08X0_msg(err_buf) )
         nciftr_log_message( 1, 2, err_buf, "issue_Dummy_200" ,0);

      /* Step 2.  Put host message together - build bitmap. */
      else if ( false == build_host_dummy_msg(err_buf) )
         nciftr_log_message( 1, 2, err_buf, "issue_Dummy_200",0 );

      /* Step 3.  Send request to host. */
      else if ( false == send_18nn_to_host(err_buf) )
         nciftr_log_message( 1, 2, err_buf, "issue_Dummy_200",0 );
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         issue_rsp_08X0
 *
 *  DESCRIPTION:  This function sends a network management response to the
 *                host. It displays a message on Monitor (except for echoes),
 *                then sends the response out.
 *
 *  INPUTS:       nm_type - Type of 1814 request
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT issue_rsp_08X0( INT nm_type )
{
   INT   ret_val = false;
   CHAR  err_buf[100]="";

   if ( nm_type == LOGON )
   {
      nciftr_log_message( 1, 1, "Sending Log ON response to host.", "issue_rsp_08X0",0 );
   }
   else if ( nm_type == LOGOFF )
   {
      nciftr_log_message( 1, 1, "Sending Log OFF response to host.", "issue_rsp_08X0" ,0);
   }

   /* Issue the 1814 response. */
   set_tx_key_to_response();

   /* Step 1.  Build the data fields. */
   if ( false == build_08X0_msg_response(err_buf) )
      nciftr_log_message( 1, 2, err_buf, "issue_rsp_08X0" ,0);

   /* Step 2.  Put host message together - build bitmap. */
   else if ( false == build_host_msg(err_buf) )
      nciftr_log_message( 1, 2, err_buf, "issue_rsp_08X0",0 );

   /* Step 3.  Send request to host. */
   else if ( false == send_18nn_to_host(err_buf) )
      nciftr_log_message( 1, 2, err_buf, "issue_rsp_08X0",0 );

   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         process_08X0_request
 *
 *  DESCRIPTION:  This function handles an 08X0 request from the host.
 *                It will update shared memory to reflect a change in host
 *                state; it will send an 1814 response to the host.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction info containing NMI Code
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_08X0_request()
{
   INT     ret_val = true;
   INT     type;
   CHAR    buffer[200];
   CHAR    err_buf[200]  = "";
   CHAR    new_state[3]  = "99";
   CHAR    same_state[3] = "99";

   /* Read shared memory to get current state of host. */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );

   if ( 0 == strcmp(NMI_LOGON, NMI_Code) )
   {
      type = LOGON;
      nciftr_log_message( 1, 1, "Received Log ON request from host.",
                  "process_08X0_request" ,0);
      if ( 0 == strcmp(ONLINE, current_state) )
         nciftr_log_message( 1, 1, "Already online.", "process_08X0_request",0 );
      else
         strcpy( new_state, ONLINE );
   }
   else if ( 0 == strcmp(NMI_LOGOFF, NMI_Code) )
   {
      type = LOGOFF;
      nciftr_log_message( 1, 1, "Received Log OFF request from host.",
                  "process_08X0_request",0 );
      if ( 0 == strcmp(OFFLINE, current_state) )
         nciftr_log_message( 1, 1, "Not online.", "process_08X0_request",0 );
      else
         strcpy( new_state, OFFLINE );
   }
   else if ( 0 == strcmp(NMI_ECHO, NMI_Code) )
   {
      /* Only do echoes if online. */
      type = ECHO;
      if ( 0 != strcmp(ONLINE, current_state) )
         ret_val = false;
   }
   else
   {
      /* If NMI code is anything else, we do not support it. */
      ret_val = false;
      if ( 0 == strcmp(NMI_KEYCHANGE, NMI_Code) )
         nciftr_log_message( 1, 1, "Key Change request from host not supported.",
                     "process_08X0_request",0 );

      else if ( 0 == strcmp(NMI_CUTOFF, NMI_Code) )
         nciftr_log_message( 1, 1, "Cutoff (08X0) request from host not supported.",
                     "process_08X0_request" ,0);

      else if ( 0 == strcmp(NMI_SAF, NMI_Code) )
         nciftr_log_message( 1, 1, "SAF (08X0) request from host not supported.",
                     "process_08X0_request",0 );

      else
      {
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Received unsupported 08X0 request from host. NMI = %s",
                  NMI_Code );
         nciftr_log_message( 1, 1, buffer, "process_08X0_request",0 );
      }
   }

   /* Continue to process any 08X0 msg that is supported. */
   if ( ret_val == true )
   {
      /* Send an 1814 response to host. */
      if ( true == issue_rsp_08X0( type ) )
      {
         if ( 0 != strcmp(new_state, same_state) )
         {
            /* Host state has changed.  Update shared memory. */
            if ( false == set_host_state( new_state, err_buf ) )
            {
               memset(buffer, 0x00, sizeof(buffer) );
               strcpy( buffer, "Unable to update host state. " );
               strcat( buffer, err_buf );
               nciftr_log_message( 1, 2, buffer, "process_08X0_request" ,0);
            }
         }
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         process_08X0_response
 *
 *  DESCRIPTION:  This function handles an 1814 response from the host.
 *                It updates shared memory if a change is warranted. If
 *                bringing the host online, then a keychange is initiated
  *
 *  INPUTS:       cmd -- Logon or Logoff
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_08X0_response(int cmd)
{
   INT     ret_val = true;
   CHAR    buffer[200]  = "";
   CHAR    err_buf[200] = "";
   CHAR    echo_req_timer_key[LOOKUP_KEY_SIZE]="";
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   CHAR    destination_que[10]="";
   CHAR     strTemp[256] = "" ;
   CHAR     strTemp1[256] = "" ;
   CHAR		bindir[256]  ="";
   CHAR     SbatchCMD[128]="";

   /* Populate this field for statistical purposes. */
   strcpy( Auth_Tx.TLF01_details.response_code, ISO_APPROVED );

   strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
   
   if ( 0 == cmd )
   {
      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 != strcmp(ONLINE, current_state) )
      {
	     if ( true == set_host_state( ONLINE, err_buf ) )
         {
         	strcpy( buffer, "host state to online Now. " );
            nciftr_log_message( 1, 2, buffer, "process_08X0_response" ,0);

		    //  We have to start sbatmp as Cadencie become online
		    strncpy(Auth_Tx.TLF01_details.acquirer_id,"nciftr",6);
			strncpy(destination_que,"nciftr",6);
			
			GetAscendentBinDirectory(bindir);
			strcat(bindir,"sbatmp");
			sprintf(SbatchCMD,"%s %s",bindir,destination_que);
			strcat(SbatchCMD,"&");
			
			sprintf( strTemp, "sbatmp CMD to start: %s", SbatchCMD);
			nciftr_log_message( 1, 2, strTemp, "process_08X0_response" ,0);
			
			ret_val = system(SbatchCMD);
			if(ret_val==-1)
			{
				strcpy( strTemp1, "Starting sbatmp for MP Stip Transaction Failed");
				nciftr_log_message( 1, 2, strTemp1, "process_08X0_response",0 );
		    }
			else
			{
				strcpy( strTemp1, "Starting sbatmp for MP Stip Transaction");
				nciftr_log_message( 1, 2, strTemp1, "process_08X0_response",0 );
			}
			
         }
         else
         {
            strcpy( buffer, "Unable to update host state to online. " );
            strcat( buffer, err_buf );
            nciftr_log_message( 1, 2, buffer, "process_08X0_response",0 );
         }
      }
	  else
	  {
	  	  strcpy(buffer,"Host is already online");
		  nciftr_log_message(1, 1, buffer, "process_08X0_response",5);
	  }

      /* This field may have been overwritten on the Key Change.
       * Need for updating txn statistics.
       */
    //  Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   }
   else if ( 1 == cmd )
   {
      /* Read shared memory to get current state of host. */
      read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                             current_state, err_buf );

      if ( 0 != strcmp(OFFLINE, current_state) )
      {
         nciftr_log_message( 1, 1, "Received Log OFF response. Setting state offline.",
                     "process_08X0_response" ,0);

         if ( false == set_host_state( OFFLINE, err_buf ) )
         {
            strcpy( buffer, "Unable to update host state to offline. " );
            strcat( buffer, err_buf );
            nciftr_log_message( 1, 1, buffer, "process_08X0_response",0 );
         }
      }
	   else
	  {
	  	  strcpy(buffer,"Host is already offline");
		  nciftr_log_message(1, 1, buffer, "process_08X0_response",0);
	  }
   }
   else
   {
      strcpy( Auth_Tx.TLF01_details.response_code, INVALID_TXN );
      sprintf( buffer,
              "Received response to unsupported network management message. NMI=%s,STAN=%s",
               NMI_Code, Auth_Tx.TLF01_details.sys_trace_audit_num );

      nciftr_log_message( 1, 2, buffer, "process_08X0_response",0 );
   }

   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, &Auth_Tx, NA );

   return;
}

/******************************************************************************
 *
 *  NAME:         PROCESS_NET_MANAGEMENT_FROM_HOST
 *
 *  DESCRIPTION:  This function handles network management (08nn) messages
 *                from the host.  These can either be requests or responses.
 *                This function decides which, and calls the appropriate
 *                handler.
 *
 *  INPUTS:       cmd - log on or logoff
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:      Abhishek
 *
 ******************************************************************************/
void process_net_management_from_host(int cmd)
{
      process_08X0_response(cmd);

   return;
}

/******************************************************************************
 *
 *  NAME:         BUILD_08X0_MSG
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 08X0 request.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_08X0_msg( pCHAR err_buf )
{
   INT		ret_val = true;
   CHAR		time_date[25];
   CHAR		date_str[9];
   CHAR		time_str[7];
   INT		len = 0;
   CHAR		temp_len[5]="";
   CHAR     temp_data[40]="";
   CHAR     private_62[7]="";
   CHAR     private_63[100]="";

   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 3 - PROCESSING CODE : 000000 */
	memcpy( Auth_Tx.TLF01_details.processing_code,"000000",DE3_SIZE);
	genutil_asc_to_bcd (Auth_Tx.TLF01_details.processing_code, 
				    	sizeof(HOST_MESSAGE.proccode)*2,
				    	HOST_MESSAGE.proccode) ; 

   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
	create_stan( Auth_Tx.TLF01_details.sys_trace_audit_num );
    strncpy(trmp_stand,Auth_Tx.TLF01_details.sys_trace_audit_num, DE11_SIZE);
    genutil_asc_to_bcd (Auth_Tx.TLF01_details.sys_trace_audit_num,
			  			sizeof(HOST_MESSAGE.stan)*2,
						HOST_MESSAGE.stan) ;

	/* FIELD 22 - POS ENTRY MODE */
	strcpy( Auth_Tx.TLF01_details.pos_entry_mode,"0802");
	genutil_asc_to_bcd ( Auth_Tx.TLF01_details.pos_entry_mode,
		 				 sizeof(HOST_MESSAGE.posentry)*2,
		 				 HOST_MESSAGE.posentry) ;
	/* FIELD 24 -NII */
	strcpy(Auth_Tx.TLF01_details.nii,"0123");
	genutil_asc_to_bcd (Auth_Tx.TLF01_details.nii, 
						sizeof(HOST_MESSAGE.nii)*2, 
						HOST_MESSAGE.nii);
	
    /* FIELD 25 - POS CONDITION CODE */
    memcpy(Auth_Tx.TLF01_details.pos_condition_code,"00", DE25_SIZE);
	genutil_asc_to_bcd (Auth_Tx.TLF01_details.pos_condition_code, 
						sizeof(HOST_MESSAGE.pos_condcd)*2, 
						HOST_MESSAGE.pos_condcd) ;

	/* FIELD 35 - TRACK DATA */
	strcpy(temp_data,"4111111111111111=19082011270000000432");
	memcpy(Auth_Tx.TLF01_details.track2, temp_data,strlen(temp_data));
	len = strlen( Auth_Tx.TLF01_details.track2 );
    sprintf( temp_len, "%02d", len );
	genutil_asc_to_bcd (temp_len,
						sizeof(HOST_MESSAGE.track2_len)*2, 
						HOST_MESSAGE.track2_len) ;
	genutil_asc_to_vbcd (Auth_Tx.TLF01_details.track2, 
						strlen(Auth_Tx.TLF01_details.track2),
						HOST_MESSAGE.track2) ;
	
	/* FIELD 41 -TERMINAL ID */
	memcpy(HOST_MESSAGE.termid,DUMMY_TID, DE41_SIZE);

	/* FIELD 42 - CARD ACCEPT ID */
	memset(HOST_MESSAGE.cardaccid,0x20,DE42_SIZE);
	memcpy(HOST_MESSAGE.cardaccid,DUMMY_MID,DE42_SIZE);

	/* FIELD 62 - PRIVATE DATA */
	strcpy(private_62,"000307");
	memcpy(HOST_MESSAGE.priv62,private_62, strlen(private_62));
	
	len = strlen( HOST_MESSAGE.priv62);
	sprintf( temp_len, "%04d", len );
	genutil_asc_to_vbcd (temp_len,
						sizeof(HOST_MESSAGE.priv62_len)*2, 
						HOST_MESSAGE.priv62_len) ;
	

	/* FIELD 63 - PRIVATE DATA */
	strcpy(private_63,"03");
	memcpy(HOST_MESSAGE.priv63,private_63, strlen(private_63));
	
	len = strlen( HOST_MESSAGE.priv63);
	sprintf( temp_len, "%04d", len );
	genutil_asc_to_vbcd (temp_len,
						sizeof(HOST_MESSAGE.priv63_len)*2, 
						HOST_MESSAGE.priv63_len) ;
	
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         build_08X0_msg_response
 *
 *  DESCRIPTION:  This function will populate the HOST_MESSAGE structure for
 *                an 1814 response.
 *
 *  INPUTS:       Auth_Tx - (Global)Transaction information
 *
 *  OUTPUTS:      HOST_MESSAGE - (Global)Structure with data to go to host
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT build_08X0_msg_response( pCHAR err_buf )
{
   INT     ret_val = true;
   INT		len = 0;
   CHAR		temp_len[3]="";


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   /* FIELD 3 - PROCESSING CODE : 000000 */
		memcpy(HOST_MESSAGE.proccode,"000000",DE3_SIZE);

   /* FIELD 11 - SYSTEM TRACE AUDIT NUMBER */
		memcpy( HOST_MESSAGE.stan,
						 Auth_Tx.TLF01_details.sys_trace_audit_num,
						DE11_SIZE );

   /* FIELD 12 - TIME (LOCAL TIME) */
	  
		  memcpy( HOST_MESSAGE.time,
              Auth_Tx.TLF01_details.time_hhmmss,
              HHMMSS_LEN);

	/* FIELD 13 - DATE */
		 
		 memcpy( HOST_MESSAGE.date,
				  Auth_Tx.TLF01_details.date_yyyymmdd + MMDD_START_POS,
				 MMDD_LEN);
	
	/* FIELD 33 - FORWARDING INSTITUTION IDENTIFICATION CODE */
	
		//len = MAX_DE33_SIZE;
		//sprintf( temp_len, "%02d", len );
		//memcpy( HOST_MESSAGE.fwd_inst_len, temp_len, 2 );

	/*	memcpy(HOST_MESSAGE.fwd_inst_id,"000000",6);*/

	
		//memcpy(HOST_MESSAGE.fwd_inst_id,Auth_Tx.forwarding_inst_id_cd,MAX_DE33_SIZE);


   /* FIELD 39 - RESPONSE CODE */

   /* Populate this field for logging and statistical purposes only.
    * It does not get sent to the host.
    */
	strcpy( HOST_MESSAGE.responsecd, NETMGMT_APPROVED );


   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SEND_08NN_TO_HOST
 *
 *  DESCRIPTION:  This function sends an 08X0/1814 message to the host.
 *                It is the same as sending any message to the host, but
 *                the error message is modified a little bit to include
 *                transmission timestamp.  This is because other identifying
 *                information like card number, TID & MID, are not present
 *                in an 08nn message.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message if an error occurs
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_18nn_to_host( pCHAR err_buf )
{
   INT   ret_val;
   CHAR  buffer[100]="";

   ret_val = send_message_to_host( err_buf,true );//praneeth added true

   if ( ret_val == false )
   {
      sprintf( buffer,
              "%s  DE7 = %s.",
               err_buf,
               Auth_Tx.TLF01_details.transmission_timestamp );
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_MONITOR_QUERY
 *
 *  DESCRIPTION:  This function answers the call of the Monitor by sending
 *                its host state (online, offline) back to the Monitor.
 *
 *  INPUTS:       p_msg - PTE message from Monitor
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void process_monitor_query( pPTE_MSG p_msg )
{
   pPTE_MSG    p_msg_query = NULL_PTR;
   NCF01       ncf01;
   CHAR        err_buf[200]="";
   CHAR        Buff[256]="";
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0};
   XINT        ReturnCode = 0 ;
   pBYTE       pCurrent;

   /* Request from Monitor to send status information */
   read_shared_mem_table( net_consec_tmouts_ctr, active_txns_ctr,
                          current_state, err_buf );
   pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg));
   memset( &ncf01, 0,        Ncf01_Size );
   memcpy( &ncf01, pCurrent, Ncf01_Size );
   strcpy( ncf01.status,     current_state );
 
   p_msg_query = ptemsg_build_msg( MT_SYS_MONITOR,
                                   ST1_NETWORK_UPDATE, 
                                   ST2_NONE,
                                   applnk_que_name, 
                                   application_que_name,
                                   (pBYTE)&ncf01,
                                   sizeof(ncf01), 
                                   0 );
   if ( NULL_PTR == p_msg_query )
   {
      nciftr_log_message( 2, 3, "Unable to build PTE message for Monitor query",
                   "process_monitor_query" , 0);
   }
   else
   {
      memset (ConnectionId, 0, sizeof (ConnectionId)) ;
      ReturnCode = ReadMemMonitorTable (ConnectionId) ;
      if (ConnectionId[0] != 0)
      {
         ptemsg_set_orig_comms_info( p_msg_query, ConnectionId) ;
         if ( false == send_msg( p_msg_query, applnk_que_name, err_buf ))
            nciftr_log_message( 2, 3, err_buf, "process_monitor_query",0 );
      }
      free( p_msg_query );
   }
   return;
}

