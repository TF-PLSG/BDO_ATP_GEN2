// ArchiveLookup.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ArchiveLookup.h"
#include "ArchiveLookupDlg.h"
#include "MainMenuDlg.h"

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

CString gszUserName;
char strFileRecord[56] = "";
int idleTime;
extern "C"
{
	//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
	CHAR appProductVersion[MAX_APPVERSION_SIZE];
}

/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupApp

BEGIN_MESSAGE_MAP(CArchiveLookupApp, CWinApp)
//{{AFX_MSG_MAP(CArchiveLookupApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupApp construction

CArchiveLookupApp::CArchiveLookupApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CArchiveLookupApp object

CArchiveLookupApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupApp initialization

BOOL CArchiveLookupApp::InitInstance()
{
	AfxEnableControlContainer();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	char error_msg[300];
	CMyUtils UtilsObject;
	CString szConnectStr;
    CHAR ini_file[256];
	
	//Get App Product Version from String Table & copy it into appProductVersion 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	
	// establish connection with applink server
	// The connection INIT is performed once here regardless of CONNECTMODE
	if( !ptetcp_init( error_msg ) )
	{
		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
		return FALSE;
	}
	
	// Get the connection configuration parameter "CONNECTMODE". If not present
	// default to "MULTIPLE". The other valid value is "SINGLE" to enable connect and
	// disconnect only at start up and exit. Other values same as MULTIPLE.
	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );
	
	GetPrivateProfileString("GUI", "CONNECTMODE", "multiple", szConnectStr.GetBuffer(MAX_NAME_LENGTH),MAX_NAME_LENGTH,ini_file );
	szConnectStr.ReleaseBuffer(MAX_NAME_LENGTH);
	szConnectStr.MakeUpper();
	if ( 0 == szConnectStr.Compare("SINGLE") ) {
		// Configured for SINGLE archlnk connection at start up
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
	
    char tmp[256]={'\0'};
    int cnt = 0, j = 0;
    char u_name[51]={'\0'};
    char u_pwd[51]={'\0'};    
	BYTE rcode;		
	UCF01 ucf01;
	
	CString szCmdLine = m_lpCmdLine;
	
	
	int pos = szCmdLine.Find(' ');
	gszUserName = szCmdLine.Left(pos);
	CString szPwd = szCmdLine.Right(szCmdLine.GetLength() - (pos + 1));
	
	if(gszUserName.GetLength() == 0 || szPwd.GetLength() == 0)
	{
		return false;
	}
	
	//Get User info and check the given password.
	
	memset( &ucf01, 0, sizeof( UCF01 ) );
	strcpy( (char *)ucf01.primary_key.name, gszUserName);     
/*	rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
	
	CString str;
	GetCurrentDirectory(120, str.GetBuffer(120));
	str.ReleaseBuffer(120);
	if( rcode == PTEMSG_NOT_FOUND )
	{        
		//return false;
	}
	else if( rcode != PTEMSG_OK )
	{        
        //return FALSE;
	}
	// check password	
	if( szPwd.Compare((char *)ucf01.password) != 0 )
	{		
		//return FALSE;
	}   
	*/
	RunDialog();
	
	if ( 0 == szConnectStr.Compare("SINGLE") )
	{
		// before exiting this GUI. Will prevent archlnk & system problems.
		ptetcp_disconnect();
	}
	
	return FALSE;
}

void CArchiveLookupApp::RunDialog()
{
	//Start Dialog
	
	CMainMenuDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}
