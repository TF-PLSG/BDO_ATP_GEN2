// MerchantConfiguration.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantConfiguration dialog


class CMerchantConfiguration : public CDialog
{
// Construction
public:
	CMerchantConfiguration(CWnd* pParent = NULL);   // standard constructor

    
// Dialog Data
	//{{AFX_DATA(CMerchantConfiguration)
	enum { IDD = IDD_MERCHANT_CONFIGURATION };
	CEdit	m_CntlMerchantIDFld;
	CString	m_StrMerchantIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantConfiguration)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMerchantConfiguration)
	virtual BOOL OnInitDialog();
	afx_msg void OnMerchantAddButton();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
