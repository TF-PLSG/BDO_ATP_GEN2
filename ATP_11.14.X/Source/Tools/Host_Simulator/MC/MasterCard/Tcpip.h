// tcpip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// tcpip dialog

class tcpip : public CDialog
{
// Construction
public:
	tcpip(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(tcpip)
	enum { IDD = IDD_DIALOG1 };
	CEdit	m_port;
	CEdit	m_host;
	CString	m_StrHost;
	CString	m_StrPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(tcpip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(tcpip)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
