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
#include "monitor.h"
#include "monitorDoc.h"
#include "monitorView.h"
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

// JMG added on 2/7/02 to save host name for auto reconnection after initial connection
// failure. This buffer will contain data only if the initial connection fails.
CHAR  hostname_saved[50];

/* Global structures used for holding statistics for individual services. */
CONTROLLER_STATS  Controller_Stats[MAX_INI_ID_LIST_SIZE*2];
INT               Controller_Count;

DIALOG_STATUS   Dialog_Status[MAX_INI_ID_LIST_SIZE];
INT             Dialog_ID_Count;

/* Interval in seconds for timer to request statistics and statuses */
INT   Timer_Interval;

//Time interval for Messagebox, which causes queueing if left non-responded
DWORD promptAlive;
//File writing flag for time out alerts
INT timeoutFileFlag;

CString szDeviceControllers;
//ramya
CString szMracon;


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
	//}}AFX_MSG_MAP
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
}

CMonitorView::~CMonitorView()
{
}

void CMonitorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorView)
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
	CHAR  hostname[50] = {0};
	CHAR  buffer[256], error_msg[100];
	INT  nTabStop[2] = {0,0};
	INT  nDialogTabStop[2] = {0,0};
	INT  nActTabStop[7] = {0,0,0,0,0,0,0};
	INT	nLogTabStop[2] = {0,0};   
	CHAR ini_file[256] = {0};
	CHAR server_selection_flag[20] = {0};
	int rc = 0;
	int server_selection_flag_int = 0;
	CHAR messageBoxTime[10] = "";
	
	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes
	
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	
	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

	    GetPrivateProfileString(
        "MONITOR_TIMEOUT_PROMPT", 	               // points to section name 
        "PROMPT_ALIVE_TIMER",	       // points to key name 
        "3",	                   // points to default string 
        messageBoxTime,                   // points to destination buffer 
        sizeof(messageBoxTime) - 1,       // size of destination buffer 
        ini_file ); 

	promptAlive = atol(messageBoxTime)*1000;

	memset(&messageBoxTime, 0, sizeof(messageBoxTime));
	GetPrivateProfileString(
        "MONITOR_TIMEOUT_PROMPT", 	               // points to section name 
        "TIMEOUT_FILE_FLAG",	       // points to key name 
        "0",	                   // points to default string 
        messageBoxTime,                   // points to destination buffer 
        sizeof(messageBoxTime) - 1,       // size of destination buffer 
        ini_file ); 

	timeoutFileFlag = atoi(messageBoxTime);

      rc = GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "1",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        ini_file ); 

	 server_selection_flag_int = atoi(server_selection_flag); 

	// get the primary machine id from registry, login into primary 
	//machine instance

		
	 if(server_selection_flag_int == 1)
	 {
		sprintf(monitor1handle, "%p", this->m_hWnd);
		if( !GetThisMachineId( hostname ) )
		{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
			be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname, "localhost" );
		}
	 }

	 else if(server_selection_flag_int == 2)
	 {
		sprintf(monitor2handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_2( hostname ) )
		{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
			be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname, "localhost" );
		}
	 }

	 else if(server_selection_flag_int == 3)
	 {
		sprintf(monitor3handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_3( hostname ) )
		{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
			be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname, "localhost" );
		}
	 }

	 else if(server_selection_flag_int == 4)
	 {
		 sprintf(monitor4handle, "%p", this->m_hWnd);
		if( !GetThisMachineId_4( hostname ) )
		{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
			be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname, "localhost" );
		}
	 }

	if( !ptetcp_init( error_msg ) )
	{
		sprintf( buffer, "%s.", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		return;
	}
	m_bMonitorUp = false;
	if( !ptetcp_connect( hostname, error_msg ) )
	{
		//Initial connection failed!
		sprintf( buffer, "Unable to connect to Applink Server: %s and port = %s", error_msg,str_port );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		// JMG added on 2/7/02 to save host name for later reconnection
		// when the initial connection fails
		strcpy( hostname_saved, hostname);
		//We now fall thru to allow the windows to set up, even though we are not connected.
	}
	else
	{
		m_bMonitorUp = true;
		// Initial connection established!
		// Clear the saved host name buffer to indicate
		// a valid initial connection was established and then initialize
		// the applnk connection data.
		memset( hostname_saved, 0, sizeof( hostname_saved ) );
		InitConnectionData();
		
		//Krishna@tf, 19.5.11 : Validate the Commandline params & continue the App
		BYTE rcode = 0;
		UCF01 ucf01;
		char error_msg[256];
		
		memset( &ucf01, 0, sizeof( UCF01 ) );
		m_LogMessages.SetHorizontalExtent(4096);//praneeth
		// next authenticate the user
		strcpy( (char *)ucf01.primary_key.name, u_name);
		
		// Cannot use txdsapi_get_record() here, that internally calls ptetcp_send_receive() as
		// LogMessageHandler() thread is already started and reading the socket. You cannot
		// share a socket for reading from two threads!!
		// rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
		
		rcode = monitor_fetch_ucf01((pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg);
		
		if( rcode == PTEMSG_NOT_FOUND || rcode != PTEMSG_OK )
		{   
			::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
			return;
		}
		
		// check password	
		if( strcmp( (char *)ucf01.password, u_pwd ) != 0 )
		{	
			::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
			return;
		}
		//End, Krishna changes
		
	}

	
	//set the tab stops for a list box
	CRect rectList;
	
	m_NetworkStatus.GetWindowRect(&rectList);
	
	CDC* dc = GetDC();
	CRect size = rectList;
	CFont* pOldFont;
	
	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());
	
	// accurately measure the average width of a character
	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);
	
	//  find the tab stop, being careful with rounding
	INT ListBoxWidth = ((rectList.Width()*4) / ((25+size.Width())/52));
	
	nTabStop[0] = ( ListBoxWidth * 16 ) / 100;
	nTabStop[1] = ( ListBoxWidth * 47 ) / 100;
	nTabStop[2] = ( ListBoxWidth * 69 ) / 100;
	
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);
	
	m_NetworkStatus.SetTabStops(2, nTabStop);
	
	/*set tab stops for dialog connect list box*/
	CRect rectDialogList;
	
	m_DialogConnectStatus.GetWindowRect(&rectDialogList);
	
	CRect dialog_size = rectDialogList;
	
	//  find the tab stop, being careful with rounding
	INT DialogListBoxWidth = ((rectDialogList.Width()*4) / ((25+dialog_size.Width())/52));
	
	nDialogTabStop[0] = ( DialogListBoxWidth * 47) / 100;
	
	m_DialogConnectStatus.SetTabStops(1, nDialogTabStop);
	
	/*set tab stops for tran activity list box */
	CRect rectActList;
	
	m_NetworkStatus.GetWindowRect(&rectActList);
	
	CRect act_size = rectActList;
	
	//  find the tab stop, being careful with rounding
	INT ActListBoxWidth = ((rectActList.Width()*4) / ((25+act_size.Width())/52));
	
	//  nActTabStop[0] = ( ListBoxWidth * 55) / 100;
	//	nActTabStop[1] = ( ListBoxWidth * 95) / 100;
	//	nActTabStop[2] = ( ListBoxWidth * 140) / 100;
	
	//	m_TranActivityList.SetTabStops(3, nActTabStop);
	//	m_TranActivityList.SetTabStops();
	
	//	m_LogMessages.SetTabStops (2);
	m_LogMessages.SetHorizontalExtent(4096);
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
		AfxMessageBox( "Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		return;
	}
	
	// Get Initial network status from dataserver
	m_DataServerReply = CreateEvent(NULL,FALSE,FALSE,NULL);
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
		AfxMessageBox( "Insufficient memory to build monitor up message" );
		return;
	}
	
	// Note that we cannot call PtetcpSend here to avoid infinite error loops.
	if( !ptetcp_send( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Monitor Up message: %s", error_msg );
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
	m_hTestOff = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTOFF));
	m_hTesting = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_TESTING));
	
	m_hNCGreen	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_GREEN));
	m_hNCRed		= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_RED));
	m_hNCYellow	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_NC_YELLOW));
	
	m_TestResult.SetBitmap( m_hTestOff );
	m_CntlNCStatus.SetBitmap( m_hNCYellow );
	
	m_CntlMuteBeep.SetCheck(_MuteOn);
	
	// Start a timer for Timer_Interval milliseconds to get periodic status updates
	m_RefreshTimer = SetTimer( Timer, Timer_Interval, NULL );
	if( m_RefreshTimer == 0 )
		AfxMessageBox( "Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
	_NCStatusTimer = SetTimer( Timer+1, _NCStatus_Beep_Interval, NULL );
	if( _NCStatusTimer == 0 )
		AfxMessageBox( "Unable to start Network Status Timer", MB_ICONSTOP | MB_OK );
	
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
			sprintf( buffer, "Unable to connect to Applink Server: %s.", error_msg );
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
			MessageBox( error_msg ); // Display library error message
			return 1; // Bypass any local error messages, because reconnection OK.
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			MessageBox( error_msg ); // Display library error message
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
			sprintf( buffer, "Unable to connect to Applink Server: %s.", error_msg );
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
			MessageBox( error_msg ); // Display library error message
		}
		else if( NULL != strstr( error_msg, "lost") )
		{
			MessageBox( error_msg ); // Display library error message
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
	INT ret = AfxMessageBox( "Are you sure to clear the logs?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	
	//    m_LogMessages.DeleteAllItems();
	m_LogMessages.ResetContent();
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
	// TODO: Add your control notification handler code here

	
	CHAR  buffer[256] ={0};
	CHAR error_msg[100] = {0};
	CHAR handle[256] = {0};
	pPTE_MSG    p_msg_out;
	CHAR displayhostname[50] ={0};
	m_bMonitorUp = false;
	CString szAutoSaveFile;

	sprintf(handle, "%p", this->m_hWnd);

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
	else if(strcmp(handle,monitor4handle) == 0)
	{
		strcpy(displayhostname,HostName4);
	}

	CTime time = CTime::GetCurrentTime();
	szAutoSaveFile.Format("monitor_auto_save_%s_%s.log", displayhostname,time.Format("%m%d%Y%H%M%S"));
	SaveLogIntoFile(szAutoSaveFile);
	// send monitor down message to dataserver
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
		AfxMessageBox( "Insufficient memory to build monitor down message" );
		ptetcp_disconnect();
		Sleep( 100 );
		GetParentFrame()->DestroyWindow() ;
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Monitor Up message: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	
	free( p_msg_out );
	
	/*kill timer*/
	/*  bool m_KillTimer = 0;
	if ( KillTimer(Timer))
	AfxMessageBox("Timer Killed", MB_ICONSTOP | MB_OK);
	else
	AfxMessageBox("Timer Kill Failed", MB_ICONSTOP | MB_OK);*/
	
	
	ptetcp_disconnect();
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
		AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
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
		AfxMessageBox( "Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending EchoTest Request: %s", error_msg );
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
		AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(st1 == ST1_ACQUIRER_OFF)
	{
		str.Format("Are you sure to Acquirer off for %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else  if(st1 == ST1_ACQUIRER_ON)
	{
		str.Format("Are you sure to Acquirer on %s?", str_network_id);
		INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return;
	}
	else
	{
		str.Format("Are you sure to logoff %s?", str_network_id);
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
		AfxMessageBox( "Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Logoff Request: %s", error_msg );
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
		AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	str.Format("Are you sure to logon %s?",str_network_id);

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
		AfxMessageBox( "Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Logon Request: %s", error_msg );
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
	BeginWaitCursor();
	while( !m_retrieval_done )
	{
		if( WaitForSingleObject( m_DataServerReply, 5000 ) != WAIT_OBJECT_0 )
		{
			EndWaitCursor();
			AfxMessageBox( "Timed out getting Network Status from DataServer", MB_ICONSTOP | MB_OK  );
			return;
		}
	}
	EndWaitCursor();
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
	sprintf(handle, "%p", this->m_hWnd);

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
	}

	sprintf(filenamewithhostname,"monitor_%s.log",displayhostname);

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
			sprintf( temp_str, "Unable to open file %s", log_filename ); 
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
	
	ResetEvent( m_DataServerReply );
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
		MessageBox( "Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Request to dataserver: %s", error_msg );
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
	
	m_CntlNetTranList.SetHeadings("Service Name, 90; Status, 90; Network Name, 90; Received, 70; Sent, 50; Rate, 50; Approved, 70; Declined, 60; TimedOut, 70; Reversed, 80; Mgt Counts, 80; STIP Sent, 80; STIP Recv, 80;");
	
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
	CHAR        log_msg[1024] = {0};
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
			sprintf(log_msg, "DATE: %.10s TIME: %.8s\n%s", time_str, time_str+12, (pCHAR)opr_message->message);
			//MessageBox(log_msg, "Ascendent System Monitor - Time Out Alert", MB_ICONWARNING);

			//you must load user32.dll before calling the function
			HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
    
			if (hUser32)
			{
				UINT uiFlags = MB_OK|MB_SETFOREGROUND|MB_ICONWARNING;
				MessageBoxTimeout(this->m_hWnd, log_msg, _T("Ascendent System Monitor - Time Out Alert"), uiFlags, 0, promptAlive);
				FreeLibrary(hUser32);
			}

			if(timeoutFileFlag == 1)
				write_timeout_data_file(log_msg);

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


void CMonitorView::write_timeout_data_file(char *log_data)
{
	FILE *tp;
	CString Buffer;
	CHAR handle[50] = "", displayhostname[50] = "";

	sprintf(handle, "%p", this->m_hWnd);
	if(strcmp(handle,monitor1handle) == 0)
		strcpy(displayhostname, HostName1);
	else if(strcmp(handle,monitor2handle) == 0)
		strcpy(displayhostname, HostName2);
	else if(strcmp(handle,monitor3handle) == 0)
		strcpy(displayhostname, HostName3);
	else if(strcmp(handle,monitor4handle) == 0)
		strcpy(displayhostname, HostName4);

	CTime time = CTime::GetCurrentTime();
	Buffer.Format("..\\log\\timeout_details_%s_%s.log", displayhostname, time.Format("%d%m%Y"));

	tp = fopen(Buffer, "a+");
	if(tp == NULL)
	{
		CreateDirectory("..\\log\\", NULL);
		tp = fopen(Buffer, "a+");
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
			sprintf( buffer, "Insufficient memory to build query for %s",
				m_ncf01_local_table[temp].primary_key.network_id );
			MessageBox( buffer );
			return;
		}
		
		if( !PtetcpSend( p_msg_out, error_msg ) )
		{
			sprintf( buffer, "Communication error sending query to %s: %s",
				m_ncf01_local_table[temp].primary_key.network_id, error_msg );
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
				
				AfxMessageBox("Server closed the connection. Exiting..",MB_ICONSTOP |MB_ICONHAND | MB_OK );
				EnableDialogButtons(FALSE, FALSE);
				
				// Exit this log thread as well as the main application here.
				
				ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
				AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);  // below 2 line replaced for above 2 lines fix the monitor issue.
				
				return;	// Exit thread. Close Application.
				break;
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
							AfxMessageBox( "ERROR: Two Monitors are already connected from other Systems.", MB_SYSTEMMODAL | MB_OK );
							EnableDialogButtons(FALSE, FALSE);
							
							ASSERT( AfxGetApp ()->GetMainWnd ()!=NULL);
							AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE); 
							
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
							msg.Format("Primary Monitor is already connected. This will be a read-only session.\n %s user has already connected", tmpUserInfo.user_name);
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
							SetEvent( m_DataServerReply );
							p_data = p_data + sizeof(AUTH_TX);
							if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
							{
								strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
								sprintf( error_msg, "Database Error: %s", temp_str );
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
								MessageBox( "NCF01 table contents exceeds local capacity. Truncating Data" );
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
									AfxMessageBox( "Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
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
									SetEvent( m_DataServerReply );
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
										AfxMessageBox( "Unable to create a Get Network Status thread", MB_OK | MB_ICONSTOP );
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
		AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);
	
		str.Format("Are you sure to Get Advises for %s?", str_network_id);
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
			AfxMessageBox( "Insufficient memory to build request message" );
		}
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				sprintf( buffer,
					"Communication error sending Logon Request: %s",
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
	
	if(nIDEvent == Timer)
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
	if(nIDEvent == (Timer+1))
	{
		if(nc_status == RED && !m_CntlMuteBeep.GetCheck())
		{
			MessageBeep(MB_ICONERROR);
		}
	}
	
	
	CFormView::OnTimer(nIDEvent);
}

void CMonitorView::OnResetCountsButton() 
{
		INT ret = AfxMessageBox( "Are you sure to Reset All the counts?", MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;
	/*Set activity counts to zero*/
	//InitializeActivityStatsList();
	if(m_CntlNetTranList.Lock() == 0)
	{
		AfxMessageBox("List is being updated. Please try again.");
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
			sprintf( tmpstring, "Unable to open ini file: %s", filename );
		
		else if ( num_of_ids == -2 )
			sprintf( tmpstring, "Cannot find section %s in ini file: %s",
			sectionname, filename );
		
		else if ( num_of_ids == -3 )
			sprintf( tmpstring, 
			"Size of list is too small. File: %s, section: %s",
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
		strcpy(err_msg,"Insufficient Memory to build PTE msg for status request");
		ret_code = false;
	}
	else
	{
		if( !PtetcpSend( p_msg, error_msg ) )
		{
			strcpy( err_msg, "Failed to transmit status request to queue: " );
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

		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_RECEIVED, Controller_Stats[i].received_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_SENT,  Controller_Stats[i].sent_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_RATE,  Controller_Stats[i].rate);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_APPROVED,  Controller_Stats[i].approved_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_DECLINED,  Controller_Stats[i].declined_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_TIMEDOUT,  Controller_Stats[i].timeout_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_REVERSED,  Controller_Stats[i].reversed_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_MGT_COUNTS,  Controller_Stats[i].mgt_request_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_STIP_SENT,  Controller_Stats[i].stip_sent_count);
		m_CntlNetTranList.UpdateItemText( Controller_Stats[i].service_id, LS_STIP_RECV,  Controller_Stats[i].stip_recv_count);
		
	}
	m_CntlNetTranList.Sort();
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
			strcpy(error_msg,"Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "Failed to transmit status request to queue: " );
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
				strcpy(error_msg,"Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "Failed to transmit status request to queue: " );
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
			strcpy(error_msg,"Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "Failed to transmit  Dialog status request to queue: " );
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
				strcpy(error_msg,"Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "Failed to transmit  Dialog status request to queue: " );
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

void CMonitorView::OnCheck1() 
{
	//Mute
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
		CString str;
		AfxGetApp()->m_pMainWnd->GetWindowText(str);
		str += " - READ ONLY";
		AfxGetApp()->m_pMainWnd->SetWindowText(str);
		m_bReadOnly = true;
	}
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
			AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
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
			strcpy(error_msg,"Insufficient memory to build request message");
			return;
		}
		
		else
		{
			if( !PtetcpSend( p_msg_out, error_msg ) )
			{
				strcpy( error_msg, "Failed to transmit status request to queue: " );
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
				strcpy(error_msg,"Insufficient memory to build request message");
				return;
			}
			
			else
			{
				if( !PtetcpSend( p_msg_out, error_msg ) )
				{
					strcpy( error_msg, "Failed to transmit status request to queue: " );
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
	
/*	GetPinnacleLogDirectory( log_filename );
	strcat( log_filename, filename.GetBuffer( 100 ) );*/


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
			AfxMessageBox("Unable to create directory");
		}
		log_fp = fopen( log_filename, "w+" );
		if( log_fp == NULL )
		{

			sprintf( temp_str, "Unable to save the log", log_filename ); 
			AfxMessageBox( temp_str, MB_ICONSTOP | MB_OK );
			return;
		}
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
		AfxMessageBox( "Please select a network ID from list", MB_ICONSTOP | MB_OK );
		return;
	}
	strcpy(str_network_id, str);

	if(0 == strncmp(str_network_id,"nccup",5))
	{

		if(st1 == ST1_CUP_KEY_REQUEST)
		{
			str.Format("Are you sure to Request Key for %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == ST1_CUP_KEY_SEND)
		{
			str.Format("Are you sure to Send Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
		}
		else  if(st1 == STI_CUP_KEY_RELOAD)
		{
			str.Format("Are you sure to Reload Key to %s?", str_network_id);
			INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
			if( ret == IDNO )
				return;
			else
			{
				st1 = ST1_CUP_ACTIVE_ACTIVE_TRIGGER;
				str.Format("Reload Key request is sent to %s?", str_network_id);
				AfxMessageBox( str, MB_ICONSTOP | MB_OK );
			}
		}
		else
		{
			str.Format("Are you sure to logoff %s?", str_network_id);
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
				str.Format("Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
	
			else  if(st1 == STI_CUP_KEY_RELOAD)
			{
				// Key RELOAD for DCI
				st1 = ST1_DINERS_KEY_RELOD_REQUEST;
				str.Format("Are you sure to Reload Key to %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
				else
				{
					str.Format("Reload Key request is sent to %s?", str_network_id);
					AfxMessageBox( str, MB_ICONSTOP | MB_OK );
				}
			}
		else
		{
			str.Format("invalid option %s?", str_network_id);
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
				str.Format("Are you sure want to do key exchange for %s?", str_network_id);
				INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
				if( ret == IDNO )
					return;
			}
			else
			{
				str.Format("invalid option %s?", str_network_id);
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
		AfxMessageBox( "Insufficient memory to build request message" );
		return;
	}
	
	if( !PtetcpSend( p_msg_out, error_msg ) )
	{
		sprintf( buffer, "Communication error sending Logoff Request: %s", error_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}
	free( p_msg_out );
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