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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarSheet.h  $
   
      Rev 1.6   Dec 01 2003 11:28:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:34   iarustam
    
*
************************************************************************************/
// CalendarSheet.h : header file
//
#include "CalendarPage.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarSheet

class CCalendarSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCalendarSheet)

// Construction
public:
	CCalendarSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCalendarSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CCalendarPage m_calendarpage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCalendarSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarSheet)
	//}}AFX_MSG
    

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
