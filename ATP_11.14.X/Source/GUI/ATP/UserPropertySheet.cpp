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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserPropertySheet.cpp  $

     Rev 1.7   Jan 01 2011 11:30:22  
    Added ucf01 table for unlocking user account from GUI-TF-Srikanth
*
      Rev 1.6   Dec 01 2003 11:30:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:38   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:06   iarustam
    
*
************************************************************************************/
// UserPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "UserPropertySheet.h"
//#include "htmlhelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUserAccess* pAccessPage;
extern CUserWorkstation* pWorkstationPage;
extern CUserAccountStatus* pAccountStatusPage;




/////////////////////////////////////////////////////////////////////////////
// CUserPropertySheet

IMPLEMENT_DYNAMIC(CUserPropertySheet, CPropertySheet)

CUserPropertySheet::CUserPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CUserPropertySheet::CUserPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   
	AddPage(&m_DefinitionPage);
    AddPage(&m_AccessPage);
    AddPage(&m_WorkstationPage);
	AddPage(&m_UserAccountStatusPage);
    AddPage(&m_ChangeHistoryPage);	
	SetData ();

	
    
}

CUserPropertySheet::~CUserPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CUserPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CUserPropertySheet)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserPropertySheet message handlers
void CUserPropertySheet::SetData()
{
	pAccessPage = &m_AccessPage;
	pAccessPage->m_bPopulate = FALSE;
	m_AccessPage.m_bAccess = FALSE;
	m_AccessPage.m_bGroup = FALSE;
	m_AccessPage.m_nDialogMode = MODE_DONE;

	pAccountStatusPage = &m_UserAccountStatusPage;
	m_UserAccountStatusPage.m_bStatus=FALSE;
	m_UserAccountStatusPage.m_bPopulate=FALSE;
	//m_UserAccountStatusPage.m_bUAccStatus=FALSE;
	m_UserAccountStatusPage.m_nDialogMode = MODE_DONE;

	pWorkstationPage = &m_WorkstationPage;
	m_WorkstationPage.m_bWorkstation = FALSE;
	m_WorkstationPage.m_bGroup = FALSE;
	m_WorkstationPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_USER;


	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
}



BOOL CUserPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	if ( m_nActiveTab == 0 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_USER_DEFINITON);
	if ( m_nActiveTab == 1 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_USER_ACCESS);
	if ( m_nActiveTab == 2 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_USER_WORKSTATION);
	if ( m_nActiveTab == 20 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_CHANGE_HISTORY);
*/	
	return TRUE;
}

BOOL CUserPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);
	//raghavendra
	GetPage(0)->SetModified();              //Enable the Apply Button
	SendMessage(DM_SETDEFID, ID_APPLY_NOW);

	return bResult;
}
