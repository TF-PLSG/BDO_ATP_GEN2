// DialogPg.cpp : implementation file
//

#include "stdafx.h"
#include "Loginfrm.h"

extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
#include "cdsapi.h"

#include "ptetcp.h"
#include "ptemsg.h"
}


#include "PinDlg.h"
#include "DialogPg.h"


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
extern BOOL Trace;
extern DM_CONFIG DmConfig;		//get instance of DM_Config table structure
extern DM_CONFIG_LIST DmConfig_list;
extern ROUTER_TABLE_LIST Router_list;
extern TPDU_TABLE_LIST TPDU_list;
extern ROUTER_TABLE Router;	
extern TPDU	TPDUTable;		
//extern char GUI_que_name[MAX_QUE_NAME_SIZE];
extern char Servname[100];
extern int Servsel;
extern HANDLE  hGET_TPDU;
extern HANDLE  hGET_ROUTER;
extern HANDLE	hUPDATE_DM_CONFIG;
extern HANDLE  hUPDATE_ROUTER;
extern HANDLE  hUPDATE_TPDU;
extern HANDLE  hINSERT_ROUTER;
extern HANDLE  hINSERT_TPDU;
extern HANDLE  hDELETE_ROUTER;
extern HANDLE  hDELETE_TPDU;

#define INCOMING 1
#define OUTGOING 2
#define CLEARTRACE 3

extern "C"
{
   #include "ntutils.h"
   #include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
   #include "nc_dbstruct.h"
   #include "tx_dbstruct.h"
}


//This cntr Varis defined in global space to access the value across the other dialogs.
int cntr;
UINT timer_handle;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
CString idle_time_val_str;
extern CSharedMem shrmem;
CLoginfrm loginfrm;
CTime c_time, shm_time;

UCF01 ucf01;
volatile BOOL wait_f;

extern "C"
{
	//Krishna@TF, Contains Version number of the App
	extern CHAR  appProductVersion[MAX_APPVERSION_SIZE];
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_appversion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnAboutBoxOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_appversion = _T("");
	//}}AFX_DATA_INIT

	char temp_buff[50];
	sprintf(temp_buff, "ATP - Dialog Manager.\nVersion : %s", appProductVersion);
	m_appversion = temp_buff;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_APPVERSION, m_appversion);
	DDV_MaxChars(pDX, m_appversion, 30);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDOK, OnAboutBoxOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPg property page

IMPLEMENT_DYNCREATE(CDialogPg, CPropertyPage)

CDialogPg::CDialogPg() : CPropertyPage(CDialogPg::IDD)
{
	//{{AFX_DATA_INIT(CDialogPg)
	//}}AFX_DATA_INIT
	
	//Instantiate the Object for SHM
	pLock = new CDMLock("IdleLock");
}

CDialogPg::~CDialogPg()
{
   delete pLock;
   pLock = NULL;
}

void CDialogPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPg)
	DDX_Control(pDX, IDC_SERVICES_LIST, m_Services_lst);
	DDX_Control(pDX, IDC_APPLY1, m_Apply1_btn);
	DDX_Control(pDX, IDC_CONNECT_BUTTON, m_Connect_btn);
	DDX_Control(pDX, IDC_ONOFF_INDICATOR, m_OnOff_Indicator);
	DDX_Control(pDX, IDC_INCOMMING_GRB, m_Incoming_grb);
	DDX_Control(pDX, IDC_TRACE_FNAME_ED, m_TraceFName);
	DDX_Control(pDX, IDC_OUTTPDU_ED, m_OutTPDU);
	DDX_Control(pDX, IDC_OUTMESSAGE_ED, m_OutMessage);
	DDX_Control(pDX, IDC_OUTMESLEN_ED, m_OutMesLen);
	DDX_Control(pDX, IDC_INTPDU_ED, m_InTPDU);
	DDX_Control(pDX, IDC_INMESSAGE_ED, m_InMessage);
	DDX_Control(pDX, IDC_INMESLEN_ED, m_InMesLen);
	DDX_Control(pDX, IDC_OUTGOING_GRB, m_Outgoing_grb);
	DDX_Control(pDX, IDC_TRACE_FNAME_LBL, m_TraceFName_lbl);
	DDX_Control(pDX, IDC_TRACE_CHECK, m_TraceCheck);
	DDX_Control(pDX, IDC_OUTTPDU_LBL, m_OutTPDU_lbl);
	DDX_Control(pDX, IDC_OUTMESLEN_LBL, m_OutMesLen_lbl);
	DDX_Control(pDX, IDC_INTPDU_LBL, m_InTPDU_lbl);
	DDX_Control(pDX, IDC_INMESLEN_LBL, m_InMesLen_lbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPg, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogPg)
	ON_BN_CLICKED(IDC_TRACE_CHECK, OnTraceCheck)
	ON_BN_CLICKED(IDC_ABOUT_BUTTON, OnAboutButton)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_WM_CTLCOLOR()
	ON_EN_SETFOCUS(IDC_INMESSAGE_ED, OnSetfocusInmessageEd)
	ON_EN_SETFOCUS(IDC_INMESLEN_ED, OnSetfocusInmeslenEd)
	ON_EN_SETFOCUS(IDC_INTPDU_ED, OnSetfocusIntpduEd)
	ON_EN_SETFOCUS(IDC_OUTMESLEN_ED, OnSetfocusOutmeslenEd)
	ON_EN_SETFOCUS(IDC_OUTMESSAGE_ED, OnSetfocusOutmessageEd)
	ON_EN_SETFOCUS(IDC_OUTTPDU_ED, OnSetfocusOuttpduEd)
	ON_EN_SETFOCUS(IDC_TRACE_FNAME_ED, OnSetfocusTraceFnameEd)
	ON_BN_CLICKED(IDC_CLOSE1, OnClose1)
	ON_BN_CLICKED(IDC_APPLY1, OnApply1)
	ON_BN_CLICKED(IDC_HELP1, OnHelp1)
	ON_CBN_SELCHANGE(IDC_SERVICES_LIST, OnSelchangeServicesList)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPg message handlers
BOOL CDialogPg::OnInitDialog() 
{
	char ini_file[256] = {'\0'};
	CDialog::OnInitDialog();

	//Load bitmaps
	m_hOffLine = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_OffLine));
	m_hOnLine = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_OnLine));

	stopflag = FALSE;
	Servsel = -1;
	shownoerrors = FALSE;

	m_pEditBkBrush.CreateSolidBrush(RGB(255, 255, 255));

	//Set font for edit controls
	CFont	SystemFont;
	BOOL temp = SystemFont.CreatePointFont( 100, "System" );
	m_InMessage.SetFont( &SystemFont, TRUE );
	m_InMesLen.SetFont( &SystemFont, TRUE );
	m_InTPDU.SetFont( &SystemFont, TRUE );
	m_OutMessage.SetFont( &SystemFont, TRUE );
	m_OutMesLen.SetFont( &SystemFont, TRUE );
	m_OutTPDU.SetFont( &SystemFont, TRUE );
	m_TraceFName.SetFont( &SystemFont, TRUE );
	m_Services_lst.SetFont( &SystemFont, TRUE );


	//Create events of getting responses 
	hCONNECT_CONFIRM = CreateEvent(NULL,FALSE,FALSE,NULL);
	hDISCONNECT_CONFIRM = CreateEvent(NULL,FALSE,FALSE,NULL);
	hCONNECTED_TRACE = CreateEvent(NULL,FALSE,FALSE,NULL);
	hCONNECTED_NOTRACE = CreateEvent(NULL,FALSE,FALSE,NULL);
	hTRACEON_CONFIRM = CreateEvent(NULL,FALSE,FALSE,NULL);
	hTRACEOFF_CONFIRM = CreateEvent(NULL,FALSE,FALSE,NULL);
	hCOM_STATUS = CreateEvent(NULL,FALSE,FALSE,NULL);
	hGET_DM_CONFIG = CreateEvent(NULL,FALSE,FALSE,NULL);

    

  //Krishna, 10.4.11 : Below changes are made for Idle time calculation, 
   //displaying the Login Screen & blocking other screens from editing.
   	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );

	//Get the Idle TIme from tf.ini
	GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "IDLE_TIMEOUT_VALUE",	   // points to key name 
        "600",					   // points to default string 
        idle_time_val,             // points to destination buffer 
        sizeof(idle_time_val) - 1, // size of destination buffer 
        ini_file );                // points to initialization filename 
		 
	    idle_time_val[sizeof(idle_time_val)] = 0; //Make sure string is terminated
		
		idle_time = atoi(idle_time_val);

	//Start the Timer
	timer_handle = StartTimer(1000);

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }

   shrmem.CreateMutexLock();
   shrmem.CreateSharedMemorySegment();
   shrmem.MapSharedMemorySegment();
 

	//Krishna : Get the Current time & store it in SHM
	ResetCntr();	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);




	Trace = FALSE;
	EnableTrace(FALSE);
	m_Connect_btn.EnableWindow(FALSE);
	m_TraceCheck.EnableWindow(FALSE);
	m_OnOff_Indicator.EnableWindow(FALSE);


	//run function to get DMCONFIG_LIST structure populated 
	if (cdsapi_get_dmconfig_table(&DmConfig_list,ErrMsg) != PTEMSG_OK)
	{
		AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
		return TRUE;
	}
 
	//populate Services drop-down list
	int i;
	char BldString[60];
	for (i = 0; i < DmConfig_list.num_returned; ++i)
	{
		ZeroMemory(BldString,sizeof(BldString));

	   if ( DmConfig_list.dm_config_record[i].db_connection_type[0] == 'S' )
	   {
         sprintf( BldString, "%s, %s, %s, %s",
                  (char *)DmConfig_list.dm_config_record[i].db_subsystem_name,
                  (char *)DmConfig_list.dm_config_record[i].db_connection_type,
                  (char *)DmConfig_list.dm_config_record[i].db_com_port,
		            (char *)DmConfig_list.dm_config_record[i].db_baud_rate );
	   }
	   else
	   {
         if ( DmConfig_list.dm_config_record[i].db_tcp_socket_type[0] == 'S' )
         {
            sprintf( BldString, "%s, %s, %s",
                  (char *)DmConfig_list.dm_config_record[i].db_subsystem_name,
                  (char *)DmConfig_list.dm_config_record[i].db_connection_type,
                  (char *)DmConfig_list.dm_config_record[i].db_tcp_port );
         }
         else if ( DmConfig_list.dm_config_record[i].db_tcp_socket_type[0] == 'C' )
         {
            sprintf( BldString, "%s, %s, %s, %s",
                  (char *)DmConfig_list.dm_config_record[i].db_subsystem_name,
                  (char *)DmConfig_list.dm_config_record[i].db_connection_type,
                  (char *)DmConfig_list.dm_config_record[i].db_tcp_port,
                  (char *)DmConfig_list.dm_config_record[i].db_host_name );
         }
	   }

   	m_Services_lst.AddString(BldString);
	}

	//run function to get TPDU and ROUTER_TABLE structures populated
	if (cdsapi_get_tpdu_table(&TPDU_list,ErrMsg) != PTEMSG_OK)
	{
		AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
		return TRUE;
	}

		if (cdsapi_get_router_table(&Router_list,ErrMsg) != PTEMSG_OK)
	{
		AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
		return TRUE;
	}

	hThread = NULL;
	//create thread for getting trace information from DMS
	hThread = CreateThread( NULL,
							NULL,
							(LPTHREAD_START_ROUTINE)TraceThreadStarter,
							this,
							NULL,
							&ThreadId );
	if( hThread == NULL )
	{
		AfxMessageBox( "Unable to create a receiving thread", MB_OK | MB_ICONSTOP );
		m_TraceCheck.SetCheck(0);
		EnableTrace(FALSE);
		return TRUE;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogPg::OnTraceCheck() 
{
	BeginWaitCursor();
	int traceint = m_TraceCheck.GetCheck();
	//if trace is on
	if (traceint == 1)
	{

		if (!MsgToDialogMan(MT_COM_REQUEST,ST1_TRACE_ON))
		{
			m_TraceCheck.SetCheck(0);
			EndWaitCursor();
			return;
		}
		if (!(WaitForSingleObject(hTRACEON_CONFIRM,WAITRESP_TIME) == WAIT_OBJECT_0))
		{
			m_TraceCheck.SetCheck(0);
			EndWaitCursor();
			return;
		}

		SetTraceFName();

		m_colorFlag = CLEARTRACE;
		EnableTrace(TRUE);

	}
	else
		//trace is off.
	{
		if (!MsgToDialogMan(MT_COM_REQUEST,ST1_TRACE_OFF))
		{
			m_TraceCheck.SetCheck(1);
			EndWaitCursor();
			return;
		}
		if (!(WaitForSingleObject(hTRACEOFF_CONFIRM,WAITRESP_TIME) == WAIT_OBJECT_0))
		{
			m_TraceCheck.SetCheck(1);
			EndWaitCursor();
			return;
		}
      ResetEvent(hTRACEOFF_CONFIRM);

		EnableTrace(FALSE);
	}

	EndWaitCursor();
}


void CDialogPg::SetTraceFName()
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	char			MsgData[50];

		p_msg_data = ptemsg_get_pte_msg_data( pMsg_tfname );
		p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len = ptemsg_get_pte_msg_data_data_length( p_msg_data );
		ZeroMemory(MsgData,sizeof(MsgData));
		memcpy( MsgData, (char *)p_data, data_len );
		m_TraceFName.SetWindowText(MsgData);

	return ;
}


void CDialogPg::EnableTrace(BOOL onoff)
{
	m_Incoming_grb.EnableWindow(onoff);
	m_InMesLen_lbl.EnableWindow(onoff);
	m_InTPDU_lbl.EnableWindow(onoff);
	m_Outgoing_grb.EnableWindow(onoff);
	m_OutMesLen_lbl.EnableWindow(onoff);
	m_OutTPDU_lbl.EnableWindow(onoff);
	m_TraceFName_lbl.EnableWindow(onoff);

	if (onoff == FALSE)
	{
		m_InMesLen.SetWindowText(NULL);
		m_InMessage.SetWindowText(NULL);
		m_InTPDU.SetWindowText(NULL);
		m_OutTPDU.SetWindowText(NULL);
		m_OutMessage.SetWindowText(NULL);
		m_OutMesLen.SetWindowText(NULL);
		m_TraceFName.SetWindowText(NULL);

	}
	m_InMesLen.EnableWindow(onoff);
	m_InMessage.EnableWindow(onoff);
	m_InTPDU.EnableWindow(onoff);
	m_OutTPDU.EnableWindow(onoff);
	m_OutMessage.EnableWindow(onoff);
	m_OutMesLen.EnableWindow(onoff);
	m_TraceFName.EnableWindow(onoff);
}


void CDialogPg::TraceHandler()
{
	pPTE_MSG		pMsg;
	pCHAR			orig_que;
	LONG			data_len;
	pBYTE			p_data;
	BYTE			msg_type,msg_subtype,data_type,result;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			MsgData[200];
	CHAR			MsgLen[4];
	CString     temp;
   CHAR        error_msg[100];
   CHAR        tpdu_data[20];   

	hFlashColor = CreateEvent(NULL,TRUE,FALSE,NULL);
	SetEvent(hFlashColor);

   //loop to get messages from interactive queue
	while (TRUE)
	{
      pMsg = ptetcp_receive( 0, error_msg );
      if( pMsg == NULL_PTR )
      {
		   AfxFormatString1(m_MesString,IDS_COMERROR, error_msg);
			AfxMessageBox( m_MesString, MB_OK | MB_ICONSTOP, 0 );
         break;
      }


		msg_type = ptemsg_get_msg_type(pMsg);
		msg_subtype = ptemsg_get_msg_subtype1(pMsg);

		if (msg_type == MT_COM_REQUEST && msg_subtype == ST1_EXIT)
			break;


		if (msg_type == MT_COM_REPLY)
		{
			switch	(msg_subtype)
			{
			case ST1_CONNECT_CONFIRM:
				{
					SetEvent(hCONNECT_CONFIRM);
					break;
				}
			case ST1_DISCONNECT_CONFIRM:
				{
					if (!stopflag)
					{
						Connected = FALSE;
						SetEvent(hCOM_STATUS);
					}
					else
					{
						if (Trace)
						{
							m_TraceCheck.SetCheck(0);
							EnableTrace(FALSE);
							m_TraceCheck.EnableWindow(FALSE);
						}

						SetEvent(hDISCONNECT_CONFIRM);
					}

					break;
				}
			case ST1_CONNECTED_TRACE:
				{
					Connected = TRUE;
					Trace = TRUE;
					pMsg_tfname = pMsg;
					SetEvent(hCOM_STATUS);
					break;
				}
			case ST1_CONNECTED_NOTRACE:
				{
					Connected = TRUE;
					Trace = FALSE;
					SetEvent(hCOM_STATUS);
					break;
				}
			case ST1_TRACE_ON_CONFIRM:
				{
					Trace = TRUE;
					pMsg_tfname = pMsg;
					SetEvent(hTRACEON_CONFIRM);
					break;
				}
			case ST1_TRACE_OFF_CONFIRM:
				{
					Trace = FALSE;
					SetEvent(hTRACEOFF_CONFIRM);
					break;
				}
			case ST1_STOP:
				{
					Connected = FALSE;
					ZeroMemory(&DmConfig,sizeof(DmConfig));
					if (Trace)
					{
						m_TraceCheck.SetCheck(0);
						EnableTrace(FALSE);
						m_TraceCheck.EnableWindow(FALSE);
					}
					m_Connect_btn.SetWindowText("CONNECT");
					m_OnOff_Indicator.SetBitmap(m_hOffLine);
					Trace = FALSE;
					EnableTrace(FALSE);
					m_Connect_btn.EnableWindow(FALSE);
					m_TraceCheck.EnableWindow(FALSE);
					m_OnOff_Indicator.EnableWindow(FALSE);
				}
			}
		}

		if (msg_type == MT_DB_REPLY)
		{
			p_msg_data = ptemsg_get_pte_msg_data( pMsg );
			data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
			
			if (msg_subtype == ST1_DB_SELECT)
			{
				switch  (data_type)
				{
				case  DM_CONFIG_DATA:
					{
						cdsapi_parse_dm_config(&DmConfig,pMsg);
						SetEvent(hGET_DM_CONFIG);
						break;
					}
				case TPDU_DATA:
					{
						cdsapi_parse_tpdu_record(&TPDUTable,pMsg);
						SetEvent(hGET_TPDU);
						break;
					}
				case ROUTER_DATA:
					{
						cdsapi_parse_router_record(&Router,pMsg);
						SetEvent(hGET_ROUTER);
						break;
					}
				case UCF01_DATA:
					{
						p_msg_data    = ptemsg_get_pte_msg_data( pMsg );
						p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
						data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

						p_data = sizeof(AUTH_TX) + p_data;
						//data_len = data_len - sizeof(AUTH_TX);
						memcpy(&ucf01, p_data, sizeof(UCF01));
						wait_f = false;
						
						break;
					}
				}
			}

			if (msg_subtype == ST1_DB_UPDATE)
			{
				result = ptemsg_get_result_code(pMsg);
				switch  (data_type)
				{
				case  DM_CONFIG_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hUPDATE_DM_CONFIG);
						break;
					}
				case TPDU_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hUPDATE_TPDU);
						break;
					}
				case ROUTER_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hUPDATE_ROUTER);
						break;
					}
				}
			}

			if (msg_subtype == ST1_DB_INSERT)
			{
				result = ptemsg_get_result_code(pMsg);
				switch  (data_type)
				{
				case TPDU_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hINSERT_TPDU);
						break;
					}
				case ROUTER_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hINSERT_ROUTER);
						break;
					}
				}
			}

			if (msg_subtype == ST1_DB_DELETE)
			{
				result = ptemsg_get_result_code(pMsg);
				switch  (data_type)
				{
				case TPDU_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hDELETE_TPDU);
						break;
					}
				case ROUTER_DATA:
					{
						if (result == PTEMSG_OK)
							SetEvent(hDELETE_ROUTER);
						break;
					}
				}
			}
		}

		if (msg_type == MT_INCOMING || msg_type == MT_OUTGOING)
		{
			orig_que = ptemsg_get_msg_orig_queue(pMsg);
			if (strcmp(_strupr(orig_que),_strupr(cur_que_name)) == 0)
			{
				if (msg_subtype == ST1_TRACE && Trace)
				{
					p_msg_data = ptemsg_get_pte_msg_data( pMsg );
					p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
					data_len = ptemsg_get_pte_msg_data_data_length( p_msg_data );

					ZeroMemory(MsgData,sizeof(MsgData));
					memcpy( MsgData, (char *)p_data, data_len );
					ltoa(data_len,MsgLen,10);

					WaitForSingleObject(hFlashColor,INFINITE);
					ResetEvent(hFlashColor);
        
               p_msg_data    = ptemsg_get_chained_pte_msg_data( pMsg, 2 );
               p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
               strncpy( tpdu_data, (pCHAR)p_data, data_len );

					if (msg_type == MT_INCOMING)	
					{
						//populate incomming part of the trace controls
						m_colorFlag = INCOMING;
						m_InMesLen.SetWindowText(MsgLen);
						m_InTPDU.SetWindowText(tpdu_data);
						m_InMessage.SetWindowText(MsgData);
						m_OutMessage.GetWindowText(temp);
						m_OutMessage.SetWindowText(temp);

					}

					if (msg_type == MT_OUTGOING)	
					{
						//populate outgoing part of the trace controls
						m_colorFlag = OUTGOING;
						m_OutMesLen.SetWindowText(MsgLen);
						m_OutTPDU.SetWindowText(tpdu_data);
						m_OutMessage.SetWindowText(MsgData);
						m_InMessage.GetWindowText(temp);
						m_InMessage.SetWindowText(temp);
					}
				}
			}
		}
	}
}


void CDialogPg::OnAboutButton() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();	
}

void CDialogPg::OnConnectButton() 
{
	CString Connect_lbl;
	m_Connect_btn.GetWindowText(Connect_lbl);

	if (Connect_lbl == "CONNECT")
	{
		// check if any of the pages are dirty, make user to apply changes first
		if (AppliedComSet == FALSE || AppliedTPDUSet == FALSE || AppliedRouterSet == FALSE)
			if (AfxMessageBox( "There are settings that were changed and not applyed.\nPress YES to apply new settings.\nPress NO to connect with old settings.", MB_YESNO | MB_ICONSTOP, 0 ) == IDYES)
			{
				m_PSheet->PressButton(PSBTN_APPLYNOW);
				if (AppliedComSet == TRUE && AppliedTPDUSet == TRUE && AppliedRouterSet == TRUE)
					m_Apply1_btn.EnableWindow(FALSE);
			}
		//send connect message and receive connect confirm message
		BeginWaitCursor();
		if (MsgToDialogMan(MT_COM_REQUEST,ST1_CONNECT) == FALSE)
			return;
		if (WaitForSingleObject(hCONNECT_CONFIRM,WAITRESP_TIME) != WAIT_OBJECT_0)
		{
			AfxMessageBox( "No response from service.", MB_OK | MB_ICONSTOP, 0 );
			return;
		}
		Connected = TRUE;
		m_Connect_btn.SetWindowText("DISCONNECT");
		m_OnOff_Indicator.SetBitmap(m_hOnLine);
		m_TraceCheck.EnableWindow(TRUE);
		EndWaitCursor();
	}

	if (Connect_lbl == "DISCONNECT")
	{
		stopflag = TRUE;
		if (Trace)
		{
			m_TraceCheck.SetCheck(0);
			OnTraceCheck();
		}
		
		m_TraceCheck.EnableWindow(FALSE);

		BeginWaitCursor();
		if (MsgToDialogMan(MT_COM_REQUEST,ST1_DISCONNECT) == FALSE)
			return;

      Sleep(5000);

		Connected = FALSE;
		m_Connect_btn.SetWindowText("CONNECT");
		m_OnOff_Indicator.SetBitmap(m_hOffLine);

		stopflag = FALSE;
		EndWaitCursor();
	}
}


BOOL CDialogPg::MsgToDialogMan(BYTE outmsgtype,BYTE msgsubtype)
{
	pPTE_MSG pMsg_out;
	char ErrMsg[100];
   CHAR buffer[256];

	BeginWaitCursor();
	pMsg_out = ptemsg_build_msg (outmsgtype,
								 msgsubtype,
								 0,
								 DM_que_name,
								 NULL_PTR,
								 NULL_PTR,
								 0,
								 0);
	if (pMsg_out == NULL_PTR)
	{
		AfxMessageBox( IDC_MSGBLDERROR, MB_OK | MB_ICONSTOP, 0 );
		EndWaitCursor();
		return FALSE;
	}
	else
   {
      if( !ptetcp_send( pMsg_out, ErrMsg ) )
      {
         sprintf( buffer, "Error sending request to %s: %s", DM_que_name, ErrMsg );
         AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	      free(pMsg_out);
         return FALSE;
      }
	   free(pMsg_out);
   }
	return TRUE;
}


BOOL CDialogPg::OnApply() 
{
	int i, NumServices = 0;
	char temp_que[21];

	if (AppliedComSet == FALSE)
	{
		//Alert DMS to get update DM_Config record from database
		//if (MsgToDialogMan(MT_UPDATE_TABLE,ST1_DMCONFIG) == FALSE)
			//return FALSE;
		MsgToDialogMan(MT_UPDATE_TABLE,ST1_DMCONFIG); // rs-071399
		AppliedComSet = TRUE;
	}

	if ( (AppliedTPDUSet == FALSE) || (AppliedRouterSet == FALSE) )
	{
		shownoerrors = TRUE;
		NumServices = DmConfig_list.num_returned;
		for (i = 0; i < NumServices; ++i)
		{
			//Get a queue name of the service
			strcpy(temp_que,(char *)DmConfig_list.dm_config_record[i].db_subsystem_name);
			memset(DM_que_name,0,sizeof(DM_que_name));
			memcpy(DM_que_name,temp_que,strlen(temp_que));
			strcat(DM_que_name,"A");

			if (AppliedTPDUSet == FALSE)
			{
				//Alert DMS to get update TPDU records from database
				//if (MsgToDialogMan(MT_UPDATE_TABLE,ST1_TPDU) == FALSE)
				//	return FALSE;
				//AppliedTPDUSet = TRUE;
				MsgToDialogMan(MT_UPDATE_TABLE,ST1_TPDU); // rs-071399
				//
				// Why the Sleep()? - Because the current thread goes out
				// of scope before the I/O operations have a chance to finish..
				//
				Sleep(750); 
			}

			if (AppliedRouterSet == FALSE)
			{
				//Alert DMS to get update Router table records from database
				//if (MsgToDialogMan(MT_UPDATE_TABLE,ST1_ROUTER) == FALSE)
				//	return FALSE;
				//AppliedRouterSet = TRUE;
				MsgToDialogMan(MT_UPDATE_TABLE,ST1_ROUTER); // rs-071399
				//
				// Why the Sleep()? - Because the current thread goes out
				// of scope before the I/O operations have a chance to finish..
				//
				Sleep(750); 
			}
		}
		AppliedTPDUSet = TRUE;		// rs-071399
		AppliedRouterSet = TRUE;	// rs-071399

		strcpy(DM_que_name,cur_que_name);
	}

	shownoerrors = FALSE;

	return CPropertyPage::OnApply();
}



void CDialogPg::OnCancel() 
{

	CPropertyPage::OnCancel();
	m_PSheet->DestroyWindow();
}

void CDialogPg::ExitApp() 
{
	BeginWaitCursor();
	
   if (Trace)
   {
	   m_TraceCheck.SetCheck(0);
		OnTraceCheck();
   }

	//strcpy(DM_que_name,GUI_que_name);
	MsgToDialogMan(MT_COM_REQUEST,ST1_EXIT);
	strcpy(DM_que_name,cur_que_name);

	if( hThread != NULL )
	{
		Sleep(2000);
		TerminateThread( hThread, 0 );
		hThread = NULL;
	}

	EndWaitCursor();
}

void CDialogPg::MinMaxWin() 
{
	if (Trace)
	{
		m_TraceCheck.SetCheck(0);
		OnTraceCheck();
	}

}


HBRUSH CDialogPg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX)
	{
		if (pWnd == &m_InMessage)
		{
			switch (m_colorFlag)
			{
			case INCOMING:
				// Set background color to green 
				pDC->SetBkColor(RGB(0,255,0));
			break;
			case OUTGOING:
			case CLEARTRACE:
				// Set background color to white 
				pDC->SetBkColor(RGB(255, 255, 255));
			break;
			}
			SetEvent(hFlashColor);
			return (HBRUSH)(m_pEditBkBrush.GetSafeHandle());
		}

		if (pWnd == &m_OutMessage)
		{
			switch (m_colorFlag)
			{
			case INCOMING:
			case CLEARTRACE:
				// Set background color to white 
				pDC->SetBkColor(RGB(255,255,255));
			break;
			case OUTGOING:
				// Set background color to yellow 
				pDC->SetBkColor(RGB(255,255,0));
			break;
			}
			SetEvent(hFlashColor);
			return (HBRUSH)(m_pEditBkBrush.GetSafeHandle());
		}
	}

        return CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

}


void CDialogPg::OnSetfocusInmessageEd() 
{
	m_Connect_btn.SetFocus();	
}

void CDialogPg::OnSetfocusInmeslenEd() 
{
	m_Connect_btn.SetFocus();	
	
}

void CDialogPg::OnSetfocusIntpduEd() 
{
	m_Connect_btn.SetFocus();	
	
}

void CDialogPg::OnSetfocusOutmeslenEd() 
{
	m_Connect_btn.SetFocus();	
	
}

void CDialogPg::OnSetfocusOutmessageEd() 
{
	m_Connect_btn.SetFocus();	
	
}

void CDialogPg::OnSetfocusOuttpduEd() 
{
	m_Connect_btn.SetFocus();	
}

void CDialogPg::OnSetfocusTraceFnameEd() 
{
	m_Connect_btn.SetFocus();	
	
}


void CDialogPg::OnClose1() 
{
	//Delete the associated Mutex lock 
	shrmem.DestroySharedMemorySegment();
	shrmem.DeleteMutexLock();	

	m_PSheet->PressButton(PSBTN_CANCEL);
	// TODO: Add extra cleanup here
}

void CDialogPg::OnApply1() 
{
	m_PSheet->PressButton(PSBTN_APPLYNOW);
	if (AppliedComSet == TRUE && AppliedTPDUSet == TRUE && AppliedRouterSet == TRUE)
		m_Apply1_btn.EnableWindow(FALSE);
}

void CDialogPg::OnHelp1() 
{
	m_PSheet->PressButton(PSBTN_HELP);
	
}

BOOL CDialogPg::OnSetActive() 
{
	if (NewComSetup)
	{
		m_Services_lst.DeleteString(m_Services_lst.GetCurSel());
		m_Services_lst.AddString(Servname);
		m_Services_lst.SelectString(-1,Servname);
		NewComSetup = FALSE;
	}


	if (AppliedComSet == FALSE || AppliedTPDUSet == FALSE || AppliedRouterSet == FALSE)
		m_Apply1_btn.EnableWindow(TRUE);
	else
		m_Apply1_btn.EnableWindow(FALSE);
	
	return CPropertyPage::OnSetActive();
}

void CDialogPg::OnSelchangeServicesList() 
{
	char *comma;
	int n;
	char temp_que[MAX_QUE_NAME_SIZE];
	BOOL ret = TRUE;

   // if trace is on for the currently selected service, make sure that it is
   // turned off before any other service can be selected.
   if( Trace )
   {
      AfxMessageBox( "Trace should be turned off before selecting another service", 
                     MB_ICONSTOP | MB_OK );
		m_Services_lst.SetCurSel(Servsel);
      return;
   }

	strcpy(temp_que,DM_que_name);

	//Get a queue name of selected DMS
	m_Services_lst.GetLBText(m_Services_lst.GetCurSel(),Servname);

	comma = strchr(Servname,',');
	memset(DM_que_name,0,sizeof(DM_que_name));
	memcpy(DM_que_name,Servname,comma - Servname);
	strcat(DM_que_name,"A");
	strcpy(cur_que_name,DM_que_name);

	//Get connection state from DMS
	if (!MsgToDialogMan(MT_COM_REQUEST,ST1_STATUS))
	{
		ret = FALSE;
	}
	else
	{
		if (!(WaitForSingleObject(hCOM_STATUS,WAITRESP_TIME) == WAIT_OBJECT_0))
		{
			AfxMessageBox( "No response from service.", MB_OK | MB_ICONSTOP, 0 );
			ret = FALSE;
		}
		else
		{
			ResetEvent(hCOM_STATUS);

			// Run function to get data from DMS into DmConfig structure
			ZeroMemory(&DmConfig,sizeof(DmConfig));
			n = comma - Servname;
			memcpy((char *)DmConfig.db_subsystem_name,Servname,n);
			if (cdsapi_get_dm_config_snd ( &DmConfig, ErrMsg ) != PTEMSG_OK)
			{
				AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
				ret = FALSE;
			}
			else
			{
				if (!(WaitForSingleObject(hGET_DM_CONFIG,WAITRESP_TIME) == WAIT_OBJECT_0))
				{
					AfxMessageBox( "Failed to retrieve current setup (dm_config).", MB_OK | MB_ICONSTOP, 0 );
					ret = FALSE;
				}
				else
				{
					ResetEvent(hGET_DM_CONFIG);

					if (Connected == TRUE)
					{
						m_OnOff_Indicator.SetBitmap(m_hOnLine);	
						m_Connect_btn.SetWindowText("DISCONNECT");
						m_TraceCheck.EnableWindow(TRUE);


						if (Trace)
						{
							m_TraceCheck.SetCheck(1);
							EnableTrace(TRUE);
							SetTraceFName();
						}
						else
						{
							m_TraceCheck.SetCheck(0);
							EnableTrace(FALSE);
						}
					}
					else
					{
						m_OnOff_Indicator.SetBitmap(m_hOffLine);	
						m_Connect_btn.SetWindowText("CONNECT");
						m_TraceCheck.EnableWindow(FALSE);
						
					}
				}
			}
		}
	}
	if (!ret)
	{
		m_Services_lst.SetCurSel(Servsel);
		m_Services_lst.GetLBText(Servsel,Servname);
		strcpy(DM_que_name,temp_que);
		strcpy(cur_que_name,temp_que);
	}
	else
	{
		Servsel = m_Services_lst.GetCurSel();
		m_Connect_btn.EnableWindow(TRUE);
		m_OnOff_Indicator.EnableWindow(TRUE);
	}

	return;
}

void CDialogPg::OnRefresh() 
{
   m_Services_lst.GetLBText(m_Services_lst.GetCurSel(),Servname);
   if (strlen(Servname))
   {
      OnSelchangeServicesList(); 	
   }
   else
   {
		AfxMessageBox( "Select a service to monitor.", MB_OK | MB_ICONSTOP, 0 );
   }
}

int CDialogPg::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CDialogPg::StopTimer()
{
	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}


LRESULT CDialogPg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
    {	

      case WM_TIMER:
		{
			cntr++;
			
			shm_time = shrmem.GetLastUpdateTime();

			if(shm_time > c_time)
			{
				c_time = shm_time;
				ResetCntr();
			}
			if ( cntr == idle_time/2 )
			{
				shrmem.SetIdleStatus(0);
			}
			//Check whether Max Idle Time has elapsed
			if(cntr >= idle_time)
			{
				ret = LockScreen();

				if(ret == -1 || ret == -2)
				{				
					::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
				}
			}
		}
		break;
	
		//On X button press of Dialog
		case WM_CLOSE:				
				
				StopTimer();
				//Delete the SHM segment & Delete Mutex Lock
				shrmem.DestroySharedMemorySegment();
				shrmem.DeleteMutexLock();
				
				break;

	 default:break;	

	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CDialogPg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	ResetCntr();
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


short CDialogPg::LockScreen()
{
	int retcode = 0;

	CLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			AfxMessageBox("Session Timeout. Please Login.");
			//::MessageBox(this->m_hWnd,"Session Timeout. Please Login","Dialog Manager",MB_OK);
			
			ret = idleloginfrm.CheckLoginDetails();

			if(ret == CANCEL)
			{
				//Update SHM with IdleStatus as 2 meaning to Close all the Apps
				shrmem.SetIdleStatus(2);
				
				retcode = -1;			
			}
			else
			{				
				shrmem.SetIdleStatus(1);
				retcode =  1;
			}
		}
		else if (status == 1)
		{
				retcode = 0;
			// Continue. Password check was successful.
		}
		else if (status == 2)
		{
			retcode =  -1;			
		}
		else
		{
			retcode = -2;
			LogEvent("Unkown SHM Status",ERROR_MSG);
			// TODO:  Error case - log a message. Status is out of sync.
		}
		//After Login Validation is successful, release the Mutex Lock
		pLock->Unlock();
	}

	StartTimer(1000);

	return retcode;	
}


void CDialogPg::ResetCntr()
{
	cntr = 0;
}

void CAboutDlg::OnAboutBoxOK() 
{
	// TODO: Add your control notification handler code here
	
}
