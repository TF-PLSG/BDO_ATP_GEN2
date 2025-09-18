#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "BinList.h"
#include "Visa3DS2ConfigMain.h"
#include "Visa3DS2ConfigPropertySheet.h"
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
extern CString StrCAVV;
extern CString StrCAVVresponsecode;
extern CString StrCAVVdescription;


IMPLEMENT_DYNCREATE(CVisa3DS2ConfigMainPage,CPropertyPage)

CVisa3DS2ConfigMainPage::CVisa3DS2ConfigMainPage() : CPropertyPage(CVisa3DS2ConfigMainPage::IDD)
{
	m_StrCAVV = _T("");	
	m_StrCAVVresponsecode= _T("");
	m_StrCAVVdescription= _T("");
	m_StrMessageLbl= _T("");
}

CVisa3DS2ConfigMainPage::~CVisa3DS2ConfigMainPage()
{
	//delete pLock;
	//pLock = NULL;
}

void CVisa3DS2ConfigMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAVV, m_CntlCAVV);
	DDX_Text(pDX, IDC_CAVV, m_StrCAVV);
	DDV_MaxChars(pDX,m_StrCAVV,1);

	DDX_Control(pDX, IDC_CAVVDESCRIPTION_FLD, m_CntlCAVVdescription);
	DDX_Text(pDX, IDC_CAVVDESCRIPTION_FLD, m_StrCAVVdescription);
	DDV_MaxChars(pDX,m_StrCAVVdescription,40);

	DDX_Control(pDX, IDC_CAVVRESPONSECODE_FLD, m_CntlCAVVresponsecode);
	DDX_Text(pDX, IDC_CAVVRESPONSECODE_FLD, m_StrCAVVresponsecode);
	DDV_MaxChars(pDX,m_StrCAVVresponsecode,2);

	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);


}


BEGIN_MESSAGE_MAP(CVisa3DS2ConfigMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBinDefinition)
	ON_BN_CLICKED(IDC_ADD_CAVV_BUTTON, OnAddCAVVButton)
	ON_BN_CLICKED(IDC_DELETE_CAVV_BUTTON, OnDeleteCAVVButton)
	ON_BN_CLICKED(IDC_UPDATE_CAVV_BUTTON, OnUpdateCAVVButton)
	ON_BN_CLICKED(IDC_VIEW_CAVV_BUTTON, OnViewCAVVButton)
	ON_BN_CLICKED(IDC_LIST_CAVV_BUTTON, OnListCAVVButton)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_CAVV, &CVisa3DS2ConfigMainPage::OnEnChangeCavv)
END_MESSAGE_MAP()

BOOL CVisa3DS2ConfigMainPage::OnInitDialog() 
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

void CVisa3DS2ConfigMainPage::OnAddCAVVButton() 
{
	 VISA3DS2 svisa3ds2;
    char strErrorMessage [200]="";   
    m_StrMessageLbl = ""; 
	UpdateData(TRUE);
	m_nDialogMode = MODE_ADD;                                                                                       
    // check if network ID field is not blank
	if (!CheckDialogForCompleteness())
        return ;	

	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	ZeroMemory(&svisa3ds2,sizeof(VISA3DS2));

	memcpy ((LPSTR)(svisa3ds2.cavv), m_StrCAVV ,sizeof (svisa3ds2.cavv));
	memcpy ((LPSTR)(svisa3ds2.response_code), m_StrCAVVresponsecode ,sizeof (svisa3ds2.response_code));
	memcpy ((LPSTR)(svisa3ds2.description), m_StrCAVVdescription ,sizeof (svisa3ds2.description));
	
	int rcode = txdsapi_get_record((pBYTE)&svisa3ds2, sizeof (VISA3DS2), VISA3DS2_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Entered Rule already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }

	rcode = txdsapi_insert_record  ((pBYTE)&svisa3ds2, sizeof (VISA3DS2), VISA3DS2_DATA, strErrorMessage);
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
		memcpy ((LPSTR)m_strPreviousData, svisa3ds2.cavv, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "CAVV Result code", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
		PopulateHistoryTable();
		UpdateData( FALSE );
				
			
				
		return;
	}

	return ;

}

void CVisa3DS2ConfigMainPage::OnDeleteCAVVButton() 
{
		CString strMessage;
    int nReturnCode;
    VISA3DS2 svisa3ds2Record;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_DELETE;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete the record for CAVV value  %s?", (LPCTSTR) m_StrCAVV,(LPCTSTR) m_StrCAVVresponsecode) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	// try to delete
	memcpy ((LPTSTR)svisa3ds2Record.cavv,	m_StrCAVV,		sizeof (svisa3ds2Record.cavv)) ;
	memcpy ((LPTSTR)svisa3ds2Record.response_code,	m_StrCAVVresponsecode,		sizeof (svisa3ds2Record.response_code)) ;


    int rcode = txdsapi_delete_record ((pBYTE)&svisa3ds2Record, sizeof (VISA3DS2), VISA3DS2_DATA, strErrorMessage);
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: CAVV  record for %s and %s does not exist", m_StrCAVV ,m_StrCAVVresponsecode);
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {

		memcpy ((LPSTR)m_strPreviousData, svisa3ds2Record.cavv, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "CAVV Result Value", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();

		m_StrCAVV = _T("");	
		m_StrCAVVresponsecode = _T("");
		m_StrCAVVdescription = _T("");

        m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}


		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
	
		m_CntlCAVV.EnableWindow(TRUE);
		m_CntlCAVVresponsecode.EnableWindow(TRUE);
}

void CVisa3DS2ConfigMainPage::OnViewCAVVButton() 
{
		HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;
	

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "VISA3DS2", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_OMC01;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();

}

void CVisa3DS2ConfigMainPage::OnListCAVVButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrCAVV;
	listDlg.strItem1 = m_StrCAVVresponsecode;
	listDlg.strItem2 = m_StrCAVVdescription;
	listDlg.m_nDialogMode = MODE_VISA3DS2;

    nReturnCode = listDlg.DoModal();
	 if ( nReturnCode == IDOK)
	{
		m_StrCAVV = StrCAVV;
		m_StrCAVVresponsecode = StrCAVVresponsecode;
		m_StrCAVVdescription = StrCAVVdescription;

		m_CntlCAVV.EnableWindow(TRUE);
		m_CntlCAVVresponsecode.EnableWindow(TRUE);
		m_CntlCAVVdescription.EnableWindow(TRUE);

	}

	     if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

void CVisa3DS2ConfigMainPage::OnUpdateCAVVButton() 
{
	HST01 sHistoryRecord;
	char strErrorMessage[200];
	VISA3DS2 visa3ds2old;
	VISA3DS2 visa3ds2new;

   m_nDialogMode = MODE_EDIT;
   	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   	// get data from DB
   memcpy ((LPSTR)(visa3ds2old.cavv), m_StrCAVV, sizeof (visa3ds2old.cavv) );
   memcpy ((LPSTR)(visa3ds2old.response_code), m_StrCAVVresponsecode, sizeof (visa3ds2old.response_code) );   

	int rcode = txdsapi_get_record( (pBYTE)&visa3ds2old, sizeof (VISA3DS2), VISA3DS2_DATA, strErrorMessage);
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: CAVV  record for CAVV Value %s and Response code does not exist", m_StrCAVV,m_StrCAVVresponsecode );
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
	memcpy ( visa3ds2new.cavv,		m_StrCAVV,		sizeof ( visa3ds2new.cavv));
	memcpy ( visa3ds2new.response_code,			m_StrCAVVresponsecode,			sizeof (  visa3ds2new.response_code));
	memcpy ( visa3ds2new.description,		m_StrCAVVdescription,		sizeof (visa3ds2new.description));
	


	/* compare with old values and update the history table*/

	if (strcmp ( m_StrCAVV, (char *)visa3ds2old.cavv) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCAVV, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, visa3ds2old.cavv, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "CAVV Value", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrCAVVresponsecode, (char *)visa3ds2old.response_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCAVVresponsecode, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, visa3ds2old.response_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Response code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrCAVVdescription, (char *)visa3ds2old.description) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCAVVdescription, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, visa3ds2old.description, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Description", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

		 rcode = txdsapi_update_record ( (pBYTE)&visa3ds2new, sizeof (VISA3DS2), VISA3DS2_DATA, strErrorMessage);
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

BOOL CVisa3DS2ConfigMainPage::OnApply() 
{
	m_StrCAVV = _T("");	
	m_StrCAVVresponsecode= _T("");
	m_StrCAVVdescription= _T("");
	m_StrMessageLbl= _T("");
	UpdateData(FALSE);
	return true;
}

void CVisa3DS2ConfigMainPage::OnCancel() 
{
	m_StrCAVV = _T("");	
	m_StrCAVVresponsecode= _T("");
	m_StrCAVVdescription= _T("");
	m_StrMessageLbl= _T("");
		UpdateData(FALSE);
}

BOOL CVisa3DS2ConfigMainPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	


	return TRUE;
}

BOOL CVisa3DS2ConfigMainPage::OnSetActive() 
{
	((CVisa3DS2ConfigPropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CVisa3DS2ConfigMainPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CVisa3DS2ConfigMainPage::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
	    CHAR temp_connect_str[50];
	CHAR temp_name_str[50];
    int temp1=0;
	/**********************/
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	 if (myUtilsObject.IsEmpty (m_StrCAVV, this))
    {
		m_StrMessageLbl = "Please enter CAVV Value";	
		m_CntlCAVV.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	 if(m_nDialogMode != MODE_DELETE)
	 {
		  if (myUtilsObject.IsEmpty (m_StrCAVVresponsecode, this))
		{
			m_StrMessageLbl = "Please enter response code";	
			m_CntlCAVVresponsecode.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}

		 if (myUtilsObject.IsEmpty (m_StrCAVVdescription, this))
		{
			m_StrMessageLbl = "Please enter description";	
			m_CntlCAVVdescription.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
	 }

	return TRUE;

}

void CVisa3DS2ConfigMainPage::PopulateHistoryTable()
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

    
	strcpy (strPrimaryKey, m_StrCAVV);
	
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"VISA3DS2", sizeof (sHistoryData.file_name));
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

void CVisa3DS2ConfigMainPage::PopulateCAVVDialogFromDatabase(VISA3DS2 svisa3ds2)
{
	CString temp;
	CMyUtils myUtilsObject;
	int MGRP_len=0;

	if ( m_nDialogMode != MODE_VIEW )	
	{
		m_CntlCAVV.EnableWindow(TRUE);
		m_CntlCAVVresponsecode.EnableWindow(TRUE);
		m_CntlCAVVdescription .EnableWindow(TRUE);

	}

	m_StrCAVV	 = svisa3ds2.cavv;
	m_StrCAVVresponsecode	 = svisa3ds2.response_code;
	m_StrCAVVdescription  = svisa3ds2.description;


	UpdateData(FALSE);

	return;
}

void CVisa3DS2ConfigMainPage::OnOK() 
{
   OnApply();
	
   CDialog::OnOK();
}





void CVisa3DS2ConfigMainPage::OnEnChangeCavv()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
