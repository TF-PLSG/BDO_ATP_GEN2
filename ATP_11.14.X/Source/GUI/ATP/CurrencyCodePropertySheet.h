
// CurrencyCodePropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRelatedPropertySheet

#include "CurrencyCodeDefinition.h"
#include "ChangeHistory.h"


class CurrencyCodePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CurrencyCodePropertySheet)

// Construction
public:
	CurrencyCodePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CurrencyCodePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:


	CurrencyCodeDefinition m_CurrencyCodeParameterPage;
    CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData();
	BOOL CurrencyCodePropertySheet::OnInitDialog();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelatedPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CurrencyCodePropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRelatedPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
