#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "Cup3DS2ParameterDefinition.h"
#include "Cup3DS2PropertySheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
#include "PteTime.h"
}

CUP3DS2 soldCup3ds2,snewCup3ds2;
extern CSharedMem shrmem;
extern UCF01 ucf01;
UCF01 user_information;
int fst_checkk;
extern char login_name[51];
IMPLEMENT_DYNAMIC(CCup3DS2Parameter, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CCup3DS2Parameter::CCup3DS2Parameter()
	: CPropertyPage(CCup3DS2Parameter::IDD)	
{
	//memset(&m_ATP_Monitor_Service, 0, sizeof(ATP_MONITOR_SERVICE));
//	memset(&m_LastSubsidyQuota, 0, sizeof(SUBSIDY_QUOTA));
	
	
	//m_strFieldTag = "ATP_MONITOR_SERVICE";  
	
}

CCup3DS2Parameter::~CCup3DS2Parameter()
{
}

void CCup3DS2Parameter::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CSystemGlobalParameter)
	DDX_Control(pDX, IDC_AV_VERIFICATION_RC, m_CntlAV_VERIFICATION_RC);
	DDX_Text(pDX, IDC_AV_VERIFICATION_RC, m_StrAV_VERIFICATION_RC);
	DDV_MaxChars(pDX,m_StrAV_VERIFICATION_RC,2);

	DDX_Control(pDX, IDC_AUTHENTICATION_ATTEMPT_RC, m_CntlAUTHENTICATION_ATTEMPT_RC);
	DDX_Text(pDX, IDC_AUTHENTICATION_ATTEMPT_RC, m_StrAUTHENTICATION_ATTEMPT_RC);
	DDV_MaxChars(pDX,m_StrAUTHENTICATION_ATTEMPT_RC,2);

	DDX_Control(pDX, IDC_CVN2_VALIDATION_FLAG, m_CntlCVN2_VALIDATION_FLAG);
	DDX_Control(pDX, IDC_AV_VALIDATION_FLAG, m_CntlAV_VALIDATION_FLAG);

	DDX_Control(pDX, IDC_CUP_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG);
	DDX_CBString(pDX, IDC_CUP_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);

	DDX_Control(pDX, IDC_CUP3DS2_VIEW_BUTTON, m_ViewButton);
	DDX_Text(pDX, IDC_MESSAGE_CUP3DS2_LBL, m_StrMessageLbl);


}


BEGIN_MESSAGE_MAP(CCup3DS2Parameter, CPropertyPage) // very imprtnt.
	ON_BN_CLICKED(IDC_CUP3DS2_VIEW_BUTTON, OnViewButton)
	//ON_CBN_SELCHANGE(IDC_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, &CAmex3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag)
END_MESSAGE_MAP()

BOOL CCup3DS2Parameter::OnSetActive() 
{
	((CCup3DS2PropertySheet*) this->GetParent())->m_nActiveTab = 1;

	return TRUE;
}

void CCup3DS2Parameter::OnOK() 
{
	if ( m_nDialogMode == MODE_DONE)	
		CPropertyPage::OnOK();
	else 
	{
		m_nDialogMode = MODE_DONE;
		return;
	}
}

void CCup3DS2Parameter::OnCancel() 
{

	m_StrAV_VERIFICATION_RC = _T("");	
	m_StrAUTHENTICATION_ATTEMPT_RC= _T("");
	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG= _T("");

   m_nDialogMode   = MODE_DONE;

	CPropertyPage::OnCancel();
}

BOOL CCup3DS2Parameter::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CCup3DS2Parameter::OnInitDialog() 
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
	GetDlgItem(IDC_MESSAGE_CUP3DS2_LBL)->SetFont (&m_font);
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	CString temp;
	CString cvn2_validation_flg;
	CString av_validation_flg;
	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
	int nIndex = -1;


    m_nDialogMode = MODE_DONE;

	memset(&soldCup3ds2,0x00,sizeof(soldCup3ds2));
	memset(&snewCup3ds2,0x00,sizeof(snewCup3ds2));
	memset(&user_information,0x00,sizeof(user_information));
	strncpy((char *)user_information.primary_key.name,login_name,strlen(login_name));
	rcode = txdsapi_get_record  ((pBYTE)&user_information, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData( FALSE );
		return FALSE;
	}
	if(user_information.control[4].action[2]=='1')
	{
		rcode = txdsapi_get_record  ((pBYTE)&soldCup3ds2, sizeof (CUP3DS2), CUP3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_StrAV_VERIFICATION_RC =soldCup3ds2.av_verification_rc;
				m_StrAUTHENTICATION_ATTEMPT_RC = soldCup3ds2.authentication_attempt_rc;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldCup3ds2.cup_authentication_attempt_verify_flag;
				cvn2_validation_flg=soldCup3ds2.cvn2_validation_flag;
				av_validation_flg=soldCup3ds2.av_validation_flag;
					if ( cvn2_validation_flg == "Y" )
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvn2_validation_flg == "N")
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(0);

					if ( av_validation_flg == "Y" )
						m_CntlAV_VALIDATION_FLAG.SetCheck(1);
					else if ( av_validation_flg == "N")
						m_CntlAV_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";

				prev_StrAV_VERIFICATION_RC =soldCup3ds2.av_verification_rc;
				prev_StrAUTHENTICATION_ATTEMPT_RC = soldCup3ds2.authentication_attempt_rc;
				prev_StrCVN2_VALIDATION_FLAG=soldCup3ds2.cvn2_validation_flag;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldCup3ds2.cup_authentication_attempt_verify_flag;
				prev_StrAV_VALIDATION_FLAG=soldCup3ds2.av_validation_flag;

					/*if ( cvn2_validation_flg == "Y" )
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvn2_validation_flg == "N")
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(0);

					if ( av_validation_flg == "Y" )
						m_CntlAV_VALIDATION_FLAG.SetCheck(1);
					else if ( av_validation_flg == "N")
						m_CntlAV_VALIDATION_FLAG.SetCheck(0);

					if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";*/

				m_StrMessageLbl="";
				UpdateData( FALSE );
				
				return TRUE;
		} 
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl = "Please add the records";
			fst_checkk = 1 ;
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
		rcode = txdsapi_get_record  ((pBYTE)&soldCup3ds2, sizeof (CUP3DS2), CUP3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_StrAV_VERIFICATION_RC =soldCup3ds2.av_verification_rc;
				m_StrAUTHENTICATION_ATTEMPT_RC = soldCup3ds2.authentication_attempt_rc;
				cvn2_validation_flg=soldCup3ds2.cvn2_validation_flag;
				av_validation_flg=soldCup3ds2.av_validation_flag;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG =soldCup3ds2.cup_authentication_attempt_verify_flag;

					if ( cvn2_validation_flg == "Y" )
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvn2_validation_flg == "N")
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(0);

					if ( av_validation_flg == "Y" )
						m_CntlAV_VALIDATION_FLAG.SetCheck(1);
					else if ( av_validation_flg == "N")
						m_CntlAV_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";


				prev_StrAV_VERIFICATION_RC =soldCup3ds2.av_verification_rc;
				prev_StrAUTHENTICATION_ATTEMPT_RC = soldCup3ds2.authentication_attempt_rc;
				prev_StrCVN2_VALIDATION_FLAG=soldCup3ds2.cvn2_validation_flag;
				prev_StrAV_VALIDATION_FLAG=soldCup3ds2.av_validation_flag;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldCup3ds2.cup_authentication_attempt_verify_flag;

					/*if ( cvn2_validation_flg == "Y" )
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvn2_validation_flg == "N")
						m_CntlCVN2_VALIDATION_FLAG.SetCheck(0);

					if ( av_validation_flg == "Y" )
						m_CntlAV_VALIDATION_FLAG.SetCheck(1);
					else if ( av_validation_flg == "N")
						m_CntlAV_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";*/

				m_StrMessageLbl="";

				m_CntlAV_VERIFICATION_RC.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ATTEMPT_RC.EnableWindow(FALSE);
				m_CntlCVN2_VALIDATION_FLAG.EnableWindow(FALSE);
				m_CntlAV_VALIDATION_FLAG.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG.SetCurSel(0);
				UpdateData( FALSE );

				
				return TRUE;
		}
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			//ramya
			m_StrMessageLbl = "AV Verification Response Code";
			m_CntlAV_VERIFICATION_RC.EnableWindow(FALSE);
			
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

BOOL CCup3DS2Parameter::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};

	m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;
		
	if (!CheckDialogForCompleteness())
        return FALSE;

	
	if(fst_checkk==1)
	{
		fst_checkk = 0 ;
		strcpy((char *)snewCup3ds2.av_verification_rc ,m_StrAV_VERIFICATION_RC);
		strcpy((char *)snewCup3ds2.authentication_attempt_rc,m_StrAUTHENTICATION_ATTEMPT_RC);


			if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";

					strcpy((char *)snewCup3ds2.cup_authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);
			if ( m_CntlCVN2_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewCup3ds2.cvn2_validation_flag, "Y", sizeof ( snewCup3ds2.cvn2_validation_flag));
		else
			memcpy ( (char *)snewCup3ds2.cvn2_validation_flag, "N", sizeof ( snewCup3ds2.cvn2_validation_flag));

			if ( m_CntlAV_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewCup3ds2.av_validation_flag, "Y", sizeof ( snewCup3ds2.av_validation_flag));
		else
			memcpy ( (char *)snewCup3ds2.av_validation_flag, "N", sizeof ( snewCup3ds2.av_validation_flag));

		rcode = txdsapi_insert_record  ((pBYTE)&snewCup3ds2, sizeof (CUP3DS2), CUP3DS2_DATA,strErrorMessage);
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
	if(user_information.control[4].action[2]=='1')
	{
		
		strcpy((char *)snewCup3ds2.av_verification_rc ,m_StrAV_VERIFICATION_RC);
		strcpy((char *)snewCup3ds2.authentication_attempt_rc,m_StrAUTHENTICATION_ATTEMPT_RC);


		if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";

				strcpy((char *)snewCup3ds2.cup_authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);
			if ( m_CntlCVN2_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewCup3ds2.cvn2_validation_flag, "Y", sizeof ( snewCup3ds2.cvn2_validation_flag));
		else
			memcpy ( (char *)snewCup3ds2.cvn2_validation_flag, "N", sizeof ( snewCup3ds2.cvn2_validation_flag));

			if ( m_CntlAV_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewCup3ds2.av_validation_flag, "Y", sizeof ( snewCup3ds2.av_validation_flag));
		else
			memcpy ( (char *)snewCup3ds2.av_validation_flag, "N", sizeof ( snewCup3ds2.av_validation_flag));



		rcode = txdsapi_update_record  ((pBYTE)&snewCup3ds2, sizeof (CUP3DS2), CUP3DS2_DATA,strErrorMessage);

		new_StrAV_VERIFICATION_RC = snewCup3ds2.av_verification_rc;
		new_StrAUTHENTICATION_ATTEMPT_RC = snewCup3ds2.authentication_attempt_rc;
		new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=snewCup3ds2.cup_authentication_attempt_verify_flag;
		new_StrCVN2_VALIDATION_FLAG = snewCup3ds2.cvn2_validation_flag;
		new_StrAV_VALIDATION_FLAG=snewCup3ds2.av_validation_flag;

		PopulateControlStructure();
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

void CCup3DS2Parameter::OnViewButton() 
{
	HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "CUP3DS2", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

	
}

BOOL CCup3DS2Parameter::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	m_StrMessageLbl = "";
	if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    if (myUtilsObject.IsEmpty (m_StrAV_VERIFICATION_RC, this))
    {
		m_StrMessageLbl = "Please Enter the AV Validation Response code";	
		m_CntlAV_VERIFICATION_RC.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}


	else if (myUtilsObject.IsEmpty (m_StrAUTHENTICATION_ATTEMPT_RC, this))
    {
		m_StrMessageLbl = "Please Enter Authentication attempt RC";	
		m_CntlAUTHENTICATION_ATTEMPT_RC.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	return TRUE;

}

void CCup3DS2Parameter::PopulateHistoryTable()
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

    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CUP3DS2", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );


	if((new_StrAV_VERIFICATION_RC != prev_StrAV_VERIFICATION_RC) && change_AV_VERIFICATION_RC==true)
	{
		change_AV_VERIFICATION_RC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewCup3ds2.av_verification_rc , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldCup3ds2.av_verification_rc, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AV Verification RC", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}


	else if((new_StrAUTHENTICATION_ATTEMPT_RC!=prev_StrAUTHENTICATION_ATTEMPT_RC) && change_AUTHENTICATION_ATTEMPT_RC == true)
	{
		change_AUTHENTICATION_ATTEMPT_RC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewCup3ds2.authentication_attempt_rc , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldCup3ds2.authentication_attempt_rc, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"Auth Attempt RC", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
	else if((new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG) && change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG == true)
	{
		change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewCup3ds2.cup_authentication_attempt_verify_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldCup3ds2.cup_authentication_attempt_verify_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AUTH ATTEMPT VERIFY FLAG", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
	else if((new_StrCVN2_VALIDATION_FLAG!=prev_StrCVN2_VALIDATION_FLAG) && change_StrCVN2_VALIDATION_FLAG == true)
	{
		change_StrCVN2_VALIDATION_FLAG = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewCup3ds2.cvn2_validation_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldCup3ds2.cvn2_validation_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"CVN2 Validation Flag", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_StrAV_VALIDATION_FLAG!=prev_StrAV_VALIDATION_FLAG) && change_StrAV_VALIDATION_FLAG == true)
	{
		change_StrAV_VALIDATION_FLAG = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewCup3ds2.av_validation_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldCup3ds2.av_validation_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AV Validation Flag", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}



}

void CCup3DS2Parameter::PopulateControlStructure()
{

	if(new_StrAV_VERIFICATION_RC != change_AV_VERIFICATION_RC)
	{
		change_AV_VERIFICATION_RC = true;
	}
	

	if(new_StrAUTHENTICATION_ATTEMPT_RC!=prev_StrAUTHENTICATION_ATTEMPT_RC)
	{
		change_AUTHENTICATION_ATTEMPT_RC = true;
	}

	if(new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG)
	{
		change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG = true;
	}

	if(new_StrCVN2_VALIDATION_FLAG!=prev_StrCVN2_VALIDATION_FLAG)
	{
		change_StrCVN2_VALIDATION_FLAG = true;
	}

	if(new_StrAV_VALIDATION_FLAG!=prev_StrAV_VALIDATION_FLAG)
	{
		change_StrAV_VALIDATION_FLAG = true;
	}

}
