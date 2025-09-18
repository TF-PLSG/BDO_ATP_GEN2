/***************************************************************************
 *
 * MODULE:      PING_CONSTANTS.H
 *
 * TITLE:       ATP Ping Service Utility Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the ATP Ping Service application.
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

#ifndef PING_CONSTANTS_H
#define PING_CONSTANTS_H


/* Ping Statuses */
#define  PING_SUCCESS  0
#define  PING_WAIT     1
#define  PING_LATE     2

/* Ping Values */
#define  PING_DEFAULT_INTERVAL  "0"    /* No Pinging                     */
#define  PING_DEFAULT_WAIT      "10"   /* Wait 10 seconds for a response */
#define  PING_DEFAULT_MAX_TMOUT "5"    /* Max timeouts prior to warning  */

/* TF INI File Section Names */
#define  PINGING_SECTION   "DATASERVER_PINGING"

/* Debug Option */
#define  PING_DEBUG_OFF  0
#define  PING_DEBUG_ON   1

/* Ping Structures */
typedef struct
{
   BYTE appName[80];
   INT  pingStatus;
   INT  pingLateCtr;
   INT  pingMsgLogged;   /* Set to true or false */

} PingStruct, *pPingStruct;

/* Miscellaneous */
#define MAX_SERVICES  150

#endif
