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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanPropertySheet.cpp  $
   
      Rev 1.7   Dec 01 2003 11:30:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:40   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:54:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:14   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:54   iarustam
   Bug fixes
   
      Rev 1.1   Apr 09 1999 16:51:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:56   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:34   iarustam
    
*
************************************************************************************/
// PanPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "PanPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanPropertySheet

IMPLEMENT_DYNAMIC(CPanPropertySheet, CPropertySheet)

CPanPropertySheet::CPanPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPanPropertySheet::CPanPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&MainPage);
	MainPage.m_pSheet = this;
	AddPage(&ProcPage);
	AddPage(&DemoPage);
//	AddPage(&TxPage);
	AddPage(&ChHistPage);

	MainPage.pProcPage = &ProcPage;
	MainPage.pDemoPage = &DemoPage;
	MainPage.pTxPage = &TxPage;
	MainPage.pChHistPage = &ChHistPage;
		
}

CPanPropertySheet::~CPanPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CPanPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPanPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanPropertySheet message handlers
void CPanPropertySheet::SetData()
{
	MainPage.DialogMode = MODE_DONE;
	DemoPage.DialogMode = MODE_DONE;
	ProcPage.DialogMode = MODE_DONE;
	TxPage.DialogMode = MODE_DONE;
	TxPage.m_bTxPage = FALSE;
	ChHistPage.m_nDialogMode = MODE_DONE;
	ChHistPage.m_bChangeHistory = FALSE;

}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CPanPropertySheet::OnInitDialog()
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


