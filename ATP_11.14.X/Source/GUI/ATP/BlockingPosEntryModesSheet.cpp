// BlockingPosEntryModePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "BlockingPosEntryModesSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CBlockingPosEntryModePropertySheet

IMPLEMENT_DYNAMIC(CBlockingPosEntryModePropertySheet, CPropertySheet)

CBlockingPosEntryModePropertySheet::CBlockingPosEntryModePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CBlockingPosEntryModePropertySheet::CBlockingPosEntryModePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_mainpage);
	AddPage(&m_generalpage);
	AddPage(&m_ChangeHistoryPage);
}

CBlockingPosEntryModePropertySheet::~CBlockingPosEntryModePropertySheet()
{
}

BEGIN_MESSAGE_MAP(CBlockingPosEntryModePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CBlockingPosEntryModePropertySheet)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBlockingPosEntryModePropertySheet::SetData()
{
	m_mainpage.pGeneralpage = &m_generalpage;
	m_mainpage.pChangeHistoryPage =&m_ChangeHistoryPage;

	m_generalpage.m_nDialogMode = MODE_DONE;	
	m_generalpage.m_bGeneral = FALSE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_BIN;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
}

BOOL CBlockingPosEntryModePropertySheet::OnInitDialog()
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