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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantPropertySheet.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jul 10 2002 09:56:24   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.6   Dec 16 1999 09:34:04   mbysani
   corrected some error based on error reports
   
      Rev 1.3   Sep 30 1999 16:17:26   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:53:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:52   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:56:36   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:32:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:44   iarustam
    
*
************************************************************************************/
// MerchantPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantPropertySheet

IMPLEMENT_DYNAMIC(CMerchantPropertySheet, CPropertySheet)

CMerchantPropertySheet::CMerchantPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMerchantPropertySheet::CMerchantPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);
    AddPage(&m_LocationPage);
    AddPage(&m_CapabilitiesPage);
    AddPage(&m_GeneralPage);
   //AddPage(&m_DebitCardPage);
    AddPage(&m_VelocityPage);
    AddPage(&m_ContactPage);
    AddPage(&m_BlockingPage);
	AddPage(&m_SettlementPage);
	AddPage(&m_ChangeHistoryPage);
  //  AddPage(&m_TerminalPage);

    
    
  
}

CMerchantPropertySheet::~CMerchantPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CMerchantPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantPropertySheet message handlers

void CMerchantPropertySheet::SetData()
{
	m_LocationPage.m_bLocation = FALSE;
	m_LocationPage.m_nDialogMode = MODE_DONE;

	m_BlockingPage.m_bBlocking = FALSE;
	m_BlockingPage.m_nDialogMode = MODE_DONE;

	m_CapabilitiesPage.m_bCapabilities = FALSE;
	m_CapabilitiesPage.m_nDialogMode = MODE_DONE;

	m_ContactPage.m_bContact = FALSE;
	m_ContactPage.m_nDialogMode = MODE_DONE;

	m_VelocityPage.m_bVelocity = FALSE;
	m_VelocityPage.m_nDialogMode = MODE_DONE;

	m_SettlementPage.m_bSettlement = FALSE;
	m_SettlementPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_GeneralPage.m_bGeneral = FALSE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;

	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.pLocationPage = &m_LocationPage;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;
	m_DefinitionPage.pBlockingPage = &m_BlockingPage;
	m_DefinitionPage.pCapabilitiesPage = &m_CapabilitiesPage;
	m_DefinitionPage.pContactPage = &m_ContactPage;
	m_DefinitionPage.pVelocityPage = &m_VelocityPage;
	m_DefinitionPage.pSettlementPage = &m_SettlementPage;
	m_DefinitionPage.m_nDialogMode = MODE_DONE;

}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CMerchantPropertySheet::OnInitDialog()
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

