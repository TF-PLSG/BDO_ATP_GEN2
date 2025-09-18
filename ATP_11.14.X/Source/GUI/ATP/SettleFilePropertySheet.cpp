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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SettleFilePropertySheet.cpp  $
   
      Rev 1.5   Dec 01 2003 11:30:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:24   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:26   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:06   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:54   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:50   iarustam
    
*
************************************************************************************/
// SettleFilePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "SettleFilePropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettleFilePropertySheet

IMPLEMENT_DYNAMIC(CSettleFilePropertySheet, CPropertySheet)

CSettleFilePropertySheet::CSettleFilePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSettleFilePropertySheet::CSettleFilePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_DefinitionPage);
	AddPage (&m_PrefixTimePage);
	AddPage (&m_ChangeHistoryPage);
}

CSettleFilePropertySheet::~CSettleFilePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CSettleFilePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSettleFilePropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettleFilePropertySheet message handlers

BOOL CSettleFilePropertySheet::OnInitDialog() 
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
