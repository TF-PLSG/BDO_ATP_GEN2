/*******************************************************************************
*  
* MODULE:      monitor.cpp
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent System Monitor
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\monitor\files\MONITOR.CPP  $
   
      Rev 1.2   Aug 29 2000 15:24:36   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:00   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:02   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:32   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:08   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:06   jhapitas
   Initial checkin
*
*******************************************************************************/
// monitor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "monitordco.h"

#include "MainFrm.h"
#include "monitordcoDoc.h"
#include "monitordcoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{

#include "ntutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "TXDSAPI.h"
	
}
CString cmdlinestr;
char u_name[51]={'\0'};
char u_id[51]={'\0'};
char u_pwd[51]={'\0'};
char strFirstServerID[56] = "";
char HostName1[50] = {0};
char HostName2[50] = {0};
char HostName3[50] = {0};
char HostName4[50] = {0};

extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
   extern CHAR appProductVersion[MAX_APPVERSION_SIZE];
}
/////////////////////////////////////////////////////////////////////////////
// CMonitorApp

BEGIN_MESSAGE_MAP(CMonitorApp, CWinApp)
	//{{AFX_MSG_MAP(CMonitorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorApp construction

CMonitorApp::CMonitorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMonitorApp object

CMonitorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMonitorApp initialization

BOOL CMonitorApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
     CHAR filename[50] = {0};
	 CHAR hostname[50] = {0};
	 CHAR server_selection_flag[10] = {0};
	 INT server_selection_flag_int = 0;
	 CHAR port_str[10] = {0};
	 CHAR tmpstr[50] = {0};

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
InitCommonControls();
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMonitorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMonitorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//Krishan@TF, 26.5.11, Done to validate running of Monitor Exe by Cmdline Args & continue the App
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;	
	
	//Object m_lpCmdLine of CWinApp class contains the Command line params.
	//Each param is seperated by a space with others.		
	cmdlinestr = m_lpCmdLine;

	int i;
	char tmp[256]={'\0'};
	int cnt = 0, j = 0;		

	strcpy(tmp,cmdlinestr);
	
	for(i = 0; tmp[i]!= '\0';i++)
	{
		if(tmp[i] == '|') { cnt++; j = 0; continue;}

		switch(cnt)
		{
		case 0: u_id[j++] = tmp[i];
			break;
		case 1:  u_name[j++] = tmp[i];
			break;
		case 2:  u_pwd[j++] = tmp[i];
			break;
		default : break;
		}
	}	

	if(strlen(u_id)==0 || strlen(u_pwd)==0)
	{
		
		//return FALSE;

	}  
	//Krishna changes end

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	/*memset(strFirstServerID, 0, sizeof(strFirstServerID));
	if ( !GetFirstServerId (strFirstServerID))
	{
		memset(strFirstServerID, 0, sizeof(strFirstServerID));
	}*/

		/*GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "1",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        filename ); 

	
	  	 server_selection_flag_int = atoi(server_selection_flag);*/


		//new changes - amith
		//get all hostnames
		//if(server_selection_flag_int == 1)
		  {

				if( !GetHostName_1( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
				strcpy(HostName1,hostname);
		  }
		  //else if(server_selection_flag_int == 2)
		  {
				if( !GetHostName_2( hostname ) )
				{
					AfxMessageBox( "Secondary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
				strcpy(HostName2,hostname);
		  }
		  //else if(server_selection_flag_int == 3)
		  {
			
				if( !GetHostName_3( hostname ) )
				{
					AfxMessageBox( "Third Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
				strcpy(HostName3,hostname);
		  }
		  //else if(server_selection_flag_int == 4)
		  {
				
				if( !GetHostName_4( hostname ) )
				{
					AfxMessageBox( "Fourth Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
				strcpy(HostName4,hostname);
		  }


	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	CString title;
	m_pMainWnd->GetWindowText(title);
	//title = title + "(" + strFirstServerID +")" + " - Version - ";
	//title = title + "(" + hostname +")" + " - Version - ";
	title = title + " - Version - ";
	title += appProdVer;
	m_pMainWnd->SetWindowText(title);
	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_app_version;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_app_version = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_APP_VERSION, m_app_version);
	DDV_MaxChars(pDX, m_app_version, 25);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMonitorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	char temp_buff[50];

	sprintf(temp_buff, "ATP Version : %s", appProductVersion);	
	aboutDlg.m_app_version = temp_buff;
	aboutDlg.DoModal();
	
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorApp commands
