// TPSR_GUI.h : main header file for the TPSR_GUI application
//

#if !defined(AFX_TPSR_GUI_H__3B8C098C_F3DF_428B_B37F_71DC0ECA5062__INCLUDED_)
#define AFX_TPSR_GUI_H__3B8C098C_F3DF_428B_B37F_71DC0ECA5062__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTPSR_GUIApp:
// See TPSR_GUI.cpp for the implementation of this class
//

class CTPSR_GUIApp : public CWinApp
{
public:
	CTPSR_GUIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPSR_GUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTPSR_GUIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPSR_GUI_H__3B8C098C_F3DF_428B_B37F_71DC0ECA5062__INCLUDED_)
