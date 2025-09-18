/******************************************************************************

   MCRDMAIN.C

   Copyright (c) 2007, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  MasterCard Host Handler Driver

   This module is the host handler for the MasterCard host. It
   will process all messages destined to, or originated from,
   the MasterCard host.

   APPLICATION:  Advanced Transaction Processor (ATP)

  REVISION HISTORY:

	$Log:svn://192.168.1.229/EpicPortz/ECN/Trunk/Source/Server_AIX/ncmcr2/mcrdmain.c$
	
		Rev 1.15   Dec 21 2009   TF-Ajay 
    Updated version to 1.0.0.19
	Client will run multiple of sbatch at a time just renameing of EXE. Dont compare sbacthA as query name exactly.
	So changed to check only first 4 character 
	
	  Rev 1.14   Nov 23 2009   TF-Ajay 
    Updated version to 1.0.0.18
    Changed code to send void sale also as reversal(0400)
	Reversal timestamp should be the same as the original 0100 message.
	Rel 09-76


			Rev 1.13   Nov 10 2009   TF-Ajay
	BDO wants to send void and reversal txn to cadencie
	Updated version 1.0.0.17
   
	 Rev 1.12   Aug 13 2009   TF-Phani  
   Txn process as a STIP and if it is originally comes from the DCPISO or voice 
   no need pass fld 6,9,19, and 51 
   Updated version 1.0.0.16

		Rev 1.11   Aug 10 2009   TF-Phani  
   ATP should not be doing any PIN TRANSALATION during offline mode. since during STIP,
   ATP will be the one validating the cards.  Directly validates the PIN. 
   Updated version 1.0.0.15
	  
		Rev 1.10   Jul 13 2009   TF-Ajay 
   Txn process as a STIP and After process if it gets the reversal
   It should go to host as a 0420 msg
   Updated version 1.0.0.14

		  Rev 1.0.0.10 Jun 12, 2009 Ajay  @ThoughtFocus
   Updated version to 1.0.0.13
   Chnaged the code to send Reversal advice as a 0420 if 0120 get time out
   process_timeout_msg()
   Rel 09-37
   
	 Rev 1.0.0.9 Nov 11, 2008 Girija Y @ThoughtFocus
   Updated version to 1.0.0.12
   Chnaged the code to send LOGON_RESPONSE when host comes ONLINE(Earlier the event was sent
   to Txcntl when Host comes ONLINE as well for OFFLINE).
   Rel 08-11

   Rev 1.0.0.8 Aug 14, 2008 Girija Y @ThoughtFocus
   Updated version to 1.0.0.8
   Dont insert into TLF01 Table if the transaction is Originating from STIP Manager
   Rel 08-16

   REV 1.0.0.7 July 28, 2008 - Cadience standin changes to support 0120, 0121, 
               131 message types. Updated the version to 1.0.0.7
   Ravikumar K N @ThoughtFocus, Cadience standin processing changes TFrel 08-16
     
 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <string.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "memmnger.h"
#include "mcrdmain.h"
#include "ncutils.h"
#include "txtrace.h"
#include "diskio.h"
#include "txutils.h"
#include "tx_dbstruct.h"


/* GLOBAL VARIABLES */
CHAR  Stan[7]           = "";
CHAR  Logon_NMI_Code[4] = "";
CHAR  ncmcr2_Error_warning_Filename[256]={0};
CHAR  ncmcr2_module_error_warning_file_name[256]={0};
CHAR  ncmcr2_error_warning_file_path[256]={0};
CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* Queue definitions */
CHAR  cur_app_name [10];
CHAR  updateds_que_name[]          = "updatdsA";
CHAR  authorizer_que_name[]        = "txcntlA";
CHAR  oprmon_que_name[]            = "oprmonI";
CHAR  dialog_que_name []           = "dialogA";
CHAR  applnk_que_name []           = "applnkA";
CHAR  trands_que_name[]            = "trandsA";
CHAR  devds_que_name[]             = "devdsA";
CHAR  netds_que_name[]             = "netdsA";
CHAR  security_que_name[]          = "racalA";
CHAR  cirrus_que_name[]            = "nccirsA";
CHAR  timerds_que_name[]           = "timerdsA";
CHAR  AppName[8] = "";
CHAR  Version[] = "ATP_11.13.0";
/* Database structures */
NCF01               ncf01_i;
NCF01               ncf01_a;
NCF30               ncf30;
NEG01               neg01;
TLF01               tlf01;
MCC_ONLINE_UPDATES  Mcc_Gui;
BDKKEY01			Bdkkey01;

/* Database structure sizes */
INT    Tlf01_Size;
INT    Ncf01_Size;
INT    Ncf30_Size;
INT    Auth_Tx_Size;
INT    Mcc_Gui_Size;
INT	   Bdkkey01_Size;

/* Shared memory variables */
BYTE   net_consec_tmouts_ctr[4];
BYTE   active_txns_ctr[4];
BYTE   current_state[2];
CHAR   SHARED_MEMORY_TBL[15];

/* Counters */
INT    MaxActiveTxns;
INT    MaxConsecTimeouts;

/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

/* These are used for reject 0620/0630 messages. */
CHAR   fwd_inst_id_len[3];
CHAR   fwd_inst_id[12];
CHAR   addl_data_len[4];
CHAR   addl_data[101];
CHAR   advice_len[4];
CHAR   advice[61];
CHAR   recv_inst_len[3];
CHAR   recv_inst[12];
CHAR   F60_len[4];
CHAR   reject_advice_code[4];
CHAR   reject_detail_code[5];
CHAR   reject_text[54];

INT    reqidx;                 /* next open position in request buffer */
INT    buflen;                 /* total size of buffer */
INT    bitmapidx;              /* offset of bitmap... */
INT    invalid_msg_format;     /* flag used in parsing host message */

BYTE   ReqRsp_Buffer [COMM_BUF_SZE];
INT    BitFieldSize = NORMAL_BITFIELD;

CHAR   A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
CHAR   E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

/* Define direction of transaction flow - switched in or switched out. */
INT    Tran_Flow;

INT    MCC_Validation_Functionality;

/* System variables */
extern INT  EndProcessSignalled;
extern INT  MainProcessDone;
extern CHAR ServiceName[12];
extern INT rvrsl_attmpts_value_fr_laterspns_mc;
extern INT rvrsl_attmpts_value_fr_timeout_mc;

/******************************************************************************
 *
 *  NAME:         PinnacleMsgHandler
 *
 *  DESCRIPTION:  This function acts as a switch for messages coming into this
 *                host handler.  It will transfer control to the appropriate
 *                module, based on message type.
 *
 *  INPUTS:       p_msg_in - Incoming message
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pPTE_MSG          p_cur_msg;
   pPTE_MSG          p_msg_out;
   pBYTE             p_data_data = NULL_PTR;
   BYTE              subtype1;
   AUTH_TX           auth_tx;

   pBYTE       pCurrent;
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   BYTE        timer_result;
   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   LONG        ret_code;
   CHAR        err_mesg[512] = {0};
   CHAR        Buff[512]={0};
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   XINT        ReturnCode = 0 ;

   p_cur_msg = p_msg_in;
   p_msg_out = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

   /* Determine message type so we know which module to send it to. */
   switch( ptemsg_get_msg_type(p_cur_msg) )
   {

      case MT_AUTH_REQUEST:

         /* Request coming in from the terminal handler */
         (void)process_incoming_message( p_msg_in );
      break;

      case MT_AUTH_RESPONSE: 

         /* Request coming in from the authorizer */
         (void)process_incoming_message( p_msg_in );
      break;

      case MT_INCOMING: 

         /* Reply from the external host */
         (void)incoming_host_message( p_msg_in );
      break;

      case MT_DB_REPLY:
         /* Message coming from the data server */
         if ( PTEMSG_OK == ptemsg_get_result_code(p_msg_in) )
            mcrdutil_parse_db_ok(p_msg_in);
         else
            mcrdutil_parse_db_error(p_msg_in);
      break;

      case MT_TIMER_TIMEOUT: 
         /* Reply about timeout of ptemsg */
         if ( OK != process_timeout_msg(p_msg_in) )
        	 ncmcr2_log_message( 1, 3, "Error processing timeout message" , "PinnacleMsgHandler" , 0 ,"");
      break;

      case MT_INCOMING_GUI:
         /* GUI is initiating an 0302 message for MasterCard2. or Response from MC
		   312 message*/
         if ( OK != process_file_update_txn( p_msg_in ) )
        	 ncmcr2_log_message( 1, 3, "Error processing a file update txn" , "PinnacleMsgHandler" , 0 ,"");
      break;

      case MT_SYS_MONITOR:

         memset( &auth_tx, 0, Auth_Tx_Size);
         switch( ptemsg_get_msg_subtype1( p_cur_msg ) )
         {
            case ST1_LOGON:
 
               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( LOGON_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  ncmcr2_log_message( 1, 3, "Error executing host LogOn request" , "PinnacleMsgHandler" , 0 ,"");
               }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcr2_log_message( 1, 3, "Error sending network status to monitor" , "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_LOGOFF:
 
               /* Request from Opermon to send LogOff */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( LOGOFF_REQUEST,
                                                         SOURCE_IS_OPERATOR,
                                                         &auth_tx ) )
               {
                  ncmcr2_log_message( 1, 3, "Error executing host LogOff request" , "PinnacleMsgHandler" , 0 ,"");
               }
               if (send_network_status_to_monitor() != OK )
               {
                  ncmcr2_log_message( 1, 1, "Error sending network status to monitor" , "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_ECHOTEST:

               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( ECHO_REQUEST,
                                                      SOURCE_IS_OPERATOR,
                                                      &auth_tx ) )
               {
                  ncmcr2_log_message( 1, 1, "Error sending network status to monitor" , "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_SAF_REQUEST:
 
               /* Request from Opermon to send LogOn */
               p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
               p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
               memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
               if ( OK != perform_host_logon_request( SAF_REQUEST,
                                                      SOURCE_IS_OPERATOR,
                                                      &auth_tx ) )
               {
                  ncmcr2_log_message( 1, 3, "Error executing host SAF request", "PinnacleMsgHandler" , 0 ,"");
               }
               if ( send_network_status_to_monitor() != OK )
               {
                  ncmcr2_log_message( 1, 3, "Error executing host SAF request", "PinnacleMsgHandler" , 0 ,"");
               }
            break;

            case ST1_NETWORK_QUERY:
                /* Request from Monitor to send status information */
                read_shared_mem_table( AppName, net_consec_tmouts_ctr, 
                                       active_txns_ctr, current_state, err_mesg );
                pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg_in));
                memset( &ncf01, 0,        Ncf01_Size );
                memcpy( &ncf01, pCurrent, Ncf01_Size );
                strcpy( ncf01.status,     current_state );
 
                p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                              ST1_NETWORK_UPDATE, 
                                              ST2_NONE,
                                              applnk_que_name, 
                                              application_que_name,
                                              (pBYTE)&ncf01,
                                              Ncf01_Size, 
                                              0 );
                if ( NULL_PTR == p_msg_ins )
                {
                   ncmcr2_log_message( 2, 1, "Error sending network status to monitor" , "PinnacleMsgHandler" , 0 ,"");
                }

 	             memset (ConnectionId, 0, sizeof (ConnectionId)) ;
 	             ReturnCode = ReadMemMonitorTable (ConnectionId) ;
 	             if (ConnectionId[0] != 0)
                {
                   ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
                   ret_code = pteipc_send( p_msg_ins, applnk_que_name );
                   if ( 0 > ret_code )
                   {
                      pteipc_get_errormsg( ret_code, err_mesg );
                      sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
                      ncmcr2_log_message( 3, 3, Buff , "PinnacleMsgHandler" , 0 ,"");
                    }
                }
                free( p_msg_ins );
                break;

            default:
               break;
         }  /* End switch on subtype */
         break;  /* End MT_SYS_MONITOR    */

      case MT_SYSTEM_REQUEST:
         ptesystm_system_msg_handler( p_msg_in );
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            if ( Tracing == TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = TRACE_OFF;
                  sprintf( Buff,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
                  ncmcr2_log_message( 1, 1, Buff , "PinnacleMsgHandler" , 0 ,"");
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( Buff,
                    "Pinged -> Network Controller Service: %s, version %s",
                     AppName, Version );
            ncmcr2_log_message( 1, 1, Buff , "PinnacleMsgHandler" , 0 ,"");
         }
         else if(ST1_LOAD_TO_REVERSAL_ATTEMPTS == ptemsg_get_msg_subtype1(p_msg_in) ||
        		 ST1_LOAD_LATE_RESP_REVERSAL_ATTEMPTS == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncmcr2_read_TO_config_details_from_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( Buff,
        	          "rvrsl_attmpts_value_fr_timeout_mc: %d, rvrsl_attmpts_value_fr_laterspns_mc %d",
					  rvrsl_attmpts_value_fr_timeout_mc, rvrsl_attmpts_value_fr_laterspns_mc );
        	 ncmcr2_log_message( 1, 1, Buff, "PingMsgHandler" ,0);
         }
      break;

      case MT_SYSTEM_REPLY: 
      break;

      case MT_TIMER_REPLY:
         /* Message to host has timed out */
         if ( PTEMSG_OK == ptemsg_get_result_code(p_msg_in) )
         {
            subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
            if ( ST1_TIMER_CLEAR == subtype1 )
            {
                incoming_host_message_timer_reply( p_msg_in );
            }
         }
         else
         {
            p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
            p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
            memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
            timer_result = ptemsg_get_result_code(p_msg_in);

            memset( err_mesg, 0x00, sizeof(err_mesg) );
            sprintf( err_mesg,
                    "Error from Timerds, result code = %c, stan = %s",
                     timer_result, auth_tx.TLF01_details.sys_trace_audit_num );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            memcpy( auth_tx.TLF01_details.response_text, err_mesg,
                          sizeof(auth_tx.TLF01_details.response_text)-1 );
            strcat(err_mesg,Log_Bufr);
            ncmcr2_log_message( 3, 2, err_mesg , "PinnacleMsgHandler" , 0 ,auth_tx.TLF01_details.card_num);
         }
      break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE:
         if ( OK != security_reply(p_msg_in) )
         {
            ncmcr2_log_message( 3, 2, "Error processing pin translation reply" , "PinnacleMsgHandler" , 0 ,"");
         }
         break;
      case MT_NSP_DECRYPT_TRANSLATE_PIN_RESPONSE:
    	  if ( OK == security_reply_for_decrypn_encrypn(p_msg_in))
    	  {
              ncmcr2_log_message( 3, 2, "Error processing pin decryption encryption translation reply" , "PinnacleMsgHandler" , 0 ,"");
          }
    	  break;
      case MT_NSP_DECRYPT_PIN_RESPONSE:
    	  if (OK == security_reply_for_decrypn(p_msg_in))
    	  {
              ncmcr2_log_message( 3, 2, "Error processing pin decryption translation reply" , "PinnacleMsgHandler" , 0 ,"");
          }
    	  break;

      case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
         break;

      default :
         ncmcr2_log_message( 3, 2, "Unknown message type received" , "PinnacleMsgHandler" , 0 ,"");
      break;
   }  /* switch msg_type */

} /* PinnacleMsgHandler */


/******************************************************************************
 *
 *  NAME:         MainProcessor
 *
 *  DESCRIPTION:  This is the driver function for the MasterCard host handler.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void MainProcessor()
{
   pPTE_MSG  p_msg;
   LONG      ret_code;
   CHAR      ErrorMsg[100] = {0};
   CHAR      Buffer[256]   = {0};
   INT       ret_val;
   INT      err_ctr = 0;


   GetAppName( AppName );
   strcpy( cur_app_name, AppName );
   sprintf( Buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   ncmcr2_get_error_warning_file_name_path();
   ncmcr2_log_message( 2, 1, Buffer , "MainProcessor" , 0 ,"");
   printf( "%s\n", Buffer );


   /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***************/
#ifdef _DEBUG   
   if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) ) 
#else
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName,"pte_ipc"))
#endif
   {
      ncmcr2_log_message( 3, 3, "Failed to create XIPC queues", "MainProcessor" , 0 ,"");

      /* Signal that we are ready to shut down and quit */
      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
      MainProcessDone = 1;
   }
   else
   {
      /* MOVE ALL APP SPECIFIC INITIALIZATION FROM YOUR MAIN FUNCTION HERE */

	  /* Read tf.ini for configuration details */
	   ncmcr2_read_tf_ini();
	   ncmcr2_read_TO_config_details_from_tf_ini();
	   ncmcr2_read_queue_name_for_0302_from_tf_ini();
	   ncmcr2_read_Online_PIN_flag_from_tf_ini();
      /* Perform Service initialization. */
      ret_val = StartUp();
      if ( OK == ret_val )
      {
         while( !EndProcessSignalled )
         {
            /* You are blocked here waiting for a message on either app queue
             * or control queue. If there is no message on either queue for 5
             * seconds, the blocking call returns.
             *
             * The following line will be used to get messages from the queue.
             * For now the message is automatically created here, but this will
             * be removed.
             */
            p_msg = pteipc_receive( application_que_name, control_que_name,
                                    5, &ret_code ); 

            if ( NULL_PTR != p_msg )
            {
               PinnacleMsgHandler( p_msg );
               free( p_msg );
            }
			else if ( QUE_ER_TIMEOUT != ret_code ) 
           {
               pteipc_get_errormsg( ret_code, ErrorMsg );
               sprintf( Buffer, "Error on pteipc_receive %s", ErrorMsg );
               ncmcr2_log_message( 3, 3, Buffer, "MainProcessor" , 0 ,"");

               /* There is a problem with the queue.  Delay 1 second
                * so as to not flood Applnk's queue and syslog.
                */
               #ifdef WIN32
                  Sleep( 1000 );
               #else
                  usleep( 1000000 );
               #endif

               /* Inform Operators if service's queue is no longer readable. */
               if ( ++err_ctr > 10 )
               {
                  err_ctr = 0;
                  sprintf( Buffer,
                          "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
                           ServiceName );
                  ncmcr2_log_message( 3, 3, Buffer, "MainProcessor" , 0 ,"");
                  #ifdef WIN32
                     Sleep (5000);
                  #else
                     usleep (5000000);
                  #endif
               }
            }
            
         }
      }

      /* Shutting down, need to change state before exiting Xipc. */
      write_to_shared_mem_table( AppName, "0", "0", DOWN,
                                 HOST_STATE_CHANGED, Buffer );
      send_network_status_to_monitor();

      /********** MODIFY THIS CALL IF IT IS A SINGLE INSTANCE APP ***********/
#ifdef _DEBUG
      pteipc_shutdown_single_instance_app(); 
#else
      pteipc_shutdown_multiple_instance_app();
#endif
      MainProcessDone = 1;
   }
}


/******************************************************************************
 *
 *  NAME:         EndProcess
 *
 *  DESCRIPTION:  This is the function used to clean up when the app is
 *                being exited.  All clean up code goes in here.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void EndProcess()
{
   CHAR Buffer[100] = "";

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         ncmcr2_log_message( 3, 1, Buffer, "EndProcess" , 0 ,"");
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncmcr2_log_message( 2, 1, Buffer, "EndProcess" , 0 ,"");
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}


/******************************************************************************
 *
 *  NAME:         StartUp
 *
 *  DESCRIPTION:  This function performs required initialization when the
 *                service starts up.  It creates the shared memory table, gets
 *                NCF01, and may perform auto logon.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT StartUp()
{
   CHAR		Buffer[256]={0};
   CHAR     err_msg[256]={0};
   AUTH_TX  auth_tx;
   INT      ret_val;
   INT      rc;
   BYTE     ncf01_ret_val;

   /* Calculate Data Structure sizes to save time during processing. */
   Tlf01_Size   = sizeof(TLF01);
   Ncf01_Size   = sizeof(NCF01);
   Ncf30_Size   = sizeof(NCF30);
   Auth_Tx_Size = sizeof(AUTH_TX);
   Mcc_Gui_Size = sizeof(MCC_ONLINE_UPDATES);
   Bdkkey01_Size = sizeof(BDKKEY01);

   /*  Create a table to be used for shared memory between multiple
    *  instances of this Network Control Module.  It contains three 
    *  fields:  # of consecutive timerouts, # of active transactions, 
    *  and the current state of the host.
    */
   ret_val = create_shared_mem_table( AppName, err_msg );
   if ( OK != ret_val )
   {
      ncmcr2_log_message( 3, 3, err_msg, "StartUp" , 0 ,"");
   }
   else
   {
      /* Get NCF01 record for switched out transactions. */
      memset(&ncf01_i, 0, Ncf01_Size);
      strcpy(ncf01_i.primary_key.network_id, AppName);
      ncf01_i.primary_key.network_type[0] = 'I';
      ncf01_ret_val = get_ncf01_cfg( &ncf01_i, err_msg);
      if ( PTEMSG_OK != ncf01_ret_val )
      {
         ret_val = NOT_OK;
         memset( Buffer, 0x00, sizeof(Buffer) );
         strcpy( Buffer, "Error on select of NCF01 issuer: " );
         strncat( Buffer, err_msg, 50 );
         ncmcr2_log_message( 3, 3, Buffer, "StartUp" , 0 ,"");
      }
      else
      {
         /* Get NCF01 record for switched in transactions. */
         memset(&ncf01_a, 0, Ncf01_Size);
         strcpy(ncf01_a.primary_key.network_id, AppName);
         ncf01_a.primary_key.network_type[0] = 'A';
         ncf01_ret_val = get_ncf01_cfg( &ncf01_a, err_msg);
         if ( PTEMSG_OK != ncf01_ret_val )
         {
            ret_val = NOT_OK;
            memset( Buffer, 0x00, sizeof(Buffer) );
            strcpy( Buffer, "Error on select of NCF01 acquirer: " );
            strncat( Buffer, err_msg, 50 );
            ncmcr2_log_message( 3, 3, Buffer, "StartUp" , 0 ,"");
         }
         else
         {
        	/* Get BDKkey01 details for Online PIN validation */
             memset(&Bdkkey01, 0, Bdkkey01_Size);
             strcpy(Bdkkey01.card_brand, "MC");
             ncf01_ret_val = get_bdkkey01_cfg( &Bdkkey01, err_msg);

            /* Initialize transaction statistics */
            (void)reset_stats( &Txn_Counts );

            /* Initialize configurable counters. */
            MaxActiveTxns     = atoi(ncf01_i.max_active_txns);
            MaxConsecTimeouts = atoi(ncf01_i.max_consecutive_timeouts);
            if(MaxConsecTimeouts < 0 || MaxConsecTimeouts > 50)
            {
            	MaxConsecTimeouts = 24;
            	ncmcr2_log_message( 2, 2, "Reading of max_consecutive_timeout failed and set to 24", "StartUp" , 0 ,"");
            }

            /* Determine if transaction tracing should be turned on. */
            rc = get_trace_ini_info( AppName, DirPath, err_msg );
            if ( rc == 0 )
            {
               Tracing = TRACE_ON;

               /* Open the transaction trace file. */
               memset( TraceFile, 0x00, sizeof(TraceFile) );

               pTraceFile_fp  = &TraceFile_fp;
               ppTraceFile_fp = &pTraceFile_fp;

               if (false == open_trace_file(AppName,DirPath,
                                            ppTraceFile_fp,TraceFile))
               {
                  Tracing = TRACE_OFF;
                  sprintf( err_msg,
                          "Unable to open trace file %s. Tracing is off",
                           TraceFile );
                  ncmcr2_log_message( 1, 1, err_msg, "StartUp" , 0 ,"");
               }
               else
            	   ncmcr2_log_message( 1, 1, "Tracing is turned ON", "StartUp" , 0 ,"");
            }
            else
            {
               Tracing = TRACE_OFF;
               if ( rc == -1 )
               {
                  ncmcr2_log_message( 1, 1, err_msg, "StartUp" , 0 );
               }
               ncmcr2_log_message( 1, 1, "Tracing is turned off", "StartUp" , 0 ,"");
            }

            /* Check the 'Auto Logon' flag. */
            ret_val = OK;
            if (0 == strcmp(ncf01_i.auto_logon,"1"))
            {
               /* Set up a logon request to the host in auth_tx */
               memset( &auth_tx, 0, Auth_Tx_Size );
               perform_host_logon_request( LOGON_REQUEST, SOURCE_IS_OPERATOR,
                                           &auth_tx);
            }
            if (0 == strcmp( "L", ncf01_i.pad_char ) )
            {
               ret_val = write_to_shared_mem_table( AppName, "0", "0", ONLINE,
                                                    HOST_STATE_CHANGED,err_msg);
            }
         }

         /* Set MCC Validation either On or Off. */
         MCC_Validation_Functionality = MCC_VALIDATION_OFF;
      }
   }
   return( ret_val );
}
