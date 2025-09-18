

#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "OnlinePINConfigureBDKKey.h"
#include "OnlinePINConfigureBDKKeyGeneral.h"
#include "OnlinePINControlBDKKeySheet.h"
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
extern char card_type[20];

IMPLEMENT_DYNAMIC(CBDKKeyConfigurationGeneral, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CBDKKeyConfigurationGeneral::CBDKKeyConfigurationGeneral()
	: CPropertyPage(CBDKKeyConfigurationGeneral::IDD)	
{
	m_StrCard_brand          = _T("");
	m_StrBdk_key    		 = _T("");

	
}

CBDKKeyConfigurationGeneral::~CBDKKeyConfigurationGeneral()
{
}
void CBDKKeyConfigurationGeneral::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CFraudControlConfigurationFraudRule)
	             
	DDX_Control(pDX, IDC_CONFIGURE_KEY_ID_FLD, m_CntlCard_brand);
	DDX_Control(pDX, IDC_CONFIGURE_BDK_FOR_CARD, m_CntlBdk_key);

	DDX_Text(pDX, IDC_CONFIGURE_KEY_ID_FLD, m_StrCard_brand);
	DDV_MaxChars(pDX, m_StrCard_brand, 10);	

	DDX_Text(pDX, IDC_CONFIGURE_BDK_FOR_CARD , m_StrBdk_key);
	DDV_MaxChars(pDX, m_StrBdk_key, 64);

	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);

}


BEGIN_MESSAGE_MAP(CBDKKeyConfigurationGeneral, CPropertyPage) // very imprtnt.
	//ON_EN_KILLFOCUS(IDC_CONFIGURE_BDK_FOR_CARD, OnKillfocusBDKKeyFld)
	
END_MESSAGE_MAP()


BOOL CBDKKeyConfigurationGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	 if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CBDKKeyConfigurationGeneral::OnInitDialog() 
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
	m_bBDKKEYGeneral = TRUE;


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

void CBDKKeyConfigurationGeneral::EnableConfigurGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {
		//m_CntlCard_brand.EnableWindow(nStatus);	
		m_CntlBdk_key.EnableWindow(nStatus);	
  
	}
}

void CBDKKeyConfigurationGeneral::CleanConfigurGeneralDialog()
{
	//m_StrCard_brand          = _T("");
	m_StrBdk_key    		 = _T("");


	m_CntlCard_brand.Clear();
	m_StrBdk_key.Empty();
	UpdateData( FALSE );
}

void CBDKKeyConfigurationGeneral::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, card_type);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"BDKKEY01",               sizeof (sHistoryData.file_name));
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

BOOL CBDKKeyConfigurationGeneral::PopulateDatabaseFromGeneralDialog( pBDKKEY01 psBDKKEY01 )
{
	CMyUtils myUtilsObject; 
	CString strAmount;
	char strErrorMessage[250];

	/*m_StrCard_brand = m_StrCardBrandFld; */
	memcpy ( psBDKKEY01->card_brand,			m_StrCard_brand       , sizeof(psBDKKEY01->card_brand) );	
	memcpy ( psBDKKEY01->bdk_key,		m_StrBdk_key      , sizeof(psBDKKEY01->bdk_key) );
	

	
	return TRUE;
}


BOOL CBDKKeyConfigurationGeneral::CompareStructures()
{
	if(strcmp(m_StrCard_brand,(char*)BDKKEY01_old.card_brand) !=0)
	{
		memcpy(m_StrCurrentData, m_StrCard_brand, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, BDKKEY01_old.card_brand, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Card Brand", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if(strcmp(m_StrBdk_key,(char*)BDKKEY01_old.bdk_key) !=0)
	{
		memcpy(m_StrCurrentData, m_StrBdk_key, sizeof (m_StrCurrentData));
		memcpy(m_StrPreviousData, BDKKEY01_old.bdk_key, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "BDK Key", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}


	return true;

}
BOOL CBDKKeyConfigurationGeneral::PopulateGeneralDialogFromDatabase( BDKKEY01 sBDKKEY01  )
{
	CMyUtils myUtilsObject; 
	CString strAmount;
	char pos_mode[5] ={0};
	char strErrorMessage[250];

	m_StrCard_brand       	 =	sBDKKEY01.card_brand;			
	m_StrBdk_key 	 		 =	sBDKKEY01.bdk_key;

				

		 if ( m_nDialogMode != MODE_VIEW )
			EnableConfigurGeneralDialog(TRUE);
		 else
		 {
			EnableConfigurGeneralDialog(FALSE);
		 }


	 UpdateData(FALSE);

	 memcpy ( &BDKKEY01_old, &sBDKKEY01, sizeof ( BDKKEY01));

     return TRUE;

}
/*
void CBDKKeyConfigurationGeneral::OnKillfocusBDKKeyFld() 
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

	int length = m_StrBdk_key.GetLength();


	if ( length < 32 )
	{
		m_CntlBdk_key.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
	


	UpdateData(FALSE);

}*/
BOOL CBDKKeyConfigurationGeneral::OnApply() 
{
 	 UpdateData(TRUE);
   m_StrMessageLbl = "";

   int length = m_StrBdk_key.GetLength();
	if ( length < 32 )
	{
		m_CntlBdk_key.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);

	}

	return CPropertyPage::OnApply();
}