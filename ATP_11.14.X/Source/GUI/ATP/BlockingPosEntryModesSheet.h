/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet

#include  "ChangeHistory.h"
#include  "BlockingPosEntryModesMainPage.h"
#include "BlockingPosEntryModesGeneralPage.h"

class CBlockingPosEntryModePropertySheet : public CPropertySheet
{
		DECLARE_DYNAMIC(CBlockingPosEntryModePropertySheet)

		// Construction
public:
	CBlockingPosEntryModePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CBlockingPosEntryModePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Attributes
public:
	/*Page declarations*/
	CBlockingPosEntryModesMainPage m_mainpage;
	CBlockingPosEntryModesGeneralPage m_generalpage;
	CChangeHistory		m_ChangeHistoryPage;
	int m_nActiveTab;
	// Operations
public:
	void SetData();
	virtual ~CBlockingPosEntryModePropertySheet();


		// Generated message map functions
protected:
	//{{AFX_MSG(CDebitPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CBlockingPosEntryModePropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
