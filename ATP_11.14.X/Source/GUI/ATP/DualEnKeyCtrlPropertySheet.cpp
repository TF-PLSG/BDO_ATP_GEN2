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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DualEnKeyCtrlPropertySheet.cpp  $
   
      Rev 1.0   Feb 02 2011 11:30:22   TF_Srikanth
   Added DualEncrKeyControl Class for Changing Encryption Key in Database from GUI  
    
*
************************************************************************************/
// DualEnKeyCtrlPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DualEnKeyCtrlPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDualEnKeyCtrlPropertySheet

IMPLEMENT_DYNAMIC(CDualEnKeyCtrlPropertySheet, CPropertySheet)

CDualEnKeyCtrlPropertySheet::CDualEnKeyCtrlPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CDualEnKeyCtrlPropertySheet::CDualEnKeyCtrlPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DualEncrKeyControl);
}

CDualEnKeyCtrlPropertySheet::~CDualEnKeyCtrlPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CDualEnKeyCtrlPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDualEnKeyCtrlPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDualEnKeyCtrlPropertySheet message handlers

BOOL CDualEnKeyCtrlPropertySheet::OnInitDialog() 
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
