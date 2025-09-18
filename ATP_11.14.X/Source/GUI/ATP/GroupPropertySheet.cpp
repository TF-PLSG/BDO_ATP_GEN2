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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\GroupPropertySheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:12   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:58:34   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:22   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:28   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:28   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:48   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:04   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 16:42:26   iarustam
   bug fixes
   
      Rev 1.2   Jan 13 1999 14:20:00   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:43:26   iarustam
    
*
************************************************************************************/
// GroupPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "GroupPropertySheet.h"
//#include "htmlhelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupPropertySheet

IMPLEMENT_DYNAMIC(CGroupPropertySheet, CPropertySheet)

CGroupPropertySheet::CGroupPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CGroupPropertySheet::CGroupPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_DefinitionPage);
    AddPage (&m_AccessPage);
    AddPage (&m_WorkstationPage);
    AddPage (&m_ChangeHistoryPage);

	

}

CGroupPropertySheet::~CGroupPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CGroupPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CGroupPropertySheet)
	ON_WM_CREATE()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupPropertySheet message handlers
void CGroupPropertySheet::SetData()
{
	m_DefinitionPage.pAccessPage = &m_AccessPage;
	m_DefinitionPage.pWorkstationPage = &m_WorkstationPage;
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.pAccessPage->m_bPopulate = FALSE;

	m_WorkstationPage.m_bWorkstation = FALSE;
	m_WorkstationPage.m_nDialogMode = MODE_DONE;

	m_AccessPage.m_bAccess = FALSE;
	m_AccessPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_GROUP;

}


BOOL CGroupPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{    
/*	if ( m_nActiveTab == 0 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_GROUP_DEFINITION);
	if ( m_nActiveTab == 1 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_GROUPS_ACCESS);
	if ( m_nActiveTab == 2 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_GROUPS_WORKSTATION);
	if ( m_nActiveTab == 20 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_CHANGE_HISTORY);
	
	*/
	return TRUE;
}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CGroupPropertySheet::OnInitDialog()
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
