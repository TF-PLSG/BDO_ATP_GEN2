/******************************************************************************
  
   Module: NC_Database.h
  
   Title: Network Controller Dataserver DB Routines
  
   Description: 

   Application:          ATP

   Authors:              Last modified by Dirby.
   
   Revision History:     1.0
    
   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\nc_database.h  $
   
      Rev 1.13   Jun 23 2005 08:42:24   dirby
   Added support for Fraudguard.
   SCR 14421
   
      Rev 1.12   Jul 09 2002 10:16:44   dirby
   Added function prototype 'db_select_ncf30_for_voice'.  This is
   used to cancell voice auths from networks.
   SCR 810
   
      Rev 1.11   Jun 24 2000 21:14:12   dirby
   Added function prototype 'st2_db_select_ncf30_by_rrn_and_terminal'.
   This is used by NCVISA to find original txn to an ATM CONFIRM message (0102).
   
      Rev 1.10   Feb 14 2000 16:09:26   svelaga
   changed the signature of db_update_ncf30_batch_nbr function.
   
      Rev 1.9   Jan 18 2000 16:35:38   ddabberu
   removed c++ style comments
   
      Rev 1.8   Nov 18 1999 10:40:16   dirby
   Added function prototype 'db_select_neg01_by_card_num'.
   
      Rev 1.7   19 Aug 1999 14:31:28   dirby
   Added function prototype 'copy_auth_tx_to_saf01'.
   
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
#include "equitdb.h"


#define ESTIMATED_TRANSACTION_KEY "PRIMARY"

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
pPTE_MSG process_get_count(pPTE_MSG);

void ncf01_to_db( pNCF01 );
void ncf02_to_db( pNCF02 );
void ncf20_to_db( pNCF20 );
void ncf21_to_db( pNCF21 );
void ncf30_to_db( pNCF30 );
void atm01_to_db( pATM01 );
void neg01_to_db( pNEG01 );
void saf01_to_db( pSAF01 );
void fg01_to_db ( pFG01  );

void db_to_ncf01( pNCF01 );
void db_to_ncf02( pNCF02 );
void db_to_ncf20( pNCF20 );
void db_to_ncf21( pNCF21 );
void db_to_ncf30( pNCF30 );
void db_to_atm01( pATM01 );
void db_to_neg01( pNEG01 );
void db_to_saf01( pSAF01 );
void db_to_fg01 ( pFG01  );


BYTE db_select_ncf01 (pCHAR, pNCF01, pCHAR);
BYTE db_select_ncf02 (pCHAR, pNCF02, pCHAR); 
BYTE db_select_ncf20 (pCHAR, pNCF20, pCHAR);
BYTE db_select_ncf21 (pCHAR, pNCF21, pCHAR);
BYTE db_select_ncf30 (pCHAR, pNCF30, pCHAR);
BYTE db_select_neg01 (pCHAR, pNEG01, pCHAR);
BYTE db_select_neg01_by_card_num (pCHAR, pNEG01, pCHAR);
BYTE db_select_atm01 (pCHAR, pATM01, pCHAR);
BYTE db_select_saf01 (pSAF01,pSAF01, pCHAR);
BYTE db_select_fg01  (pCHAR, pFG01,  pCHAR);
BYTE db_select_obs01  (pCHAR, pOBS01,  pCHAR);
BYTE db_select_visa3ds2  (pCHAR, pVISA3DS2,  pCHAR);
BYTE db_select_max_batch_ncf21      (pCHAR,  pCHAR,  pCHAR);
BYTE db_select_ncf30_by_stan        (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_termid      (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_card        (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_merchant_id (pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_for_voice      (pNCF30, pNCF30, pCHAR);
BYTE ddb_select_and_update_ncf30_for_Incrementalauthorization(pNCF30, pNCF30, pCHAR);
BYTE db_select_and_update_ncf30_for_Partialreversalauthorization(pNCF30, pNCF30, pCHAR);
BYTE db_select_ncf30_by_rrn_and_terminal (pNCF30, pNCF30, pCHAR);
BYTE db_select_tlf01_for_pulse_reversal (  pAUTH_TX auth_tx, pTLF01 tlf01_ptr, pCHAR ErrorMsg );

BYTE db_update_ncf01 (pNCF01, pCHAR);
BYTE db_update_ncf02 (pNCF02, pCHAR);
BYTE db_update_ncf20 (pNCF20, pCHAR);
BYTE db_update_ncf21 (pNCF21, pCHAR);
BYTE db_update_ncf30 (pNCF30, pCHAR);
BYTE db_update_neg01 (pNEG01, pCHAR);
BYTE db_update_atm01 (pATM01, pCHAR);
BYTE db_update_saf01 (pSAF01, pCHAR);
BYTE db_update_fg01  (pFG01,  pCHAR);
BYTE db_update_obs01  (pOBS01,  pCHAR);
BYTE db_update_visa3ds2  (pVISA3DS2,  pCHAR);
BYTE db_update_ncf21_batch_number( pNCF21, pCHAR );
BYTE db_update_ncf30_batch_nbr( pCHAR, pCHAR , pCHAR, pCHAR, pCHAR, pCHAR );

BYTE db_delete_ncf01 (pNCF01, pCHAR);
BYTE db_delete_ncf02 (pNCF02, pCHAR);
BYTE db_delete_ncf20 (pNCF20, pCHAR);
BYTE db_delete_ncf21 (pNCF21, pCHAR);
BYTE db_delete_ncf30 (pNCF30, pCHAR);
BYTE db_delete_neg01 (pNEG01, pCHAR);
BYTE db_delete_atm01 (pATM01, pCHAR);
BYTE db_delete_saf01 (pSAF01, pCHAR);
BYTE db_delete_fg01  (pFG01,  pCHAR);
BYTE db_delete_obs01  (pOBS01,  pCHAR);
BYTE db_delete_visa3ds2  (pVISA3DS2,  pCHAR);

BYTE db_insert_ncf01 (pNCF01, pCHAR);
BYTE db_insert_ncf02 (pNCF02, pCHAR);
BYTE db_insert_ncf20 (pNCF20, pCHAR);
BYTE db_insert_ncf21 (pNCF21, pCHAR);
BYTE db_insert_ncf30 (pNCF30, pCHAR);
BYTE db_insert_neg01 (pNEG01, pCHAR);
BYTE db_insert_atm01 (pATM01, pCHAR);
BYTE db_insert_saf01 (pSAF01, pCHAR);
BYTE db_insert_fg01  (pFG01,  pCHAR);
BYTE db_insert_obs01  (pOBS01,  pCHAR);
BYTE db_insert_visa3ds2  (pVISA3DS2,  pCHAR);

BYTE db_get_ncf01_gui_list( pNCF01 ncf01_ptr, pNCF01_GUI_LIST Ncf01List,
                            pCHAR  ErrorMsg);
BYTE db_get_ncf01_service_list( pNCF01 ncf01_ptr, pNCF01_LIST Ncf01List,
                            pCHAR  ErrorMsg);

BYTE db_get_ncf01_gui_list_by_type( pCHAR Network_Id, pNCF01_GUI_LIST Ncf01List,
                                    pCHAR ErrorMsg );
                                                
BYTE db_get_ncf02_gui_list( pNCF02 ncf02_ptr, pNCF02_GUI_LIST Ncf02_List,
                            pCHAR  ErrorMsg);

BYTE db_get_ncf30_service_list( pNCF30, pNCF30_BATCH_LIST, pCHAR );

BYTE db_get_fg01_gui_list( pFG01 fg01_ptr, pFG01_GUI_LIST Fg01List,
                           pCHAR  ErrorMsg);
BYTE db_get_obs01_gui_list( pOBS01 obs01_ptr, pOBS01_GUI_LIST OBS01_List,
                            pCHAR  ErrorMsg);
BYTE db_get_visa3ds2_gui_list( pVISA3DS2 visa3ds2_ptr, pVISA3DS2_GUI_LIST VISA3DS2_List,
                            pCHAR  ErrorMsg);



BYTE db_get_obs01_count( pCHAR table_count, pCHAR ErrorMsg );

void    process_logger_reply( pPTE_MSG );
BOOLEAN perform_refeed ( pCHAR );
BYTE    validate_last_trid ( pPTE_MSG );
void    rollback_the_update();
void    commit_the_update();
void    copy_auth_tx_to_saf01( pAUTH_TX, pSAF01 );

#endif
