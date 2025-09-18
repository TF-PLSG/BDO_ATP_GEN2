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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\JOBDLG.CPP  $
   
      Rev 1.6   Dec 01 2003 11:29:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:50:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:36   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:19:40   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:56:24   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:28   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:52   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:08   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:18:02   iarustam
    
   
      Rev 1.0   Dec 04 1998 15:21:02   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:04:16   MSALEH
   initial release
*
************************************************************************************/
// JobDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "JobDlg.h"
#include "SchTypes.h"

#include "app_info.h"
#include "MyUtils.h"

#include "SharedMem.h"


extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobDlg dialog


CJobDlg::CJobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJobDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobDlg)
	m_JobIdStr = _T("");
	m_LastRun = _T("");
	m_Message = _T("");
	//}}AFX_DATA_INIT
}


void CJobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobDlg)
	DDX_Control(pDX, IDC_STATUS, m_Status);
	DDX_Control(pDX, IDC_JOB_ID, m_JobIdCtrl);
	DDX_Control(pDX, IDC_MINUTE, m_Minute);
	DDX_Control(pDX, IDC_APPNAME, m_AppName);
	DDX_Control(pDX, IDC_HOUR, m_Hour);
	DDX_Text(pDX, IDC_JOB_ID, m_JobIdStr);
	DDV_MaxChars(pDX, m_JobIdStr, 15);
	DDX_Text(pDX, IDC_LAST_RUN, m_LastRun);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobDlg, CDialog)
	//{{AFX_MSG_MAP(CJobDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobDlg message handlers


BOOL CJobDlg::OnInitDialog() 
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
   CHAR temp_str[5], temp_str2[5];
   INT  loop, done, num_returned, index;
   BYTE rcode;
   CHAR error_msg[256];
   APF01_GUI_LIST apf01_list;
   APF01 apf01;
   m_Message = "";
         
   // populate all the combo boxes

   // hour box
   for( loop = 0; loop < 24; loop++ )
   {
      itoa( loop, temp_str, 10 );
      if( loop < 10 )
         sprintf( temp_str2, "0%s", temp_str );
      else
         sprintf( temp_str2, "%s", temp_str );
      m_Hour.AddString( temp_str2 );
   }

   // minute box
   for( loop = 0; loop < 60; loop++ )
   {
      itoa( loop, temp_str, 10 );
      if( loop < 10 )
         sprintf( temp_str2, "0%s", temp_str );
      else
         sprintf( temp_str2, "%s", temp_str );
      m_Minute.AddString( temp_str2 );
   }

   // status box
   m_Status.AddString( "Active" );
   m_Status.AddString( "Inactive" );

   // appname box
   done = 0;
   lstrcpy( (pCHAR)apf01.primary_key.app_name, "   " );
   while( !done )
   {
      memset( &apf01_list, 0, sizeof( apf01_list ) );
         if( ( rcode = txdsapi_get_list( (pBYTE)&apf01, sizeof (APF01), 
													(pBYTE)&apf01_list, sizeof (APF01_GUI_LIST),
													0, APF01_DATA, error_msg ) ) != PTEMSG_OK )
      {
         AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
         break;
      }
      else
      {
         num_returned = atoi( (pCHAR)apf01_list.num_returned );
         for( loop = 0; loop < num_returned; loop++ )
               m_AppName.AddString( (pCHAR)apf01_list.app_name[loop] );

         if( num_returned < GUI_MAX_LIST_SIZE )
            done = 1;
         else
            lstrcpy( (pCHAR)apf01.primary_key.app_name, (pCHAR)apf01_list.app_name[num_returned-1] );
      }

   }

   switch( m_mode )
   {
      case SCHMODE_EDIT:
         // disable primary key field
         m_JobIdCtrl.EnableWindow( FALSE );

         // populate remaining fields
         strncpy( temp_str, (pCHAR)m_jbf01.time_to_run, 2 );
         temp_str[2] = '\0';

         index = m_Hour.FindString( -1, temp_str );
         m_Hour.SetCurSel( index );

         strcpy( temp_str, (pCHAR)&m_jbf01.time_to_run[2] );
         index = m_Minute.FindString( -1, temp_str );
         m_Minute.SetCurSel( index );

         if( strcmp( (pCHAR)m_jbf01.status, "A" ) == 0 )
            m_Status.SetCurSel( 0 );
         else
            m_Status.SetCurSel( 1 );

         index = m_AppName.FindString( -1, (pCHAR)m_jbf01.app_name );
         m_AppName.SetCurSel( index );

         m_LastRun = (pCHAR)m_jbf01.last_run;
         UpdateData( FALSE );
         break;

      case SCHMODE_ADD:

         m_Hour.SetCurSel( 0 );
         m_Minute.SetCurSel( 0 );
         m_Status.SetCurSel( 0 );
         m_AppName.SetCurSel( 0 );
         break;

      case SCHMODE_ADD_TEMP:

         m_JobIdCtrl.EnableWindow( FALSE );
         m_Hour.SetCurSel( 0 );
         m_Minute.SetCurSel( 0 );
         m_Status.SetCurSel( 0 );
         m_AppName.SetCurSel( 0 );
         break;

   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CJobDlg::OnOK() 
{
	// TODO: Add extra validation here
   INT ret;
   CHAR temp_str[20];

   CMyUtils myUtilsObject;

   UpdateData( TRUE );

   if ( myUtilsObject.IsEmpty (m_JobIdStr, this))
   {
	   m_Message = "Please enter Job ID";
	   UpdateData(FALSE);
	   return;	
   }

   ret = m_AppName.GetCurSel();
   if( ret == CB_ERR )
   {
      m_Message = "Please enter App Name";
	  UpdateData(FALSE);
      return;
   }
   m_AppName.GetLBText( ret, temp_str );
   lstrcpy( (pCHAR)m_jbf01.app_name, temp_str );

   ret = m_Status.GetCurSel();
   if( ret == 0 ) //active
      lstrcpy( (pCHAR)m_jbf01.status, "A" );
   else
      lstrcpy( (pCHAR)m_jbf01.status, "I" );

   ret = m_Hour.GetCurSel();
   m_Hour.GetLBText( ret, temp_str );
   lstrcpy( (pCHAR)m_jbf01.time_to_run, temp_str );

   ret = m_Minute.GetCurSel();
   m_Minute.GetLBText( ret, temp_str );
   lstrcat( (pCHAR)m_jbf01.time_to_run, temp_str );

	lstrcpy( (pCHAR)m_jbf01.primary_key.job_id, m_JobIdStr );
	CDialog::OnOK();
}

BOOL CJobDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
