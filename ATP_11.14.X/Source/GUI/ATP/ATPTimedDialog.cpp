// ATPTimedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "ATPTimedDialog.h"
#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CATPTimedDialog dialog


CATPTimedDialog::CATPTimedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CATPTimedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CATPTimedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CATPTimedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CATPTimedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CATPTimedDialog, CDialog)
	//{{AFX_MSG_MAP(CATPTimedDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CATPTimedDialog message handlers

BOOL CATPTimedDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::OnCommand(wParam, lParam);
}

void CATPTimedDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}
