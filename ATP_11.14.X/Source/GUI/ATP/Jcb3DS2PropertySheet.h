/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet
#include "Jcb3DS2ParameterDefinition.h"
#include "ChangeHistory.h"



class CJcb3DS2PropertySheet : public CPropertySheet
{
		DECLARE_DYNAMIC(CJcb3DS2PropertySheet)

		// Construction
public:
	CJcb3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CJcb3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// Attributes
public:
	/*Page declarations*/
	CJcb3DS2Parameter m_Jcb3DS2ParameterPage;
	CChangeHistory		m_ChangeHistoryPage;
	int m_nActiveTab;
	// Operations
public:
	void SetData();
	virtual ~CJcb3DS2PropertySheet();


		// Generated message map functions
protected:
	//{{AFX_MSG(CDebitPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CJcb3DS2PropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
