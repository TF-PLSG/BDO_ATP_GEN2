/******************************************************************************
  
        Module: pte.h
  
         Title: Common Pinnacle PTE Table Definitions
  
   Description:

   $Log:   S:/ePicPortz/Realpay/archives/Products/Shared/Source/Libraries/Core/Include/pte.h-arc  $
   
      Rev 1.23   Jul 26 2005 09:44:06   dirby
   Increased size of Pindlg lists from 32 to 64.  Equitable has reached
   the limit.
   SCR 14421
   
      Rev 1.22   Jan 28 2002 16:34:10   msaleh
   added definition for TIME_BETWEEN_CHECKS
   
      Rev 1.21   Oct 14 1999 07:24:08   MSALEH
   integrate SSL modifications
   
      Rev 1.20   Aug 30 1999 16:52:28   MSALEH
   increased dialog/router/tpdu lists to 32 entries
   
      Rev 1.19   Nov 17 1998 12:09:22   MSALEH
   Added new message suntypes under system
   to handle Logger/Reply Primary/Secondary
   designation
   
      Rev 1.18   Oct 22 1998 14:14:26   drodden
   Changed the subtype1 db-echo-test to just be db-echo.
   
      Rev 1.17   Jul 23 1998 16:29:00   MSALEH
   added third key into GENERIC_KEYS structure
   
   
      Rev 1.16   Jul 20 1998 11:43:52   MSALEH
   added ST1_DB_SEL_FOR_UPD 7
   message subtype 1
   
      Rev 1.15   May 27 1998 11:58:08   doxtoby
   Att STQ)SYS_DUMP_STATS to MT_SYSTEM_REQUEST msg type
   
      Rev 1.14   27 May 1998 11:19:10   rmalathk
   added msg_type and sub_types for Service
   Manager/Service Monitor
   
      Rev 1.13   May 18 1998 15:58:40   doxtoby
   Add message type (MT_PRIVATE_LOGGER_REPLY) for messages between Logger and Reply.
   Move Become Master from MT_LOGGER_REPLY to MT_PRIVATE_LOGGER_REPLY
   
     Rev 1.12   11 May 1998 14:39:24   MSALEH
   renamed queue PinDlgI to pindlgI
   to avoid NT lower/upper case conversion
   
      Rev 1.11   07 May 1998 10:10:08   MSALEH
   1) rename dialog gui queue to PinDlgI
   2) Remove other hardcoded comm queue names
   since we're going to use only one GUI
   for all comm apps
   
      Rev 1.10   01 May 1998 11:19:28   MSALEH
   reworked the COM message type into
   3 main msg types and several subtypes
   under each
   
      Rev 1.9   Apr 29 1998 16:14:02   drodden
   Added the logout message types.
   
   
      Rev 1.8   27 Apr 1998 12:29:12   MSALEH
   added message type to handle COM
   applications, deleted all message types
   related to DIALOG or HSTCON
   
      Rev 1.7   Apr 27 1998 09:40:50   drodden
   Moved all of the common system wide defines and typedefs out of the application
   headers (app_info) into the core headers (pte, coreinfo).
   This is part of the creation of the Core Dataserver.
   
   
      Rev 1.6   Apr 23 1998 11:11:08   drodden
   Added the login request/reply message types.
   
   
      Rev 1.5   Apr 10 1998 10:09:48   drodden
   Changed the defines for the dialog/hstcon queues to be uppercase
   to follow the Pinnacle standard for defines/type definitions.
   
   
      Rev 1.4   31 Mar 1998 14:20:50   muysal
   Added new message types which are 
   utilized by the Dialog/Hostcon GUI and
   Service apps.
   
      Rev 1.3   Mar 26 1998 10:07:36   drodden
   Moved all of the system wide message type definitions to PTE
   header.  This includes the dialog/hostcon/comms message types.
   
   
      Rev 1.2   Mar 13 1998 09:11:38   drodden
   changed to handle new table structure
   of dm_config.
   
   
      Rev 1.1   Jan 15 1998 10:59:38   drodden
   Added the hostcon fields to the dm_config table.
   
   
      Rev 1.0   Jan 14 1998 11:03:42   drodden
   Added the checkpoint record definition.
   

******************************************************************************/

#ifndef PTE_H
#define PTE_H

#include "basictyp.h"

/*
 * These are the Pinnacle system-wide message types.  All message types must
 * be defined here for system consistency.  Applications 
 * the 'msg_type' field of the PTE_MSG.
 */
#define MT_SYSTEM_REQUEST    1
#define MT_SYSTEM_REPLY      2
#define    ST1_SYS_PING            1
#define    ST1_SYS_TERMINATE       2
#define    ST1_SYS_APP_CHECKIN     3
#define    ST1_SYS_TPDU_UPDATE     4
#define    ST1_SYS_ROUTER_UPDATE   5
#define    ST1_SYS_RESTART1        6
#define    ST1_SYS_RESTART2        7
#define    ST1_SYS_IPC_TRACE_ON    8
#define    ST1_SYS_IPC_TRACE_OFF   9
#define    ST1_SYS_ALERT           10
#define    ST1_SYS_DUMP_STATS      11
#define    ST1_SYS_ARE_YOU_PRIMARY 12
#define    ST1_SYS_I_AM_PRIMARY    13
#define    ST1_SYS_I_AM_SECONDARY  14
#define    ST1_SYS_NOT_IN_DUPLEX_MODE_AND_PRIMARY 15
#define    ST1_SYS_NOT_IN_DUPLEX_MODE_AND_NOT_PRIMARY 16
#define    ST1_SYS_NULLCHECK_ON   19
#define    ST1_SYS_NULLCHECK_OFF  20
#define    ST1_SYS_DBSTATICS_ON   21
#define    ST1_SYS_DBSTATICS_OFF  22




#define MT_INCOMING          3
#define MT_OUTGOING          4

#define MT_LOGGER_REPLY      5
#define    ST1_LOG_RESTART          1
#define    ST1_LOG_REFEED           2
#define    ST1_LOG_REFEED_COMPLETE  3
#define    ST1_LOG_RESTART_COMPLETE 4

#define MT_TIMER_REQUEST     6
#define MT_TIMER_REPLY       7
#define    ST1_TIMER_START         1
#define    ST1_TIMER_READ          2
#define    ST1_TIMER_CLEAR         3
#define    ST1_TIMER_RT_APPROVED	4
#define	   ST1_TIMER_RT_DECLINED	5
#define	   ST1_TIMER_RT_TIMEOUT		6
#define MT_TIMER_TIMEOUT     8

#define MT_DB_REQUEST        9
#define MT_DB_REPLY          10
#define    ST1_DB_INSERT           1
#define    ST1_DB_DELETE           2
#define    ST1_DB_UPDATE           3
#define    ST1_DB_SELECT           4
#define    ST1_DB_GET_ALL          5
#define    ST1_DB_ECHO             6
#define    ST1_DB_SEL_FOR_UPD      7

/* Message types between Dialog Manager and Dialog GUI Interface */
#define MT_COM_REQUEST            11
#define    ST1_EXIT                 1
#define    ST1_STATUS               2
#define    ST1_DISCONNECT           3
#define    ST1_CONNECT              4
#define    ST1_TRACE_ON             5
#define    ST1_TRACE_OFF            6

#define MT_COM_REPLY              12
#define    ST1_DISCONNECT_CONFIRM   1
#define    ST1_CONNECT_CONFIRM      2
#define    ST1_CONNECTED_TRACE      3
#define    ST1_CONNECTED_NOTRACE    4
#define    ST1_TRACE_ON_CONFIRM     5
#define    ST1_TRACE_OFF_CONFIRM    6
#define    ST1_TRACE                7
#define    ST1_STOP                 8


#define MT_UPDATE_TABLE           13
#define    ST1_DMCONFIG             1
#define    ST1_TPDU                 2
#define    ST1_ROUTER               3
#define    ST1_CMCONFIG             4


#define MT_LOGIN_REQUEST     41
#define MT_LOGIN_REPLY       42
#define MT_LOGOUT_REQUEST    43
#define MT_LOGOUT_REPLY      44

/*  Message types between TFLogger/Reply - private message formats */

#define MT_PRIVATE_LOGGER_REPLY 45
#define    ST1_PRV_STARTUP_STATUS   1
#define    ST1_PRV_BE_PRIMARY       2
#define    ST1_PRV_BE_SECONDARY     3
#define    ST1_PRV_CATCHUP_START    4
#define    ST1_PRV_CATCHUP_COMPLETE 5
#define    ST1_PRV_TRID_UPDATE      6
#define    ST1_PRV_TF_BLOCK         7
#define    ST1_PRV_SWITCH_PRIMARY   8
#define    ST1_PRV_REPLY_RESTART    9

/*  Message types between Service Manager/Service Monitor - private message formats */

#define    MT_SRVC_CONTROL		  46
#define    ST1_SRVC_INQ            1
#define    ST1_SRVC_START          2
#define    ST1_SRVC_STOP           3
#define    ST1_SRVC_ENABLE         4
#define    ST1_SRVC_DISABLE        5
#define    ST1_SRVC_TRACEON        6
#define    ST1_SRVC_TRACEOFF       7
#define    ST1_SRVC_GETALL         8
#define    ST1_SRVC_STARTALL       9
#define    ST1_SRVC_STOPALL        10
#define    ST1_SRVC_ENCON          11
#define    ST1_SRVC_ENCOFF         12
#define    ST1_SRVC_MRAON          13
#define    ST1_SRVC_MRAOFF         14
#define    ST1_SRVC_FALCONON       15
#define    ST1_SRVC_FALCONOFF      16
#define    ST1_SRVC_GETNONNC       17
#define    ST1_SRVC_GETNC          18
#define	   ST1_LOAD_RPS_FLAG	   19
#define    ST1_LOAD_OBS_RECORDS	   20
#define    ST1_LOAD_VISA3DS2_RECORDS  21
#define    ST1_LOAD_AMEX3DS2_RECORDS  22
#define    ST1_LOAD_DINERS3DS2_RECORDS  23
#define    ST1_LOAD_JCB3DS2_RECORDS  24
#define    ST1_LOAD_CUP3DS2_RECORDS  25
#define    ST1_LOAD_JCB3DS2_TFINIRECORDS  26
#define	   ST1_LOAD_MID_CPNG_TFINIRECORDS 27
#define    ST1_RELOAD_FALCON_FLAG	28
#define    ST1_LOAD_TO_RC_VALUES  29
#define    ST1_LOAD_VOICE_TO_RC_VALUES  30
#define    ST1_LOAD_RC_FOR_FAI  31
#define	   ST1_LOAD_TO_REVERSAL_ATTEMPTS 32
#define    ST1_LOAD_LATE_RESP_REVERSAL_ATTEMPTS	33
#define    ST1_SYS_FALCONRT_ON    33
#define    ST1_SYS_FALCONRT_OFF   34
#define    ST1_LOAD_SECONDARYEXPIRYDATE_FLAG  35
#define    ST1_VERTEXON_CARD_STATUS_FLAG 36
#define	   ST1_SYS_FALCONRT_EXT10_ON   37
#define	   ST1_SYS_FALCONRT_EXT10_OFF  38
#define	   ST1_SYS_KEYBLOCK_ON		   39
#define	   ST1_SYS_KEYBLOCK_OFF		   40
/*
 * These are the Pinnacle system-wide message subtype "none" values.
 */
#define ST1_NONE  0
#define ST2_NONE  0


/*
 * Definitions to set the value of 'app_data_type' in
 * a PTE_MSG_DATA structure.
 *
 * NOTE: Pinnalce PTE common app data types are 1..99!!!  Stay below!!!
 *
 */
#define NO_DATA_INCLUDED   0
#define APPL_INFO_DATA     1
#define  SELECT_DATA       2
#define DM_CONFIG_DATA     3
#define ROUTER_DATA        4
#define TPDU_DATA          5
#define CM_CONFIG_DATA     6


#define DIALOG_GUI_IQUEUE             "pindlgI"

/*
 * Size definitions
 */
#define DIALOG_LIST_SIZE   64
#define ROUTER_LIST_SIZE   64
#define TPDU_LIST_SIZE     64
#define MAX_KEY_SIZE       41
/*
#define ROUTER_ID_SIZE     17 
#define TPDU_ID_SIZE       16
*/
#define COMMGR_LIST_SIZE   32

#define TIME_BETWEEN_CHECKS   5L    /* 5 seconds */

/*
 * 
 */
typedef struct
{
   CHAR executable_name   [MAX_APP_NAME_SIZE];
   CHAR application_name  [MAX_APP_NAME_SIZE];
   CHAR shared_queue      [MAX_QUE_NAME_SIZE];  /* shared queue */
   CHAR control_queue     [MAX_QUE_NAME_SIZE];  /* formerly the synchronous queue */
   CHAR interactive_queue [MAX_QUE_NAME_SIZE];  /* formerly the asynchronous queue */
} APPL_INFO, *pAPPL_INFO;


typedef struct
{
   CHAR  key1 [MAX_KEY_SIZE + 1];
   CHAR  key2 [MAX_KEY_SIZE + 1];
   CHAR  key3 [MAX_KEY_SIZE + 1];
   CHAR  key4 [MAX_KEY_SIZE + 1];
} GENERIC_KEYS, *pGENERIC_KEYS;


#endif

