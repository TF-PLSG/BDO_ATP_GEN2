/******************************************************************************
  
   Module: NC_Database.h
  
   Title: Network Controller Dataserver DB Routines
  
   Description: 

   Application:          ATP

   Authors:              Last modified by Dirby.
   
   Revision History:     1.0
    
   $Log:   N:\PVCS\PTE\bea\headers\nc_database.h  $
   
      Rev 1.2   Sep 23 1999 13:21:48   iarustam
   Removed saf01
   
      Rev 1.1   Sep 21 1999 14:39:22   svelaga
   changed include file from "equitdb.h" to 
   "beadb.h"
   
      Rev 1.0   08 Sep 1999 10:54:06   epriyev
   initial BEA load
   
      Rev 1.6   19 Aug 1999 12:40:54   dirby
   Added prototype for function 'db_select_ncf30_by_merchant_id'.
   
      Rev 1.5   18 Aug 1999 13:56:54   dirby
   Added function prototypes for selecting records from NCF30
   by stan, device_id, and card number.
   
      Rev 1.4   17 Aug 1999 10:19:26   dirby
   Added prototypes for SAF01 - Store and Forward database table.
   
      Rev 1.3   03 Aug 1999 15:57:46   dirby
   Added function prototype for 'process_alldata_delete_msg'.
   
      Rev 1.2   Apr 09 1999 10:43:02   iarustam
   Added ncf01_list for transaction lookup GUI
   
      Rev 1.1   12 Feb 1999 08:54:00   dirby
   Stripped out all none network controller structures and prototypes.
   Added table structures NCF21, NCF30, NEG01, and ATM01.

******************************************************************************/

#ifndef NC_DATABASE_H
#define NC_DATABASE_H

#include "app_info.h"
#include "nc_dbstruct.h"
#include "beadb.h"


void     genutil_format_date( pCHAR );
pPTE_MSG process_insert_msg(  pPTE_MSG );
pPTE_MSG process_update_msg(  pPTE_MSG );
pPTE_MSG process_delete_msg(  pPTE_MSG );
pPTE_MSG process_select_msg(  pPTE_MSG );
pPTE_MSG process_get_gui_list_msg( pPTE_MSG );
pPTE_MSG process_get_service_list_msg( pPTE_MSG );
pPTE_MSG process_echo_msg( pPTE_MSG );
pPTE_MSG process_db_request( pPTE_MSG );
BYTE     process_alldata_msg( pPTE_MSG );
BYTE     process_alldata_delete_msg( pPTE_MSG );

void ncf01_to_db( pNCF01 );
void ncf02_to_db( pNCF02 );
void ncf20_to_db( pNCF20 );
void ncf21_to_db( pNCF21 );
void ncf30_to_db( pNCF30 );
void atm01_to_db( pATM01 );
void neg01_to_db( pNEG01 );

void db_to_ncf01( pNCF01 );
void db_to_ncf02( pNCF02 );
void db_to_ncf20( pNCF20 );
void db_to_ncf21( pNCF21 );
void db_to_ncf30( pNCF30 );
void db_to_atm01( pATM01 );
void db_to_neg01( pNEG01 );


BYTE db_select_ncf01 (pCHAR, pNCF01, pCHAR);
BYTE db_select_ncf02 (pCHAR, pNCF02, pCHAR); 
BYTE db_select_ncf20 (pCHAR, pNCF20, pCHAR);
BYTE db_select_ncf21 (pCHAR, pNCF21, pCHAR);
BYTE db_select_ncf30 (pCHAR, pNCF30, pCHAR);
BYTE db_select_neg01 (pCHAR, pNEG01, pCHAR);
BYTE db_select_atm01 (pCHAR, pATM01, pCHAR);
BYTE db_select_max_batch_ncf21      (pCHAR,  pCHAR,  pCHAR);
BYTE db_select_ncf30_by_stan        (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_termid      (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_card        (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_merchant_id (pNCF30, pNCF30, pCHAR);

BYTE db_update_ncf01 (pNCF01, pCHAR);
BYTE db_update_ncf02 (pNCF02, pCHAR);
BYTE db_update_ncf20 (pNCF20, pCHAR);
BYTE db_update_ncf21 (pNCF21, pCHAR);
BYTE db_update_ncf30 (pNCF30, pCHAR);
BYTE db_update_neg01 (pNEG01, pCHAR);
BYTE db_update_atm01 (pATM01, pCHAR);
BYTE db_update_ncf21_batch_number( pNCF21, pCHAR );
BYTE db_update_ncf30_batch_nbr( pCHAR, pCHAR , pCHAR, pCHAR );

BYTE db_delete_ncf01 (pNCF01, pCHAR);
BYTE db_delete_ncf02 (pNCF02, pCHAR);
BYTE db_delete_ncf20 (pNCF20, pCHAR);
BYTE db_delete_ncf21 (pNCF21, pCHAR);
BYTE db_delete_ncf30 (pNCF30, pCHAR);
BYTE db_delete_neg01 (pNEG01, pCHAR);
BYTE db_delete_atm01 (pATM01, pCHAR);

BYTE db_insert_ncf01 (pNCF01, pCHAR);
BYTE db_insert_ncf02 (pNCF02, pCHAR);
BYTE db_insert_ncf20 (pNCF20, pCHAR);
BYTE db_insert_ncf21 (pNCF21, pCHAR);
BYTE db_insert_ncf30 (pNCF30, pCHAR);
BYTE db_insert_neg01 (pNEG01, pCHAR);
BYTE db_insert_atm01 (pATM01, pCHAR);


BYTE db_get_ncf01_gui_list( pNCF01 ncf01_ptr, pNCF01_GUI_LIST Ncf01List,
                            pCHAR  ErrorMsg);
BYTE db_get_ncf01_service_list( pNCF01 ncf01_ptr, pNCF01_LIST Ncf01List,
                            pCHAR  ErrorMsg);

BYTE db_get_ncf01_gui_list_by_type( pCHAR Network_Id, pNCF01_GUI_LIST Ncf01List,
                                    pCHAR ErrorMsg );
                                                
BYTE db_get_ncf02_gui_list( pNCF02 ncf02_ptr, pNCF02_GUI_LIST Ncf02_List,
                            pCHAR  ErrorMsg);

BYTE db_get_ncf30_service_list( pNCF30, pNCF30_BATCH_LIST, pCHAR );

void    process_logger_reply( pPTE_MSG );
BOOLEAN perform_refeed ( pCHAR );
BYTE    validate_last_trid ( pPTE_MSG );
void    rollback_the_update();
void    commit_the_update();

#endif