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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredPropertySheet.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:06   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:26   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:17:24   dperson
   Added PVCS header
*   
************************************************************************************/
// DeferredPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DeferredPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeferredPropertySheet

IMPLEMENT_DYNAMIC(CDeferredPropertySheet, CPropertySheet)

CDeferredPropertySheet::CDeferredPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CDeferredPropertySheet::CDeferredPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   AddPage( &m_DefinitionPage );
   AddPage( &m_GeneralPage );
   AddPage( &m_ChangeHistoryPage );

}

CDeferredPropertySheet::~CDeferredPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CDeferredPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDeferredPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeferredPropertySheet message handlers

void CDeferredPropertySheet::SetData()
{
	m_DefinitionPage.m_nDialogMode = MODE_DONE;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;	
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;

	m_GeneralPage.m_bGeneral = FALSE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CDeferredPropertySheet::OnInitDialog()
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
