/******************************************************************************
  
        Module: trafsim.h
  
         Title:
  
   Description:

   $Log:   N:\PVCS\PTE\Tools\Flow_File_Generator\HostGen\TRAFSIM.H  $
   
      Rev 1.0   Dec 30 1999 13:14:34   dirby
   Initial release for Host Generator.
   
      Rev 1.1   Jan 14 1999 14:19:44   MSALEH
   increased the maximum number of messages
   that can be sent to 5000
   
      Rev 1.0   Nov 30 1998 11:39:38   MSALEH
   Dbs_settlement trafgen
   works with dbs_sett.flw flow file
   
      Rev 1.8   Oct 17 1998 14:11:20   MSALEH
   Modify the COMM routines to conform to the new
   COMM structures.
   the COMM setup page has been deleted,
   all the parameters are read from hostsim.ini
   file
   
      Rev 1.7   Oct 15 1998 11:58:56   drodden
   Split the GUI update code out of the timer thread into a separate 
   thread that only runs every 250ms or so.
   Optimized the threads to try and maximize throughput.
   Changed the average times displayed to be the true average
   time.  It was showing an ideal average time as if there isn't
   any backup in the system.
   Added lo and hi transaction times to the screen to go along with
   the average time.
   
   
      Rev 1.6   Aug 26 1998 09:38:30   MSALEH
   added functionality to allow sending 
   transactions without a TPDU simulating
   an internet environment
   
   
      Rev 1.5   05 Jun 1998 12:00:10   MSALEH
   add functionality to support procesing ISO
   messages: 
   1) Enable flag to process ISO
   2) Prepare .flw file with ascii representation
   of messages
   
      Rev 1.4   Mar 27 1998 11:45:28   drodden
   Changed to not use Sleep() for the transaction interval, but rather a
   loop that checks the time.  This uses more CPU, but allows finer control
   over the interval.  The problem with Sleep() is that NT uses a minimum
   time approx 10ms (i.e Sleep(1) will be for 10ms).
   Changed the average tran/message times to be computed from the
   elapsed time divided by the number of trans/messages.  The generator
   tries to compute them on a per- tran/msg basis, but the numbers were
   coming out wrong.  I could not see the problem, so I implemented this
   temporary fix.  Need to re-visit this.
   Put all global variable definitions into convert and trafsim headers.
   
   
      Rev 1.3   26 Mar 1998 10:47:14   MSALEH
   Changed the code to use the ptecom.lib
   library routines
   
      Rev 1.2   Mar 18 1998 14:12:06   drodden
   Reworked the threads to avoid deadlocking, etc.
   Changed to use ptetime for the timestamps rather than clock.  Clock 
   appears to return milliseconds, but really can't be because of the 
   rounding errors I encountered.   
   Moved all screen updates from the send-thread to the timer-thread 
   so they won't affect the send speed.
   Changed the timer-thread to only update the gui every so often so 
   it doesn't use up all the processor time.
   Changed the tcp code to use memcpy rather than memmove for 
   performance.  Memcpy will optimize into inline code.
   Changed many global variables to be private (static) and local
   to the module that declares an uses them.
   

******************************************************************************/

#ifndef TRAFSIM_H
#define TRAFSIM_H

#include "basictyp.h"
#include "ptecom.h"
#include "pteipc.h"
#include "convert.h"

#define NTFLOW_MAXMSG    5000
#define NTFLOW_MAXMSGLEN 2048
#define MAX_TERMINALS    10000

typedef struct
{
   double volatile TranStartTime; //Time last transaction started
   double volatile MsgStartTime;  //Time last message sent
   BYTE   volatile Step;          //Step of transaction
   char   volatile MesNum;        //Message number
   char            Tid [9];       //Terminal Id
} TERMLIST, *pTERMLIST;


typedef struct
{
  FILE      *fp;                                      /* Running state */
  int       nMsg;
  char      vMsg [NTFLOW_MAXMSG][NTFLOW_MAXMSGLEN];   /* Message Vector */
} MSGFLOW;


typedef struct
{
   int    volatile Trans_Started;
   int    volatile Trans_Completed;
   int    volatile Trans_Timeout;
   int    volatile Trans_Failed;
   int    volatile Msg_Sent;
   int    volatile Msg_Received;
   double volatile Msg_Elapsed;
   double volatile Trans_Elapsed;
   double volatile lo_tran_time;
   double volatile hi_tran_time;
} STATS, *pSTATS;

extern TERMLIST   tlist [MAX_TERMINALS];
extern MSGFLOW    msgflow;
extern STATS      stats;

extern CHAR       tpdu_selected[11];
/*
extern HANDLE     hPortWriteSem;
*/

extern LONG    volatile  lid,hid;
extern BOOLEAN volatile  SentFlag;
extern BOOLEAN volatile  TranStartedFlag;
extern BOOLEAN volatile  ReceivedFlag;
extern BOOLEAN volatile  CompletedFlag;
extern BOOLEAN volatile  FailedFlag;
extern BOOLEAN volatile  TimedOutFlag;

extern   INT               Trace;
extern   CHAR              MSGFLW_FILE_PATH[256];
extern   LOCAL_DM_CONFIG   trafgen_config;
extern   INT               connection_type;

void set_connection_status (INT status);

void terminate_connection ();

INT  trafgen_connection ();

INT  connection_is_up ();

INT  establish_connection ();

void SetStartupEnvironment();

void refresh_trafgen ();
void SetConnectionType(pCHAR connection_type);

#endif
