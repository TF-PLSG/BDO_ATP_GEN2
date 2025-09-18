/******************************************************************************
  
        Module: archlnk.c
   
         Title: archlnk
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ARCHLNK\ARCHLNK.C  $
   
  
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#ifdef WIN32
#include <process.h>
#else
#include <signal.h>
#endif

#include "pte.h"
#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptesystm.h"
#include "ptestats.h"
#include "ntutils.h"
#include "coreinfo.h"

#define COMMA ','

/* Extern definition section */
extern int  volatile EndProcessSignalled;
extern int  volatile MainProcessDone;
extern CHAR          ServiceName[12];

PRIVATE INT          connection_type;

INT                  system_message_priority;

PRIVATE LOCAL_DM_CONFIG     dm_config;

PRIVATE BYTE outgoing_buf[MAX_OUTGOING_BUFFER_SIZE];

#ifdef WIN32
_declspec(dllimport) XipcMaxThreads;
#else
extern XINT XipcMaxThreads;
#endif

CHAR   Version[] = "ATP_11.1.0";

/*************************************************************
 *************************************************************/
void SetStartupEnvironment()
{
   DWORD rc;
   CHAR sectionname  [] = "ARCHLNK";
   CHAR tmpfilename  [61];
   CHAR filename     [256];
   CHAR tmpstr       [256];
   
   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   strcpy(dm_config.db_subsystem_name  , "ARCHLNK");
   strcpy(dm_config.db_connection_type , "TCP/IP");
   strcpy(dm_config.db_tcp_socket_type , "SERVER");
   strcpy(dm_config.db_tpdu_processing , "WITHOUT");
   dm_config.db_tcp_header_length = 2;

   rc = GetPrivateProfileString(
      sectionname,	                  
      "ARCHLNK_TCP_PORT",                     
      "1450",     	                 
      tmpfilename,                    
      sizeof(tmpfilename) - 1,        
      filename                         
   );
   
   dm_config.db_tcp_port = atoi(tmpfilename);
}   /* SetStartupEnvironment() */

/*************************************************************
 *************************************************************/
void set_connection_status(INT status)
{
  connection_up = status;
}

/*************************************************************
 *************************************************************/
void terminate_connection()
{
  if (port_close) 
     port_close(connection_type);
  
  set_connection_status(FALSE);
}


/*****************************************************************/
/*****************************************************************/
INT dm_connection()
{
  BOOLEAN conn_successful = FALSE;
  CHAR    info_msg[256];

  if (dm_config.db_tcp_port <= 1024)
  {
    LogEvent("ASCENDENT: dm_connection, invalid socket port number, must be > 1024", ERROR_MSG);
  }
  else
  {
     conn_successful = tcp_convert((BYTE)connection_type, dm_config.db_tcp_port,
                                   dm_config.db_tcp_header_length, dm_config.db_host_name);
     sprintf(info_msg, "ASCENDENT: dm_connection, TCP/IP Connection using port number %0ld, IP Address %s",
                    dm_config.db_tcp_port, (strlen(dm_config.db_host_name) ? dm_config.db_host_name : "NONE"));

     LogEvent(info_msg, INFO_MSG);
  }

  set_connection_status (conn_successful);

  return(conn_successful);
}

/*************************************************************
 *************************************************************/
INT connection_is_up()
{
  return(connection_up);
}

/*************************************************************
 *************************************************************/
INT establish_connection()
{
  if (!dm_connection()) 
  {
    LogEvent("Ascendent: establish_connection, archlnk connection not established" , ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

/*************************************************************
 *************************************************************/
void MainProcessor()
{
  pPTE_MSG    p_msg;
  INT         write_status;
  LONG        ret_code, msg_len;
  IPC_INFO    ipc_info;
  pCHAR       orig_info, ptr, pptr = NULL;
  CHAR        AppName [8];
  CHAR        ErrorMsg[100];
  CHAR        Buffer  [256];
  INT		  secondaryMonitorSocket;

  XipcMaxThreads = 1024;
  sprintf( Buffer, "Starting ARCHLNK service, version %s", Version );
  LogEvent( Buffer, INFO_MSG );
  GetAppName( AppName );
  GetXipcInstanceName( Buffer );
  if( !pteipc_init_single_instance_app( ServiceName, Buffer ) ) 
  {
    LogEvent( "Ascendent: MainProcessor, failed to create XIPC queues",
               ERROR_MSG ); 
    EndProcessSignalled = 1;
  }
  else
  {          /* Successful XIPC instance started */
    ret_code = pteipc_quespool(application_que_name, application_que_name);
    if (ret_code < 0)
    {   
      sprintf( ErrorMsg,
               "Ascendent: MainProcessor, pteipc_quespool Failed, Queue: %s",
                application_que_name ) ;
      LogEvent(ErrorMsg, ERROR_MSG);
    }
    SetStartupEnvironment();
    connection_type = DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU;
    set_connection_status(FALSE);  
    establish_connection();
    memset(&ipc_info, 0, sizeof(IPC_INFO));
    ipc_info.messageheaderlength = dm_config.db_tcp_header_length;
    GetSystemMessagePriority(&system_message_priority);
  }         /* Successful XIPC instance started */

// Nirbhay - Ignoring SIGPIPE that is being received whenever there is a late
// write to a closed socket. archlnk was crashing on receiving SIGPIPE signal.
#ifdef SIGPIPE
	 signal( SIGPIPE, SIG_IGN );
#endif

  while(!EndProcessSignalled)
  {         /* Loop until process terminated */
    /* You are blocked here waiting for a message on either app queue or
       control que if there is no message on either que for 5 seconds,
       the blocking call returns */
    p_msg = pteipc_receive( application_que_name, control_que_name, 5,
                            &ret_code );
    if( p_msg != NULL_PTR )
    {       /* Valid message was retrieved */
      ptestats_accumulate_msg_stats(p_msg); /* JMG - this is a NULL routine */
      orig_info = ptemsg_get_msg_orig_comms_info(p_msg);
      /* JMG - Sanity check on orig_info */
      if ( 7 != strspn(orig_info, "archlnkA") )
      {     /* App queue name prefix is incorrect */
        sprintf( ErrorMsg,
                 "Ascendent: MainProcessor, App queue name error, Queue: %s",
                 orig_info ) ;
        LogEvent(ErrorMsg, ERROR_MSG);
        sprintf( Buffer,"Ascendent: MainProcessor, freeing %01ld",
                        (long) p_msg );
        LogEvent( Buffer, ERROR_MSG );
        free (p_msg); /* First free this bad message area */
        continue;     /* Then continue with next message on queue */
      }     /* App queue name prefix is incorrect */ 
      switch( ptemsg_get_msg_type( p_msg ) )
      {     /* Switch on message type */
        case MT_SYSTEM_REQUEST : 
          ptesystm_system_msg_handler(p_msg);
          break;
        default:       /* Not a system request */
          /* The format of orig_comms_info set in notify() is 
             archlnkA,3232 the port number is varying depending
             on the socket that received the transaction,
             that same socket will be used to send the response back
           */
          ptr = (pCHAR)strchr(orig_info, COMMA);
          if (ptr)
          {      /* Found comma before socket ID */
            ptr++; /* Point to first digit of port number */

			// If this is a message for ATP Monitor, there can be more than one
			// connections.
			pptr = (pCHAR) strchr ( ptr, COMMA);
			if (pptr)
			{				// SECONDARY MONITOR SOCKET CHECKS
				pptr++;
				// skip the redundant queue name archlnkA.
				pptr = (pCHAR) strchr ( pptr, COMMA);
				if (pptr)
				{
					pptr++;
					// Now pptr is pointing to the first digit of secondary read-only monitor connection socket. Validate it.
					if ( !isdigit((int) *pptr) )
            		{
						LogEvent("Ascendent: MainProcessor, Invalid secondary Monitor socket number",
								  ERROR_MSG);
						//break; // Nirbhay: Do not break here as primary socket might still be valid and can
						// still be used even though secondary socket is invalid.
						pptr = NULL;	// Do not send to secondary socket.
					}

					secondaryMonitorSocket = (SOCKET)(atol(pptr));
					/* Error check on socket number */
					if ( ERANGE == errno )
					{
						LogEvent("Ascendent: MainProcessor, secondary Monitor Socket number conv error",
						ERROR_MSG);
						//break;  // Nirbhay: Do not break here as primary socket might still be valid and can
						// still be used even though secondary socket is invalid.
						pptr = NULL;	// Do not send to secondary socket.
					}
					/* JMG - Range check on socket number */
					if ( (0 ==  secondaryMonitorSocket) ||
					(INT_MAX ==  secondaryMonitorSocket) ||
					(INT_MIN ==  secondaryMonitorSocket) )
					{
						LogEvent("Ascendent: MainProcessor, Range error on secondary Monitor socket number",
						ERROR_MSG);
						//break; // Nirbhay: Do not break here as primary socket might still be valid and can
							// still be used even though secondary socket is invalid.
						pptr = NULL;	// Do not send to secondary socket.
					}
				}
				else
					pptr = NULL;
			}
			else
				pptr = NULL;

			// PRIMARY SOCKET CHECKS

            /* JMG - Sanity check on socket number */
            if ( !isdigit((int) *ptr) )
            {
              LogEvent("Ascendent: MainProcessor, Invalid socket number",
                        ERROR_MSG);
              break; /* Abort write to socket */
            }
			
            ipc_info.socket_id = (SOCKET)(atol(ptr));
            /* JMG - Error check on socket number */
            if ( ERANGE == errno )
            {
              LogEvent("Ascendent: MainProcessor, Socket number conv error",
                        ERROR_MSG);
              break; /* Abort write to socket */
            }
            /* JMG - Range check on socket number */
            if ( (0 ==  ipc_info.socket_id) ||
                 (INT_MAX == ipc_info.socket_id) ||
                 (INT_MIN ==  ipc_info.socket_id) )
            {
              LogEvent("Ascendent: MainProcessor, Range error on socket number",
                        ERROR_MSG);
              break; /* Abort write to socket */
            }
            msg_len = ptemsg_get_msg_length(p_msg); 
            /* JMG - Error check on message length */
            if ( ERANGE == errno )
            {
              LogEvent("Ascendent: MainProcessor, Message length conv error",
                        ERROR_MSG);
              break; /* Abort write to socket */
            }
            /* JMG - Range check on message length */
            if ( (0 == msg_len) ||
                 (LONG_MAX == msg_len) ||
                 (LONG_MIN == msg_len) )
            {
              LogEvent("Ascendent: MainProcessor, Range error on msg length",
                        ERROR_MSG);
              break; /* Abort write to socket */
            }

			// Send to primary socket.
			printf("\nSend to primary socket.%d\n", ipc_info.socket_id);
			write_status = port_write((WORD)msg_len, p_msg, &ipc_info);
			if (!write_status)
				LogEvent("Ascendent: MainProcessor, ARCHLNK port_write failed",
				ERROR_MSG);

			// If secondary socket was found, broadcast the message to secondary connection too.
			// It will be a read-only Monitor connection.
			if ( pptr != NULL && secondaryMonitorSocket != ipc_info.socket_id) 
			{/* Checking, if promary and secondary sockets are same Don't send the message  */
				ipc_info.socket_id = (SOCKET)secondaryMonitorSocket;	// socket is already validated above.

				printf("\nSend to secondaryMonitorSocket .%d\n", ipc_info.socket_id);
				write_status = port_write((WORD)msg_len, p_msg, &ipc_info);
				if (!write_status)
					LogEvent("Ascendent: MainProcessor, ARCHLNK port_write failed on secondary monitor socket",
					ERROR_MSG);
			}
          }      /* Found comma before socket ID */
          else
          {      /* NO comma before socket ID */
            LogEvent("Ascendent: MainProcessor, ARCHLNK missing socket id",
                      ERROR_MSG);
          }      /* NO comma before socket ID */
      }     /* Switch on message type */
      /*
      sprintf( Buffer,"Ascendent: MainProcessor, freeing %01ld", (long) p_msg );
      LogEvent( Buffer, ERROR_MSG );
       */
      free (p_msg);
    }       /* Valid message was retrieved */
    else if( ret_code != QUE_ER_TIMEOUT ) 
    {       /* Error retrieving message from queue */
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "Ascendent: MainProcessor, error at pteipc_receive %s",
               ErrorMsg );
      LogEvent( Buffer, ERROR_MSG );
      if (ret_code == QUE_ER_NOTLOGGEDIN)
        break;
    }       /* Error retrieving message from queue */
    /* If the connection is down, and we're still supposed 
       to reconnect, then do it */
    if (!connection_up && !EndProcessSignalled)
    {
      terminate_connection();
      pteipc_sleep(3000);
      establish_connection();
    }
  }         /* Loop until process terminated */
   LogEvent("Ascendent: ARCHLNK service terminating XIPC connection", INFO_MSG);
   pteipc_shutdown_single_instance_app(); 
   MainProcessDone = 1;
}

/*************************************************************
 *************************************************************/
void EndProcess()
{
  CHAR Buffer[100] = "";

  LogEvent("Ascendent: ARCHLNK service terminating TCP connection", INFO_MSG);
  terminate_connection();
  LogEvent("Ascendent: ARCHLNK service terminating XIPC msg spooling", INFO_MSG);
  pteipc_quespool_off( application_que_name );
  sprintf( Buffer, "ARCHLNK service, version %s, is shutting down.", Version );
  LogEvent( Buffer, INFO_MSG );
}
