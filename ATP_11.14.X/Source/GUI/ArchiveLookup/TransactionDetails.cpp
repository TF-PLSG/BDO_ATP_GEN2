// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "TransactionDetails.h"
#include "MyUtils.h"
#include "GenUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	m_StrAcquiringAmtFld = _T("");
	m_StrAcquiringCurrencyFld = _T("");
	m_StrCardNameFld = _T("");
	m_StrCategoryCodeFld = _T("");
	m_StrCVCResultCodeFld = _T("");
	m_StrServiceCodeFld = _T("");
	m_StrStopRegionFld = _T("");
	m_StrReasonCodeFld = _T("");
	m_StrPurgeDateFld = _T("");
	m_StrLocationFld = _T("");
	m_StrOriginalTxnAmount = _T("");
	m_StrEarMarkedCurrency = _T("");
	m_OrgTxnAmtCurrency = _T("");
	m_StrECI = _T("");
	m_StrOriginalECI_Reason = _T("");
	m_StrEarMarkedAmount = _T("");
	m_StrPosData = _T("");
	m_StrBinLow = _T("");
	m_StrBdoMercAcqTx = _T("");
	m_StrAuthProfile = _T("");
	m_StrAcquiringCountry = _T("");
	m_StrVisaMerchantIdentifier = _T("");
	m_StrDigitalCommProgramIndicator = _T("");
    m_StrDigitalEntryIdentifier = _T("");
	m_StrCUPIndicatorIDFld = _T("");
	//}}AFX_DATA_INIT
}


void CTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_ARQC_Present, m_ChkARQCPresent);
	DDX_Control(pDX, IDC_ARQC_VALIDATED, m_ChkArqcValidated);
	DDX_Control(pDX, IDC_ATP_STIP, m_ChkAtpStip);
	DDX_Control(pDX, IDC_TO_ATP_STIP, m_ChkTOAtpStip);
	DDX_Control(pDX, IDC_CAVV_Validated, m_ChkCAVVValidated);
	DDX_Control(pDX, IDC_CAVV_Present, m_ChkCAVVPresent);
	DDX_Control(pDX, IDC_VOICE_VOD, m_ChkVoice);
	DDX_Control(pDX, IDC_SCHEME_STIP, m_ChkSchemeStip);
	DDX_Control(pDX, IDC_PIN_Validated, m_ChkPINValidated);
	DDX_Control(pDX, IDC_PIN_Present, m_ChkPINPresent);
	DDX_Control(pDX, IDC_MERCHANT_OMD, m_ChkOMD);
	DDX_Control(pDX, IDC_HOST_PROCESSED, m_ChkHostProcessed);
	DDX_Control(pDX, IDC_CVV2_Validated, m_ChkCVV2Validated);
	DDX_Control(pDX, IDC_CVV2_Present, m_ChkCVV2Present);
	DDX_Control(pDX, IDC_CVV_Validated, m_ChkCvvValidated);
	DDX_Control(pDX, IDC_CVV_PRESENT, m_ChkCVVPresent);
		DDX_Control(pDX, IDC_FG_Result_FLD, m_CntlFGResultFld);
	DDX_Text(pDX, IDC_ACQUIRING_INST_ID_FLD, m_StrAcquiringInstIDFld);
	DDX_Text(pDX, IDC_AUTH_ID_RESPONSE_FLD, m_StrAuthIDResponseFld);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDX_Text(pDX, IDC_FORWARDING_INSTITUTION_ID_FLD, m_StrForwardingInstIDFld);
	DDX_Text(pDX, IDC_INVOICE_NUMBER_FLD, m_StrInvoiceNumberFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD1, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_POS_CONDITION_CODE_FLD, m_StrPOSConditionCodeFld);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE_FLD, m_StrPOSEntryModeFld);
	DDX_Text(pDX, IDC_PROCESSING_CODE_FLD, m_StrProcessingCodeFld);
	DDX_Text(pDX, IDC_RESPONSE_TEXT_FLD, m_StrResponseTextFld);
	DDX_Text(pDX, IDC_RETRIEVAL_REF_NUM_FLD, m_StrRetrievalRefNumFld);
	DDX_Text(pDX, IDC_SYSTEM_TRACE_NUMBER_FLD, m_StrSystemTraceNumberFld);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_TRAN_AMOUNT_FLD, m_StrTranAmountFld);
	DDX_Text(pDX, IDC_TRAN_DATE_FLD, m_StrTranDateFld);
	DDX_Text(pDX, IDC_TRAN_TIME_FLD, m_StrTranTimeFld);
	DDX_Text(pDX, IDC_TRAN_TYPE_FLD, m_StrTranTypeFld);
	DDX_Text(pDX, IDC_TRAN_MESSAGE_TYPE, m_StrTranMessageTypeFld);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_TEXT_FLD, m_StrTextFld);
	DDX_Text(pDX, IDC_EXP_DATE_FLD, m_StrExpirationDateFld);
	DDX_Text(pDX, IDC_CREDIT_LIMIT_FLD, m_StrCreditLimitFld);
	DDX_Text(pDX, IDC_OUTSTANDING_BALANCE_FLD, m_StrOutstandingBalanceFld);
	DDX_Text(pDX, IDC_CURRENCY_CODE_FLD, m_StrCurrencyCodeFld);
	DDX_Text(pDX, IDC_OPERATOR_ID_FLD, m_StrOperatorIDFld);
	DDX_Text(pDX, IDC_SYSTEM_DATE_FLD, m_StrSystemDateFld);
	DDX_Text(pDX, IDC_EMV_FLD, m_StrEmvFld);
	DDX_Text(pDX, IDC_FG_Result_FLD, m_StrFGResultFld);
	DDX_Text(pDX, IDC_GROSS_AMOUNT,  m_StrGrossAmountFld);
	DDX_Text(pDX, IDC_MONTHLY_AMORT, m_StrMonthlyAmortFld);
	DDX_Text(pDX, IDC_TERM_LENGTH,   m_StrTermLengthFld);
	DDX_Text(pDX, IDC_FACTOR,        m_StrFactorFld);
	DDX_Text(pDX, IDC_ACQUIRING_AMT_FLD, m_StrAcquiringAmtFld);
	DDX_Text(pDX, IDC_ACQUIRING_CURRENCY_FLD, m_StrAcquiringCurrencyFld);
	DDX_Text(pDX, IDC_CARD_NAME_FLD, m_StrCardNameFld);
	DDX_Text(pDX, IDC_CATEGORY_CODE_FLD, m_StrCategoryCodeFld);
	DDX_Text(pDX, IDC_CVC_RESULT_CODE_FLD, m_StrCVCResultCodeFld);
	DDX_Text(pDX, IDC_SERVICE_CODE_FLD, m_StrServiceCodeFld);
	DDX_Text(pDX, IDC_STOP_REGION_FLD, m_StrStopRegionFld);
	DDX_Text(pDX, IDC_REASON_CODE_FLD, m_StrReasonCodeFld);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD, m_StrPurgeDateFld);
	DDX_Text(pDX, IDC_LOCATION_FLD, m_StrLocationFld);
	DDX_Text(pDX, IDC_Orginial_Txn_Amount, m_StrOriginalTxnAmount);
	DDX_Text(pDX, IDC_ORIGINAL_CURRENCY_CODE, m_StrEarMarkedCurrency);
	DDX_Text(pDX, IDC_Original_Txn_Amt_Currency, m_OrgTxnAmtCurrency);
	DDX_Text(pDX, IDC_ECI, m_StrECI);
	DDX_Text(pDX, IDC_ORI_ECI_REASOPN, m_StrOriginalECI_Reason);
	DDX_Text(pDX, IDC_EARMARKED_AMT, m_StrEarMarkedAmount);
	DDX_Text(pDX, IDC_POS_DATA, m_StrPosData);
	DDX_Text(pDX, IDC_BIN_LOW, m_StrBinLow);
	DDX_Text(pDX, IDC_BDO_MERC_ACQ_TX, m_StrBdoMercAcqTx);
	DDX_Text(pDX, IDC_AUTH_PROFILE, m_StrAuthProfile);
	DDX_Text(pDX, IDC_Acquiring_Country, m_StrAcquiringCountry);
	DDX_Text(pDX, IDC_VISA_MERCHANT_IND, m_StrVisaMerchantIdentifier);
	DDX_Text(pDX, IDC_DIGITAL_COMM_PRG_IND, m_StrDigitalCommProgramIndicator);
	DDX_Text(pDX, IDC_DIGITAL_ENTRY_IDENTIFIER, m_StrDigitalEntryIdentifier);
	DDX_Text(pDX, IDC_CUP_INDICATOR_ARC, m_StrCUPIndicatorIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransactionDetails, CDialog)
	//{{AFX_MSG_MAP(CTransactionDetails)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails message handlers
BOOL CTransactionDetails::PopulateTranDialogFromDatabase ()
{
	CMyUtils myUtilsObject;
	CHAR     strErrorMessage[200] = "";	
	CHAR     strFactor[8] = "";
	CString  temp;
	CString  strResponse;
	double   nFactor;
	
	int rcode = txdsapi_get_record_by_subtype2 ( (pBYTE)&sTranRecord, sizeof (sTranRecord),ARCH01_DATA, TLF01_DATA, strErrorMessage);
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
	
	m_StrAcquiringInstIDFld   =  sTranRecord.acquiring_id;
	m_StrAuthIDResponseFld    =  sTranRecord.auth_number;
	m_StrCardNumberFld        =  sTranRecord.card_num;
	m_StrForwardingInstIDFld  =  sTranRecord.forwarding_institution_id;
	m_StrHostTimeFld          =  sTranRecord.host_finish_time;
	m_StrMerchantIDFld        =  sTranRecord.merchant_id;
	m_StrPOSConditionCodeFld  =  sTranRecord.pos_condition_code; 
	m_StrPOSEntryModeFld      =  sTranRecord.pos_entry_mode;
	m_StrProcessingCodeFld    =  sTranRecord.processing_code;
	m_StrResponseCodeFld      =  sTranRecord.response_code;
	m_StrResponseTextFld      =  sTranRecord.response_text;
	m_StrRetrievalRefNumFld   =  sTranRecord.retrieval_ref_num;
	m_StrSystemTraceNumberFld =  sTranRecord.sys_trace_audit_num;
	m_StrTerminalIDFld        =  sTranRecord.terminal_id;
	m_StrFGResultFld          = "";
	//		checkMaskCardNumber(); //To mask the card number
	
	if(this->m_bMaskCard == 1)
	{
		int len = m_StrCardNumberFld.GetLength();
		if(len >= 12)
		{
			char *ptr = m_StrCardNumberFld.GetBuffer(len);
			memcpy(ptr + 6, "******", 6);
			m_StrCardNumberFld.ReleaseBuffer();
		}
	}
	temp = sTranRecord.total_amount ;
	m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	
	temp = sTranRecord.credit_limit;
	m_StrCreditLimitFld = myUtilsObject.DisplayAmount(temp);
	
	temp = sTranRecord.outstanding_balance;
	m_StrOutstandingBalanceFld = myUtilsObject.DisplayAmount(temp);
	
	m_StrExpirationDateFld =  sTranRecord.exp_date;
	char tt[10] = {0};
	memcpy(tt, sTranRecord.date_yyyymmdd, 9);
	m_StrTranDateFld		  =  tt;
	m_StrTranTimeFld       =  sTranRecord.time_hhmmss;
	m_StrTextFld           =  sTranRecord.voice_auth_text;
	m_StrCurrencyCodeFld   =  sTranRecord.currency_code;
	m_StrOperatorIDFld     =  sTranRecord.operator_id;
	m_StrSystemDateFld     =  sTranRecord.system_date;
	m_StrPurgeDateFld	  =  sTranRecord.purge_date;
	m_StrCUPIndicatorIDFld = sTranRecord.product_code;
	
		if( (0 == strcmp((char *)sTranRecord.acquirer_id,"nciftr"))||
		(0 == strcmp((char *)sTranRecord.authorizing_host_queue,"nciftr")))
	{
		m_StrTextFld += (CString)(char *)"\r\n";
		m_StrTextFld += (CString)(char *) "F48 <";
		m_StrTextFld += (CString)(char *) sTranRecord.cvc_data;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *) "F60 <";
		if(0 == strncmp((CString)(char *)sTranRecord.message_type,"0320",4))
		{	
			m_StrTextFld += (CString)(char *) sTranRecord.orig_message;
			m_StrTextFld += (CString)(char *) sTranRecord.orig_sys_trace_num;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *)sTranRecord.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.message_type,"0200",4))
		{
			m_StrTextFld += (CString)(char *) sTranRecord.orig_amount;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		m_StrTextFld += (CString)(char *) "F61 <";
		m_StrTextFld += (CString)(char *)sTranRecord.product_code;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F62 <";
		m_StrTextFld += (CString)(char *) sTranRecord.invoice_number;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F63 <";
		m_StrTextFld += (CString)(char *)sTranRecord.num_sales;
		m_StrTextFld += (CString)(char *)sTranRecord.sales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.num_refunds;
		m_StrTextFld += (CString)(char *)sTranRecord.refund_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.num_dbsales;
		m_StrTextFld += (CString)(char *)sTranRecord.dbsales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.num_dbrefunds;
		m_StrTextFld += (CString)(char *)sTranRecord.dbrefund_amount;
		m_StrTextFld += (CString)(char *)" > \r\n";
	
	
	}
    
	else
	{
		if(0 == strncmp((CString)(char *)sTranRecord.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *)"\r\n";
			m_StrTextFld += (CString)(char *)"F61 <";
			m_StrTextFld += (CString)(char *)sTranRecord.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *)"F63 <";
			m_StrTextFld += (CString)(char *)sTranRecord.num_sales;
			m_StrTextFld += (CString)(char *)sTranRecord.sales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.num_refunds;
			m_StrTextFld += (CString)(char *)sTranRecord.refund_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.num_dbsales;
			m_StrTextFld += (CString)(char *)sTranRecord.dbsales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.num_dbrefunds;
			m_StrTextFld += (CString)(char *)sTranRecord.dbrefund_amount;
			m_StrTextFld += " > \r\n";
		}
			
	}
	
	
   	/*************************************************************************************/
	
	if( strcmp( (char *) sTranRecord.host_start_time, "5.000") == 0)
	{
		m_StrEarMarkedCurrency = "PHP";
		temp = sTranRecord.product_codes[9].amount ;
		m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);
	}
	else if( strcmp( (char *) sTranRecord.host_start_time, "7.000") == 0)
	{
		m_StrEarMarkedCurrency = "USD";
		temp = sTranRecord.product_codes[9].amount ;
		m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);
	}
	else 
	{
		m_StrEarMarkedCurrency	= sTranRecord.currency_code;
		temp = sTranRecord.total_amount ;
		m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);
		
	}
	
	m_StrBdoMercAcqTx = sTranRecord.product_codes[4].code;
	m_StrAuthProfile  = sTranRecord.product_codes[3].code;
	m_StrBinLow		  = sTranRecord.product_codes[2].code;
	
	//m_StrOriginalTxnAmount = sTranRecord.total_amount;
	//m_OrgTxnAmtCurrency = sTranRecord.currency_code;
	m_StrECI = sTranRecord.profile;
	if(0 == strcmp((char *)sTranRecord.pos_condition_code,"59"))
	{
		CString s2 = _T("2");
		m_StrECI= s2 + sTranRecord.profile;
	
	}
	char temp_StrOriginalECI_Reason[4] = {0};

	CString temp_OriginalECI_Reason = sTranRecord.product_codes[14].code ;
	m_StrOriginalECI_Reason = temp_OriginalECI_Reason.Mid(0,3) ;
	
    int OriginalECI_Reason_len=m_StrOriginalECI_Reason.GetLength();
	if(OriginalECI_Reason_len > 0)
	{
		CString s1 = _T("/"); 
		m_StrOriginalECI_Reason = m_StrOriginalECI_Reason + s1 + sTranRecord.product_codes[14].quantity;

    }

	/*m_StrMCC =sTranRecord.category_code;
	m_StrMerchantName =sTranRecord.card_holder_name;*/
	
	// Store AMEX POS Data from tlf details feild product_codes[16].amount
	m_StrPosData = sTranRecord.product_codes[16].amount ;
	
	m_StrAcquiringCountry = sTranRecord.currency_code;
	
	if(strcmp((char *)sTranRecord.handler_queue,"dcpiso") != 0)
	{
		if(strcmp((char *)sTranRecord.product_codes[4].code,"ISSUE") == 0)
		{
			if(sTranRecord.nfi_seq_nbr != 0)
			{
				m_OrgTxnAmtCurrency = sTranRecord.nfi_seq_nbr;
				m_StrOriginalTxnAmount = sTranRecord.sales_amount;
			}
			
		}
		else
		{
			m_StrOriginalTxnAmount = sTranRecord.total_amount;
			m_OrgTxnAmtCurrency = sTranRecord.currency_code;
		}
	}
	else
	{
		m_StrOriginalTxnAmount = sTranRecord.total_amount;
		//m_StrOriginalTxnAmount = 45678;
		m_OrgTxnAmtCurrency = sTranRecord.currency_code;
	}

	m_StrVisaMerchantIdentifier  = sTranRecord.product_codes[4].amount;
    m_StrDigitalCommProgramIndicator = sTranRecord.product_codes[8].code;
    m_StrDigitalEntryIdentifier = sTranRecord.product_codes[5].amount;
	
	
	/*************************************************************************************/
	
	
	/* Deferred Payment Data */
	if ( 0x00 != sTranRecord.deferred_term_length[0] )
	{
		temp = sTranRecord.def_gross_amt;
		m_StrGrossAmountFld =  myUtilsObject.DisplayAmount(temp);
		
		temp = sTranRecord.monthly_amort_amt;
		m_StrMonthlyAmortFld =  myUtilsObject.DisplayAmount(temp);
		
		m_StrTermLengthFld =  sTranRecord.deferred_term_length;
		
		/* Convert Def Factor from nnnnnnn to nnn.nnnn (0050000 = 5%). */
		m_StrFactorFld = sTranRecord.deferred_factor;
		nFactor = atof( m_StrFactorFld ) / 10000.0;
		sprintf( strFactor, "%8.4f", nFactor );
		m_StrFactorFld = strFactor;
	}
	
	m_StrTranMessageTypeFld = sTranRecord.message_type; 
	//m_StrTranTypeFld =  sTranRecord.tx_key;
	(CString)m_StrTranTypeFld = (CString)((char *) sTranRecord.tx_key);
	//if ( m_StrTranTypeFld > 100 )
	if ( atoi(m_StrTranTypeFld) > 100 )	
	strResponse = " RESPONSE";
	else
		strResponse = "";
	
	switch (sTranRecord.tx_key)
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
		{ 
			if ( strcmp ((char *)sTranRecord.processing_code, "180000" ) == 0)
			{
				m_StrTranTypeFld = "QUASI CASH" + strResponse;
			}
			else
			{
				if ( ( strcmp((const char *)(sTranRecord.voice_auth), "Y") == 0) ||
					( strcmp((const char *)(sTranRecord.voice_auth), "y") == 0) )
				{
					m_StrTranTypeFld = "VOICE AUTHORIZATION" + strResponse; 
				}
				else
				{
					m_StrTranTypeFld = "AUTHORIZATION" + strResponse; 
				}
			}
		}
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
		
		m_StrTranTypeFld = "REVERSAL - " ;
		
		if ( strcmp ((char *)sTranRecord.processing_code, "700000" ) == 0)
            m_StrTranTypeFld +="DEFERRED";
		
		else if( strcmp ((char *)sTranRecord.processing_code, "180000" ) == 0)
            m_StrTranTypeFld +="QUASI CASH";
		
		else if((strcmp((char *)sTranRecord.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0100" ) == 0))
            m_StrTranTypeFld +="AUTHORIZATION";
		
		else if((strcmp((char *)sTranRecord.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="PURCHASE";
		
		else if((strcmp((char *)sTranRecord.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE COMPLETION";
		
		else if( strcmp ((char *)sTranRecord.processing_code, "200000" ) == 0) 
            m_StrTranTypeFld +="REFUND";
		
		else if( strcmp ((char *)sTranRecord.processing_code, "010000" ) == 0) 
            m_StrTranTypeFld +="CASH";
		
		else if((strcmp((char *)sTranRecord.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID SALE";
		
		else if((strcmp((char *)sTranRecord.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID REFUND";
		
		else if((strcmp((char *)sTranRecord.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE ADJUSTMENT";
		
		else if((strcmp((char *)sTranRecord.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="REFUND";
		
		if ( sTranRecord.orig_message[0] == 0x00 )
		{
            /* No original transaction for this reversal. */
            m_StrTranTypeFld +=" (without updates)";
		}
		else
            m_StrTranTypeFld +=" (with updates)";
		
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
   
   m_StrInvoiceNumberFld =  sTranRecord.invoice_number;
   
   // m_StrTranDateFld   = m_StrTranDateFld.Right(2)   + "/" + m_StrTranDateFld.Mid(4,2)   + "/" + m_StrTranDateFld.Left(4);
   m_StrTranTimeFld   = m_StrTranTimeFld.Left(2)    + ":" + m_StrTranTimeFld.Mid(2,2)   + ":" + m_StrTranTimeFld.Right(2);
   m_StrSystemDateFld = m_StrSystemDateFld.Right(2) + "/" + m_StrSystemDateFld.Mid(4,2) + "/" + m_StrSystemDateFld.Left(4);
   
   m_StrCategoryCodeFld			= sTranRecord.category_code;
   m_StrServiceCodeFld			= sTranRecord.service_code;
   m_StrAcquiringAmtFld			= sTranRecord.orig_amount;
   m_StrAcquiringCurrencyFld	= sTranRecord.nfi_seq_nbr;
   m_StrCVCResultCodeFld		= sTranRecord.security_response_code;
   
   /*newwwwwwwwwwwwwwww********************************************************************************************************/
   m_ChkHostProcessed.SetCheck(0); 
   m_ChkSchemeStip.SetCheck(0);
   m_ChkAtpStip.SetCheck(0);
   if(strcmp((char *)sTranRecord.product_codes[3].quantity, "19") == 0)
   {
	   m_ChkTOAtpStip.SetCheck(1);
   }
   else if(strcmp((char *)sTranRecord.product_codes[1].code, STANDIN) == 0)
   {
	   m_ChkAtpStip.SetCheck(1);
   }
   else
   {
	   m_ChkHostProcessed.SetCheck(1);
   }
   
   if( (sTranRecord.tx_key == AUTH_AUTHORIZATION_OVERRIDE) || (sTranRecord.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) )
   {
	   m_ChkVoice.SetCheck(1);
   }
   else
   {
	   m_ChkVoice.SetCheck(0);
   }
   
   if( strlen((char *)sTranRecord.nbr_of_prod_codes) > 0)
   {
	   m_ChkOMD.SetCheck(1);
   }
   else
   {
	   m_ChkOMD.SetCheck(0);
   }
   
   if( strlen((char *)sTranRecord.cvc_data) > 0)
   {
	   m_ChkCVVPresent.SetCheck(1);
	   m_ChkCVV2Present.SetCheck(1);
   }
   else
   {
	   m_ChkCVVPresent.SetCheck(0);
	   m_ChkCVV2Present.SetCheck(0);
   }
   
   if( (strcmp((char *)sTranRecord.security_response_code,"00") == 0) && ( (char *)sTranRecord.cvc_data != '\0') )
   {
	   m_ChkCvvValidated.SetCheck(1);
	   m_ChkCVV2Validated.SetCheck(1);
   }
   else
   {
	   m_ChkCvvValidated.SetCheck(0);
	   m_ChkCVV2Validated.SetCheck(0);
   }
   
   
   if ( 0 == strlen((char *)sTranRecord.pin_block))
	   
   {
	   m_ChkPINPresent.SetCheck(0);
   }
   else
   {
	   m_ChkPINPresent.SetCheck(1);
   }
   
   if( (strcmp((char *)sTranRecord.security_response_code,"00") == 0) && ( 0 == strlen((char *)sTranRecord.pin_block)) )
   {
	   m_ChkPINValidated.SetCheck(1);
   }
   else
   {
	   m_ChkPINValidated.SetCheck(0);
   }
   
   if((char *)sTranRecord.pos_entry_mode == "0051" || (char *)sTranRecord.pos_entry_mode == "0071" ||
	   (char *)sTranRecord.pos_entry_mode == "0081")
   {
	   m_ChkARQCPresent.SetCheck(1);
   }
   else
   {
	   m_ChkARQCPresent.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.response_code,"00") == 0)
   {
	   m_ChkArqcValidated.SetCheck(1);
   }
   else
   {
	   m_ChkArqcValidated.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.nfi_end_date,"1") == 0)
   {
	   m_ChkCAVVPresent.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVPresent.SetCheck(0);
   }
   
   if( (strcmp((char *)sTranRecord.security_response_code,"00") == 0) && (strcmp((char *)sTranRecord.nfi_end_date,"1") == 0))
   {
	   m_ChkCAVVValidated.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVValidated.SetCheck(0);
   }
   
   /*   
   ***********************************m_CntlFGResultFld*/
   /*********************************************************************************************************/
   
   
   UpdateData(FALSE);
   
   return TRUE;
}

BOOL CTransactionDetails::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateTitleWithVersion(this);
CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 13;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	PopulateTranDialogFromDatabase();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
