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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeCardFileControlPropertySheet.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:04   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:46   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:52   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:46   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:10   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:12   iarustam
    
*
************************************************************************************/// NegativeCardFileControlPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeCardFileControlPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControlPropertySheet

IMPLEMENT_DYNAMIC(CNegativeCardFileControlPropertySheet, CPropertySheet)

CNegativeCardFileControlPropertySheet::CNegativeCardFileControlPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CNegativeCardFileControlPropertySheet::CNegativeCardFileControlPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);
	AddPage(&m_ChangeHistoryPage);
}

CNegativeCardFileControlPropertySheet::~CNegativeCardFileControlPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CNegativeCardFileControlPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNegativeCardFileControlPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControlPropertySheet message handlers
void CNegativeCardFileControlPropertySheet::SetData()
{
	m_DefinitionPage.m_nDialogMode = MODE_DONE;


	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;

}

BOOL CNegativeCardFileControlPropertySheet::OnInitDialog() 
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
		
	return bResult;
}
