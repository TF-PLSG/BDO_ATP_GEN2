#if !defined(AFX_CB_PROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_)
#define AFX_CB_PROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CB_PropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CB_PropertySheet

#include "CB_Definition.h"
#include "CB_General.h"
#include "CB_Amounts.h"
#include "ChangeHistory.h"


class CCB_PropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCB_PropertySheet)

// Construction
public:
	CCB_PropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCB_PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CCB_Definition m_DefinitionPage;
	CCB_General    m_GeneralPage;
   CCB_Amounts    m_AmountsPage;
	CChangeHistory m_ChangeHistoryPage;


// Operations
public:
	void SetData();
	BOOL CCB_PropertySheet::OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCB_PropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCB_PropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCB_PropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CB_PROPERTYSHEET_H__4A2AA89A_9B2F_4CBE_AF5D_855A01995798__INCLUDED_)
