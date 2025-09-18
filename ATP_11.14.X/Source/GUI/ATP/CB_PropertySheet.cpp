// CB_PropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CB_PropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCB_PropertySheet

IMPLEMENT_DYNAMIC(CCB_PropertySheet, CPropertySheet)

CCB_PropertySheet::CCB_PropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCB_PropertySheet::CCB_PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);
	AddPage(&m_GeneralPage);
	AddPage(&m_AmountsPage);
	AddPage(&m_ChangeHistoryPage);
}

CCB_PropertySheet::~CCB_PropertySheet()
{
}


BEGIN_MESSAGE_MAP(CCB_PropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCB_PropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCB_PropertySheet message handlers

void CCB_PropertySheet::SetData()
{
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;
	m_DefinitionPage.pAmountsPage = &m_AmountsPage;
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.m_nDialogMode = MODE_DONE;

	m_GeneralPage.m_nDialogMode = MODE_DONE;
	m_GeneralPage.m_bGeneral = FALSE;

	m_AmountsPage.m_nDialogMode = MODE_DONE;
	m_AmountsPage.m_bAmounts = FALSE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_ADD;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
}

/* This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CCB_PropertySheet::OnInitDialog()
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
