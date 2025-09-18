/******************************************************************************
  
        Module: dstimer.c
  
         Title: Pinnacle Dataserver Timer Routines
  
   Description:

   $Log:   N:\PVCS\PTE\CORE\ptedb_lib\DSTIMER.C  $
   
      Rev 1.7   Sep 28 1999 14:44:18   MSALEH
   AIX Modifications
   
      Rev 1.6   Aug 07 1998 10:17:16   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   Added a parameter to the startup routine to control whether the 
   timer routines user the timer database table.
   
   
      Rev 1.5   Jun 11 1998 13:24:04   drodden
   Changed delete-timer function to always remove the timer from
   the link list, even if it had an error deleting from the database.
   This is a merge of a change made in a branch in v1.0.5.
   
   
      Rev 1.4   May 19 1998 09:28:46   drodden
   Changed the dataserver timer routines to NOT send a timeout if the 
   timer request did originate on this system.  This is for high availability
   systems. 
   
   
      Rev 1.3   Apr 23 1998 13:30:10   drodden
   Changed to call the dbcommon database routines in ptedb.lib
   
   
      Rev 1.2   Jan 26 1998 09:48:16   drodden
   Changed the dataserver timer to use a table in the database,
   including commit/rollback logic to recover the timer table.
   
   
      Rev 1.1   Jan 06 1998 09:47:58   drodden
   Added the timer functionality to the dataservers.
   
   
      Rev 1.0   19 Dec 1997 11:24:58   rmalathk
   Initial Revision
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "dstimer.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ptetimer.h"
#include "pte.h"
#include "ntutils.h"
#include "dbcommon.h"
#include <string.h>



extern BOOLEAN in_refeed_mode;



PRIVATE const pCHAR txt_check_for_timeouts   = "dstimer_check_for_timeouts: ";
PRIVATE const pCHAR txt_startup              = "dstimer_startup: ";
PRIVATE const pCHAR txt_shutdown             = "dstimer_shutdown: ";
PRIVATE const pCHAR txt_start_a_timer        = "start_a_timer: ";
PRIVATE const pCHAR txt_read_a_timer         = "read_a_timer: ";
PRIVATE const pCHAR txt_clear_a_timer        = "clear_a_timer: ";
PRIVATE const pCHAR txt_build_timer_reply    = "build_timer_reply: ";
PRIVATE const pCHAR txt_process_request      = "dstimer_process_request: ";
PRIVATE const pCHAR txt_insert_timer         = "insert_timer: ";
PRIVATE const pCHAR txt_delete_timer         = "delete_timer: ";
/* PRIVATE const pCHAR txt_  = ; */

PRIVATE const pCHAR txt_starting_the_app     = "starting the Service";
PRIVATE const pCHAR txt_stopping_the_app     = "shutting down the Service";
PRIVATE const pCHAR txt_startup_failed       = "pteinit startup failed";
PRIVATE const pCHAR txt_ipc_receive_error    = "ipc receive error: ";
PRIVATE const pCHAR txt_ipc_send_error       = "ipc send error: ";
PRIVATE const pCHAR txt_invalid_msg_type     = "invalid msg_type received";
PRIVATE const pCHAR txt_invalid_timer_req    = "invalid timer request received";
PRIVATE const pCHAR txt_alloc_element        = "error allocating new list element";
PRIVATE const pCHAR txt_duplicate            = "key already exists: ";
PRIVATE const pCHAR txt_not_found            = "entry does not exist for key: ";
PRIVATE const pCHAR txt_timeout_occurred     = "timeout occurred for key: ";
PRIVATE const pCHAR txt_alloc_pte_msg        = "error allocating pte msg";
PRIVATE const pCHAR txt_starting_timer       = "starting a timer for key: ";
PRIVATE const pCHAR txt_reading_timer        = "reading a timer for key: ";
PRIVATE const pCHAR txt_clearing_timer       = "clearing a timer for key: ";
PRIVATE const pCHAR txt_table_read_error     = "error reading timer table from database: ";
PRIVATE const pCHAR txt_insert_error         = "error inserting timer into database: ";
PRIVATE const pCHAR txt_delete_error         = "error deleting timer from database: ";
/* PRIVATE const pCHAR txt_  = ; */


PRIVATE LINKED_LIST timer_list;
PRIVATE LONG        last_time_list_checked = 0;
PRIVATE BOOLEAN     updates_pending        = false;
PRIVATE BOOLEAN     using_database         = true;


#ifdef _DEBUG
PRIVATE BOOLEAN     display_status_messages = true;
#else
PRIVATE BOOLEAN     display_status_messages = false;
#endif



/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE void log_a_message ( pCHAR proc_name,
                             pCHAR msg1,
                             pCHAR msg2,
                             int   type       )
{
   LONG        buf_avail;
   LONG        move_len;
   CHAR        buffer [100];


   /*
    *  Start out with a null filled buffer and put in the 
    *  procedure name.
    */
   memset (buffer, 0, sizeof(buffer));

   if (proc_name != NULL_PTR)
      strncpy (buffer, proc_name, sizeof(buffer)-1);

   if (msg1 != NULL_PTR)
   {
      /*
       *  We have a first message string.
       *  Determine the amount of space left in the buffer and
       *  concat as much as we can of the 1st msg text.
       */
      buf_avail = sizeof(buffer) - strlen(buffer) - 1;
      move_len  = strlen(msg1);

      if (move_len > buf_avail)
         move_len = buf_avail;

      strncat (buffer, msg1, move_len);
   }

   if (msg2 != NULL_PTR)
   {
      /*
       *  We have a second message string.
       *  Determine the amount of space left in the buffer and
       *  concat as much as we can of the 2nd msg text.
       */
      buf_avail = sizeof(buffer) - strlen(buffer) - 1;
      move_len  = strlen(msg2);

      if (move_len > buf_avail)
         move_len = buf_avail;

      strncat (buffer, msg2, move_len);
   }

   if (display_status_messages == true)
      printf ("%s\n", buffer);

   if (type != 0)
      LogEvent (buffer, type);
}



/*******************************************************************************
*
*
*
*
*
*
*
*******************************************************************************/
PRIVATE pPTE_MSG build_timer_reply ( pTIMER_DATA p_timer_data,
                                     BYTE        msg_type,
                                     BYTE        msg_subtype1,
                                     pCHAR       dest_que,
                                     BYTE        result_code  )
{
   LONG                 app_data_len;
   pBYTE                p_data;
   pBYTE                p_temp;
   pPTE_MSG             p_reply_msg;
   pTIMER_REPLY_DATA    p_reply_data;


   p_reply_msg = NULL_PTR;

   if (p_timer_data != NULL_PTR)
   {
      /*
       *  I have reply data to send, build a message with it.
       */
      app_data_len = (p_timer_data->timer_size - sizeof(TIMER_DATA));

      if ( (p_reply_data = malloc (sizeof(TIMER_REPLY_DATA) + app_data_len)) != NULL_PTR)
      {
         memset (p_reply_data, 0, (sizeof(TIMER_REPLY_DATA) + app_data_len));
         memcpy (p_reply_data->timer_id,  p_timer_data->timer_id,  sizeof(p_reply_data->timer_id));
         memcpy (p_reply_data->app_data1, p_timer_data->app_data1, sizeof(p_reply_data->app_data1 ));
         memcpy (p_reply_data->app_data2, p_timer_data->app_data2, sizeof(p_reply_data->app_data2 ));

         if (app_data_len > 0)
         {
            p_data = (pBYTE) p_timer_data;
            p_data+= sizeof(TIMER_DATA);
            p_temp = (pBYTE) p_reply_data;
            p_temp+= sizeof(TIMER_REPLY_DATA);

            memcpy (p_temp, p_data, app_data_len);
         }

         p_reply_msg = ptemsg_build_msg (msg_type,
                                         msg_subtype1,
                                         ST2_NONE,
                                         dest_que,
                                         application_que_name,
                                         (pBYTE) p_reply_data,
                                         (sizeof(TIMER_REPLY_DATA) + app_data_len),
                                         0);

         free (p_reply_data);
      }
   }
   else
   {
      /*
       *  Build a message without any reply data.  Will just send a result code.
       */
      p_reply_msg = ptemsg_build_msg (msg_type,
                                      msg_subtype1,
                                      ST2_NONE,
                                      dest_que,
                                      application_que_name,
                                      NULL_PTR,
                                      0,
                                      0);
   }

   if (p_reply_msg == NULL_PTR)
      log_a_message (txt_build_timer_reply, txt_alloc_pte_msg, NULL_PTR, ERROR_MSG);
   else
      ptemsg_set_result_code (p_reply_msg, result_code);
   
   return (p_reply_msg);
}



/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pLIST_ELEMENT scan_the_list ( pCHAR timer_id )
{
   pTIMER_DATA    p_timer;
   pLIST_ELEMENT  p_element;

   
   /*
    *  Scan through the link list looking for a match on the lookup key.
    */
   p_element = ptellist_first_element (&timer_list);

   while (p_element != NULL_PTR)
   {
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);

      if (memcmp (timer_id, p_timer->timer_id, sizeof(p_timer->timer_id)) == 0)
         break;

      p_element = ptellist_next_element (p_element);
   }  /* while */

   return (p_element);
}  /* scan_the_list */



/******************************************************************************
*
*
*
*
*
******************************************************************************/
PRIVATE pLIST_ELEMENT add_to_list ( pTIMER_DATA p_new_timer )
{
   pLIST_ELEMENT  p_element;
   pTIMER_DATA    p_timer;

   
   /*
    *  Insert this timer into the link list so that the 
    *  list is sorted by the timeouts.  Handles all cases:
    *    a) list is empty.
    *    b) insert into the list.
    *    c) append to the list.
    */
   p_element = ptellist_first_element (&timer_list);

   while (p_element != NULL_PTR)
   {
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);

      /*
       *  If the current element timeout is > new timeout I want to insert
       *  in front of this element.  This also places the new timer after
       *  any duplicate timeouts that may be in the list.
       */
      if (p_timer->timeout > p_new_timer->timeout)
         break;      

      p_element = ptellist_next_element (p_element);
   }  /* while */


   p_element = ptellist_insert_element (&timer_list,
                                        p_element,
                                       (pBYTE) p_new_timer, 
                                       p_new_timer->timer_size);

   return (p_element);
}  /* add_to_list */



/******************************************************************************
*
*
*
*
*
******************************************************************************/
PRIVATE BYTE insert_timer ( pTIMER_DATA p_new_timer )
{
   BYTE           rcode;
   CHAR           error_msg [100];
   pLIST_ELEMENT  p_element;

   
   /*
    *  If the lookup key is already in the table, duplicate error.
    */
   if (scan_the_list (p_new_timer->timer_id) != NULL_PTR)
   {
      log_a_message (txt_insert_timer, txt_duplicate, p_new_timer->timer_id, WARN_MSG);
      return (PTEMSG_DUPLICATE_ENTRY);
   }

   /*
    *  Insert the timer into the link list.
    */
   if ( (p_element = add_to_list (p_new_timer)) == NULL_PTR)
   {
      log_a_message (txt_insert_timer, txt_alloc_element, p_new_timer->timer_id, ERROR_MSG);
      return (PTEMSG_INSUFFICIENT_MEMORY);
   }

   if (using_database == true)
   {
      /*
       *  Insert the timer into the database.
       */
      if ( (rcode = dbcommon_insert_timer (p_new_timer, error_msg)) != PTEMSG_OK )
      {
         log_a_message (txt_insert_timer, txt_insert_error, p_new_timer->timer_id, ERROR_MSG);

         ptellist_delete_element (&timer_list, p_element);
      }
      else
         updates_pending = true;
   }
   else
      rcode = PTEMSG_OK;

   return (rcode);
}  /* insert_timer */



/******************************************************************************
*
*
*
*
*
******************************************************************************/
PRIVATE BYTE delete_timer ( pLIST_ELEMENT  p_element,
                            pTIMER_DATA    p_timer    )
{
   BYTE           rcode;
   CHAR           error_msg [100];


   if (using_database == true)
   {
      if ( (rcode = dbcommon_delete_timer (p_timer, error_msg)) != PTEMSG_OK )
      {
         if (rcode == PTEMSG_NOT_FOUND)
            log_a_message (txt_delete_timer, txt_not_found,    p_timer->timer_id, ERROR_MSG);
         else
            log_a_message (txt_delete_timer, txt_delete_error, p_timer->timer_id, ERROR_MSG);
      }
      else
         updates_pending = true;
   }
   else
      rcode = PTEMSG_OK;

   ptellist_delete_element (&timer_list, p_element);

   return (rcode);
}  /* delete_timer */



/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG start_a_timer ( pPTE_MSG p_msg_in )
{
   LONG              app_data_len;
   pBYTE             p_data;
   pBYTE             p_temp;
   pTIMER_DATA       p_new_timer;
   pPTE_MSG_DATA     p_msg_data;
   pTIMER_START_DATA p_request;
   pPTE_MSG          p_reply_msg;


   p_msg_data = ptemsg_get_pte_msg_data      (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);
   p_request  = (pTIMER_START_DATA) p_data;

   log_a_message (txt_start_a_timer, txt_starting_timer, p_request->timer_id, 0);

   /*
    *  Create the response message.
    */
   p_reply_msg = build_timer_reply (NULL_PTR,
                                    MT_TIMER_REPLY,
                                    ptemsg_get_msg_subtype1   (p_msg_in),
                                    ptemsg_get_msg_orig_queue (p_msg_in),
                                    PTEMSG_OK);

   if (p_reply_msg == NULL_PTR)
      return (NULL_PTR);

   /*
    *  Build the new timer and put it into the link list.
    *  If the caller sent extra app data to be stored with the timer,
    *  be sure to allocate room for it and copy it in.
    */
   app_data_len = ptemsg_get_pte_msg_data_data_length (p_msg_data) - 
                  sizeof(TIMER_START_DATA);

   if ( (p_new_timer = malloc (sizeof(TIMER_DATA) + app_data_len)) == NULL_PTR)
   {
      ptemsg_set_result_code (p_reply_msg, PTEMSG_INSUFFICIENT_MEMORY);
      return (p_reply_msg);
   }

   memset (p_new_timer, 0, (sizeof(TIMER_DATA) + app_data_len));
   memcpy (p_new_timer->timer_id,    p_request->timer_id,  sizeof(p_new_timer->timer_id ));
   memcpy (p_new_timer->app_data1,   p_request->app_data1, sizeof(p_new_timer->app_data1));
   memcpy (p_new_timer->app_data2,   p_request->app_data2, sizeof(p_new_timer->app_data2));
   memcpy (p_new_timer->reply_que,   p_request->reply_que, sizeof(p_new_timer->reply_que));
   memcpy (p_new_timer->orig_system, ptemsg_get_msg_orig_system(p_msg_in), sizeof(p_new_timer->orig_system));
   p_new_timer->timeout    = atol(p_request->timeout);
   p_new_timer->interval   = p_request->interval;
   p_new_timer->timer_size = (sizeof(TIMER_DATA) + app_data_len);

   if (app_data_len > 0) 
   {
      p_temp = (pBYTE)p_new_timer;
      p_temp+= sizeof(TIMER_DATA);
      p_data+= sizeof(TIMER_START_DATA);
      memcpy (p_temp, p_data, app_data_len);
   }

   ptemsg_set_result_code (p_reply_msg, insert_timer (p_new_timer));

   free (p_new_timer);

   return (p_reply_msg);
}  /* start_a_timer */



/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG read_a_timer ( pPTE_MSG p_msg_in )
{
   BYTE              rcode;
   pTIMER_DATA       p_timer;
   pPTE_MSG_DATA     p_msg_data;
   pTIMER_KEY_DATA   p_request;
   pPTE_MSG          p_reply_msg;
   pLIST_ELEMENT     p_element;


   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_request  = (pTIMER_KEY_DATA) ptemsg_get_pte_msg_data_data (p_msg_data);
 
   if (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_CLEAR)
      log_a_message (txt_read_a_timer, txt_clearing_timer, p_request->timer_id, 0);
   else
      log_a_message (txt_read_a_timer, txt_reading_timer, p_request->timer_id, 0);

   p_element = scan_the_list (p_request->timer_id);

   if (p_element == NULL_PTR)
   {
      /*
       *  The lookup key is not found in the table.
       */
      log_a_message (txt_read_a_timer, txt_not_found, p_request->timer_id, WARN_MSG);

      p_timer = NULL_PTR;
      rcode   = PTEMSG_NOT_FOUND;
   }
   else
   {
      /*
       *  The found it OK.
       */
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);
      rcode   = PTEMSG_OK;
   }

   /*
    *  Build the response and delete the element.
    */
   p_reply_msg = build_timer_reply (p_timer,
                                    MT_TIMER_REPLY,
                                    ptemsg_get_msg_subtype1   (p_msg_in),
                                    ptemsg_get_msg_orig_queue (p_msg_in),
                                    rcode);

   if (p_reply_msg == NULL_PTR)
      return (NULL_PTR);

   /*
    *  Sent the response OK.  If this is a clear request, delete it from the list.
    */
   if ( (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_CLEAR) && 
        (p_element                          != NULL_PTR       )    )
   {
      ptemsg_set_result_code (p_reply_msg, delete_timer (p_element, p_timer));
   }

   return (p_reply_msg);
}  /* read_a_timer */


/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG read_a_timer_rt_approved ( pPTE_MSG p_msg_in )
{
   BYTE              rcode;
   pTIMER_DATA       p_timer;
   pPTE_MSG_DATA     p_msg_data;
   pTIMER_KEY_DATA   p_request;
   pPTE_MSG          p_reply_msg;
   pLIST_ELEMENT     p_element;


   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_request  = (pTIMER_KEY_DATA) ptemsg_get_pte_msg_data_data (p_msg_data);

   if ((ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_APPROVED))
      log_a_message (txt_read_a_timer, txt_clearing_timer, p_request->timer_id, 0);
   else
      log_a_message (txt_read_a_timer, txt_reading_timer, p_request->timer_id, 0);

   p_element = scan_the_list (p_request->timer_id);

   if (p_element == NULL_PTR)
   {
      /*
       *  The lookup key is not found in the table.
       */
      log_a_message (txt_read_a_timer, txt_not_found, p_request->timer_id, WARN_MSG);

      p_timer = NULL_PTR;
      rcode   = PTEMSG_NOT_FOUND;
   }
   else
   {
      /*
       *  The found it OK.
       */
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);
      rcode   = PTEMSG_OK;
   }

   /*
    *  Build the response and delete the element.
    */
   p_reply_msg = build_timer_reply (p_timer,
                                    MT_TIMER_REPLY,
									ST1_TIMER_RT_APPROVED,
                                    ptemsg_get_msg_orig_queue (p_msg_in),
                                    rcode);

   if (p_reply_msg == NULL_PTR)
      return (NULL_PTR);

   /*
    *  Sent the response OK.  If this is a rt approved request, delete it from the list.
    */
   if ( (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_APPROVED) &&
        (p_element                          != NULL_PTR       )    )
   {
      ptemsg_set_result_code (p_reply_msg, delete_timer (p_element, p_timer));
   }

   return (p_reply_msg);
}  /* read_a_timer_rt_approved */


/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG read_a_timer_rt_declined ( pPTE_MSG p_msg_in )
{
   BYTE              rcode;
   pTIMER_DATA       p_timer;
   pPTE_MSG_DATA     p_msg_data;
   pTIMER_KEY_DATA   p_request;
   pPTE_MSG          p_reply_msg;
   pLIST_ELEMENT     p_element;


   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_request  = (pTIMER_KEY_DATA) ptemsg_get_pte_msg_data_data (p_msg_data);

   if ((ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_DECLINED))
      log_a_message (txt_read_a_timer, txt_clearing_timer, p_request->timer_id, 0);
   else
      log_a_message (txt_read_a_timer, txt_reading_timer, p_request->timer_id, 0);

   p_element = scan_the_list (p_request->timer_id);

   if (p_element == NULL_PTR)
   {
      /*
       *  The lookup key is not found in the table.
       */
      log_a_message (txt_read_a_timer, txt_not_found, p_request->timer_id, WARN_MSG);

      p_timer = NULL_PTR;
      rcode   = PTEMSG_NOT_FOUND;
   }
   else
   {
      /*
       *  The found it OK.
       */
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);
      rcode   = PTEMSG_OK;
   }

   /*
    *  Build the response and delete the element.
    */
   p_reply_msg = build_timer_reply (p_timer,
                                    MT_TIMER_REPLY,
									ST1_TIMER_RT_DECLINED,
                                    ptemsg_get_msg_orig_queue (p_msg_in),
                                    rcode);

   if (p_reply_msg == NULL_PTR)
      return (NULL_PTR);

   /*
    *  Sent the response OK.  If this is a rt declined request, delete it from the list.
    */
   if ( (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_DECLINED) &&
        (p_element                          != NULL_PTR       )    )
   {
      ptemsg_set_result_code (p_reply_msg, delete_timer (p_element, p_timer));
   }

   return (p_reply_msg);
}  /* read_a_timer_rt_declined */


/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG read_a_timer_rt_timeout( pPTE_MSG p_msg_in )
{
   BYTE              rcode;
   pTIMER_DATA       p_timer;
   pPTE_MSG_DATA     p_msg_data;
   pTIMER_KEY_DATA   p_request;
   pPTE_MSG          p_reply_msg;
   pLIST_ELEMENT     p_element;


   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_request  = (pTIMER_KEY_DATA) ptemsg_get_pte_msg_data_data (p_msg_data);

   if ( (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_TIMEOUT) )
      log_a_message (txt_read_a_timer, txt_clearing_timer, p_request->timer_id, 0);
   else
      log_a_message (txt_read_a_timer, txt_reading_timer, p_request->timer_id, 0);

   p_element = scan_the_list (p_request->timer_id);

   if (p_element == NULL_PTR)
   {
      /*
       *  The lookup key is not found in the table.
       */
      log_a_message (txt_read_a_timer, txt_not_found, p_request->timer_id, WARN_MSG);

      p_timer = NULL_PTR;
      rcode   = PTEMSG_NOT_FOUND;
   }
   else
   {
      /*
       *  The found it OK.
       */
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);
      rcode   = PTEMSG_OK;
   }

   /*
    *  Build the response and delete the element.
    */
   p_reply_msg = build_timer_reply (p_timer,
                                    MT_TIMER_REPLY,
									ST1_TIMER_RT_TIMEOUT,
                                    ptemsg_get_msg_orig_queue (p_msg_in),
                                    rcode);

   if (p_reply_msg == NULL_PTR)
      return (NULL_PTR);

   /*
    *  Sent the response OK.  If this is a rt timeout request, delete it from the list.
    */
   if ( (ptemsg_get_msg_subtype1 (p_msg_in) == ST1_TIMER_RT_TIMEOUT) &&
        (p_element                          != NULL_PTR       )    )
   {
      ptemsg_set_result_code (p_reply_msg, delete_timer (p_element, p_timer));
   }

   return (p_reply_msg);
}  /* read_a_timer_rt_timeout */



/******************************************************************************
*
*
*
******************************************************************************/
PRIVATE pPTE_MSG clear_a_timer ( pPTE_MSG p_msg_in )
{
   return (read_a_timer (p_msg_in));
}



/******************************************************************************
*
*
*
******************************************************************************/
pPTE_MSG dstimer_process_request ( pPTE_MSG p_msg_in,
                                   pBOOLEAN p_updates_to_db_made )
{
   pPTE_MSG    p_msg_out;

   
   if ( (p_msg_in                       == NULL_PTR        ) || 
        (ptemsg_get_msg_type (p_msg_in) != MT_TIMER_REQUEST)    )
      return (NULL_PTR);
   
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_TIMER_START  : p_msg_out = start_a_timer (p_msg_in);
                              break;
      case ST1_TIMER_READ   : p_msg_out = read_a_timer  (p_msg_in);
                              break;
      case ST1_TIMER_CLEAR  : p_msg_out = clear_a_timer (p_msg_in);
                              break;
      case ST1_TIMER_RT_APPROVED  : p_msg_out = read_a_timer_rt_approved (p_msg_in);
                                    break;
      case ST1_TIMER_RT_DECLINED  : p_msg_out = read_a_timer_rt_declined (p_msg_in);
                                    break;
      case ST1_TIMER_RT_TIMEOUT   : p_msg_out = read_a_timer_rt_timeout (p_msg_in);
                                    break;

      default :
         log_a_message (txt_process_request, txt_invalid_timer_req,
                        NULL_PTR, ERROR_MSG);

         p_msg_out = NULL_PTR;
         break;
   }  /* switch subtype1 */

   *p_updates_to_db_made = updates_pending;
   return (p_msg_out);
}



/******************************************************************************
*
*
*
******************************************************************************/
void dstimer_check_for_timeouts ()
{
   BYTE           rcode;
   LONG           ipc_rcode;
   LONG           current_time;
   CHAR           error_msg [80];
   pPTE_MSG       p_reply_msg;
   pLIST_ELEMENT  p_element;
   pLIST_ELEMENT  p_next_element;
   pTIMER_DATA    p_timer;
   pTIMER_DATA    p_save_timer;

   
   current_time = time (NULL_PTR);

   if ((current_time - last_time_list_checked) < TIME_BETWEEN_CHECKS)
      return;

   /*
    *  Scan the link list to see if there are any that have expired.  
    *  Keep looping until I reach the last element.  Note that I'm 
    *  getting the last element now, rather than in the loop.
    */
   rcode        = PTEMSG_OK;
   p_save_timer = NULL_PTR;
   p_element    = ptellist_first_element (&timer_list);

   while (p_element != NULL_PTR)
   {
      /*
       *  Check this element to see if it has reached a timeout (current
       *  time is >= the element timeout).
       *
       *  Note: The link list is sorted by the element timeout field.
       *        If the current time is < this element's timeout, I don't
       *        need to check any more elements.
       */
      p_timer = (pTIMER_DATA) ptellist_element_data (p_element);

      if (current_time < p_timer->timeout)
         break;

      /*
       *  Log a message that a timeout has occurred.
       */
      log_a_message (txt_check_for_timeouts, txt_timeout_occurred, p_timer->timer_id, 0);

      /*
       *  Save a copy of the timer.
       */
      if ( (p_save_timer = malloc (p_timer->timer_size)) == NULL_PTR)
      {
         log_a_message (txt_startup, txt_alloc_element, 
                        p_timer->timer_id, ERROR_MSG);
         rcode = PTEMSG_INSUFFICIENT_MEMORY;
         break;
      }

      memcpy (p_save_timer, p_timer, p_timer->timer_size);

      /*
       *  Delete the timer.
       */
       p_next_element = ptellist_next_element (p_element);

       if ( (rcode = delete_timer (p_element, p_save_timer)) != PTEMSG_OK)
          break;

      /*
       *  If there is an interval specified, reset the timeout using the 
       *  interval, so that it will expire again in interval number seconds,
       *  and add it as a new timer.  Do this so it will continue to expire 
       *  until the application sends a clear-timer for it.
       */
      if (p_save_timer->interval != 0)
      {
         p_save_timer->timeout = current_time + p_save_timer->interval;

         /*
          *  If I am unable to add the timer, really have a problem now.
          *  Need to force a rollback recovery of the timer table.
          */
         if ( (rcode = insert_timer (p_save_timer)) != PTEMSG_OK)
            break;
      }


      /*
       *  Only send a timeout message if it originated on this system.
       */
      if (strcmp (p_save_timer->orig_system, GetThisSystemsName()) == 0)
      {
         /*
          *  Send the timeout message to the application.
          */
         p_reply_msg = build_timer_reply (p_save_timer,
                                          MT_TIMER_TIMEOUT,
                                          ST1_NONE,
                                          p_save_timer->reply_que,
                                          PTEMSG_OK);

         if (p_reply_msg != NULL_PTR)
         {
            ipc_rcode = pteipc_send (p_reply_msg, p_save_timer->reply_que);

            free (p_reply_msg);
   
            if (ipc_rcode < 0)
            {
               pteipc_get_errormsg (ipc_rcode, error_msg);
               log_a_message       (txt_check_for_timeouts, txt_ipc_send_error, 
                                    error_msg, ERROR_MSG);
            }
         }

         if ( (p_reply_msg == NULL_PTR) || (ipc_rcode < 0) )
         {
            /*
             *  Could not deliver the timeout message.  If there is NOT an interval, 
             *  the timer is not in the table AND the application doesn't know about
             *  the timeout.  This is bad.  Try to recover.
             *  Set an interval and new timeout, then put the timer back into the 
             *  table so that it will expire again in interval number of seconds.
             *  Do this so it will continue to expire until able to send the timeout
             *  to the application and it sends a clear-timer for it.
             */
            if (p_save_timer->interval == 0)
            {
               p_save_timer->interval = 3;
               p_save_timer->timeout  = current_time + p_save_timer->interval;

               /*
                *  If I am unable to add the timer, really have a problem now.
                *  Need to force a rollback recovery of the timer table.
                */
               if ( (rcode = insert_timer (p_save_timer)) != PTEMSG_OK)
                  break;
            }
         }
      }  /* if it originated on this system */

      free (p_save_timer);
      p_save_timer = NULL_PTR;

      /*
       *  next element.
       */
      p_element = p_next_element;
   }  /* while */

   if (p_save_timer == NULL_PTR)
      free (p_save_timer);

   if (rcode != PTEMSG_OK)
   {
      if (updates_pending == true)
      {
         if (using_database == true)
         {
            dbcommon_rollback();
            dstimer_rollback ();
         }

         updates_pending = false;
      }
   }
   else
   {
      if (updates_pending == true)
      {
         if (using_database == true)
         {
            dbcommon_commit ();
         }

         updates_pending = false;
      }
   }

   last_time_list_checked = time (NULL_PTR);
}  /* check_for_timeouts */



/******************************************************************************
*  function dstimer_startup
*
*  Call this after logging into the ipc instance.
*
******************************************************************************/
BOOLEAN dstimer_startup ( BOOLEAN use_database )
{
   int            i;
   CHAR           error_msg [100];
   TIMER_DB_LIST  db_list;
   
   
   memset (&timer_list, 0, sizeof(timer_list));

   ptellist_init_list (&timer_list);

   using_database = use_database;

   if (using_database == true)
   {
      if (dbcommon_get_timer_table (&db_list, error_msg) != PTEMSG_OK)
      {
         log_a_message (txt_startup, txt_table_read_error, error_msg, ERROR_MSG);
         return (false);
      }

      for (i=0; i < db_list.num_returned; i++)
      {
         if (add_to_list (&db_list.db_rec [i]) == NULL_PTR)
            {
               log_a_message (txt_startup, txt_alloc_element, 
                              db_list.db_rec [i].timer_id, ERROR_MSG);
               return (PTEMSG_INSUFFICIENT_MEMORY);
            }
      }
   }

   last_time_list_checked = time (NULL_PTR);
   return (true);
}



/******************************************************************************
*  function dstimer_shutdown
*
*  Call this before logging out of the ipc instance.
*
******************************************************************************/
void dstimer_shutdown ()
{
   /*
    * Don't worry to much about the timers that may be in the list.  They
    * are in the logger tran file an we'll get them again when the dataserver
    * does a restart.  About all I'm going to do is force a check of the list
    * to send any timeouts that might be pending.
    */
   last_time_list_checked = 0;

   dstimer_check_for_timeouts ();

   ptellist_free_list_memory (&timer_list);
}



/******************************************************************************
*  function dstimer_commit
*
*  
*
******************************************************************************/
void dstimer_commit ()
{

   updates_pending = false;

}



/******************************************************************************
*  function dstimer_rollback
*
*  
*
******************************************************************************/
void dstimer_rollback ()
{
   if (updates_pending == true)
   {
      dstimer_shutdown ();

      dstimer_startup (using_database);

      updates_pending = false;
   }
}


