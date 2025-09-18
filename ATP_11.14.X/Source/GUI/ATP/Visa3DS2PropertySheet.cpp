/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet
#include "stdafx.h"
#include "Atp.h"
#include "Visa3DS2ConfigMain.h"
#include "Visa3DS2ConfigPropertySheet.h"



/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet

IMPLEMENT_DYNAMIC(CVisa3DS2ConfigPropertySheet, CPropertySheet)

CVisa3DS2ConfigPropertySheet::CVisa3DS2ConfigPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CVisa3DS2ConfigPropertySheet::CVisa3DS2ConfigPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_mainpage);
	AddPage(&m_ChangeHistoryPage);
}

CVisa3DS2ConfigPropertySheet::~CVisa3DS2ConfigPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CVisa3DS2ConfigPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CVisa3DS2ConfigPropertySheet)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CVisa3DS2ConfigPropertySheet::SetData()
{
	m_mainpage.m_nDialogMode=MODE_DONE;
	m_mainpage.pChangeHistoryPage =&m_ChangeHistoryPage;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
}

BOOL CVisa3DS2ConfigPropertySheet::OnInitDialog()
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