/*******************************************************************************
*  
* MODULE:      stdafx.cpp
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent System Monitor
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\STDAFX.CPP  $
   
      Rev 1.3   Apr 04 2005 15:58:50   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:38   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:02   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:12   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:36   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:06   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:02   jhapitas
   Initial checkin
*
*******************************************************************************/
// stdafx.cpp : source file that includes just the standard includes
//	monitor.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

