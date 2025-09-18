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
    
*
************************************************************************************/
// TransactionDetailsPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "TransactionDetailsPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetailsPropertySheet

IMPLEMENT_DYNAMIC(CTransactionDetailsPropertySheet, CPropertySheet)

CTransactionDetailsPropertySheet::CTransactionDetailsPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTransactionDetailsPropertySheet::CTransactionDetailsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	//AddPage (&m_TranDetailsPage);
    //AddPage (&m_FraudDetailsPage);
	/*AddPage (&m_AddlTranDetailsPage);
	AddPage (&m_VEDQPTranDetailsPage);*/

    //AddPage (&m_CapabilitiesPage);
}

CTransactionDetailsPropertySheet::~CTransactionDetailsPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CTransactionDetailsPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetailsPropertySheet message handlers
void CTransactionDetailsPropertySheet::SetData()
{
	//m_TranDetailsPage.pFraudDetailsPage = &m_FraudDetailsPage;



}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CTransactionDetailsPropertySheet::OnInitDialog()
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
	
/*
	GetPage(0)->SetModified();              //Enable the Apply Button
	SendMessage(DM_SETDEFID, ID_APPLY_NOW); //Set the Apply button as the default button(instead of OK button
*/
	return TRUE;
}
