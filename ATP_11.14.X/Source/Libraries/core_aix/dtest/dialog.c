/******************************************************************************
  
        Module: xtest.c
   
         Title: xtest
  
   Description: 
			
			  test code for XCOM
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#else
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include "pte.h"
#include "convert.h"
#include "ptecom.h"
#include "pteipc.h"
#include "ptesystm.h"
#include "ptestats.h"
#include "ntutils.h"
#include "coreinfo.h"
#include "cdsapi.h"

/* Extern definition section */
extern int  volatile EndProcessSignalled;
extern int  volatile MainProcessDone;
extern CHAR          ServiceName[12];

#ifndef WIN32
extern int errno;
extern char *sys_errlist[];
#endif


PRIVATE INT          cur_tpdu = 0;
PRIVATE INT volatile received_disconnect_from_gui;
PRIVATE INT          tpdu_tran_len;
PRIVATE INT          tpdu_dst_len;
PRIVATE INT          tpdu_src_len;
PRIVATE INT          tpdu_length;
PRIVATE INT          connection_type;
PRIVATE INT          length_of_tpdu_scroll;
PRIVATE INT          length_of_router_scroll;

PRIVATE WORD         primary_tcp_port_number, secondary_tcp_port_number;
PRIVATE CHAR         primary_tcp_host_name[21], secondary_tcp_host_name[21];
BOOLEAN volatile     use_primary;

PRIVATE pTPDU_TABLE_LIST    db_tpdu_list;
PRIVATE pROUTER_TABLE_LIST  db_router_list;
PRIVATE pDM_CONFIG          db_dm_config;

PRIVATE LOCAL_DM_CONFIG     dm_config;
PRIVATE LOCAL_TPDU          tpdu_scroll   [MAX_CONFIG_ENTRY];
PRIVATE LOCAL_ROUTER_TABLE  router_scroll [MAX_CONFIG_ENTRY];

PRIVATE BYTE outgoing_buf[MAX_OUTGOING_BUFFER_SIZE];


/* Functions */
void send_confirm(BYTE msg_subtype, char * dest_queue, char *orig_info);
extern void initialize_activity_timer( pCHAR service_name );

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

  switch (connection_type)
  {
#ifdef WIN32
     case DM_PORTTYPE_SERIAL_WITH_TPDU:
     case DM_PORTTYPE_SERIAL_WITHOUT_TPDU:
     case DM_PORTTYPE_SERIAL_IGNORE_TPDU:
      conn_successful = serial_convert((BYTE)connection_type      , dm_config.db_com_port, 
                                       dm_config.db_baud_rate     , dm_config.db_byte_size, 
                                       dm_config.db_parity        , dm_config.db_stop_bits, 
                                       dm_config.db_tcp_header_length);
      LogEvent("ASCENDENT: dm_connection, Serial Connection", INFO_MSG);
      break;
#endif

    case DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
    case DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
    case DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
    case DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
    case DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
    case DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
      if ((primary_tcp_port_number <= 1024) || (secondary_tcp_port_number <= 1024))
      {
        LogEvent("ASCENDENT: dm_connection, invalid socket port number, must be > 1024", ERROR_MSG);
      }
      else
      {
         if (use_primary)
         {
            conn_successful = tcp_convert((BYTE)connection_type, primary_tcp_port_number,
                                          dm_config.db_tcp_header_length, primary_tcp_host_name);

            sprintf(info_msg, "ASCENDENT: dm_connection, TCP/IP Connection using port number %0ld, IP Address %s",
                           primary_tcp_port_number, 
                           (strlen(primary_tcp_host_name) ? primary_tcp_host_name : "NONE"));

         }
         else
         {
            conn_successful = tcp_convert((BYTE)connection_type, secondary_tcp_port_number,
                                          dm_config.db_tcp_header_length, secondary_tcp_host_name);

            sprintf(info_msg, "ASCENDENT: dm_connection, TCP/IP Connection using port number %0ld, IP Address %s",
                           secondary_tcp_port_number, 
                           (strlen(secondary_tcp_host_name) ? secondary_tcp_host_name : "NONE"));

         }

         LogEvent(info_msg, INFO_MSG);
      }
      break;

    default: 
      LogEvent("ASCENDENT: dm_connection, illegal value for connection type", ERROR_MSG);
      break;
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
    LogEvent("Ascendent: establish_connection, nac connection not established" , ERROR_MSG);
    return(FALSE);
  }

  return(TRUE);
}

/*************************************************************
 *************************************************************/
INT exists_in_tpdu_scroll(pCHAR inStr)
{
  CHAR tmp_buf[100];
  INT i;

  for (i = 0; i < length_of_tpdu_scroll; i++) 
  {
    if (strcmp(inStr, tpdu_scroll[i].tpdu_id) == 0)
    {
      cur_tpdu = i;
      return(TRUE);
    }
  }

  sprintf(tmp_buf, "Ascendent: exists_in_tpdu_scroll, cannot find tpdu_id<%s> ", inStr);
  LogEvent(tmp_buf, ERROR_MSG);
  return (FALSE);
}

/*************************************************************
 *************************************************************/
INT get_tpdu_length()
{
  return(tpdu_length);
}

/*************************************************************
 *************************************************************/
void refresh_dialog()
{
  if (strcmp(dm_config.db_connection_type, "SERIAL") == 0) 
  {
     if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
        connection_type = DM_PORTTYPE_SERIAL_WITH_TPDU;
     else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
        connection_type = DM_PORTTYPE_SERIAL_WITHOUT_TPDU;
     else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
        connection_type = DM_PORTTYPE_SERIAL_IGNORE_TPDU;
  }
  else 
  {
     if (strcmp(dm_config.db_tcp_socket_type, "SERVER") == 0)
     {
      if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
        connection_type = DM_PORTTYPE_TCP_SERVER_WITH_TPDU;
      else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
        connection_type = DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU;
      else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
        connection_type = DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU;
     }
     else
     {
      if (strcmp(dm_config.db_tpdu_processing, "WITH") == 0)
        connection_type = DM_PORTTYPE_TCP_CLIENT_WITH_TPDU;
      else if (strcmp(dm_config.db_tpdu_processing, "WITHOUT") == 0)
        connection_type = DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU;
      else if (strcmp(dm_config.db_tpdu_processing, "IGNORE") == 0)
        connection_type = DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU;
     }
  }

  if (exists_in_tpdu_scroll("TPDU_SIZE"))
  {
    tpdu_tran_len = tpdu_scroll[cur_tpdu].tran_len;
    tpdu_dst_len  = tpdu_scroll[cur_tpdu].dst_len;
    tpdu_src_len  = tpdu_scroll[cur_tpdu].src_len;
    tpdu_length   = tpdu_tran_len + tpdu_dst_len + tpdu_src_len;
  }
  else
  {
    tpdu_tran_len = 1;
    tpdu_dst_len  = 2;
    tpdu_src_len  = 2;
    tpdu_length   = 5;
  }
}

/*************************************************************
 *************************************************************/
INT update_dm_config()
{
  pCHAR  ptr;
  CHAR   err_msg[100];
  CHAR   temp[256];

  strcpy(db_dm_config->db_subsystem_name, ServiceName);

  if (cdsapi_get_dm_config(db_dm_config, err_msg) == PTEMSG_OK) 
  {
    /* copy the returned struct to locally defined struct */
    strcpy(dm_config.db_subsystem_name  , db_dm_config->db_subsystem_name);
    strcpy(dm_config.db_connection_type , db_dm_config->db_connection_type);
    strcpy(dm_config.db_com_port        , db_dm_config->db_com_port);
    strcpy(dm_config.db_baud_rate       , db_dm_config->db_baud_rate);
    strcpy(dm_config.db_byte_size       , db_dm_config->db_byte_size);
    strcpy(dm_config.db_parity          , db_dm_config->db_parity);
    strcpy(dm_config.db_stop_bits       , db_dm_config->db_stop_bits);

    /* 
      we will use Primary_tcp_port_number and secondary_tcp_port_number
      to support primary/secondary tcp connections
     */

    strcpy(temp, db_dm_config->db_tcp_port);
    ptr = (pCHAR)strchr(temp, ORIG_INFO_SEPARATOR);
    if (ptr)
    {
       *ptr = 0x00;
       primary_tcp_port_number   = atoi(temp);
       ptr++;
       secondary_tcp_port_number = atoi(ptr);
    }
    else
    {
       primary_tcp_port_number   = 
       secondary_tcp_port_number = atoi(temp);
    }
    
    strcpy(temp, db_dm_config->db_host_name);
    ptr = (pCHAR)strchr(temp, ORIG_INFO_SEPARATOR);
    if (ptr)
    {
       *ptr = 0x00;
       strcpy(primary_tcp_host_name, temp);
       ptr++;
       strcpy(secondary_tcp_host_name, ptr);
    }
    else
    {
       strcpy(primary_tcp_host_name, temp); 
       strcpy(secondary_tcp_host_name, temp); 
    }

    use_primary = TRUE;

    /* dm_config.db_tcp_port = atoi(db_dm_config->db_tcp_port); */
    /* strcpy(dm_config.db_host_name       , db_dm_config->db_host_name); */

    dm_config.db_tcp_header_length = atoi(db_dm_config->db_tcp_header_length);
    
    strcpy(dm_config.db_tcp_socket_type , db_dm_config->db_tcp_socket_type);
    strcpy(dm_config.db_tpdu_processing , db_dm_config->db_tpdu_processing);

    refresh_dialog();
    return(TRUE);
  }
  else
  {
    sprintf(temp, "Ascendent: update_dm_config, %s, failed - %s", ServiceName, err_msg);
    LogEvent(temp, ERROR_MSG);
    return(FALSE);
  }
}

/*************************************************************
 *************************************************************/
INT update_router()
{
  INT   count;
  CHAR  err_msg[100];
  CHAR  temp[150];

  if (cdsapi_get_router_table(db_router_list, err_msg) == PTEMSG_OK) 
  {
    //copy the list to locally defined struct
    length_of_router_scroll = db_router_list->num_returned;
    for (count = 0; count < length_of_router_scroll; count++)
    {
      strcpy(router_scroll[count].router_identifier, db_router_list->router_record[count].router_identifier);
      strcpy(router_scroll[count].router_queue, db_router_list->router_record[count].router_queue);
      strcpy(router_scroll[count].router_app_desc, db_router_list->router_record[count].router_app_desc);
      router_scroll[count].router_offset = atoi(db_router_list->router_record[count].router_offset);
      router_scroll[count].router_length = atoi(db_router_list->router_record[count].router_length);
      strcpy(router_scroll[count].router_datatype, db_router_list->router_record[count].router_datatype);
      strcpy(router_scroll[count].router_lo_end, db_router_list->router_record[count].router_lo_end);
      strcpy(router_scroll[count].router_hi_end, db_router_list->router_record[count].router_hi_end);
    }
    return(TRUE);
  }
  else
  {
    sprintf(temp, "Ascendent: update_router, failed - %s", err_msg);
    LogEvent(temp, ERROR_MSG);
    return(FALSE);
  }
}

/*************************************************************
 *************************************************************/
INT update_tpdu()
{
  INT   count;
  CHAR  err_msg[100];
  CHAR  temp[150];

  if (cdsapi_get_tpdu_table(db_tpdu_list, err_msg) == PTEMSG_OK) 
  {
    //copy the list to locally defined struct
    length_of_tpdu_scroll = db_tpdu_list->num_returned;
    for (count = 0; count < length_of_tpdu_scroll; count++)
    {
      strcpy(tpdu_scroll[count].tpdu_id, db_tpdu_list->tpdu_record[count].tpdu_id);
      tpdu_scroll[count].tran_len = atoi(db_tpdu_list->tpdu_record[count].tran_len);
      tpdu_scroll[count].dst_len = atoi(db_tpdu_list->tpdu_record[count].dst_len);
      tpdu_scroll[count].src_len = atoi(db_tpdu_list->tpdu_record[count].src_len);
      strcpy(tpdu_scroll[count].tpdu_string, db_tpdu_list->tpdu_record[count].tpdu_string);
    }
    return(TRUE);
  }
  else
  {
    sprintf(temp, "Ascendent: update_tpdu, failed - %s", err_msg);
    LogEvent(temp, ERROR_MSG);
    return(FALSE);
  }
}

/*************************************************************
 *************************************************************/
INT decode_tpdu(pPTE_MSG p_cur_msg, pBYTE tmpbuf, pCHAR dest_que_name, SOCKET *client_socket)      
{
  INT   total_tpdu_len, len1, len2, len3;
  pCHAR ptr;
  CHAR  tpdu[60], tmp_tpdu[60];

  strcpy(tmp_tpdu, ptemsg_get_msg_orig_comms_info(p_cur_msg));

  /* The format of orig_comms_info set in notify() is 
     ^6000010000,1234 or *dialogA,3232 the port number is varying depending
     on the socket that received the transaction, that same socket will be used
     to send the response back
     OR a tpdu id
  */
  ptr = (pCHAR)strchr(tmp_tpdu, ORIG_INFO_SEPARATOR);
  if (ptr)
  {
     *ptr = 0x00;
     ptr++;
     *client_socket = (SOCKET)(atoi(ptr));
  }
  else 
     *client_socket = 0;
  
  /* IMPORTANT:
     Org Info Starting with ORIGINAL_REQUEST means a response to a terminal request

     Org Info Starting with FORWARD_MESSAGE means data forward to Direct Host, 
     then use the rest of the entry as the destination queue name

     Org Info Starting with MSG_WITHOUT_TPDU means that this is message that is outgoing 
     and was forwarded to this application from dialog manager
  */
  switch(tmp_tpdu[0])
  {
  case ORIGINAL_REQUEST:
    ascii_to_bin(tmp_tpdu + 1, tpdu, tpdu_length);
    /* this is a response to a terminal request 
       Will Reverse TPDU only if it is a response going out to the terminal 
       Copy TPDU tran type, then reverse the src/dst 
    */
    memcpy(tmpbuf, tpdu, tpdu_tran_len);
    memcpy(tmpbuf + tpdu_tran_len, tpdu + tpdu_tran_len + tpdu_dst_len, tpdu_src_len);
    memcpy(tmpbuf + tpdu_tran_len + tpdu_dst_len, tpdu + tpdu_tran_len, tpdu_dst_len);
    return(tpdu_length);

  case MSG_WITHOUT_TPDU:
    /*  
        This is an outgoing message without a TPDU, send it as is
        Unless it is an IGNORE situation, in that case, add on a dummy
        5 byte TPDU starting with 60, we'll take the string associated with
        TPDU_SIZE in the TPDU table so that this string is configurable
    */
    if (strcmp(dm_config.db_tpdu_processing, "IGNORE"))
    {
      /* Not equal to IGNORE, process normally */
      return(SEND_MSG);
    }
    else
    {
      if (exists_in_tpdu_scroll("TPDU_SIZE"))
      {
         /* if it is with TPDU */
         /* Regular transaction through NAC */
         len1 = tpdu_scroll[cur_tpdu].tran_len;
         len2 = tpdu_scroll[cur_tpdu].dst_len;
         len3 = tpdu_scroll[cur_tpdu].src_len;
        
         /* Test if any of them is zero */
         if ((len1 * len2 * len3) != 0)  
         {
            total_tpdu_len = len1 + len2 + len3;
            ascii_to_bin (tpdu_scroll[cur_tpdu].tpdu_string, tmpbuf, total_tpdu_len);
            return(total_tpdu_len);
         }

         /* bad tpdu setup */
         LogEvent("Ascendent: decode_tpdu, tpdu length error" , ERROR_MSG);
         return(TPDU_DECODE_ERROR);
      }
      else
      {
         LogEvent("Ascendent, decode_tpdu, TPDU_SIZE missing from TPDU Table" , ERROR_MSG);
         return(TPDU_DECODE_ERROR);
      }
    }
    break;

  default:
    /* this is a PTE_MSG foreward to an external host auth */
    if (exists_in_tpdu_scroll(tmp_tpdu) == false)
    {
      LogEvent("Ascendent, decode_tpdu, tpdu id error" , ERROR_MSG);
      return(TPDU_DECODE_ERROR);
    }

    if (tpdu_scroll[cur_tpdu].tpdu_string[0] != FORWARD_MESSAGE)
    {
      /* if it is with TPDU */
      /* Regular transaction through NAC */
      len1 = tpdu_scroll[cur_tpdu].tran_len;
      len2 = tpdu_scroll[cur_tpdu].dst_len;
      len3 = tpdu_scroll[cur_tpdu].src_len;
        
      /* Test if any of them is zero */
      if ((len1 * len2 * len3) != 0)  
      {
        total_tpdu_len = len1 + len2 + len3;
        ascii_to_bin (tpdu_scroll[cur_tpdu].tpdu_string, tmpbuf, total_tpdu_len);
        return(total_tpdu_len);
      }

      /* bad tpdu setup */
      LogEvent("Ascendent: decode_tpdu, tpdu length error" , ERROR_MSG);
      return(TPDU_DECODE_ERROR);
    }
    else
    {
      /* Direct connection to Host, extract the queue name to be whatever
         after the FORWARD_MESSAGE, and send to that queue
      */
      strcpy(dest_que_name, &(tpdu_scroll[cur_tpdu].tpdu_string[1]));
      return(REROUTE_MESSAGE);
    }
  }   
}

/*************************************************************
 *************************************************************/
INT get_queue_name (pCHAR str, pCHAR dest_que)
{
   LONG  router_len;
   LONG  router_off;
   INT   index;
   INT   comp_int;
   INT   compare_ok = false;

   for (index = 0; index < length_of_router_scroll; index++) 
   {
      router_len = router_scroll [index].router_length;
      router_off = router_scroll [index].router_offset;

      if (router_scroll[index].router_datatype[0] == 'c')      //"char"
      {
         if ( (memcmp (&str[router_off], router_scroll[index].router_lo_end, router_len) >= 0) &&
              (memcmp (&str[router_off], router_scroll[index].router_hi_end, router_len) <= 0) )
            compare_ok= true;
      }
      else if (router_scroll[index].router_datatype[0] == 'i')   //"int"
      {
         comp_int = 0;
         memcpy (&comp_int, &str[router_off], router_len);

         if ( (comp_int >= atoi(router_scroll[index].router_lo_end)) && 
              (comp_int <= atoi(router_scroll[index].router_hi_end)))               
            compare_ok= true;
      }
      else 
         return (false);
    
      if (compare_ok)
      {
         strcpy (dest_que, router_scroll[index].router_queue);
         return (true);
      }
   }

   return (FALSE);
} 

/*************************************************************
 *************************************************************/
INT config_dialog()
{
  connection_type = -1;
  strcpy(trace_queue, DIALOG_GUI_IQUEUE);
  
  db_dm_config    = NULL;
  db_tpdu_list    = NULL;
  db_router_list  = NULL;
  db_dm_config    = (pDM_CONFIG)malloc(sizeof(DM_CONFIG));
  
  if (db_dm_config == NULL) 
    return (FALSE);
  
  db_tpdu_list    = (pTPDU_TABLE_LIST)malloc(sizeof(TPDU_TABLE_LIST));
  
  if (db_tpdu_list == NULL) 
  {
    free(db_dm_config);
    return (FALSE);
  }
  
  db_router_list  = (pROUTER_TABLE_LIST)malloc(sizeof(ROUTER_TABLE_LIST));
  
  if (db_router_list == NULL) 
  {
    free(db_dm_config);
    free(db_tpdu_list);
    return (FALSE);
  }
  
  if (!(update_router() && update_tpdu() && update_dm_config())) 
    return (FALSE);

  trace_number = 0;
  trace_on = FALSE;
  sprintf(trace_file, "%s.%03d", ServiceName, trace_number);
  received_disconnect_from_gui = FALSE;
  return (TRUE);
}

/*************************************************************
 *************************************************************/
void send_confirm(BYTE msg_subtype, char * dest_queue, char *orig_info)
{
  pPTE_MSG  p_msg_out;
  LONG      ret_code;
  CHAR      ErrorMsg[100];
  CHAR      Buffer  [256];

  p_msg_out = ptemsg_build_msg(MT_COM_REPLY,
                               msg_subtype,
                               0,
                               dest_queue,
                               application_que_name,
                               trace_file,
                               strlen(trace_file),
                               0);

   if ( p_msg_out == NULL_PTR )
   {
    LogEvent("Ascendent: send_confirm, insufficient memory to build confirm message to GUI" , ERROR_MSG);
    return;
   }

   ptemsg_set_orig_comms_info( p_msg_out, orig_info );
   ret_code = pteipc_send( p_msg_out, dest_queue );
   if( ret_code < 0 )
   {
    pteipc_get_errormsg( ret_code, ErrorMsg );
    sprintf(Buffer, "Ascendent: send_confirm to GUI failed: at pteipc_send to que %s: %s", dest_queue, ErrorMsg );
    LogEvent(Buffer, ERROR_MSG);
   }
   free( p_msg_out );
}

/*************************************************************
 *************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_cur_msg )
{
  INT       write_status, response_len, data_len;
  pCHAR     orig_queue, orig_info;
  FILE      *fptr;
  CHAR      ascii_tpdu      [20];
  CHAR      dest_queue_name [20];
  CHAR      log_dir         [100];
  IPC_INFO  ipc_info;


  ptestats_accumulate_msg_stats (p_cur_msg);
  orig_queue = ptemsg_get_msg_orig_queue(p_cur_msg);
  orig_info = ptemsg_get_msg_orig_comms_info(p_cur_msg);

   switch( ptemsg_get_msg_type( p_cur_msg ) )
   {
    case MT_OUTGOING:
      memset(&ipc_info, 0, sizeof(IPC_INFO));
      response_len = decode_tpdu(p_cur_msg, outgoing_buf, dest_queue_name, &(ipc_info.socket_id));

      ipc_info.messageheaderlength  = dm_config.db_tcp_header_length;
      ipc_info.connectiontype       = connection_type;

      if (response_len > 0)
      { 
        data_len = ptemsg_get_pte_msg_data_data_length(ptemsg_get_pte_msg_data(p_cur_msg)); 
        memcpy(outgoing_buf + response_len, ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_cur_msg)), data_len);
        response_len+= data_len;
        
        write_status = port_write((WORD)response_len, outgoing_buf, &ipc_info);                       
        
        if (write_status)
        {
          if (trace_on) 
          {
            memset(ascii_tpdu, 0, sizeof(ascii_tpdu));
            bin_to_ascii((pBYTE)outgoing_buf, (pBYTE)ascii_tpdu, get_tpdu_length());
            
            while (trace_lock_on)
               pteipc_sleep(100);

            trace_lock_on = TRUE;
            PrintBuffer(outgoing_buf, response_len, TRUE, ascii_tpdu, "dialog");
            trace_lock_on = FALSE;
          }
        }
        else              
        {
          LogEvent("Ascendent: PinnacleMSgHandler, dialog port_write failed", ERROR_MSG);
          /* We don't want to do that because there might be other connections established
          set_connection_status(FALSE);
          */
        }
      } 
      else
      {
        switch(response_len)
        {
        case REROUTE_MESSAGE:
          /* 
            Make sure we insert the correct indicator in the org_info field 
            so that we don't get into an infinite loop situation where the 
            org info is being decoded over and over again.this applied to 
            forwarding messages only.
          */
          ptemsg_set_orig_comms_info(p_cur_msg, MSG_WITHOUT_TPDU_IND);
          pteipc_send(p_cur_msg, dest_queue_name);
          
          if (trace_on) 
          {
            while (trace_lock_on)
               pteipc_sleep(100);
            
            trace_lock_on = TRUE;
            PrintBuffer(ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_cur_msg)), 
                        ptemsg_get_pte_msg_data_data_length(ptemsg_get_pte_msg_data(p_cur_msg)), 
                        TRUE, dest_queue_name, "dialog");
            trace_lock_on = FALSE;
          }

          break;

        case SEND_MSG:
          data_len = ptemsg_get_pte_msg_data_data_length(ptemsg_get_pte_msg_data(p_cur_msg)); 
          memcpy(outgoing_buf, ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_cur_msg)), data_len);
        
          write_status = port_write((WORD)data_len, outgoing_buf, &ipc_info);                       
        
          if (write_status)
          {
            if (trace_on) 
            {
               while (trace_lock_on)
                  pteipc_sleep(100);
               trace_lock_on = TRUE;
               PrintBuffer(outgoing_buf, data_len, TRUE, "NO TPDU", "dialog");
               trace_lock_on = FALSE;
            }
          }
          else              
          {
            LogEvent("Ascendent: PinnacleMSgHandler, dialog port_write failed", ERROR_MSG);
            /* We don't want to do that because there might be other connections established
            set_connection_status(FALSE);
            */
          }
          break;

        default:
          LogEvent("Ascendent: PinnacleMsgHandler, tpdu decoding failed", ERROR_MSG);
          break;
        }
      }
      break;


    case MT_COM_REQUEST:
      switch (ptemsg_get_msg_subtype1( p_cur_msg ))
      {
        case ST1_STATUS:
          strcpy(gui_orig_info, orig_info);
          if (connection_is_up())
          {
            if(trace_on) 
              send_confirm(ST1_CONNECTED_TRACE, orig_queue, orig_info);
            else 
              send_confirm(ST1_CONNECTED_NOTRACE, orig_queue, orig_info);
          }
          else
            send_confirm(ST1_DISCONNECT_CONFIRM, orig_queue, orig_info);
          break;

        case ST1_DISCONNECT:
          received_disconnect_from_gui = TRUE;
          trace_on = FALSE;
          if(connection_is_up()) 
            terminate_connection();
          break;

        case ST1_CONNECT:
          received_disconnect_from_gui = FALSE;
          if (!connection_is_up())
          {
            if (establish_connection())
              send_confirm(ST1_CONNECT_CONFIRM, orig_queue, orig_info);
            else
              send_confirm(ST1_DISCONNECT_CONFIRM, orig_queue, orig_info);
          }
          else 
            send_confirm(ST1_CONNECT_CONFIRM, orig_queue, orig_info);
          break;

        case ST1_TRACE_ON:
          trace_on = TRUE;
          trace_number++;
          if (trace_number > 99) 
            trace_number = 1;
      
          sprintf(trace_file, "%s.%03d", ServiceName, trace_number);

          strcpy(trace_queue, orig_queue);
          strcpy(gui_orig_info, orig_info);
          send_confirm(ST1_TRACE_ON_CONFIRM, orig_queue, orig_info);
          GetPinnacleLogDirectory(log_dir);

          strcat(log_dir, trace_file);
          if ((fptr = fopen(log_dir,"w")) != NULL)
          {
            fputs("  \n", fptr);
            fclose(fptr);
          }
          break;

        case ST1_EXIT:
          trace_on = FALSE;
          break;

        case ST1_TRACE_OFF:
          trace_on = FALSE;
          send_confirm(ST1_TRACE_OFF_CONFIRM, orig_queue, orig_info);
          break;

        default :
          LogEvent( "Ascendent: PinnacleMsgHandler, MT_COM_REQUEST, unknown msg_subtype received", ERROR_MSG );
          break;
      }  /* case MT_COM_REQUEST */
      break;

    case MT_UPDATE_TABLE:
      switch (ptemsg_get_msg_subtype1( p_cur_msg ))
      {
        case ST1_DMCONFIG:
          update_dm_config();
          break;

        case ST1_TPDU:
          update_tpdu();
          break;

        case ST1_ROUTER:
          update_router();
          break;

        default :
          LogEvent( "Ascendent: PinnacleMsgHandler, MT_UPDATE_TABLE, unknown msg_subtype received", ERROR_MSG );
          break;
      }
      break;

    case MT_SYSTEM_REQUEST : 
      ptesystm_system_msg_handler(p_cur_msg);
      break;

    default :
      LogEvent( "Ascendent: PinnacleMsgHandler, unknown msg_type received", ERROR_MSG );
      break;
   }     /* switch msg_type */
}

#ifdef WIN32

#define OS_SOCKET_ERROR   SOCKET_ERROR
#define OS_INVALID_SOCKET INVALID_SOCKET

#else

#define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF))
#define LOBYTE(w)   ((BYTE) (w))
#define OS_SOCKET_ERROR   -1
#define OS_INVALID_SOCKET -1

#endif

#define SIMULTANEOUS_POS_CONNECTIONS 1.0
#define TPS_RATE 2.0

// Number of milliseconds each thread will sleep, before sending the next transaction.
#define NO_OF_MILLISEC_BETWEEN_SUCCESSIVE_TXNS (1000.0 * (SIMULTANEOUS_POS_CONNECTIONS / TPS_RATE))

unsigned char devmerch[(int)SIMULTANEOUS_POS_CONNECTIONS][23] =
		{
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x34""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x35""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x36""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x37""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x38""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x39""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x31""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x34""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x35""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x36""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x37""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x38""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x31""\x39""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x31""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x34""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x35""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x36""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x37""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x38""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x39""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31",
			"\x30""\x30""\x30""\x30""\x30""\x30""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31"
		};

volatile int tcount = 0;

void sendHandler(int *tlevel)
{
	char inst_name[256];
	pPTE_MSG  p_msg_out;
	CHAR      Buffer[256], ErrorMsg[100];
	LONG      ret_code;
	CHAR      dest_que[17] = "dcpisoA";
	CHAR      szBuff[256];
	int newlevel;
	INT ret;
	LONG stanInt = 0;

#ifdef WIN32
	PRIVATE HANDLE            thandle;
	DWORD	   lthread_id;
#else
	PRIVATE pthread_t tThread;
#endif
	// Sale transaction.
	unsigned char txnToSend[] = "\x02""\x00""\x70""\x24""\x05""\x80""\x00""\xC0""\x00""\x00""\x16""\x41""\x77""\x77""\x77""\x77""\x77""\x77""\x74""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x01""\x00""\x11""\x11""\x14""\x07""\x10""\x00""\x10""\x00""\x03""\x00""\x33""\x30""\x30""\x30""\x30""\x30""\x30""\x32""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x30""\x31";

	// log into XCOM/XIPC, send a transaction to ATP, increment
	// thread-count, create another thread until MainProcessDone
	// is not 1

	tcount++;	// number of threads started.
	printf ("Thread started - level = %d, count = %d.\n", *tlevel, tcount);
	newlevel = (*tlevel) + 1 ;

	// Now launch next thread.
	if ( newlevel < SIMULTANEOUS_POS_CONNECTIONS )
	{
		// Start another thread. Total 2^10 threads will try to
		// simultaneously log into XCOM/XIPC.
#ifdef WIN32
		/* create thread to listen on the socket */
		thandle = CreateThread(  NULL,
							   0,
							   (LPTHREAD_START_ROUTINE)sendHandler,
								 &newlevel,
							   0,
							   &lthread_id );
		if( thandle == NULL )
		{
			LogEvent( "XCOM: dtest: send thread, Unable to create thread", ERROR_MSG );
			pteipc_shutdown_single_instance_app(); 
			MainProcessDone = 1;
			return;
		}
#else
		ret = pthread_create( &tThread, NULL, (void*(*)(void*))sendHandler, (void*)&newlevel );
		if( ret != 0 )
		{
			sprintf(szBuff,
			"XCOM: dtest: send thread, Unable to create thread, %d %s\n", errno, sys_errlist[errno] );
			LogEvent(szBuff, ERROR_MSG);
			pteipc_shutdown_single_instance_app(); 
			MainProcessDone = 1;
			return;
		}
#endif

	}

	// Format XCOM/XIPC login instance name
	pteipc_format_xipc_instance_name(inst_name);
	// Log into XCOM/XIPC
	if(thread_logon(inst_name, "dtest") > -1)
	{
		// Modify device/merchant depending on the 'newlevel' (i.e. thread number)
		// This is not required to be changed in the loop inside.
		memcpy(&txnToSend[38], devmerch [ newlevel - 1 ], 23);

		do		// START SEND LOOP...
		{
			// Modify STAN in txnToSend here
			stanInt++;
			if ( stanInt > 999999 )
				stanInt = 1;
//			txnToSend[28] = ( stanInt / 10000 ) % 100 ;
//			txnToSend[29] = ( stanInt / 100 ) % 100 ;
//			txnToSend[30] = stanInt % 100 ;
			txnToSend[28] = (((stanInt/10000) % 100) %10) +((((stanInt/10000) % 100)/10)*16);
			txnToSend[29] = (((stanInt/100) % 100)%10) +  ((((stanInt/100) % 100)/10)*16) ;
			txnToSend[30] = ((stanInt % 100)%10) + (((stanInt % 100)/10)*16) ;
			// send a transaction (from txnToSend buffer).
			p_msg_out = ptemsg_build_msg( MT_INCOMING,
										  0,
										  1,
										  "dcpisoA",
										  application_que_name,
										  txnToSend,
										  (int)sizeof(txnToSend),
										  0);

			if (p_msg_out != NULL_PTR)
			{
				sprintf(Buffer, "6000010002,3");	// dummy tpdu and socket
				ptemsg_set_orig_comms_info(p_msg_out, Buffer);

				/* If the buffer contains an IP address then include it */
				ptemsg_set_last_msg_tran_id ( p_msg_out, "127.0.0.1" );	// hard code IP

				ret_code = pteipc_send( p_msg_out, dest_que );

				free(p_msg_out);

				if( ret_code < 0 )
				{
					pteipc_get_errormsg( ret_code, ErrorMsg );
					sprintf( Buffer, "XCOM: dtest: pteipc_send, error at sending to que %s: %s", dest_que, ErrorMsg );
					LogEvent( Buffer, ERROR_MSG );
					printf( "XCOM: dtest: pteipc_send, error at sending to que %s: %s", dest_que, ErrorMsg );
				}
			}
			else
			{
				LogEvent("XCOM: dtest: out of memory when creating new message", ERROR_MSG);
			}

			// Now wait until NO_OF_MILLISEC_BETWEEN_SUCCESSIVE_TXNS milliseconds to
			// generate a TPS rate of 'TPS_RATE' transactions per second coming from
			// 'SIMULTANEOUS_POS_CONNECTIONS' simultaneous POS connections.
			{
				useconds_t usecs = (useconds_t)((NO_OF_MILLISEC_BETWEEN_SUCCESSIVE_TXNS) * 1000.0);
	#ifdef WIN32
			Sleep((DWORD)(NO_OF_MILLISEC_BETWEEN_SUCCESSIVE_TXNS));
	#else
			//usleep((useconds_t)((NO_OF_MILLISEC_BETWEEN_SUCCESSIVE_TXNS) * 1000));
			usleep(usecs);
	#endif
			}

			// Wait until it is time to exit.
		}while ( MainProcessDone != 1 );

		thread_logoff();
	}
}

CHAR       AppName [8];

/*************************************************************
 *************************************************************/
void MainProcessor()
{
	pPTE_MSG   p_msg;
	LONG       ret_code;
	CHAR       ErrorMsg[100];
	CHAR       Buffer  [256];
	CHAR      szBuff[256];
	INT ret;
	int ptlevel = 0;
#ifdef WIN32
	PRIVATE HANDLE            thandle;
	DWORD	   lthread_id;
#else
	PRIVATE pthread_t tThread;
#endif

	LogEvent("XCOM: dtest starts", INFO_MSG);

	GetAppName( AppName );
	GetXipcInstanceName( Buffer );

	if( !pteipc_init_single_instance_app( ServiceName, Buffer ) ) 
	{
		LogEvent( "Ascendent: MainProcessor, failed to create XIPC queues", ERROR_MSG );

		pteipc_shutdown_single_instance_app(); 
		MainProcessDone = 1;
		return;
	}

	// create a sending thread
#ifdef WIN32
	/* create thread to listen on the socket */
	thandle = CreateThread(  NULL,
                           0,
                           (LPTHREAD_START_ROUTINE)sendHandler,
		                     &ptlevel,
                           0,
                           &lthread_id );
	if( thandle == NULL )
	{
		LogEvent( "XCOM: dtest: send thread, Unable to create thread", ERROR_MSG );
		pteipc_shutdown_single_instance_app(); 
		MainProcessDone = 1;
		return;
	}
#else
	ret = pthread_create( &tThread, NULL, (void*(*)(void*))sendHandler, (void*)&ptlevel );
	if( ret != 0 )
	{
		sprintf(szBuff,
		"XCOM: dtest: send thread, Unable to create thread, %d %s\n", errno, sys_errlist[errno] );
		LogEvent(szBuff, ERROR_MSG);
		pteipc_shutdown_single_instance_app(); 
		MainProcessDone = 1;
		return;
	}
#endif


	while(!EndProcessSignalled)
	{
		/* You are blocked here waiting for a message on either app queue or control que  */
		/* if there is no message on either que for 5 seconds, the blocking call returns   */
		p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code );

		if( p_msg != NULL_PTR )
		{
			// ignore the responses.
			free (p_msg);
		}
		else if( ret_code != QUE_ER_TIMEOUT ) 
		{
			pteipc_get_errormsg( ret_code, ErrorMsg );
			sprintf( Buffer, "XCOM: dtest, error at pteipc_receive %s", ErrorMsg );
			LogEvent( Buffer, ERROR_MSG );
		}
	}

	pteipc_shutdown_single_instance_app(); 
	MainProcessDone = 1;
	sleep(30);
}

/*************************************************************
 *************************************************************/
void EndProcess()
{
	received_disconnect_from_gui = TRUE;
	terminate_connection();

	if (db_dm_config) free(db_dm_config);
	if (db_tpdu_list) free(db_tpdu_list);
	if (db_router_list) free(db_router_list);

	pteipc_quespool_off( application_que_name );

	LogEvent("Ascendent: dialog manager service is shutting down", INFO_MSG);
}
