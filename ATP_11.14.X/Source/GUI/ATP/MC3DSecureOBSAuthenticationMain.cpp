

#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "BinList.h"
#include "MC3DSecureOBSAuthenticationMain.h"
#include "MC3DSecureOBSAuthenticationSheet.h"
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
extern CString Stronbehalfresult;
extern CString Stronbehalfservice;
extern CString Strresponsecode;
extern CString Strdescription;

IMPLEMENT_DYNCREATE(CMC3DSecureOBSAuthenticationMainPage, CPropertyPage)

CMC3DSecureOBSAuthenticationMainPage::CMC3DSecureOBSAuthenticationMainPage() : CPropertyPage(CMC3DSecureOBSAuthenticationMainPage::IDD)
{
	m_Stronbehalfservice = _T("");	
	m_Stronbehalfresult = _T("");
	m_Strresponsecode= _T("");
	m_Strdescription= _T("");
	m_StrMessageLbl= _T("");
}

CMC3DSecureOBSAuthenticationMainPage::~CMC3DSecureOBSAuthenticationMainPage()
{
	//delete pLock;
	//pLock = NULL;
}

void CMC3DSecureOBSAuthenticationMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBS_FLD, m_Cntlonbehalfservice);
	DDX_Text(pDX, IDC_OBS_FLD, m_Stronbehalfservice);
	DDV_MaxChars(pDX,m_Stronbehalfservice,2);

	DDX_Control(pDX, IDC_OBSRESULT_FLD, m_Cntlonbehalfresult);
	DDX_Text(pDX, IDC_OBSRESULT_FLD, m_Stronbehalfresult);
	DDV_MaxChars(pDX,m_Stronbehalfresult,1);

	DDX_Control(pDX, IDC_OBSDESCRIPTION_FLD, m_Cntldescription);
	DDX_Text(pDX, IDC_OBSDESCRIPTION_FLD, m_Strdescription);
	DDV_MaxChars(pDX,m_Strdescription,40);

	DDX_Control(pDX, IDC_OBSRESULT_FLD2, m_Cntlresponsecode);
	DDX_Text(pDX, IDC_OBSRESULT_FLD2, m_Strresponsecode);
	DDV_MaxChars(pDX,m_Strresponsecode,2);

	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);


}

BEGIN_MESSAGE_MAP(CMC3DSecureOBSAuthenticationMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBinDefinition)
	ON_BN_CLICKED(IDC_ADD_OBS_BUTTON, OnAddOBSButton)
	ON_BN_CLICKED(IDC_DELETE_OBS_BUTTON, OnDeleteOBSButton)
	ON_BN_CLICKED(IDC_UPDATE_OBS_BUTTON, OnUpdateOBSButton)
	ON_BN_CLICKED(IDC_VIEW_OBS_BUTTON, OnViewOBSButton)
	ON_BN_CLICKED(IDC_LIST_OBS_BUTTON, OnListOBSButton)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CMC3DSecureOBSAuthenticationMainPage::OnInitDialog() 
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

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMC3DSecureOBSAuthenticationMainPage::OnAddOBSButton() 
{
	 OBS01 sobs01;
    char strErrorMessage [200]="";   
	BYTE num_of_records[5]= {0};
    m_StrMessageLbl = ""; 
	UpdateData(TRUE);
	m_nDialogMode = MODE_ADD;    
	int obs01_count = 0;
    // check if network ID field is not blank
	if (!CheckDialogForCompleteness())
        return ;	

	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	ZeroMemory(&sobs01,sizeof(OBS01));

	memcpy ((LPSTR)(sobs01.on_behalf_service), m_Stronbehalfservice ,sizeof (sobs01.on_behalf_service));
	memcpy ((LPSTR)(sobs01.on_behalf_result), m_Stronbehalfresult ,sizeof (sobs01.on_behalf_result));
	memcpy ((LPSTR)(sobs01.response_code), m_Strresponsecode ,sizeof (sobs01.response_code));
	memcpy ((LPSTR)(sobs01.description), m_Strdescription ,sizeof (sobs01.description));
	

	int rcode = txdsapi_get_num_records(num_of_records,OBS01_DATA,strErrorMessage);
	
	if(rcode == PTEMSG_OK )
	{
		obs01_count = atoi((char *)num_of_records);
		if (obs01_count < 20)
		{
			// Do nothing
		}
		else
		{
			m_StrMessageLbl = "ERROR: OBS rules reached its maximum count, user can add 20 records only";
			m_nDialogMode = MODE_DONE;
        
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
				return ;
		}
	
	}

	
	rcode = txdsapi_get_record((pBYTE)&sobs01, sizeof (OBS01), OBS01_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Enetered OBS rule already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }
	


	rcode = txdsapi_insert_record  ((pBYTE)&sobs01, sizeof (OBS01), OBS01_DATA,strErrorMessage);
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
		memcpy ((LPSTR)m_strPreviousData, sobs01.on_behalf_result, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "On behalf Result", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
		PopulateHistoryTable();
		UpdateData( FALSE );
				
		return;
	}

	return ;

}

void CMC3DSecureOBSAuthenticationMainPage::OnDeleteOBSButton() 
{
		CString strMessage;
    int nReturnCode;
    OBS01 sObs01Record;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_DELETE;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete OBS Service  %s,\n OBS result %s?", (LPCTSTR) m_Stronbehalfservice,(LPCTSTR) m_Stronbehalfresult) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	// try to delete
	memcpy ((LPTSTR)sObs01Record.on_behalf_service,	m_Stronbehalfservice,		sizeof (sObs01Record.on_behalf_service)) ;
	memcpy ((LPTSTR)sObs01Record.on_behalf_result,m_Stronbehalfresult, sizeof (sObs01Record.on_behalf_result)) ;
	memcpy ((LPTSTR)sObs01Record.response_code,m_Strresponsecode, sizeof (sObs01Record.response_code)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sObs01Record, sizeof ( OBS01), OBS01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: OBS  record for %s and %s does not exist", m_Stronbehalfservice ,m_Stronbehalfresult);
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
        memcpy ((LPSTR)m_strPreviousData, sObs01Record.on_behalf_result, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "On behalf Result", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();

		m_Stronbehalfservice = _T("");	
		m_Stronbehalfresult = _T("");
		m_Strresponsecode = _T("");
		m_Strdescription = _T("");
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
		
	
		m_Cntlonbehalfservice.EnableWindow(TRUE);
		m_Cntlonbehalfresult.EnableWindow(TRUE);
}

void CMC3DSecureOBSAuthenticationMainPage::OnViewOBSButton() 
{
		HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "OBS01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_OMC01;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

}


void CMC3DSecureOBSAuthenticationMainPage::OnListOBSButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_Stronbehalfservice;
	listDlg.strItem1 = m_Stronbehalfresult;
	listDlg.strItem2 = m_Strresponsecode;
	listDlg.strItem3 = m_Strdescription;
	listDlg.m_nDialogMode = MODE_OBS01;

    nReturnCode = listDlg.DoModal();
	 if ( nReturnCode == IDOK)
	{
		m_Stronbehalfservice = Stronbehalfservice;
		m_Stronbehalfresult = Stronbehalfresult;
		m_Strresponsecode = Strresponsecode;
		m_Strdescription = Strdescription;

		m_Cntlonbehalfservice.EnableWindow(TRUE);
		m_Cntlonbehalfresult.EnableWindow(TRUE);
		m_Cntlresponsecode.EnableWindow(TRUE);
		m_Cntldescription.EnableWindow(TRUE);
	
	}

	     if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

void CMC3DSecureOBSAuthenticationMainPage::OnUpdateOBSButton() 
{
	HST01 sHistoryRecord;
	char strErrorMessage[200];
	OBS01 obs01old;
	OBS01 obs01new;

   m_nDialogMode = MODE_EDIT;
   	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   	// get data from DB
   memcpy ((LPSTR)(obs01old.on_behalf_service), m_Stronbehalfservice, sizeof (obs01old.on_behalf_service) );
   memcpy ((LPSTR)(obs01old.on_behalf_result), m_Stronbehalfresult, sizeof (obs01old.on_behalf_result) );  
   memcpy ((LPSTR)(obs01old.response_code), m_Strresponsecode, sizeof (obs01old.response_code) );  
   memcpy ((LPSTR)(obs01old.description), m_Strdescription, sizeof (obs01old.description) );  
	int rcode = txdsapi_get_record( (pBYTE)&obs01old, sizeof (OBS01), OBS01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: OBS  record for OBS service %s and OBS result does not exist", m_Stronbehalfservice,m_Stronbehalfresult );
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
	memcpy ( obs01new.on_behalf_service,		m_Stronbehalfservice,		sizeof ( obs01new.on_behalf_service));
	memcpy ( obs01new.on_behalf_result,		m_Stronbehalfresult,		sizeof (obs01new.on_behalf_result));
	memcpy ( obs01new.response_code,			m_Strresponsecode,			sizeof (  obs01new.response_code));
	memcpy ( obs01new.description,		m_Strdescription,		sizeof (obs01new.description));
	


	/* compare with old values and update the history table*/

	if (strcmp ( m_Stronbehalfservice, (char *)obs01old.on_behalf_service) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Stronbehalfservice, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, obs01old.on_behalf_service, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "On Behalf Service", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_Stronbehalfresult, (char *)obs01old.on_behalf_result) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Stronbehalfresult, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, obs01old.on_behalf_result, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "On Behalf Result", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_Strresponsecode, (char *)obs01old.response_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Strresponsecode, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, obs01old.response_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Response code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_Strdescription, (char *)obs01old.description) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_Strdescription, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, obs01old.description, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Description", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

		 rcode = txdsapi_update_record ( (pBYTE)&obs01new, sizeof (OBS01), OBS01_DATA, strErrorMessage );
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
BOOL CMC3DSecureOBSAuthenticationMainPage::OnApply() 
{
	m_Stronbehalfservice = _T("");	
	m_Stronbehalfresult = _T("");
	m_Strresponsecode= _T("");
	m_Strdescription= _T("");
	m_StrMessageLbl= _T("");
	UpdateData(FALSE);
	return true;

}

void CMC3DSecureOBSAuthenticationMainPage::OnCancel() 
{
	m_Stronbehalfservice = _T("");	
	m_Stronbehalfresult = _T("");
	m_Strresponsecode= _T("");
	m_Strdescription= _T("");
	m_StrMessageLbl= _T("");
		UpdateData(FALSE);
}

BOOL CMC3DSecureOBSAuthenticationMainPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	


	return TRUE;
}

BOOL CMC3DSecureOBSAuthenticationMainPage::OnSetActive() 
{
	((CMC3DSecureOBSAuthenticationSheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CMC3DSecureOBSAuthenticationMainPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CMC3DSecureOBSAuthenticationMainPage::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	    CHAR temp_connect_str[50];
	CHAR temp_name_str[50];
    int temp1=0;
	/**********************/
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	 if (myUtilsObject.IsEmpty (m_Stronbehalfservice, this))
    {
		m_StrMessageLbl = "Please enter On Behalf Service details";	
		m_Cntlonbehalfservice.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	 if (myUtilsObject.IsEmpty (m_Stronbehalfresult, this))
    {
		m_StrMessageLbl = "Please enter On Behalf Result details";	
		m_Cntlonbehalfresult.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	 if(m_nDialogMode != MODE_DELETE)
	 {
		  if (myUtilsObject.IsEmpty (m_Strresponsecode, this))
		{
			m_StrMessageLbl = "Please enter response code details";	
			m_Cntlresponsecode.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}

		 if (myUtilsObject.IsEmpty (m_Strdescription, this))
		{
			m_StrMessageLbl = "Please enter description";	
			m_Cntldescription.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
	 }

	return TRUE;

}

void CMC3DSecureOBSAuthenticationMainPage::PopulateHistoryTable()
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
		strcpy ( strPrimaryKey, m_Stronbehalfservice);
		strcat ( strPrimaryKey, m_Stronbehalfresult );
	}
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"OBS01", sizeof (sHistoryData.file_name));
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

void CMC3DSecureOBSAuthenticationMainPage::PopulateOBSDialogFromDatabase(OBS01 sobs01)
{
	CString temp;
	CMyUtils myUtilsObject;
	int MGRP_len=0;

	if ( m_nDialogMode != MODE_VIEW )	
	{
		m_Cntlonbehalfservice.EnableWindow(TRUE);
		m_Cntlonbehalfresult.EnableWindow(TRUE);
		m_Cntlresponsecode.EnableWindow(TRUE);
		m_Cntldescription.EnableWindow(TRUE);
	}

	m_Stronbehalfservice	 = sobs01.on_behalf_service;
	m_Stronbehalfresult	 = sobs01.on_behalf_result;
	m_Strresponsecode		 = sobs01.response_code;
	m_Strdescription		 = sobs01.description;

	UpdateData(FALSE);

	return;
}