// Serial.cpp : implementation file
//

#include "stdafx.h"
#include "hostsimc.h"
#include "serialc.h"
#include "ptecom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString commtype;
extern	CString sPort;
extern	CString sBaud;
extern	CString sByte;
extern	CString sParity;
extern	CString sStop;
extern	CString tHost;
//extern LOCAL_DM_CONFIG dm_config;
//extern int connection_type;
//extern int pbsn_tcpip_port;
//extern	LOCAL_DM_CONFIG dm_config;
//extern  int connection_type;
//extern  int pbsn_tcpip_port;

//extern  DM_CONFIG dm_config;
/////////////////////////////////////////////////////////////////////////////
// Serial dialog


Serialc::Serialc(CWnd* pParent /*=NULL*/)
	: CDialog(Serialc::IDD, pParent)
{
	//{{AFX_DATA_INIT(Serialc)
	//}}AFX_DATA_INIT
}


void Serialc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Serialc)
	DDX_Control(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_COM1, m_com1);
	DDX_Control(pDX, IDC_COM2, m_com2);
	DDX_Control(pDX, IDC_BAUDRATE, m_baudrate);
	DDX_Control(pDX, IDC_9600, m_9600);
	DDX_Control(pDX, IDC_19200, m_19200);
	DDX_Control(pDX, IDC_38400, m_38400);
	DDX_Control(pDX, IDC_BYTESIZE, m_bytesize);
	DDX_Control(pDX, IDC_SIZE8, m_size8);
	DDX_Control(pDX, IDC_SIZE7, m_size7);
	DDX_Control(pDX, IDC_PARITY, m_mparity);
	DDX_Control(pDX, IDC_NONE, m_none);
	DDX_Control(pDX, IDC_ODD, m_odd);
	DDX_Control(pDX, IDC_EVEN, m_even);
	DDX_Control(pDX, IDC_STOPBITS, m_stopbits);
	DDX_Control(pDX, IDC_STOP1, m_stop1);
	DDX_Control(pDX, IDC_STOP12, m_stop12);
	DDX_Control(pDX, IDC_STOP2, m_stop2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Serialc, CDialog)
	//{{AFX_MSG_MAP(Serialc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Serial message handlers

BOOL Serialc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (sPort=="Com1")
 	   m_com1.SetCheck(1);
	else
	   m_com2.SetCheck(1);
	if (sBaud=="9600 ")
	   m_9600.SetCheck(1);
	else
	{
		if (sBaud=="19200")
			m_19200.SetCheck(1);
		else
			m_38400.SetCheck(1);
	}
    if (sByte=="8")
	   m_size8.SetCheck(1);
	else
	   m_size7.SetCheck(1);
	if (sParity=="none")
	   m_none.SetCheck(1);
	else
	{
		if (sParity=="odd ")
			m_odd.SetCheck(1);
		else
			m_even.SetCheck(1);
	}
	if (sStop=="1  ")
	   m_stop1.SetCheck(1);
	else
	{
		if (sStop=="1.5")
			m_stop12.SetCheck(1);
		else
			m_stop2.SetCheck(1);
	}


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Serialc::OnOK() 
{
	// TODO: Add extra validation here
	//ZeroMemory(&dm_config,sizeof(dm_config));*/
	//connection_type = SERIAL_CONNECTION;
	//strcpy (dm_config.db_connection_type,"Serial"); 
	if (m_com1.GetCheck())
	{
		sPort="Com1";
    	//strcpy (dm_config.db_com_port,"COM1"); 
	}
	else
	{
		sPort="Com2";
    	//strcpy (dm_config.db_com_port,"COM2"); 
	}
	if (m_9600.GetCheck())
	{
		sBaud="9600 ";
    	//strcpy (dm_config.db_baud_rate,"9600"); 
	}
	else
	{
		if (m_19200.GetCheck())
		{
			sBaud="19200";
         	//strcpy (dm_config.db_baud_rate,"19200"); 
		}
		else
		{
			sBaud="38400";
         	//strcpy (dm_config.db_baud_rate,"38400"); 
		}
	}
	if (m_size8.GetCheck())
	{
		sByte="8";
		//strcpy (dm_config.db_byte_size,"8"); 
	}
	else
	{
		sByte="7";
		//strcpy (dm_config.db_byte_size,"7"); 
	}
    if (m_none.GetCheck())
	{
		sParity="none";
		//strcpy (dm_config.db_parity,"none"); 
	}
	else
	{
		if (m_odd.GetCheck())
		{
			sParity="odd ";
			//strcpy (dm_config.db_parity,"odd"); 
		}
		else
		{
			sParity="even";
			//strcpy (dm_config.db_parity,"even"); 
		}
}
	if (m_stop1.GetCheck())
	{
		sStop="1  ";
		//strcpy (dm_config.db_stop_bits,"1"); 
	}
	else
	{
		if (m_stop12.GetCheck())
		{
			sStop="1.5";
			//strcpy (dm_config.db_stop_bits,"1.5"); 
		}
		else
		{
			sStop="2  ";
			//strcpy (dm_config.db_stop_bits,"2"); 
		}
	}
	
	CDialog::OnOK();
}
