/******************************************************************************
  
        Module: coreinfo.h
  
         Title: Common Pinnacle PTE Table Definitions
  
   Description:

   $Log:   N:\PVCS\PTE\CORE\COREDS\COREINFO.H  $
   
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
   BYTE db_host_name         [21];
   BYTE db_host_tcp_port     [11];
}DM_CONFIG, *pDM_CONFIG;


typedef struct
{
   INT       num_returned;
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
   INT          num_returned;
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
   INT  num_returned;
   TPDU tpdu_record [TPDU_LIST_SIZE];
}TPDU_TABLE_LIST, *pTPDU_TABLE_LIST;


#endif

