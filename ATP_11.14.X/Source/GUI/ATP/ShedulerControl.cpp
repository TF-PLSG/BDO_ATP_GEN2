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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ShedulerControl.cpp  $
   
      Rev 1.6   Dec 01 2003 11:30:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:26   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:22   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:28   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:39:20   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:57:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:08   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:58   iarustam  
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:22   iarustam
    
   
      Rev 1.0   Dec 04 1998 15:21:52   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:05:34   MSALEH
   initial release
*
************************************************************************************/
// ShedulerControl.cpp : implementation file
//

    
#include "stdafx.h"
#include "Atp.h"
#include "ShedulerControl.h"

#include <time.h>
#include "basictyp.h" 

#include "JobDlg.h" 

#include "schduler.h"
#include "schtypes.h" 

#include "SharedMem.h"	
extern "C"
{
   #include "ptetcp.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CShedulerControl property page

IMPLEMENT_DYNCREATE(CShedulerControl, CPropertyPage)

CShedulerControl::CShedulerControl() : CPropertyPage(CShedulerControl::IDD)
{
	//{{AFX_DATA_INIT(CShedulerControl)
	m_SchControlTitle = _T("");
	m_Message = _T("");
	//}}AFX_DATA_INIT
}

CShedulerControl::~CShedulerControl()
{
}

void CShedulerControl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShedulerControl)
	DDX_Control(pDX, IDC_SCHCTRL_STATUS, m_SchControlStatus);
	DDX_Control(pDX, IDC_JOB_LIST, m_JobList);
	DDX_Text(pDX, IDC_SCHCONTROL_TITLE, m_SchControlTitle);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShedulerControl, CPropertyPage)
	//{{AFX_MSG_MAP(CShedulerControl)
	ON_BN_CLICKED(IDC_ADDJOB, OnAddjob)
	ON_BN_CLICKED(IDC_CANCELJOB, OnCanceljob)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShedulerControl message handlers

void CShedulerControl::OnAddjob() 
{
	// TODO: Add your control notification handler code here
   CJobDlg JobDlg;
   INT ret;
   BYTE rcode;
   APF01 apf01;
   CHAR  error_msg[100];
   struct tm * cur_time;
   time_t      clock ;
   LOCAL_JOB_TABLE job_record;
   pPTE_MSG p_msg_out, p_msg_in;

   if( m_num_jobs == SCH_MAX_LIST_SIZE )
   {
	  m_SchControlStatus.SetWindowText( "ERROR: Cannot add job.  Maximum number of jobs reached." );
      return;
   }
      
   m_SchControlStatus.SetWindowText( " ");

   memset( &job_record, 0, sizeof( job_record ) );
   memset( &JobDlg.m_jbf01, 0, sizeof( JobDlg.m_jbf01 ) );
   time( &clock );
   cur_time = localtime( &clock );

   sprintf( (pCHAR)JobDlg.m_jbf01.primary_key.job_id, "**temp_%d%d%d**", 
            cur_time->tm_hour,
            cur_time->tm_min,
            cur_time->tm_sec );

	JobDlg.m_JobIdStr = (pCHAR)JobDlg.m_jbf01.primary_key.job_id;
   JobDlg.m_mode = SCHMODE_ADD_TEMP;
   ret = JobDlg.DoModal();	
   if( ret == IDOK )
   {
      memset( &apf01, 0, sizeof( apf01 ) );
      lstrcpy( (pCHAR)apf01.primary_key.app_name, (pCHAR)JobDlg.m_jbf01.app_name );
      rcode = txdsapi_get_record( (pBYTE)&apf01,sizeof (APF01), APF01_DATA, error_msg );
      if( rcode != PTEMSG_OK )
      {
		  m_SchControlStatus.SetWindowText( "ERROR: Application does not exist" );
         return;
      }

      memcpy( &job_record.jbf01, &JobDlg.m_jbf01, sizeof( JBF01 ) );
      sprintf( (pCHAR)job_record.command_line, "%s%s", (pCHAR)apf01.path, (pCHAR)apf01.exe_name );
      strcpy( (pCHAR)job_record.parameters, (pCHAR)apf01.parameters );
      job_record.is_spawned = false;
      job_record.is_db_updated = false;

	   p_msg_out = ptemsg_build_msg( MT_SCHEDULER_CONTROL,
								     ST1_SCH_ADDJOB,
								     0,
								     "schdulerA",
                             NULL_PTR,
								     (pBYTE)&job_record,
								     sizeof( job_record ),
								     0 );

	   if( p_msg_out == NULL_PTR )
	   {
		   m_SchControlStatus.SetWindowText( "ERROR: Unable to build request message for Scheduler service");
	      return;
	   }

      BeginWaitCursor();
      p_msg_in = ptetcp_send_receive( p_msg_out, 3, error_msg );
      EndWaitCursor();

      if( p_msg_in == NULL_PTR )
	   {
		   m_SchControlStatus.SetWindowText( error_msg );
	      return;
	   }

      if( ( rcode = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
	   {
         m_SchControlStatus.SetWindowText( "ERROR: Unable to add job" );
	      return;
      }

      OnRefresh();
   }
	
}


void CShedulerControl::OnCanceljob() 
{
	// TODO: Add your control notification handler code here
   INT ret, loop, selected;
   CHAR error_msg[256];
   pPTE_MSG p_msg_out, p_msg_in;
   BYTE rcode;

   m_SchControlStatus.SetWindowText( " ");
   selected = -1;
   for( loop = 0; loop < m_num_jobs; loop++ )
   {

      ret = m_JobList.GetItemState( loop, LVIS_SELECTED );
      if( ret == LVIS_SELECTED )
      {
         selected = loop;
         break;
      }
   }

   if( selected == -1 )
   {
      m_Message = "Please select a job to cancel";
	  UpdateData(FALSE);
      return;
   }
	
   sprintf( error_msg, "Do you wish to cancel %s?", m_job_table[selected].jbf01.primary_key.job_id );
   ret = AfxMessageBox( error_msg, MB_YESNO | MB_ICONQUESTION );
   if( ret == IDNO )
        return;

   if( strcmp( (pCHAR)m_job_table[selected].jbf01.status, "A" ) != 0 )
   {
      AfxMessageBox( "This job is not active", MB_ICONSTOP | MB_OK );
      return;
   }

	p_msg_out = ptemsg_build_msg( MT_SCHEDULER_CONTROL,
								  ST1_SCH_CANCELJOB,
								  0,
								  "schdulerA",
                          NULL_PTR,
								  (pBYTE)m_job_table[selected].jbf01.primary_key.job_id,
								  sizeof( m_job_table[selected].jbf01.primary_key.job_id ),
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		m_SchControlStatus.SetWindowText( "ERROR: Unable to build request message for Scheduler service");
	   return;
	}

   BeginWaitCursor();
   p_msg_in = ptetcp_send_receive( p_msg_out, 3, error_msg );
   EndWaitCursor();
   if( p_msg_in == NULL_PTR )
	{
		m_SchControlStatus.SetWindowText( error_msg );
	   return;
	}

   if( ( rcode = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
	{
      m_SchControlStatus.SetWindowText( "ERROR: Unable to cancel job" );
	   return;
   }

   OnRefresh();
}




void CShedulerControl::OnRefresh() 
{
	// TODO: Add your control notification handler code here
   // get status from scheduler service
   pPTE_MSG p_msg_out, p_msg_in;
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[100];
   BYTE        rcode;
   CHAR        error_msg[256], buffer[300];
	LV_ITEM lvItem;
   int nRowItem = 0;
   int nActualItem = 0;

   m_SchControlStatus.SetWindowText( " ");
   m_JobList.DeleteAllItems();
   memset( &m_job_table, 0, sizeof( m_job_table ) );
	p_msg_out = ptemsg_build_msg( MT_SCHEDULER_CONTROL,
								  ST1_SCH_STATUS,
								  0,
								  "schdulerA",
								  NULL_PTR,
								  NULL_PTR,
								  0,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		m_SchControlStatus.SetWindowText( "ERROR: Unable to build request message for Scheduler service");
	   return;
	}

   BeginWaitCursor();
   p_msg_in = ptetcp_send_receive( p_msg_out, 3, error_msg );
   EndWaitCursor();
   if( p_msg_in == NULL_PTR )
	{
	   sprintf( buffer, "ERROR: Unable to get job table from Scheduler service: %s", error_msg );
		m_SchControlStatus.SetWindowText( buffer );
	   return;
	}

   if( ( rcode = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
	{
	   m_SchControlStatus.SetWindowText( "ERROR: Unable to get job table from Scheduler service" );
	   return;
   }

   /* first retrieve the job table from the message */
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset( m_job_table, 0, sizeof( m_job_table ) );
   memcpy( m_job_table, p_data, data_len );

   /* next retrieve the value for num jobs */
   p_msg_data    = ptemsg_get_chained_pte_msg_data( p_msg_in, 2 );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset( temp_str, 0, sizeof( temp_str ) );
   strncpy( temp_str, (pCHAR)p_data, data_len );
   m_num_jobs = atoi( temp_str );

   for (int nItem = 0; nItem <  m_num_jobs; nItem++,nRowItem ++)
   {
      m_JobList.EnableWindow(TRUE);  
	   for (int nSubItem = 0; nSubItem < 6; nSubItem++)
      {		
	      lvItem.mask = LVIF_TEXT ;
         lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
			lvItem.iSubItem = nSubItem;
				
			if (nSubItem == 0)
         {
						
				lvItem.pszText = (pCHAR)m_job_table[nItem].jbf01.primary_key.job_id;
			}
			else if (nSubItem == 1)
         {
            CHAR temp_str[30];

            if( m_job_table[nItem].is_spawned == true )
               lstrcpy( temp_str, "Completed" );
            else if( strcmp( (pCHAR)m_job_table[nItem].jbf01.status, "A" ) == 0 )
               lstrcpy( temp_str, "Active" );
            else if( strcmp( (pCHAR)m_job_table[nItem].jbf01.status, "I" ) == 0 )
               lstrcpy( temp_str, "Inactive" );
            else if( strcmp( (pCHAR)m_job_table[nItem].jbf01.status, "C" ) == 0 )
               lstrcpy( temp_str, "Cancelled" );
						
				lvItem.pszText = temp_str ;
			}
			else if (nSubItem == 2)
         {
											
				lvItem.pszText = (pCHAR)m_job_table[nItem].jbf01.time_to_run;
			}
			else if (nSubItem == 3)
         { 
				lvItem.pszText = (pCHAR)m_job_table[nItem].jbf01.last_run;
			} 
         else if (nSubItem == 4)
         { 
				lvItem.pszText = (pCHAR)m_job_table[nItem].command_line;						
			} 
         else if (nSubItem == 5)
         { 
				lvItem.pszText = (pCHAR)m_job_table[nItem].parameters;
			} // end else if
					
			if (nSubItem == 0) 
				nActualItem = m_JobList.InsertItem(&lvItem);
			else 
				m_JobList.SetItem(&lvItem);
					
		/*	if (nSubItem == 1) // transaction id
				m_CntlList.SetColumnWidth (nSubItem, 0) ;
					
			else 
				m_CntlList.SetColumnWidth (nSubItem, LVSCW_AUTOSIZE_USEHEADER) ;				
                 */
			m_JobList.SetFocus() ;
					
		} // for
	} // for 
	return;
}




BOOL CShedulerControl::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();
	
 CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

   m_Message = "";

	LV_COLUMN lvColumn ;
    char strHeaderData[6][50] = {"JOB ID           ", "STATUS    ","SCHEDULED TIME", "LAST RUN                 ", "COMMAND       ", "PARAMETERS       "} ;

    for (int nHeader = 0; nHeader < 6; nHeader++) { 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_JobList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
        m_JobList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
        m_JobList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
        m_JobList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
        m_JobList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
        m_JobList.SetColumnWidth (4, LVSCW_AUTOSIZE_USEHEADER) ;
        m_JobList.SetColumnWidth (5, LVSCW_AUTOSIZE_USEHEADER) ;
        //m_JobList.SetColumnWidth (6, LVSCW_AUTOSIZE_USEHEADER) ;
    }

   struct tm * cur_time;
   time_t      clock ;
   pCHAR       temp_str;
   CHAR        time_str[50], title_str[100];

   time( &clock );
   cur_time = localtime( &clock );
   temp_str = asctime( cur_time );
   strcpy( title_str, "Jobs scheduled for " );

   strncpy( time_str, temp_str, 10 );
   time_str[10] = ' ';
   time_str[11] = '\0';

   strcat( time_str, &temp_str[20] );
   strcat( title_str, time_str );
   m_SchControlTitle = title_str;
   UpdateData( FALSE );

	

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




BOOL CShedulerControl::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
   OnRefresh();
	return CPropertyPage::OnSetActive();
}

BOOL CShedulerControl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	 
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
