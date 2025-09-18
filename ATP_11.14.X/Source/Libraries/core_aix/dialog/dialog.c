/******************************************************************************
  
        Module: dialog.c
   
         Title: dialog manager
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\DIALOG\COMMS\DIALOG.C  $
   
      Rev 1.37   Jan 21 2000 11:03:24   MSALEH
   remove unix specific code to retrieve xipc
   messages pending further research
   
      Rev 1.36   Jan 18 2000 12:12:24   MSALEH
   call pteipc_receive_r() on Unix to prevent
   from blocking forever
   
      Rev 1.35   Nov 09 1999 16:27:28   MSALEH
   * Wait 5 seconds after establishing a connection
   to give threads enough time to initialize
   * if we fail on a write, then log an error, but
   do not re establish the connection because
   there might be other sockets connected.
   
      Rev 1.34   Oct 07 1999 11:15:02   MSALEH
   properly save the gui org info
   
      Rev 1.33   Oct 06 1999 11:32:14   MSALEH
   use pteipc_sleep() instead of sleep()
   
      Rev 1.32   Sep 30 1999 16:04:00   MSALEH
   AIX Modifications
   Use #defines instead of literals
   
      Rev 1.31   Jun 18 1999 11:18:56   rmalathk
   changes made to add 'Activity Timer' functionality.
   
      Rev 1.30   Apr 19 1999 17:16:22   MSALEH
   Handle IGNORE on messages originating from
   the system, will use the string associated with
   TPDU_SIZE in the TPDU table to use in front
   of the message
   
      Rev 1.29   Apr 13 1999 14:02:10   MSALEH
   Add mods to handle IGNORE tpdu option
   
      Rev 1.28   Apr 06 1999 16:19:20   MSALEH
   Correct bug with forwarding messages to
   another hostcon application
   
      Rev 1.27   Apr 01 1999 12:23:48   MSALEH
   modifuy code to conform to new library routines
   
      Rev 1.26   Feb 03 1999 16:03:50   MSALEH
   When GUI exists, the service should not disconnect
   also, close all traces before exiting the service
   without prompting the user
   
      Rev 1.25   Oct 14 1998 10:27:46   MSALEH
   add support for secondary IP address/port
   number, applies to TCP/IP only
   
      Rev 1.24   Oct 14 1998 16:50:32   MSALEH
   when a connection failure is detected
   force a disconnect then reconnect
   
      Rev 1.23   Oct 13 1998 14:46:10   MSALEH
   1) Don't send confirm for a DISCONNECT
   2) Process ST1_EXIT
   
      Rev 1.22   Oct 09 1998 16:44:54   MSALEH
   1) When forwarding a message, null out
   the org_info field so that it is not decoded 
   again
   2) do not use strupr because it changes
   the case of queue names resulting in errors
   in the event viewer
   
      Rev 1.21   Oct 09 1998 11:11:30   MSALEH
   use strcmp instead of strncmp
   because WITH,WITHOUT share the same 
   first 4 letters
   
      Rev 1.20   Oct 08 1998 16:38:04   MSALEH
   1) Change "PINNACLE" to "ASCENDENT"
   2) Add new fields to dm_config
       db_tcp_socket_type
       db_tcp_headet_length
       db_tpdu_processing
   3) delete the field db_host_tcp_port from dm_config
   4) Restructure ptecom.lib/coreds/pindlg/dialog
   to be more configurable
   5) Update GUI to reflect new fields
   6) Rework the functionality of the connection
   spy to by included within the main loop
   7) Client connect tries only once then return
   and tries again after a timeout value, this will
   prevent the Client Connection from hanging during connect
   8) Streamline functions/variables
   9) All character comparisons are case insensitive 
   
   
      Rev 1.19   Sep 04 1998 18:29:14   skatuka2
   1.changed 'Pinnacle' to 'Ascendent'
   
      Rev 1.18   Sep 03 1998 08:21:16   drodden
   Changed disconnet-from-gui variable to be volatile because multiple 
   threads access and/or modify it.
   
   
      Rev 1.17   Aug 26 1998 09:41:18   MSALEH
   added another state to the COMM setup 
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.16   19 Aug 1998 15:10:18   rmalathk
   1. modified the send_confirm function to
       return the orig_comms_info field in the
       reply message.
   2. All references to hard-coded GUI queue
       names have been taken out.
   
      Rev 1.15   07 May 1998 15:52:44   MSALEH
   Send ST1_CONNECT_CONFIRM when
   DM receives a CONNECT COMMAND
   
      Rev 1.14   01 May 1998 11:26:00   MSALEH
   Use new msgtype, msgsubtype definitions
   
      Rev 1.13   28 Apr 1998 10:54:34   MSALEH
   1) Use common message types
   2) Modify to work with one gui
   
      Rev 1.12   Apr 10 1998 10:25:56   drodden
   Changed the defines for the dialog/hstcon queues to be uppercase
   to follow the Ascendent standard for defines/type definitions.

      Rev 1.11   Apr 03 1998 09:22:06   IGOLDFIE
   Changed message types because it has been changed in PTE.H
   
      Rev 1.10   30 Mar 1998 16:45:06   MSALEH
   added mods to handle routing transactions to
   a direct host connection in the following manner:
   in the tpdu table, if the tpdu string is preceeded
   by '~' then dialog manager will take what 
   follows as the destination queue name, and
   will forward the message to that queue name,
   the queue name is case sensitive.
   
      Rev 1.9   Mar 26 1998 10:34:50   drodden
   Moved all of the system wide message type definitions to PTE
   header.  This includes the dialog/hostcon/comms message types.
   Moved the common CONVERT module into the comm library.
   
   
      Rev 1.8   Mar 26 1998 09:45:32   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types wherever possible.
   
   
      Rev 1.7   17 Mar 1998 14:05:26   MSALEH
   Use the new ptecom.lib library routines,
   all comm apps are to use one convert.c
   one dialog-h and one dialogg-h, these three
   files can be found in the PTE - Common Comm files
   project
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
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

CHAR        Version[] = "ATP_11.1.0";

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
   BYTE  str_nii[5]={0};
   INT   compare_ok = false;

   for (index = 0; index < length_of_router_scroll; index++) 
   {
      router_len = router_scroll [index].router_length;
      router_off = router_scroll [index].router_offset;

       if (router_scroll[index].router_datatype[0] == 'i')   //"int"
        {
               comp_int = 0;
               if(router_len > 4)
               {
            	   // Ideally GUI should validate the len of router, should not be more than 4
            	   // if more than 4, default to dcpisoA
            	   LogEvent("Ascendent: get_queue_name, Router length is more than 4 , default to DCPISO module" , ERROR_MSG);
            	   strcpy (dest_que, "dcpisoA",7);
            	   return (true);
               }
               else
               {
            	   memcpy (&str_nii, &str[router_off], router_len);
               }
               comp_int = atoi(str_nii);

               if ( (comp_int >= atoi(router_scroll[index].router_lo_end)) &&
                    (comp_int <= atoi(router_scroll[index].router_hi_end)))
                  compare_ok= true;
        }

      else if (router_scroll[index].router_datatype[0] == 'c')      //"char"
      {
         if ( (memcmp (&str[router_off], router_scroll[index].router_lo_end, router_len) >= 0) &&
              (memcmp (&str[router_off], router_scroll[index].router_hi_end, router_len) <= 0) )
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

/*************************************************************
 *************************************************************/
void MainProcessor()
{
   pPTE_MSG   p_msg;
   LONG       ret_code;
   CHAR       AppName [8];
   CHAR       ErrorMsg[100];
   CHAR       Buffer  [256];

  LogEvent("Ascendent: MainProcessor dialog manager service starts", INFO_MSG);

#ifdef WIN32
pteipc_sleep(30);
#else
	 sleep(30);
#endif

  GetAppName( AppName );
  GetXipcInstanceName( Buffer );

  if( !pteipc_init_single_instance_app( ServiceName, Buffer ) ) 
  {
    LogEvent( "Ascendent: MainProcessor, failed to create XIPC queues", ERROR_MSG );

    pteipc_shutdown_single_instance_app(); 
    MainProcessDone = 1;
    return;
  }

  set_connection_status(FALSE);  
  if (!config_dialog())
  {
    LogEvent("Ascendent: MainProcessor, retrieving dialog configuration from database failed, service not started", ERROR_MSG);
    pteipc_shutdown_single_instance_app(); 
    MainProcessDone = 1;
    return;
  }

  initialize_activity_timer( ServiceName );

  establish_connection();

  ret_code = pteipc_quespool(application_que_name, application_que_name);

  if (ret_code < 0)
  {
    sprintf( ErrorMsg, "Ascendent: MainProcessor, pteipc_quespool Failed, Queue: %s", application_que_name ) ;
    LogEvent(ErrorMsg, ERROR_MSG);
  }

// Nirbhay - Ignoring SIGPIPE that is being received whenever there is a late
// write to a closed socket. Applnk was crashing on receiving SIGPIPE signal.
#ifdef SIGPIPE
         signal( SIGPIPE, SIG_IGN );
#endif

  while(!EndProcessSignalled)
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for 5 seconds, the blocking call returns   */
      p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code );
  
      if( p_msg != NULL_PTR )
      {
        PinnacleMsgHandler( p_msg );
        free (p_msg);
      }
      else if( ret_code != QUE_ER_TIMEOUT ) 
      {
        pteipc_get_errormsg( ret_code, ErrorMsg );
        sprintf( Buffer, "Ascendent: MainProcessor, error at pteipc_receive %s", ErrorMsg );
        LogEvent( Buffer, ERROR_MSG );
      }

      /* 
         if the connection is down, and we're still supposed 
         to reconnect, then do it
      */
      if (!connection_up && !EndProcessSignalled && !received_disconnect_from_gui)
      {
         terminate_connection();
         if (use_primary)
            use_primary = FALSE;
         else
            use_primary = TRUE;
         establish_connection();
         /* give it enough time to restart properly */
         pteipc_sleep(5000);
      }
   }

   pteipc_shutdown_single_instance_app(); 
   MainProcessDone = 1;
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
