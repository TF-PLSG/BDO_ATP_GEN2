/****************************************************************************************

   Module: srvcmon.cpp

   Title: Pinnacle Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\SRVCMON.CPP  $
   
      Rev 1.3   Mar 02 1999 16:37:40   MSALEH
   increase the time to wait for a response from 
   service manager
   
      Rev 1.2   Sep 04 1998 13:09:24   skatuka2
   1. Modified the title.
   
      Rev 1.1   20 Aug 1998 09:03:22   rmalathk
   minor bug fixes and changes needed for applink.
   
      Rev 1.0   27 May 1998 16:54:24   rmalathk
   Initial Revision
   
*****************************************************************************************/
// srvcmon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "srvcmon.h"

#include "MainFrm.h"
#include "srvcmonDoc.h"
#include "srvcmonView.h"
#include "odbclogin.h"	
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString cmdlinestr;
char u_name[51]={'\0'};
char u_pwd[51]={'\0'};
extern "C"
{
	//Krishna@TF, Contains Version number of the App
	extern CHAR  appProductVersion[MAX_APPVERSION_SIZE];
}

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonApp

BEGIN_MESSAGE_MAP(CSrvcmonApp, CWinApp)
	//{{AFX_MSG_MAP(CSrvcmonApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonApp construction

CSrvcmonApp::CSrvcmonApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSrvcmonApp object

CSrvcmonApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonApp initialization

BOOL CSrvcmonApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CWnd    *wnd;
				
	wnd = CWnd::FindWindow(NULL, "Ascendent Services Monitor - ATP Gen-II");
	

	if (wnd != NULL)
	{
		wnd->SetForegroundWindow();
		wnd->PostMessage(WM_SYSCOMMAND, SC_RESTORE,0);
		return(FALSE);
	}


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

/*
   CODBCLogin ODBCLogin;
	ODBCLogin.ODBCLogon();
    
	//If OK disconnect and continue as normal.
	if (ODBCLogin.bLogonSuccess == TRUE)
        ODBCLogin.FreeConnect();
	else
	{
	   //Free the handles and terminate application.
	   ODBCLogin.FreeConnect();
	   AfxMessageBox( "Access denied! Please use the Ascendent Main Menu.", MB_ICONEXCLAMATION, 0);
	   return(FALSE);
	}
*/
    // Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSrvcmonDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSrvcmonView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//Krishan@TF, 26.5.11, Done to validate rumnning of Monitor Exe by Cmdline Args
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
		
		return FALSE;
	}  
	//Krishna changes end

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	//Set the title for the application
	m_pMainWnd->SetWindowText("Ascendent Services Monitor - ATP Gen-II");
	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pMainWnd->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pMainWnd->SetWindowText(lpszTitle);



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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSrvcmonApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	char temp_buff[50];
	sprintf(temp_buff, "ATP - Service Monitor.\nVersion : %s", appProductVersion);
	aboutDlg.m_appversion = temp_buff;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonApp commands

int CSrvcmonApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::ExitInstance();
}
