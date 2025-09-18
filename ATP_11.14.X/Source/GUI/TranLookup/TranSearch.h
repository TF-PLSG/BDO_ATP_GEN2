// TranSearch.h : main header file for the TRANSEARCH application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef TRANSEARCH
#define TRANSEARCH

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTranSearchApp:
// See TranSearch.cpp for the implementation of this class
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

class CTranSearchApp : public CWinApp
{
public:
	CTranSearchApp();
	HANDLE m_hFileMappingObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranSearchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTranSearchApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#endif