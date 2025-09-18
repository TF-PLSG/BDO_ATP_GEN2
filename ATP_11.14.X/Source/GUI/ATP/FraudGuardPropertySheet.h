#if !defined(AFX_FRAUDGUARDPROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_)
#define AFX_FRAUDGUARDPROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FraudGuardPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardPropertySheet

#include "FraudGuardDefinition.h"
#include "FraudGuardGeneral.h"
#include "ChangeHistory.h"


class CFraudGuardPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFraudGuardPropertySheet)

// Construction
public:
	CFraudGuardPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFraudGuardPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CFraudGuardDefinition m_DefinitionPage;
	CFraudGuardGeneral m_GeneralPage;
	CChangeHistory m_ChangeHistoryPage;


// Operations
public:
	void SetData();
	BOOL CFraudGuardPropertySheet::OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFraudGuardPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFraudGuardPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFraudGuardPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAUDGUARDPROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_)
