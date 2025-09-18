/******************************************************************************

       Module: common.h

        Title: ASCENDENT Dialog Manager

  Description: This is a modified file of original common.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\COMMON.H  $
   
      Rev 1.20   Jan 14 2000 14:34:48   MSALEH
   add new general use field to IPC_INFO, 
   to be used for passing the IP address
   
      Rev 1.19   Nov 05 1999 16:40:54   MSALEH
   add send_keepalive variable
   
      Rev 1.18   Oct 15 1999 10:17:44   MSALEH
   Modify Project settings for SSL
   
      Rev 1.17   Oct 14 1999 08:12:46   MSALEH
   integrate SSL modifications
   
      Rev 1.16   Sep 30 1999 15:33:38   MSALEH
   Use #defines instead of literal values
   
      Rev 1.15   Sep 29 1999 14:49:58   MSALEH
   AIX Modifications
   
      Rev 1.13   Sep 28 1999 14:48:46   MSALEH
   AIX Modifications
   
      Rev 1.11   Aug 30 1999 12:25:34   MSALEH
   Remove router_extended_comms from
   router_table
   
      Rev 1.10   Jul 15 1999 16:35:20   MSALEH
   added router_extend_comms to local_router_table
   
      Rev 1.9   May 20 1999 17:02:24   MSALEH
   increased buffer size to 64K
   
      Rev 1.8   Apr 14 1999 15:23:08   MSALEH
   Mods to handle new IGNORE option
   
      Rev 1.7   Apr 01 1999 12:22:00   MSALEH
   reworked tcp.c to handle multiple socket connections
   in server mode, also streamlined the code
   to be more efficient
   
      Rev 1.6   Nov 03 1998 15:55:00   MSALEH
   Size of Host_name is 41 not 21
   
      Rev 1.5   Oct 08 1998 16:42:48   MSALEH
   1) Change "PINNACLE" to "ASCENDENT"
   2) Add new fields to dm_config
       db_tcp_socket_type
       db_tcp_headet_length
       db_tpdu_processing
   3) delete the field db_host_tcp_port from dm_config
   4) Restructure ptecom.lib/coreds/pindlg/dialog
   to be more configurable
   5) Update GUI to reflect new fields
   6) Rework the functionality of the connection
   spy to by included within the main loop
   7) Client connect tries only once then return
   and tries again after a timeout value, this will
   prevent the Client Connection from hanging during connect
   8) Streamline functions/variables
   9) All character comparisons are case insensitive 
   
   
      Rev 1.4   Aug 26 1998 09:41:58   MSALEH
   added another state to the COMM setup tab
   the new value is TCPNOTPDU, allows processing
   transactions without a TPDU
   
      Rev 1.3   19 Aug 1998 14:58:42   rmalathk
   1. modified send_trace_to_gui function to
       send trace messages from a service to
       GUI via applink.
   2. The tpdu field is no longer sent in the 
       orig_comms_info field to GUI. It is sent
       as a second piece of data chained to the
       trace.
   
   
      Rev 1.2   27 Mar 1998 16:07:40   MSALEH
   1) extract notify routines from convert.c
   into notify.c and notify.h
   2) Applications can either use the default notification
   routines or override them with their own
   local routines
   3) Add routines to handle "RAW" tcp read and write
   that DO NOT append or parse two byte length
   from an outgoing or incoming message
   4) Cannot handle concatenation and segmentation
   in that case, the calling application will have
   to handle that
   
      Rev 1.1   Mar 26 1998 08:41:12   drodden
   Changed the tcp ports in the local-dm-config structure to be 
   WORDs rather than INTs for consistency and compatability.
   
   
      Rev 1.0   Mar 25 1998 15:31:08   drodden
   Reworked the comm library a little to:
     - have an easier set ot headers to use.
     - prototyped all functions in a header for each module.
     - moved the shared type definitions and global variables
       to common.c  (and .h).
     - use PTE basic data types wherever possible.
   

******************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32

#include <winsock.h>

#ifdef SSL_SUPPORT
#include "e_os.h"
#include "sslc.h"
#endif

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#endif

#include "basictyp.h"
#include <string.h>

#ifndef WIN32
#define SOCKET INT
#endif

#define DM_PORTTYPE_SERIAL_WITH_TPDU         1
#define DM_PORTTYPE_SERIAL_WITHOUT_TPDU      2
#define DM_PORTTYPE_TCP_CLIENT_WITH_TPDU     3
#define DM_PORTTYPE_TCP_SERVER_WITH_TPDU     4
#define DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU  5
#define DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU  6
#define DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU   7
#define DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU   8
#define DM_PORTTYPE_SERIAL_IGNORE_TPDU       9

#ifdef SSL_SUPPORT
#define CM_PORTTYPE_SSL_SERVER_WITH_TPDU     10
#endif

#define WITHOUT_TPDU 1
#define WITH_TPDU    2
#define IGNORE_TPDU  3

#define ORIGINAL_REQUEST     '^'
#define MSG_WITHOUT_TPDU     '*'
#define MSG_WITHOUT_TPDU_IND "*"
#define FORWARD_MESSAGE      '~'
#define ORIG_INFO_SEPARATOR  ','

#define TPDU_DECODE_ERROR 0
#define REROUTE_MESSAGE   -1
#define SEND_MSG          -2

#define MAX_PENDING_CONNECTS      5     /* The backlog allowed for listen() */ 
#define NO_FLAGS_SET              0     /* Used with recv()/send() */ 

#define DM_ERRBUF_SIZE            256

#define MAX_INCOMING_BUFFER_SIZE       65536 
#define MAX_OUTGOING_BUFFER_SIZE       65536
#define MAX_CHARS_TO_DISPLAY           50
#define MAX_DC_RETRIES                 1000
#define MAX_CONFIG_ENTRY               100 
#define MAX_INCOMING_SSL_BUFFER_SIZE   512

#define SEVENBITS  ((WORD)0x0007)
#define EIGHTBITS  ((WORD)0x0008)

typedef struct
{
   INT       connectiontype;
   CHAR      instancename[51];
   CHAR      loginname[51];
   INT       messageheaderlength;
   BYTE      tpduoption;
   SOCKET    socket_id;

#ifdef SSL_SUPPORT
   CHAR		ssl_port[51];
   BIO		*bio_out; 
   BIO      *bio_err;
   BIO	   *abio;
   SSL_CTX  *ssl_ctx;
   SSL      *ssl;
   CHAR     ssl_id[MAX_TRAN_ID_SIZE];

   /* used by SSL Client to set export/import cipher strength */
   /* 0 = default  - attempts strongest to weakest            */
   /* 1 = EXPORT   - supports export ciphers                  */
   /* 2 = DOMESTIC - supports domestic cipher                 */
   INT		cipher_strength;
   /* use this until receive reply from SSL */
   INT		reconnect;
#endif

   CHAR     buffer[32];       /* General purpose buffer */

} IPC_INFO, *pIPC_INFO;


typedef struct
{
   WORD       usPort;
   pBYTE      HostName;
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_TCP_STRUCT;

#ifdef SSL_SUPPORT
typedef struct
{
   WORD       usPort;
   pBYTE      HostName;
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_SSL_STRUCT;
#endif

typedef struct
{
   DWORD      dwBaudRate;
   BYTE       bByteSize;
   BYTE       bParity;
   BYTE       bStopBits;
   CHAR       cPort[128];
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_SERIAL_STRUCT;


typedef struct
{
#ifdef WIN32
   SOCKADDR_IN   sin;
#else
   struct sockaddr_in sin;
#endif

   SOCKET        listener;
} DM_TCP_STRUCT;


typedef struct
{
#ifdef WIN32
   HANDLE        idComDev;
   DCB           dcb;
   OVERLAPPED    osWrite, osRead;
#else
   INT           idComDev;
#endif

   INT volatile  fConnected;
}  DM_SERIAL_STRUCT;


typedef struct
{
  CHAR  db_subsystem_name [21];
  CHAR  db_connection_type[11];
  CHAR  db_com_port       [11];
  CHAR  db_baud_rate      [11];
  CHAR  db_byte_size      [11];
  CHAR  db_parity         [11];
  CHAR  db_stop_bits      [11];
  WORD  db_tcp_port;
  CHAR  db_host_name      [41];
  WORD  db_tcp_header_length;
  CHAR  db_tcp_socket_type[11];
  CHAR  db_tpdu_processing[11];
} LOCAL_DM_CONFIG;

#ifdef SSL_SUPPORT
/* added for SSL 06/23/99 - jjj */
typedef struct
{
   CHAR db_subsystem_name     [21];
   CHAR db_connection_type    [11];
   CHAR db_com_port           [11];
   CHAR db_baud_rate          [11];
   CHAR db_byte_size          [11];
   CHAR db_parity             [11];
   CHAR db_stop_bits          [11];
   WORD db_tcp_port;    
   CHAR db_host_name          [41];
   WORD db_tcp_header_length;
   CHAR db_tcp_socket_type    [11];
   CHAR db_tpdu_processing    [11];
   CHAR db_ssl_processing     [11];
   CHAR db_client_certificate [11];
   WORD db_server_method;
   WORD db_cipher_suite;
}LOCAL_CM_CONFIG;
#endif

typedef struct
{
  CHAR tpdu_id     [16];
  INT  tran_len;
  INT  dst_len;
  INT  src_len;
  CHAR tpdu_string [51];
} LOCAL_TPDU;


typedef struct
{
  CHAR router_identifier [17];
  CHAR router_queue      [11];
  CHAR router_app_desc   [21];
  INT  router_offset;
  INT  router_length;
  CHAR router_datatype   [5];
  CHAR router_lo_end     [11];
  CHAR router_hi_end     [11];
} LOCAL_ROUTER_TABLE;


#define SIN( x )          (x.sin)
#define LISTNER( x )      (x.listener)
#define TCPCONNECTED( x ) (x.sConnected)

#define COMDEV( x )     (x.idComDev)
#define CONNECTED( x )  (x.fConnected)
#define COMDCB( x )     (x.dcb)
#define PARITY( x )     (x.dcb.Parity)
#define STOPBITS( x )   (x.dcb.StopBits)
#define BAUDRATE( x )   (x.dcb.BaudRate)
#define BYTESIZE( x )   (x.dcb.ByteSize)
#define WRITE_OS( x )   (x.osWrite)
#define READ_OS( x )    (x.osRead)


/* Global Pointers to Functions/Variables */
extern INT     volatile connection_up;
extern BYTE    volatile send_keepalive;

extern INT     volatile trace_on;
extern BYTE    volatile trace_number;
extern INT     volatile trace_lock_on;
extern CHAR    trace_file  [64];
extern CHAR    trace_queue [MAX_QUE_NAME_SIZE];

extern CHAR    gui_orig_info[20];

extern void    (*pfNotify)   (pBYTE, WORD, pIPC_INFO);
extern BOOLEAN (*port_write) (const WORD, const void *, pIPC_INFO);
extern void    (*port_close) (INT);

/* Common.c Function Prototypes */
WORD    bcd2bin(const pBYTE lpBcd);

void    bin2bcd(pBYTE lpBcd, const WORD wInt);

BOOLEAN port_init(BYTE type, const void * userdata, pIPC_INFO ipc_info);

INT     thread_logon(pCHAR instancename, pCHAR loginname);

void    thread_logoff();

void    bin_to_ascii (pCHAR src, pCHAR dst, INT len);

void    ascii_to_bin (pBYTE src, pBYTE dst, INT len);

BOOLEAN tpsr_port_init(BYTE type, const void * userdata, pIPC_INFO ipc_info, int tlevel);

#endif

