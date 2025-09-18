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



//POS DATA for MC VISA AMEX JCB CUP 
#define DE60_1_SIZE    4
#define DE61_2_SIZE    11
#define MESSAGE_REPONSE_CODE_FEILD_60_1 "0000" // 
#define MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4
#define DE22_SIZE_AMEX 12
#define POS_ENTRY_NORMAL         0
#define POS_ENTRY_EMV_CHIP       1
#define POS_ENTRY_EMV_MAGSTRIPE  2
#define POS_ENTRY_EMV_FALLBACK   3
#define  SERVICE_CODE_SIZE_PLUS_1        4
#define  ECOM_POS_CC   "59"
#define ID_MOBILE_POS_ACCP_DEVICES 9 //Identification of Mobile POS Acceptance Devices
#define  AUTO_FUEL_MCC   "5542" /* Automated fuel dispenser Merchant cat code */
#define  MCC_RANGE2_LOW   5960
#define  MCC_RANGE2_HIGH  5969


/* Industry Codes */
#define  RETAIL      100
#define  RESTAURANT  101
#define  MAIL_PHONE  500
#define  ECOMMERCE   700
#define  RPS         800
#define  CASH        900
#define  ATM         901
#define  QUASI_CASH  902

#define MESSAGE_REPONSE_CODE_FEILD_60_1 "0000" // #define MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4
#define MESSAGE_REPONSE_CODE_FEILD_60_1_LEN 4



#endif
