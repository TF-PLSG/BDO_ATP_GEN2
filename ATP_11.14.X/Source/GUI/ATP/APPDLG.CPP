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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\APPDLG.CPP  $
   
	 
 Rev        April 19.5.11          Krishna Kiran S@TF
	 Added Reseting Idle time, whenever any event is fired in GUI

      Rev 1.6   Dec 01 2003 11:28:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:52:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.3   Sep 30 1999 16:16:14   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:39:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:40   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:44   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 10:55:26   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:55:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:40   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:56:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:04:40   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:17:20   iarustam
   bug fixes
   
      Rev 1.0   Dec 04 1998 15:19:58   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:02:56   MSALEH
   initial release
*
************************************************************************************/
// AppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "AppDlg.h"
#include "SchTypes.h"
#include "MyUtils.h"
#include "Sharedmem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CAppDlg dialog


CAppDlg::CAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppDlg)
	m_AppNameStr = _T("");
	m_ExeName = _T("");
	m_Parameters = _T("");
	m_Path = _T("");
	m_Message = _T("");
	//}}AFX_DATA_INIT
}


void CAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppDlg)
	DDX_Control(pDX, IDC_APP_NAME, m_AppNameCtrl);
	DDX_Text(pDX, IDC_APP_NAME, m_AppNameStr);
	DDV_MaxChars(pDX, m_AppNameStr, 15);
	DDX_Text(pDX, IDC_EXE_NAME, m_ExeName);
	DDV_MaxChars(pDX, m_ExeName, 15);
	DDX_Text(pDX, IDC_PARAMETERS, m_Parameters);
	DDV_MaxChars(pDX, m_Parameters, 99);
	DDX_Text(pDX, IDC_PATH, m_Path);
	DDV_MaxChars(pDX, m_Path, 99);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppDlg, CDialog)
	//{{AFX_MSG_MAP(CAppDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppDlg message handlers

BOOL CAppDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	 CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
   m_Message = "";
   switch( m_mode )
   {
      case SCHMODE_EDIT:
         m_AppNameCtrl.EnableWindow( FALSE );
         m_AppNameStr = (pCHAR)m_apf01.primary_key.app_name;
         m_ExeName = (pCHAR)m_apf01.exe_name;
         m_Path =  (pCHAR)m_apf01.path;
         m_Parameters = (pCHAR)m_apf01.parameters;
         UpdateData( FALSE );
         break;
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAppDlg::OnOK() 
{
	// TODO: Add extra validation here

   // check for blank fields

   CMyUtils myUtilsObject;

   UpdateData( TRUE );

   if ( myUtilsObject.IsEmpty (m_AppNameStr, this))
   {
	   m_Message = "Please enter App Name";
	   UpdateData(FALSE);
	   return;	
   }
   lstrcpy( (pCHAR)m_apf01.primary_key.app_name, m_AppNameStr );

   if( m_ExeName.IsEmpty() )
   {
      m_Message = "Please enter Exe Name";
	  UpdateData(FALSE);
      return;
   }
   lstrcpy( (pCHAR)m_apf01.exe_name, m_ExeName );
   
   if( m_Path.IsEmpty() )
   {
      m_Message = "Please enter Path";
	  UpdateData(FALSE);
      return;
   }
   lstrcpy( (pCHAR)m_apf01.path, m_Path );
   lstrcpy( (pCHAR)m_apf01.parameters, m_Parameters );
   
	CDialog::OnOK();
}

BOOL CAppDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
