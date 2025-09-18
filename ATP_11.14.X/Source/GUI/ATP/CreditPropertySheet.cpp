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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CreditPropertySheet.cpp  $
   
      Rev 1.7   Apr 01 2005 10:13:44   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.6   Dec 01 2003 11:29:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:02   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:22   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:06   iarustam
    
*
************************************************************************************/
// CreditPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CreditPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreditPropertySheet

IMPLEMENT_DYNAMIC(CCreditPropertySheet, CPropertySheet)

CCreditPropertySheet::CCreditPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCreditPropertySheet::CCreditPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_DefinitionPage);	
	AddPage(&m_ProcessingPage);
	AddPage(&m_CardsProfilePage);
    AddPage(&m_MerchantLimitPage);
	AddPage(&m_Time1Page);
	AddPage(&m_Time2Page);
	AddPage(&m_Time3Page);
	AddPage(&m_CntryLimitsPage);
	AddPage(&m_ChangeHistoryPage);
}

CCreditPropertySheet::~CCreditPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CCreditPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCreditPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreditPropertySheet message handlers

void CCreditPropertySheet::SetData()
{
  m_DefinitionPage.m_nCardMode = MODE_CREDIT;
  m_DefinitionPage.pProcessingPage = &m_ProcessingPage;
  m_DefinitionPage.pMerchantLimitPage = &m_MerchantLimitPage;	
  m_DefinitionPage.pTime1Page = &m_Time1Page;
  m_DefinitionPage.pTime2Page = &m_Time2Page;
  m_DefinitionPage.pTime3Page = &m_Time3Page;
  m_DefinitionPage.pCntryAmtLimitsPage = &m_CntryLimitsPage;
  m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
  m_DefinitionPage.pCardsProfilePage = &m_CardsProfilePage;
  m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
  m_ChangeHistoryPage.m_bChangeHistory = FALSE;

  m_MerchantLimitPage.m_bMerchant = FALSE;
  m_MerchantLimitPage.m_nDialogMode = MODE_DONE;

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

  m_CntryLimitsPage.m_bCntryAmtLmt = FALSE;
  m_CntryLimitsPage.m_nDialogMode = MODE_DONE;


}

BOOL CCreditPropertySheet::OnInitDialog()
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