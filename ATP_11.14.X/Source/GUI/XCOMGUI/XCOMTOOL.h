// XCOMTOOL.h : main header file for the XCOMTOOL application
//

#if !defined(AFX_XCOMTOOL_H__4B5C53DF_9A4C_4F7D_B29B_C4F7410B6E99__INCLUDED_)
#define AFX_XCOMTOOL_H__4B5C53DF_9A4C_4F7D_B29B_C4F7410B6E99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXCOMTOOLApp:
// See XCOMTOOL.cpp for the implementation of this class
//

class CXCOMTOOLApp : public CWinApp
{
public:
	CXCOMTOOLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXCOMTOOLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXCOMTOOLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XCOMTOOL_H__4B5C53DF_9A4C_4F7D_B29B_C4F7410B6E99__INCLUDED_)
