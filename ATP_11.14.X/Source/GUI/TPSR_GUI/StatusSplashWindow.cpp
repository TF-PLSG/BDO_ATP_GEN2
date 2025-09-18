// StatusSplashWindow.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "StatusSplashWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusSplashWindow dialog


CStatusSplashWindow::CStatusSplashWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusSplashWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatusSplashWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CStatusSplashWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusSplashWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusSplashWindow, CDialog)
	//{{AFX_MSG_MAP(CStatusSplashWindow)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusSplashWindow message handlers
