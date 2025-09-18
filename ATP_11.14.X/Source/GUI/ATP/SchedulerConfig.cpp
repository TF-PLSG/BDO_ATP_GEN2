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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SchedulerConfig.cpp  $
   
      Rev 1.8   Dec 01 2003 11:30:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 12 2000 14:14:48   mbysani
   Changes mades acc to bugs
   
      Rev 1.4   Oct 15 1999 17:21:00   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:46   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:59:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jun 28 1999 13:59:24   iarustam
   Initial Release
   
 
*
************************************************************************************/
// SchedulerConfig.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "SchedulerConfig.h"

#include "schtypes.h"
#include "ProfileDlg.h"
#include "JobDlg.h"
#include "AppDlg.h"
#include "SchSearch.h"
#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CSchedulerConfig property page

IMPLEMENT_DYNCREATE(CSchedulerConfig, CPropertyPage)

CSchedulerConfig::CSchedulerConfig() : CPropertyPage(CSchedulerConfig::IDD)
{
	//{{AFX_DATA_INIT(CSchedulerConfig)
	m_Message = _T("");
	m_StrStatus = _T("");
	//}}AFX_DATA_INIT
}

CSchedulerConfig::~CSchedulerConfig()
{
}

void CSchedulerConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedulerConfig)
	DDX_Control(pDX, IDC_SCH_NEXT20, m_NextButton);
	DDX_Control(pDX, IDC_SCH_STATUS, m_SchStatus);
	DDX_Control(pDX, IDC_SCH_LIST, m_SchList);
	DDX_Control(pDX, IDC_SCH_TAB, m_SchTab);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_Message);
	DDX_Text(pDX, IDC_SCH_STATUS, m_StrStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedulerConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CSchedulerConfig)
	ON_BN_CLICKED(IDC_SCH_ADD, OnSchAdd)
	ON_BN_CLICKED(IDC_SCH_EDIT, OnSchEdit)
	ON_BN_CLICKED(IDC_SCH_DELETE, OnSchDelete)
	ON_BN_CLICKED(IDC_SCH_FIND, OnSchFind)
	ON_BN_CLICKED(IDC_SCH_NEXT20, OnSchNext20)
	ON_LBN_DBLCLK(IDC_SCH_LIST, OnDblclkSchList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SCH_TAB, OnSelchangeSchTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedulerConfig message handlers

BOOL CSchedulerConfig::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	 CFont m_font;
	LOGFONT lf;
   INT size  = 0;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
   m_Message = "";

	char	strTabTitle[40];
	int		nIndex;
	TC_ITEM ItemDef;		// TC_ITEM specifies or retrieves items of a tab
	char    strTabList[3][40];

	lstrcpy (strTabList[0], "   Profile   ");
	lstrcpy (strTabList[1], "   Application   ");
	lstrcpy (strTabList[2], "   Job   ");
   
	for (nIndex= 0; nIndex < 3; nIndex++ )
	{
		lstrcpy (strTabTitle, strTabList[nIndex]);
		// Specify parameters for the tab
		ItemDef.mask = TCIF_TEXT;
		ItemDef.pszText = strTabTitle;
		ItemDef.cchTextMax = lstrlen(strTabTitle);
		if (m_SchTab.InsertItem(nIndex, &ItemDef) == -1) {
			AfxMessageBox( "ERROR: Tab Control construction failed",MB_ICONSTOP);
			return FALSE ;
		}
	}


   // get the list of first 20 profiles and populate list
   INT   num_returned, loop;
   BYTE  rcode;
   CHAR  error_msg[256];

   m_TabSelected = TAB_PROFILE;

   memset( &m_bpf01, 0, sizeof( m_bpf01 ) );
   memset( &m_bpf01_list, 0, sizeof( BPF01_GUI_LIST ) );
   strcpy( (pCHAR)m_bpf01.primary_key.profile_id, "    " );
   if( ( rcode = txdsapi_get_list( (pBYTE)&m_bpf01, sizeof (BPF01),
									(pBYTE)&m_bpf01_list, sizeof (BPF01_GUI_LIST),
									0, BPF01_DATA, error_msg ) ) != PTEMSG_OK )
   {
         m_Message =  error_msg ;
         UpdateData(FALSE);
   }
   else
   {
      num_returned = atoi( (pCHAR)m_bpf01_list.num_returned );
      if ( num_returned < 21 )
      {
         m_NextButton.EnableWindow(FALSE);
         size = num_returned;
      }
      else
         size = num_returned - 1;
      if( num_returned == 0 )
      {
	//	 m_Message = "ERROR: No data found";
    //     UpdateData(FALSE);
         lstrcpy( m_LastProfileId, "   " );
      }
      else
      {
         for( loop = 0; loop < size; loop++ )
            m_SchList.AddString( (pCHAR)m_bpf01_list.profile_id [loop] );
         lstrcpy( m_LastProfileId, (pCHAR)m_bpf01_list.profile_id [num_returned-1] );
      }
   }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSchedulerConfig::OnSchAdd() 
{
	// TODO: Add your control notification handler code here
   BYTE rcode;
   CHAR error_msg[256];
   INT ret;
   CProfileDlg ProfileDlg;
   CJobDlg     JobDlg;
   CAppDlg     AppDlg;
   BPF01_SCH_LIST bpf01_sch_list;

   m_Message = "";
   UpdateData(FALSE);

   switch( m_TabSelected )
   {
      case TAB_PROFILE:
         ProfileDlg.m_mode = SCHMODE_ADD;
		 memset( &ProfileDlg.m_bpf01_sch_list, 0, sizeof( ProfileDlg.m_bpf01_sch_list ) );
         ret = ProfileDlg.DoModal();	
         if( ret == IDOK )
         {
            CString temp = ProfileDlg.m_bpf01_sch_list.batch_profile[0].primary_key.profile_id;
            strcpy ( (pCHAR)&m_bpf01.primary_key.profile_id, temp );
            rcode = txdsapi_get_sch_list( (pBYTE)&m_bpf01, sizeof (BPF01),
					(pBYTE)&bpf01_sch_list, sizeof (BPF01_SCH_LIST),
					0, BPF01_DATA, error_msg );

            if( rcode == PTEMSG_OK )
            {
               // if num returned is 0, the profile does not exist
               int num_returned = atoi( (pCHAR)bpf01_sch_list.num_returned );
               if( num_returned != 0 )
               {				   		
				   m_Message = "ERROR:Profile already exists.";
				   UpdateData(FALSE);	
               }
               else if( ( rcode = txdsapi_update_record( (pBYTE)&ProfileDlg.m_bpf01_sch_list, sizeof (BPF01_SCH_LIST), BPF01_DATA,error_msg ) ) != PTEMSG_OK )
			      {
				      m_Message = error_msg;
				      UpdateData(FALSE);
			      } 
               else
               {
                  m_Message = "Profile added.";
                  UpdateData(FALSE); 
                  strcpy( m_LastProfileId, "    " );
                  m_SchList.ResetContent();
                  OnSchNext20();
               }
            }
 
         }
         break;

      case TAB_JOB:
         JobDlg.m_mode = SCHMODE_ADD;
         memset( &JobDlg.m_jbf01, 0, sizeof( JobDlg.m_jbf01 ) );
         ret = JobDlg.DoModal();	
         if( ret == IDOK )
         {
            if( ( rcode = txdsapi_insert_record( (pBYTE)&JobDlg.m_jbf01, sizeof (JBF01), JBF01_DATA, error_msg ) ) != PTEMSG_OK )
			{
				m_Message = "ERROR:Job already exists.";
				UpdateData(FALSE);
			}
            else
            {
               m_Message = "Job added.";
               UpdateData(FALSE);
               strcpy( m_LastJobId, "    " );
               m_SchList.ResetContent();
               OnSchNext20();
            }
         }
         break;

   
	  case TAB_APP:
         AppDlg.m_mode = SCHMODE_ADD;
         memset( &AppDlg.m_apf01, 0, sizeof( AppDlg.m_apf01 ) );
         ret = AppDlg.DoModal();	
         if( ret == IDOK )
         {
            if( ( rcode = txdsapi_insert_record( (pBYTE)&AppDlg.m_apf01, sizeof (APF01), APF01_DATA, error_msg ) ) != PTEMSG_OK )
			{
				m_Message = "ERROR:Application already exists.";
				UpdateData(FALSE);
			}
            else
            {
               m_Message = "Application added.";
			   UpdateData(FALSE);
               strcpy( m_LastAppName, "    " );
               m_SchList.ResetContent();
               OnSchNext20();
            }
         }
         break;
   }
}



void CSchedulerConfig::OnSchEdit() 
{
	// TODO: Add your control notification handler code here
   INT   ListIndex;
   CProfileDlg ProfileDlg;
   CJobDlg     JobDlg;
   CAppDlg     AppDlg;
   BYTE rcode;
   CHAR error_msg[256];
   INT ret;

   m_Message = "";
   UpdateData(FALSE);
   ListIndex = m_SchList.GetCurSel();
   if( ListIndex == LB_ERR )
   {
      m_Message = "Please select a record to edit";
	  UpdateData(FALSE);
      return;
   }

   switch( m_TabSelected )
   {
      case TAB_PROFILE:
         ProfileDlg.m_mode = SCHMODE_EDIT;
			m_SchList.GetText( ListIndex, ProfileDlg.m_ProfileIdStr );
         ret = ProfileDlg.DoModal();	
         if( ret == IDOK )
         {
            if( ( rcode = txdsapi_update_record( (pBYTE)&ProfileDlg.m_bpf01_sch_list, sizeof (BPF01_SCH_LIST), BPF01_DATA, error_msg ) ) != PTEMSG_OK )
			{
               m_SchStatus.SetWindowText( error_msg );
			   m_Message =  error_msg ;
			}
            else
               m_Message = "Profile updated.";
               UpdateData(FALSE);

         }
         break;

      case TAB_JOB:
         JobDlg.m_mode = SCHMODE_EDIT;
			m_SchList.GetText( ListIndex, JobDlg.m_JobIdStr );
         memset( &JobDlg.m_jbf01, 0, sizeof( JobDlg.m_jbf01 ) );
         lstrcpy( (pCHAR)JobDlg.m_jbf01.primary_key.job_id, JobDlg.m_JobIdStr );
         rcode = txdsapi_get_record( (pBYTE)&JobDlg.m_jbf01, sizeof (JBF01), JBF01_DATA,error_msg );
         if( rcode != PTEMSG_OK )
         {
			 m_Message = "ERROR:Job does not exists.";
             UpdateData(FALSE);
            return;
         }

         ret = JobDlg.DoModal();	
         if( ret == IDOK )
         {
            if( ( rcode = txdsapi_update_record( (pBYTE)&JobDlg.m_jbf01, sizeof (JBF01), JBF01_DATA, error_msg ) ) != PTEMSG_OK )
			{
               m_SchStatus.SetWindowText( error_msg );
			   m_Message =  error_msg ;
			}
            else
			{
               m_SchStatus.SetWindowText( "Job Updated" );
			   m_Message = "Job Updated";
			}
			UpdateData(FALSE);

         }
         break;

      case TAB_APP:
         AppDlg.m_mode = SCHMODE_EDIT;
			m_SchList.GetText( ListIndex, AppDlg.m_AppNameStr );
         memset( &AppDlg.m_apf01, 0, sizeof( AppDlg.m_apf01 ) );
         lstrcpy( (pCHAR)AppDlg.m_apf01.primary_key.app_name, AppDlg.m_AppNameStr );
      rcode = txdsapi_get_record( (pBYTE)&AppDlg.m_apf01,sizeof (APF01), APF01_DATA, error_msg );
         if( rcode != PTEMSG_OK )
         {
			 m_SchStatus.SetWindowText( "ERROR: Application does not exist" );
			 m_Message =  "ERROR: Application does not exist" ;
			 UpdateData(FALSE);
            return;
         }

         ret = AppDlg.DoModal();	
         if( ret == IDOK )
         {
            if( ( rcode = txdsapi_update_record( (pBYTE)&AppDlg.m_apf01, sizeof (APF01), APF01_DATA, error_msg ) ) != PTEMSG_OK )
			{
               m_SchStatus.SetWindowText( error_msg );
			   m_Message =  error_msg ;
			}
            else
			{
               m_SchStatus.SetWindowText( "Application Updated" );
					m_Message = "Application Updated" ;
			}
			UpdateData(FALSE);

         }
         break;
   }
	
}


void CSchedulerConfig::OnSchDelete() 
{
	// TODO: Add your control notification handler code here
   INT   ListIndex;
   BYTE rcode;
   CHAR error_msg[256];
   INT resp;

   m_SchStatus.SetWindowText( " " );
   ListIndex = m_SchList.GetCurSel();
   if( ListIndex == LB_ERR )
   {
     m_Message = "Please select a record to delete";
	  UpdateData(FALSE);
      return;
   }

   switch( m_TabSelected )
   {
      case TAB_PROFILE:
         
         memset( &m_bpf01, 0, sizeof( m_bpf01 ) );
			m_SchList.GetText( ListIndex, (pCHAR)m_bpf01.primary_key.profile_id );
         
         //get confirmation from user
         sprintf( error_msg, "Do you wish to delete profile %s?", m_bpf01.primary_key.profile_id );
         resp = AfxMessageBox( error_msg, MB_YESNO | MB_ICONQUESTION );
         if( resp == IDNO )
              return;

        if( ( rcode = txdsapi_delete_record_by_subtype2( (pBYTE)&m_bpf01, sizeof( m_bpf01 ),
              BPF01_DATA,ST2_DB_DELETE_BPF01_BY_PROFILE_ID, error_msg ) ) != PTEMSG_OK )
		{
			m_Message = error_msg;
            m_SchStatus.SetWindowText( error_msg );      
			UpdateData(FALSE);
		}
         else
         {
            m_SchList.DeleteString( ListIndex );
            m_Message = "Profile Deleted";
            UpdateData(FALSE);
         }
         break;

   
      case TAB_JOB:
        
         memset( &m_jbf01, 0, sizeof( m_jbf01 ) );
			m_SchList.GetText( ListIndex, (pCHAR)m_jbf01.primary_key.job_id );
         
         //get confirmation from user
         sprintf( error_msg, "Do you wish to delete job %s?", m_jbf01.primary_key.job_id );
         resp = AfxMessageBox( error_msg, MB_YESNO | MB_ICONQUESTION );
         if( resp == IDNO )
              return;

         if( ( rcode = txdsapi_delete_record( (pBYTE)&m_jbf01, sizeof( m_jbf01 ),
                                              JBF01_DATA, error_msg ) ) != PTEMSG_OK )
		 {
            m_SchStatus.SetWindowText( error_msg );
			m_Message = error_msg;
			UpdateData(FALSE);
		 }
         else
         {
            m_SchList.DeleteString( ListIndex );
            m_Message = "Job Deleted";
            UpdateData(FALSE);
         }
         break;

      case TAB_APP:
         
         memset( &m_apf01, 0, sizeof( m_apf01 ) );
			m_SchList.GetText( ListIndex, (pCHAR)m_apf01.primary_key.app_name );
         
         //get confirmation from user
         sprintf( error_msg, "Do you wish to delete Application %s?", m_apf01.primary_key.app_name );
         resp = AfxMessageBox( error_msg, MB_YESNO | MB_ICONQUESTION );
         if( resp == IDNO )
              return;

         if( ( rcode = txdsapi_delete_record( (pBYTE)&m_apf01, sizeof( m_apf01 ),
                                              APF01_DATA, error_msg ) ) != PTEMSG_OK )
		 {
               m_SchStatus.SetWindowText( error_msg );
			   m_Message = error_msg;
			   UpdateData(FALSE);
		 }
         else
         {
            m_SchList.DeleteString( ListIndex );
            m_Message = "Application Deleted";
            UpdateData(FALSE);
         }
         break;
   }
	
}



void CSchedulerConfig::OnSchFind() 
{
	// TODO: Add your control notification handler code here
   CSchSearch SearchDlg;
   CProfileDlg ProfileDlg;
   CJobDlg JobDlg;
   CAppDlg AppDlg;
   INT ret, num_returned;
   BYTE rcode;
   CHAR error_msg[256];

   m_SchStatus.SetWindowText( " " );
   switch( m_TabSelected )
   {
      case TAB_PROFILE:
         SearchDlg.m_SearchLbl = "&Enter Profile ID to lookup";
         ret = SearchDlg.DoModal();
         if( ret == IDOK )
         {
            memset( &m_bpf01, 0, sizeof( m_bpf01 ) );
            lstrcpy( (pCHAR)m_bpf01.primary_key.profile_id, SearchDlg.m_SearchItem );
            memset( &m_bpf01_sch_list, 0, sizeof( m_bpf01_sch_list ) );
            rcode = txdsapi_get_sch_list( (pBYTE)&m_bpf01, sizeof (BPF01),
										(pBYTE)&m_bpf01_sch_list, sizeof (BPF01_SCH_LIST),
										0, BPF01_DATA, error_msg );

            if( rcode == PTEMSG_OK )
            {
               // if num returned is 0, the profile does not exist
               num_returned = atoi( (pCHAR)m_bpf01_sch_list.num_returned );
               if( num_returned == 0 )
               {				   		
				   m_Message = "ERROR:Profile does not exists.";
				   UpdateData(FALSE);	
               }
               else
               {
                  ProfileDlg.m_mode = SCHMODE_EDIT;
                  ProfileDlg.m_ProfileIdStr = SearchDlg.m_SearchItem;
                  ret = ProfileDlg.DoModal();	
                  if( ret == IDOK )
                  {
                     if( ( rcode = txdsapi_update_record( (pBYTE)&ProfileDlg.m_bpf01_sch_list, sizeof (BPF01), BPF01_DATA, error_msg ) ) != PTEMSG_OK )
					   m_SchStatus.SetWindowText( error_msg );
                     else
                        m_SchStatus.SetWindowText( "Profile Updated" );
                  }
               }
            }
            else
               m_SchStatus.SetWindowText( error_msg );               
         }
		 UpdateData(FALSE);	
         break;

      case TAB_JOB:
         SearchDlg.m_SearchLbl = "&Enter Job ID to lookup";
         ret = SearchDlg.DoModal();
         if( ret == IDOK )
         {
            memset( &JobDlg.m_jbf01, 0, sizeof( JobDlg.m_jbf01 ) );
            lstrcpy( (pCHAR)JobDlg.m_jbf01.primary_key.job_id, SearchDlg.m_SearchItem );
            rcode = txdsapi_get_record( (pBYTE)&JobDlg.m_jbf01, sizeof (JBF01), JBF01_DATA, error_msg );
            if( rcode == PTEMSG_OK )
            {
               JobDlg.m_mode = SCHMODE_EDIT;
               JobDlg.m_JobIdStr = SearchDlg.m_SearchItem;
               ret = JobDlg.DoModal();	
               if( ret == IDOK )
               {
                  if( ( rcode = txdsapi_update_record( (pBYTE)&JobDlg.m_jbf01, sizeof (JBF01), JBF01_DATA,error_msg ) ) != PTEMSG_OK )
                     m_SchStatus.SetWindowText( error_msg );
                  else
                     m_SchStatus.SetWindowText( "Job Updated" );
               }
            }
            else               
            {
				m_Message = "ERROR:Job does not exist.";
				UpdateData(FALSE);
			}
         }
         break;

      case TAB_APP:
         SearchDlg.m_SearchLbl = "&Enter App Name to lookup";
         ret = SearchDlg.DoModal();
         if( ret == IDOK )
         {
            memset( &AppDlg.m_apf01, 0, sizeof( AppDlg.m_apf01 ) );
            lstrcpy( (pCHAR)AppDlg.m_apf01.primary_key.app_name, SearchDlg.m_SearchItem );
      rcode = txdsapi_get_record( (pBYTE)&AppDlg.m_apf01,sizeof (APF01), APF01_DATA, error_msg );
            if( rcode == PTEMSG_OK )
            {
               AppDlg.m_mode = SCHMODE_EDIT;
               AppDlg.m_AppNameStr = SearchDlg.m_SearchItem;
               ret = AppDlg.DoModal();	
               if( ret == IDOK )
               {
                  if( ( rcode = txdsapi_update_record( (pBYTE)&AppDlg.m_apf01, sizeof (APF01), APF01_DATA,error_msg ) ) != PTEMSG_OK )
                     m_SchStatus.SetWindowText( error_msg );
                  else
                     m_SchStatus.SetWindowText( "Application Updated" );
               }
            }
            else
			{
				m_Message = "ERROR:Application does not exist.";
				UpdateData(FALSE);
			}               
         }
         break;	  

   }
	
	
}



void CSchedulerConfig::OnSchNext20() 
{
	// TODO: Add your control notification handler code here
   INT   num_returned, loop;
   BYTE  rcode;
   CHAR  error_msg[256];
   INT   size = 0;

   m_NextButton.EnableWindow(TRUE);

   m_TabSelected = m_SchTab.GetCurSel();
   switch( m_TabSelected )
   {
      case TAB_PROFILE:
         memset( &m_bpf01, 0, sizeof( m_bpf01 ) );
         memset( &m_bpf01_list, 0, sizeof( BPF01_GUI_LIST ) );
         strcpy( (pCHAR)m_bpf01.primary_key.profile_id, m_LastProfileId );
         if( ( rcode = txdsapi_get_list( (pBYTE)&m_bpf01, sizeof (BPF01),
										(pBYTE)&m_bpf01_list, sizeof (BPF01_GUI_LIST),
										0, BPF01_DATA, error_msg ) ) != PTEMSG_OK )
               m_SchStatus.SetWindowText( error_msg );
         else
         {
            num_returned = atoi( (pCHAR)m_bpf01_list.num_returned );
            if ( num_returned < 21 )
            {
               m_NextButton.EnableWindow(FALSE);
               size = num_returned;
            }
            else
               size = num_returned - 1;

            if( num_returned == 0 )
				m_SchStatus.SetWindowText( "ERROR: No data found" );
            else
            {
               for( loop = 0; loop < size; loop++ )
                  m_SchList.AddString( (pCHAR)m_bpf01_list.profile_id [loop] );
               lstrcpy( m_LastProfileId, (pCHAR)m_bpf01_list.profile_id [num_returned-1] );
            }
         }
         break;

      case TAB_JOB:
         memset( &m_jbf01, 0, sizeof( m_jbf01 ) );
         memset( &m_jbf01_list, 0, sizeof( JBF01_GUI_LIST ) );
         strcpy( (pCHAR)m_jbf01.primary_key.job_id, m_LastJobId );
         if( ( rcode = txdsapi_get_list( (pBYTE)&m_jbf01, sizeof (JBF01),
										(pBYTE)&m_jbf01_list, sizeof (JBF01_GUI_LIST),
										0, JBF01_DATA, error_msg ) ) != PTEMSG_OK )
               m_SchStatus.SetWindowText( error_msg );
         else
         {
            num_returned = atoi( (pCHAR)m_jbf01_list.num_returned );
            if ( num_returned < 21 )
            {
               m_NextButton.EnableWindow(FALSE);
               size = num_returned;
            }
            else
               size = num_returned - 1;

            if( num_returned == 0 )
				m_SchStatus.SetWindowText( "ERROR: No data found" );
            else
            {
               for( loop = 0; loop < size; loop++ )
                  m_SchList.AddString( (pCHAR)m_jbf01_list.job_id [loop] );
               lstrcpy( m_LastJobId, (pCHAR)m_jbf01_list.job_id [num_returned-1] );
            }
         }
         break;

      case TAB_APP:
         memset( &m_apf01, 0, sizeof( m_apf01 ) );
         memset( &m_apf01_list, 0, sizeof( APF01_GUI_LIST ) );
         strcpy( (pCHAR)m_apf01.primary_key.app_name, m_LastAppName );
         if( ( rcode = txdsapi_get_list( (pBYTE)&m_apf01, sizeof (APF01), 
													(pBYTE)&m_apf01_list, sizeof (APF01_GUI_LIST),
													0, APF01_DATA, error_msg ) ) != PTEMSG_OK )
               m_SchStatus.SetWindowText( error_msg );
         else
         {
            num_returned = atoi( (pCHAR)m_apf01_list.num_returned );
            if ( num_returned < 21 )
            {
               m_NextButton.EnableWindow(FALSE);
               size = num_returned;
            }
            else
               size = num_returned - 1;
            if( num_returned == 0 )
				m_SchStatus.SetWindowText( "ERROR: No data found" );
            else
            {
				m_SchStatus.SetWindowText( " " );
               for( loop = 0; loop < size; loop++ )
                  m_SchList.AddString( (pCHAR)m_apf01_list.app_name [loop] );
               lstrcpy( m_LastAppName, (pCHAR)m_apf01_list.app_name [num_returned-1] );
            }
         }
         break;

   }
	
	
}

void CSchedulerConfig::OnDblclkSchList() 
{
	// TODO: Add your control notification handler code here
   m_Message = "";
   UpdateData(FALSE);
   OnSchEdit();	
}




void CSchedulerConfig::OnSelchangeSchTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
   m_SchList.ResetContent();
   strcpy( m_LastProfileId, "   " );
   strcpy( m_LastJobId, "   " );
   strcpy( m_LastAppName, "   " );   
   m_Message = "";
   m_SchStatus.SetWindowText( " " );

   UpdateData(FALSE);
			
   OnSchNext20();
	UpdateData(FALSE);
	*pResult = 0;
}

BOOL CSchedulerConfig::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
