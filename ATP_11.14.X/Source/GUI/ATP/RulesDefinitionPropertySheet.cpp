
// RulesDefinitionPropertySheet.cpp : implementation file


#include "stdafx.h"
#include "Atp.h"
#include "RulesDefinitionPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRulesDefinitionPropertySheet

IMPLEMENT_DYNAMIC(CRulesDefinitionPropertySheet, CPropertySheet)

CRulesDefinitionPropertySheet::CRulesDefinitionPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}


CRulesDefinitionPropertySheet::CRulesDefinitionPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_mainpage);
    AddPage (&m_RulesProcessingPage);
	AddPage (&m_CardNumBlockingPage);
    AddPage (&m_ChangeHistoryPage);



    //AddPage (&m_CapabilitiesPage);
}

CRulesDefinitionPropertySheet::~CRulesDefinitionPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CRulesDefinitionPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPropertySheet message handlers
void CRulesDefinitionPropertySheet::SetData()
{
	m_mainpage.pRulesProcessingPage = &m_RulesProcessingPage;
	m_mainpage.pCardNumBlockingPage = &m_CardNumBlockingPage;
	m_mainpage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_RulesProcessingPage.m_bProcessing = FALSE;
	m_RulesProcessingPage.m_nDialogMode = MODE_DONE;

	m_CardNumBlockingPage.m_bExceptionCardNum = FALSE;
	m_CardNumBlockingPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

}

BOOL CRulesDefinitionPropertySheet::OnInitDialog()
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
