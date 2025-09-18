/*********************************************************************************

                MERCHBATCH - EFTPOS - UTILITY - THOUGHTFOCUS 

/********************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  



#define SWIPE_CARD   "0022"
#define MANUAL_ENTRY "0012"


/* ThoughtFocus - Chandra */

/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

#define EPOS_POS_DATA_SIZE 12

// Length for printing Coustmer identifier 
#define PERCH_ID_LENGTH 32
#define CUSTOM_IDENTIFIER_TYPE_LENGTH 6 //Custom Identifier Type

/* E-commerce POS Condition Code */   
#define  ECOM_POS_CC   "59"  
/* Duplicate Extraction flags - TF Phani*/
#define ALLOW_DUPLICATES		0
#define DONT_ALLOW_DUPLICATES	1
/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"
/*** Common structure for current terminal batch ***/

typedef struct
{
   CHAR batch_number                      [7];
   CHAR terminal_id                       [9];
   CHAR merchant_id                       [16];
   CHAR open_date                         [9];
   CHAR open_time                         [7];
   CHAR number_credit_tran_for_batch      [11];
   CHAR number_debit_tran_for_batch       [11];
   CHAR totals_credit_for_batch           [16];
   CHAR totals_debit_for_batch            [16];

     
}  TC_TOTALS ;

typedef struct
{
   BYTE  num_returned     [5];
   EMV_RAW batch_list       [BATCH_SERVICE_LIST_SIZE];
}EMV_LIST,*pEMV_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   TLF01 batch_list       [BATCH_SERVICE_LIST_SIZE];
}TLOG01_LIST,*pTLOG01_LIST;

/*** Function Prototypes ***/
void MainProcessor(void);
LONG calc_bonus_amount( pBON03_LIST bon03_ptr );
void calc_julian_day(char date[], char julian_str[]);
INT  create_credit_detail(void);
INT  create_credit_detail_arc(void);
INT  create_debit_detail(void);
INT  create_debit_detail_arc(void);
void create_headers(void);
void create_trailers(void);
/* void create_raffle_detail(LONG);  
void create_raffle_detail_arc(LONG);     -  ThoughtFocus - Chandra - Not Needed for New Requirement - EFTPOS */
BOOLEAN bch01_handler(void);
/* BOOLEAN create_raffle_header(pBON03_LIST); */  /* ThoughtFocus - Chandra - Not Needed for New Requirement - EFTPOS */
BOOLEAN create_settle_files(void);
void display_settle_info(void);
void EndProcess(void);
BOOLEAN get_currency_code(void);
BOOLEAN open_credit_file(void);
BOOLEAN open_debit_file(void);
BOOLEAN open_empty_raffle_file(void);
/* BOOLEAN open_raffle_file(void); */  /* ThoughtFocus - Chandra - Not Needed for New Requirement - EFTPOS */
void returnCurrentTime(char *currentTime);
void Rj_with_lead_zeros(pCHAR str, INT FieldWidth);
void set_date(void);
BOOLEAN write_to_credit_settle_file(void);
BOOLEAN write_to_debit_settle_file(void);
/* T F Phani */
void Get_duplicate_restrict_flag_from_ini( void );
BYTE db_update_bch01_batch_Status_After_Writing_File(pBCH01 bch01_ptr, pCHAR ErrorMsg);

/* BOOLEAN write_to_raffle_file(void); */  /* ThoughtFocus - Chandra - Not Needed for New Requirement - EFTPOS */
BYTE db_get_tlf_log_service_list( pTLF01 tlf01_ptr,
                                pTLOG01_LIST tlog01_list,
                               pCHAR ErrorMsg );

void    write_to_dump_file(CHAR *);
extern BYTE db_set_rollback_segment_online(pCHAR );
extern BYTE db_set_rollback_segment_offline(pCHAR );
void   get_reload_confirm( pBCH10 bch10_ptr );

void    get_date( pCHAR time_date, pCHAR date_str );
INT     db_process_bch10_records( pCHAR ErrorMsg );
INT     db_insert_bch11_delete_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg );
BYTE    db_select_curr_code( pCHAR   curr_code, pCHAR  device_type, 
                             pDCF01  dcf01_ptr, pCHAR  ErrorMsg );

BYTE db_get_bon03_service_list( pCHAR       tran_id,
                                pBON03_LIST bon03_list,
                                pCHAR       ErrorMsg );

BYTE db_get_emv_service_list( pBCH10 bch10_ptr, pEMV_LIST Emv_List,pCHAR ErrorMsg );
BYTE db_get_emv_arc_service_list( pBCH11 bch11_ptr, pEMV_LIST Emv_List,pCHAR ErrorMsg );
BYTE db_get_bch01_service_list1( pBCH01 bch01_ptr,
                                pBCH01_LIST Bch01_List,
                                pCHAR ErrorMsg );
void parseEmvDataToSegment(INT elength,  CHAR emv_seg13[], CHAR emv_seg14[], CHAR emv_seg15[]);



