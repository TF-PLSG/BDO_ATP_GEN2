

#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "BinList.h"
#include "Jcb3DS2ParameterDefinition.h"
#include "Jcb3DS2PropertySheet.h"
#include "MyUtils.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
#include "PteTime.h"
}



extern UCF01 ucf01;
extern CSharedMem shrmem;
extern CString Str3DS_Authentiction_RC_Value;
extern CString Str3DS_Authentiation_Deline_Response_Code;
extern CString Str3DS_AUTHENTICATION_VERIFY_FLAG;

IMPLEMENT_DYNCREATE(CJcb3DS2Parameter, CPropertyPage)

CJcb3DS2Parameter::CJcb3DS2Parameter() : CPropertyPage(CJcb3DS2Parameter::IDD)
{
	m_Str3DS_Authentiction_RC_Value = _T("");	
	m_Str3DS_Authentication_Decline_Response_Code = _T("");
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG =_T("");
	m_StrMessageLbl= _T("");
}

CJcb3DS2Parameter::~CJcb3DS2Parameter()
{
	//delete pLock;
	//pLock = NULL;
}

void CJcb3DS2Parameter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_3DS2_AUTH_RESULT_CODE_VALUE_FLD, m_Cntl3DS_Authentiction_RC_Value);
	DDX_Text(pDX, IDC_3DS2_AUTH_RESULT_CODE_VALUE_FLD, m_Str3DS_Authentiction_RC_Value);
	DDV_MaxChars(pDX,m_Str3DS_Authentiction_RC_Value,2);

	DDX_Control(pDX, IDC_3DS2_AUTH_RESULT_CODE_RC_FLD, m_Cntl3DS_Authentication_Decline_Response_Code);
	DDX_Text(pDX, IDC_3DS2_AUTH_RESULT_CODE_RC_FLD, m_Str3DS_Authentication_Decline_Response_Code);
	DDV_MaxChars(pDX,m_Str3DS_Authentication_Decline_Response_Code,2);

	DDX_Control(pDX, IDC_3DS_AUTHENTICATION_VERIFY_FLAG, m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG);
	DDX_CBString(pDX, IDC_3DS_AUTHENTICATION_VERIFY_FLAG, m_Str3DS_AUTHENTICATION_VERIFY_FLAG);

	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);


}

BEGIN_MESSAGE_MAP(CJcb3DS2Parameter, CPropertyPage)
	//{{AFX_MSG_MAP(CBinDefinition)
	ON_BN_CLICKED(IDC_ADD_3DS2_BUTTON, OnAdd3DS2Button)
	ON_BN_CLICKED(IDC_DELETE_3DS2_BUTTON, OnDelete3DS2Button)
	ON_BN_CLICKED(IDC_UPDATE_3DS2_BUTTON, OnUpdate3DS2Button)
	ON_BN_CLICKED(IDC_VIEW_3DS2_BUTTON, OnView3DS2Button)
	ON_BN_CLICKED(IDC_LIST_3DS2_BUTTON, OnList3DS2Button)
	//ON_CBN_SELCHANGE(IDC_3DS_AUTHENTICATION_VERIFY_FLAG, &CJcb3DS2Parameter::OnCbnSelchangeAuthenticationAttemptVerifyFlag)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CJcb3DS2Parameter::OnInitDialog() 
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
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG = "APPROVE";

	if (m_nDialogMode == MODE_ADD )
    {		

		m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG.SetCurSel(0);

    }

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJcb3DS2Parameter::OnAdd3DS2Button() 
{
	JCB3DS2 sjcb3ds2;
    char strErrorMessage [200]="";   
	BYTE num_of_records[5]= {0};
    m_StrMessageLbl = ""; 
	UpdateData(TRUE);
	m_nDialogMode = MODE_ADD;    
	char strAuthneticationflag[2];
	int jcb3ds2_count = 0;

    // check if network ID field is not blank
	if (!CheckDialogForCompleteness())
        return ;	

	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	ZeroMemory(&sjcb3ds2,sizeof(JCB3DS2));

	GetValidationFlag();
	memcpy ((LPSTR)(sjcb3ds2.auth_result_code_value), m_Str3DS_Authentiction_RC_Value ,sizeof (sjcb3ds2.auth_result_code_value));
	memcpy ((LPSTR)(sjcb3ds2.response_code), m_Str3DS_Authentication_Decline_Response_Code ,sizeof (sjcb3ds2.response_code));
	memcpy ((LPSTR)(sjcb3ds2.validation_flag), strFlag ,sizeof (sjcb3ds2.validation_flag));

	int	rcode = txdsapi_get_record((pBYTE)&sjcb3ds2, sizeof (JCB3DS2), JCB3DS2_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Entered Value already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }
	

	rcode = txdsapi_insert_record  ((pBYTE)&sjcb3ds2, sizeof (JCB3DS2), JCB3DS2_DATA,strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData( FALSE );
		return ;
	}   
	else
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
		m_StrMessageLbl = "Record added";
		memcpy ((LPSTR)m_strPreviousData, sjcb3ds2.auth_result_code_value, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auth Result Code value", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
		PopulateHistoryTable();
		UpdateData( FALSE );
				
		return;
	}

	return ;

}

void CJcb3DS2Parameter::OnDelete3DS2Button() 
{
		CString strMessage;
    int nReturnCode;
    JCB3DS2 sJcb3ds2Record;
	char strErrorMessage[200];
	char strAuthneticationflag[2];
    
     m_nDialogMode = MODE_DELETE;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Authentication result code value %s?", (LPCTSTR) m_Str3DS_Authentiction_RC_Value) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	// try to delete
	GetValidationFlag();
	memcpy ((LPSTR)(sJcb3ds2Record.auth_result_code_value), m_Str3DS_Authentiction_RC_Value ,sizeof (sJcb3ds2Record.auth_result_code_value));
	memcpy ((LPSTR)(sJcb3ds2Record.response_code), m_Str3DS_Authentication_Decline_Response_Code ,sizeof (sJcb3ds2Record.response_code));

	memcpy ((LPSTR)(sJcb3ds2Record.validation_flag), strFlag ,sizeof (sJcb3ds2Record.validation_flag));

    int rcode = txdsapi_delete_record ((pBYTE)&sJcb3ds2Record, sizeof ( JCB3DS2), JCB3DS2_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Authentication result code value %s does not exist",m_Str3DS_Authentiction_RC_Value);
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
        memcpy ((LPSTR)m_strPreviousData, sJcb3ds2Record.auth_result_code_value, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auth Result Code value", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();

	m_Str3DS_Authentiction_RC_Value = _T("");	
	m_Str3DS_Authentication_Decline_Response_Code = _T("");
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG =_T("");
        m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}


		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
	
		m_Cntl3DS_Authentiction_RC_Value.EnableWindow(TRUE);
		m_Cntl3DS_Authentication_Decline_Response_Code.EnableWindow(TRUE);
			m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG.EnableWindow(TRUE);
}

void CJcb3DS2Parameter::OnView3DS2Button() 
{
		HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	char strAuthneticationflag[2];

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "JCB3DS2", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_OMC01;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

}


void CJcb3DS2Parameter::OnUpdate3DS2Button() 
{
	HST01 sHistoryRecord;
	char strErrorMessage[200];
	JCB3DS2 jcb3ds2old;
	JCB3DS2 jcb3ds2new;
	char strAuthneticationflag[2];


   m_nDialogMode = MODE_EDIT;
   	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   	// get data from DB
   ZeroMemory ( &jcb3ds2old, sizeof (JCB3DS2));
   GetValidationFlag();
   memcpy ((LPSTR)(jcb3ds2old.auth_result_code_value), m_Str3DS_Authentiction_RC_Value, sizeof (jcb3ds2old.auth_result_code_value) );
   memcpy ((LPSTR)(jcb3ds2old.response_code), m_Str3DS_Authentication_Decline_Response_Code, sizeof (jcb3ds2old.response_code) );  
	memcpy ((LPSTR)(jcb3ds2old.validation_flag), strFlag ,sizeof (jcb3ds2old.validation_flag));
  
	int rcode = txdsapi_get_record( (pBYTE)&jcb3ds2old, sizeof (JCB3DS2), JCB3DS2_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Authentication result code value %s does not exist",m_Str3DS_Authentiction_RC_Value);
            break;
         }

       // all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->PopulateHistoryDialog();


	/* Populate with new values*/
	memcpy ( jcb3ds2new.auth_result_code_value,		m_Str3DS_Authentiction_RC_Value,		sizeof ( jcb3ds2new.auth_result_code_value));
	memcpy ( jcb3ds2new.response_code,		m_Str3DS_Authentication_Decline_Response_Code,		sizeof (jcb3ds2new.response_code));

	if ( m_Str3DS_AUTHENTICATION_VERIFY_FLAG == "APPROVE")
			memcpy ((LPTSTR) strFlag,	"A", sizeof(strFlag)); 
			else if ( m_Str3DS_AUTHENTICATION_VERIFY_FLAG == "DECLINE" )
			memcpy ((LPTSTR) strFlag,	"D", sizeof(strFlag)); 

	memcpy ( jcb3ds2new.validation_flag,strFlag,sizeof (  jcb3ds2new.validation_flag));

	


	/* compare with old values and update the history table*/

	if (strcmp ( m_Str3DS_Authentiction_RC_Value, (char *)jcb3ds2old.auth_result_code_value) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Str3DS_Authentiction_RC_Value, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, jcb3ds2old.auth_result_code_value, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Authn result code value", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_Str3DS_Authentication_Decline_Response_Code, (char *)jcb3ds2old.response_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Str3DS_Authentication_Decline_Response_Code, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, jcb3ds2old.response_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Decline Response code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_Str3DS_AUTHENTICATION_VERIFY_FLAG, (char *)jcb3ds2old.validation_flag) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Str3DS_AUTHENTICATION_VERIFY_FLAG, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, jcb3ds2old.validation_flag, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "verify flag", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}


		 rcode = txdsapi_update_record ( (pBYTE)&jcb3ds2new, sizeof (JCB3DS2), JCB3DS2_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
		else
		{
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

		}

}
BOOL CJcb3DS2Parameter::OnApply() 
{
	m_Str3DS_Authentiction_RC_Value = _T("");	
	m_Str3DS_Authentication_Decline_Response_Code = _T("");
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG =_T("");
	m_StrMessageLbl= _T("");
	UpdateData(FALSE);
	return true;

}

void CJcb3DS2Parameter::OnCancel() 
{
	m_Str3DS_Authentiction_RC_Value = _T("");	
	m_Str3DS_Authentication_Decline_Response_Code = _T("");
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG =_T("");
	m_StrMessageLbl= _T("");
		UpdateData(FALSE);
}

BOOL CJcb3DS2Parameter::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	


	return TRUE;
}

BOOL CJcb3DS2Parameter::OnSetActive() 
{
	((CJcb3DS2PropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CJcb3DS2Parameter::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CJcb3DS2Parameter::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	    CHAR temp_connect_str[50];
	CHAR temp_name_str[50];
    int temp1=0;
	/**********************/
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	 if (myUtilsObject.IsEmpty (m_Str3DS_Authentiction_RC_Value, this))
    {
		m_StrMessageLbl = "Please enter Authentication result code";	
		m_Cntl3DS_Authentiction_RC_Value.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}


	return TRUE;

}

void CJcb3DS2Parameter::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    
	if(m_nDialogMode ==MODE_ADD)
	{
		strcpy ( strPrimaryKey, m_Str3DS_Authentiction_RC_Value);
	}
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"JCB3DS2", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}
    
}

void CJcb3DS2Parameter::PopulateJCB3ds2DialogFromDatabase(JCB3DS2 sjcb3ds2)
{
	CString temp;
	CMyUtils myUtilsObject;
	int MGRP_len=0;

	if ( m_nDialogMode != MODE_VIEW )	
	{
		m_Cntl3DS_Authentiction_RC_Value.EnableWindow(TRUE);
		m_Cntl3DS_Authentication_Decline_Response_Code.EnableWindow(TRUE);
		m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG.SetCurSel(1);
	}

	m_Str3DS_Authentiction_RC_Value	 = sjcb3ds2.auth_result_code_value;
	m_Str3DS_Authentication_Decline_Response_Code	 = sjcb3ds2.auth_result_code_value;
	m_Str3DS_AUTHENTICATION_VERIFY_FLAG		 = sjcb3ds2.validation_flag;


	UpdateData(FALSE);

	return;
}

void CJcb3DS2Parameter::OnList3DS2Button() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_Str3DS_Authentiction_RC_Value;
	listDlg.strItem1 = m_Str3DS_Authentication_Decline_Response_Code;
	listDlg.strItem2 = m_Str3DS_AUTHENTICATION_VERIFY_FLAG;
	listDlg.m_nDialogMode = MODE_JCB3DS2;

    nReturnCode = listDlg.DoModal();
	 if ( nReturnCode == IDOK)
	{
		m_Str3DS_Authentiction_RC_Value = Str3DS_Authentiction_RC_Value;
		m_Str3DS_Authentication_Decline_Response_Code = Str3DS_Authentiation_Deline_Response_Code;
		m_Str3DS_AUTHENTICATION_VERIFY_FLAG= Str3DS_AUTHENTICATION_VERIFY_FLAG;

		m_Cntl3DS_Authentiction_RC_Value.EnableWindow(TRUE);
		m_Cntl3DS_Authentication_Decline_Response_Code.EnableWindow(TRUE);
		m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG.EnableWindow(TRUE);
	
	}

	     if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

BOOL CJcb3DS2Parameter::GetValidationFlag()
{
	if ( strcmp (m_Str3DS_AUTHENTICATION_VERIFY_FLAG, "APPROVE") == 0)
         memcpy ((LPTSTR) strFlag,	"A", sizeof(strFlag)); 
	if ( strcmp (m_Str3DS_AUTHENTICATION_VERIFY_FLAG, "DECLINE")== 0)
         memcpy ((LPTSTR) strFlag,	"D", sizeof(strFlag)); 

	
	return TRUE;
}
