/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   
*
************************************************************************************/
// DebitPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DebitPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebitPropertySheet

IMPLEMENT_DYNAMIC(CDebitPropertySheet, CPropertySheet)

CDebitPropertySheet::CDebitPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CDebitPropertySheet::CDebitPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);	
	AddPage(&m_ProcessingPage);
	AddPage(&m_CardsProfilePage);
    AddPage(&m_MerchantLimitPage);
	AddPage(&m_Time1Page);
	AddPage(&m_Time2Page);
	AddPage(&m_Time3Page);
	AddPage(&m_CntryLimitPage);
	AddPage(&m_ChangeHistoryPage);
}

CDebitPropertySheet::~CDebitPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CDebitPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDebitPropertySheet)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebitPropertySheet message handlers
void CDebitPropertySheet::SetData()
{
  m_DefinitionPage.m_nCardMode = MODE_DEBIT;
  m_DefinitionPage.pProcessingPage = &m_ProcessingPage;
  m_DefinitionPage.pCardsProfilePage = &m_CardsProfilePage;
  m_DefinitionPage.pMerchantLimitPage = &m_MerchantLimitPage;	
  m_DefinitionPage.pTime1Page = &m_Time1Page;
  m_DefinitionPage.pTime2Page = &m_Time2Page;
  m_DefinitionPage.pTime3Page = &m_Time3Page;
  m_DefinitionPage.pCntryAmtLimitsPage = &m_CntryLimitPage;
  m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;

  m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
  m_ChangeHistoryPage.m_bChangeHistory = FALSE;

  m_MerchantLimitPage.m_bMerchant = FALSE;
  m_MerchantLimitPage.m_nDialogMode = MODE_DONE;

  m_CntryLimitPage.m_nDialogMode = MODE_DONE;
  m_CntryLimitPage.m_bCntryAmtLmt = FALSE;

  m_ProcessingPage.m_bProcessing = FALSE;
  m_ProcessingPage.m_nDialogMode = MODE_DONE;

  m_CardsProfilePage.m_bProcessing = FALSE;
  m_CardsProfilePage.m_nDialogMode = MODE_DONE;

  m_Time1Page.m_bTime1 = FALSE;
  m_Time1Page.m_nDialogMode = MODE_DONE;

  m_Time2Page.m_bTime2 = FALSE;
  m_Time2Page.m_nDialogMode = MODE_DONE;

  m_Time3Page.m_bTime3 = FALSE; 
  m_Time3Page.m_nDialogMode = MODE_DONE;
}


BOOL CDebitPropertySheet::OnInitDialog()
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
