// UserStatusControl.h : main header file for the UserStatusControl application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlApp:
// See UserStatusControl.cpp for the implementation of this class
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

class CUserStatusControlApp : public CWinApp
{
public:
	CUserStatusControlApp();
	HANDLE m_hFileMappingObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserStatusControlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	BOOL Update_User_status();
// Implementation

	//{{AFX_MSG(CUserStatusControlApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};


/////////////////////////////////////////////////////////////////////////////
