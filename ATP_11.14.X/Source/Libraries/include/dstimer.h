/******************************************************************************
  
        Module: dstimer.h
  
         Title: Pinnacle Dataserver Timer Routines
  
   Description:

   $Log:   N:\PVCS\PTE\CORE\ptedb_lib\DSTIMER.H  $
   
      Rev 1.3   Aug 07 1998 10:17:16   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   Added a parameter to the startup routine to control whether the 
   timer routines user the timer database table.
   
   
      Rev 1.2   Jan 26 1998 09:48:16   drodden
   Changed the dataserver timer to use a table in the database,
   including commit/rollback logic to recover the timer table.
   
   
      Rev 1.1   Jan 06 1998 09:48:00   drodden
   Added the timer functionality to the dataservers.
   
   
      Rev 1.0   19 Dec 1997 11:24:58   rmalathk
   Initial Revision
   
******************************************************************************/

#ifndef DSTIMER_H
#define DSTIMER_H


#include "basictyp.h" 
#include "ptemsg.h" 


/*
 * When your dataserver is providing timer functions for other
 * applications, use this timeout value for your IPC receive.
 * After you process each IPC message received (OR the receive 
 * times out), call the check_for_timeouts routine.
 */
//#define TIME_BETWEEN_CHECKS   1L    /* 1 second */



/******************************************************************************
*  function dstimer_startup
*
*  Call this just before going into your IPC receive loop.
*
*  Parameters:
*     use_database.....Pass in true and these routines will store the timers
*                      in the timer database table.  Otherwise the database
*                      table is not used.  If you are not absolutely sure,
*                      pass true.
*
******************************************************************************/
BOOLEAN dstimer_startup ( BOOLEAN use_database );


/******************************************************************************
*  function dstimer_shutdown
*
*  Call this just after your IPC receive loop.
*
******************************************************************************/
void dstimer_shutdown ();


/******************************************************************************
*  function dstimer_check_for_timeouts
*
*  Call this function at the bottom of your receive loop.  It should be called 
*  after processing each IPC msg received OR after the IPC receive times out.
*
******************************************************************************/
void dstimer_check_for_timeouts ();


/******************************************************************************
*  function dstimer_clear_a_timer
*
*  Call this function to process the timer request, p_msg_in (ie msg_type is
*  MT_TIMER_REQUEST).
*
******************************************************************************/
pPTE_MSG dstimer_process_request ( pPTE_MSG p_msg_in,
                                   pBOOLEAN p_updates_to_db_made );


/******************************************************************************
*  function dstimer_commit
*
*  
*
******************************************************************************/
void dstimer_commit ();


/******************************************************************************
*  function dstimer_rollback
*
*  
*
******************************************************************************/
void dstimer_rollback ();


#endif


