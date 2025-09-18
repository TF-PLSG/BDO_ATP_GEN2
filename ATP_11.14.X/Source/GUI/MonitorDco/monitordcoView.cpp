/*******************************************************************************
*  
* MODULE:      monitorview.cpp
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\monitorView.cpp  $

Rev 1.13.1.3   Jun 02 2006 16:43:44   dirby
Added a TEST Button to send a message to txcntl and get a response.
If a response is received, a message is displayed to indicate
Monitor is working.  Button stays RED until a response is received.
SCR 19357

Rev 1.13.1.2   Jun 22 2004 14:57:44   dirby
1.  Modified to sort Network ID by connection status.
2.  Do not show Logon Bin modal if none exist.
SCR 920

  Rev 1.13.1.1   Jun 09 2004 15:21:30   lmitchel
  SCR920 - Modification to set network online status to red if offline or down status.
  
	Rev 1.13.1.0   Oct 15 2003 19:46:16   dirby
	Change the "Get Advices" button to bring up a new window.
	This new window brings up 3 options.  Two for Visa SMS to
	start and stop advice transmissions; the third is the original
	SAF button, currently only used by MC.
	SCR 1083
	
	  Rev 1.13   Jul 30 2002 09:57:10   lmitchel
	  SCR812: Corrected setting of red text when dialog status = Disconnected.  Changed Log messages control from CComboBox to CColorListBox to implement text color assignment to log records.  Log messages for reversals and timeouts will be displayed in red.
	  
		Rev 1.12   Feb 07 2002 14:10:12   jgrguric
		Added support for auto reconnection logic now buily in to
		ptetcp.lib library. This application will now survive loss of
		applnk connection and reconnect automatically on next
		operation, when applnk is available again. Also, has extra
		logic to allow initial connection to fail ant then have the GUI
		connect at a later time when applnk comes up.
		
		  Rev 1.11   Jan 24 2002 12:58:06   dirby
		  Initialized some buffers, especially the display_buff prior to using them.
		  SCR 591 - Monitor aborts on reset counts button on Win95/98.
		  
			Rev 1.10   Jan 18 2002 15:29:02   dirby
			Modified sprintf statement that displays transaction activity to
			use varied spacing depending on the size of the data.  This is
			to keep the columns aligned.
			SCR 484
			
			  Rev 1.9   Jan 17 2002 08:24:46   lmitchel
			  SCR484: added activity counts; approved, declined, timed out and reversed to Transaction Activity dialog
			  
				Rev 1.8   Dec 11 2001 15:42:52   dirby
				Added network status of DOWN; removed Timed Out.
				SCR 488
				
				  Rev 1.7   Apr 03 2001 10:22:36   SYARLAGA
				  Added functions  for the Ping Dialog and Ping service buttons.
				  
					Rev 1.6   Sep 07 2000 11:03:34   lmitchel
					Correct PAN list for network logon/logoff using ncf01.identifiers
					
					  Rev 1.5   Sep 05 2000 12:00:54   lmitchel
					  corrected 'illegal message' message on monitor startup on Windows 95/98 machine.
					  
						Rev 1.4   Aug 29 2000 15:24:38   lmitchel
						New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
						
						  Rev 1.3   Nov 24 1999 14:32:04   lmitchel
						  Recompile w/new libraries ver 3_2_0/core1_3_1_3
						  
							Rev 1.2   May 27 1999 11:55:56   npogosya
							Added SAF(save and forward) button
							
							  Rev 1.1   01 Apr 1999 11:16:04   rmalathk
							  1. Made a change to send NCF01 record
							  when requesting status updates from network handlers.
							  2. Increased sleep time between queries.
							  
								Rev 1.0   24 Feb 1999 15:59:20   rmalathk
								Initial Revision
								
								  Rev 1.0   Dec 04 1998 15:33:38   MSALEH
								  initial release
								  
									Rev 1.0   Dec 04 1998 15:00:12   MSALEH
									initial release
									
									  Rev 1.0   28 Oct 1998 16:31:18   jhapitas
									  Initial checkin
									  *
*******************************************************************************/
// monitorView.cpp : implementation of the CMonitorView class
//

#include "stdafx.h"
#include <Winbase.h>
extern "C"
{
#include "ntutils.h"
#include "pte.h"
#include "ptetcp.h"
#include "txutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "TXDSAPI.h"
}

#include "app_info.h"
#include "monitordco.h"
#include "monitordcoDoc.h"
#include "monitordcoView.h"
#include "PrimaryAccountList.h"
#include "Advices.h"

/*new messagebox timeout headers starts*/
#include <windows.h>
#include <tchar.h>
/*new messagebox timeout headers ends*/

#define OFFSET_YEAR 1900
#define OFFSET_MONTH 1
#define MAX_SPACES   7
#define MAXLEN		 5
#define SEPARATOR '::'
#define  MAX_NETWORKS   30
#define  BLACK  0
#define  RED    1
#define  YELLOW 2
#define  GREEN  3

/*new messagebox timeout declarations starts*/
//Functions & other definitions required-->
typedef int (__stdcall *MSGBOXAAPI)(IN HWND hWnd, 
        IN LPCSTR lpText, IN LPCSTR lpCaption, 
        IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef int (__stdcall *MSGBOXWAPI)(IN HWND hWnd, 
        IN LPCWSTR lpText, IN LPCWSTR lpCaption, 
        IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, 
    IN LPCSTR lpCaption, IN UINT uType, 
    IN WORD wLanguageId, IN DWORD dwMilliseconds);
int MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, 
    IN LPCWSTR lpCaption, IN UINT uType, 
    IN WORD wLanguageId, IN DWORD dwMilliseconds);

#ifdef UNICODE
    #define MessageBoxTimeout MessageBoxTimeoutW
#else
    #define MessageBoxTimeout MessageBoxTimeoutA
#endif 

#define MB_TIMEDOUT 32000
/*new messagebox timeout declarations ends*/
/*Definitions at the end*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char monitor1handle[256] ={0};
char monitor2handle[256] ={0};
char monitor3handle[256] ={0};
char monitor4handle[256] ={0};

extern char HostName1[50];
extern char HostName2[50];
extern char HostName3[50];
extern char HostName4[50];

CHAR mon1_name[50];
CHAR mon2_name[50];
CHAR mon3_name[50];
CHAR mon4_name[50];

int readonlyCount = 0;

CHAR netds_que_name[] = "netdsA";
CHAR txcntl_que_name[] = "txcntlA";
UINT m_RefreshTimer;
UINT _NCStatusTimer;
UINT Timer = 0;
INT		_NCStatus_Beep_Interval;
CHAR _MuteOn;
extern CString cmdlinestr;
extern char u_name[51];
extern char u_pwd[51];
int num_rows_returned;

UINT m_RefreshTimer2;
UINT _NCStatusTimer2;
UINT Timer2 = 0;
INT		_NCStatus_Beep_Interval2;
CHAR _MuteOn2;

UINT m_RefreshTimer3;
UINT _NCStatusTimer3;
UINT Timer3 = 0;
INT		_NCStatus_Beep_Interval3;
CHAR _MuteOn3;

UINT m_RefreshTimer4;
UINT _NCStatusTimer4;
UINT Timer4 = 0;
INT		_NCStatus_Beep_Interval4;
CHAR _MuteOn4;

// JMG added on 2/7/02 to save host name for auto reconnection after initial connection
// failure. This buffer will contain data only if the initial connection fails.
CHAR  hostname_saved[50];
CHAR  hostname_saved2[50];
CHAR  hostname_saved3[50];
CHAR  hostname_saved4[50];

/* Global structures used for holding statistics for individual services. */
CONTROLLER_STATS  Controller_Stats[MAX_INI_ID_LIST_SIZE*2];
INT               Controller_Count;

DIALOG_STATUS   Dialog_Status[MAX_INI_ID_LIST_SIZE];
INT             Dialog_ID_Count;

//Monitor2
CONTROLLER_STATS  Controller_Stats2[MAX_INI_ID_LIST_SIZE*2];
INT               Controller_Count2;

DIALOG_STATUS   Dialog_Status2[MAX_INI_ID_LIST_SIZE];
INT             Dialog_ID_Count2;

//Monitor3
CONTROLLER_STATS  Controller_Stats3[MAX_INI_ID_LIST_SIZE*2];
INT               Controller_Count3;

DIALOG_STATUS   Dialog_Status3[MAX_INI_ID_LIST_SIZE];
INT             Dialog_ID_Count3;

//Monitor4
CONTROLLER_STATS  Controller_Stats4[MAX_INI_ID_LIST_SIZE*2];
INT               Controller_Count4;

DIALOG_STATUS   Dialog_Status4[MAX_INI_ID_LIST_SIZE];
INT             Dialog_ID_Count4;

/* Interval in seconds for timer to request statistics and statuses */
INT   Timer_Interval;
INT   Timer_Interval2;
INT   Timer_Interval3;
INT   Timer_Interval4;

//Time interval for Messagebox, which causes queueing if left non-responded
DWORD promptAlive;
//File writing flag for time out alerts
INT timeoutFileFlag;

CString szDeviceControllers;
CString szMracon;

CString szDeviceControllers2;
CString szMracon2;

CString szDeviceControllers3;
CString szMracon3;

CString szDeviceControllers4;
CString szMracon4;

#define ST2_MONITOR_CLEARED	6
#define ST1_MONITOR_CLEAR_SOCKETS	101
#define MT_MONITOR_CLEAR_SOCKETS	100

#define ST1_ACQUIRER_OFF	150
#define ST1_ACQUIRER_ON		151

#define MRACON_LEN 6
#define FCONDS_LEN 6

extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
	CHAR appProductVersion[MAX_APPVERSION_SIZE];
	extern CHAR str_port[10];
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorView

	IMPLEMENT_DYNCREATE(CMonitorView, CFormView)

	BEGIN_MESSAGE_MAP(CMonitorView, CFormView)
	//{{AFX_MSG_MAP(CMonitorView)
	//MONITOR 1
	ON_BN_CLICKED(IDC_CLEARLOG, OnClearlog)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseBtn)
	ON_BN_CLICKED(IDC_ECHOTEST, OnEchotest)
	ON_BN_CLICKED(IDC_LOGOFF, OnLogoff)
	ON_BN_CLICKED(IDC_LOGON, OnLogon)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_SAVELOG, OnSavelog)
	ON_BN_CLICKED(IDC_SAF, OnSaf)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESET_COUNTS_BUTTON, OnResetCountsButton)
	ON_BN_CLICKED(IDC_PING_BUTTON, OnPingButton)
	ON_BN_CLICKED(IDC_PING_DIALOG_BUTTON1, OnPingDialogButton)
	ON_BN_CLICKED(IDC_TEST_BUTTON, OnTestButton)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ACQUIRER_OFF, OnAcquirerOff)
	ON_LBN_SELCHANGE(IDC_NETWORK_STATUS, OnSelchangeNetworkStatus)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_NET_TRAN_LIST, OnItemchangedNetTranList)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CUP_KEY_REQ_BUTTON, OnCupKeyRequestButton)
	ON_BN_CLICKED(IDC_CUP_KEY_UPDATE_BUTTON, OnCupKeySendButton)
	ON_BN_CLICKED(IDC_CUP_KEY_RELOAD, OnCupKeyReloadButton)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeMonitorTab1)

	////MONITOR 2 buttons functions
	ON_BN_CLICKED(IDC_CLEARLOG2, OnClearlog2)
	ON_BN_CLICKED(IDC_ECHOTEST2, OnEchotest2)
	ON_BN_CLICKED(IDC_LOGOFF2, OnLogoff2)
	ON_BN_CLICKED(IDC_LOGON2, OnLogon2)
	ON_BN_CLICKED(IDC_REFRESH2, OnRefresh2)
	ON_BN_CLICKED(IDC_SAVELOG2, OnSavelog2)
	ON_BN_CLICKED(IDC_SAF2, OnSaf2)
	ON_BN_CLICKED(IDC_RESET_COUNTS_BUTTON2, OnResetCountsButton2)
	ON_BN_CLICKED(IDC_PING_BUTTON2, OnPingButton2)
	ON_BN_CLICKED(IDC_PING_DIALOG_BUTTON2, OnPingDialogButton2)
	ON_BN_CLICKED(IDC_TEST_BUTTON2, OnTestButton2)
	ON_BN_CLICKED(IDC_ACQUIRER_OFF2, OnAcquirerOff2)
	ON_BN_CLICKED(IDC_PING2, OnPing2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_NET_TRAN_LIST2, OnItemchangedNetTranList2)
	ON_BN_CLICKED(IDC_CUP_KEY_REQ_BUTTON2, OnCupKeyRequestButton2)
	ON_BN_CLICKED(IDC_CUP_KEY_UPDATE_BUTTON3, OnCupKeySendButton2)
	ON_BN_CLICKED(IDC_CUP_KEY_RELOAD2, OnCupKeyReloadButton2)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, OnSelchangeMonitorTab2)
	//}}AFX_MSG_MAP

	//MONITOR 3
	ON_BN_CLICKED(IDC_CLEARLOG3, OnClearlog3)
	ON_BN_CLICKED(IDC_ECHOTEST3, OnEchotest3)
	ON_BN_CLICKED(IDC_LOGOFF3, OnLogoff3)
	ON_BN_CLICKED(IDC_LOGON3, OnLogon3)
	ON_BN_CLICKED(IDC_REFRESH3, OnRefresh3)
	ON_BN_CLICKED(IDC_SAVELOG3, OnSavelog3)
	ON_BN_CLICKED(IDC_SAF3, OnSaf3)
	ON_BN_CLICKED(IDC_RESET_COUNTS_BUTTON3, OnResetCountsButton3)
	ON_BN_CLICKED(IDC_PING_BUTTON3, OnPingButton3)
	ON_BN_CLICKED(IDC_PING_DIALOG_BUTTON3, OnPingDialogButton3)
	ON_BN_CLICKED(IDC_TEST_BUTTON3, OnTestButton3)
	ON_BN_CLICKED(IDC_ACQUIRER_OFF3, OnAcquirerOff3)
	ON_BN_CLICKED(IDC_PING3, OnPing3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_NET_TRAN_LIST3, OnItemchangedNetTranList3)
	ON_BN_CLICKED(IDC_CUP_KEY_REQ_BUTTON3, OnCupKeyRequestButton3)
	ON_BN_CLICKED(IDC_CUP_KEY_UPDATE_BUTTON4, OnCupKeySendButton3)
	ON_BN_CLICKED(IDC_CUP_KEY_RELOAD3, OnCupKeyReloadButton3)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB3, OnSelchangeMonitorTab3)

	//MONITOR 4
	ON_BN_CLICKED(IDC_CLEARLOG4, OnClearlog4)
	ON_BN_CLICKED(IDC_ECHOTEST4, OnEchotest4)
	ON_BN_CLICKED(IDC_LOGOFF4, OnLogoff4)
	ON_BN_CLICKED(IDC_LOGON4, OnLogon4)
	ON_BN_CLICKED(IDC_REFRESH4, OnRefresh4)
	ON_BN_CLICKED(IDC_SAVELOG4, OnSavelog4)
	ON_BN_CLICKED(IDC_SAF4, OnSaf4)
	ON_BN_CLICKED(IDC_RESET_COUNTS_BUTTON4, OnResetCountsButton4)
	ON_BN_CLICKED(IDC_PING_BUTTON4, OnPingButton4)
	ON_BN_CLICKED(IDC_PING_DIALOG_BUTTON4, OnPingDialogButton4)
	ON_BN_CLICKED(IDC_TEST_BUTTON4, OnTestButton4)
	ON_BN_CLICKED(IDC_ACQUIRER_OFF4, OnAcquirerOff4)
	ON_BN_CLICKED(IDC_PING4, OnPing4)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_NET_TRAN_LIST4, OnItemchangedNetTranList4)
	ON_BN_CLICKED(IDC_CUP_KEY_REQ_BUTTON4, OnCupKeyRequestButton4)
	ON_BN_CLICKED(IDC_CUP_KEY_UPDATE_BUTTON5, OnCupKeySendButton4)
	ON_BN_CLICKED(IDC_CUP_KEY_RELOAD4, OnCupKeyReloadButton4)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB4, OnSelchangeMonitorTab4)

	ON_BN_CLICKED(IDC_CHECK5, OnMuteAll)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CMonitorView construction/destruction

	CMonitorView::CMonitorView()
: CFormView(CMonitorView::IDD)
{
	//{{AFX_DATA_INIT(CMonitorView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	nc_status = 0;
	m_bAcqOn = false;
	m_bReadOnly = false;

	nc_status2 = 0;
	m_bAcqOn2 = false;
	m_bReadOnly2 = false;

	nc_status3 = 0;
	m_bAcqOn3 = false;
	m_bReadOnly3 = false;

	nc_status4 = 0;
	m_bAcqOn4 = false;
	m_bReadOnly4 = false;
}

CMonitorView::~CMonitorView()
{
}

void CMonitorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorView)
	//MONITOR1
	DDX_Control(pDX, IDC_CUP_KEY_UPDATE_BUTTON, m_CntlCupKeySend);
	DDX_Control(pDX, IDC_CUP_KEY_RELOAD, m_CntlCupKeyReload);
	DDX_Control(pDX, IDC_CUP_KEY_REQ_BUTTON, m_CntlCupKeyRequest);
	DDX_Control(pDX, IDC_NET_TRAN_LIST, m_CntlNetTranList);
	DDX_Control(pDX, IDC_ACQUIRER_OFF, m_CntlAcqOff);
	DDX_Control(pDX, IDC_SAF, m_CntlGetAdvisesButton);
	DDX_Control(pDX, IDC_REFRESH, m_CntlRefreshButton);
	DDX_Control(pDX, IDC_PING_DIALOG_BUTTON1, m_CntlPingDialogButton);
	DDX_Control(pDX, IDC_ECHOTEST, m_CntlEchoTestButton);
	DDX_Control(pDX, IDC_LOGOFF, m_CntlLogOff);
	DDX_Control(pDX, IDC_LOGON, m_CntlLogOn);
	DDX_Control(pDX, IDC_CLEARLOG, m_CntlClearLogButton);
	DDX_Control(pDX, IDC_SAVELOG, m_CntlSaveLogButton);
	DDX_Control(pDX, IDC_CHECK1, m_CntlMuteBeep);
	DDX_Control(pDX, IDC_NC_STATUS, m_CntlNCStatus);
	DDX_Control(pDX, IDC_TEST_RESULT, m_TestResult);
	DDX_Control(pDX, IDC_TEST_BUTTON, m_TEST_BUTTON);
	DDX_Control(pDX, IDC_PING_BUTTON, m_PingButton);
	DDX_Control(pDX, IDC_TRAN_ACTIVITY_LIST, m_TranActivityList);
	DDX_Control(pDX, IDC_RESET_COUNTS_BUTTON, m_CntlResetCountsButton);
	DDX_Control(pDX, IDC_DIALOG_CONNECT_STATUS, m_DialogConnectStatus);
	DDX_Control(pDX, IDC_LOG_MESSAGES, m_LogMessages);
	DDX_Control(pDX, IDC_NETWORK_STATUS, m_NetworkStatus);
	DDX_Control(pDX, IDC_MONITOR1_NAME, m_Monitor1Name);
	DDX_Control(pDX, IDC_PING, m_Ping);

	DDX_Control(pDX, IDC_TAB1, m_MainTab1);
	DDX_Control(pDX, IDC_DIALOG_ID_LABEL, m_DialogIdLabel);
	DDX_Control(pDX, IDC_LOG_STATUS_LABEL, m_LogStatusLabel);
	DDX_Control(pDX, IDC_NAME_LABEL, m_NameLabel);
	DDX_Control(pDX, IDC_LOG_DATE_LABEL, m_LogDateLabel);
	DDX_Control(pDX, IDC_LOG_TIME_LABEL, m_LogTimeLabel);
	DDX_Control(pDX, IDC_LOG_TYPE_LABEL, m_LogTypeLabel);
	DDX_Control(pDX, IDC_LOG_SERVICE_LABEL, m_LogServiceLabel);
	DDX_Control(pDX, IDC_LOG_MSG_LABEL, m_LogMessageLabel);

	//MONITOR2
	DDX_Control(pDX, IDC_CUP_KEY_UPDATE_BUTTON3, m_CntlCupKeySend2);
	DDX_Control(pDX, IDC_CUP_KEY_RELOAD2, m_CntlCupKeyReload2);
	DDX_Control(pDX, IDC_CUP_KEY_REQ_BUTTON2, m_CntlCupKeyRequest2);
	DDX_Control(pDX, IDC_NET_TRAN_LIST2, m_CntlNetTranList2);
	DDX_Control(pDX, IDC_ACQUIRER_OFF2, m_CntlAcqOff2);
	DDX_Control(pDX, IDC_SAF2, m_CntlGetAdvisesButton2);
	DDX_Control(pDX, IDC_REFRESH2, m_CntlRefreshButton2);
	DDX_Control(pDX, IDC_PING_DIALOG_BUTTON2, m_CntlPingDialogButton2);
	DDX_Control(pDX, IDC_ECHOTEST2, m_CntlEchoTestButton2);
	DDX_Control(pDX, IDC_LOGOFF2, m_CntlLogOff2);
	DDX_Control(pDX, IDC_LOGON2, m_CntlLogOn2);
	DDX_Control(pDX, IDC_CLEARLOG2, m_CntlClearLogButton2);
	DDX_Control(pDX, IDC_SAVELOG2, m_CntlSaveLogButton2);
	DDX_Control(pDX, IDC_CHECK2, m_CntlMuteBeep2);
	DDX_Control(pDX, IDC_NC_STATUS2, m_CntlNCStatus2);
	DDX_Control(pDX, IDC_TEST_RESULT2, m_TestResult2);
	DDX_Control(pDX, IDC_TEST_BUTTON2, m_TEST_BUTTON2);
	DDX_Control(pDX, IDC_PING_BUTTON2, m_PingButton2);
	DDX_Control(pDX, IDC_TRAN_ACTIVITY_LIST2, m_TranActivityList2);
	DDX_Control(pDX, IDC_RESET_COUNTS_BUTTON2, m_CntlResetCountsButton2);
	DDX_Control(pDX, IDC_DIALOG_CONNECT_STATUS2, m_DialogConnectStatus2);
	DDX_Control(pDX, IDC_LOG_MESSAGES2, m_LogMessages2);
	DDX_Control(pDX, IDC_NETWORK_STATUS2, m_NetworkStatus2);
	DDX_Control(pDX, IDC_MONITOR2_NAME, m_Monitor2Name);
	DDX_Control(pDX, IDC_PING2, m_Ping2);

	DDX_Control(pDX, IDC_TAB2, m_MainTab2);
	DDX_Control(pDX, IDC_DIALOG_ID_LABEL2, m_DialogIdLabel2);
	DDX_Control(pDX, IDC_LOG_STATUS_LABEL2, m_LogStatusLabel2);
	DDX_Control(pDX, IDC_NAME_LABEL2, m_NameLabel2);
	DDX_Control(pDX, IDC_LOG_DATE_LABEL2, m_LogDateLabel2);
	DDX_Control(pDX, IDC_LOG_TIME_LABEL2, m_LogTimeLabel2);
	DDX_Control(pDX, IDC_LOG_TYPE_LABEL2, m_LogTypeLabel2);
	DDX_Control(pDX, IDC_LOG_SERVICE_LABEL2, m_LogServiceLabel2);
	DDX_Control(pDX, IDC_LOG_MSG_LABEL2, m_LogMessageLabel2);

	//MONITOR3
	DDX_Control(pDX, IDC_CUP_KEY_UPDATE_BUTTON4, m_CntlCupKeySend3);
	DDX_Control(pDX, IDC_CUP_KEY_RELOAD3, m_CntlCupKeyReload3);
	DDX_Control(pDX, IDC_CUP_KEY_REQ_BUTTON3, m_CntlCupKeyRequest3);
	DDX_Control(pDX, IDC_NET_TRAN_LIST3, m_CntlNetTranList3);
	DDX_Control(pDX, IDC_ACQUIRER_OFF3, m_CntlAcqOff3);
	DDX_Control(pDX, IDC_SAF3, m_CntlGetAdvisesButton3);
	DDX_Control(pDX, IDC_REFRESH3, m_CntlRefreshButton3);
	DDX_Control(pDX, IDC_PING_DIALOG_BUTTON3, m_CntlPingDialogButton3);
	DDX_Control(pDX, IDC_ECHOTEST3, m_CntlEchoTestButton3);
	DDX_Control(pDX, IDC_LOGOFF3, m_CntlLogOff3);
	DDX_Control(pDX, IDC_LOGON3, m_CntlLogOn3);
	DDX_Control(pDX, IDC_CLEARLOG3, m_CntlClearLogButton3);
	DDX_Control(pDX, IDC_SAVELOG3, m_CntlSaveLogButton3);
	DDX_Control(pDX, IDC_CHECK3, m_CntlMuteBeep3);
	DDX_Control(pDX, IDC_NC_STATUS3, m_CntlNCStatus3);
	DDX_Control(pDX, IDC_TEST_RESULT3, m_TestResult3);
	DDX_Control(pDX, IDC_TEST_BUTTON3, m_TEST_BUTTON3);
	DDX_Control(pDX, IDC_PING_BUTTON3, m_PingButton3);
	DDX_Control(pDX, IDC_TRAN_ACTIVITY_LIST3, m_TranActivityList3);
	DDX_Control(pDX, IDC_RESET_COUNTS_BUTTON3, m_CntlResetCountsButton3);
	DDX_Control(pDX, IDC_DIALOG_CONNECT_STATUS3, m_DialogConnectStatus3);
	DDX_Control(pDX, IDC_LOG_MESSAGES3, m_LogMessages3);
	DDX_Control(pDX, IDC_NETWORK_STATUS3, m_NetworkStatus3);
	DDX_Control(pDX, IDC_MONITOR3_NAME, m_Monitor3Name);
	DDX_Control(pDX, IDC_PING3, m_Ping3);

	DDX_Control(pDX, IDC_TAB3, m_MainTab3);
	DDX_Control(pDX, IDC_DIALOG_ID_LABEL3, m_DialogIdLabel3);
	DDX_Control(pDX, IDC_LOG_STATUS_LABEL3, m_LogStatusLabel3);
	DDX_Control(pDX, IDC_NAME_LABEL3, m_NameLabel3);
	DDX_Control(pDX, IDC_LOG_DATE_LABEL3, m_LogDateLabel3);
	DDX_Control(pDX, IDC_LOG_TIME_LABEL3, m_LogTimeLabel3);
	DDX_Control(pDX, IDC_LOG_TYPE_LABEL3, m_LogTypeLabel3);
	DDX_Control(pDX, IDC_LOG_SERVICE_LABEL3, m_LogServiceLabel3);
	DDX_Control(pDX, IDC_LOG_MSG_LABEL3, m_LogMessageLabel3);

	//MONITOR4
	DDX_Control(pDX, IDC_CUP_KEY_UPDATE_BUTTON5, m_CntlCupKeySend4);
	DDX_Control(pDX, IDC_CUP_KEY_RELOAD4, m_CntlCupKeyReload4);
	DDX_Control(pDX, IDC_CUP_KEY_REQ_BUTTON4, m_CntlCupKeyRequest4);
	DDX_Control(pDX, IDC_NET_TRAN_LIST4, m_CntlNetTranList4);
	DDX_Control(pDX, IDC_ACQUIRER_OFF4, m_CntlAcqOff4);
	DDX_Control(pDX, IDC_SAF4, m_CntlGetAdvisesButton4);
	DDX_Control(pDX, IDC_REFRESH4, m_CntlRefreshButton4);
	DDX_Control(pDX, IDC_PING_DIALOG_BUTTON4, m_CntlPingDialogButton4);
	DDX_Control(pDX, IDC_ECHOTEST4, m_CntlEchoTestButton4);
	DDX_Control(pDX, IDC_LOGOFF4, m_CntlLogOff4);
	DDX_Control(pDX, IDC_LOGON4, m_CntlLogOn4);
	DDX_Control(pDX, IDC_CLEARLOG4, m_CntlClearLogButton4);
	DDX_Control(pDX, IDC_SAVELOG4, m_CntlSaveLogButton4);
	DDX_Control(pDX, IDC_CHECK4, m_CntlMuteBeep4);
	DDX_Control(pDX, IDC_NC_STATUS4, m_CntlNCStatus4);
	DDX_Control(pDX, IDC_TEST_RESULT4, m_TestResult4);
	DDX_Control(pDX, IDC_TEST_BUTTON4, m_TEST_BUTTON4);
	DDX_Control(pDX, IDC_PING_BUTTON4, m_PingButton4);
	DDX_Control(pDX, IDC_TRAN_ACTIVITY_LIST4, m_TranActivityList4);
	DDX_Control(pDX, IDC_RESET_COUNTS_BUTTON4, m_CntlResetCountsButton4);
	DDX_Control(pDX, IDC_DIALOG_CONNECT_STATUS4, m_DialogConnectStatus4);
	DDX_Control(pDX, IDC_LOG_MESSAGES4, m_LogMessages4);
	DDX_Control(pDX, IDC_NETWORK_STATUS4, m_NetworkStatus4);
	DDX_Control(pDX, IDC_MONITOR4_NAME, m_Monitor4Name);
	DDX_Control(pDX, IDC_PING4, m_Ping4);

	DDX_Control(pDX, IDC_TAB4, m_MainTab4);
	DDX_Control(pDX, IDC_DIALOG_ID_LABEL4, m_DialogIdLabel4);
	DDX_Control(pDX, IDC_LOG_STATUS_LABEL4, m_LogStatusLabel4);
	DDX_Control(pDX, IDC_NAME_LABEL4, m_NameLabel4);
	DDX_Control(pDX, IDC_LOG_DATE_LABEL4, m_LogDateLabel4);
	DDX_Control(pDX, IDC_LOG_TIME_LABEL4, m_LogTimeLabel4);
	DDX_Control(pDX, IDC_LOG_TYPE_LABEL4, m_LogTypeLabel4);
	DDX_Control(pDX, IDC_LOG_SERVICE_LABEL4, m_LogServiceLabel4);
	DDX_Control(pDX, IDC_LOG_MSG_LABEL4, m_LogMessageLabel4);

	DDX_Control(pDX, IDC_CHECK5, m_CntlMuteAll);
	DDX_Control(pDX, IDC_CLOSE, m_CloseAll);
	//}}AFX_DATA_MAP
}

BOOL CMonitorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorView diagnostics

#ifdef _DEBUG
void CMonitorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMonitorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

/*
CMonitorDoc* CMonitorView::GetDocument() // non-debug version is inline
{
ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMonitorDoc)));
return (CMonitorDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMonitorView message handlers

BYTE CMonitorView::monitor_fetch_ucf01( pBYTE psStructurePtr, int nStructureSize, 
									   BYTE  app_data_type,
									   pCHAR error_msg )
{
	pPTE_MSG	   p_msg_out;
	BYTE		   rcode;
	
	memset( (char*)ucf01SendRecvBuffer, 0, sizeof( ucf01SendRecvBuffer ) );
	memcpy (((char*)ucf01SendRecvBuffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);
	p_msg_out = ptemsg_build_msg(	MT_DB_REQUEST,
		ST1_DB_SELECT,
		0,
		"trandsA",
		interactive_que_name,
		(pBYTE) ucf01SendRecvBuffer,
		sizeof( ucf01SendRecvBuffer ),
		UCF01_DATA );
	if( p_msg_out == NULL_PTR )
		return PTEMSG_INSUFFICIENT_MEMORY;
	
	// Set a flag for response not received.
	responceUCF01FetchNotReceived = 1;
	rcode = ptetcp_send( p_msg_out, error_msg);
	if( p_msg_out != NULL_PTR )
		free( p_msg_out );

	if ( rcode == PTEMSG_OK )	// Successfully sent.
	{
		int waitCount = 0;
		// wait until response is received.
		while ( responceUCF01FetchNotReceived && waitCount < 500 )	// Maximum wait 10 seconds = 500 X 20ms = 10000 ms
		{
			Sleep(20);
			waitCount++;
		}
		// If response was still NOT received...
		if ( responceUCF01FetchNotReceived )
		{
			rcode = PTEMSG_GUI_TIMEDOUT;
		}
		else
		{
			// Response was received. Copy to passed pointer parameter.
			memcpy (psStructurePtr, ((char*)ucf01SendRecvBuffer + sizeof(AUTH_TX)), nStructureSize);
			rcode = PTEMSG_OK;
		}
	}
	
	return ( rcode );
}

BYTE CMonitorView::UserAuthentication()
{
	BYTE rcode = 0;
	UCF01 ucf01;
	char error_msg[256] = {0};
		
	// next authenticate the user
	memset( &ucf01, 0, sizeof( UCF01 ) );
	strcpy( (char *)ucf01.primary_key.name, u_name);
	rcode = monitor_fetch_ucf01((pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg);

	if( rcode == PTEMSG_NOT_FOUND || rcode != PTEMSG_OK )
	{   
		::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		return 1;
	}
		
	// check password	
	if( strcmp( (char *)ucf01.password, u_pwd ) != 0 )
	{	
		::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		return 1;
	}
	return 0;
}

BYTE CMonitorView::monitor_fetch_ucf01_new( pBYTE psStructurePtr, int nStructureSize, 
									   BYTE  app_data_type,
									   pCHAR error_msg )
{
	pPTE_MSG	   p_msg_out, p_msg_in;
	pPTE_MSG_DATA   p_msg_data = NULL_PTR ;
    pBYTE           p_data = NULL_PTR ;
	BYTE		   rcode;
	
	memset( (char*)ucf01SendRecvBuffer, 0, sizeof( ucf01SendRecvBuffer ) );
	memcpy (((char*)ucf01SendRecvBuffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

	p_msg_out = ptemsg_build_msg(	MT_DB_REQUEST,
		ST1_DB_SELECT,
		0,
		"trandsA",
		interactive_que_name,
		(pBYTE) ucf01SendRecvBuffer,
		sizeof( ucf01SendRecvBuffer ),
		UCF01_DATA );

	if( p_msg_out == NULL_PTR )
		return PTEMSG_INSUFFICIENT_MEMORY;
	
	p_msg_in = ptetcp_send_receive( p_msg_out, 3, error_msg );

	if( p_msg_in == NULL_PTR )	
		return 1;
	if( ( rcode = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
		return 1;

	p_msg_data = ptemsg_get_pte_msg_data (p_msg_in); 
	p_data   = ptemsg_get_pte_msg_data_data (p_msg_data); 
	p_data   = p_data + sizeof(AUTH_TX);
	
	memcpy (psStructurePtr, p_data, nStructureSize);

	return PTEMSG_OK;
}

void CMonitorView::get_timeout_ini_data()
{
	CHAR buffer[10] = "";
	CHAR ini_file[256] = {0};

	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

	    GetPrivateProfileString(
        "MONITOR_TIMEOUT_PROMPT", 	               // points to section name 
        "PROMPT_ALIVE_TIMER",	       // points to key name 
        "3",	                   // points to default string 
        buffer,                   // points to destination buffer 
        sizeof(buffer) - 1,       // size of destination buffer 
        ini_file ); 

	promptAlive = atol(buffer)*1000;

	memset(&buffer, 0, sizeof(buffer));
	GetPrivateProfileString(
        "MONITOR_TIMEOUT_PROMPT", 	               // points to section name 
        "TIMEOUT_FILE_FLAG",	       // points to key name 
        "0",	                   // points to default string 
        buffer,                   // points to destination buffer 
        sizeof(buffer) - 1,       // size of destination buffer 
        ini_file ); 

	timeoutFileFlag = atoi(buffer);
}
/*************************************************************************************
NAME:               OnInitialUpdate

DESCRIPTION:        Perform the initialisation for the application.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	m_font.CreatePointFont(85,"Courier New");
	// TODO: Add your specialized code here and/or call the base class
	CHAR  buffer[256], error_msg[100];
	CHAR  hostname[50]		= {0}; 
	int ConnectStatus;

	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes
	
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	get_timeout_ini_data();
	
	if( !ptetcp_init( error_msg ) )
	{
		sprintf( buffer, "%s.", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		return;
	}

	m_bMonitorUp = false;
	m_bMonitorUp2 = false;
	m_bMonitorUp3 = false;
	m_bMonitorUp4 = false;
	m_hMonitor1 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_MONITOR1));
	m_hMonitor2 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_MONITOR2));
	m_hMonitor3 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_MONITOR3));
	m_hMonitor4 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_MONITOR4));

	m_Monitor1Name.SetBitmap(m_hMonitor1);
	m_Monitor2Name.SetBitmap(m_hMonitor2);
	m_Monitor3Name.SetBitmap(m_hMonitor3);
	m_Monitor4Name.SetBitmap(m_hMonitor4);

	//get hostnames, connect and initialize the controls
	for(int index=0; index<4; index++)
	{
		strcpy(hostname, GetHostname(MONITOR1+index));
		if(strlen(hostname) > 0)
		{
			ConnectStatus = HostConnect(hostname, MONITOR1+index);
			if(ConnectStatus == PTEMSG_OK)
				InitializeMonitorControls(MONITOR1+index);
			else if(ConnectStatus == AUTH_FAILURE)
			{
				PostQuitMessage(0);
				break;
			}
		}
		else if(strlen(hostname) <= 0 && index == 0)
		{
			AfxMessageBox( "Primary Machine ID cannot be empty", MB_ICONSTOP | MB_OK );
			PostQuitMessage(0);
			break;
		}
		else
		{
			DisableAllButtons(MONITOR1+index);
		}
	}

	//added tab controls for network status & log messages
	//Tab intialization for All Monitors
	InitTabControls();
}


char* CMonitorView::GetHostname(int sel)
{
	char hname[50] = {0};

	if(sel == 1)
	{
		sprintf(monitor1handle, "%p", this->m_hWnd);
		if( !GetThisMachineId( hname ) )
		{
			AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
				be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
			strcpy( hname, "localhost" );
		}
	}
	else if(sel == 2)
	{
		sprintf(monitor2handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_2( hname ) )
		{
			AfxMessageBox( "Secondary Machine ID not found in Registry; Registry may not \
				be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
			strcpy( hname, "localhost" );
		}
	}
	else if(sel == 3)
	{
		sprintf(monitor3handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_3( hname ) )
		{
			AfxMessageBox( "Third Machine ID not found in Registry; Registry may not \
				be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
			strcpy( hname, "localhost" );
		}
	}
	else if(sel == 4)
	{
		sprintf(monitor4handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_4( hname ) )
		{
			AfxMessageBox( "Fourth Machine ID not found in Registry; Registry may not \
				be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
			strcpy( hname, "localhost" );
		}
	}

	return hname;
}

BYTE CMonitorView::HostConnect(char *hname, int sel)
{
	CHAR  buffer[256] = {0}, error_msg[250] = {0};

	if(sel == 1)
	{
		if( !ptetcp_connect( hname, error_msg ) )
		{
			//Initial connection failed!
			sprintf( buffer, "MONITOR1 GH1: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			// JMG added on 2/7/02 to save host name for later reconnection
			// when the initial connection fails
			strcpy( hostname_saved, hname);
			//We now fall thru to allow the windows to set up, even though we are not connected.

			DisableAllButtons(1);

			return 1;//Error
		}
		else
		{
			m_bMonitorUp = true;

			// Initial connection established!
			// Clear the saved host name buffer to indicate
			// a valid initial connection was established and then initialize
			// the applnk connection data.

			memset( hostname_saved, 0, sizeof( hostname_saved ) );

			UserAuthentication();

			InitConnectionData();
		}
		//memset( mon1_name, 0, sizeof( mon1_name ) );
		//sprintf(mon1_name, "MONITOR2 GH1 - %s", hname);
		//m_Monitor1Name.SetWindowText(mon1_name);

		if ( m_MainTab1.InsertItem( 1, "Network Status" ) == -1 )
			AfxMessageBox( "MONITOR1 GH1: Tab Control construction Failed", MB_OK, 0);
		if ( m_MainTab1.InsertItem( 1, "Log Messages" ) == -1 )
			AfxMessageBox( "MONITOR1 GH1: Tab Control construction Failed", MB_OK, 0);
		m_MainTab1.SetCurSel(0);
	}
	else if(sel == 2)
	{
		if( !ptetcp_connect2( hname, error_msg ) )
		{
			//Initial connection failed!
			sprintf( buffer, "MONITOR2 GH2: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			strcpy( hostname_saved2, hname);
			DisableAllButtons(2);

			return 1;//Error
		}
		else
		{
			m_bMonitorUp2 = true;

			// Initial connection established!
			memset( hostname_saved2, 0, sizeof( hostname_saved2 ) );
			InitConnectionData2();
		}
		//memset( mon2_name, 0, sizeof( mon2_name ) );
		//sprintf(mon2_name, "MONITOR2 GH2 - %s", hname);
		//m_Monitor2Name.SetWindowText(mon2_name);
		
		if ( m_MainTab2.InsertItem( 1, "Network Status" ) == -1 )
			AfxMessageBox( "MONITOR2 GH2: Tab Control construction Failed", MB_OK, 0);
		if ( m_MainTab2.InsertItem( 1, "Log Messages" ) == -1 )
			AfxMessageBox( "MONITOR2 GH2: Tab Control construction Failed", MB_OK, 0);
		m_MainTab2.SetCurSel(0);
	}
	else if(sel == 3)
	{
		if( !ptetcp_connect3( hname, error_msg ) )
		{
			//Initial connection failed!
			sprintf( buffer, "MONITOR3 MKT1: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			strcpy( hostname_saved3, hname);
			DisableAllButtons(3);

			return 1;//Error
		}
		else
		{
			m_bMonitorUp3 = true;

			memset( hostname_saved3, 0, sizeof( hostname_saved3 ) );
			InitConnectionData3();
		}
		//memset( mon3_name, 0, sizeof( mon3_name ) );
		//sprintf(mon3_name, "MONITOR3 MKT1 - %s", hname);
		//m_Monitor3Name.SetWindowText(mon3_name);

		if ( m_MainTab3.InsertItem( 1, "Network Status" ) == -1 )
			AfxMessageBox( "MONITOR3 MKT1: Tab Control construction Failed", MB_OK, 0);
		if ( m_MainTab3.InsertItem( 1, "Log Messages" ) == -1 )
			AfxMessageBox( "MONITOR3 MKT1: Tab Control construction Failed", MB_OK, 0);
		m_MainTab3.SetCurSel(0);
	}
	else if(sel == 4)
	{
		if( !ptetcp_connect4( hname, error_msg ) )
		{
			//Initial connection failed!
			sprintf( buffer, "MONITOR4 MKT2: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			strcpy( hostname_saved4, hname);
			DisableAllButtons(4);

			return 1;//Error
		}
		else
		{
			m_bMonitorUp4 = true;

			// Initial connection established!
			memset( hostname_saved4, 0, sizeof( hostname_saved4 ) );
			InitConnectionData4();
		}
		//memset( mon4_name, 0, sizeof( mon4_name ) );
		//sprintf(mon4_name, "MONITOR4 MKT2 - %s", hname);
		//m_Monitor4Name.SetWindowText(mon4_name);

		if ( m_MainTab4.InsertItem( 1, "Network Status" ) == -1 )
			AfxMessageBox( "MONITOR4 MKT2: Tab Control construction Failed", MB_OK, 0);
		if ( m_MainTab4.InsertItem( 1, "Log Messages" ) == -1 )
			AfxMessageBox( "MONITOR4 MKT2: Tab Control construction Failed", MB_OK, 0);
		m_MainTab4.SetCurSel(0);
	}

	return 0;//Success
}

void CMonitorView::InitializeMonitorControls(int mon)
{
	INT  nTabStop[2]		= {0,0};
	INT  nDialogTabStop[2]	= {0,0};
	INT  nActTabStop[7]		= {0,0,0,0,0,0,0};
	INT  nLogTabStop[2]		= {0,0};

	CRect rectList, size;
	CDC* dc = GetDC();
	CFont* pOldFont;
	INT ListBoxWidth;

	CRect rectDialogList, dialog_size;
	INT DialogListBoxWidth;

	CRect rectActList, act_size;
	INT ActListBoxWidth;

	if(mon == MONITOR1)
		m_NetworkStatus.GetWindowRect(&rectList);
	else if(mon == MONITOR2)
		m_NetworkStatus2.GetWindowRect(&rectList);
	else if(mon == MONITOR3)
		m_NetworkStatus3.GetWindowRect(&rectList);
	else if(mon == MONITOR4)
		m_NetworkStatus4.GetWindowRect(&rectList);
	size = rectList;
	
	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());
	
	// accurately measure the average width of a character
	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);
	
	//  find the tab stop, being careful with rounding
	ListBoxWidth = ((rectList.Width()*4) / ((25+size.Width())/52));
	
	nTabStop[0] = ( ListBoxWidth * 16 ) / 100;
	nTabStop[1] = ( ListBoxWidth * 47 ) / 100;
	nTabStop[2] = ( ListBoxWidth * 69 ) / 100;
	
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);
	
	if(mon == MONITOR1)
	{
		m_NetworkStatus.SetTabStops(2, nTabStop);
		/*set tab stops for dialog connect list box*/
		m_DialogConnectStatus.GetWindowRect(&rectDialogList);
	}
	else if(mon == MONITOR2)
	{
		m_NetworkStatus2.SetTabStops(2, nTabStop);
		m_DialogConnectStatus2.GetWindowRect(&rectDialogList);
	}
	else if(mon == MONITOR3)
	{
		m_NetworkStatus3.SetTabStops(2, nTabStop);
		m_DialogConnectStatus3.GetWindowRect(&rectDialogList);
	}
	else if(mon == MONITOR4)
	{
		m_NetworkStatus4.SetTabStops(2, nTabStop);
		m_DialogConnectStatus4.GetWindowRect(&rectDialogList);
	}	
	dialog_size = rectDialogList;
	
	//  find the tab stop, being careful with rounding
	DialogListBoxWidth = ((rectDialogList.Width()*4) / ((25+dialog_size.Width())/52));
	nDialogTabStop[0] = ( DialogListBoxWidth * 47) / 100;
	if(mon == MONITOR1)
	{
		m_DialogConnectStatus.SetTabStops(1, nDialogTabStop);
		/*set tab stops for tran activity list box */
		m_NetworkStatus.GetWindowRect(&rectActList);
		m_LogMessages.SetHorizontalExtent(4096);
	}
	else if(mon == MONITOR2)
	{
		m_DialogConnectStatus2.SetTabStops(1, nDialogTabStop);
		m_NetworkStatus2.GetWindowRect(&rectActList);
		m_LogMessages2.SetHorizontalExtent(4096);
	}
	else if(mon == MONITOR3)
	{
		m_DialogConnectStatus3.SetTabStops(1, nDialogTabStop);
		m_NetworkStatus3.GetWindowRect(&rectActList);
		m_LogMessages3.SetHorizontalExtent(4096);
	}
	else if(mon == MONITOR4)
	{
		m_DialogConnectStatus4.SetTabStops(1, nDialogTabStop);
		m_NetworkStatus4.GetWindowRect(&rectActList);
		m_LogMessages4.SetHorizontalExtent(4096);
	}
	act_size = rectActList;
	
	//  find the tab stop, being careful with rounding
	ActListBoxWidth = ((rectActList.Width()*4) / ((25+act_size.Width())/52));	
}

void CMonitorView::InitTabControls()
{
	if(m_bMonitorUp == true)
		m_CntlMuteBeep.SetCheck(_MuteOn);
	if(m_bMonitorUp2 == true)
		m_CntlMuteBeep2.SetCheck(_MuteOn2);
	if(m_bMonitorUp3 == true)
		m_CntlMuteBeep3.SetCheck(_MuteOn3);
	if(m_bMonitorUp4 == true)
		m_CntlMuteBeep4.SetCheck(_MuteOn4);

	//Monitor 1
	m_DialogConnectStatus.ShowWindow(SW_SHOW);
	m_CntlPingDialogButton.ShowWindow(SW_SHOW);
	m_LogMessages.ShowWindow(SW_HIDE);
	m_CntlClearLogButton.ShowWindow(SW_HIDE);
	m_CntlSaveLogButton.ShowWindow(SW_HIDE);
	m_TestResult.ShowWindow(SW_HIDE);
	m_TEST_BUTTON.ShowWindow(SW_HIDE);

	m_DialogIdLabel.ShowWindow(SW_SHOW);
	m_LogStatusLabel.ShowWindow(SW_SHOW);
	m_NameLabel.ShowWindow(SW_SHOW);
	m_LogDateLabel.ShowWindow(SW_HIDE);
	m_LogTimeLabel.ShowWindow(SW_HIDE);
	m_LogTypeLabel.ShowWindow(SW_HIDE);
	m_LogServiceLabel.ShowWindow(SW_HIDE);
	m_LogMessageLabel.ShowWindow(SW_HIDE);

	//Monitor 2
	m_DialogConnectStatus2.ShowWindow(SW_SHOW);
	m_CntlPingDialogButton2.ShowWindow(SW_SHOW);
	m_LogMessages2.ShowWindow(SW_HIDE);
	m_CntlClearLogButton2.ShowWindow(SW_HIDE);
	m_CntlSaveLogButton2.ShowWindow(SW_HIDE);
	m_TestResult2.ShowWindow(SW_HIDE);
	m_TEST_BUTTON2.ShowWindow(SW_HIDE);

	m_DialogIdLabel2.ShowWindow(SW_SHOW);
	m_LogStatusLabel2.ShowWindow(SW_SHOW);
	m_NameLabel2.ShowWindow(SW_SHOW);
	m_LogDateLabel2.ShowWindow(SW_HIDE);
	m_LogTimeLabel2.ShowWindow(SW_HIDE);
	m_LogTypeLabel2.ShowWindow(SW_HIDE);
	m_LogServiceLabel2.ShowWindow(SW_HIDE);
	m_LogMessageLabel2.ShowWindow(SW_HIDE);

	//Monitor3
	m_DialogConnectStatus3.ShowWindow(SW_SHOW);
	m_CntlPingDialogButton3.ShowWindow(SW_SHOW);
	m_LogMessages3.ShowWindow(SW_HIDE);
	m_CntlClearLogButton3.ShowWindow(SW_HIDE);
	m_CntlSaveLogButton3.ShowWindow(SW_HIDE);
	m_TestResult3.ShowWindow(SW_HIDE);
	m_TEST_BUTTON3.ShowWindow(SW_HIDE);

	m_DialogIdLabel3.ShowWindow(SW_SHOW);
	m_LogStatusLabel3.ShowWindow(SW_SHOW);
	m_NameLabel3.ShowWindow(SW_SHOW);
	m_LogDateLabel3.ShowWindow(SW_HIDE);
	m_LogTimeLabel3.ShowWindow(SW_HIDE);
	m_LogTypeLabel3.ShowWindow(SW_HIDE);
	m_LogServiceLabel3.ShowWindow(SW_HIDE);
	m_LogMessageLabel3.ShowWindow(SW_HIDE);

	//Monitor4
	m_DialogConnectStatus4.ShowWindow(SW_SHOW);
	m_CntlPingDialogButton4.ShowWindow(SW_SHOW);
	m_LogMessages4.ShowWindow(SW_HIDE);
	m_CntlClearLogButton4.ShowWindow(SW_HIDE);
	m_CntlSaveLogButton4.ShowWindow(SW_HIDE);
	m_TestResult4.ShowWindow(SW_HIDE);
	m_TEST_BUTTON4.ShowWindow(SW_HIDE);

	m_DialogIdLabel4.ShowWindow(SW_SHOW);
	m_LogStatusLabel4.ShowWindow(SW_SHOW);
	m_NameLabel4.ShowWindow(SW_SHOW);
	m_LogDateLabel4.ShowWindow(SW_HIDE);
	m_LogTimeLabel4.ShowWindow(SW_HIDE);
	m_LogTypeLabel4.ShowWindow(SW_HIDE);
	m_LogServiceLabel4.ShowWindow(SW_HIDE);
	m_LogMessageLabel4.ShowWindow(SW_HIDE);

	m_CntlMuteAll.SetCheck(0);
}

/*************************************************************************************
NAME:               InitConnectionData

DESCRIPTION:        This function initializes the connection data via an applnk
connection that has already been established.
It is used at monitor startup and after a reconnection to
applnk.
INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Johnnie Grgurich (2/5/02) 
*************************************************************************************/
void CMonitorView::InitConnectionData() 
{
	pPTE_MSG    p_msg_out;
	CHAR  buffer[256], error_msg[100];
	MT_USER_INFO sMonitorUserInfo;
	// spawn thread to handle operator/host update messages
	HANDLE hThread;
	DWORD  ThreadId;
	
	strcpy((char *)sMonitorUserInfo.user_name,u_name);
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	EnableDialogButtons(FALSE, FALSE);

	hThread = CreateThread( NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)LogThreadStarter,
		this,
		NULL,
		&ThreadId );
	
	if( hThread == NULL )
	{
		AfxMessageBox("MONITOR1 GH1: Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		return;
	}
	
	// Get Initial network status from dataserver
	//m_DataServerReply = CreateEvent(NULL,FALSE,FALSE,NULL);
	OnRefresh();
	
	// send monitor up message to dataserver
	Sleep( 2000 );	// debug
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_MONITOR_UP,
		0,		// connects only, if this is the first monitor connection.
		//3,	// monitor override (connects this connections, even if there was an earlier connection).
		netds_que_name,
		interactive_que_name,
		(pBYTE) &sMonitorUserInfo,
		sizeof(MT_USER_INFO),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox("MONITOR1 GH1: Insufficient memory to build monitor up message" );
		return;
	}
	
	// Note that we cannot call PtetcpSend here to avoid infinite error loops.
	if( !ptetcp_send( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending Monitor Up message: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
	
	/*Read monitor.ini file for timer interval and list of controllers and dialogs */
	(void)get_ini_info();
	
	/* Send requests to get controller statistics and dialog statuses. */
	send_for_stats();
	/*Set activity counts to zero*/
	InitializeActivityStatsList();
	/*Set Dialog Status defaults*/
	InitializeDialogStatusList();

	m_TEST_BUTTON.EnableWindow(TRUE);
	m_TestResult.EnableWindow(FALSE);
	m_CntlNCStatus.EnableWindow(FALSE);
	
	//Load bitmaps
	m_hTestOff  = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTOFF));
	m_hTesting  = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTING));
	m_hNCGreen	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_GREEN));
	m_hNCRed	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_RED));
	m_hNCYellow	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_YELLOW));

	m_TestResult.SetBitmap( m_hTestOff );
	m_CntlNCStatus.SetBitmap( m_hNCYellow );
	m_Monitor1Name.SetBitmap(m_hMonitor1);

	//m_CntlMuteBeep.SetCheck(_MuteOn);

	//memset( mon1_name, 0, sizeof( mon1_name ) );
	//sprintf(mon1_name, "MONITOR1 GH1 - %s", hname);
	//m_Monitor1Name.SetWindowText(mon1_name);	
	
	// Start a timer for Timer_Interval milliseconds to get periodic status updates
	m_RefreshTimer = SetTimer( Timer, Timer_Interval, NULL );
	if( m_RefreshTimer == 0 )
		AfxMessageBox("MONITOR1 GH1: Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
	_NCStatusTimer = SetTimer( Timer+1, _NCStatus_Beep_Interval, NULL );
	if( _NCStatusTimer == 0 )
		AfxMessageBox("MONITOR1 GH1: Unable to start Network Status Timer", MB_ICONSTOP | MB_OK );
	
}

void CMonitorView::InitConnectionData2() 
{
	pPTE_MSG    p_msg_out;
	CHAR  buffer[256], error_msg[100];
	MT_USER_INFO sMonitorUserInfo;
	// spawn thread to handle operator/host update messages
	HANDLE hThread2;
	DWORD  ThreadId2;
	
	strcpy((char *)sMonitorUserInfo.user_name,u_name);
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	EnableDialogButtons2(FALSE, FALSE);

	hThread2 = CreateThread( NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)LogThreadStarter2,
		this,
		NULL,
		&ThreadId2 );
	
	if( hThread2 == NULL )
	{
		AfxMessageBox("MONITOR2 GH2: Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		return;
	}

	// Get Initial network status from dataserver
	//m_DataServerReply2 = CreateEvent(NULL,FALSE,FALSE,NULL);
	OnRefresh2();
	
	// send monitor up message to dataserver
	Sleep( 2000 );	// debug
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_MONITOR_UP,
		0,		// connects only, if this is the first monitor connection.
		//3,	// monitor override (connects this connections, even if there was an earlier connection).
		netds_que_name,
		interactive_que_name,
		(pBYTE) &sMonitorUserInfo,
		sizeof(MT_USER_INFO),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox("MONITOR2 GH2: Insufficient memory to build monitor up message" );
		return;
	}
	
	// Note that we cannot call PtetcpSend here to avoid infinite error loops.
	if( !ptetcp_send2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending Monitor Up message: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
	
	/*Read monitor.ini file for timer interval and list of controllers and dialogs */
	(void)get_ini_info2();
	
	/* Send requests to get controller statistics and dialog statuses. */
	send_for_stats2();
	/*Set activity counts to zero*/
	InitializeActivityStatsList2();
	/*Set Dialog Status defaults*/
	InitializeDialogStatusList2();
	
	//Initialized in primary monitor, need to make it separate functions

	m_TEST_BUTTON2.EnableWindow(TRUE);
	m_TestResult2.EnableWindow(FALSE);
	m_CntlNCStatus2.EnableWindow(FALSE);
	
	//Load bitmaps
	m_hTestOff2 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTOFF));
	m_hTesting2 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTING));
	
	m_hNCGreen2	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_GREEN));
	m_hNCRed2		= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_RED));
	m_hNCYellow2	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_YELLOW));
	
	m_TestResult2.SetBitmap( m_hTestOff2 );
	m_CntlNCStatus2.SetBitmap( m_hNCYellow2 );
	
	//m_CntlMuteBeep2.SetCheck(_MuteOn2);
	
	// Start a timer for Timer_Interval milliseconds to get periodic status updates
	m_RefreshTimer2 = SetTimer( Timer2, Timer_Interval2, NULL );
	if( m_RefreshTimer2 == 0 )
		AfxMessageBox("MONITOR2 GH2: Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
	_NCStatusTimer2 = SetTimer( Timer2+1, _NCStatus_Beep_Interval2, NULL );
	if( _NCStatusTimer2 == 0 )
		AfxMessageBox("MONITOR2 GH2: Unable to start Network Status Timer", MB_ICONSTOP | MB_OK );
	
}

void CMonitorView::InitConnectionData3() 
{
	pPTE_MSG    p_msg_out;
	CHAR  buffer[256], error_msg[100];
	MT_USER_INFO sMonitorUserInfo;
	// spawn thread to handle operator/host update messages
	HANDLE hThread3;
	DWORD  ThreadId3;
	
	strcpy((char *)sMonitorUserInfo.user_name,u_name);
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	EnableDialogButtons3(FALSE, FALSE);

	hThread3 = CreateThread( NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)LogThreadStarter3,
		this,
		NULL,
		&ThreadId3 );
	
	if( hThread3 == NULL )
	{
		AfxMessageBox("MONITOR3 MKT1: Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		return;
	}

	// Get Initial network status from dataserver
	//m_DataServerReply3 = CreateEvent(NULL,FALSE,FALSE,NULL);
	OnRefresh3();
	
	// send monitor up message to dataserver
	Sleep( 2000 );	// debug
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_MONITOR_UP,
		0,		// connects only, if this is the first monitor connection.
		//3,	// monitor override (connects this connections, even if there was an earlier connection).
		netds_que_name,
		interactive_que_name,
		(pBYTE) &sMonitorUserInfo,
		sizeof(MT_USER_INFO),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox("MONITOR3 MKT1: Insufficient memory to build monitor up message" );
		return;
	}
	
	// Note that we cannot call PtetcpSend here to avoid infinite error loops.
	if( !ptetcp_send3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending Monitor Up message: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
	
	/*Read monitor.ini file for timer interval and list of controllers and dialogs */
	(void)get_ini_info3();
	
	/* Send requests to get controller statistics and dialog statuses. */
	send_for_stats3();
	/*Set activity counts to zero*/
	InitializeActivityStatsList3();
	/*Set Dialog Status defaults*/
	InitializeDialogStatusList3();
	
	//Initialized in primary monitor, need to make it separate functions

	m_TEST_BUTTON3.EnableWindow(TRUE);
	m_TestResult3.EnableWindow(FALSE);
	m_CntlNCStatus3.EnableWindow(FALSE);
	
	//Load bitmaps
	m_hTestOff3 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTOFF));
	m_hTesting3 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTING));
	
	m_hNCGreen3	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_GREEN));
	m_hNCRed3		= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_RED));
	m_hNCYellow3	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_YELLOW));
	
	m_TestResult3.SetBitmap( m_hTestOff3 );
	m_CntlNCStatus3.SetBitmap( m_hNCYellow3 );
	
	//m_CntlMuteBeep3.SetCheck(_MuteOn3);
	
	// Start a timer for Timer_Interval milliseconds to get periodic status updates
	m_RefreshTimer3 = SetTimer( Timer3, Timer_Interval3, NULL );
	if( m_RefreshTimer3 == 0 )
		AfxMessageBox("MONITOR3 MKT1: Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
	_NCStatusTimer3 = SetTimer( Timer3+1, _NCStatus_Beep_Interval3, NULL );
	if( _NCStatusTimer3 == 0 )
		AfxMessageBox("MONITOR3 MKT1: Unable to start Network Status Timer", MB_ICONSTOP | MB_OK );
	
}

void CMonitorView::InitConnectionData4() 
{
	pPTE_MSG    p_msg_out;
	CHAR  buffer[256], error_msg[100];
	MT_USER_INFO sMonitorUserInfo;
	// spawn thread to handle operator/host update messages
	HANDLE hThread4;
	DWORD  ThreadId4;
	
	strcpy((char *)sMonitorUserInfo.user_name,u_name);
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	EnableDialogButtons4(FALSE, FALSE);

	hThread4 = CreateThread( NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)LogThreadStarter4,
		this,
		NULL,
		&ThreadId4 );
	
	if( hThread4 == NULL )
	{
		AfxMessageBox("MONITOR4 MKT2: Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		return;
	}

	// Get Initial network status from dataserver
	//m_DataServerReply4 = CreateEvent(NULL,FALSE,FALSE,NULL);
	OnRefresh4();
	
	// send monitor up message to dataserver
	Sleep( 2000 );	// debug
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_MONITOR_UP,
		0,		// connects only, if this is the first monitor connection.
		//3,	// monitor override (connects this connections, even if there was an earlier connection).
		netds_que_name,
		interactive_que_name,
		(pBYTE) &sMonitorUserInfo,
		sizeof(MT_USER_INFO),
		0 );

	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox("MONITOR4 MKT2: Insufficient memory to build monitor up message" );
		return;
	}
	
	// Note that we cannot call PtetcpSend here to avoid infinite error loops.
	if( !ptetcp_send4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending Monitor Up message: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
	
	/*Read monitor.ini file for timer interval and list of controllers and dialogs */
	(void)get_ini_info4();
	
	/* Send requests to get controller statistics and dialog statuses. */
	send_for_stats4();
	/*Set activity counts to zero*/
	InitializeActivityStatsList4();
	/*Set Dialog Status defaults*/
	InitializeDialogStatusList4();
	
	//Initialized in primary monitor, need to make it separate functions

	m_TEST_BUTTON4.EnableWindow(TRUE);
	m_TestResult4.EnableWindow(FALSE);
	m_CntlNCStatus4.EnableWindow(FALSE);
	
	//Load bitmaps
	m_hTestOff4 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTOFF));
	m_hTesting4 = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTING));
	
	m_hNCGreen4	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_GREEN));
	m_hNCRed4		= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_RED));
	m_hNCYellow4	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_YELLOW));
	
	m_TestResult4.SetBitmap( m_hTestOff4 );
	m_CntlNCStatus4.SetBitmap( m_hNCYellow4 );
	
	//m_CntlMuteBeep4.SetCheck(_MuteOn4);
	
	// Start a timer for Timer_Interval milliseconds to get periodic status updates
	m_RefreshTimer4 = SetTimer( Timer4, Timer_Interval4, NULL );
	if( m_RefreshTimer4 == 0 )
		AfxMessageBox("MONITOR4 MKT2: Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
	_NCStatusTimer4 = SetTimer( Timer4+1, _NCStatus_Beep_Interval4, NULL );
	if( _NCStatusTimer4 == 0 )
		AfxMessageBox("MONITOR4 MKT2: Unable to start Network Status Timer", MB_ICONSTOP | MB_OK );
	
}

/*************************************************************************************
NAME:               PtetcpSend

DESCRIPTION:        This function mimics the library ptetcp_send function with
logic to intercept an auto reconnect and init the connection
data after an auto reconnect. Otherwise, it behaves exactly
like the library function. If the reconnection was successful,
the library error message will be displayed and any local error
logic will be bypassed by returning a succcess code. If the
reconnection attempt failed, then the connection is down and
the "Connection lost" message will override the normal failure
message.
Also, we will first check to see if an initial connection was
established at startup. If not, we need to establish the
connection before we can do the send. If we cannot connect, we
just show the error message and get out.
INPUTS:
p_msg_out	Pointer to message buffer to send
error_msg	Pointer to error message bufer to load
OUTPUTS:
error_msg	Pointer to error message loaded in case of error
RETURNS:           
0	If error occurs
1	If no error, or error and reconnect attempted at a lower level
AUTHOR:             Johnnie Grgurich (2/5/02) 
*************************************************************************************/
INT CMonitorView::PtetcpSend( pPTE_MSG p_msg_out, pCHAR error_msg )
{
	INT return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a send.
	if (strlen(hostname_saved) != 0 )
	{

		if( !ptetcp_connect( hostname_saved, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR1 GH1: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return 1; // Bypass any local error messages to show library failure message.
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved, 0, sizeof( hostname_saved ) );
		InitConnectionData();
	}
	
	return_code = ptetcp_send( p_msg_out, error_msg );
	if( !return_code )
	{
		// An error occurred during send - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return a success code
		// to bypass the higher level error message and allow the connection status message
		// to be displayed.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData();
			sprintf( buffer, "MONITOR1 GH1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because reconnection OK.
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			sprintf( buffer, "MONITOR1 GH1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because attempted reconnection failed.
		}
	}
	return return_code;
}

INT CMonitorView::PtetcpSend2( pPTE_MSG p_msg_out, pCHAR error_msg )
{
	INT return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a send.
	if (strlen(hostname_saved2) != 0 )
	{
		if( !ptetcp_connect2( hostname_saved2, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR2 GH2: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return 1; // Bypass any local error messages to show library failure message.
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved2, 0, sizeof( hostname_saved2 ) );
		InitConnectionData2();
	}
	
	return_code = ptetcp_send2( p_msg_out, error_msg );
	if( !return_code )
	{
		// An error occurred during send - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return a success code
		// to bypass the higher level error message and allow the connection status message
		// to be displayed.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData2();
			sprintf( buffer, "MONITOR2 GH2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because reconnection OK.
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			sprintf( buffer, "MONITOR2 GH2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because attempted reconnection failed.
		}
	}
	return return_code;
}

INT CMonitorView::PtetcpSend3( pPTE_MSG p_msg_out, pCHAR error_msg )
{
	INT return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a send.
	if (strlen(hostname_saved3) != 0 )
	{
		if( !ptetcp_connect3( hostname_saved3, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR3 MKT1: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return 1; // Bypass any local error messages to show library failure message.
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved3, 0, sizeof( hostname_saved3 ) );
		InitConnectionData3();
	}
	
	return_code = ptetcp_send3( p_msg_out, error_msg );
	if( !return_code )
	{
		// An error occurred during send - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return a success code
		// to bypass the higher level error message and allow the connection status message
		// to be displayed.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData3();
			sprintf( buffer, "MONITOR3 MKT1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because reconnection OK.
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			sprintf( buffer, "MONITOR3 MKT1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because attempted reconnection failed.
		}
	}
	return return_code;
}

INT CMonitorView::PtetcpSend4( pPTE_MSG p_msg_out, pCHAR error_msg )
{
	INT return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a send.
	if (strlen(hostname_saved4) != 0 )
	{
		if( !ptetcp_connect4( hostname_saved4, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR4 MKT2: Unable to connect to Applink Server: %s.", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return 1; // Bypass any local error messages to show library failure message.
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved4, 0, sizeof( hostname_saved4 ) );
		InitConnectionData4();
	}
	
	return_code = ptetcp_send4( p_msg_out, error_msg );
	if( !return_code )
	{
		// An error occurred during send - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return a success code
		// to bypass the higher level error message and allow the connection status message
		// to be displayed.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData4();
			sprintf( buffer, "MONITOR4 MKT2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because reconnection OK.
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			sprintf( buffer, "MONITOR4 MKT2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
			return 1; // Bypass any local error messages, because attempted reconnection failed.
		}
	}
	return return_code;
}

/*************************************************************************************
NAME:               PtetcpReceive

  DESCRIPTION:        This function mimics the library ptetcp_receive function with
  logic to intercept an auto reconnect and init the connection
  data after an auto reconnect. Otherwise, it behaves exactly
  like the library function. If the reconnection was successful,
  the library error message will be displayed and any local error
  logic will be bypassed by returning a succcess code.
  Also, we will first check to see if an initial connection was
  established at startup. If not, we need to establish the
  connection before we can do the receive. If we cannot connect, we
  just show the error message and get out.
  INPUTS:
  time_out		Time out value for blocked read
  error_msg	Pointer to error message bufer to load
  OUTPUTS:
  error_msg	Pointer to error message loaded in case of error
  RETURNS:           
  NULL_PTR		If error occurs
  pPTE_MSG		If no error, or error and reconnect successful at a lower level
  AUTHOR:             Johnnie Grgurich (2/5/02) 
*************************************************************************************/
pPTE_MSG CMonitorView::PtetcpReceive( LONG time_out, pCHAR error_msg )
{
	pPTE_MSG return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a receive.
	if (strlen(hostname_saved) != 0 )
	{
		if( !ptetcp_connect( hostname_saved, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR1 GH1: Unable to connect to Applink Server: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return NULL_PTR; // Return falure condition
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved, 0, sizeof( hostname_saved ) );
		InitConnectionData();
	}
	
	return_code = ptetcp_receive( time_out, error_msg );
	if( NULL_PTR == return_code )
	{
		// An error occurred during receive - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return the NULL code
		// to allow the read operation to be aborted since we have no message.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData();
			sprintf( buffer, "MONITOR1 GH1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			m_bMonitorUp = false;
			sprintf( buffer, "MONITOR1 GH1: %s.\nNOTE: GH1 server might be down please verify and relaunch the DCO Monitor", error_msg );
			MessageBox( buffer ); // Display library error message
			DisableAllButtons(MONITOR1);
		}
	}
	return return_code;
}

pPTE_MSG CMonitorView::PtetcpReceive2( LONG time_out, pCHAR error_msg )
{
	pPTE_MSG return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a receive.
	if (strlen(hostname_saved2) != 0 )
	{
		if( !ptetcp_connect2( hostname_saved2, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR2 GH2: Unable to connect to Applink Server: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return NULL_PTR; // Return falure condition
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved2, 0, sizeof( hostname_saved2 ) );
		InitConnectionData2();
	}
	
	return_code = ptetcp_receive2( time_out, error_msg );
	if( NULL_PTR == return_code )
	{
		// An error occurred during receive - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return the NULL code
		// to allow the read operation to be aborted since we have no message.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData2();
			sprintf( buffer, "MONITOR2 GH2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			m_bMonitorUp2 = false;
			sprintf( buffer, "MONITOR2 GH2: %s.\nNOTE: GH2 might be down please verify and relaunch the DCO Monitor.", error_msg );
			MessageBox( buffer ); // Display library error message
			DisableAllButtons(MONITOR2);
		}
	}
	return return_code;
}

pPTE_MSG CMonitorView::PtetcpReceive3( LONG time_out, pCHAR error_msg )
{
	pPTE_MSG return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a receive.
	if (strlen(hostname_saved3) != 0 )
	{
		if( !ptetcp_connect3( hostname_saved3, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR3 MKT1: Unable to connect to Applink Server: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return NULL_PTR; // Return falure condition
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved3, 0, sizeof( hostname_saved3 ) );
		InitConnectionData3();
	}
	
	return_code = ptetcp_receive3( time_out, error_msg );
	if( NULL_PTR == return_code )
	{
		// An error occurred during receive - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return the NULL code
		// to allow the read operation to be aborted since we have no message.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData3();
			sprintf( buffer, "MONITOR3 MKT1: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			m_bMonitorUp3 = false;
			sprintf( buffer, "MONITOR3 MKT1: %s.\nNOTE: MKT1 might be down please verify the server and relaunch the DCO Monitor.", error_msg );
			MessageBox( buffer ); // Display library error message
			DisableAllButtons(MONITOR3);
		}
	}
	return return_code;
}

pPTE_MSG CMonitorView::PtetcpReceive4( LONG time_out, pCHAR error_msg )
{
	pPTE_MSG return_code;
	CHAR  buffer[256];
	
	// First check to see if we connected to applnk OK on startup. If not, we need to do an
	// initial connection to applnk instead of a receive.
	if (strlen(hostname_saved4) != 0 )
	{
		if( !ptetcp_connect4( hostname_saved, error_msg ) )
		{
			//Initial connection attempt failed again - display error and return
			sprintf( buffer, "MONITOR4 MKT2: Unable to connect to Applink Server: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			return NULL_PTR; // Return falure condition
		}
		// Need to clear the hostname_saved string to indicate initial connection OK.
		// Also, established connection OK, so now initialize the connection data & do the send.
		memset( hostname_saved4, 0, sizeof( hostname_saved4 ) );
		InitConnectionData4();
	}
	
	return_code = ptetcp_receive4( time_out, error_msg );
	if( NULL_PTR == return_code )
	{
		// An error occurred during receive - intercept an auto reconnect attempt
		// The error_msg contents will have the word "Reconnect" in it for a successful
		// reconnect or the word "lost" in it for an attempt that failed. If successful,
		// we will init the new connection. In either case, we will return the NULL code
		// to allow the read operation to be aborted since we have no message.
		if( NULL != strstr( error_msg, "Reconnect") )
		{
			InitConnectionData4();
			sprintf( buffer, "MONITOR4 MKT2: %s.", error_msg );
			MessageBox( buffer ); // Display library error message
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			m_bMonitorUp4 = false;
			sprintf( buffer, "MONITOR4 MKT2: %s.\nNOTE: MKT2 might be down please verify and relaunch the DCO Monitor.", error_msg );
			MessageBox( buffer ); // Display library error message
			DisableAllButtons(MONITOR4);
		}
	}
	return return_code;
}
/*************************************************************************************
NAME:               OnClearlog

  DESCRIPTION:        This function clears the log on the monitor screen..
  
	INPUTS:
	None
	OUTPUTS:
	None
	RETURNS:           
	None
	AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnClearlog() 
{
	// TODO: Add your control notification handler code here
	
	// get confirmation from user
	INT ret = AfxMessageBox("MONITOR1 GH1: Are you sure to clear the logs?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	
	//    m_LogMessages.DeleteAllItems();
	m_LogMessages.ResetContent();
}

void CMonitorView::OnClearlog2() 
{
	// TODO: Add your control notification handler code here
	
	// get confirmation from user
	INT ret = AfxMessageBox("MONITOR2 GH2: Are you sure to clear the logs?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	
	//    m_LogMessages2.DeleteAllItems();
	m_LogMessages2.ResetContent();
}

void CMonitorView::OnClearlog3() 
{
	// TODO: Add your control notification handler code here
	
	// get confirmation from user
	INT ret = AfxMessageBox("MONITOR3 MKT1: Are you sure to clear the logs?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	
	//    m_LogMessages3.DeleteAllItems();
	m_LogMessages3.ResetContent();
}

void CMonitorView::OnClearlog4() 
{
	// TODO: Add your control notification handler code here
	
	// get confirmation from user
	INT ret = AfxMessageBox("MONITOR4 MKT2: Are you sure to clear the logs?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	
	//    m_LogMessages4.DeleteAllItems();
	m_LogMessages4.ResetContent();
}

/*************************************************************************************
NAME:               OnClose

  DESCRIPTION:        This function is called when the close button is pressed.
  It sends a message to trands that it is shutting down before
  it actually does so.
  
	INPUTS:
	None
	OUTPUTS:
	None
	RETURNS:           
	None
	AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnCloseBtn() 
{
	CHAR  buffer[256] ={0};
	CHAR error_msg[100] = {0};
	CHAR handle[256] = {0};
	pPTE_MSG    p_msg_out;
	CString szAutoSaveFile;

	CTime time = CTime::GetCurrentTime();

	// send monitor down message to dataserver
	if(m_bMonitorUp)
	{
		m_bMonitorUp = false;

		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_MONITOR_DOWN,
			0,
			netds_que_name,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );

		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox("MONITOR1 GH1: Insufficient memory to build Monitor down message" );
			ptetcp_disconnect();
			Sleep( 100 );
			GetParentFrame()->DestroyWindow() ;
			return;
		}
	
		if( !PtetcpSend( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR1 GH1: Communication error sending Monitor down message: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		}
		free( p_msg_out );
		ptetcp_disconnect();
	}

	//Monitor 2
	if(m_bMonitorUp2)
	{
		m_bMonitorUp2 = false;

		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_MONITOR_DOWN,
			0,
			netds_que_name,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );

		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox("MONITOR2 GH2: Insufficient memory to build Monitor down message" );
			ptetcp_disconnect2();
			Sleep( 100 );
			GetParentFrame()->DestroyWindow() ;
			return;
		}
	
		if( !PtetcpSend2( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR2 GH2: Communication error sending Monitor down message: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		}
		free( p_msg_out );
		ptetcp_disconnect2();
	}

	//Monitor 3
	if(m_bMonitorUp3)
	{
		m_bMonitorUp3 = false;

		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_MONITOR_DOWN,
			0,
			netds_que_name,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );

		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox("MONITOR3 MKT1: Insufficient memory to build Monitor down message" );
			ptetcp_disconnect3();
			Sleep( 100 );
			GetParentFrame()->DestroyWindow() ;
			return;
		}
	
		if( !PtetcpSend3( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR3 MKT1: Communication error sending Monitor down message: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		}
		free( p_msg_out );
		ptetcp_disconnect3();
	}

	//Monitor 4
	if(m_bMonitorUp4)
	{
		m_bMonitorUp4 = false;

		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_MONITOR_DOWN,
			0,
			netds_que_name,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox("MONITOR4 MKT2: Insufficient memory to build Monitor down message" );
			ptetcp_disconnect4();
			Sleep( 100 );
			GetParentFrame()->DestroyWindow() ;
			return;
		}
	
		if( !PtetcpSend4( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR4 MKT2: Communication error sending Monitor down message: %s", error_msg );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		}
		free( p_msg_out );
		ptetcp_disconnect4();
	}

	szAutoSaveFile.Empty();
	szAutoSaveFile.Format("dco_monitor_auto_save_%s.log", time.Format("%m%d%Y%H%M%S"));
	SaveLogIntoFile(szAutoSaveFile);

	/*kill timer*/
	/*  bool m_KillTimer = 0;
	if ( KillTimer(Timer))
	AfxMessageBox("Timer Killed", MB_ICONSTOP | MB_OK);
	else
	AfxMessageBox("Timer Kill Failed", MB_ICONSTOP | MB_OK);*/
	
	Sleep( 5000 );
	GetParentFrame()->DestroyWindow() ;
}


/*************************************************************************************
NAME:               OnEchotest

  DESCRIPTION:        This function is called when the EchoTest button is pressed.
  It sends a appropriate message to the selected network handler.
  
	INPUTS:
	None
	OUTPUTS:
	None
	RETURNS:           
	None
	AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnEchotest() 
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0;	/*index to local ncf01 table*/
	INT       cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
	//m_NetworkStatus.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table [table_index].logon_bin,
        sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data, 0x00, sizeof(m_msg_data) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data, &m_auth_tx, sizeof( m_auth_tx ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_ECHOTEST,
		0,
		(pCHAR)m_ncf01_local_table [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data,
		sizeof( m_msg_data ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending EchoTest Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}	
	free( p_msg_out );
}



/*************************************************************************************
NAME:               OnLogoff

DESCRIPTION:        This function is called when the Logoff button is pressed.
It sends a appropriate message to the selected network handler.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnLogoff() 
{
	ProcessOFFMessage(ST1_LOGOFF);
	
}

void CMonitorView::ProcessOFFMessage(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(st1 == ST1_ACQUIRER_OFF)
	{
		str.Format("MONITOR1 GH1: Are you sure to Acquirer off for %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else  if(st1 == ST1_ACQUIRER_ON)
	{
		str.Format("MONITOR1 GH1: Are you sure to Acquirer on %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else
	{
		str.Format("MONITOR1 GH1: Are you sure to logoff %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	//m_NetworkStatus.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data, 0x00, sizeof(m_msg_data) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data, &m_auth_tx, sizeof( m_auth_tx ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data,
		sizeof( m_msg_data ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}


/*************************************************************************************
NAME:               OnLogon

DESCRIPTION:        This function is called when the Logon button is pressed.
It sends a appropriate message to the selected network handler.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnLogon() 
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	str.Format("MONITOR1 GH1: Are you sure to logon %s?",str_network_id);

	INT ret = AfxMessageBox(str , MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;

	//m_NetworkStatus.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data, 0x00, sizeof(m_msg_data) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data, &m_auth_tx, sizeof( m_auth_tx ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_LOGON,
		0,
		(pCHAR)m_ncf01_local_table [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data,
		sizeof( m_msg_data ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending Logon Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}		
	free( p_msg_out );
}

/*************************************************************************************
NAME:               OnRefresh

DESCRIPTION:        This function is called when the Refresh button is pressed.
It updates the network status list retrieving it from trands.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnRefresh() 
{
	// TODO: Add your control notification handler code here
	m_curr_index = 0;
	memset( &m_ncf01_local_table, 0, sizeof( m_ncf01_local_table ) );
	memset( &m_ncf01, 0, sizeof( NCF01 ) );
	strcpy( (pCHAR)m_ncf01.primary_key.network_id, "      " );
	strcpy( (pCHAR)m_ncf01.primary_key.network_type, " " );
	
	m_NetworkStatus.ResetContent();
	m_NetworkStatus.UpdateWindow();
	m_retrieval_done = 0;
	GetNCF01List();
	//BeginWaitCursor();
	/*while( !m_retrieval_done )
	{
		if( WaitForSingleObject( m_DataServerReply, 5000 ) != WAIT_OBJECT_0 )
		{
			EndWaitCursor();
			AfxMessageBox( "Timed out getting Network Status from DataServer for Monitor 1", MB_ICONSTOP | MB_OK  );
			return;
		}
	}*/
	//EndWaitCursor();
}

void CMonitorView::OnRefresh2() 
{
	// TODO: Add your control notification handler code here
	m_curr_index2 = 0;
	memset( &m_ncf01_local_table2, 0, sizeof( m_ncf01_local_table2 ) );
	memset( &m_ncf012, 0, sizeof( NCF01 ) );
	strcpy( (pCHAR)m_ncf012.primary_key.network_id, "      " );
	strcpy( (pCHAR)m_ncf012.primary_key.network_type, " " );
	
	m_NetworkStatus2.ResetContent();
	m_NetworkStatus2.UpdateWindow();
	m_retrieval_done2 = 0;
	GetNCF01List2();
	//BeginWaitCursor();
	/*while( !m_retrieval_done2 )
	{
		if( WaitForSingleObject( m_DataServerReply2, 5000 ) != WAIT_OBJECT_0 )
		{
			EndWaitCursor();
			AfxMessageBox( "Timed out getting Network Status from DataServer Monitor 2", MB_ICONSTOP | MB_OK  );
			return;
		}
	}*/
	//EndWaitCursor();
}

void CMonitorView::OnRefresh3() 
{
	// TODO: Add your control notification handler code here
	m_curr_index3 = 0;
	memset( &m_ncf01_local_table3, 0, sizeof( m_ncf01_local_table3 ) );
	memset( &m_ncf013, 0, sizeof( NCF01 ) );
	strcpy( (pCHAR)m_ncf013.primary_key.network_id, "      " );
	strcpy( (pCHAR)m_ncf013.primary_key.network_type, " " );
	
	m_NetworkStatus3.ResetContent();
	m_NetworkStatus3.UpdateWindow();
	m_retrieval_done3 = 0;
	GetNCF01List3();
	//BeginWaitCursor();
	/*while( !m_retrieval_done3 )
	{
		if( WaitForSingleObject( m_DataServerReply3, 5000 ) != WAIT_OBJECT_0 )
		{
			EndWaitCursor();
			AfxMessageBox( "Timed out getting Network Status from DataServer Monitor 3", MB_ICONSTOP | MB_OK  );
			return;
		}
	}*/
	//EndWaitCursor();
}

void CMonitorView::OnRefresh4() 
{
	// TODO: Add your control notification handler code here
	m_curr_index4 = 0;
	memset( &m_ncf01_local_table4, 0, sizeof( m_ncf01_local_table4 ) );
	memset( &m_ncf014, 0, sizeof( NCF01 ) );
	strcpy( (pCHAR)m_ncf014.primary_key.network_id, "      " );
	strcpy( (pCHAR)m_ncf014.primary_key.network_type, " " );
	
	m_NetworkStatus4.ResetContent();
	m_NetworkStatus4.UpdateWindow();
	m_retrieval_done4 = 0;
	GetNCF01List4();
	//BeginWaitCursor();
	/*while( !m_retrieval_done4 )
	{
		if( WaitForSingleObject( m_DataServerReply4, 5000 ) != WAIT_OBJECT_0 )
		{
			EndWaitCursor();
			AfxMessageBox( "Timed out getting Network Status from DataServer Monitor 4", MB_ICONSTOP | MB_OK  );
			return;
		}
	}*/
	//EndWaitCursor();
}

/*************************************************************************************
NAME:               OnSavelog

DESCRIPTION:        This function is called when the Savelog button is pressed.
It prompts the user for a location and file name and saves
the log to the file.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::OnSavelog() 
{
	// TODO: Add your control notification handler code here
	CHAR log_filename[256]= {0};
	CString filename;
	char displayhostname[50] = {0};
	char filenamewithhostname[256] = {0};
	FILE *log_fp;
	CHAR temp_str[256], buffer[256];
	INT num_messages = 0;
	char szCurDir[200] = {0};
	char handle[256] = {0};


	
	/*GetPinnacleLogDirectory( log_filename );
	FileDlg.m_ofn.lpstrInitialDir = log_filename;*/
	/*sprintf(handle, "%p", this->m_hWnd);

	if(strcmp(handle,monitor1handle) == 0)
	{
		strcpy(displayhostname,HostName1);
	}
	else if(strcmp(handle,monitor2handle) == 0)
	{
		strcpy(displayhostname,HostName2);
	}
	else if(strcmp(handle,monitor3handle) == 0)
	{
		strcpy(displayhostname,HostName3);
	}
	else
	{
		strcpy(displayhostname,HostName4);
	}*/

	strcpy(displayhostname,HostName1);
	sprintf(filenamewithhostname,"monitor1_%s.log",displayhostname);

	CFileDialog FileDlg( FALSE, "log", filenamewithhostname );
	GetCurrentDirectory(200, szCurDir);
	INT ret = FileDlg.DoModal();


	if( ret == IDOK )
	{
		//filename = FileDlg.GetFileName();
		//strcat( log_filename, filename.GetBuffer( 100 ) );

		// Below two lines are used to create the monitor log file in the required path.
		SetCurrentDirectory(szCurDir);
		CString filename = FileDlg.GetPathName();

		// try to open the file
		log_fp = fopen( filename, "w+" );
		if( log_fp == NULL )
		{
			sprintf( temp_str, "MONITOR1 GH1: Unable to open file %s", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		} 
		
		// now save the log to the file, writing one line at a time
		num_messages = m_LogMessages.GetCount();    //GetItemCount();
		for( INT item = 0; item < num_messages; item++ )
		{
			memset( buffer, 0, sizeof( buffer ) );
			memset (temp_str, 0x00, sizeof(temp_str));
			
			m_LogMessages.GetText( item, temp_str );
			memcpy(buffer, temp_str, sizeof(temp_str) );
			strcat( buffer, "\n" );
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		
		// close the log file and return
		fclose( log_fp );
	}
}

void CMonitorView::OnSavelog2() 
{
	// TODO: Add your control notification handler code here
	CHAR log_filename[256]= {0};
	CString filename;
	char displayhostname[50] = {0};
	char filenamewithhostname[256] = {0};
	FILE *log_fp;
	CHAR temp_str[256], buffer[256];
	INT num_messages = 0;
	char szCurDir[200] = {0};
	char handle[256] = {0};

	/*GetPinnacleLogDirectory( log_filename );
	FileDlg.m_ofn.lpstrInitialDir = log_filename;*/

	strcpy(displayhostname,HostName2);
	sprintf(filenamewithhostname,"monitor2_%s.log",displayhostname);

	CFileDialog FileDlg( FALSE, "log", filenamewithhostname );
	GetCurrentDirectory(200, szCurDir);
	INT ret = FileDlg.DoModal();

	if( ret == IDOK )
	{
		//filename = FileDlg.GetFileName();
		//strcat( log_filename, filename.GetBuffer( 100 ) );

		// Below two lines are used to create the monitor log file in the required path.
		SetCurrentDirectory(szCurDir);
		CString filename = FileDlg.GetPathName();

		// try to open the file
		log_fp = fopen( filename, "w+" );
		if( log_fp == NULL )
		{
			sprintf( temp_str, "MONITOR2 GH2: Unable to open file %s", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		} 
		
		// now save the log to the file, writing one line at a time
		num_messages = m_LogMessages2.GetCount();    //GetItemCount();
		for( INT item = 0; item < num_messages; item++ )
		{
			memset( buffer, 0, sizeof( buffer ) );
			memset (temp_str, 0x00, sizeof(temp_str));
			
			m_LogMessages2.GetText( item, temp_str );
			memcpy(buffer, temp_str, sizeof(temp_str) );
			strcat( buffer, "\n" );
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		
		// close the log file and return
		fclose( log_fp );
	}
}

void CMonitorView::OnSavelog3() 
{
	// TODO: Add your control notification handler code here
	CHAR log_filename[256]= {0};
	CString filename;
	char displayhostname[50] = {0};
	char filenamewithhostname[256] = {0};
	FILE *log_fp;
	CHAR temp_str[256], buffer[256];
	INT num_messages = 0;
	char szCurDir[200] = {0};
	char handle[256] = {0};

	/*GetPinnacleLogDirectory( log_filename );
	FileDlg.m_ofn.lpstrInitialDir = log_filename;*/

	strcpy(displayhostname,HostName3);
	sprintf(filenamewithhostname,"monitor3_%s.log",displayhostname);

	CFileDialog FileDlg( FALSE, "log", filenamewithhostname );
	GetCurrentDirectory(200, szCurDir);
	INT ret = FileDlg.DoModal();

	if( ret == IDOK )
	{
		//filename = FileDlg.GetFileName();
		//strcat( log_filename, filename.GetBuffer( 100 ) );

		// Below two lines are used to create the monitor log file in the required path.
		SetCurrentDirectory(szCurDir);
		CString filename = FileDlg.GetPathName();

		// try to open the file
		log_fp = fopen( filename, "w+" );
		if( log_fp == NULL )
		{
			sprintf( temp_str, "MONITOR3 MKT1: Unable to open file %s", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		} 
		
		// now save the log to the file, writing one line at a time
		num_messages = m_LogMessages3.GetCount();    //GetItemCount();
		for( INT item = 0; item < num_messages; item++ )
		{
			memset( buffer, 0, sizeof( buffer ) );
			memset (temp_str, 0x00, sizeof(temp_str));
			
			m_LogMessages3.GetText( item, temp_str );
			memcpy(buffer, temp_str, sizeof(temp_str) );
			strcat( buffer, "\n" );
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		
		// close the log file and return
		fclose( log_fp );
	}
}

void CMonitorView::OnSavelog4() 
{
	// TODO: Add your control notification handler code here
	CHAR log_filename[256]= {0};
	CString filename;
	char displayhostname[50] = {0};
	char filenamewithhostname[256] = {0};
	FILE *log_fp;
	CHAR temp_str[256], buffer[256];
	INT num_messages = 0;
	char szCurDir[200] = {0};
	char handle[256] = {0};

	/*GetPinnacleLogDirectory( log_filename );
	FileDlg.m_ofn.lpstrInitialDir = log_filename;*/

	strcpy(displayhostname,HostName4);
	sprintf(filenamewithhostname,"monitor4_%s.log",displayhostname);

	CFileDialog FileDlg( FALSE, "log", filenamewithhostname );
	GetCurrentDirectory(200, szCurDir);
	INT ret = FileDlg.DoModal();

	if( ret == IDOK )
	{
		//filename = FileDlg.GetFileName();
		//strcat( log_filename, filename.GetBuffer( 100 ) );

		// Below two lines are used to create the monitor log file in the required path.
		SetCurrentDirectory(szCurDir);
		CString filename = FileDlg.GetPathName();

		// try to open the file
		log_fp = fopen( filename, "w+" );
		if( log_fp == NULL )
		{
			sprintf( temp_str, "MONITOR4 MKT2: Unable to open file %s", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		} 
		
		// now save the log to the file, writing one line at a time
		num_messages = m_LogMessages4.GetCount();    //GetItemCount();
		for( INT item = 0; item < num_messages; item++ )
		{
			memset( buffer, 0, sizeof( buffer ) );
			memset (temp_str, 0x00, sizeof(temp_str));
			
			m_LogMessages4.GetText( item, temp_str );
			memcpy(buffer, temp_str, sizeof(temp_str) );
			strcat( buffer, "\n" );
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		
		// close the log file and return
		fclose( log_fp );
	}
}

/*************************************************************************************
NAME:               GetNCF01List

DESCRIPTION:        This function sends a request to trands to retrieve the
NCF01 table.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::GetNCF01List() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	char        list_buffer[sizeof(AUTH_TX) + sizeof (NCF01)];

	//ResetEvent( m_DataServerReply );
	memcpy ((list_buffer + sizeof(AUTH_TX)), &m_ncf01, sizeof (NCF01));    
	
	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
		ST1_DB_GET_GUI_LIST,
		0,
		netds_que_name,  /*netsdA*/
		interactive_que_name,
		(pBYTE)list_buffer,
		sizeof( list_buffer ),
		NCF01_DATA );
	if( p_msg_out == NULL_PTR )
	{
		MessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending Request to dataserver: %s", error_msg );
		MessageBox( buffer );
	}	
	
	free( p_msg_out );
}

void CMonitorView::GetNCF01List2() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	char        list_buffer[sizeof(AUTH_TX) + sizeof (NCF01)];
	
	//ResetEvent( m_DataServerReply2 );
	memcpy ((list_buffer + sizeof(AUTH_TX)), &m_ncf012, sizeof (NCF01));     

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
		ST1_DB_GET_GUI_LIST,
		0,
		netds_que_name,  /*netsdA*/
		interactive_que_name,
		(pBYTE)list_buffer,
		sizeof( list_buffer ),
		NCF01_DATA );
	if( p_msg_out == NULL_PTR )
	{
		MessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending Request to dataserver: %s", error_msg );
		MessageBox( buffer );
	}	
	
	free( p_msg_out );
}

void CMonitorView::GetNCF01List3() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	char        list_buffer[sizeof(AUTH_TX) + sizeof (NCF01)];
	
	//ResetEvent( m_DataServerReply3 );
	memcpy ((list_buffer + sizeof(AUTH_TX)), &m_ncf013, sizeof (NCF01));     

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
		ST1_DB_GET_GUI_LIST,
		0,
		netds_que_name,  /*netsdA*/
		interactive_que_name,
		(pBYTE)list_buffer,
		sizeof( list_buffer ),
		NCF01_DATA );

	if( p_msg_out == NULL_PTR )
	{
		MessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending Request to dataserver: %s", error_msg );
		MessageBox( buffer );
	}	
	
	free( p_msg_out );
}

void CMonitorView::GetNCF01List4() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	char        list_buffer[sizeof(AUTH_TX) + sizeof (NCF01)];
	
	//ResetEvent( m_DataServerReply4 );
	memcpy ((list_buffer + sizeof(AUTH_TX)), &m_ncf014, sizeof (NCF01));     

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
		ST1_DB_GET_GUI_LIST,
		0,
		netds_que_name,  /*netsdA*/
		interactive_que_name,
		(pBYTE)list_buffer,
		sizeof( list_buffer ),
		NCF01_DATA );

	if( p_msg_out == NULL_PTR )
	{
		MessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending Request to dataserver: %s", error_msg );
		MessageBox( buffer );
	}	
	
	free( p_msg_out );
}

/******************************************************************************
NAME:               PopulateNetworkStatusList

DESCRIPTION:        This function populates the list box with the latest
NCF01 table

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*******************************************************************************/
void CMonitorView::PopulateNetworkStatusList()
{
	INT   status_color;
	CHAR  display_buff[512] = "";
	CHAR  status_str[20]    = "";
	CHAR  network_id[11]    = "";
	CHAR	 network_name[30] = {0};
	
	/* 'states' contains the sort order, where
	*     0 = online (also 4 and 5)
	*     1 = offline
	*     2 = querying
	*     3 = down
	*/
	CHAR  states[] = {"130452"}; /* Offline first, Querying last. */
	
	INT   found;
	INT   entry;
	INT   state; /* Index for 'states'. */
	INT   num_sorted = 0;
	INT   sort_stat_idx;
	INT   index;
	
	typedef struct
	{
		CHAR  network_id[11];
		CHAR  status[2];
		CHAR	network_name[30];
	} NET_SORT_STATUS;
	
	NET_SORT_STATUS  net_status_list[MAX_NETWORKS];
	
	
	/* Initialize status list */
	memset( &net_status_list, 0x00, sizeof(NET_SORT_STATUS)*MAX_NETWORKS );
	
	/* Initialize the window. */
	m_NetworkStatus.ResetContent();
	m_NetworkStatus.UpdateWindow();
	
	/* Loop through each state. */
	for ( state=0; state<6; state++ )
	{
	/* Loop through the main network id list, picking out
	* those whose states match the current state index.
		*/
		for ( index=0; index<m_curr_index; index++ )
		{
			if ( m_ncf01_local_table[index].status[0] == states[state] )
			{
			/* This network id is set to the state we are sorting.
			* Has it already been sorted? (Remember there are 2
			* network types (I and A).  Don't do both.
				*/
				found = false;
				strcpy( network_id,
					(pCHAR)m_ncf01_local_table[index].primary_key.network_id );
				
				strcpy(network_name, (pCHAR)m_ncf01_local_table[index].name);
				
				for ( sort_stat_idx=0; sort_stat_idx<num_sorted; sort_stat_idx++ )
				{
					if ( 0 == strcmp(net_status_list[sort_stat_idx].network_id, network_id) )
					{
						found = true;
						break;
					}
				}
				
				if ( found == false )
				{
					/* This network id has not been sorted yet.  Put it in list. */
					strcpy(net_status_list[num_sorted].network_name, network_name);
					strcpy( net_status_list[num_sorted].network_id, network_id );
					net_status_list[num_sorted].status[0] = states[state];
					num_sorted++;
				}
			}
		}
	}
	
	nc_status=GREEN;
	
	/* Now populate the display window. */
	for ( index=0; index<m_curr_index; index++ )
	{
		entry = true;
		switch( net_status_list[index].status[0] )
		{
		case '0':
		case '4':
		case '5':
			strcpy( status_str, "Online" );
			status_color = BLACK;
			break;
			
		case '1':
			strcpy( status_str, "Offline" );
			status_color = RED;
			nc_status = RED;
			break;
			
		case '2':
			strcpy( status_str, "Querying" );
			status_color = BLACK;
			if(nc_status != RED)
				nc_status = YELLOW;
			break;
			
		case '3':
			strcpy( status_str, "Down" );
			status_color = RED;
			nc_status = RED;
			break;
			
		default:
			entry = false;
			break;
		}
		
		if ( entry == true )
		{
			//if(strlen(net_status_list[index].network_id) == 0)
			CString str1 = net_status_list[index].network_id;
			str1.MakeUpper();
			if(str1.Find('N') < 0)
			{
				/*sprintf( display_buff, "%d %d\t\t\t%s",
				m_curr_index,num_rows_returned,
					status_str );*/
			}
			else
			{
				if(strcmpi(net_status_list[index].network_id,"ncmcrd") == 0)
				{
					if(strcmpi(status_str,"Offline") == 0)
					{
						m_CntlAcqOff.SetWindowText("ACQ ON");
						m_bAcqOn = true;
					}
					else
					{
						m_CntlAcqOff.SetWindowText("ACQ OFF");
						m_bAcqOn = false;
					}
				}
				m_CntlNetTranList.UpdateItemText( net_status_list[index].network_id, LS_STATUS, status_str);
				m_CntlNetTranList.UpdateItemText( net_status_list[index].network_id, LS_NETWORK_NAME, net_status_list[index].network_name);
				if(status_color == RED)
				{
					m_CntlNetTranList.SetRed(net_status_list[index].network_id, true);
				}
				else
				{
					m_CntlNetTranList.SetRed(net_status_list[index].network_id, false);
				}

				sprintf( display_buff, "%s\t\t%-13s\t%s",
					net_status_list[index].network_id, status_str, net_status_list[index].network_name );
			}
			
			/* A status of offline or down will be displayed in red. */
			if ( status_color == BLACK )
			{
			//	m_NetworkStatus.AddString(display_buff); /* black */
			}
			else
			{
			//	m_NetworkStatus.AddString(display_buff,RGB(255,0,0)); /* red */
			}
		}
	}
	
	switch(nc_status)
	{
	case GREEN:
		m_CntlNCStatus.SetBitmap( m_hNCGreen );
		break;
	case YELLOW:
		m_CntlNCStatus.SetBitmap( m_hNCYellow );
		break;
	default:
		m_CntlNCStatus.SetBitmap( m_hNCRed);
		if(!m_CntlMuteBeep.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
			Sleep(500);
		}
		
		break;
	}
	
	return;
}

void CMonitorView::PopulateNetworkStatusList2()
{
	INT   status_color;
	CHAR  display_buff[512] = "";
	CHAR  status_str[20]    = "";
	CHAR  network_id[11]    = "";
	CHAR	 network_name[30] = {0};
	
	/* 'states' contains the sort order, where
	*     0 = online (also 4 and 5)
	*     1 = offline
	*     2 = querying
	*     3 = down
	*/
	CHAR  states[] = {"130452"}; /* Offline first, Querying last. */
	
	INT   found;
	INT   entry;
	INT   state; /* Index for 'states'. */
	INT   num_sorted = 0;
	INT   sort_stat_idx;
	INT   index;
	
	typedef struct
	{
		CHAR  network_id[11];
		CHAR  status[2];
		CHAR	network_name[30];
	} NET_SORT_STATUS;
	
	NET_SORT_STATUS  net_status_list[MAX_NETWORKS];
	
	
	/* Initialize status list */
	memset( &net_status_list, 0x00, sizeof(NET_SORT_STATUS)*MAX_NETWORKS );
	
	/* Initialize the window. */
	m_NetworkStatus2.ResetContent();
	m_NetworkStatus2.UpdateWindow();
	
	/* Loop through each state. */
	for ( state=0; state<6; state++ )
	{
	/* Loop through the main network id list, picking out
	* those whose states match the current state index.
		*/
		for ( index=0; index<m_curr_index2; index++ )
		{
			if ( m_ncf01_local_table2[index].status[0] == states[state] )
			{
			/* This network id is set to the state we are sorting.
			* Has it already been sorted? (Remember there are 2
			* network types (I and A).  Don't do both.
				*/
				found = false;
				strcpy( network_id,
					(pCHAR)m_ncf01_local_table2[index].primary_key.network_id );
				
				strcpy(network_name, (pCHAR)m_ncf01_local_table2[index].name);
				
				for ( sort_stat_idx=0; sort_stat_idx<num_sorted; sort_stat_idx++ )
				{
					if ( 0 == strcmp(net_status_list[sort_stat_idx].network_id, network_id) )
					{
						found = true;
						break;
					}
				}
				
				if ( found == false )
				{
					/* This network id has not been sorted yet.  Put it in list. */
					strcpy(net_status_list[num_sorted].network_name, network_name);
					strcpy( net_status_list[num_sorted].network_id, network_id );
					net_status_list[num_sorted].status[0] = states[state];
					num_sorted++;
				}
			}
		}
	}
	
	nc_status2=GREEN;
	
	/* Now populate the display window. */
	for ( index=0; index<m_curr_index2; index++ )
	{
		entry = true;
		switch( net_status_list[index].status[0] )
		{
		case '0':
		case '4':
		case '5':
			strcpy( status_str, "Online" );
			status_color = BLACK;
			break;
			
		case '1':
			strcpy( status_str, "Offline" );
			status_color = RED;
			nc_status2 = RED;
			break;
			
		case '2':
			strcpy( status_str, "Querying" );
			status_color = BLACK;
			if(nc_status2 != RED)
				nc_status2 = YELLOW;
			break;
			
		case '3':
			strcpy( status_str, "Down" );
			status_color = RED;
			nc_status2 = RED;
			break;
			
		default:
			entry = false;
			break;
		}
		
		if ( entry == true )
		{
			//if(strlen(net_status_list[index].network_id) == 0)
			CString str1 = net_status_list[index].network_id;
			str1.MakeUpper();
			if(str1.Find('N') < 0)
			{
				/*sprintf( display_buff, "%d %d\t\t\t%s",
				m_curr_index,num_rows_returned,
					status_str );*/
			}
			else
			{
				if(strcmpi(net_status_list[index].network_id,"ncmcrd") == 0)
				{
					if(strcmpi(status_str,"Offline") == 0)
					{
						m_CntlAcqOff2.SetWindowText("ACQ ON");
						m_bAcqOn2 = true;
					}
					else
					{
						m_CntlAcqOff2.SetWindowText("ACQ OFF");
						m_bAcqOn2 = false;
					}
				}
				m_CntlNetTranList2.UpdateItemText( net_status_list[index].network_id, LS_STATUS, status_str);
				m_CntlNetTranList2.UpdateItemText( net_status_list[index].network_id, LS_NETWORK_NAME, net_status_list[index].network_name);

				if(status_color == RED)
				{
					m_CntlNetTranList2.SetRed(net_status_list[index].network_id, true);
				}
				else
				{
					m_CntlNetTranList2.SetRed(net_status_list[index].network_id, false);
				}

				sprintf( display_buff, "%s\t\t%-13s\t%s",
					net_status_list[index].network_id, status_str, net_status_list[index].network_name );
			}
			
			/* A status of offline or down will be displayed in red. */
			if ( status_color == BLACK )
			{
			//	m_NetworkStatus.AddString(display_buff); /* black */
			}
			else
			{
			//	m_NetworkStatus.AddString(display_buff,RGB(255,0,0)); /* red */
			}
		}
	}
	
	switch(nc_status2)
	{
	case GREEN:
		m_CntlNCStatus2.SetBitmap( m_hNCGreen2 );
		break;
	case YELLOW:
		m_CntlNCStatus2.SetBitmap( m_hNCYellow2 );
		break;
	default:
		m_CntlNCStatus2.SetBitmap( m_hNCRed2);
		if(!m_CntlMuteBeep2.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
			Sleep(500);
		}
		
		break;
	}
	
	return;
}

void CMonitorView::PopulateNetworkStatusList3()
{
	INT   status_color;
	CHAR  display_buff[512] = "";
	CHAR  status_str[20]    = "";
	CHAR  network_id[11]    = "";
	CHAR	 network_name[30] = {0};
	
	/* 'states' contains the sort order, where
	*     0 = online (also 4 and 5)
	*     1 = offline
	*     2 = querying
	*     3 = down
	*/
	CHAR  states[] = {"130452"}; /* Offline first, Querying last. */
	
	INT   found;
	INT   entry;
	INT   state; /* Index for 'states'. */
	INT   num_sorted = 0;
	INT   sort_stat_idx;
	INT   index;
	
	typedef struct
	{
		CHAR  network_id[11];
		CHAR  status[2];
		CHAR	network_name[30];
	} NET_SORT_STATUS;
	
	NET_SORT_STATUS  net_status_list[MAX_NETWORKS];
	
	
	/* Initialize status list */
	memset( &net_status_list, 0x00, sizeof(NET_SORT_STATUS)*MAX_NETWORKS );
	
	/* Initialize the window. */
	m_NetworkStatus3.ResetContent();
	m_NetworkStatus3.UpdateWindow();
	
	/* Loop through each state. */
	for ( state=0; state<6; state++ )
	{
	/* Loop through the main network id list, picking out
	* those whose states match the current state index.
		*/
		for ( index=0; index<m_curr_index3; index++ )
		{
			if ( m_ncf01_local_table3[index].status[0] == states[state] )
			{
			/* This network id is set to the state we are sorting.
			* Has it already been sorted? (Remember there are 2
			* network types (I and A).  Don't do both.
				*/
				found = false;
				strcpy( network_id,
					(pCHAR)m_ncf01_local_table3[index].primary_key.network_id );
				
				strcpy(network_name, (pCHAR)m_ncf01_local_table3[index].name);
				
				for ( sort_stat_idx=0; sort_stat_idx<num_sorted; sort_stat_idx++ )
				{
					if ( 0 == strcmp(net_status_list[sort_stat_idx].network_id, network_id) )
					{
						found = true;
						break;
					}
				}
				
				if ( found == false )
				{
					/* This network id has not been sorted yet.  Put it in list. */
					strcpy(net_status_list[num_sorted].network_name, network_name);
					strcpy( net_status_list[num_sorted].network_id, network_id );
					net_status_list[num_sorted].status[0] = states[state];
					num_sorted++;
				}
			}
		}
	}
	
	nc_status3=GREEN;
	
	/* Now populate the display window. */
	for ( index=0; index<m_curr_index3; index++ )
	{
		entry = true;
		switch( net_status_list[index].status[0] )
		{
		case '0':
		case '4':
		case '5':
			strcpy( status_str, "Online" );
			status_color = BLACK;
			break;
			
		case '1':
			strcpy( status_str, "Offline" );
			status_color = RED;
			nc_status3 = RED;
			break;
			
		case '2':
			strcpy( status_str, "Querying" );
			status_color = BLACK;
			if(nc_status3 != RED)
				nc_status3 = YELLOW;
			break;
			
		case '3':
			strcpy( status_str, "Down" );
			status_color = RED;
			nc_status3 = RED;
			break;
			
		default:
			entry = false;
			break;
		}
		
		if ( entry == true )
		{
			//if(strlen(net_status_list[index].network_id) == 0)
			CString str1 = net_status_list[index].network_id;
			str1.MakeUpper();
			if(str1.Find('N') < 0)
			{
				/*sprintf( display_buff, "%d %d\t\t\t%s",
				m_curr_index,num_rows_returned,
					status_str );*/
			}
			else
			{
				if(strcmpi(net_status_list[index].network_id,"ncmcrd") == 0)
				{
					if(strcmpi(status_str,"Offline") == 0)
					{
						m_CntlAcqOff3.SetWindowText("ACQ ON");
						m_bAcqOn3 = true;
					}
					else
					{
						m_CntlAcqOff3.SetWindowText("ACQ OFF");
						m_bAcqOn3 = false;
					}
				}
				m_CntlNetTranList3.UpdateItemText( net_status_list[index].network_id, LS_STATUS, status_str);
				m_CntlNetTranList3.UpdateItemText( net_status_list[index].network_id, LS_NETWORK_NAME, net_status_list[index].network_name);

				if(status_color == RED)
				{
					m_CntlNetTranList3.SetRed(net_status_list[index].network_id, true);
				}
				else
				{
					m_CntlNetTranList3.SetRed(net_status_list[index].network_id, false);
				}

				sprintf( display_buff, "%s\t\t%-13s\t%s",
					net_status_list[index].network_id, status_str, net_status_list[index].network_name );
			}
			
			/* A status of offline or down will be displayed in red. */
			if ( status_color == BLACK )
			{
			//	m_NetworkStatus3.AddString(display_buff); /* black */
			}
			else
			{
			//	m_NetworkStatus3.AddString(display_buff,RGB(255,0,0)); /* red */
			}
		}
	}
	
	switch(nc_status3)
	{
	case GREEN:
		m_CntlNCStatus3.SetBitmap( m_hNCGreen3 );
		break;
	case YELLOW:
		m_CntlNCStatus3.SetBitmap( m_hNCYellow3 );
		break;
	default:
		m_CntlNCStatus3.SetBitmap( m_hNCRed3);
		if(!m_CntlMuteBeep3.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
			Sleep(500);
		}
		
		break;
	}
	
	return;
}

void CMonitorView::PopulateNetworkStatusList4()
{
	INT   status_color;
	CHAR  display_buff[512] = "";
	CHAR  status_str[20]    = "";
	CHAR  network_id[11]    = "";
	CHAR	 network_name[30] = {0};
	
	/* 'states' contains the sort order, where
	*     0 = online (also 4 and 5)
	*     1 = offline
	*     2 = querying
	*     3 = down
	*/
	CHAR  states[] = {"130452"}; /* Offline first, Querying last. */
	
	INT   found;
	INT   entry;
	INT   state; /* Index for 'states'. */
	INT   num_sorted = 0;
	INT   sort_stat_idx;
	INT   index;
	
	typedef struct
	{
		CHAR  network_id[11];
		CHAR  status[2];
		CHAR	network_name[30];
	} NET_SORT_STATUS;
	
	NET_SORT_STATUS  net_status_list[MAX_NETWORKS];
	
	
	/* Initialize status list */
	memset( &net_status_list, 0x00, sizeof(NET_SORT_STATUS)*MAX_NETWORKS );
	
	/* Initialize the window. */
	m_NetworkStatus4.ResetContent();
	m_NetworkStatus4.UpdateWindow();
	
	/* Loop through each state. */
	for ( state=0; state<6; state++ )
	{
	/* Loop through the main network id list, picking out
	* those whose states match the current state index.
		*/
		for ( index=0; index<m_curr_index4; index++ )
		{
			if ( m_ncf01_local_table4[index].status[0] == states[state] )
			{
			/* This network id is set to the state we are sorting.
			* Has it already been sorted? (Remember there are 2
			* network types (I and A).  Don't do both.
				*/
				found = false;
				strcpy( network_id,
					(pCHAR)m_ncf01_local_table4[index].primary_key.network_id );
				
				strcpy(network_name, (pCHAR)m_ncf01_local_table4[index].name);
				
				for ( sort_stat_idx=0; sort_stat_idx<num_sorted; sort_stat_idx++ )
				{
					if ( 0 == strcmp(net_status_list[sort_stat_idx].network_id, network_id) )
					{
						found = true;
						break;
					}
				}
				
				if ( found == false )
				{
					/* This network id has not been sorted yet.  Put it in list. */
					strcpy(net_status_list[num_sorted].network_name, network_name);
					strcpy( net_status_list[num_sorted].network_id, network_id );
					net_status_list[num_sorted].status[0] = states[state];
					num_sorted++;
				}
			}
		}
	}
	
	nc_status4=GREEN;
	
	/* Now populate the display window. */
	for ( index=0; index<m_curr_index4; index++ )
	{
		entry = true;
		switch( net_status_list[index].status[0] )
		{
		case '0':
		case '4':
		case '5':
			strcpy( status_str, "Online" );
			status_color = BLACK;
			break;
			
		case '1':
			strcpy( status_str, "Offline" );
			status_color = RED;
			nc_status4 = RED;
			break;
			
		case '2':
			strcpy( status_str, "Querying" );
			status_color = BLACK;
			if(nc_status4 != RED)
				nc_status4 = YELLOW;
			break;
			
		case '3':
			strcpy( status_str, "Down" );
			status_color = RED;
			nc_status4 = RED;
			break;
			
		default:
			entry = false;
			break;
		}
		
		if ( entry == true )
		{
			//if(strlen(net_status_list[index].network_id) == 0)
			CString str1 = net_status_list[index].network_id;
			str1.MakeUpper();
			if(str1.Find('N') < 0)
			{
				/*sprintf( display_buff, "%d %d\t\t\t%s",
				m_curr_index,num_rows_returned,
					status_str );*/
			}
			else
			{
				if(strcmpi(net_status_list[index].network_id,"ncmcrd") == 0)
				{
					if(strcmpi(status_str,"Offline") == 0)
					{
						m_CntlAcqOff4.SetWindowText("ACQ ON");
						m_bAcqOn4 = true;
					}
					else
					{
						m_CntlAcqOff4.SetWindowText("ACQ OFF");
						m_bAcqOn4 = false;
					}
				}
				m_CntlNetTranList4.UpdateItemText( net_status_list[index].network_id, LS_STATUS, status_str);
				m_CntlNetTranList4.UpdateItemText( net_status_list[index].network_id, LS_NETWORK_NAME, net_status_list[index].network_name);

				if(status_color == RED)
				{
					m_CntlNetTranList4.SetRed(net_status_list[index].network_id, true);
				}
				else
				{
					m_CntlNetTranList4.SetRed(net_status_list[index].network_id, false);
				}

				sprintf( display_buff, "%s\t\t%-13s\t%s",
					net_status_list[index].network_id, status_str, net_status_list[index].network_name );
			}
			
			/* A status of offline or down will be displayed in red. */
			if ( status_color == BLACK )
			{
			//	m_NetworkStatus4.AddString(display_buff); /* black */
			}
			else
			{
			//	m_NetworkStatus4.AddString(display_buff,RGB(255,0,0)); /* red */
			}
		}
	}
	
	switch(nc_status4)
	{
	case GREEN:
		m_CntlNCStatus4.SetBitmap( m_hNCGreen4 );
		break;
	case YELLOW:
		m_CntlNCStatus4.SetBitmap( m_hNCYellow4 );
		break;
	default:
		m_CntlNCStatus4.SetBitmap( m_hNCRed4);
		if(!m_CntlMuteBeep4.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
			Sleep(500);
		}
		
		break;
	}
	
	return;
}
/*************************************************************************************
NAME:               FindNCF01Record

DESCRIPTION:        This function searches the local NCF01 table for a record with
the specified network id and type.

INPUTS:
None
OUTPUTS:
None
RETURNS:           index to the local NCF01 table.

AUTHOR:             Ram Malathkar 
*************************************************************************************/
INT CMonitorView::FindNCF01Record( pCHAR network_id, pCHAR network_type )
{
	
	for( INT temp = 0; temp < m_curr_index; temp++ )
	{
		if( (strcmp( (pCHAR)m_ncf01_local_table [temp].primary_key.network_id, network_id ) == 0)
			&& (strcmp( (pCHAR)m_ncf01_local_table [temp].primary_key.network_type, network_type ) == 0) )
			return temp;
	}
	
	return -1;
}

INT CMonitorView::FindNCF01Record2( pCHAR network_id, pCHAR network_type )
{
	
	for( INT temp = 0; temp < m_curr_index2; temp++ )
	{
		if( (strcmp( (pCHAR)m_ncf01_local_table2 [temp].primary_key.network_id, network_id ) == 0)
			&& (strcmp( (pCHAR)m_ncf01_local_table2 [temp].primary_key.network_type, network_type ) == 0) )
			return temp;
	}
	
	return -1;
}

INT CMonitorView::FindNCF01Record3( pCHAR network_id, pCHAR network_type )
{
	
	for( INT temp = 0; temp < m_curr_index3; temp++ )
	{
		if( (strcmp( (pCHAR)m_ncf01_local_table3 [temp].primary_key.network_id, network_id ) == 0)
			&& (strcmp( (pCHAR)m_ncf01_local_table3 [temp].primary_key.network_type, network_type ) == 0) )
			return temp;
	}
	
	return -1;
}

INT CMonitorView::FindNCF01Record4( pCHAR network_id, pCHAR network_type )
{
	
	for( INT temp = 0; temp < m_curr_index4; temp++ )
	{
		if( (strcmp( (pCHAR)m_ncf01_local_table4 [temp].primary_key.network_id, network_id ) == 0)
			&& (strcmp( (pCHAR)m_ncf01_local_table4 [temp].primary_key.network_type, network_type ) == 0) )
			return temp;
	}
	
	return -1;
}
/*************************************************************************************
NAME:               InitializeActivityStatsList

DESCRIPTION:        This function initializes the activity statistics list box
with zeroes

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Linda Mitchell 
*************************************************************************************/
void CMonitorView::InitializeActivityStatsList()
{
	CHAR  display_buff[512];
	CHAR  spaces[MAX_SPACES]="";
	
	m_CntlNetTranList.SetHeadings("Service Name, 125; Status, 100; Network Name, 150; Received, 70; Sent, 50; Rate, 50; Approved, 70; Declined, 60; TimedOut, 70; Reversed, 80; Mgt Counts, 80; STIP Sent, 80; STIP Recv, 80;");
	m_CntlNetTranList.DeleteAllItems(ItemdataProc, (LPARAM)this);
	m_TranActivityList.ResetContent(); /*empty list box*/
	
	memset( spaces, ' ', MAX_SPACES-1 );
	for(int i=0;i< Controller_Count; i++)
	{
		m_CntlNetTranList.InsertItem(i, (pCHAR)Controller_Stats[i].service_id,
			"Querying", 
			(pCHAR)Controller_Stats[i].service_id, 
			"0",
			"0",
			"0.0",
			"0",
			"0",
			"0",
			"0",
			"0",
			"0",
			"0");	
	}
	m_CntlNetTranList.Sort(0,1);

	for( INT temp = 0; temp < Controller_Count; temp++ )
	{
		/*reset previous sent count to zero*/
		Controller_Stats[temp].previous_sent_count = 0;
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld",
			(pCHAR)Controller_Stats[temp].service_id,
			0, spaces,
			0, spaces,
			0.0,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0);
		
		m_TranActivityList.AddString( display_buff );
	}
}

void CMonitorView::InitializeActivityStatsList2()
{
	CHAR  display_buff[512];
	CHAR  spaces[MAX_SPACES]="";

	m_CntlNetTranList2.SetHeadings("Service Name, 125; Status, 100; Network Name, 150; Received, 70; Sent, 50; Rate, 50; Approved, 70; Declined, 60; TimedOut, 70; Reversed, 80; Mgt Counts, 80; STIP Sent, 80; STIP Recv, 80;");
	m_CntlNetTranList2.DeleteAllItems(ItemdataProc, (LPARAM)this);
	m_TranActivityList2.ResetContent(); 

	memset( spaces, ' ', MAX_SPACES-1 );
	for(int i=0;i< Controller_Count2; i++)
	{
		m_CntlNetTranList2.InsertItem(i, (pCHAR)Controller_Stats2[i].service_id,
			"Querying", 
			(pCHAR)Controller_Stats2[i].service_id, 
			"0", "0", "0.0", "0", "0", "0", "0", "0", "0", "0");		
	}
	m_CntlNetTranList2.Sort(0,1);

	for( INT temp = 0; temp < Controller_Count2; temp++ )
	{
		/*reset previous sent count to zero*/
		Controller_Stats2[temp].previous_sent_count = 0;
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld",
			(pCHAR)Controller_Stats2[temp].service_id,
			0, spaces,
			0, spaces,
			0.0,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0);

		m_TranActivityList2.AddString( display_buff );
	}
}

void CMonitorView::InitializeActivityStatsList3()
{
	CHAR  display_buff[512];
	CHAR  spaces[MAX_SPACES]="";

	m_CntlNetTranList3.SetHeadings("Service Name, 125; Status, 100; Network Name, 150; Received, 70; Sent, 50; Rate, 50; Approved, 70; Declined, 60; TimedOut, 70; Reversed, 80; Mgt Counts, 80; STIP Sent, 80; STIP Recv, 80;");
	m_CntlNetTranList3.DeleteAllItems(ItemdataProc, (LPARAM)this);
	m_TranActivityList3.ResetContent(); 

	memset( spaces, ' ', MAX_SPACES-1 );
	for(int i=0;i< Controller_Count3; i++)
	{
		m_CntlNetTranList3.InsertItem(i, (pCHAR)Controller_Stats3[i].service_id,
			"Querying", 
			(pCHAR)Controller_Stats3[i].service_id, 
			"0", "0", "0.0", "0", "0", "0", "0", "0", "0", "0");		
	}
	m_CntlNetTranList3.Sort(0,1);

	for( INT temp = 0; temp < Controller_Count3; temp++ )
	{
		/*reset previous sent count to zero*/
		Controller_Stats3[temp].previous_sent_count = 0;
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld",
			(pCHAR)Controller_Stats3[temp].service_id,
			0, spaces,
			0, spaces,
			0.0,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0);

		m_TranActivityList3.AddString( display_buff );
	}
}

void CMonitorView::InitializeActivityStatsList4()
{
	CHAR  display_buff[512];
	CHAR  spaces[MAX_SPACES]="";

	m_CntlNetTranList4.SetHeadings("Service Name, 125; Status, 100; Network Name, 150; Received, 70; Sent, 50; Rate, 50; Approved, 70; Declined, 60; TimedOut, 70; Reversed, 80; Mgt Counts, 80; STIP Sent, 80; STIP Recv, 80;");
	m_CntlNetTranList4.DeleteAllItems(ItemdataProc, (LPARAM)this);
	m_TranActivityList4.ResetContent(); 
	
	memset( spaces, ' ', MAX_SPACES-1 );
	for(int i=0;i< Controller_Count4; i++)
	{
		m_CntlNetTranList4.InsertItem(i, (pCHAR)Controller_Stats4[i].service_id,
			"Querying", 
			(pCHAR)Controller_Stats4[i].service_id, 
			"0", "0", "0.0", "0", "0", "0", "0", "0", "0", "0");		
	}
	m_CntlNetTranList4.Sort(0,1);

	for( INT temp = 0; temp < Controller_Count4; temp++ )
	{
		/*reset previous sent count to zero*/
		Controller_Stats4[temp].previous_sent_count = 0;
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld",
			(pCHAR)Controller_Stats4[temp].service_id,
			0, spaces,
			0, spaces,
			0.0,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0, spaces,
			0);

		m_TranActivityList4.AddString( display_buff );
	}
}
/*************************************************************************************
NAME:               InitializeDialogStatusList

DESCRIPTION:        This function initializes the dialog status list box
with No Data for each dialog service id

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Linda Mitchell 
*************************************************************************************/
void CMonitorView::InitializeDialogStatusList()
{
	CHAR  display_buff[512];
	m_DialogConnectStatus.SetFont(&m_font);

	/*populate list box display*/
	for( INT temp = 0; temp < Dialog_ID_Count; temp++ )
	{
		memset( display_buff, 0x00, sizeof(display_buff) );
		sprintf( display_buff, "%-14s\t%-13s\t%s",
			(pCHAR)Dialog_Status[temp].dialog_id,
			"No Data",Dialog_Status[temp].dialog_name);
		
		m_DialogConnectStatus.AddString( display_buff );
		
		/*Initialize Dialog_Status.status structure*/
		Dialog_Status[temp].status = NO_DATA;
		
	}
	
	/*Initialize Dialog_Status structure*/
	
}

void CMonitorView::InitializeDialogStatusList2()
{
	CHAR  display_buff[512];

	m_DialogConnectStatus2.SetFont(&m_font);
	/*populate list box display*/
	for( INT temp = 0; temp < Dialog_ID_Count2; temp++ )
	{
		memset( display_buff, 0x00, sizeof(display_buff) );
		sprintf( display_buff, "%-14s\t%-13s\t%s",
			(pCHAR)Dialog_Status2[temp].dialog_id,
			"NODATA",Dialog_Status2[temp].dialog_name);
		
		m_DialogConnectStatus2.AddString( display_buff );
		
		/*Initialize Dialog_Status.status structure*/
		Dialog_Status2[temp].status = NO_DATA;
		
	}
}

void CMonitorView::InitializeDialogStatusList3()
{
	CHAR  display_buff[512];

	m_DialogConnectStatus3.SetFont(&m_font);

	/*populate list box display*/
	for( INT temp = 0; temp < Dialog_ID_Count3; temp++ )
	{
		memset( display_buff, 0x00, sizeof(display_buff) );
		sprintf( display_buff, "%-14s\t%-13s\t%s",
			(pCHAR)Dialog_Status3[temp].dialog_id,
			"No Data",Dialog_Status3[temp].dialog_name);
		
		m_DialogConnectStatus3.AddString( display_buff );
		
		/*Initialize Dialog_Status.status structure*/
		Dialog_Status3[temp].status = NO_DATA;	
	}
}

void CMonitorView::InitializeDialogStatusList4()
{
	CHAR  display_buff[512];

	m_DialogConnectStatus4.SetFont(&m_font);

	/*populate list box display*/
	for( INT temp = 0; temp < Dialog_ID_Count4; temp++ )
	{
		memset( display_buff, 0x00, sizeof(display_buff) );
		sprintf( display_buff, "%-14s\t%-13s\t%s",
			(pCHAR)Dialog_Status4[temp].dialog_id,
			"NO DATA",Dialog_Status4[temp].dialog_name);
		
		m_DialogConnectStatus4.AddString( display_buff );
		
		/*Initialize Dialog_Status.status structure*/
		Dialog_Status4[temp].status = NO_DATA;	
	}
}
/*************************************************************************************
NAME:               InsertLogMessage

DESCRIPTION:        This function will insert an operator message into the  
list control.

INPUTS:
opr_message	   Pointer to the OPR_MESSAGE structure.
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
/*************************************************************************************/
void CMonitorView::InsertLogMessage( OPR_MESSAGE *opr_message )
{
	//   pCHAR       time_ptr, module_ptr, msg_text_ptr, function_ptr;
	//   pCHAR       msg_type_ptr, severity_ptr;
	INT         nIndex;
	DWORD       dwColor;
	CHAR        log_msg[1024];
	CHAR		   time_str[50];
	CHAR		   tab_str[10] = {0};
	struct tm * cur_time;
	time_t      clock ;
	
	
	m_LogMessages.EnableWindow(TRUE);  
	
	/* ******************************************************************************
	The complete message is in the format:
	YYYYMMDDHHIISSJJJ:: service::severity::message_text::function
	****************************************************************************** */
	
	time( &clock );
	cur_time = localtime( &clock );
	sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + OFFSET_YEAR,
		cur_time->tm_mon + OFFSET_MONTH, cur_time->tm_mday, cur_time->tm_hour,
		cur_time->tm_min, cur_time->tm_sec ) ;
	
	if(strlen((pCHAR)opr_message->message_type) > MAXLEN)    /*2 tabs if message_type > 5, ie. warning */
		sprintf(tab_str, "\t");								/* keep columns lined up */
	else
		sprintf(tab_str, "\t\t");
	
	/*if reversal or timeout set text to Red*/
	if(strstr((pCHAR)opr_message->message, "Reversal")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "timed out")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "DB Reconnection failed")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else 
	{
		if( (strstr((pCHAR)opr_message->message, "Maximum consecutive")!= NULL) &&
			(strstr((pCHAR)opr_message->message, "MTI"				  )!= NULL) &&
			((strcmp((pCHAR)opr_message->module, "natma2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nposa2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nccup2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncdci2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "nciftr") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncjcb2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncmcr2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncvis2") == 0)   ))
		{
			sprintf(log_msg, "MONITOR1 GH1:\nDATE: %.10s TIME: %.8s\n%s", time_str, time_str+12, (pCHAR)opr_message->message);
			//MessageBox(log_msg, "Ascendent System DCO Monitor - Time Out Alert", MB_ICONWARNING);

			//you must load user32.dll before calling the function
			HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
    
			if (hUser32)
			{
				UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_ICONWARNING;
				MessageBoxTimeout(this->m_hWnd, log_msg, _T("Ascendent System DCO Monitor - Time Out Alert"), uiFlags, 0, promptAlive);
				FreeLibrary(hUser32);
			}

			if(timeoutFileFlag == 1)
				write_timeout_data_file(MONITOR1, log_msg);

			memset(log_msg, 0, sizeof(log_msg));
		}
		dwColor = RGB(0,0,0);
	}
	
	sprintf(log_msg, "%s  %s,  \t%s, %s%-s,    %-s",
		time_str, 
		(pCHAR)opr_message->module,
		(pCHAR)opr_message->message_type,
		tab_str,					/*2 tabs if message_type > 5, ie. warning */
		(pCHAR)opr_message->message, 
		(pCHAR)opr_message->function);
	
	nIndex = m_LogMessages.InsertString(0, log_msg, dwColor);
	m_LogMessages.SetCurSel(nIndex);
	m_LogMessages.SetFocus() ;
}

void CMonitorView::InsertLogMessage2( OPR_MESSAGE *opr_message )
{
	//   pCHAR       time_ptr, module_ptr, msg_text_ptr, function_ptr;
	//   pCHAR       msg_type_ptr, severity_ptr;
	INT         nIndex;
	DWORD       dwColor;
	CHAR        log_msg[1024];
	CHAR		   time_str[50];
	CHAR		   tab_str[10] = {0};
	struct tm * cur_time;
	time_t      clock ;
	
	
	m_LogMessages2.EnableWindow(TRUE);  
	
	/* ******************************************************************************
	The complete message is in the format:
	YYYYMMDDHHIISSJJJ:: service::severity::message_text::function
	****************************************************************************** */
	
	time( &clock );
	cur_time = localtime( &clock );
	sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + OFFSET_YEAR,
		cur_time->tm_mon + OFFSET_MONTH, cur_time->tm_mday, cur_time->tm_hour,
		cur_time->tm_min, cur_time->tm_sec ) ;
	
	if(strlen((pCHAR)opr_message->message_type) > MAXLEN)    /*2 tabs if message_type > 5, ie. warning */
		sprintf(tab_str, "\t");								/* keep columns lined up */
	else
		sprintf(tab_str, "\t\t");
	
	/*if reversal or timeout set text to Red*/
	if(strstr((pCHAR)opr_message->message, "Reversal")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "timed out")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "DB Reconnection failed")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else 
	{
		if( (strstr((pCHAR)opr_message->message, "Maximum consecutive")!= NULL) &&
			(strstr((pCHAR)opr_message->message, "MTI"				  )!= NULL) &&
			((strcmp((pCHAR)opr_message->module, "natma2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nposa2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nccup2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncdci2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "nciftr") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncjcb2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncmcr2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncvis2") == 0)   ))
		{
			sprintf(log_msg, "MONITOR2 GH2:\nDATE: %.10s TIME: %.8s\n%s", time_str, time_str+12, (pCHAR)opr_message->message);
			//MessageBox(log_msg, "Ascendent System DCO Monitor - Time Out Alert", MB_ICONWARNING);

			//you must load user32.dll before calling the function
			HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
    
			if (hUser32)
			{
				UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_ICONWARNING;
				MessageBoxTimeout(this->m_hWnd, log_msg, _T("Ascendent System DCO Monitor - Time Out Alert"), uiFlags, 0, promptAlive);
				FreeLibrary(hUser32);
			}

			if(timeoutFileFlag == 1)
				write_timeout_data_file(MONITOR2, log_msg);

			memset(log_msg, 0, sizeof(log_msg));
		}
		dwColor = RGB(0,0,0);
	}
	
	sprintf(log_msg, "%s  %s,  \t%s, %s%-s,    %-s",
		time_str, 
		(pCHAR)opr_message->module,
		(pCHAR)opr_message->message_type,
		tab_str,					/*2 tabs if message_type > 5, ie. warning */
		(pCHAR)opr_message->message, 
		(pCHAR)opr_message->function);
	
	nIndex = m_LogMessages2.InsertString(0, log_msg, dwColor);
	m_LogMessages2.SetCurSel(nIndex);
	m_LogMessages2.SetFocus() ;
}

void CMonitorView::InsertLogMessage3( OPR_MESSAGE *opr_message )
{
	//   pCHAR       time_ptr, module_ptr, msg_text_ptr, function_ptr;
	//   pCHAR       msg_type_ptr, severity_ptr;
	INT         nIndex;
	DWORD       dwColor;
	CHAR        log_msg[1024];
	CHAR		   time_str[50];
	CHAR		   tab_str[10] = {0};
	struct tm * cur_time;
	time_t      clock ;
	
	
	m_LogMessages3.EnableWindow(TRUE);  
	
	/* ******************************************************************************
	The complete message is in the format:
	YYYYMMDDHHIISSJJJ:: service::severity::message_text::function
	****************************************************************************** */
	
	time( &clock );
	cur_time = localtime( &clock );
	sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + OFFSET_YEAR,
		cur_time->tm_mon + OFFSET_MONTH, cur_time->tm_mday, cur_time->tm_hour,
		cur_time->tm_min, cur_time->tm_sec ) ;
	
	if(strlen((pCHAR)opr_message->message_type) > MAXLEN)    /*2 tabs if message_type > 5, ie. warning */
		sprintf(tab_str, "\t");								/* keep columns lined up */
	else
		sprintf(tab_str, "\t\t");
	
	/*if reversal or timeout set text to Red*/
	if(strstr((pCHAR)opr_message->message, "Reversal")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "timed out")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "DB Reconnection failed")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else 
	{
		if( (strstr((pCHAR)opr_message->message, "Maximum consecutive")!= NULL) &&
			(strstr((pCHAR)opr_message->message, "MTI"				  )!= NULL) &&
			((strcmp((pCHAR)opr_message->module, "natma2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nposa2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nccup2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncdci2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "nciftr") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncjcb2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncmcr2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncvis2") == 0)   ))
		{
			sprintf(log_msg, "MONITOR3 MKT1:\nDATE: %.10s TIME: %.8s\n%s", time_str, time_str+12, (pCHAR)opr_message->message);
			//MessageBox(log_msg, "Ascendent System DCO Monitor - Time Out Alert", MB_ICONWARNING);

			//you must load user32.dll before calling the function
			HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
    
			if (hUser32)
			{
				UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_ICONWARNING;
				MessageBoxTimeout(this->m_hWnd, log_msg, _T("Ascendent System DCO Monitor - Time Out Alert"), uiFlags, 0, promptAlive);
				FreeLibrary(hUser32);
			}

			if(timeoutFileFlag == 1)
				write_timeout_data_file(MONITOR3, log_msg);

			memset(log_msg, 0, sizeof(log_msg));
		}
		dwColor = RGB(0,0,0);
	}
	
	sprintf(log_msg, "%s  %s,  \t%s, %s%-s,    %-s",
		time_str, 
		(pCHAR)opr_message->module,
		(pCHAR)opr_message->message_type,
		tab_str,					/*2 tabs if message_type > 5, ie. warning */
		(pCHAR)opr_message->message, 
		(pCHAR)opr_message->function);
	
	nIndex = m_LogMessages3.InsertString(0, log_msg, dwColor);
	m_LogMessages3.SetCurSel(nIndex);
	m_LogMessages3.SetFocus() ;
}

void CMonitorView::InsertLogMessage4( OPR_MESSAGE *opr_message )
{
	//   pCHAR       time_ptr, module_ptr, msg_text_ptr, function_ptr;
	//   pCHAR       msg_type_ptr, severity_ptr;
	INT         nIndex;
	DWORD       dwColor;
	CHAR        log_msg[1024];
	CHAR		   time_str[50];
	CHAR		   tab_str[10] = {0};
	struct tm * cur_time;
	time_t      clock ;
	
	
	m_LogMessages4.EnableWindow(TRUE);  
	
	/* ******************************************************************************
	The complete message is in the format:
	YYYYMMDDHHIISSJJJ:: service::severity::message_text::function
	****************************************************************************** */
	
	time( &clock );
	cur_time = localtime( &clock );
	sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + OFFSET_YEAR,
		cur_time->tm_mon + OFFSET_MONTH, cur_time->tm_mday, cur_time->tm_hour,
		cur_time->tm_min, cur_time->tm_sec ) ;
	
	if(strlen((pCHAR)opr_message->message_type) > MAXLEN)    /*2 tabs if message_type > 5, ie. warning */
		sprintf(tab_str, "\t");								/* keep columns lined up */
	else
		sprintf(tab_str, "\t\t");
	
	/*if reversal or timeout set text to Red*/
	if(strstr((pCHAR)opr_message->message, "Reversal")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "timed out")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else if(strstr((pCHAR)opr_message->message, "DB Reconnection failed")!= NULL)
	{
		dwColor = RGB(255,0,0);
	}
	else 
	{
		if( (strstr((pCHAR)opr_message->message, "Maximum consecutive")!= NULL) &&
			(strstr((pCHAR)opr_message->message, "MTI"				  )!= NULL) &&
			((strcmp((pCHAR)opr_message->module, "natma2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nposa2") == 0) || 
			 (strcmp((pCHAR)opr_message->module, "nccup2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncdci2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "nciftr") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncjcb2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncmcr2") == 0) ||
			 (strcmp((pCHAR)opr_message->module, "ncvis2") == 0)   ))
		{
			sprintf(log_msg, "MONITOR4 MKT2:\nDATE: %.10s TIME: %.8s\n%s", time_str, time_str+12, (pCHAR)opr_message->message);
			//MessageBox(log_msg, "Ascendent System DCO Monitor - Time Out Alert", MB_ICONWARNING);

			//you must load user32.dll before calling the function
			HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
    
			if (hUser32)
			{
				UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_ICONWARNING;
				MessageBoxTimeout(this->m_hWnd, log_msg, _T("Ascendent System DCO Monitor - Time Out Alert"), uiFlags, 0, promptAlive);
				FreeLibrary(hUser32);
			}

			if(timeoutFileFlag == 1)
				write_timeout_data_file(MONITOR4, log_msg);

			memset(log_msg, 0, sizeof(log_msg));
		}
		dwColor = RGB(0,0,0);
	}
	
	sprintf(log_msg, "%s  %s,  \t%s, %s%-s,    %-s",
		time_str, 
		(pCHAR)opr_message->module,
		(pCHAR)opr_message->message_type,
		tab_str,					/*2 tabs if message_type > 5, ie. warning */
		(pCHAR)opr_message->message, 
		(pCHAR)opr_message->function);
	
	nIndex = m_LogMessages4.InsertString(0, log_msg, dwColor);
	m_LogMessages4.SetCurSel(nIndex);
	m_LogMessages4.SetFocus() ;
}

void CMonitorView::write_timeout_data_file(int mon, char *log_data)
{
	FILE *tp;
	CString Buffer;
	CHAR handle[50] = "", displayhostname[50] = "";

	if(mon == 1)
	{
		strcpy(displayhostname, "MON1_GH1_");
		strcat(displayhostname, HostName1);
	}
	else if(mon == 2)
	{
		strcpy(displayhostname, "MON2_GH2_");
		strcat(displayhostname, HostName2);
	}
	else if(mon == 3)
	{
		strcpy(displayhostname, "MON3_MKT1_");
		strcat(displayhostname, HostName3);
	}
	else if(mon == 4)
	{
		strcpy(displayhostname, "MON3_MKT2_");
		strcat(displayhostname, HostName4);
	}

	CTime time = CTime::GetCurrentTime();
	Buffer.Format("..\\log\\DCO_timeout_details_%s_%s.log", displayhostname, time.Format("%d%m%Y"));

	tp = fopen(Buffer, "w");
	if(tp == NULL)
	{
		CreateDirectory("..\\log\\", NULL);
		tp = fopen(Buffer, "w");
		if(tp == NULL)
			return;
	}

	strcat(log_data, "\n\n");
	fwrite( log_data, sizeof( CHAR ), strlen( log_data ), tp );
	fflush(tp);
	fclose(tp);
}
/*************************************************************************************

/*************************************************************************************
NAME:               GetCurrentStatus

DESCRIPTION:        This function sends a request to trands to retrieve the
NCF01 table.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::GetCurrentStatus() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	
	for( INT temp = 0; temp < m_curr_index; temp++ )
	{
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_NETWORK_QUERY,
			0,
			(pCHAR)m_ncf01_local_table [temp].que_name,
			interactive_que_name,
			(pBYTE)&m_ncf01_local_table [temp],
			sizeof( NCF01 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			sprintf( buffer, "MONITOR1 GH1: Insufficient memory to build query for %s",
				m_ncf01_local_table[temp].primary_key.network_id );
			MessageBox( buffer );
			return;
		}
		
		if( !PtetcpSend( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR1 GH1: Communication error sending query to %s: %s",
				m_ncf01_local_table[temp].primary_key.network_id, error_msg );
			MessageBox( buffer );
		}	
		
		free( p_msg_out );
		Sleep( 500 );
	}
}

void CMonitorView::GetCurrentStatus2() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	
	for( INT temp = 0; temp < m_curr_index2; temp++ )
	{
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_NETWORK_QUERY,
			0,
			(pCHAR)m_ncf01_local_table2 [temp].que_name,
			interactive_que_name,
			(pBYTE)&m_ncf01_local_table2 [temp],
			sizeof( NCF01 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			sprintf( buffer, "MONITOR2 GH2: Insufficient memory to build query for %s",
				m_ncf01_local_table2[temp].primary_key.network_id );
			MessageBox( buffer );
			return;
		}
		
		if( !PtetcpSend2( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR2 GH2: Communication error sending query to %s: %s",
				m_ncf01_local_table2[temp].primary_key.network_id, error_msg );
			MessageBox( buffer );
		}	
		
		free( p_msg_out );
		Sleep( 500 );
	}
}

void CMonitorView::GetCurrentStatus3() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	
	for( INT temp = 0; temp < m_curr_index3; temp++ )
	{
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_NETWORK_QUERY,
			0,
			(pCHAR)m_ncf01_local_table3 [temp].que_name,
			interactive_que_name,
			(pBYTE)&m_ncf01_local_table3 [temp],
			sizeof( NCF01 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			sprintf( buffer, "MONITOR3 MKT1: Insufficient memory to build query for %s",
				m_ncf01_local_table3[temp].primary_key.network_id );
			MessageBox( buffer );
			return;
		}
		
		if( !PtetcpSend3( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR3 MKT1: Communication error sending query to %s: %s",
				m_ncf01_local_table3[temp].primary_key.network_id, error_msg );
			MessageBox( buffer );
		}	
		
		free( p_msg_out );
		Sleep( 500 );
	}
}

void CMonitorView::GetCurrentStatus4() 
{
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	
	for( INT temp = 0; temp < m_curr_index4; temp++ )
	{
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			ST1_NETWORK_QUERY,
			0,
			(pCHAR)m_ncf01_local_table4 [temp].que_name,
			interactive_que_name,
			(pBYTE)&m_ncf01_local_table4 [temp],
			sizeof( NCF01 ),
			0 );

		if( p_msg_out == NULL_PTR )
		{
			sprintf( buffer, "MONITOR4 MKT2: Insufficient memory to build query for %s",
				m_ncf01_local_table4[temp].primary_key.network_id );
			MessageBox( buffer );
			return;
		}
		
		if( !PtetcpSend4( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "MONITOR4 MKT2: Communication error sending query to %s: %s",
				m_ncf01_local_table4[temp].primary_key.network_id, error_msg );
			MessageBox( buffer );
		}	
		
		free( p_msg_out );
		Sleep( 500 );
	}
}

/*************************************************************************************
NAME:               LogMessageHandler

DESCRIPTION:        This is a thread that scans for any TCP/IP responses for 
requests made from other functions.

INPUTS:
None
OUTPUTS:
None
RETURNS:           
None
AUTHOR:             Ram Malathkar 
*************************************************************************************/
void CMonitorView::LogMessageHandler()
{
	pPTE_MSG       p_msg_in;
	LONG           data_len;
	pBYTE          p_data;
	pPTE_MSG_DATA  p_msg_data;
	CHAR           error_msg[256];
	CHAR           temp_str[100];
	BYTE           dberror;
	BYTE           msgtype;
	pCHAR          orig_que = NULL_PTR;
	NCF01_GUI_LIST ncf01_list;
	INT            num_returned, index;
	OPR_MESSAGE    opr_msg;
	NCF01          ncf01;
	HANDLE hThread;
	DWORD  ThreadId;
	CString msg;
	INT ret=0;
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	MT_USER_INFO	 tmpUserInfo;
	
	
	
	while( 1 )
	{
		p_msg_in = PtetcpReceive( 0, error_msg );
		if ( p_msg_in == NULL_PTR )
		{
			return;
		}
		
		p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
		
		msgtype = ptemsg_get_msg_type( p_msg_in );
		switch( msgtype )
		{
		case MT_MONITOR_CLEAR_SOCKETS:
			switch( ptemsg_get_msg_subtype1( p_msg_in ) )
			{
			case ST1_MONITOR_CLEAR_SOCKETS:
				//Monitor closed by the server.
				
				//close_dco_monitor();
				
				AfxMessageBox("MONITOR1 GH1: Server closed the connection. Exiting..", MB_SETFOREGROUND | MB_ICONSTOP | MB_ICONHAND | MB_OK );
				//EnableDialogButtons(FALSE, FALSE);

				// Exit this log thread as well as the main application here.
				
				ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
				AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);  // below 2 line replaced for above 2 lines fix the monitor issue.
				
				CloseHandle(this);
				ExitThread(0);
				//return; // Exit thread. Close Application.
				//break;
			}
			break;
			case MT_SYS_MONITOR:
				{
					
					switch( ptemsg_get_msg_subtype1( p_msg_in ) )
					{
					case ST1_OPRMSG:
						memset( &opr_msg, 0, sizeof( OPR_MESSAGE ) );
						memcpy( &opr_msg, (OPR_MESSAGE *)p_data, sizeof( OPR_MESSAGE ) );
						InsertLogMessage( &opr_msg );
						break;
						
					case ST1_NETWORK_UPDATE:
						memset( &ncf01, 0, sizeof( NCF01 ) );
						memcpy( &ncf01, (pNCF01)p_data, sizeof( NCF01 ) );
						index = FindNCF01Record( (pCHAR)ncf01.primary_key.network_id,
							(pCHAR)ncf01.primary_key.network_type );
						if( index != -1 )
						{
							strcpy( (pCHAR)m_ncf01_local_table [index].status, (pCHAR)ncf01.status );
							m_NetworkStatus.ResetContent();
							m_NetworkStatus.UpdateWindow();
							PopulateNetworkStatusList();
						}
						break;
						
					case ST1_MONITOR_UP:
						
						m_bMonitorUp = true;
						switch( ptemsg_get_msg_subtype2( p_msg_in ) )
						{
						case ST2_MONITOR_ALREADY_RUNNING:
							m_bMonitorUp = false;
							// Someone is already connected - you are not allowed to connect. Use override.
							AfxMessageBox( "MONITOR1 GH1: ERROR - Two Monitors are already connected from other Systems.", MB_SYSTEMMODAL | MB_OK );
							EnableDialogButtons(FALSE, FALSE);

							//OnCloseBtn();
							
							//ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
							//AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE); 
							
							//new changes - amith
							//don't close monitors, clear the initialized data
							m_CntlNetTranList.DeleteAllItems(ItemdataProc, (LPARAM)this);
							m_TranActivityList.ResetContent();
							m_DialogConnectStatus.ResetContent();
							m_LogMessages.ResetContent();
							DisableAllButtons(1);

							return;	// Exit thread. Close Application.
							break; // Not required, but kept for the sake of correctness.
							
						case ST2_MONITOR_READ_ONLY:
							
								memset(&tmpUserInfo, 0x00, sizeof(MT_USER_INFO));
								
								if(p_data != NULL)
								{
									memcpy( &tmpUserInfo, (pMT_USER_INFO)p_data, sizeof( MT_USER_INFO ) );
								}
							
							// Monitor was already running so this is a read-only connection.
								if(strlen((char *)tmpUserInfo.user_name) == 0)
								{
									strcpy((char *)tmpUserInfo.user_name, "UNKNOWN");
								}
							msg.Format("MONITOR1 GH1: Primary Monitor is already connected. This will be a read-only session.\n %s user has already connected", tmpUserInfo.user_name);
						//	AfxMessageBox( msg, MB_YESNO | MB_ICONSTOP  );
							

							/*AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
							AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
							/*ret = AfxMessageBox( msg , MB_YESNO | MB_ICONQUESTION );
							if( ret == IDNO )
								break ;
							else*/
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
								EnableDialogButtons(FALSE, TRUE);
							
							break;
							
						case ST2_MONITOR_RECONNECT:
							// Nothing to do here.
							break;
							
						default:
							{
								orig_que = ptemsg_get_msg_orig_queue( p_msg_in );
								
								if( 0 == strcmp(orig_que,txcntl_que_name) )
								{
									/* Sleep 2 seconds to make sure user sees state change. */
									Sleep( 2000 );
									
									/* Reset the Test Result field. */
									m_TestResult.SetBitmap( m_hTestOff );
									
									/* Log a message to indicate successful test. */
									memset( &opr_msg, 0x00, sizeof(OPR_MESSAGE) );
									
									strcpy( (pCHAR)opr_msg.message, "Monitor is operational. Successful Test");
									strcpy( (pCHAR)opr_msg.module,	"Monitor" );
									strcpy( (pCHAR)opr_msg.message_type, "INFO" );
									opr_msg.severity = '1';
									
									InsertLogMessage( &opr_msg );
								}
								else		// Buttons should not get enabled when a read-only monitor receives a response for Test button - which is same as a full monitor response. Difference is only in the initial response for a monitor connection.
									EnableDialogButtons(TRUE, FALSE); // enable buttons now.
							}
							break;
						}
						break;
						
						default:
							break;
					}
				}
				break;
				
				case MT_SYSTEM_REPLY:
					process_ping(p_msg_in,msgtype);
					break;
					
				case MT_COM_REPLY:
				case MT_GET_STATS_RESPONSE:
					{
					/* Received a response from a dialog service or from a device
					* or network controller.  This response contains either a
					* dialog's 'connect' status, or a controller's transaction
					* statistics.
						*/
						process_stats( p_msg_in, msgtype );
					}
					break;
					
				case MT_DB_REPLY:
					
					switch(ptemsg_get_msg_subtype1( p_msg_in ))
					{
					case ST1_DB_GET_GUI_LIST:
						{
							//SetEvent( m_DataServerReply );
							p_data = p_data + sizeof(AUTH_TX);
							if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
							{
								strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
								sprintf( error_msg, "MONITOR1 GH1: Database Error: %s", temp_str );
								MessageBox( error_msg );
								break;
							}
							
							memset( &ncf01_list, 0, sizeof( ncf01_list ) );
							memcpy( &ncf01_list, (pNCF01_GUI_LIST)p_data, sizeof( NCF01_GUI_LIST ) );
							
							num_returned = atoi( (pCHAR)ncf01_list.num_returned );
							// set all the status fields to '2'. This means status unknown
							for( INT loop = 0; loop < num_returned; loop++ )
							{
								bool insert = true;
								for(int xy=0;xy<Controller_Count && insert;xy++)
								{
									if(strcmp((pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id, Controller_Stats[xy].service_id) == 0)
									{
										insert = false;
									}
								}
								if(insert)
								{
									strcpy(Controller_Stats[Controller_Count].service_id, (pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id);
									Controller_Count++;
								}
								
								strcpy( (pCHAR)ncf01_list.ncf01_record [loop].status, "2" );
							}
							if( (m_curr_index + num_returned) > NCF01_LOCAL_TABLESIZE )
							{
								MessageBox( "MONITOR1 GH1: NCF01 table contents exceeds local capacity. Truncating Data" );
								PopulateNetworkStatusList();
								//PopulateNetworkStatusList2();
								
								// now query each of the network handlers for the current status
								//	GetCurrentStatus();
								hThread = CreateThread( NULL,
									NULL,
									(LPTHREAD_START_ROUTINE)GetNetworkStatusThread,
									this,
									NULL,
									&ThreadId );
								
								if( hThread == NULL )
								{
									AfxMessageBox( "MONITOR1 GH1: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
									return;
								}
							}
							else
							{
								memcpy( &m_ncf01_local_table [ m_curr_index ], ncf01_list.ncf01_record,
									( num_returned * sizeof( NCF01 ) ) );
								m_curr_index += num_returned;
								num_rows_returned = num_returned;
								if( num_returned < GUI_MAX_LIST_SIZE )
								{
									m_retrieval_done = 1;
									//SetEvent( m_DataServerReply );
									PopulateNetworkStatusList();
									
									// now query each of the network handlers for the current status
									//	GetCurrentStatus();
									hThread = CreateThread( NULL,
										NULL,
										(LPTHREAD_START_ROUTINE)GetNetworkStatusThread,
										this,
										NULL,
										&ThreadId );
									
									if( hThread == NULL )
									{
										AfxMessageBox( "MONITOR1 GH1: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
										return;
									}
								}
								else
								{
									strcpy( (pCHAR)m_ncf01.primary_key.network_id,
										(pCHAR)m_ncf01_local_table [m_curr_index-1].primary_key.network_id );
									strcpy( (pCHAR)m_ncf01.primary_key.network_type,
										(pCHAR)m_ncf01_local_table [m_curr_index-1].primary_key.network_type );
									GetNCF01List();
								}
							}
						}
						break;
						
					case ST1_DB_SELECT:
						if ( ptemsg_get_pte_msg_data_app_data_type(p_msg_data) == UCF01_DATA )
						{
							// Copy the UCF01 record in the shared buffer, and flag the main thread.
							// We are waiting in the main GUI thread for this data.
							if (data_len < sizeof (ucf01SendRecvBuffer) )
							{
								memcpy ((char*)ucf01SendRecvBuffer, p_data, data_len);
							}
							responceUCF01FetchNotReceived = 0; // flag the main thread that data is received.
						}
						break;
						
					default:
						break;
					}
					break;
					default:
						break;
		}
	}
}

void CMonitorView::LogMessageHandler2()
{
	pPTE_MSG       p_msg_in;
	LONG           data_len;
	pBYTE          p_data;
	pPTE_MSG_DATA  p_msg_data;
	CHAR           error_msg[256];
	CHAR           temp_str[100];
	BYTE           dberror;
	BYTE           msgtype;
	pCHAR          orig_que = NULL_PTR;
	NCF01_GUI_LIST ncf01_list;
	INT            num_returned, index;
	OPR_MESSAGE    opr_msg;
	NCF01          ncf01;
	HANDLE hThread;
	DWORD  ThreadId;
	CString msg;
	INT ret=0;
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	MT_USER_INFO	 tmpUserInfo;
	
	
	
	while( 1 )
	{
		p_msg_in = PtetcpReceive2( 0, error_msg );
		if ( p_msg_in == NULL_PTR )
		{
			return;
		}
		
		p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
		
		msgtype = ptemsg_get_msg_type( p_msg_in );
		switch( msgtype )
		{
		case MT_MONITOR_CLEAR_SOCKETS:
			switch( ptemsg_get_msg_subtype1( p_msg_in ) )
			{
			case ST1_MONITOR_CLEAR_SOCKETS:
				//Monitor closed by the server.
				
				//close_dco_monitor();

				AfxMessageBox("MONITOR2 GH2: Server closed the connection. Exiting..", MB_SETFOREGROUND | MB_ICONSTOP | MB_ICONHAND | MB_OK );
				//EnableDialogButtons(FALSE, FALSE);

				// Exit this log thread as well as the main application here.
				
				ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
				AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);  // below 2 line replaced for above 2 lines fix the monitor issue.
				
				CloseHandle(this);
				ExitThread(0);
				//return;
				//break;
			}
			break;
			case MT_SYS_MONITOR:
				{
					
					switch( ptemsg_get_msg_subtype1( p_msg_in ) )
					{
					case ST1_OPRMSG:
						memset( &opr_msg, 0, sizeof( OPR_MESSAGE ) );
						memcpy( &opr_msg, (OPR_MESSAGE *)p_data, sizeof( OPR_MESSAGE ) );
						InsertLogMessage2( &opr_msg );
						break;
						
					case ST1_NETWORK_UPDATE:
						memset( &ncf01, 0, sizeof( NCF01 ) );
						memcpy( &ncf01, (pNCF01)p_data, sizeof( NCF01 ) );
						index = FindNCF01Record2( (pCHAR)ncf01.primary_key.network_id,
							(pCHAR)ncf01.primary_key.network_type );
						if( index != -1 )
						{
							strcpy( (pCHAR)m_ncf01_local_table2 [index].status, (pCHAR)ncf01.status );
							m_NetworkStatus2.ResetContent();
							m_NetworkStatus2.UpdateWindow();
							PopulateNetworkStatusList2();
						}
						break;
						
					case ST1_MONITOR_UP:
						
						m_bMonitorUp2 = true;
						switch( ptemsg_get_msg_subtype2( p_msg_in ) )
						{
						case ST2_MONITOR_ALREADY_RUNNING:
							m_bMonitorUp2 = false;
							// Someone is already connected - you are not allowed to connect. Use override.
							AfxMessageBox( "MONITOR2 GH2: ERROR - Two Monitors are already connected from other Systems.", MB_SYSTEMMODAL | MB_OK );
							EnableDialogButtons2(FALSE, FALSE);

							//OnCloseBtn();

							//ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
							//AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE); 
							
							//new changes - amith
							//don't close monitors, clear the initialized data
							m_CntlNetTranList2.DeleteAllItems(ItemdataProc, (LPARAM)this);
							m_TranActivityList2.ResetContent();
							m_DialogConnectStatus2.ResetContent();
							m_LogMessages2.ResetContent();
							DisableAllButtons(2);

							return;	// Exit thread. Close Application.
							break; // Not required, but kept for the sake of correctness.
							
						case ST2_MONITOR_READ_ONLY:
							
								memset(&tmpUserInfo, 0x00, sizeof(MT_USER_INFO));
								
								if(p_data != NULL)
								{
									memcpy( &tmpUserInfo, (pMT_USER_INFO)p_data, sizeof( MT_USER_INFO ) );
								}
							
							// Monitor was already running so this is a read-only connection.
								if(strlen((char *)tmpUserInfo.user_name) == 0)
								{
									strcpy((char *)tmpUserInfo.user_name, "UNKNOWN");
								}
							msg.Format("MONITOR2 GH2: Primary Monitor is already connected. This will be a read-only session.\n %s user has already connected", tmpUserInfo.user_name);
						//	AfxMessageBox( msg, MB_YESNO | MB_ICONSTOP  );
							

							/*AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
							AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
							/*ret = AfxMessageBox( msg , MB_YESNO | MB_ICONQUESTION );
							if( ret == IDNO )
								break ;
							else*/
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
								EnableDialogButtons2(FALSE, TRUE);
							
							break;
							
						case ST2_MONITOR_RECONNECT:
							// Nothing to do here.
							break;
							
						default:
							{
								orig_que = ptemsg_get_msg_orig_queue( p_msg_in );
								
								if( 0 == strcmp(orig_que,txcntl_que_name) )
								{
									/* Sleep 2 seconds to make sure user sees state change. */
									Sleep( 2000 );
									
									/* Reset the Test Result field. */
									m_TestResult2.SetBitmap( m_hTestOff2 );
									
									/* Log a message to indicate successful test. */
									memset( &opr_msg, 0x00, sizeof(OPR_MESSAGE) );
									
									strcpy( (pCHAR)opr_msg.message, "Monitor is operational. Successful Test");
									strcpy( (pCHAR)opr_msg.module,	"Monitor" );
									strcpy( (pCHAR)opr_msg.message_type, "INFO" );
									opr_msg.severity = '1';
									
									InsertLogMessage2( &opr_msg );
								}
								else		// Buttons should not get enabled when a read-only monitor receives a response for Test button - which is same as a full monitor response. Difference is only in the initial response for a monitor connection.
									EnableDialogButtons2(TRUE, FALSE); // enable buttons now.
							}
							break;
						}
						break;
						
						default:
							break;
					}
				}
				break;
				
				case MT_SYSTEM_REPLY:
					process_ping2(p_msg_in,msgtype);
					break;
					
				case MT_COM_REPLY:
				case MT_GET_STATS_RESPONSE:
					{
					/* Received a response from a dialog service or from a device
					* or network controller.  This response contains either a
					* dialog's 'connect' status, or a controller's transaction
					* statistics.
						*/
						process_stats2( p_msg_in, msgtype );
					}
					break;
					
				case MT_DB_REPLY:
					
					switch(ptemsg_get_msg_subtype1( p_msg_in ))
					{
					case ST1_DB_GET_GUI_LIST:
						{
							//SetEvent( m_DataServerReply2 );

							p_data = p_data + sizeof(AUTH_TX);
							if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
							{
								strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
								sprintf( error_msg, "MONITOR2 GH2: Database Error: %s", temp_str );
								MessageBox( error_msg );
								break;
							}
							
							memset( &ncf01_list, 0, sizeof( ncf01_list ) );
							memcpy( &ncf01_list, (pNCF01_GUI_LIST)p_data, sizeof( ncf01_list ) );
							
							num_returned = atoi( (pCHAR)ncf01_list.num_returned );
							// set all the status fields to '2'. This means status unknown
							for( INT loop = 0; loop < num_returned; loop++ )
							{
								bool insert = true;
								for(int xy=0;xy<Controller_Count2 && insert;xy++)
								{
									if(strcmp((pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id, Controller_Stats2[xy].service_id) == 0)
									{
										insert = false;
									}
								}
								if(insert)
								{
									strcpy(Controller_Stats2[Controller_Count2].service_id, (pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id);
									Controller_Count2++;
								}
								
								strcpy( (pCHAR)ncf01_list.ncf01_record [loop].status, "2" );
							}
							if( (m_curr_index2 + num_returned) > NCF01_LOCAL_TABLESIZE )
							{
								MessageBox( "MONITOR2 GH2: NCF01 table contents exceeds local capacity. Truncating Data" );
								PopulateNetworkStatusList2();
								
								// now query each of the network handlers for the current status
								//	GetCurrentStatus();
								hThread = CreateThread( NULL,
									NULL,
									(LPTHREAD_START_ROUTINE)GetNetworkStatusThread2,
									this,
									NULL,
									&ThreadId );
								
								if( hThread == NULL )
								{
									AfxMessageBox( "MONITOR2 GH2: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
									return;
								}
							}
							else
							{
								memcpy( &m_ncf01_local_table2 [ m_curr_index2 ], ncf01_list.ncf01_record,
									( num_returned * sizeof( NCF01 ) ) );
								m_curr_index2 += num_returned;
								num_rows_returned = num_returned;
								if( num_returned < GUI_MAX_LIST_SIZE )
								{
									m_retrieval_done2 = 1;
									//SetEvent( m_DataServerReply2 );
									PopulateNetworkStatusList2();
									
									// now query each of the network handlers for the current status
									//	GetCurrentStatus();
									hThread = CreateThread( NULL,
										NULL,
										(LPTHREAD_START_ROUTINE)GetNetworkStatusThread2,
										this,
										NULL,
										&ThreadId );
									
									if( hThread == NULL )
									{
										AfxMessageBox( "MONITOR2 GH2: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
										return;
									}
								}
								else
								{
									strcpy( (pCHAR)m_ncf012.primary_key.network_id,
										(pCHAR)m_ncf01_local_table2 [m_curr_index2-1].primary_key.network_id );
									strcpy( (pCHAR)m_ncf012.primary_key.network_type,
										(pCHAR)m_ncf01_local_table2 [m_curr_index2-1].primary_key.network_type );
									GetNCF01List2();
								}
							}
						}
						break;
						
					case ST1_DB_SELECT:
						if ( ptemsg_get_pte_msg_data_app_data_type(p_msg_data) == UCF01_DATA )
						{
							// Copy the UCF01 record in the shared buffer, and flag the main thread.
							// We are waiting in the main GUI thread for this data.
							if (data_len < sizeof (ucf01SendRecvBuffer) )
							{
								memcpy ((char*)ucf01SendRecvBuffer, p_data, data_len);
							}
							responceUCF01FetchNotReceived2 = 0; // flag the main thread that data is received.
						}
						break;
						
					default:
						break;
					}
					break;
					default:
						break;
		}
	}
}

void CMonitorView::LogMessageHandler3()
{
	pPTE_MSG       p_msg_in;
	LONG           data_len;
	pBYTE          p_data;
	pPTE_MSG_DATA  p_msg_data;
	CHAR           error_msg[256];
	CHAR           temp_str[100];
	BYTE           dberror;
	BYTE           msgtype;
	pCHAR          orig_que = NULL_PTR;
	NCF01_GUI_LIST ncf01_list;
	INT            num_returned, index;
	OPR_MESSAGE    opr_msg;
	NCF01          ncf01;
	HANDLE hThread;
	DWORD  ThreadId;
	CString msg;
	INT ret=0;
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	MT_USER_INFO	 tmpUserInfo;
	
	
	
	while( 1 )
	{
		p_msg_in = PtetcpReceive3( 0, error_msg );
		if ( p_msg_in == NULL_PTR )
		{
			return;
		}
		
		p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
		
		msgtype = ptemsg_get_msg_type( p_msg_in );
		switch( msgtype )
		{
		case MT_MONITOR_CLEAR_SOCKETS:
			switch( ptemsg_get_msg_subtype1( p_msg_in ) )
			{
			case ST1_MONITOR_CLEAR_SOCKETS:
				//Monitor closed by the server.
				
				//close_dco_monitor();

				AfxMessageBox("MONITOR3 MKT1: Server closed the connection. Exiting..", MB_SETFOREGROUND | MB_ICONSTOP | MB_ICONHAND | MB_OK );
				//EnableDialogButtons(FALSE, FALSE);

				// Exit this log thread as well as the main application here.
				
				ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
				AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);  // below 2 line replaced for above 2 lines fix the monitor issue.
				
				CloseHandle(this);
				ExitThread(0);
				//return;
				//break;
			}
			break;
			case MT_SYS_MONITOR:
				{
					
					switch( ptemsg_get_msg_subtype1( p_msg_in ) )
					{
					case ST1_OPRMSG:
						memset( &opr_msg, 0, sizeof( OPR_MESSAGE ) );
						memcpy( &opr_msg, (OPR_MESSAGE *)p_data, sizeof( OPR_MESSAGE ) );
						InsertLogMessage3( &opr_msg );
						break;
						
					case ST1_NETWORK_UPDATE:
						memset( &ncf01, 0, sizeof( NCF01 ) );
						memcpy( &ncf01, (pNCF01)p_data, sizeof( NCF01 ) );
						index = FindNCF01Record3( (pCHAR)ncf01.primary_key.network_id,
							(pCHAR)ncf01.primary_key.network_type );
						if( index != -1 )
						{
							strcpy( (pCHAR)m_ncf01_local_table3 [index].status, (pCHAR)ncf01.status );
							m_NetworkStatus3.ResetContent();
							m_NetworkStatus3.UpdateWindow();
							PopulateNetworkStatusList3();
						}
						break;
						
					case ST1_MONITOR_UP:
						
						m_bMonitorUp3 = true;
						switch( ptemsg_get_msg_subtype2( p_msg_in ) )
						{
						case ST2_MONITOR_ALREADY_RUNNING:
							m_bMonitorUp3 = false;
							// Someone is already connected - you are not allowed to connect. Use override.
							AfxMessageBox( "MONITOR3 MKT1: ERROR - Two Monitors are already connected from other Systems", MB_SYSTEMMODAL | MB_OK );
							EnableDialogButtons3(FALSE, FALSE);

							//OnCloseBtn();

							//ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
							//AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE); 
							
							//new changes - amith
							//don't close monitors, clear the initialized data
							m_CntlNetTranList3.DeleteAllItems(ItemdataProc, (LPARAM)this);
							m_TranActivityList3.ResetContent();
							m_DialogConnectStatus3.ResetContent();
							m_LogMessages3.ResetContent();
							DisableAllButtons(3);

							return;	// Exit thread. Close Application.
							break; // Not required, but kept for the sake of correctness.
							
						case ST2_MONITOR_READ_ONLY:
							
								memset(&tmpUserInfo, 0x00, sizeof(MT_USER_INFO));
								
								if(p_data != NULL)
								{
									memcpy( &tmpUserInfo, (pMT_USER_INFO)p_data, sizeof( MT_USER_INFO ) );
								}
							
							// Monitor was already running so this is a read-only connection.
								if(strlen((char *)tmpUserInfo.user_name) == 0)
								{
									strcpy((char *)tmpUserInfo.user_name, "UNKNOWN");
								}
							msg.Format("MONITOR3 MKT1: Primary Monitor is already connected. This will be a read-only session.\n %s user has already connected", tmpUserInfo.user_name);
						//	AfxMessageBox( msg, MB_YESNO | MB_ICONSTOP  );
							

							/*AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
							AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
							/*ret = AfxMessageBox( msg , MB_YESNO | MB_ICONQUESTION );
							if( ret == IDNO )
								break ;
							else*/
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
								EnableDialogButtons3(FALSE, TRUE);
							
							break;
							
						case ST2_MONITOR_RECONNECT:
							// Nothing to do here.
							break;
							
						default:
							{
								orig_que = ptemsg_get_msg_orig_queue( p_msg_in );
								
								if( 0 == strcmp(orig_que,txcntl_que_name) )
								{
									/* Sleep 2 seconds to make sure user sees state change. */
									Sleep( 2000 );
									
									/* Reset the Test Result field. */
									m_TestResult3.SetBitmap( m_hTestOff3 );
									
									/* Log a message to indicate successful test. */
									memset( &opr_msg, 0x00, sizeof(OPR_MESSAGE) );
									
									strcpy( (pCHAR)opr_msg.message, "Monitor is operational. Successful Test");
									strcpy( (pCHAR)opr_msg.module,	"Monitor" );
									strcpy( (pCHAR)opr_msg.message_type, "INFO" );
									opr_msg.severity = '1';
									
									InsertLogMessage3( &opr_msg );
								}
								else		// Buttons should not get enabled when a read-only monitor receives a response for Test button - which is same as a full monitor response. Difference is only in the initial response for a monitor connection.
									EnableDialogButtons3(TRUE, FALSE); // enable buttons now.
							}
							break;
						}
						break;
						
						default:
							break;
					}
				}
				break;
				
				case MT_SYSTEM_REPLY:
					process_ping3(p_msg_in,msgtype);
					break;
					
				case MT_COM_REPLY:
				case MT_GET_STATS_RESPONSE:
					{
					/* Received a response from a dialog service or from a device
					* or network controller.  This response contains either a
					* dialog's 'connect' status, or a controller's transaction
					* statistics.
						*/
						process_stats3( p_msg_in, msgtype );
					}
					break;
					
				case MT_DB_REPLY:
					
					switch(ptemsg_get_msg_subtype1( p_msg_in ))
					{
					case ST1_DB_GET_GUI_LIST:
						{
							//SetEvent( m_DataServerReply3 );

							p_data = p_data + sizeof(AUTH_TX);
							if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
							{
								strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
								sprintf( error_msg, "MONITOR3 MKT1: Database Error: %s", temp_str );
								MessageBox( error_msg );
								break;
							}
							
							memset( &ncf01_list, 0, sizeof( ncf01_list ) );
							memcpy( &ncf01_list, (pNCF01_GUI_LIST)p_data, sizeof( ncf01_list ) );
							
							num_returned = atoi( (pCHAR)ncf01_list.num_returned );
							// set all the status fields to '2'. This means status unknown
							for( INT loop = 0; loop < num_returned; loop++ )
							{
								bool insert = true;
								for(int xy=0;xy<Controller_Count3 && insert;xy++)
								{
									if(strcmp((pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id, Controller_Stats3[xy].service_id) == 0)
									{
										insert = false;
									}
								}
								if(insert)
								{
									strcpy(Controller_Stats3[Controller_Count3].service_id, (pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id);
									Controller_Count3++;
								}
								
								strcpy( (pCHAR)ncf01_list.ncf01_record [loop].status, "2" );
							}
							if( (m_curr_index3 + num_returned) > NCF01_LOCAL_TABLESIZE )
							{
								MessageBox( "MONITOR3 MKT1: NCF01 table contents exceeds local capacity. Truncating Data" );
								PopulateNetworkStatusList3();
								
								// now query each of the network handlers for the current status
								//	GetCurrentStatus();
								hThread = CreateThread( NULL,
									NULL,
									(LPTHREAD_START_ROUTINE)GetNetworkStatusThread3,
									this,
									NULL,
									&ThreadId );
								
								if( hThread == NULL )
								{
									AfxMessageBox( "MONITOR3 MKT1: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
									return;
								}
							}
							else
							{
								memcpy( &m_ncf01_local_table3 [ m_curr_index3 ], ncf01_list.ncf01_record,
									( num_returned * sizeof( NCF01 ) ) );
								m_curr_index3 += num_returned;
								num_rows_returned = num_returned;
								if( num_returned < GUI_MAX_LIST_SIZE )
								{
									m_retrieval_done3 = 1;
									//SetEvent( m_DataServerReply3 );
									PopulateNetworkStatusList3();
									
									// now query each of the network handlers for the current status
									//	GetCurrentStatus();
									hThread = CreateThread( NULL,
										NULL,
										(LPTHREAD_START_ROUTINE)GetNetworkStatusThread3,
										this,
										NULL,
										&ThreadId );
									
									if( hThread == NULL )
									{
										AfxMessageBox( "MONITOR3 MKT1: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
										return;
									}
								}
								else
								{
									strcpy( (pCHAR)m_ncf013.primary_key.network_id,
										(pCHAR)m_ncf01_local_table3 [m_curr_index3-1].primary_key.network_id );
									strcpy( (pCHAR)m_ncf013.primary_key.network_type,
										(pCHAR)m_ncf01_local_table3 [m_curr_index3-1].primary_key.network_type );
									GetNCF01List3();
								}
							}
						}
						break;
						
					case ST1_DB_SELECT:
						if ( ptemsg_get_pte_msg_data_app_data_type(p_msg_data) == UCF01_DATA )
						{
							// Copy the UCF01 record in the shared buffer, and flag the main thread.
							// We are waiting in the main GUI thread for this data.
							if (data_len < sizeof (ucf01SendRecvBuffer) )
							{
								memcpy ((char*)ucf01SendRecvBuffer, p_data, data_len);
							}
							responceUCF01FetchNotReceived3 = 0; // flag the main thread that data is received.
						}
						break;
						
					default:
						break;
					}
					break;
					default:
						break;
		}
	}
}

void CMonitorView::LogMessageHandler4()
{
	pPTE_MSG       p_msg_in;
	LONG           data_len;
	pBYTE          p_data;
	pPTE_MSG_DATA  p_msg_data;
	CHAR           error_msg[256];
	CHAR           temp_str[100];
	BYTE           dberror;
	BYTE           msgtype;
	pCHAR          orig_que = NULL_PTR;
	NCF01_GUI_LIST ncf01_list;
	INT            num_returned, index;
	OPR_MESSAGE    opr_msg;
	NCF01          ncf01;
	HANDLE hThread;
	DWORD  ThreadId;
	CString msg;
	INT ret=0;
	// Disable buttons until we get a response from ATP. Enable them later.
	// This is required as we are supporting a read-only monitor as well.
	MT_USER_INFO	 tmpUserInfo;
	
	while( 1 )
	{
		p_msg_in = PtetcpReceive4( 0, error_msg );
		if ( p_msg_in == NULL_PTR )
		{
			return;
		}
		
		p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
		
		msgtype = ptemsg_get_msg_type( p_msg_in );
		switch( msgtype )
		{
		case MT_MONITOR_CLEAR_SOCKETS:
			switch( ptemsg_get_msg_subtype1( p_msg_in ) )
			{
			case ST1_MONITOR_CLEAR_SOCKETS:
				//Monitor closed by the server.
				
				//close_dco_monitor();

				AfxMessageBox("MONITOR4 MKT2: Server closed the connection. Exiting..", MB_SETFOREGROUND | MB_ICONSTOP | MB_ICONHAND | MB_OK );
				//EnableDialogButtons(FALSE, FALSE);

				// Exit this log thread as well as the main application here.
				
				ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
				AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);  // below 2 line replaced for above 2 lines fix the monitor issue.
				
				CloseHandle(this);
				ExitThread(0);
				//return;
				//break;
			}
			break;
			case MT_SYS_MONITOR:
				{
					
					switch( ptemsg_get_msg_subtype1( p_msg_in ) )
					{
					case ST1_OPRMSG:
						memset( &opr_msg, 0, sizeof( OPR_MESSAGE ) );
						memcpy( &opr_msg, (OPR_MESSAGE *)p_data, sizeof( OPR_MESSAGE ) );
						InsertLogMessage4( &opr_msg );
						break;
						
					case ST1_NETWORK_UPDATE:
						memset( &ncf01, 0, sizeof( NCF01 ) );
						memcpy( &ncf01, (pNCF01)p_data, sizeof( NCF01 ) );
						index = FindNCF01Record4( (pCHAR)ncf01.primary_key.network_id,
							(pCHAR)ncf01.primary_key.network_type );
						if( index != -1 )
						{
							strcpy( (pCHAR)m_ncf01_local_table4 [index].status, (pCHAR)ncf01.status );
							m_NetworkStatus4.ResetContent();
							m_NetworkStatus4.UpdateWindow();
							PopulateNetworkStatusList4();
						}
						break;
						
					case ST1_MONITOR_UP:
						
						m_bMonitorUp4 = true;
						switch( ptemsg_get_msg_subtype2( p_msg_in ) )
						{
						case ST2_MONITOR_ALREADY_RUNNING:
							m_bMonitorUp4 = false;
							// Someone is already connected - you are not allowed to connect. Use override.
							AfxMessageBox( "MONITOR4 MKT2: ERROR - Two Monitors are already connected from other System.", MB_SYSTEMMODAL | MB_OK );
							EnableDialogButtons4(FALSE, FALSE);

							//OnCloseBtn();

							//ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
							//AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE); 
							
							//new changes - amith
							//don't close monitors, clear the initialized data
							m_CntlNetTranList4.DeleteAllItems(ItemdataProc, (LPARAM)this);
							m_TranActivityList4.ResetContent();
							m_DialogConnectStatus4.ResetContent();
							m_LogMessages4.ResetContent();
							DisableAllButtons(4);

							return;	// Exit thread. Close Application.
							break; // Not required, but kept for the sake of correctness.
							
						case ST2_MONITOR_READ_ONLY:
							
								memset(&tmpUserInfo, 0x00, sizeof(MT_USER_INFO));
								
								if(p_data != NULL)
								{
									memcpy( &tmpUserInfo, (pMT_USER_INFO)p_data, sizeof( MT_USER_INFO ) );
								}
							
							// Monitor was already running so this is a read-only connection.
								if(strlen((char *)tmpUserInfo.user_name) == 0)
								{
									strcpy((char *)tmpUserInfo.user_name, "UNKNOWN");
								}
							msg.Format("MONITOR4 MKT2: Primary Monitor is already connected. This will be a read-only session.\n %s user has already connected", tmpUserInfo.user_name);
						//	AfxMessageBox( msg, MB_YESNO | MB_ICONSTOP  );
							

							/*AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
							AfxGetApp()->m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);*/
							/*ret = AfxMessageBox( msg , MB_YESNO | MB_ICONQUESTION );
							if( ret == IDNO )
								break ;
							else*/
							AfxMessageBox(msg , MB_OK | MB_SYSTEMMODAL  );
								EnableDialogButtons4(FALSE, TRUE);
							
							break;
							
						case ST2_MONITOR_RECONNECT:
							// Nothing to do here.
							break;
							
						default:
							{
								orig_que = ptemsg_get_msg_orig_queue( p_msg_in );
								
								if( 0 == strcmp(orig_que,txcntl_que_name) )
								{
									/* Sleep 2 seconds to make sure user sees state change. */
									Sleep( 2000 );
									
									/* Reset the Test Result field. */
									m_TestResult4.SetBitmap( m_hTestOff4 );
									
									/* Log a message to indicate successful test. */
									memset( &opr_msg, 0x00, sizeof(OPR_MESSAGE) );
									
									strcpy( (pCHAR)opr_msg.message, "Monitor is operational. Successful Test");
									strcpy( (pCHAR)opr_msg.module,	"Monitor" );
									strcpy( (pCHAR)opr_msg.message_type, "INFO" );
									opr_msg.severity = '1';
									
									InsertLogMessage4( &opr_msg );
								}
								else		// Buttons should not get enabled when a read-only monitor receives a response for Test button - which is same as a full monitor response. Difference is only in the initial response for a monitor connection.
									EnableDialogButtons4(TRUE, FALSE); // enable buttons now.
							}
							break;
						}
						break;
						
						default:
							break;
					}
				}
				break;
				
				case MT_SYSTEM_REPLY:
					process_ping4(p_msg_in,msgtype);
					break;
					
				case MT_COM_REPLY:
				case MT_GET_STATS_RESPONSE:
					{
					/* Received a response from a dialog service or from a device
					* or network controller.  This response contains either a
					* dialog's 'connect' status, or a controller's transaction
					* statistics.
						*/
						process_stats4( p_msg_in, msgtype );
					}
					break;
					
				case MT_DB_REPLY:
					
					switch(ptemsg_get_msg_subtype1( p_msg_in ))
					{
					case ST1_DB_GET_GUI_LIST:
						{
							//SetEvent( m_DataServerReply4 );

							p_data = p_data + sizeof(AUTH_TX);
							if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
							{
								strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
								sprintf( error_msg, "MONITOR4 MKT2: Database Error: %s", temp_str );
								MessageBox( error_msg );
								break;
							}
							
							memset( &ncf01_list, 0, sizeof( ncf01_list ) );
							memcpy( &ncf01_list, (pNCF01_GUI_LIST)p_data, sizeof( ncf01_list ) );
							
							num_returned = atoi( (pCHAR)ncf01_list.num_returned );
							// set all the status fields to '2'. This means status unknown
							for( INT loop = 0; loop < num_returned; loop++ )
							{
								bool insert = true;
								for(int xy=0;xy<Controller_Count4 && insert;xy++)
								{
									if(strcmp((pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id, Controller_Stats4[xy].service_id) == 0)
									{
										insert = false;
									}
								}
								if(insert)
								{
									strcpy(Controller_Stats4[Controller_Count4].service_id, (pCHAR)ncf01_list.ncf01_record [loop].primary_key.network_id);
									Controller_Count4++;
								}
								
								strcpy( (pCHAR)ncf01_list.ncf01_record [loop].status, "2" );
							}
							if( (m_curr_index4 + num_returned) > NCF01_LOCAL_TABLESIZE )
							{
								MessageBox( "MONITOR4 MKT2: NCF01 table contents exceeds local capacity. Truncating Data" );
								PopulateNetworkStatusList4();
								
								// now query each of the network handlers for the current status
								//	GetCurrentStatus();
								hThread = CreateThread( NULL,
									NULL,
									(LPTHREAD_START_ROUTINE)GetNetworkStatusThread4,
									this,
									NULL,
									&ThreadId );
								
								if( hThread == NULL )
								{
									AfxMessageBox( "MONITOR4 MKT2: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
									return;
								}
							}
							else
							{
								memcpy( &m_ncf01_local_table4 [ m_curr_index4 ], ncf01_list.ncf01_record,
									( num_returned * sizeof( NCF01 ) ) );
								m_curr_index4 += num_returned;
								num_rows_returned = num_returned;
								if( num_returned < GUI_MAX_LIST_SIZE )
								{
									m_retrieval_done4 = 1;
									//SetEvent( m_DataServerReply4 );
									PopulateNetworkStatusList4();
									
									// now query each of the network handlers for the current status
									//	GetCurrentStatus();
									hThread = CreateThread( NULL,
										NULL,
										(LPTHREAD_START_ROUTINE)GetNetworkStatusThread4,
										this,
										NULL,
										&ThreadId );
									
									if( hThread == NULL )
									{
										AfxMessageBox( "MONITOR4 MKT2: Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
										return;
									}
								}
								else
								{
									strcpy( (pCHAR)m_ncf014.primary_key.network_id,
										(pCHAR)m_ncf01_local_table4 [m_curr_index4-1].primary_key.network_id );
									strcpy( (pCHAR)m_ncf014.primary_key.network_type,
										(pCHAR)m_ncf01_local_table4 [m_curr_index4-1].primary_key.network_type );
									GetNCF01List4();
								}
							}
						}
						break;
						
					case ST1_DB_SELECT:
						if ( ptemsg_get_pte_msg_data_app_data_type(p_msg_data) == UCF01_DATA )
						{
							// Copy the UCF01 record in the shared buffer, and flag the main thread.
							// We are waiting in the main GUI thread for this data.
							if (data_len < sizeof (ucf01SendRecvBuffer) )
							{
								memcpy ((char*)ucf01SendRecvBuffer, p_data, data_len);
							}
							responceUCF01FetchNotReceived4 = 0; // flag the main thread that data is received.
						}
						break;
						
					default:
						break;
					}
					break;
					default:
						break;
		}
	}
}

void CMonitorView::close_dco_monitor()
{
	AfxMessageBox("DCO MONITOR : Server closed the connection. Exiting..", MB_SETFOREGROUND | MB_ICONSTOP | MB_ICONHAND | MB_OK );
				
	ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
	AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
		
	exit(0); //closes the application and all its threads
}

void CMonitorView::OnSaf() 
{
	int i;
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	BYTE        subtype1;
	INT         cnt = 0;
	INT         ret = 0;
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	
	memset (display_buff,   0, sizeof(display_buff)  );
	memset (str_network_id, 0, sizeof(str_network_id));
	memset (m_msg_data,     0, sizeof(m_msg_data)    );
	
	//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
		str.Format("MONITOR1 GH1: Are you sure to Get Advises for %s?", str_network_id);
		ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;

	//m_NetworkStatus.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined, no need to go to Pan Dialog screen.
	*/
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined*/
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		/* Display Primary Account List and process selection */
		ret = PANDlg.DoModal();
		if ( ret == IDCANCEL )
			return;
		
		/* Memcpy the selected account number into the auth_tx field. */
		memset( &m_auth_tx, 0, sizeof(m_auth_tx) );
		strcpy( (char *)m_auth_tx.TLF01_details.card_num, PANDlg.m_pan );
		memcpy( m_msg_data, &m_auth_tx, sizeof( m_auth_tx ) );
	}
	
	/* Display ADVICES window to allow selection of Advice type. */
	Advices  AdviceDlg;
	ret = AdviceDlg.DoModal();
	if ( (ret != IDC_EXIT) && (ret != IDCANCEL) )
	{
		switch( ret )
		{
		case IDC_START_ADVICES:  subtype1 = ST1_PROCESS_START; break;
		case IDC_STOP_ADVICES:   subtype1 = ST1_SCH_CANCELJOB; break;
		case IDC_SAF:            subtype1 = ST1_SAF_REQUEST;   break;
		}
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			subtype1,
			0,
			(pCHAR)m_ncf01_local_table[table_index].que_name,
			interactive_que_name,
			(pBYTE)m_msg_data,
			sizeof( m_msg_data ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		}
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				sprintf( buffer,
					"MONITOR1 GH1: Communication error sending Logon Request: %s",
					error_msg );
				AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			}
			free( p_msg_out );
		}
	}
	return;
}

void CMonitorView::OnTimer(UINT nIDEvent) 
{
	
	if(nIDEvent == Timer && m_bMonitorUp)
	{
		// Refesh Timer
		/*Update Dialog Statuses */
		UpdateDialogStatusList();		
		
		/*Update Controller Stats*/
		UpdateControllerStatsList();
		
		/*send request to controllers and dialog to send current stats*/
		send_for_stats();
				
		/*Enable Reset Counts Button*/	
		m_CntlResetCountsButton.EnableWindow(!m_bReadOnly);
	}
	if(nIDEvent == (Timer+1) && m_bMonitorUp)
	{
		if(nc_status == RED && !m_CntlMuteBeep.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
		}
	}

	if(nIDEvent == Timer2 && m_bMonitorUp2)
	{
		// Refesh Timer
		/*Update Dialog Statuses */
		UpdateDialogStatusList2();
			
		/*Update Controller Stats*/
		UpdateControllerStatsList2();
		
		/*send request to controllers and dialog to send current stats*/
		send_for_stats2();
			
		/*Enable Reset Counts Button*/	
		m_CntlResetCountsButton2.EnableWindow(!m_bReadOnly2);
	}
	if(nIDEvent == (Timer2+1) && m_bMonitorUp2)
	{
		if(nc_status2 == RED && !m_CntlMuteBeep2.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
		}
	}

	if(nIDEvent == Timer3 && m_bMonitorUp3)
	{
		// Refesh Timer
		/*Update Dialog Statuses */
		UpdateDialogStatusList3();
			
		/*Update Controller Stats*/
		UpdateControllerStatsList3();
		
		/*send request to controllers and dialog to send current stats*/
		send_for_stats3();
			
		/*Enable Reset Counts Button*/	
		m_CntlResetCountsButton3.EnableWindow(!m_bReadOnly3);
	}
	if(nIDEvent == (Timer3+1) && m_bMonitorUp3)
	{
		if(nc_status3 == RED && !m_CntlMuteBeep3.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
		}
	}
		
	if(nIDEvent == Timer4 && m_bMonitorUp4)
	{
		// Refesh Timer
		/*Update Dialog Statuses */
		UpdateDialogStatusList4();
			
		/*Update Controller Stats*/
		UpdateControllerStatsList4();
		
		/*send request to controllers and dialog to send current stats*/
		send_for_stats4();
			
		/*Enable Reset Counts Button*/	
		m_CntlResetCountsButton4.EnableWindow(!m_bReadOnly4);
	}
	if(nIDEvent == (Timer4+1) && m_bMonitorUp4)
	{
		if(nc_status4 == RED && !m_CntlMuteBeep4.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CMonitorView::OnResetCountsButton() 
{
		INT ret = AfxMessageBox( "MONITOR1 GH1: Are you sure to Reset All the counts?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	/*Set activity counts to zero*/
	//InitializeActivityStatsList();
	if(m_CntlNetTranList.Lock() == 0)
	{
		AfxMessageBox("MONITOR1 GH1: List is being updated. Please try again.");
		return;
	}
	int count = m_CntlNetTranList.GetItemCount();
	for(int i=0;i<count;i++)
	{
		m_CntlNetTranList.SetItemText(i, LS_SENT, "0");
		m_CntlNetTranList.SetItemText(i, LS_RECEIVED,"0");
		m_CntlNetTranList.SetItemText(i, LS_SENT,  "0");
		m_CntlNetTranList.SetItemText(i, LS_RATE,  "0.0");
		m_CntlNetTranList.SetItemText(i, LS_APPROVED,  "0");
		m_CntlNetTranList.SetItemText(i, LS_DECLINED,  "0");
		m_CntlNetTranList.SetItemText(i, LS_TIMEDOUT,  "0");
		m_CntlNetTranList.SetItemText(i, LS_REVERSED,  "0");
		m_CntlNetTranList.SetItemText(i, LS_MGT_COUNTS,  "0");
		m_CntlNetTranList.SetItemText(i, LS_STIP_SENT,  "0");
		m_CntlNetTranList.SetItemText(i, LS_STIP_RECV,  "0");
	}
	m_CntlNetTranList.UnLock();

		/*Send MT_RESET_STATS to all Controllers*/
		SendResetControllerStats();
		
		/*Disable Reset Counts Button*/
		m_CntlResetCountsButton.EnableWindow(FALSE);
	
	
}

BOOLEAN read_id_ex( FILE **fp, pCHAR id_string )
{
#define  MAX_LEN  MAX_QUE_NAME_SIZE+MAX_INSTANCE_NAME_SIZE
	
	BOOLEAN   rc = true;
	CHAR      temp_id_string[MAX_LEN];
	INT       len;
	INT       i;
	
	if ( NULL == fgets(id_string, MAX_LEN, *fp) )
	{
		/* Either an error occurred, or we reached the end of file. */
		rc = false;
	}
	else
	{
	/* The read was successful.
	* Remove '\n' character at end of string.
	* Check for end of list, blank line, or a section name.
		*/
		len = strlen(id_string);
		if ( len > 0 )
			id_string[len-1] = '\0';
		
		if ( id_string[0] == '[' )
		{
			/* New section - end of current list. */
			rc = false;
		}
		else if ( blank_or_null_str(id_string, len) )
		{
			/* Blank line */
			rc = false;
		}
		else
		{
			/* Convert to uppercase to test for string of END OF LIST. */
			memset(temp_id_string, 0x00, sizeof(temp_id_string) );
			strcpy( temp_id_string, id_string );
			for( i=0; i<len; i++ )
				temp_id_string[i] = toupper( id_string[i] );
			
			if ( 0 == strcmp(temp_id_string, END_OF_LIST) )
				rc = false;
		}
	}
	return( rc );
}

INT GetPrivateProfileListEx( pCHAR filename, pCHAR section_name,
							pCHAR list,     INT   size_of_element )
{
	INT      rc  = 0;
	BOOLEAN  end_of_list;
	CHAR     id_string[MAX_QUE_NAME_SIZE+MAX_INSTANCE_NAME_SIZE + 1];
	FILE     *fp    =  NULL;
	FILE     **p_fp =  &(fp);
	
	fp = fopen(filename,"r");
	
	if ( fp == NULL)
	{
		/* Unable to open the file. */
		rc = -1;
	}
	else 
	{
		if ( false == find_section_name(p_fp, section_name) )
		{
			/* Cannot find section name in file. */
			rc = -2;
		}
		else
		{
			/* Read through the list of ID's for the given section. */
			end_of_list = false;
			while( !end_of_list )
			{
				memset( id_string, 0x00, sizeof(id_string) );
				if ( false == read_id_ex(p_fp, id_string) )
					end_of_list = true;
				else
				{
				/* Successful read - result is in id_string.
				* Make sure we have room to store it.
					*/
					if ( rc >= MAX_INI_ID_LIST_SIZE )
					{
					/* Out of storage space.
					* Do not read anymore.
					* Do not store the ID we just read.
						*/
						end_of_list = true;
					}
					else if ( !copy_id_to_list(id_string,list,size_of_element,rc) )
					{
						/* 'list' is too small to hold this ID. */
						rc = -3;
						end_of_list = true;
					}
					else
					{
						++rc;
						list += size_of_element;
					}
				}
			} /* End of WHILE Loop */
		}
		fclose(fp);
	}
	return( rc );
}

/******************************************************************************
*
*  NAME:         get_ini_info
*
*  DESCRIPTION:  This function reads information from the monitor ini file.
*
*  INPUTS:       None
*
*  OUTPUTS:      
*
*  RTRN VALUE:   True if successful, False if a problem occurred
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:
******************************************************************************/
INT get_ini_info()
{
	INT   ret_code;
	INT   num_of_ids;
	INT   size;
	CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
	CHAR  filename     [MAX_APP_NAME_SIZE];
	CHAR  tmpstr       [MAX_APP_NAME_SIZE];
	CHAR  tmpstring    [10];
	CHAR  ini_info     [10];
	INT    i;

		
	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MONITOR_INI_FILE);
	
	
	/*--------------------*/
	/* Get Timer Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	Timer_Interval = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( Timer_Interval < 1 )
	{
		Timer_Interval = atoi(DEFAULT_TIMER_VALUE);
	}
	Timer_Interval *= 1000;  /* Convert to milliseconds */
	

	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"NETWORK_BEEP_INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_NCStatus_Beep_Interval = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( _NCStatus_Beep_Interval < 1 )
	{
		_NCStatus_Beep_Interval = atoi(DEFAULT_TIMER_VALUE);
	}
	_NCStatus_Beep_Interval *= 1000;  /* Convert to milliseconds */
	

	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"MUTEON",                 /* points to key name           */
		"0",       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_MuteOn = atoi(tmpstring);
	

	/*------------------------*/
	/* Get List of Dialog IDs */
	/*------------------------*/
	ret_code = true;
	Dialog_ID_Count = 0;
	size = sizeof(DIALOG_STATUS);
	memset( &Dialog_Status, 0x00, size * MAX_INI_ID_LIST_SIZE );
	num_of_ids = GetPrivateProfileListEx( filename, DIALOG_SECTION,
		(pCHAR)&Dialog_Status, size );

	if ( num_of_ids < 0 )
	{
		strcpy( sectionname, DIALOG_SECTION );
		ret_code = false;
	}
	else
	{
		Dialog_ID_Count = num_of_ids;
		
		for(i=0; i< Dialog_ID_Count;i++)
		{
			char temp_name[MAX_INSTANCE_NAME_SIZE+1] = {0};
			Dialog_Status[i].flag = 0;
			char *pch = strchr(Dialog_Status[i].dialog_name,'=');
			if(pch != NULL)
			{
				pch++;
				*(pch-1) = '\0';
				strcpy(temp_name, pch);
				strcpy(Dialog_Status[i].dialog_id, Dialog_Status[i].dialog_name);
				strcpy(Dialog_Status[i].dialog_name, temp_name);
				
			}
			else
			{
				strcpy(Dialog_Status[i].dialog_id, Dialog_Status[i].dialog_name);
			}
			
		}
		
	/*--------------------------------*/
		/* Get List of Device Controllers */
		/*--------------------------------*/
		Controller_Count = 0;
		size = sizeof(CONTROLLER_STATS);
		
		/* Initialize size times '2' to account for DC's and NC's. */
		memset( &Controller_Stats, 0x00, size * MAX_INI_ID_LIST_SIZE * 2 );
		
		/* The 'size' given must be for only the DC's, so no '*2'. */
		num_of_ids = GetPrivateProfileList( filename, DC_SECTION,
			(pCHAR)&Controller_Stats, size );

		
		if ( num_of_ids < 0 )
		{
			strcpy( sectionname, DC_SECTION );
			ret_code = false;
		}
		else
		{
			Controller_Count = num_of_ids;
			
			for(i = 0;i<num_of_ids;i++)
			{
				szDeviceControllers +=	Controller_Stats[i].service_id ;
				szDeviceControllers += "|";
			}
			/*---------------------------------*/
			/* Get List of Network Controllers */
			/*---------------------------------*/
			
			/* Network Controllers go into the same list as device controllers.
			* The list pointer must be modified to account for device
			* controllers that already reside in the list.
			*/
			
			num_of_ids = GetPrivateProfileList(filename,
				NC_SECTION,
				(pCHAR)&Controller_Stats[Controller_Count],
				size );

		
			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, NC_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count += num_of_ids;
			}

				num_of_ids = GetPrivateProfileList( filename, MRA_SECTION,
			(pCHAR)&Controller_Stats[Controller_Count], size );

			

			for(i = Controller_Count-1;i<Controller_Count;i++)
			{
				szMracon +=	Controller_Stats[i+1].service_id ;
			}

			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, MRA_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count += num_of_ids;
			}

		}
	}
	for(i=0; i< Controller_Count;i++)
	{
		Controller_Stats[i].flag = 0;
	}

	/* Report any errors. */
	if ( ret_code == false )
	{
		if ( num_of_ids == -1 )
			sprintf( tmpstring, "MONITOR1 GH1: Unable to open ini file: %s", filename );
		
		else if ( num_of_ids == -2 )
			sprintf( tmpstring, "MONITOR1 GH1: Cannot find section %s in ini file: %s",
			sectionname, filename );
		
		else if ( num_of_ids == -3 )
			sprintf( tmpstring, 
			"MONITOR1 GH1: Size of list is too small. File: %s, section: %s",
			filename, sectionname );
		
		AfxMessageBox( tmpstring, MB_OK | MB_ICONSTOP );
	}
	return( ret_code );
}

INT get_ini_info2()
{
	INT   ret_code;
	INT   num_of_ids;
	INT   size;
	CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
	CHAR  filename     [MAX_APP_NAME_SIZE];
	CHAR  tmpstr       [MAX_APP_NAME_SIZE];
	CHAR  tmpstring    [10];
	CHAR  ini_info     [10];
	INT    i;
	
	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MONITOR_INI_FILE);
	
	/*--------------------*/
	/* Get Timer Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	Timer_Interval2 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( Timer_Interval2 < 1 )
	{
		Timer_Interval2 = atoi(DEFAULT_TIMER_VALUE);
	}
	Timer_Interval2 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"NETWORK_BEEP_INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_NCStatus_Beep_Interval2 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( _NCStatus_Beep_Interval2 < 1 )
	{
		_NCStatus_Beep_Interval2 = atoi(DEFAULT_TIMER_VALUE);
	}
	_NCStatus_Beep_Interval2 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"MUTEON",                 /* points to key name           */
		"0",       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_MuteOn2 = atoi(tmpstring);
	
	/*------------------------*/
	/* Get List of Dialog IDs */
	/*------------------------*/
	ret_code = true;
	Dialog_ID_Count2 = 0;
	size = sizeof(DIALOG_STATUS);
	memset( &Dialog_Status2, 0x00, size * MAX_INI_ID_LIST_SIZE );
	num_of_ids = GetPrivateProfileListEx( filename, DIALOG_SECTION,
		(pCHAR)&Dialog_Status2, size );

	if ( num_of_ids < 0 )
	{
		strcpy( sectionname, DIALOG_SECTION );
		ret_code = false;
	}
	else
	{
		Dialog_ID_Count2 = num_of_ids;
		
		for(i=0; i< Dialog_ID_Count2;i++)
		{
			char temp_name[MAX_INSTANCE_NAME_SIZE+1] = {0};
			Dialog_Status2[i].flag = 0;
			char *pch = strchr(Dialog_Status2[i].dialog_name,'=');
			if(pch != NULL)
			{
				pch++;
				*(pch-1) = '\0';
				strcpy(temp_name, pch);
				strcpy(Dialog_Status2[i].dialog_id, Dialog_Status2[i].dialog_name);
				strcpy(Dialog_Status2[i].dialog_name, temp_name);			
			}
			else
			{
				strcpy(Dialog_Status2[i].dialog_id, Dialog_Status2[i].dialog_name);
			}		
		}
		
		/*--------------------------------*/
		/* Get List of Device Controllers */
		/*--------------------------------*/
		Controller_Count2 = 0;
		size = sizeof(CONTROLLER_STATS);
		
		/* Initialize size times '2' to account for DC's and NC's. */
		memset( &Controller_Stats2, 0x00, size * MAX_INI_ID_LIST_SIZE * 2 );
		
		/* The 'size' given must be for only the DC's, so no '*2'. */
		num_of_ids = GetPrivateProfileList( filename, DC_SECTION,
			(pCHAR)&Controller_Stats2, size );
		
		if ( num_of_ids < 0 )
		{
			strcpy( sectionname, DC_SECTION );
			ret_code = false;
		}
		else
		{
			Controller_Count2 = num_of_ids;
			
			for(i = 0;i<num_of_ids;i++)
			{
				szDeviceControllers2 +=	Controller_Stats2[i].service_id ;
				szDeviceControllers2 += "|";
			}
			/*---------------------------------*/
			/* Get List of Network Controllers */
			/*---------------------------------*/
			
			/* Network Controllers go into the same list as device controllers.
			* The list pointer must be modified to account for device
			* controllers that already reside in the list.
			*/
			
			num_of_ids = GetPrivateProfileList(filename,
				NC_SECTION,
				(pCHAR)&Controller_Stats2[Controller_Count2],
				size );
		
			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, NC_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count2 += num_of_ids;
			}
			num_of_ids = GetPrivateProfileList( filename, MRA_SECTION,
			(pCHAR)&Controller_Stats2[Controller_Count2], size );

			for(i = Controller_Count2-1;i<Controller_Count2;i++)
			{
				szMracon2 +=	Controller_Stats2[i+1].service_id ;
			}

			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, MRA_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count2 += num_of_ids;
			}
		}
	}
	for(i=0; i< Controller_Count2;i++)
	{
		Controller_Stats2[i].flag = 0;
	}

	/* Report any errors. */
	if ( ret_code == false )
	{
		if ( num_of_ids == -1 )
			sprintf( tmpstring, "MONITOR2 GH2: Unable to open ini file: %s", filename );
		
		else if ( num_of_ids == -2 )
			sprintf( tmpstring, "MONITOR2 GH2: Cannot find section %s in ini file: %s",
			sectionname, filename );
		
		else if ( num_of_ids == -3 )
			sprintf( tmpstring, 
			"MONITOR2 GH2: Size of list is too small. File: %s, section: %s",
			filename, sectionname );
		
		AfxMessageBox( tmpstring, MB_OK | MB_ICONSTOP );
	}
	return( ret_code );
}

INT get_ini_info3()
{
	INT   ret_code;
	INT   num_of_ids;
	INT   size;
	CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
	CHAR  filename     [MAX_APP_NAME_SIZE];
	CHAR  tmpstr       [MAX_APP_NAME_SIZE];
	CHAR  tmpstring    [10];
	CHAR  ini_info     [10];
	INT    i;
	
	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MONITOR_INI_FILE);
	
	/*--------------------*/
	/* Get Timer Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	Timer_Interval3 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( Timer_Interval3 < 1 )
	{
		Timer_Interval3 = atoi(DEFAULT_TIMER_VALUE);
	}
	Timer_Interval3 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"NETWORK_BEEP_INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_NCStatus_Beep_Interval3 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( _NCStatus_Beep_Interval3 < 1 )
	{
		_NCStatus_Beep_Interval3 = atoi(DEFAULT_TIMER_VALUE);
	}
	_NCStatus_Beep_Interval3 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"MUTEON",                 /* points to key name           */
		"0",       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_MuteOn3 = atoi(tmpstring);
	
	/*------------------------*/
	/* Get List of Dialog IDs */
	/*------------------------*/
	ret_code = true;
	Dialog_ID_Count3 = 0;
	size = sizeof(DIALOG_STATUS);
	memset( &Dialog_Status3, 0x00, size * MAX_INI_ID_LIST_SIZE );
	num_of_ids = GetPrivateProfileListEx( filename, DIALOG_SECTION,
		(pCHAR)&Dialog_Status3, size );

	if ( num_of_ids < 0 )
	{
		strcpy( sectionname, DIALOG_SECTION );
		ret_code = false;
	}
	else
	{
		Dialog_ID_Count3 = num_of_ids;
		
		for(i=0; i< Dialog_ID_Count3;i++)
		{
			char temp_name[MAX_INSTANCE_NAME_SIZE+1] = {0};
			Dialog_Status3[i].flag = 0;
			char *pch = strchr(Dialog_Status3[i].dialog_name,'=');
			if(pch != NULL)
			{
				pch++;
				*(pch-1) = '\0';
				strcpy(temp_name, pch);
				strcpy(Dialog_Status3[i].dialog_id, Dialog_Status3[i].dialog_name);
				strcpy(Dialog_Status3[i].dialog_name, temp_name);			
			}
			else
			{
				strcpy(Dialog_Status3[i].dialog_id, Dialog_Status3[i].dialog_name);
			}		
		}
		
		/*--------------------------------*/
		/* Get List of Device Controllers */
		/*--------------------------------*/
		Controller_Count3 = 0;
		size = sizeof(CONTROLLER_STATS);
		
		/* Initialize size times '2' to account for DC's and NC's. */
		memset( &Controller_Stats3, 0x00, size * MAX_INI_ID_LIST_SIZE * 2 );
		
		/* The 'size' given must be for only the DC's, so no '*2'. */
		num_of_ids = GetPrivateProfileList( filename, DC_SECTION,
			(pCHAR)&Controller_Stats3, size );
		
		if ( num_of_ids < 0 )
		{
			strcpy( sectionname, DC_SECTION );
			ret_code = false;
		}
		else
		{
			Controller_Count3 = num_of_ids;
			
			for(i = 0;i<num_of_ids;i++)
			{
				szDeviceControllers3 +=	Controller_Stats3[i].service_id ;
				szDeviceControllers3 += "|";
			}
			/*---------------------------------*/
			/* Get List of Network Controllers */
			/*---------------------------------*/
			
			/* Network Controllers go into the same list as device controllers.
			* The list pointer must be modified to account for device
			* controllers that already reside in the list.
			*/
			
			num_of_ids = GetPrivateProfileList(filename,
				NC_SECTION,
				(pCHAR)&Controller_Stats3[Controller_Count3],
				size );
		
			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, NC_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count3 += num_of_ids;
			}
			num_of_ids = GetPrivateProfileList( filename, MRA_SECTION,
			(pCHAR)&Controller_Stats3[Controller_Count3], size );

			for(i = Controller_Count3-1;i<Controller_Count3;i++)
			{
				szMracon3 +=	Controller_Stats3[i+1].service_id ;
			}

			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, MRA_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count3 += num_of_ids;
			}
		}
	}
	for(i=0; i< Controller_Count3;i++)
	{
		Controller_Stats3[i].flag = 0;
	}

	/* Report any errors. */
	if ( ret_code == false )
	{
		if ( num_of_ids == -1 )
			sprintf( tmpstring, "MONITOR3 MKT1: Unable to open ini file: %s", filename );
		
		else if ( num_of_ids == -2 )
			sprintf( tmpstring, "MONITOR3 MKT1: Cannot find section %s in ini file: %s",
			sectionname, filename );
		
		else if ( num_of_ids == -3 )
			sprintf( tmpstring, 
			"MONITOR3 MKT1: Size of list is too small. File: %s, section: %s",
			filename, sectionname );
		
		AfxMessageBox( tmpstring, MB_OK | MB_ICONSTOP );
	}
	return( ret_code );
}

INT get_ini_info4()
{
	INT   ret_code;
	INT   num_of_ids;
	INT   size;
	CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
	CHAR  filename     [MAX_APP_NAME_SIZE];
	CHAR  tmpstr       [MAX_APP_NAME_SIZE];
	CHAR  tmpstring    [10];
	CHAR  ini_info     [10];
	INT    i;
	
	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, MONITOR_INI_FILE);
	
	/*--------------------*/
	/* Get Timer Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	Timer_Interval4 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( Timer_Interval4 < 1 )
	{
		Timer_Interval4 = atoi(DEFAULT_TIMER_VALUE);
	}
	Timer_Interval4 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"NETWORK_BEEP_INTERVAL",                 /* points to key name           */
		DEFAULT_TIMER_VALUE,       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_NCStatus_Beep_Interval4 = atoi(tmpstring);
	
	/* Check for legitimate value. */
	if ( _NCStatus_Beep_Interval4 < 1 )
	{
		_NCStatus_Beep_Interval4 = atoi(DEFAULT_TIMER_VALUE);
	}
	_NCStatus_Beep_Interval4 *= 1000;  /* Convert to milliseconds */
	
	/*--------------------*/
	/* Get Beep Interval */
	/*--------------------*/
	strcpy( sectionname, MONITOR_SECTION );
	memset( ini_info, 0x00, sizeof(ini_info) );
	ret_code = GetPrivateProfileString(
		sectionname,               /* points to section name       */
		"MUTEON",                 /* points to key name           */
		"0",       /* points to default string     */
		ini_info,                  /* points to destination buffer */
		sizeof(ini_info) - 1,      /* size of destination buffer   */
		filename                   /* points to ini filename       */
		);
	
	/* Convert entry from a string into a number. */
	memset( tmpstring, 0x00, sizeof(tmpstring) );
	memcpy( tmpstring, ini_info, sizeof(tmpstring)-1 );
	_MuteOn4 = atoi(tmpstring);
	
	/*------------------------*/
	/* Get List of Dialog IDs */
	/*------------------------*/
	ret_code = true;
	Dialog_ID_Count4 = 0;
	size = sizeof(DIALOG_STATUS);
	memset( &Dialog_Status4, 0x00, size * MAX_INI_ID_LIST_SIZE );
	num_of_ids = GetPrivateProfileListEx( filename, DIALOG_SECTION,
		(pCHAR)&Dialog_Status4, size );

	if ( num_of_ids < 0 )
	{
		strcpy( sectionname, DIALOG_SECTION );
		ret_code = false;
	}
	else
	{
		Dialog_ID_Count4 = num_of_ids;
		
		for(i=0; i< Dialog_ID_Count4;i++)
		{
			char temp_name[MAX_INSTANCE_NAME_SIZE+1] = {0};
			Dialog_Status4[i].flag = 0;
			char *pch = strchr(Dialog_Status4[i].dialog_name,'=');
			if(pch != NULL)
			{
				pch++;
				*(pch-1) = '\0';
				strcpy(temp_name, pch);
				strcpy(Dialog_Status4[i].dialog_id, Dialog_Status4[i].dialog_name);
				strcpy(Dialog_Status4[i].dialog_name, temp_name);			
			}
			else
			{
				strcpy(Dialog_Status4[i].dialog_id, Dialog_Status4[i].dialog_name);
			}		
		}
		
		/*--------------------------------*/
		/* Get List of Device Controllers */
		/*--------------------------------*/
		Controller_Count4 = 0;
		size = sizeof(CONTROLLER_STATS);
		
		/* Initialize size times '2' to account for DC's and NC's. */
		memset( &Controller_Stats4, 0x00, size * MAX_INI_ID_LIST_SIZE * 2 );
		
		/* The 'size' given must be for only the DC's, so no '*2'. */
		num_of_ids = GetPrivateProfileList( filename, DC_SECTION,
			(pCHAR)&Controller_Stats4, size );
		
		if ( num_of_ids < 0 )
		{
			strcpy( sectionname, DC_SECTION );
			ret_code = false;
		}
		else
		{
			Controller_Count4  = num_of_ids;
			
			for(i = 0;i<num_of_ids;i++)
			{
				szDeviceControllers4 +=	Controller_Stats4[i].service_id ;
				szDeviceControllers4 += "|";
			}
			/*---------------------------------*/
			/* Get List of Network Controllers */
			/*---------------------------------*/
			
			/* Network Controllers go into the same list as device controllers.
			* The list pointer must be modified to account for device
			* controllers that already reside in the list.
			*/
			
			num_of_ids = GetPrivateProfileList(filename,
				NC_SECTION,
				(pCHAR)&Controller_Stats4[Controller_Count4],
				size );
		
			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, NC_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count4 += num_of_ids;
			}
			num_of_ids = GetPrivateProfileList( filename, MRA_SECTION,
			(pCHAR)&Controller_Stats4[Controller_Count4], size );

			for(i = Controller_Count4-1;i<Controller_Count4;i++)
			{
				szMracon4 +=	Controller_Stats4[i+1].service_id ;
			}

			if ( num_of_ids < 0 )
			{
				strcpy( sectionname, MRA_SECTION );
				ret_code = false;
			}
			else
			{
				Controller_Count4 += num_of_ids;
			}
		}
	}
	for(i=0; i< Controller_Count4;i++)
	{
		Controller_Stats4[i].flag = 0;
	}

	/* Report any errors. */
	if ( ret_code == false )
	{
		if ( num_of_ids == -1 )
			sprintf( tmpstring, "MONITOR4 MKT2: Unable to open ini file: %s", filename );
		
		else if ( num_of_ids == -2 )
			sprintf( tmpstring, "MONITOR4 MKT2: Cannot find section %s in ini file: %s",
			sectionname, filename );
		
		else if ( num_of_ids == -3 )
			sprintf( tmpstring, 
			"MONITOR4 MKT2: Size of list is too small. File: %s, section: %s",
			filename, sectionname );
		
		AfxMessageBox( tmpstring, MB_OK | MB_ICONSTOP );
	}
	return( ret_code );
}
/******************************************************************************
*
*  NAME:         SEND_FOR_STATS
*
*  DESCRIPTION:  This function steps through the device/network controllers
*                list and the dialog list and sends a request to each ID in
*                the lists.  This request instructs the services to send their
*                statistics or status to the Monitor.
*
*  INPUTS:       None
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:
******************************************************************************/
void  CMonitorView::send_for_stats()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	CHAR queue1[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	for(i=0; i<Dialog_ID_Count; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Dialog_Status[i].dialog_id );
		strcat( queue, "A" );
		if( Dialog_Status[i].flag == 0)
		{
			ret_code = send_pte_msg( MT_COM_REQUEST, ST1_STATUS, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
				Dialog_Status[i].flag =1;
			
		}
	}
	for(i=0; i<Controller_Count; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats[i].service_id );
		
		if(strncmp(queue,"mracon",MRACON_LEN) == 0)
		{
			strncpy(queue1,"mracon",MRACON_LEN);
			memset( queue, 0x00, queue_size );
			strncpy(queue,"fconds", FCONDS_LEN);
		}
		strcat( queue, "A" );
		if ( Controller_Stats[i].flag == 0)
		{
			ret_code = send_pte_msg( MT_GET_STATS, 0, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
			{
				Controller_Stats[i].flag = 1;
			}
			if(strncmp(queue1,"mracon",MRACON_LEN) == 0)
			{
				ret_code = send_pte_msg( MT_GET_STATS, 0, "mraconA", err_msg );
			}
			
		}
	}
	return;
}

void  CMonitorView::send_for_stats2()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	CHAR queue1[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	for(i=0; i<Dialog_ID_Count2; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Dialog_Status2[i].dialog_id );
		strcat( queue, "A" );
		if( Dialog_Status2[i].flag == 0)
		{
			ret_code = send_pte_msg2( MT_COM_REQUEST, ST1_STATUS, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
				Dialog_Status2[i].flag =1;	
		}
	}
	for(i=0; i<Controller_Count2; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats2[i].service_id );
		
		if(strncmp(queue,"mracon",MRACON_LEN) == 0)
		{
			strncpy(queue1,"mracon",MRACON_LEN);
			memset( queue, 0x00, queue_size );
			strncpy(queue,"fconds", FCONDS_LEN);
		}
		strcat( queue, "A" );
		if ( Controller_Stats2[i].flag == 0)
		{
			ret_code = send_pte_msg2( MT_GET_STATS, 0, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
			{
				Controller_Stats2[i].flag = 1;
			}
			if(strncmp(queue1,"mracon",MRACON_LEN) == 0)
			{
				ret_code = send_pte_msg2( MT_GET_STATS, 0, "mraconA", err_msg );
			}	
		}
	}
	return;
}

void  CMonitorView::send_for_stats3()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	CHAR queue1[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	for(i=0; i<Dialog_ID_Count3; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Dialog_Status3[i].dialog_id );
		strcat( queue, "A" );
		if( Dialog_Status3[i].flag == 0)
		{
			ret_code = send_pte_msg3( MT_COM_REQUEST, ST1_STATUS, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
				Dialog_Status3[i].flag =1;	
		}
	}
	for(i=0; i<Controller_Count3; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats3[i].service_id );
		
		if(strncmp(queue,"mracon",MRACON_LEN) == 0)
		{
			strncpy(queue1,"mracon",MRACON_LEN);
			memset( queue, 0x00, queue_size );
			strncpy(queue,"fconds", FCONDS_LEN);
		}
		strcat( queue, "A" );
		if ( Controller_Stats3[i].flag == 0)
		{
			ret_code = send_pte_msg3( MT_GET_STATS, 0, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
			{
				Controller_Stats3[i].flag = 1;
			}
			if(strncmp(queue1,"mracon",MRACON_LEN) == 0)
			{
				ret_code = send_pte_msg3( MT_GET_STATS, 0, "mraconA", err_msg );
			}	
		}
	}
	return;
}

void  CMonitorView::send_for_stats4()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	CHAR queue1[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	for(i=0; i<Dialog_ID_Count4; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Dialog_Status4[i].dialog_id );
		strcat( queue, "A" );
		if( Dialog_Status4[i].flag == 0)
		{
			ret_code = send_pte_msg4( MT_COM_REQUEST, ST1_STATUS, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
				Dialog_Status4[i].flag =1;	
		}
	}
	for(i=0; i<Controller_Count4; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats4[i].service_id );
		
		if(strncmp(queue,"mracon",MRACON_LEN) == 0)
		{
			strncpy(queue1,"mracon",MRACON_LEN);
			memset( queue, 0x00, queue_size );
			strncpy(queue,"fconds", FCONDS_LEN);
		}
		strcat( queue, "A" );
		if ( Controller_Stats4[i].flag == 0)
		{
			ret_code = send_pte_msg4( MT_GET_STATS, 0, queue, err_msg );
			if ( ret_code == false )
				AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
			else
			{
				Controller_Stats4[i].flag = 1;
			}
			if(strncmp(queue1,"mracon",MRACON_LEN) == 0)
			{
				ret_code = send_pte_msg4( MT_GET_STATS, 0, "mraconA", err_msg );
			}	
		}
	}
	return;
}
/******************************************************************************
*
*  NAME:         SEND_RESET_CONTROLLER_STATS
*
*  DESCRIPTION:  This function steps through the device/network controllers
*                list and sends a request to each ID. This request instructs 
*		          the services to set their number of message sent and number of
*				  messages received to zero.
*
*  INPUTS:       None
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Linda Mitchell
*
*  MODIFICATIONS:
******************************************************************************/
void CMonitorView::SendResetControllerStats()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	
	for(i=0; i<Controller_Count; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats[i].service_id );
		strcat( queue, "A" );
		ret_code = send_pte_msg( MT_RESET_STATS, 0, queue, err_msg );
		if ( ret_code == false )
			AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::SendResetControllerStats2()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	
	for(i=0; i<Controller_Count2; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats2[i].service_id );
		strcat( queue, "A" );
		ret_code = send_pte_msg2( MT_RESET_STATS, 0, queue, err_msg );
		if ( ret_code == false )
			AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::SendResetControllerStats3()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	
	for(i=0; i<Controller_Count3; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats3[i].service_id );
		strcat( queue, "A" );
		ret_code = send_pte_msg3( MT_RESET_STATS, 0, queue, err_msg );
		if ( ret_code == false )
			AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::SendResetControllerStats4()
{
	INT  i;
	INT  ret_code;
	INT  queue_size;
	CHAR err_msg[100] = "";
	CHAR queue[MAX_QUE_NAME_SIZE+1];
	
	queue_size = sizeof(queue);
	
	for(i=0; i<Controller_Count4; i++)
	{
		memset( queue, 0x00, queue_size );
		strcpy( queue, Controller_Stats4[i].service_id );
		strcat( queue, "A" );
		ret_code = send_pte_msg4( MT_RESET_STATS, 0, queue, err_msg );
		if ( ret_code == false )
			AfxMessageBox( err_msg, MB_OK | MB_ICONSTOP );
	}
	return;
}
/******************************************************************************
*
*  NAME:         SEND_PTE_MSG
*
*  DESCRIPTION:  This function sends a PTE message to the queue specified.
*                The message type is specified.  All other data is non-
*                applicable, except the originator queue, which is specified
*                in this function.
*
*  INPUTS:       msgtype - PTE Message type
*				  subtype1- PTE Subtype (ST1_STATUS)
*                queue   - Destination queue
*
*  OUTPUTS:      err_msg - Textual description of an error condition
*
*  RTRN VALUE:   True if message is successfully sent, else false
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:
******************************************************************************/
INT CMonitorView::send_pte_msg( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg )
{
	pPTE_MSG    p_msg;
	INT         ret_code = true;
	CHAR        error_msg[200];
	
	p_msg = ptemsg_build_msg( msgtype, subtype1, 0, queue,
		interactive_que_name,
		NULL_PTR, 0, 0 );
	
	
	
	if ( NULL_PTR == p_msg  )
	{
		strcpy(err_msg,"MONITOR1 GH1: Insufficient Memory to build PTE msg for status request");
		ret_code = false;
	}
	else
	{
		if( !PtetcpSend( p_msg, error_msg ) )
		{
			strcpy( err_msg, "MONITOR1 GH1: Failed to transmit status request to queue: " );
			strcat( err_msg, queue );
			strcat( err_msg, ".  " );
			strncat( err_msg, error_msg, 40 );
			ret_code = false;
		} 
		
		free(p_msg);
	}
	return( ret_code );
	
}

INT CMonitorView::send_pte_msg2( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg )
{
	pPTE_MSG    p_msg;
	INT         ret_code = true;
	CHAR        error_msg[200];
	
	p_msg = ptemsg_build_msg( msgtype, subtype1, 0, queue,
		interactive_que_name,
		NULL_PTR, 0, 0 );
	
	
	
	if ( NULL_PTR == p_msg  )
	{
		strcpy(err_msg,"MONITOR2 GH2: Insufficient Memory to build PTE msg for status request");
		ret_code = false;
	}
	else
	{
		if( !PtetcpSend2( p_msg, error_msg ) )
		{
			strcpy( err_msg, "MONITOR2 GH2: Failed to transmit status request to queue: " );
			strcat( err_msg, queue );
			strcat( err_msg, ".  " );
			strncat( err_msg, error_msg, 40 );
			ret_code = false;
		} 
		
		free(p_msg);
	}
	return( ret_code );
	
}

INT CMonitorView::send_pte_msg3( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg )
{
	pPTE_MSG    p_msg;
	INT         ret_code = true;
	CHAR        error_msg[200];
	
	p_msg = ptemsg_build_msg( msgtype, subtype1, 0, queue,
		interactive_que_name,
		NULL_PTR, 0, 0 );
	
	if ( NULL_PTR == p_msg  )
	{
		strcpy(err_msg,"MONITOR3 MKT1: Insufficient Memory to build PTE msg for status request");
		ret_code = false;
	}
	else
	{
		if( !PtetcpSend3( p_msg, error_msg ) )
		{
			strcpy( err_msg, "MONITOR3 MKT1: Failed to transmit status request to queue: " );
			strcat( err_msg, queue );
			strcat( err_msg, ".  " );
			strncat( err_msg, error_msg, 40 );
			ret_code = false;
		} 
		
		free(p_msg);
	}
	return( ret_code );
	
}

INT CMonitorView::send_pte_msg4( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg )
{
	pPTE_MSG    p_msg;
	INT         ret_code = true;
	CHAR        error_msg[200];
	
	p_msg = ptemsg_build_msg( msgtype, subtype1, 0, queue,
		interactive_que_name,
		NULL_PTR, 0, 0 );
	
	if ( NULL_PTR == p_msg  )
	{
		strcpy(err_msg,"MONITOR4 MKT2: Insufficient Memory to build PTE msg for status request");
		ret_code = false;
	}
	else
	{
		if( !PtetcpSend4( p_msg, error_msg ) )
		{
			strcpy( err_msg, "MONITOR4 MKT2: Failed to transmit status request to queue: " );
			strcat( err_msg, queue );
			strcat( err_msg, ".  " );
			strncat( err_msg, error_msg, 40 );
			ret_code = false;
		} 
		
		free(p_msg);
	}
	return( ret_code );
	
}

/*************************************************************************************
   NAME:               UpdateControllerStatsList

   DESCRIPTION:        This function populates the Transaction Activity list box 
					   with the latest controller_stats                           
   INPUTS:
       None
   OUTPUTS:
       None
   RETURNS:           
       None
   AUTHOR:             Linda Mitchell 
*************************************************************************************/
void CMonitorView::UpdateControllerStatsList()
{
	CHAR  display_buff[512];
	
	CHAR  spaces_rate[MAX_SPACES] ="";
	CHAR  spaces_rec [MAX_SPACES] ="";
	CHAR  spaces_sent[MAX_SPACES] ="";
	CHAR  spaces_app [MAX_SPACES] ="";
	CHAR  spaces_decl[MAX_SPACES] ="";
	CHAR  spaces_time[MAX_SPACES] ="";
	CHAR  spaces_rev [MAX_SPACES] ="";
	CHAR	 spaces_mgt_request[MAX_SPACES] ="";//praneeth
	CHAR	 spaces_stip_sent[MAX_SPACES] ="";	//TF Anna
	CHAR	 spaces_stip_recv[MAX_SPACES] ="";	//TF Anna
	
	
	m_TranActivityList.ResetContent();
	
	for( INT i = 0; i < Controller_Count; i++ )
	{
		/* Initialize the space before each value to align the columns. */
		memset( spaces_rate, ' ', 2 );
		memset( spaces_rec,  ' ', MAX_SPACES-1 );
		memset( spaces_sent, ' ', MAX_SPACES-1 );
		memset( spaces_app,  ' ', MAX_SPACES-1 );
		memset( spaces_decl, ' ', MAX_SPACES-1 );
		memset( spaces_time, ' ', MAX_SPACES-1 );
		memset( spaces_rev,  ' ', MAX_SPACES-1 );
		memset( spaces_mgt_request,  ' ', MAX_SPACES-1 );//praneeth
		memset( spaces_stip_sent,  ' ', MAX_SPACES-1 );//TF Anna
		memset( spaces_stip_recv,  ' ', MAX_SPACES-1 );//TF Anna
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		/* Adjust the spacing to keep columns aligned. */
		if ( Controller_Stats[i].received_count > 9 )
			memset( spaces_rec, 0x00, MAX_SPACES );
		
		if ( Controller_Stats[i].sent_count > 9 )
		{
			memset( spaces_sent, 0x00, MAX_SPACES  );
			memset( spaces_rate, 0x00, MAX_SPACES  );
		}
		
		if ( Controller_Stats[i].approved_count > 9 )
			memset( spaces_app, 0x00, MAX_SPACES );
		
		if ( Controller_Stats[i].declined_count > 9 )
			memset( spaces_decl, 0x00, MAX_SPACES );
		
		if ( Controller_Stats[i].timeout_count > 9 )
			memset( spaces_time, 0x00, MAX_SPACES );
		
		if ( Controller_Stats[i].reversed_count > 9 )
			memset( spaces_rev, 0x00, MAX_SPACES );
		//praneeth
		if ( Controller_Stats[i].mgt_request_count > 9 )
			memset( spaces_mgt_request, 0x00, MAX_SPACES );
		//TF Anna
		if ( Controller_Stats[i].stip_sent_count > 9 )
			memset( spaces_stip_sent, 0x00, MAX_SPACES );
		if ( Controller_Stats[i].stip_recv_count > 9 )
			memset( spaces_stip_recv, 0x00, MAX_SPACES );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s",
			Controller_Stats[i].service_id,
			
			Controller_Stats[i].received_count,    spaces_rec,
			Controller_Stats[i].sent_count,        spaces_sent,
			Controller_Stats[i].rate,              spaces_rate,
			Controller_Stats[i].approved_count,    spaces_app,
			Controller_Stats[i].declined_count,    spaces_decl,
			Controller_Stats[i].timeout_count,     spaces_time,
			Controller_Stats[i].reversed_count,    spaces_rev,
			Controller_Stats[i].mgt_request_count, spaces_mgt_request,
			Controller_Stats[i].stip_sent_count, spaces_stip_sent,
			Controller_Stats[i].stip_recv_count, spaces_stip_recv
			);	 //praneeth
		
		/* m_TranActivityList.AddString( display_buff ); */ 
		if ( Controller_Stats[i].flag == 1)
			m_TranActivityList.InsertString(0,display_buff,RGB(255,0,0));
		else 
			m_TranActivityList.AddString( display_buff );
		
		if(szDeviceControllers.Find(Controller_Stats[i].service_id) > -1)
		{
			if ( Controller_Stats[i].flag == 1)
			{
				m_CntlNetTranList.SetRed(Controller_Stats[i].service_id, true);
				
				m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList.SetRed(Controller_Stats[i].service_id, false);
				m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_NETWORK_NAME, "DEVICE CONTROLLER");
		}

		//ramya
		if(szMracon.Find(Controller_Stats[i].service_id) > -1)
		{
			if ( Controller_Stats[i].flag == 1)
			{
				m_CntlNetTranList.SetRed(Controller_Stats[i].service_id, true);
				m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList.SetRed(Controller_Stats[i].service_id, false);
				m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList.UpdateItemText(Controller_Stats[i].service_id, LS_NETWORK_NAME, "MRACON");
		}

		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_RECEIVED,		Controller_Stats[i].received_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_SENT,			Controller_Stats[i].sent_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_RATE,			Controller_Stats[i].rate);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_APPROVED,		Controller_Stats[i].approved_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_DECLINED,		Controller_Stats[i].declined_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_TIMEDOUT,		Controller_Stats[i].timeout_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_REVERSED,		Controller_Stats[i].reversed_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_MGT_COUNTS,	Controller_Stats[i].mgt_request_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_STIP_SENT,		Controller_Stats[i].stip_sent_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_STIP_RECV,		Controller_Stats[i].stip_recv_count);
		
	}
	m_CntlNetTranList.Sort();
}

void CMonitorView::UpdateControllerStatsList2()
{
	CHAR  display_buff[512];
	
	CHAR  spaces_rate[MAX_SPACES] ="";
	CHAR  spaces_rec [MAX_SPACES] ="";
	CHAR  spaces_sent[MAX_SPACES] ="";
	CHAR  spaces_app [MAX_SPACES] ="";
	CHAR  spaces_decl[MAX_SPACES] ="";
	CHAR  spaces_time[MAX_SPACES] ="";
	CHAR  spaces_rev [MAX_SPACES] ="";
	CHAR	 spaces_mgt_request[MAX_SPACES] ="";//praneeth
	CHAR	 spaces_stip_sent[MAX_SPACES] ="";	//TF Anna
	CHAR	 spaces_stip_recv[MAX_SPACES] ="";	//TF Anna
	
	
	m_TranActivityList2.ResetContent();
	
	for( INT i = 0; i < Controller_Count2; i++ )
	{
		// Initialize the space before each value to align the columns. 
		memset( spaces_rate, ' ', 2 );
		memset( spaces_rec,  ' ', MAX_SPACES-1 );
		memset( spaces_sent, ' ', MAX_SPACES-1 );
		memset( spaces_app,  ' ', MAX_SPACES-1 );
		memset( spaces_decl, ' ', MAX_SPACES-1 );
		memset( spaces_time, ' ', MAX_SPACES-1 );
		memset( spaces_rev,  ' ', MAX_SPACES-1 );
		memset( spaces_mgt_request,  ' ', MAX_SPACES-1 );//praneeth
		memset( spaces_stip_sent,  ' ', MAX_SPACES-1 );//TF Anna
		memset( spaces_stip_recv,  ' ', MAX_SPACES-1 );//TF Anna
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		// Adjust the spacing to keep columns aligned.
		if ( Controller_Stats2[i].received_count > 9 )
			memset( spaces_rec, 0x00, MAX_SPACES );
		
		if ( Controller_Stats2[i].sent_count > 9 )
		{
			memset( spaces_sent, 0x00, MAX_SPACES  );
			memset( spaces_rate, 0x00, MAX_SPACES  );
		}
		
		if ( Controller_Stats2[i].approved_count > 9 )
			memset( spaces_app, 0x00, MAX_SPACES );
		
		if ( Controller_Stats2[i].declined_count > 9 )
			memset( spaces_decl, 0x00, MAX_SPACES );
		
		if ( Controller_Stats2[i].timeout_count > 9 )
			memset( spaces_time, 0x00, MAX_SPACES );
		
		if ( Controller_Stats2[i].reversed_count > 9 )
			memset( spaces_rev, 0x00, MAX_SPACES );
		//praneeth
		if ( Controller_Stats2[i].mgt_request_count > 9 )
			memset( spaces_mgt_request, 0x00, MAX_SPACES );
		//TF Anna
		if ( Controller_Stats2[i].stip_sent_count > 9 )
			memset( spaces_stip_sent, 0x00, MAX_SPACES );
		if ( Controller_Stats2[i].stip_recv_count > 9 )
			memset( spaces_stip_recv, 0x00, MAX_SPACES );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s",
			Controller_Stats2[i].service_id,
			
			Controller_Stats2[i].received_count,    spaces_rec,
			Controller_Stats2[i].sent_count,        spaces_sent,
			Controller_Stats2[i].rate,              spaces_rate,
			Controller_Stats2[i].approved_count,    spaces_app,
			Controller_Stats2[i].declined_count,    spaces_decl,
			Controller_Stats2[i].timeout_count,     spaces_time,
			Controller_Stats2[i].reversed_count,    spaces_rev,
			Controller_Stats2[i].mgt_request_count, spaces_mgt_request,
			Controller_Stats2[i].stip_sent_count, spaces_stip_sent,
			Controller_Stats2[i].stip_recv_count, spaces_stip_recv
			);	 //praneeth
		
		// m_TranActivityList.AddString( display_buff );  
		if ( Controller_Stats2[i].flag == 1)
		{
			m_TranActivityList2.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{
			m_TranActivityList2.AddString( display_buff );
		}
		
		if(szDeviceControllers2.Find(Controller_Stats2[i].service_id) > -1)
		{
			if ( Controller_Stats2[i].flag == 1)
			{
				m_CntlNetTranList2.SetRed(Controller_Stats2[i].service_id, true);
				m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList2.SetRed(Controller_Stats2[i].service_id, false);
				m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_NETWORK_NAME, "DEVICE CONTROLLER");
		}

		//ramya
		if(szMracon2.Find(Controller_Stats2[i].service_id) > -1)
		{
			if ( Controller_Stats2[i].flag == 1)
			{
				m_CntlNetTranList2.SetRed(Controller_Stats2[i].service_id, true);
				m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList2.SetRed(Controller_Stats2[i].service_id, false);
				m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList2.UpdateItemText(Controller_Stats2[i].service_id, LS_NETWORK_NAME, "MRACON");
		}

		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_RECEIVED,	Controller_Stats2[i].received_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_SENT,		Controller_Stats2[i].sent_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_RATE,		Controller_Stats2[i].rate);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_APPROVED,	Controller_Stats2[i].approved_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_DECLINED,	Controller_Stats2[i].declined_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_TIMEDOUT,	Controller_Stats2[i].timeout_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_REVERSED,	Controller_Stats2[i].reversed_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_MGT_COUNTS,  Controller_Stats2[i].mgt_request_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_STIP_SENT,	Controller_Stats2[i].stip_sent_count);
		m_CntlNetTranList2.UpdateItemText( Controller_Stats2[i].service_id, LS_STIP_RECV,	Controller_Stats2[i].stip_recv_count);		
	}
	m_CntlNetTranList2.Sort();
}

void CMonitorView::UpdateControllerStatsList3()
{
	CHAR  display_buff[512];
	
	CHAR  spaces_rate[MAX_SPACES] ="";
	CHAR  spaces_rec [MAX_SPACES] ="";
	CHAR  spaces_sent[MAX_SPACES] ="";
	CHAR  spaces_app [MAX_SPACES] ="";
	CHAR  spaces_decl[MAX_SPACES] ="";
	CHAR  spaces_time[MAX_SPACES] ="";
	CHAR  spaces_rev [MAX_SPACES] ="";
	CHAR	 spaces_mgt_request[MAX_SPACES] ="";//praneeth
	CHAR	 spaces_stip_sent[MAX_SPACES] ="";	//TF Anna
	CHAR	 spaces_stip_recv[MAX_SPACES] ="";	//TF Anna
	
	
	m_TranActivityList3.ResetContent();
	
	for( INT i = 0; i < Controller_Count3; i++ )
	{
		// Initialize the space before each value to align the columns. 
		memset( spaces_rate, ' ', 2 );
		memset( spaces_rec,  ' ', MAX_SPACES-1 );
		memset( spaces_sent, ' ', MAX_SPACES-1 );
		memset( spaces_app,  ' ', MAX_SPACES-1 );
		memset( spaces_decl, ' ', MAX_SPACES-1 );
		memset( spaces_time, ' ', MAX_SPACES-1 );
		memset( spaces_rev,  ' ', MAX_SPACES-1 );
		memset( spaces_mgt_request,  ' ', MAX_SPACES-1 );//praneeth
		memset( spaces_stip_sent,  ' ', MAX_SPACES-1 );//TF Anna
		memset( spaces_stip_recv,  ' ', MAX_SPACES-1 );//TF Anna
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		// Adjust the spacing to keep columns aligned.
		if ( Controller_Stats3[i].received_count > 9 )
			memset( spaces_rec, 0x00, MAX_SPACES );
		
		if ( Controller_Stats3[i].sent_count > 9 )
		{
			memset( spaces_sent, 0x00, MAX_SPACES  );
			memset( spaces_rate, 0x00, MAX_SPACES  );
		}
		
		if ( Controller_Stats3[i].approved_count > 9 )
			memset( spaces_app, 0x00, MAX_SPACES );
		
		if ( Controller_Stats3[i].declined_count > 9 )
			memset( spaces_decl, 0x00, MAX_SPACES );
		
		if ( Controller_Stats3[i].timeout_count > 9 )
			memset( spaces_time, 0x00, MAX_SPACES );
		
		if ( Controller_Stats3[i].reversed_count > 9 )
			memset( spaces_rev, 0x00, MAX_SPACES );
		//praneeth
		if ( Controller_Stats3[i].mgt_request_count > 9 )
			memset( spaces_mgt_request, 0x00, MAX_SPACES );
		//TF Anna
		if ( Controller_Stats3[i].stip_sent_count > 9 )
			memset( spaces_stip_sent, 0x00, MAX_SPACES );
		if ( Controller_Stats3[i].stip_recv_count > 9 )
			memset( spaces_stip_recv, 0x00, MAX_SPACES );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s",
			Controller_Stats3[i].service_id,
			
			Controller_Stats3[i].received_count,    spaces_rec,
			Controller_Stats3[i].sent_count,        spaces_sent,
			Controller_Stats3[i].rate,              spaces_rate,
			Controller_Stats3[i].approved_count,    spaces_app,
			Controller_Stats3[i].declined_count,    spaces_decl,
			Controller_Stats3[i].timeout_count,     spaces_time,
			Controller_Stats3[i].reversed_count,    spaces_rev,
			Controller_Stats3[i].mgt_request_count, spaces_mgt_request,
			Controller_Stats3[i].stip_sent_count, spaces_stip_sent,
			Controller_Stats3[i].stip_recv_count, spaces_stip_recv
			);	 //praneeth
		
		// m_TranActivityList.AddString( display_buff );  
		if ( Controller_Stats3[i].flag == 1)
		{
			m_TranActivityList3.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{
			m_TranActivityList3.AddString( display_buff );
		}
		
		if(szDeviceControllers3.Find(Controller_Stats3[i].service_id) > -1)
		{
			if ( Controller_Stats3[i].flag == 1)
			{
				m_CntlNetTranList3.SetRed(Controller_Stats3[i].service_id, true);
				m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList3.SetRed(Controller_Stats3[i].service_id, false);
				m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_NETWORK_NAME, "DEVICE CONTROLLER");
		}

		//ramya
		if(szMracon3.Find(Controller_Stats3[i].service_id) > -1)
		{
			if ( Controller_Stats3[i].flag == 1)
			{
				m_CntlNetTranList3.SetRed(Controller_Stats3[i].service_id, true);
				m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList3.SetRed(Controller_Stats3[i].service_id, false);
				m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList3.UpdateItemText(Controller_Stats3[i].service_id, LS_NETWORK_NAME, "MRACON");
		}

		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_RECEIVED,	Controller_Stats3[i].received_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_SENT,		Controller_Stats3[i].sent_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_RATE,		Controller_Stats3[i].rate);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_APPROVED,	Controller_Stats3[i].approved_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_DECLINED,	Controller_Stats3[i].declined_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_TIMEDOUT,	Controller_Stats3[i].timeout_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_REVERSED,	Controller_Stats3[i].reversed_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_MGT_COUNTS,  Controller_Stats3[i].mgt_request_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_STIP_SENT,	Controller_Stats3[i].stip_sent_count);
		m_CntlNetTranList3.UpdateItemText( Controller_Stats3[i].service_id, LS_STIP_RECV,	Controller_Stats3[i].stip_recv_count);		
	}
	m_CntlNetTranList3.Sort();
}

void CMonitorView::UpdateControllerStatsList4()
{
	CHAR  display_buff[512];
	
	CHAR  spaces_rate[MAX_SPACES] ="";
	CHAR  spaces_rec [MAX_SPACES] ="";
	CHAR  spaces_sent[MAX_SPACES] ="";
	CHAR  spaces_app [MAX_SPACES] ="";
	CHAR  spaces_decl[MAX_SPACES] ="";
	CHAR  spaces_time[MAX_SPACES] ="";
	CHAR  spaces_rev [MAX_SPACES] ="";
	CHAR	 spaces_mgt_request[MAX_SPACES] ="";//praneeth
	CHAR	 spaces_stip_sent[MAX_SPACES] ="";	//TF Anna
	CHAR	 spaces_stip_recv[MAX_SPACES] ="";	//TF Anna
	
	
	m_TranActivityList4.ResetContent();
	
	for( INT i = 0; i < Controller_Count4; i++ )
	{
		// Initialize the space before each value to align the columns. 
		memset( spaces_rate, ' ', 2 );
		memset( spaces_rec,  ' ', MAX_SPACES-1 );
		memset( spaces_sent, ' ', MAX_SPACES-1 );
		memset( spaces_app,  ' ', MAX_SPACES-1 );
		memset( spaces_decl, ' ', MAX_SPACES-1 );
		memset( spaces_time, ' ', MAX_SPACES-1 );
		memset( spaces_rev,  ' ', MAX_SPACES-1 );
		memset( spaces_mgt_request,  ' ', MAX_SPACES-1 );//praneeth
		memset( spaces_stip_sent,  ' ', MAX_SPACES-1 );//TF Anna
		memset( spaces_stip_recv,  ' ', MAX_SPACES-1 );//TF Anna
		memset( display_buff, 0x00, sizeof(display_buff) );
		
		// Adjust the spacing to keep columns aligned.
		if ( Controller_Stats4[i].received_count > 9 )
			memset( spaces_rec, 0x00, MAX_SPACES );
		
		if ( Controller_Stats4[i].sent_count > 9 )
		{
			memset( spaces_sent, 0x00, MAX_SPACES  );
			memset( spaces_rate, 0x00, MAX_SPACES  );
		}
		
		if ( Controller_Stats4[i].approved_count > 9 )
			memset( spaces_app, 0x00, MAX_SPACES );
		
		if ( Controller_Stats4[i].declined_count > 9 )
			memset( spaces_decl, 0x00, MAX_SPACES );
		
		if ( Controller_Stats4[i].timeout_count > 9 )
			memset( spaces_time, 0x00, MAX_SPACES );
		
		if ( Controller_Stats4[i].reversed_count > 9 )
			memset( spaces_rev, 0x00, MAX_SPACES );
		//praneeth
		if ( Controller_Stats4[i].mgt_request_count > 9 )
			memset( spaces_mgt_request, 0x00, MAX_SPACES );
		//TF Anna
		if ( Controller_Stats4[i].stip_sent_count > 9 )
			memset( spaces_stip_sent, 0x00, MAX_SPACES );
		if ( Controller_Stats4[i].stip_recv_count > 9 )
			memset( spaces_stip_recv, 0x00, MAX_SPACES );
		
		sprintf( display_buff,
			"%s\t\t%6ld%s\t\t%6ld%s\t\t%6.2f%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s\t\t%6ld%s",
			Controller_Stats4[i].service_id,
			
			Controller_Stats4[i].received_count,    spaces_rec,
			Controller_Stats4[i].sent_count,        spaces_sent,
			Controller_Stats4[i].rate,              spaces_rate,
			Controller_Stats4[i].approved_count,    spaces_app,
			Controller_Stats4[i].declined_count,    spaces_decl,
			Controller_Stats4[i].timeout_count,     spaces_time,
			Controller_Stats4[i].reversed_count,    spaces_rev,
			Controller_Stats4[i].mgt_request_count, spaces_mgt_request,
			Controller_Stats4[i].stip_sent_count, spaces_stip_sent,
			Controller_Stats4[i].stip_recv_count, spaces_stip_recv
			);	 //praneeth
		
		// m_TranActivityList.AddString( display_buff );  
		if ( Controller_Stats4[i].flag == 1)
		{
			m_TranActivityList4.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{
			m_TranActivityList4.AddString( display_buff );
		}
		
		if(szDeviceControllers4.Find(Controller_Stats4[i].service_id) > -1)
		{
			if ( Controller_Stats4[i].flag == 1)
			{
				m_CntlNetTranList4.SetRed(Controller_Stats4[i].service_id, true);
				m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList4.SetRed(Controller_Stats4[i].service_id, false);
				m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_NETWORK_NAME, "DEVICE CONTROLLER");
		}

		//ramya
		if(szMracon4.Find(Controller_Stats4[i].service_id) > -1)
		{
			if ( Controller_Stats4[i].flag == 1)
			{
				m_CntlNetTranList4.SetRed(Controller_Stats4[i].service_id, true);
				m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_STATUS, "Offline");
			}
			else
			{
				m_CntlNetTranList4.SetRed(Controller_Stats4[i].service_id, false);
				m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_STATUS, "Online");
			}
			m_CntlNetTranList4.UpdateItemText(Controller_Stats4[i].service_id, LS_NETWORK_NAME, "MRACON");
		}

		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_RECEIVED,	Controller_Stats4[i].received_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_SENT,		Controller_Stats4[i].sent_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_RATE,		Controller_Stats4[i].rate);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_APPROVED,	Controller_Stats4[i].approved_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_DECLINED,	Controller_Stats4[i].declined_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_TIMEDOUT,	Controller_Stats4[i].timeout_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_REVERSED,	Controller_Stats4[i].reversed_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_MGT_COUNTS,  Controller_Stats4[i].mgt_request_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_STIP_SENT,	Controller_Stats4[i].stip_sent_count);
		m_CntlNetTranList4.UpdateItemText( Controller_Stats4[i].service_id, LS_STIP_RECV,	Controller_Stats4[i].stip_recv_count);		
	}
	m_CntlNetTranList4.Sort();
}
/*************************************************************************************
   NAME:               UpdateDialogStatusList

   DESCRIPTION:        This function populates the Dialog Status Message list box 
					   with the latest dialog_status        
   INPUTS:
       None
   OUTPUTS:
       None
   RETURNS:           
       None
   AUTHOR:             Linda Mitchell 
*************************************************************************************/
void CMonitorView::UpdateDialogStatusList()
{
	CHAR  display_buff[512];
	CHAR  status_str[20];
	
	m_DialogConnectStatus.ResetContent();
	// m_DialogConnectStatus.UpdateWindow();
	nc_status = RED;
	for( INT i = 0; i < Dialog_ID_Count; i++ )
	{
		memset(display_buff, 0x00, sizeof(display_buff) );
		memset(status_str,   0x00, sizeof(status_str)   );
		switch(Dialog_Status[i].status)
		{
		case CONNECTED:
			strcpy( status_str, "Connected" );
			break;
			
		case DISCONNECTED:
			strcpy( status_str, "Disconnected" );
			break;
			
		case NO_DATA:
			strcpy( status_str, "No Data" );
			break;
			
		default:
			strcpy( status_str, "No Data" );
			break;
		}
		
		m_DialogConnectStatus.SetFont(&m_font);
		sprintf( display_buff, "%-14s%-14s%s",
			(pCHAR)Dialog_Status[i].dialog_id,
			status_str,Dialog_Status[i].dialog_name);
		
		if ( Dialog_Status[i].status == 1) //Disconnected status
			m_DialogConnectStatus.InsertString(0,display_buff,RGB(255,0,0));
		else 
			m_DialogConnectStatus.AddString( display_buff );
	}
}

void CMonitorView::UpdateDialogStatusList2()
{
	CHAR  display_buff[512];
	CHAR  status_str[20];
	
	m_DialogConnectStatus2.ResetContent();
	// m_DialogConnectStatus.UpdateWindow();
	nc_status2 = RED;
	for( INT i = 0; i < Dialog_ID_Count2; i++ )
	{
		memset(display_buff, 0x00, sizeof(display_buff) );
		memset(status_str,   0x00, sizeof(status_str)   );
		switch(Dialog_Status2[i].status)
		{
		case CONNECTED:
			strcpy( status_str, "Connected" );
			break;
			
		case DISCONNECTED:
			strcpy( status_str, "Disconnected" );
			break;
			
		case NO_DATA:
			strcpy( status_str, "No Data" );
			break;
			
		default:
			strcpy( status_str, "No Data" );
			break;
		}
		
		m_DialogConnectStatus2.SetFont(&m_font);

		sprintf( display_buff, "%-14s%-14s%s",
			(pCHAR)Dialog_Status2[i].dialog_id,
			status_str,Dialog_Status2[i].dialog_name);
		
		if ( Dialog_Status2[i].status == 1) //Disconnected status
		{
			m_DialogConnectStatus2.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{	
			m_DialogConnectStatus2.AddString( display_buff );
		}
	}
}

void CMonitorView::UpdateDialogStatusList3()
{
	CHAR  display_buff[512];
	CHAR  status_str[20];
	
	m_DialogConnectStatus3.ResetContent();
	// m_DialogConnectStatus3.UpdateWindow();
	nc_status3 = RED;
	for( INT i = 0; i < Dialog_ID_Count3; i++ )
	{
		memset(display_buff, 0x00, sizeof(display_buff) );
		memset(status_str,   0x00, sizeof(status_str)   );
		switch(Dialog_Status3[i].status)
		{
		case CONNECTED:
			strcpy( status_str, "Connected" );
			break;
			
		case DISCONNECTED:
			strcpy( status_str, "Disconnected" );
			break;
			
		case NO_DATA:
			strcpy( status_str, "No Data" );
			break;
			
		default:
			strcpy( status_str, "No Data" );
			break;
		}
		
		m_DialogConnectStatus3.SetFont(&m_font);

		sprintf( display_buff, "%-14s%-14s%s",
			(pCHAR)Dialog_Status3[i].dialog_id,
			status_str,Dialog_Status3[i].dialog_name);
		
		if ( Dialog_Status3[i].status == 1) //Disconnected status
		{
			m_DialogConnectStatus3.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{	
			m_DialogConnectStatus3.AddString( display_buff );
		}
	}
}

void CMonitorView::UpdateDialogStatusList4()
{
	CHAR  display_buff[512];
	CHAR  status_str[20];
	
	m_DialogConnectStatus4.ResetContent();
	// m_DialogConnectStatus4.UpdateWindow();
	nc_status4 = RED;
	for( INT i = 0; i < Dialog_ID_Count4; i++ )
	{
		memset(display_buff, 0x00, sizeof(display_buff) );
		memset(status_str,   0x00, sizeof(status_str)   );
		switch(Dialog_Status4[i].status)
		{
		case CONNECTED:
			strcpy( status_str, "Connected" );
			break;
			
		case DISCONNECTED:
			strcpy( status_str, "Disconnected" );
			break;
			
		case NO_DATA:
			strcpy( status_str, "No Data" );
			break;
			
		default:
			strcpy( status_str, "No Data" );
			break;
		}
		
		m_DialogConnectStatus4.SetFont(&m_font);

		sprintf( display_buff, "%-14s%-14s%s",
			(pCHAR)Dialog_Status4[i].dialog_id,
			status_str,Dialog_Status4[i].dialog_name);
		
		if ( Dialog_Status4[i].status == 1) //Disconnected status
		{
			m_DialogConnectStatus4.InsertString(0,display_buff,RGB(255,0,0));
		}
		else 
		{	
			m_DialogConnectStatus4.AddString( display_buff );
		}
	}
}
/******************************************************************************
*
*  NAME:         PROCESS_STATS
*
*  DESCRIPTION:  This function expects one of two message types.  One is
*                for dialog services, the other is for device and network
*                controllers.  For dialog services, the status of the dialog
*                who sent this message is updated, depending upon the value
*                of subtype1. For the controllers, a structure of MONITOR_STATS
*                is extracted.  In there lies transaction counts that will get
*                stored into Monitor's global memory. A rate is also calculated
*                based on previous counts and the timer interval.
*
*  INPUTS:       p_msg_in - PTE message containing our information
*                msgtype  - Type of PTE message
*
*  OUTPUTS:      (Global)Dialog_Status    - List of dialogs and their status
*                (Global)Controller_Stats - List of controllers and statistics
*
*  RTRN VALUE:   None
*
*  AUTHOR:       Dennis Irby
*
*  MODIFICATIONS:
******************************************************************************/
void CMonitorView::process_stats( pPTE_MSG p_msg_in, BYTE msgtype )
{
	INT    i = 0;
	INT    len;
	INT    MRA_sentcount_Flag = 0;
	DWORD  count_delta;
	BYTE   subtype1;
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data_data = NULL_PTR;
	LONG            data_len;
	MONITOR_STATS   count_struct;
	
	
	/* Find the originator of this message - strip off the queue letter. */
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );

	if(strncmp(originator,"fcondsA",6) == 0)
	{
		memset(originator,0x00,sizeof(originator));
		strncpy(originator,"mraconA",7);
		MRA_sentcount_Flag = 1;
	}
	
		len = strlen( originator ) - 1;
	originator[len] = '\0';

	if ( MT_COM_REPLY == msgtype )
	{
		/* Loop through the list of Dialogs to find this one. */
		subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
		for( i=0; i<Dialog_ID_Count; i++ )
		{
			if ( 0 == strcmp(originator, Dialog_Status[i].dialog_id) )
			{
				/* Found the dialog service.  Update its status. */
				Dialog_Status[i].flag = 0;
				if ( (subtype1 == ST1_CONNECTED_TRACE) ||
					(subtype1 == ST1_CONNECTED_NOTRACE) )
					Dialog_Status[i].status = CONNECTED;
				
				else if ( subtype1 == ST1_DISCONNECT_CONFIRM )
					Dialog_Status[i].status = DISCONNECTED;
				
				else
					Dialog_Status[i].status = NO_DATA;
				break;
			}
		}
	}
	else if ( MT_GET_STATS_RESPONSE == msgtype )
	{
		/* Update a Controller's Statistics */
		
		/* Copy structure from PTE message into local structure */
		memset( &count_struct, 0x00, sizeof(MONITOR_STATS) );
		p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
		if ( sizeof(MONITOR_STATS) < data_len )
			len = sizeof(MONITOR_STATS);
		else
			len = data_len;
		
		p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
		memcpy( &count_struct, p_data_data, len );
		
		/* Loop through the list of Controllers to find this one. */
		
		for( i=0; i<Controller_Count; i++ )
		{
			if ( 0 == strcmp(originator, Controller_Stats[i].service_id) )
			{
				/* Found the controller service.  Update its statistics. */
				Controller_Stats[i].previous_sent_count = 
					Controller_Stats[i].sent_count;
				Controller_Stats[i].sent_count     = atoi(count_struct.cnt_to_network);
				
				Controller_Stats[i].received_count = atoi(count_struct.cnt_from_network);
				if ( 1 == MRA_sentcount_Flag)
				{
				// No need to set the online status flag for MRACON, Its only for getting sent count.	
				}
				else
				{
				Controller_Stats[i].flag           = 0;
				}
				Controller_Stats[i].approved_count = atoi(count_struct.num_approved);
				Controller_Stats[i].declined_count = atoi(count_struct.num_declined);
				Controller_Stats[i].reversed_count = atoi(count_struct.num_reversed);
				Controller_Stats[i].timeout_count  = atoi(count_struct.num_time_out);
				Controller_Stats[i].mgt_request_count  = atoi(count_struct.num_mgt_request);//praneeth
				Controller_Stats[i].stip_sent_count = atoi(count_struct.num_stip_sent);
				Controller_Stats[i].stip_recv_count = atoi(count_struct.num_stip_recv); //TF Anna
				/* Get delta count.  Could be negative from a reset command. */
				if ( Controller_Stats[i].sent_count  >  
					Controller_Stats[i].previous_sent_count )
				{
					count_delta = Controller_Stats[i].sent_count -
						Controller_Stats[i].previous_sent_count;
				}
				else
					count_delta = 0UL;
				
				/* Calculate rate = (sent - previous count)/time */
				if ( Timer_Interval > 0 )
				{
					/* Be sure to convert time to seconds from milliseconds. */
					Controller_Stats[i].rate = (double)count_delta /
						(double)(Timer_Interval / 1000.0);
				}
				else
					Controller_Stats[i].rate = 0.0;
				break;
			}
		}
	}
	
	return;
}

void CMonitorView::process_stats2( pPTE_MSG p_msg_in, BYTE msgtype )
{
	INT    i = 0;
	INT    len;
	INT    MRA_sentcount_Flag = 0;
	DWORD  count_delta;
	BYTE   subtype1;
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data_data = NULL_PTR;
	LONG            data_len;
	MONITOR_STATS   count_struct;
	
	
	/* Find the originator of this message - strip off the queue letter. */
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );

	if(strncmp(originator,"fcondsA",6) == 0)
	{
		memset(originator,0x00,sizeof(originator));
		strncpy(originator,"mraconA",7);
		MRA_sentcount_Flag = 1;
	}
	
		len = strlen( originator ) - 1;
	originator[len] = '\0';

	if ( MT_COM_REPLY == msgtype )
	{
		/* Loop through the list of Dialogs to find this one. */
		subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
		for( i=0; i<Dialog_ID_Count2; i++ )
		{
			if ( 0 == strcmp(originator, Dialog_Status2[i].dialog_id) )
			{
				/* Found the dialog service.  Update its status. */
				Dialog_Status2[i].flag = 0;
				if ( (subtype1 == ST1_CONNECTED_TRACE) ||
					(subtype1 == ST1_CONNECTED_NOTRACE) )
					Dialog_Status2[i].status = CONNECTED;
				
				else if ( subtype1 == ST1_DISCONNECT_CONFIRM )
					Dialog_Status2[i].status = DISCONNECTED;
				
				else
					Dialog_Status2[i].status = NO_DATA;
				break;
			}
		}
	}
	else if ( MT_GET_STATS_RESPONSE == msgtype )
	{
		/* Update a Controller's Statistics */
		
		/* Copy structure from PTE message into local structure */
		memset( &count_struct, 0x00, sizeof(MONITOR_STATS) );
		p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
		if ( sizeof(MONITOR_STATS) < data_len )
			len = sizeof(MONITOR_STATS);
		else
			len = data_len;
		
		p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
		memcpy( &count_struct, p_data_data, len );
		
		/* Loop through the list of Controllers to find this one. */
		
		for( i=0; i<Controller_Count2; i++ )
		{
			if ( 0 == strcmp(originator, Controller_Stats2[i].service_id) )
			{
				/* Found the controller service.  Update its statistics. */
				Controller_Stats2[i].previous_sent_count = 
					Controller_Stats2[i].sent_count;
				Controller_Stats2[i].sent_count     = atoi(count_struct.cnt_to_network);
				
				Controller_Stats2[i].received_count = atoi(count_struct.cnt_from_network);
				if ( 1 == MRA_sentcount_Flag)
				{
				// No need to set the online status flag for MRACON, Its only for getting sent count.	
				}
				else
				{
				Controller_Stats2[i].flag           = 0;
				}
				Controller_Stats2[i].approved_count = atoi(count_struct.num_approved);
				Controller_Stats2[i].declined_count = atoi(count_struct.num_declined);
				Controller_Stats2[i].reversed_count = atoi(count_struct.num_reversed);
				Controller_Stats2[i].timeout_count  = atoi(count_struct.num_time_out);
				Controller_Stats2[i].mgt_request_count  = atoi(count_struct.num_mgt_request);//praneeth
				Controller_Stats2[i].stip_sent_count = atoi(count_struct.num_stip_sent);
				Controller_Stats2[i].stip_recv_count = atoi(count_struct.num_stip_recv); //TF Anna
				/* Get delta count.  Could be negative from a reset command. */
				if ( Controller_Stats2[i].sent_count  >  
					Controller_Stats2[i].previous_sent_count )
				{
					count_delta = Controller_Stats2[i].sent_count -
						Controller_Stats2[i].previous_sent_count;
				}
				else
					count_delta = 0UL;
				
				/* Calculate rate = (sent - previous count)/time */
				if ( Timer_Interval2 > 0 )
				{
					/* Be sure to convert time to seconds from milliseconds. */
					Controller_Stats2[i].rate = (double)count_delta /
						(double)(Timer_Interval2 / 1000.0);
				}
				else
					Controller_Stats2[i].rate = 0.0;
				break;
			}
		}
	}
	
	return;
}

void CMonitorView::process_stats3( pPTE_MSG p_msg_in, BYTE msgtype )
{
	INT    i = 0;
	INT    len;
	INT    MRA_sentcount_Flag = 0;
	DWORD  count_delta;
	BYTE   subtype1;
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data_data = NULL_PTR;
	LONG            data_len;
	MONITOR_STATS   count_struct;
	
	
	/* Find the originator of this message - strip off the queue letter. */
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );

	if(strncmp(originator,"fcondsA",6) == 0)
	{
		memset(originator,0x00,sizeof(originator));
		strncpy(originator,"mraconA",7);
		MRA_sentcount_Flag = 1;
	}
	
	len = strlen( originator ) - 1;
	originator[len] = '\0';

	if ( MT_COM_REPLY == msgtype )
	{
		/* Loop through the list of Dialogs to find this one. */
		subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
		for( i=0; i<Dialog_ID_Count3; i++ )
		{
			if ( 0 == strcmp(originator, Dialog_Status3[i].dialog_id) )
			{
				/* Found the dialog service.  Update its status. */
				Dialog_Status3[i].flag = 0;
				if ( (subtype1 == ST1_CONNECTED_TRACE) ||
					(subtype1 == ST1_CONNECTED_NOTRACE) )
					Dialog_Status3[i].status = CONNECTED;
				
				else if ( subtype1 == ST1_DISCONNECT_CONFIRM )
					Dialog_Status3[i].status = DISCONNECTED;
				
				else
					Dialog_Status3[i].status = NO_DATA;
				break;
			}
		}
	}
	else if ( MT_GET_STATS_RESPONSE == msgtype )
	{
		/* Update a Controller's Statistics */
		
		/* Copy structure from PTE message into local structure */
		memset( &count_struct, 0x00, sizeof(MONITOR_STATS) );
		p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
		if ( sizeof(MONITOR_STATS) < data_len )
			len = sizeof(MONITOR_STATS);
		else
			len = data_len;
		
		p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
		memcpy( &count_struct, p_data_data, len );
		
		/* Loop through the list of Controllers to find this one. */
		
		for( i=0; i<Controller_Count3; i++ )
		{
			if ( 0 == strcmp(originator, Controller_Stats3[i].service_id) )
			{
				/* Found the controller service.  Update its statistics. */
				Controller_Stats3[i].previous_sent_count = 
					Controller_Stats3[i].sent_count;
				Controller_Stats3[i].sent_count     = atoi(count_struct.cnt_to_network);
				
				Controller_Stats3[i].received_count = atoi(count_struct.cnt_from_network);
				if ( 1 == MRA_sentcount_Flag)
				{
				// No need to set the online status flag for MRACON, Its only for getting sent count.	
				}
				else
				{
				Controller_Stats3[i].flag           = 0;
				}
				Controller_Stats3[i].approved_count = atoi(count_struct.num_approved);
				Controller_Stats3[i].declined_count = atoi(count_struct.num_declined);
				Controller_Stats3[i].reversed_count = atoi(count_struct.num_reversed);
				Controller_Stats3[i].timeout_count  = atoi(count_struct.num_time_out);
				Controller_Stats3[i].mgt_request_count  = atoi(count_struct.num_mgt_request);//praneeth
				Controller_Stats3[i].stip_sent_count = atoi(count_struct.num_stip_sent);
				Controller_Stats3[i].stip_recv_count = atoi(count_struct.num_stip_recv); //TF Anna
				/* Get delta count.  Could be negative from a reset command. */
				if ( Controller_Stats3[i].sent_count  >  
					Controller_Stats3[i].previous_sent_count )
				{
					count_delta = Controller_Stats3[i].sent_count -
						Controller_Stats3[i].previous_sent_count;
				}
				else
					count_delta = 0UL;
				
				/* Calculate rate = (sent - previous count)/time */
				if ( Timer_Interval3 > 0 )
				{
					/* Be sure to convert time to seconds from milliseconds. */
					Controller_Stats3[i].rate = (double)count_delta /
						(double)(Timer_Interval3 / 1000.0);
				}
				else
					Controller_Stats3[i].rate = 0.0;
				break;
			}
		}
	}
	
	return;
}

void CMonitorView::process_stats4( pPTE_MSG p_msg_in, BYTE msgtype )
{
	INT    i = 0;
	INT    len;
	INT    MRA_sentcount_Flag = 0;
	DWORD  count_delta;
	BYTE   subtype1;
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	
	pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
	pBYTE           p_data_data = NULL_PTR;
	LONG            data_len;
	MONITOR_STATS   count_struct;
	
	
	/* Find the originator of this message - strip off the queue letter. */
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );

	if(strncmp(originator,"fcondsA",6) == 0)
	{
		memset(originator,0x00,sizeof(originator));
		strncpy(originator,"mraconA",7);
		MRA_sentcount_Flag = 1;
	}
	
	len = strlen( originator ) - 1;
	originator[len] = '\0';

	if ( MT_COM_REPLY == msgtype )
	{
		/* Loop through the list of Dialogs to find this one. */
		subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
		for( i=0; i<Dialog_ID_Count4; i++ )
		{
			if ( 0 == strcmp(originator, Dialog_Status4[i].dialog_id) )
			{
				/* Found the dialog service.  Update its status. */
				Dialog_Status4[i].flag = 0;
				if ( (subtype1 == ST1_CONNECTED_TRACE) ||
					(subtype1 == ST1_CONNECTED_NOTRACE) )
					Dialog_Status4[i].status = CONNECTED;
				
				else if ( subtype1 == ST1_DISCONNECT_CONFIRM )
					Dialog_Status4[i].status = DISCONNECTED;
				
				else
					Dialog_Status4[i].status = NO_DATA;
				break;
			}
		}
	}
	else if ( MT_GET_STATS_RESPONSE == msgtype )
	{
		/* Update a Controller's Statistics */
		
		/* Copy structure from PTE message into local structure */
		memset( &count_struct, 0x00, sizeof(MONITOR_STATS) );
		p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
		if ( sizeof(MONITOR_STATS) < data_len )
			len = sizeof(MONITOR_STATS);
		else
			len = data_len;
		
		p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
		memcpy( &count_struct, p_data_data, len );
		
		/* Loop through the list of Controllers to find this one. */
		
		for( i=0; i<Controller_Count4; i++ )
		{
			if ( 0 == strcmp(originator, Controller_Stats4[i].service_id) )
			{
				/* Found the controller service.  Update its statistics. */
				Controller_Stats4[i].previous_sent_count = 
					Controller_Stats4[i].sent_count;
				Controller_Stats4[i].sent_count     = atoi(count_struct.cnt_to_network);
				
				Controller_Stats4[i].received_count = atoi(count_struct.cnt_from_network);
				if ( 1 == MRA_sentcount_Flag)
				{
				// No need to set the online status flag for MRACON, Its only for getting sent count.	
				}
				else
				{
				Controller_Stats4[i].flag           = 0;
				}
				Controller_Stats4[i].approved_count = atoi(count_struct.num_approved);
				Controller_Stats4[i].declined_count = atoi(count_struct.num_declined);
				Controller_Stats4[i].reversed_count = atoi(count_struct.num_reversed);
				Controller_Stats4[i].timeout_count  = atoi(count_struct.num_time_out);
				Controller_Stats4[i].mgt_request_count  = atoi(count_struct.num_mgt_request);//praneeth
				Controller_Stats4[i].stip_sent_count = atoi(count_struct.num_stip_sent);
				Controller_Stats4[i].stip_recv_count = atoi(count_struct.num_stip_recv); //TF Anna
				/* Get delta count.  Could be negative from a reset command. */
				if ( Controller_Stats4[i].sent_count  >  
					Controller_Stats4[i].previous_sent_count )
				{
					count_delta = Controller_Stats4[i].sent_count -
						Controller_Stats4[i].previous_sent_count;
				}
				else
					count_delta = 0UL;
				
				/* Calculate rate = (sent - previous count)/time */
				if ( Timer_Interval4 > 0 )
				{
					/* Be sure to convert time to seconds from milliseconds. */
					Controller_Stats4[i].rate = (double)count_delta /
						(double)(Timer_Interval4 / 1000.0);
				}
				else
					Controller_Stats4[i].rate = 0.0;
				break;
			}
		}
	}
	
	return;
}

void CMonitorView::OnPingButton() 
{
	// TODO: Add your control notification handler code here
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList.GetSelCount())
	{
		
		
		i = m_TranActivityList.GetCurSel();
		m_TranActivityList.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR1 GH1: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
		
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR1 GH1: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}
			
			
		}
		
	}
}

void CMonitorView::process_ping(pPTE_MSG p_msg_in, BYTE msgtype)
{
	
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	INT    len , i;
	
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );
	len = strlen( originator ) - 1;
	originator[len] = '\0';
	
	/* Loop through the list of Dialogs to find this one. */
	
	for( i=0; i<Dialog_ID_Count; i++ )
	{
		if ( 0 == strcmp(originator, Dialog_Status[i].dialog_id) )
		{
			/* Found the dialog service.  Update its status. */
			Dialog_Status[i].flag = 0;
		}
	}
	
	for( i=0; i<Controller_Count; i++ )
	{
		if ( 0 == strcmp(originator, Controller_Stats[i].service_id) )
		{
			Controller_Stats[i].flag = 0;  
		}
	}
}

void CMonitorView::process_ping2(pPTE_MSG p_msg_in, BYTE msgtype)
{
	
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	INT    len , i;
	
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );
	len = strlen( originator ) - 1;
	originator[len] = '\0';
	
	/* Loop through the list of Dialogs to find this one. */
	
	for( i=0; i<Dialog_ID_Count2; i++ )
	{
		if ( 0 == strcmp(originator, Dialog_Status2[i].dialog_id) )
		{
			/* Found the dialog service.  Update its status. */
			Dialog_Status2[i].flag = 0;
		}
	}
	
	for( i=0; i<Controller_Count2; i++ )
	{
		if ( 0 == strcmp(originator, Controller_Stats2[i].service_id) )
		{
			Controller_Stats2[i].flag = 0;  
		}
	}
}

void CMonitorView::process_ping3(pPTE_MSG p_msg_in, BYTE msgtype)
{
	
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	INT    len , i;
	
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );
	len = strlen( originator ) - 1;
	originator[len] = '\0';
	
	/* Loop through the list of Dialogs to find this one. */
	
	for( i=0; i<Dialog_ID_Count3; i++ )
	{
		if ( 0 == strcmp(originator, Dialog_Status3[i].dialog_id) )
		{
			/* Found the dialog service.  Update its status. */
			Dialog_Status3[i].flag = 0;
		}
	}
	
	for( i=0; i<Controller_Count3; i++ )
	{
		if ( 0 == strcmp(originator, Controller_Stats3[i].service_id) )
		{
			Controller_Stats3[i].flag = 0;  
		}
	}
}

void CMonitorView::process_ping4(pPTE_MSG p_msg_in, BYTE msgtype)
{
	
	CHAR   originator[MAX_QUE_NAME_SIZE+1];
	INT    len , i;
	
	memset( originator, 0x00, sizeof(originator) );
	strcpy( originator, ptemsg_get_msg_orig_queue ( p_msg_in ) );
	len = strlen( originator ) - 1;
	originator[len] = '\0';
	
	/* Loop through the list of Dialogs to find this one. */
	
	for( i=0; i<Dialog_ID_Count4; i++ )
	{
		if ( 0 == strcmp(originator, Dialog_Status4[i].dialog_id) )
		{
			/* Found the dialog service.  Update its status. */
			Dialog_Status4[i].flag = 0;
		}
	}
	
	for( i=0; i<Controller_Count4; i++ )
	{
		if ( 0 == strcmp(originator, Controller_Stats4[i].service_id) )
		{
			Controller_Stats4[i].flag = 0;  
		}
	}
}

void CMonitorView::OnPingDialogButton() 
{
	// TODO: Add your control notification handler code here
	CHAR     text[120];
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	
	if(	m_TranActivityList.GetSelCount())
	{
		
		i = m_TranActivityList.GetCurSel();
		m_TranActivityList.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR1 GH1: Failed to transmit  Dialog status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Dialog_ID_Count; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Dialog_Status[i].dialog_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR1 GH1: Failed to transmit  Dialog status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}
			
			
		}
	}
}	


void CMonitorView::OnTestButton() 
{
	INT  retval;
	CHAR errbuf[100] = "";
	
	m_TestResult.SetBitmap( m_hTesting );
	
	retval = send_pte_msg( MT_SYS_MONITOR, ST1_MONITOR_UP, txcntl_que_name, errbuf );
	
	if ( retval == false )
	{
		AfxMessageBox( errbuf, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::OnTestButton2() 
{
	INT  retval;
	CHAR errbuf[100] = "";
	
	m_TestResult2.SetBitmap( m_hTesting2 );
	
	retval = send_pte_msg2( MT_SYS_MONITOR, ST1_MONITOR_UP, txcntl_que_name, errbuf );
	
	if ( retval == false )
	{
		AfxMessageBox( errbuf, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::OnTestButton3() 
{
	INT  retval;
	CHAR errbuf[100] = "";
	
	m_TestResult3.SetBitmap( m_hTesting3 );
	
	retval = send_pte_msg3( MT_SYS_MONITOR, ST1_MONITOR_UP, txcntl_que_name, errbuf );
	
	if ( retval == false )
	{
		AfxMessageBox( errbuf, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::OnTestButton4() 
{
	INT  retval;
	CHAR errbuf[100] = "";
	
	m_TestResult4.SetBitmap( m_hTesting3 );
	
	retval = send_pte_msg4( MT_SYS_MONITOR, ST1_MONITOR_UP, txcntl_que_name, errbuf );
	
	if ( retval == false )
	{
		AfxMessageBox( errbuf, MB_OK | MB_ICONSTOP );
	}
	return;
}

void CMonitorView::OnCheck1() 
{
	//Mute
}

void CMonitorView::DisableAllButtons(int monitor)
{
	if(monitor == 1)
	{
		m_CntlRefreshButton.EnableWindow(FALSE);
		m_CntlPingDialogButton.EnableWindow(FALSE);
		m_CntlGetAdvisesButton.EnableWindow(FALSE);
		m_CntlEchoTestButton.EnableWindow(FALSE);
		m_CntlLogOff.EnableWindow(FALSE);
		m_CntlLogOn.EnableWindow(FALSE);
		m_PingButton.EnableWindow(FALSE);
		m_CntlResetCountsButton.EnableWindow(FALSE);
		m_CntlClearLogButton.EnableWindow(FALSE);
		m_CntlSaveLogButton.EnableWindow(FALSE);
		m_TEST_BUTTON.EnableWindow(FALSE);
		m_Ping.EnableWindow(FALSE);
		m_CntlMuteBeep.SetCheck(1);
		m_CntlMuteBeep.EnableWindow(FALSE);
		m_CntlNCStatus.SetBitmap( m_hNCYellow );
		/*if(m_bMonitorUp == true)//disable windows if ATP goes down when monitor is open.
		{
			m_CntlNetTranList.EnableWindow(FALSE);
			m_TranActivityList.EnableWindow(FALSE);
			m_DialogConnectStatus.EnableWindow(FALSE);
			m_LogMessages.EnableWindow(FALSE);
		}*/
	}
	else if(monitor == 2)
	{
		m_CntlRefreshButton2.EnableWindow(FALSE);
		m_CntlPingDialogButton2.EnableWindow(FALSE);
		m_CntlGetAdvisesButton2.EnableWindow(FALSE);
		m_CntlEchoTestButton2.EnableWindow(FALSE);
		m_CntlLogOff2.EnableWindow(FALSE);
		m_CntlLogOn2.EnableWindow(FALSE);		
		m_PingButton2.EnableWindow(FALSE);
		m_CntlResetCountsButton2.EnableWindow(FALSE);
		m_CntlClearLogButton2.EnableWindow(FALSE);
		m_CntlSaveLogButton2.EnableWindow(FALSE);
		m_TEST_BUTTON2.EnableWindow(FALSE);
		m_Ping2.EnableWindow(FALSE);
		m_CntlMuteBeep2.SetCheck(1);
		m_CntlMuteBeep2.EnableWindow(FALSE);
		m_CntlNCStatus2.SetBitmap( m_hNCYellow );
		/*if(m_bMonitorUp2 == true)
		{
			m_CntlNetTranList2.EnableWindow(FALSE);
			m_TranActivityList2.EnableWindow(FALSE);
			m_DialogConnectStatus2.EnableWindow(FALSE);
			m_LogMessages2.EnableWindow(FALSE);
		}*/
	}
	else if(monitor == 3)
	{
		m_CntlRefreshButton3.EnableWindow(FALSE);
		m_CntlPingDialogButton3.EnableWindow(FALSE);
		m_CntlGetAdvisesButton3.EnableWindow(FALSE);
		m_CntlEchoTestButton3.EnableWindow(FALSE);
		m_CntlLogOff3.EnableWindow(FALSE);
		m_CntlLogOn3.EnableWindow(FALSE);
		m_PingButton3.EnableWindow(FALSE);
		m_CntlResetCountsButton3.EnableWindow(FALSE);
		m_CntlClearLogButton3.EnableWindow(FALSE);
		m_CntlSaveLogButton3.EnableWindow(FALSE);
		m_TEST_BUTTON3.EnableWindow(FALSE);
		m_Ping3.EnableWindow(FALSE);
		m_CntlMuteBeep3.SetCheck(1);
		m_CntlMuteBeep3.EnableWindow(FALSE);
		m_CntlNCStatus3.SetBitmap( m_hNCYellow );
		/*if(m_bMonitorUp3 == true)
		{
			m_CntlNetTranList3.EnableWindow(FALSE);
			m_TranActivityList3.EnableWindow(FALSE);
			m_DialogConnectStatus3.EnableWindow(FALSE);
			m_LogMessages3.EnableWindow(FALSE);
		}*/
	}
	else if(monitor == 4)
	{
		m_CntlRefreshButton4.EnableWindow(FALSE);
		m_CntlPingDialogButton4.EnableWindow(FALSE);
		m_CntlGetAdvisesButton4.EnableWindow(FALSE);
		m_CntlEchoTestButton4.EnableWindow(FALSE);
		m_CntlLogOff4.EnableWindow(FALSE);
		m_CntlLogOn4.EnableWindow(FALSE);
		m_PingButton4.EnableWindow(FALSE);
		m_CntlResetCountsButton4.EnableWindow(FALSE);
		m_CntlClearLogButton4.EnableWindow(FALSE);
		m_CntlSaveLogButton4.EnableWindow(FALSE);
		m_TEST_BUTTON4.EnableWindow(FALSE);
		m_Ping4.EnableWindow(FALSE);
		m_CntlMuteBeep4.SetCheck(1);
		m_CntlMuteBeep4.EnableWindow(FALSE);
		m_CntlNCStatus4.SetBitmap( m_hNCYellow );
		/*if(m_bMonitorUp4 == true)
		{
			m_CntlNetTranList4.EnableWindow(FALSE);
			m_TranActivityList4.EnableWindow(FALSE);
			m_DialogConnectStatus4.EnableWindow(FALSE);
			m_LogMessages4.EnableWindow(FALSE);
		}*/
	}
}


void CMonitorView::EnableDialogButtons(int nStatus, int changeToReadOnlyTitleBar)
{
	//m_CntlRefreshButton.EnableWindow(nStatus);
	m_CntlPingDialogButton.EnableWindow(nStatus);
	m_CntlGetAdvisesButton.EnableWindow(nStatus);
	m_CntlEchoTestButton.EnableWindow(nStatus);
	m_CntlLogOff.EnableWindow(nStatus);
	m_CntlLogOn.EnableWindow(nStatus);
	//m_CntlMuteBeep.EnableWindow(nStatus);
	
	m_PingButton.EnableWindow(nStatus);
	m_CntlResetCountsButton.EnableWindow(nStatus);
	//m_CntlClearLogButton.EnableWindow(nStatus);
	//m_CntlSaveLogButton.EnableWindow(nStatus);
	//m_TEST_BUTTON.EnableWindow(nStatus);
	
	if ( changeToReadOnlyTitleBar )
	{
		m_bReadOnly = true;
		readonlyCount++;
		toggle_read_only_title(MONITOR1);
	}
}

void CMonitorView::EnableDialogButtons2(int nStatus, int changeToReadOnlyTitleBar)
{
	//m_CntlRefreshButton.EnableWindow(nStatus);
	m_CntlPingDialogButton2.EnableWindow(nStatus);
	m_CntlGetAdvisesButton2.EnableWindow(nStatus);
	m_CntlEchoTestButton2.EnableWindow(nStatus);
	m_CntlLogOff2.EnableWindow(nStatus);
	m_CntlLogOn2.EnableWindow(nStatus);
	//m_CntlMuteBeep2.EnableWindow(nStatus);
	
	m_PingButton2.EnableWindow(nStatus);
	m_CntlResetCountsButton2.EnableWindow(nStatus);
	//m_CntlClearLogButton2.EnableWindow(nStatus);
	//m_CntlSaveLogButton2.EnableWindow(nStatus);
	//m_TEST_BUTTON2.EnableWindow(nStatus);
	
	if ( changeToReadOnlyTitleBar )
	{
		m_bReadOnly2 = true;
		readonlyCount++;
		toggle_read_only_title(MONITOR2);
	}
}

void CMonitorView::EnableDialogButtons3(int nStatus, int changeToReadOnlyTitleBar)
{
	//m_CntlRefreshButton.EnableWindow(nStatus);
	m_CntlPingDialogButton3.EnableWindow(nStatus);
	m_CntlGetAdvisesButton3.EnableWindow(nStatus);
	m_CntlEchoTestButton3.EnableWindow(nStatus);
	m_CntlLogOff3.EnableWindow(nStatus);
	m_CntlLogOn3.EnableWindow(nStatus);
	//m_CntlMuteBeep3.EnableWindow(nStatus);
	
	m_PingButton3.EnableWindow(nStatus);
	m_CntlResetCountsButton3.EnableWindow(nStatus);
	//m_CntlClearLogButton3.EnableWindow(nStatus);
	//m_CntlSaveLogButton3.EnableWindow(nStatus);
	//m_TEST_BUTTON3.EnableWindow(nStatus);
	
	if ( changeToReadOnlyTitleBar )
	{
		m_bReadOnly3 = true;
		readonlyCount++;
		toggle_read_only_title(MONITOR3);
	}
}

void CMonitorView::EnableDialogButtons4(int nStatus, int changeToReadOnlyTitleBar)
{
	//m_CntlRefreshButton4.EnableWindow(nStatus);
	m_CntlPingDialogButton4.EnableWindow(nStatus);
	m_CntlGetAdvisesButton4.EnableWindow(nStatus);
	m_CntlEchoTestButton4.EnableWindow(nStatus);
	m_CntlLogOff4.EnableWindow(nStatus);
	m_CntlLogOn4.EnableWindow(nStatus);
	//m_CntlMuteBeep4.EnableWindow(nStatus);
	
	m_PingButton4.EnableWindow(nStatus);
	m_CntlResetCountsButton4.EnableWindow(nStatus);
	//m_CntlClearLogButton4.EnableWindow(nStatus);
	//m_CntlSaveLogButton4.EnableWindow(nStatus);
	//m_TEST_BUTTON4.EnableWindow(nStatus);
	
	if ( changeToReadOnlyTitleBar )
	{
		m_bReadOnly4 = true;
		readonlyCount++;
		toggle_read_only_title(MONITOR4);
	}
}

void CMonitorView::toggle_read_only_title(int mon)
{
	CString str;
	char buff[10] = {0};

	if(mon == MONITOR1)
		sprintf(buff, "(GH1)");
	else if(mon == MONITOR2)
		sprintf(buff, "(GH2)");
	else if(mon == MONITOR3)
		sprintf(buff, "(MKT1)");
	else if(mon == MONITOR4)
		sprintf(buff, "(MKT2)");
	AfxGetApp()->m_pMainWnd->GetWindowText(str);
	if(strstr(str, "READ ONLY") == NULL)
	{	
		str += " - READ ONLY ";
	}
	str += buff;
	AfxGetApp()->m_pMainWnd->SetWindowText(str);
}

void CMonitorView::OnClose() 
{
	
	
}

LRESULT CMonitorView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CString str;
	switch(message)
	{
	case WM_DESTROY:
		str = "destroy";
		if(m_bMonitorUp)
		{
			OnCloseBtn();
		}
		break;
		
	default:
		return CFormView::DefWindowProc(message, wParam, lParam);
	}
	return 0;
}

void CMonitorView::OnAcquirerOff() 
{
	if(m_bAcqOn)
	{
		ProcessOFFMessage(ST1_ACQUIRER_ON);
		m_CntlAcqOff.SetWindowText("ACQ OFF");
		m_bAcqOn = false;
	}
	else
	{
		ProcessOFFMessage(ST1_ACQUIRER_OFF);
		m_CntlAcqOff.SetWindowText("ACQ ON");
		m_bAcqOn = true;
	}
	// TODO: Add your control notification handler code here
	
}


void CMonitorView::OnSelchangeNetworkStatus() 
{
/*	int pos = m_NetworkStatus.GetCurSel();
	CString str;
	m_NetworkStatus.GetText(pos, str);
	if(str.Find("ncmcrd") > -1)
	{
		m_CntlAcqOff.EnableWindow(TRUE);
	}
	else
	{
		m_CntlAcqOff.EnableWindow(FALSE);
	}
*/	
}
BOOL CMonitorView::ItemdataProc(DWORD dwData, LPARAM lParam)
{
	// TODO: Process your item data here
	
	// Please return TRUE to proceed the deletion, return FALSE to abort.
	return TRUE;
}

void CMonitorView::OnPing() 
{
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	int selcount = m_CntlNetTranList.GetSelectedItemCount();
	if(selcount > 0)
	{
		CString str;
		m_CntlNetTranList.GetSelectedItemText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
			return;
		}
		strcpy( queue, str);
		strcat( queue, "C" );
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR1 GH1: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR1 GH1: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR1 GH1: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}
			
			
		}
		
	}
	
	
}

void CMonitorView::OnItemchangedNetTranList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
		CString title;
		AfxGetApp()->m_pMainWnd->GetWindowText(title);
		if(!m_bReadOnly)
		{
			
			CString str;
			m_CntlNetTranList.GetSelectedItemText(str);
			
			if(szDeviceControllers.Find(str) > -1 )
			{
				EnableDialogButtons(false, false);
			}
			else
			{
				EnableDialogButtons(true, false);
			}
			if(str.Find("ncmcrd") > -1)
			{
				m_CntlAcqOff.EnableWindow(TRUE);
			}
			else
			{
				m_CntlAcqOff.EnableWindow(FALSE);
			}

			m_CntlCupKeyReload.EnableWindow(FALSE);
			if((str.CompareNoCase("nccup3") == 0)||
			   (str.CompareNoCase("ncdci3") == 0))
			{
				m_CntlCupKeyRequest.EnableWindow(TRUE);
				if((str.CompareNoCase("ncdci2") != 0))
				{
					m_CntlCupKeyReload.EnableWindow(TRUE);
				}
				
			}	
			else
			{
				m_CntlCupKeyRequest.EnableWindow(FALSE);
			}

			if(str.CompareNoCase("nccup2") == 0 )				
			{
				m_CntlCupKeySend.EnableWindow(TRUE);
			}
			else
			{
				m_CntlCupKeySend.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("nccup3") == 0))
			{
				
				// This is done so that below else if condition should work for only nccup module, not for nccup3 module.
			}
	 		else if((str.CompareNoCase("nccup") == 0))
			{
				m_CntlLogOn.EnableWindow(FALSE);
				m_CntlLogOff.EnableWindow(FALSE);
				m_CntlEchoTestButton.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("ncpul") == 0))
			{
				m_CntlCupKeyRequest.EnableWindow(TRUE);
				
			}	
		}
    }
	
	*pResult = 0;
}

int CMonitorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CMonitorView::SaveLogIntoFile(CString filename)
{
	CHAR log_filename[256] = {0};
	FILE *log_fp = NULL;
	CHAR temp_str[256] = {0}, buffer[256] = {0};
	INT num_messages = 0;
	CHAR ini_file[256] = {0};

	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );
	
	(void) GetPrivateProfileString(
        "MONITORLOG", 	                   // points to section name 
        "MONITORDEFAULTPATH",	   // points to key name 
        "",					   // points to default string 
        log_filename,             // points to destination buffer 
        sizeof(log_filename) - 1, // size of destination buffer 
        ini_file );    

	if(0 == strlen(log_filename))
	{
		GetPinnacleLogDirectory( log_filename );
		//strcat( log_filename, filename.GetBuffer( 100 ) );	
	}
	
	strcat( log_filename, filename.GetBuffer( 100 ) );
	// try to open the file
	log_fp = fopen( log_filename, "w+" );
	if(log_fp == NULL)
	{
		if(!CreateDirectory("..\\log\\",NULL))
		{
			AfxMessageBox("DCO MONITOR: Unable to create directory");
		}
		log_fp = fopen( log_filename, "w+" );
		if( log_fp == NULL )
		{
			sprintf( temp_str, "DCO MONITOR: Unable to save the log", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		}
	}
 	// now save the log to the file, writing one line at a time
	num_messages = m_LogMessages.GetCount();    //GetItemCount();
	fwrite( "MONITOR1 GH1\n------------\n", sizeof( CHAR ), 26, log_fp );
	for( INT item = 0; item < num_messages; item++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		memset (temp_str, 0x00, sizeof(temp_str));
		
		m_LogMessages.GetText( item, temp_str );
		memcpy(buffer, temp_str, sizeof(temp_str) );
		strcat( buffer, "\n" );
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
	}

	num_messages = m_LogMessages2.GetCount();    //GetItemCount();
	fwrite( "\nMONITOR2 GH2\n------------\n", sizeof( CHAR ), 27, log_fp );
	for( INT item = 0; item < num_messages; item++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		memset (temp_str, 0x00, sizeof(temp_str));
		
		m_LogMessages2.GetText( item, temp_str );
		memcpy(buffer, temp_str, sizeof(temp_str) );
		strcat( buffer, "\n" );
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
	}

	num_messages = m_LogMessages3.GetCount();    //GetItemCount();
	fwrite( "\nMONITOR3 MKT1\n-------------\n", sizeof( CHAR ), 29, log_fp );
	for( INT item = 0; item < num_messages; item++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		memset (temp_str, 0x00, sizeof(temp_str));
		
		m_LogMessages3.GetText( item, temp_str );
		memcpy(buffer, temp_str, sizeof(temp_str) );
		strcat( buffer, "\n" );
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
	}

	num_messages = m_LogMessages4.GetCount();    //GetItemCount();
	fwrite( "\nMONITOR4 MKT2\n-------------\n", sizeof( CHAR ), 29, log_fp );
	for( INT item = 0; item < num_messages; item++ )
	{
		memset( buffer, 0, sizeof( buffer ) );
		memset (temp_str, 0x00, sizeof(temp_str));
		
		m_LogMessages4.GetText( item, temp_str );
		memcpy(buffer, temp_str, sizeof(temp_str) );
		strcat( buffer, "\n" );
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
	}
	
	// close the log file and return
	fclose( log_fp );
}

void CMonitorView::OnCupKeyRequestButton() 
{
	ProcessCUPMessage(ST1_CUP_KEY_REQUEST);
	// TODO: Add your control notification handler code here
	
}

void CMonitorView::OnCupKeySendButton() 
{
	ProcessCUPMessage(ST1_CUP_KEY_SEND);
	// TODO: Add your control notification handler code here
	
}

void CMonitorView::OnCupKeyReloadButton() 
{
	ProcessCUPMessage(STI_CUP_KEY_RELOAD);
	// TODO: Add your control notification handler code here
}


void CMonitorView::ProcessCUPMessage(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR1 GH1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(0 == strncmp(str_network_id,"nccup",5))
	{

		if(st1 == ST1_CUP_KEY_REQUEST)
		{
			str.Format("MONITOR1 GH1: Are you sure to Request Key for %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == ST1_CUP_KEY_SEND)
		{
			str.Format("MONITOR1 GH1: Are you sure to Send Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == STI_CUP_KEY_RELOAD)
		{
			str.Format("MONITOR1 GH1: Are you sure to Reload Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
			else
			{
				st1 = ST1_CUP_ACTIVE_ACTIVE_TRIGGER;
				str.Format("MONITOR1 GH1: Reload Key request is sent to %s?", str_network_id);
				AfxMessageBox( str, MB_ICONSTOP | MB_OK );
			}
		}
		else
		{
			str.Format("MONITOR1 GH1: Are you sure to logoff %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncdci",5))
	{
		// Lets us same key button define for CUP and adjust below based on DCI
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key test for DCI
				st1 = ST1_DINERS_KEY_TEST_REQUEST;
				str.Format("MONITOR1 GH1: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	
			else  if(st1 == STI_CUP_KEY_RELOAD)
			{
				// Key RELOAD for DCI
				st1 = ST1_DINERS_KEY_RELOD_REQUEST;
				str.Format("MONITOR1 GH1: Are you sure to Reload Key to %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
				else
				{
					str.Format("MONITOR1 GH1: Reload Key request is sent to %s?", str_network_id);
					AfxMessageBox( str, MB_ICONSTOP | MB_OK );
				}
			}
		else
		{
			str.Format("MONITOR1 GH1: invalid option %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncpul",5))
	{
		// Lets us same key button define for CUP and adjust below based on PULSE
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key change request  for Pulse
				st1 = ST1_PULSE_KEY_REQUEST;
				str.Format("MONITOR1 GH1: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
			else
			{
				str.Format("MONITOR1 GH1: invalid option %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	}
	//m_NetworkStatus.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data, 0x00, sizeof(m_msg_data) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data, &m_auth_tx, sizeof( m_auth_tx ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data,
		sizeof( m_msg_data ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR1 GH1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR1 GH1: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}

void CMonitorView::ProcessCUPMessage2(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList2.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(0 == strncmp(str_network_id,"nccup",5))
	{

		if(st1 == ST1_CUP_KEY_REQUEST)
		{
			str.Format("MONITOR2 GH2: Are you sure to Request Key for %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == ST1_CUP_KEY_SEND)
		{
			str.Format("MONITOR2 GH2: Are you sure to Send Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == STI_CUP_KEY_RELOAD)
		{
			str.Format("MONITOR2 GH2: Are you sure to Reload Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
			else
			{
				st1 = ST1_CUP_ACTIVE_ACTIVE_TRIGGER;
				str.Format("MONITOR2 GH2: Reload Key request is sent to %s?", str_network_id);
				AfxMessageBox( str, MB_ICONSTOP | MB_OK );
			}
		}
		else
		{
			str.Format("MONITOR2 GH2: Are you sure to logoff %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncdci",5))
	{
		// Lets us same key button define for CUP and adjust below based on DCI
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key test for DCI
				st1 = ST1_DINERS_KEY_TEST_REQUEST;
				str.Format("MONITOR2 GH2: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	
			else  if(st1 == STI_CUP_KEY_RELOAD)
			{
				// Key RELOAD for DCI
				st1 = ST1_DINERS_KEY_RELOD_REQUEST;
				str.Format("MONITOR2 GH2: Are you sure to Reload Key to %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
				else
				{
					str.Format("MONITOR2 GH2: Reload Key request is sent to %s?", str_network_id);
					AfxMessageBox( str, MB_ICONSTOP | MB_OK );
				}
			}
		else
		{
			str.Format("MONITOR2 GH2: invalid option %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncpul",5))
	{
		// Lets us same key button define for CUP and adjust below based on PULSE
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key change request  for Pulse
				st1 = ST1_PULSE_KEY_REQUEST;
				str.Format("MONITOR2 GH2: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
			else
			{
				str.Format("MONITOR2 GH2: invalid option %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	}
	//m_NetworkStatus2.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table2[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table2[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table2 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data2, 0x00, sizeof(m_msg_data2) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx2.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data2, &m_auth_tx2, sizeof( m_auth_tx2 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table2[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data2,
		sizeof( m_msg_data2 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}

void CMonitorView::ProcessCUPMessage3(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		display_buff[50];
	CHAR		str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus3.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList3.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(0 == strncmp(str_network_id,"nccup",5))
	{

		if(st1 == ST1_CUP_KEY_REQUEST)
		{
			str.Format("MONITOR3 MKT1: Are you sure to Request Key for %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == ST1_CUP_KEY_SEND)
		{
			str.Format("MONITOR3 MKT1: Are you sure to Send Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == STI_CUP_KEY_RELOAD)
		{
			str.Format("MONITOR3 MKT1: Are you sure to Reload Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
			else
			{
				st1 = ST1_CUP_ACTIVE_ACTIVE_TRIGGER;
				str.Format("MONITOR3 MKT1: Reload Key request is sent to %s?", str_network_id);
				AfxMessageBox( str, MB_ICONSTOP | MB_OK );
			}
		}
		else
		{
			str.Format("MONITOR3 MKT1: Are you sure to logoff %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncdci",5))
	{
		// Lets us same key button define for CUP and adjust below based on DCI
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key test for DCI
				st1 = ST1_DINERS_KEY_TEST_REQUEST;
				str.Format("MONITOR3 MKT1: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	
			else  if(st1 == STI_CUP_KEY_RELOAD)
			{
				// Key RELOAD for DCI
				st1 = ST1_DINERS_KEY_RELOD_REQUEST;
				str.Format("MONITOR3 MKT1: Are you sure to Reload Key to %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
				else
				{
					str.Format("MONITOR3 MKT1: Reload Key request is sent to %s?", str_network_id);
					AfxMessageBox( str, MB_ICONSTOP | MB_OK );
				}
			}
		else
		{
			str.Format("MONITOR3 MKT1: invalid option %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncpul",5))
	{
		// Lets us same key button define for CUP and adjust below based on PULSE
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key change request  for Pulse
				st1 = ST1_PULSE_KEY_REQUEST;
				str.Format("MONITOR3 MKT1: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
			else
			{
				str.Format("MONITOR3 MKT1: invalid option %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	}
	//m_NetworkStatus3.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table3[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table3[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table3 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data3, 0x00, sizeof(m_msg_data3) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx3.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data3, &m_auth_tx3, sizeof( m_auth_tx3 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table3[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data3,
		sizeof( m_msg_data3 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );	
}

void CMonitorView::ProcessCUPMessage4(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		display_buff[50];
	CHAR		str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus3.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList4.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(0 == strncmp(str_network_id,"nccup",5))
	{

		if(st1 == ST1_CUP_KEY_REQUEST)
		{
			str.Format("MONITOR4 MKT2: Are you sure to Request Key for %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == ST1_CUP_KEY_SEND)
		{
			str.Format("MONITOR4 MKT2: Are you sure to Send Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == STI_CUP_KEY_RELOAD)
		{
			str.Format("MONITOR4 MKT2: Are you sure to Reload Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
			else
			{
				st1 = ST1_CUP_ACTIVE_ACTIVE_TRIGGER;
				str.Format("MONITOR4 MKT2: Reload Key request is sent to %s?", str_network_id);
				AfxMessageBox( str, MB_ICONSTOP | MB_OK );
			}
		}
		else
		{
			str.Format("MONITOR4 MKT2: Are you sure to logoff %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncdci",5))
	{
		// Lets us same key button define for CUP and adjust below based on DCI
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key test for DCI
				st1 = ST1_DINERS_KEY_TEST_REQUEST;
				str.Format("MONITOR4 MKT2: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	
			else  if(st1 == STI_CUP_KEY_RELOAD)
			{
				// Key RELOAD for DCI
				st1 = ST1_DINERS_KEY_RELOD_REQUEST;
				str.Format("MONITOR4 MKT2: Are you sure to Reload Key to %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
				else
				{
					str.Format("MONITOR4 MKT2: Reload Key request is sent to %s?", str_network_id);
					AfxMessageBox( str, MB_ICONSTOP | MB_OK );
				}
			}
		else
		{
			str.Format("MONITOR4 MKT2: invalid option %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
	}
	else if(0 == strncmp(str_network_id,"ncpul",5))
	{
		// Lets us same key button define for CUP and adjust below based on PULSE
			if(st1 == ST1_CUP_KEY_REQUEST)
			{
				// Key change request  for Pulse
				st1 = ST1_PULSE_KEY_REQUEST;
				str.Format("MONITOR4 MKT2: Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
			else
			{
				str.Format("MONITOR4 MKT2: invalid option %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	}
	//m_NetworkStatus4.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table4[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table4[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table4 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data4, 0x00, sizeof(m_msg_data4) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx4.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data4, &m_auth_tx4, sizeof( m_auth_tx4 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table4[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data4,
		sizeof( m_msg_data4 ),
		0 );

	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );	
}


void CMonitorView::OnItemchangedNetTranList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
		CString title;
		AfxGetApp()->m_pMainWnd->GetWindowText(title);
		if(!m_bReadOnly2)
		{	
			CString str;
			m_CntlNetTranList2.GetSelectedItemText(str);
			
			if(szDeviceControllers2.Find(str) > -1 )
			{
				EnableDialogButtons2(false, false);
			}
			else
			{
				EnableDialogButtons2(true, false);
			}
			if(str.Find("ncmcrd") > -1)
			{
				m_CntlAcqOff2.EnableWindow(TRUE);
			}
			else
			{
				m_CntlAcqOff2.EnableWindow(FALSE);
			}

			m_CntlCupKeyReload2.EnableWindow(FALSE);
			if((str.CompareNoCase("nccup3") == 0)||
			   (str.CompareNoCase("ncdci3") == 0))
			{
				m_CntlCupKeyRequest2.EnableWindow(TRUE);
				if((str.CompareNoCase("ncdci2") != 0))
				{
					m_CntlCupKeyReload2.EnableWindow(TRUE);
				}
				
			}	
			else
			{
				m_CntlCupKeyRequest2.EnableWindow(FALSE);
			}

			if(str.CompareNoCase("nccup2") == 0 )				
			{
				m_CntlCupKeySend2.EnableWindow(TRUE);
			}
			else
			{
				m_CntlCupKeySend2.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("nccup3") == 0))
			{
				
				// This is done so that below else if condition should work for only nccup module, not for nccup3 module.
			}
	 		else if((str.CompareNoCase("nccup") == 0))
			{
				m_CntlLogOn2.EnableWindow(FALSE);
				m_CntlLogOff2.EnableWindow(FALSE);
				m_CntlEchoTestButton2.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("ncpul") == 0))
			{
				m_CntlCupKeyRequest2.EnableWindow(TRUE);
				
			}	
		}
    }
	
	*pResult = 0;
}

void CMonitorView::OnLogon2()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR	display_buff[50];
	CHAR	 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList2.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	str.Format("MONITOR2 GH2: Are you sure to logon %s?",str_network_id);

	INT ret = AfxMessageBox(str , MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;

	//m_NetworkStatus2.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table2[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table2[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table2 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data2, 0x00, sizeof(m_msg_data2) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx2.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data2, &m_auth_tx2, sizeof( m_auth_tx2 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_LOGON,
		0,
		(pCHAR)m_ncf01_local_table2 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data2,
		sizeof( m_msg_data2 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending Logon Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}		
	free( p_msg_out );
}


void CMonitorView::OnLogoff2()
{
	ProcessOFFMessage2(ST1_LOGOFF);	
}

void CMonitorView::ProcessOFFMessage2(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList2.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(st1 == ST1_ACQUIRER_OFF)
	{
		str.Format("MONITOR2 GH2: Are you sure to Acquirer off for %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else  if(st1 == ST1_ACQUIRER_ON)
	{
		str.Format("MONITOR2 GH2: Are you sure to Acquirer on %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else
	{
		str.Format("MONITOR2 GH2: Are you sure to logoff %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	//m_NetworkStatus2.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table2[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table2[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table2 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data2, 0x00, sizeof(m_msg_data2) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx2.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data2, &m_auth_tx2, sizeof( m_auth_tx2 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table2[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data2,
		sizeof( m_msg_data2 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}

void CMonitorView::ProcessOFFMessage3(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList3.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(st1 == ST1_ACQUIRER_OFF)
	{
		str.Format("MONITOR3 MKT1: Are you sure to Acquirer off for %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else  if(st1 == ST1_ACQUIRER_ON)
	{
		str.Format("MONITOR3 MKT1: Are you sure to Acquirer on %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else
	{
		str.Format("MONITOR3 MKT1: Are you sure to logoff %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	//m_NetworkStatus3.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table3[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table3[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table3 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data3, 0x00, sizeof(m_msg_data3) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx3.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data3, &m_auth_tx3, sizeof( m_auth_tx3 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table3[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data3,
		sizeof( m_msg_data3 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}

void CMonitorView::ProcessOFFMessage4(int st1)
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	
	CString str;
	m_CntlNetTranList4.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(st1 == ST1_ACQUIRER_OFF)
	{
		str.Format("MONITOR4 MKT2: Are you sure to Acquirer off for %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else  if(st1 == ST1_ACQUIRER_ON)
	{
		str.Format("MONITOR4 MKT2: Are you sure to Acquirer on %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else
	{
		str.Format("MONITOR4 MKT2: Are you sure to logoff %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	//m_NetworkStatus4.GetText(index, display_buff);/*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) // get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table4[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table4[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}  
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table4 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data4, 0x00, sizeof(m_msg_data4) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx4.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data4, &m_auth_tx4, sizeof( m_auth_tx4 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		st1,
		0,
		(pCHAR)m_ncf01_local_table4[table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data4,
		sizeof( m_msg_data4 ),
		0 );

	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
}


void CMonitorView::OnEchotest2()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0;	/*index to local ncf01 table*/
	INT       cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//	index = m_NetworkStatus2.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList2.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
	//m_NetworkStatus2.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table2[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table2[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table2 [table_index].logon_bin,
        sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data2, 0x00, sizeof(m_msg_data2) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx2.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data2, &m_auth_tx2, sizeof( m_auth_tx2 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_ECHOTEST,
		0,
		(pCHAR)m_ncf01_local_table2 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data2,
		sizeof( m_msg_data2 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend2( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR2 GH2: Communication error sending EchoTest Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}	
	free( p_msg_out );
}


void CMonitorView::OnSaf2()
{
	int i;
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	BYTE        subtype1;
	INT         cnt = 0;
	INT         ret = 0;
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	
	memset (display_buff,   0, sizeof(display_buff)  );
	memset (str_network_id, 0, sizeof(str_network_id));
	memset (m_msg_data2,     0, sizeof(m_msg_data2)    );
	
	//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList2.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
		str.Format("MONITOR2 GH2: Are you sure to Get Advises for %s?", str_network_id);
		ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;

	//m_NetworkStatus2.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table2[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table2[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table2 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined, no need to go to Pan Dialog screen.
	*/
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined*/
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		/* Display Primary Account List and process selection */
		ret = PANDlg.DoModal();
		if ( ret == IDCANCEL )
			return;
		
		/* Memcpy the selected account number into the auth_tx field. */
		memset( &m_auth_tx2, 0, sizeof(m_auth_tx2) );
		strcpy( (char *)m_auth_tx2.TLF01_details.card_num, PANDlg.m_pan );
		memcpy( m_msg_data2, &m_auth_tx2, sizeof( m_auth_tx2 ) );
	}
	
	/* Display ADVICES window to allow selection of Advice type. */
	Advices  AdviceDlg;
	ret = AdviceDlg.DoModal();
	if ( (ret != IDC_EXIT) && (ret != IDCANCEL) )
	{
		switch( ret )
		{
		case IDC_START_ADVICES:  subtype1 = ST1_PROCESS_START; break;
		case IDC_STOP_ADVICES:   subtype1 = ST1_SCH_CANCELJOB; break;
		case IDC_SAF:            subtype1 = ST1_SAF_REQUEST;   break;
		}
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			subtype1,
			0,
			(pCHAR)m_ncf01_local_table2[table_index].que_name,
			interactive_que_name,
			(pBYTE)m_msg_data2,
			sizeof( m_msg_data2 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox( "MONITOR2 GH2: Insufficient memory to build request message" );
		}
		else
		{
			if( !PtetcpSend2( p_msg_out, error_msg ) )
			{
				sprintf( buffer,
					"MONITOR2 GH2: Communication error sending Logon Request: %s",
					error_msg );
				AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			}
			free( p_msg_out );
		}
	}
	return;
}


void CMonitorView::OnAcquirerOff2()
{
	if(m_bAcqOn2)
	{
		ProcessOFFMessage2(ST1_ACQUIRER_ON);
		m_CntlAcqOff2.SetWindowText("ACQ OFF");
		m_bAcqOn2 = false;
	}
	else
	{
		ProcessOFFMessage2(ST1_ACQUIRER_OFF);
		m_CntlAcqOff2.SetWindowText("ACQ ON");
		m_bAcqOn2 = true;
	}
}


void CMonitorView::OnPing2()
{
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	int selcount = m_CntlNetTranList2.GetSelectedItemCount();
	if(selcount > 0)
	{
		CString str;
		m_CntlNetTranList2.GetSelectedItemText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox( "MONITOR2 GH2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
			return;
		}
		strcpy( queue, str);
		strcat( queue, "C" );
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend2( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR2 GH2: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count2; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats2[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend2( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR2 GH2: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}


void CMonitorView::OnResetCountsButton2()
{
	INT ret = AfxMessageBox( "MONITOR2 GH2: Are you sure to Reset All the counts?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	/*Set activity counts to zero*/
	//InitializeActivityStatsList2();
	if(m_CntlNetTranList2.Lock() == 0)
	{
		AfxMessageBox("MONITOR2 GH2: List is being updated. Please try again.");
		return;
	}
	int count = m_CntlNetTranList2.GetItemCount();
	for(int i=0;i<count;i++)
	{
		m_CntlNetTranList2.SetItemText(i, LS_SENT, "0");
		m_CntlNetTranList2.SetItemText(i, LS_RECEIVED,"0");
		m_CntlNetTranList2.SetItemText(i, LS_SENT,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_RATE,  "0.0");
		m_CntlNetTranList2.SetItemText(i, LS_APPROVED,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_DECLINED,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_TIMEDOUT,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_REVERSED,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_MGT_COUNTS,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_STIP_SENT,  "0");
		m_CntlNetTranList2.SetItemText(i, LS_STIP_RECV,  "0");
	}
	m_CntlNetTranList2.UnLock();

	/*Send MT_RESET_STATS to all Controllers*/
	SendResetControllerStats2();
		
	/*Disable Reset Counts Button*/
	m_CntlResetCountsButton2.EnableWindow(FALSE);
	
}


void CMonitorView::OnCupKeyRequestButton2()
{
	ProcessCUPMessage2(ST1_CUP_KEY_REQUEST);
}


void CMonitorView::OnCupKeySendButton2()
{
	ProcessCUPMessage2(ST1_CUP_KEY_SEND);
}


void CMonitorView::OnCupKeyReloadButton2()
{
	ProcessCUPMessage2(STI_CUP_KEY_RELOAD);
}


void CMonitorView::OnPingButton2()
{
	// TODO: Add your control notification handler code here
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList2.GetSelCount())
	{
		i = m_TranActivityList2.GetCurSel();
		m_TranActivityList2.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend2( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR2 GH2: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
		
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count2; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats2[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend2( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR2 GH2: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}

void CMonitorView::OnPingButton3()
{
	// TODO: Add your control notification handler code here
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList3.GetSelCount())
	{
		i = m_TranActivityList3.GetCurSel();
		m_TranActivityList3.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend3( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
		
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count3; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats3[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend3( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}

void CMonitorView::OnPingButton4()
{
	// TODO: Add your control notification handler code here
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList4.GetSelCount())
	{
		i = m_TranActivityList4.GetCurSel();
		m_TranActivityList4.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
			return;
		}
		else
		{
			if( !PtetcpSend4( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}	
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count4; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats4[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
				return;
			}
			else
			{
				if( !PtetcpSend4( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}


void CMonitorView::OnPingDialogButton2()
{
	CHAR     text[120];
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList2.GetSelCount())
	{
		i = m_TranActivityList2.GetCurSel();
		m_TranActivityList2.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend2( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR2 GH2: Failed to transmit  Dialog status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Dialog_ID_Count2; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Dialog_Status2[i].dialog_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );
			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR2 GH2: Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend2( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR2 GH2: Failed to transmit  Dialog status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}


void CMonitorView::OnLogon3()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR	 display_buff[50];
	CHAR	 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList3.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	str.Format("MONITOR3 MKT1: Are you sure to logon %s?",str_network_id);

	INT ret = AfxMessageBox(str , MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;

	//m_NetworkStatus3.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table3[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table3[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}	
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table3 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data3, 0x00, sizeof(m_msg_data3) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx3.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data3, &m_auth_tx3, sizeof( m_auth_tx3 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_LOGON,
		0,
		(pCHAR)m_ncf01_local_table3 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data3,
		sizeof( m_msg_data3 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending Logon Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}		
	free( p_msg_out );
}

void CMonitorView::OnLogon4()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR	 display_buff[50];
	CHAR	 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0; /*index to local ncf01 table*/
	INT         cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
//	index = m_NetworkStatus4.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList4.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	str.Format("MONITOR4 MKT2: Are you sure to logon %s?",str_network_id);

	INT ret = AfxMessageBox(str , MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;

	//m_NetworkStatus4.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table4[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table4[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}	
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table4 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data4, 0x00, sizeof(m_msg_data4) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx4.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data4, &m_auth_tx4, sizeof( m_auth_tx4 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_LOGON,
		0,
		(pCHAR)m_ncf01_local_table4 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data4,
		sizeof( m_msg_data4 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending Logon Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}		
	free( p_msg_out );
}

void CMonitorView::OnLogoff3()
{
	ProcessOFFMessage3(ST1_LOGOFF);	
}

void CMonitorView::OnLogoff4()
{
	ProcessOFFMessage4(ST1_LOGOFF);	
}


void CMonitorView::OnEchotest3()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0;	/*index to local ncf01 table*/
	INT       cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//	index = m_NetworkStatus2.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList3.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
	//m_NetworkStatus3.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table3[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table3[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table3 [table_index].logon_bin,
        sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data3, 0x00, sizeof(m_msg_data3) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx3.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data3, &m_auth_tx3, sizeof( m_auth_tx3 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_ECHOTEST,
		0,
		(pCHAR)m_ncf01_local_table3 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data3,
		sizeof( m_msg_data3 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend3( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR3 MKT1: Communication error sending EchoTest Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}	
	free( p_msg_out );
}

void CMonitorView::OnEchotest4()
{
	int i;
	// TODO: Add your control notification handler code here
	pPTE_MSG  p_msg_out;
	CHAR      error_msg[256], buffer[300];
	CHAR		 display_buff[50];
	CHAR		 str_network_id[11];
	INT		 index = 0;
	INT		 buf_index = 0;
	INT		 str_index = 0;
	INT		 table_index = 0;	/*index to local ncf01 table*/
	INT       cnt = 0;
	
	memset (display_buff, 0, sizeof(display_buff));
	memset (str_network_id, 0, sizeof(str_network_id));
	
	//	index = m_NetworkStatus2.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList4.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
	//m_NetworkStatus4.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table4[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table4[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}
		
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table4 [table_index].logon_bin,
        sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined,
		* no need to go to Pan Dialog screen
	*/
	memset( m_msg_data4, 0x00, sizeof(m_msg_data4) );
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined */
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		INT ret = PANDlg.DoModal();
		if( ret == IDCANCEL )
			return;
		
		// memcpy the selected account number into the auth_tx field
		if ( strcmp( PANDlg.m_pan, "(No Bin)" ) != 0 )
		{
			strcpy( (char *)m_auth_tx4.TLF01_details.card_num, PANDlg.m_pan );
			memcpy( m_msg_data4, &m_auth_tx4, sizeof( m_auth_tx4 ) );
		}
	}
	
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
		ST1_ECHOTEST,
		0,
		(pCHAR)m_ncf01_local_table4 [table_index].que_name,
		interactive_que_name,
		(pBYTE)m_msg_data4,
		sizeof( m_msg_data4 ),
		0 );
	if( p_msg_out == NULL_PTR )
	{
		AfxMessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend4( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "MONITOR4 MKT2: Communication error sending EchoTest Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}	
	free( p_msg_out );
}

void CMonitorView::OnSaf3()
{
	int i;
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	BYTE        subtype1;
	INT         cnt = 0;
	INT         ret = 0;
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	
	memset (display_buff,   0, sizeof(display_buff)  );
	memset (str_network_id, 0, sizeof(str_network_id));
	memset (m_msg_data3,     0, sizeof(m_msg_data3)    );
	
	//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList3.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
		str.Format("MONITOR3 MKT1: Are you sure to Get Advises for %s?", str_network_id);
		ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;

	//m_NetworkStatus3.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table3[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table3[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}	
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table3 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined, no need to go to Pan Dialog screen.
	*/
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined*/
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		/* Display Primary Account List and process selection */
		ret = PANDlg.DoModal();
		if ( ret == IDCANCEL )
			return;
		
		/* Memcpy the selected account number into the auth_tx field. */
		memset( &m_auth_tx3, 0, sizeof(m_auth_tx3) );
		strcpy( (char *)m_auth_tx3.TLF01_details.card_num, PANDlg.m_pan );
		memcpy( m_msg_data3, &m_auth_tx3, sizeof( m_auth_tx3 ) );
	}
	
	/* Display ADVICES window to allow selection of Advice type. */
	Advices  AdviceDlg;
	ret = AdviceDlg.DoModal();
	if ( (ret != IDC_EXIT) && (ret != IDCANCEL) )
	{
		switch( ret )
		{
		case IDC_START_ADVICES:  subtype1 = ST1_PROCESS_START; break;
		case IDC_STOP_ADVICES:   subtype1 = ST1_SCH_CANCELJOB; break;
		case IDC_SAF:            subtype1 = ST1_SAF_REQUEST;   break;
		}
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			subtype1,
			0,
			(pCHAR)m_ncf01_local_table3[table_index].que_name,
			interactive_que_name,
			(pBYTE)m_msg_data3,
			sizeof( m_msg_data3 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox( "MONITOR3 MKT1: Insufficient memory to build request message" );
		}
		else
		{
			if( !PtetcpSend3( p_msg_out, error_msg ) )
			{
				sprintf( buffer,
					"MONITOR3 MKT1: Communication error sending Logon Request: %s",
					error_msg );
				AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			}
			free( p_msg_out );
		}
	}
	return;
}

void CMonitorView::OnSaf4()
{
	int i;
	pPTE_MSG    p_msg_out;
	CHAR        error_msg[256], buffer[300];
	CHAR		   display_buff[50];
	CHAR		   str_network_id[11];
	BYTE        subtype1;
	INT         cnt = 0;
	INT         ret = 0;
	INT		   index = 0;
	INT		   buf_index = 0;
	INT		   str_index = 0;
	INT		   table_index = 0; /*index to local ncf01 table*/
	
	memset (display_buff,   0, sizeof(display_buff)  );
	memset (str_network_id, 0, sizeof(str_network_id));
	memset (m_msg_data4,     0, sizeof(m_msg_data4)    );
	
	//	index = m_NetworkStatus.GetCurSel();			/*get position in list of selected item*/
	CString str;
	m_CntlNetTranList4.GetSelectedItemText(str);
	if(str.IsEmpty())
	{
		AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
		str.Format("MONITOR4 MKT2: Are you sure to Get Advises for %s?", str_network_id);
		ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;

	//m_NetworkStatus4.GetText(index, display_buff); /*get selected string data from list box.*/
	/*
	for (buf_index = 0, str_index = 0; buf_index < sizeof(str_network_id); buf_index++)
	{
		if(isalnum(display_buff[buf_index])) //get network_id
			str_network_id[str_index++] = display_buff[buf_index];
		else
			break;
	}
	*/
	
	/*get correct record index from local_table*/
	for (int i = 0; strlen((pCHAR)m_ncf01_local_table4[i].primary_key.network_id) != 0; i++)
	{
		if (strcmp( (pCHAR)m_ncf01_local_table4[i].primary_key.network_id, str_network_id) == 0)
		{
			table_index = i;	
			break;
		}	
	}
	
	CPrimaryAccountList PANDlg;
	memset( &PANDlg.m_logon_bin, 0, sizeof( PANDlg.m_logon_bin ) );
	memcpy( &PANDlg.m_logon_bin, m_ncf01_local_table4 [table_index].logon_bin,
		sizeof( PANDlg.m_logon_bin ) ); 
	
		/* Check for defined logon bins in ncf01;
		* If there are no logon bins defined, no need to go to Pan Dialog screen.
	*/
	for (i = 0, cnt = 0; i < 10; i++)  
	{
		/* Get count of logon bins defined*/
		if (strlen((pCHAR)PANDlg.m_logon_bin[i].identifier) != 0)
			cnt++;
	}
	
	if (cnt > 0)	/*Display Primary Account List and process selection*/
	{
		/* Display Primary Account List and process selection */
		ret = PANDlg.DoModal();
		if ( ret == IDCANCEL )
			return;
		
		/* Memcpy the selected account number into the auth_tx field. */
		memset( &m_auth_tx4, 0, sizeof(m_auth_tx4) );
		strcpy( (char *)m_auth_tx4.TLF01_details.card_num, PANDlg.m_pan );
		memcpy( m_msg_data4, &m_auth_tx4, sizeof( m_auth_tx4 ) );
	}
	
	/* Display ADVICES window to allow selection of Advice type. */
	Advices  AdviceDlg;
	ret = AdviceDlg.DoModal();
	if ( (ret != IDC_EXIT) && (ret != IDCANCEL) )
	{
		switch( ret )
		{
		case IDC_START_ADVICES:  subtype1 = ST1_PROCESS_START; break;
		case IDC_STOP_ADVICES:   subtype1 = ST1_SCH_CANCELJOB; break;
		case IDC_SAF:            subtype1 = ST1_SAF_REQUEST;   break;
		}
		p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,
			subtype1,
			0,
			(pCHAR)m_ncf01_local_table4[table_index].que_name,
			interactive_que_name,
			(pBYTE)m_msg_data4,
			sizeof( m_msg_data4 ),
			0 );
		if( p_msg_out == NULL_PTR )
		{
			AfxMessageBox( "MONITOR4 MKT2: Insufficient memory to build request message" );
		}
		else
		{
			if( !PtetcpSend4( p_msg_out, error_msg ) )
			{
				sprintf( buffer,
					"MONITOR4 MKT2: Communication error sending Logon Request: %s",
					error_msg );
				AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
			}
			free( p_msg_out );
		}
	}
	return;
}


void CMonitorView::OnAcquirerOff3()
{
	if(m_bAcqOn3)
	{
		ProcessOFFMessage3(ST1_ACQUIRER_ON);
		m_CntlAcqOff3.SetWindowText("ACQ OFF");
		m_bAcqOn3 = false;
	}
	else
	{
		ProcessOFFMessage3(ST1_ACQUIRER_OFF);
		m_CntlAcqOff3.SetWindowText("ACQ ON");
		m_bAcqOn3 = true;
	}
}

void CMonitorView::OnAcquirerOff4()
{
	if(m_bAcqOn4)
	{
		ProcessOFFMessage4(ST1_ACQUIRER_ON);
		m_CntlAcqOff4.SetWindowText("ACQ OFF");
		m_bAcqOn4 = false;
	}
	else
	{
		ProcessOFFMessage4(ST1_ACQUIRER_OFF);
		m_CntlAcqOff4.SetWindowText("ACQ ON");
		m_bAcqOn4 = true;
	}
}


void CMonitorView::OnPing3()
{
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	int selcount = m_CntlNetTranList3.GetSelectedItemCount();
	if(selcount > 0)
	{
		CString str;
		m_CntlNetTranList3.GetSelectedItemText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox( "MONITOR3 MKT1: Please select a network ID from list", MB_ICONSTOP | MB_OK );
			return;
		}
		strcpy( queue, str);
		strcat( queue, "C" );
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );

		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
			return;
		}	
		else
		{
			if( !PtetcpSend3( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count3; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats3[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );

			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
				return;
			}	
			else
			{
				if( !PtetcpSend3( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}

void CMonitorView::OnPing4()
{
	CHAR     text[120] = {0};
	CHAR     seps[]   = " ,\t\n";
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	int selcount = m_CntlNetTranList4.GetSelectedItemCount();
	if(selcount > 0)
	{
		CString str;
		m_CntlNetTranList4.GetSelectedItemText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox( "MONITOR4 MKT2: Please select a network ID from list", MB_ICONSTOP | MB_OK );
			return;
		}
		strcpy( queue, str);
		strcat( queue, "C" );
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );

		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
			return;
		}	
		else
		{
			if( !PtetcpSend4( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Controller_Count4; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Controller_Stats4[i].service_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );

			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
				return;
			}	
			else
			{
				if( !PtetcpSend4( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}

void CMonitorView::OnResetCountsButton3()
{
	INT ret = AfxMessageBox( "MONITOR3 MKT1: Are you sure to Reset All the counts?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	/*Set activity counts to zero*/
	//InitializeActivityStatsList2();
	if(m_CntlNetTranList3.Lock() == 0)
	{
		AfxMessageBox("MONITOR3 MKT1: List is being updated. Please try again.");
		return;
	}
	int count = m_CntlNetTranList3.GetItemCount();
	for(int i=0;i<count;i++)
	{
		m_CntlNetTranList3.SetItemText(i, LS_SENT, "0");
		m_CntlNetTranList3.SetItemText(i, LS_RECEIVED,"0");
		m_CntlNetTranList3.SetItemText(i, LS_SENT,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_RATE,  "0.0");
		m_CntlNetTranList3.SetItemText(i, LS_APPROVED,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_DECLINED,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_TIMEDOUT,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_REVERSED,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_MGT_COUNTS,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_STIP_SENT,  "0");
		m_CntlNetTranList3.SetItemText(i, LS_STIP_RECV,  "0");
	}
	m_CntlNetTranList3.UnLock();

	/*Send MT_RESET_STATS to all Controllers*/
	SendResetControllerStats3();
		
	/*Disable Reset Counts Button*/
	m_CntlResetCountsButton3.EnableWindow(FALSE);
}

void CMonitorView::OnResetCountsButton4()
{
	INT ret = AfxMessageBox( "MONITOR4 MKT2: Are you sure to Reset All the counts?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	/*Set activity counts to zero*/
	//InitializeActivityStatsList2();
	if(m_CntlNetTranList4.Lock() == 0)
	{
		AfxMessageBox("MONITOR4 MKT2: List is being updated. Please try again.");
		return;
	}
	int count = m_CntlNetTranList4.GetItemCount();
	for(int i=0;i<count;i++)
	{
		m_CntlNetTranList4.SetItemText(i, LS_SENT, "0");
		m_CntlNetTranList4.SetItemText(i, LS_RECEIVED,"0");
		m_CntlNetTranList4.SetItemText(i, LS_SENT,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_RATE,  "0.0");
		m_CntlNetTranList4.SetItemText(i, LS_APPROVED,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_DECLINED,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_TIMEDOUT,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_REVERSED,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_MGT_COUNTS,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_STIP_SENT,  "0");
		m_CntlNetTranList4.SetItemText(i, LS_STIP_RECV,  "0");
	}
	m_CntlNetTranList4.UnLock();

	/*Send MT_RESET_STATS to all Controllers*/
	SendResetControllerStats4();
		
	/*Disable Reset Counts Button*/
	m_CntlResetCountsButton4.EnableWindow(FALSE);
}

void CMonitorView::OnCupKeySendButton3()
{
	ProcessCUPMessage3(ST1_CUP_KEY_SEND);
}

void CMonitorView::OnCupKeyRequestButton3()
{
	ProcessCUPMessage3(ST1_CUP_KEY_REQUEST);
}

void CMonitorView::OnCupKeyReloadButton3()
{
	ProcessCUPMessage3(STI_CUP_KEY_RELOAD);
}


void CMonitorView::OnCupKeySendButton4()
{
	ProcessCUPMessage4(ST1_CUP_KEY_SEND);
}

void CMonitorView::OnCupKeyRequestButton4()
{
	ProcessCUPMessage4(ST1_CUP_KEY_REQUEST);
}

void CMonitorView::OnCupKeyReloadButton4()
{
	ProcessCUPMessage4(STI_CUP_KEY_RELOAD);
}

void CMonitorView::OnPingDialogButton3()
{
	CHAR     text[120];
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList3.GetSelCount())
	{
		i = m_TranActivityList3.GetCurSel();
		m_TranActivityList3.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend3( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit  Dialog status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Dialog_ID_Count3; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Dialog_Status3[i].dialog_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );

			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR3 MKT1: Insufficient memory to build request message");
				return;
			}
			else
			{
				if( !PtetcpSend3( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR3 MKT1: Failed to transmit  Dialog status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}

void CMonitorView::OnPingDialogButton4()
{
	CHAR     text[120];
	CHAR     seps[]   = " ,\t\n";
	CHAR     *token;
	INT      i;   
	CHAR     queue[MAX_QUE_NAME_SIZE+1];
	pPTE_MSG p_msg_out;
	CHAR     temp_str[200] , error_msg[100];
	p_msg_out =NULL_PTR;
	/*	p_msg_in  =NULL_PTR;*/
	error_msg[0] = NULL_PTR;
	INT  queue_size;
	
	if(	m_TranActivityList4.GetSelCount())
	{
		i = m_TranActivityList4.GetCurSel();
		m_TranActivityList4.GetText(i,text);     
		printf( "%s\n\nTokens:\n", text );
		/* Establish string and get the first token: */
		token = strtok( text, seps );     
		
		strcpy( queue, token);
		strcat( queue, "C" );
		
		p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
			ST1_SYS_PING,
			0,						
			queue,
			interactive_que_name,
			NULL_PTR,
			0,
			0 );
		if (p_msg_out== NULL_PTR)
		{
			strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend4( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit  Dialog status request to queue: " );
				strcat( error_msg, queue );
				strcat( error_msg, ".  " );
				strncat( error_msg, temp_str, 40 );
				return;
			}
		}
	}
	else
	{
		queue_size = sizeof(queue);
		for(i=0; i<Dialog_ID_Count4; i++)
		{
			memset( queue, 0x00, queue_size );
			strcpy( queue, Dialog_Status4[i].dialog_id );
			strcat( queue, "C" );
			p_msg_out = ptemsg_build_msg(MT_SYSTEM_REQUEST,
				ST1_SYS_PING,
				0,						
				queue,
				interactive_que_name,
				NULL_PTR,
				0,
				0 );

			if (p_msg_out== NULL_PTR)
			{
				strcpy(error_msg,"MONITOR4 MKT2: Insufficient memory to build request message");
				return;
			}
			else
			{
				if( !PtetcpSend4( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "MONITOR4 MKT2: Failed to transmit  Dialog status request to queue: " );
					strcat( error_msg, queue );
					strcat( error_msg, ".  " );
					strncat( error_msg, temp_str, 40 );
					return;
				}
			}	
		}
	}
}


void CMonitorView::OnItemchangedNetTranList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
		CString title;
		AfxGetApp()->m_pMainWnd->GetWindowText(title);
		if(!m_bReadOnly3)
		{	
			CString str;
			m_CntlNetTranList3.GetSelectedItemText(str);
			
			if(szDeviceControllers3.Find(str) > -1 )
			{
				EnableDialogButtons3(false, false);
			}
			else
			{
				EnableDialogButtons3(true, false);
			}
			if(str.Find("ncmcrd") > -1)
			{
				m_CntlAcqOff3.EnableWindow(TRUE);
			}
			else
			{
				m_CntlAcqOff3.EnableWindow(FALSE);
			}

			m_CntlCupKeyReload3.EnableWindow(FALSE);
			if((str.CompareNoCase("nccup3") == 0)||
			   (str.CompareNoCase("ncdci3") == 0))
			{
				m_CntlCupKeyRequest3.EnableWindow(TRUE);
				if((str.CompareNoCase("ncdci2") != 0))
				{
					m_CntlCupKeyReload3.EnableWindow(TRUE);
				}	
			}	
			else
			{
				m_CntlCupKeyRequest3.EnableWindow(FALSE);
			}

			if(str.CompareNoCase("nccup2") == 0 )				
			{
				m_CntlCupKeySend3.EnableWindow(TRUE);
			}
			else
			{
				m_CntlCupKeySend3.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("nccup3") == 0))
			{
				
				// This is done so that below else if condition should work for only nccup module, not for nccup3 module.
			}
	 		else if((str.CompareNoCase("nccup") == 0))
			{
				m_CntlLogOn3.EnableWindow(FALSE);
				m_CntlLogOff3.EnableWindow(FALSE);
				m_CntlEchoTestButton3.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("ncpul") == 0))
			{
				m_CntlCupKeyRequest3.EnableWindow(TRUE);
				
			}	
		}
    }
	
	*pResult = 0;
}

void CMonitorView::OnItemchangedNetTranList4(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE) 
        && (pNMListView->uNewState & LVNI_SELECTED))
    {
		CString title;
		AfxGetApp()->m_pMainWnd->GetWindowText(title);
		if(!m_bReadOnly4)
		{	
			CString str;
			m_CntlNetTranList4.GetSelectedItemText(str);
			
			if(szDeviceControllers4.Find(str) > -1 )
			{
				EnableDialogButtons4(false, false);
			}
			else
			{
				EnableDialogButtons4(true, false);
			}
			if(str.Find("ncmcrd") > -1)
			{
				m_CntlAcqOff4.EnableWindow(TRUE);
			}
			else
			{
				m_CntlAcqOff4.EnableWindow(FALSE);
			}

			m_CntlCupKeyReload4.EnableWindow(FALSE);
			if((str.CompareNoCase("nccup3") == 0)||
			   (str.CompareNoCase("ncdci3") == 0))
			{
				m_CntlCupKeyRequest4.EnableWindow(TRUE);
				if((str.CompareNoCase("ncdci2") != 0))
				{
					m_CntlCupKeyReload4.EnableWindow(TRUE);
				}	
			}	
			else
			{
				m_CntlCupKeyRequest4.EnableWindow(FALSE);
			}

			if(str.CompareNoCase("nccup2") == 0 )				
			{
				m_CntlCupKeySend4.EnableWindow(TRUE);
			}
			else
			{
				m_CntlCupKeySend4.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("nccup3") == 0))
			{	
				// This is done so that below else if condition should work for only nccup module, not for nccup3 module.
			}
	 		else if((str.CompareNoCase("nccup") == 0))
			{
				m_CntlLogOn4.EnableWindow(FALSE);
				m_CntlLogOff4.EnableWindow(FALSE);
				m_CntlEchoTestButton4.EnableWindow(FALSE);
			}
			if((str.CompareNoCase("ncpul") == 0))
			{
				m_CntlCupKeyRequest4.EnableWindow(TRUE);
			}	
		}
    }
	
	*pResult = 0;
}

void CMonitorView::OnSelchangeMonitorTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_MainTab1.GetCurSel();
	if(index == 0)
	{
		m_DialogConnectStatus.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton.EnableWindow(TRUE);
		m_LogMessages.ShowWindow(SW_HIDE);
		m_CntlClearLogButton.ShowWindow(SW_HIDE);
		m_CntlSaveLogButton.ShowWindow(SW_HIDE);
		m_TestResult.ShowWindow(SW_HIDE);
		m_TEST_BUTTON.ShowWindow(SW_HIDE);

		m_DialogIdLabel.ShowWindow(SW_SHOW);
		m_LogStatusLabel.ShowWindow(SW_SHOW);
		m_NameLabel.ShowWindow(SW_SHOW);
		m_LogDateLabel.ShowWindow(SW_HIDE);
		m_LogTimeLabel.ShowWindow(SW_HIDE);
		m_LogTypeLabel.ShowWindow(SW_HIDE);
		m_LogServiceLabel.ShowWindow(SW_HIDE);
		m_LogMessageLabel.ShowWindow(SW_HIDE);
	}
	else
	{
		m_DialogConnectStatus.ShowWindow(SW_HIDE);
		m_CntlPingDialogButton.ShowWindow(SW_HIDE);
		m_LogMessages.ShowWindow(SW_SHOW);
		m_LogMessages.SetFocus();
		m_CntlClearLogButton.ShowWindow(SW_SHOW);
		m_CntlSaveLogButton.ShowWindow(SW_SHOW);
		m_TestResult.ShowWindow(SW_SHOW);
		m_TEST_BUTTON.ShowWindow(SW_SHOW);

		m_DialogIdLabel.ShowWindow(SW_HIDE);
		m_LogStatusLabel.ShowWindow(SW_HIDE);
		m_NameLabel.ShowWindow(SW_HIDE);
		m_LogDateLabel.ShowWindow(SW_SHOW);
		m_LogTimeLabel.ShowWindow(SW_SHOW);
		m_LogTypeLabel.ShowWindow(SW_SHOW);
		m_LogServiceLabel.ShowWindow(SW_SHOW);
		m_LogMessageLabel.ShowWindow(SW_SHOW);
	}

	*pResult = 0;
}

void CMonitorView::OnSelchangeMonitorTab2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_MainTab2.GetCurSel();
	if(index == 0)
	{
		m_DialogConnectStatus2.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton2.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton2.EnableWindow(TRUE);
		m_LogMessages2.ShowWindow(SW_HIDE);
		m_CntlClearLogButton2.ShowWindow(SW_HIDE);
		m_CntlSaveLogButton2.ShowWindow(SW_HIDE);
		m_TestResult2.ShowWindow(SW_HIDE);
		m_TEST_BUTTON2.ShowWindow(SW_HIDE);

		m_DialogIdLabel2.ShowWindow(SW_SHOW);
		m_LogStatusLabel2.ShowWindow(SW_SHOW);
		m_NameLabel2.ShowWindow(SW_SHOW);
		m_LogDateLabel2.ShowWindow(SW_HIDE);
		m_LogTimeLabel2.ShowWindow(SW_HIDE);
		m_LogTypeLabel2.ShowWindow(SW_HIDE);
		m_LogServiceLabel2.ShowWindow(SW_HIDE);
		m_LogMessageLabel2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_DialogConnectStatus2.ShowWindow(SW_HIDE);
		m_CntlPingDialogButton2.ShowWindow(SW_HIDE);
		m_LogMessages2.ShowWindow(SW_SHOW);
		m_LogMessages2.SetFocus();
		m_CntlClearLogButton2.ShowWindow(SW_SHOW);
		m_CntlSaveLogButton2.ShowWindow(SW_SHOW);
		m_TestResult2.ShowWindow(SW_SHOW);
		m_TEST_BUTTON2.ShowWindow(SW_SHOW);

		m_DialogIdLabel2.ShowWindow(SW_HIDE);
		m_LogStatusLabel2.ShowWindow(SW_HIDE);
		m_NameLabel2.ShowWindow(SW_HIDE);
		m_LogDateLabel2.ShowWindow(SW_SHOW);
		m_LogTimeLabel2.ShowWindow(SW_SHOW);
		m_LogTypeLabel2.ShowWindow(SW_SHOW);
		m_LogServiceLabel2.ShowWindow(SW_SHOW);
		m_LogMessageLabel2.ShowWindow(SW_SHOW);
	}

	*pResult = 0;
}

void CMonitorView::OnSelchangeMonitorTab3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_MainTab3.GetCurSel();
	if(index == 0)
	{
		m_DialogConnectStatus3.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton3.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton3.EnableWindow(TRUE);
		m_LogMessages3.ShowWindow(SW_HIDE);
		m_CntlClearLogButton3.ShowWindow(SW_HIDE);
		m_CntlSaveLogButton3.ShowWindow(SW_HIDE);
		m_TestResult3.ShowWindow(SW_HIDE);
		m_TEST_BUTTON3.ShowWindow(SW_HIDE);

		m_DialogIdLabel3.ShowWindow(SW_SHOW);
		m_LogStatusLabel3.ShowWindow(SW_SHOW);
		m_NameLabel3.ShowWindow(SW_SHOW);
		m_LogDateLabel3.ShowWindow(SW_HIDE);
		m_LogTimeLabel3.ShowWindow(SW_HIDE);
		m_LogTypeLabel3.ShowWindow(SW_HIDE);
		m_LogServiceLabel3.ShowWindow(SW_HIDE);
		m_LogMessageLabel3.ShowWindow(SW_HIDE);
	}
	else
	{
		m_DialogConnectStatus3.ShowWindow(SW_HIDE);
		m_CntlPingDialogButton3.ShowWindow(SW_HIDE);
		m_LogMessages3.ShowWindow(SW_SHOW);
		m_LogMessages3.SetFocus();
		m_CntlClearLogButton3.ShowWindow(SW_SHOW);
		m_CntlSaveLogButton3.ShowWindow(SW_SHOW);
		m_TestResult3.ShowWindow(SW_SHOW);
		m_TEST_BUTTON3.ShowWindow(SW_SHOW);

		m_DialogIdLabel3.ShowWindow(SW_HIDE);
		m_LogStatusLabel3.ShowWindow(SW_HIDE);
		m_NameLabel3.ShowWindow(SW_HIDE);
		m_LogDateLabel3.ShowWindow(SW_SHOW);
		m_LogTimeLabel3.ShowWindow(SW_SHOW);
		m_LogTypeLabel3.ShowWindow(SW_SHOW);
		m_LogServiceLabel3.ShowWindow(SW_SHOW);
		m_LogMessageLabel3.ShowWindow(SW_SHOW);
	}

	*pResult = 0;
}

void CMonitorView::OnSelchangeMonitorTab4(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int index = m_MainTab4.GetCurSel();
	if(index == 0)
	{
		m_DialogConnectStatus4.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton4.ShowWindow(SW_SHOW);
		m_CntlPingDialogButton4.EnableWindow(TRUE);
		m_LogMessages4.ShowWindow(SW_HIDE);
		m_CntlClearLogButton4.ShowWindow(SW_HIDE);
		m_CntlSaveLogButton4.ShowWindow(SW_HIDE);
		m_TestResult4.ShowWindow(SW_HIDE);
		m_TEST_BUTTON4.ShowWindow(SW_HIDE);

		m_DialogIdLabel4.ShowWindow(SW_SHOW);
		m_LogStatusLabel4.ShowWindow(SW_SHOW);
		m_NameLabel4.ShowWindow(SW_SHOW);
		m_LogDateLabel4.ShowWindow(SW_HIDE);
		m_LogTimeLabel4.ShowWindow(SW_HIDE);
		m_LogTypeLabel4.ShowWindow(SW_HIDE);
		m_LogServiceLabel4.ShowWindow(SW_HIDE);
		m_LogMessageLabel4.ShowWindow(SW_HIDE);
	}
	else
	{
		m_DialogConnectStatus4.ShowWindow(SW_HIDE);
		m_CntlPingDialogButton4.ShowWindow(SW_HIDE);
		m_LogMessages4.ShowWindow(SW_SHOW);
		m_LogMessages4.SetFocus();
		m_CntlClearLogButton4.ShowWindow(SW_SHOW);
		m_CntlSaveLogButton4.ShowWindow(SW_SHOW);
		m_TestResult4.ShowWindow(SW_SHOW);
		m_TEST_BUTTON4.ShowWindow(SW_SHOW);

		m_DialogIdLabel4.ShowWindow(SW_HIDE);
		m_LogStatusLabel4.ShowWindow(SW_HIDE);
		m_NameLabel4.ShowWindow(SW_HIDE);
		m_LogDateLabel4.ShowWindow(SW_SHOW);
		m_LogTimeLabel4.ShowWindow(SW_SHOW);
		m_LogTypeLabel4.ShowWindow(SW_SHOW);
		m_LogServiceLabel4.ShowWindow(SW_SHOW);
		m_LogMessageLabel4.ShowWindow(SW_SHOW);
	}

	*pResult = 0;
}

void CMonitorView::OnMuteAll()
{
	int mute_all = m_CntlMuteAll.GetCheck();

	if(mute_all)
	{
		//Mute all the monitors
		m_CntlMuteBeep.SetCheck(1);
		m_CntlMuteBeep2.SetCheck(1);
		m_CntlMuteBeep3.SetCheck(1);
		m_CntlMuteBeep4.SetCheck(1);
	}
	else
	{
		//Unmute all the monitors if it is up.
		if(m_bMonitorUp)
			m_CntlMuteBeep.SetCheck(0);
		if(m_bMonitorUp2)
			m_CntlMuteBeep2.SetCheck(0);
		if(m_bMonitorUp3)
			m_CntlMuteBeep3.SetCheck(0);
		if(m_bMonitorUp4)
			m_CntlMuteBeep4.SetCheck(0);
	}
}


/*New messagebox timeout api definitions starts*/
int MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, 
    LPCSTR lpCaption, UINT uType, WORD wLanguageId, 
    DWORD dwMilliseconds)
{
    static MSGBOXAAPI MsgBoxTOA = NULL;

    if (!MsgBoxTOA)
    {
        HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
        if (hUser32)
        {
            MsgBoxTOA = (MSGBOXAAPI)GetProcAddress(hUser32, 
                                      "MessageBoxTimeoutA");
            //fall through to 'if (MsgBoxTOA)...'
        }
        else
        {
            //stuff happened, add code to handle it here 
            //(possibly just call MessageBox())
            return 0;
        }
    }

    if (MsgBoxTOA)
    {
        return MsgBoxTOA(hWnd, lpText, lpCaption, 
              uType, wLanguageId, dwMilliseconds);
    }

    return 0;
}

int MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, 
    LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds)
{
    static MSGBOXWAPI MsgBoxTOW = NULL;

    if (!MsgBoxTOW)
    {
        HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
        if (hUser32)
        {
            MsgBoxTOW = (MSGBOXWAPI)GetProcAddress(hUser32, 
                                      "MessageBoxTimeoutW");
            //fall through to 'if (MsgBoxTOW)...'
        }
        else
        {
            //stuff happened, add code to handle it here 
            //(possibly just call MessageBox())
            return 0;
        }
    }

    if (MsgBoxTOW)
    {
        return MsgBoxTOW(hWnd, lpText, lpCaption, 
               uType, wLanguageId, dwMilliseconds);
    }

    return 0;
}
/*New messagebox timeout api definitions ends*/
