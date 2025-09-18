/***************************************************************************
 *
 * MODULE:      KILL_CONSTANTS.H
 *
 * TITLE:       ATP Kill Service Utility Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the ATP Kill Service application.
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

      Rev 1.2   Jun 26 2007  DIrby
   Added constants to allow multiple kill attempts on a process.
   SCR 25003

      Rev 1.1   Apr 03 2007  DIrby
   Initial revision

****************************************************************************/

#ifndef KILL_CONSTANTS_H
#define KILL_CONSTANTS_H

/* Error Conditons */
#define  KILL_APP_SUCCESS            0
#define  KILL_APP_XIPC_ERROR        -1
#define  KILL_APP_DESTROY_ERROR     -2

/* Confirmation Prompt Results */
#define  KILL_APP_CONFIRM_NO   0
#define  KILL_APP_CONFIRM_YES  1

/* Kill Retries */
#define  MAX_KILL_ATTEMPTS   100 /* Max times to attempt to kill a process. */
#define  DEFAULT_KILL_RETRY_WAIT_TIME  2  /* Wait x secs and retry the kill */


#endif
