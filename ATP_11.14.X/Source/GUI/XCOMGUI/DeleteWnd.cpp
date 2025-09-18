// DeleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "XCOMTOOL.h"
#include "DeleteWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteWnd dialog


CDeleteWnd::CDeleteWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteWnd, CDialog)
	//{{AFX_MSG_MAP(CDeleteWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteWnd message handlers

int CDeleteWnd::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	OK_f = TRUE;
	Cancel_f = FALSE;
	return CDialog::DoModal();
}

void CDeleteWnd::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDeleteWnd::OnCancel() 
{
	// TODO: Add extra cleanup here
		OK_f = FALSE;
	Cancel_f = TRUE;
	CDialog::OnCancel();
}
