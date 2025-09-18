// stipmgr.h : main header file for the STIPMGR application
//

#if !defined(AFX_STIPMGR_H__A9ABE42D_4AB5_420D_AC32_3DAD43549867__INCLUDED_)
#define AFX_STIPMGR_H__A9ABE42D_4AB5_420D_AC32_3DAD43549867__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStipmgrApp:
// See stipmgr.cpp for the implementation of this class
//
extern "C"
{
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
    #include "TXDSAPI.h"
}

class CStipmgrApp : public CWinApp
{
public:
	CStipmgrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStipmgrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStipmgrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STIPMGR_H__A9ABE42D_4AB5_420D_AC32_3DAD43549867__INCLUDED_)
