/*******************************************************************************
*  
* MODULE:      monitor.h
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\MONITOR.H  $
   
      Rev 1.7   Apr 04 2005 15:58:46   lmitchel
   Rebuild 4_4_1
   
      Rev 1.6   Apr 09 2003 11:45:10   svelaga
   Multiple monitor startup prevention
   
      Rev 1.5   Feb 07 2002 14:10:30   jgrguric
   Added support for auto reconnection logic now buily in to
    ptetcp.lib library. This application will now survive loss of
    applnk connection and reconnect automatically on next
    operation, when applnk is available again. Also, has extra
    logic to allow initial connection to fail ant then have the GUI
    connect at a later time when applnk comes up.
   
      Rev 1.4   Jan 17 2002 08:24:38   lmitchel
   SCR484: added activity counts; approved, declined, timed out and reversed to Transaction Activity dialog
   
      Rev 1.3   Apr 03 2001 09:42:24   SYARLAGA
   Added an Integer variable flag to structure DIALOG_STATUS  and CONTROLLER_STATS.
   process_ping definition added
   
      Rev 1.2   Aug 29 2000 15:24:36   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:00   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:04   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:34   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:08   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:06   jhapitas
   Initial checkin
*
*******************************************************************************/
// monitor.h : main header file for the MONITOR application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "app_info.h"

/////////////////////////////////////////////////////////////////////////////
// CMonitorApp:
// See monitor.cpp for the implementation of this class
//

class CMonitorApp : public CWinApp
{
public:
	CMonitorApp();

	// Start of SCR 485 Fix
	BOOL MonitorStatus();
	// End of SCR 485 Fix

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMonitorApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/* Structure to hold statistics of each Device and Network Controller. */
typedef struct
{
   CHAR   service_id[MAX_QUE_NAME_SIZE+1];
   DWORD  received_count;
   DWORD  sent_count;
   double rate;
   DWORD  previous_sent_count;
   INT    flag;
   DWORD  approved_count;
   DWORD  declined_count;
   DWORD  reversed_count;
   DWORD  timeout_count;
   DWORD  mgt_request_count;//praneeth
   DWORD  stip_sent_count;	//TF Anna
   DWORD  stip_recv_count;	//TF Anna
} CONTROLLER_STATS;

/* Structure to hold status of each Dialog Service. */
typedef struct
{
   CHAR   dialog_name[MAX_INSTANCE_NAME_SIZE+1];
   CHAR   dialog_id[MAX_QUE_NAME_SIZE+1];
   INT    status;
   INT    flag;
} DIALOG_STATUS;

/* Dialog Connection Statuses */
#define  CONNECTED     0
#define  DISCONNECTED  1
#define  NO_DATA       2

/* Monitor INI File Name */
#define  MONITOR_INI_FILE   "monitor.ini"

/* Monitor INI File Section Names */
#define  MONITOR_SECTION   "MONITOR"
#define  DIALOG_SECTION    "DIALOGS"
#define  DC_SECTION        "DEVICE_CONTROLLERS"
#define  NC_SECTION        "NETWORK_CONTROLLERS"

//ramya_to_display_MRACON
#define MRA_SECTION        "MERCHANT_RISK_ASSESSMENT"

/* Default Timer Value in seconds */
#define  DEFAULT_TIMER_VALUE    "10"

#define ST2_MONITOR_ALREADY_RUNNING 1
#define ST2_MONITOR_NOT_RUNNING 2
#define ST2_MONITOR_OVERRIDE 3
#define ST2_MONITOR_RECONNECT 4
#define ST2_MONITOR_READ_ONLY 5	// Read only monitor connection - used as a response subtype2




/* Function Prototypes */
INT   get_ini_info();



/////////////////////////////////////////////////////////////////////////////
