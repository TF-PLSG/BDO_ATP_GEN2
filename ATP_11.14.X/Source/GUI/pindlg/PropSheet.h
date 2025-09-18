// PropSheet.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPropSheet

class CPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropSheet)

	CDialogPg	DialogPage;
	CComSetupPg	ComSetPage;
	CTPDUPg	TPDUPage;

// Construction
public:
	CPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	CPropSheet() : CPropertySheet()
	{
		Construct("Ascendent System - Dialog Manager Gen-II", this, 0);

		
		AddPage(&DialogPage);
		DialogPage.m_PSheet = this;
		AddPage(&ComSetPage);
		ComSetPage.m_PSheet = this;
		AddPage(&TPDUPage);
		TPDUPage.m_PSheet = this;

		int nResponse = Create(NULL,WS_SYSMENU|WS_POPUP|WS_CAPTION|DS_MODALFRAME|WS_MINIMIZEBOX|WS_VISIBLE,0);
		if (nResponse = 0)
			AfxMessageBox( "Failed to create main window.", MB_OK | MB_ICONSTOP, 0 );

	}

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropSheet)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
