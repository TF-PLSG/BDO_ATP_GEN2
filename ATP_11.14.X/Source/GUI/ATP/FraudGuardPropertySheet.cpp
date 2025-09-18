// FraudGuardPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "FraudGuardPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardPropertySheet

IMPLEMENT_DYNAMIC(CFraudGuardPropertySheet, CPropertySheet)

CFraudGuardPropertySheet::CFraudGuardPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CFraudGuardPropertySheet::CFraudGuardPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);
	AddPage(&m_GeneralPage);
	AddPage(&m_ChangeHistoryPage);
}

CFraudGuardPropertySheet::~CFraudGuardPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CFraudGuardPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFraudGuardPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardPropertySheet message handlers

void CFraudGuardPropertySheet::SetData()
{
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.m_nDialogMode = MODE_DONE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_nDialogMode = MODE_ADD;
	m_GeneralPage.m_bGeneral = FALSE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CFraudGuardPropertySheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

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
	return TRUE;
}
