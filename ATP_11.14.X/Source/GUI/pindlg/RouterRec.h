// RouterRec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRouterRec dialog

class CRouterRec : public CDialog
{
// Construction
public:
	CRouterRec(CWnd* pParent = NULL);   // standard constructor

	int m_Mode;
	BOOL CorrectEntry();

// Dialog Data
	//{{AFX_DATA(CRouterRec)
	enum { IDD = IDD_ROUTREC };
	CEdit	m_Offset_ed;
	CEdit	m_LoRange_ed;
	CEdit	m_Length_ed;
	CEdit	m_HiRange_ed;
	CEdit	m_DestQue_ed;
	CEdit	m_DataType_ed;
	CEdit	m_AppDesc_ed;
	CString	m_AppDesc;
	CString	m_DataType;
	CString	m_DestQue;
	CString	m_HiRange;
	CString	m_Length;
	CString	m_LoRange;
	CString	m_Offset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRouterRec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRouterRec)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
