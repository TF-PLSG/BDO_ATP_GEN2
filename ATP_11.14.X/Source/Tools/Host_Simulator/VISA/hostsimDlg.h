// hostsimDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHostsimDlg dialog

class CHostsimDlg : public CDialog
{
// Construction
public:
	CHostsimDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHostsimDlg)
	enum { IDD = IDD_HOSTSIM_DIALOG };
	CEdit	m_Message;
	CButton	m_exit;
	CButton	m_CommSetup;
	CComboBox	m_commtype;
	CButton	m_Offline;
	CButton	m_Online;
	CString	m_StrPortNumberFld;
	//}}AFX_DATA

	CString m_commtypeSelected;
	CString m_hostSelected;
	CString m_portSelected;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHostsimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHostsimDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg void OnOnline();
	afx_msg void OnOffline();
	afx_msg void OnCommsetup();
	afx_msg void OnSelchangeCommtype();
	afx_msg void SetUpComm();
	virtual void OnCancel();
	afx_msg int GetPrivateProfileString1(char * section_name,
                             char * key_name,
                             char * default_str,
                             char * dest_buffer,
                             UINT  buffer_size,
                             char * file_name);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
