/******************************************************************************
  
   Module: Database.h
  
   Title: Transaction Server Dataserver DB Routines
  
   Description: 

   Application:          BP

   Authours:             Last modified by supriya.
   
   Revision History:     1.0
    
   $Log:   N:\PVCS\PTE\bea\headers\dc_database.h  $
   
      Rev 1.3   Oct 22 1999 17:18:28   iarustam
    
   
      Rev 1.2   Oct 07 1999 10:19:28   dperson
   Added db_get_tlf01_list
   
      Rev 1.1   16 Sep 1999 17:32:26   epriyev
   added db_select_bch20 definition
   
      Rev 1.0   08 Sep 1999 10:54:04   epriyev
   initial BEA load
   
      Rev 1.13   Jul 07 1999 12:19:54   dperson
   Added db_select_bon01 function prototype
   
      Rev 1.12   May 06 1999 16:40:26   iarustam
   Added db_select_bch10_by_auth_nbr and
   BYTE db_select_bch10_by_original_transaction functions
   
      Rev 1.11   May 05 1999 14:05:04   iarustam
   Added bch11_gui_list
   
      Rev 1.10   Apr 01 1999 11:04:38   iarustam
   Added select_tlf01_by_auth_number for
   Voice Auth Reversal transaction
   
      Rev 1.9   Mar 30 1999 11:30:02   iarustam
   Added new function tlf01_gui_Iist_dynamic for transaction lookup GUI
   
      Rev 1.8   Mar 24 1999 16:19:56   iarustam
   Added new function  db_update_bch01_insert_bch10_insert_tlf01
   
      Rev 1.7   Mar 22 1999 17:28:56   iarustam
   Bug fixes
   
      Rev 1.6   Mar 22 1999 14:35:32   iarustam
   Added tlf01_gui_list
   
      Rev 1.5   26 Feb 1999 15:17:46   dirby
   Added prototype: db_'select_tlf01_by_terminal_id'.
   
      Rev 1.4   Feb 19 1999 16:36:32   IGOLDFIE
   Added select_tlf01_for_sales_completion
   
      Rev 1.3   Feb 17 1999 16:09:30   IGOLDFIE
   Added 2 new function to select TLF01 record
   
      Rev 1.2   Feb 10 1999 10:50:34   IGOLDFIE
   Added db_select_tlf01_saf
   
      Rev 1.1   Feb 04 1999 10:09:02   IGOLDFIE
   Added db_get_tlf01_service_list, 
   db_get_bch01_service_list functions.
   
      Rev 1.0   01 Feb 1999 14:42:52   jhapitas
   Initial Revision

******************************************************************************/

#ifndef _DATABASE
#define _DATABASE


BYTE db_select_tlf01 (pCHAR, pTLF01, pCHAR); 
BYTE db_select_tlf01_saf(pTLF01, pTLF01, pCHAR);        
BYTE db_insert_tlf01 (pTLF01, pCHAR);         
BYTE db_update_tlf01 (pTLF01, pCHAR);
BYTE db_delete_tlf01 (pTLF01, pCHAR); 
BYTE db_delete_tlf01_by_date( pCHAR , pCHAR ErrorMsg);
BYTE db_select_tlf01_by_rrn (pTLF01 tlf01_record,  pTLF01 tlf01_ptr,  pCHAR ErrorMsg );
BYTE db_select_tlf01_by_stan (pTLF01,pTLF01,pCHAR );
BYTE db_select_tlf01_by_terminal_id   (pTLF01,pTLF01,pCHAR );
BYTE db_select_tlf01_by_original_tran (pTLF01,pTLF01,pCHAR );
BYTE db_select_tlf01_by_reversed_tran (pTLF01,pTLF01,pCHAR );
BYTE db_select_tlf01_by_original_tran (pTLF01,pTLF01,pCHAR );
BYTE db_select_tlf01_for_sales_completion (pTLF01,pTLF01,pCHAR );
BYTE db_get_tlf01_list( pTLF01 Tlf01_ptr, 
                        pTLF01_LOG_LIST log_list, 
                        pCHAR ErrorMsg );
BYTE db_get_tlf01_service_list( pTLF01 tlf01_ptr,
                                pTLF01_LIST Tlf01_List,
                                pCHAR ErrorMsg );
BYTE db_get_tlf01_gui_list_dynamic (pTLF01_LIMITS, pTLF01_GUI_LIST,  pCHAR );
BYTE db_get_tlf01_gui_list( pTLF01, pTLF01_GUI_LIST, pCHAR );
BYTE db_select_tlf01_by_auth_nbr (pTLF01, pTLF01, pCHAR );

BYTE db_select_nbf01 (  pCHAR key, pNBF01 nbf01_ptr, pCHAR ErrorMsg );
BYTE db_insert_nbf01 (pNBF01, pCHAR);
BYTE db_update_nbf01 (pNBF01, pCHAR);
BYTE db_delete_nbf01 (pNBF01, pCHAR);
BYTE db_get_nbf01_gui_list( pNBF01 ,pNBF01_GUI_LIST, pCHAR ); 
BYTE db_get_nbf01_service_list( pNBF01 nbf01_ptr, pNBF01_LIST Nbf01_List,  pCHAR ErrorMsg );
BYTE db_get_nbf01_count( pCHAR nbfcount, pCHAR ErrorMsg );
                         
BYTE db_select_ngf01 (  pCHAR key, pNGF01 ngf01_ptr,pCHAR ErrorMsg );
BYTE db_insert_ngf01 (pNGF01, pCHAR);
BYTE db_update_ngf01 (pNGF01, pCHAR);
BYTE db_delete_ngf01 (pNGF01, pCHAR);
BYTE db_get_ngf01_gui_list( pCHAR , pNGF01_GUI_LIST,  pCHAR);
BYTE db_get_ngf01_list_by_binrange( pNGF01 ngf01_rec, pNGF01_LIST Ngf01List,  pCHAR ErrorMsg );
                                               
BYTE db_get_nfi01_service_list( pNFI01 ,pNFI01_LIST ,pCHAR ); 
BYTE db_insert_nfi01( pNFI01, pCHAR );
BYTE db_delete_nfi01_by_date( pCHAR , pCHAR ErrorMsg);
BYTE db_delete_nfi01( pNFI01 , pCHAR ErrorMsg);

BYTE db_select_nfi02 (  pCHAR, pNFI02, pCHAR ) ; 
BYTE db_insert_nfi02 (pNFI02, pCHAR );         
BYTE db_update_nfi02( pNFI02, pCHAR );
BYTE db_delete_nfi02( pNFI02, pCHAR );
                                                 
BYTE db_get_nmf01_list(pNMF01_REQUEST key,  pNMF01_LIST NMF01List, pCHAR ErrMsg);
BYTE db_update_nmf01_action_code ( pCHAR ErrorMsg);
BYTE db_update_delete_nmf01_action_code (pNMF01 nmf01_rec, pCHAR ErrorMsg);
BYTE db_update_nmf01 (pNMF01, pCHAR);

BYTE db_move_ntf01_to_nmf01( pCHAR ErrorMsg);
BYTE db_delete_all_ntf01( pCHAR ErrorMsg);
BYTE db_insert_ntf01( pNTF01 ntf01_ptr, pCHAR ErrorMsg );
BYTE db_move_recs_from_ntf01_to_nmf01( pNTF01, pCHAR ErrorMsg); 

BYTE db_select_mcf20 (pCHAR, pMCF20, pCHAR);         
BYTE db_insert_mcf20 (pMCF20, pCHAR);         
BYTE db_update_mcf20 (pMCF20, pCHAR);
BYTE db_delete_mcf20 (pMCF20, pCHAR);        

BYTE db_select_ocf20 (pCHAR, pOCF20, pCHAR);         
BYTE db_insert_ocf20 (pOCF20, pCHAR);         
BYTE db_update_ocf20 (pOCF20, pCHAR);
BYTE db_delete_ocf20 (pOCF20, pCHAR);        

BYTE db_select_ocf01 (pCHAR, pOCF01, pCHAR);         
BYTE db_insert_ocf01 (pOCF01, pCHAR);         
BYTE db_update_ocf01 (pOCF01, pCHAR);
BYTE db_delete_ocf01 (pOCF01, pCHAR);        

BYTE db_select_mcf01 (pCHAR, pMCF01, pCHAR);         
BYTE db_insert_mcf01 (pMCF01, pCHAR);         
BYTE db_update_mcf01 (pMCF01, pCHAR);
BYTE db_delete_mcf01 (pMCF01, pCHAR);        
BYTE db_get_mcf01_gui_list( pCHAR Primary_Key, BYTE Select_Flag, pMCF01_GUI_LIST Mcf01List, pCHAR ErrorMsg );
  
BYTE db_select_dcf01 (pCHAR, pDCF01, pCHAR);         
BYTE db_insert_dcf01 (pDCF01, pCHAR);         
BYTE db_update_dcf01 (pDCF01, pCHAR);
BYTE db_delete_dcf01 (pDCF01, pCHAR);        
BYTE db_get_dcf01_gui_list( pCHAR Device_Id, pDCF01_GUI_LIST Dcf01List, pCHAR ErrorMsg );

BYTE db_insert_drf01( pDRF01 drf01_ptr, pCHAR ErrorMsg );
BYTE db_delete_drf01_by_date(  pCHAR p_data, pCHAR ErrorMsg );

BYTE db_insert_dlf01( pDLF01 dlf01_ptr, pCHAR ErrorMsg );
                        
BYTE db_select_bch01 (pCHAR, pBCH01, pCHAR); 
BYTE db_select_bch01_for_gui (  pBCH01, pBCH01, pCHAR );    
BYTE db_select_bch01_by_status (pBCH01, pBCH01, pCHAR); 
BYTE db_select_or_insert_bch01_by_status (pBCH01 Batch_status, pBCH01 bch01_ptr, pCHAR ErrorMsg );                                 
BYTE db_select_bch01_by_date (pBCH01 ,pBCH01 , pCHAR);        
BYTE db_select_bch01_by_status_and_tid (pBCH01, pBCH01, pCHAR); 
BYTE db_insert_bch01 (pBCH01, pCHAR);         
BYTE db_insert_bch01_update_dcf01 (pBCH01, pCHAR);         
BYTE db_update_bch01 (pBCH01, pCHAR);
BYTE db_update_bch01_good_settle ( pBCH01, pCHAR);
BYTE db_update_bch01_delete_bch10( pBCH01, pCHAR);
BYTE db_return_bch01_delete_bch10( pBCH01, pCHAR);
BYTE db_update_bch01_to_close_batch( pBCH01 , pCHAR  );
BYTE db_update_bch01_insert_bch10( pBCH01 bch01_ptr,pBCH10 bch10_ptr, pCHAR ErrorMsg);
BYTE db_get_bch01_gui_list( pBCH01 bch01_ptr, pBCH01_GUI_LIST Bch01List, pCHAR ErrorMsg); 
BYTE db_update_bch01_move_bch10tobch11_delete_bch10(pBCH01 bch01_ptr, pCHAR ErrorMsg);
BYTE db_delete_bchrecs_for_cleanup ( pCHAR date, pCHAR ErrorMsg);
BYTE db_get_bch01_service_list( pBCH01 bch01_ptr,
                                pBCH01_LIST Bch01_List,
                                pCHAR ErrorMsg );

BYTE db_insert_bch10 (pBCH10, pCHAR);         
BYTE db_insert_bch10_settle(pBCH10, pCHAR);
BYTE db_delete_bch10 (pBCH10, pCHAR);
BYTE db_delete_bch10_by_stan( pBCH10 bch10_ptr, pCHAR ErrorMsg);
BYTE db_delete_active_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg);
BYTE db_update_bch10( pBCH10, pCHAR);
BYTE db_select_bch10 (pBCH10, pBCH10, pCHAR);
BYTE db_select_active_bch10 (pBCH10, pBCH10, pCHAR);
BYTE db_select_bch10_by_stan (pBCH10,pBCH10,pCHAR );
BYTE db_select_bch10_by_batch_nbr (pCHAR,pBCH10,pCHAR );
BYTE db_get_bch10_service_list( pBCH10 bch10_ptr, pBCH10_LIST Bch10List, pCHAR ErrorMsg); 
BYTE db_get_bch10_gui_list( pBCH10 bch10_ptr, pBCH10_GUI_LIST Bch10List, pCHAR ErrorMsg); 
BYTE db_get_bch11_gui_list( pBCH11, pBCH11_GUI_LIST, pCHAR ); 
BYTE db_move_bch10_to_bch11(pCHAR, pCHAR);
BYTE db_move_bch11_to_bch10(pCHAR PrimaryKey, pCHAR ErrorMsg);
BYTE db_update_bch10_to_close_batch( pBCH10 bch10_ptr, pCHAR ErrorMsg );
BYTE db_select_bch10_detail_by_batch_tid_stan(pCHAR ,pCHAR ,pCHAR,pBCH01,pCHAR);
BYTE db_select_bch10_by_auth_nbr (pBCH10, pBCH10, pCHAR );
BYTE db_select_bch10_by_original_transaction (pBCH10, pBCH10, pCHAR );

BYTE db_insert_bch20( pBCH20 bch20_ptr, pCHAR ErrorMsg);
BYTE db_select_bch20_by_stan (pBCH20, pBCH20,  pCHAR);
BYTE db_select_bch20_by_rrn (pBCH20, pBCH20,  pCHAR);

BYTE db_select_bch11_by_batch_nbr (pCHAR, pBCH11, pCHAR);
BYTE db_get_bch11_service_list( pBCH11 bch11_ptr, pBCH11_LIST Bch11_List, pCHAR ErrorMsg );
BYTE db_update_bch01_insert_bch10_insert_tlf01(pAUTH_TX, pCHAR );

BYTE db_select_bon01 (pCHAR, pCHAR);

#endif