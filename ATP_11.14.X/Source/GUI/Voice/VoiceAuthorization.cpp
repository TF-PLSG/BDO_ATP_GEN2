/*********************************************************************************
* Module:		VoiceAuthorization.cpp : Defines the class behaviors for the application.
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\New_voice_auth\VoiceAuthorization.cpp  $     

      Rev 1.6   Apr 04 2005 16:19:44   lmitchel
   Rebuild 4_4_1
   
      Rev 1.5   Sep 27 2004 10:19:28   lmitchel
   Version 4.4.0.3; SCR12484: Added host id to voice auth title bar
   
      Rev 1.4   May 01 2002 15:59:48   jgrguric
   Added new configuration logic for CONNECTMODE
   parameter to allow multiple or single connections.
   
      Rev 1.3   Jan 28 2002 13:55:06   dirby
   Commented out the Instance handling code.  This is to allow
   multiple instances of VoiceAuth to run simultaneously on a
   single PC.
   SCR 583
   
      Rev 1.2   Jun 14 2001 15:31:08   jgrguric
   Added call to ptetcp_disconnect in InitInstance() to disconnect from applnk on exit from GUI. This should prevent "Bad File number", "Invalid argument" and other such errors from applnk, as well as other  unpredictable system errors after this GUI exits.
   
      Rev 1.1   May 18 2001 16:45:58   lmitchel
   Updated file version info in .rc file
   SCR312:  For override denial on deferred transaction, populating gross amount and monthly amortization amount
   SCR314: Disable commit button on denial response
   Enable verify button after card number error after commit
**********************************************************************************/
// Following line added by JMG on 4/26/02 for use of GetPrivateProfileString


#include "stdafx.h"
#include <windows.h>
#include "VoiceAuthorization.h"

#include "MainFrm.h"
#include "VoiceAuthorizationDoc.h"
#include "VoiceAuthorizationView.h"
#include "VoiceAuth.h"
#include "MyUtils.h"

extern "C"
{
   #include "ptetcp.h"
   #include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString name;
char strFileRecord[56] = "";
extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
   CHAR appProductVersion[MAX_APPVERSION_SIZE];
}

UCF01 ucf01;

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationApp

BEGIN_MESSAGE_MAP(CVoiceAuthorizationApp, CWinApp)
	//{{AFX_MSG_MAP(CVoiceAuthorizationApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationApp construction

CVoiceAuthorizationApp::CVoiceAuthorizationApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVoiceAuthorizationApp object

CVoiceAuthorizationApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationApp initialization

BOOL CVoiceAuthorizationApp::InitInstance()
{
	char error_msg[300]  = {0};
	CMyUtils UtilsObject;
// Next three variables added on 4/26/02 by JMG for new connection logic.
	CHAR connect_str[50] = {0};
    CHAR ini_file[256]  = {0};
	CHAR title_buffer[100] = {0};
	int i1;
	int temp_len = 0;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

/*	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVoiceAuthorizationDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CVoiceAuthorizationView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;*/

   /* --------------------------------------------------------------------- *
    * Comment this section out to allow                                     *
    * multiple Voice Auth running simultaneously.                           *
    * --------------------------------------------------------------------- *
	if (UtilsObject.IfExistsPreviousInstance()) {
		UtilsObject.ActivateCurrentInstance() ;
		return FALSE ; // m_hFileMappingObject is NULL at this point
					   // so that CloseHandle will not be called in ExitInstance ;	
	}
	else {
		ASSERT (UtilsObject.m_hFileMappingObject != NULL) ;
		m_hFileMappingObject = UtilsObject.m_hFileMappingObject ;
	}
   * ---------------------------------------------------------------------- */

	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes   

   // establish connection with applink server
   // JMG Note: The connection INIT is performed once here regardless of CONNECTMODE
   if( !ptetcp_init( error_msg ) )
   {
      AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
      return FALSE;
   }


// Following connection logic added by JMG on 4/26/02, as we can now connect on each operation
// This change made in concert with atptcp.lib change to process_request() to connect
// and disconnect on each call, based on CONNECTMODE parameter in tf.ini.

   // Get the connection configuration parameter "CONNECTMODE". If not present
   // default to "MULTIPLE". The other valid value is "SINGLE" to enable connect and
   // disconnect only at start up and exit. Other values same as MULTIPLE.
   GetAscendentConfigDirectory( ini_file );
   strcat( ini_file, "tf.ini" );

   (void) GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "CONNECTMODE",	           // points to key name 
        "multiple",	               // points to default string 
        connect_str,               // points to destination buffer 
        sizeof(connect_str) - 1,   // size of destination buffer 
        ini_file );                // points to initialization filename 

   //connect_str[strlen(connect_str)] = 0; //Make sure string is terminated
   temp_len=strlen(connect_str);
   connect_str[temp_len] =0; //Make sure string is terminated
   i1 = 0;
   while ( 0 != connect_str[i1] )
	   connect_str[i1++] = toupper(connect_str[i1]); // Convert string to upper case
   if ( 0 == strcmp("SINGLE", connect_str) ) {
     // Configured for SINGLE applnk connection at start up
	 if ( !GetFirstServerId (strFileRecord))
	 {
        AfxGetMainWnd()->MessageBox ("Missing Primary Machine ID in registry. Run the executable from installation Bin directory.") ;
        return FALSE;
	 }

     if( !ptetcp_connect( strFileRecord, error_msg ) )
	 {
        AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
        return FALSE;
     }
   }

    //Krishna@tf, 19.5.11 : Validate the Commandline params & continue the App	
	int i;
    char tmp[256]={'\0'};
    int cnt = 0, j = 0;
    char u_name[51]={'\0'};
    char u_pwd[51]={'\0'};    
	BYTE rcode;		
  
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
#ifndef _DEBUG

	if(strlen(u_name)==0 || strlen(u_pwd)==0)
	{
		//AfxMessageBox("Please start this application from ATP");
		
		return false;
	}
#endif
 
	memset( &ucf01, 0, sizeof( UCF01 ) );
    
     // next authenticate the user
     strcpy( (char *)ucf01.primary_key.name, u_name);
	 name = u_name;

     rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
	 
     if( rcode == PTEMSG_NOT_FOUND )
     {        
		return false;
     }
     else if( rcode != PTEMSG_OK )
     {        
        return FALSE;
     }
#ifndef _DEBUG

	// check password	
	if( strcmp( (char *)ucf01.password, u_pwd ) != 0 )
	{		
		return FALSE;
	}   
	//End, Krishna changes
#endif


	CVoiceAuth inputDlg;
	inputDlg.m_bEnableOverrideDenial = atoi((LPSTR) ucf01.user_info_5);
	inputDlg.DoModal();

   // Following disconnect logic by JMG on 4/26/02 as we can now connect and disconnect
   // on every operation
   if ( 0 == strcmp("SINGLE", connect_str) )
     // Configured for SINGLE applnk connection at start up
	 // Next line added by JMG on 6/14/01 to disconnect applnk connection
	 // before exiting this GUI. Will prevent applnk & system problems.
	 ptetcp_disconnect();

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
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_appversion = _T("");
	//}}AFX_DATA_INIT
	char temp_buff[50];
	sprintf(temp_buff, "ATP - TranSearch. Version : %s", appProductVersion);
	m_appversion = temp_buff;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_APPVERION, m_appversion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CVoiceAuthorizationApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationApp message handlers

