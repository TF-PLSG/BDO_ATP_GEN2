/***************************************************************************
 *
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCEQ_MEMORY.C
 *  
 * TITLE:       Equitable Network Controller Shared Memory Processor
 *  
 * DESCRIPTION: This module contains functions that interface with
 *              XIPC's shared memory.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_memory.c  $
   
      Rev 1.7   Sep 17 2002 15:33:06   dirby
   Moved include of memmnger.h below equitdb.h to allow for a
   clean compile.
   SCR 877
   
      Rev 1.6   Aug 02 2002 15:35:14   dirby
   Removed code so time outs of echo requests do not increment
    the timeout counter.  This is so we do not go offline due to too
   many echo request time outs.  Only increment the timeout
   counter for timeouts of financial transactions.
   SCR 594
   
      Rev 1.5   Jul 26 2002 13:25:30   dirby
   Modified to allow 0800 echo requests to go out when host
   state is Offline due to Maximum Time Outs.  Will go back Online
   when a response to the echo request is received.
   SCR 594
   
      Rev 1.4   Jul 12 2002 13:12:28   dirby
   Modified to update network status is Max Timeouts occurs.
   SCR 594
   
      Rev 1.3   Jul 07 2002 02:39:10   dirby
   Reimplemented going offline when max time outs occur.
   SCR 594
   
      Rev 1.2   Jan 18 2002 11:24:44   dirby
   Corrected some minor bugs.
   SCRs 488 and 484
   
      Rev 1.1   Dec 10 2001 14:07:40   dirby
   Modified to always forward requests to host, even if offline.
   Modified to ignore Max Active and Max Timeout counts.
   SCR 488 and 547
   
      Rev 1.0   Aug 06 2001 09:48:18   dirby
   Initial revision.
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
#include "nceq_constants.h"
#include "nceq_prototypes.h"


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

extern char Version[];
extern INT  MainProcessDone;
/******************************************************************************
 *
 *  NAME:         CREATE_SHARED_MEM_TABLE
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT create_shared_mem_table( pCHAR errbuf )
{
   INT      ret_val = false;
   INT      ret_code;
   CHAR     table_name[15];

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
       * Initialize host counters and state upon startup.
       */
     ret_val = write_to_shared_mem_table( "0", "0", OFFLINE, errbuf );
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
 *  NAME:         WRITE_TO_SHARED_MEM_TABLE
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT write_to_shared_mem_table( pBYTE consec_time_out_count,
                               pBYTE active_tran_count,
                               pBYTE host_state,
                               pCHAR errbuf )
{
   INT   ret_val = true;
   LONG  ret_code;
   CHAR  table_name[15];

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = WriteMemTable( table_name, consec_time_out_count,
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
 *  NAME:         READ_SHARED_MEM_TABLE
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_shared_mem_table( pBYTE consec_time_out_count,
                           pBYTE active_tran_count,
                           pBYTE host_state,
                           pCHAR errbuf )
{
   INT   ret_val = true;
   INT   ret_code;
   CHAR  table_name[15];

   memset( table_name, 0x00, sizeof(table_name) );
   strcpy( table_name, AppName );
   strcat( table_name, "Table" );

   ret_code = ReadMemTable( table_name, consec_time_out_count,
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
 *  NAME:         SET_HOST_STATE
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT set_host_state( pBYTE host_state, pCHAR err_buf )
{
   INT   ret_val;
   INT   timeouts;
   BYTE  active_cnt[4];
   BYTE  timeout_cnt[4];
   BYTE  prev_host_state[2];

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt,
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
         ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt,
                                              host_state,  err_buf );
         if ( true == ret_val )
         {
            /* Inform Monitor of change of status. */
            (void)send_network_status_to_monitor();

            if ( 0 == strcmp(host_state, ONLINE) )
            {
               /*+--------+
                 | ONLINE |
                 +--------+*/

               /* Since we just went online, begin the echo timer. */
               if ( false == set_echo_timer(err_buf) )
                  nceqit_log_message( 1, 2, err_buf, "set_host_state",0 );
            }
            else if ( 0 == strcmp(host_state, DOWN) )
            {
               /*+------+
                 | DOWN |
                 +------+*/

               /* Since we just went down, clear the echo timer. */
               if ( false == clear_echo_timer(err_buf) )
                  nceqit_log_message( 1, 2, err_buf, "set_host_state",0 );
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
               (void)clear_echo_timer(err_buf);
               if ( MaxConsecTimeouts > 0 )
               {
                  timeouts = atoi(timeout_cnt);
                  if ( timeouts >= MaxConsecTimeouts )
                  {
                     if ( false == set_echo_timer(err_buf) )
                        nceqit_log_message( 1, 2, err_buf, "set_host_state",0 );
                  }
               }

            }
         }
      }
   }
   else
   {
	   sprintf(err_buf,"Unable to set host state. Problem with shared memory.Shutting down the Service");
	   	nceqit_log_message(1, 2, err_buf, "set_host_state", 0);
		shut_down();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         DECREMENT_ACTIVE_COUNT
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
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT decrement_active_count( pCHAR errbuf )
{
   INT      ret_val = false;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;
   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt,
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
            set_host_state( ONLINE, errbuf );
            strcpy( host_state, ONLINE );
         }
      }

      /* Update the values into shared memory. */
      ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt,
                                           host_state, errbuf );
   }
   else
   {
	   sprintf(errbuf,"Unable to Decrement Active count. Problem with shared memory.Shutting down the service.");
	   	   	nceqit_log_message(1, 2, errbuf, "decrement_active_count", 0);
	   	 shut_down();

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_ACTIVE_COUNT
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT increment_active_count( pCHAR errbuf )
{
   INT      ret_val = false;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;
   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt,
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
         (void)set_host_state( QUERYING, errbuf );
         strcpy( errbuf,
                "Active transaction count too high. Continuing to process.");

         nceqit_log_message( 1, 2, errbuf, "increment_active_count",0 );
      }
      else
      {
         /* Don't let this value get too high. */
         if ( tempint > 999 )
            tempint = 1;
         sprintf( active_cnt, "%d", tempint );

         /* Update the values into shared memory. */
         ret_val = write_to_shared_mem_table( timeout_cnt, active_cnt,
                                              host_state, errbuf );
      }
   }
   else
      {
	   sprintf(errbuf,"Unable to Increment Active count. Problem with shared memory.Shutting down the service");
	   	   	nceqit_log_message(1, 2, errbuf, "increment_active_count", 0);
	   	 shut_down();
      }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_TIMEOUT_COUNT
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
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT increment_timeout_count( pCHAR err_buf )
{
   INT      ret_val;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];
   INT      tempint;

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt,
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

      /* Update the values into shared memory. */
      (void)write_to_shared_mem_table( timeout_cnt, active_cnt,
                                       host_state, err_buf );
   }
   else
   {
	   sprintf(err_buf,"Unable to Increment timeout count. Problem with shared memory.Shutting down the service");
	   	nceqit_log_message(1, 2, err_buf, "increment_timeout_count", 0);
	   	shut_down();
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         RESET_TIMEOUT_COUNT
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
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_timeout_count( pCHAR errbuf )
{
   INT      ret_val = false;
   BYTE     active_cnt[4];
   BYTE     timeout_cnt[4];
   BYTE     host_state[2];

   /* Read the current values in order to echo back the counts. */
   ret_val = read_shared_mem_table( timeout_cnt, active_cnt,
                                    host_state, errbuf );
   if ( true == ret_val )
   {
      /* Update the values into shared memory, setting time out count to 0. */
      ret_val = write_to_shared_mem_table( "0", active_cnt,
                                           host_state, errbuf);
   }
   else
   {
	   sprintf(errbuf,"Unable to reset timeout count. Problem with shared memory.Shutting down the service");
	   nceqit_log_message(1, 2, errbuf, "reset_timeout_count", 0);
	   shut_down();

   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         ReadMemTable
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
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
XINT ReadMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
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
 *  NAME:         WriteMemTable
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
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
LONG WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state )
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

void shut_down(void)
{
   CHAR Buffer[100] = "";

   /* Added to fix a timing problem */
	#ifdef WIN32
		Sleep(2000);
	#else
		sleep(2);
	#endif

   /* Added to fix Equitable error report #852 */
   sprintf( Buffer,"Stopping nceqit program, version %s",Version);
   nceqit_log_message( 1, 1, Buffer, "shut_down",0 );

   pteipc_shutdown_single_instance_app();
   MainProcessDone = 1;

   return;

} /* shut_down */
