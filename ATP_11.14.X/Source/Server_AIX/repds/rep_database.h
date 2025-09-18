#ifndef REP_DATABASE
#define REP_DATABASE

#include "app_info.h"
#include "equitdb.h"
#include "tx_dbstruct.h"
#include "nc_dbstruct.h"


BYTE db_get_tlf01_gui_list_dynamic ( pTLF01_LIMITS, pTLF01_GUI_LIST, pCHAR);  
BYTE db_get_mcf01_gui_list (pMCF01, pMCF01_REPORT_LIST, pCHAR ) ; 
BYTE db_get_ccf01_gui_list (pCCF01, pCCF01_REPORT_LIST, pCHAR ) ; 
BYTE db_get_paf01_gui_list (pPAF01, pPAF01_REPORT_LIST, pCHAR ) ; 

BYTE db_select_tlf01 ( pCHAR , pTLF01,   pCHAR );
BYTE db_select_emvraw( pCHAR , pEMV_RAW, pCHAR );
BYTE db_select_fguard( pCHAR , pFGUARD,  pCHAR );
BYTE db_select_fgraw ( pCHAR , pFG_RAW,  pCHAR );

void db_to_tlf01 ( pTLF01  );
void db_to_fguard( pFGUARD );
void db_to_mcf01_report ( pMCF01_REPORT_LIST );
void tlf01_to_db( pTLF01 tlf01_ptr );           /* ThoughtFocus Girija Y */
void ptsn01_to_db( pPTSN01 ptsn01_ptr );
void db_to_mra01( pMRA01 ptr_MRA01 );
void db_to_ftlf01( pFTLF01 ptr_ftlf01 );
void db_to_ptsn01 ( pPTSN01 ptsn01_ptr );
#endif      
