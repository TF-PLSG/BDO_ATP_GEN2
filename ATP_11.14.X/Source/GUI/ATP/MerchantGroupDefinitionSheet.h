#if !defined(AFX_MERCHANTGROUPDEFINITIONSHEET_H__F60C722E_1317_4534_B360_A681EBA8DCA2__INCLUDED_)
#define AFX_MERCHANTGROUPDEFINITIONSHEET_H__F60C722E_1317_4534_B360_A681EBA8DCA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantGroupDefinitionSheet.h : header file
//
#include "MerchantGroupGeneral.h"
#include "MerchantGroupDefinition.h"
#include "ChangeHistory.h"

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupDefinitionSheet

class CMerchantGroupDefinitionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMerchantGroupDefinitionSheet)

// Construction
public:
	CMerchantGroupDefinitionSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMerchantGroupDefinitionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);


// Attributes
public:

	CMerchantGroupDefinition m_MerchantGroupDefinitionPage;
	CMerchantGroupGeneral m_MerchantGroupGeneralPage;
    CChangeHistory m_ChangeHistoryPage;

	void SetData();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantGroupDefinitionSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMerchantGroupDefinitionSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantGroupDefinitionSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERCHANTGROUPDEFINITIONSHEET_H__F60C722E_1317_4534_B360_A681EBA8DCA2__INCLUDED_)
