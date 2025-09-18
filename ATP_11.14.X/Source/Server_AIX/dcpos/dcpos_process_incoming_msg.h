#ifndef _PROCESS_INCOMING_MSG
#define  _PROCESS_INCOMING_MSG

#include "basictyp.h"

BYTE Process_Incoming_Message_From_Terminal (pPTE_MSG p_msg_in) ;
BYTE Copy_Message_Data_To_Global_Buffer (pCHAR indata, LONG msglen) ;
BYTE Copy_Message_Fields_From_Global_Structure_To_Auth_Tx (void) ;
BYTE Get_Transaction_Type_From_Global_Structure (void) ;
BYTE Populate_Sections_Of_Auth_Tx (BYTE tx_type, pPTE_MSG p_msg_in) ;
BYTE Process_Terminal_Statistics_Transaction (BYTE) ;
BYTE Generate_Transaction_Date_In_YYYYMMDD_format (BYTE* strTranDate, 
												   BYTE* datestr1) ;
BYTE  Process_Debit_Magstripe_Transaction_Flow(void);
BYTE  Process_Debit_EMV_Transaction_Flow(void);
BYTE  Process_DCC_Sale_Transaction_Flow(void);
BYTE  Process_Dinner_Transaction_Flow(void);
BYTE  Process_Fleet_Transaction_Flow(void);
BYTE  Process_CUP_Magstripe_Transaction_Flow(void);
BYTE  Process_Dual_Branded_Debit_Transaction_Flow(void);
BYTE  Process_When_Debit_Bin_Range_Found_Dcpos(void);
BYTE  Process_When_Credit_Bin_Range_Found_Dcpos(void);
BYTE  Process_AMEX_TRANSACTION_ON_BIN_NII_Flow(void);
BYTE  Process_No_Bin_Found_Flow(void);


#endif
