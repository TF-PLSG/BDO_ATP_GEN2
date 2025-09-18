#ifndef _BATCH_UPLOAD
#define _BATCH_UPLOAD
#include "basictyp.h"
#include "dc_dbstruct.h"

BYTE Process_Batch_Upload (void) ;
BYTE Get_Transaction_Type_For_Original_Transaction (void) ;

BYTE If_Transaction_Total_Equals_Batch_Total (BCH01) ;
BYTE Process_When_Transaction_Total_Equals_Batch_Total (BCH01) ;
BYTE Process_When_Transaction_Total_Does_Not_Equal_Batch_Total (BCH01) ;


#endif
