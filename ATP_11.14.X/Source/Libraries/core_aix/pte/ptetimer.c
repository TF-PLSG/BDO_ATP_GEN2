/******************************************************************************

        Module: ptetimer.c
  
         Title: APIs to construct/manipulate state timer messages
  
   Description:

  $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTETIMER.C  $
   
      Rev 1.3   Aug 07 1998 10:06:42   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   
   
      Rev 1.2   Jan 26 1998 09:49:44   drodden
   Changed the dataserver timer to use a table in the database,
   including commit/rollback logic to recover the timer table.
   
   
      Rev 1.1   Dec 23 1997 08:11:04   drodden
   Changed to check the app data parms for null ptrs.
   
   
      Rev 1.0   Dec 16 1997 12:25:16   drodden
   Initial code.
   

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ptetimer.h"
#include "pteipc.h" 
#include "pte.h"


/*******************************************************************************
*
*
*
*
*
*
*
*******************************************************************************/
pPTE_MSG ptetimer_bld_start_timer_req ( pCHAR    timer_id,
                                        pCHAR    app_data1,
                                        pCHAR    app_data2,
                                        pCHAR    dest_que,
                                        pCHAR    reply_que,
                                        WORD     num_seconds,
                                        BYTE     interval,
                                        pBYTE    p_user_data,
                                        LONG     user_data_len )
{
   LONG                 temp_time;
   pBYTE                p_temp;
   pTIMER_START_DATA    p_start_data;
   pPTE_MSG             p_msg;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   if (p_user_data == NULL_PTR)
      user_data_len = 0;

   if ( (p_start_data = malloc (sizeof(TIMER_START_DATA) + user_data_len)) == NULL_PTR)
      return (NULL_PTR);

   memset  (p_start_data, 0, (sizeof(TIMER_START_DATA) + user_data_len));
   strncpy (p_start_data->timer_id, timer_id, sizeof(p_start_data->timer_id) - 1);

   if (app_data1 != NULL_PTR)
      strncpy (p_start_data->app_data1, app_data1, sizeof(p_start_data->app_data1) - 1);

   if (app_data2 != NULL_PTR)
      strncpy (p_start_data->app_data2, app_data2, sizeof(p_start_data->app_data2) - 1);

   if (reply_que != NULL_PTR)
      strncpy (p_start_data->reply_que, reply_que, sizeof(p_start_data->reply_que) - 1);

   if ( (p_user_data != NULL_PTR) && (user_data_len > 0) )
   {
      p_temp = (pBYTE) p_start_data;
      p_temp+= sizeof(TIMER_START_DATA);
      memcpy (p_temp, p_user_data, user_data_len);
   }

   p_start_data->interval = interval;
   temp_time              = time(NULL_PTR) + num_seconds;
   ltoa (temp_time, p_start_data->timeout, 10);

   p_msg  = ptemsg_build_msg (MT_TIMER_REQUEST,
                              ST1_TIMER_START,
                              ST2_NONE,
                              dest_que,
                              reply_que,
                              (pBYTE) p_start_data,
                              (sizeof(TIMER_START_DATA) + user_data_len),
                              0);

   free (p_start_data);
   return (p_msg);
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
pPTE_MSG ptetimer_bld_read_timer_req ( pCHAR timer_id,
                                       pCHAR dest_que,
                                       pCHAR reply_que  )
{
   TIMER_KEY_DATA    key_data;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   memset  (&key_data, 0, sizeof(key_data));
   strncpy (key_data.timer_id, timer_id, sizeof(key_data.timer_id) - 1);

   return (ptemsg_build_msg (MT_TIMER_REQUEST,
                             ST1_TIMER_READ,
                             ST2_NONE,
                             dest_que,
                             reply_que,
                             (pBYTE) &key_data,
                             sizeof(key_data),
                             0));
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
pPTE_MSG ptetimer_bld_clear_timer_req ( pCHAR timer_id,
                                        pCHAR dest_que,
                                        pCHAR reply_que  )
{
   TIMER_KEY_DATA    key_data;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   memset  (&key_data, 0, sizeof(key_data));
   strncpy (key_data.timer_id, timer_id, sizeof(key_data.timer_id) - 1);

   return (ptemsg_build_msg (MT_TIMER_REQUEST,
                             ST1_TIMER_CLEAR,
                             ST2_NONE,
                             dest_que,
                             reply_que,
                             (pBYTE) &key_data,
                             sizeof(key_data),
                             0));
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
pPTE_MSG ptetimer_bld_clear_timer_req_rt_approved ( pCHAR timer_id,
                                        pCHAR dest_que,
                                        pCHAR reply_que  )
{
   TIMER_KEY_DATA    key_data;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   memset  (&key_data, 0, sizeof(key_data));
   strncpy (key_data.timer_id, timer_id, sizeof(key_data.timer_id) - 1);

   return (ptemsg_build_msg (MT_TIMER_REQUEST,
		   	   	   	   	   	 ST1_TIMER_RT_APPROVED,
                             ST2_NONE,
                             dest_que,
                             reply_que,
                             (pBYTE) &key_data,
                             sizeof(key_data),
                             0));
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
pPTE_MSG ptetimer_bld_clear_timer_req_rt_declined ( pCHAR timer_id,
                                        pCHAR dest_que,
                                        pCHAR reply_que  )
{
   TIMER_KEY_DATA    key_data;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   memset  (&key_data, 0, sizeof(key_data));
   strncpy (key_data.timer_id, timer_id, sizeof(key_data.timer_id) - 1);

   return (ptemsg_build_msg (MT_TIMER_REQUEST,
		   	   	   	   	     ST1_TIMER_RT_DECLINED,
                             ST2_NONE,
                             dest_que,
                             reply_que,
                             (pBYTE) &key_data,
                             sizeof(key_data),
                             0));
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
pPTE_MSG ptetimer_bld_clear_timer_req_rt_timeout( pCHAR timer_id,
                                        pCHAR dest_que,
                                        pCHAR reply_que  )
{
   TIMER_KEY_DATA    key_data;


   if (timer_id == NULL_PTR)
      return (NULL_PTR);

   memset  (&key_data, 0, sizeof(key_data));
   strncpy (key_data.timer_id, timer_id, sizeof(key_data.timer_id) - 1);

   return (ptemsg_build_msg (MT_TIMER_REQUEST,
		   	   	   	   	   	 ST1_TIMER_RT_TIMEOUT,
                             ST2_NONE,
                             dest_que,
                             reply_que,
                             (pBYTE) &key_data,
                             sizeof(key_data),
                             0));
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
LONG ptetimer_get_timer_reply ( pPTE_MSG p_msg,
                                pCHAR    timer_id,
                                pCHAR    app_data1,
                                pCHAR    app_data2,
                                pBYTE    p_user_data,
                                LONG     user_data_len )
{
   LONG                 data_len;
   pBYTE                p_data;
   pTIMER_REPLY_DATA    p_reply_data;
   pPTE_MSG_DATA        p_msg_data;
   

   data_len = 0;

   if (timer_id != NULL_PTR)
      timer_id [0] = '\0';
   
   if (app_data1 != NULL_PTR)
      app_data1 [0] = '\0';

   if (app_data2 != NULL_PTR)
      app_data2 [0] = '\0';

   if ( (p_user_data == NULL_PTR) || (user_data_len == 0) ) 
   {
      /* 
       * if the pointer is null or the length is zero, make both null: 
       * must have both.
       */
      p_user_data   = NULL_PTR;
      user_data_len = 0;
   }

   if (p_user_data != NULL_PTR)
      memset (p_user_data, 0, user_data_len);

   if (p_msg == NULL_PTR)
      return (0);

   if ( (ptemsg_get_msg_type (p_msg) == MT_TIMER_REPLY  ) ||
        (ptemsg_get_msg_type (p_msg) == MT_TIMER_TIMEOUT)    )
   {
      p_msg_data   = ptemsg_get_pte_msg_data (p_msg);
      p_reply_data = (pTIMER_REPLY_DATA) ptemsg_get_pte_msg_data_data (p_msg_data);

      if (p_reply_data != NULL_PTR)
      {
         if (timer_id != NULL_PTR)
            strcpy (timer_id, p_reply_data->timer_id);

         if (app_data1 != NULL_PTR)
            strcpy (app_data1,  p_reply_data->app_data1 );

         if (app_data2 != NULL_PTR)
            strcpy (app_data2,  p_reply_data->app_data2 );

         if (p_user_data != NULL_PTR)
         {
            data_len = ptemsg_get_pte_msg_data_data_length (p_msg_data) - 
                       sizeof(TIMER_REPLY_DATA); 

            if (data_len > 0)
            {
               /* 
                * there is user data attached to the timer info.  Copy it to
                * the caller's buffer.
                */
               if (data_len > user_data_len)
                  data_len = user_data_len;

               p_data = (pBYTE) p_reply_data;
               p_data+= sizeof(TIMER_REPLY_DATA);
               memcpy (p_user_data, p_data, data_len);
            }
         }  
      }
   }

   return (data_len);
}



