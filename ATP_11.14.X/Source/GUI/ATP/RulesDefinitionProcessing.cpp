#include "stdafx.h"
#include "Atp.h"
#include "RulesDefinitionProcessing.h"
#include "MyUtils.h"
#include "BinList.h"
#include "SharedMem.h"
#include "MyUtils.h"
extern "C"
{
#include "PteTime.h"
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//int nLowRange;
//int nHighRange; 
extern CString strBinLow;
extern CString strBinHigh;

extern UCF01 ucf01;
extern CString name;
extern CString StrException_cardnum;
extern CSharedMem shrmem;

IMPLEMENT_DYNCREATE(CRulesDefinitionProcessing, CPropertyPage)

CRulesDefinitionProcessing::CRulesDefinitionProcessing() : CPropertyPage(CRulesDefinitionProcessing::IDD)
{
		m_StrRuleID_Fld = _T("");
		m_StrRuleName_Fld = _T("");
		m_StrRuleStatus_Fld = _T("");
		m_StrRuleprocessing_Fld = _T("");
		m_StrRuleBinHigh_Fld = _T("");
		m_StrRuleBinLow_Fld = _T("");
		m_StrResponseCode_Fld = _T("");
		m_StrSingleAmtLimit_Fld = _T("");
		m_StrAccAmtLimit_Fld = _T("");
		m_StrAccTransactionLimit_Fld = _T("");
		m_StrSingleAmtLimit_Fld = _T("");
		m_StrAccAmtLimitCounter_Fld     =_T("");
		m_StrAccTransactionLimitCounter_Fld=_T("");

}

CRulesDefinitionProcessing::~CRulesDefinitionProcessing()
{
}
void CRulesDefinitionProcessing::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RULE_ID_FLD, m_CntlRuleID);
	DDX_Control(pDX, IDC_RULE_NAME_FLD, m_CntlRuleName);
	DDX_Control(pDX, IDC_SINGLE_AMT_LIMIT_FLD, m_CntlSingleAmtLimit);
	DDX_Control(pDX, IDC_ACC_AMT_LIMIT_FLD, m_CntlAccAmtLimit);
	DDX_Control(pDX, IDC_ACC_TRANSACTION_LIMIT_FLD, m_CntlAccTransactionLimit);
	DDX_Control(pDX, IDC_ACC_AMT_LIMIT_COUNTER_FLD, m_CntlAccAmtLimit_counter);
	DDX_Control(pDX, IDC_ACC_TRANSACTION_LIMIT_COUNTER_FLD, m_CntlAccTransactionLimit_counter);
	DDX_Control(pDX, IDC_RULE_BIN_HIGH_FLD, m_CntlRuleBinHigh);
	DDX_Control(pDX, IDC_RULE_BIN_LOW_FLD, m_CntlRuleBinLow);
	DDX_Control(pDX, IDC_RULE_RESPONSE_CODE_FLD, m_CntlResponse_code);
	DDX_Control(pDX, IDC_RULE_STATUS_FLD, m_CntlRuleStatus);
	DDX_Control(pDX, IDC_RULE_PROCESSING_FLD, m_CntlRuleprocessing);
	DDX_CBString(pDX, IDC_RULE_STATUS_FLD, m_StrRuleStatus_Fld);
	DDX_CBString(pDX, IDC_RULE_PROCESSING_FLD, m_StrRuleprocessing_Fld);
	DDX_Text(pDX, IDC_RULE_ID_FLD, m_StrRuleID_Fld);
	DDV_MaxChars(pDX, m_StrRuleID_Fld, 10);
	DDX_Text(pDX, IDC_RULE_NAME_FLD, m_StrRuleName_Fld);
	DDV_MaxChars(pDX, m_StrRuleName_Fld, 30);
	DDX_Text(pDX, IDC_SINGLE_AMT_LIMIT_FLD, m_StrSingleAmtLimit_Fld);
	DDV_MaxChars(pDX, m_StrSingleAmtLimit_Fld, 10);
    DDX_Text(pDX, IDC_ACC_AMT_LIMIT_FLD, m_StrAccAmtLimit_Fld);
	DDV_MaxChars(pDX, m_StrAccAmtLimit_Fld, 10);
	DDX_Text(pDX, IDC_ACC_TRANSACTION_LIMIT_FLD, m_StrAccTransactionLimit_Fld);
	DDV_MaxChars(pDX, m_StrAccTransactionLimit_Fld, 12);

	DDX_Text(pDX, IDC_ACC_AMT_LIMIT_COUNTER_FLD, m_StrAccAmtLimitCounter_Fld);
	DDV_MaxChars(pDX, m_StrAccAmtLimitCounter_Fld, 10);
	DDX_Text(pDX, IDC_ACC_TRANSACTION_LIMIT_COUNTER_FLD, m_StrAccTransactionLimitCounter_Fld);
	DDV_MaxChars(pDX, m_StrAccTransactionLimitCounter_Fld, 12);

	DDX_Text(pDX, IDC_RULE_BIN_HIGH_FLD, m_StrRuleBinHigh_Fld);
	DDV_MaxChars(pDX, m_StrRuleBinHigh_Fld, 19);
	DDX_Text(pDX, IDC_RULE_BIN_LOW_FLD, m_StrRuleBinLow_Fld);
	DDV_MaxChars(pDX, m_StrRuleBinLow_Fld, 19);
	DDX_Text(pDX, IDC_RULE_RESPONSE_CODE_FLD, m_StrResponseCode_Fld);
	DDV_MaxChars(pDX, m_StrResponseCode_Fld, 3);

	DDX_Control(pDX, IDC_LIST_BIN_BUTTON, m_ListButton);
}

BEGIN_MESSAGE_MAP(CRulesDefinitionProcessing, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsProcessing)

	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_MESSAGE_LBL, &CRulesDefinitionProcessing::OnStnClickedMessageLbl)
	ON_BN_CLICKED(IDC_LIST_BIN_BUTTON, OnListBinRuleButton)
//	ON_EN_KILLFOCUS(IDC_RULE_BIN_HIGH_FLD, OnKillfocusHighRange1)
END_MESSAGE_MAP()

BOOL CRulesDefinitionProcessing::OnInitDialog() 
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

    m_bProcessing = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{	
		if (m_nDialogMode != MODE_VIEW)
			EnableProcessingDialog(TRUE);	
	}    
	    if (m_nDialogMode == MODE_ADD )
    {		
	    m_CntlRuleStatus.SetCurSel(0); 
	    m_CntlRuleprocessing.SetCurSel(0);	    
	}
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

 BOOL CRulesDefinitionProcessing::PopulateDatabaseFromRulesProcessingDialog( pCR01 psRuleID )
{
	CMyUtils myUtilsObject;
    char strRuleStatus[2]; 
	char strRuleProcessing[2];
	  CString strAmount;
	  char strErrorMessage[250]={0};

		strAmount = myUtilsObject.GenerateTranAmount ( m_StrSingleAmtLimit_Fld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlSingleAmtLimit.SetFocus();
		return FALSE;
	}
   else
      m_StrSingleAmtLimit_Fld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrAccAmtLimit_Fld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlAccAmtLimit.SetFocus();
		return FALSE;
	}
   else
      m_StrAccAmtLimit_Fld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrAccAmtLimitCounter_Fld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlAccAmtLimit_counter.SetFocus();
		return FALSE;
	}
   else
      m_StrAccAmtLimitCounter_Fld = strAmount;

	memset ( psRuleID, 0, sizeof ( psRuleID)); 
	memcpy (psRuleID->rule_id,m_StrRuleID_Fld, sizeof (psRuleID->rule_id));
    memcpy ( psRuleID->rulename,m_StrRuleName_Fld,sizeof ( psRuleID->rulename));
    memcpy ( psRuleID->single_amtlimit,m_StrSingleAmtLimit_Fld,sizeof ( psRuleID->single_amtlimit));
    memcpy ( psRuleID->acc_amtlimit,m_StrAccAmtLimit_Fld,sizeof ( psRuleID->acc_amtlimit));
    memcpy ( psRuleID->acc_transactionlimit,m_StrAccTransactionLimit_Fld,sizeof ( psRuleID->acc_transactionlimit));
	memcpy ( psRuleID->rule_bin_high,m_StrRuleBinHigh_Fld,sizeof ( psRuleID->rule_bin_high));
	memcpy ( psRuleID->rule_bin_low,m_StrRuleBinLow_Fld,sizeof ( psRuleID->rule_bin_low));
	memcpy ( psRuleID->response_code,m_StrResponseCode_Fld,sizeof ( psRuleID->response_code));
	 memcpy ( psRuleID->acc_amtlimit_counter,m_StrAccAmtLimitCounter_Fld,sizeof ( psRuleID->acc_amtlimit));
    memcpy ( psRuleID->acc_transactionlimit_counter,m_StrAccTransactionLimitCounter_Fld,sizeof ( psRuleID->acc_transactionlimit));

	//memcpy ( psRuleID->single_amtlimit,myUtilsObject.GenerateAmount (m_StrSingleAmtLimit_Fld,12),sizeof ( psRuleID->single_amtlimit));
	memset ( strRuleProcessing, 0, sizeof(strRuleStatus));    
	if ( m_StrRuleprocessing_Fld == "INSTALLMENT" )
        memcpy ( strRuleProcessing, "0", sizeof (strRuleProcessing));
	else if ( m_StrRuleprocessing_Fld == "STRAIGHT" )
        memcpy ( strRuleProcessing, "1", sizeof (strRuleProcessing));

   memcpy ( psRuleID->rule_processing,strRuleProcessing,sizeof ( psRuleID->rule_processing));


	memset ( strRuleStatus, 0, sizeof(strRuleStatus));    
	if ( strcmp (m_StrRuleStatus_Fld, "ACTIVATED") == 0)
         memcpy (strRuleStatus,	"A", sizeof(strRuleStatus)); 
	if ( strcmp (m_StrRuleStatus_Fld, "DEACTIVATED")== 0)
         memcpy (strRuleStatus,	"D", sizeof(strRuleStatus)); 

	memcpy ( psRuleID->status,strRuleStatus,sizeof ( psRuleID->status));

	return TRUE;
}

	BOOL CRulesDefinitionProcessing::PopulateRulesProcessingDialogFromDatabase( CR01 sRuleID )
{
	CMyUtils myUtilsObject;
    CString rulestatus;
	CString strruleProcessing;

    if ( m_nDialogMode != MODE_VIEW )
        EnableProcessingDialog(TRUE);



	m_StrRuleID_Fld = sRuleID.rule_id;
 
    m_StrRuleName_Fld              = sRuleID.rulename;
    m_StrSingleAmtLimit_Fld        = sRuleID.single_amtlimit;
    m_StrAccAmtLimit_Fld           = sRuleID.acc_amtlimit;
    m_StrAccTransactionLimit_Fld   = sRuleID.acc_transactionlimit;
	m_StrRuleBinHigh_Fld			=sRuleID.rule_bin_high;
	m_StrRuleBinLow_Fld				=sRuleID.rule_bin_low;
	m_StrResponseCode_Fld			=sRuleID.response_code;
	m_StrAccAmtLimitCounter_Fld     =sRuleID.acc_amtlimit_counter;
	m_StrAccTransactionLimitCounter_Fld=sRuleID.acc_transactionlimit_counter;


	rulestatus                      = sRuleID.status;
    strruleProcessing               = sRuleID.rule_processing;

	   if ( strruleProcessing == "0" )
      m_StrRuleprocessing_Fld = "INSTALLMENT";
   else if ( strruleProcessing == "1" )
      m_StrRuleprocessing_Fld = "STRAIGHT";

	 if ( rulestatus == "A")
        m_StrRuleStatus_Fld = "ACTIVATED";
    else if ( rulestatus == "D")
        m_StrRuleStatus_Fld = "DEACTIVATED"; 

	if(sRuleID.single_amtlimit[0]!='\0')
	m_StrSingleAmtLimit_Fld = myUtilsObject.DisplayAmount( m_StrSingleAmtLimit_Fld );

	if(sRuleID.acc_amtlimit[0]!='\0')
	m_StrAccAmtLimit_Fld = myUtilsObject.DisplayAmount( m_StrAccAmtLimit_Fld );


	m_StrAccAmtLimitCounter_Fld = myUtilsObject.DisplayAmount( m_StrAccAmtLimitCounter_Fld );

	UpdateData(FALSE);

   // memcpy ( &sRuleIdOld, &sRuleID, sizeof ( CR01));


    return TRUE;
}

void CRulesDefinitionProcessing::CleanProcessingDialog()
{
	m_StrRuleID_Fld = _T("");
	m_StrRuleName_Fld = _T("");
	m_StrRuleStatus_Fld = _T("");
	m_StrRuleprocessing_Fld = _T("");
	m_StrAccAmtLimit_Fld = _T("");
	m_StrAccTransactionLimit_Fld = _T("");
	m_StrSingleAmtLimit_Fld = _T("");
	m_StrRuleBinHigh_Fld = _T("");
	m_StrRuleBinLow_Fld = _T("");
	m_StrResponseCode_Fld = _T("");
	m_StrAccAmtLimitCounter_Fld     =_T("");
	m_StrAccTransactionLimitCounter_Fld=_T("");
	UpdateData(FALSE);
}

void CRulesDefinitionProcessing::EnableProcessingDialog(int nStatus)
{
	m_CntlRuleID.EnableWindow(nStatus);
	m_CntlRuleName.EnableWindow(nStatus);
	m_CntlRuleStatus.EnableWindow(nStatus);
	m_CntlRuleprocessing.EnableWindow(nStatus);
	m_CntlSingleAmtLimit.EnableWindow(nStatus);
	m_CntlAccAmtLimit.EnableWindow(nStatus);
	m_CntlAccTransactionLimit.EnableWindow(nStatus);
	m_CntlRuleBinHigh.EnableWindow(nStatus);
	m_CntlRuleBinLow.EnableWindow(nStatus);
	m_CntlResponse_code.EnableWindow(nStatus);

	if (m_nDialogMode == MODE_ADD)
   {
	   m_StrRuleStatus_Fld = "ACTIVATED";
	   m_StrRuleprocessing_Fld = "INSTALLMENT";
	}
	return;

}

BOOL CRulesDefinitionProcessing::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CRulesDefinitionProcessing::CompareStructures(CR01 sRuleIdOld)
{
	CMyUtils myUtilsObject;
    CString Rulestatus;
	CString strRuleProcessing;
	CString temp;

	if(strcmp ( m_StrRuleName_Fld, (char *)sRuleIdOld.rulename) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRuleName_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.rulename, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Rule Name", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrSingleAmtLimit_Fld, (char *)sRuleIdOld.single_amtlimit) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrSingleAmtLimit_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.single_amtlimit, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Single Amt Limit", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrAccTransactionLimit_Fld, (char *)sRuleIdOld.acc_transactionlimit) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAccTransactionLimit_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.acc_transactionlimit, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Acc Transaction Limit", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	    if (strcmp ( m_StrAccAmtLimit_Fld, (char *)sRuleIdOld.acc_amtlimit) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAccAmtLimit_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.acc_amtlimit, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Acc Amt Limit", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	


	if (strcmp ( m_StrRuleBinHigh_Fld, (char *)sRuleIdOld.rule_bin_high) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRuleBinHigh_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.rule_bin_high, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Bin High", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	
	if (strcmp ( m_StrRuleBinLow_Fld, (char *)sRuleIdOld.rule_bin_low) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRuleBinLow_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.rule_bin_low, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Bin Low", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	
	if (strcmp ( m_StrResponseCode_Fld, (char *)sRuleIdOld.response_code) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrResponseCode_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sRuleIdOld.response_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Response Code", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if ( m_StrRuleprocessing_Fld == "INSTALLMENT" )
	{
        strRuleProcessing = "0";
	}
	else if( m_StrRuleprocessing_Fld == "STRAIGHT" )
	{
        strRuleProcessing = "1";
	}

	if (strcmp ( strRuleProcessing, (char *)sRuleIdOld.rule_processing) != 0)
    {
		strRuleProcessing = sRuleIdOld.rule_processing;

	   if ( strRuleProcessing == "0" )
			strRuleProcessing = "INSTALLMENT";
	  else if ( strRuleProcessing == "1" )
           strRuleProcessing = "STRAIGHT";

		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRuleprocessing_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, strRuleProcessing, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Rule Processing", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

    Rulestatus= sRuleIdOld.status;

	if (Rulestatus == "A")
		temp = "ACTIVATED";
	else if (Rulestatus == "R")
		temp = "DEACTIVATED";	

	if (strcmp ( m_StrRuleStatus_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRuleStatus_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Rule Status", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	return TRUE;
}


void CRulesDefinitionProcessing::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
	

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));
	
    strcpy ( strPrimaryKey, m_StrRuleID_Fld);

    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,(LPCTSTR)"CR01",sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
      temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,(LPCTSTR)strPrimaryKey,sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,sizeof (sHistoryData.primary_key.unique_id));    
    memcpy ((LPTSTR) sHistoryData.time_modified,(LPCTSTR)strTime,sizeof (sHistoryData.time_modified));             
    memcpy ((LPTSTR) sHistoryData.current_data,(LPCTSTR)m_StrCurrentData,sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data,(LPCTSTR)m_StrPreviousData,sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,(LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR) sHistoryData.field_tag,(LPCTSTR)m_StrFieldTag,sizeof (sHistoryData.field_tag));
	
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );

	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

void CRulesDefinitionProcessing::OnStnClickedMessageLbl()
{
	// TODO: Add your control notification handler code here
}
/*void CBinMCCBlocking::OnKillfocusClickedAccumulated_Limit() 
{
	
	CMyUtils myUtilsObject;
	 		
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrRuleBinLow_Fld.IsEmpty() && m_StrRuleBinHigh_Fld.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrRuleBinLow_Fld, this))
    {
		m_StrMessageLbl = "Please enter Low Bin";	
		m_CntlLowRange1.SetFocus();		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrRuleBinHigh_Fld, this))
    {
		m_StrMessageLbl = "Please enter High Bin ";	
		m_CntlLowRange1.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
		
	}	
			nLowRange = atoi(m_StrRuleBinLow_Fld);
			nHighRange = atoi(m_StrRuleBinHigh_Fld);           
			
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range1 cannot be less than or equal to Low Range1";	
				m_CntlLowRange1.SetFocus();
				
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			
			//	m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}

			//m_CntlLowRange2.EnableWindow(TRUE);
			//m_CntlHighRange2.EnableWindow(TRUE);
		//	m_CntlLowRange2.SetFocus();     
			//return TRUE;	
			return;
}*/

void CRulesDefinitionProcessing::OnListBinRuleButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);
		


	listDlg.m_nDialogMode = MODE_RULE_GENERAL;
	listDlg.strItem0 = m_StrRuleBinLow_Fld;
	listDlg.strItem1 = m_StrRuleBinHigh_Fld;


    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrRuleBinLow_Fld = strBinLow;
		m_StrRuleBinHigh_Fld = strBinHigh;

	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}
BOOL CRulesDefinitionProcessing::OnApply() 
{
//	if ( !CheckDialogForCompleteness() )
//		return FALSE;
	
	return CPropertyPage::OnApply();
}

BOOL CRulesDefinitionProcessing::CheckDialogForCompleteness()
{

	CMyUtils myUtilsObject;
	int nLowLength;
	int nHighLength;
	int nBinLength;
	int nCurLength;
	int nPanLength;
	CAF01 sAuthRecord;
	CTF01 sCardTypeRecord;
	NCF01 sNetworkRecord;
	char strErrorMessage [200] = "";

	ZeroMemory ( &sAuthRecord, sizeof (CAF01));

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

		if (myUtilsObject.IsEmpty (m_StrRuleStatus_Fld, this))

		{

			m_StrMessageLbl = "Please choose Status";	
					m_CntlRuleStatus.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
		
		if (myUtilsObject.IsEmpty (m_StrRuleName_Fld, this))

		{

			m_StrMessageLbl = "Please enter Rule Name";	
					m_CntlRuleName.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			AfxMessageBox( "Please  Enter Rule Name ", MB_ICONSTOP | MB_OK );
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
		if (myUtilsObject.IsEmpty (m_StrRuleprocessing_Fld, this))

		{

			m_StrMessageLbl = "Please Choose Rule processing option";	
			m_CntlRuleprocessing.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
		if (myUtilsObject.IsEmpty (m_StrRuleBinLow_Fld, this))

		{

			m_StrMessageLbl = "Please Enter Bin Low";	
			m_CntlRuleBinLow.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			AfxMessageBox( "Please  Enter Bin Low ", MB_ICONSTOP | MB_OK );
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
				if (myUtilsObject.IsEmpty (m_StrRuleBinHigh_Fld, this))

		{

			m_StrMessageLbl = "Please Enter Bin High";	
			m_CntlRuleBinHigh.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		

			}
			AfxMessageBox( "Please Enter Bin High ", MB_ICONSTOP | MB_OK );
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
		if (myUtilsObject.IsEmpty (m_StrResponseCode_Fld, this))

		{

			m_StrMessageLbl = "Please Enter Response Code";	
			m_CntlResponse_code.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			AfxMessageBox( "Please Enter Response Code ", MB_ICONSTOP | MB_OK );
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}

		    return TRUE;

}