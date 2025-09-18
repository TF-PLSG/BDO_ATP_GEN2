/***************************************************************************
 *
 * MODULE:      PING_PROTOTYPES.H
 *
 * TITLE:       ATP Ping Services Utility Prototypes
 *  
 * DESCRIPTION: This header file contains function prototypes that
 *              are used by the ATP Ping Services application.
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

   $Log$

****************************************************************************/

#ifndef PING_PROTOTYPES_H
#define PING_PROTOTYPES_H

INT  getPingList       ( void );
INT  getPingTimerConfig( void );
void performPinging    ( void );
INT  startup           ( void );
void checkForLateResponse( double current_time );
void issuePings          ( double current_time );
void PingMsgHandler     ( pPTE_MSG p_msg_in );
void processPingResponse( pPTE_MSG p_msg_in );




#endif
