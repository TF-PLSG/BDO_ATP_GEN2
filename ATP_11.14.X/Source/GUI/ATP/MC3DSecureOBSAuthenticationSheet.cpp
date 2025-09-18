// MC3DSecureOBSAuthenticationSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MC3DSecureOBSAuthenticationMain.h"
#include "MC3DSecureOBSAuthenticationSheet.h"


/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet

IMPLEMENT_DYNAMIC(CMC3DSecureOBSAuthenticationSheet, CPropertySheet)

CMC3DSecureOBSAuthenticationSheet::CMC3DSecureOBSAuthenticationSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMC3DSecureOBSAuthenticationSheet::CMC3DSecureOBSAuthenticationSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_mainpage);
	AddPage(&m_ChangeHistoryPage);
}

CMC3DSecureOBSAuthenticationSheet::~CMC3DSecureOBSAuthenticationSheet()
{
}

BEGIN_MESSAGE_MAP(CMC3DSecureOBSAuthenticationSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMC3DSecureOBSAuthenticationSheet)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMC3DSecureOBSAuthenticationSheet::SetData()
{
	m_mainpage.m_nDialogMode=MODE_DONE;
	m_mainpage.pChangeHistoryPage =&m_ChangeHistoryPage;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
}

BOOL CMC3DSecureOBSAuthenticationSheet::OnInitDialog()
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