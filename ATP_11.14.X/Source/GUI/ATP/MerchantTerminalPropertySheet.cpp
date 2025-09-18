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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalPropertySheet.cpp  $
   
      Rev 1.7   Jul 16 2004 16:07:44   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.6   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:28   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:00   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:56   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:06   iarustam
    
*
************************************************************************************/
// MerchantTerminalPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPropertySheet

IMPLEMENT_DYNAMIC(CMerchantTerminalPropertySheet, CPropertySheet)

CMerchantTerminalPropertySheet::CMerchantTerminalPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMerchantTerminalPropertySheet::CMerchantTerminalPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_DefinitionPage);
    AddPage (&m_GeneralPage);
    AddPage (&m_CurrentBatchPage); 
	AddPage (&m_CurrentCUPBatchPage);
    AddPage (&m_BatchHistoryPage);
    AddPage (&m_TranPage);   
    AddPage (&m_KeysPage);
	AddPage (&m_AutoInitPage);
    AddPage (&m_ChangeHistoryPage);


    //AddPage (&m_CapabilitiesPage);
}

CMerchantTerminalPropertySheet::~CMerchantTerminalPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CMerchantTerminalPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPropertySheet message handlers
void CMerchantTerminalPropertySheet::SetData()
{
	m_DefinitionPage.m_bDefinition = FALSE;
	m_DefinitionPage.m_nDialogMode = MODE_DONE;

	m_GeneralPage.m_bGeneral = FALSE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;

	m_CurrentBatchPage.m_bCurrentBatch = FALSE;
	m_CurrentBatchPage.m_nDialogMode = MODE_DONE;

	m_CurrentCUPBatchPage.m_bCurrentBatch = FALSE;
	m_CurrentCUPBatchPage.m_nDialogMode = MODE_DONE;

	m_BatchHistoryPage.m_bBatchHistory = FALSE;
	m_BatchHistoryPage.m_nDialogMode = MODE_DONE;

	m_KeysPage.m_bKeys = FALSE;
	m_KeysPage.m_nDialogMode = MODE_DONE;

	m_TranPage.m_bTran = FALSE;
	m_TranPage.m_nDialogMode = MODE_DONE;

	m_AutoInitPage.m_bAutoInit = FALSE;
	m_AutoInitPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;
	m_DefinitionPage.pCurrentPage = &m_CurrentBatchPage;
	m_DefinitionPage.pCUPCurrentPage = &m_CurrentCUPBatchPage;
	m_DefinitionPage.pBatchHistoryPage = &m_BatchHistoryPage;
	m_DefinitionPage.pKeysPage = &m_KeysPage;
	m_DefinitionPage.pTranPage = &m_TranPage;
	m_DefinitionPage.pAutoInitPage = &m_AutoInitPage;


}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CMerchantTerminalPropertySheet::OnInitDialog()
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
