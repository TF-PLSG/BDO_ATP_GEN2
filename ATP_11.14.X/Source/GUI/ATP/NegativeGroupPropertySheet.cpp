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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeGroupPropertySheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:32   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:06   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:54   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:18   iarustam
    
*
************************************************************************************/
// NegativeGroupPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeGroupPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNegativeGroupPropertySheet

IMPLEMENT_DYNAMIC(CNegativeGroupPropertySheet, CPropertySheet)

CNegativeGroupPropertySheet::CNegativeGroupPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CNegativeGroupPropertySheet::CNegativeGroupPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage (&m_DefinitionPage);
  //  AddPage (&m_PointsPage);   
    AddPage (&m_GeneralPage); 
    AddPage (&m_ChangeHistoryPage);

}

CNegativeGroupPropertySheet::~CNegativeGroupPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CNegativeGroupPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNegativeGroupPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeGroupPropertySheet message handlers
void CNegativeGroupPropertySheet::SetData()
{
	m_DefinitionPage.m_nDialogMode = MODE_DONE;

	m_GeneralPage.m_bGeneral = FALSE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;

	m_ChangeHistoryPage.m_nDialogMode = MODE_DONE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;

	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;


}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CNegativeGroupPropertySheet::OnInitDialog()
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
