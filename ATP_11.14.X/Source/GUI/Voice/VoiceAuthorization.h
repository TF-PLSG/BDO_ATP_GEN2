// VoiceAuthorization.h : main header file for the VOICEAUTHORIZATION application
//

#if !defined(AFX_VOICEAUTHORIZATION_H__20C113FA_714A_11D3_9764_0008C789BA2A__INCLUDED_)
#define AFX_VOICEAUTHORIZATION_H__20C113FA_714A_11D3_9764_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
extern "C"
{
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
    #include "TXDSAPI.h"
}

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationApp:
// See VoiceAuthorization.cpp for the implementation of this class
//

class CVoiceAuthorizationApp : public CWinApp
{
public:
	CVoiceAuthorizationApp();
#define MODE_OVERRIDE 1
#define MODE_DONE	    2
#define MODE_TRANSACTION 3

	HANDLE m_hFileMappingObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthorizationApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVoiceAuthorizationApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEAUTHORIZATION_H__20C113FA_714A_11D3_9764_0008C789BA2A__INCLUDED_)
