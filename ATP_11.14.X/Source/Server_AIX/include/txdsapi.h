/*******************************************************************************
*  
* MODULE:      txdsapi.h
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: ATP library
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\TXDSAPI.H  $
   
      Rev 1.17   Jun 24 2005 09:33:42   dirby
   Added support for Fraudguard
   SCR 14421
   
      Rev 1.16   Dec 05 2003 08:26:28   dirby
   Added que_name as an input parameter to txdsapi_send_amex_record.
   This is because EB changes the service names of the Amex
   Network Controllers.  This value will be can be input on the GUI.
   SCR 1200
   
      Rev 1.15   Nov 21 2003 16:19:28   dirby
   Added function for Amex Online Updates.
   SCR 1200
   
      Rev 1.14   Mar 03 2003 11:09:36   dirby
   Modified function send_mcc_record to not have separate structures
   for request and response.  Combined them into a single structure.
   SCR 891
   
      Rev 1.13   Nov 14 2001 11:56:06   dirby
   Added support for JCB Online File Updates     SCR 251
   
      Rev 1.12   Jun 13 2001 13:52:22   dirby
   Reverted back to revision 1.10.  Revision 1.11 contains
   Base product stuff that is not in the current Equitable.
   
      Rev 1.11   Dec 01 2000 18:37:54   rmalathk
   updated for Realpay: UCF03, UCF03L, PCID, TLF01 and batch tables.
   
      Rev 1.10   Jan 18 2000 16:35:52   ddabberu
   removed c++ style comments
   
      Rev 1.9   Nov 16 1999 14:05:52   mbysani
    
   
      Rev 1.8   Nov 16 1999 10:02:42   mbysani
   Added 3 functions  
 
      Rev 1.7   Jun 04 1999 17:37:50   iarustam
   added new function find_bin
   
      Rev 1.5   Feb 24 1999 17:16:44   iarustam
   added logic for checking data_types
   
      Rev 1.4   Feb 20 1999 14:28:22   iarustam
   Added send_voice_auth_tran
   
      Rev 1.3   Feb 18 1999 17:01:22   iarustam
   Added mcc_record
   
      Rev 1.2   08 Feb 1999 15:53:06   dirby
   Added function to get batch for NCF30.
   
      Rev 1.1   Feb 03 1999 15:26:20   IGOLDFIE
   Added select_bch01_service_list and
   select_tlf01_service_list functions.
   
      Rev 1.0   03 Feb 1999 10:41:06   jhapitas
   Intial Revision
   
      Rev 1.0   Dec 04 1998 14:44:06   MSALEH
   initial release
*
*******************************************************************************/
#ifndef __TXDSAPI__
#define __TXDSAPI__



BYTE txdsapi_delete_record( pBYTE , int ,  BYTE , pCHAR  );


BYTE txdsapi_get_record( pBYTE , int , BYTE , pCHAR  );

BYTE txdsapi_insert_record( pBYTE , int , BYTE , pCHAR  );

BYTE txdsapi_update_record( pBYTE , int , BYTE , pCHAR  );


BYTE txdsapi_get_list (pBYTE , int , pBYTE , int , BYTE , BYTE , pCHAR );

BYTE txdsapi_get_service_list (pBYTE , int , pBYTE , int , BYTE , BYTE , pCHAR );

BYTE txdsapi_get_sch_list (pBYTE , int , pBYTE , int , BYTE , BYTE , pCHAR );

BYTE txdsapi_truncate_ntf01_table( BYTE, pCHAR );

BYTE txdsapi_get_num_records( pBYTE , BYTE , pCHAR  );

BYTE txdsapi_get_mcf01_cfg( pMCF01 mcf01, pCHAR error_msg );
BYTE txdsapi_get_dcf01_cfg( pDCF01 dcf01, pCHAR error_msg );
BYTE txdsapi_get_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg );
BYTE txdsapi_delete_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg );
BYTE txdsapi_update_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg );
BYTE txdsapi_insert_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg );

BYTE txdsapi_get_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg );
BYTE txdsapi_delete_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg );
BYTE txdsapi_update_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg );
BYTE txdsapi_insert_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg );

BYTE txdsapi_get_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg );
BYTE txdsapi_delete_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg );
BYTE txdsapi_update_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg );
BYTE txdsapi_insert_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg );
BYTE txdsapi_get_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg );
BYTE txdsapi_delete_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg );
BYTE txdsapi_update_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg );
BYTE txdsapi_insert_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg );
BYTE txdsapi_get_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg );
BYTE txdsapi_delete_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg );
BYTE txdsapi_update_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg );
BYTE txdsapi_insert_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg );
BYTE txdsapi_get_acf01_cfg( pACF01 pacf01, pCHAR error_msg );
BYTE txdsapi_delete_acf01_cfg( pACF01 pacf01, pCHAR error_msg );
BYTE txdsapi_update_acf01_cfg( pACF01 pacf01, pCHAR error_msg );
BYTE txdsapi_insert_acf01_cfg( pACF01 pacf01, pCHAR error_msg );
BYTE txdsapi_get_bch01_cfg( pBCH01 pbch01, pCHAR error_msg );
BYTE txdsapi_update_bch01_cfg( pBCH01 pbch01, pCHAR error_msg );
BYTE txdsapi_check_bch01_for_settlement(pBCH01 pbch01, pCHAR error_msg);
BYTE txdsapi_move_trans( pBCH10 bch10, pCHAR error_msg );
BYTE txdsapi_get_settle_tran_list( pBCH10 settle_info,	pBCH10_LIST settle_trans_list, pCHAR error_msg );
BYTE txdsapi_select_bch01_by_date(pBCH01 pbch01, pBCH01 bch01_rec,pCHAR error_msg);
BYTE txdsapi_return_bch01_delete_bch10( pBCH01 pbch01, pCHAR error_msg );
BYTE txdsapi_select_bch10_service_list(pBCH10 pbch10,pBCH10_LIST Bch10List, pCHAR error_msg);
BYTE txdsapi_select_bch11_service_list(pBCH11 pbch11,pBCH11_LIST Bch11List, pCHAR error_msg);
BYTE txdsapi_select_bch01_service_list(pBCH01 pbch01,pBCH01_LIST Bch01List, pCHAR error_msg);
BYTE txdsapi_select_tlf01_service_list(pTLF01 ptlf01,pTLF01_LIST Tlf01List, pCHAR error_msg);
BYTE txdsapi_check_bch01_for_settlement(pBCH01 pbch01, pCHAR error_msg);
BYTE txdsapi_update_bch01_cfg( pBCH01 pbch01, pCHAR error_msg );
BYTE txdsapi_update_bch01_record( pBYTE psStructurePtr, int nStructureSize, 
							BYTE app_data_type,
							pCHAR error_msg );
BYTE txdsapi_update_bch10_record( pBYTE psStructurePtr, int nStructureSize, 
							BYTE app_data_type,
							pCHAR error_msg );
BYTE txdsapi_select_ncf30_service_list(pNCF30,pNCF30_BATCH_LIST, pCHAR);
BYTE txdsapi_send_mcc_record( pMCC_ONLINE_UPDATES, BYTE, pCHAR );
BYTE txdsapi_send_voice_auth_tran( pAUTH_TX, BYTE, pCHAR  );
BYTE txdsapi_find_bin( pCHAR, pBIN01, BYTE, pCHAR error_msg );

BYTE txdsapi_delete_bch10_update_bch01( pBCH10 bch10, pCHAR error_msg );
BYTE txdsapi_delete_record_by_subtype2( pBYTE psStructurePtr, int nStructureSize, 
						  BYTE app_data_type, BYTE Subtype2,  pCHAR error_msg );
BYTE txdsapi_get_record_by_subtype2( pBYTE psStructurePtr, int nStructureSize, 
						       BYTE app_data_type, BYTE Subtype2, pCHAR error_msg );
BYTE txdsapi_send_all_data( pAUTH_TX psAuthTxPtr,  pCHAR error_msg );
BYTE txdsapi_send_jcb_record( pJCB_ONLINE_UPDATES psStructurePtr,
                              BYTE app_data_type, pCHAR error_msg );

BYTE txdsapi_send_amex_record( pAMEX_ONLINE_UPDATES psStructurePtr,
                               BYTE                 app_data_type,
                               pCHAR                que_name,
                               pCHAR                error_msg );

BYTE txdsapi_fraudguard( INT      mode,
                         pAUTH_TX p_auth_tx,
                         pCHAR    fg_data,
                         pLONG    fg_data_len,
                         pCHAR    error_msg );
#endif
