// Createwnd.cpp : implementation file
//

#include "stdafx.h"
#include "XCOMTOOL.h"
#include "Createwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreatewnd dialog


CCreatewnd::CCreatewnd(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatewnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreatewnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreatewnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatewnd)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT1, Createwnd_Edit_Box1);
	DDX_Control(pDX, IDC_EDIT2, Createwnd_Edit_Box2);
	DDX_Control(pDX, IDC_EDIT3, Createwnd_Edit_Box3);
	//DDX_Control(pDX, IDC_STATIC_CREATE_NAME, create_name);
}


BEGIN_MESSAGE_MAP(CCreatewnd, CDialog)
	//{{AFX_MSG_MAP(CCreatewnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatewnd message handlers

int CCreatewnd::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

void CCreatewnd::OnOK() 
{
	// TODO: Add extra validation here
	memset(Createwnd_NAME, 0x00, sizeof(Createwnd_NAME));
	memset(Createwnd_NoBytes, 0x00, sizeof(Createwnd_NoBytes));
	memset(Createwnd_NoMsgs, 0x00, sizeof(Createwnd_NoMsgs));

	Createwnd_Edit_Box1.GetWindowText(Createwnd_NAME,20);
	Createwnd_Edit_Box2.GetWindowText(Createwnd_NoBytes,10);
	Createwnd_Edit_Box3.GetWindowText(Createwnd_NoMsgs,10);

	if(strlen(Createwnd_NAME) > 0 && strlen(Createwnd_NoBytes) > 0 && strlen(Createwnd_NoMsgs) > 0)
	{
		OK_f = TRUE;
		Cancel_f = FALSE;
	}
	CDialog::OnOK();
}

void CCreatewnd::OnCancel() 
{
	// TODO: Add extra cleanup here
	OK_f = FALSE;
	Cancel_f = TRUE;
	CDialog::OnCancel();
}

BOOL CCreatewnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit  *m_edit_name = (CEdit *)GetDlgItem(IDC_EDIT1);
	CWnd   *m_edit3 = (CWnd *)GetDlgItem(IDC_STATIC_3);
	
	CEdit  *m_edit_no_bytes = (CEdit *)GetDlgItem(IDC_EDIT2);
	CEdit  *m_edit_no_msg = (CEdit *)GetDlgItem(IDC_EDIT3);
	
	m_edit_name->LimitText(15);
	m_edit_no_bytes->LimitText(10);
	m_edit_no_msg->LimitText(10);

	// TODO: Add extra initialization here
	if(Mem_f)
	{
		Mem_f = FALSE;		
		//m_edit_no_msg->EnableWindow(FALSE);
		m_edit_no_msg->ShowWindow(SW_HIDE);
		m_edit3->ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
