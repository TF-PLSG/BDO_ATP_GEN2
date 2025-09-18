// SubsidyQuotaSheet.cpp : implementation file
//
#include "StdAfx.h"
#include "Atp.h"
#include "OnlinePINControlBDKKeySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CATP_Monitor_Service_Sheet

IMPLEMENT_DYNAMIC(COnlinePINConfigureBDKKeySheet, CPropertySheet)

COnlinePINConfigureBDKKeySheet::COnlinePINConfigureBDKKeySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

COnlinePINConfigureBDKKeySheet::COnlinePINConfigureBDKKeySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_ConfigureBDKmainPage); // *m_ConfigureRulemainPage
	AddPage(&m_ConfigureBDKGenralPage);
	AddPage(&m_ChangeHistoryPage);

	//SetData ();
}

COnlinePINConfigureBDKKeySheet::~COnlinePINConfigureBDKKeySheet()
{
}


BEGIN_MESSAGE_MAP(COnlinePINConfigureBDKKeySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCardsKeyblockPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COnlinePINConfigureBDKKeySheet::SetData()  // based on rqrmnt.
{
	m_ConfigureBDKmainPage.m_nDialogMode=MODE_DONE;

	m_ConfigureBDKmainPage.pCBDKKeyConfigurationGeneral = &m_ConfigureBDKGenralPage;
	m_ConfigureBDKmainPage.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	//m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
	m_ConfigureBDKGenralPage.m_nDialogMode=MODE_DONE;
	m_ConfigureBDKGenralPage.m_bBDKKEYGeneral=FALSE;
}
// CATP_Monitor_Service_Sheet message handlers

BOOL COnlinePINConfigureBDKKeySheet::OnInitDialog() 
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