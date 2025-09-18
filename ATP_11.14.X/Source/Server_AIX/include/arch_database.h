/******************************************************************************
  
   Module: arch_database.h
  
   Title: Transaction Server Dataserver DB Routines
  
   Description: 

   Application:          Equitable

   Authours:             Last modified by supriya.
   
   Revision History:     1.0
    
   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\arch_database.h  $
   
******************************************************************************/

#ifndef _DATABASE
#define _DATABASE

#include "app_info.h"
#include "equitdb.h"
#include "util_dbstruct.h"


BYTE db_get_tlf01_gui_list_dynamic ( pARCHIVE_TLF01_LIMITS, pARCHIVE_TLF01_GUI_LIST, pCHAR);  
BYTE db_get_mcf01_gui_list (pMCF01, pMCF01_REPORT_LIST, pCHAR ) ; 
BYTE db_get_ccf01_gui_list (pCCF01, pCCF01_REPORT_LIST, pCHAR ) ; 
BYTE db_get_paf01_gui_list (pPAF01, pPAF01_REPORT_LIST, pCHAR ) ; 

BYTE db_select_tlf01_archive ( pCHAR , pARCHIVE_TLF01,   pCHAR );
BYTE db_select_emvraw( pCHAR , pEMV_RAW, pCHAR );
BYTE db_select_fguard( pCHAR , pFGUARD,  pCHAR );
BYTE db_select_fgraw ( pCHAR , pFG_RAW,  pCHAR );

void db_to_tlf01 ( pARCHIVE_TLF01  );
void db_to_fguard( pFGUARD );
void db_to_mcf01_report ( pMCF01_REPORT_LIST );
void tlf01_to_db( pARCHIVE_TLF01 tlf01_ptr );           



#endif
