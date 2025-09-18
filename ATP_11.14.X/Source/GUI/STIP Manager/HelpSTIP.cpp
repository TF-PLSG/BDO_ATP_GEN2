// HelpSTIP.cpp : implementation file
//

#include "stdafx.h"
#include "stipmgr.h"
#include "HelpSTIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpSTIP dialog


CHelpSTIP::CHelpSTIP(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpSTIP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpSTIP)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHelpSTIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpSTIP)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpSTIP, CDialog)
	//{{AFX_MSG_MAP(CHelpSTIP)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpSTIP message handlers
