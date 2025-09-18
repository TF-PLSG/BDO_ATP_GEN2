// PinDlg.h : main header file for the PINDLG application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define MODE_ADD  0
#define MODE_EDIT 1
#define WAITRESP_TIME 20000
/////////////////////////////////////////////////////////////////////////////
// CPinDlgApp:
// See PinDlg.cpp for the implementation of this class
//
class CPinDlgApp : public CWinApp
{
public:


	CPinDlgApp();
	

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPinDlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPinDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
