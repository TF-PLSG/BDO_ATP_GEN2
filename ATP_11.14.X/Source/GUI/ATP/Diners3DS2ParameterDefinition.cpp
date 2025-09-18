#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "Diners3DS2ParameterDefinition.h"
#include "Diners3DS2PropertySheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
#include "PteTime.h"
}

DCI3DS2 soldDiners3ds2,snewDiners3ds2;
extern CSharedMem shrmem;
extern UCF01 ucf01;
UCF01 user_information_DCI;
int fast_checkk;
extern char login_name[51];
IMPLEMENT_DYNAMIC(CDiners3DS2Parameter, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CDiners3DS2Parameter::CDiners3DS2Parameter()
	: CPropertyPage(CDiners3DS2Parameter::IDD)	
{
	//memset(&m_ATP_Monitor_Service, 0, sizeof(ATP_MONITOR_SERVICE));
//	memset(&m_LastSubsidyQuota, 0, sizeof(SUBSIDY_QUOTA));
	
	
	//m_strFieldTag = "ATP_MONITOR_SERVICE";  
	
}

CDiners3DS2Parameter::~CDiners3DS2Parameter()
{
}

void CDiners3DS2Parameter::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAVV_RESULT_PASS, m_CntlCAVV_RESULT_PASS);
	DDX_Text(pDX, IDC_CAVV_RESULT_PASS, m_StrCAVV_RESULT_PASS);
	DDV_MaxChars(pDX,m_StrCAVV_RESULT_PASS,2);

	DDX_Control(pDX, IDC_CAVV_RESULT_CODE, m_CntlCAVV_RESULT_CODE);
	DDX_Text(pDX, IDC_CAVV_RESULT_CODE, m_StrCAVV_RESULT_CODE);
	DDV_MaxChars(pDX,m_StrCAVV_RESULT_CODE,2);

	DDX_Control(pDX, IDC_AUTHENTICATION_ACTION_CODE, m_CntlAUTHENTICATION_ACTION_CODE);
	DDX_Text(pDX, IDC_AUTHENTICATION_ACTION_CODE, m_StrAUTHENTICATION_ACTION_CODE);
	DDV_MaxChars(pDX,m_StrAUTHENTICATION_ACTION_CODE,3);

	DDX_Control(pDX, IDC_AUTHENTICATION_CAVV_RESULT_CODE, m_CntlAUTHENTICATION_CAVV_RESULT_CODE);
	DDX_Text(pDX, IDC_AUTHENTICATION_CAVV_RESULT_CODE, m_StrAUTHENTICATION_CAVV_RESULT_CODE);
	DDV_MaxChars(pDX,m_StrAUTHENTICATION_CAVV_RESULT_CODE,2);

    DDX_Control(pDX, IDC_ECI6_ACTION_CODE, m_CntlECI6_ACTION_CODE);
	DDX_Text(pDX, IDC_ECI6_ACTION_CODE, m_StrECI6_ACTION_CODE);
	DDV_MaxChars(pDX,m_StrECI6_ACTION_CODE,3);

	DDX_Control(pDX, IDC_CVV2_VALIDATION_FLAG, m_CntlCVV2_VALIDATION_FLAG);

	DDX_Control(pDX, IDC_DINERS3DS2_VIEW_BUTTON, m_ViewButton);
	DDX_Text(pDX, IDC_MESSAGE_DINERS3DS2_LBL, m_StrMessageLbl);

	DDX_Control(pDX, IDC_DCI_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG);
	DDX_CBString(pDX, IDC_DCI_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);

}

BEGIN_MESSAGE_MAP(CDiners3DS2Parameter, CPropertyPage) // very imprtnt.
	ON_BN_CLICKED(IDC_DINERS3DS2_VIEW_BUTTON, OnViewButton)
	ON_CBN_SELCHANGE(IDC_DCI_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, &CDiners3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag)
	ON_BN_CLICKED(IDC_CVV2_VALIDATION_FLAG, &CDiners3DS2Parameter::OnBnClickedCvv2ValidationFlag)
END_MESSAGE_MAP()

BOOL CDiners3DS2Parameter::OnSetActive() 
{
	((CDiners3DS2PropertySheet*) this->GetParent())->m_nActiveTab = 1;

	return TRUE;
}

void CDiners3DS2Parameter::OnOK() 
{
	if ( m_nDialogMode == MODE_DONE)	
		CPropertyPage::OnOK();
	else 
	{
		m_nDialogMode = MODE_DONE;
		return;
	}
}

void CDiners3DS2Parameter::OnCancel() 
{
	m_StrCAVV_RESULT_PASS = _T("");
	m_StrCAVV_RESULT_CODE = _T("");	
	m_StrAUTHENTICATION_ACTION_CODE= _T("");
	m_StrAUTHENTICATION_CAVV_RESULT_CODE = _T("");	
	m_StrECI6_ACTION_CODE= _T("");
	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG= _T("");

   m_nDialogMode   = MODE_DONE;

	CPropertyPage::OnCancel();
}

BOOL CDiners3DS2Parameter::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CDiners3DS2Parameter::OnInitDialog() 
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
	GetDlgItem(IDC_MESSAGE_DINERS3DS2_LBL)->SetFont (&m_font);
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	CString temp;
	CString cvv2_validation_flg;
	int nIndex = -1;

	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
	//m_CntlStatus.SetCurSel(0);

    m_nDialogMode = MODE_DONE;

	memset(&soldDiners3ds2,0x00,sizeof(soldDiners3ds2));
	memset(&snewDiners3ds2,0x00,sizeof(snewDiners3ds2));
	memset(&user_information_DCI,0x00,sizeof(user_information_DCI));
	strncpy((char *)user_information_DCI.primary_key.name,login_name,strlen(login_name));
	rcode = txdsapi_get_record  ((pBYTE)&user_information_DCI, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData( FALSE );
		return FALSE;
	}
	if(user_information_DCI.control[4].action[2]=='1')
	{
		rcode = txdsapi_get_record  ((pBYTE)&soldDiners3ds2, sizeof (DCI3DS2), DINERS3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_StrCAVV_RESULT_PASS = soldDiners3ds2.self_validation_pass_cavv_rc ;
				m_StrCAVV_RESULT_CODE =soldDiners3ds2.self_validation_cavv_rc ;
				m_StrAUTHENTICATION_ACTION_CODE = soldDiners3ds2.authentication_result_code_ac;
				m_StrAUTHENTICATION_CAVV_RESULT_CODE=soldDiners3ds2.authentication_result_code_cavv_rc;
				m_StrECI6_ACTION_CODE =soldDiners3ds2.authentication_attmpt_ac;
				cvv2_validation_flg=soldDiners3ds2.cvv2_validation_flag;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = soldDiners3ds2.authentication_attempt_verify_flag;

					if ( cvv2_validation_flg == "Y" )
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvv2_validation_flg == "N")
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";
				
				prev_StrCAVV_RESULT_PASS = soldDiners3ds2.self_validation_pass_cavv_rc;
				prev_StrCAVV_RESULT_CODE = soldDiners3ds2.self_validation_cavv_rc;
				prev_StrAUTHENTICATION_ACTION_CODE = soldDiners3ds2.authentication_result_code_ac;
				prev_StrAUTHENTICATION_CAVV_RESULT_CODE=soldDiners3ds2.authentication_result_code_cavv_rc;
				prev_StrECI6_ACTION_CODE =soldDiners3ds2.authentication_attmpt_ac;
				//cvv2_validation_flg=soldDiners3ds2.cvv2_validation_flag;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = soldDiners3ds2.authentication_attempt_verify_flag;
				prev_StrCVV2_VALIDATION_FLAG=soldDiners3ds2.cvv2_validation_flag;

					/*if ( cvv2_validation_flg == "Y" )
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvv2_validation_flg == "N")
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(0);
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
			fast_checkk = 1 ;
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
		rcode = txdsapi_get_record  ((pBYTE)&soldDiners3ds2, sizeof (DCI3DS2), DINERS3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{	
				m_StrCAVV_RESULT_PASS =soldDiners3ds2.self_validation_pass_cavv_rc;
				m_StrCAVV_RESULT_CODE =soldDiners3ds2.self_validation_cavv_rc;
				m_StrAUTHENTICATION_ACTION_CODE = soldDiners3ds2.authentication_result_code_ac;
				m_StrAUTHENTICATION_CAVV_RESULT_CODE=soldDiners3ds2.authentication_result_code_cavv_rc;
				m_StrECI6_ACTION_CODE =soldDiners3ds2.authentication_attmpt_ac;
				cvv2_validation_flg=soldDiners3ds2.cvv2_validation_flag;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = soldDiners3ds2.authentication_attempt_verify_flag;

					if ( cvv2_validation_flg == "Y" )
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvv2_validation_flg == "N")
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";

				prev_StrCAVV_RESULT_PASS =soldDiners3ds2.self_validation_pass_cavv_rc;
				prev_StrCAVV_RESULT_CODE =soldDiners3ds2.self_validation_cavv_rc;
				prev_StrAUTHENTICATION_ACTION_CODE = soldDiners3ds2.authentication_result_code_ac;
				prev_StrAUTHENTICATION_CAVV_RESULT_CODE=soldDiners3ds2.authentication_result_code_cavv_rc;
				prev_StrECI6_ACTION_CODE =soldDiners3ds2.authentication_attmpt_ac;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldDiners3ds2.authentication_attempt_verify_flag;
				prev_StrCVV2_VALIDATION_FLAG=soldDiners3ds2.cvv2_validation_flag;

					/*if ( cvv2_validation_flg == "Y" )
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(1);
					else if ( cvv2_validation_flg == "N")
						m_CntlCVV2_VALIDATION_FLAG.SetCheck(0);

					if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";*/


				m_StrMessageLbl="";

				m_CntlCAVV_RESULT_PASS.EnableWindow(FALSE);
				m_CntlCAVV_RESULT_CODE.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ACTION_CODE.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_CAVV_RESULT_CODE.EnableWindow(FALSE);
				m_CntlECI6_ACTION_CODE.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG.SetCurSel(0);
				m_CntlCVV2_VALIDATION_FLAG.EnableWindow(FALSE);

				UpdateData( FALSE );

				
				return TRUE;
		}
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			//ramya
			m_StrMessageLbl = "CAVV Result Scode";
			m_CntlCAVV_RESULT_PASS.EnableWindow(FALSE);
			
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

BOOL CDiners3DS2Parameter::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};

	m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;
		
	if (!CheckDialogForCompleteness())
        return FALSE;

	
	if(fast_checkk==1)
	{
		fast_checkk = 0 ;
		strcpy((char *)snewDiners3ds2.self_validation_pass_cavv_rc ,m_StrCAVV_RESULT_PASS);
		strcpy((char *)snewDiners3ds2.self_validation_cavv_rc ,m_StrCAVV_RESULT_CODE);
		strcpy((char *)snewDiners3ds2.authentication_result_code_ac,m_StrAUTHENTICATION_ACTION_CODE);
		strcpy((char *)snewDiners3ds2.authentication_result_code_cavv_rc ,m_StrAUTHENTICATION_CAVV_RESULT_CODE);
			if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";
		strcpy((char *)snewDiners3ds2.authentication_attmpt_ac,m_StrECI6_ACTION_CODE);
		strcpy((char *)snewDiners3ds2.authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);

			if ( m_CntlCVV2_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewDiners3ds2.cvv2_validation_flag, "Y", sizeof ( snewDiners3ds2.cvv2_validation_flag));
		else
			memcpy ( (char *)snewDiners3ds2.cvv2_validation_flag, "N", sizeof ( snewDiners3ds2.cvv2_validation_flag));

		rcode = txdsapi_insert_record  ((pBYTE)&snewDiners3ds2, sizeof (DCI3DS2), DINERS3DS2_DATA,strErrorMessage);
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
	if(user_information_DCI.control[4].action[2]=='1')
	{
		strcpy((char *)snewDiners3ds2.self_validation_pass_cavv_rc ,m_StrCAVV_RESULT_PASS);
		strcpy((char *)snewDiners3ds2.self_validation_cavv_rc ,m_StrCAVV_RESULT_CODE);
		strcpy((char *)snewDiners3ds2.authentication_result_code_ac,m_StrAUTHENTICATION_ACTION_CODE);
		strcpy((char *)snewDiners3ds2.authentication_result_code_cavv_rc ,m_StrAUTHENTICATION_CAVV_RESULT_CODE);
			if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";
		strcpy((char *)snewDiners3ds2.authentication_attmpt_ac,m_StrECI6_ACTION_CODE);
		strcpy((char *)snewDiners3ds2.authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);


			if ( m_CntlCVV2_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewDiners3ds2.cvv2_validation_flag, "Y", sizeof ( snewDiners3ds2.cvv2_validation_flag));
		else
			memcpy ( (char *)snewDiners3ds2.cvv2_validation_flag, "N", sizeof ( snewDiners3ds2.cvv2_validation_flag));

		rcode = txdsapi_update_record  ((pBYTE)&snewDiners3ds2, sizeof (DCI3DS2), DINERS3DS2_DATA,strErrorMessage);

		new_StrCAVV_RESULT_PASS =snewDiners3ds2.self_validation_pass_cavv_rc;
		new_StrCAVV_RESULT_CODE =snewDiners3ds2.self_validation_cavv_rc;
		new_StrAUTHENTICATION_ACTION_CODE = snewDiners3ds2.authentication_result_code_ac;
		new_StrAUTHENTICATION_CAVV_RESULT_CODE=snewDiners3ds2.authentication_result_code_cavv_rc;
		new_StrECI6_ACTION_CODE =snewDiners3ds2.authentication_attmpt_ac;
		new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=snewDiners3ds2.authentication_attempt_verify_flag;
		new_StrCVV2_VALIDATION_FLAG= snewDiners3ds2.cvv2_validation_flag;

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

void CDiners3DS2Parameter::OnViewButton() 
{
	HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "DCI3DS2", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

	
}

BOOL CDiners3DS2Parameter::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	m_StrMessageLbl = "";
	if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	if(myUtilsObject.IsEmpty (m_StrCAVV_RESULT_PASS, this))
    {
		m_StrMessageLbl = "Please Enter the Pass CAVV Result code";	
		m_CntlCAVV_RESULT_PASS.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

    else if (myUtilsObject.IsEmpty (m_StrCAVV_RESULT_CODE, this))
    {
		m_StrMessageLbl = "Please Enter the Fail CAVV Result code";	
		m_CntlCAVV_RESULT_CODE.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}



	else if (myUtilsObject.IsEmpty (m_StrAUTHENTICATION_ACTION_CODE, this))
    {
		m_StrMessageLbl = "Please Enter Authentication Action code";	
		m_CntlAUTHENTICATION_ACTION_CODE.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

		else if (myUtilsObject.IsEmpty (m_StrAUTHENTICATION_CAVV_RESULT_CODE, this))
    {
		m_StrMessageLbl = "Please Enter Authentication CAVV Result Code";	
		m_CntlAUTHENTICATION_CAVV_RESULT_CODE.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

		else if (myUtilsObject.IsEmpty (m_StrECI6_ACTION_CODE, this))
    {
		m_StrMessageLbl = "Please Enter ECI6 Action code";	
		m_CntlECI6_ACTION_CODE.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	return TRUE;

}

void CDiners3DS2Parameter::PopulateHistoryTable()
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

    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"DCI3DS2", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );


	if((new_StrCAVV_RESULT_PASS != prev_StrCAVV_RESULT_PASS) && change_CAVV_RESULT_PASS==true)
	{
		change_CAVV_RESULT_PASS = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.self_validation_pass_cavv_rc , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.self_validation_pass_cavv_rc, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"CAVV Result Scode", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
	else if((new_StrCAVV_RESULT_CODE != prev_StrCAVV_RESULT_CODE) && change_CAVV_RESULT_CODE==true)
	{
		change_CAVV_RESULT_CODE = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.self_validation_cavv_rc , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.self_validation_cavv_rc, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"CAVV Result Fcode", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}


	else if((new_StrAUTHENTICATION_ACTION_CODE!=prev_StrAUTHENTICATION_ACTION_CODE) && change_AUTHENTICATION_ACTION_CODE == true)
	{
		change_AUTHENTICATION_ACTION_CODE = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.authentication_result_code_ac , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.authentication_result_code_ac, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"Authen Action code", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_StrAUTHENTICATION_CAVV_RESULT_CODE!=prev_StrAUTHENTICATION_CAVV_RESULT_CODE) && change_AUTHENTICATION_CAVV_RESULT_CODE == true)
	{
		change_AUTHENTICATION_CAVV_RESULT_CODE = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.authentication_result_code_cavv_rc , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.authentication_result_code_cavv_rc, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"Auth CAVV result Code", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_StrECI6_ACTION_CODE!=prev_StrECI6_ACTION_CODE) && change_ECI6_ACTION_CODE == true)
	{
		change_ECI6_ACTION_CODE = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.authentication_attmpt_ac , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.authentication_attmpt_ac, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"ECI6 Action Code", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
	else if((new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG) && change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG == true)
	{
		change_ECI6_ACTION_CODE = false;

		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.authentication_attempt_verify_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.authentication_attempt_verify_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AUTH ATTEMPT VERIFY FLAG", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
	else if((new_StrCVV2_VALIDATION_FLAG!=prev_StrCVV2_VALIDATION_FLAG) && change_StrCVV2_VALIDATION_FLAG == true)
	{
		change_StrCVV2_VALIDATION_FLAG = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewDiners3ds2.cvv2_validation_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldDiners3ds2.cvv2_validation_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"CVV2 VALIDATION FLAG", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}




}

void CDiners3DS2Parameter::PopulateControlStructure()
{

	if(new_StrCAVV_RESULT_PASS != prev_StrCAVV_RESULT_PASS)
	{
		change_CAVV_RESULT_PASS = true;
	}

	if(new_StrCAVV_RESULT_CODE != prev_StrCAVV_RESULT_CODE)
	{
		change_CAVV_RESULT_CODE = true;
	}
	

	if(new_StrAUTHENTICATION_ACTION_CODE!=prev_StrAUTHENTICATION_ACTION_CODE)
	{
		change_AUTHENTICATION_ACTION_CODE = true;
	}

	if(new_StrAUTHENTICATION_CAVV_RESULT_CODE != prev_StrAUTHENTICATION_CAVV_RESULT_CODE)
	{
		change_AUTHENTICATION_CAVV_RESULT_CODE= true;
	}
	

	if(new_StrECI6_ACTION_CODE!=prev_StrECI6_ACTION_CODE)
	{
		change_ECI6_ACTION_CODE = true;
	}

	if(new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG)
	{
		change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG = true;
	}

	if(new_StrCVV2_VALIDATION_FLAG!=prev_StrCVV2_VALIDATION_FLAG)
	{
		change_StrCVV2_VALIDATION_FLAG = true;
	}

}

void CDiners3DS2Parameter::OnBnClickedCvv2ValidationFlag()
{
	// TODO: Add your control notification handler code here
}

void CDiners3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag()
{
	// TODO: Add your control notification handler code here
}