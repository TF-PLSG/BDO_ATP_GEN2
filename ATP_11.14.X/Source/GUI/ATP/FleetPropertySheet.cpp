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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\FleetPropertySheet.cpp  $
   
      Rev 1.9   Apr 08 2005 08:47:26   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.8   Apr 01 2005 10:13:48   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.7   Dec 01 2003 11:29:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:10   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:49:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:28   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:32   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:22   iarustam
   Bug fixes
   
      Rev 1.1   Mar 30 1999 17:39:26   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:26   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:58   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:22   iarustam
    
*
************************************************************************************/
// FleetPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "FleetPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFleetPropertySheet

IMPLEMENT_DYNAMIC(CFleetPropertySheet, CPropertySheet)

CFleetPropertySheet::CFleetPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CFleetPropertySheet::CFleetPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
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

CFleetPropertySheet::~CFleetPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CFleetPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFleetPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFleetPropertySheet message handlers

void CFleetPropertySheet::SetData()
{
  m_DefinitionPage.m_nCardMode = MODE_FLEET;
  m_DefinitionPage.pProcessingPage = &m_ProcessingPage;
  m_DefinitionPage.pCardsProfilePage = &m_CardsProfilePage;
  m_DefinitionPage.pMerchantLimitPage = &m_MerchantLimitPage;	
  m_DefinitionPage.pTime1Page = &m_Time1Page;
  m_DefinitionPage.pTime2Page = &m_Time2Page;
  m_DefinitionPage.pTime3Page = &m_Time3Page;
  m_DefinitionPage.pVelocityPage = &m_VelocityPage;
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

  m_VelocityPage.m_bVelocity = FALSE;
  m_VelocityPage.m_nDialogMode = MODE_DONE;

}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CFleetPropertySheet::OnInitDialog()
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

