#ifndef _TRANDS_H
#define _TRANDS_H
BYTE db_get_ucf01_count_by_group( pUCF02 ucf02_ptr, pCHAR crf_count, pCHAR ErrorMsg );
BYTE db_get_hst01_list_all( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
BYTE db_get_hst01_list_by_user( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
void create_system_date( pBYTE system_date );

#define HEADER_SIZE        498
#define DEFAULT_BIN01_TABLE_SIZE "5000"
#define SHM_TABLE_SIZE_LEN 4

#define FILLERCHAR		' '
#define SEPERATORCHAR	'|'

#define HCTS2 1

void WriteDSVHeader();
BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** hstptr, char *header);
long int findHST01FileSize(char file_name[]);
BOOLEAN OpenDSV_WriteHST01(HST01 *hst01);
void get_HST01_file_name_path(void );
void trands_create_Error_Filename();
void Mask_sensitive_fields(pHST01 hst01, CHAR * masked_field_Previous, CHAR *masked_field_Current);

#endif
