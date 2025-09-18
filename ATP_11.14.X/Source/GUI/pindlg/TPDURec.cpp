// TPDURec.cpp : implementation file
//

#include "stdafx.h"


extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
}


#include "PinDlg.h"
#include "TPDURec.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TPDU	TPDUTable;		//get instance of TPDU table structure

/////////////////////////////////////////////////////////////////////////////
// CTPDURec dialog


CTPDURec::CTPDURec(CWnd* pParent /*=NULL*/)
	: CDialog(CTPDURec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTPDURec)
	m_TransLen = _T("");
	m_DestLen = _T("");
	m_SrcLen = _T("");
	m_TPDUStr = _T("");
	m_TPDUId = _T("");
	//}}AFX_DATA_INIT
}


void CTPDURec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTPDURec)
	DDX_Control(pDX, IDC_SRCLEN, m_SrcLen_ed);
	DDX_Control(pDX, IDC_TRANSLEN, m_TransLen_ed);
	DDX_Control(pDX, IDC_TPDUSTR, m_TPDUStr_ed);
	DDX_Control(pDX, IDC_DESTLEN, m_DestLen_ed);
	DDX_Control(pDX, IDC_TPDUID, m_TPDUId_ed);
	DDX_Text(pDX, IDC_TRANSLEN, m_TransLen);
	DDX_Text(pDX, IDC_DESTLEN, m_DestLen);
	DDX_Text(pDX, IDC_SRCLEN, m_SrcLen);
	DDX_Text(pDX, IDC_TPDUSTR, m_TPDUStr);
	DDX_Text(pDX, IDC_TPDUID, m_TPDUId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTPDURec, CDialog)
	//{{AFX_MSG_MAP(CTPDURec)
	ON_EN_SETFOCUS(IDC_TPDUID, OnSetfocusTpduid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPDURec message handlers

void CTPDURec::OnOK() 
{
	UpdateData(TRUE);
	if (!CorrectEntry())
		return;

	lstrcpy( (char *)TPDUTable.tpdu_id, m_TPDUId);
	lstrcpy( (char *)TPDUTable.tran_len, m_TransLen);
	lstrcpy( (char *)TPDUTable.dst_len, m_DestLen);
	lstrcpy( (char *)TPDUTable.src_len, m_SrcLen);
	lstrcpy( (char *)TPDUTable.tpdu_string, m_TPDUStr);
	
	CDialog::OnOK();
}

BOOL CTPDURec::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_TransLen_ed.LimitText(1);
	m_DestLen_ed.LimitText(1);
	m_SrcLen_ed.LimitText(1);
	m_TPDUStr_ed.LimitText(50);

	if (m_Mode == MODE_EDIT)
		m_TPDUId_ed.SetReadOnly(TRUE);
	else
	{
		m_TPDUId_ed.LimitText(15);

		m_TransLen_ed.SetWindowText("1");
		m_DestLen_ed.SetWindowText("2");
		m_SrcLen_ed.SetWindowText("2");
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTPDURec::CorrectEntry() 
{
	CString m_MesString;

	if (m_TPDUId.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"TPDU Id");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_TPDUId_ed.SetFocus();
		return FALSE;
	}
	if (m_TransLen.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Transaction Id Length");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_TransLen_ed.SetFocus();
		return FALSE;
	}
	if (m_DestLen.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Destination Address Length");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_DestLen_ed.SetFocus();
		return FALSE;
	}
	if (m_SrcLen.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Originator Address Length");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_SrcLen_ed.SetFocus();
		return FALSE;
	}
	if (m_TPDUStr.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"TPDU String");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_TPDUStr_ed.SetFocus();
		return FALSE;
	}

	return TRUE;
}

void CTPDURec::OnSetfocusTpduid() 
{
	if (m_Mode == MODE_EDIT)
		m_TransLen_ed.SetFocus();
	
}
