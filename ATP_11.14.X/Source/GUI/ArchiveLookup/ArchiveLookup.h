// ArchiveLookup.h : main header file for the ARCHIVELOOKUP application
//

#if !defined(AFX_ARCHIVELOOKUP_H__4C04E15C_FE23_453A_A359_9AD454FC3435__INCLUDED_)
#define AFX_ARCHIVELOOKUP_H__4C04E15C_FE23_453A_A359_9AD454FC3435__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

extern "C"
{
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "util_dbstruct.h"
    #include "TXDSAPI.h"
}
/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupApp:
// See ArchiveLookup.cpp for the implementation of this class
//

class CArchiveLookupApp : public CWinApp
{
public:
	void RunDialog();
	CArchiveLookupApp();
	HANDLE m_hFileMappingObject ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveLookupApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CArchiveLookupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVELOOKUP_H__4C04E15C_FE23_453A_A359_9AD454FC3435__INCLUDED_)
