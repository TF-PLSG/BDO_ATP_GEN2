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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserDefinition.cpp  $
   
      Rev 1.13   Jan 01 2011 11:30:22  
    Added ucf03 table for unlocking user account from GUI-TF-Srikanth
*
************************************************************************************/
// UserAccountStatus.cpp : implementation file
//

#include "stdafx.h"
#include "myutils.h"
#include "atp.h"
#include "UserAccountStatus.h"
#include "UserDefinition.h"
#include "UserPropertySheet.h"
#include "UserDefinition.h"
#include "SharedMem.h"	

extern "C"
{
#include "PteTime.h"
#include "txutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//UCF01 sUCFStructure;
extern UCF01 ucf01;
extern CSharedMem shrmem;
extern CUserAccountStatus* pAccountStatusPage; 
CUserDefinition* pUserDefinitionPage;
extern char login_name[51];
extern GPCF01 gpcf01;
#define NEW_USER "New User Account"
/////////////////////////////////////////////////////////////////////////////
// CUserAccountStatus property page
CString UCF01_CUserStatus;
IMPLEMENT_DYNCREATE(CUserAccountStatus, CPropertyPage)

CUserAccountStatus::CUserAccountStatus() : CPropertyPage(CUserAccountStatus::IDD)
{
	//{{AFX_DATA_INIT(CUserAccountStatus)
	m_StrMessageLbl = _T("");
	m_StrUserNameFld = _T("");
	m_StrLabel1 = _T("");
	m_StrUserStatus = _T("");
	//m_StrUserExpiryDate = 0;
	//}}AFX_DATA_INIT
}

CUserAccountStatus::~CUserAccountStatus()
{
}

void CUserAccountStatus::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserAccountStatus)
	DDX_Control(pDX, IDC_RADIO_UNLOCK, m_CntlAddUnlock);
	DDX_Control(pDX, IDC_RADIO_LOCK, m_CntlAddLock);
	DDX_Control(pDX, IDC_RADIO_DELETED, m_CntlAddDeleted);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDX_Text(pDX, IDC_LABEL_STATUS, m_StrLabel1);
	DDX_Control(pDX, IDC_USER_STATUS, m_CntlUserStatus); // transfer data to and from other types of controls like listbox, combobox, etc. 
	DDX_CBString(pDX, IDC_USER_STATUS, m_StrUserStatus); //The DDX_CBString function manages the transfer of CString data between the edit control of a combo box control in a dialog box,
	//form view, or control view object and a CString data member of
	//the dialog box, form view, or control view object.
	//ramya for ED
	//DDX_Control(pDX, IDC_EXPIRY_DATE_AS, m_CntlUserExpiryDate); // transfer data to and from other types of controls like listbox, combobox, etc. 
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_StrUserExpiryDate);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_COMBO2, m_Combo2);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_DateTimePicker);
}


BEGIN_MESSAGE_MAP(CUserAccountStatus, CPropertyPage)
	//{{AFX_MSG_MAP(CUserAccountStatus)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_USER_NAME_FLD, &CUserAccountStatus::OnEnChangeUserNameFld)
	ON_BN_CLICKED(IDC_RADIO_UNLOCK, &CUserAccountStatus::OnBnClickedRadioUnlock)
	ON_STN_CLICKED(IDC_MESSAGE_LBL, &CUserAccountStatus::OnStnClickedMessageLbl)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_EXPIRY_DATE_AS, &CUserAccountStatus::OnDtnDatetimechangeExpiryDateAs)
	ON_CBN_SETFOCUS(IDC_COMBO2, &CUserAccountStatus::OnSetfocusCombo2)
//	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CUserAccountStatus::OnDatetimechangeDatetimepicker2)
ON_CBN_DROPDOWN(IDC_COMBO2, &CUserAccountStatus::OnDropdownCombo2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserAccountStatus message handlers

BOOL CUserAccountStatus::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();
	this->m_Combo2.ShowWindow(SW_SHOW);
	this->m_DateTimePicker.ShowWindow(SW_HIDE);

	
	CFont m_font;
	LOGFONT lf;
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	GetDlgItem(IDC_LABEL_STATUS)->SetFont (&m_font);//Set the message font

	m_StrMessageLbl = "";
	//m_bUAccStatus = TRUE;
	m_CntlAddLock.SetCheck(0);
	m_CntlAddUnlock.SetCheck(0);
	m_CntlAddDeleted.SetCheck(0);
	//m_CntlUserExpiryDate.SetFormat("");

	//ZeroMemory(&sUCFStructure,sizeof(UCF01)) ;	
	EnableUserAccountStatusDialog(FALSE);
	CleanUserAccountStatusDialog();
	PopulateDialogFromDatabase();
	//m_CntlUserExpiryDate.SetFormat(_T("dd/MM/yyyy"));
	//m_CntlUserExpiryDate.SetFormat(_T(""));
	
	//PopulateDialogFromDatabase(ucf01);
	UpdateData(FALSE);
	m_bStatus=TRUE;    	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	return TRUE;  
}

void CUserAccountStatus::PopulateHistoryTable()
{ 
	HST01 sHistoryData;
    CHAR dest_str[18];   
    CMyUtils myUtilsObject;
    char strErrorMsg[200] = "";
    CString temp;
    CHAR wstation[21]; 
	DWORD wsize;
	static int i=0;

    char strDate[9] = "";
    char strTime[7] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
	strcpy(strDate, temp);
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	if(change_f == true && prev_string!= m_bAccStatus)
	{
		change_f = false;

		memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
		memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
		memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
		memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
		memcpy ( sHistoryData.file_primary_key,  m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
		memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
		memcpy ( sHistoryData.field_tag, "Account Status", sizeof (sHistoryData.field_tag));
		memcpy ( sHistoryData.current_data, m_bAccStatus, sizeof (sHistoryData.current_data));
		memcpy ( sHistoryData.previous_data, prev_string, sizeof (sHistoryData.previous_data));

		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}
	
	
	}
	
		
	//ramya
	else if((new_UserStatus!=prev_UserStatus) && (new_UserStatus != "")  && change_UserStatus == true)
	{
		change_UserStatus = false;
		ZeroMemory ( &sHistoryData, sizeof (HST01));
		memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
		memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
		memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
		memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
		memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
		memcpy ( sHistoryData.file_primary_key,  m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
		memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
		memcpy ( sHistoryData.field_tag, "User Status", sizeof (sHistoryData.field_tag));
		memcpy ( sHistoryData.current_data, new_UserStatus, sizeof (sHistoryData.current_data));
		memcpy ( sHistoryData.previous_data, prev_UserStatus, sizeof (sHistoryData.previous_data));
			
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}
	}

//	else if(prev_ExpDate!=new_ExpDate && new_ExpDate != "" && change_ExpiryDate == true )
	else if(prev_ExpDate!=new_ExpDate && change_ExpiryDate == true )
	{
		change_ExpiryDate = false;
		/*ZeroMemory ( &sHistoryData, sizeof (HST01));
		memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));*/
		memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
		memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
		memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
		memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
		memcpy ( sHistoryData.file_primary_key,  m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
		memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
		memcpy ( sHistoryData.field_tag, "Expiry Date", sizeof (sHistoryData.field_tag));
		memcpy ( sHistoryData.current_data, new_ExpDate, sizeof (sHistoryData.current_data));
		memcpy ( sHistoryData.previous_data, prev_ExpDate, sizeof (sHistoryData.previous_data));
	
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}
	
	}

}


//BOOL CUserAccountStatus::PopulateDialogFromDatabase(UCF01 userstatus)
BOOL CUserAccountStatus::PopulateDialogFromDatabase()
{
//	m_StrLabel1=sUCFStructure.accountstatus;
//	m_StrLabel1=pStatus;
	//m_StrUserStatus =userstatus.User_Status;
	UCF01 sUserstatusRecord;
	int jday4;
	char ExpDate[11];
	char strErrorMessage[200] = {0};
	m_StrLabel1=m_bAccStatus;
	m_CntlAddLock.EnableWindow(TRUE);
	m_CntlAddUnlock.EnableWindow(TRUE);	
	m_CntlAddDeleted.EnableWindow(TRUE);	
	

	if(m_bAccStatus == USER_LOCKED)
		m_CntlAddLock.SetCheck(1);
	else if(m_bAccStatus == USER_UNLOCKED || m_bAccStatus == USER_NEW || m_bAccStatus == NEW_USER )
		m_CntlAddUnlock.SetCheck(1);
	else 
		m_CntlAddDeleted.SetCheck(1);

	prev_string = m_bAccStatus;
	//changed due to mismatch
	if(strlen(m_StrUserNameFld) > 0)
	{

		//if( (0 != strncmp((char*)userstatus.primary_key.name,m_StrUserNameFld, strlen(m_StrUserNameFld)))||(m_StrUserNameFld = "ADMINISTRATOR"))
		{
			memcpy ((LPSTR)(sUserstatusRecord.primary_key.name), m_StrUserNameFld, sizeof (sUserstatusRecord.primary_key.name) );
			int rcode = txdsapi_get_record  ((pBYTE)&sUserstatusRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);

			if (pAccountStatusPage->m_nDialogMode == MODE_ADD)
			{
				//m_CntlUserExpiryDate.EnableWindow(TRUE);
				m_CntlUserStatus.EnableWindow(TRUE);
				m_StrUserStatus = _T("ACTIVE");
				m_DateTimePicker.EnableWindow(TRUE);
				this->m_Combo2.ShowWindow(SW_SHOW);
				this->m_DateTimePicker.ShowWindow(SW_HIDE);

				//m_CntlUserExpiryDate.SetFormat(_T(""));
				
			}

			/*Only for vie and Update option*/
			if(pAccountStatusPage->m_nDialogMode == MODE_VIEW || pAccountStatusPage->m_nDialogMode == MODE_EDIT)
			{
				if ( rcode != PTEMSG_OK )
				{	
					switch( rcode )
					{
						case PTEMSG_NOT_FOUND:
						sprintf( strErrorMessage, "ERROR: User record status for %s does not exist", m_StrUserNameFld );
						break;
					}
					
				}
				else if (rcode == PTEMSG_OK)
				{
					//if (!(strncmp((char *)login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR"))))
					//if(!(strncmp((char *)sUserstatusRecord.control[0].action, "System Administration", sizeof("System Administration"))))
				if(!(strncmp((char *)ucf01.control[0].action,"1110",4))||!(strncmp((char *)ucf01.control[0].action,"1111",4))||
					(!(strncmp((char *)login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")))))
				//	if(strncmp(login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")) == 0 )
				{
					m_CntlUserStatus.EnableWindow(TRUE);
				}
					m_StrUserStatus = sUserstatusRecord.User_Status;
					memcpy(&(pAccountStatusPage->m_StrUserExpiryDate),&(sUserstatusRecord.User_Expiry_Date),sizeof(sUserstatusRecord.User_Expiry_Date));
					CString tep_exp_date =sUserstatusRecord.User_Expiry_Date;
					int year,mon,day;

			
					if(!tep_exp_date.IsEmpty())
					{  /* Normal  or new user who have expiry date*/
						m_Combo2.EnableWindow(TRUE);
						m_DateTimePicker.EnableWindow(TRUE);
						year = atoi(tep_exp_date.Mid(0,4));
						mon = atoi(tep_exp_date.Mid(4,2));
						day = atoi(tep_exp_date.Mid(6,2));
						CTime m_StrUserExpiryDate_temp(year,mon,day,0,0,0);
						m_StrUserExpiryDate = m_StrUserExpiryDate_temp;
						
						this->m_Combo2.ShowWindow(SW_HIDE);
						this->m_DateTimePicker.ShowWindow(SW_SHOW);
				    }
					else
					{   /* For existing users , who does not have expiry date and user status*/ 
							char toDayDate[10];
						pUserDefinitionPage->SetExpiryDate(toDayDate);
						CString tep_exp_date =toDayDate;

						int year1,mon1,day1;

						year1 = atoi(tep_exp_date.Mid(0,4));
						mon1 = atoi(tep_exp_date.Mid(4,2));
						day1 = atoi(tep_exp_date.Mid(6,2));
						CTime m_StrUserExpiryDate_temp(year1,mon1,day1,0,0,0);
						//pAccountStatusPage->m_StrUserExpiryDate = m_StrUserExpiryDate_temp;
						m_StrUserExpiryDate = 0;
						m_Combo2.EnableWindow(TRUE);
						m_DateTimePicker.EnableWindow(TRUE);
						this->m_Combo2.ShowWindow(SW_SHOW);
						this->m_DateTimePicker.ShowWindow(SW_HIDE);
					}
			    }

			}	
				
		}	
		
	}

	
	if (pAccountStatusPage->m_nDialogMode == MODE_VIEW)
	{
		m_Combo2.EnableWindow(FALSE);	
		m_DateTimePicker.EnableWindow(FALSE);
	}

	if ( m_StrUserStatus == STATUS_ACTIVE )
		m_StrUserStatus = "ACTIVE";
	else if ( m_StrUserStatus == STATUS_INACTIVE )
		m_StrUserStatus = "INACTIVE";
    else if ( m_StrUserStatus == STATUS_EXPIRED )
	  m_StrUserStatus = "EXPIRED";
	else if ( m_StrUserStatus == STATUS_DELETED )
		m_StrUserStatus = "DELETED";
	else
		m_StrUserStatus = "ACTIVE"; //If the user status is NULL, then make it active by default( applicable to old users)


	if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	SetModified ( FALSE );
	
	return TRUE;
}

void CUserAccountStatus::EnableUserAccountStatusDialog(int nStatus)
{
	m_CntlAddUnlock.EnableWindow(nStatus);
	m_CntlAddLock.EnableWindow(nStatus);
	m_CntlAddDeleted.EnableWindow(nStatus);
	m_CntlUserStatus.EnableWindow(nStatus);
	//m_CntlUserExpiryDate.EnableWindow(nStatus);
}
void CUserAccountStatus::CleanUserAccountStatusDialog()
{	m_StrUserNameFld= _T("");
    UpdateData();
	m_StrLabel1=  _T("");
	m_StrMessageLbl = _T("");
	m_StrUserStatus = _T("");
	m_CntlAddLock.SetCheck(0);
	m_CntlAddUnlock.SetCheck(0);
	m_CntlAddDeleted.SetCheck(0);
	//m_StrUserExpiryDate = 0;  //if you clean then you wont get the reqd value..
	UpdateData(FALSE);
}

BOOL CUserAccountStatus::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return TRUE;
}

BOOL CUserAccountStatus::OnSetActive() 
{
		//m_CntlUserStatus.ResetContent();
		//m_CntlUserExpiryDate.ResetContent();

		if(m_bPopulate)
		{	
			if ( m_nDialogMode != MODE_VIEW)
			OnInitDialog();
			else
			EnableUserAccountStatusDialog(FALSE);
			m_bPopulate=FALSE;
		}
		if(m_viewPopulate)
		{
			OnInitDialog();
			EnableUserAccountStatusDialog(FALSE);
			m_viewPopulate=FALSE;
		}
		
	/*	if(m_addPopulate)
		{
			m_StrLabel1=m_bAccStatus;
			EnableUserAccountStatusDialog(FALSE);
			m_viewPopulate=FALSE;
		}
		*/

	//	 return 1;
	return CPropertyPage::OnSetActive();
}

BOOL CUserAccountStatus::OnCommand(WPARAM wParam, LPARAM lParam) 
{

	if ( m_nDialogMode != MODE_VIEW)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	
	return CPropertyPage::OnCommand(wParam, lParam);
}


BOOL CUserAccountStatus::PopulateControlStructure ()
{


	if (m_bAccStatus)
	{
		if(m_CntlAddLock.GetCheck() == 1)
			m_bAccStatus =USER_LOCKED;
		else if(m_CntlAddUnlock.GetCheck() == 1)
				m_bAccStatus=USER_UNLOCKED;
		else if(m_CntlAddDeleted.GetCheck() == 1)
				m_bAccStatus=DELETED;
	
		change_f = true;
	}
		return TRUE;
}

BOOL CUserAccountStatus::PopulateControlStructure (char *UserStatus, char *ExpiryDate)
{
		 int year,month,day; char ExpDate[10];
	if((new_UserStatus!=prev_UserStatus) && (new_UserStatus != ""))
	{
	
	if ( pAccountStatusPage->m_StrUserStatus.CompareNoCase( "ACTIVE")==0)
	     memcpy (UserStatus , STATUS_ACTIVE, sizeof (UserStatus)); 

	else if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("INACTIVE")==0)
        memcpy (UserStatus , STATUS_INACTIVE, sizeof (UserStatus)); 

	else  if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("EXPIRED")==0)
        memcpy (UserStatus , STATUS_EXPIRED, sizeof (UserStatus)); 
	change_UserStatus = true;

	}	

		if(prev_ExpDate!=new_ExpDate && new_ExpDate != "")
		{
			year = pAccountStatusPage->m_StrUserExpiryDate.GetYear();
			month = pAccountStatusPage->m_StrUserExpiryDate.GetMonth();
			day = pAccountStatusPage->m_StrUserExpiryDate.GetDay();

			sprintf(ExpDate,"%d%02d%02d",year,month,day);

			memcpy(ExpiryDate,ExpDate,sizeof(ExpDate));
			change_ExpiryDate = true;
		}
	return TRUE;
		
		//return CPropertyPage::OnApply();
}

//BOOL CUserAccountStatus::PopulateUserStatus()
//{
//	UpdateData(FALSE);
//	UCF01_CUserStatus=m_StrUserStatus;
//	return TRUE;
//}


void CUserAccountStatus::OnEnChangeUserNameFld()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CUserAccountStatus::OnBnClickedRadioUnlock()
{
	// TODO: Add your control notification handler code here
}


void CUserAccountStatus::OnStnClickedMessageLbl()
{
	// TODO: Add your control notification handler code here
}


void CUserAccountStatus::OnDtnDatetimechangeExpiryDateAs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
}





void CUserAccountStatus::OnSetfocusCombo2()
{
	// TODO: Add your control notification handler code here
	this->m_Combo2.ShowWindow(SW_SHOW);
	this->m_DateTimePicker.ShowWindow(SW_SHOW);
}


//void CUserAccountStatus::OnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}


void CUserAccountStatus::OnDropdownCombo2()
{
	// TODO: Add your control notification handler code here
	this->m_Combo2.ShowWindow(SW_HIDE);
	this->m_DateTimePicker.ShowWindow(SW_SHOW);
	this->m_DateTimePicker.SendNotifyMessageA(DTN_DATETIMECHANGE,0,0);
}
