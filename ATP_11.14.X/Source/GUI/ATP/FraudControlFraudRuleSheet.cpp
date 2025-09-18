// SubsidyQuotaSheet.cpp : implementation file
//
#include "StdAfx.h"
#include "Atp.h"
#include "FraudControlFraudRuleSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CATP_Monitor_Service_Sheet

IMPLEMENT_DYNAMIC(CFraudControlConfigureRuleSheet, CPropertySheet)

CFraudControlConfigureRuleSheet::CFraudControlConfigureRuleSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CFraudControlConfigureRuleSheet::CFraudControlConfigureRuleSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_ConfigureRulemainPage); // *m_ConfigureRulemainPage
	AddPage(&m_ConfigureRuleGenralPage);
	AddPage(&m_ChangeHistoryPage);

	//SetData ();
}

CFraudControlConfigureRuleSheet::~CFraudControlConfigureRuleSheet()
{
}


BEGIN_MESSAGE_MAP(CFraudControlConfigureRuleSheet, CPropertySheet)
	
END_MESSAGE_MAP()

void CFraudControlConfigureRuleSheet::SetData()  // based on rqrmnt.
{

	m_ConfigureRulemainPage.pFraudControlConfigurationFraudRule = &m_ConfigureRuleGenralPage;
	m_ConfigureRulemainPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_ConfigureRulemainPage.m_nDialogMode=MODE_DONE;
	m_ConfigureRuleGenralPage.m_nDialogMode=MODE_DONE;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ConfigureRuleGenralPage.m_bFraudGeneral=FALSE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	//	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;

}
// CATP_Monitor_Service_Sheet message handlers

BOOL CFraudControlConfigureRuleSheet::OnInitDialog() 
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
	SendMessage(DM_SETDEFID, ID_APPLY_NOW);
	return bResult;
}