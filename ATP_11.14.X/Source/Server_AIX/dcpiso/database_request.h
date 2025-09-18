#ifndef _DATABASE_REQUEST
#define _DATABASE_REQUEST

#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#define MAX_NCF01_RECS    20

BYTE Get_DCF01_Details (BYTE  subtype2) ;
BYTE Get_MCF01_Details (void) ;
BYTE Get_BIN01_Details (BYTE read_mode) ;
BYTE Get_MCT01_Details (void) ;
BYTE Get_TLF01_Details (BYTE  subtype2) ;

BYTE Get_BCH01_Details (BYTE) ;
BYTE Get_BCH10_Details (BYTE) ;
BYTE Get_BCH20_Details (BYTE) ;

BYTE look_for_duplicate_txn( void );
BYTE Retrieve_BIN01_Record (void) ;
BYTE Insert_BCH10_Details_For_Batch_Upload (void) ;
BYTE Update_BCH01_Details (BCH01 sBCH01, BYTE subType1, BYTE subType2) ;
BYTE Update_BCH20_Details_for_OFFLINE_Sale( BYTE subType2);
BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize) ;
BYTE Update_All_Data (void) ;
BYTE Insert_TLF01_Details( BYTE subtype2 );
INT is_batch_duplicate();
void Process_Duplicate_Batch(INT);
extern INT  Process_Settlement() ;
BYTE Get_BCH20_Details_for_incremental_Preauth (BYTE retrievalType);

#endif
