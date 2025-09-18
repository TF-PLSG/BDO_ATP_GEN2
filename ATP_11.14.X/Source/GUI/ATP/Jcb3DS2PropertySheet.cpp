// MC3DSecureOBSAuthenticationSheet.cpp : implementation file
//

#include "StdAfx.h"
#include "Atp.h"
#include "Jcb3DS2PropertySheet.h"
#include "Jcb3DS2ParameterDefinition.h"


/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet

IMPLEMENT_DYNAMIC(CJcb3DS2PropertySheet, CPropertySheet)

CJcb3DS2PropertySheet::CJcb3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CJcb3DS2PropertySheet::CJcb3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_Jcb3DS2ParameterPage);
	AddPage(&m_ChangeHistoryPage);
}

CJcb3DS2PropertySheet::~CJcb3DS2PropertySheet()
{
}

BEGIN_MESSAGE_MAP(CJcb3DS2PropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMC3DSecureOBSAuthenticationSheet)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CJcb3DS2PropertySheet::SetData()
{
	m_Jcb3DS2ParameterPage.m_nDialogMode=MODE_DONE;
	m_Jcb3DS2ParameterPage.pChangeHistoryPage =&m_ChangeHistoryPage;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
}

BOOL CJcb3DS2PropertySheet::OnInitDialog()
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