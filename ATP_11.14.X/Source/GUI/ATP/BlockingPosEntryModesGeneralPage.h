
#ifndef POSGENERAL
#define POSGENERAL


class CBlockingPosEntryModesGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBlockingPosEntryModesGeneralPage)
	// Construction
public:
	CBlockingPosEntryModesGeneralPage();
	~CBlockingPosEntryModesGeneralPage();

	char strType[2];
    BOOL m_bGeneral;
    int m_nDialogMode;

	enum { IDD = IDD_BLOCKPOS_GENERAL };
	CEdit	m_CntlPosEntryMode9;
	CEdit	m_CntlPosEntryMode8;
	CEdit	m_CntlPosEntryMode7;
	CEdit	m_CntlPosEntryMode;
	CEdit	m_CntlPosEntryMode5;
	CEdit	m_CntlPosEntryMode4;
	CEdit	m_CntlPosEntryMode3;
	CEdit	m_CntlPosEntryMode2;
	CEdit	m_CntlPosEntryMode1;
	CEdit	m_CntlPosEntryMode10;
	CEdit	m_CntlResponsecode9;
	CEdit	m_CntlResponsecode8;
	CEdit	m_CntlResponsecode7;
	CEdit	m_CntlResponsecode6;
	CEdit	m_CntlResponsecode5;
	CEdit	m_CntlResponsecode4;
	CEdit	m_CntlResponsecode3;
	CEdit	m_CntlResponsecode2;
	CEdit	m_CntlResponsecode10;
	CEdit	m_CntlResponsecode1;
	CString	m_StrPosEntryMode9;
	CString	m_StrPosEntryMode8;
	CString	m_StrPosEntryMode7;
	CString	m_StrPosEntryMode;
	CString	m_StrPosEntryMode5;
	CString	m_StrPosEntryMode4;
	CString	m_StrPosEntryMode3;
	CString	m_StrPosEntryMode2;
	CString	m_StrPosEntryMode1;
	CString	m_StrPosEntryMode10;
	CString	m_StrResponsecode9;
	CString	m_StrResponsecode8;
	CString	m_StrResponsecode7;
	CString	m_StrResponsecode6;
	CString	m_StrResponsecode5;
	CString	m_StrResponsecode4;
	CString	m_StrResponsecode3;
	CString	m_StrResponsecode2;
	CString	m_StrResponsecode10;
	CString	m_StrResponsecode1;
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinGeneral)
	public:
	//virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinGeneral)
	virtual BOOL OnInitDialog();

		//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif