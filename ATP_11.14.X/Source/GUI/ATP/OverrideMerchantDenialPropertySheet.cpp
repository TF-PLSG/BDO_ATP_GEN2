// OverrideMerchantDenialPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "OverrideMerchantDenialPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenialPropertySheet

IMPLEMENT_DYNAMIC(COverrideMerchantDenialPropertySheet, CPropertySheet)

COverrideMerchantDenialPropertySheet::COverrideMerchantDenialPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COverrideMerchantDenialPropertySheet::COverrideMerchantDenialPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_OverridePage);
    AddPage(&m_ChangeHistoryPage);	

}

COverrideMerchantDenialPropertySheet::~COverrideMerchantDenialPropertySheet()
{
}


BEGIN_MESSAGE_MAP(COverrideMerchantDenialPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(COverrideMerchantDenialPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenialPropertySheet message handlers

BOOL COverrideMerchantDenialPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);
	m_ChangeHistoryPage.m_nDialogMode = MODE_VIEW;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
	memset(&(m_ChangeHistoryPage.sHistoryRecord), 0, sizeof(HST01));
    //memcpy ( &(m_ChangeHistoryPage.sHistoryRecord), &m_OverridePage.sHistoryRecord, sizeof(HST01));
	m_OverridePage.pChangeHistoryPage = &m_ChangeHistoryPage;

	return bResult;
}
