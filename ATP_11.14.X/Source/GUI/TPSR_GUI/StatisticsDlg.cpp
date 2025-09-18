// StatisticsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tpsr_gui.h"
#include "StatisticsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDlg dialog


CStatisticsDlg::CStatisticsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatisticsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticsDlg)
	m_StrTotalRecv = _T("");
	m_StrTotalSent = _T("");
	m_StrRecv = _T("");
	m_StrSent = _T("");
	//}}AFX_DATA_INIT
}


void CStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticsDlg)
	DDX_Text(pDX, IDC_STATIC_TOTAL_RECV, m_StrTotalRecv);
	DDX_Text(pDX, IDC_STATIC_TOTAL_SENT, m_StrTotalSent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticsDlg, CDialog)
	//{{AFX_MSG_MAP(CStatisticsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDlg message handlers

void CStatisticsDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CStatisticsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_StrTotalRecv = m_StrRecv;
	m_StrTotalSent = m_StrSent;
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
