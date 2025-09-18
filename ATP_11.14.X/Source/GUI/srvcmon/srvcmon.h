/****************************************************************************************

   Module: srvcmon.h

   Title: Pinnacle Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\srvcmon.h  $
   
      Rev 1.0   27 May 1998 16:54:24   rmalathk
   Initial Revision
   
*****************************************************************************************/
// srvcmon.h : main header file for the SRVCMON application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonApp:
// See srvcmon.cpp for the implementation of this class
//

class CSrvcmonApp : public CWinApp
{
public:
	CSrvcmonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSrvcmonApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSrvcmonApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
