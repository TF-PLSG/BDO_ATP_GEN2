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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ConversionPropertySheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:02   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:20   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:38   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:04   iarustam
    
*
************************************************************************************/
// ConversionPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "ConversionPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConversionPropertySheet

IMPLEMENT_DYNAMIC(CConversionPropertySheet, CPropertySheet)

CConversionPropertySheet::CConversionPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CConversionPropertySheet::CConversionPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{	
    AddPage(&m_CurrencyPage);
	AddPage(&m_ConversionPage);
    AddPage(&m_ChangeHistoryPage);
}

CConversionPropertySheet::~CConversionPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CConversionPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CConversionPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConversionPropertySheet message handlers
void CConversionPropertySheet::SetData()
{
	m_CurrencyPage.pConversionRatePage = &m_ConversionPage;	
	m_CurrencyPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
//	m_DefinitionPage.pDescriptionPage->m_bPopulate = FALSE;	
	m_ConversionPage.m_bConversion = FALSE;
	m_ConversionPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
}


/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CConversionPropertySheet::OnInitDialog()
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
