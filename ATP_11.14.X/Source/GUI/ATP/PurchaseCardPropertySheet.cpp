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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PurchaseCardPropertySheet.cpp  $
   
      Rev 1.7   Dec 01 2003 11:30:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:42   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:48   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:18   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:34:08   dperson
   Added PVCS header
*   
************************************************************************************/
// PurchaseCardPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "PurchaseCardPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardPropertySheet

IMPLEMENT_DYNAMIC(CPurchaseCardPropertySheet, CPropertySheet)

CPurchaseCardPropertySheet::CPurchaseCardPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPurchaseCardPropertySheet::CPurchaseCardPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage( &m_DefinitionPage );
   AddPage( &m_GeneralPage );
   AddPage( &m_ChangeHistoryPage );
}

CPurchaseCardPropertySheet::~CPurchaseCardPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CPurchaseCardPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPurchaseCardPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardPropertySheet message handlers
void CPurchaseCardPropertySheet::SetData()
{
	m_DefinitionPage.m_nDialogMode = MODE_DONE;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;	
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_GeneralPage.m_bGeneral = FALSE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CPurchaseCardPropertySheet::OnInitDialog()
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


BOOL CPurchaseCardPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertySheet::OnCommand(wParam, lParam);
}
