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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\cd_sett_file\settle.h  $
   
      Rev 1.17   Sep 11 2006 17:24:30   dirby
   Modified the raffle (Cash Bonus) program to look for multiple
   winner tickets per transaction, and to get the reward amount
   from BON03, not BON01.
   SCR 22835
   
      Rev 1.16   Feb 17 2006 11:21:54   dirby
   Added a function prototype to select currency code and device
   type from MCF01 and DCF01.
   SCR 19844
   
      Rev 1.15   Dec 12 2005 14:37:06   dirby
   Modified to use tran_date when processing the exception file
   transactions instead of open_date.  Also, do not include voids
   in the exception file.
   SCR 18910
   
      Rev 1.14   Oct 03 2005 16:32:14   dirby
   Modified to support exception processing - that is, settle BCH01
   and BCH10 records that are more than 45 days old.
   SCR 15009
   
      Rev 1.13   Jul 01 2005 13:57:24   dirby
   Added function prototype for 'get_reload_confirm'.
   SCR 16303
   
      Rev 1.12   Jun 09 2005 08:59:22   dirby
   Changed the functions that format credit and debit detail records
   to return true or false.
   SCR 15748
   
      Rev 1.11   Sep 30 2004 15:51:54   lmitchel
   Ver 4.4.0.2 - Added a dump file to write batch or transaction information for unsettled batches to.
   
      Rev 1.10   Oct 03 2003 10:19:34   lmitchel
   Ver: 4.2.0.5 - Added RBS37 as dedicated rollback segment for the settlement process.  
   
      Rev 1.9   Sep 25 2003 20:26:00   lmitchel
   SCR1212 : Ver4.2.0.4
   Removed while loop design to select SCH_MAX_SIZE number of bch01 records at a time into an open cursor.  If there more than SCH_MAX_SIZE batches, only the first set of batches were seclected on each iteration of the loop.  All BCH01 batches to be setteld are now selected into the open cursor.
   
      Rev 1.8   Jan 26 2000 16:12:36   farad
   (farad 1-26-2000) fixed bug #326.
   
      Rev 1.7   Jan 26 2000 14:56:12   farad
   Fixed bug #298 (farad on 1-25-2000)
   Modified the select statments such that they do not include voids. 
   
      Rev 1.6   Nov 19 1999 15:12:14   dperson
   Implemented an optional parameter "File to Process"
   and modified the output file name to include the system time
   
   
      Rev 1.5   Nov 18 1999 12:46:12   dperson
   Incorporated code to handle an additional 3 selection parameters:
     cutoff date, cutoff time, and number of hours to look back (offset)
   Incorporated code to use BCH11 if a batch has been released
   Cleaned up code a bit; alphabetized functions
   
   
      Rev 1.4   Jul 26 1999 15:02:06   apinto
   Merchant ID is 16 digits with leading zeros
   Void Sale Eliminated-verfied-OK
   Transaction Code-verified-OK
   Julian Date-verified-OK 
   
      Rev 1.3   Jul 08 1999 09:47:54   dperson
   Added function prototype for calc_bonus_amount
   Removed function prototype for trim_string
   
      Rev 1.2   Jun 24 1999 14:55:42   dperson
   Added function prototypes.
   
      Rev 1.1   Apr 08 1999 17:02:30   npogosya
   Bug fixes, changes to write to 2 files:credit and debit
   
      Rev 1.0   Feb 15 1999 13:34:16   IGOLDFIE
   initial release
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
BOOLEAN bch01_handler(void);
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
                             pDCF01  dcf01_ptr, pCHAR CUP_Merchaant_ID,pCHAR  ErrorMsg );


BYTE db_get_bon03_service_list( pCHAR       tran_id,
                                pBON03_LIST bon03_list,
                                pCHAR       ErrorMsg );
void settle_get_error_warning_file_name_path(void );
INT settle_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail);
void settle_create_Error_Filename(  );
void settle_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);

