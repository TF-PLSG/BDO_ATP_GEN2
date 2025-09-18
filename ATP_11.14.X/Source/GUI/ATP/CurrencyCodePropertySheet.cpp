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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\RelatedPropertySheet.cpp  $
   
      Rev 1.7   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:44   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:54:18   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:20   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:37:14   dperson
   Added PVCS header
*   
************************************************************************************/
// D:\TF\ATP_11.10\Source\GUI\ATP\CurrencyCodePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CurrencyCodePropertySheet.h"
#include "CurrencyCodeDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CurrencyCodePropertySheet

IMPLEMENT_DYNAMIC(CurrencyCodePropertySheet, CPropertySheet)

CurrencyCodePropertySheet::CurrencyCodePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CurrencyCodePropertySheet::CurrencyCodePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage( &m_CurrencyCodeParameterPage );
   AddPage( &m_ChangeHistoryPage );
}

CurrencyCodePropertySheet::~CurrencyCodePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CurrencyCodePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CRelatedPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelatedPropertySheet message handlers


void CurrencyCodePropertySheet::SetData()
{
	m_CurrencyCodeParameterPage.m_nDialogMode = MODE_DONE;
	m_CurrencyCodeParameterPage.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_OMC01;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

}


/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CurrencyCodePropertySheet::OnInitDialog()
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


