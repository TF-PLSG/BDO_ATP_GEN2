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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ProfileDlg.cpp  $
   
      Rev 1.6   Dec 01 2003 11:30:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:19:14   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:58:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:08   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:16   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:32:28   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:56:56   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:04   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:56   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:58   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:36   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:04   iarustam
    
   
      Rev 1.0   Dec 04 1998 15:21:38   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:05:26   MSALEH
   initial release
*
************************************************************************************/
// ProfileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "ProfileDlg.h"
#include "MyUtils.h"

#include "schtypes.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileDlg dialog

extern CSharedMem shrmem;

CProfileDlg::CProfileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProfileDlg)
	m_ProfileIdStr = _T("");
	m_Message = _T("");
	//}}AFX_DATA_INIT
}


void CProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileDlg)
	DDX_Control(pDX, IDC_PROFILE_ID, m_ProfileIdCtrl);
	DDX_Control(pDX, IDC_JOBS_INPROFILE_LIST, m_JobsInProfileList);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_AvailableList);
	DDX_Text(pDX, IDC_PROFILE_ID, m_ProfileIdStr);
	DDV_MaxChars(pDX, m_ProfileIdStr, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileDlg, CDialog)
	//{{AFX_MSG_MAP(CProfileDlg)
	ON_BN_CLICKED(IDC_PROFILE_ADD, OnProfileAdd)
	ON_BN_CLICKED(IDC_PROFILE_REMOVE, OnProfileRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileDlg message handlers

BOOL CProfileDlg::OnInitDialog() 
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
   BPF01 bpf01;
   JBF01 jbf01;
   JBF01_GUI_LIST jbf01_list;

   INT num_returned, loop, done;
   BYTE rcode;
   CHAR error_msg[256];
   m_Message = "";

   switch( m_mode )
   {
      case SCHMODE_EDIT:
         // disable primary key field
         m_ProfileIdCtrl.EnableWindow( FALSE );

         // get list of jobs in profile
         memset( &m_bpf01_sch_list, 0, sizeof( m_bpf01_sch_list ) );
         memset( &bpf01, 0, sizeof( bpf01 ) );
         lstrcpy( (pCHAR)bpf01.primary_key.profile_id, m_ProfileIdStr );
         if( ( rcode = txdsapi_get_sch_list( (pBYTE)&bpf01, sizeof (BPF01),
										(pBYTE)&m_bpf01_sch_list, sizeof (BPF01_SCH_LIST),
										0, BPF01_DATA, error_msg ) ) != PTEMSG_OK )
         {
            AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
         }
         else
         {
            num_returned = atoi( (pCHAR)m_bpf01_sch_list.num_returned );
            for( loop = 0; loop < num_returned; loop++ )
               m_JobsInProfileList.AddString( (pCHAR)m_bpf01_sch_list.batch_profile[loop].primary_key.job_id );
         }

         // get list of all jobs and populate the Available List
         done = 0;
         lstrcpy( (pCHAR)jbf01.primary_key.job_id, "   " );
         while( !done )
         {
            memset( &jbf01_list, 0, sizeof( jbf01_list ) );
            if( ( rcode = txdsapi_get_list( (pBYTE)&jbf01, sizeof (JBF01),
											(pBYTE)&jbf01_list, sizeof (JBF01_GUI_LIST),
											0, JBF01_DATA, error_msg ) ) != PTEMSG_OK )
            {
               AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
               break;
            }
            else
            {
               num_returned = atoi( (pCHAR)jbf01_list.num_returned );
               for( loop = 0; loop < num_returned; loop++ )
               {
                  if( m_JobsInProfileList.FindString( -1, (pCHAR)jbf01_list.job_id[loop] ) == LB_ERR )
                     m_AvailableList.AddString( (pCHAR)jbf01_list.job_id[loop] );
               }

               if( num_returned < GUI_MAX_LIST_SIZE )
                  done = 1;
               else
                  lstrcpy( (pCHAR)jbf01.primary_key.job_id, (pCHAR)jbf01_list.job_id[num_returned-1] );
            }

         }
         break;

      case SCHMODE_ADD:

         // get list of all jobs and populate the Available List
         done = 0;
         lstrcpy( (pCHAR)jbf01.primary_key.job_id, "   " );
         while( !done )
         {
            memset( &jbf01_list, 0, sizeof( jbf01_list ) );
            if( ( rcode = txdsapi_get_list( (pBYTE)&jbf01, sizeof (JBF01),
											(pBYTE)&jbf01_list, sizeof (JBF01_GUI_LIST),
											0, JBF01_DATA, error_msg ) ) != PTEMSG_OK )
            {
               AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
               break;
            }
            else
            {
               num_returned = atoi( (pCHAR)jbf01_list.num_returned );
               for( loop = 0; loop < num_returned; loop++ )
                  m_AvailableList.AddString( (pCHAR)jbf01_list.job_id[loop] );

               if( num_returned < GUI_MAX_LIST_SIZE )
                  done = 1;
               else
                  lstrcpy( (pCHAR)jbf01.primary_key.job_id, (pCHAR)jbf01_list.job_id[num_returned-1] );
            }

         }
         break;
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CProfileDlg::OnProfileAdd() 
{
	// TODO: Add your control notification handler code here
   INT ListIndex;
   CString SelectedStr;

   UpdateData(TRUE);

   // check if user selected anything
   ListIndex = m_AvailableList.GetCurSel();
   if( ListIndex == LB_ERR )
   {
      m_Message = "Please select a job to add from the list of available jobs";
	  UpdateData(FALSE);
      return;
   }

   // check if the limit of 100 has been reached
   if( m_JobsInProfileList.GetCount() == SCH_MAX_LIST_SIZE )
   {
      m_Message = "The maximum limit of 100 jobs for this profile has been reached";
	  UpdateData(FALSE);
      return;
   }

   m_AvailableList.GetText( ListIndex, SelectedStr );
   m_JobsInProfileList.AddString( SelectedStr );
   m_AvailableList.DeleteString( ListIndex );

	
}


void CProfileDlg::OnProfileRemove() 
{
	// TODO: Add your control notification handler code here
	
   INT ListIndex;
   CString SelectedStr;

   ListIndex = m_JobsInProfileList.GetCurSel();
   if( ListIndex == LB_ERR )
   {
      m_Message = "Please select a job to remove from the list of jobs in profile";
	  UpdateData(FALSE);
      return;
   }

   m_JobsInProfileList.GetText( ListIndex, SelectedStr );
   m_AvailableList.AddString( SelectedStr );
   m_JobsInProfileList.DeleteString( ListIndex );
}



void CProfileDlg::OnOK() 
{
	// TODO: Add extra validation here
   INT count, loop;
   CString  JobIdStr;
   CMyUtils myUtilsObject;

   UpdateData( TRUE );

   if ( myUtilsObject.IsEmpty (m_ProfileIdStr, this))
   {
	  m_Message = "Please enter Profile ID";
	  UpdateData(FALSE);
	   return;	
   }

   // verify that there is at least one job selected for profile
   count = m_JobsInProfileList.GetCount();
   if(  count == 0 )
   {
	  m_Message = "ERROR: At least one job must be added for the profile";
	  UpdateData(FALSE);
      return;
   }
	
   memset( &m_bpf01_sch_list, 0, sizeof( m_bpf01_sch_list ) );
   for( loop = 0; loop < count; loop++ )
   {
      m_JobsInProfileList.GetText( loop, JobIdStr );
      lstrcpy( (pCHAR)m_bpf01_sch_list.batch_profile[loop].primary_key.profile_id, m_ProfileIdStr );
      lstrcpy( (pCHAR)m_bpf01_sch_list.batch_profile[loop].primary_key.job_id, JobIdStr );
   }

   itoa( count, (pCHAR)m_bpf01_sch_list.num_returned, 10 );
	CDialog::OnOK();
}

BOOL CProfileDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
