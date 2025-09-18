/******************************************************************************
  
        Module: coredb.h
  
         Title: Core Dataserver DB Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\COREDS\COREDB.H  $
   
      Rev 1.1   Oct 14 1999 07:50:02   MSALEH
   integrate SSL modifications
   
      Rev 1.0   Apr 23 1998 13:57:02   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#ifndef COREDB_H
#define COREDB_H



BYTE db_select_dmconfig_record ( pCHAR Subsystem,
                                 pDM_CONFIG DmConfig, 
                                 pCHAR ErrorMsg );
BYTE db_update_dmconfig_record ( pDM_CONFIG DmConfig, 
                                 pCHAR ErrorMsg );
BYTE db_get_dm_config_table    ( pDM_CONFIG_LIST DMConfigList, 
                                 pCHAR ErrorMsg );


BYTE db_select_router_record ( pCHAR RouterId, 
                               pROUTER_TABLE Router, 
                               pCHAR ErrorMsg );
BYTE db_insert_router_record ( pROUTER_TABLE Router, 
                               pCHAR ErrorMsg );
BYTE db_update_router_record ( pROUTER_TABLE Router, 
                               pCHAR ErrorMsg );
BYTE db_delete_router_record ( pCHAR RouterId, 
                               pCHAR ErrorMsg );
BYTE db_get_router_table     ( pROUTER_TABLE_LIST RouterTableList, 
                               pCHAR ErrorMsg );


BYTE db_select_tpdu_record ( pCHAR TpduId, 
                             pTPDU Tpdu, 
                             pCHAR ErrorMsg );
BYTE db_insert_tpdu_record ( pTPDU Tpdu, 
                             pCHAR ErrorMsg );
BYTE db_update_tpdu_record ( pTPDU Tpdu, 
                             pCHAR ErrorMsg );
BYTE db_delete_tpdu_record ( pCHAR TpduId, 
                             pCHAR ErrorMsg );
BYTE db_get_tpdu_table     ( pTPDU_TABLE_LIST TpduTableList, 
                             pCHAR ErrorMsg );

#ifdef SSL_SUPPORT
BYTE db_update_cmconfig_record ( pCM_CONFIG CmConfig, 
                                 pCHAR ErrorMsg );
BYTE db_get_cm_config_table    ( pCM_CONFIG_LIST CMConfigList, 
                                 pCHAR ErrorMsg );
BYTE db_select_cmconfig_record ( pCHAR Subsystem,
                                 pCM_CONFIG CmConfig, 
                                 pCHAR ErrorMsg );
#endif

#endif

