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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DialogPropertySheet.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:49:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:24   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:30   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:22   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:52   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:16   iarustam
    
*
************************************************************************************/
// DialogPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "DialogPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPropertySheet

IMPLEMENT_DYNAMIC(CDialogPropertySheet, CPropertySheet)

CDialogPropertySheet::CDialogPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CDialogPropertySheet::CDialogPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage( &m_DialogCommSetup );
   AddPage( &m_DialogMain );
   AddPage( &m_DialogConfig );
}

CDialogPropertySheet::~CDialogPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CDialogPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDialogPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPropertySheet message handlers

BOOL CDialogPropertySheet::OnInitDialog() 
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
