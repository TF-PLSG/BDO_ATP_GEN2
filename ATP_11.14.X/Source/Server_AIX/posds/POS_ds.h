#ifndef _TRANDS_H
#define _TRANDS_H
BYTE db_get_ucf01_count_by_group( pUCF02 ucf02_ptr, pCHAR crf_count, pCHAR ErrorMsg );
BYTE db_get_hst01_list_all( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
BYTE db_get_hst01_list_by_user( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
#endif
