#if !defined(AFX_OVERRIDEMERCHANTDENIALPROPERTYSHEET_H__69BFF735_F1C3_413D_9D48_7C3ECE523C36__INCLUDED_)
#define AFX_OVERRIDEMERCHANTDENIALPROPERTYSHEET_H__69BFF735_F1C3_413D_9D48_7C3ECE523C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverrideMerchantDenialPropertySheet.h : header file
//
#include "OverrideMerchantDenial.h"
#include "ChangeHistory.h"

/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenialPropertySheet

class COverrideMerchantDenialPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COverrideMerchantDenialPropertySheet)

// Construction
public:
	COverrideMerchantDenialPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COverrideMerchantDenialPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	COverrideMerchantDenial m_OverridePage;
	    CChangeHistory m_ChangeHistoryPage;
    int m_nActiveTab;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COverrideMerchantDenialPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COverrideMerchantDenialPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(COverrideMerchantDenialPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERRIDEMERCHANTDENIALPROPERTYSHEET_H__69BFF735_F1C3_413D_9D48_7C3ECE523C36__INCLUDED_)
