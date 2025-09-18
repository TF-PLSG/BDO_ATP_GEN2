#include "StdAfx.h"
#include "Atp.h"
#include "Cup3DS2PropertySheet.h"
#include "Cup3DS2ParameterDefinition.h"

IMPLEMENT_DYNAMIC(CCup3DS2PropertySheet, CPropertySheet)

CCup3DS2PropertySheet::CCup3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CCup3DS2PropertySheet::CCup3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_Cup3DS2ParameterPage); // *
	AddPage(&m_ChangeHistoryPage);

	SetData ();
}

CCup3DS2PropertySheet::~CCup3DS2PropertySheet()
{
}


BEGIN_MESSAGE_MAP(CCup3DS2PropertySheet, CPropertySheet)
	
END_MESSAGE_MAP()

void CCup3DS2PropertySheet::SetData()  // based on rqrmnt.
{
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;

	m_Cup3DS2ParameterPage.pChangeHistoryPage = &m_ChangeHistoryPage;
}
// CATP_Monitor_Service_Sheet message handlers

BOOL CCup3DS2PropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	//set version info in the title bar  // add if rquired.
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	//lpszTitle = lpszTitle + " - Version - ";
	//lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);
	GetPage(0)->SetModified();              //Enable the Apply Button
	SendMessage(DM_SETDEFID, ID_APPLY_NOW);
	return bResult;
}