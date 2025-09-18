/***************************************************************************
 *
 * Copyright (c) 2001, ThoughtFocus, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci2_memory.c
 *  
 * TITLE:       Diners Network Controller Shared Memory Processor
 *  
 * DESCRIPTION: This module contains functions that interface with
 *              XIPC's shared memory.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      Abhishek Verma
 *
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "pteipc.h"
#include "equitdb.h"
#include "memmnger.h"
#include "nc_dbstruct.h"
#include "diners.h"
#include "ncdci2_prototypes.h"


/*+--------------------+
  | Global Definitions |
  +--------------------+*/

extern CHAR    AppName[];

typedef struct
{
   BYTE net_consec_tmouts_ctr    [4];
   BYTE active_txns_ctr          [4];
   BYTE host_state               [2];
}SHARED_MEM, *pSHARED_MEM;

/* Database structures */
extern NCF01  Ncf01_I;

/* Network Type: Issuer, Acquirer, or Both */
extern INT    NetworkType;

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;
extern INT    Echo_Timer_Count;

/******************************************************************************
 *
 *  NAME:         ncdci2_create_shared_mem_table
 *
 *  DESCRIPTION:  This function creates a table in shared memory consisting
 *                of 3 values:
 *                             number of consecutive time outs
 *                             number of active transactions
 *                             host state
 *
 *                These values get initialized to: 0, 0, 1 respectively.
 *                Note that host state of '1' means not logged on yet.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_create_shared_mem_table( pCHAR errbuf )
{
	   CHAR     table_name[15] = {0};
   INT      ret_val = false;
   INT      ret_code = 0;

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = create_generic_table( table_name );
   if ( MEMACCESSERROR == ret_code )
   {
      /* Table already exist. */
      ret_val = true;
   }
   else if ( 0 == ret_code )
   {
      /* Table was successfully created.
       * Initialize host counters and state upon ncdci2_startup.
       */
     ret_val = ncdci2_write_to_shared_mem_table( "0", "0", OFFLINE, errbuf );
   }
   else
   {
      /* Error creating the shared memory table. */
      sprintf( errbuf, "Unable to create shared memory table: %s.",
               table_name );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_write_to_shared_mem_table
 *
 *  DESCRIPTION:  This function writes to the shared memory table, defined
 *                with a global variable.  It writes 4 values to the table.
 *                These values are defined below in the INPUTS section.
 *
 *  INPUTS:       (Global)AppName - Name of application
 *                consec_time_out_count
 *                active_tran_count
 *                host_state
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_write_to_shared_mem_table( pBYTE consec_time_out_count,
                               pBYTE active_tran_count,
                               pBYTE host_state,
                               pCHAR errbuf )
{
	   CHAR  table_name[15] = {0};
   INT   ret_val = true;
   LONG  ret_code = 0;


   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = ncdci2_WriteMemTable( table_name, consec_time_out_count,
                             active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      /* Unable to write to shared table. */
      ret_val = false;
      sprintf( errbuf, "Unable to write to shared memory table: %s.",
               table_name );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_read_shared_mem_table
 *
 *  DESCRIPTION:  This function reads the shared memory table, defined
 *                with a global variable.  It reads 4 values from the table.
 *                These values are defined below in the OUTPUTS section.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *
 *  OUTPUTS:      consec_time_out_count
 *                active_tran_count
 *                host_state
 *                errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:     Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_read_shared_mem_table( pBYTE consec_time_out_count,
                           pBYTE active_tran_count,
                           pBYTE host_state,
                           pCHAR errbuf )
{
	   CHAR  table_name[15] = {0};
	   INT   ret_val = true;
	   LONG  ret_code = 0;

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = ncdci2_ReadMemTable( table_name, consec_time_out_count,
                            active_tran_count, host_state );
   if ( 0 != ret_code )
   {
      ret_val = false;
      sprintf( errbuf, "Unable to read shared memory table: %s",
               table_name );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_set_host_state
 *
 *  DESCRIPTION:  This function sets the state of the host in shared memory.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *                host_state - State to set the host
 *                                0 = Logged on
 *                                1 = Not lgged on
 *                                2 = Logged on, but busy
 *                                3 = Application is shut down
 *
 *  OUTPUTS:      err_buf - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_set_host_state( pBYTE host_state, pCHAR err_buf )
{
	   BYTE  active_cnt[4] = {0};
	   BYTE  timeout_cnt[4] = {0};
	   BYTE  prev_host_state[2] = {0};
   INT   ret_val = 0;
   INT   timeouts = 0;


   /* Read the current values in order to echo back the counts. */
   ret_val = ncdci2_read_shared_mem_table( timeout_cnt, active_cnt,
                                    prev_host_state, err_buf );
   if ( true == ret_val )
   {
      /* Is host state changing? */
      if ( 0 != strcmp(host_state, prev_host_state) )
      {
         /* YES */
         if ( (0 == strcmp( host_state, ONLINE ))  ||
              (0 == strcmp( host_state, DOWN   ))  ||
              (0 == strcmp( host_state, OFFLINE)) )
         {
            memset( active_cnt, 0x00, sizeof(active_cnt) );
            memset( timeout_cnt,0x00, sizeof(timeout_cnt));
            strcpy( timeout_cnt, "0" );
            strcpy( active_cnt,  "0" );
         }
         ret_val = ncdci2_write_to_shared_mem_table( timeout_cnt, active_cnt,
                                              host_state,  err_buf );
         if ( true == ret_val )
         {
            /* Inform Monitor of change of status. */
            (void)ncdci2_send_network_status_to_monitor();

            if ( 0 == strcmp(host_state, ONLINE) )
            {
               /*+--------+
                 | ONLINE |
                 +--------+*/


               /* Since we just went online, begin the echo timer. */
               //if ( false == ncdci2_set_echo_timer(err_buf) )
                //  ncdci2_log_message( 3, 2, err_buf, "ncdci2_set_host_state" );
            }
            else if ( 0 == strcmp(host_state, DOWN) )
            {
               /*+------+
                 | DOWN |
                 +------+*/

               /* Since we just went down, clear the echo timer. */
               //if ( false == ncdci2_clear_echo_timer(err_buf) )
                 // ncdci2_log_message( 3, 2, err_buf, "ncdci2_set_host_state" );
            }
            else if ( 0 == strcmp(host_state, OFFLINE) )
            {
               /*+---------+
                 | OFFLINE |
                 +---------+*/

               /* Clear echo timer only if we did not go offline
                * due to max time outs.  To do this, clear it,
                * then reset it if offline due to max timeouts.
                */
               //(void)ncdci2_clear_echo_timer(err_buf);
               if ( MaxConsecTimeouts > 0 )
               {
                  timeouts = atoi(timeout_cnt);
                  if ( timeouts >= MaxConsecTimeouts )
                  {
                     //if ( false == ncdci2_set_echo_timer(err_buf) )
                      //  ncdci2_log_message( 3, 2, err_buf, "ncdci2_set_host_state" );
                  }
               }

            }
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_decrement_active_count
 *
 *  DESCRIPTION:  This function reads the shared memory table, decrements the
 *                active transaction counter, and stores those values back
 *                into shared memory.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci2_decrement_active_count( pCHAR errbuf )
{
	   BYTE     active_cnt[4] = {0};
	   BYTE     timeout_cnt[4] ={0};
	   BYTE     host_state[2] = {0};
	INT      ret_val = false;
   INT      tempint = 0;

   /* Read the current values in order to echo back the counts. */
   ret_val = ncdci2_read_shared_mem_table( timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( true == ret_val )
   {
      /* Decrement the active transactions counter field */
      tempint = atoi(active_cnt) - 1;
      if ( 0 > tempint )
      {
         /* If less than zero set it to zero */
         strcpy( active_cnt,"0" );
      }
      else
      {
         sprintf( active_cnt, "%d", tempint );
      }

      if ( 0 == strcmp(host_state,QUERYING) )
      {
         if ( (tempint < MaxActiveTxns) && (MaxActiveTxns > 0) )
         {
            ncdci2_set_host_state( ONLINE, errbuf );
            strcpy( host_state, ONLINE );
         }
      }

      /* Update the values into shared memory. */
      ret_val = ncdci2_write_to_shared_mem_table( timeout_cnt, active_cnt,
                                           host_state, errbuf );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_increment_active_count
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                active transaction counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of active transactions to the max allowed.  If too high,
 *                the host state is set to 1.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_increment_active_count( pCHAR errbuf )
{
	   BYTE     active_cnt[4] = {0};
	   BYTE     timeout_cnt[4] ={0};
	   BYTE     host_state[2] = {0};
	   INT      ret_val = false;
	   INT      tempint = 0;

   /* Read the current values in order to echo back the counts. */
   ret_val = ncdci2_read_shared_mem_table( timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( true == ret_val )
   {
      /* Increment the active transactions counter field */
      tempint = atoi(active_cnt) + 1;

      /* Check number of active transactions. */
      if ( (tempint >= MaxActiveTxns) && (MaxActiveTxns > 0) )
      {
         /* Transaction count is too high.
          * Set state to not allow any more.
          */
         (void)ncdci2_set_host_state( QUERYING, errbuf );
         strcpy( errbuf,
                "Active transaction count too high. Continuing to process.");

         ncdci2_log_message( 1, 2, errbuf, "ncdci2_increment_active_count" ,0);
      }
      else
      {
         /* Don't let this value get too high. */
         if ( tempint > 999 )
            tempint = 1;
         sprintf( active_cnt, "%d", tempint );

         /* Update the values into shared memory. */
         ret_val = ncdci2_write_to_shared_mem_table( timeout_cnt, active_cnt,
                                              host_state, errbuf );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_increment_timeout_count
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                transaction timeout counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of time outs to the max allowed.  If too high, a warning
 *                is posted.
 *
 *  INPUTS:       AppName - (Global)Name of application
 *
 *  OUTPUTS:      err_buf - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred or max
 *                consecutive timeouts occurred.
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci2_increment_timeout_count( pCHAR err_buf )
{
    BYTE     active_cnt[4] = {0};
   BYTE     timeout_cnt[4] ={0};
   BYTE     host_state[2] = {0};
   INT      ret_val = false;
   INT      tempint = 0;

   /* Read the current values in order to echo back the counts. */
   ret_val = ncdci2_read_shared_mem_table( timeout_cnt, active_cnt,
                                    host_state, err_buf );
   if ( true == ret_val )
   {
      /* Increment the consecutive timeout counter field */
      tempint = atoi(timeout_cnt) + 1;

      /* Check number of consecutive timeouts. */
      if ( MaxConsecTimeouts > 0 )
      {
         if ( host_state[0] == cONLINE )
         {
            if ( tempint > MaxConsecTimeouts )
            {
               strcpy( host_state, OFFLINE );
               strcpy( err_buf, "Maximum consecutive timeouts reached.");
               ret_val = false;
            }
         }
      }

      /* Don't let this value get too high. */
      if ( tempint > 999 )
         tempint = 1;
      sprintf( timeout_cnt, "%d", tempint );

      if(tempint == (MaxConsecTimeouts+1))
      {
    	  /* Update the values into shared memory. */
    	  (void)ncdci2_write_to_shared_mem_table( timeout_cnt, active_cnt,
                                       	   	   	  host_state, err_buf );
    	  sleep(2);
      }
      else
      {
    	  (void)ncdci2_write_to_shared_mem_table( timeout_cnt, active_cnt,
                                       	   	   	  host_state, err_buf );
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_reset_timeout_count
 *
 *  DESCRIPTION:  This function reads the shared memory table, resets the
 *                transaction timeout counter to zero, and stores those
 *                values back into shared memory.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is false
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci2_reset_timeout_count( pCHAR errbuf )
{

   BYTE     active_cnt[4] = {0};
   BYTE     timeout_cnt[4] = {0};
   BYTE     host_state[2] = {0};
   INT      ret_val = false;

   /* Read the current values in order to echo back the counts. */
   ret_val = ncdci2_read_shared_mem_table( timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( true == ret_val )
   {
      /* Update the values into shared memory, setting time out count to 0. */
      ret_val = ncdci2_write_to_shared_mem_table( "0", active_cnt,
                                           host_state, errbuf);
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ncdci2_ReadMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and reads the information contained in it.
 *
 *  INPUTS:       TableName - Name of application
 *
 *  OUTPUTS:      tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  RTRN VALUE:   0 if successful, MEMACCESSERROR if unable to access table
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
XINT ncdci2_ReadMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
{
   XINT           Rt, Ret;
   SHARED_MEM     nc_tbl;
   LONG           CurrentPosition = 0L;

 
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(MEMACCESSERROR);

   memset(&nc_tbl, 0, sizeof(SHARED_MEM));
   Ret = MemRead(Rt,CurrentPosition +1,sizeof(SHARED_MEM),&nc_tbl,MEM_WAIT);

   strcpy( tmouts, nc_tbl.net_consec_tmouts_ctr);
   strcpy( txns,   nc_tbl.active_txns_ctr      );
   strcpy( state,  nc_tbl.host_state           );

   return(0);
      
}


/******************************************************************************
 *
 *  NAME:         ncdci2_WriteMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and writes information into it.
 *
 *  INPUTS:       TableName - Name of application
 *                tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   0 if successful, else a number to indicate an XIPC error
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
LONG ncdci2_WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
{
   XINT               Rt, Ret,result;
   SECTION            TempSec, RetSec;
   MEMINFOMEM         MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   SHARED_MEM         nc_tbl;
   
   memset(&MemInfo,0x00,sizeof(MEMINFOMEM));
   
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);

   memset( &nc_tbl, 0x00, sizeof(SHARED_MEM) );
   strcpy( nc_tbl.net_consec_tmouts_ctr, tmouts );
   strcpy( nc_tbl.active_txns_ctr,       txns   );
   strcpy( nc_tbl.host_state,            state  );

   Ret = MemWrite(Rt, Current_Offset + 1, sizeof(nc_tbl), &nc_tbl, MEM_WAIT);
   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   
   return(0L);
}

/*****************************************
 * To get the time out count from SHM and
 * display the log message only once if
 * SHM update is delayed
 * ****************************************/
int GetTimeoutCount()
{
	int 	tCount = -1, ret_val;
	BYTE 	time_out_cnt[4];
	BYTE 	active_cnt[4];
	BYTE    host_state[2];
	CHAR	err_buf[100];

	ret_val = ncdci2_read_shared_mem_table( time_out_cnt, active_cnt,
	                                    host_state, err_buf );
	if(ret_val == true)
	{
		tCount = atoi(time_out_cnt);
	}

	return tCount;
}
