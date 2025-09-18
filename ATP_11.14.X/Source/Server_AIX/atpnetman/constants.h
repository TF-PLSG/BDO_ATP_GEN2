/******************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      constants.h
 *
 * TITLE:       ATPNETMAN CONSTANT DEFINITIONS
 *
 *	DESCRIPTION: This file contains constants/macros used in atpnetman.
 *
 * APPLICATION: ATPNETMAN
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\atpnetman\constants.h  $
   
      Rev 1.0   Nov 04 2004 15:22:02   dirby
   Initial revision.
 *
 *****************************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* This constant defines the number of records allowed in the linked list. */
#define  MAX_NUM_NCF01_RECS  60


/* XIPC Usage */
#define  XIPC_CHECK_IN   0
#define  XIPC_CHECK_OUT  1

#define  XIPC_ACTIVE    0
#define  XIPC_INACTIVE  1

/* Defines number of seconds to wait for host state to change
 * after a request is sent.
 */
#define  WAIT_SECONDS  10
#define LOCAL_GUI_MAX_LIST_SIZE  60

/* Defines size of a sign on Bin. */
#define  BIN_LENGTH  7

CHAR  Dest_Queue_atpnetman[MAX_QUE_NAME_SIZE];

/* This structure defines the look of the linked list. */
typedef struct
{
   BYTE   network_id[MAX_QUE_NAME_SIZE];
   BYTE   network_type[2];
   BYTE   queue[MAX_QUE_NAME_SIZE];
   BYTE   bin[10][BIN_LENGTH];
}NCF01_REC, *pNCF01_REC;




typedef struct
{
   BYTE  num_returned             [5];
   NCF01 ncf01_record             [LOCAL_GUI_MAX_LIST_SIZE];

} LOCAL_NCF01_GUI_LIST, *LOCAL_pNCF01_GUI_LIST;


/* Network Commands */
#define  LOGON     "LOGON"
#define  LOGOFF    "LOGOFF"
#define  ECHO      "ECHO"
#define  SIGNON_ADVICE_RETRIEVAL "SIGNON_ADVICE"
#define  SIGNOFF_ADVICE_RETRIEVAL "SIGNOFF_ADVICE"
static int Local_count=0;

/* USER INPUT STATUS */
#define  INVALID  0
#define  VALID    1


#define  EVEN  0
#define  ODD   1

#endif
