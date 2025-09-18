// PinDlg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"


extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
#include "cdsapi.h"

#include "ptetcp.h"
#include "ptemsg.h"

#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "TXDSAPI.h"

}


#include "PinDlg.h"
#include "ComSetupPg.h"
#include "DialogPg.h"
#include "TPDUPg.h"
#include "PropSheet.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSharedMem shrmem;

extern "C"
{
	//Krishna@TF, Contains Version number of the App
	CHAR  appProductVersion[MAX_APPVERSION_SIZE];
}
/////////////////////////////////////////////////////////////////////////////
// CPinDlgApp

BEGIN_MESSAGE_MAP(CPinDlgApp, CWinApp)
	//{{AFX_MSG_MAP(CPinDlgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPinDlgApp construction
BOOL AppliedComSet = TRUE;
BOOL AppliedTPDUSet = TRUE;
BOOL AppliedRouterSet = TRUE;
BOOL Connected;
BOOL NewComSetup;
BOOL Trace;
char Servname[100];
int Servsel;
DM_CONFIG DmConfig;		//get instance of DM_Config table structure
DM_CONFIG_LIST DmConfig_list;
ROUTER_TABLE_LIST Router_list;
TPDU_TABLE_LIST TPDU_list;
ROUTER_TABLE Router;	//get instance of Router table structure
TPDU TPDUTable;	//get instance of TPDU table structure
HANDLE  hGET_TPDU;
HANDLE  hGET_ROUTER;
HANDLE	hUPDATE_DM_CONFIG;
HANDLE  hUPDATE_ROUTER;
HANDLE  hUPDATE_TPDU;
HANDLE  hINSERT_ROUTER;
HANDLE  hINSERT_TPDU;
HANDLE  hDELETE_ROUTER;
HANDLE  hDELETE_TPDU;


CPinDlgApp::CPinDlgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPinDlgApp object

CPinDlgApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CPinDlgApp initialization

BOOL CPinDlgApp::InitInstance()
{

	CWnd *wnd;
   CHAR error_msg[256];
   CHAR server_name[56];

	//Check if there is other instance of the same application is running now.
	wnd = CWnd::FindWindow(NULL,"Ascendent System - Dialog Manager Gen-II");
	if (wnd != NULL)
	{
		wnd->SetForegroundWindow();
		wnd->PostMessage(WM_SYSCOMMAND,SC_RESTORE,0);
		return FALSE;
	}

	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

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



	CPropSheet sht;
	m_pMainWnd = &sht;
	Run();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

   ptetcp_disconnect();
	return FALSE;
}



