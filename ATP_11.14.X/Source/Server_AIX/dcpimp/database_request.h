#ifndef _DATABASE_REQUEST
#define _DATABASE_REQUEST

#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#define DOLLAR_ACCOUNT_TYPE       "05"
#define PESO_ACCOUNT_TYPE         "06"
#define MAX_NCF01_RECS    20

// currency codes

#define PESO_CURRENCY "608"
#define USDOLLAR_CURRENCY "840"

#define BALANCE_ACCOUNT_TYPE_IS_DEFERRED 0
#define BALANCE_ACCOUNT_TYPE_IS_OTHER    1



BYTE Get_DCF01_Details (BYTE  subtype2) ;
BYTE Get_MCF01_Details (void) ;
BYTE Get_BIN01_Details (BYTE read_mode) ;
BYTE Get_MCT01_Details (void) ;
BYTE Get_TLF01_Details (BYTE  subtype2) ;

BYTE Get_BCH01_MP_Details (BYTE) ;
BYTE Get_BCH10_Details (BYTE) ;
BYTE Get_BCH20_Details (BYTE) ;
BYTE Get_CCF02P_Details (void) ;
BYTE Get_MPPARA01_Details (char* Product_code); 

BYTE Get_MGF01_Details (void) ;
BYTE Get_ACF01_Details (void);
BYTE Get_CCF03P_Details (void);
BYTE Get_MCF03P_Details (void);
BYTE Get_CCF02P_Details_For_RT (void);
BYTE Get_ACF01_Details_For_RT (void);
BYTE Get_PAF01_Details (void);
BYTE Get_PAF01_Details_For_RT (void);

#ifdef HCTS3
BYTE Retrieve_BIN01_Record_Online (void);
BYTE Get_BIN01_Details_Online (BYTE);
BYTE Dcpimp_process_when_bin_range_found_online();
BYTE Dcpimp_Retrieve_CAF_Profile_Online(BYTE*, BYTE*);
BYTE Dcpimp_Get_CAF01_Details_Online (BYTE, BYTE*, BYTE*);
BYTE Dcpimp_process_when_caf_found_online();
#endif

BYTE look_for_duplicate_txn( void );
BYTE Retrieve_BIN01_Record (void) ;
BYTE Insert_BCH10_Details_For_Batch_Upload (void) ;
BYTE Update_BCH01_MP_Details (BCH01_MP sBCH01_MP, BYTE subType1, BYTE subType2) ;
BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize) ;
BYTE Update_All_Data (void) ;
BYTE Insert_TLF01_Details( BYTE subtype2 );
INT is_batch_duplicate();
void Process_Duplicate_Batch(INT);
extern INT  Process_Settlement() ;

#endif
