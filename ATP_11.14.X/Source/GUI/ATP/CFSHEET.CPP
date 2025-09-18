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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFSHEET.CPP  $
   
      Rev 1.7   Dec 01 2003 11:29:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:00   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:00   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:55:54   iarustam
   Bug fixes
   
      Rev 1.1   Apr 09 1999 16:50:34   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:48   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:48   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:41:50   iarustam
    
*
************************************************************************************/
// CFSheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCFSheet

IMPLEMENT_DYNAMIC(CCFSheet, CPropertySheet)

CCFSheet::CCFSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCFSheet::CCFSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&MainPage);
	MainPage.m_pSheet = this;
	AddPage(&GenPage);
	AddPage(&BalPage);
	AddPage(&CapPage);
	AddPage(&DemogrPage);
	AddPage(&ActivPage);
	AddPage(&MerchUsPage);
//	AddPage(&TxPage);
	AddPage(&ChHistPage);

	MainPage.pGenPage = &GenPage;
	MainPage.pCapPage = &CapPage;
	MainPage.pBalPage = &BalPage;
	MainPage.pDemogrPage = &DemogrPage;
	MainPage.pActivPage = &ActivPage;
	MainPage.pMerchUsPage = &MerchUsPage;
	MainPage.pTxPage = &TxPage;
	MainPage.pChHistPage = &ChHistPage;
}

CCFSheet::~CCFSheet()
{
}


BEGIN_MESSAGE_MAP(CCFSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCFSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFSheet message handlers
void CCFSheet::SetData()
{
	MainPage.DialogMode = MODE_DONE;
	MainPage.m_strCardNbr.Empty();
	MainPage.m_strCardType.Empty();

	GenPage.DialogMode = MODE_DONE;
	
	BalPage.DialogMode = MODE_DONE;
	
	CapPage.DialogMode = MODE_DONE;
	
	DemogrPage.DialogMode = MODE_DONE;
	DemogrPage.m_bCCF01 = FALSE;
	DemogrPage.m_bDemogrChanged = FALSE;
	
	ActivPage.DialogMode = MODE_DONE;
	
	MerchUsPage.DialogMode = MODE_DONE;
	MerchUsPage.m_bMerchUsage = FALSE;
	MerchUsPage.m_bCCF03 = FALSE;

	TxPage.DialogMode = MODE_DONE;
	TxPage.m_bTxPage = FALSE;
	
	ChHistPage.m_nDialogMode = MODE_DONE;
	ChHistPage.m_bChangeHistory = FALSE;

}
/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/

BOOL CCFSheet::OnInitDialog()
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
