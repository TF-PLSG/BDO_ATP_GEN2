// DBFToolWindows.h : main header file for the DBFTOOLWINDOWS application
//

#if !defined(AFX_DBFTOOLWINDOWS_H__8D9B53D4_95A6_4D36_8A02_4B722F96C9FC__INCLUDED_)
#define AFX_DBFTOOLWINDOWS_H__8D9B53D4_95A6_4D36_8A02_4B722F96C9FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDBFToolWindowsApp:
// See DBFToolWindows.cpp for the implementation of this class
//

class CDBFToolWindowsApp : public CWinApp
{
public:
	CDBFToolWindowsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFToolWindowsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDBFToolWindowsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBFTOOLWINDOWS_H__8D9B53D4_95A6_4D36_8A02_4B722F96C9FC__INCLUDED_)
