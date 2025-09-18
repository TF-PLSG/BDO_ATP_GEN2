// SubsidyQuotaSheet.cpp : implementation file
//
#include "StdAfx.h"
#include "atp.h"
#include "FraudControlConfigureLinkSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CATP_Monitor_Service_Sheet

IMPLEMENT_DYNAMIC(CFraudControlConfigureLinkSheet, CPropertySheet)

CFraudControlConfigureLinkSheet::CFraudControlConfigureLinkSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CFraudControlConfigureLinkSheet::CFraudControlConfigureLinkSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_ConfigureLinkPage); // 
	AddPage(&m_ConfigureGenralPage);
	AddPage(&m_ChangeHistoryPage);

	//SetData ();
}

CFraudControlConfigureLinkSheet::~CFraudControlConfigureLinkSheet()
{
}


BEGIN_MESSAGE_MAP(CFraudControlConfigureLinkSheet, CPropertySheet)
	
END_MESSAGE_MAP()

void CFraudControlConfigureLinkSheet::SetData()  // based on rqrmnt.
{

	m_ConfigureLinkPage.pFraudControlConfigurationLinkGeneralpage = &m_ConfigureGenralPage;
	m_ConfigureLinkPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_ConfigureLinkPage.m_nDialogMode=MODE_DONE;
	m_ConfigureGenralPage.m_nDialogMode=MODE_DONE;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ConfigureGenralPage.m_bFraudGeneral=FALSE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	//	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
}
// CATP_Monitor_Service_Sheet message handlers

BOOL CFraudControlConfigureLinkSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	//set version info in the title bar  // add if rquired.
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