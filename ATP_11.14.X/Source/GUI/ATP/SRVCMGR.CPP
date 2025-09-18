/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SRVCMGR.CPP  $
   
      Rev 1.6   Dec 01 2003 11:30:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:22   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:39:48   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:57:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.2   Jan 22 1999 09:53:24   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.1   Jan 21 1999 13:09:42   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:12   iarustam
    
*
************************************************************************************/
// SrvcMgr.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "SrvcMgr.h"
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
}
extern CSharedMem shrmem;


/////////////////////////////////////////////////////////////////////////////
// CSrvcMgr dialog


CSrvcMgr::CSrvcMgr(CWnd* pParent /*=NULL*/)
	: CDialog(CSrvcMgr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSrvcMgr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSrvcMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSrvcMgr)
	DDX_Control(pDX, IDC_SERVICE_TAB, m_MainTab);
	DDX_Control(pDX, IDC_SERVICE_LIST, m_ServiceList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSrvcMgr, CDialog)
	//{{AFX_MSG_MAP(CSrvcMgr)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_BN_CLICKED(IDC_PINGALL, OnPingall)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STARTALL, OnStartall)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_STOPALL, OnStopall)
	ON_BN_CLICKED(IDC_TRACEOFF, OnTraceoff)
	ON_BN_CLICKED(IDC_TRACEON, OnTraceon)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_SERVICE_TAB, OnSelchangeServiceTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSrvcMgr message handlers

 
//******************************************************************
//******************************************************************
void CSrvcMgr::OnClose() 
{
	// TODO: Add your control notification handler code here
   m_StatusWindow->SetMessageText( " " );
   EndDialog( IDOK );
	
}


//******************************************************************
//******************************************************************
void CSrvcMgr::OnDisable() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to disable", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running == '1' )
    {
        sprintf( buffer, "Service %s is started. Service should be stopped first.",
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnEnable() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to enable", MB_ICONSTOP | MB_OK );
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnPing() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to ping", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has been started
    if( m_ServiceTable [index].running != '1' )
    {
        sprintf( buffer, "Service %s is not started",
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnPingall() 
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
    }

    RefreshList();
    m_AutoRefreshOff = 0;
    EndWaitCursor();		
	
}



//******************************************************************
//******************************************************************
void CSrvcMgr::OnRefresh() 
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnStart() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected

    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to start", MB_ICONSTOP | MB_OK );
        return;
    }

    // check if service has already been started
    if( m_ServiceTable [index].running == '1' )
    {
        sprintf( buffer, "Service %s is already started", m_ServiceTable [index].display_name );
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
    sprintf( buffer, "Attempting to start %s ...", m_ServiceTable [index].display_name );
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
                                           Service Manager." );                                                                                                
            m_ServiceTable [index].running = '1';
            RefreshList();
            sprintf( buffer, "Service %s is already started", m_ServiceTable [index].display_name );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
            break;
	
        case PTEMSG_SRVC_STARTFAILED:
            sprintf( buffer, "Unable to start %s.  Check event log on Ascendent server.",
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnStartall() 
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnStop() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to stop", MB_ICONSTOP | MB_OK );
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
    sprintf( buffer, "Attempting to stop %s ...", m_ServiceTable [index].display_name );
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
            sprintf( buffer, "Unable to stop %s.  Check event log on Ascendent server",
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnStopall() 
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

    // now get confirmation for each service
    for( temp = 0; temp < m_NumServices; temp++ )
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnTraceoff() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to disable trace", MB_ICONSTOP | MB_OK );
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



//******************************************************************
//******************************************************************
void CSrvcMgr::OnTraceon() 
{
	// TODO: Add your control notification handler code here
    INT index;
    CHAR err_msg[100], buffer[256];
    BYTE rcode;

    // first get the name of the service selected
    if( ( index = m_ServiceList.GetCurSel() ) == LB_ERR )
    {
        AfxMessageBox( "Please select a service to enable trace", MB_ICONSTOP | MB_OK );
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
        sprintf( buffer, "Service must be started to enable trace" );
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
            sprintf( buffer, "Error sending Trace Off request to %s via Service Manager %s",
                              m_ServiceTable [index].service_name, err_msg );
            AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
    }		
    m_AutoRefreshOff = 0;	
}



//******************************************************************
//******************************************************************
void CSrvcMgr::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    //CHAR buffer[256];

    // if we are in the middle of something, don't refresh
    if( !m_AutoRefreshOff )
        OnRefresh();

/*    
    if( !m_FirstUpdate )
    {
        m_FirstUpdate = 1;
        sprintf( buffer, "Connected to Primary Server: %s", m_PrimaryMachine );
        m_StatusWindow->SetMessageText( buffer );
    }
*/
    m_StatusWindow->SetMessageText( m_ConnectString );
	
	CDialog::OnTimer(nIDEvent);
}



//********************************************************************
//********************************************************************
void CSrvcMgr::RefreshList() 
{
    CHAR run_status[12], trace_status[12];
    INT  temp;
    CHAR buffer[256];

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
    
    m_ServiceList.UpdateWindow();

}


//********************************************************************
//********************************************************************
void CSrvcMgr::OnSelchangeServiceTab(NMHDR* pNMHDR, LRESULT* pResult) 
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
        ret = srvccomm_change_connection( m_PrimaryMachine, m_XipcInstance );
        sprintf( m_ConnectString, "Connected to Primary Server: %s", m_PrimaryMachine );
    }
    else
    {
        ret = srvccomm_change_connection( m_SecondaryMachine, m_XipcInstance );
        sprintf( m_ConnectString, "Connected to Secondary Server: %s", m_SecondaryMachine );
    }

    if( !ret )
    {
        if( nIndex == 0 )
            sprintf( buffer, "Unable to connect to Primary Server %s", m_PrimaryMachine );
        else
            sprintf( buffer, "Unable to connect to Primary Server %s", m_PrimaryMachine );

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



//********************************************************************
//********************************************************************
BOOL CSrvcMgr::OnInitDialog() 
{
	CDialog::OnInitDialog();

   TC_ITEM ItemDef;
	char    TabTitle[40];
    UINT    Timer;
    INT     temp;
   
	
	// TODO: Add extra initialization here
    Timer = 0;
    m_StatusWindow = GetParentFrame();

    // read the registry, initialize the Logins
    //InitLogins();


	// Create the image list for the tab control
	//m_pImageList = new CImageList();		
	//ASSERT(m_pImageList != NULL);    // no serious allocation failure checking
	//m_pImageList->Create(32, 32, FALSE/*bMask*/,1/*nInitial*/, 1/*nGrow*/);

	//m_pImageList->Add(m_pApplicationObject->LoadIcon(IDI_SETUP));
	//m_MainTab.SetImageList(m_pImageList) ;
	

    // get the primary machine id from registry, login into primary 
    // machine instance
    if( !GetThisMachineId( m_PrimaryMachine ) )
    {
        AfxMessageBox( "Primary Machine ID does not exist in Registry.  Registry may not \
                        be initialized for Ascendent", MB_ICONSTOP | MB_OK );
        strcpy( m_PrimaryMachine, "Local" );
    }



    // create the tabs based on the HighAvailability indicator
    sprintf( TabTitle, " %s", m_PrimaryMachine );
	ItemDef.mask = TCIF_TEXT; //|TCIF_IMAGE;
	ItemDef.pszText = TabTitle;
	ItemDef.cchTextMax = strlen( TabTitle );
	ItemDef.iImage = 0;
	if ( m_MainTab.InsertItem( 0, &ItemDef ) == -1 )
		AfxMessageBox( "Tab Control construction failed", MB_OK, 0);

   sprintf( m_ConnectString, "Connected to Primary Server: %s", m_PrimaryMachine );

/*
    if( m_HighAvailability )
    {
        sprintf( TabTitle, " %s", m_SecondaryMachine );
		ItemDef.mask = TCIF_TEXT|TCIF_IMAGE;
	    ItemDef.pszText = TabTitle;
	    ItemDef.cchTextMax = strlen( TabTitle );
	    ItemDef.iImage = 0;
	    if ( m_MainTab.InsertItem( 1, &ItemDef ) == -1 )
		    AfxMessageBox( "Tab Control construction failed", MB_OK, 0);
    }
*/
    
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
/*
    // Start a 10 sec timer to get periodic updates
    m_RefreshTimer = SetTimer( Timer, 10000, NULL );
    if( m_RefreshTimer == 0 )
        AfxMessageBox( "Unable to start Refresh Timer", MB_ICONSTOP | MB_OK );
*/
    for( temp = 0; temp < MAX_SERVICES; temp++ )
        strcpy( m_PingStatus [temp], " " );

    PostMessage( WM_TIMER, 0, 0 );

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSrvcMgr::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
