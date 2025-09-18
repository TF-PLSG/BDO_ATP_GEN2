/******************************************************************************
  
        Module: coreinfo.h
  
         Title: Common Pinnacle PTE Table Definitions
  
   Description:

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\COREINFO.H  $
   
      Rev 1.6   Oct 14 1999 07:24:08   MSALEH
   integrate SSL modifications
   
      Rev 1.5   Aug 25 1999 10:33:40   MSALEH
   removed mods for router_extended comms
   
      Rev 1.4   Jul 01 1999 13:53:58   MSALEH
   added extend comms to router table
   
      Rev 1.3   Oct 14 1998 10:26:48   MSALEH
   increase size of db_host_name to 41 bytes
   to accomodate a secondary IP address
   
      Rev 1.2   12 Oct 1998 15:52:14   rmalathk
   changed all the 'num_returned' declarations
   from INT to BYTE for Unix compatibility.
   
      Rev 1.1   Oct 08 1998 16:35:44   MSALEH
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
   
   
      Rev 1.0   Apr 23 1998 13:57:02   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#ifndef COREINFO_H
#define COREINFO_H

/*
 * 
 */
typedef struct
{
   BYTE db_subsystem_name    [21];
   BYTE db_connection_type   [11];
   BYTE db_com_port          [11];
   BYTE db_baud_rate         [11];
   BYTE db_byte_size         [11];
   BYTE db_parity            [11];
   BYTE db_stop_bits         [11];
   BYTE db_tcp_port          [11];
   BYTE db_host_name         [41];
   BYTE db_tcp_header_length [11];
   BYTE db_tcp_socket_type   [11];
   BYTE db_tpdu_processing   [11];
}DM_CONFIG, *pDM_CONFIG;


typedef struct
{
   BYTE       num_returned;
   DM_CONFIG dm_config_record [DIALOG_LIST_SIZE];
}DM_CONFIG_LIST, *pDM_CONFIG_LIST;



typedef struct
{
   BYTE router_identifier    [17];
   BYTE router_queue         [11];
   BYTE router_app_desc      [21];
   BYTE router_offset        [11];
   BYTE router_length        [11];
   BYTE router_datatype      [5];
   BYTE router_lo_end        [11];
   BYTE router_hi_end        [11];
}ROUTER_TABLE, *pROUTER_TABLE;


typedef struct
{
   BYTE          num_returned;
   ROUTER_TABLE router_record [ROUTER_LIST_SIZE];
}ROUTER_TABLE_LIST, *pROUTER_TABLE_LIST;


typedef struct
{
   BYTE tpdu_id              [16];
   BYTE tran_len             [11];
   BYTE dst_len              [11];
   BYTE src_len              [11];
   BYTE tpdu_string          [51];
}TPDU, *pTPDU;


typedef struct
{
   BYTE  num_returned;
   TPDU tpdu_record [TPDU_LIST_SIZE];
}TPDU_TABLE_LIST, *pTPDU_TABLE_LIST;

#ifdef SSL_SUPPORT
typedef struct
{
   BYTE db_subsystem_name     [21];
   BYTE db_connection_type    [11];
   BYTE db_com_port           [11];
   BYTE db_baud_rate          [11];
   BYTE db_byte_size          [11];
   BYTE db_parity             [11];
   BYTE db_stop_bits          [11];
   BYTE db_tcp_port           [11];
   BYTE db_host_name          [41];
   BYTE db_tcp_header_length  [11];
   BYTE db_tcp_socket_type    [11];
   BYTE db_tpdu_processing    [11];
   BYTE db_ssl_processing     [11];
   BYTE db_client_certificate [11];
   BYTE db_server_method      [11];
   BYTE db_cipher_suite       [11];
}CM_CONFIG, *pCM_CONFIG;

typedef struct
{
   BYTE       num_returned;
   CM_CONFIG cm_config_record [COMMGR_LIST_SIZE];
}CM_CONFIG_LIST, *pCM_CONFIG_LIST;
#endif

#endif

