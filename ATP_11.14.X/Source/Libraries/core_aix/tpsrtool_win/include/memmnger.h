/******************************************************************************
  
   Module:               memmnger.h
  
   Title:                XIPC Shared Memory Routines
  
   Description:          Prototypes for ATP Library XIPC memory functions

   Application:          Equitable Bank

   Authours:             Last modified by supriya.
   
   Revision History:     1.0

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\MEMMNGER.H  $

      Rev 1.5   May 25 2007 DIrby
   Added function prototypes to access the Auth Number Table in memory.
   SCR 12299

      Rev 1.4   Mar 01 2007 10:39:46   DIrby
   Added function prototype to insert a record into SQL01.
   SCR 24081
   
      Rev 1.3   Feb 13 2007 11:33:44   DIrby
   Added function prototypes for XIPC shared memory functions
   relating to table SQL01.
   SCR 24081
   
      Rev 1.2   Sep 13 2002 16:16:14   dirby
   Added function prototypes for BON01 table, which was placed
   into shared memory.
   SCR 804
   
      Rev 1.1   Jan 18 2000 16:35:50   ddabberu
   removed c++ style comments
   
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
XINT ReadBON01Table(  pCHAR TableName, pBON01 p_bon01 );
LONG WriteBON01Table( pCHAR TableName, pBON01 p_bon01 );

INT CreateSQL01_Table( void );
INT WriteSQL01_Table( pSQL01 p_sql01 );
INT ReadSQL01_SingleRec_Table( pSQL01 p_sql01 );
INT ReadSQL01_MultipleRecs_Table( pBYTE key, pSQL01_LIST p_sql01_list );
INT InsertSQL01_Table( pBYTE p_sql01 );

INT WriteAuthTable( pCHAR auth_number, INT lock );
INT ReadAuthTable ( pCHAR auth_number, INT lock );

#endif
