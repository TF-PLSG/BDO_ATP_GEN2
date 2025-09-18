/******************************************************************************
  
        Module: convert.c
  
         Title:
  
   Description: This file is derived from original dialog manager,

   $Log:   N:\PVCS\PTE\Tools\Flow_File_Generator\HostGen\CONVERT.C  $
   
      Rev 1.0   Dec 30 1999 13:14:32   dirby
   Initial release for Host Generator.
   
      Rev 1.0   Nov 30 1998 11:39:38   MSALEH
   Dbs_settlement trafgen
   works with dbs_sett.flw flow file
   
      Rev 1.15   Oct 17 1998 14:11:12   MSALEH
   Modify the COMM routines to conform to the new
   COMM structures.
   the COMM setup page has been deleted,
   all the parameters are read from hostsim.ini
   file
   
      Rev 1.14   Oct 15 1998 11:58:48   drodden
   Split the GUI update code out of the timer thread into a separate 
   thread that only runs every 250ms or so.
   Optimized the threads to try and maximize throughput.
   Changed the average times displayed to be the true average
   time.  It was showing an ideal average time as if there isn't
   any backup in the system.
   Added lo and hi transaction times to the screen to go along with
   the average time.
   
   
      Rev 1.13   Sep 17 1998 19:09:40   drodden
   Changed the ISO response processing to actually parse the
   ISO message rather than look at hard coded offsets for the result code.
   
   
      Rev 1.12   Sep 10 1998 12:11:40   drodden
   Changed the ISO message processing to locate the terminal ID 
   in the outgoing message with a string scan rather than hard 
   coded offsets.
   
   
      Rev 1.11   Aug 26 1998 09:38:02   MSALEH
   added functionality to allow sending 
   transactions without a TPDU simulating
   an internet environment
   
   
      Rev 1.10   Jul 16 1998 09:30:10   drodden
   Changed to convert the BCD trancode to an integer
   directly rather than using sprintf an atoi.
   
   
      Rev 1.9   Jul 09 1998 14:58:30   MSALEH
   handle 0210 bad respnose
   
   
      Rev 1.8   07 Jul 1998 16:08:12   MSALEH
   Mods to support ISO8583 Sale transaction
   
      Rev 1.7   05 Jun 1998 11:59:42   MSALEH
   add functionality to support procesing ISO
   messages: 
   1) Enable flag to process ISO
   2) Prepare .flw file with ascii representation
   of messages
   
      Rev 1.6   Mar 27 1998 11:45:08   drodden
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
   
   
      Rev 1.5   26 Mar 1998 10:47:02   MSALEH
   Changed the code to use the ptecom.lib
   library routines
   
      Rev 1.4   Mar 18 1998 14:12:02   drodden
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

/*
#include <windows.h>
*/
#include <stdlib.h>
#include <stdio.h>

#include "convert.h"

