#ifndef _DATABASE_RESPONSE
#define _DATABASE_RESPONSE

#include "basictyp.h"
#include "ptemsg.h" 

BYTE 		Process_Dataserver_OK_Message (pPTE_MSG p_msg_in) ;
BYTE		Process_Dataserver_ERROR_Message (pPTE_MSG p_msg_in) ;
BYTE		Process_Dataserver_NotFound_Message (pPTE_MSG p_msg_in) ;

BYTE Process_select_dcf01_db_OK (pBYTE pCurrent) ;
BYTE Process_select_dcf01_auto_init (pBYTE pCurrent);
BYTE Process_select_mcf01_db_OK (pBYTE pCurrent) ;
BYTE Process_select_bch10_db_OK (pBYTE pCurrent) ;
BYTE Process_select_bch20_db_OK (pBYTE pCurrent) ;
BYTE Process_select_bin01_db_OK (pBYTE pCurrent) ;
BYTE Process_select_mct01_db_OK (pBYTE pCurrent) ;
BYTE Process_select_bch01_db_OK (pBYTE pCurrent, BYTE subType2) ;
BYTE Process_select_tlf01_db_OK (pBYTE pCurrent) ;

BYTE Process_update_bch01_db_OK (BYTE) ;
BYTE Process_insert_bch10_db_OK (void) ;

BYTE Process_dcf01_db_not_found	(pBYTE pCurrent) ;
BYTE Process_mcf01_db_not_found	(pBYTE pCurrent) ;
BYTE Process_bch10_db_not_found	(pPTE_MSG p_msg_in, pBYTE pCurrent) ;
BYTE Process_bch20_db_not_found	(pPTE_MSG p_msg_in, pBYTE pCurrent) ;
BYTE Process_bin01_db_not_found	(pBYTE pCurrent) ;
BYTE Process_mct01_db_not_found	(pBYTE pCurrent) ;
BYTE Process_bch01_db_not_found	(pBYTE pCurrent, BYTE subtype2) ;
BYTE Process_tlf01_db_not_found	(pBYTE pCurrent) ;

BYTE Process_When_Bin_Range_Not_Found (void) ;
BYTE Process_When_Bin_Range_Found (void) ;
BYTE process_duplicate_txn( void );

BYTE Edit_DCF01_Record (DCF01 sDCF01) ;
BYTE Edit_MCF01_Record (MCF01 sMCF01) ;
BYTE Process_select_bch20_db_OK_incremental_preauth (pBYTE pCurrent);
BYTE Process_select_bch20_db_OK_preauth_void (pBYTE pCurrent);
BYTE Process_select_bch20_db_OK_preauth_total_amount (pBYTE pCurrent);
BYTE Process_select_bch20_preauth_void_db_not_found (void);
BYTE Process_select_bch20_db_OK_preauth_total_amount_not_found(void);
void Process_get_response_text_for_preauth_void(char *);

#define CUP_error_response_code_for_PREAUTH_COMPLETION_VALIDATION "05"

#endif
