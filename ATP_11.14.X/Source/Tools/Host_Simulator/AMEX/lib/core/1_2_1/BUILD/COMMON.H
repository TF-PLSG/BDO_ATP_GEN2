/******************************************************************************

       Module: common.h

        Title: Pinnacle Dialog Manager

  Description: This is a modified file of original common.c.
               Error logging to DB is replaced by logging to NT's
               event log

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\common.h  $
   
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

#include <winsock.h>

#include "basictyp.h"


#define DM_PORTTYPE_TCP           1
#define DM_PORTTYPE_SERIAL        2
#define DM_PORTTYPE_DIRECT        3
#define DM_PORTTYPE_RAW_DIRECT    4
#define DM_PORTTYPE_RAW_TCP       5
#define DM_PORTTYPE_TCP_NO_TPDU   6

#define MAX_PENDING_CONNECTS      5     // The backlog allowed for listen() 
#define NO_FLAGS_SET              0     // Used with recv()/send() 

#define DM_ERRBUF_SIZE            256

#define MAX_INCOMING_BUFFER_SIZE  8193  // 2^13 + 1 
#define MAX_OUTGOING_BUFFER_SIZE  8193
#define MAX_CHARS_TO_DISPLAY      50
#define MAX_DC_RETRIES            1000
#define MAX_CONFIG_ENTRY          100 

#define SERIAL_TO_NAC             1
#define SERIAL_TO_HOST            2

#define DIRECT_INDICATOR         "DIRECT"
#define HSTCON_QUEUE_NAME        "hstconA"

typedef struct
{
   INT       connectiontype;
   CHAR      instancename[51];
   CHAR      loginname[51];
} IPC_INFO, *pIPC_INFO;


typedef struct
{
   WORD       usPort;
   pBYTE      lpBuf;
   WORD       wBufferLen;
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_TCP_STRUCT;


typedef struct
{
   DWORD      dwBaudRate;
   BYTE       bByteSize;
   BYTE       bParity;
   BYTE       bStopBits;
   char       cPort[5]; 
   pBYTE      lpBuf;
   WORD       wBufferLen;
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_SERIAL_STRUCT;


typedef struct
{
   WORD       DirectPort;
   pBYTE      HostName;
   pBYTE      lpBuf;
   WORD       wBufferLen;
   void       (*pfNotify)(pBYTE, WORD, pIPC_INFO);
} USER_DC_STRUCT;


typedef struct
{
   SOCKADDR_IN   sin;
   SOCKET        listener;
   INT volatile  sConnected;
   INT volatile  thread_logged_in;
   INT volatile  socket_closed;
} DM_TCP_STRUCT;


typedef struct
{
   HANDLE        idComDev;
   DCB           dcb;
   OVERLAPPED    osWrite, osRead;
   INT volatile  fConnected;
   INT volatile  thread_logged_in;
}  DM_SERIAL_STRUCT;


typedef struct
{
	CHAR pbsn_com_port  [21];
	CHAR pbsn_baud_rate [21];
	CHAR pbsn_byte_size [21];
	CHAR pbsn_parity    [21];
	CHAR pbsn_stop_bits [21];
} SERIAL_BLOCK;


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
  CHAR  db_host_name      [21];
  WORD  db_host_tcp_port;
} LOCAL_DM_CONFIG;


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
#define LOGGED_IN( x )    (x.thread_logged_in)
#define SOCK_CLOSED( x )  (x.socket_closed)

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
extern INT      volatile connection_up;

extern INT      volatile trace_on;
extern BYTE     volatile trace_number;
extern INT      volatile trace_lock_on;
extern CHAR     trace_file  [64];
extern CHAR     trace_queue [MAX_QUE_NAME_SIZE];

extern pBYTE   lpBuf;       // global pointer to main read thread buffer
extern WORD    wBufLen;

extern CHAR     gui_orig_info[20];

extern void    (*pfNotify)   (pBYTE, WORD, pIPC_INFO);
extern BOOLEAN (*port_write) (const WORD, const void *);
extern void    (*port_close) (INT);




/* Common.c Function Prototypes */
WORD    bcd2bin(const pBYTE lpBcd);

void    bin2bcd(pBYTE lpBcd, const WORD wInt);

BOOLEAN port_init(BYTE type, const void * userdata, pIPC_INFO ipc_info);

INT     thread_logon(pCHAR instancename, pCHAR loginname);

void    thread_logoff();

void    logoff_xipc(INT connection_type);

void    login_xipc(INT connection_type, pCHAR instancename, pCHAR loginname);

void    bin_to_ascii (pCHAR src, pCHAR dst, INT len);

void    ascii_to_bin (pBYTE src, pBYTE dst, INT len);


#endif

