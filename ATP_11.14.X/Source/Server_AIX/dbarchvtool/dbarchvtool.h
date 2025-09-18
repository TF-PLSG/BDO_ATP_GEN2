
/* Macro definitions */
#define   MAX_APPNAME_SIZE   10
#define   MAX_FILENAME_SIZE  256
#define   MAX_INI_INFO_SIZE  30
#define   DBARCHVTOOL_INI "dbarchvtool.ini"
#define   TABLE_LIST_SECTION "TABLES_LIST"
#define   MAX_TABLES  	50
#define   MAX_VALID_YR	9999
#define   MIN_VALID_YR	1800
/* USER INPUT STATUS */
#define  INVALID  0
#define  VALID    1


/* DB ERROR CODES*/
#define  TABLE_NOT_EXIST -942


/* Function declaration*/

INT isnum(pCHAR in_str);
void write_into_log_file(pCHAR Error_Buf);
INT validate_LookBack_days(INT);
INT isLeap(int year);

extern long Txutils_Calculate_Julian_Date(BYTE * );
/* Tables list structure*/

typedef struct
{
   BYTE   table_name[50];
} TABLES_LIST ,pTABLES_LIST ;
