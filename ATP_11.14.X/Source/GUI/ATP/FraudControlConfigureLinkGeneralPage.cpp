

#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "FraudControlConfigureLinkMainPage.h"
#include "FraudControlConfigureLinkGeneralPage.h"
#include "FraudControlConfigureLinkSheet.h"
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
extern char networkid1[11];

IMPLEMENT_DYNAMIC(CFraudControlConfigurationLinkGeneralpage, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CFraudControlConfigurationLinkGeneralpage::CFraudControlConfigurationLinkGeneralpage()
	: CPropertyPage(CFraudControlConfigurationLinkGeneralpage::IDD)	
{
	m_StrMessageLbl			= _T("");

}



CFraudControlConfigurationLinkGeneralpage::~CFraudControlConfigurationLinkGeneralpage()
{

}
void CFraudControlConfigurationLinkGeneralpage::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkGeneral)
	DDX_Control(pDX, IDC_CONFIGURE_LINK_CONTROL_ENABLE, m_CntlEnableFraudControl);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_CONTROL_ENABLE2, m_CntlEnableFraudRule);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);

	//}}AFX_DATA_MAP
} 



BEGIN_MESSAGE_MAP(CFraudControlConfigurationLinkGeneralpage, CPropertyPage) // very imprtnt.
	//{{AFX_MSG_MAP(CNetworkGeneral1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFraudControlConfigurationLinkGeneralpage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	  if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CFraudControlConfigurationLinkGeneralpage::OnInitDialog() 
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

void CFraudControlConfigurationLinkGeneralpage::EnableConfigurGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {	
		m_CntlEnableFraudControl.EnableWindow(nStatus);
		m_CntlEnableFraudRule.EnableWindow(nStatus);
	}
}

void CFraudControlConfigurationLinkGeneralpage::CleanConfigurGeneralDialog()
{
	m_StrMessageLbl			= _T("");
	m_CntlEnableFraudControl.SetCheck(0);
	m_CntlEnableFraudRule.SetCheck(0);
	UpdateData( FALSE );
}


void CFraudControlConfigurationLinkGeneralpage::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, networkid1);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"FCFL01",               sizeof (sHistoryData.file_name));
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

BOOL CFraudControlConfigurationLinkGeneralpage::CompareStructures()
{
	CString temp="";
	CString fraudControlenable= "";
	CString fraudruleenable= "";

	if (m_CntlEnableFraudControl.GetCheck() == 0)
		temp = "0";
	else if (m_CntlEnableFraudControl.GetCheck() == 1)
		temp = "1";

	if(strcmp(temp,fclf01_old.fraud_control_enable_flag) !=0)
	{
		memcpy(m_StrCurrentData, temp, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, fclf01_old.fraud_control_enable_flag, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Control Flag", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}

	if (m_CntlEnableFraudRule.GetCheck() == 0)
		temp = "0";
	else if (m_CntlEnableFraudRule.GetCheck() == 1)
		temp = "1";

	if(strcmp(temp,fclf01_old.fraud_rule_enable_flag) !=0)
	{
		memcpy(m_StrCurrentData, temp, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, fclf01_old.fraud_rule_enable_flag, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Rule Flag", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}
	 return TRUE;
}

BOOL CFraudControlConfigurationLinkGeneralpage::PopulateDatabaseFromGeneralDialog( pFCLF01 psFclf01 )
{


	if (m_CntlEnableFraudControl.GetCheck() == 1)
		psFclf01->fraud_control_enable_flag[0] = '1';
	else
		psFclf01->fraud_control_enable_flag[0] = '0';


	if (m_CntlEnableFraudRule.GetCheck() == 1)
		psFclf01->fraud_rule_enable_flag[0] = '1';
	else
		psFclf01->fraud_rule_enable_flag[0] = '0';

	 return TRUE;
}

BOOL CFraudControlConfigurationLinkGeneralpage::PopulateGeneralDialogFromDatabase( pFCLF01 psFclf01 )
{
	CString fraud_control_enable_flag;    
    CString fraud_rule_enable_flag; 


	 ZeroMemory ( &fclf01_old, sizeof (FCLF01));  


	 fraud_control_enable_flag			= psFclf01->fraud_control_enable_flag;
	 fraud_rule_enable_flag		= psFclf01->fraud_rule_enable_flag;

	 if (fraud_control_enable_flag == '1')
	 {
		 m_CntlEnableFraudControl.SetCheck(TRUE);
	 }

	 if(fraud_rule_enable_flag == '1')
	 {
		m_CntlEnableFraudRule.SetCheck(TRUE);
	 }
	 
	 if ( m_nDialogMode != MODE_VIEW )
		EnableConfigurGeneralDialog(TRUE);
	 else 
	 {
		 EnableConfigurGeneralDialog(FALSE);
	 }

	 UpdateData(FALSE);

	 memcpy ( &fclf01_old, psFclf01, sizeof ( FCLF01));

     return TRUE;

}
