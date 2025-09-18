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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardTypePropertySheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:55:54   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:10   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:10   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:08   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:42   iarustam
    
*
************************************************************************************/
// CardTypePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardTypePropertySheet.h"
//#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardTypePropertySheet

IMPLEMENT_DYNAMIC(CCardTypePropertySheet, CPropertySheet)

CCardTypePropertySheet::CCardTypePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCardTypePropertySheet::CCardTypePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
     //add pages to property sheet in order they have to appear
	 AddPage(&m_DefinitionPage);
     AddPage(&m_DescriptionPage);
     AddPage(&m_ChangeHistoryPage);
}

CCardTypePropertySheet::~CCardTypePropertySheet()
{
}

BEGIN_MESSAGE_MAP(CCardTypePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCardTypePropertySheet)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardTypePropertySheet message handlers
/******************************************************************************
 *
 * MODULE:	    OnInitDialog
 *
 * DESCRIPTION: This function performs special initialization processing and
 *              sets all the values to default.
 *              
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

void CCardTypePropertySheet::SetData()
{
	//set default values
	m_DefinitionPage.pDescriptionPage = &m_DescriptionPage;	
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_DescriptionPage.m_bDescription = FALSE;
    m_DescriptionPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_CARD;	
}

BOOL CCardTypePropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	if ( m_nActiveTab == 0 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_CARD_DEFINITION);
	if ( m_nActiveTab == 1 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_CARD_TYPE_DESCRIPTION);	
	if ( m_nActiveTab == 20 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_CHANGE_HISTORY);
*/
	return TRUE;
}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CCardTypePropertySheet::OnInitDialog()
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
