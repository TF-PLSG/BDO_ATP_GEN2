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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DBPasswordsPropertySheet.cpp  $
   
      Rev 1.0   Jan 01 2011 11:30:22   TF_Srikanth
   Added DBPasswordsPropertySheet for Storing Dualencryption key control Class    
    
*
************************************************************************************/
// DBPasswordsPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DBPasswordsPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBPasswordsPropertySheet

IMPLEMENT_DYNAMIC(CDBPasswordsPropertySheet, CPropertySheet)

CDBPasswordsPropertySheet::CDBPasswordsPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CDBPasswordsPropertySheet::CDBPasswordsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DBPasswordsPage);
}

CDBPasswordsPropertySheet::~CDBPasswordsPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CDBPasswordsPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDBPasswordsPropertySheet)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBPasswordsPropertySheet message handlers

BOOL CDBPasswordsPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}


BOOL CDBPasswordsPropertySheet::OnInitDialog() 
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
