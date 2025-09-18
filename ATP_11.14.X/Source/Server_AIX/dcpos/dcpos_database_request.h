#ifndef _DATABASE_REQUEST
#define _DATABASE_REQUEST

#include "dc_dbstruct.h"

BYTE Get_BIN01_Details (BYTE read_mode) ;
BYTE Get_TLF01_Details (BYTE  subtype2) ;
BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize) ;

BYTE Insert_TLF01_Details( BYTE subtype2 );
BYTE Retrieve_BIN01_Record_dcpos (void);

#endif
