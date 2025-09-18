// TPDURec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTPDURec dialog

class CTPDURec : public CDialog
{
// Construction
public:
	CTPDURec(CWnd* pParent = NULL);   // standard constructor
	
	int m_Mode;

	BOOL CorrectEntry();

// Dialog Data
	//{{AFX_DATA(CTPDURec)
	enum { IDD = IDD_TPDUREC };
	CEdit	m_SrcLen_ed;
	CEdit	m_TransLen_ed;
	CEdit	m_TPDUStr_ed;
	CEdit	m_DestLen_ed;
	CEdit	m_TPDUId_ed;
	CString	m_TransLen;
	CString	m_DestLen;
	CString	m_SrcLen;
	CString	m_TPDUStr;
	CString	m_TPDUId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPDURec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTPDURec)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusTpduid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
