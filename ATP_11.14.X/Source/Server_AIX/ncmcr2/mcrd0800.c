/******************************************************************************
 
 	MCRD0800.C
 
    Copyright (c) 2002, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  MasterCard Host Handler Network Management Transaction Processor
 
    This module is a part of the MasterCard Host Handler.  It contains the
    network management (08xx) message processing.
 
    APPLICATION:  Advanced Transaction Processor (ATP)

 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_database.h"
#include "nc_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "memmnger.h"
#include "mcrdmain.h"
#include "mcrdutil.h"
#include "ncutils.h"
#include "txutils.h"



/* GLOBAL VARIABLES */
extern CHAR  Stan[7];
extern CHAR  Logon_NMI_Code[4];
extern INT   Tran_Flow;
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
/* Queue definitions */
extern CHAR  cur_app_name [];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  security_que_name[];
extern CHAR  cirrus_que_name[];
extern CHAR  timerds_que_name[];
extern CHAR  AppName[8];

/* Database structures */
extern NCF01     ncf01_i;
extern NCF01     ncf01_a;
extern NCF30     ncf30;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];
extern CHAR   SHARED_MEMORY_TBL[15];

extern CHAR   addl_data[101];
extern INT    reqidx;                 /* next open position in request buffer */
extern INT    buflen;                 /* total size of buffer */
extern INT    bitmapidx;              /* offset of bitmap... */
extern BYTE   ReqRsp_Buffer[COMM_BUF_SZE];
extern INT    BitFieldSize;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;


/******************************************************************************
 *
 *  NAME:         PERFORM_HOST_LOGON_REQUEST
 *
 *  DESCRIPTION:  This procedure will send a logon request to the host to
 *                ensure that the host is up and running.
 *
 *  INPUTS:       source - Indicates who called this function.
 *                         It could be called by:
 *                            Host Handler - Must log on before any transactions
 *                                           can be approved.
 *                            State Timer  - It has been a while since we talked
 *                                           to the host.  Must let it know we
 *                                           are still connected.
 *                            MCard Host   - The host can send a logon request
 *                                           in which case we must send an echo.
 *                                           The host can also respond to our
 *                                           logon request with an echo.
 *                            Operator     - A system operator can initiate a
 *                                           log ON / OFF request from Opermon.
 *
 *                request_type - Can be Log Off, Logon, or Echo Request or NULL
 *                p_auth_tx    - Transaction record, may be NULL
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/17/99 - Modified SOURCE_IS_HOST to not check current_state, but
 *                     set it to LIVE if not already set.
 ******************************************************************************/
INT perform_host_logon_request(  CHAR request_type[], INT source,
                                pAUTH_TX p_auth_tx )
{
   INT   ret_val = OK;
   CHAR  ErrMsg[512] = {0};
   CHAR  param[6];
   CHAR  time_date[25] = "";
   CHAR  lLogon_Code[4];

   /* Create a STAN unique for this day; use the local time. */
   ptetime_get_timestamp( time_date );
   get_time( time_date, Stan );

   /* Determine who called this function. */
   read_shared_mem_table(AppName, net_consec_tmouts_ctr, active_txns_ctr,
                         current_state, ErrMsg );
   switch( source )
   {
      case SOURCE_IS_HOST_HANDLER:

               /* We need to issue a logon request to the MCard host. */
               memcpy( p_auth_tx->TLF01_details.response_text, "Sending logon request to host.",
                             sizeof(p_auth_tx->TLF01_details.response_text)-1 );
               ncmcr2_log_message( 1, 1, "Sending logon request to host" , "perform_host_logon_request" , 0 ,"");

               p_auth_tx->TLF01_details.tx_key         = AUTH_LOGON;
               p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;
               strcpy( p_auth_tx->TLF01_details.processing_code, "920000" );
               strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
            break;

      case SOURCE_IS_HOST:

               /* The MasterCard host has sent a log on request to us.
                * We must send a response back.
                */
               if ( 0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID ) )
               {
                 memcpy( p_auth_tx->TLF01_details.response_text, "Received logon request from host.",
                               sizeof(p_auth_tx->TLF01_details.response_text)-1 );
                 ncmcr2_log_message( 1, 1, "Received logon request from host." , "perform_host_logon_request" , 0 ,"");
               }
               else if ( 0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF ) )
               {
                  memcpy( p_auth_tx->TLF01_details.response_text, "Received logoff request from host.",
                                sizeof(p_auth_tx->TLF01_details.response_text)-1 );
                  ncmcr2_log_message( 1, 1, "Received logoff request from host." , "perform_host_logon_request" , 0 ,"");
               }
               
               p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
               strncpy( Stan,
                        p_auth_tx->TLF01_details.sys_trace_audit_num,
                        bf11_len );
               strncpy( p_auth_tx->TLF01_details.response_code, APPROVED, 2 );

               strcpy(lLogon_Code,Logon_NMI_Code);
               /* Send response to host. */
               perform_net_mngmnt_response( Logon_NMI_Code, p_auth_tx );

               /* Update the host state in shared memory. */
               if ( 0 == strcmp( lLogon_Code, LOGON_NMI_GROUP_ID ) )
               {
                  if ( 0 != strcmp( ONLINE, current_state ) )
                   {
                      set_host_state( AppName, ONLINE, ErrMsg );
                      memcpy( p_auth_tx->TLF01_details.response_text, "Log ON Successful. Setting Host ONLINE",
                                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
                      ncmcr2_log_message( 1, 1, "Log ON Successful. Setting Host ONLINE" , "perform_host_logon_request" , 0 ,"");
                   }
               }
               else if( 0 == strcmp( lLogon_Code, LOGON_NMI_GROUP_LOGOFF ) )
               {
                  if ( 0 != strcmp( OFFLINE, current_state ) )
                  {
                     set_host_state( AppName, OFFLINE, ErrMsg );
                     memcpy( p_auth_tx->TLF01_details.response_text, "Log OFF Successful. Setting Host OFFLINE",
                                   sizeof(p_auth_tx->TLF01_details.response_text)-1 );
                     ncmcr2_log_message( 1, 1, "Log OFF Successful. Setting Host OFFLINE" , "perform_host_logon_request" , 0 ,"");
                  }
               }
               break;
               /* end case SOURCE_IS_HOST */

      case SOURCE_IS_OPERATOR:

               /* This is an operator initiated log on or off request. */
               if ( OK != perform_net_mngmnt_request( request_type,
                                                      p_auth_tx ) )
                  return( NOT_OK );
               else
                  return( OK );
               break;

      case SOURCE_IS_STATE:

               /* This is the result of lack of activity to the MCard host.
                * The activity timer has expired.  Need to send MCard a
                * message to let them know we are still alive.
                */
               if ( 0 == strcmp(ONLINE,current_state) )
               {
                  memcpy( p_auth_tx->TLF01_details.response_text, "Sending logon ECHO request to host.",
                                sizeof(p_auth_tx->TLF01_details.response_text)-1 );
                  ncmcr2_log_message( 1, 1, "Sending logon ECHO request to host" , "perform_host_logon_request" , 0 ,"");

                  p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
                  strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
                  if ( OK != perform_net_mngmnt_request( ECHO_REQUEST,
                                                         p_auth_tx ) )
                     return( NOT_OK );
                  else
                     return( OK );
               }
               break;
               /* end case SOURCE_IS_STATE */

      default:
               strcpy( ErrMsg, "Invalid param in call to function.  Param = ");
               sprintf( param, "%d", source );
               strcat( ErrMsg, param );
               ncmcr2_log_message( 1, 1, ErrMsg , "perform_host_logon_request" , 0 ,"");
            break;
   }
   return( ret_val );
}  /* perform_host_logon_request */


/******************************************************************************
 *
 *  NAME:         PERFORM_NET_MNGMNT_REQUEST
 *
 *  DESCRIPTION:  This procedure prepares a network management request
 *                message for the host.  These are also known as log on
 *                and log off messages.
 *
 *  INPUTS:       request_type - Transaction type of request transaction
 *                p_auth_tx    - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_net_mngmnt_request( char request_type[], pAUTH_TX p_auth_tx )
{
   CHAR      gmt_time[18] = {0};
   CHAR      msg_buf[40]  = {0};

   /* Populate some required fields in auth_tx. */
   strncpy( p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id,
            sizeof(ncf01_i.tpdu_id));
   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;

   /* Current Greenwich Mean Time for the transmission-to-host time.
    * Format is YYYYMMDDhhmmss.
    */
   pintime_get_gmt( gmt_time );
   memcpy( p_auth_tx->TLF01_details.host_start_time, gmt_time, 14 );

   /* Use the time hhmmss for sys_trace_audit_num */
   /*  to maintian consistency : using the Stan
   memcpy( p_auth_tx->TLF01_details.sys_trace_audit_num, gmt_time, 6 );*/

   memcpy( p_auth_tx->TLF01_details.sys_trace_audit_num, Stan, 6 );

   /* Determine type of request. */
   memset( msg_buf, 0x00, sizeof(msg_buf) );
   if ( 0 == strcmp(request_type, LOGON_REQUEST) )
   {
      /* Determine if log on is by BIN or Group Id */
      if ( 0 == strcmp( p_auth_tx->TLF01_details.card_num, NULL_STR ) )
         strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_ID );
      else
         strcpy( Logon_NMI_Code, LOGON_NMI_BIN );

      strcpy( msg_buf, "Sending LOGON Request to host." );
   }
   else if ( 0 == strcmp(request_type, LOGOFF_REQUEST) )
   {
      /* Determine if log off is by BIN or Group Id */
      if ( 0 == strcmp( p_auth_tx->TLF01_details.card_num, NULL_STR ) )
         strcpy( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF );
      else
         strcpy( Logon_NMI_Code, LOGON_NMI_BIN_LOGOFF );

      strcpy( msg_buf, "Sending LOGOFF Request to host." );
   }
   else if ( 0 == strcmp(request_type, SAF_REQUEST) )
   {
      strcpy( Logon_NMI_Code, LOGON_SAF_REQUEST );
      strcpy( msg_buf, "Sending SAF Request to host." );
   }
   else
   {
      /* Log ON ECHO TEST */
      strcpy( Logon_NMI_Code, ECHO_NMI );
      strcpy( msg_buf, "Sending ECHO TEST to host." );
   }

   /* Inform MONITOR of our request to the host. */
   memcpy( p_auth_tx->TLF01_details.response_text, msg_buf,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcr2_log_message( 1, 1, msg_buf , "perform_net_mngmnt_request" , 0 ,"");

   /* Send the request to the host. */
   return( send_net_mngmnt_request( p_auth_tx ) );

} /* perform_net_mngmnt_request */

      
/******************************************************************************
 *
 *  NAME:         SEND_NET_MNGMNT_REQUEST
 *
 *  DESCRIPTION:  This procedure will send a network management request to
 *                the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record going to the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 02/25/99 - Added return value checking to 'build_host_request'.
 *                     Added code to set host timer prior to sending request.
 ******************************************************************************/
INT send_net_mngmnt_request( pAUTH_TX p_auth_tx )
{
   pPTE_MSG	 p_msg_host_out = NULL_PTR;
   INT       ret_code = NOT_OK;
   CHAR      err_msg[512]={0};
   WORD      request_timer;

   /* Format message to send to host. */
   Tran_Flow = SWITCHED_OUT;
   build_host_structure( p_auth_tx );
   if ( OK != build_host_request( p_auth_tx ) )
   {
      memcpy( p_auth_tx->TLF01_details.response_text, "Failed to build host request message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcr2_log_message( 1, 1, " Failed to build host request message " , "send_net_mngmnt_request" , 0 ,"");
   }
   else
   {
      /* Set host timer. */
      request_timer = (WORD)(atoi(ncf01_i.request_timer));
      ret_code = set_host_timer( p_auth_tx, application_que_name,
                                 request_timer, err_msg );
      if ( OK == ret_code )
      {
         /* Build PTE message. */
         ret_code = build_and_send_request_message( MT_OUTGOING, 
                                             ST1_NONE,
                                             ST2_NONE,
                                             dialog_que_name,
                                             ReqRsp_Buffer,
                                             buflen,
                                             0,
                                             p_auth_tx->TLF01_details.tpdu_id,
                                             err_msg );
         if ( OK != ret_code )
         {
          
            memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            ncmcr2_log_message( 3, 3, err_msg , "send_net_mngmnt_request" , 0 ,"");
         }
         else
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

            /* Log transaction to trace file. */
            write_to_txn_file();
         }
      }
   }
   return( ret_code );
} /* send_net_mngmnt_request */


/******************************************************************************
 *
 *  NAME:         PERFORM_NET_MNGMNT_RESPONSE
 *
 *  DESCRIPTION:  This procedure prepares a network management response.
 *                It will then send the response to the host.  This function
 *                is called when the host issues a log on/off to ATP, and we
 *                must respond to it.
 *
 *  INPUTS:       nmi_code  - Indicates if it is a log on, log off, or echo msg
 *                p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT perform_net_mngmnt_response( char nmi_code[], pAUTH_TX p_auth_tx )
{
   CHAR   msg_buf[512]={0};
   INT    ret_code;

   /* Need to find our way back to the host. */
   strncpy( p_auth_tx->TLF01_details.tpdu_id,
            ncf01_i.tpdu_id,
            sizeof(ncf01_i.tpdu_id) );

   /* Determine type of message to send to MONITOR. */
   memset( msg_buf, 0x00, sizeof(msg_buf) );
   if ( 0 == strcmp( nmi_code, LOGON_NMI_GROUP_ID ) )
   {
      strcpy( msg_buf, "Sending LOGON Response to host." );
   }
   else if ( 0 == strcmp( nmi_code, LOGON_NMI_GROUP_LOGOFF ) )
   {
      strcpy( msg_buf, "Sending LOGOFF Response to host." );
   }
   else 
   {
      strcpy( msg_buf, "Sending ECHO Response to host." );
   }

   /* Inform MONITOR of our response to the host. */
   memcpy( p_auth_tx->TLF01_details.response_text, msg_buf,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcr2_log_message( 1, 1, msg_buf , "perform_net_mngmnt_response" , 0 ,"");

   /* Send the response to the host.
    * Do not display error; any error is displayed within child function.
    */
   ret_code = send_net_mngmnt_response( p_auth_tx );
   return( ret_code );

} /* perform_net_mngmnt_response */


/******************************************************************************
 *
 *  NAME:         SEND_NET_MNGMNT_RESPONSE
 *
 *  DESCRIPTION:  This procedure will send the network management response to
 *                the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record going to the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_net_mngmnt_response( pAUTH_TX p_auth_tx )
{
   INT       ret_code = NOT_OK;
   CHAR      err_msg[512]={0};


   Tran_Flow = SWITCHED_IN;
   build_host_structure( p_auth_tx );
   if ( OK != build_host_request( p_auth_tx ) )
   {
      memcpy( p_auth_tx->TLF01_details.response_text, "Failed to build host request message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcr2_log_message( 1, 1, "Failed to build host request message" , "send_net_mngmnt_response" , 0 ,"");
   }
   else
   {
      /* Build PTE message. */
      ret_code = build_and_send_request_message( MT_OUTGOING, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              dialog_que_name,
                                              ReqRsp_Buffer,
                                              buflen,
                                              0,
                                              p_auth_tx->TLF01_details.tpdu_id,
                                              err_msg );
      if ( OK != ret_code )
      {
         memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 3, 3, err_msg , "send_net_mngmnt_response" , 0 ,"");
      }
      else
      {
         /* Update transaction statistics */
         update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

         /* Log transaction to trace file. */
         write_to_txn_file();
      }
   }
   return( ret_code );
} /* send_net_mngmnt_response */



/******************************************************************************
 *
 *  NAME:         PROCESS_LOGON_RESPONSE
 *
 *  DESCRIPTION:  This function will process a logon response by resetting
 *                the host app fields to there original values.  This function
 *                is entered after a logon request is sent to the host and the
 *                host responds.
 *
 *  INPUTS:       p_auth_tx - Transaction response from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Always returns OK
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_logon_response( pAUTH_TX p_auth_tx )
{
   CHAR  Buffer[512]={0};
   INT   insert_flag = 0;
   INT   ret_code = NOT_OK;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   CHAR        err_mesg[200] = {0};
   CHAR  Buff[512]={0};

     /* Act slightly differently for Log ONs versus Log OFFs */
   if ( (0 == strncmp( Logon_NMI_Code, LOGON_NMI_BIN_LOGOFF,   3 )) ||
        (0 == strncmp( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF, 3 ))   )
   {
      /* LOG OFF */
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, APPROVED, 2 ) )
      {
         set_host_state( AppName, OFFLINE, Buffer );
         memcpy( p_auth_tx->TLF01_details.response_text, "Log OFF Successful. Setting Host State to OFFLINE",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 1, "Log OFF Successful. Setting Host State to OFFLINE" , "process_logon_response" , 0 ,"");
      }
      else
      {
         insert_flag = 1;
         memcpy( p_auth_tx->TLF01_details.response_text, "Log OFF attempt Failed",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 1, "Log OFF attempt Failed" , "process_logon_response" , 0 ,"");
      }
   }
   else if ( (0 == strncmp( Logon_NMI_Code, LOGON_NMI_BIN,   3 )) ||
             (0 == strncmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID, 3 ))   )
   {
      /* LOG ON */
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, APPROVED, 2 ) )
      {
         set_host_state( AppName, ONLINE, Buffer );
         memcpy( p_auth_tx->TLF01_details.response_text, "Log ON Successful. Setting Host State to ONLINE",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 1, "Log ON Successful. Setting Host State to ONLINE" , "process_logon_response" , 0 ,"");

      }
      else
      {
         insert_flag = 1;
         memcpy( p_auth_tx->TLF01_details.response_text, "Log On attempt Failed",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 2, "Log On attempt Failed" , "process_logon_response" , 0 ,"");
      }
   }
   else if ( (0 == strncmp( Logon_NMI_Code, LOGON_SAF_REQUEST, 3 ) ) )
   {
      /* Response to an SAF Request */
      insert_flag = 1;
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, APPROVED, 2 ) )
      {
         memcpy( p_auth_tx->TLF01_details.response_text, "Received approved response to SAF request.",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 1, "Received approved response to SAF request." , "process_logon_response" , 0 ,"");
      }
      else
      {
		  
         sprintf( Buffer, "SAF Request failed. Response code = '%s'.",
                  p_auth_tx->TLF01_details.response_code );
         memcpy( p_auth_tx->TLF01_details.response_text, Buffer,
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 2, Buffer , "process_logon_response" , 0 ,"");
      }
   }
   else
   {
      /* ECHO TEST */
      insert_flag = 1;
      if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, APPROVED, 2 ) )
      {
         memcpy( p_auth_tx->TLF01_details.response_text, "Log On ECHO TEST Successful.",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 1, "Log On ECHO TEST Successful." , "process_logon_response" , 0 ,"");
      }
      else
      {
         set_host_state( AppName, OFFLINE, Buffer );
         memcpy( p_auth_tx->TLF01_details.response_text, "Log On ECHO TEST FAILED!!",
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         ncmcr2_log_message( 1, 2, "Log On ECHO TEST FAILED!!" , "process_logon_response" , 0 ,"");
      }
   }
	/* Initialize this global variable for the next Log ON/OFF/... command. */
//   strcpy( Logon_NMI_Code, NULL_STR ); /* Commented and put this condition after sending event to Txcntl - Girija Y */
   

   /* Insert Log ON and Log OFF if successful; do not insert Echo or SAF. */
   if( insert_flag == 0 )
   {
	   /* Transaction is completed.  Insert it into the database. */
      strcpy( p_auth_tx->TLF01_details.message_type, "0810" );
      ret_code = insert_into_TLF01( p_auth_tx );
   }
   /* Support Automatic Resend Stand-In Tx -*/
   if ( insert_flag == 0 )		
   {
	   if ( (0 == strncmp( Logon_NMI_Code, LOGON_NMI_BIN,   3 )) ||
             (0 == strncmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID, 3 ))   )
	   {
		   if ( 0 == strncmp( p_auth_tx->TLF01_details.response_code, APPROVED, 2 ) )
		   {
			   strcpy(p_auth_tx->TLF01_details.originator_queue,application_que_name);
			   p_msg_ins = ptemsg_build_msg( MT_LOGON_RESPONSE,
												  ST1_NONE, 
												  ST2_NONE,
												  application_que_name, 
												  authorizer_que_name,
												  (pBYTE)p_auth_tx,
												  sizeof(AUTH_TX), 
												  0 );
				if ( NULL_PTR == p_msg_ins )
				{
					ncmcr2_log_message( 1, 1, "Error sending network status to Txcntl" , "process_logon_response" , 0 ,"");
				}
			   ret_code = pteipc_send(p_msg_ins, authorizer_que_name);
			   if ( ret_code < 0 )
			   {
				   pteipc_get_errormsg( ret_code, err_mesg );
				   sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
				   ncmcr2_log_message( 3, 3, Buff , "process_logon_response" , 0 ,"");
			   }
		   }
	   }
   }
   /* Initialize this global variable for the next Log ON/OFF/... command. */
   strcpy( Logon_NMI_Code, NULL_STR ); /* Girija Y - ThoughtFocus */
   return( OK );
}


/******************************************************************************
 *
 *  NAME:         PROCESS_NETWORK_ADVICE
 *
 *  DESCRIPTION:  This procedure is called when an 0820 message is received
 *                from the host.  This is a Network Advice generated by the
 *                host.  This function will display the message in field 70
 *                then return.  No further processing is required.  A response
 *                to the host is not expected.
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void process_network_advice( pAUTH_TX p_auth_tx )
{
   char      tempstr[256]={0};

   /*+------------------+
     | Display message. |
     +------------------+*/
   strcpy( tempstr, "** M/C NETWORK ADVICE (0820) - Field 70 code = " );
   strcat( tempstr, Logon_NMI_Code );
   strcat( tempstr, ",  " );
   strcat( tempstr, addl_data );
   memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   ncmcr2_log_message( 1, 1, tempstr , "process_network_advice" , 0 ,"");
}  /* process_network_advice */


