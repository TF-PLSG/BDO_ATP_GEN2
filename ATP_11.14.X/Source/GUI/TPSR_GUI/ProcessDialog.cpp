// ProcessDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "ProcessDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessDialog

IMPLEMENT_DYNCREATE(CProcessDialog, CWinThread)

CProcessDialog::CProcessDialog()
{
}

CProcessDialog::~CProcessDialog()
{
}

BOOL CProcessDialog::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CProcessDialog::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProcessDialog, CWinThread)
	//{{AFX_MSG_MAP(CProcessDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_THREAD_MESSAGE(WM_MSG1, HandleThreadMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessDialog message handlers

int CProcessDialog::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinThread::Run();
}

BOOL CProcessDialog::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinThread::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

afx_msg void CProcessDialog::HandleThreadMessage(WPARAM wParam, LPARAM lParam)
{
    ::MessageBox(::GetDesktopWindow(), "Hi","A",MB_OK);
}
