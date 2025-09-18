
#ifndef CURR_CODE_DEFINITION
#define CURR_CODE_DEFINITION

#include "basictyp.h" 
#include "ChangeHistory.h"
#include "afxwin.h"
#define  IDT_TIMER  WM_USER + 200

class CurrencyCodeDefinition : public CPropertyPage 
{
	DECLARE_DYNCREATE(CurrencyCodeDefinition)

// Construction
public:
	CurrencyCodeDefinition();
	~CurrencyCodeDefinition();

    int m_nDialogMode;
	int m_nPrevMode;

	CChangeHistory *pChangeHistoryPage;

	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL PopulateCurrencyCodeDatabaseStructureFromDialog ( pCCL01 );
	void PopulateCurrencyCodeDialogFromDatabase( CCL01 );

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];


	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CRelatedDefinition)
	enum { IDD = IDD_CURRENCY_CODE_DEFINITION };

	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	
	
	
	
	CEdit	m_CntlCurrencyValueFld;
	CEdit	m_CntlCurrencyCodeFld;
	CEdit	m_CntlDecPlacesFld;
	CEdit	m_CntlCurrencyNameFld;
	CString	m_StrCurrencyValueFld;
	CString	m_StrCurrencyCodeFld;
	CString	m_StrDecimalPlacesFld;
	CString	m_StrCurrencyNameFld;

	
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRelatedDefinition)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRelatedDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	afx_msg void OnListButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedListmcc();
	virtual BOOL OnSetActive();
};
#endif