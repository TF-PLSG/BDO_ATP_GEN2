// DialogPg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDialogPg dialog

#include "SharedMem.h"
#include "Lock.h"

#define  IDT_TIMER  WM_USER + 200
class CDialogPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogPg)

// Construction
public:
	CDialogPg();
	~CDialogPg();
	BOOL MsgToDialogMan(BYTE outmsgtype,BYTE msgsubtype);
	void SetTraceFName();
	void ExitApp();

	static void TraceThreadStarter( CDialogPg * _this )
	{
		_this->TraceHandler();
	}

	void EnableTrace(BOOL onoff);
	void TraceHandler();
	void MinMaxWin(); 

	char DM_que_name[MAX_QUE_NAME_SIZE];
	char cur_que_name[MAX_QUE_NAME_SIZE];
	BOOL stopflag;
	BOOL shownoerrors;

	HANDLE	hCONNECT_CONFIRM;
	HANDLE	hDISCONNECT_CONFIRM;
	HANDLE	hCOM_STATUS;
	HANDLE  hGET_DM_CONFIG;
	HANDLE  hTRACEON_CONFIRM;
	HANDLE	hTRACEOFF_CONFIRM;
	HANDLE	hCONNECTED_TRACE;
	HANDLE	hCONNECTED_NOTRACE;
	pPTE_MSG		pMsg_tfname;

	CBrush m_pEditBkBrush;
	int m_colorFlag;
	char ErrMsg[100];
	CString m_MesString;

	CPropertySheet *m_PSheet;
	HANDLE	hThread;
	HANDLE  hFlashColor;
	DWORD	ThreadId;
	
	int idle_time;
	char idle_time_val[7];
	int ret, status;
	bool StopTimer();
	int StartTimer(int time);
	void DisableEditControls();
	void EnableEditControls();
	short LockScreen();
	void ResetCntr();
	bool block_f;
	

// Dialog Data
	//{{AFX_DATA(CDialogPg)
	enum { IDD = IDD_DIALOG };
	CComboBox	m_Services_lst;
	CButton	m_Apply1_btn;
	CButton	m_Connect_btn;
	CStatic	m_OnOff_Indicator;
	CButton	m_Incoming_grb;
	CEdit	m_TraceFName;
	CEdit	m_OutTPDU;
	CEdit	m_OutMessage;
	CEdit	m_OutMesLen;
	CEdit	m_InTPDU;
	CEdit	m_InMessage;
	CEdit	m_InMesLen;
	CButton	m_Outgoing_grb;
	CStatic	m_TraceFName_lbl;
	CButton	m_TraceCheck;
	CStatic	m_OutTPDU_lbl;
	CStatic	m_OutMesLen_lbl;
	CStatic	m_InTPDU_lbl;
	CStatic	m_InMesLen_lbl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogPg)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);		
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HBITMAP	m_hOffLine, m_hOnLine;

	// Generated message map functions
	//{{AFX_MSG(CDialogPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTraceCheck();
	afx_msg void OnAboutButton();
	afx_msg void OnConnectButton();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetfocusInmessageEd();
	afx_msg void OnSetfocusInmeslenEd();
	afx_msg void OnSetfocusIntpduEd();
	afx_msg void OnSetfocusOutmeslenEd();
	afx_msg void OnSetfocusOutmessageEd();
	afx_msg void OnSetfocusOuttpduEd();
	afx_msg void OnSetfocusTraceFnameEd();
	afx_msg void OnClose1();
	afx_msg void OnApply1();
	afx_msg void OnHelp1();
	afx_msg void OnSelchangeServicesList();
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDMLock * pLock;

};
