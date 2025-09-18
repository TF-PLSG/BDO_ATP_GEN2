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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinPropertySheet.cpp  $
   
      Rev 1.9   Apr 01 2005 10:13:30   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.8   Dec 01 2003 11:28:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jul 10 2002 09:55:42   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.6   Oct 16 2001 15:20:34   dirby
   Added blocking of a bin by Merchant Category Code (MCC)     SCR 361
   
      Rev 1.5   Apr 17 2000 09:56:54   iarustam
   added bin blocking
   
   
      Rev 1.4   Jan 05 2000 14:52:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:41:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:58   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:14   iarustam
    
*
************************************************************************************/

// BinPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "BinPropertySheet.h"
//#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


MIDBINBLK01	sMidBinBlkRecord_Global = {0};
MIDBINBLK01	sOldMidBinBlkRecord_Global = {0};
/////////////////////////////////////////////////////////////////////////////
// CBinPropertySheet

IMPLEMENT_DYNAMIC(CBinPropertySheet, CPropertySheet)

CBinPropertySheet::CBinPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CBinPropertySheet::CBinPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_DefinitionPage);
    AddPage (&m_GeneralPage);
    AddPage (&m_BlockingPage); 
	AddPage (&m_MCCBlockingPage);
	AddPage (&m_MidBlockingPage);
    AddPage (&m_ChangeHistoryPage);
}

CBinPropertySheet::~CBinPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CBinPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CBinPropertySheet)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinPropertySheet message handlers

void CBinPropertySheet::SetData()
{
   m_DefinitionPage.pGeneralPage = &m_GeneralPage;
   m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
   m_DefinitionPage.pBlockingPage = &m_BlockingPage;
   m_DefinitionPage.pMccBlockingPage = &m_MCCBlockingPage;
   m_DefinitionPage.pMidBlockingPage = &m_MidBlockingPage;			

	m_GeneralPage.m_nDialogMode = MODE_DONE;	
	m_GeneralPage.m_bGeneral = FALSE;

	m_BlockingPage.m_nDialogMode = MODE_DONE;	
	m_BlockingPage.m_bBlocking = FALSE;

	m_MCCBlockingPage.m_nDialogMode = MODE_DONE;	
	m_MCCBlockingPage.m_bMCCBlocking = FALSE;

	m_MidBlockingPage.m_nDialogMode = MODE_DONE;	
	m_MidBlockingPage.m_bBlocking = FALSE;

	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
	m_ChangeHistoryPage.m_nPropertySheetMode = MODE_BIN;
	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	
}

BOOL CBinPropertySheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	if ( m_nActiveTab == 0 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_BIN_DEFINITION);
	if ( m_nActiveTab == 1 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_BIN_GENERAL);	
	if ( m_nActiveTab == 20 )
		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"bin.chm>bin",HH_HELP_CONTEXT,IDD_CHANGE_HISTORY);
	
*/	
	return TRUE;
}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CBinPropertySheet::OnInitDialog()
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