/*******************************************************************************
*  
* MODULE:      monitorview.h
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\monitorView.h  $
   
      Rev 1.10   Jun 02 2006 16:43:44   dirby
   Added a TEST Button to send a message to txcntl and get a response.
   If a response is received, a message is displayed to indicate
   Monitor is working.  Button stays RED until a response is received.
   SCR 19357
   
      Rev 1.9   Jun 09 2004 15:24:02   lmitchel
   SCR920 - Modification to set network online status to red if offline or down status.  Changed network status resource type from CListBox to CColorListBox
   
      Rev 1.8   Apr 09 2003 11:44:20   svelaga
   Multiple monitor startup prevention
   
      Rev 1.7   Apr 08 2003 09:51:50   lmitchel
   Initial check-in with  changes for SCR920 to display network status OFFLINE in red, and changes to not display primary account screen on logon if there are no logon bins defined.
   
      Rev 1.6   Jul 30 2002 09:57:10   lmitchel
   SCR812: Corrected setting of red text when dialog status = Disconnected.  Changed Log messages control from CComboBox to CColorListBox to implement text color assignment to log records.  Log messages for reversals and timeouts will be displayed in red.
   
      Rev 1.5   Feb 07 2002 14:10:22   jgrguric
   Added support for auto reconnection logic now buily in to
    ptetcp.lib library. This application will now survive loss of
    applnk connection and reconnect automatically on next
    operation, when applnk is available again. Also, has extra
    logic to allow initial connection to fail ant then have the GUI
    connect at a later time when applnk comes up.
   
      Rev 1.4   Apr 03 2001 12:46:46   SYARLAGA
   Modified  m_DialogConnectStatus and m_TranActivityList
   
      Rev 1.4   Apr 03 2001 12:38:32   SYARLAGA
   changed the m_TranActivityList and m_DialogConnectStatus from CListBox to CColorListBox
   
      Rev 1.3   Aug 29 2000 15:24:38   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.2   Nov 24 1999 14:32:04   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.1   May 27 1999 11:55:56   npogosya
   Added SAF(save and forward) button
   
      Rev 1.0   24 Feb 1999 15:59:20   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:38   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:14   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:18   jhapitas
   Initial checkin
*
*******************************************************************************/
// monitorView.h : interface of the CMonitorView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef MONITORVIEW
#define MONITORVIEW

#include "ColorListBox.h"
#include "ListExCtrl.h"
extern "C"
{
#include "app_info.h"
#include "nc_dbstruct.h"
#include "equitdb.h"
}

#define NCF01_LOCAL_TABLESIZE    500

#define MONITOR1	1
#define MONITOR2	2
#define MONITOR3	3
#define MONITOR4	4
#define AUTH_FAILURE	2

class CMonitorView : public CFormView
{
protected: // create from serialization only
	CMonitorView();
	DECLARE_DYNCREATE(CMonitorView)
CFont m_font;
public:
	//{{AFX_DATA(CMonitorView)
	//MONITOR 1
	enum { IDD = IDD_MONITORDCO_FORM };
	CButton	m_CntlCupKeySend;
	CButton m_CntlCupKeyReload;
	CButton	m_CntlCupKeyRequest;
	CListExCtrl	m_CntlNetTranList;
	CButton	m_CntlAcqOff;
	CButton	m_CntlGetAdvisesButton;
	CButton	m_CntlRefreshButton;
	CButton	m_CntlPingDialogButton;
	CButton	m_CntlEchoTestButton;
	CButton	m_CntlLogOff;
	CButton	m_CntlLogOn;
	CButton	m_CntlClearLogButton;
	CButton	m_CntlSaveLogButton;
	CButton	m_CntlMuteBeep;
	CStatic	m_CntlNCStatus;
	CStatic	m_TestResult;
	CButton	m_TEST_BUTTON;
	CButton	m_PingButton;
	CColorListBox  m_TranActivityList;
	CButton	m_CntlResetCountsButton;
	CColorListBox	m_DialogConnectStatus;
	CColorListBox	m_LogMessages;
	CColorListBox	m_NetworkStatus;
	CStatic	m_Monitor1Name;
	CButton	m_Ping;

	CTabCtrl m_MainTab1;
	CStatic	m_DialogIdLabel;
	CStatic	m_LogStatusLabel;
	CStatic	m_NameLabel;
	CStatic	m_LogDateLabel;
	CStatic	m_LogTimeLabel;
	CStatic m_LogTypeLabel;
	CStatic	m_LogServiceLabel;
	CStatic	m_LogMessageLabel;

	//MONITOR 2
	CButton	m_CntlCupKeySend2;
	CButton m_CntlCupKeyReload2;
	CButton	m_CntlCupKeyRequest2;
	CListExCtrl	m_CntlNetTranList2;
	CButton	m_CntlAcqOff2;
	CButton	m_CntlGetAdvisesButton2;
	CButton	m_CntlRefreshButton2;
	CButton	m_CntlPingDialogButton2;
	CButton	m_CntlEchoTestButton2;
	CButton	m_CntlLogOff2;
	CButton	m_CntlLogOn2;
	CButton	m_CntlClearLogButton2;
	CButton	m_CntlSaveLogButton2;
	CButton	m_CntlMuteBeep2;
	CStatic	m_CntlNCStatus2;
	CStatic	m_TestResult2;
	CButton	m_TEST_BUTTON2;
	CButton	m_PingButton2;
	CColorListBox  m_TranActivityList2;
	CButton	m_CntlResetCountsButton2;
	CColorListBox	m_DialogConnectStatus2;
	CColorListBox	m_LogMessages2;
	CColorListBox	m_NetworkStatus2;
	CStatic	m_Monitor2Name;
	CButton	m_Ping2;

	CTabCtrl m_MainTab2;
	CStatic	m_DialogIdLabel2;
	CStatic	m_LogStatusLabel2;
	CStatic	m_NameLabel2;
	CStatic	m_LogDateLabel2;
	CStatic	m_LogTimeLabel2;
	CStatic m_LogTypeLabel2;
	CStatic	m_LogServiceLabel2;
	CStatic	m_LogMessageLabel2;

	//MONITOR3
	CButton	m_CntlCupKeySend3;
	CButton m_CntlCupKeyReload3;
	CButton	m_CntlCupKeyRequest3;
	CListExCtrl	m_CntlNetTranList3;
	CButton	m_CntlAcqOff3;
	CButton	m_CntlGetAdvisesButton3;
	CButton	m_CntlRefreshButton3;
	CButton	m_CntlPingDialogButton3;
	CButton	m_CntlEchoTestButton3;
	CButton	m_CntlLogOff3;
	CButton	m_CntlLogOn3;
	CButton	m_CntlClearLogButton3;
	CButton	m_CntlSaveLogButton3;
	CButton	m_CntlMuteBeep3;
	CStatic	m_CntlNCStatus3;
	CStatic	m_TestResult3;
	CButton	m_TEST_BUTTON3;
	CButton	m_PingButton3;
	CColorListBox  m_TranActivityList3;
	CButton	m_CntlResetCountsButton3;
	CColorListBox	m_DialogConnectStatus3;
	CColorListBox	m_LogMessages3;
	CColorListBox	m_NetworkStatus3;
	CStatic	m_Monitor3Name;
	CButton	m_Ping3;

	CTabCtrl m_MainTab3;
	CStatic	m_DialogIdLabel3;
	CStatic	m_LogStatusLabel3;
	CStatic	m_NameLabel3;
	CStatic	m_LogDateLabel3;
	CStatic	m_LogTimeLabel3;
	CStatic m_LogTypeLabel3;
	CStatic	m_LogServiceLabel3;
	CStatic	m_LogMessageLabel3;

	//MONITOR4
	CButton	m_CntlCupKeySend4;
	CButton m_CntlCupKeyReload4;
	CButton	m_CntlCupKeyRequest4;
	CListExCtrl	m_CntlNetTranList4;
	CButton	m_CntlAcqOff4;
	CButton	m_CntlGetAdvisesButton4;
	CButton	m_CntlRefreshButton4;
	CButton	m_CntlPingDialogButton4;
	CButton	m_CntlEchoTestButton4;
	CButton	m_CntlLogOff4;
	CButton	m_CntlLogOn4;
	CButton	m_CntlClearLogButton4;
	CButton	m_CntlSaveLogButton4;
	CButton	m_CntlMuteBeep4;
	CStatic	m_CntlNCStatus4;
	CStatic	m_TestResult4;
	CButton	m_TEST_BUTTON4;
	CButton	m_PingButton4;
	CColorListBox  m_TranActivityList4;
	CButton	m_CntlResetCountsButton4;
	CColorListBox	m_DialogConnectStatus4;
	CColorListBox	m_LogMessages4;
	CColorListBox	m_NetworkStatus4;
	CStatic	m_Monitor4Name;
	CButton	m_Ping4;

	CTabCtrl m_MainTab4;
	CStatic	m_DialogIdLabel4;
	CStatic	m_LogStatusLabel4;
	CStatic	m_NameLabel4;
	CStatic	m_LogDateLabel4;
	CStatic	m_LogTimeLabel4;
	CStatic m_LogTypeLabel4;
	CStatic	m_LogServiceLabel4;
	CStatic	m_LogMessageLabel4;

	//Mute all button
	CButton	m_CntlMuteAll;
	CButton	m_CloseAll;

	//}}AFX_DATA

// Attributes
public:
	//CMonitorDoc* GetDocument();
	//MONITOR 1
	bool m_bReadOnly;
	CHAR nc_status;
	HANDLE	m_DataServerReply;
	NCF01    m_ncf01_local_table[NCF01_LOCAL_TABLESIZE];
	INT      m_curr_index;
	NCF01    m_ncf01;
	INT      m_retrieval_done;
	char     m_msg_data[ISO_MSG_LENGTH + sizeof(AUTH_TX)];
	AUTH_TX  m_auth_tx;
	volatile char	 ucf01SendRecvBuffer[sizeof(AUTH_TX) + 4000];
	volatile int		 responceUCF01FetchNotReceived;
	
	//MONITOR 2
	bool m_bReadOnly2;
	CHAR nc_status2;
	HANDLE	m_DataServerReply2;
	NCF01    m_ncf01_local_table2[NCF01_LOCAL_TABLESIZE];
	INT      m_curr_index2;
	NCF01    m_ncf012;
	INT      m_retrieval_done2;
	char     m_msg_data2[ISO_MSG_LENGTH + sizeof(AUTH_TX)];
	AUTH_TX  m_auth_tx2;
	volatile int		 responceUCF01FetchNotReceived2;

	//MONITOR 3
	bool m_bReadOnly3;
	CHAR nc_status3;
	HANDLE	m_DataServerReply3;
	NCF01    m_ncf01_local_table3[NCF01_LOCAL_TABLESIZE];
	INT      m_curr_index3;
	NCF01    m_ncf013;
	INT      m_retrieval_done3;
	char     m_msg_data3[ISO_MSG_LENGTH + sizeof(AUTH_TX)];
	AUTH_TX  m_auth_tx3;
	volatile int		 responceUCF01FetchNotReceived3;

	//MONITOR 4
	bool m_bReadOnly4;
	CHAR nc_status4;
	HANDLE	m_DataServerReply4;
	NCF01    m_ncf01_local_table4[NCF01_LOCAL_TABLESIZE];
	INT      m_curr_index4;
	NCF01    m_ncf014;
	INT      m_retrieval_done4;
	char     m_msg_data4[ISO_MSG_LENGTH + sizeof(AUTH_TX)];
	AUTH_TX  m_auth_tx4;
	volatile int		 responceUCF01FetchNotReceived4;

// Operations
public:
	//MONITOR 1
	static BOOL CALLBACK ItemdataProc(DWORD dwData, LPARAM lParam);	//For list box
	static void LogThreadStarter( CMonitorView * _this )
	{
		_this->LogMessageHandler();
	}
	static void GetNetworkStatusThread( CMonitorView * _this )
	{
		_this->GetCurrentStatus();
	}

	//MONITOR 2
	static void LogThreadStarter2( CMonitorView * _this )
	{
		_this->LogMessageHandler2();
	}
	static void GetNetworkStatusThread2( CMonitorView * _this )
	{
		_this->GetCurrentStatus2();
	}
	
	//MONITOR 3
	static void LogThreadStarter3( CMonitorView * _this )
	{
		_this->LogMessageHandler3();
	}
	static void GetNetworkStatusThread3( CMonitorView * _this )
	{
		_this->GetCurrentStatus3();
	}

	//MONITOR 4
	static void LogThreadStarter4( CMonitorView * _this )
	{
		_this->LogMessageHandler4();
	}
	static void GetNetworkStatusThread4( CMonitorView * _this )
	{
		_this->GetCurrentStatus4();
	}

   char* GetHostname(int);
   BYTE HostConnect(char *, int);
   void InitializeMonitorControls(int);
   void InitTabControls();
   void toggle_read_only_title(int);

   void get_timeout_ini_data();
   void write_timeout_data_file(int, char *);

   //MONITOR 1
   void DisableAllButtons(int);
   void EnableDialogButtons(int nStatus, int changeToReadOnlyTitleBar);   
   void LogMessageHandler();  
   void GetNCF01List();    
   void PopulateNetworkStatusList();  
   INT  FindNCF01Record( pCHAR network_id, pCHAR network_type );   
   void InsertLogMessage( OPR_MESSAGE *opr_message );   
   void GetCurrentStatus();   
   void InitializeActivityStatsList();
   void InitializeDialogStatusList();   
   void SendResetControllerStats();   
   void UpdateControllerStatsList(); 
   void UpdateDialogStatusList();
   
   //MONITOR 2
   void EnableDialogButtons2(int nStatus, int changeToReadOnlyTitleBar);
   void LogMessageHandler2();
   void GetNCF01List2(); 
   void PopulateNetworkStatusList2();
   INT  FindNCF01Record2( pCHAR network_id, pCHAR network_type );
   void InsertLogMessage2( OPR_MESSAGE *opr_message );
   void GetCurrentStatus2();
   void InitializeActivityStatsList2();
   void InitializeDialogStatusList2();
   void SendResetControllerStats2();
   void UpdateControllerStatsList2();
   void UpdateDialogStatusList2();

   //MONITOR 3
   void EnableDialogButtons3(int nStatus, int changeToReadOnlyTitleBar);
   void LogMessageHandler3();
   void GetNCF01List3(); 
   void PopulateNetworkStatusList3();
   INT  FindNCF01Record3( pCHAR network_id, pCHAR network_type );
   void InsertLogMessage3( OPR_MESSAGE *opr_message );
   void GetCurrentStatus3();
   void InitializeActivityStatsList3();
   void InitializeDialogStatusList3();
   void SendResetControllerStats3();
   void UpdateControllerStatsList3();
   void UpdateDialogStatusList3();

   //MONITOR 4
   void EnableDialogButtons4(int nStatus, int changeToReadOnlyTitleBar);
   void LogMessageHandler4();
   void GetNCF01List4(); 
   void PopulateNetworkStatusList4();
   INT  FindNCF01Record4( pCHAR network_id, pCHAR network_type );
   void InsertLogMessage4( OPR_MESSAGE *opr_message );
   void GetCurrentStatus4();
   void InitializeActivityStatsList4();
   void InitializeDialogStatusList4();
   void SendResetControllerStats4();
   void UpdateControllerStatsList4();
   void UpdateDialogStatusList4();

   // Added by JMG on 2/6/02 to support auto reconnect to applnk 
   // void InitConnectionData(INT stage); /* stage : 0 - means inital connect   1 - means re connection */
   //MONITOR 1
   void InitConnectionData();    
   INT PtetcpSend( pPTE_MSG p_msg_out, pCHAR error_msg );   
   pPTE_MSG PtetcpReceive( LONG time_out, pCHAR error_msg ); 
   INT send_pte_msg( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg );   
   void  send_for_stats();   
   void  process_stats( pPTE_MSG, BYTE msgtype );
   void  process_ping(pPTE_MSG,BYTE msgtype);
   BYTE monitor_fetch_ucf01( pBYTE psStructurePtr, int nStructureSize, 
					 BYTE  app_data_type,
					 pCHAR error_msg );

   //new changes - amith
   BYTE UserAuthentication();
   BYTE monitor_fetch_ucf01_new( pBYTE psStructurePtr, int nStructureSize, 
					 BYTE  app_data_type,
					 pCHAR error_msg );
	

   //MONITOR 2
   void InitConnectionData2(); 
   INT PtetcpSend2( pPTE_MSG p_msg_out, pCHAR error_msg );
   pPTE_MSG PtetcpReceive2( LONG time_out, pCHAR error_msg );
   INT send_pte_msg2( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg );
   void  send_for_stats2();
   void  process_stats2( pPTE_MSG, BYTE msgtype );
   void  process_ping2(pPTE_MSG,BYTE msgtype);

   //MONITOR 3
   void InitConnectionData3(); 
   INT PtetcpSend3( pPTE_MSG p_msg_out, pCHAR error_msg );
   pPTE_MSG PtetcpReceive3( LONG time_out, pCHAR error_msg );
   INT send_pte_msg3( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg );
   void  send_for_stats3();
   void  process_stats3( pPTE_MSG, BYTE msgtype );
   void  process_ping3(pPTE_MSG,BYTE msgtype);

   //MONITOR 4
   void InitConnectionData4(); 
   INT PtetcpSend4( pPTE_MSG p_msg_out, pCHAR error_msg );
   pPTE_MSG PtetcpReceive4( LONG time_out, pCHAR error_msg );
   INT send_pte_msg4( BYTE msgtype, BYTE subtype1, pCHAR queue, pCHAR err_msg );
   void  send_for_stats4();
   void  process_stats4( pPTE_MSG, BYTE msgtype );
   void  process_ping4(pPTE_MSG,BYTE msgtype);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	//MONITOR 1
	void SaveLogIntoFile(CString filename);
	bool m_bAcqOn;	
	void ProcessOFFMessage(int st1);	
	void ProcessCUPMessage(int st1);
	BOOL m_bMonitorUp;
	
	//MONITOR 2
	bool m_bAcqOn2;
	void ProcessOFFMessage2(int st1);
	void ProcessCUPMessage2(int st1);
	BOOL m_bMonitorUp2;

	//MONITOR 3
	bool m_bAcqOn3;
	void ProcessOFFMessage3(int st1);
	void ProcessCUPMessage3(int st1);
	BOOL m_bMonitorUp3;

	//MONITOR 4
	bool m_bAcqOn4;
	void ProcessOFFMessage4(int st1);
	void ProcessCUPMessage4(int st1);
	BOOL m_bMonitorUp4;

	virtual ~CMonitorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//MONITOR 1
	HBITMAP	m_hTestOff, m_hTesting, m_hMonitor1;
	HBITMAP	m_hNCGreen, m_hNCRed, m_hNCYellow;

	//{{AFX_MSG(CMonitorView)
	afx_msg void OnClearlog();
	afx_msg void OnCloseBtn();
	afx_msg void OnEchotest();
	afx_msg void OnLogoff();
	afx_msg void OnLogon();
	afx_msg void OnRefresh();
	afx_msg void OnSavelog();
	afx_msg void OnSaf();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnResetCountsButton();
	afx_msg void OnPingButton();
	afx_msg void OnPingDialogButton();
	afx_msg void OnTestButton();
	afx_msg void OnCheck1();
	afx_msg void OnClose();
	afx_msg void OnAcquirerOff();
	afx_msg void OnSelchangeNetworkStatus();
	afx_msg void OnPing();
	afx_msg void OnItemchangedNetTranList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCupKeyRequestButton();
	afx_msg void OnCupKeySendButton();
	afx_msg void OnCupKeyReloadButton();
	afx_msg void OnSelchangeMonitorTab1(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//public:

	//MONITOR 2
	HBITMAP	m_hTestOff2, m_hTesting2, m_hMonitor2;
	HBITMAP	m_hNCGreen2, m_hNCRed2, m_hNCYellow2;

	afx_msg void OnClearlog2();
	afx_msg void OnEchotest2();
	afx_msg void OnLogoff2();
	afx_msg void OnLogon2();
	afx_msg void OnRefresh2();
	afx_msg void OnSavelog2();
	afx_msg void OnSaf2();
	afx_msg void OnResetCountsButton2();
	afx_msg void OnPingButton2();
	afx_msg void OnPingDialogButton2();
	afx_msg void OnTestButton2();
	afx_msg void OnCheck2();
	afx_msg void OnAcquirerOff2();
	afx_msg void OnPing2();
	afx_msg void OnItemchangedNetTranList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCupKeyRequestButton2();
	afx_msg void OnCupKeySendButton2();
	afx_msg void OnCupKeyReloadButton2();
	afx_msg void OnSelchangeMonitorTab2(NMHDR *pNMHDR, LRESULT *pResult);

	//MONITOR 3
	HBITMAP	m_hTestOff3, m_hTesting3, m_hMonitor3;
	HBITMAP	m_hNCGreen3, m_hNCRed3, m_hNCYellow3;

	afx_msg void OnClearlog3();
	afx_msg void OnEchotest3();
	afx_msg void OnLogoff3();
	afx_msg void OnLogon3();
	afx_msg void OnRefresh3();
	afx_msg void OnSavelog3();
	afx_msg void OnSaf3();
	afx_msg void OnResetCountsButton3();
	afx_msg void OnPingButton3();
	afx_msg void OnPingDialogButton3();
	afx_msg void OnTestButton3();
	afx_msg void OnCheck3();
	afx_msg void OnAcquirerOff3();
	afx_msg void OnPing3();
	afx_msg void OnItemchangedNetTranList3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCupKeyRequestButton3();
	afx_msg void OnCupKeySendButton3();
	afx_msg void OnCupKeyReloadButton3();
	afx_msg void OnSelchangeMonitorTab3(NMHDR *pNMHDR, LRESULT *pResult);

	//MONITOR 4
	HBITMAP	m_hTestOff4, m_hTesting4, m_hMonitor4;
	HBITMAP	m_hNCGreen4, m_hNCRed4, m_hNCYellow4;

	afx_msg void OnClearlog4();
	afx_msg void OnEchotest4();
	afx_msg void OnLogoff4();
	afx_msg void OnLogon4();
	afx_msg void OnRefresh4();
	afx_msg void OnSavelog4();
	afx_msg void OnSaf4();
	afx_msg void OnResetCountsButton4();
	afx_msg void OnPingButton4();
	afx_msg void OnPingDialogButton4();
	afx_msg void OnTestButton4();
	afx_msg void OnCheck4();
	afx_msg void OnAcquirerOff4();
	afx_msg void OnPing4();
	afx_msg void OnItemchangedNetTranList4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCupKeyRequestButton4();
	afx_msg void OnCupKeySendButton4();
	afx_msg void OnCupKeyReloadButton4();
	afx_msg void OnSelchangeMonitorTab4(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMuteAll();
	HBITMAP	m_hCloseAll;
	void close_dco_monitor();
};

/*
#ifndef _DEBUG  // debug version in monitorView.cpp
inline CMonitorDoc* CMonitorView::GetDocument()
   { return (CMonitorDoc*)m_pDocument; }
#endif
*/

/////////////////////////////////////////////////////////////////////////////


#endif
