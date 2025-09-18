
#include "StdAfx.h"
#include "Atp.h"
#include "Amex3DS2PropertySheet.h"
#include "Amex3DS2ParameterDefinition.h"

IMPLEMENT_DYNAMIC(CAmex3DS2PropertySheet, CPropertySheet)

CAmex3DS2PropertySheet::CAmex3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CAmex3DS2PropertySheet::CAmex3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_Amex3DS2ParameterPage); // *
	AddPage(&m_ChangeHistoryPage);

	SetData ();
}

CAmex3DS2PropertySheet::~CAmex3DS2PropertySheet()
{
}


BEGIN_MESSAGE_MAP(CAmex3DS2PropertySheet, CPropertySheet)
	
END_MESSAGE_MAP()

void CAmex3DS2PropertySheet::SetData()  // based on rqrmnt.
{
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;

	m_Amex3DS2ParameterPage.pChangeHistoryPage = &m_ChangeHistoryPage;
}
// CATP_Monitor_Service_Sheet message handlers

BOOL CAmex3DS2PropertySheet::OnInitDialog() 
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