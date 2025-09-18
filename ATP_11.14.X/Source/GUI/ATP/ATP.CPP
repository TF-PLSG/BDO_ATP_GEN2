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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\ATP.CPP  $

	  Rev 1.16   Nov 26 2010 11:30:22  
    Added PCI changes - Password Validations,Locking user account ,IdleTime Calculation
	and added ucf03 table for Unlocking User Account from GUI-TF-Srikanth

    
      Rev 1.15   Apr 01 2005 10:13:16   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.14   Sep 23 2004 14:27:30   lmitchel
   Ver 4.4.0.2: Added Host ID to ATP GUI Title Bar
   
      Rev 1.13   Dec 01 2003 11:28:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Feb 27 2003 10:04:16   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.11   May 01 2002 16:08:26   jgrguric
   Added new configuration logic for CONNECTMODE
   parameter to allow multiple or single connections.
   
      Rev 1.10   Jan 09 2002 17:44:22   dirby
   Made workstation global so other ATP screens can access it.
   SCR 574 - Log workstation and operator_id to TLF01 for JCB Updates.
   
      Rev 1.9   Jun 21 2000 14:24:06   lmitchel
   Modifed AlphaLookup.cpp and repds to select and display chain name(mcf01.name02) in merchant lookup list.
   Added 'Update' button to account file screen(mcc102.cpp)
   
      Rev 1.8   Mar 02 2000 11:55:24   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.7   Jan 12 2000 14:14:32   mbysani
   Changes mades acc to bugs
   
      Rev 1.1   Sep 22 1999 15:40:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:46   iarustam
   Initial Release
   
      Rev 1.3   Jun 02 1999 16:21:46   iarustam
   Bug fixes
   
      Rev 1.2   May 10 1999 10:55:58   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:55:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 12 1999 11:04:46   iarustam
   initial release
   
      Rev 1.1   Jan 22 1999 10:56:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 22 1999 09:50:34   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.2   Jan 13 1999 14:18:04   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:40:02   iarustam
    
*
************************************************************************************/

// Atp.cpp : Defines the class behaviors for the application.
//

#define _WIN32_WINNT 0x0500

#include "stdafx.h"
#include "Atp.h"
#include <math.h>
#include "MainFrm.h"
#include "AtpDoc.h"
#include "AtpView.h"
#include "MyUtils.h"

#include "login.h"
#include "changepassword.h"
#include "AfxWin.h"
#include "RegistryUpdate.h"
#include "SharedMem.h"
extern "C"
{
   #include "ptetcp.h"
   #include "ntutils.h"
   #include "Txutils.h"  

}


extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
   CHAR appProductVersion[MAX_APPVERSION_SIZE];
}

CHAR hostname1[50] = {0}, hostname2[50] = {0}, hostname3[50] = {0}, hostname4[50] = {0};
SHORT port_num1, port_num2, port_num3, port_num4;

//Added 5/1/02 by JMG
CHAR connect_str[25];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString workstation;
UCF01 ucf01;
//UCF03 ucf03;
ATP02 atp02;
CCF03 sCCF03Record;

CSharedMem shrmem;
WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
/////////////////////////////////////////////////////////////////////////////
// CAtpApp

BEGIN_MESSAGE_MAP(CAtpApp, CWinApp)
	//{{AFX_MSG_MAP(CAtpApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtpApp construction

CAtpApp::CAtpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CAtpApp object

CAtpApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CAtpApp initialization

BOOL CAtpApp::InitInstance()
{
	CMyUtils UtilsObject ;
	char strFileRecord[56] = "";
	char strErrorMessage[200] = "";
	char title_buffer[100] = "";
	CHAR wstation[21];
	
	ATP01 sATPStructure;
	int ret;

	
	// Next two variables added on 4/26/02 by JMG for new connection logic.
	CHAR ini_file[256];
	int i1;

	CLogin login;
	InitEventLogging();


	if (UtilsObject.IfExistsPreviousInstance()) {
		UtilsObject.ActivateCurrentInstance() ;
		return FALSE ; // m_hFileMappingObject is NULL at this point
					   // so that CloseHandle will not be called in ExitInstance ;	
	}
	else {
		ASSERT (UtilsObject.m_hFileMappingObject != NULL) ;
		m_hFileMappingObject = UtilsObject.m_hFileMappingObject ;
	}


	// Create main MDI Frame window.
	// CMainFrame* pMainFrame = new CMainFrame;
	//if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	//   return FALSE;
	// Save the pointer to the main frame window.  This is the
	// only way the framework will have knowledge of what the
	// main frame window is.
	// m_pMainWnd = pMainFrame;
	// pMainFrame->SetWindowText("ATP Demo");

	// Show the   main window using the nCmdShow parameter
	// passed to the application when it was first launched.
	//m_nCmdShow = SW_SHOWMAXIMIZED;
	//pMainFrame->ShowWindow(m_nCmdShow);
	//pMainFrame->UpdateWindow();


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

	CHAR error_msg[256];
	BYTE rcode;

	GetCurrentDirectory(256, error_msg);

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

	connect_str[strlen(connect_str)] = 0; //Make sure string is terminated

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

	 	 WritePrivateProfileString(
		  "SERVER_SELECT",	/* pointer to section name*/
		  "SERVER_NAME",	/* pointer to key name*/
		  "1" ,	/* pointer to string to add*/
		  ini_file			/* pointer to initialization filename*/
	 );

	 if( !ptetcp_connect( strFileRecord, error_msg ) )
	 {
		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
		return FALSE;
	 }
	}

	DWORD wsize = sizeof(wstation);
	GetComputerName( wstation, &wsize );

	strcpy( (char *)atp02.primary_key.workstation_id, wstation );
	rcode = txdsapi_get_record( (pBYTE)&atp02, sizeof (ATP02), ATP02_DATA,error_msg );
	if ( rcode == PTEMSG_NOT_FOUND ) 
	{
		 AfxMessageBox( "Your workstation is not authorized to run ATP. Please contact your System Administrator.", MB_ICONSTOP | MB_OK );
	return FALSE;
	}
	else if( rcode != PTEMSG_OK )
	{
	  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
	  return FALSE;
	}
	if  ( strcmp( (char *)atp02.workstation_status,"A"  ) != 0 )    
	{
	  AfxMessageBox( "Your workstation is disabled/inactive to run ATP. Please contact your System Administrator.", MB_ICONSTOP | MB_OK );
	  return FALSE;
	}

	/* Copy workstation Id into global variable
	* so other portions of ATP can access it.
	*/
	workstation = wstation;

	//Create Mutex Lock
   shrmem.CreateMutexLock();
   //Krishna : Create a Shared Memory
   shrmem.CreateSharedMemorySegment();
   shrmem.MapSharedMemorySegment();

   shrmem.DeleteMutexLock();
	
	//Krishna, 27.4.11: Made Login Verification as a function, to reuse it in different clases
	ret = login.CheckLoginDetails();

	if(ret == false || ret == FALSE || ret == CANCEL) 
		return FALSE;
   
   // Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	LogEvent( "authentication done", INFO_MSG );


	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAtpDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAtpView));
	AddDocTemplate(pDocTemplate);

	LogEvent( "class init done", INFO_MSG );

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	LogEvent( "command line done", INFO_MSG );

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	LogEvent( "shell command done", INFO_MSG );

	ZeroMemory(&sATPStructure,sizeof(ATP01)) ;

	// memcpy ( sATPStructure.site_name, "ATP SigCapture Concord EFS", sizeof (sATPStructure.site_name));

	rcode = txdsapi_get_record  ((pBYTE)&sATPStructure,sizeof (ATP01), ATP01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{   
		AfxMessageBox(strErrorMessage);
		return FALSE;
	}

	 /*Set title bar with site and host id*/ 
	 memset(&title_buffer, 0x00, sizeof(title_buffer));
	 sprintf (title_buffer, "%s     (%s)", (char*)sATPStructure.site_name, strFileRecord);
	 m_pMainWnd->SetWindowText((char*)title_buffer);

	 
	// Show the   main window using the nCmdShow parameter
	// passed to the application when it was first launched.
	m_nCmdShow = SW_SHOWMAXIMIZED;
	CString title;
	m_pMainWnd->GetWindowText(title);
	title = title + " - Version - ";
	title += appProdVer;
	m_pMainWnd->SetWindowText(title);
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	LogEvent( "InitInstance done", INFO_MSG );

	OnReadDcoMonitorDetails();

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAtpApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	char temp_buff[50];

	sprintf(temp_buff, "ATP Version : %s", appProductVersion);	
	aboutDlg.m_app_version = temp_buff;
	aboutDlg.DoModal();
}


void CAtpApp::OnReadDcoMonitorDetails()
{
	CHAR tmpstr[50] = {0};
	CHAR filename[50] = {0};
	CHAR port_str[10] = {0};

	GetAscendentConfigDirectory(tmpstr);
	sprintf(filename, "%stf.ini", tmpstr);
	 

	GetPrivateProfileString(
					"APPLNK", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

	port_num1 = atol( port_str );

	if( !GetHostName_1( hostname1 ) )
	{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
							be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname1, "localhost" );
	}

	GetPrivateProfileString(
					"APPLNK2", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

	port_num2 = atol( port_str );
				
	if( !GetHostName_2( hostname2 ) )
	{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname2, "localhost" );
	}


	GetPrivateProfileString(
					"APPLNK3", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

	port_num3 = atol( port_str );
				
	if( !GetHostName_3( hostname3 ) )
	{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname3, "localhost" );
	}

	GetPrivateProfileString(
					"APPLNK4", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

	port_num4 = atol( port_str );

	if( !GetHostName_4( hostname4 ) )
	{
		AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
		strcpy( hostname4, "localhost" );
	}
}
/////////////////////////////////////////////////////////////////////////////
// CAtpApp commands
