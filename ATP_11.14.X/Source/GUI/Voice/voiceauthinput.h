// VoiceAuthInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthInput dialog

class CVoiceAuthInput : public CDialog
{
// Construction
public:
	CVoiceAuthInput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVoiceAuthInput)
	enum { IDD = IDD_VOICE_AUTH_INPUT };
	CComboBox	m_CntlIndustryCode;
	CButton	m_ClearButton;
	CButton	m_ProcessButton;
	CComboBox	m_CntlTransactionType;
	CEdit	m_CntlCVV2Fld;
	CEdit	m_CntlResponseCodeFld;
	CEdit	m_CntlAuthNumberFld;
	CEdit	m_CntlDeferredPurchaseTermFld;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlExpiryDateFld;
	CEdit	m_CntlAmountFld;
	CEdit	m_CntlAccountNumberFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrAmountFld;
	CString	m_StrAuthNumberFld;
	CString	m_StrDeferredPurchaseTermFld;
	CString	m_StrExpiryDateFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrResponseCodeFld;
	CString	m_StrTransactionType;
	CString	m_StrCVV2Fld;
	CString	m_IndustryCode;
	//}}AFX_DATA

	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate( CString );
	int m_nDialogMode;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVoiceAuthInput)
	virtual BOOL OnInitDialog();
	afx_msg void OnProcessAuthButton();
	afx_msg void OnSelchangeTransactionType();
	virtual void OnOk();
	afx_msg void OnOk1();
	afx_msg void OnClearButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
