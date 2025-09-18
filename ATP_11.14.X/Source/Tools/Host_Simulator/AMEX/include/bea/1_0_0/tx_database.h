/******************************************************************************
  
   Module: tx_database.h
  
   Title: Transaction Server Dataserver DB Routines
  
   Description: 

   Application:          BP

   Authours:             Last modified by supriya.
   
   Revision History:     1.0
    
   $Log:   N:\PVCS\PTE\bea\headers\tx_database.h  $
   
      Rev 1.2   Oct 22 1999 19:11:58   iarustam
   added function delete_bpf01_by_profile_id
   
      Rev 1.1   Sep 21 1999 14:39:30   svelaga
   changed include file from "equitdb.h" to 
   "beadb.h"
   
      Rev 1.0   08 Sep 1999 10:54:08   epriyev
   initial BEA load
   
      Rev 1.6   Jun 04 1999 17:36:30   iarustam
   Added new function def01_gui_list_by_merchant_and_org
   
      Rev 1.5   Apr 15 1999 18:01:48   sdasgupt
   Added prototype for the function db_update_acf01_for_deferred_account
   
      Rev 1.4   Mar 20 1999 08:56:06   iarustam
   Changes for GUI lists
   
      Rev 1.3   Mar 08 1999 17:12:46   iarustam
   Added def01_gui_list function
   
      Rev 1.2   Feb 25 1999 18:41:50   sdasgupt
   Added protofypes for the bin02 functions
   
      Rev 1.1   Feb 16 1999 18:24:50   iarustam
   Added ccf04_gui_list and
              acf01_gui_list

******************************************************************************/

#ifndef _DATABASE
#define _DATABASE

#include "app_info.h"
#include "tx_dbstruct.h" 
#include "beadb.h"

pPTE_MSG process_select_auth_id_msg (pPTE_MSG p_msg_in) ;

BYTE db_select_csf01 (pCHAR, pCSF01, pCHAR);         
BYTE db_insert_csf01 (pCSF01, pCHAR);         
BYTE db_update_csf01 (pCSF01, pCHAR);
BYTE db_delete_csf01 (pCSF01, pCHAR);        
BYTE db_get_csf01_gui_list( pCHAR Primary_Key, pCSF01_GUI_LIST Csf01List, pCHAR ErrorMsg);
BYTE db_get_csf01_list(pCSF01_LIST, pCHAR);
BYTE db_get_csf01_count( pCHAR csf_count, pCHAR ErrorMsg );


BYTE db_select_clf01 (pCHAR, pCLF01, pCHAR);         
BYTE db_insert_clf01 (pCLF01, pCHAR);         
BYTE db_update_clf01 (pCLF01, pCHAR);
BYTE db_delete_clf01 (pCLF01, pCHAR);        

BYTE db_select_apf01 (pCHAR, pAPF01, pCHAR);         
BYTE db_insert_apf01 (pAPF01, pCHAR);         
BYTE db_update_apf01 (pAPF01, pCHAR);
BYTE db_delete_apf01 (pAPF01, pCHAR);        
BYTE db_get_apf01_gui_list( pCHAR App_Name, pAPF01_GUI_LIST Apf01List, pCHAR ErrorMsg); 

BYTE db_select_bpf01 (pCHAR, pBPF01, pCHAR);         
BYTE db_insert_bpf01 (pBPF01, pCHAR);         
BYTE db_update_bpf01 (pBPF01_SCH_LIST, pCHAR);
BYTE db_delete_bpf01 (pBPF01, pCHAR);
BYTE db_delete_bpf01_by_job_id ( pBPF01 bpf01_ptr, pCHAR ErrorMsg);
BYTE db_check_bpf01_by_job_id ( pBPF01 bpf01_rec,pCHAR ErrorMsg ); 
BYTE db_delete_bpf01_by_profile_id( pBPF01, pCHAR  );

BYTE db_get_bpf01_sch_list( pBPF01 bpf01_ptr, pBPF01_SCH_LIST Bpf01List, pCHAR ErrorMsg); 
BYTE db_get_bpf01_gui_list( pCHAR Profile_Id, pBPF01_GUI_LIST Bpf01List, pCHAR ErrorMsg); 

BYTE db_select_jbf01 (pCHAR, pJBF01, pCHAR);         
BYTE db_insert_jbf01 (pJBF01, pCHAR);         
BYTE db_update_jbf01 (pJBF01, pCHAR);
BYTE db_delete_jbf01 (pJBF01, pCHAR);
BYTE db_delete_jbf01_by_app_name( pJBF01 jbf01_ptr, pCHAR ErrorMsg);
BYTE db_check_jbf01_by_app_name (  pCHAR   Primary_key, pCHAR    ErrorMsg ) ;
BYTE db_get_jbf01_gui_list( pCHAR Job_Id, pJBF01_GUI_LIST Jbf01List, pCHAR ErrorMsg); 

BYTE db_select_faf01 (pCHAR, pFAF01, pCHAR);         
BYTE db_insert_faf01 (pFAF01, pCHAR);         
BYTE db_update_faf01 (pFAF01, pCHAR);
BYTE db_delete_faf01 (pFAF01, pCHAR);        
BYTE db_get_faf01_count( pCHAR, pCHAR );
BYTE db_get_faf01_list( pFAF01_LIST Faf01List, pCHAR ErrorMsg );

BYTE db_select_ccf10 (pCHAR, pCCF10, pCHAR);         
BYTE db_insert_ccf10 (pCCF10, pCHAR);         
BYTE db_update_ccf10 (pCCF10, pCHAR);
BYTE db_delete_ccf10 (pCCF10, pCHAR);  
BYTE db_delete_ccf10_by_date( pCHAR, pCHAR );      
BYTE db_get_ccf10_gui_list( pCCF10 Ccf10_Details, pCCF10_GUI_LIST Ccf10List, pCHAR ErrorMsg); 

BYTE db_select_bon01 (pBON01, pCHAR);         
BYTE db_insert_bon01 (pBON01, pCHAR);         
BYTE db_update_bon01 (pBON01, pCHAR);
BYTE db_delete_bon01 (pBON01, pCHAR);  

BYTE db_select_bin02 (pCHAR, pBIN02, pCHAR);         
BYTE db_insert_bin02 (pBIN02, pCHAR);         
BYTE db_update_bin02 (pBIN02, pCHAR);
BYTE db_delete_bin02 (pCHAR);  

BYTE db_select_ccf01 (pCHAR, pCCF01, pCHAR);         
BYTE db_insert_ccf01 (pCCF01, pCHAR);         
BYTE db_update_ccf01 (pCCF01, pCHAR);
BYTE db_delete_ccf01 (pCCF01, pCHAR);        

BYTE db_select_acf01 (pCHAR, pACF01, pCHAR);         
BYTE db_insert_acf01 (pACF01, pCHAR);         
BYTE db_update_acf01 (pACF01, pCHAR);
BYTE db_delete_acf01 (pACF01, pCHAR); 
BYTE db_get_acf01_gui_list ( pACF01, pACF01_GUI_LIST, pCHAR );       
BYTE db_get_acf01_gui_list_by_type ( pACF01, pACF01_GUI_LIST, pCHAR );       

BYTE db_select_crf01 (pCHAR, pCRF01, pCHAR);         
BYTE db_insert_crf01 (pCRF01, pCHAR);         
BYTE db_update_crf01 (pCRF01, pCHAR);
BYTE db_delete_crf01 (pCRF01, pCHAR);        
BYTE db_get_crf01_count( pCHAR, pCHAR );
BYTE db_get_crf01_gui_list( pCHAR Currency_Code, pCRF01_GUI_LIST Crf01List, pCHAR ErrorMsg); 
BYTE db_get_crf01_list( pCRF01_LIST Crf01List, pCHAR ErrorMsg );

BYTE db_select_paf01 (pCHAR, pPAF01, pCHAR);         
BYTE db_insert_paf01 (pPAF01, pCHAR);         
BYTE db_update_paf01 (pPAF01, pCHAR);
BYTE db_delete_paf01 (pPAF01, pCHAR);        
BYTE db_get_paf01_gui_list( pCHAR Primary_Key, pPAF01_GUI_LIST Paf01List, pCHAR ErrorMsg);

BYTE db_select_def01 (pCHAR, pDEF01, pCHAR);         
BYTE db_insert_def01 (pDEF01, pCHAR);         
BYTE db_update_def01 (pDEF01, pCHAR);
BYTE db_delete_def01 (pDEF01, pCHAR);  
BYTE db_get_def01_gui_list ( pDEF01 , pDEF01_GUI_LIST, pCHAR );
BYTE db_get_def01_gui_list_by_merchant_and_org( pDEF01, pDEF01_GUI_LIST, pCHAR );

BYTE db_select_mct01 (pCHAR, pMCT01, pCHAR);         
BYTE db_insert_mct01 (pMCT01, pCHAR);         
BYTE db_update_mct01 (pMCT01, pCHAR);
BYTE db_delete_mct01 (pMCT01, pCHAR);        
BYTE db_get_mct01_gui_list( pMCT01 Merch_Card, pMCT01_GUI_LIST Mct01List, pCHAR ErrorMsg); 
BYTE db_get_mct01_gui_list_by_type( pMCT01 mct01_ptr,  pMCT01_GUI_LIST Mct01_List,  pCHAR ErrorMsg );
                               
BYTE db_select_ctf01 (pCHAR, pCTF01, pCHAR);         
BYTE db_insert_ctf01 (pCTF01, pCHAR);         
BYTE db_update_ctf01 (pCTF01, pCHAR);
BYTE db_delete_ctf01 (pCTF01, pCHAR);        
BYTE db_get_ctf01_service_list( pCHAR Card_type, pCTF01_LIST Ctf01List, pCHAR ErrorMsg); 
BYTE db_get_ctf01_gui_list( pCHAR Card_type, pCTF01_GUI_LIST Ctf01List, pCHAR ErrorMsg); 

BYTE db_select_caf01 (pCHAR, pCAF01, pCHAR);         
BYTE db_insert_caf01 (pCAF01, pCHAR);         
BYTE db_update_caf01 (pCAF01, pCHAR);
BYTE db_delete_caf01 (pCAF01, pCHAR);        
BYTE db_get_caf01_count( pCHAR, pCHAR );
BYTE db_get_caf01_gui_list( pCHAR Caf_Profile_Id, pCAF01_GUI_LIST Caf01List, pCHAR ErrorMsg); 
BYTE db_get_caf01_gui_list_by_type( pCHAR Caf_Profile_Id, pCAF01_GUI_LIST Caf01List, pCHAR ErrorMsg); 
BYTE db_get_caf01_service_list( pCHAR Caf_Profile_Id, pCAF01_LIST Caf01List, pCHAR ErrorMsg); 
BYTE db_get_caf01_list( pCAF01_LIST Caf01List, pCHAR ErrorMsg );

BYTE db_select_atp02 (pCHAR, pATP02, pCHAR);         
BYTE db_insert_atp02 (pATP02, pCHAR);         
BYTE db_update_atp02 (pATP02, pCHAR);
BYTE db_delete_atp02 (pATP02, pCHAR);        
BYTE db_get_atp02_count( pCHAR, pCHAR );
BYTE db_get_atp02_service_list( pATP02 atp02_ptr, pATP02_LIST Atp02List, pCHAR ErrorMsg); 
BYTE db_get_atp02_gui_list( pATP02 atp02_ptr, pATP02_GUI_LIST Atp02List, pCHAR ErrorMsg); 

BYTE db_select_fcf02 (pCHAR, pFCF02, pCHAR);         
BYTE db_insert_fcf02 (pFCF02, pCHAR);         
BYTE db_update_fcf02 (pFCF02, pCHAR);
BYTE db_delete_fcf02 (pFCF02, pCHAR);        

BYTE db_select_ccf02p (pCHAR, pCCF02P, pCHAR);         
BYTE db_insert_ccf02p (pCCF02P, pCHAR);         
BYTE db_update_ccf02p (pCCF02P, pCHAR);
BYTE db_delete_ccf02p (pCCF02P, pCHAR);        
BYTE db_get_ccf02p_gui_list( pCHAR PrimaryKey, pCCF02P_GUI_LIST Ccf02List, pCHAR ErrorMsg );
BYTE db_get_ccf02p_gui_list_by_type( pCHAR PrimaryKey, pCCF02P_GUI_LIST Ccf02List, pCHAR ErrorMsg );

BYTE db_select_ccf04 (pCHAR, pCCF04, pCHAR);         
BYTE db_insert_ccf04 (pCCF04, pCHAR);         
BYTE db_update_ccf04 (pCCF04, pCHAR);
BYTE db_delete_ccf04 (pCCF04, pCHAR);  
BYTE db_get_ccf04_gui_list ( pCCF04, pCCF04_GUI_LIST, pCHAR );      
BYTE db_get_ccf04_gui_list_by_type ( pCCF04, pCCF04_GUI_LIST, pCHAR );      

BYTE db_select_ccf02h (pCHAR, pCCF02H, pCHAR);
BYTE db_insert_ccf02h (pCCF02H, pCHAR);         
BYTE db_update_ccf02h (pCCF02H, pCHAR);
BYTE db_delete_ccf02h (pCCF02H, pCHAR);              

BYTE db_select_ccf03p (pCHAR, pCCF03, pCHAR);         
BYTE db_insert_ccf03p (pCCF03, pCHAR);         
BYTE db_update_ccf03p (pCCF03, pCHAR);
BYTE db_delete_ccf03p (pCCF03, pCHAR);        

BYTE db_select_atp01 (pCHAR, pATP01, pCHAR);         
BYTE db_insert_atp01 (pATP01, pCHAR);         
BYTE db_update_atp01 (pATP01, pCHAR);
BYTE db_delete_atp01 (pATP01, pCHAR);        

BYTE db_select_ucf01 (pCHAR, pUCF01, pCHAR);         
BYTE db_insert_ucf01 (pUCF01, pCHAR);         
BYTE db_update_ucf01 (pUCF01, pCHAR);
BYTE db_delete_ucf01 (pUCF01, pCHAR);        
BYTE db_get_ucf01_list( pCHAR Name, pUCF01_LIST Ucf01List, pCHAR ErrorMsg );
BYTE db_get_ucf01_list_by_terminal( pUCF01 ucf01_rec,pUCF01_LIST Ucf01List,pCHAR ErrorMsg);
   

BYTE db_select_ucf02 (pCHAR, pUCF02, pCHAR);         
BYTE db_insert_ucf02 (pUCF02, pCHAR);         
BYTE db_update_ucf02 (pUCF02, pCHAR);
BYTE db_delete_ucf02 (pUCF02, pCHAR);        
BYTE db_get_ucf02_list( pCHAR Name, pUCF02_LIST Ucf02List, pCHAR ErrorMsg );
BYTE db_get_ucf02_list_by_terminal( pUCF02 ucf02_rec, pUCF02_LIST Ucf02List, pCHAR ErrorMsg );

// Added by SDG on Feb 2
BYTE db_select_auth_id (pUAG01 auth01_ptr, pCHAR ErrorMsg) ;
void db_to_auth01 (pUAG01 auth01_ptr) ;
void auth01_to_db (pUAG01 auth01_ptr) ;
void GenerateNewAuthId (char* pstrCurrentAuthId, char* pstrNewAuthId) ;
                        

                                                             
BYTE db_select_bin01 (pCHAR, pBIN01, pCHAR);         
BYTE db_insert_bin01 (pBIN01, pCHAR);         
BYTE db_delete_bin01 (pBIN01, pCHAR);        
BYTE db_update_bin01 (pBIN01, pCHAR);
BYTE db_get_bin01_count( pCHAR, pCHAR );
BYTE db_get_bin01_gui_list( pBIN01 bin01_ptr, pBIN01_GUI_LIST Bin01List, pCHAR ErrorMsg); 
BYTE db_get_bin01_gui_list_by_type( pBIN01 bin01_ptr, pBIN01_GUI_LIST Bin01List, pCHAR ErrorMsg); 
BYTE db_get_bin01_service_list( pBIN01 bin01_ptr, pBIN01_LIST Bin01List, pCHAR ErrorMsg); 
BYTE db_check_binn_range(pCHAR card_num, pBIN01 binn_range, pCHAR ErrorMsg ); 
BYTE db_get_bin01_list( pBIN01_LIST Bin01List, pCHAR ErrorMsg );

BYTE db_select_hst01 (pCHAR, pHST01, pCHAR);         
BYTE db_insert_hst01 (pHST01, pCHAR);         
BYTE db_delete_hst01 (pHST01, pCHAR);
BYTE db_delete_hst01_by_date( pCHAR p_data,pCHAR ErrorMsg );        

BYTE db_get_hst01_list_by_file_name( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg);
BYTE db_get_hst01_list_by_file_name_key( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg);
BYTE db_get_hst01_list_by_user_name( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg);

void db_to_bin02 (BIN02* binn_range);
BYTE db_check_binn02_range(pCHAR card_num, pBIN02 binn_range, pCHAR ErrorMsg ) ;
BYTE db_update_acf01_for_deferred_account (pAUTH_TX	auth_tx, 
										   pACF01 acf01_ptr, 
										   pCHAR ErrorMsg) ;

//BYTE db_get_ptf01_gui_list( pCHAR File_Prefix, pPTF01_GUI_LIST Ptf01List, pCHAR ErrorMsg); 

#endif