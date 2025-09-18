/***************************************************************************
 *
 * MODULE:      KILL_PROTOTYPES.H
 *
 * TITLE:       ATP Kill Service Utility Prototypes
 *  
 * DESCRIPTION: This header file contains function prototypes that
 *              are used by the ATP Kill Service application.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * Copyright (c) 2007, Hypercom, Inc. All Rights Reserved.
 *
 ***************************************************************************/

/***************************************************************************

   REVISION HISTORY
   ----------------

      Rev 1.1   Apr 03 2007  DIrby
   Initial revision

****************************************************************************/

#ifndef KILL_PROTOTYPES_H
#define KILL_PROTOTYPES_H

void Usage( void );
INT  XipcCheckin( void );
INT  ConfirmInputs( void );
INT  ShutdownService( pCHAR service );
INT  GetQueueId( pCHAR queName, pINT qid );
INT  DestroyQueue( XINT qid, pCHAR queName );
INT  RemoveUser(   XINT uid, pCHAR queName );
INT  GetUserId(    XINT qid, pCHAR queName, pINT uid );
INT  GetProcessId( XINT uid, pCHAR queName, pINT pid );
INT  GetLocalUid( void );
void Cleanup( void );

#endif
