/******************************************************************************
  
        Module: dbtimer.h
  
         Title: Dataserver Timer DB Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\SYSTEM\dbtimer.h  $
   
      Rev 1.0   Jan 26 1998 09:48:22   drodden
   Changed the dataserver timer to use a table in the database,
   including commit/rollback logic to recover the timer table.
   

******************************************************************************/

#ifndef DBTIMER_H
#define DBTIMER_H

#include "basictyp.h"
#include "ptetimer.h"


typedef struct {
   CHAR  timer_id   [MAX_TIMER_KEY_SIZE];
   CHAR  app_data1  [MAX_APP_DATA_SIZE];
   CHAR  app_data2  [MAX_APP_DATA_SIZE];
   CHAR  reply_que  [MAX_QUE_NAME_SIZE];
   LONG  timeout;
   LONG  interval;
} TIMER_DATA, *pTIMER_DATA;


#define TIMER_DB_LIST_SIZE  500

typedef struct 
{
   INT         num_returned;
   TIMER_DATA  db_rec [TIMER_DB_LIST_SIZE];
} TIMER_DB_LIST, *pTIMER_DB_LIST;



BYTE db_insert_timer    ( pTIMER_DATA    p_timer, 
                          pCHAR          ErrorMsg );
BYTE db_delete_timer    ( pTIMER_DATA    p_timer, 
                          pCHAR          ErrorMsg );
BYTE db_get_timer_table ( pTIMER_DB_LIST p_db_list,
                          pCHAR          ErrorMsg );

#endif