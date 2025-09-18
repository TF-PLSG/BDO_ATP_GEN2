// RefreshWnd.cpp : implementation file
//

#include "stdafx.h"
#include "XCOMTOOL.h"
#include "RefreshWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRefreshWnd dialog


CRefreshWnd::CRefreshWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CRefreshWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRefreshWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRefreshWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRefreshWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
		DDX_Control(pDX, IDC_EDIT1, Refresh_Edit_Box);
}


BEGIN_MESSAGE_MAP(CRefreshWnd, CDialog)
	//{{AFX_MSG_MAP(CRefreshWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRefreshWnd message handlers

int CRefreshWnd::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

void CRefreshWnd::OnOK() 
{
	// TODO: Add extra validation here
	Refresh_Edit_Box.GetWindowText(Refresh_Val,10);
	
	CDialog::OnOK();
}

BOOL CRefreshWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CEdit  *m_edit_name = (CEdit *)GetDlgItem(IDC_EDIT1);

	m_edit_name->LimitText(5);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
