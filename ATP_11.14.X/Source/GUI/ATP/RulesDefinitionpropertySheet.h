// CRulesDefinitionPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRulesDefinitionPropertySheet
#ifndef CHIPFALLBACKRULESSHEET
#define CHIPFALLBACKRULESSHEET


#include "RulesDefinitionProcessing.h"
#include "RulesDefinitionMain.h"
#include "RulesExceptionList.h"
#include "ChangeHistory.h"


class CRulesDefinitionPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CRulesDefinitionPropertySheet)

// Construction
public:
	CRulesDefinitionPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CRulesDefinitionPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

    CRuleDefinitionMain				m_mainpage;
    CRulesDefinitionProcessing		m_RulesProcessingPage;  
	CRulesDefinitionExceptionList   m_CardNumBlockingPage;
	CChangeHistory					m_ChangeHistoryPage;

// Operations
public:

	void SetData();
	virtual ~CRulesDefinitionPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CRulesDefinitionPropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
#endif