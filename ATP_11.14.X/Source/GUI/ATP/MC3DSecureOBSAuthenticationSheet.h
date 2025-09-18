/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet
#include  "MC3DSecureOBSAuthenticationMain.h"
#include  "ChangeHistory.h"



class CMC3DSecureOBSAuthenticationSheet : public CPropertySheet
{
		DECLARE_DYNAMIC(CMC3DSecureOBSAuthenticationSheet)

		// Construction
public:
	CMC3DSecureOBSAuthenticationSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMC3DSecureOBSAuthenticationSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Attributes
public:
	/*Page declarations*/
	CMC3DSecureOBSAuthenticationMainPage m_mainpage;
	CChangeHistory		m_ChangeHistoryPage;
	int m_nActiveTab;
	// Operations
public:
	void SetData();
	virtual ~CMC3DSecureOBSAuthenticationSheet();


		// Generated message map functions
protected:
	//{{AFX_MSG(CDebitPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CMC3DSecureOBSAuthenticationSheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
