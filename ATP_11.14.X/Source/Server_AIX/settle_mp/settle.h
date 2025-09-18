/*******************************************************************************
*  
* MODULE:           settle.h
*
* Copyright (c) 2006, Hypercom, Inc. All Rights Reserved.   
*  
* TITLE:            Visa Outgoing Center Transaction files creator . 
*  
* DESCRIPTION:      This module allows to create Settlement files
*                   for particular day and time.  
*
* APPLICATION:      cd_settle_file.exe
*
* AUTHOR:           Irene Goldfild
*
* REVISION HISTORY:
*
*******************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  



#define SWIPE_CARD   "0022"
#define MANUAL_ENTRY "0012"

#define EXCEPTION_BASE_NAME  "no_exceptions"
#define EXCEPTION_DAYS_BACK  45


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


/*** Function Prototypes ***/
void MainProcessor(void);
LONG calc_bonus_amount( pBON03_LIST bon03_ptr );
void calc_julian_day(char date[], char julian_str[]);
INT  create_credit_detail(void);
INT  create_credit_detail_arc(void);
INT  create_debit_detail(void);
INT  create_debit_detail_arc(void);
void create_headers(void);
void create_raffle_detail(LONG);
void create_raffle_detail_arc(LONG);
BOOLEAN bch01_mp_handler(void);
BOOLEAN create_raffle_header(pBON03_LIST);
BOOLEAN create_settle_files(void);
void display_settle_info(void);
void EndProcess(void);
BOOLEAN get_currency_code(void);
BOOLEAN open_credit_file(void);
BOOLEAN open_debit_file(void);
BOOLEAN open_empty_raffle_file(void);
BOOLEAN open_raffle_file(void);
void returnCurrentTime(char *currentTime);
void Rj_with_lead_zeros(pCHAR str, INT FieldWidth);
void set_date(void);
BOOLEAN write_to_credit_settle_file(void);
BOOLEAN write_to_debit_settle_file(void);
BOOLEAN write_to_raffle_file(void);
BOOLEAN write_to_except_settle_file(void);
void    write_to_dump_file(CHAR *);
extern BYTE db_set_rollback_segment_online(pCHAR );
extern BYTE db_set_rollback_segment_offline(pCHAR );
void   get_reload_confirm( pBCH10 bch10_ptr );

void    process_exception_txns( void );
void    get_date( pCHAR time_date, pCHAR date_str );
BOOLEAN open_exception_file( void );
INT     store_detail_except_record( pBCH10 p_bch10 );

INT     db_get_except_record_cnt( pCHAR ErrorMsg );
INT     db_process_bch10_records( pCHAR ErrorMsg );
INT     db_process_bch10_except_records( pCHAR ErrorMsg );
INT     db_insert_bch11_delete_bch10( pBCH10 bch10_ptr, pCHAR ErrorMsg );
BYTE    db_select_curr_code( pCHAR   curr_code, pCHAR  device_type, 
                             pDCF01  dcf01_ptr, pCHAR  ErrorMsg );

BYTE db_get_bon03_service_list( pCHAR       tran_id,
                                pBON03_LIST bon03_list,
                                pCHAR       ErrorMsg );
void settle_mp_get_error_warning_file_name_path(void );
INT settle_mp_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail);
void settle_mp_create_Error_Filename(  );
void settle_mp_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
