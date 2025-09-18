/******************************************************************************

        Module: ptemsg.h
  
         Title: APIs to construct/manipulate IPC messages
  
   Description: This file contains all definitions and structures used to
                build messages to be sent between processes for the Pinnacle
                Transaction Environment.

  $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTEMSG.H  $
   
      Rev 1.28   Dec 03 1998 10:56:56   drodden
   Data caching changes.
   
   
      Rev 1.27   Nov 03 1998 15:26:46   MSALEH
   Added PTEMSG_NOT_CONNECTED_TO_DB
   return type to handle dataserver reconnection
   to database
   
      Rev 1.26   27 May 1998 11:20:58   rmalathk
   added error codes for Service Manager
   
      Rev 1.25   May 01 1998 07:55:22   drodden
   Added the message version, originator system name, and log flag
   to the message header.
   
   
      Rev 1.24   Apr 23 1998 11:11:24   drodden
   Added a login failed result code.
   
   
      Rev 1.23   Mar 26 1998 10:07:36   drodden
   Moved all of the system wide message type definitions to PTE
   header.  This includes the dialog/hostcon/comms message types.
   
   
      Rev 1.22   19 Jan 1998 16:08:18   MSALEH
    
   
      Rev 1.21   Jan 19 1998 11:25:18   drodden
   Added error number for the dataserver in refeed mode.
   
   
      Rev 1.20   18 Jan 1998 12:05:30   MSALEH
   added ST1_LOG_RESTART_COMPLETE
   
      Rev 1.19   18 Jan 1998 11:52:02   MSALEH
   added ST1_LOG_REFEED_COMPLETE
   
      Rev 1.18   Jan 14 1998 11:07:02   drodden
   Added two new error numbers for ptemsg transID validation
   and checkpointing (to the database).
   
   
      Rev 1.17   07 Jan 1998 16:13:46   MSALEH
   added ST1_SYS_ALERT
   
      Rev 1.16   10 Jan 2000 11:39:42   MSALEH
   added:
   ST1_LOG_RESTART
   ST1_LOG_REFEED
   
      Rev 1.15   Jan 07 1998 10:28:04   drodden
   Added support for the current/last tran IDs.
   Added the type-of-reply-needed field and APIs.
   
   
      Rev 1.14   Dec 23 1997 08:14:00   drodden
   Consolidated all of the error codes to this header.
   
   
      Rev 1.13   Dec 17 1997 09:36:20   drodden
   Moved some of the common size definitions to BASICTYP
   from PTEMSG and APP_INFO.
   
   
      Rev 1.12   Dec 16 1997 14:29:24   drodden
   Incorporated changes that were made to an unlocked copy of this
   header that had been placed out on the network shared directory.
   
   
      Rev 1.11   Dec 16 1997 12:31:14   drodden
   Added the timer message types/subtypes.
   Put the subtype "none" definitions back in.
   
   
      Rev 1.10   Dec 01 1997 08:54:20   drodden
   Added APIs to get/set the originator comms info.
   
   
      Rev 1.9   19 Nov 1997 13:57:32   MSALEH
   added MT_LOGGER_REPLY
   
      Rev 1.8   Oct 23 1997 11:42:28   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.7   Oct 03 1997 10:48:22   drodden
   Added functions to get/set the msg transaction id.
   
   
      Rev 1.6   26 Sep 1997 14:16:18   mvandenb
   Correct mis-spelled routine name in prototype definition.
   
      Rev 1.5   26 Sep 1997 09:49:16   mvandenb
   Added routine prototypes for routines in ptemsg.c and
   a field to PTE_MSG structure for return values ('result_code').
   
      Rev 1.4   25 Sep 1997 15:32:08   mvandenb
   Added field to PTE_MSG structure for holding the
   transaction ID (msg_tran_id).
   
      Rev 1.3   25 Sep 1997 11:19:02   mvandenb
   Changes to make data/message chaining more straightforward.
   
      Rev 1.2   19 Sep 1997 14:32:56   mvandenb
   Moved some definitions out of this file and into
   the application specific file.
   
      Rev 1.1   17 Sep 1997 09:03:04   mvandenb
   Removed application specific stuff and added a couple of
   new definitions.
   
      Rev 1.0   15 Sep 1997 16:07:18   mvandenb
   Initial version of inter-process messaging structures.

*****************************************************************************/

#ifndef PTEMSG_H
#define PTEMSG_H

#include "basictyp.h"
#include "pte.h"


/*
 * Definitions for maximum array sizes.
 */
#define MAX_ORIG_INFO_SIZE   32


/*
 * These are the Pinnacle system-wide "type_of_reply_needed" codes.
 */
#define PTEMSG_REPLY_NORMAL          0
#define PTEMSG_REPLY_ONLY_IF_FAILS   1


/*
 * These are the Pinnacle system-wide "type_of_reply_needed" codes.
 */
#define PTEMSG_LOG_NORMAL            0
#define PTEMSG_LOG_NOFORWARD         1


 /*
 * These are the Pinnacle system-wide error codes.
 */
#define PTEMSG_OK                    0
#define PTEMSG_NOT_FOUND             1 
#define PTEMSG_INSERT_FAILED         2
#define PTEMSG_UPDATE_FAILED         3
#define PTEMSG_DELETE_FAILED         4
#define PTEMSG_DATABASE_ERROR        5
#define PTEMSG_INSUFFICIENT_MEMORY   6
#define PTEMSG_INVALID_MSGTYPE       7
#define PTEMSG_INVALID_SUBTYPE1      8
#define PTEMSG_INVALID_SUBTYPE2      9
#define PTEMSG_INVALID_DATATYPE      10
#define PTEMSG_END_OF_TABLE          11
#define PTEMSG_COULD_NOT_FORWARD     12
#define PTEMSG_DUPLICATE_ENTRY       13
#define PTEMSG_IPC_ERROR             14
#define PTEMSG_INVALID_TRID          15
#define PTEMSG_CHECKPOINT_FAILED     16
#define PTEMSG_IN_REFEED_MODE        17
#define PTEMSG_LOGIN_FAILED          18
#define PTEMSG_SRVC_NOTFOUND         19
#define PTEMSG_SRVC_DISABLED         20
#define PTEMSG_SRVC_STARTFAILED      21
#define PTEMSG_SRVC_STOPFAILED       22
#define PTEMSG_SRVC_RUNNING          23
#define PTEMSG_SRVC_STOPPED          24
#define PTEMSG_NOT_CONNECTED_TO_DB   25
#define PTEMSG_NOT_CACHED            26
#define PTEMSG_CACHE_CREATE_ERROR    27
#define PTEMSG_CACHE_READ_ERROR      28
#define PTEMSG_CACHE_WRITE_ERROR     29
#define PTEMSG_CACHE_IS_FULL         30
#define PTEMSG_INDEX_IS_FULL         31
#define PTEMSG_GUI_TIMEDOUT         32
#define PARTIAL_AMOUNT_EXCEEDED_TOTAL_AMOUNT 33
#define TOTAL_AMOUNT_EMPTY  34


/*
 * Definitions for setting the 'data_type' field
 * in a PTE_MSG_DATA structure.
 */
#define  NO_PTE_MSG_DATA   0  /* Use when no data is being attached to a PTE message */
#define  APPLICATION_DATA  1  /* Use if the data is for application use. */
#define  PTE_MESSAGE_DATA  2  /* Use if the data is another PTE message. */


typedef struct
{
   BYTE  data_type;                    /* PTE_MESSAGE_DATA or APPLICATION_DATA */
   BYTE  app_data_type;                /* Application data type designator */
   CHAR  data_length [MAX_ITOA_SIZE];  /* Length of the data. */
   BYTE  data;                         /* First byte of the data. */
} PTE_MSG_DATA, *pPTE_MSG_DATA;


/*
 * The following header MUST be the first portion of any and all
 * messages passed from one Pinnacle process to another.
 *
 * NOTE: Do not move the position of the message version field!
 *       It must remain at offset 3 of the message.
 */
#define PTEMSG_MSG_VERSION    20

typedef struct
{
   BYTE          msg_type;                               /* Message type. */
   BYTE          msg_subtype1;                           /* message subtype 1 */
   BYTE          msg_subtype2;                           /* message subtype 2 */
   BYTE          msg_version;                            /* ptemsg internal version # */
   BYTE          result_code;                            /* Field to hold return values of requests. */
   BYTE          type_of_reply_needed;                   /* reply desired: only on error, etc */
   BYTE          log_flag;                               /* logger/reply flag */
   BYTE          pad1;                                   /* pad to 32bit boundary */
   CHAR          current_tran_id [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          last_tran_id    [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          dest_queue      [MAX_QUE_NAME_SIZE];    /* */
   CHAR          orig_queue      [MAX_QUE_NAME_SIZE];    /* */
   CHAR          orig_comms_info [MAX_ORIG_INFO_SIZE];   /* communications info about the originator */
   CHAR          orig_system     [MAX_SYSTEM_NAME_SIZE]; /* */
   CHAR          msg_length      [MAX_ITOA_SIZE];        /* Length of entire message. */
   BYTE          num_chained_data;                       /* Number of data elements in chain. */
   BYTE          num_chained_messages;                   /* Number of messages in chain. */
   BYTE          pad2 [2];                               /* pad to 32bit boundary */
   PTE_MSG_DATA  msg_data;
} PTE_MSG, *pPTE_MSG;


#define   PTE_MSG_HDR_SIZE       sizeof ( PTE_MSG ) - 1
#define   PTE_MSG_DATA_HDR_SIZE  sizeof ( PTE_MSG_DATA ) - 1


/*
 * Prototypes for routines in ptemsg.c.
 */
pPTE_MSG ptemsg_build_msg ( BYTE  msg_type,
                            BYTE  msg_subtype1,
                            BYTE  msg_subtype2,
                            pCHAR dest_queue,
                            pCHAR orig_queue,
                            pBYTE p_data,
                            LONG  data_len,
                            BYTE  app_data_type );


pPTE_MSG ptemsg_chain_data_or_message ( pPTE_MSG p_pte_msg,
                                        pBYTE    p_data,
                                        LONG     data_len,
                                        BYTE     data_type,
                                        BYTE     app_data_type );


pPTE_MSG ptemsg_chain_new_message ( pPTE_MSG p_pte_msg,
                                    BYTE     msg_type,
                                    BYTE     msg_subtype1,
                                    BYTE     msg_subtype2,
                                    pCHAR    dest_queue,
                                    pCHAR    orig_queue,
                                    pBYTE    p_data,
                                    LONG     data_len,
                                    BYTE     app_data_type );


pPTE_MSG ptemsg_chain_message ( pPTE_MSG p_pte_msg1,
                                pPTE_MSG p_pte_msg2 );


pPTE_MSG ptemsg_chain_data ( pPTE_MSG p_pte_msg,
                             pBYTE    p_data,
                             LONG     data_len,
                             BYTE     app_data_type );


BYTE ptemsg_get_msg_type        ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_msg_type     ( pPTE_MSG p_pte_msg,
                                  BYTE     msg_type );


BYTE ptemsg_get_msg_subtype1    ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_msg_subtype1 ( pPTE_MSG p_pte_msg,
                                  BYTE     msg_subtype1 );


BYTE ptemsg_get_msg_subtype2    ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_msg_subtype2 ( pPTE_MSG p_pte_msg,
                                  BYTE     msg_subtype2 );


BYTE ptemsg_get_result_code     ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_result_code  ( pPTE_MSG p_pte_msg,
                                  BYTE     result_code );


pCHAR ptemsg_get_msg_dest_queue ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_dest_queue   ( pPTE_MSG p_pte_msg,
                                  pCHAR    queue_name );


pCHAR ptemsg_get_msg_orig_queue ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_orig_queue   ( pPTE_MSG p_pte_msg,
                                  pCHAR    queue_name );


pCHAR ptemsg_get_msg_orig_comms_info ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_orig_comms_info   ( pPTE_MSG p_pte_msg,
                                       pCHAR    comms_info );


LONG ptemsg_get_msg_length ( pPTE_MSG p_pte_msg );


LONG ptemsg_get_num_chained_messages ( pPTE_MSG p_pte_msg );


LONG ptemsg_get_num_chained_data ( pPTE_MSG p_pte_msg );


pPTE_MSG_DATA ptemsg_get_pte_msg_data ( pPTE_MSG p_pte_msg );


pPTE_MSG_DATA ptemsg_get_chained_pte_msg_data ( pPTE_MSG p_pte_msg,
                                                LONG     desired_link );


pPTE_MSG ptemsg_get_chained_pte_msg ( pPTE_MSG p_pte_msg,
                                      LONG     desired_message_num );


LONG ptemsg_get_pte_msg_data_data_length ( pPTE_MSG_DATA p_msg_data );


pBYTE ptemsg_get_pte_msg_data_data ( pPTE_MSG_DATA p_msg_data );


BYTE ptemsg_get_pte_msg_data_data_type ( pPTE_MSG_DATA p_msg_data );


BYTE ptemsg_get_pte_msg_data_app_data_type ( pPTE_MSG_DATA p_msg_data );


pCHAR   ptemsg_get_current_msg_tran_id ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_current_msg_tran_id ( pPTE_MSG p_pte_msg,
                                         pCHAR    tran_id    );

pCHAR   ptemsg_get_last_msg_tran_id ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_last_msg_tran_id ( pPTE_MSG p_pte_msg,
                                      pCHAR    tran_id    );

BYTE ptemsg_get_type_of_reply_needed    ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_type_of_reply_needed ( pPTE_MSG p_pte_msg,
                                          BYTE     type_of_reply_needed );


pCHAR ptemsg_get_msg_orig_system ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_orig_system   ( pPTE_MSG p_pte_msg,
                                   pCHAR    system_name );


BYTE ptemsg_get_log_flag         ( pPTE_MSG p_pte_msg );


BOOLEAN ptemsg_set_log_flag      ( pPTE_MSG p_pte_msg,
                                   BYTE     log_flag );


#endif
