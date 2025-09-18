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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarButton.cpp  $
   
      Rev 1.6   Dec 01 2003 11:28:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:42   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:28   iarustam
    
*
************************************************************************************/

// CalendarButton.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CalendarButton.h"
#include "JobDefDlg.h"
//#include "CalendarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarButton

CCalendarButton::CCalendarButton()
{
  
}

CCalendarButton::~CCalendarButton()
{
}


BEGIN_MESSAGE_MAP(CCalendarButton, CButton)
	//{{AFX_MSG_MAP(CCalendarButton)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarButton message handlers

void CCalendarButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// if the button is blank return else set the focus and 
	// tell the dialog box to set the job for this date.

	CString mytext;
	GetWindowText(mytext);
	if ((" " == mytext) || ("" == mytext))
		return; 	
	SetFocus();
	GetParent()->PostMessage(WM_COMMAND,GetDlgCtrlID()|(NM_RCLICK << 16),(LONG)GetSafeHwnd());
}

//==============================================================================
void CCalendarButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//CString mytext;
	//GetWindowText(mytext);
	//if ((" " == mytext) || ("" == mytext))
	// 	return;
	
	CButton::OnLButtonDown(nFlags,point);

}

//==============================================================================
void CCalendarButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//CString mytext;
	//GetWindowText(mytext);
	//if ((" " == mytext) || ("" == mytext))
  
	//	return;
	
	CButton::OnLButtonUp(nFlags,point);

	GetParent()->PostMessage(WM_COMMAND,GetDlgCtrlID()|(NM_CLICK << 16),(LONG)GetSafeHwnd());

}

