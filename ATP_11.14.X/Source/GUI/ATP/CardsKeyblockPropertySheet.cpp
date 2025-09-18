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

    
*
************************************************************************************/
// CCardsKeyblockPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsKeyblockPropertySheet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardsKeyblockPropertySheet

IMPLEMENT_DYNAMIC(CCardsKeyblockPropertySheet, CPropertySheet)

CCardsKeyblockPropertySheet::CCardsKeyblockPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCardsKeyblockPropertySheet::CCardsKeyblockPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{   
	AddPage(&m_DefinitionPage);
	AddPage(&m_ProcessingPage);
	AddPage(&m_ChangeHistoryPage);
}

CCardsKeyblockPropertySheet::~CCardsKeyblockPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CCardsKeyblockPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCardsKeyblockPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsKeyblockPropertySheet message handlers

void CCardsKeyblockPropertySheet::SetData()
{
  m_DefinitionPage.m_nCardMode = MODE_CREDIT;
  m_DefinitionPage.pProcessingPage = &m_ProcessingPage;

  //m_DefinitionPage.pMerchantLimitPage = &m_MerchantLimitPage;	
  //m_DefinitionPage.pTime1Page = &m_Time1Page;
  //m_DefinitionPage.pTime2Page = &m_Time2Page;
  //m_DefinitionPage.pTime3Page = &m_Time3Page;
 // m_DefinitionPage.pCntryAmtLimitsPage = &m_CntryLimitsPage;
  m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
  //m_DefinitionPage.pCardsProfilePage = &m_CardsProfilePage;
  m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
  m_ChangeHistoryPage.m_bChangeHistory = FALSE;

  /*m_MerchantLimitPage.m_bMerchant = FALSE;
  m_MerchantLimitPage.m_nDialogMode = MODE_DONE;*/

  m_ProcessingPage.m_bProcessing = FALSE;
  m_ProcessingPage.m_nDialogMode = MODE_DONE;

 /* m_CardsProfilePage.m_bProcessing = FALSE;
  m_CardsProfilePage.m_nDialogMode = MODE_DONE;

  m_Time1Page.m_bTime1 = FALSE;
  m_Time1Page.m_nDialogMode = MODE_DONE;

  m_Time2Page.m_bTime2 = FALSE;
  m_Time2Page.m_nDialogMode = MODE_DONE;

  m_Time3Page.m_bTime3 = FALSE; 
  m_Time3Page.m_nDialogMode = MODE_DONE;

  m_CntryLimitsPage.m_bCntryAmtLmt = FALSE;
  m_CntryLimitsPage.m_nDialogMode = MODE_DONE;*/
}

BOOL CCardsKeyblockPropertySheet::OnInitDialog()
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