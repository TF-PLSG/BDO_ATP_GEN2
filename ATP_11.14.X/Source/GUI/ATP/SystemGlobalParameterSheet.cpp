// SubsidyQuotaSheet.cpp : implementation file
//
#include "StdAfx.h"
#include "Atp.h"
#include "SystemGlobalParameterSheet.h"
#include "SystemGlobalParameterDefinition.h"

// CATP_Monitor_Service_Sheet

IMPLEMENT_DYNAMIC(CSystemGlobalParameterSheet, CPropertySheet)

CSystemGlobalParameterSheet::CSystemGlobalParameterSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CSystemGlobalParameterSheet::CSystemGlobalParameterSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_SystemGlobalParameterPage); // *
	AddPage(&m_ChangeHistoryPage);

	SetData ();
}

CSystemGlobalParameterSheet::~CSystemGlobalParameterSheet()
{
}


BEGIN_MESSAGE_MAP(CSystemGlobalParameterSheet, CPropertySheet)
	
END_MESSAGE_MAP()

void CSystemGlobalParameterSheet::SetData()  // based on rqrmnt.
{
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;

	m_SystemGlobalParameterPage.pChangeHistoryPage = &m_ChangeHistoryPage;
}
// CATP_Monitor_Service_Sheet message handlers

BOOL CSystemGlobalParameterSheet::OnInitDialog() 
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