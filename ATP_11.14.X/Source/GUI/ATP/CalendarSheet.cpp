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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarSheet.cpp  $
   
      Rev 1.6   Dec 01 2003 11:28:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:32   iarustam
    
*
************************************************************************************/

// CalendarSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CalendarSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarSheet

IMPLEMENT_DYNAMIC(CCalendarSheet, CPropertySheet)

CCalendarSheet::CCalendarSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCalendarSheet::CCalendarSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_calendarpage);
}

CCalendarSheet::~CCalendarSheet()
{
}


BEGIN_MESSAGE_MAP(CCalendarSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCalendarSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////




BOOL CCalendarSheet::OnInitDialog() 
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
