#if !defined(AFX_MERCHANTPAYPARAMSHEET_H__7AC6B34E_ECC3_436E_B6BE_52EE8E3CBDB4__INCLUDED_)
#define AFX_MERCHANTPAYPARAMSHEET_H__7AC6B34E_ECC3_436E_B6BE_52EE8E3CBDB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantPayParamSheet.h : header file
//
#include "ChangeHistory.h"
#include "MerchantPayParam.h"
#include "MerchantPayParamDefinition.h"

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamSheet

class CMerchantPayParamSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMerchantPayParamSheet)

// Construction
public:
	CMerchantPayParamSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMerchantPayParamSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CMerchantPayParamDefinition m_MerchantPayParamDefinitionPage;
	CMerchantPayParam m_MerchantPayParamGeneralPage;
    CChangeHistory m_ChangeHistoryPage;

	void SetData();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantPayParamSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMerchantPayParamSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantPayParamSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERCHANTPAYPARAMSHEET_H__7AC6B34E_ECC3_436E_B6BE_52EE8E3CBDB4__INCLUDED_)
