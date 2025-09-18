// stipmgr.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "stipmgr.h"
#include "stipmgrDlg.h"
#include "sharedmem.h"
extern "C"
{
   #include "ptetcp.h"
   #include "ntutils.h"
   #include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
	#include "TXDSAPI.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString strUserName;
CHAR server_name[56];
extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
   CHAR appProductVersion[MAX_APPVERSION_SIZE];
}
/////////////////////////////////////////////////////////////////////////////
// CStipmgrApp

BEGIN_MESSAGE_MAP(CStipmgrApp, CWinApp)
	//{{AFX_MSG_MAP(CStipmgrApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
//	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStipmgrApp construction

CStipmgrApp::CStipmgrApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStipmgrApp object

CStipmgrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStipmgrApp initialization

BOOL CStipmgrApp::InitInstance()
{
	char error_msg[300];
	
	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes

	AfxEnableControlContainer();

//	CHAR connect_str[25];
//    CHAR ini_file[256];
//	int i1;
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
//	CStipmgrDlg dlg;
//	m_pMainWnd = &dlg;

//	    m_lpCmdLine = GetCommandLine();
		strUserName = m_lpCmdLine;

   // establish connection with applink server
   // JMG Note: The connection INIT is performed once here regardless of CONNECTMODE
   if( !ptetcp_init( error_msg ) )
   {
      AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
      return FALSE;
   }
  

	GetFirstServerId( server_name );
     if( !ptetcp_connect( server_name, error_msg ) )
	 {
        AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
        return FALSE;
     }
   
	 //Krishna@tf, 19.5.11 : Validate the Commandline params & continue the App	
	int i;
    char tmp[256]={'\0'};
    int cnt = 0, j = 0;
    char u_name[51]={'\0'};
    char u_pwd[51]={'\0'};    
	BYTE rcode;		
	UCF01 ucf01;
  
	//cmdlinestr = GetCommandLine();

	//Object m_lpCmdLine of CWinApp class contains the Command line params.
	//Each param is seperated by a space with others.
	strcpy(tmp,m_lpCmdLine);
	
	for(i = 0; tmp[i]!= '\0';i++)
	{
		if(tmp[i] == ' ') { cnt++; j = 0; continue;}

		switch(cnt)
		{
			case 0:  u_name[j++] = tmp[i];
				break;
			case 1:  u_pwd[j++] = tmp[i];
				break;
			default : break;
		}
	}	

	if(strlen(u_name)==0 || strlen(u_pwd)==0)
	{
		//AfxMessageBox("Please start this application from ATP");
		
		return false;

	}
  
	//Init User name of Command line params to this field
	strUserName = u_name;

	memset( &ucf01, 0, sizeof( UCF01 ) );
    
     // next authenticate the user
     strcpy( (char *)ucf01.primary_key.name, u_name);     

     rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
	 
     if( rcode == PTEMSG_NOT_FOUND )
     {        
		return false;
     }
     else if( rcode != PTEMSG_OK )
     {        
        return FALSE;
     }

	// check password	
	if( strcmp( (char *)ucf01.password, u_pwd ) != 0 )
	{		
		return FALSE;
	}   
	//End, Krishna changes


//	dlg.DoModal();

   // Following disconnect logic by JMG on 4/26/02 as we can now connect and disconnect
   // on every operation
//   if ( 0 == strcmp("SINGLE", connect_str) )
     // Configured for SINGLE applnk connection at start up
	 // Next line added by JMG on 6/14/01 to disconnect applnk connection
	 // before exiting this GUI. Will prevent applnk & system problems.
//	 ptetcp_disconnect();

/*	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}*/

	CStipmgrDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	
	 ptetcp_disconnect();
	return true;
}
