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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarButton.h  $
   
      Rev 1.6   Dec 01 2003 11:28:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:30   iarustam
    
*
************************************************************************************/
// CalendarButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalendarButton window

#ifndef _CalendarButton_

#define _CalendarButton_

class CCalendarButton : public CButton
{
// Construction
public:
	CCalendarButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCalendarButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarButton)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif

/////////////////////////////////////////////////////////////////////////////
