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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkAcquirerPropertySheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:34   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:06   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:50   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:50   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:20   iarustam
    
*
************************************************************************************/
// NetworkKeyblockPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkKeyblockPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkKeyblockPropertySheet

IMPLEMENT_DYNAMIC(CNetworkKeyblockPropertySheet, CPropertySheet)

CNetworkKeyblockPropertySheet::CNetworkKeyblockPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CNetworkKeyblockPropertySheet::CNetworkKeyblockPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_DefinitionPage);
	AddPage(&m_GeneralPage);
/*	AddPage(&m_TotalsPage);	
	AddPage(&m_ProcessingPage);
	AddPage(&m_LogonPage);*/
	AddPage(&m_ChangeHistoryPage);
}

CNetworkKeyblockPropertySheet::~CNetworkKeyblockPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CNetworkKeyblockPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNetworkKeyblockPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkKeyblockPropertySheet message handlers

void CNetworkKeyblockPropertySheet::SetData()
{
    //m_DefinitionPage.m_nNetworkMode = MODE_ACQUIRER;
	m_DefinitionPage.pGeneralPage = &m_GeneralPage;
	//m_DefinitionPage.pTotalsPage = &m_TotalsPage;
	//m_DefinitionPage.pProcessingPage = &m_ProcessingPage;
	//m_DefinitionPage.pLogonPage = &m_LogonPage;
	m_DefinitionPage.pChangeHistoryPage = &m_ChangeHistoryPage;
	m_DefinitionPage.m_nDialogMode = MODE_DONE;
	m_GeneralPage.m_nDialogMode = MODE_DONE;
	/*m_TotalsPage.m_nDialogMode = MODE_DONE;
	m_ProcessingPage.m_nDialogMode = MODE_DONE;
	m_LogonPage.m_nDialogMode = MODE_DONE;*/
	m_ChangeHistoryPage.m_nDialogMode = MODE_ADD;
	m_GeneralPage.m_bGeneral = FALSE;
	//m_TotalsPage.m_bTotals = FALSE;
	//m_ProcessingPage.m_bProcessing = FALSE;
	//m_LogonPage.m_bLogon = FALSE;
	m_ChangeHistoryPage.m_bChangeHistory = FALSE;
}

/*07092002 LM Added to set the apply button as the default.  
This is to allow the enter/return to apply data but not exit back to the main menu*/
BOOL CNetworkKeyblockPropertySheet::OnInitDialog()
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
