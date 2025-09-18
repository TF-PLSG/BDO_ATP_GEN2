// MerchantCardTypePropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypePropertySheet

class CMerchantCardTypePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMerchantCardTypePropertySheet)

// Construction
public:
	CMerchantCardTypePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMerchantCardTypePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantCardTypePropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMerchantCardTypePropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantCardTypePropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
