// ComSetupPg.cpp : implementation file
//

#include "stdafx.h"
#include "Sharedmem.h"

extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
#include "cdsapi.h"

#include "ptemsg.h"
}


#include "PinDlg.h"
#include "ComSetupPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AppliedComSet;
extern BOOL AppliedTPDUSet;
extern BOOL AppliedRouterSet;
extern BOOL Connected;
extern BOOL NewComSetup;
extern DM_CONFIG DmConfig;		//get instance of DM_Config table structure
extern char Servname[100];
extern int Servsel;
extern HANDLE	hUPDATE_DM_CONFIG;

//This cntr Variable is defined as to access its value across the other dialogs.
extern int cntr;
extern UINT timer_handle;
extern CSharedMem shrmem;
extern CTime c_time, shm_time;
/////////////////////////////////////////////////////////////////////////////
// CComSetupPg property page

IMPLEMENT_DYNCREATE(CComSetupPg, CPropertyPage)

CComSetupPg::CComSetupPg() : CPropertyPage(CComSetupPg::IDD)
{
	//{{AFX_DATA_INIT(CComSetupPg)
	//}}AFX_DATA_INIT
}

CComSetupPg::~CComSetupPg()
{
}

void CComSetupPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComSetupPg)
	DDX_Control(pDX, IDC_SINGLE_THREAD_CHECK, m_SerialSingleThread);
	DDX_Control(pDX, IDC_COMM_PORT, m_ComPortValue);
	DDX_Control(pDX, IDC_IGNORE_TPDU_RADIO, m_IgnoreTheTPDU);
	DDX_Control(pDX, IDC_TPDU_GRB, m_TPDU_grb);
	DDX_Control(pDX, IDC_WITHOUT_TPDU_RADIO, m_WithoutTPDU);
	DDX_Control(pDX, IDC_WITH_TPDU_RADIO, m_WithTPDU);
	DDX_Control(pDX, IDC_SOCKET_TYPE_GRB, m_SocketType_grb);
	DDX_Control(pDX, IDC_SERVER_SOCKET_RADIO, m_ServerSocket);
	DDX_Control(pDX, IDC_CLIENT_SOCKET_RADIO, m_ClientSocket);
	DDX_Control(pDX, IDC_HEADER_LENGTH_LBL, m_HeaderLength_lbl);
	DDX_Control(pDX, IDC_HEADER_LENGTH_ED, m_HeaderLength);
	DDX_Control(pDX, IDC_HOST_NAME_ED, m_HostName);
	DDX_Control(pDX, IDC_SERVICE, m_Service_ed);
	DDX_Control(pDX, IDC_HOST_NAME_LBL, m_HostName_lbl);
	DDX_Control(pDX, IDC_APPLY0, m_Apply0_btn);
	DDX_Control(pDX, IDC_SETBTN, m_SetBtn);
	DDX_Control(pDX, IDC_COMTYPE_LBL, m_ComType_lbl);
	DDX_Control(pDX, IDC_STOPBITS_GRB, m_StopBits);
	DDX_Control(pDX, IDC_STOPB2_RADIO, m_StopB2_Radio);
	DDX_Control(pDX, IDC_STOPB15_RADIO, m_StopB15_Radio);
	DDX_Control(pDX, IDC_STOPB1_RADIO, m_StopB1_Radio);
	DDX_Control(pDX, IDC_PAR_EVEN_RADIO, m_ParEven_Radio);
	DDX_Control(pDX, IDC_PAR_NONE_RADIO, m_ParNone_Radio);
	DDX_Control(pDX, IDC_PAR_ODD_RADIO, m_ParOdd_Radio);
	DDX_Control(pDX, IDC_PARITY_GRB, m_Parity);
	DDX_Control(pDX, IDC_COM_PORT_GRB, m_ComPort);
	DDX_Control(pDX, IDC_BTS8_RADIO, m_BTS8_Radio);
	DDX_Control(pDX, IDC_BTS7_RADIO, m_BTS7_Radio);
	DDX_Control(pDX, IDC_BYTE_SIZE_GRB, m_BTS);
	DDX_Control(pDX, IDC_9600_RADIO, m_BaudRate1_Radio);
	DDX_Control(pDX, IDC_38400_RADIO, m_BaudRate3_Radio);
	DDX_Control(pDX, IDC_19200_RADIO, m_BaudRate2_Radio);
	DDX_Control(pDX, IDC_BAUD_RATE_GRB, m_BaudRate_Grb);
	DDX_Control(pDX, IDC_PORT_LBL, m_TCPIP_PORT_LBL);
	DDX_Control(pDX, IDC_SERIAL_SETUP_GRB, m_SERIAL_SETUP);
	DDX_Control(pDX, IDC_TCPIP_SETUP_GRB, m_TCPIP_SETUP);
	DDX_Control(pDX, IDC_TCPIP_PORT_ED, m_TCPIPPort);
	DDX_Control(pDX, IDC_COMTYPE_COMBO, m_ComType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComSetupPg, CPropertyPage)
	//{{AFX_MSG_MAP(CComSetupPg)
	ON_CBN_SELCHANGE(IDC_COMTYPE_COMBO, OnSelchangeComtypeCombo)
	ON_BN_CLICKED(IDC_SETBTN, OnSetbtn)
	ON_EN_CHANGE(IDC_TCPIP_PORT_ED, OnChangeTcpipPortEd)
	ON_BN_CLICKED(IDC_CLOSE0, OnClose0)
	ON_BN_CLICKED(IDC_APPLY0, OnApply0)
	ON_BN_CLICKED(IDC_HELP0, OnHelp0)
	ON_EN_CHANGE(IDC_HOST_NAME_ED, OnChangeHostNameEd)
	ON_EN_CHANGE(IDC_HEADER_LENGTH_ED, OnChangeHeaderLengthEd)
	ON_BN_CLICKED(IDC_CLIENT_SOCKET_RADIO, OnClientSocketRadio)
	ON_BN_CLICKED(IDC_SERVER_SOCKET_RADIO, OnServerSocketRadio)
	ON_EN_CHANGE(IDC_COMM_PORT, OnChangeCommPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComSetupPg message handlers
BOOL CComSetupPg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CFont	SystemFont;
	BOOL temp = SystemFont.CreatePointFont( 100, "System" );
	m_ComType.SetFont( &SystemFont, TRUE );
   m_HeaderLength.SetFont( &SystemFont, TRUE);
   m_ComPortValue.SetFont( &SystemFont, TRUE);
	m_TCPIPPort.SetFont( &SystemFont, TRUE );
	m_HostName.SetFont( &SystemFont, TRUE );
	m_Service_ed.SetFont( &SystemFont, TRUE );

	hUPDATE_DM_CONFIG = CreateEvent(NULL,FALSE,FALSE,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CComSetupPg::OnSelchangeComtypeCombo() 
{
	int comtypeint = m_ComType.GetCurSel();
	m_ComType.GetLBText( comtypeint, m_ComTypeSelected );
	if (m_ComTypeSelected == "SERIAL")
	{
		EnableTCPIPGrb(FALSE);
		EnableDirectTCP(FALSE);
		EnableSerialGrb(TRUE);
      EnableTPDUGrb(TRUE);
	}
	else
	{
		EnableSerialGrb(FALSE);
	   EnableTCPIPGrb(TRUE);
   	EnableDirectTCP(TRUE);
      EnableTPDUGrb(TRUE);
	}

}

void CComSetupPg::EnableTPDUGrb(BOOL onoff)
{
   m_WithTPDU.EnableWindow(onoff);
   m_WithoutTPDU.EnableWindow(onoff);
   m_IgnoreTheTPDU.EnableWindow(onoff);
	m_HeaderLength_lbl.EnableWindow(onoff);
	m_HeaderLength.EnableWindow(onoff);
}

void CComSetupPg::EnableDirectTCP(BOOL onoff)
{
	m_HostName_lbl.EnableWindow(onoff);
	m_HostName.EnableWindow(onoff);
}

void CComSetupPg::ClearDirectTCP()
{
	m_HostName.SetWindowText("");
}

void CComSetupPg::EnableTCPIPGrb(BOOL onoff)
{
	m_TCPIP_SETUP.EnableWindow(onoff);

	m_SocketType_grb.EnableWindow(onoff);
	m_ClientSocket.EnableWindow(onoff);
	m_ServerSocket.EnableWindow(onoff);

	m_TCPIP_PORT_LBL.EnableWindow(onoff);
	m_TCPIPPort.EnableWindow(onoff);
}

void CComSetupPg::ClearTCPIPGrb()
{
	m_ClientSocket.SetCheck(0);
	m_ServerSocket.SetCheck(0);
}

void CComSetupPg::EnableSerialGrb(BOOL onoff)
{
	m_SERIAL_SETUP.EnableWindow(onoff);
	m_BaudRate_Grb.EnableWindow(onoff);
	m_BaudRate1_Radio.EnableWindow(onoff);
	m_BaudRate2_Radio.EnableWindow(onoff);
	m_BaudRate3_Radio.EnableWindow(onoff);
	m_BTS.EnableWindow(onoff);
	m_BTS7_Radio.EnableWindow(onoff);
	m_BTS8_Radio.EnableWindow(onoff);
	m_ComPort.EnableWindow(onoff);
	m_ComPortValue.EnableWindow(onoff);
   m_SerialSingleThread.EnableWindow(onoff);
	m_Parity.EnableWindow(onoff);
	m_ParOdd_Radio.EnableWindow(onoff);
	m_ParNone_Radio.EnableWindow(onoff);
	m_ParEven_Radio.EnableWindow(onoff);
	m_StopBits.EnableWindow(onoff);
	m_StopB1_Radio.EnableWindow(onoff);
	m_StopB2_Radio.EnableWindow(onoff);
	m_StopB15_Radio.EnableWindow(onoff);
}

void CComSetupPg::ClearSerialGrb()
{
	m_BaudRate1_Radio.SetCheck(0);
	m_BaudRate2_Radio.SetCheck(0);
	m_BaudRate3_Radio.SetCheck(0);
	m_BTS7_Radio.SetCheck(0);
	m_BTS8_Radio.SetCheck(0);
   m_ComPortValue.SetWindowText("");
   m_SerialSingleThread.SetCheck(0);
	m_ParOdd_Radio.SetCheck(0);
	m_ParNone_Radio.SetCheck(0);
	m_ParEven_Radio.SetCheck(0);
	m_StopB1_Radio.SetCheck(0);
	m_StopB2_Radio.SetCheck(0);
	m_StopB15_Radio.SetCheck(0);
}


void CComSetupPg::SetSerialData()
{
	ClearTCPIPGrb();
   ClearDirectTCP();

   m_ComPortValueSelected.Format( (char *)DmConfig.db_com_port );
   m_ComPortValue.SetWindowText(m_ComPortValueSelected);

   if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITH") == 0)
   {
	   m_WithTPDU.SetCheck(1);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(0);
      m_SerialSingleThread.SetCheck(0);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITHOUT") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(1);
      m_IgnoreTheTPDU.SetCheck(0);
      m_SerialSingleThread.SetCheck(0);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "IGNORE") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(1);
      m_SerialSingleThread.SetCheck(0);
   }
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITH_AN") == 0)
   {
	   m_WithTPDU.SetCheck(1);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(0);
      m_SerialSingleThread.SetCheck(1);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITHOUT_AN") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(1);
      m_IgnoreTheTPDU.SetCheck(0);
      m_SerialSingleThread.SetCheck(1);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "IGNORE_AN") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(1);
      m_SerialSingleThread.SetCheck(1);
   } 
	else
	{
		AfxMessageBox( "Failed to Retrieve TPDU Processing.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

	if (strcmp( (char *)DmConfig.db_baud_rate,"9600") == 0)
			m_BaudRate1_Radio.SetCheck(1);
	else if (strcmp( (char *)DmConfig.db_baud_rate,"19200") == 0)
		m_BaudRate2_Radio.SetCheck(1);
	else if (strcmp( (char *)DmConfig.db_baud_rate,"38400") == 0)
		m_BaudRate3_Radio.SetCheck(1);
	else
	{
		AfxMessageBox( "Failed to Retrieve Baud Rate.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

	if (strcmp((char *)DmConfig.db_byte_size,"7") == 0)
		m_BTS7_Radio.SetCheck(1);
	else if (strcmp( (char *)DmConfig.db_byte_size,"8") == 0)
		m_BTS8_Radio.SetCheck(1);
	else
	{
		AfxMessageBox( "Failed to Retrieve Byte Size.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

	if (strcmp( _strupr((char *)DmConfig.db_parity),"NONE") == 0)
		m_ParNone_Radio.SetCheck(1);
	else if (strcmp( _strupr((char *)DmConfig.db_parity),"EVEN") == 0)
		m_ParEven_Radio.SetCheck(1);
	else if (strcmp( _strupr((char *)DmConfig.db_parity),"ODD") == 0)
		m_ParOdd_Radio.SetCheck(1);
	else
	{
		AfxMessageBox( "Failed to Retrieve Parity.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

	if (strcmp( (char *)DmConfig.db_stop_bits,"1") == 0)
		m_StopB1_Radio.SetCheck(1);
	else if (strcmp( (char *)DmConfig.db_stop_bits,"1.5") == 0)
		m_StopB15_Radio.SetCheck(1);
	else if (strcmp( (char *)DmConfig.db_stop_bits,"2") == 0)
		m_StopB2_Radio.SetCheck(1);
	else
	{
	   AfxMessageBox( "Failed to Retrieve Stop Bits.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}
   m_HeaderLengthSelected.Format( (char *)DmConfig.db_tcp_header_length );
   m_HeaderLength.SetWindowText(m_HeaderLengthSelected);
}

void CComSetupPg::SetTCPIPData()
{
	ClearSerialGrb();

   if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITH") == 0)
   {
	   m_WithTPDU.SetCheck(1);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(0);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "WITHOUT") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(1);
      m_IgnoreTheTPDU.SetCheck(0);
   } 
   else if (strcmp(_strupr((char *)DmConfig.db_tpdu_processing), "IGNORE") == 0)
   {
	   m_WithTPDU.SetCheck(0);
	   m_WithoutTPDU.SetCheck(0);
      m_IgnoreTheTPDU.SetCheck(1);
   } 
	else
	{
		AfxMessageBox( "Failed to Retrieve TPDU Processing.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

   // CHANGE THIS TO DB_TCP_SOCKET_TYPE
   if (strcmp(_strupr((char *)DmConfig.db_tcp_socket_type), "CLIENT") == 0)
   {
      m_ClientSocket.SetCheck(1);
      m_ServerSocket.SetCheck(0);
   }
   else if (strcmp(_strupr((char *)DmConfig.db_tcp_socket_type), "SERVER") == 0)
   {
      m_ClientSocket.SetCheck(0);
      m_ServerSocket.SetCheck(1);
   }
	else
	{
		AfxMessageBox( "Failed to Retrieve Socket Type.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}

   m_HeaderLengthSelected.Format( (char *)DmConfig.db_tcp_header_length );
   m_HeaderLength.SetWindowText(m_HeaderLengthSelected);

   if (strcmp(_strupr((char *)DmConfig.db_tcp_socket_type), "SERVER") == 0)
   {
		m_TCPIPPortSelected.Format( (char *)DmConfig.db_tcp_port );
		m_TCPIPPort.SetWindowText(m_TCPIPPortSelected);
   }
   else if (strcmp(_strupr((char *)DmConfig.db_tcp_socket_type), "CLIENT") == 0)
   {
		m_TCPIPPortSelected.Format( (char *)DmConfig.db_tcp_port );
		m_TCPIPPort.SetWindowText(m_TCPIPPortSelected);

		m_HostNameSelected.Format( (char *)DmConfig.db_host_name );
		m_HostName.SetWindowText(m_HostNameSelected);
   }
}

void CComSetupPg::OnSetbtn() 
{
	ZeroMemory(&DmConfig,sizeof(DmConfig));

	char *comma = strchr(Servname,',');
	int n = comma - Servname;
	memcpy((char *)DmConfig.db_subsystem_name,Servname,n);
	lstrcpy( (char *)DmConfig.db_connection_type, m_ComTypeSelected);

   if (m_WithTPDU.GetCheck() == 1)
   {
      if (m_SerialSingleThread.GetCheck() == 0)
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "WITH");
      else
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "WITH_AN");
   }
   else if (m_WithoutTPDU.GetCheck() == 1)
   {
      if (m_SerialSingleThread.GetCheck() == 0)
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "WITHOUT");
      else
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "WITHOUT_AN");
   }
   else if (m_IgnoreTheTPDU.GetCheck() == 1)
   {
      if (m_SerialSingleThread.GetCheck() == 0)
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "IGNORE");
      else
	      lstrcpy( (char *)DmConfig.db_tpdu_processing, "IGNORE_AN");
   }

	if ( m_ComTypeSelected == "TCP/IP" )
	{
		if (CheckTCPIPEntry() == FALSE)
			return ;

	   if (m_ClientSocket.GetCheck() == 1)
      {
		   lstrcpy( (char *)DmConfig.db_tcp_socket_type,"CLIENT");
			lstrcpy( (char *)DmConfig.db_tcp_port,m_TCPIPPortSelected);
			lstrcpy( (char *)DmConfig.db_host_name,m_HostNameSelected);
      }
      else if (m_ServerSocket.GetCheck() == 1)
      {
		   lstrcpy( (char *)DmConfig.db_tcp_socket_type,"SERVER");
			lstrcpy( (char *)DmConfig.db_tcp_port,m_TCPIPPortSelected);
      }
	}

	if (m_ComTypeSelected == "SERIAL")
	{
	   if (m_ComPortValueSelected.IsEmpty())
	   {
		   AfxFormatString1(m_MesString,IDS_MUSTVAL,"Serial Com Port");
		   AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		   m_ComPortValue.SetFocus();
		   return;
	   }

		lstrcpy( (char *)DmConfig.db_com_port, m_ComPortValueSelected);

		if (m_BaudRate1_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_baud_rate,"9600");
		else if (m_BaudRate2_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_baud_rate,"19200");
		else if (m_BaudRate3_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_baud_rate,"38400");

		if (m_BTS7_Radio.GetCheck() == 1)
			lstrcpy((char *)DmConfig.db_byte_size,"7");
		else if (m_BTS8_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_byte_size,"8");

		if (m_ParNone_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_parity,"NONE");
		else if (m_ParEven_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_parity,"EVEN");
		else if (m_ParOdd_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_parity,"ODD");

		if (m_StopB1_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_stop_bits,"1");
		else if (m_StopB15_Radio.GetCheck() == 1)
			lstrcpy( (char *)DmConfig.db_stop_bits,"1.5");
		else if (m_StopB2_Radio.GetCheck() == 1)
			strcmp( (char *)DmConfig.db_stop_bits,"2");
		
	   if (m_HeaderLengthSelected.IsEmpty())
	   {
		   AfxFormatString1(m_MesString,IDS_MUSTVAL,"Serial Header Length");
		   AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		   m_HeaderLength.SetFocus();
		   return;
	   }
	}

   /* Do header length for both Serial and TCP, 102 for TCP means that
      the header length bytes include themselves, 0 for serial means
      <STX>data<ETX> this was a racal mod
   */
   lstrcpy( (char *)DmConfig.db_tcp_header_length,m_HeaderLengthSelected);

	//Send update data from structure to the database
	if (cdsapi_update_dm_config_snd(&DmConfig,ErrMsg) != PTEMSG_OK)
	{
		AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
		return ;
	}

	if (!(WaitForSingleObject(hUPDATE_DM_CONFIG,WAITRESP_TIME) == WAIT_OBJECT_0))
	{
		AfxMessageBox( "Failed to Update Communication Setup.", MB_OK | MB_ICONSTOP, 0 );
		return;
	}
	ResetEvent(hUPDATE_DM_CONFIG);

	AppliedComSet = FALSE;
	SetModified(TRUE);
	m_Apply0_btn.EnableWindow(TRUE);

	//change service name, clear previous selections
	ZeroMemory(Servname,sizeof(Servname));
	if ( DmConfig.db_connection_type[0] == 'S' )
	{
		ClearTCPIPGrb();
		ClearDirectTCP();

      sprintf( Servname, "%s, %s, %s, %s",
                  (char *)DmConfig.db_subsystem_name,
                  (char *)DmConfig.db_connection_type,
                  (char *)DmConfig.db_com_port,
		            (char *)DmConfig.db_baud_rate );
	}
	else
	{
		ClearSerialGrb();

      if ( DmConfig.db_tcp_socket_type[0] == 'S' )
      {
         sprintf( Servname, "%s, %s, %s",
                  (char *)DmConfig.db_subsystem_name,
                  (char *)DmConfig.db_connection_type,
                  (char *)DmConfig.db_tcp_port );
      }
      else if ( DmConfig.db_tcp_socket_type[0] == 'C' )
      {
         sprintf( Servname, "%s, %s, %s, %s",
                  (char *)DmConfig.db_subsystem_name,
                  (char *)DmConfig.db_connection_type,
                  (char *)DmConfig.db_tcp_port,
                  (char *)DmConfig.db_host_name );
      }
	}

	m_Service_ed.SetWindowText(Servname);
	NewComSetup = TRUE;
}


BOOL CComSetupPg::CheckTCPIPEntry() 
{
	if ( (m_WithTPDU.GetCheck() == 0) && (m_WithoutTPDU.GetCheck() == 0) && (m_IgnoreTheTPDU.GetCheck() == 0))
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"TPDU Setting");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_WithTPDU.SetFocus();
		return FALSE;
	}

	if ( (m_ClientSocket.GetCheck() == 0) && (m_ServerSocket.GetCheck() == 0))
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"Socket Type");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_ClientSocket.SetFocus();
		return FALSE;
	}

	if (m_HeaderLengthSelected.IsEmpty())
	{
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"TCP/IP Header Length");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_HeaderLength.SetFocus();
		return FALSE;
	}

	if (m_TCPIPPortSelected.IsEmpty())
	{
		m_PSheet->SetActivePage(0);
		AfxFormatString1(m_MesString,IDS_MUSTVAL,"TCP/IP Port Number");
		AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
		m_TCPIPPort.SetFocus();
		return FALSE;
	}

	if (m_ClientSocket.GetCheck() == 1)
	{
		if (m_HostNameSelected.IsEmpty())
		{
			AfxFormatString1(m_MesString,IDS_MUSTVAL,"Host Name/IP Address ");
			AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
			m_HostName.SetFocus();
			return FALSE;
		}
	}

	return TRUE;
}

void CComSetupPg::OnChangeTcpipPortEd() 
{
	m_TCPIPPort.GetWindowText(m_TCPIPPortSelected);
	
}


BOOL CComSetupPg::OnSetActive() 
{
	// Display Service name.
	m_Service_ed.SetWindowText(Servname);

	//Move the data from DmConfig structure into controls of the page.
	if (strcmp( _strupr((char *)DmConfig.db_connection_type),"SERIAL") == 0)
	{
		m_ComType.SetCurSel(0);
		m_ComType.GetLBText(0, m_ComTypeSelected );
		SetSerialData();
	}
	else if (strcmp( _strupr((char *)DmConfig.db_connection_type),"TCP/IP") == 0)
	{
		m_ComType.SetCurSel(1);
		m_ComType.GetLBText(1, m_ComTypeSelected );
		SetTCPIPData();
	}
		
	if (AppliedComSet == FALSE || AppliedTPDUSet == FALSE || AppliedRouterSet == FALSE)
		m_Apply0_btn.EnableWindow(TRUE);
	else
		m_Apply0_btn.EnableWindow(FALSE);
		
	if (Connected == FALSE)						//if not connected to the server
	{
		m_ComType_lbl.EnableWindow(TRUE);
		m_ComType.EnableWindow(TRUE);
		m_SetBtn.EnableWindow(TRUE);
		if (m_ComTypeSelected == "SERIAL")
		{
			ClearTCPIPGrb();
			ClearDirectTCP();
			EnableTCPIPGrb(FALSE);
			EnableDirectTCP(FALSE);
			EnableSerialGrb(TRUE);
         EnableTPDUGrb(TRUE);
		}
		else
		{
			ClearSerialGrb();
			EnableSerialGrb(FALSE);
         EnableTPDUGrb(TRUE);

	      if (m_ServerSocket.GetCheck() == 1)
			{
				EnableTCPIPGrb(TRUE);
				EnableDirectTCP(FALSE);
			}
         else if (m_ClientSocket.GetCheck() == 1)
			{
				EnableTCPIPGrb(TRUE);
				EnableDirectTCP(TRUE);
			}
         else
			{
				EnableDirectTCP(TRUE);
				EnableTCPIPGrb(TRUE);
			}
		}
	}
	else
	{
		m_ComType_lbl.EnableWindow(FALSE);
		m_ComType.EnableWindow(FALSE);
		EnableSerialGrb(FALSE);
		EnableTCPIPGrb(FALSE);
		EnableDirectTCP(FALSE);
      EnableTPDUGrb(FALSE);
		m_SetBtn.EnableWindow(FALSE);
	}

	return CPropertyPage::OnSetActive();
}

void CComSetupPg::OnClose0() 
{	
	//if(!KillTimer(timer_handle))
	//		::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
				
	//Delete the associated Mutex lock 
	shrmem.DestroySharedMemorySegment();
	shrmem.DeleteMutexLock();
	
	m_PSheet->PressButton(PSBTN_CANCEL);
	
}

void CComSetupPg::OnApply0() 
{
	m_PSheet->PressButton(PSBTN_APPLYNOW);
	if (AppliedComSet == TRUE && AppliedTPDUSet == TRUE && AppliedRouterSet == TRUE)
		m_Apply0_btn.EnableWindow(FALSE);
}

void CComSetupPg::OnHelp0() 
{
	m_PSheet->PressButton(PSBTN_HELP);
	
}

void CComSetupPg::OnChangeHostNameEd() 
{
	m_HostName.GetWindowText(m_HostNameSelected);
}

void CComSetupPg::OnChangeHeaderLengthEd() 
{
	m_HeaderLength.GetWindowText(m_HeaderLengthSelected);
}

void CComSetupPg::ClearTPDUGrb()
{
	m_WithTPDU.SetCheck(0);
	m_WithoutTPDU.SetCheck(0);
   m_IgnoreTheTPDU.SetCheck(0);
	m_HeaderLength.SetWindowText("");	
   m_TCPIPPort.SetWindowText("");
}

void CComSetupPg::OnClientSocketRadio() 
{
	EnableDirectTCP(TRUE);
}

void CComSetupPg::OnServerSocketRadio() 
{
	m_HostName.SetWindowText("");
	EnableDirectTCP(FALSE);
}


void CComSetupPg::OnChangeCommPort() 
{
	m_ComPortValue.GetWindowText(m_ComPortValueSelected);
}

BOOL CComSetupPg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	cntr = 0;
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
