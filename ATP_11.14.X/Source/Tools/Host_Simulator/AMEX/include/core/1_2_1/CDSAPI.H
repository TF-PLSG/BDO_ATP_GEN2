/******************************************************************************
  
        Module: cdsapi.h
  
         Title: Terminal Dataserver API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\cdsapi\CDSAPI.H  $
   
      Rev 1.4   10 Aug 1998 12:04:36   rmalathk
   1. moved these files out of ptedb.lib.
   2. made modifications to seperate pteipc calls
       from ptetcp calls.
   3. This file is now shared by two library projects,
       pteipc.lib and ptetcp.lib
   
      Rev 1.3   May 14 1998 09:21:50   JXIAO
   Add more functions required by new merged dialog and hostcon GUI
   
      Rev 1.2   01 May 1998 11:13:34   MSALEH
   include "ptemsg.h", "coreinfo.h" in cdsapi.c
   to access definitions needed for functions.
   corrected data structure names that were 
   misspelled.
   
      Rev 1.1   May 01 1998 10:07:56   JXIAO
    
   
      Rev 1.0   Apr 23 1998 11:01:56   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#ifndef CDSAPI_H
#define CDSAPI_H

#include "ptemsg.h"
#include "coreinfo.h"


BYTE cdsapi_delete_record ( pGENERIC_KEYS generic_keys, 
                            BYTE          app_data_type,
                            pCHAR         error_msg );


/* Dialog Manager functions */
BYTE cdsapi_get_dm_config        ( pDM_CONFIG dm_config,          pCHAR error_msg );
BYTE cdsapi_update_dm_config     ( pDM_CONFIG dm_config,          pCHAR error_msg );
BYTE cdsapi_get_dmconfig_table   ( pDM_CONFIG_LIST dmconfig_list, pCHAR error_msg );
BYTE cdsapi_get_dm_config_snd    ( pDM_CONFIG dm_config         , pCHAR error_msg );
BYTE cdsapi_parse_dm_config      ( pDM_CONFIG dm_config         , pPTE_MSG p_msg_in );
BYTE cdsapi_parse_dmconfig_table ( pDM_CONFIG_LIST dmconfig_list, pPTE_MSG p_msg_in);

BYTE cdsapi_get_router_record    ( pROUTER_TABLE router, pCHAR error_msg );
BYTE cdsapi_update_router_config ( pROUTER_TABLE router_table, pCHAR error_msg );
BYTE cdsapi_insert_router_config ( pROUTER_TABLE router_table, pCHAR error_msg );
BYTE cdsapi_get_router_table     ( pROUTER_TABLE_LIST router_list, pCHAR error_msg );
BYTE cdsapi_get_router_record_snd( pROUTER_TABLE router, pCHAR error_msg );
BYTE cdsapi_get_router_table_snd ( pCHAR error_msg );
BYTE cdsapi_parse_router_record  ( pROUTER_TABLE router           , pPTE_MSG p_msg_in );
BYTE cdsapi_parse_router_table   ( pROUTER_TABLE_LIST router_list , pPTE_MSG p_msg_in );

BYTE cdsapi_get_tpdu_record      ( pTPDU tpdu, pCHAR error_msg );
BYTE cdsapi_update_tpdu_config   ( pTPDU tpdu, pCHAR error_msg );
BYTE cdsapi_insert_tpdu_config   ( pTPDU tpdu, pCHAR error_msg );
BYTE cdsapi_get_tpdu_table       ( pTPDU_TABLE_LIST tpdu_list, pCHAR error_msg );
BYTE cdsapi_get_tpdu_table_snd   ( pCHAR error_msg );
BYTE cdsapi_get_tpdu_record_snd  ( pTPDU tpdu                 , pCHAR error_msg );
BYTE cdsapi_parse_tpdu_record    ( pTPDU tpdu                 , pPTE_MSG p_msg_in );
BYTE cdsapi_parse_tpdu_table     ( pTPDU_TABLE_LIST tpdu_list , pPTE_MSG p_msg_in );

BYTE cdsapi_delete_record_snd    ( pGENERIC_KEYS generic_keys, BYTE pp_data_type, pCHAR error_msg ); 
BYTE cdsapi_update_dm_config_snd ( pDM_CONFIG dm_config, pCHAR error_msg );                                                             
BYTE cdsapi_update_router_config_snd( pROUTER_TABLE router_table, pCHAR error_msg );
BYTE cdsapi_update_tpdu_config_snd( pTPDU tpdu, pCHAR error_msg );
BYTE cdsapi_insert_router_config_snd( pROUTER_TABLE router_table, pCHAR error_msg );
BYTE cdsapi_insert_tpdu_config_snd( pTPDU tpdu, pCHAR error_msg );

/* Note: function ends with snd is created for the new GUI app logic, snd means second */
#endif
