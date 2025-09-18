// ComSetupPg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CComSetupPg dialog

class CComSetupPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CComSetupPg)

// Construction
public:
	void EnableTPDUGrb();
	CComSetupPg();
	~CComSetupPg();

// Dialog Data
	//{{AFX_DATA(CComSetupPg)
	enum { IDD = IDD_COMM_SETUP };
	CButton	m_SerialSingleThread;
	CEdit	m_ComPortValue;
	CButton	m_IgnoreTheTPDU;
	CButton	m_IgnoreTPDU;
	CButton	m_dmrec_delete;
	CButton	m_dmrec_modify;
	CButton	m_TPDU_grb;
	CButton	m_WithoutTPDU;
	CButton	m_WithTPDU;
	CButton	m_SocketType_grb;
	CButton	m_ServerSocket;
	CButton	m_ClientSocket;
	CStatic	m_HeaderLength_lbl;
	CEdit	m_HeaderLength;
	CEdit	m_HostName;
	CStatic	m_Service_ed;
	CStatic	m_HostName_lbl;
	CButton	m_Apply0_btn;
	CButton	m_SetBtn;
	CStatic	m_ComType_lbl;
	CButton	m_StopBits;
	CButton	m_StopB2_Radio;
	CButton	m_StopB15_Radio;
	CButton	m_StopB1_Radio;
	CButton	m_ParEven_Radio;
	CButton	m_ParNone_Radio;
	CButton	m_ParOdd_Radio;
	CButton	m_Parity;
	CButton	m_ComPort;
	CButton	m_BTS8_Radio;
	CButton	m_BTS7_Radio;
	CButton	m_BTS;
	CButton	m_BaudRate1_Radio;
	CButton	m_BaudRate3_Radio;
	CButton	m_BaudRate2_Radio;
	CButton	m_BaudRate_Grb;
	CStatic	m_TCPIP_PORT_LBL;
	CButton	m_SERIAL_SETUP;
	CButton	m_TCPIP_SETUP;
	CEdit	m_TCPIPPort;
	CComboBox	m_ComType;
	//}}AFX_DATA

	CPinDlgApp *m_app;
	CPropertySheet *m_PSheet;
	CString m_ComTypeSelected;
	CString m_TCPIPPortSelected;
	CString m_MesString;
	CString m_HostNameSelected;
	CString m_HeaderLengthSelected;
   CString m_ComPortValueSelected;
	char ErrMsg[100];

	void EnableSerialGrb(BOOL onoff);
	void EnableDirectTCP(BOOL onoff);
	void EnableTCPIPGrb(BOOL onoff);
	void SetSerialData();
	void SetTCPIPData();
	void ClearDirectTCP();
	void ClearTCPIPGrb();
	void ClearSerialGrb();
	BOOL CheckTCPIPEntry(); 
	void EnableTPDUGrb(BOOL onoff);
	void ClearTPDUGrb();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CComSetupPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CComSetupPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComtypeCombo();
	afx_msg void OnSetbtn();
	afx_msg void OnChangeTcpipPortEd();
	afx_msg void OnClose0();
	afx_msg void OnApply0();
	afx_msg void OnHelp0();
	afx_msg void OnChangeHostNameEd();
	afx_msg void OnChangeHeaderLengthEd();
	afx_msg void OnClientSocketRadio();
	afx_msg void OnServerSocketRadio();
	afx_msg void OnChangeCommPort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
