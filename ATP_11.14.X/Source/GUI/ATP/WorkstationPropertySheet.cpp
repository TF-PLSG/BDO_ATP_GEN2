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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationPropertySheet.cpp  $
   
      Rev 1.5   Dec 01 2003 11:30:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:44   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:16   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:34   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:20   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:14   iarustam
    
*
************************************************************************************/
// WorkstationPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "WorkstationPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkstationPropertySheet

IMPLEMENT_DYNAMIC(CWorkstationPropertySheet, CPropertySheet)

CWorkstationPropertySheet::CWorkstationPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CWorkstationPropertySheet::CWorkstationPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage( &m_WorkstationDefinition );
   AddPage( &m_WorkstationDescription );
   AddPage( &m_ChangeHistoryPage );
}

CWorkstationPropertySheet::~CWorkstationPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CWorkstationPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWorkstationPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkstationPropertySheet message handlers
void CWorkstationPropertySheet::SetData()
{
	m_WorkstationDefinition.pDescriptionPage = &m_WorkstationDescription;	
	m_WorkstationDescription.m_bDescription = FALSE;
	m_WorkstationDefinition.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_WORKSTATION;
}

BOOL CWorkstationPropertySheet::OnInitDialog() 
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
