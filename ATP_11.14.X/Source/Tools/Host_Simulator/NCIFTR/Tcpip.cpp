// tcpip.cpp : implementation file
//

#include "stdafx.h"
#include "hostsimc.h"
#include "tcpip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString tHost;
extern CString tPort;


tcpip::tcpip(CWnd* pParent /*=NULL*/)
	: CDialog(tcpip::IDD, pParent)
{
	//{{AFX_DATA_INIT(tcpip)
	m_StrHost = _T("");
	m_StrPort = _T("");
	//}}AFX_DATA_INIT
}


void tcpip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(tcpip)
	DDX_Control(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_HOST, m_host);
	DDX_Text(pDX, IDC_HOST, m_StrHost);
	DDX_Text(pDX, IDC_PORT, m_StrPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(tcpip, CDialog)
	//{{AFX_MSG_MAP(tcpip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// tcpip message handlers

BOOL tcpip::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_host.SetWindowText(tHost);
	m_port.SetWindowText(tPort);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void tcpip::OnOK() 
{
	/*ZeroMemory(&dm_config,sizeof(dm_config));*/
	//connection_type = TCPIP_CONNECTION;
	//strcpy (dm_config.db_connection_type,"TCP/IP");
	m_host.GetWindowText(tHost);
	m_port.GetWindowText(tPort);
	//strcpy (dm_config.db_host_name,tHost);
    //strcpy (dm_config.db_host_tcp_port,tPort);
    //tHost.Format((char *)dm_config.db_host_name);
	//dm_config.db_host_tcp_port = atoi(tPort);
	//pbsn_tcpip_port=atoi(tPort);
	CDialog::OnOK();
}
