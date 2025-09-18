/******************************************************************************

   Module:               memmnger.c

   Title:                Mem Manager

   Description: 

   Application:          BP

   Authours:             Last modified by supriya.
   
   Revision History:     1.0 

   $Log:   N:\PVCS\PTE\Equitable\App_lib\Source\MEMMNGER.C  $
   
      Rev 1.7   Sep 13 2002 16:18:00   dirby
   Added the read and write functions for BON01 table - it is now
   in shared memory.
   SCR 804
   
      Rev 1.6   Jul 11 2002 11:27:46   dirby
   Modified to clean structure mcrd_tbl in WriteGenericTable so
   that there is no garbage in the shared memory table.
   SCR 813
   
      Rev 1.5   Jan 07 2000 13:06:52   dirby
   Added a line in WriteGenericTable to initialize
   the buffer before populating it.
   
      Rev 1.4   Nov 16 1999 14:13:18   mbysani
    
   
      Rev 1.3   Nov 16 1999 11:19:10   mbysani
   Added 3 functions
   
      Rev 1.3   Nov 16 1999 09:50:40   mbysani
    
   
      Rev 1.2   Sep 01 1999 13:47:02   egable
   Modified routines in memmnger.c, checked for return codes
   Change function that writes the OPRMON data to the shared memory segment on startup
   These changes originally made by Sanjoy Dasgupta for BP.
   
      Rev 1.1   Feb 02 1999 15:36:24   IGOLDFIE
   Changed include files
   
      Rev 1.0   01 Feb 1999 16:24:36   jhapitas
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:58:16   MSALEH
   initial release

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "pteipc.h"
#include "ntutils.h"

//#include "database.h"

#include "tx_database.h"
#include "dc_database.h"
#include "nc_database.h"

#include "memmnger.h"

PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;


/*************************************************************************************/
// Retrieves xipc memsys poolsize
/*************************************************************************************/
LONG GetMemorySize()
{
   XINT ReturnCode;
   MEMINFOSYS InfoSys;

   ReturnCode = MemInfoSys(&InfoSys);
   if(ReturnCode < 0)
      return(ReturnCode);
   return(InfoSys.MemPoolSizeBytes);
}

/*************************************************************************************/
// Create Table
/*************************************************************************************/
LONG CreateXipcTable(INT NumRecords, INT RecordSize, pCHAR SectionName)
{
   XINT Rt, Ret;
   LONG TableSize;
   char strError[256] = {0} ;

   Rt = MemAccess(SectionName);

   if (Rt < 0)
   {
		sprintf (strError, "Inside CreateXIPCTable - first function (not really an error): MemAccess Return %d SectionName is %s\n", Rt, SectionName) ;
		LogEvent( strError, WARN_MSG );
   }
   if(Rt >= 0)
   {
      
      Ret = MemDestroy(Rt);
      if(Ret < 0)
	  {
		  sprintf (strError, "Inside CreateXIPCTable: MemDestroy Return %d, SectionName is %s\n", Ret, SectionName) ;
		  LogEvent( strError, WARN_MSG );
          return(Ret);
	  }
   }
   TableSize = (NumRecords + 2) * (RecordSize + 1);
   Rt = MemCreate(SectionName, TableSize);
   if(Rt < 0)
   { 
	   sprintf (strError, "Inside CreateXIPCTable: MemCreate Return %d SectionName is %s TableSize is %ld\n", Rt, SectionName, TableSize) ;
	   LogEvent( strError, WARN_MSG );
      return(Rt);
   }	   
   Rt = MemAccess(SectionName);
   if(Rt < 0)
   {
	   sprintf (strError, "Inside CreateXIPCTable: MemAccess Return %d SectionName is %s (this is an error)\n", Rt, SectionName) ;
	   LogEvent( strError, WARN_MSG );
      return(Rt);
   }
   Ret = MemSecDef(MemSection(Rt, 0L, TableSize));

   if(Ret >= 0)
   {
	   
	   return(TableSize);
   }
   else
   {
	   sprintf (strError, "Inside CreateXIPCTable: MemSecDef Return %d \n", Ret) ;
	   LogEvent( strError, WARN_MSG );
      return(Ret);
   }
}

/*************************************************************************************/
// Write Table
/*************************************************************************************/
LONG WriteTable(pCHAR SectionName, LONG TableSize, pBYTE TableList)
{
   XINT Rt, Ret, result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize, Current_Offset, Offset;
   INT  nRec, i;
   pBYTE p_current;
   char strError [256] = {0} ;

   Offset = 0L;
   Rt = MemAccess(SectionName);
   if(Rt < 0)
   {
	   sprintf (strError, "Inside WriteTable: MemAccess returns %d, SectionName is %s\n", Rt, SectionName) ;
	   LogEvent (strError, WARN_MSG) ;
      return(Rt);
   }
   /* Added by SDG */
   MemInfo.SListOffset = MemInfo.WListOffset = 0; 

   Ret = MemInfoMem(Rt, &MemInfo);

   if (Ret < 0)
   {
		sprintf (strError, "Inside WriteTable: MemInfoMem returns %d SectionName is %s\n", Ret, SectionName) ;
		LogEvent (strError, WARN_MSG) ;
   }
   
   SectionSize = MemInfo.Size;

   	p_current = TableList + 5;
	nRec = atoi(TableList);


	
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);

   if (Ret < 0)
   {
	   sprintf (strError, "Inside WriteTable: MemLock returns %d, SectionName is %s\n", Ret, SectionName) ;
	   LogEvent (strError, WARN_MSG) ;
   }

   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   if (Ret < 0)
   {
	   sprintf (strError, "Inside WriteTable: MemWrite returns %d SectionSize is %ld\n", Ret,SectionSize) ;
	   LogEvent (strError, WARN_MSG) ;
   }
   for(i = 0; i < nRec; i++)
   {
      Ret = MemWrite(Rt, Current_Offset + 1, TableSize, p_current, MEM_WAIT);
      if(Ret < 0)
      {
         //Unlock before return in case of error

	     sprintf (strError, "Inside WriteTable: MemWrite returns %d\n", Ret) ;
	     LogEvent (strError, WARN_MSG) ;
   
         result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
		 if (result < 0)
		 {
			 sprintf (strError, "Inside WriteTable: MemUnlock returns %ld\n", result) ;
	         LogEvent (strError, WARN_MSG) ;
		 }

         return(Ret);
         break;
      }
      Current_Offset = Current_Offset + TableSize + 1;
      p_current = p_current + TableSize;
   }
   Ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if (Ret < 0)
   {
			 sprintf (strError, "Inside WriteTable: MemUnlock returns %d\n", Ret) ;
	         LogEvent (strError, WARN_MSG) ;
   }
   return(0L);
}

/*************************************************************************************/
// Delete Mem Record
/*************************************************************************************/
LONG DeleteMemRecord(pCHAR TableName, LONG Offset, LONG RecordSize)
{
   XINT    Rt, Ret;
   SECTION TempSec, RetSec;
   BYTE    delete_flag = 1;

   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, RecordSize + 1),
                                  MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, Offset, 1L, &delete_flag, MEM_WAIT);
   Ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, RecordSize + 1),
                                  MEM_EOL), &RetSec);
   return(0);
}

/*************************************************************************************/
// Update Mem Record
/*************************************************************************************/
LONG UpdateMemRecord(pCHAR TableName, LONG Offset, LONG RecordSize, pBYTE RecContent)
{
   XINT    Rt, Ret;
   SECTION TempSec, RetSec;

   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, RecordSize + 1),
                                  MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, Offset + 1, RecordSize, RecContent, MEM_WAIT);
   Ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, RecordSize + 1),
                                  MEM_EOL), &RetSec);
   return(0);
}

/*************************************************************************************/
// Find record
/*************************************************************************************/
LONG FindMemRecord(pBYTE RecordAddress, LONG Primary_Key_Len, LONG RecordSize, pCHAR TableName)
{
   BOOLEAN MatchFound = false;
   XINT    Rt, Ret;
   char    MemPrimaryKey[PRIMARY_KEY_MAX_LEN + 1];
   LONG    CurrentPosition = 0L;
   LONG    SectionSize;
   pBYTE   p_current;
   MEMINFOMEM MemInfo;
   BYTE Zeroflg = 0;

   Rt = MemAccess(TableName);

   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   if(Rt < 0)
      return(Rt);
   while( !MatchFound )
   {
      
      if((CurrentPosition + RecordSize + 1) > SectionSize)
         break;
      memset(MemPrimaryKey, 0, sizeof(MemPrimaryKey));
      Ret = MemRead(Rt, CurrentPosition, Primary_Key_Len + 1, MemPrimaryKey, MEM_WAIT);
      p_current = MemPrimaryKey;
      if (memcmp(p_current, &Zeroflg, 1) == 0)
      {
         p_current = p_current + 1;
         if(strlen(p_current) == 0)
            break;
         if(memcmp(p_current, RecordAddress, Primary_Key_Len) == 0)
         {
            MatchFound = true;
            Ret = MemRead(Rt, CurrentPosition + 1, RecordSize, RecordAddress, MEM_WAIT);
            break;
         }
      }
      CurrentPosition = CurrentPosition + RecordSize + 1;
   }
   if(MatchFound)
      return(CurrentPosition);
   else
      return(-1L);
}
/*************************************************************************************/
/* Function    :  ReadMemMonitorTable
   Parameters  :  Table name, record_size, address length of OPRMON
   Return      :  XINT  
   Description :  if OPRMON is running then its address will be available in the 
                  Monitor Table in memory, else return NULL 
   Authour     :  Oct,13th'98, Supriya.
*/
/*************************************************************************************/

XINT ReadMemMonitorTable(pCHAR MonitorMemAdd)
{

   XINT           Rt, Ret;
   CHAR strTemp[MAX_ORIG_INFO_SIZE+1] = {0} ;
 
   Rt = MemAccess("MonitorTable");
   if(Rt < 0)
      return(MEMACCESSERROR);
   Ret = MemRead(Rt, 0L, MAX_ORIG_INFO_SIZE + 1, strTemp, MEM_WAIT);
   strcpy (MonitorMemAdd, strTemp+1) ;
   return(0);
      
}
/*
pCHAR ReadMemMonitorTable()
{

   XINT           Rt, Ret;
   static char    MonitorMemAdd[MAX_ORIG_INFO_SIZE + 1];

   Rt = MemAccess("MonitorTable");
 
   if(Rt < 0)
     return(NULL);

   memset(MonitorMemAdd, 0, sizeof(MonitorMemAdd));
   Ret = MemRead(Rt, 0L, MAX_ORIG_INFO_SIZE + 1, MonitorMemAdd, MEM_WAIT);
   if(strlen(MonitorMemAdd+1) == 0)
      return NULL;
   else
      return(MonitorMemAdd + 1);
      
}*/

/***************************************************************************************/
/***************************************************************************************/
LONG WriteMemMonitorTable(  pBYTE data)
{
   XINT Rt, Ret, result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   
   Rt = MemAccess("MonitorTable");
   if(Rt < 0)
      return(Rt);

   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   Ret = MemWrite(Rt, Current_Offset + 1, strlen(data), data, MEM_WAIT);

   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   return(0L);
}

/***************************************************************************************/
/*************************************************************************************/
int create_generic_table(pCHAR TableName)
{
   LONG                 xipc_code;
   char                 Flag;

   MemSize = GetMemorySize();
   MemAvailable = MemSize;

   if(MemAvailable > 0L)
   {
      xipc_code = CreateXipcTable(1, 10, TableName);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = WriteTable(TableName, 10 + 1, &Flag);
         /*if(xipc_code >= 0L)
            GenTablInMem = true;      
         else
            GenTablInMem = false;*/
         if(xipc_code < 0L)
            return (-1);
      }
      return 0;
   }
   else
      return (-1);
}
/*************************************************************************************/
/***************************************************************************************/
XINT ReadGenericTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state)
{

   XINT           Rt, Ret;
   CARD_MEM       mem_crdtbl;
   LONG           CurrentPosition = 0L;

 
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(MEMACCESSERROR);
   memset(&mem_crdtbl, 0, sizeof(CARD_MEM));
   Ret = MemRead(Rt, CurrentPosition +1, sizeof(CARD_MEM), &mem_crdtbl, MEM_WAIT);
   strcpy(tmouts, mem_crdtbl.net_consec_tmouts_ctr);
   strcpy(txns, mem_crdtbl.active_txns_ctr);
   strcpy(state, mem_crdtbl.current_state);

   return(0);
      
}
/***************************************************************************************/
/***************************************************************************************/
LONG WriteGenericTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state)
{
   XINT Rt, Ret,result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   CARD_MEM    mcrdtbl;
   
   
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);

   memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);

   memset( &mcrdtbl, 0x00, sizeof(CARD_MEM) );
   strcpy( mcrdtbl.net_consec_tmouts_ctr, tmouts );
   strcpy( mcrdtbl.active_txns_ctr, txns);
   strcpy( mcrdtbl.current_state, state);
   Ret = MemWrite(Rt, Current_Offset + 1, sizeof(CARD_MEM), &mcrdtbl, MEM_WAIT);
   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   
   return(0L);
}


/***************************************************************************************/

LONG FindMemBinnRange(pBYTE CardNumber, pBYTE Type, pBYTE BinRecord)
{
   BOOLEAN    MatchFound = false;
   XINT       Rt, Ret;
   BIN01_MEM  bin01_detail;
   LONG       CurrentPosition = 0L;
   LONG       SectionSize;
   LONG       MaxLen;
   INT        PanLength, CardLength, BinLength;
   CHAR       CardComp[21];
   MEMINFOMEM MemInfo;

   Rt = MemAccess("Bin01Table");

   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   if(Rt < 0)
      return(Rt);
   CardLength = strlen(CardNumber);
   while( !MatchFound )
   {
      MaxLen = CurrentPosition + sizeof(BIN01_MEM);
      if(MaxLen > SectionSize)
         break;
      memset(&bin01_detail, 0, sizeof(BIN01_MEM));
      Ret = MemRead(Rt, CurrentPosition, sizeof(BIN01_MEM), &bin01_detail, MEM_WAIT);

      if(bin01_detail.delete_flag == 0)
      {
         BinLength = atoi(bin01_detail.bin01_record.primary_key.length);
         if(BinLength == 0)
            break;
         PanLength = atoi(bin01_detail.bin01_record.pan_length);
         if(PanLength == 0 || PanLength == CardLength)
         {
            memset(CardComp, 0, sizeof(CardComp));
            strncpy(CardComp, CardNumber, BinLength);
            if((strcmp(CardComp, bin01_detail.bin01_record.primary_key.low) >= 0) &&
               (strcmp(CardComp, bin01_detail.bin01_record.high) <= 0) &&
               (strcmp(Type, bin01_detail.bin01_record.primary_key.type) == 0))
            {
               MatchFound = true;
               memcpy(BinRecord, &bin01_detail.bin01_record, sizeof(BIN01));
               break;
            }
         }
      }
      CurrentPosition = CurrentPosition + sizeof(BIN01_MEM);
   }
   if(MatchFound)
      return(CurrentPosition);
   else
      return(-1L);
}

/*************************************************************************************/
// Get Section Size
/*************************************************************************************/
LONG GetSectionSize(pCHAR TableName)
{
   XINT    Rt, Ret;
   MEMINFOMEM MemInfo;

   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   MemInfo.SListOffset = MemInfo.WListOffset = 0 ;  /* Added by SDG */
   Ret = MemInfoMem(Rt, &MemInfo);
   if(Ret < 0)
      return(Ret);
   return(MemInfo.Size);
}

/*************************************************************************************/
// DestroyTable
/*************************************************************************************/
LONG DestroyXipcTable(pCHAR SectionName)
{
   XINT Rt, Ret;

   Rt = MemAccess(SectionName);
   if(Rt >= 0)
   {
      Ret = MemDestroy(Rt);
      return(Ret);
   }
   else
      return(Rt);
}


/******************************************************************************/
/******************************************************************************/
XINT ReadBON01Table( pCHAR TableName, pBON01 p_bon01 )
{
   XINT     Rt, Ret, ret_val;
   LONG     CurrentPosition = 0L;
 
   Rt = MemAccess( TableName );
   if(Rt < 0)
   {
      ret_val = MEMACCESSERROR;
   }
   else
   {
      ret_val = 0;
      memset( p_bon01, 0x00, sizeof(BON01) );
      Ret = MemRead( Rt, CurrentPosition +1, sizeof(BON01), p_bon01, MEM_WAIT );
   }
   return( ret_val );
}

/******************************************************************************/
/******************************************************************************/
LONG WriteBON01Table( pCHAR TableName, pBON01 p_bon01 )
{
   XINT       Rt, Ret,result;
   SECTION    TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG       SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   LONG       ret_val;


   Rt = MemAccess( TableName );
   if(Rt < 0)
   {
      ret_val = (LONG)Rt;
   }
   else
   {
      memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
      MemInfo.SListOffset = MemInfo.WListOffset = 0;
      Ret = MemInfoMem(Rt, &MemInfo);
      SectionSize = MemInfo.Size;
      Current_Offset = 0L;
      Ret = MemLock( MEM_ALL,
                     MemList(*MemSectionBuild( &TempSec,
                                                Rt,
                                                Offset,
                                                SectionSize),
                              MEM_EOL),
                    &RetSec,
                     MEM_WAIT );

      Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);

      Ret = MemWrite(Rt, Current_Offset + 1, sizeof(BON01), p_bon01, MEM_WAIT);
      result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                     MEM_EOL), &RetSec);
      if(Ret < 0)
         ret_val = (LONG)Ret;
      else
         ret_val = 0L;
   }
   return( ret_val );
}

