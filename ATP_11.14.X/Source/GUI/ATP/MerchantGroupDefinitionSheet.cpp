// MerchantGroupDefinitionSheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantGroupDefinitionSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupDefinitionSheet

IMPLEMENT_DYNAMIC(CMerchantGroupDefinitionSheet, CPropertySheet)

CMerchantGroupDefinitionSheet::CMerchantGroupDefinitionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMerchantGroupDefinitionSheet::CMerchantGroupDefinitionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_MerchantGroupDefinitionPage);
	AddPage(&m_MerchantGroupGeneralPage);
	AddPage(&m_ChangeHistoryPage);
}

CMerchantGroupDefinitionSheet::~CMerchantGroupDefinitionSheet()
{
}

void CMerchantGroupDefinitionSheet::SetData()
{
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_MerchantGroupGeneralPage.m_nDialogMode = MODE_DONE;

	m_MerchantGroupGeneralPage.m_bGeneral = FALSE;
	m_MerchantGroupDefinitionPage.m_nDialogMode = MODE_DONE;

	m_MerchantGroupDefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_MerchantGroupDefinitionPage.pMerchantGroupGeneral = &m_MerchantGroupGeneralPage;

}

BEGIN_MESSAGE_MAP(CMerchantGroupDefinitionSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantGroupDefinitionSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupDefinitionSheet message handlers

BOOL CMerchantGroupDefinitionSheet::OnInitDialog() 
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
	
	GetPage(0)->SetModified();              //Enable the Apply Button
	SendMessage(DM_SETDEFID, ID_APPLY_NOW); //Set the Apply button as the default button(instead of OK button	

	return bResult;
}
