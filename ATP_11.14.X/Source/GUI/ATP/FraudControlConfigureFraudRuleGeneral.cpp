

#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "FraudControlConfigureFraudRule.h"
#include "FraudControlConfigureFraudRuleGeneral.h"
#include "FraudControlFraudRuleSheet.h"
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
extern char frule[20];

IMPLEMENT_DYNAMIC(CFraudControlConfigurationFraudRule, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CFraudControlConfigurationFraudRule::CFraudControlConfigurationFraudRule()
	: CPropertyPage(CFraudControlConfigurationFraudRule::IDD)	
{
	m_StrCard_brand          = _T("");
	m_StrAcq_country_code    = _T("");
	m_StrBilling_amt         = _T("");
	m_StrPos_mode_1          = _T("");
	m_StrPos_mode_2          = _T("");
	m_StrPos_mode_3          = _T("");
	m_StrMcc_1               = _T("");
	m_StrMcc_2               = _T("");
	m_StrMcc_3               = _T("");
	
}

CFraudControlConfigurationFraudRule::~CFraudControlConfigurationFraudRule()
{
}
void CFraudControlConfigurationFraudRule::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CFraudControlConfigurationFraudRule)
	             
	DDX_Control(pDX, IDC_CARD_BRAND       , m_CntlCard_brand);
	DDX_Control(pDX, IDC_ACQ_COUNTRY_CODE , m_CntlAcq_country_code);
	DDX_Control(pDX, IDC_BILLING_AMOUNT   , m_CntlBilling_amt);
	DDX_Control(pDX, IDC_POS_MODE         , m_CntlPos_mode_1);
	DDX_Control(pDX, IDC_POS_MODE2        , m_CntlPos_mode_2);
	DDX_Control(pDX, IDC_POS_MODE3 	   	  , m_CntlPos_mode_3);
	DDX_Control(pDX, IDC_FRAUD_MCC    	  , m_CntlMcc_1);
	DDX_Control(pDX, IDC_FRAUD_MCC2       , m_CntlMcc_2);
	DDX_Control(pDX, IDC_FRAUD_MCC3       , m_CntlMcc_3);
	DDX_CBString(pDX, IDC_CARD_BRAND       , m_StrCard_brand);
	DDV_MaxChars(pDX, m_StrCard_brand, 10);
	DDX_Text(pDX, IDC_ACQ_COUNTRY_CODE , m_StrAcq_country_code);
	DDV_MaxChars(pDX, m_StrAcq_country_code, 3);
	DDX_Text(pDX, IDC_BILLING_AMOUNT   , m_StrBilling_amt);
	DDV_MaxChars(pDX, m_StrBilling_amt, 13);

	DDX_Text(pDX, IDC_POS_MODE         , m_StrPos_mode_1);
	DDV_MaxChars(pDX, m_StrPos_mode_1, 3);
	DDX_Text(pDX, IDC_POS_MODE2        , m_StrPos_mode_2);
	DDV_MaxChars(pDX, m_StrPos_mode_2, 3);
	DDX_Text(pDX, IDC_POS_MODE3 	   , m_StrPos_mode_3);
	DDV_MaxChars(pDX, m_StrPos_mode_3, 3);
	DDX_Text(pDX, IDC_FRAUD_MCC    	   , m_StrMcc_1);
	DDV_MaxChars(pDX, m_StrMcc_1, 4);
	DDX_Text(pDX, IDC_FRAUD_MCC2       , m_StrMcc_2);
	DDV_MaxChars(pDX, m_StrMcc_2, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_FRAUD_MCC3       , m_StrMcc_3);
	DDV_MaxChars(pDX, m_StrMcc_3, 4);
}


BEGIN_MESSAGE_MAP(CFraudControlConfigurationFraudRule, CPropertyPage) // very imprtnt.
	ON_EN_KILLFOCUS(IDC_BILLING_AMOUNT, OnKillfocusBillingAmtFld)
	
END_MESSAGE_MAP()


BOOL CFraudControlConfigurationFraudRule::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	 if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CFraudControlConfigurationFraudRule::OnInitDialog() 
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
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);    
	m_bFraudGeneral = TRUE;


	if(m_nDialogMode != MODE_DONE )
	{
		EnableConfigurGeneralDialog(TRUE);
		//if ( m_nDialogMode != MODE_ADD )
			// PopulateGeneralDialogFromDatabase (fclf01_old);
	}
	else
	{
		EnableConfigurGeneralDialog(FALSE);
	}
		  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	return TRUE;
}

void CFraudControlConfigurationFraudRule::EnableConfigurGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {
		m_CntlCard_brand.EnableWindow(nStatus);	
		m_CntlAcq_country_code.EnableWindow(nStatus);	
		m_CntlBilling_amt.EnableWindow(nStatus);
		m_CntlPos_mode_1.EnableWindow(nStatus);
		m_CntlPos_mode_2.EnableWindow(nStatus);       
		m_CntlPos_mode_3.EnableWindow(nStatus);       
		m_CntlMcc_1.EnableWindow(nStatus);            
		m_CntlMcc_2.EnableWindow(nStatus);            
		m_CntlMcc_3.EnableWindow(nStatus);   
	}
}

void CFraudControlConfigurationFraudRule::CleanConfigurGeneralDialog()
{
	m_StrCard_brand          = _T("");
	m_StrAcq_country_code    = _T("");
	m_StrBilling_amt         = _T("");
	m_StrPos_mode_1          = _T("");
	m_StrPos_mode_2          = _T("");
	m_StrPos_mode_3          = _T("");
	m_StrMcc_1               = _T("");
	m_StrMcc_2               = _T("");
	m_StrMcc_3               = _T("");
	m_StrMessageLbl			= _T("");

	m_CntlCard_brand.Clear();
	m_StrCard_brand.Empty();
	UpdateData( FALSE );
}

void CFraudControlConfigurationFraudRule::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
	CString temp;
    CHAR wstation[21]; DWORD wsize;
    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, frule);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"FRULE01",               sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));               
    memcpy ((LPTSTR)sHistoryData.current_data,          (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,         (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,            (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag,             (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
}

BOOL CFraudControlConfigurationFraudRule::PopulateDatabaseFromGeneralDialog( pFRULE01 psFRULE01 )
{
	CMyUtils myUtilsObject; 
	CString strAmount;
	char strErrorMessage[250];

	memcpy ( psFRULE01->card_brand,			m_StrCard_brand       , sizeof(psFRULE01->card_brand) );	
	memcpy ( psFRULE01->acq_country_code,	m_StrAcq_country_code ,	sizeof(psFRULE01->acq_country_code) );
	
	if(!m_StrBilling_amt.IsEmpty())
	{

		strAmount = myUtilsObject.GenerateTranAmount ( m_StrBilling_amt, 12, strErrorMessage );
		if ( strAmount == "Error" )
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
			m_CntlBilling_amt.SetFocus();
			return FALSE;
		}
		else
			m_StrBilling_amt = strAmount;
	}
	memcpy ( psFRULE01->billing_amt,		m_StrBilling_amt      , sizeof(psFRULE01->billing_amt) );
	
	if(!m_StrPos_mode_1.IsEmpty())
	{
		m_StrPos_mode_1 = '0'+m_StrPos_mode_1+'0';
	}
	if(!m_StrPos_mode_2.IsEmpty())
	{
		m_StrPos_mode_2 = '0'+m_StrPos_mode_2+'0';
	}
	if(!m_StrPos_mode_3.IsEmpty())
	{
		m_StrPos_mode_3 = '0'+m_StrPos_mode_3+'0';
	}
	memcpy ( psFRULE01->pos_mode_1,			m_StrPos_mode_1       , sizeof(psFRULE01->pos_mode_1) );	
	memcpy ( psFRULE01->pos_mode_2,			m_StrPos_mode_2       ,	sizeof(psFRULE01->pos_mode_2) );
	memcpy ( psFRULE01->pos_mode_3,			m_StrPos_mode_3       , sizeof(psFRULE01->pos_mode_3) );
	memcpy ( psFRULE01->mcc_1,				m_StrMcc_1            , sizeof(psFRULE01->mcc_1) );	
	memcpy ( psFRULE01->mcc_2,				m_StrMcc_2            ,	sizeof(psFRULE01->mcc_2) );
	memcpy ( psFRULE01->mcc_3,				m_StrMcc_3            , sizeof(psFRULE01->mcc_3) );
	
	return TRUE;
}


BOOL CFraudControlConfigurationFraudRule::CompareStructures()
{
	if(strcmp(m_StrCard_brand,(char*)FRULE01_old.card_brand) !=0)
	{
		memcpy(m_StrCurrentData, m_StrCard_brand, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.card_brand, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Card Brand", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrAcq_country_code,(char*)FRULE01_old.acq_country_code) !=0)
	{
		memcpy(m_StrCurrentData, m_StrAcq_country_code, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.acq_country_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ACQ Country Code", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrBilling_amt,(char*)FRULE01_old.billing_amt) !=0)
	{
		memcpy(m_StrCurrentData, m_StrBilling_amt, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.billing_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Billing Amount", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

		if(strcmp(m_StrPos_mode_1,(char*)FRULE01_old.pos_mode_1) !=0)
	{
		memcpy(m_StrCurrentData, m_StrPos_mode_1, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.pos_mode_1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "POS Entry Mode 1", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}
	if(strcmp(m_StrPos_mode_2,(char*)FRULE01_old.pos_mode_2) !=0)
	{
		memcpy(m_StrCurrentData, m_StrPos_mode_2, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.pos_mode_2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "POS Entry Mode 2", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrPos_mode_3,(char*)FRULE01_old.pos_mode_3) !=0)
	{
		memcpy(m_StrCurrentData, m_StrPos_mode_3, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.pos_mode_3, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "POS Entry Mode 3", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrMcc_1,(char*)FRULE01_old.mcc_1) !=0)
	{
		memcpy(m_StrCurrentData, m_StrMcc_1, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.mcc_1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MCC 1", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrMcc_2,(char*)FRULE01_old.mcc_2) !=0)
	{
		memcpy(m_StrCurrentData, m_StrMcc_2, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.mcc_2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MCC 2", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrMcc_3,(char*)FRULE01_old.mcc_3) !=0)
	{
		memcpy(m_StrCurrentData, m_StrMcc_3, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, FRULE01_old.mcc_3, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MCC 3", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}
	return true;

}
BOOL CFraudControlConfigurationFraudRule::PopulateGeneralDialogFromDatabase( pFRULE01 psFRULE01 )
{
	CMyUtils myUtilsObject; 
	CString strAmount;
	char pos_mode[5] ={0};
	char strErrorMessage[250];

	m_StrCard_brand       	 =	psFRULE01->card_brand;			
	m_StrAcq_country_code 	 =	psFRULE01->acq_country_code;
	m_StrBilling_amt		 =  psFRULE01->billing_amt;
	if (strlen(psFRULE01->billing_amt)> 0)
		m_StrBilling_amt		 =  myUtilsObject.DisplayAmount ( m_StrBilling_amt );

	/*m_StrPos_mode_1          =	&psFRULE01->pos_mode_1[1];			
	m_StrPos_mode_2       	 =	&psFRULE01->pos_mode_2[1];			
	m_StrPos_mode_3       	 =	&psFRULE01->pos_mode_3[1];*/	

	strncpy(pos_mode,(char *)&psFRULE01->pos_mode_1[1] ,2);
	m_StrPos_mode_1 = pos_mode;

	strncpy(pos_mode,(char *)&psFRULE01->pos_mode_2[1] ,2);
	m_StrPos_mode_2 = pos_mode;

	strncpy(pos_mode,(char *)&psFRULE01->pos_mode_3[1] ,2);
	m_StrPos_mode_3 = pos_mode;

	m_StrMcc_1            	 =	psFRULE01->mcc_1;				
	m_StrMcc_2            	 =	psFRULE01->mcc_2;				
	m_StrMcc_3            	 =	psFRULE01->mcc_3;				

		 if ( m_nDialogMode != MODE_VIEW )
			EnableConfigurGeneralDialog(TRUE);
		 else
		 {
			EnableConfigurGeneralDialog(FALSE);
		 }


	 UpdateData(FALSE);

	 memcpy ( &FRULE01_old, psFRULE01, sizeof ( FRULE01));

     return TRUE;

}

void CFraudControlConfigurationFraudRule::OnKillfocusBillingAmtFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;
	double nCreditLimit;
	char temp[20];
	char  buffer[200];

	//int count;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

	int length = m_StrBilling_amt.GetLength();

	int res = strspn ( m_StrBilling_amt, "1234567890." );
	if ( res < length )
	{
		m_CntlBilling_amt.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
	
   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrBilling_amt, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlBilling_amt.SetFocus();
		return ;
	}
	nCreditLimit = atof ( m_StrBilling_amt );

	UpdateData(FALSE);

}
BOOL CFraudControlConfigurationFraudRule::OnApply() 
{
	OnKillfocusBillingAmtFld() ;
	return CPropertyPage::OnApply();
}