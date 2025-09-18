

#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "SystemGlobalParameterDefinition.h"
#include "SystemGlobalParameterSheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
#include "PteTime.h"
}

GPCF01 soldGPCF01RecordRecord,snewGPCF01RecordRecord;
extern CSharedMem shrmem;
extern UCF01 ucf01;
UCF01 user_rcrd;
int fst_chk;
extern char login_name[51];
IMPLEMENT_DYNAMIC(CSystemGlobalParameter, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CSystemGlobalParameter::CSystemGlobalParameter()
	: CPropertyPage(CSystemGlobalParameter::IDD)	
{
	//memset(&m_ATP_Monitor_Service, 0, sizeof(ATP_MONITOR_SERVICE));
//	memset(&m_LastSubsidyQuota, 0, sizeof(SUBSIDY_QUOTA));
	
	
	//m_strFieldTag = "ATP_MONITOR_SERVICE";  
	
}

CSystemGlobalParameter::~CSystemGlobalParameter()
{
}
void CSystemGlobalParameter::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CSystemGlobalParameter)
	DDX_Control(pDX, IDC_GLOBAL_PARAM, m_GlblParam);
	DDX_Text(pDX, IDC_GLOBAL_PARAM, m_Strglblparam);
	//ramya
	DDX_Control(pDX, IDC_GLOBAL_PARAM2, m_GlblParam2);
	DDX_Text(pDX, IDC_GLOBAL_PARAM2, m_Strglblparam2);

	DDX_Control(pDX, IDC_GLOBAL_PARAM3, m_GlblParam3);
	DDX_Text(pDX, IDC_GLOBAL_PARAM3, m_Strglblparam3);

	DDX_Control(pDX, IDC_GLOBAL_PARAM4, m_GlblParam4);
	DDX_Text(pDX, IDC_GLOBAL_PARAM4, m_Strglblparam4);

	DDX_Control(pDX, IDC_DAYSB4EXPIRY, m_GlblDaysB4Expiry);
	DDX_Text(pDX, IDC_DAYSB4EXPIRY, m_StrglblDaysB4Expiry);

	DDX_Control(pDX, IDC_GLOBAL_VIEW_BUTTON, m_ViewButton);
	DDX_Text(pDX, IDC_MESSAGE_GLBL_LBL, m_StrMessageLbl);

	DDX_Control(pDX, IDC_USER_STATUS_TIMEOUT_VALUE,m_UserStatsTimeoutVal );
	DDX_Text(pDX, IDC_USER_STATUS_TIMEOUT_VALUE,m_StrglblUserStatsTimeoutVal );
}


BEGIN_MESSAGE_MAP(CSystemGlobalParameter, CPropertyPage) // very imprtnt.
	ON_BN_CLICKED(IDC_GLOBAL_VIEW_BUTTON, OnViewButton)
END_MESSAGE_MAP()

BOOL CSystemGlobalParameter::OnSetActive() 
{
	((CSystemGlobalParameterSheet*) this->GetParent())->m_nActiveTab = 1;

	return TRUE;
}

void CSystemGlobalParameter::OnOK() 
{
	if ( m_nDialogMode == MODE_DONE)	
		CPropertyPage::OnOK();
	else 
	{
		m_nDialogMode = MODE_DONE;
		return;
	}
}

void CSystemGlobalParameter::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class

   //clear out all the user fields on the main tab
	m_Strglblparam = _T("");

	m_Strglblparam2 = _T("");

	m_Strglblparam3 = _T("");

	m_Strglblparam4 = _T("");

	m_StrglblDaysB4Expiry = _T("");

	m_StrglblUserStatsTimeoutVal = _T("");
   m_nDialogMode   = MODE_DONE;

	CPropertyPage::OnCancel();
}
/*
BOOL CSystemGlobalParameter::OnApply() 
{
	


	if (m_nDialogMode == MODE_EDIT)
	{
		
	}

	return CPropertyPage::OnApply();
}*/

BOOL CSystemGlobalParameter::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CSystemGlobalParameter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	BYTE rcode;
	char strErrorMessage[200]={0};
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_GLBL_LBL)->SetFont (&m_font);
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	m_Strglblparam = "";
	m_Strglblparam2 = "";
	CString temp;
	int nIndex = -1;

	//to take specified number of characters
	  m_GlblParam.SetLimitText(4);
	  m_GlblParam2.SetLimitText(2);
	  m_GlblParam3.SetLimitText(1);
	  m_GlblParam4.SetLimitText(2);
	  m_GlblDaysB4Expiry.SetLimitText(4);
	  m_UserStatsTimeoutVal.SetLimitText(4);

    m_nDialogMode = MODE_DONE;
	
	memset(&soldGPCF01RecordRecord,0x00,sizeof(soldGPCF01RecordRecord));
	memset(&snewGPCF01RecordRecord,0x00,sizeof(soldGPCF01RecordRecord));
	memset(&user_rcrd,0x00,sizeof(user_rcrd));
	strncpy((char *)user_rcrd.primary_key.name,login_name,strlen(login_name));
	rcode = txdsapi_get_record  ((pBYTE)&user_rcrd, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData( FALSE );
		return FALSE;
	}
	if(user_rcrd.control[16].action[2]=='1')
	{
		rcode = txdsapi_get_record  ((pBYTE)&soldGPCF01RecordRecord, sizeof (GPCF01), GPCF01_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_Strglblparam =soldGPCF01RecordRecord.days_inactive;
				//ramya
				m_Strglblparam2 = soldGPCF01RecordRecord.max_prev_passwords_not_allowed;
				m_Strglblparam3 = soldGPCF01RecordRecord.max_password_retries;
				m_Strglblparam4 = soldGPCF01RecordRecord.min_password_length;
				m_StrglblDaysB4Expiry = soldGPCF01RecordRecord.days_before_expiration;
				m_StrglblUserStatsTimeoutVal = soldGPCF01RecordRecord.user_status_cntrl_tmeout_val;


				prev_days_inactive = soldGPCF01RecordRecord.days_inactive;
				prev_max_prev_passwords_not_allowed = soldGPCF01RecordRecord.max_prev_passwords_not_allowed;
				prev_max_password_retries = soldGPCF01RecordRecord.max_password_retries;
				prev_min_password_length= soldGPCF01RecordRecord.min_password_length;
				prev_days_before_expiry = soldGPCF01RecordRecord.days_before_expiration;
				prev_UserStatsTimeoutVal = soldGPCF01RecordRecord.user_status_cntrl_tmeout_val;
				m_StrMessageLbl="";
				UpdateData( FALSE );
				
				return TRUE;
		} 
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl = "Days Inactive is empty Please Add";
			fst_chk = 1 ;
			UpdateData( FALSE );
			return TRUE;
		}
		else 
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData( FALSE );
			return FALSE;	
		}
	}
	else
	{
		rcode = txdsapi_get_record  ((pBYTE)&soldGPCF01RecordRecord, sizeof (GPCF01), GPCF01_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_Strglblparam =soldGPCF01RecordRecord.days_inactive;
				//ramya
				m_Strglblparam2 = soldGPCF01RecordRecord.max_prev_passwords_not_allowed;

				m_Strglblparam3 = soldGPCF01RecordRecord.max_password_retries;

				m_Strglblparam4 = soldGPCF01RecordRecord.min_password_length;
				m_StrglblDaysB4Expiry = soldGPCF01RecordRecord.days_before_expiration;
				m_StrglblUserStatsTimeoutVal = soldGPCF01RecordRecord.user_status_cntrl_tmeout_val;

				prev_days_inactive = soldGPCF01RecordRecord.days_inactive;
				prev_max_prev_passwords_not_allowed = soldGPCF01RecordRecord.max_prev_passwords_not_allowed;
				prev_max_password_retries = soldGPCF01RecordRecord.max_password_retries;
				prev_min_password_length= soldGPCF01RecordRecord.min_password_length;
				prev_days_before_expiry = soldGPCF01RecordRecord.days_before_expiration;
				prev_UserStatsTimeoutVal = soldGPCF01RecordRecord.user_status_cntrl_tmeout_val;

				m_StrMessageLbl="";
				m_GlblParam.EnableWindow(FALSE);
				//ramya
				m_GlblParam2.EnableWindow(FALSE);
				m_GlblParam3.EnableWindow(FALSE);
				m_GlblParam4.EnableWindow(FALSE);
				m_GlblDaysB4Expiry.EnableWindow(FALSE);
				m_UserStatsTimeoutVal.EnableWindow(FALSE);
				UpdateData( FALSE );

				
				return TRUE;
		}
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			//ramya
			m_StrMessageLbl = "Days Inactive is empty";
			m_GlblParam.EnableWindow(FALSE);
			
			UpdateData( FALSE );
			return TRUE;
		}
		else
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData( FALSE );
			return FALSE;	
		}
	}
	  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//void CSystemGlobalParameter::OnUpdateButton() 
BOOL CSystemGlobalParameter::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};

	m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;
		
	if (!CheckDialogForCompleteness())
        return FALSE;

	
	if(fst_chk==1)
	{
		fst_chk = 0 ;
		strcpy((char *)snewGPCF01RecordRecord.days_inactive ,m_Strglblparam);
		strcpy((char *)snewGPCF01RecordRecord.max_prev_passwords_not_allowed,m_Strglblparam2);
		strcpy((char *)snewGPCF01RecordRecord.max_password_retries,m_Strglblparam3);
		strcpy((char *)snewGPCF01RecordRecord.min_password_length,m_Strglblparam4);
		strcpy((char *)snewGPCF01RecordRecord.days_before_expiration , m_StrglblDaysB4Expiry);
		strcpy((char *)snewGPCF01RecordRecord.user_status_cntrl_tmeout_val , m_StrglblUserStatsTimeoutVal);

		rcode = txdsapi_insert_record  ((pBYTE)&snewGPCF01RecordRecord, sizeof (GPCF01), GPCF01_DATA,strErrorMessage);
		if ( rcode != PTEMSG_OK )
		{
				m_StrMessageLbl = strErrorMessage;
				UpdateData( FALSE );
				return FALSE;
		}   
		else
		{
				m_StrMessageLbl = "Record added";
				UpdateData( FALSE );
				PopulateHistoryTable();
				
				return CPropertyPage::OnApply();
		}
	}
	if(user_rcrd.control[16].action[2]=='1')
	{
		
		strcpy((char *)snewGPCF01RecordRecord.days_inactive ,m_Strglblparam);
		strcpy((char *)snewGPCF01RecordRecord.max_prev_passwords_not_allowed ,m_Strglblparam2);
		strcpy((char *)snewGPCF01RecordRecord.max_password_retries,m_Strglblparam3);
		strcpy((char *)snewGPCF01RecordRecord.min_password_length,m_Strglblparam4);
		strcpy((char *)snewGPCF01RecordRecord.days_before_expiration , m_StrglblDaysB4Expiry);
		strcpy((char *)snewGPCF01RecordRecord.user_status_cntrl_tmeout_val , m_StrglblUserStatsTimeoutVal);

		rcode = txdsapi_update_record  ((pBYTE)&snewGPCF01RecordRecord, sizeof (GPCF01), GPCF01_DATA,strErrorMessage);

		new_days_inactive = snewGPCF01RecordRecord.days_inactive;
		new_max_prev_passwords_not_allowed = snewGPCF01RecordRecord.max_prev_passwords_not_allowed;
		new_max_password_retries = snewGPCF01RecordRecord.max_password_retries;
		new_min_password_length = snewGPCF01RecordRecord.min_password_length;
		new_days_before_expiry = snewGPCF01RecordRecord.days_before_expiration;
		new_UserStatsTimeoutVal = snewGPCF01RecordRecord.user_status_cntrl_tmeout_val;
		PopulateControlStructure();
		PopulateHistoryTable();
		PopulateHistoryTable();
		PopulateHistoryTable();
		PopulateHistoryTable();
		PopulateHistoryTable();
		PopulateHistoryTable();

		if ( rcode != PTEMSG_OK )
		{
				m_StrMessageLbl = strErrorMessage;
				UpdateData( FALSE );
				return FALSE;
		}    
		
		m_StrMessageLbl = "Record Updated";
		UpdateData( FALSE );
	}
	
	return CPropertyPage::OnApply();
	
	
}

void CSystemGlobalParameter::OnViewButton() 
{
	HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "GPCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

	
}

BOOL CSystemGlobalParameter::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	m_StrMessageLbl = "";
	if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    if (myUtilsObject.IsEmpty (m_Strglblparam, this))
    {
		m_StrMessageLbl = "Please Enter the Days inactive";	
		m_GlblParam.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	else if (myUtilsObject.IsEmpty (m_Strglblparam2, this))
    {
		m_StrMessageLbl = "Please Enter the max Prev Passwords not allowed";	
		m_GlblParam2.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	else if (myUtilsObject.IsEmpty (m_Strglblparam3, this))
    {
		m_StrMessageLbl = "Please Enter the max Password Retries";	
		m_GlblParam3.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	else if (myUtilsObject.IsEmpty (m_Strglblparam4, this))
    {
		m_StrMessageLbl = "Please enter the minimum password Length";	
		m_GlblParam4.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	//else if (myUtilsObject.IsEmpty (m_StrglblDaysB4Expiry, this))
 //   {
	//	m_StrMessageLbl = "Please enter the minimum password Length";	
	//	m_GlblDaysB4Expiry.SetFocus();
	//	m_nDialogMode = MODE_DONE;
	//	if (!UpdateData(FALSE))
	//	{
	//		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	//	}	
	//	return FALSE;
	//}

	else if (myUtilsObject.IsEmpty (m_StrglblUserStatsTimeoutVal, this))
    {
		m_StrMessageLbl = "Please enter the minimum password Length";	
		m_UserStatsTimeoutVal.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	int len = m_Strglblparam.GetLength();
	char *str = m_Strglblparam.GetBuffer(len);
	for(int i=0;i<len;i++)
	{
		if( (str[i] > 47 && str[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed in Days Inactive";	
			m_GlblParam.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_Strglblparam.ReleaseBuffer(len);

			return FALSE;
		}
	}
	/**********************/
	
	int len1 = m_Strglblparam2.GetLength();
	char *str1 = m_Strglblparam2.GetBuffer(len);
	for(int i=0;i<len1;i++)
	{
		if( (str1[i] > 47 && str1[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed";	
			m_GlblParam2.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_Strglblparam2.ReleaseBuffer(len);

			return FALSE;
		}
	}
	/****************************/
	int len3 = m_Strglblparam3.GetLength();
	char *str3 = m_Strglblparam3.GetBuffer(len);
	for(int i=0;i<len3;i++)
	{
		if( (str3[i] > 47 && str3[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed";	
			m_GlblParam3.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_Strglblparam3.ReleaseBuffer(len);

			return FALSE;
		}
	}
	/*************************/

	int len4 = m_Strglblparam4.GetLength();
	char *str4 = m_Strglblparam4.GetBuffer(len);
	for(int i=0;i<len4;i++)
	{
		if( (str4[i] > 47 && str4[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed";	
			m_GlblParam4.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_Strglblparam4.ReleaseBuffer(len);

			return FALSE;
		}
	}
	/********************************/
	int len5 = m_StrglblDaysB4Expiry.GetLength();
	char *str5 = m_StrglblDaysB4Expiry.GetBuffer(len);
	for(int i=0;i<len5;i++)
	{
		if( (str5[i] > 47 && str5[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed";	
			m_GlblDaysB4Expiry.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_StrglblDaysB4Expiry.ReleaseBuffer(len);

			return FALSE;
		}
	}

	int len6 = m_StrglblUserStatsTimeoutVal.GetLength();
	char *str6 = m_StrglblUserStatsTimeoutVal.GetBuffer(len);
	for(int i=0;i<len6;i++)
	{
		if( (str6[i] > 47 && str6[i] < 58))
		{
				//The character is a NUMBER
		}
		else
		{
			m_StrMessageLbl = "No Special characters  and Alphabets are allowed";	
			m_UserStatsTimeoutVal.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_StrglblUserStatsTimeoutVal.ReleaseBuffer(len);

			return FALSE;
		}
	}
	/*****************************************/
	return TRUE;
	/*************************/
}

void CSystemGlobalParameter::PopulateHistoryTable()
{
	HST01 sHistoryData;
	CMyUtils myUtilsObject;
	CHAR dest_str[18];
	char strErrorMsg[200] = "";
	CString temp;
	CHAR wstation[21]; DWORD wsize;
	UCF01 ucf01;
    char strDate[9] = "";
    char strTime[7] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));

	myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);
	pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"GPCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	//if((strcmp((char *)snewGPCF01RecordRecord.days_inactive, (char *)soldGPCF01RecordRecord.days_inactive)!=0) && change_days_inactive==true)
	if((new_days_inactive != prev_days_inactive) && change_days_inactive==true)
	{
		change_days_inactive = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.days_inactive , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.days_inactive, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"DAYS_INACTIVE", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	//else if((strcmp((char *)snewGPCF01RecordRecord.max_prev_passwords_not_allowed, (char *)soldGPCF01RecordRecord.max_prev_passwords_not_allowed)!=0) && change_max_prev_passwords_not_allowed==true)
	else if((new_max_prev_passwords_not_allowed!=prev_max_prev_passwords_not_allowed) && change_max_prev_passwords_not_allowed==true)
	{
		change_max_prev_passwords_not_allowed = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.max_prev_passwords_not_allowed , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.max_prev_passwords_not_allowed, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"MAX PASSWORDS ALLOWED", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	//else if((strcmp((char *)snewGPCF01RecordRecord.max_password_retries, (char *)soldGPCF01RecordRecord.max_password_retries)!=0) && change_max_password_retries == true)
	else if((new_max_password_retries!=prev_max_password_retries) && change_max_password_retries == true)
	{
		change_max_password_retries = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.max_password_retries , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.max_password_retries, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"MAX PASSWORD RETRIES", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	//else if((strcmp((char *)snewGPCF01RecordRecord.min_password_length, (char *)soldGPCF01RecordRecord.min_password_length)!=0) && change_min_password_length==true)
	else if((new_min_password_length!=prev_min_password_length) && change_min_password_length==true)
	{
		change_min_password_length = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.min_password_length , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.min_password_length, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"MIN PASSWORD LENGTH", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_days_before_expiry!=prev_days_before_expiry) && change_days_before_expiry==true)
	{
		change_days_before_expiry = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.days_before_expiration , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.days_before_expiration, sizeof (sHistoryData.previous_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"DAYS BEFORE EXPIRATION", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_UserStatsTimeoutVal!=prev_UserStatsTimeoutVal) && change_UserStatsTimeoutVal==true)
	{
		change_UserStatsTimeoutVal = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewGPCF01RecordRecord.user_status_cntrl_tmeout_val , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldGPCF01RecordRecord.user_status_cntrl_tmeout_val, sizeof (sHistoryData.previous_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"DAYS BEFORE EXPIRATION", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}


}

void CSystemGlobalParameter::PopulateControlStructure()
{
	//if(strcmp((char *)snewGPCF01RecordRecord.days_inactive, (char *)soldGPCF01RecordRecord.days_inactive)!=0)
	if(new_days_inactive != prev_days_inactive)
	{
		change_days_inactive = true;
	}
	
	//if(strcmp((char *)snewGPCF01RecordRecord.max_prev_passwords_not_allowed, (char *)soldGPCF01RecordRecord.max_prev_passwords_not_allowed)!=0)
	if(new_max_prev_passwords_not_allowed!=prev_max_prev_passwords_not_allowed)
	{
		change_max_prev_passwords_not_allowed = true;

	}

	if(new_max_password_retries!=prev_max_password_retries)
	{
		change_max_password_retries = true;
	}

	if(new_min_password_length!=prev_min_password_length)
	{
		change_min_password_length = true;
	}

	if(new_days_before_expiry!=prev_days_before_expiry)
	{
		change_days_before_expiry = true;
	}
	if(new_UserStatsTimeoutVal!=prev_UserStatsTimeoutVal)
	{
		change_UserStatsTimeoutVal = true;
	}
}