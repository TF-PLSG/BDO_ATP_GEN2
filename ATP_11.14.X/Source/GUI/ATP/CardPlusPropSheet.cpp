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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardPlusPropSheet.cpp  $
   
      Rev 1.7   Dec 01 2003 11:28:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:55:48   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:06   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:00:52   dperson
   Added PVCS header
*   
************************************************************************************/
// CardPlusPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardPlusPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardPlusPropSheet

IMPLEMENT_DYNAMIC(CCardPlusPropSheet, CPropertySheet)

CCardPlusPropSheet::CCardPlusPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCardPlusPropSheet::CCardPlusPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
     //add pages to property sheet in order they have to appear
     AddPage(&m_DataPage);
	  AddPage(&m_MainPage);
	  m_MainPage.m_pCPSheet = this;

     m_MainPage.pDataPage = &m_DataPage;

}

CCardPlusPropSheet::~CCardPlusPropSheet()
{
}


BEGIN_MESSAGE_MAP(CCardPlusPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCardPlusPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardPlusPropSheet message handlers

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CCardPlusPropSheet::OnInitDialog()
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
