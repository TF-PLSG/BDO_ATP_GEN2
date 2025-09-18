/******************************************************************************
  
        Module: dbcommon.h
  
         Title: Common Dataserver DB Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptedb_lib\dbcommon.h  $
   
      Rev 1.2   Aug 07 1998 10:23:18   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   
   
      Rev 1.1   May 19 1998 09:28:46   drodden
   Changed the dataserver timer routines to NOT send a timeout if the 
   timer request did originate on this system.  This is for high availability
   systems. 
   
   
      Rev 1.0   Apr 23 1998 10:57:58   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#ifndef DBCOMMON_H
#define DBCOMMON_H


#define C_TO_VARCHAR(v,c) v.len=strlen(c);strcpy(v.arr,c)
#define VARCHAR_TO_C(c,v) memcpy(&c,&v.arr,v.len);

typedef struct { WORD len; BYTE arr [256];  } RAW256;
typedef struct { WORD len; BYTE arr [512];  } RAW512;
typedef struct { WORD len; BYTE arr [768];  } RAW768;
typedef struct { WORD len; BYTE arr [1024]; } RAW1024;
typedef struct { WORD len; BYTE arr [1280]; } RAW1280;
typedef struct { WORD len; BYTE arr [1536]; } RAW1536;


#define TIMER_DB_LIST_SIZE  500


typedef struct 
{
   CHAR  timer_id    [MAX_TIMER_KEY_SIZE];
   CHAR  app_data1   [MAX_APP_DATA_SIZE];
   CHAR  app_data2   [MAX_APP_DATA_SIZE];
   CHAR  reply_que   [MAX_QUE_NAME_SIZE];
   CHAR  orig_system [MAX_SYSTEM_NAME_SIZE];
   LONG  timeout;
   LONG  interval;
   LONG  timer_size;
} TIMER_DATA, *pTIMER_DATA;


typedef struct 
{
   INT         num_returned;
   TIMER_DATA  db_rec [TIMER_DB_LIST_SIZE];
} TIMER_DB_LIST, *pTIMER_DB_LIST;


typedef struct
{
   CHAR  checkpoint_id        [MAX_TRAN_ID_SIZE];
   CHAR  checkpoint_trid      [MAX_TRAN_ID_SIZE];
} CHECKPOINT, *pCHECKPOINT;



BYTE dbcommon_insert_timer    ( pTIMER_DATA    p_timer, 
                                pCHAR          ErrorMsg );
BYTE dbcommon_delete_timer    ( pTIMER_DATA    p_timer, 
                                pCHAR          ErrorMsg );
BYTE dbcommon_get_timer_table ( pTIMER_DB_LIST p_db_list,
                                pCHAR          ErrorMsg );

BYTE dbcommon_select_checkpoint ( pCHAR       checkpoint_id, 
                                  pCHECKPOINT p_checkpoint, 
                                  pCHAR       ErrorMsg );
BYTE dbcommon_update_checkpoint ( pCHECKPOINT p_checkpoint, 
                                  pCHAR       ErrorMsg );
BYTE dbcommon_delete_checkpoint ( pCHAR       checkpoint_id, 
                                  pCHAR       ErrorMsg );


BYTE dbcommon_connect    ( pCHAR username, 
                           pCHAR password,
                           pCHAR alias,    
                           pCHAR database,
                           pCHAR ErrorMsg );
BYTE dbcommon_disconnect ( pCHAR ErrorMsg );


void dbcommon_commit   ();
void dbcommon_rollback ();


BYTE dbcommon_oracle_to_pte_rcode ( LONG oracle_rcode, pCHAR ErrorMsg );



#endif

