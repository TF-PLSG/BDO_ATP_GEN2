/////////////////////////////////////////////////////////////////////////////
// CVisa3DS2ConfigPropertySheet
#include  "Visa3DS2ConfigMain.h"
#include  "ChangeHistory.h"



class CVisa3DS2ConfigPropertySheet : public CPropertySheet
{
		DECLARE_DYNAMIC(CVisa3DS2ConfigPropertySheet);

		// Construction
public:
	CVisa3DS2ConfigPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CVisa3DS2ConfigPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Attributes
public:
	/*Page declarations*/
	CVisa3DS2ConfigMainPage m_mainpage;
	CChangeHistory		m_ChangeHistoryPage;
	int m_nActiveTab;
	// Operations
public:
	void SetData();
	virtual ~CVisa3DS2ConfigPropertySheet();


		// Generated message map functions
protected:
	//{{AFX_MSG(CDebitPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CVisa3DS2ConfigPropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
