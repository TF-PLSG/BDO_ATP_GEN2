#ifndef RULE_DEFINITION_PROCESSING
#define RULE_DEFINITION_PROCESSING

class CRulesDefinitionProcessing : public CPropertyPage
{
	DECLARE_DYNCREATE(CRulesDefinitionProcessing)

// Construction
public:
	CRulesDefinitionProcessing();
	~CRulesDefinitionProcessing();
    CR01 sRuleIdOld;

	BOOL m_nprocessingPage;
	int m_nDialogMode;

	BOOL m_bProcessing;

	char strStatus[2];
    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50]; 
	void CleanProcessingDialog();
    void EnableProcessingDialog(int);
    void PopulateHistoryTable();

    BOOL PopulateRulesProcessingDialogFromDatabase( CR01 sRuleID );
    BOOL PopulateDatabaseFromRulesProcessingDialog( pCR01 psRuleId );
	BOOL CompareStructures(CR01);
	BOOL CheckDialogForCompleteness();
	BOOL OnApply() ;

	CTime c_time;

	// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = ID_RULESDEFINITION_PROCESSING };
	CEdit	m_CntlRuleID;
	CEdit	m_CntlRuleName;
	CComboBox	m_CntlRuleStatus;
	CComboBox m_CntlRuleprocessing;
	CEdit	m_CntlRuleBinHigh;
	CEdit	m_CntlRuleBinLow;
	CEdit	m_CntlSingleAmtLimit;
	CEdit	m_CntlAccAmtLimit;
	CEdit	m_CntlAccTransactionLimit;
	CEdit	m_CntlResponse_code;
	CEdit	m_CntlAccAmtLimit_counter;
	CEdit	m_CntlAccTransactionLimit_counter;
	CString	m_StrRuleID_Fld;
	CString	m_StrRuleName_Fld;
	CString	m_StrRuleStatus_Fld;
	CString	m_StrRuleprocessing_Fld;
	CString	m_StrRuleBinHigh_Fld;
	CString	m_StrRuleBinLow_Fld;
	CString	m_StrSingleAmtLimit_Fld;
	CString	m_StrAccAmtLimit_Fld;
	CString	m_StrAccTransactionLimit_Fld;
	CString	m_StrResponseCode_Fld;
	CString	m_StrAccAmtLimitCounter_Fld;
	CString	m_StrAccTransactionLimitCounter_Fld;
	CString	m_StrMessageLbl;
	CButton	m_ListButton;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsProcessing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsProcessing)
	virtual BOOL OnInitDialog();
	afx_msg void OnListBinRuleButton();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeRuleProcessingFld();
	afx_msg void OnStnClickedMessageLbl();
	//afx_msg void OnKillfocusHighRange1();
};
#endif