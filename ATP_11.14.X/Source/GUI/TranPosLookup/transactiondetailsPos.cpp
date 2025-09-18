/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
*
										  *
************************************************************************************/
// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "TranPosSearch.h"
#include "TransactionDetailsPos.h"
#include "MyUtils.h"
#include "GenUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_bTran;
extern BOOL m_bFGuard;
extern CString strUserName;

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails dialog


CTransactionDetails::CTransactionDetails(CWnd* pParent /*=NULL*/)
: CDialog(CTransactionDetails::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransactionDetails)
	m_StrHostTimeFld = _T("");
	m_StrAcquiringInstIDFld = _T("");
	m_StrAuthIDResponseFld = _T("");
	m_StrCardNumberFld = _T("");
	m_StrForwardingInstIDFld = _T("");
	m_StrInvoiceNumberFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrPOSConditionCodeFld = _T("");
	m_StrPOSEntryModeFld = _T("");
	m_StrProcessingCodeFld = _T("");
	m_StrResponseTextFld = _T("");
	m_StrRetrievalRefNumFld = _T("");
	m_StrSystemTraceNumberFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrTranAmountFld = _T("");
	m_StrTranDateFld = _T("");
	m_StrTranTimeFld = _T("");
	m_StrTranTypeFld = _T("");
	m_StrTranMessageTypeFld = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrTextFld = _T("");
	m_StrExpirationDateFld = _T("");
	m_StrCreditLimitFld = _T("");
	m_StrOutstandingBalanceFld = _T("");
	m_StrCurrencyCodeFld = _T("");
	m_StrOperatorIDFld = _T("");
	m_StrSystemDateFld = _T("");
	m_StrEmvFld = _T("");
	m_StrFGResultFld = _T("");
	m_StrGrossAmountFld = _T("");
	m_StrMonthlyAmortFld = _T("");
	m_StrTermLengthFld = _T("");
	m_StrFactorFld = _T("");
	m_StrBdoMercAcqTx = _T("");
	m_StrBinLow = _T("");
	m_StrBinHigh = _T("");
	m_StrBinType = _T("");
	m_StrBinCardFamily = _T("");
	m_StrNii    = _T("");
	m_StrEarMarkedAmount = _T("");
	m_StrEarMarkedCurrency = _T("");
	m_StrAuthProfile = _T("");
	m_StrServiceCode = _T("");
	m_StrOriginalTxnAmount = _T("");
	m_OrgTxnAmtCurrency = _T("");
	m_StrECI = _T("");
	m_StrMerchantName = _T("");
	m_StrMCC = _T("");
	m_StrPosData = _T("");
	m_StrAcquiringCountry = _T("");
	m_bMaskCardNumbers = false;
	m_StrVisaMerchantIdentifier = _T("");
	m_StrDigitalCommProgramIndicator = _T("");
    m_StrDigitalEntryIdentifier = _T("");
	//}}AFX_DATA_INIT
	
	m_bViewEmvData = FALSE;
}


void CTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_HOST_PROCESSED, m_ChkHostProcessed);
	DDX_Text(pDX, IDC_AUTH_ID_RESPONSE_FLD, m_StrAuthIDResponseFld);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD1, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_POS_CONDITION_CODE_FLD, m_StrPOSConditionCodeFld);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE_FLD, m_StrPOSEntryModeFld);
	DDX_Text(pDX, IDC_PROCESSING_CODE_FLD, m_StrProcessingCodeFld);
	DDX_Text(pDX, IDC_RESPONSE_TEXT_FLD, m_StrResponseTextFld);
	DDX_Text(pDX, IDC_RETRIEVAL_REF_NUM_FLD, m_StrRetrievalRefNumFld);
	DDX_Text(pDX, IDC_SYSTEM_TRACE_NUMBER_FLD, m_StrSystemTraceNumberFld);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_TRAN_DATE_FLD, m_StrTranDateFld);
	DDX_Text(pDX, IDC_TRAN_TIME_FLD, m_StrTranTimeFld);
	DDX_Text(pDX, IDC_TRAN_TYPE_FLD, m_StrTranTypeFld);
	DDX_Text(pDX, IDC_MESSAGE_TYPE, m_StrTranMessageTypeFld);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_OPERATOR_ID_FLD, m_StrOperatorIDFld);
	DDX_Text(pDX, IDC_SYSTEM_DATE_FLD, m_StrSystemDateFld);
	DDX_Text(pDX, IDC_BIN_LOW, m_StrBinLow);
	DDX_Text(pDX, IDC_BIN_HIGH, m_StrBinHigh);
	DDX_Text(pDX, IDC_BIN_TYPE, m_StrBinType);
	DDX_Text(pDX, IDC_BIN_CARD_FAMILY, m_StrBinCardFamily);
	DDX_Text(pDX, IDC_TRAN_NII, m_StrNii);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransactionDetails, CDialog)
//{{AFX_MSG_MAP(CTransactionDetails)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails message handlers

BOOL CTransactionDetails::OnInitDialog() 
{
	
	
	CDialog::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 13;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	if(m_bTran)
	{
		PopulateTranDialogFromDatabase ( sTranRecord );
	}
	else
	
		
		UpdateData(FALSE);	
		
		return TRUE;  
}

INT Check_If_AMEX_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '4') ||
		(card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

BOOL CTransactionDetails::PopulateTranDialogFromDatabase ( TRANS_RECORD sTranRecord  )
{
	CMyUtils myUtilsObject;
	CHAR     strErrorMessage[200] = "";	
	CHAR     strFactor[8] = "";
	CHAR     strMPFlag[2]="1";
	CString  temp;
	CString  strResponse;
	CString  strcard_family;
	CString  strBin_type;
	int card_family=0;
	
	int rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TPOS01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
			break;
		}
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		return FALSE;
	}
	


	m_StrCardNumberFld        =  sTranRecord.tpos01.card_num;
	m_StrMerchantIDFld        =  sTranRecord.tpos01.merchant_id;
	m_StrPOSConditionCodeFld  =  sTranRecord.tpos01.pos_condition_code; 
	m_StrPOSEntryModeFld      =  sTranRecord.tpos01.pos_entry_mode;
	m_StrProcessingCodeFld    =  sTranRecord.tpos01.processing_code;
	m_StrResponseCodeFld      =  sTranRecord.tpos01.response_code;
	m_StrResponseTextFld      =  sTranRecord.tpos01.response_text;
	m_StrRetrievalRefNumFld   =  sTranRecord.tpos01.retrieval_ref_num;
	m_StrSystemTraceNumberFld =  sTranRecord.tpos01.sys_trace_audit_num;
	m_StrTerminalIDFld        =  sTranRecord.tpos01.terminal_id;
	m_StrFGResultFld          = "";
	checkMaskCardNumber(); //To mask the card number

	m_StrTranDateFld       =  sTranRecord.tpos01.date_YYYYMMDD;
	m_StrTranTimeFld       =  sTranRecord.tpos01.time_HHMMSS;
	m_StrSystemDateFld     =  sTranRecord.tpos01.primary_key.system_date;
    m_StrOperatorIDFld     =  strUserName;
	m_StrBinHigh		   =  sTranRecord.tpos01.bin_low;
	m_StrBinLow			   =  sTranRecord.tpos01.bin_high;
	strBin_type          =  sTranRecord.tpos01.bin_type; 
	if(strBin_type == "C")
	{
		m_StrBinType = "CREDIT";
	}
	else if( true == Check_If_AMEX_Transaction((char *)sTranRecord.tpos01.card_num))
	{
		m_StrBinType = "";
	}
	else
	{
		m_StrBinType = "DEBIT";
	}
	strcard_family		   =  sTranRecord.tpos01.card_family;
	
	card_family = atoi((char*)(LPCTSTR)strcard_family);

	switch(card_family )
	{
		case CARD_AMEX_VALUE: 
			m_StrBinCardFamily = "CARD_AMEX";
			break;
		case CARD_VISA_VALUE: 
			m_StrBinCardFamily = "CARD_VISA";
			break;
		case CARD_MASTERCARD_VALUE:
			m_StrBinCardFamily = "CARD_MASTERCARD";
			break;
		case CARD_JCB_VALUE:     
			m_StrBinCardFamily = "CARD_JCB";
			break;
		case CARD_CUP_VALUE:   
			m_StrBinCardFamily = "CARD_CUP";
			break;
		case CARD_DCC_VALUE:
			m_StrBinCardFamily = "CARD_DCC";
			break;
		case CARD_DINERS_VALUE:
			m_StrBinCardFamily = "CARD_DINERS";
			break;
		case CARD_DUAL_VALUE:
			m_StrBinCardFamily = "CARD_DUAL";
			break;
		case CARD_FLEET_VALUE:
			m_StrBinCardFamily = "CARD_FLEET";
			break;
		default:
			m_StrBinCardFamily = "INVALID";
			break;
	}
	m_StrNii			   =  sTranRecord.tpos01.nii;
	
	/*************************************************************************************/

	m_StrTranMessageTypeFld = sTranRecord.tpos01.message_type;
	strResponse = "POS Transaction";
	
   
   m_StrTranDateFld   = m_StrTranDateFld.Right(2)   + "/" + m_StrTranDateFld.Mid(4,2)   + "/" + m_StrTranDateFld.Left(4);
   m_StrTranTimeFld   = m_StrTranTimeFld.Left(2)    + ":" + m_StrTranTimeFld.Mid(2,2)   + ":" + m_StrTranTimeFld.Right(2);
   m_StrSystemDateFld = m_StrSystemDateFld.Right(2) + "/" + m_StrSystemDateFld.Mid(4,2) + "/" + m_StrSystemDateFld.Left(4);
  
  // m_StrTranTypeFld =  sTranRecord.tpos01.tx_key;
   (CString)m_StrTranTypeFld = (CString)((char *) sTranRecord.tpos01.tx_key);
	strResponse = " RESPONSE";
	switch (sTranRecord.tpos01.tx_key)
	{
	case AUTH_INVALID:
		m_StrTranTypeFld = "INVALID" + strResponse;
		break;
		
	case AUTH_SALE:
	case AUTH_SALE_RESPONSE:
			m_StrTranTypeFld = "SALE" + strResponse;
		break;
		
	case  AUTH_REFUND:
	case  AUTH_REFUND_RESPONSE:
		m_StrTranTypeFld = "REFUND" + strResponse; 
		break;
		
	case  AUTH_CASH_ADVANCE:
	case  AUTH_CASH_ADVANCE_RESPONSE:
		m_StrTranTypeFld = "CASH ADVANCE" + strResponse; 
		break;
		
	case AUTH_PAYMENT:
	case AUTH_PAYMENT_RESPONSE:
		m_StrTranTypeFld = "PAYMENT" + strResponse; 
		break;
		
	case AUTH_OFFLINE_SALE:
	case AUTH_OFFLINE_SALE_RESPONSE:
		m_StrTranTypeFld = "OFFLINE SALE" + strResponse; 
		break;
		
	case AUTH_OFFLINE_REFUND:
	case AUTH_OFFLINE_REFUND_RESPONSE:
		m_StrTranTypeFld = "OFFLINE REFUND" + strResponse; 
		break;
		
	case AUTH_SALE_ADJUSTMENT:
	case AUTH_SALE_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "SALE ADJUSTMENT" + strResponse; 
		break;
		
	case AUTH_REFUND_ADJUSTMENT:
	case AUTH_REFUND_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "REFUND ADJUSTMENT" + strResponse; 
		break;
		
	case AUTH_PAYMENT_ADJUSTMENT:
	case AUTH_PAYMENT_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "PAYMENT ADJUSTMENT" + strResponse; 
		break;
		
	case AUTH_PRE_AUTHORIZATION:
	case AUTH_PRE_AUTHORIZATION_RESPONSE:
		m_StrTranTypeFld = "PRE AUTHORIZATION" + strResponse; 
		break;
		
	case AUTH_AUTHORIZATION:
	case AUTH_AUTHORIZATION_RESPONSE:
		 m_StrTranTypeFld = "AUTHORIZATION" + strResponse; 
		break;
		
	case AUTH_BALANCE_INQUIRY:
	case AUTH_BALANCE_INQUIRY_RESPONSE:
		m_StrTranTypeFld = "BALANCE INQUIRY" + strResponse; 
		break;
		
	case AUTH_VOID_SALE:
	case AUTH_VOID_SALE_RESPONSE:
		m_StrTranTypeFld = "VOID SALE" + strResponse;
		break;
		
	case AUTH_VOID_REFUND:
	case AUTH_VOID_REFUND_RESPONSE:
		m_StrTranTypeFld = "VOID REFUND" + strResponse; 
		break;
		
	case AUTH_SETTLE:
	case AUTH_SETTLE_RESPONSE:
		m_StrTranTypeFld = "SETTLE" + strResponse; 
		break;
		
	case AUTH_BATCH_UPLOAD:
	case AUTH_BATCH_UPLOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH UPLOAD" + strResponse; 
		break;
		
	case AUTH_SETTLE_TRAILER:
	case AUTH_SETTLE_TRAILER_RESPONSE:
		m_StrTranTypeFld = "SETTLE TRAILER" + strResponse; 
		break;
		
	case AUTH_STATISTICS:
	case AUTH_STATISTICS_RESPONSE:
		m_StrTranTypeFld = "STATISTICS" + strResponse; 
		break;
		
	case AUTH_REVERSAL:
	case AUTH_REVERSAL_RESPONSE:
		m_StrTranTypeFld = "REVERSAL - " + strResponse;;

		break;
		
	case AUTH_LOGON:
	case AUTH_LOGON_RESPONSE:
		m_StrTranTypeFld = "LOGON" + strResponse; 
		break;
		
	case AUTH_TEST:
	case AUTH_TEST_RESPONSE:
		m_StrTranTypeFld = "TEST" + strResponse; 
		break;
		
	case AUTH_PIN_CHANGE:
	case AUTH_PIN_CHANGE_RESPONSE:
		m_StrTranTypeFld = "PIN CHANGE" + strResponse; 
		break;
		
	case AUTH_VOID_CASH_ADVANCE:
	case AUTH_VOID_CASH_ADVANCE_RESPONSE:
		m_StrTranTypeFld = "VOID CASH ADVANCE" + strResponse; 
		break;
		
	case AUTH_SALE_CASH:
	case AUTH_SALE_CASH_RESPONSE:
		m_StrTranTypeFld = "SALE CASH" + strResponse; 
		break;
		
	case AUTH_CHECK_VERIFICATION:
	case AUTH_CHECK_VERIFICATION_RESPONSE:
		m_StrTranTypeFld = "CHECK VERIFICATION" + strResponse; 
		break;
		
	case AUTH_CARD_VERIFICATION:
	case AUTH_CARD_VERIFICATION_RESPONSE:
		m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
		break;
		
	case AUTH_VOID_PRE_AUTHORIZATION:
	case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
		m_StrTranTypeFld = "VOID PREAUTHORIZATION" + strResponse;
		break;
		
	case AUTH_SALES_COMPLETION:
	case AUTH_SALES_COMPLETION_RESPONSE:
		m_StrTranTypeFld = "SALES COMPLETION" + strResponse; 
		break;
		
	case AUTH_GUARANTEED_LATE_ARRIVAL:
		m_StrTranTypeFld = "GUARANTEED LATE ARRIVAL" + strResponse; 
		break;
		
	case AUTH_BATCH_DOWN_LINE_LOAD:
	case AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH DOWN LINE LOAD" + strResponse; 
		break;
		
	case AUTH_SIGNATURE_DATA:
	case AUTH_SIGNATURE_DATA_RESPONSE:
		m_StrTranTypeFld = "SIGNATURE DATA" + strResponse; 
		break;
		
	case AUTH_INITIALIZATION:
	case AUTH_INITIALIZATION_RESPONSE:
		m_StrTranTypeFld = "INITIALIZATION" + strResponse; 
		break;
		
	case AUTH_PLEASE_WAIT_ADVICE:
	case AUTH_PLEASE_WAIT_ADVICE_RESPONSE:
		m_StrTranTypeFld = "PLEASE WAIT ADVICE" + strResponse; 
		break;
		
	case AUTH_NEGATIVE_DOWNLOAD:
	case AUTH_NEGATIVE_DOWNLOAD_RESPONSE:
		m_StrTranTypeFld = "NEGATIVE DOWNLOAD" + strResponse; 
		break;
		
	case AUTH_ADMIN_ADVICE:
	case AUTH_ADMIN_ADVICE_RESPONSE:
		m_StrTranTypeFld = "ADMIN ADVICE" + strResponse; 
		break;
		
	case AUTH_VOID_PAYMENT:
	case AUTH_VOID_PAYMENT_RESPONSE:
		m_StrTranTypeFld = "VOID PAYMENT" + strResponse; 
		break;
		
	case AUTH_REGISTRATION:
	case AUTH_REGISTRATION_RESPONSE:
		m_StrTranTypeFld = "REGISTRATION" + strResponse; 
		break;
		
	case AUTH_CHECK_DL:
	case AUTH_CHECK_DL_RESPONSE:
		m_StrTranTypeFld = "CHECK DL" + strResponse; 
		break;
		
	case AUTH_BRANCH_PMT: 
		m_StrTranTypeFld = "BRANCH PMT" + strResponse; 
		break;
		
	case AUTH_QUASI_CASH:
	case AUTH_QUASI_CASH_RESPONSE: 
		m_StrTranTypeFld = "QUASI CASH" + strResponse; 
		break;
		
	case AUTH_CASH_ADVANCE_AUTHORIZATION:
	case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE: 
		m_StrTranTypeFld = "CASH ADVANCE AUTHORIZATION" + strResponse; 
		break;
		
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION: 
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE: 
		m_StrTranTypeFld = "DEFERRED PURCHASE" + strResponse; 
		break;
		
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
		m_StrTranTypeFld = "DEFERRED PURCHASE OVERRIDE" + strResponse; 
		break;
		
	case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
	case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
		m_StrTranTypeFld = "CASH ADVANCE AUTHORIZATION OVERRIDE" + strResponse;
		break;
		
	case AUTH_AUTHORIZATION_OVERRIDE:
	case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
		m_StrTranTypeFld = "AUTHORIZATION OVERRIDE" + strResponse; 
		break;
		
	case AUTH_ADVICE:
	case AUTH_ADVICE_RESPONSE:
		m_StrTranTypeFld = "ADVICE" + strResponse; 
		break;
		
	case AUTH_REVERSAL_ADVICE:
	case AUTH_REVERSAL_ADVICE_RESPONSE:
		m_StrTranTypeFld = "REVERSAL ADVICE" + strResponse; 
		break;
		
	case AUTH_CARD_UPDATE:
	case AUTH_CARD_UPDATE_RESPONSE:
		m_StrTranTypeFld = "CARD UPDATE" + strResponse; 
		break;
		
	case AUTH_RELOAD:
	case AUTH_RELOAD_RESPONSE:
		m_StrTranTypeFld = "CASH LOAD" + strResponse;
		break;
		
	case AUTH_RELOAD_REPEAT:
	case AUTH_RELOAD_REPEAT_RESPONSE:
		m_StrTranTypeFld = "CASH LOAD REPEAT" + strResponse;
		break;
		
	case AUTH_RELOAD_CONFIRM:
	case AUTH_RELOAD_CONFIRM_RESPONSE:
		m_StrTranTypeFld = "CASH LOAD CONFIRM" + strResponse;
		break;
		
	case AUTH_VOID_RELOAD:
	case AUTH_VOID_RELOAD_RESPONSE:
		m_StrTranTypeFld = "CASH LOAD VOID" + strResponse;
		break;
		
	case AUTH_EMV_ADVICE:
	case AUTH_EMV_ADVICE_RESPONSE:
		m_StrTranTypeFld = "EMV SCRIPT RESULTS" + strResponse;
		break;
		
	case AUTH_EMV_UPLOAD:
	case AUTH_EMV_UPLOAD_RESPONSE:
		m_StrTranTypeFld = "TC UPLOAD" + strResponse;
		break;
   }
 
   UpdateData(FALSE);
   return TRUE;
}



void CTransactionDetails::CleanTranDialog()
{
	m_StrHostTimeFld = _T("");
	m_StrAcquiringInstIDFld = _T("");
	m_StrAuthIDResponseFld = _T("");
	m_StrCardNumberFld = _T("");
	m_StrForwardingInstIDFld = _T("");
	m_StrInvoiceNumberFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrPOSConditionCodeFld = _T("");
	m_StrPOSEntryModeFld = _T("");
	m_StrProcessingCodeFld = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrResponseTextFld = _T("");
	m_StrRetrievalRefNumFld = _T("");
	m_StrSystemTraceNumberFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrTranAmountFld = _T("");
	m_StrTranDateFld = _T("");
	m_StrTranTimeFld = _T("");
	m_StrTranTypeFld = _T("");	
	m_StrTranMessageTypeFld = _T("");
	m_StrTextFld = _T("");
	m_StrExpirationDateFld = _T("");
	m_StrEmvFld = _T("");
	
}


/*include below functions to fix a linking error*/

/*****************************************************************************

  Function:   genutil_bin_to_ascii
  
	Description:
	Convert binary string to ascii string
	Author: 
	JHapitas   5/6/98
	Inputs:
	bstring  - incoming binary string
	blen     - length of bstring
	astr_len - length of astr
	Outputs:
	astr     - pointer to the outgoing ascii string
	Return values:
	Modified by:
	BGable      6/26/98
******************************************************************************/
/*****************************************
convert binary string to ascii string     JH May 06  New Procedure
*****************************************/
void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
	INT  i = 0;
	
	memset (astr, 0x00, astr_len);
	
	for (i = 0; i < (blen * 2); i+=2)
	{
		if ((bstring[i/2] / 16) <= 9) 
			astr[i] = (bstring[i/2] / 16) + '0';
		else
			astr[i] = (bstring[i/2] / 16 - 0x0a)+'A';
		
		if ((bstring[i/2] & 0x0f) <= 9)
			astr[i+1] = (bstring[i/2] & 0x0F) + '0'; 
		else
			astr[i+1] = (bstring[i/2] & 0x0F) - 0x0a + 'A';    
	}
} /* genutil_bin_to_ascii */


  /*****************************************************************************
  
	Function:   genutil_bcd_to_int
	
	  Description:
      Convert a bcd string into an integer value.
	  Author: 
      unknown
	  Inputs:
      p_bcd_data  -  pointer to the incoming bcd string
      bcd_len     -  length of the bcd string
	  Outputs:
	  Return values:
      INT         -  integer value of the bcd string
	  Modified by:
      BGable      6/26/98
******************************************************************************/
INT genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len)
{
	INT   temp_int;
	INT   multiplier;
	INT   i;
	
	
	multiplier = 1;
	temp_int   = 0;
	
	for (i = (bcd_len-1); i >= 0; i--)
	{
		temp_int+= (p_bcd_data [i] & 0x0f) * multiplier;
		multiplier = multiplier * 10;
		temp_int+= (p_bcd_data [i] / 16) * multiplier;
		multiplier = multiplier * 10;
	}
	
	return (temp_int);
} /*genutil_bcd_to_int */

void CTransactionDetails::checkMaskCardNumber()
{
	if(m_bMaskCardNumbers)
	{
		CString str = m_StrCardNumberFld;
		OriginalCardNoList.AddHead(str);
		int len = str.GetLength();
		if(len > 7)
		{
			char *pStr = str.GetBuffer(len);
			pStr[6] = 'X';
			pStr[7] = 'X';
			pStr[8] = 'X';
			pStr[9] = 'X';
			pStr[10] = 'X';
			pStr[11] = 'X';
			str.ReleaseBuffer(len);
		}
		
		m_StrCardNumberFld = str;
		
	}
	else
	{
		if(OriginalCardNoList.GetCount() > 0)
		{
			int count = OriginalCardNoList.GetCount() ;
			for(int i=0;i<count;i++)
			{
				CString origCardNo = OriginalCardNoList.RemoveTail();
				m_StrCardNumberFld = origCardNo;
				
			}
		}
	}
}

