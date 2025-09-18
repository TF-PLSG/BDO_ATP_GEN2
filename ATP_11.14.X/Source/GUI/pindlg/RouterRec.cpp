// RouterRec.cpp : implementation file
//

#include "stdafx.h"


extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
}


#include "PinDlg.h"
#include "RouterRec.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ROUTER_TABLE Router;	//get instance of Router table structure

/////////////////////////////////////////////////////////////////////////////
// CRouterRec dialog


CRouterRec::CRouterRec(CWnd* pParent /*=NULL*/)
	: CDialog(CRouterRec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRouterRec)
	m_AppDesc = _T("");
	m_DataType = _T("");
	m_DestQue = _T("");
	m_HiRange = _T("");
	m_Length = _T("");
	m_LoRange = _T("");
	m_Offset = _T("");
	//}}AFX_DATA_INIT
}


void CRouterRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRouterRec)
	DDX_Control(pDX, IDC_OFFSET, m_Offset_ed);
	DDX_Control(pDX, IDC_LORANGE, m_LoRange_ed);
	DDX_Control(pDX, IDC_LENGTH, m_Length_ed);
	DDX_Control(pDX, IDC_HIRANGE, m_HiRange_ed);
	DDX_Control(pDX, IDC_DESTQUE, m_DestQue_ed);
	DDX_Control(pDX, IDC_DATATYPE, m_DataType_ed);
	DDX_Control(pDX, IDC_APPDESC, m_AppDesc_ed);
	DDX_Text(pDX, IDC_APPDESC, m_AppDesc);
	DDX_Text(pDX, IDC_DATATYPE, m_DataType);
	DDX_Text(pDX, IDC_DESTQUE, m_DestQue);
	DDX_Text(pDX, IDC_HIRANGE, m_HiRange);
	DDX_Text(pDX, IDC_LENGTH, m_Length);
	DDX_Text(pDX, IDC_LORANGE, m_LoRange);
	DDX_Text(pDX, IDC_OFFSET, m_Offset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRouterRec, CDialog)
	//{{AFX_MSG_MAP(CRouterRec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRouterRec message handlers

void CRouterRec::OnOK() 
{
	UpdateData(TRUE);
	if (!CorrectEntry())
		return;

	lstrcpy( (char *)Router.router_queue, m_DestQue);	
	lstrcpy( (char *)Router.router_app_desc, m_AppDesc);	
	lstrcpy( (char *)Router.router_offset, m_Offset);	
	lstrcpy( (char *)Router.router_length, m_Length);
	m_DataType.MakeLower();
	lstrcpy( (char *)Router.router_datatype, m_DataType);	
	lstrcpy( (char *)Router.router_lo_end, m_LoRange);	
	lstrcpy( (char *)Router.router_hi_end, m_HiRange);	

	CDialog::OnOK();
}

BOOL CRouterRec::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Offset_ed.LimitText(5);
	m_LoRange_ed.LimitText(10);
	m_Length_ed.LimitText(2);
	m_HiRange_ed.LimitText(10);
	m_DestQue_ed.LimitText(10);
	m_DataType_ed.LimitText(4);
	m_AppDesc_ed.LimitText(20);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CRouterRec::CorrectEntry() 
{
	CString m_MesString;

	if (m_DestQue.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Destination Queue");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_DestQue_ed.SetFocus();
		return FALSE;
	}
	if (m_AppDesc.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Service Description");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_AppDesc_ed.SetFocus();
		return FALSE;
	}
	if (m_Offset.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Byte Offset");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_Offset_ed.SetFocus();
		return FALSE;
	}
	if (m_Length.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Length");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_Length_ed.SetFocus();
		return FALSE;
	}
	if (m_DataType.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Data Type");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_DataType_ed.SetFocus();
		return FALSE;
	}

	if (m_LoRange.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Low Range");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_LoRange_ed.SetFocus();
		return FALSE;
	}
	if (m_HiRange.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"High Range");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_HiRange_ed.SetFocus();
		return FALSE;
	}

	return TRUE;
}
