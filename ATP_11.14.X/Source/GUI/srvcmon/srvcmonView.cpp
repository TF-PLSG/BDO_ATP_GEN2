/****************************************************************************************

   Module: srvcmonView.cpp

   Title: Ascendent Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\srvcmonView.cpp  $
   
	   Rev 1.9     May 12.5.11          TF@Krishna Kiran S
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
	 Added Creation of SHM & Mutex for Synchronisation of dislpalying Login
	 Form from different windows

      Rev 1.8   Oct 07 1999 07:09:00   MSALEH
   Disconnect socket on shutdown
   
      Rev 1.7   Aug 27 1999 16:38:30   MSALEH
   1) increase the main list size to show
   more services
   
      Rev 1.6   Mar 03 1999 15:16:38   MSALEH
   When shutting down ascendent, follow
   the list in reverse starting with the last
   service
   
      Rev 1.5   Nov 18 1998 10:05:40   MSALEH
   Ran through Purify and corrected some
   calls to free()
   
      Rev 1.4   Nov 17 1998 12:12:36   MSALEH
   Added new functionality to manager
   logger/reply Primary/secondary designation,
   it works with logger usiong special message
   subtypes under MT_SYSTEM_REQUEST
   
      Rev 1.3   Sep 09 1998 15:37:22   MSALEH
   added functionality top srvcmgr
   to ping all registered applications
   and to shutdown ascendent if the number
   of failed pings per process reached
   the registry var MaxPingRetries.
   The interval between pings is controlled
   by the registry var FailoverPingInterval, to 
   disable this functionalty, set this var to 0
   
      Rev 1.2   Sep 04 1998 13:10:58   skatuka2
   1. changed 'pinnacle' to 'ascendent'
   
      Rev 1.1   20 Aug 1998 09:04:00   rmalathk
   minor bug fixes and changes needed for applink.
   
      Rev 1.0   27 May 1998 16:05:22   rmalathk
   Initial Revision
   
*****************************************************************************************/
// srvcmonView.cpp : implementation of the CSrvcmonView class
//

#include "stdafx.h"
#include "srvcmon.h"

#include "srvcmonDoc.h"
#include "srvcmonView.h"
#include "sysconfig.h"
#include "IdleLoginfrm.h"
#include "SharedMem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern "C"
{
#include "srvccomm.h"
#include "ntutils.h"
#include "pte.h"
#include "ptetcp.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "TXDSAPI.h"
}

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonView

CIdleLoginfrm loginfrm;
CString idle_time_val_str;
CSharedMem shrmem;
extern CString cmdlinestr;

extern  char u_name[51];
extern  char u_pwd[51];

extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
   CHAR appProductVersion[MAX_APPVERSION_SIZE];
}		

CString szNetworkControllers = "ncvis3:ncposa:ncpos:ncpos2:nposa2:natma2:ncatma:ncatm:ncposa2:neqit2:ncamex:nccirs:fguard:ncjcb:ncjcb2:ncmcr2:ncmcrd:dcpiso:natma2:ncamaq:nceqit:ncvis2:ncvisa:ncvsms:nccup:nccup2:nciftr:dcpimp";

IMPLEMENT_DYNCREATE(CSrvcmonView, CFormView)

BEGIN_MESSAGE_MAP(CSrvcmonView, CFormView)
	//{{AFX_MSG_MAP(CSrvcmonView)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_STARTALL, OnStartall)
	ON_BN_CLICKED(IDC_STOPALL, OnStopall)
	ON_BN_CLICKED(IDC_TRACEOFF, OnTraceoff)
	ON_BN_CLICKED(IDC_TRACEON, OnTraceon)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_BN_CLICKED(IDC_PINGALL, OnPingall)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_SERVICE_TAB, OnSelchangeServiceTab)
	ON_BN_CLICKED(IDC_SYS_CONFIG, OnSysConfig)
	ON_BN_CLICKED(IDC_ENCRYPT_ON, OnEncryptOn)
	ON_BN_CLICKED(IDC_ENCRYPT_OFF, OnEncryptOff)
	ON_BN_CLICKED(IDC_MRA_ON, OnMraOn)
	ON_BN_CLICKED(IDC_MRA_OFF, OnMraOff)
	ON_BN_CLICKED(IDC_FALCON_ON, OnFalconOn)
	ON_BN_CLICKED(IDC_FALCON_OFF, OnFalconOff)
	ON_BN_CLICKED(IDC_NULL_CHECK_ON,OnNullcheckOn )
	ON_BN_CLICKED(IDC_NULL_CHECK_OFF, OnNullcheckOff)
	ON_BN_CLICKED(IDC_DB_STATICS_ON, OnDbstaticsOn )
	ON_BN_CLICKED(IDC_DB_STATICS_OFF, OnDbstaticsOff)
	ON_BN_CLICKED(IDC_LOAD_RPS_FLAG, OnLoadRpsFlag)
	ON_BN_CLICKED(IDC_LOAD_TO_RC_VALUES, OnLoadToRcValue_forallcardbrands)
	ON_BN_CLICKED(IDC_LOAD_FAI_DECLINE_RC_VALUES, OnLoadFaiDeclineRcValue_forallcardbrands)
	ON_BN_CLICKED(IDC_LOAD_NC_TO_REV_ATTEMPTS_VALUE,OnLoadReversalattepmts_for_TORequest)
	ON_BN_CLICKED(IDC_LOAD_NC_LATE_RESP_REV_ATTEMPTS_VALUE,OnLoadReversalattepmts_for_LateResponse)
	ON_BN_CLICKED(IDC_RELOAD_OBS, OnBnClickedReloadObs)
	ON_BN_CLICKED(IDC_RELOAD_VISA3DS2, OnBnClickedReloadVisa3ds2)
	ON_BN_CLICKED(IDC_RELOAD_AMEX3DS2, OnBnClickedReloadAmex3ds2)
	ON_BN_CLICKED(IDC_RELOAD_JCB3DS2, OnBnClickedReloadJcb3ds2)
	ON_BN_CLICKED(IDC_RELOAD_DINERS3DS2, OnBnClickedReloadDiners3ds2)
	ON_BN_CLICKED(IDC_RELOAD_CUP3DS2, OnBnClickedReloadCup3ds2)
	ON_BN_CLICKED(IDC_RELOAD_MIDCPNG_TFINI, OnBnClickedReloadMIDCpng)
	ON_BN_CLICKED(IDC_FALCONRT_ON, OnFalconRTOn )
	ON_BN_CLICKED(IDC_FALCONRT_OFF, OnFalconRTOff)
	ON_BN_CLICKED(IDC_FALCONRT_RELOAD, OnReLoadFalcon)
	ON_BN_CLICKED(IDC_SECONDARYEXPIRYDATEVAL_RELOAD, OnReLoadSecExpiryDateValidationFlag)
	ON_LBN_SELCHANGE(IDC_SERVICE_LIST, OnSelchangeServiceList)

	//}}AFX_MSG_MAP


	ON_BN_CLICKED(IDC_BUTTON1, &CSrvcmonView::OnClickedVertexonCardStatus)
	ON_BN_CLICKED(IDC_FRT_EXT10_ON, OnFalconRT_EXT10On)
	ON_BN_CLICKED(IDC_FRT_EXT10_OFF, OnFalconRT_EXT10Off)
	ON_BN_CLICKED(IDC_KEYBLOCK_ON, OnKeyblockOn)
	ON_BN_CLICKED(IDC_KEYBLOCK_OFF, OnKeyblockOff)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonView construction/destruction

CSrvcmonView::CSrvcmonView()
	: CFormView(CSrvcmonView::IDD)
{
	//{{AFX_DATA_INIT(CSrvcmonView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_pImageList = NULL ;
	//Set the pointer to the application object 
	m_pApplicationObject = (CSrvcmonApp*) AfxGetApp() ;

   m_FirstUpdate = 0;

   //Instantiate the Object for SHM
	pLock = new CDMLock("IdleLock");
}

CSrvcmonView::~CSrvcmonView()
{
   TerminateLogins();
   
   delete pLock;
   pLock = NULL;
}

void CSrvcmonView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSrvcmonView)
	DDX_Control(pDX, IDC_ENCRYPT_ON, m_CntlEncOn);
	DDX_Control(pDX, IDC_ENCRYPT_OFF, m_CntlEncOff);
	DDX_Control(pDX, IDC_FALCON_OFF, m_CntlFalconOff);
	DDX_Control(pDX, IDC_FALCON_ON, m_CntlFalconOn);
	DDX_Control(pDX, IDC_MRA_OFF, m_CntlMraOff);
	DDX_Control(pDX, IDC_MRA_ON, m_CntlMraOn);
	DDX_Control(pDX, IDC_NULL_CHECK_ON, m_CntlNullcheckOn);
	DDX_Control(pDX, IDC_NULL_CHECK_OFF, m_CntlNullcheckOff);
	DDX_Control(pDX, IDC_DB_STATICS_ON, m_CntlDbstaticsOn);
	DDX_Control(pDX, IDC_DB_STATICS_OFF, m_CntlDbstaticsOff);
	DDX_Control(pDX, IDC_SERVICE_LIST, m_ServiceList);
	DDX_Control(pDX, IDC_LOAD_RPS_FLAG, m_CntlLoadRpsflag);
	DDX_Control(pDX, IDC_RELOAD_OBS, m_CntlLoadOBSrecords);
	DDX_Control(pDX, IDC_RELOAD_VISA3DS2, m_CntlLoadVISA3DS2records);
	DDX_Control(pDX, IDC_RELOAD_AMEX3DS2, m_CntlLoadAMEX3DS2records);
	DDX_Control(pDX, IDC_RELOAD_DINERS3DS2, m_CntlLoadDINERS3DS2records);
	DDX_Control(pDX, IDC_RELOAD_JCB3DS2, m_CntlLoadJCB3DS2records);
	DDX_Control(pDX, IDC_RELOAD_CUP3DS2, m_CntlLoadCUP3DS2records);
	DDX_Control(pDX, IDC_RELOAD_MIDCPNG_TFINI, m_CntlLoadMIDCpngrecords);
	DDX_Control(pDX, IDC_FALCONRT_ON, m_CntlFalconRTOn);
	DDX_Control(pDX, IDC_FALCONRT_OFF, m_CntlFalconRTOff);
	DDX_Control(pDX, IDC_FALCONRT_RELOAD, m_CntlReLoadFalconflag);
	DDX_Control(pDX,IDC_LOAD_TO_RC_VALUES, m_CntlLoadToRcValue);
	DDX_Control(pDX,IDC_LOAD_FAI_DECLINE_RC_VALUES, m_CntlLoadFaiDeclineRcValue);
	DDX_Control(pDX,IDC_LOAD_NC_TO_REV_ATTEMPTS_VALUE,m_CntlLoadReversalattepmtsTO);
	DDX_Control(pDX,IDC_LOAD_NC_LATE_RESP_REV_ATTEMPTS_VALUE,m_CntlLoadReversalattepmtsLR);
	DDX_Control(pDX, IDC_SECONDARYEXPIRYDATEVAL_RELOAD, m_CntlLoadSecExpiryDateValidation);
	DDX_Control(pDX, IDC_SERVICE_TAB, m_MainTab);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_VCARD_STATUS, m_VertexonCardStatus);
	DDX_Control(pDX, IDC_FRT_EXT10_ON, m_CntlFalconRT_EXT10On);
	DDX_Control(pDX, IDC_FRT_EXT10_OFF, m_CntlFalconRT_EXT10Off);
	DDX_Control(pDX, IDC_KEYBLOCK_ON, m_CntlKeyblockOn);
	DDX_Control(pDX, IDC_KEYBLOCK_OFF, m_CntlKeyblockOff);

}

BOOL CSrvcmonView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonView diagnostics

#ifdef _DEBUG
void CSrvcmonView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSrvcmonView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

/*

CSrvcmonDoc* CSrvcmonView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSrvcmonDoc)));
	return (CSrvcmonDoc*)m_pDocument;
}

*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonView message handlers
void CSrvcmonView::OnInitialUpdate() 
{
	TC_ITEM ItemDef;
	CHAR    TabTitle[40];
    INT     temp;
	CHAR	 ini_file[256];
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	
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

    ResetCntr();

    c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);



    m_StatusWindow = GetParentFrame();
	GetParentFrame()->RecalcLayout() ;
	ResizeParentToFit(FALSE) ;

   // read the registry, initialize the XIPC Logins
   InitLogins();


	// Create the image list for the tab control
	m_pImageList = new CImageList();		
	ASSERT(m_pImageList != NULL);    // no serious allocation failure checking
	m_pImageList->Create(32, 32, FALSE/*bMask*/,1/*nInitial*/, 1/*nGrow*/);

	m_pImageList->Add(m_pApplicationObject->LoadIcon(IDI_SETUP));
	m_MainTab.SetImageList(m_pImageList) ;
	

   // create the tabs based on the HighAvailability indicator
   sprintf( TabTitle, " %s", m_ThisMachine );
	ItemDef.mask = TCIF_TEXT|TCIF_IMAGE;
	ItemDef.pszText = TabTitle;
	ItemDef.cchTextMax = strlen( TabTitle );
	ItemDef.iImage = 0;
	if ( m_MainTab.InsertItem( 0, &ItemDef ) == -1 )
	   AfxMessageBox( " Tab Control construction Failed", MB_OK, 0);

   if( m_HighAvailability )
   {
      sprintf( TabTitle, " %s", m_OtherMachine );
		ItemDef.mask = TCIF_TEXT|TCIF_IMAGE;
	   ItemDef.pszText = TabTitle;
	   ItemDef.cchTextMax = strlen( TabTitle );
	   ItemDef.iImage = 0;
	   if ( m_MainTab.InsertItem( 1, &ItemDef ) == -1 )
		   AfxMessageBox( " Tab Control construction Failed", MB_OK, 0);
   }
    
    //set the tab stops for a list box
	CRect rectList;
	//CListBox *pList = (CListBox*) GetDlgItem(IDC_RESULTS);
	m_ServiceList.GetWindowRect(&rectList);

	INT  nTabStop[3];

	CDC* dc = GetDC();
	CRect size = rectList;
	CFont* pOldFont;

	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());

	// accurately measure the average width of a character
  	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
  		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);

	//  find the tab stop, being careful with rounding
	INT ListBoxWidth = ((rectList.Width()*4) / ((25+size.Width())/52));

   nTabStop[0] = ( ListBoxWidth * 42 ) / 100;
   nTabStop[1] = ( ListBoxWidth * 66 ) / 100;
   nTabStop[2] = ( ListBoxWidth * 80 ) / 100;

	
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);

	m_ServiceList.SetTabStops(3, nTabStop);

   // update the list box with the service info

   m_AutoRefreshOff = 0;

   // set the tab selection
   m_MainTab.SetCurSel(0);
   m_TabSelected = 0;


   for( temp = 0; temp < MAX_SERVICES; temp++ )
      strcpy( m_PingStatus [temp], " " );

   // populate display
   OnRefresh();


    
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::TerminateLogins() 
{
    srvccomm_disconnect();
}


//**********************************************************************
//**********************************************************************
void CSrvcmonView::InitLogins() 
{
	CHAR AppName[12] = { 0 };
	CHAR buffer[256] = { 0 };

    INT  login_failed=0;
	CHAR error_msg[100] = { 0 };

    GetAppName( AppName );
    login_failed = 0;

	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes

    ptetcp_init( error_msg );


    // get the primary machine id from registry, login into primary 
    // machine instance
    if( !GetFirstServerId( m_ThisMachine ) )
    {
        AfxMessageBox( "First Server Id not found in INI file", MB_ICONSTOP | MB_OK );
        strcpy( m_ThisMachine, "Local" );
        login_failed = 1;
    }

    if( !ptetcp_connect( m_ThisMachine, error_msg ) )
    {
      sprintf( buffer, "Unable to Connect to %s. Error: %s", m_ThisMachine, error_msg );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
      login_failed = 1;
    }

 
	//Krishna@tf, 19.5.11 : Validate the Commandline params & continue the App
	BYTE rcode;		
	UCF01 ucf01;	

	memset( &ucf01, 0, sizeof( UCF01 ) );

	 // next authenticate the user
	 strcpy( (char *)ucf01.primary_key.name, u_name);

	 rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
	 
	 if( rcode == PTEMSG_NOT_FOUND )
	 {   
		 ::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		//this->SendMessage(WM_CLOSE);
		//return;
	 }
	 else if( rcode != PTEMSG_OK )
	 {  
		 ::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		//this->SendMessage(WM_CLOSE);
		//return;
	 }

	// check password	
	if( strcmp( (char *)ucf01.password, u_pwd ) != 0 )
	{	
		::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
		//this->SendMessage(WM_CLOSE);
		//return;
	}   
	//End, Krishna changes




    //check if high availability configuration
    if( !GetHighAvailableIndicator( &m_HighAvailability ) )
    {
      AfxMessageBox( "High Availablity Indicator not Found in INI File", MB_ICONSTOP | MB_OK );
    }

    if( login_failed )
      sprintf( m_ConnectString, "Unable to Connect to Primary Server: %s", m_ThisMachine );
    else
      sprintf( m_ConnectString, "Connected to Primary Server: %s", m_ThisMachine );

    if( !m_HighAvailability )
      return;

    // repeat the above process for the secondary machine
    if( !GetSecondServerId( m_OtherMachine ) )
    {
      AfxMessageBox( "Second Server Id not found in INI file", MB_ICONSTOP | MB_OK );
      strcpy( m_OtherMachine, "Local" );
    }

    // disconnect from primary instance and login to secondary
    srvccomm_disconnect();

    if( !ptetcp_connect( m_OtherMachine, error_msg ) )
    {
      sprintf( buffer, "Unable to Connect to %s. Error: %s", m_OtherMachine, error_msg );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
      login_failed = 1;
    }


    // switch connection to the primary instance
    if( !srvccomm_change_connection( m_ThisMachine ) )
    {
      sprintf( buffer, "Unable to Reconnect to This Server" );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }

    sprintf( m_ConnectString, "Connected to This Server: %s", m_ThisMachine );

	
}

//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnStart() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected

    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Start", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has already been started
    if( m_ServiceTable [index].running == '1' )
    {
        sprintf( buffer, "Service %s is already started.", m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }


    // check if service has been disabled
    if( m_ServiceTable [index].disable == '1' )
    {
        sprintf( buffer, "Cannot start %s. Service is disabled.", m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    sprintf( buffer, "Attempting to start %s..........", m_ServiceTable [index].display_name );
    m_StatusWindow->SetMessageText( buffer );
    m_StatusWindow->UpdateWindow();

    BeginWaitCursor();

    // send a message to service manager to start the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_START,
                                      err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcat( buffer, "OK" );
            m_StatusWindow->SetMessageText( buffer );
            m_ServiceTable [index].running = '1';
            RefreshList();
            break;

        case PTEMSG_SRVC_RUNNING:
            m_StatusWindow->SetMessageText( "Service was already running. Synchronized with \
                                           Service Manager" );                                                                                                
            m_ServiceTable [index].running = '1';
            RefreshList();
            sprintf( buffer, "Service %s is already started.", m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;
	
        case PTEMSG_SRVC_STARTFAILED:
            sprintf( buffer, "Unable to start %s, check event log on Ascendent server",
                     m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;

        default:
            AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
    }

    //m_StatusWindow->SetMessageText( m_ConnectString );
    //m_StatusWindow->UpdateWindow();
    m_AutoRefreshOff = 0;
}




//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnStop() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Stop", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to stop %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service has already been stopped
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service %s is already stopped", m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    sprintf( buffer, "Attempting to stop %s..........", m_ServiceTable [index].display_name );
    m_StatusWindow->SetMessageText( buffer );
    m_StatusWindow->UpdateWindow();
    BeginWaitCursor();

    Sleep( 500 );

    // send a message to service manager to start the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_STOP,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcat( buffer, "OK" );
            m_StatusWindow->SetMessageText( buffer );
            m_ServiceTable [index].trace_on = '0';
            RefreshList();
            break;

        case PTEMSG_SRVC_STOPPED:
            m_ServiceTable [index].running = '0';
            m_ServiceTable [index].trace_on = '0';
            RefreshList();
            sprintf( buffer, "Service %s is already stopped", m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;
	
        case PTEMSG_SRVC_STOPFAILED:
            sprintf( buffer, "Unable to stop %s, check system log on Ascendent server",
                     m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;

        default:
            AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
    }

    //m_StatusWindow->SetMessageText( m_ConnectString );
    //m_StatusWindow->UpdateWindow();
    m_AutoRefreshOff = 0;
    
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnStartall() 
{
	// TODO: Add your control notification handler code here
    CHAR err_msg[100], buffer[256];
    BYTE rcode;
    INT  temp;

    // get confirmation from user
    sprintf( buffer, "Do you wish to start all Ascendent Services?" );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to start all services
    rcode = srvccomm_process_all( ST1_SRVC_STARTALL, err_msg );

    if( rcode != PTEMSG_OK )
    {
        AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
        m_AutoRefreshOff = 0;
        EndWaitCursor();
        return;
    }

    // now get confirmation for each service
    for( temp = 0; temp < m_NumServices; temp++ )
    {
        m_ServiceList.SetCurSel( temp );
        rcode = srvccomm_confirm_status( err_msg );

        if( rcode == PTEMSG_OK )
        {
            m_ServiceTable [temp].running = '1';
            RefreshList();
            m_ServiceList.SetCurSel( temp );
        }
        else if( rcode == PTEMSG_SRVC_STARTFAILED )
        {
            m_ServiceTable [temp].running = 'F';
            RefreshList();
            m_ServiceList.SetCurSel( temp );
        }
        Sleep( 1000 );
    }

    RefreshList();
    EndWaitCursor();
    m_AutoRefreshOff = 0;
	
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnStopall() 
{
	// TODO: Add your control notification handler code here
    CHAR err_msg[100], buffer[256];
    BYTE rcode;
    INT  temp;


    // get confirmation from user
    sprintf( buffer, "Do you wish to stop all Ascendent Services?" );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to start all services
    rcode = srvccomm_process_all( ST1_SRVC_STOPALL, err_msg );

    if( rcode != PTEMSG_OK )
    {
        AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
        m_AutoRefreshOff = 0;
        EndWaitCursor();
        return;
    }

    Sleep(2000);

    // now get confirmation for each service
    for( temp = (m_NumServices - 1); temp >= 0; temp-- )
    {
        m_ServiceList.SetCurSel( temp );
        rcode = srvccomm_confirm_status( err_msg );

        if( rcode == PTEMSG_OK )
        {
            m_ServiceTable [temp].running = '0';
            m_ServiceTable [temp].trace_on = '0';
            RefreshList();
            m_ServiceList.SetCurSel( temp );
        }
        else if( rcode == PTEMSG_SRVC_STOPFAILED )
        {
            m_ServiceTable [temp].running = 'P';
            RefreshList();
            m_ServiceList.SetCurSel( temp );
        }
        Sleep( 1000 );
    }

    RefreshList();
    m_AutoRefreshOff = 0;
    EndWaitCursor();		
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnTraceoff() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to disable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to disable trace for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service is not started. Trace is currently disabled." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to disable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_TRACEOFF,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].trace_on = '0';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending Trace Off request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		
	
    m_AutoRefreshOff = 0;
}


//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnTraceon() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to enable trace for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to enable trace." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_TRACEON,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].trace_on = '1';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending Trace On request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		

    m_AutoRefreshOff = 0;
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnClose() 
{
	// TODO: Add your control notification handler code here
	GetParentFrame()->DestroyWindow() ;
    ptetcp_disconnect();

   // TODO: Add extra cleanup here
    //StopTimer();
				
   //Delete the associated Mutex lock 
   shrmem.DestroySharedMemorySegment();
   shrmem.DeleteMutexLock();
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnDisable() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Disable", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running == '1' )
    {
        sprintf( buffer, "Service %s is started. Service should be stopped first",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to start the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_DISABLE,
                                      err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].disable = '1';
            RefreshList();
            break;

        case PTEMSG_SRVC_RUNNING:
            m_ServiceTable [index].running = '1';
            RefreshList();
            sprintf( buffer, "Service %s is started. Cannot disable a started service.",
                     m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;
	
        default:
            AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
    }	

    m_AutoRefreshOff = 0;
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnEnable() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Enable", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been already enabled
    if( m_ServiceTable [index].disable == '0' )
        return;

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to start the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_ENABLE,
                                      err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].disable = '0';
            RefreshList();
            break;

        default:
            AfxMessageBox( err_msg, MB_ICONSTOP | MB_OK );
    }		

    m_AutoRefreshOff = 0;
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnPing() 
{
	// TODO: Add your control notification handler code here
    INT index=0;
    CHAR err_msg[1000] = {0};
	CHAR buffer[256] = {0};
    BYTE rcode=0;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_ping_service( m_ServiceTable [index].que_name, err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnPingall() 
{
	// TODO: Add your control notification handler code here
    CHAR err_msg[100], buffer[256];
    BYTE rcode;
    INT  temp;


    // get confirmation from user
    sprintf( buffer, "Do you wish to ping all Ascendent Services?" );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // ping all the running services one at a time
    for( temp = 0; temp < m_NumServices; temp++ )
    {

        if( m_ServiceTable [temp].running != '1' )
            continue;

        // try to ping the service
        m_ServiceList.SetCurSel( temp );
        rcode = srvccomm_ping_service( m_ServiceTable [temp].que_name, err_msg );


        // process the response from service manager
        switch( rcode )
        {
            case PTEMSG_OK:
                strcpy( m_PingStatus [temp], "Ping OK" );
                RefreshList();
                m_ServiceList.SetCurSel( temp );
                strcpy( m_PingStatus [temp], " " );
                break;

            default:
                strcpy( m_PingStatus [temp], "Ping Failed" );
                RefreshList();
                m_ServiceList.SetCurSel( temp );
                strcpy( m_PingStatus [temp], " " );
        }	
        Sleep( 1000 );

		//Krishna@TF, 31.5.11, Update time stamp in SHM for every ping activity
		UpdateLastEventUpdateTime();
    }

    RefreshList();
    m_AutoRefreshOff = 0;
    EndWaitCursor();	
}



//**********************************************************************
//**********************************************************************
void CSrvcmonView::OnRefresh() 
{
	// TODO: Add your control notification handler code here
    CHAR ErrMsg[100], buffer[256];

    // get the new service table from service manager
    memset( m_TempTable, 0, sizeof( m_TempTable ) );
    if( srvccomm_get_service_table( m_TempTable, &m_NumServices,  ErrMsg ) != PTEMSG_OK )
    {
        sprintf( buffer, "Unable to get Service Info from ServiceManager - %s", ErrMsg );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    // check if it has changed, update list box if so
    if( memcmp( m_TempTable, m_ServiceTable, sizeof( m_TempTable ) ) != 0 )
    {
        memcpy( m_ServiceTable, m_TempTable, sizeof( m_TempTable ) );
        RefreshList();
    }

	return;
}



//********************************************************************
//********************************************************************
void CSrvcmonView::RefreshList() 
{
   CHAR run_status[12], trace_status[12];
   INT  temp;
   CHAR buffer[256];

   int lastTopIndex = m_ServiceList.GetTopIndex();

	m_ServiceList.ResetContent();
   for( temp = 0; temp < m_NumServices; temp++ )
   {
       //grab the display name, service status and trace status
       if( m_ServiceTable [temp].disable == '1' )
           strcpy( run_status, "Disabled" );
       else if( m_ServiceTable [temp].running == 'F' )
           strcpy( run_status, "Start Failed" );
       else if( m_ServiceTable [temp].running == 'P' )
           strcpy( run_status, "Stop Failed" );
       else if( m_ServiceTable [temp].running == '1' )
           strcpy( run_status, "Started" );
       else
           strcpy( run_status, "Stopped" );

       if( m_ServiceTable [temp].trace_on == '1' )
           strcpy( trace_status, "ON" );
       else
           strcpy( trace_status, "OFF" );

       //m_ServiceList.SetTabStops( 1 );
       //sprintf( buffer, "%s\t\t%s\t\t%s\t       %s",
       sprintf( buffer, "%s\t%s\t%s\t%s",
                         m_ServiceTable [temp].display_name,
                         run_status, trace_status, m_PingStatus [temp] );
       m_ServiceList.AddString( buffer );
   }

   m_ServiceList.SetTopIndex(lastTopIndex);
   m_ServiceList.UpdateWindow();
}



//********************************************************************
//********************************************************************
void CSrvcmonView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
   //CHAR buffer[256];

   // if we are in the middle of something, don't refresh
   /* Disable auto refresh
   if( !m_AutoRefreshOff )
        OnRefresh();
   */

/*    
    if( !m_FirstUpdate )
    {
        m_FirstUpdate = 1;
        sprintf( buffer, "Connected to Primary Server: %s", m_ThisMachine );
        m_StatusWindow->SetMessageText( buffer );
    }
*/
   m_StatusWindow->SetMessageText( m_ConnectString );

   CFormView::OnTimer(nIDEvent);
}



//********************************************************************
//********************************************************************
void CSrvcmonView::ModifyList( pSERVICE_INFO service_info, INT index ) 
{
    CHAR run_status[12], trace_status[12];
    CHAR buffer[256];

    if( service_info->disable == '1' )
        strcpy( run_status, "Disabled" );
    else if( service_info->running == 'F' )
        strcpy( run_status, "Start Failed" );
    else if( service_info->running == 'P' )
        strcpy( run_status, "Stop Failed" );
    else if( service_info->running == '1' )
        strcpy( run_status, "Started" );
    else
        strcpy( run_status, "Stopped" );

    if( service_info->trace_on == '1' )
        strcpy( trace_status, "ON" );
    else
        strcpy( trace_status, "OFF" );

    sprintf( buffer, "%s\t\t%s\t\t%s",
                      service_info->display_name,
                      run_status, trace_status );

    m_ServiceList.DeleteString( index );
    m_ServiceList.InsertString( index, buffer );
}




//********************************************************************
//********************************************************************
void CSrvcmonView::OnSelchangeServiceTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
   INT nIndex, ret, temp;
   CHAR buffer[100];
   CHAR ErrMsg[100];

   // clear the list box
   m_ServiceList.ResetContent();

   // switch the instance connection based on the tab selected.
	nIndex = m_MainTab.GetCurSel();
   m_TabSelected = nIndex;

   if( nIndex == 0 )
   {
     ret = srvccomm_change_connection( m_ThisMachine );
     sprintf( m_ConnectString, "Connected to Primary Server: %s", m_ThisMachine );
   }
   else
   {
     ret = srvccomm_change_connection( m_OtherMachine );
     sprintf( m_ConnectString, "Connected to Other Server: %s", m_OtherMachine );
   }

   if( !ret )
   {
     if( nIndex == 0 )
         sprintf( buffer, "Unable to connect to Primary Server %s", m_ThisMachine );
     else
         sprintf( buffer, "Unable to connect to Other Server %s", m_OtherMachine );
      
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }

    // get the initial service table from service manager.
    ZeroMemory( m_ServiceTable, sizeof( m_ServiceTable ) );
    if( srvccomm_get_service_table( m_ServiceTable, &m_NumServices,  ErrMsg ) != PTEMSG_OK )
    {
        sprintf( buffer, "Unable to get Service Info from ServiceManager - %s", ErrMsg );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    for( temp = 0; temp < MAX_SERVICES; temp++ )
        strcpy( m_PingStatus [temp], " " );
    RefreshList();

    m_StatusWindow->SetMessageText( m_ConnectString );
	
	*pResult = 0;
}

void CSrvcmonView::OnSysConfig() 
{
   BOOLEAN i;
   INT   res;
   INT   HighAvailability;
   CHAR  ThisMachineId     [MAX_SYSTEM_NAME_SIZE];
   CHAR  OtherMachineId    [MAX_SYSTEM_NAME_SIZE];
   CHAR  ThisXipcInstance  [MAX_SYSTEM_NAME_SIZE*2];
   CHAR  OtherXipcInstance [MAX_SYSTEM_NAME_SIZE*2];
   CHAR  ThisLoggerMSQueName [MAX_QUE_NAME_SIZE];

   CSysConfig SysConfig;

   i = GetHighAvailableIndicator(&HighAvailability);
   i = GetThisMachineId(ThisMachineId);
   i = GetOtherMachineId(OtherMachineId);
   i = GetThisXipcInstance(ThisXipcInstance);
   i = GetOtherXipcInstance(OtherXipcInstance);
   i = GetLoggerMSQueName(ThisLoggerMSQueName);

   i = SysConfig.PopulateSysConfigDialog( &HighAvailability,
                                          ThisMachineId,
                                          OtherMachineId, 
                                          ThisXipcInstance,
                                          OtherXipcInstance,
                                          ThisLoggerMSQueName);
   if (i)
	   res = SysConfig.DoModal();
}

LRESULT CSrvcmonView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
					//this->SendMessage(WM_CLOSE);
					OnClose();
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

	return CFormView::DefWindowProc(message, wParam, lParam);
}

BOOL CSrvcmonView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateLastEventUpdateTime();
	
	return CFormView::OnCommand(wParam, lParam);
}



int CSrvcmonView::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CSrvcmonView::StopTimer()
{
	if (!KillTimer (timer_handle))	//Stop Timer	
	{
			::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}

short CSrvcmonView::LockScreen()
{
	int retcode = 0;

	CIdleLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			::MessageBox(this->m_hWnd,"Session Timeout. Please Login","Service Monitor",MB_OK);
			//AfxMessageBox("Session Timeout. Please Login");
			
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

void CSrvcmonView::ResetCntr()
{
	cntr = 0;
}

void CSrvcmonView::UpdateLastEventUpdateTime()
{
	cntr=0;	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
}

void CSrvcmonView::OnEncryptOn() 
{
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable Encryption.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to enable Ecnryption for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to enable encryption." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable encryption of trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_ENCON,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].encryption_on = '1';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending Encryption On request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		

    m_AutoRefreshOff = 0;	
}

void CSrvcmonView::OnEncryptOff() 
{
   INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to disable Encryption.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to disable encryption for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service is not started. encryption is currently disabled." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to disable encryption of trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_ENCOFF,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].encryption_on = '0';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending encryption Off request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		
	
    m_AutoRefreshOff = 0;	
}

void CSrvcmonView::OnMraOn() 
{
   INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable MRA.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to enable MRA for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to enable MRA." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable encryption of trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_MRAON,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].mra_on = '1';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending MRA On request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		

    m_AutoRefreshOff = 0;	
}

void CSrvcmonView::OnMraOff() 
{
 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to disable MRA.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to disable MRA for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service is not started. MRA is currently disabled." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to disable MRA for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_MRAOFF,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].mra_on = '0';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending MRA Off request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		
	
    m_AutoRefreshOff = 0;	
}

void CSrvcmonView::OnFalconOn() 
{
  INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable FALCON.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to enable FALCON for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to enable FALCON." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable encryption of trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_FALCONON,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].falcon_on = '1';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending FALCON On request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		

    m_AutoRefreshOff = 0;		
}

void CSrvcmonView::OnFalconOff() 
{
	INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to disable FALCON.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to disable FALCON for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service is not started. FALCON is currently disabled." );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to disable FALCON for the service
    // service manager in will send a message to the service
    rcode = srvccomm_control_service( m_ServiceTable [index].service_name, ST1_SRVC_FALCONOFF,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            m_ServiceTable [index].falcon_on = '0';
            RefreshList();
            break;

        default:
            sprintf( buffer, "Error sending FALCON Off request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		
	
    m_AutoRefreshOff = 0;		
}

void CSrvcmonView::OnNullcheckOn()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_NULLCHECK_ON,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnNullcheckOff()
{
// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_NULLCHECK_OFF,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;


}


void CSrvcmonView::OnDbstaticsOn()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_DBSTATICS_ON,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnDbstaticsOff()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_DBSTATICS_OFF,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnFalconRTOn()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_FALCONRT_ON,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnFalconRTOff()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_FALCONRT_OFF,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnSelchangeServiceList() 
{
	INT index;

    // first get the name of the service selected
    index = m_ServiceList.GetCurSel();

	if(( strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0) || 
	   ( strcmp( m_ServiceTable [index].exe_name, "trands") == 0) ||
	   ( strcmp( m_ServiceTable [index].exe_name, "dcpimp") == 0))
	{
		m_VertexonCardStatus.EnableWindow(TRUE);
	}
	else
	{
		m_VertexonCardStatus.EnableWindow(FALSE);
	}

	if (( strcmp( m_ServiceTable [index].exe_name, "dcpiso") == 0) || 
		 ( strcmp( m_ServiceTable [index].exe_name, "dcpimp") == 0) ||
		 ( strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0) )
	{
		m_CntlMraOff.EnableWindow(TRUE);
		m_CntlMraOn.EnableWindow(TRUE);
	}
	else
	{
		m_CntlMraOff.EnableWindow(FALSE);
		m_CntlMraOn.EnableWindow(FALSE);
	}
	if( (strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0)||
		 (strcmp( m_ServiceTable [index].exe_name, "dcpimp") == 0))
	{
		m_CntlFalconOn.EnableWindow(TRUE);
		m_CntlFalconOff.EnableWindow(TRUE);
		m_CntlFalconRTOn.EnableWindow(TRUE);
		m_CntlFalconRTOff.EnableWindow(TRUE);
		m_CntlReLoadFalconflag.EnableWindow(TRUE);
		m_CntlFalconRT_EXT10On.EnableWindow(TRUE);
		m_CntlFalconRT_EXT10Off.EnableWindow(TRUE);
		m_CntlKeyblockOn.EnableWindow(TRUE);
		m_CntlKeyblockOff.EnableWindow(TRUE);
	}
	else
	{
		m_CntlFalconOn.EnableWindow(FALSE);
		m_CntlFalconOff.EnableWindow(FALSE);
		m_CntlFalconRTOn.EnableWindow(FALSE);
		m_CntlFalconRTOff.EnableWindow(FALSE);
		m_CntlReLoadFalconflag.EnableWindow(FALSE);		
		m_CntlFalconRT_EXT10On.EnableWindow(FALSE);
		m_CntlFalconRT_EXT10Off.EnableWindow(FALSE);
		m_CntlKeyblockOn.EnableWindow(FALSE);
		m_CntlKeyblockOff.EnableWindow(FALSE);
	}

	/* For cadencie links enable Falconrt ON and OFF */

	if((strcmp( m_ServiceTable [index].exe_name, "nciftr") == 0))
	{
		m_CntlFalconRTOn.EnableWindow(TRUE);
		m_CntlFalconRTOff.EnableWindow(TRUE);
	}

	if( szNetworkControllers.Find(m_ServiceTable [index].exe_name) != -1)
	{
		m_CntlEncOn.EnableWindow(TRUE);
		m_CntlEncOff.EnableWindow(TRUE);
	}
	else
	{
		m_CntlEncOn.EnableWindow(FALSE);
		m_CntlEncOff.EnableWindow(FALSE);
	}
		if( (strcmp( m_ServiceTable [index].exe_name, "archds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "devds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "mrads") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "netds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "posds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "repds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "trands") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "dvmpds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "fconds") == 0)||
			(strcmp( m_ServiceTable [index].exe_name, "updatds") == 0))
	{
		m_CntlNullcheckOn.EnableWindow(TRUE);
		m_CntlNullcheckOff.EnableWindow(TRUE);
		m_CntlDbstaticsOn.EnableWindow(TRUE);
		m_CntlDbstaticsOff.EnableWindow(TRUE);
	}
	else
	{
		m_CntlNullcheckOn.EnableWindow(FALSE);
		m_CntlNullcheckOff.EnableWindow(FALSE);
		m_CntlDbstaticsOn.EnableWindow(FALSE);
		m_CntlDbstaticsOff.EnableWindow(FALSE);
	}
	if((strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0) ||
		(strcmp( m_ServiceTable [index].exe_name, "ncdci3") == 0) ||
		(strcmp( m_ServiceTable [index].exe_name, "ncdci") == 0)  ||
		(strcmp( m_ServiceTable [index].exe_name, "ncjcb") == 0)  ||
		(strcmp( m_ServiceTable [index].exe_name, "ncjcb3") == 0))
	{
		if((strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0))
		{
			m_CntlLoadRpsflag.EnableWindow(TRUE);
			m_CntlLoadOBSrecords.EnableWindow(TRUE);
			m_CntlLoadVISA3DS2records.EnableWindow(TRUE);
			m_CntlLoadAMEX3DS2records.EnableWindow(TRUE);
			m_CntlLoadDINERS3DS2records.EnableWindow(TRUE);
			m_CntlLoadCUP3DS2records.EnableWindow(TRUE);
			m_CntlLoadJCB3DS2records.EnableWindow(TRUE);
			m_CntlLoadMIDCpngrecords.EnableWindow(TRUE);
			m_CntlLoadSecExpiryDateValidation.EnableWindow(TRUE);

		}
		else if((strcmp( m_ServiceTable [index].exe_name, "ncdci") == 0) ||
			(strcmp( m_ServiceTable [index].exe_name, "ncdci3") == 0))
		{
			m_CntlLoadDINERS3DS2records.EnableWindow(TRUE);
			m_CntlLoadRpsflag.EnableWindow(FALSE);
			m_CntlLoadOBSrecords.EnableWindow(FALSE);
			m_CntlLoadVISA3DS2records.EnableWindow(FALSE);
			m_CntlLoadAMEX3DS2records.EnableWindow(FALSE);
			m_CntlLoadCUP3DS2records.EnableWindow(FALSE);
			m_CntlLoadJCB3DS2records.EnableWindow(FALSE);
			m_CntlLoadSecExpiryDateValidation.EnableWindow(FALSE);
		}	
		else if((strcmp( m_ServiceTable [index].exe_name, "ncjcb") == 0) ||
			(strcmp( m_ServiceTable [index].exe_name, "ncjcb3") == 0))
		{
			m_CntlLoadJCB3DS2records.EnableWindow(TRUE);
			 m_CntlLoadRpsflag.EnableWindow(FALSE);
			m_CntlLoadOBSrecords.EnableWindow(FALSE);
			m_CntlLoadVISA3DS2records.EnableWindow(FALSE);
			m_CntlLoadAMEX3DS2records.EnableWindow(FALSE);
			m_CntlLoadCUP3DS2records.EnableWindow(FALSE);
			m_CntlLoadDINERS3DS2records.EnableWindow(FALSE);
			m_CntlLoadSecExpiryDateValidation.EnableWindow(FALSE);
		}
	}
	else
	{
		m_CntlLoadRpsflag.EnableWindow(FALSE);
		m_CntlLoadOBSrecords.EnableWindow(FALSE);
		m_CntlLoadVISA3DS2records.EnableWindow(FALSE);
		m_CntlLoadAMEX3DS2records.EnableWindow(FALSE);
		m_CntlLoadDINERS3DS2records.EnableWindow(FALSE);
		m_CntlLoadCUP3DS2records.EnableWindow(FALSE);
		m_CntlLoadJCB3DS2records.EnableWindow(FALSE);
		m_CntlLoadMIDCpngrecords.EnableWindow(FALSE);
		m_CntlLoadSecExpiryDateValidation.EnableWindow(FALSE);

	}

	if(strncmp( m_ServiceTable [index].exe_name, "nposa2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "ncdci2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "nccup2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "ncjcb2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "ncmcr2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "ncvis2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "nciftr",6) == 0)
	{
		m_CntlLoadReversalattepmtsTO.EnableWindow(TRUE);
		m_CntlLoadReversalattepmtsLR.EnableWindow(TRUE);
	}
	else
	{
		m_CntlLoadReversalattepmtsTO.EnableWindow(FALSE);
		m_CntlLoadReversalattepmtsLR.EnableWindow(FALSE);
	}


	if( strncmp( m_ServiceTable [index].exe_name, "ncpos",6) == 0  ||
		strncmp( m_ServiceTable [index].exe_name, "ncdci3",6) == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "nccup3",6) == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "ncjcb3",6) == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "ncmcr3",6) == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "ncvis3",6) == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "ncdci") == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "nccup") == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "ncjcb") == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "ncmcrd") == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "ncvisa") == 0 ||
		strcmp( m_ServiceTable [index].exe_name, "txcntl") == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "dcpiso",6) == 0 ||
		strncmp( m_ServiceTable [index].exe_name, "dcpimp",6) == 0 )
	{
		m_CntlLoadToRcValue.EnableWindow(TRUE);
		m_CntlLoadFaiDeclineRcValue.EnableWindow(TRUE);
	}
	else
	{
		m_CntlLoadToRcValue.EnableWindow(FALSE);
		m_CntlLoadFaiDeclineRcValue.EnableWindow(FALSE);
	}
	if((strcmp( m_ServiceTable [index].exe_name, "dcpimp") == 0))
	{
		m_CntlLoadMIDCpngrecords.EnableWindow(TRUE);
	}
	if(strncmp( m_ServiceTable [index].exe_name, "ncdci",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "ncdci2",6) == 0 ||
	   strncmp( m_ServiceTable [index].exe_name, "txcntl",6) == 0 )
	{
		m_CntlKeyblockOn.EnableWindow(TRUE);
		m_CntlKeyblockOff.EnableWindow(TRUE);
	}
	else
	{
		m_CntlKeyblockOn.EnableWindow(FALSE);
		m_CntlKeyblockOff.EnableWindow(FALSE);
	}
}


void CSrvcmonView::OnLoadRpsFlag()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load RPS flag for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load RPS flag" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_rps_flag_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading RPS flag to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnLoadToRcValue_forallcardbrands()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load acq time out RC values for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load time out RC values" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_time_out_RC_and_reversal_attempts_values_service( m_ServiceTable [index].que_name,
                                      err_msg ,ST1_LOAD_TO_RC_VALUES);
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading time out RC values to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnLoadFaiDeclineRcValue_forallcardbrands()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode=0;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load voice time out RC values for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load time out RC values" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_time_out_RC_and_reversal_attempts_values_service( m_ServiceTable [index].que_name,
                                      err_msg ,ST1_LOAD_RC_FOR_FAI);
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading time out RC values to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnLoadReversalattepmts_for_TORequest()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode=0;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load TO reversal attempts value for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load TO reversal attempts values" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_time_out_RC_and_reversal_attempts_values_service( m_ServiceTable [index].que_name,
                                      err_msg ,ST1_LOAD_TO_REVERSAL_ATTEMPTS);
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading TO reversal attempts values to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnLoadReversalattepmts_for_LateResponse()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode=0;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load LateResponse Reversal attepmts value  for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load Reversal attepmts for LateResponse" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_time_out_RC_and_reversal_attempts_values_service( m_ServiceTable [index].que_name,
                                      err_msg ,ST1_LOAD_LATE_RESP_REVERSAL_ATTEMPTS);
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading reversal attempts values to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadObs()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load OBS records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load OBS details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_OBS_records_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading OBS details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadVisa3ds2()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load VISA3DS2 records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load VISA3DS2 details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_VISA3DS2_records_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading VISA3DS2 details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadAmex3ds2()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load AMEX3DS2 records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load AMEX3DS2 details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_AMEX3DS2_records_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading AMEX3DS2 details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadDiners3ds2()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load DINERS3DS2 records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load DINERS3DS2 details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_DINERS3DS2_records_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading DINERS3DS2 details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadJcb3ds2()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load JCB3DS2 records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load JCB3DS2 details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_JCB3DS2_records_service( m_ServiceTable [index].que_name,
                                      err_msg );

	//rcode = srvccomm_load_JCB3DS2_tfinirecords_service( m_ServiceTable [index].que_name,err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading JCB3DS2 details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadCup3ds2()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load CUP3DS2 records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load CUP3DS2 details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_CUP3DS2_records_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for loading CUP3DS2 details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedReloadMIDCpng()
{
	// TODO: Add your control notification handler code here
	 INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to load MIDCpng records for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to load MIDCpng details" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_load_MidCpng_tfinirecords_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {

   }
   else
   {
		sprintf( buffer, "Error in sending request for loading MIDCpng details to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnReLoadFalcon()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to reload Falcon flag for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to reload Falcon flag" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_reload_falcon_flag_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for reloading Falcon flag to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnReLoadSecExpiryDateValidationFlag()
{
		// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to enable trace.", MB_ICONSTOP | MB_OK );
        return;
    }

    // get confirmation from user
    sprintf( buffer, "Do you wish to reload SecExpiryDateValidation flag for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service is running
    if( m_ServiceTable [index].running == '0' )
    {
        sprintf( buffer, "Service must be started to reload SecExpiryDateValidation flag" );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // send a message to service manager to enable trace for the service
    // service manager in will send a message to the service
    rcode = srvccomm_reload_SecExpiryDateValidation_flag_service( m_ServiceTable [index].que_name,
                                      err_msg );
    EndWaitCursor();

    // process the response from service manager
   if(rcode == PTEMSG_OK)
   {
   
   }
   else
   {
		sprintf( buffer, "Error in sending request for reloading SecExpiryDateValidation flag to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


    m_AutoRefreshOff = 0;
}

void CSrvcmonView::OnBnClickedSecondaryexpirydatevalReload()
{
	// TODO: Add your control notification handler code here
}


void CSrvcmonView::OnClickedVertexonCardStatus()
{
		// TODO: Add your control notification handler code here
    INT index=0;
    CHAR err_msg[1000] = {0};
	CHAR buffer[256] = {0};
    BYTE rcode=0;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to reload", MB_ICONSTOP | MB_OK );
        return;
    }

	// get confirmation from user
    sprintf( buffer, "Do you wish to reload Vertexon card Status Parameters for %s?", m_ServiceTable [index].display_name );
    INT ret = AfxMessageBox( buffer, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDNO )
        return;

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s must be started to reload Vertexon card Status.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_vertexon_card_status_reload_service( m_ServiceTable [index].que_name, err_msg );

    EndWaitCursor();

    // process the response from service manager
    if(rcode == PTEMSG_OK)
    {
		//
    }
	else
	{
		sprintf( buffer, "Error in sending request for reloading Vertexon card Status Parameters to %s via Service Manager %s",
						m_ServiceTable [index].service_name, err_msg );
		AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
	}	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnFalconRT_EXT10On()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_FALCONRT_EXT10_ON,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnFalconRT_EXT10Off()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_FALCONRT_EXT10_OFF,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}



void CSrvcmonView::OnKeyblockOn()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_KEYBLOCK_ON,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}


void CSrvcmonView::OnKeyblockOff()
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[1000], buffer[256];
    BYTE rcode;
	
    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please Select a Service to Ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started.",
                 m_ServiceTable [index].display_name );
        AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
        return;
    }

    m_AutoRefreshOff = 1;

    // update status bar and set cursor to wait cursor
    BeginWaitCursor();

    // try to ping the service
    rcode = srvccomm_DB_control_service( m_ServiceTable [index].que_name, ST1_SYS_KEYBLOCK_OFF,err_msg );

    EndWaitCursor();

    // process the response from service manager
    switch( rcode )
    {
        case PTEMSG_OK:
            strcpy( m_PingStatus [index], "Ping OK" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
            break;

        default:
            strcpy( m_PingStatus [index], "Ping Failed" );
            RefreshList();
            strcpy( m_PingStatus [index], " " );
    }	
	
    m_AutoRefreshOff = 0;
}
