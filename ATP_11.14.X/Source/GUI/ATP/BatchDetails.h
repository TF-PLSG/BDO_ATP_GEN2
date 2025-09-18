// BatchDetails.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchDetails dialog

class CBatchDetails : public CDialog
{
// Construction
public:
	CBatchDetails(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CBatchDetails)
	enum { IDD = IDD_BATCH_DETAILS };
	CComboBox	m_CntlTransactionTypeFld;
	CButton	m_ApplyButton;
	CEdit	m_CntlTextFld;
	CEdit	m_CntlFareAmountFld;
	CEdit	m_CntlRRNFld;
	CEdit	m_CntlManNumberFld;
	CEdit	m_CntlJobNumberFld;
	CEdit	m_CntlTimeFld;
	CEdit	m_CntlInvoiceNumberFld;
	CEdit	m_CntlExpDateFld;
	CEdit	m_CntlDateFld;
	CEdit	m_CntlCardNumberFld;
	CEdit	m_CntlAuthIDFld;
	CEdit	m_CntlAmountFld;
	CString	m_StrAmountFld;
	CString	m_StrAuthIDFld;
	CString	m_StrCardNumberFld;
	CString	m_StrDateFld;
	CString	m_StrExpDateFld;
	CString	m_StrInvoiceNumberFld;
	CString	m_StrTimeFld;
	CString	m_StrTransactionType;
	CString	m_StrMessageLbl;
	CString	m_StrJobNumberFld;
	CString	m_StrManNumberFld;
	CString	m_StrRRNFld;
	CString	m_StrFareAmountFld;
	CString	m_StrTextFld;
	//}}AFX_DATA
BOOL CheckDialogForCompleteness();
BOOL InsertRecord();
BOOL PopulateStructure();
char strDeviceID[9];
void calc_julian_day( pCHAR, pCHAR  );
void get_time( pCHAR , pCHAR  );
void get_date( pCHAR , pCHAR  );
void create_rrn( pCHAR  );
BOOL ValidateDate ( CString );
BOOL ValidateTime ( CString );
BOOL ValidateExpDate ( CString );
BOOL ValidateCardNumber ( CString );


CString strOpenDate;
CString strOpenTime;





TLF01 sTLF01Record;
BCH10 sBCH10Record;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchDetails)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchDetails)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
