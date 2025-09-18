// MerchantPayParamSheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantPayParamSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamSheet

IMPLEMENT_DYNAMIC(CMerchantPayParamSheet, CPropertySheet)

CMerchantPayParamSheet::CMerchantPayParamSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMerchantPayParamSheet::CMerchantPayParamSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_MerchantPayParamDefinitionPage);
	AddPage(&m_MerchantPayParamGeneralPage);
	AddPage(&m_ChangeHistoryPage);
}

CMerchantPayParamSheet::~CMerchantPayParamSheet()
{
}

void CMerchantPayParamSheet::SetData()
{
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_MerchantPayParamGeneralPage.m_nDialogMode = MODE_DONE;

	m_MerchantPayParamGeneralPage.m_bGeneral = FALSE;
	m_MerchantPayParamGeneralPage.m_nDialogMode = MODE_DONE;
	
	m_MerchantPayParamDefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_MerchantPayParamDefinitionPage.pMerchantPayParamGeneral = &m_MerchantPayParamGeneralPage;
}

BEGIN_MESSAGE_MAP(CMerchantPayParamSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantPayParamSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamSheet message handlers

