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

class CMonitorView : public CFormView
{
protected: // create from serialization only
	CMonitorView();
	DECLARE_DYNCREATE(CMonitorView)
CFont m_font;
public:
	//{{AFX_DATA(CMonitorView)
	enum { IDD = IDD_MONITOR_FORM };
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
	//}}AFX_DATA

// Attributes
public:
	//CMonitorDoc* GetDocument();
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


// Operations
public:
	static BOOL CALLBACK ItemdataProc(DWORD dwData, LPARAM lParam);	//For list box
	static void LogThreadStarter( CMonitorView * _this )
	{
		_this->LogMessageHandler();
	}
	static void GetNetworkStatusThread( CMonitorView * _this )
	{
		_this->GetCurrentStatus();
	}
	
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
   void write_timeout_data_file(char *);

   // Added by JMG on 2/6/02 to support auto reconnect to applnk 
   // void InitConnectionData(INT stage); /* stage : 0 - means inital connect   1 - means re connection */
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
	void SaveLogIntoFile(CString filename);
	bool m_bAcqOn;
	void ProcessOFFMessage(int st1);
	void ProcessCUPMessage(int st1);
	BOOL m_bMonitorUp;
	virtual ~CMonitorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	HBITMAP	m_hTestOff, m_hTesting;
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*
#ifndef _DEBUG  // debug version in monitorView.cpp
inline CMonitorDoc* CMonitorView::GetDocument()
   { return (CMonitorDoc*)m_pDocument; }
#endif
*/

/////////////////////////////////////////////////////////////////////////////


#endif
