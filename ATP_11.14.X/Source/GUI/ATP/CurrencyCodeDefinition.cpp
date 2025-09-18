
// CurrencyCodeDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "BinList.h"
#include "CurrencyCodeDefinition.h"
#include "CurrencyCodePropertySheet.h"


#include "MyUtils.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
//#include "Login.h"

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;

extern CString  StrCurrency_Value;
extern CString  StrCurrency_code;
extern CString	StrDecimal_places;
extern CString	StrCurrency_Name;

//extern CLogin login;
/////////////////////////////////////////////////////////////////////////////
// CRelatedDefinition property page

IMPLEMENT_DYNCREATE(CurrencyCodeDefinition, CPropertyPage)

CurrencyCodeDefinition::CurrencyCodeDefinition() : CPropertyPage(CurrencyCodeDefinition::IDD)
{
	//{{AFX_DATA_INIT(CRelatedDefinition)
	m_StrCurrencyValueFld = _T("");
	m_StrCurrencyCodeFld =  _T("");
	m_StrDecimalPlacesFld =  _T("");
	m_StrCurrencyNameFld =  _T("");
		
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CurrencyCodeDefinition::~CurrencyCodeDefinition()
{
}

void CurrencyCodeDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelatedDefinition)
	DDX_Control(pDX, IDC_LIST_CC_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_VIEW_CC_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_CC_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_DELETE_CC_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_CC_BUTTON, m_AddButton);

	DDX_Control(pDX, IDC_CURR_VALUE_FLD, m_CntlCurrencyValueFld);
	DDX_Text(pDX, IDC_CURR_VALUE_FLD, m_StrCurrencyValueFld);
	DDV_MaxChars(pDX, m_StrCurrencyValueFld, 3);
	
	DDX_Control(pDX, IDC_CURR_CODE_FLD, m_CntlCurrencyCodeFld);
	DDX_Text(pDX, IDC_CURR_CODE_FLD, m_StrCurrencyCodeFld);
	DDV_MaxChars(pDX, m_StrCurrencyCodeFld, 3);
	
	DDX_Control(pDX, IDC_CURR_DEC_PLACES_FLD, m_CntlDecPlacesFld);
	DDX_Text(pDX, IDC_CURR_DEC_PLACES_FLD, m_StrDecimalPlacesFld);
	DDV_MaxChars(pDX, m_StrDecimalPlacesFld, 1);
	
	DDX_Control(pDX, IDC_CURRENCY_NAME_FLD, m_CntlCurrencyNameFld);
	DDX_Text(pDX, IDC_CURRENCY_NAME_FLD, m_StrCurrencyNameFld);
	DDV_MaxChars(pDX, m_StrCurrencyNameFld, 14);	



	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CurrencyCodeDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CRelatedDefinition)
	ON_BN_CLICKED(IDC_ADD_CC_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_CC_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_UPDATE_CC_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_CC_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_LIST_CC_BUTTON, OnListButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelatedDefinition message handlers

BOOL CurrencyCodeDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex= -1;

	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrCurrencyValueFld = _T("");
	m_StrCurrencyCodeFld = _T("");
	m_StrDecimalPlacesFld = _T("");
	m_StrCurrencyNameFld = _T("");
		
	m_StrMessageLbl = _T("");

	// set access rights according UCF01 table
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}
	else 
	{
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
		m_ViewButton.EnableWindow(FALSE);
		m_UpdateButton.EnableWindow(FALSE);
		m_ListButton.EnableWindow(FALSE);
	}


/*	cntr = 0;
   
    timer_handle = StartTimer(1000);	   

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }
*/
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CurrencyCodeDefinition::OnAddButton() 
{
	CCL01 sCCLRecord;
	char strDate[9] = "";
    
	char strErrorMessage[200];
    
    CMyUtils myUtilsObject;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	if (myUtilsObject.IsEmpty (m_StrCurrencyValueFld, this))
    {
		m_StrMessageLbl = "Please enter Currency value";	
		m_CntlCurrencyValueFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return ;
	}

	if (myUtilsObject.IsEmpty (m_StrCurrencyCodeFld, this))
    {
		m_StrMessageLbl = "Please enter Currency code";	
		m_CntlCurrencyCodeFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return ;
	}

	if (myUtilsObject.IsEmpty (m_StrDecimalPlacesFld, this))
    {
		m_StrMessageLbl = "Please enter Decimal place value";	
		m_CntlDecPlacesFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return ;
	}

	if (myUtilsObject.IsEmpty (m_StrCurrencyNameFld, this))
    {
		m_StrMessageLbl = "Please enter Currency name";	
		m_CntlCurrencyNameFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return ;
	}

	// set m_nDialogMode for all pages to MODE_ADD
	 
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;  

	// check if Merchant ID already exists

	ZeroMemory(&sCCLRecord,sizeof(CCL01)) ;

	CString strType;

    memcpy ((LPSTR)(sCCLRecord.currency_value), m_StrCurrencyValueFld , sizeof (sCCLRecord.currency_value)); 
	memcpy ((LPSTR)(sCCLRecord.currency_code), m_StrCurrencyCodeFld , sizeof (sCCLRecord.currency_code));  
	memcpy ((LPSTR)(sCCLRecord.currency_decimal_places), m_StrDecimalPlacesFld , sizeof (sCCLRecord.currency_decimal_places));  
	memcpy ((LPSTR)(sCCLRecord.currency_name), m_StrCurrencyNameFld , sizeof (sCCLRecord.currency_name));   

	int rcode = txdsapi_get_record( (pBYTE)&sCCLRecord, sizeof (CCL01), CCL01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
      m_StrMessageLbl = "ERROR: Currency value already exists";
      m_nDialogMode = MODE_DONE;
	  m_CntlCurrencyValueFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	rcode = txdsapi_insert_record  ((pBYTE)&sCCLRecord, sizeof (CCL01), CCL01_DATA,strErrorMessage);
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
		memcpy ((LPSTR)m_strPreviousData, sCCLRecord.currency_value, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency value", sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
		PopulateHistoryTable();
		UpdateData( FALSE );
				
		return;
	}

	return ;

}

void CurrencyCodeDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    CCL01 sCCLRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Currency value details %s?", (LPCTSTR) m_StrCurrencyValueFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	CString strType;

	memcpy ((LPTSTR)sCCLRecord.currency_value,   m_StrCurrencyValueFld, sizeof (sCCLRecord.currency_value)) ;
	memcpy ((LPTSTR)sCCLRecord.currency_code,   m_StrCurrencyCodeFld, sizeof (sCCLRecord.currency_code)) ;
	memcpy ((LPTSTR)sCCLRecord.currency_decimal_places,   m_StrDecimalPlacesFld, sizeof (sCCLRecord.currency_decimal_places)) ;
	memcpy ((LPTSTR)sCCLRecord.currency_name,   m_StrCurrencyNameFld, sizeof (sCCLRecord.currency_name)) ;


    int rcode = txdsapi_delete_record ((pBYTE)&sCCLRecord, sizeof (CCL01), CCL01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Currency code %s does not exist", m_StrCurrencyValueFld );
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
	  memcpy ((LPSTR)m_strPreviousData, sCCLRecord.currency_value, sizeof (m_strPreviousData));
      memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

      PopulateHistoryTable();

		m_StrCurrencyValueFld = _T("");
		m_StrCurrencyCodeFld = _T("");
		m_StrDecimalPlacesFld =_T("");
		m_StrCurrencyNameFld =_T("");
      m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}


		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
		m_CntlCurrencyValueFld.EnableWindow(TRUE);
		m_CntlCurrencyCodeFld.EnableWindow(TRUE);
		m_CntlDecPlacesFld.EnableWindow(TRUE);
		m_CntlCurrencyNameFld.EnableWindow(TRUE);
}

void CurrencyCodeDefinition::OnUpdateButton() 
{
	HST01 sHistoryRecord;
	char strErrorMessage[200];
	CCL01 ccl01old;
	CCL01 ccl01new;
	char strAuthneticationflag[2];


   m_nDialogMode = MODE_EDIT;
   	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   	// get data from DB
   ZeroMemory ( &ccl01old, sizeof (JCB3DS2));

   memcpy ((LPSTR)(ccl01old.currency_value), m_StrCurrencyValueFld, sizeof (ccl01old.currency_value) );
   memcpy ((LPSTR)(ccl01old.currency_code), m_StrCurrencyCodeFld, sizeof (ccl01old.currency_code) );  
   memcpy ((LPSTR)(ccl01old.currency_decimal_places), m_StrDecimalPlacesFld ,sizeof (ccl01old.currency_decimal_places));
   memcpy ((LPSTR)(ccl01old.currency_name), m_StrCurrencyNameFld ,sizeof (ccl01old.currency_name));
  
	int rcode = txdsapi_get_record( (pBYTE)&ccl01old, sizeof (CCL01), CCL01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Currency value %s does not exist",m_StrCurrencyValueFld);
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
   memcpy ((LPSTR)(ccl01new.currency_value), m_StrCurrencyValueFld, sizeof (ccl01new.currency_value) );
   memcpy ((LPSTR)(ccl01new.currency_code), m_StrCurrencyCodeFld, sizeof (ccl01new.currency_code) );  
   memcpy ((LPSTR)(ccl01new.currency_decimal_places), m_StrDecimalPlacesFld ,sizeof (ccl01new.currency_decimal_places));
   memcpy ((LPSTR)(ccl01new.currency_name), m_StrCurrencyNameFld ,sizeof (ccl01new.currency_name));



	/* compare with old values and update the history table*/

	if (strcmp ( m_StrCurrencyValueFld, (char *)ccl01old.currency_value) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCurrencyValueFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, (char *)ccl01old.currency_value, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency value", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrCurrencyCodeFld, (char *)ccl01old.currency_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCurrencyCodeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, (char *)ccl01old.currency_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency Code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrDecimalPlacesFld, (char *)ccl01old.currency_decimal_places) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrDecimalPlacesFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, (char *)ccl01old.currency_decimal_places, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency Decimal places", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

		if (strcmp ( m_StrCurrencyNameFld, (char *)ccl01old.currency_name) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCurrencyNameFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, (char *)ccl01old.currency_name, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency Name", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}


		 rcode = txdsapi_update_record ( (pBYTE)&ccl01new, sizeof (CCL01), CCL01_DATA, strErrorMessage );
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

void CurrencyCodeDefinition::OnViewButton() 
{
	HST01 sHistoryRecord;
	BYTE rcode;
	char buf[100]={0};
	char strErrorMessage[200]={0};
	m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;

	 ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "CCL01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), "", sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	pChangeHistoryPage->m_nDialogMode = MODE_OMC01;
	pChangeHistoryPage->bNext = FALSE;	
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	UpdateData( FALSE );
	pChangeHistoryPage->PopulateHistoryDialog();
}

void CurrencyCodeDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrCurrencyValueFld;
	listDlg.strItem1 = m_StrCurrencyCodeFld;
	listDlg.strItem2 = m_StrDecimalPlacesFld;
	listDlg.strItem3 = m_StrCurrencyNameFld;
	listDlg.m_nDialogMode = MODE_CURRENCY_CODE_LIST;

    nReturnCode = listDlg.DoModal();
	 if ( nReturnCode == IDOK)
	{
		m_StrCurrencyValueFld = StrCurrency_Value;
		m_StrCurrencyCodeFld = StrCurrency_code;
		m_StrDecimalPlacesFld= StrDecimal_places;
		m_StrCurrencyNameFld = StrCurrency_Name;


		m_CntlCurrencyValueFld.EnableWindow(FALSE);
		m_CntlCurrencyCodeFld.EnableWindow(TRUE);
		m_CntlDecPlacesFld.EnableWindow(TRUE);
		m_CntlCurrencyNameFld.EnableWindow(TRUE);
	
	}

	     if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

void CurrencyCodeDefinition::PopulateHistoryTable()
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
		strcpy ( strPrimaryKey, m_StrCurrencyValueFld);
	}
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CCL01", sizeof (sHistoryData.file_name));
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

BOOL CurrencyCodeDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrCurrencyValueFld, this))
    {
		m_StrMessageLbl = "Please enter Currency value";	
		m_CntlCurrencyValueFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
    return TRUE;

}   /* function CheckDialogForCompleteness */

/*BOOL CurrencyCodeDefinition::PopulateCurrencyCodeDatabaseStructureFromDialog ( pCCL01 psCCLRecord )
{
	CString strType;
	ZeroMemory (psCCLRecord, sizeof(CCL01)) ;	
    
	memcpy ((LPTSTR) psCCLRecord->currency_value,m_StrCurrencyValueFld, sizeof (psCCLRecord->currency_value));

	if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( psCCLRecord ))
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		return FALSE;
	}

	return TRUE;
}  */
/* function PopulateRelatedDatabaseStructureFromDialog */

void CurrencyCodeDefinition::PopulateCurrencyCodeDialogFromDatabase( CCL01 sCCLRecord )
{
	CString temp;
	CMyUtils myUtilsObject;
	int MGRP_len=0;

	if ( m_nDialogMode != MODE_VIEW )	
	{
		m_CntlCurrencyValueFld.EnableWindow(TRUE);
		m_CntlCurrencyCodeFld.EnableWindow(TRUE);
		m_CntlDecPlacesFld.EnableWindow(TRUE);
		m_CntlCurrencyNameFld.EnableWindow(TRUE);
	}

	m_StrCurrencyValueFld	 = sCCLRecord.currency_value;
	m_StrCurrencyCodeFld	 = sCCLRecord.currency_code;
	m_StrDecimalPlacesFld	 = sCCLRecord.currency_decimal_places;
	m_StrCurrencyNameFld	 = sCCLRecord.currency_name;

	UpdateData(FALSE);

	return;

}  /* function PopulateRelatedDialogFromDatabase  */

BOOL CurrencyCodeDefinition::OnApply() 
{
	m_StrCurrencyValueFld = _T("");	
	m_StrCurrencyCodeFld = _T("");
	m_StrDecimalPlacesFld =_T("");
	m_StrCurrencyNameFld =_T("");
	m_StrMessageLbl= _T("");
	UpdateData(FALSE);
	return true;
}

void CurrencyCodeDefinition::OnCancel() 
{
	m_StrCurrencyValueFld = _T("");	
	m_StrCurrencyCodeFld = _T("");
	m_StrDecimalPlacesFld =_T("");
	m_StrCurrencyNameFld =_T("");
	m_StrMessageLbl= _T("");
	UpdateData(FALSE);

} /* function OnCancel */

BOOL CurrencyCodeDefinition::OnSetActive() 
{
	((CurrencyCodePropertySheet*) this->GetParent())->m_nActiveTab = 0;

	if (m_nDialogMode == MODE_EDIT)
	{
		m_CntlCurrencyValueFld.EnableWindow(FALSE);
		m_CntlCurrencyCodeFld.EnableWindow(TRUE);
		m_CntlDecPlacesFld.EnableWindow(TRUE);
		m_CntlCurrencyNameFld.EnableWindow(TRUE);
	}
	
	return CPropertyPage::OnSetActive();
}

BOOL CurrencyCodeDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}
