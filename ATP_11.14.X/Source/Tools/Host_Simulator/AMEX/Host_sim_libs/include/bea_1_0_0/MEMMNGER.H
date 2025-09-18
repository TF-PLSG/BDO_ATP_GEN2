/******************************************************************************
  
   Module:               memmnger.h
  
   Title:                XIPC Shared Memory Routines
  
   Description: 

   Application:          BP

   Authours:             Last modified by supriya.
   
   Revision History:     1.0

   $Log:   N:\PVCS\PTE\bea\headers\MEMMNGER.H  $
   
      Rev 1.0   08 Sep 1999 10:54:00   epriyev
   initial BEA load
   
      Rev 1.0   03 Feb 1999 10:41:02   jhapitas
   Intial Revision
   
      Rev 1.0   Dec 04 1998 14:44:04   MSALEH
   initial release

******************************************************************************/

#ifndef _MEMMNGER
#define _MEMMNGER

#define MEMACCESSERROR 1

LONG GetMemorySize();/* returns memsize or <0 if fails*/

LONG CreateXipcTable(INT NumRecords, INT RecordSize, pCHAR SectionName);/*>=0 OK;<0 if fails*/

LONG WriteTable(pCHAR SectionName, LONG TableSize, pBYTE TableList);/*same as above*/

LONG DeleteMemRecord(pCHAR TableName, LONG Offset, LONG RecordSize);/*same as above*/

LONG UpdateMemRecord(pCHAR TableName, LONG Offset, LONG RecordSize, pBYTE RecContent);/*same as above*/

LONG FindMemRecord(pBYTE RecordAddress, LONG Primary_Key_Len, LONG RecordSize, pCHAR TableName);
/* returns records offset if OK, -1 if NOT FOUND, <0 mem problems*/

LONG FindMemBinnRange(pBYTE CardNumber, pBYTE Type, pBYTE BinRecord);/*same as above*/

XINT ReadMemMonitorTable(pCHAR);/* returns address or NULL if fails*/

LONG WriteMemMonitorTable( pBYTE Data);

XINT ReadGenericTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state);

LONG WriteGenericTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state);

int  create_generic_table(pCHAR );/* if OK, 0 else -1 */

LONG GetSectionSize(pCHAR TableName);

LONG DestroyXipcTable(pCHAR SectionName);

#endif