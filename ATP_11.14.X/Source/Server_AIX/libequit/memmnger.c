/******************************************************************************

   Module:               memmnger.c

   Title:                Mem Manager

   Description: 

   Application:          Equitable Bank

   Authours:             Last modified by supriya.
   
   Revision History:     1.0 

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\MEMMNGER.C  $

      Rev 1.9   May 25 2007 DIrby
   Added functions to interface with the new Auth Number table in memory.
   SCR 12299

      Rev 1.8   Feb 14 2007 08:10:54   DIrby
   Added functions to interface with table SQL01 in shared memory.
   This table holds the last query used by a data server.
   SCR 24081
   
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
#include "ptetime.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "tx_database.h"
#include "dc_database.h"
#include "nc_database.h"

#include "memmnger.h"

PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;

CHAR SQL01SectionName[] = "SQL01Table";

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

/******************************************************************************
 *  Create Table
 ******************************************************************************/
LONG CreateXipcTable( INT NumRecords, INT RecordSize, pCHAR SectionName )
{
   XINT Rt, Ret;
   LONG TableSize;
   CHAR strError[256] = {0};
   MEMINFOMEM MemInfo;
   LONG       SectionSize;

   Rt = MemAccess(SectionName);
   if( Rt >= 0 )
   {
      Ret = MemDestroy(Rt);
      if(Ret < 0)
      {
         sprintf( strError,
                 "CreateXIPCTable: MemDestroy Return %d, SectionName is %s",
                  Ret, SectionName) ;
         LogEvent( strError, WARN_MSG );
         return( Ret );
      }
   }

   TableSize = (NumRecords + 2) * (RecordSize + 1);
   Rt = MemCreate(SectionName, TableSize);
   if(Rt < 0)
   { 
      sprintf( strError,
              "CreateXIPCTable: MemCreate Return %d SectionName: %s, TableSize: %ld",
               Rt, SectionName, TableSize );
      LogEvent( strError, WARN_MSG );
      return(Rt);
   }
   Rt = MemAccess(SectionName);
   if(Rt < 0)
   {
      sprintf( strError,
              "CreateXIPCTable: MemAccess Return %d SectionName: %s",
               Rt, SectionName );
      LogEvent( strError, WARN_MSG );
      return(Rt);
   }

   Ret = MemSecDef(MemSection(Rt, 0L, TableSize));
   if(Ret >= 0)
   {
      /* Initialize the table. */
      MemInfo.SListOffset = MemInfo.WListOffset = 0; 
      Ret = MemInfoMem(Rt, &MemInfo);
      SectionSize = MemInfo.Size;

      Ret = MemWrite( Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT );
      if (Ret < 0)
      {
	      sprintf( strError,
                 "Inside CreateXipcTable: MemWrite returns %d SectionSize is %ld",
                  Ret, SectionSize );
         LogEvent (strError, WARN_MSG) ;
      }
      return( TableSize );
   }
   else
   {
      sprintf( strError,
              "CreateXIPCTable: MemSecDef Return %d",
               Ret );
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

/*****************************************************************************
 *
 * Function:     CreateSQL01_Table
 *
 * Description:  This function creates table SQL01Table in shared memory.
 *               If the table already exists, this function does not destroy
 *               it.  It will exit with success.
 *
 * Input:        None
 *
 * Output:       None
 *
 * Return Value: 0 or positive number for success
 *               negative number for failure
 *
 *****************************************************************************/
INT CreateSQL01_Table()
{
   INT  retval;

   retval = MemAccess( SQL01SectionName );
   if ( retval < 0 )
   {
      /* The table does not exist. Create it.
       * Thirty records each one the size of SQL01 structure.
       */
      retval = (INT)CreateXipcTable( 30, sizeof(SQL01), SQL01SectionName );
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     WriteSQL01_Table
 *
 * Description:  This function writes a record to the SQL01Table in shared
 *               memory.  If a record with the same primary key already
 *               exists, that record gets overwritten.  The primary key is
 *               the application name.  This function populates the system
 *               date/time field in the SQL01 record.
 *
 * Input:        p_sql01 - SQL01 structure to be stored in table
 *
 * Output:       None
 *
 * Return Value: 0 or positive number for success
 *               negative number for failure
 *
 *****************************************************************************/
INT WriteSQL01_Table( pSQL01 p_sql01 )
{
   INT    retval;
   LONG   offset;
   SQL01  tempSql01;

   /* Get system date/time in format: "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( p_sql01->sysdate );

   /* FindMemRecord will overwrite our SQL01 structure, so save it. */
   memcpy( &tempSql01, p_sql01, sizeof(SQL01) );

   /* Find the offset to our record in shared memory. */
   offset = FindMemRecord( (pBYTE)p_sql01, sizeof(p_sql01->appname),
                            sizeof(SQL01), SQL01SectionName );

   if ( offset >= 0L )
   {
      /* Found it.  Overwrite it with our new record. */
      retval = UpdateMemRecord( SQL01SectionName,
                                offset,
                                sizeof(SQL01),
                               (pBYTE)(&tempSql01) );
   }
   else if ( offset == -1 )
   {
      /* Record not found. Create it. */
      retval = InsertSQL01_Table( (pBYTE)p_sql01 );
   }
   else
   {
      retval = (INT)offset;
   }
   return( retval );
}

/*****************************************************************************
 *
 * Function:     InsertSQL01_Table
 *
 * Description:  This function inserts a new record to the SQL01Table in
 *               shared memory. This is appended to the end of the table.
 *
 * Input:        p_sql01 - SQL01 structure to be stored in table
 *
 * Output:       None
 *
 * Return Value: 0 or positive number for success
 *               negative number for failure
 *
 *****************************************************************************/
INT InsertSQL01_Table( pBYTE p_sql01 )
{
   BOOLEAN    RecordFound = true;
   XINT       Rt, Ret, Result;
   SECTION    TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG       SectionSize;
   LONG       CurrentPosition = 0L;
   CHAR       strError [256] = {0};
   CHAR       MemPrimaryKey[3];

   /* Make sure we can access this shared memory table. Get its ID in 'Rt'. */
   Rt = MemAccess( SQL01SectionName );
   if ( Rt < 0 )
   {
      sprintf( strError,
              "Shared Memory - Unable to insert SQL01: MemAccess "
              "returns %d, SectionName is %s",
               Rt, SQL01SectionName );
      LogEvent( strError, WARN_MSG );
      Ret = Rt;
   }
   else
   {
      MemInfo.SListOffset = MemInfo.WListOffset = 0; 

      Ret = MemInfoMem( Rt, &MemInfo );
      if (Ret < 0)
      {
         sprintf( strError,
                 "Inside InsertSQL01_Table: MemInfoMem returns %d SectionName is %s",
                  Ret, SQL01SectionName );
         LogEvent( strError, WARN_MSG );
      }
      SectionSize = MemInfo.Size;

      /* Lock the entire section of shared memory. */
      Ret = MemLock(MEM_ALL,
                    MemList(*MemSectionBuild(&TempSec, Rt, CurrentPosition, SectionSize),
                                 MEM_EOL), &RetSec, MEM_WAIT);
      if (Ret < 0)
      {
         sprintf( strError,
                 "Inside InsertSQL01_Table: MemLock returns %d, SectionName is %s",
                  Ret, SQL01SectionName );
         LogEvent( strError, WARN_MSG );
      }

      /* Get past all the existing records. */
      while( RecordFound )
      {
         /* DWORD is unsigned Long. Use it here to eliminate compile warning. */
         if ( (CurrentPosition + sizeof(SQL01) + 1) > (DWORD)SectionSize )
         {
            /* We are past the end of the section, so break the loop. */
            break;
         }

         /* Just read the first couple of bytes
          * of a record to see if it exists.
          */
         memset( MemPrimaryKey, 0x00, sizeof(MemPrimaryKey) );
         Ret = MemRead( Rt, CurrentPosition, 2, MemPrimaryKey, MEM_WAIT );
         if ( MemPrimaryKey[1] == 0x00 )
         {
            /* No record at this position. */
            RecordFound = false;
         }
         else
         {
            /* Look at next record. */
            CurrentPosition += sizeof(SQL01) + 1;
         }
      }

      if ( !RecordFound )
      {
         /* There is room to insert a new record into shared memory. */
         Ret = MemWrite(Rt,CurrentPosition + 1, sizeof(SQL01),p_sql01,MEM_WAIT);
         if (Ret < 0)
         {
            sprintf( strError,
                    "Inside InsertSQL01_Table: MemWrite returns %d SectionSize is %ld",
                     Ret, SectionSize );
            LogEvent( strError, WARN_MSG );
         }
      }

      /* Unlock the entire section. */
      Result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, 0L, SectionSize),
                                     MEM_EOL), &RetSec);
      if (Result < 0)
      {
         sprintf( strError,
                 "Inside InsertSQL01_Table: MemUnlock returns %d",
                  Result );
         LogEvent( strError, WARN_MSG );
      }
   }
   return( Ret );
}


/*****************************************************************************
 *
 * Function:     ReadSQL01_SingleRec_Table
 *
 * Description:  This function reads one record from SQL01 in
 *               shared memory based on the primary key is the appname.
 *
 * Input:        p_sql01 - SQL01 structure containing primary key
 *
 * Output:       p_sql01 - SQL01 record read from shared memory
 *
 * Return Value: 0 or positive number for success
 *               negative number for failure
 *
 *****************************************************************************/
INT ReadSQL01_SingleRec_Table( pSQL01 p_sql01 )
{
   LONG  retval;

   retval = FindMemRecord( (pBYTE)p_sql01, sizeof(p_sql01->appname),
                            sizeof(SQL01), SQL01SectionName );
   return( (INT)retval );   
}


/*****************************************************************************
 *
 * Function:     ReadSQL01_MultipleRecs_Table
 *
 * Description:  This function reads one or more records from SQL01 in
 *               shared memory based on the primary key as follows:
 *
 *                 Appname                   - read one record
 *                 Appname with wildcard (*) - read group of records
 *                 No appname                - read all records
 *
 *               Examples:
 *                 Appname = devds  - read one record
 *                 Appname = devds* - read devds, devds01, devds02,...
 *                 Appname = Null   - read all records from the table
 *
 * Input:        key - Primary key used for the look up
 *
 * Output:       p_sql01_list - Array of SQL01 structures
 *
 * Return Value: 0 or positive number for success
 *               negative number for failure
 *
 *****************************************************************************/
INT ReadSQL01_MultipleRecs_Table( pBYTE key, pSQL01_LIST p_sql01_list )
{
   #define    ONE_MEM_REC    1
   #define    GROUP_MEM_REC  2
   #define    ALL_MEM_REC    3

   INT        retval;
   INT        i = 0;
   INT        keylen;
   INT        rec_cnt  = 0;
   BOOLEAN    MoreRecs = true;
   BOOLEAN    fetch_type;
   XINT       Rt, Ret;
   CHAR       MemPrimaryKey[PRIMARY_KEY_MAX_LEN + 1];
   LONG       CurrentPosition = 0L;
   LONG       SectionSize;
   LONG       Primary_Key_Len = sizeof(p_sql01_list->sql01_list[0].appname);
   LONG       RecordSize      = sizeof(SQL01);
   pBYTE      p_current;
   MEMINFOMEM MemInfo;
   BYTE       Zeroflg = 0;

   Rt = MemAccess( SQL01SectionName );
   if ( Rt >= 0 )
   {
      /* Determine if we want all records, some (i.e. devds*) or one */
      if ( key[0] == 0x00 )
         fetch_type = ALL_MEM_REC;
      else if ( NULL == strchr(key,'*') )
         fetch_type = ONE_MEM_REC;
      else
      {
         fetch_type = GROUP_MEM_REC;
         keylen = strlen( key ) - 1;  /* Do not include asterisk in keylen */
      }

      MemInfo.SListOffset = 0;
      MemInfo.WListOffset = 0;

      Ret = MemInfoMem( Rt, &MemInfo );
      SectionSize = MemInfo.Size;

      /* Loop through all the records to find a match. */
      while( MoreRecs )
      {
         if ( (CurrentPosition + RecordSize + 1) > SectionSize )
         {
            MoreRecs = false;  /* Exit the loop */
         }
         else
         {
            memset( MemPrimaryKey, 0x00, sizeof(MemPrimaryKey) );
            Ret = MemRead( Rt,                   /* Memory Segment ID        */
                           CurrentPosition,      /* Rec offset in memory     */
                           Primary_Key_Len + 1,  /* Len of search key        */
                           MemPrimaryKey,        /* Buffer to read key into  */
                           MEM_WAIT);            /* If table is locked, wait */

            p_current = MemPrimaryKey;
            if ( memcmp(p_current, &Zeroflg, 1) == 0 )
            {
               /* Found a possible match. */
               p_current = p_current + 1;
               if ( strlen(p_current) == 0 )
               {
                  /* No data... */
                  MoreRecs = false;  /* Exit the loop */
               }
               else
               {
                  /* Base our comparison on fetch type. */
                  if ( fetch_type == ONE_MEM_REC )
                  {
                     if ( 0 == memcmp(p_current, key, Primary_Key_Len) )
                     {
                        MoreRecs = false; /* So we can exit the loop. */

                        /* Copy the whole record into the array. */
                        Ret = MemRead( Rt,
                                       CurrentPosition + 1,
                                       RecordSize,
                                       p_sql01_list->sql01_list[i].appname,
                                       MEM_WAIT );

                        rec_cnt = 1;
                     }
                  }
                  else if ( fetch_type == ALL_MEM_REC )
                  {
                     /* Copy the whole record into the array. */
                     Ret = MemRead( Rt,
                                    CurrentPosition + 1,
                                    RecordSize,
                                    p_sql01_list->sql01_list[i].appname,
                                    MEM_WAIT );

                     rec_cnt++; /* Num records found */
                     i++;       /* Index to next SQL01 array structure */
                  }
                  else
                  {
                     /* Look for a match for the group. */
                     if ( 0 == memcmp(p_current, key, keylen) )
                     {
                        /* Copy the whole record into the array. */
                        Ret = MemRead( Rt,
                                       CurrentPosition + 1,
                                       RecordSize,
                                       p_sql01_list->sql01_list[i].appname,
                                       MEM_WAIT );

                        rec_cnt++; /* Num records found */
                        i++;       /* Index to next SQL01 array structure */
                     }
                  }
               }
            }

            /* Increment to the next record. */
            CurrentPosition += RecordSize + 1;
         }
      }
      sprintf( p_sql01_list->num_returned, "%d", rec_cnt );
   }
   else
   {
      retval = Rt;
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     ReadAuthTable
 *
 * Description:  This function reads the authorization number and the
 *               lock flag from the AuthTable in shared memory.
 *
 *               If lock_input =
 *                  LOCK:   if table is already locked, error out, else
 *                          lock the table.
 *
 *                  NOLOCK: always return the auth number
 *
 *                  UNLOCK: unlock the table after the read
 *
 *
 * Input:        lock_input - Indicator to lock (or not) table after the read
 *
 * Output:       auth_number - 6 byte authorization number
 *
 * Return Value: MEM_OK                for success,
 *               TABLE_ALREADY_LOCKED  for record already locked,
 *               other negative number for an Xipc failure
 *
 *****************************************************************************/
INT ReadAuthTable( pCHAR auth_number, INT lock_input )
{
   INT   retval = MEM_OK;
   XINT  Rt, Ret;
   INT   rec_size = 9;  /* auth num = 6 + 1 null, lock flag = 1 + 1 null */
   CHAR  buffer[10]    = "";
   CHAR  lock_field[2] = "";

   /* See if table exists in shared memory. */
   Rt = MemAccess( "AuthTable" );
   if ( Rt < 0 )
   {
      /* No - error out. */
      retval = Rt;
   }
   else
   {
      /* Yes - table exists. Read the auth number and lock flag. */
      Ret = MemRead( Rt, 0L, rec_size, buffer, MEM_WAIT);
      if ( Ret < 0 )
      {
         /* Unable to read the record. Error out. */
         retval = Ret;
      }
      else
      {
         memcpy( lock_field, buffer+7, 1 );

         /* Take action based on lock_input parm and state of the table. */
         if ( lock_input == LOCK )
         {
            if ( lock_field[0] == LOCKED )
            {
               /* Table is already locked. Error out. */
               retval = TABLE_ALREADY_LOCKED;
            }
            else
            {
               /* Lock the table, as requested. */
               lock_field[0] = LOCKED;
            }
         }
         else if ( lock_input == UNLOCK )
         {
               lock_field[0] = UNLOCKED;
         }

         if ( retval == MEM_OK )
         {
            /* Success - need to update the state of the table. */
            if ( lock_input != NOLOCK )
            {
               buffer[7] = lock_field[0];
               Ret = MemWrite( Rt, 0L, rec_size, buffer, MEM_WAIT);
               if ( Ret < 0 )
                  retval = Ret;
            }
            memcpy( auth_number, buffer, 6 );
         }
      }
   }
   return( retval );
}


/*****************************************************************************
 *
 * Function:     WriteAuthTable
 *
 * Description:  This function writes the authorization number into the
 *               AuthTable.
 *
 *               If lock_input =
 *                  LOCK:   Lock the table after the right
 *
 *                  NOLOCK: always write the auth number to the table
 *
 *                  UNLOCK: unlock the table after the write
 *
 * Input:        auth_number - 6 byte authorization number
 *               lock_input  - Indicator to lock (or not) table after write
 *
 * Output:       None
 *
 * Return Value: 0 for success,
 *               negative number for an Xipc failure
 *
 *****************************************************************************/
INT WriteAuthTable( pCHAR auth_number, INT lock_input )
{
   INT   retval = 0;
   XINT  Rt, Ret;
   INT   rec_size = 9;  /* auth num = 6 + 1 null, lock flag = 1 + 1 null */
   CHAR  buffer[10] = "";

   /* See if table exists in shared memory. */
   Rt = MemAccess( "AuthTable" );
   if ( Rt < 0 )
   {
      /* No - error out. */
      retval = Rt;
   }
   else
   {
      /* Yes - table exists. Store the auth number
       * then set the table state according to the
       * lock_input parameter.
       */
      strcpy( buffer, auth_number );
      if ( lock_input == LOCK )
         buffer[7] = LOCKED;

      else if ( lock_input == UNLOCK )
         buffer[7] = UNLOCKED;

      Ret = MemWrite( Rt, 0L, rec_size, buffer, MEM_WAIT);
      if ( Ret < 0 )
         retval = Ret;
   }
   return( retval );
}


