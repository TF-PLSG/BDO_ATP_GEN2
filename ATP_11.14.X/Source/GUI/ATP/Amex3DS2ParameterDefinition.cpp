#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "Amex3DS2ParameterDefinition.h"
#include "Amex3DS2PropertySheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
#include "PteTime.h"
}

AMEX3DS2 soldAmex3ds2,snewAmex3ds2;
extern CSharedMem shrmem;
extern UCF01 ucf01;
UCF01 user_info;
int fst_chkk;
extern char login_name[51];
IMPLEMENT_DYNAMIC(CAmex3DS2Parameter, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CAmex3DS2Parameter::CAmex3DS2Parameter()
	: CPropertyPage(CAmex3DS2Parameter::IDD)	
{
	//memset(&m_ATP_Monitor_Service, 0, sizeof(ATP_MONITOR_SERVICE));
//	memset(&m_LastSubsidyQuota, 0, sizeof(SUBSIDY_QUOTA));
	
	
	//m_strFieldTag = "ATP_MONITOR_SERVICE";  
	
}

CAmex3DS2Parameter::~CAmex3DS2Parameter()
{
}

void CAmex3DS2Parameter::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CSystemGlobalParameter)
	DDX_Control(pDX, IDC_SELF_VALIDATION_AC, m_CntlSELF_VALIDATION_AC);
	DDX_Text(pDX, IDC_SELF_VALIDATION_AC, m_StrSELF_VALIDATION_AC);
	DDV_MaxChars(pDX,m_StrSELF_VALIDATION_AC,3);

	DDX_Control(pDX, IDC_AUTHENTICATION_RESULT_CODE_AC, m_CntlAUTHENTICATION_RESULT_CODE_AC);
	DDX_Text(pDX, IDC_AUTHENTICATION_RESULT_CODE_AC, m_StrAUTHENTICATION_RESULT_CODE_AC);
	DDV_MaxChars(pDX,m_StrAUTHENTICATION_RESULT_CODE_AC,3);

	DDX_Control(pDX, IDC_AUTHENTICATION_ATTEMPT_AC, m_CntlAUTHENTICATION_ATTEMPT_AC);
	DDX_Text(pDX, IDC_AUTHENTICATION_ATTEMPT_AC, m_StrAUTHENTICATION_ATTEMPT_AC);
	DDV_MaxChars(pDX,m_StrAUTHENTICATION_ATTEMPT_AC,3);

	DDX_Control(pDX, IDC_CSC_VALIDATION_FLAG, m_CntlCSC_VALIDATION_FLAG);

	DDX_Control(pDX, IDC_AMEX3DS2_VIEW_BUTTON, m_ViewButton);
	DDX_Text(pDX, IDC_MESSAGE_AMEX3DS2_LBL, m_StrMessageLbl);

	DDX_Control(pDX, IDC_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG);
	DDX_CBString(pDX, IDC_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);


}


BEGIN_MESSAGE_MAP(CAmex3DS2Parameter, CPropertyPage) // very imprtnt.
	ON_BN_CLICKED(IDC_AMEX3DS2_VIEW_BUTTON, OnViewButton)
	ON_CBN_SELCHANGE(IDC_AUTHENTICATION_ATTEMPT_VERIFY_FLAG, &CAmex3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag)
END_MESSAGE_MAP()

BOOL CAmex3DS2Parameter::OnSetActive() 
{
	((CAmex3DS2PropertySheet*) this->GetParent())->m_nActiveTab = 1;

	return TRUE;
}

void CAmex3DS2Parameter::OnOK() 
{
	if ( m_nDialogMode == MODE_DONE)	
		CPropertyPage::OnOK();
	else 
	{
		m_nDialogMode = MODE_DONE;
		return;
	}
}

void CAmex3DS2Parameter::OnCancel() 
{

	m_StrSELF_VALIDATION_AC = _T("");	
	m_StrAUTHENTICATION_RESULT_CODE_AC= _T("");
	m_StrAUTHENTICATION_ATTEMPT_AC= _T("");
	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG= _T("");

   m_nDialogMode   = MODE_DONE;

	CPropertyPage::OnCancel();
}

BOOL CAmex3DS2Parameter::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CAmex3DS2Parameter::OnInitDialog() 
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
	GetDlgItem(IDC_MESSAGE_AMEX3DS2_LBL)->SetFont (&m_font);
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	//m_Strglblparam = "";
	//m_Strglblparam2 = "";
	CString temp;
	CString csc_validation_flg;
	int nIndex = -1;

	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
	//m_CntlStatus.SetCurSel(0);


    m_nDialogMode = MODE_DONE;

	memset(&soldAmex3ds2,0x00,sizeof(soldAmex3ds2));
	memset(&snewAmex3ds2,0x00,sizeof(snewAmex3ds2));
	memset(&user_info,0x00,sizeof(user_info));
	strncpy((char *)user_info.primary_key.name,login_name,strlen(login_name));
	rcode = txdsapi_get_record  ((pBYTE)&user_info, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData( FALSE );
		return FALSE;
	}
	if(user_info.control[4].action[2]=='1')
	{
		rcode = txdsapi_get_record  ((pBYTE)&soldAmex3ds2, sizeof (AMEX3DS2), AMEX3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_StrSELF_VALIDATION_AC =soldAmex3ds2.self_validation_ac;
				m_StrAUTHENTICATION_RESULT_CODE_AC = soldAmex3ds2.authentication_result_code_ac;
				m_StrAUTHENTICATION_ATTEMPT_AC = soldAmex3ds2.authentication_attempt_ac;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldAmex3ds2.authentication_attempt_verify_flag;
				csc_validation_flg=soldAmex3ds2.csc_validation_flag;

					if ( csc_validation_flg == "Y" )
						m_CntlCSC_VALIDATION_FLAG.SetCheck(1);
					else if ( csc_validation_flg == "N")
						m_CntlCSC_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";


				prev_StrSELF_VALIDATION_AC =soldAmex3ds2.self_validation_ac;
				prev_StrAUTHENTICATION_RESULT_CODE_AC = soldAmex3ds2.authentication_result_code_ac;
				prev_StrAUTHENTICATION_ATTEMPT_AC = soldAmex3ds2.authentication_attempt_ac;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldAmex3ds2.authentication_attempt_verify_flag;
				prev_StrCSC_VALIDATION_FLAG=soldAmex3ds2.csc_validation_flag;

					/*if ( prev_StrCSC_VALIDATION_FLAG == "Y" )
						m_CntlCSC_VALIDATION_FLAG.SetCheck(1);
					else if ( prev_StrCSC_VALIDATION_FLAG == "N")
						m_CntlCSC_VALIDATION_FLAG.SetCheck(0);

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
			fst_chkk = 1 ;
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
		rcode = txdsapi_get_record  ((pBYTE)&soldAmex3ds2, sizeof (AMEX3DS2), AMEX3DS2_DATA, strErrorMessage);
		if ( rcode == PTEMSG_OK )
		{
				m_StrSELF_VALIDATION_AC =soldAmex3ds2.self_validation_ac;
				m_StrAUTHENTICATION_RESULT_CODE_AC = soldAmex3ds2.authentication_result_code_ac;
				m_StrAUTHENTICATION_ATTEMPT_AC = soldAmex3ds2.authentication_attempt_ac;
				m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG =soldAmex3ds2.authentication_attempt_verify_flag;
				csc_validation_flg=soldAmex3ds2.csc_validation_flag;

					if ( csc_validation_flg == "Y" )
						m_CntlCSC_VALIDATION_FLAG.SetCheck(1);
					else if ( csc_validation_flg == "N")
						m_CntlCSC_VALIDATION_FLAG.SetCheck(0);

					if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";

				prev_StrSELF_VALIDATION_AC =soldAmex3ds2.self_validation_ac;
				prev_StrAUTHENTICATION_RESULT_CODE_AC = soldAmex3ds2.authentication_result_code_ac;
				prev_StrAUTHENTICATION_ATTEMPT_AC = soldAmex3ds2.authentication_attempt_ac;
				prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=soldAmex3ds2.authentication_attempt_verify_flag;
				prev_StrCSC_VALIDATION_FLAG=soldAmex3ds2.csc_validation_flag;

					/*if ( prev_StrCSC_VALIDATION_FLAG == "Y" )
						m_CntlCSC_VALIDATION_FLAG.SetCheck(1);
					else if ( prev_StrCSC_VALIDATION_FLAG == "N")
						m_CntlCSC_VALIDATION_FLAG.SetCheck(0);

					if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "A")
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "APPROVED";
					else if ( prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "D" )
					prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "DECLINED";*/

					//strCSCFlag=soldAmex3ds2.csc_validation_flag;


				m_StrMessageLbl="";

				m_CntlSELF_VALIDATION_AC.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_RESULT_CODE_AC.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ATTEMPT_AC.EnableWindow(FALSE);
				m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG.SetCurSel(0);
				m_CntlCSC_VALIDATION_FLAG.EnableWindow(FALSE);

				UpdateData( FALSE );

				
				return TRUE;
		}
		else if(rcode == PTEMSG_NOT_FOUND )
		{
			//ramya
			m_StrMessageLbl = "SELF VLIDATION AC";
			m_CntlSELF_VALIDATION_AC.EnableWindow(FALSE);
			
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

BOOL CAmex3DS2Parameter::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};

	m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;
		
	if (!CheckDialogForCompleteness())
        return FALSE;

	
	if(fst_chkk==1)
	{
		fst_chkk = 0 ;
		strcpy((char *)snewAmex3ds2.self_validation_ac ,m_StrSELF_VALIDATION_AC);
		strcpy((char *)snewAmex3ds2.authentication_result_code_ac,m_StrAUTHENTICATION_RESULT_CODE_AC);
		strcpy((char *)snewAmex3ds2.authentication_attempt_ac,m_StrAUTHENTICATION_ATTEMPT_AC);

			if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";

		strcpy((char *)snewAmex3ds2.authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);

			if ( m_CntlCSC_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewAmex3ds2.csc_validation_flag, "Y", sizeof ( snewAmex3ds2.csc_validation_flag));
		else
			memcpy ( (char *)snewAmex3ds2.csc_validation_flag, "N", sizeof ( snewAmex3ds2.csc_validation_flag));

			new_StrCSC_VALIDATION_FLAG=snewAmex3ds2.csc_validation_flag;

		rcode = txdsapi_insert_record  ((pBYTE)&snewAmex3ds2, sizeof (AMEX3DS2), AMEX3DS2_DATA,strErrorMessage);
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
	if(user_info.control[4].action[2]=='1')
	{
		
		strcpy((char *)snewAmex3ds2.self_validation_ac ,m_StrSELF_VALIDATION_AC);
		strcpy((char *)snewAmex3ds2.authentication_result_code_ac,m_StrAUTHENTICATION_RESULT_CODE_AC);
		strcpy((char *)snewAmex3ds2.authentication_attempt_ac,m_StrAUTHENTICATION_ATTEMPT_AC);

		if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "APPROVED")
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "A";
			else if ( m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG == "DECLINED" )
			m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG = "D";

		strcpy((char *)snewAmex3ds2.authentication_attempt_verify_flag,m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG);

			if ( m_CntlCSC_VALIDATION_FLAG.GetCheck() == 1)
			memcpy ((char *)snewAmex3ds2.csc_validation_flag, "Y", sizeof ( snewAmex3ds2.csc_validation_flag));
		else
			memcpy ( (char *)snewAmex3ds2.csc_validation_flag, "N", sizeof ( snewAmex3ds2.csc_validation_flag));



		rcode = txdsapi_update_record  ((pBYTE)&snewAmex3ds2, sizeof (AMEX3DS2), AMEX3DS2_DATA,strErrorMessage);

		new_StrSELF_VALIDATION_AC = snewAmex3ds2.self_validation_ac;
		new_StrAUTHENTICATION_RESULT_CODE_AC = snewAmex3ds2.authentication_result_code_ac;
		new_StrAUTHENTICATION_ATTEMPT_AC = snewAmex3ds2.authentication_attempt_ac;
		new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG=snewAmex3ds2.authentication_attempt_verify_flag;
		new_StrCSC_VALIDATION_FLAG= snewAmex3ds2.csc_validation_flag;

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

void CAmex3DS2Parameter::OnViewButton() 
{
	HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "AMEX3DS2", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

	
}

BOOL CAmex3DS2Parameter::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	m_StrMessageLbl = "";
	if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    if (myUtilsObject.IsEmpty (m_StrSELF_VALIDATION_AC, this))
    {
		m_StrMessageLbl = "Please Enter the Self Validation Action code";	
		m_CntlSELF_VALIDATION_AC.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	else if (myUtilsObject.IsEmpty (m_StrAUTHENTICATION_RESULT_CODE_AC, this))
    {
		m_StrMessageLbl = "Please Enter Authentication Result Code AC";	
		m_CntlAUTHENTICATION_RESULT_CODE_AC.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	else if (myUtilsObject.IsEmpty (m_StrAUTHENTICATION_ATTEMPT_AC, this))
    {
		m_StrMessageLbl = "Please Enter Authentication attempt AC";	
		m_CntlAUTHENTICATION_ATTEMPT_AC.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return FALSE;
	}

	return TRUE;

}

void CAmex3DS2Parameter::PopulateHistoryTable()
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

    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"AMEX3DS2", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );


	if((new_StrSELF_VALIDATION_AC != prev_StrSELF_VALIDATION_AC) && change_SELF_VALIDATION_AC==true)
	{
		change_SELF_VALIDATION_AC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewAmex3ds2.self_validation_ac , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldAmex3ds2.self_validation_ac, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"SELF VALIDATION RC", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	
	else if((new_StrAUTHENTICATION_RESULT_CODE_AC!=prev_StrAUTHENTICATION_RESULT_CODE_AC) && change_AUTHENTICATION_RESULT_CODE_AC==true)
	{
		change_AUTHENTICATION_RESULT_CODE_AC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewAmex3ds2.authentication_result_code_ac , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldAmex3ds2.authentication_result_code_ac, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AUTH RESULT CODE AC", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}


	else if((new_StrAUTHENTICATION_ATTEMPT_AC!=prev_StrAUTHENTICATION_ATTEMPT_AC) && change_AUTHENTICATION_ATTEMPT_AC == true)
	{
		change_AUTHENTICATION_ATTEMPT_AC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewAmex3ds2.authentication_attempt_ac , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldAmex3ds2.authentication_attempt_ac, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AUTH ATTEMPT RC", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

	else if((new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG) && change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG == true)
	{
		change_AUTHENTICATION_ATTEMPT_AC = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewAmex3ds2.authentication_attempt_verify_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldAmex3ds2.authentication_attempt_verify_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"AUTH ATTEMPT VERIFY FLAG", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}
		else if((new_StrCSC_VALIDATION_FLAG!=prev_StrCSC_VALIDATION_FLAG) && change_StrCSC_VALIDATION_FLAG == true)
	{
		change_StrCSC_VALIDATION_FLAG = false;
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
		memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR)sHistoryData.file_primary_key,"", sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR)sHistoryData.primary_key.unique_id, (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
		memcpy ((LPTSTR)sHistoryData.current_data,snewAmex3ds2.csc_validation_flag , sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.previous_data, soldAmex3ds2.csc_validation_flag, sizeof (sHistoryData.current_data));
   		memcpy ((LPTSTR)sHistoryData.changed_by,login_name, strlen(login_name));
		memcpy ((LPTSTR)sHistoryData.field_tag,"CSC VALIDATION FLAG", sizeof(sHistoryData.field_tag));
 		
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
	}

      }






void CAmex3DS2Parameter::PopulateControlStructure()
{

	if(new_StrSELF_VALIDATION_AC != prev_StrSELF_VALIDATION_AC)
	{
		change_SELF_VALIDATION_AC = true;
	}
	

	if(new_StrAUTHENTICATION_RESULT_CODE_AC!=prev_StrAUTHENTICATION_RESULT_CODE_AC)
	{
		change_AUTHENTICATION_RESULT_CODE_AC = true;

	}

	if(new_StrAUTHENTICATION_ATTEMPT_AC!=prev_StrAUTHENTICATION_ATTEMPT_AC)
	{
		change_AUTHENTICATION_ATTEMPT_AC = true;
	}

	if(new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG!=prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG)
	{
		change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG = true;
	}

	if(new_StrCSC_VALIDATION_FLAG!=prev_StrCSC_VALIDATION_FLAG)
	{
		change_StrCSC_VALIDATION_FLAG = true;
	}
}


void CAmex3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag()
{
	// TODO: Add your control notification handler code here
}


