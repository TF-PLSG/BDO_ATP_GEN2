/******************************************************************************

        Module: ptetimer.h
  
         Title: APIs to construct state timer request messages
  
   Description: This file contains all definitions and structures used to
                build messages to be sent between processes for the Pinnacle
                Transaction Environment.

  $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTETIMER.H  $
   
      Rev 1.3   Aug 07 1998 10:06:42   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   
   
      Rev 1.2   Jan 26 1998 09:49:46   drodden
   Changed the dataserver timer to use a table in the database,
   including commit/rollback logic to recover the timer table.
   
   
      Rev 1.1   Dec 23 1997 08:10:18   drodden
   Improved the parameter documentation.
   
   
      Rev 1.0   Dec 16 1997 12:25:18   drodden
   Initial code.
   

******************************************************************************/

#ifndef PTETIMER_H
#define PTETIMER_H

#include "basictyp.h"
#include "ptemsg.h"


#define MAX_TIMER_KEY_SIZE 24
#define MAX_APP_DATA_SIZE  32


typedef struct {
   CHAR  timer_id  [MAX_TIMER_KEY_SIZE];
   CHAR  app_data1 [MAX_APP_DATA_SIZE];
   CHAR  app_data2 [MAX_APP_DATA_SIZE];
   CHAR  reply_que [MAX_QUE_NAME_SIZE];
   CHAR  timeout   [MAX_ITOA_SIZE];
   BYTE  interval;
} TIMER_START_DATA, *pTIMER_START_DATA;


typedef struct {
   CHAR  timer_id  [MAX_TIMER_KEY_SIZE];
} TIMER_KEY_DATA, *pTIMER_KEY_DATA;


typedef struct {
   CHAR  timer_id  [MAX_TIMER_KEY_SIZE];
   CHAR  app_data1 [MAX_APP_DATA_SIZE];
   CHAR  app_data2 [MAX_APP_DATA_SIZE];
} TIMER_REPLY_DATA, *pTIMER_REPLY_DATA;



/*******************************************************************************
*  function ptetimer_bld_start_timer_req
*
*  This function will create a timer start request PTE message using the info
*  passed in.  It will return a pointer to the message.  It is up to the 
*  application to send it to the dataserver.  Normally this message would be 
*  chained behind a database update message.
*
*  Parameters:
*     timer_id........string containing the unique key that this timer can be 
*                     identified by in subsequent read/clear calls.
*     app_data1.......application defineable data (NULL_PTR if not needed).
*     app_data2.......application defineable data (NULL_PTR if not needed).
*     dest_que........name of the dataserver's queue where this will be sent.
*     reply_que.......name of your queue where the timeout should be sent.
*     num_seconds.....number of seconds the timeout should be set for.
*     interval........if a timeout occurs, the timer will re-send the timeout 
*                     every "interval" number of seconds until a clear is
*                     received for this timer.  
*                     Passing in zero means the timeout will only be sent the
*                     first time (not recommended for normal app usage).
*                     If you are not sure, pass 3.
*     p_user_data.....Pointer to data that you would like stored with the timer
*                     and later returned by a read or clear timer call.  This 
*                     could be an auth_tx record or other state info.  If you
*                     don't want to use this feature, pass NULL_PTR.
*     user_data_len...Number of bytes in the above user data buffer.  If you
*                     don't want to use this feature, pass zero.
*
*  Returns:
*     Pointer to the message created, or NULL_PTR if a message could not
*     be allocated.
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
                                        LONG     user_data_len );


/*******************************************************************************
*  function ptetimer_bld_read_timer_req
*
*  This function will create a timer read request PTE message using the lookup
*  key passed in.  It will return a pointer to the message.  It is up to the 
*  application to send it to the dataserver.
*
*  The dataserver will send a reponse PTE message to this request.  If the 
*  result code in the response is PTEMSG_OK, use ptetimer_get_timer_reply
*  (below) to get the data out of the response message.
*
*  Parameters:
*     timer_id......string containing the unique key that the timer was 
*                     originally started with.
*     dest_que........name of the dataserver's queue where this will be sent.
*     reply_que.......name of your queue where the response should be sent.
*
*  Returns:
*     Pointer to the message created, or NULL_PTR if a message could not
*     be allocated.
*
*******************************************************************************/
pPTE_MSG ptetimer_bld_read_timer_req ( pCHAR timer_id,
                                       pCHAR dest_que,
                                       pCHAR reply_que  );



/*******************************************************************************
*  function ptetimer_bld_clear_timer_req
*
*  This function will create a timer read request PTE message using the lookup
*  key passed in.  It will return a pointer to the message.  It is up to the 
*  application to send it to the dataserver.  Normally the application would
*  chain this request in front of a database update message.
*
*  The dataserver will send a reponse PTE message to this request.  If the 
*  result code in the response is PTEMSG_OK, use ptetimer_get_timer_reply
*  (below) to get the data out of the response message.
*
*  Parameters:
*     timer_id......string containing the unique key that the timer was 
*                     originally started with.
*     dest_que........name of the dataserver's queue where this will be sent.
*     reply_que.......name of your queue where the response should be sent.
*
*  Returns:
*     Pointer to the message created, or NULL_PTR if a message could not
*     be allocated.
*
*******************************************************************************/
pPTE_MSG ptetimer_bld_clear_timer_req ( pCHAR timer_id,
                                        pCHAR dest_que,
                                        pCHAR reply_que  );



/*******************************************************************************
*  function ptetimer_get_timer_reply
*
*  This function take the timer response PTE message that you pass in and return 
*  the lookup key and app data fields to you.  The response message can either
*  be a response to your read/clear request (MT_TIMER_REPLY) or a timeout 
*  message (MT_TIMER_TIMEOUT).
*
*  Parameters:
*     timer_id..........string containing the unique key that the timer was 
*                       originally started with.
*     app_data1.........application defineable data sent in the original timer
*                       (NULL_PTR if you don't want to get this info back).
*     app_data2.........application defineable data sent in the original timer.
*                       (NULL_PTR if you don't want to get this info back).
*     p_user_data.......Pointer to data buffer where the user data (if any) that 
*                       was stored with the timer (see the bld_start_timer 
*                       function above) will be copied.  You must allocate the 
*                       buffer youself.  If you didn't store any data with the 
*                       timer or simply don't want the data back, pass NULL_PTR.
*     user_data_len.....The size of the above user data buffer.  If passing 
*                       NULL_PTR in the above parm, pass zero for this.
*
*  Returns: 
*     This function returns the number of bytes of user data copied to the
*     buffer pointed to by p_user_data.  If that parm is NULL_PTR or there 
*     isn't any user data in the timer reply, returns zero.  If you're passing
*     NULL_PTR in the p_user_data parm, just ignore/discard this return value.
*
*******************************************************************************/
LONG ptetimer_get_timer_reply ( pPTE_MSG p_msg,
                                pCHAR    timer_id,
                                pCHAR    app_data1,
                                pCHAR    app_data2,
                                pBYTE    p_user_data,
                                LONG     user_data_len );


#endif
