#ifndef _TXUTILS
#define _TXUTILS
#include "basictyp.h"

LONG Txutils_Calculate_Julian_Date (BYTE* yyyymmdd) ;
void Txutils_Calculate_Gregorian_Date(LONG julian, pCHAR yyyymmdd);
BOOLEAN TxUtils_Send_Msg_To_Operator (int nWriteToSystemMonitor, BYTE* strMessage, 
									  int nWriteToEventLog, int nEventLogMsgType, 
									  BYTE* strFunction, BYTE nSeverity, 
									  BYTE* strMessageType, BYTE* strTerminal, 
									  BYTE* strCardNumber, BYTE* strMerchant);

#endif