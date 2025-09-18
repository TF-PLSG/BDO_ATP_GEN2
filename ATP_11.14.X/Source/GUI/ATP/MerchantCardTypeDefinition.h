// MerchantCardTypeDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypeDefinition dialog

class CMerchantCardTypeDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantCardTypeDefinition)

// Construction
public:
	CMerchantCardTypeDefinition();
	~CMerchantCardTypeDefinition();

// Dialog Data
	//{{AFX_DATA(CMerchantCardTypeDefinition)
	enum { IDD = IDD_MERCHANT_CARD_TYPE_DEFINITION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantCardTypeDefinition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantCardTypeDefinition)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
