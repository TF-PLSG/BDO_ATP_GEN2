#include "basictyp.h"
#include "ptetime.h"
int strEqual (char* str1, char* str2) ;
void strTrim (char* strSource, char* strDestination) ;
int Convert_Amt_To_Auth_Tx_String_Format 
                              (double nAmount, char* strAmount) ;
void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character) ;

void create_rrn (pCHAR rrn) ;
void get_date (pCHAR time_date, pCHAR date_str) ;
void get_time (pCHAR time_date, pCHAR time_str) ;
void calc_julian_day (pCHAR date_str, pCHAR julian_str) ;
BYTE IsFieldNumeric (char str[]) ;
BYTE IsFieldBlank (char str[], int len) ;
BYTE IsFieldNull (char str[]) ;
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
INT     isnum(pCHAR string);

# define SECONDS_IN_30_YRS 946080000

void write_to_txn_file( void );
INT  send_transaction_statistics( pPTE_MSG );
INT  reset_transaction_statistics( pPTE_MSG );
INT  is_txn_voice_from_eft(void);
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1, 
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );
INT calc_seconds( pCHAR time_str);
INT get_timediff(pCHAR current_datetime, pCHAR old_datetime);
INT isalphanumSpace(pCHAR string, INT length);
BYTE dcpos_Is_DCC_terminal (void);


BYTE Eftpos_service_code[4];       //Girija GB
BYTE Eftpos_pos_condition_code[3]; //Girija GB
