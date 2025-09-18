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
* Revision History:
*  
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\tran_lookup\transactiondetails.cpp  $

  Rev 1.15   Nov 22 2005 09:15:06   dirby
  1.  Added Deferred Payment fields to Detail window.  SCR 16289
  2.  Made Tran Type more descriptive for EMV 0320 and 0340.  SCR 16288
  
	Rev 1.14   Jul 18 2005 12:56:22   lmitchel
	Implement Fraud Guard search criterea and resend failed transactions
	
      Rev 1.13   Jun 15 2005 10:32:16   chuang
	  Fix scr 16008. Voice Authorization transactions should indicate they are Voice.
	  
		Rev 1.12   Jun 14 2005 17:32:18   chuang
		Version 4.4.1.2. Fix SCR 15819, add utility to view EMV Raw Data.
		
		  Rev 1.11   Sep 28 2004 16:43:16   dirby
		  Changed label for Reload transactions to be CASH LOAD.  This
		  is so the label on Tran Lookup matches that on the terminals.
		  This consistency will avoid confusion when operators talk to merchants.
		  SCR 12580
		  
			Rev 1.10   Aug 05 2004 12:26:02   lmitchel
			PopulateTranDialogFromDatabase():  Mapped transaction type text description
			to Transaction Type Dialog Box.  SCR1287
			
			  Rev 1.9   Jun 25 2004 14:49:44   dirby
			  Corrected bug in tran type field for reversals.  To distinguish specific
			  reversal types (Sale, refund,...) need to use orig_message, not
			  message_type.  Also added text to tran type field for reversals to
			  indicate if an original txn exists for the reversal.
			  SCR 1416
			  
				Rev 1.8   Apr 29 2002 14:12:54   dirby
				Added transaction type of Card Update for the detail screen.
				SCR 698
				
				  Rev 1.7   Jun 08 2001 11:39:12   SYARLAGA
				  Broke down the reversals into distinct categories
				  
					Rev 1.6   Mar 14 2000 10:44:20   iarustam
					SCR #n475,478,479,480
					
					  Rev 1.5   Mar 02 2000 17:13:32   iarustam
					  SCR 462
					  
						Rev 1.4   Feb 01 2000 16:54:42   iarustam
						SCR # 349,350
						
						  Rev 1.3   Jun 11 1999 14:16:58   iarustam
						  Bug fixes
						  
							Rev 1.2   May 24 1999 15:11:14   iarustam
							Bug fixes
							
							  Rev 1.1   May 13 1999 15:05:04   iarustam
							  Bug fixes
							  
								Rev 1.0   Apr 15 1999 17:03:00   iarustam
								Initial Release
								
								  Rev 1.1   Mar 30 1999 17:40:00   iarustam
								  Bug fixes
								  
									Rev 1.0   Feb 23 1999 11:41:24   iarustam
									Initial Release
									
									  Rev 1.0   Feb 17 1999 10:10:12   iarustam
									  Initial Release
									  
										Rev 1.1   Dec 18 1998 13:44:58   iarustam
										
										  *
************************************************************************************/
// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "TranSearch.h"
#include "TransactionDetails.h"
#include "Fraud_Txn_Details.h"
#include "MyUtils.h"
#include "GenUtil.h"
#include "TranInquiry.h"

#define MC13_OCT2023 1
#define MC01_APR2024 1
#define MC02_APR2024 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTransactionDetails, CPropertyPage)

extern BOOL m_bTran;
extern BOOL m_bTran_stip;
extern BOOL m_bFGuard;

BYTE Transactionid[21] = {0};
BYTE Handler_queue[7]  = {0};

CTranInquiry *pCTranInquiry;
extern int m_INTSTIPSearchCheck;

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails dialog


CTransactionDetails::CTransactionDetails() : CPropertyPage(CTransactionDetails::IDD)
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
	m_StrPOSConditionCodeFld_1 = _T("");
	m_StrPOSEntryModeFld = _T("");
	m_StrPOSEntryModeFld_1 = _T("");
	m_StrProcessingCodeFld = _T("");
	m_StrProcessingCodeFld_1 = _T("");
	m_StrResponseTextFld = _T("");
	m_StrRetrievalRefNumFld = _T("");
	m_StrSystemTraceNumberFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrTranAmountFld = _T("");
	m_StrReplacementAmountFld = _T("");
	m_StrTranDateFld = _T("");
	m_StrTranTimeFld = _T("");
	m_StrTranTypeFld = _T("");
	m_StrTranMessageTypeFld = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrResponseCodeFldbyScheme = _T("");
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
	/*m_StrBdoMercAcqTx = _T("");*/
	m_StrBinLow = _T("");
	m_StrEarMarkedAmount = _T("");
	m_StrEarMarkedCurrency = _T("");
	m_StrAuthProfile = _T("");
	m_StrServiceCode = _T("");
	m_StrOriginalTxnAmount = _T("");
	m_OrgTxnAmtCurrency = _T("");
	m_StrECI = _T("");
	m_StrECI_1 = _T("");
	m_StrOriginalECI_Reason =_T("");
	m_StrMerchantName = _T("");
	m_StrMCC = _T("");
	m_StrPosData = _T("");
	m_StrAcquiringCountry = _T("");
	m_bMaskCardNumbers = false;
	m_StrVisaMerchantIdentifier = _T("");
	m_StrDigitalCommProgramIndicator = _T("");
    m_StrDigitalEntryIdentifier = _T("");
	m_StrCUPIndicatorIDFld = _T("");
	m_StrEarmarkedamount1	= _T("");
	m_StrEarmarkedcurrency	= _T("");
	m_StrBillingamount	= _T("");
	m_StrBillingCurrency	= _T("");
	m_StrNRID = _T("");
	m_StrActionCode = _T("");
	m_StrATP_sbatchtime = _T("");
	m_StrExtCondCode = _T("");
	m_Strmrasenttime = _T("");
	m_StrHostNameFld = _T("");
	m_StrNii = _T("");
	m_StrSystemDateTimeFld = _T("");
	//}}AFX_DATA_INIT
	
	m_bViewEmvData = FALSE;
}


void CTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_ARQC_VALIDATED, m_ChkArqcValidated);
	DDX_Control(pDX, IDC_CAVV_Validated, m_ChkCAVVValidated);
	DDX_Control(pDX, IDC_CAVV_Present, m_ChkCAVVPresent);
	DDX_Control(pDX, IDC_ARQC_Present, m_ChkARQCPresent);
	DDX_Control(pDX, IDC_PIN_Validated, m_ChkPINValidated);
	DDX_Control(pDX, IDC_PIN_Present, m_ChkPINPresent);
	/*DDX_Control(pDX, IDC_CVV2_Validated, m_ChkCVV2Validated);
	DDX_Control(pDX, IDC_CVV2_Present, m_ChkCVV2Present);
	DDX_Control(pDX, IDC_CVV_Validated, m_ChkCvvValidated);
	DDX_Control(pDX, IDC_CVV_PRESENT, m_ChkCVVPresent);*/
	DDX_Control(pDX, IDC_HOST_PROCESSED, m_ChkHostProcessed);
	DDX_Control(pDX, IDC_VOICE_VOD, m_ChkVoice);
	DDX_Control(pDX, IDC_SCHEME_STIP, m_ChkSchemeStip);
	DDX_Control(pDX, IDC_MERCHANT_OMD, m_ChkOMD);
	DDX_Control(pDX, IDC_ATP_STIP, m_ChkAtpStip);
	DDX_Control(pDX, IDC_TO_ATP_STIP, m_ChkTOAtpStip);
	DDX_Control(pDX, IDC_FG_Result_FLD, m_CntlFGResultFld);
	DDX_Text(pDX, IDC_ACQUIRING_INST_ID_FLD, m_StrAcquiringInstIDFld);
	DDX_Text(pDX, IDC_AUTH_ID_RESPONSE_FLD, m_StrAuthIDResponseFld);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDX_Text(pDX, IDC_FORWARDING_INSTITUTION_ID_FLD, m_StrForwardingInstIDFld);
	DDX_Text(pDX, IDC_INVOICE_NUMBER_FLD, m_StrInvoiceNumberFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD1, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_POS_CONDITION_CODE_FLD, m_StrPOSConditionCodeFld);
	DDX_Text(pDX, IDC_POS_CONDITION_CODE_FLD_1, m_StrPOSConditionCodeFld_1);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE_FLD, m_StrPOSEntryModeFld);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE_FLD_1, m_StrPOSEntryModeFld_1);
	DDX_Text(pDX, IDC_PROCESSING_CODE_FLD, m_StrProcessingCodeFld);
	DDX_Text(pDX, IDC_PROCESSING_CODE_FLD_1, m_StrProcessingCodeFld_1);
	DDX_Text(pDX, IDC_RESPONSE_TEXT_FLD, m_StrResponseTextFld);
	DDX_Text(pDX, IDC_RETRIEVAL_REF_NUM_FLD, m_StrRetrievalRefNumFld);
	DDX_Text(pDX, IDC_SYSTEM_TRACE_NUMBER_FLD, m_StrSystemTraceNumberFld);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_TRAN_AMOUNT_FLD, m_StrTranAmountFld);
	DDX_Text(pDX, IDC_REPLACEMENT_AMOUNT_FLD, m_StrReplacementAmountFld);
	DDX_Text(pDX, IDC_TRAN_DATE_FLD, m_StrTranDateFld);
	DDX_Text(pDX, IDC_TRAN_TIME_FLD, m_StrTranTimeFld);
	DDX_Text(pDX, IDC_TRAN_TYPE_FLD, m_StrTranTypeFld);
	DDX_Text(pDX, IDC_MESSAGE_TYPE, m_StrTranMessageTypeFld);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLDSCHEME, m_StrResponseCodeFldbyScheme);
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
	/*DDX_Text(pDX, IDC_BDO_MERC_ACQ_TX, m_StrBdoMercAcqTx);*/
	DDX_Text(pDX, IDC_BIN_LOW, m_StrBinLow);
	DDX_Text(pDX, IDC_EARMARKED_AMT, m_StrEarMarkedAmount);
	DDX_Text(pDX, IDC_ORIGINAL_CURRENCY_CODE, m_StrEarMarkedCurrency);
	DDX_Text(pDX, IDC_AUTH_PROFILE, m_StrAuthProfile);
	DDX_Text(pDX, IDC_SERVICE_CODE, m_StrServiceCode);
	DDX_Text(pDX, IDC_Orginial_Txn_Amount, m_StrOriginalTxnAmount);
	DDX_Text(pDX, IDC_Original_Txn_Amt_Currency, m_OrgTxnAmtCurrency);
	DDX_Text(pDX, IDC_ECI, m_StrECI);
	DDX_Text(pDX, IDC_ECI_1, m_StrECI_1);
	DDX_Text(pDX, IDC_ORI_ECI_REASON, m_StrOriginalECI_Reason);
	DDX_Text(pDX, IDC_Merchant_Name, m_StrMerchantName);
	DDX_Text(pDX, IDC_MCC, m_StrMCC);
	DDX_Text(pDX, IDC_POS_DATA, m_StrPosData);
	DDX_Text(pDX, IDC_Acquiring_Country, m_StrAcquiringCountry);
	DDX_Text(pDX, IDC_VISA_MERCHANT_IND, m_StrVisaMerchantIdentifier);
	DDX_Text(pDX, IDC_DIGITAL_COMM_PRG_IND, m_StrDigitalCommProgramIndicator);
	DDX_Text(pDX, IDC_DIGITAL_ENTRY_IDENTIFIER, m_StrDigitalEntryIdentifier);
	DDX_Text(pDX, IDC_CUP_INDICATOR, m_StrCUPIndicatorIDFld);
	DDX_Text(pDX, IDC_SETTLEMENT_DATE, m_StrSettlementDateFld);
	
	DDX_Text(pDX, IDC_EARMARKED_AMT2, m_StrEarmarkedamount1);
	DDX_Text(pDX, IDC_EARMARKED_CURRENCY2, m_StrEarmarkedcurrency);
	DDX_Text(pDX, IDC_SETTLEMENT_AMT, m_StrBillingamount);
	DDX_Text(pDX, IDC_SETTELMENT_CURRENCY, m_StrBillingCurrency);

	DDX_Text(pDX, IDC_NRID, m_StrNRID);
	DDX_Text(pDX,IDC_ACTION_CODE,m_StrActionCode);
	DDX_Text(pDX,IDC_SBATCH_TIME,m_StrATP_sbatchtime);
	DDX_Text(pDX,IDC_EXTCOND_CODE,m_StrExtCondCode);
	DDX_Text(pDX,IDC_MRA_SENT_TIME,m_Strmrasenttime);
	DDX_Text(pDX,IDC_HOSTNAME,m_StrHostNameFld);
	DDX_Text(pDX,IDC_NII,m_StrNii);
	DDX_Text(pDX, IDC_SYSTEM_DATE_TIME, m_StrSystemDateTimeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransactionDetails, CPropertyPage)
//{{AFX_MSG_MAP(CTransactionDetails)
	//ON_BN_CLICKED(IDC_FRAUD_TXN_DETAILS, OnFraudTxnDetailsButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails message handlers

BOOL CTransactionDetails::OnInitDialog() 
{
	
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 13;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	/*********************************************************************
	
	  
		// get actual size of desktop
		RECT actualDesktop;
		::GetWindowRect(GetDesktopWindow(), &actualDesktop);
		
		  
			
	/*********************************************************************/
	
	
	
	if(m_bTran)
	{
			PopulateTranDialogFromDatabase ( sTranRecord);
	}
	/*else if(m_bFGuard)
			PopulateFGuardDialogFromDatabase(sFGuardRecord);*/
	if(m_bTran_stip)
	{
		PopulateSTIPTranDialogFromDatabase(sTranRecordS);
	}
		
		UpdateData(FALSE);	
		
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTransactionDetails::PopulateTranDialogFromDatabase ( TRANS_RECORD sTranRecord)
{
	CMyUtils myUtilsObject;
	CHAR     strErrorMessage[200] = {0};	
	CHAR     strFactor[8] = {0};
	double   nFactor   = 0;
	CHAR     strMPFlag[2] ="1";
	CHAR	 Invoice_temp[9]={0};
	CString  temp  = _T("");
	CString  strResponse = _T("");
	int		 rcode = 0;
	char     cavv_buff[512] = {0};
	char * substr=NULL;
	MRA01    mra01_table = {0};
	PTSN01 sptsn01 ={0};
	CCL01	sccl01 = {0};
	char acii_rescode[5] = {0};
	char amex_rescode[4] = {0};
	char dci_rescode[4] = {0};
	char Cavv_components[7]={0};
	char Version[3]={0};
	CString  strSysDt_Tm = _T("");
	int dec_plcs = 0;
	FTLF01 sFraudRecord = {0};


	char ATN[17]={0};
	rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_DATA, strErrorMessage);
	
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




		if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			strncpy((char *)sptsn01.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id,sizeof(sptsn01.transaction_id));

	
			rcode = txdsapi_get_record ( (pBYTE)&sptsn01, sizeof (sptsn01), PTSN01_DATA, strErrorMessage);
	
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
					case PTEMSG_NOT_FOUND:
					 //m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
					break;
				}
				//m_StrMessageLbl = strErrorMessage;
				UpdateData(FALSE);
				//return FALSE;
			}
		}

		if (true == Check_If_Issuing_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if (sTranRecord.tlf01.product_codes[13].code[0] != 0x00)
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.product_codes[13].code,sizeof(sccl01.currency_value));
			}
			else if (sTranRecord.tlf01.nfi_seq_nbr[0] != 0x00)
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.nfi_seq_nbr,sizeof(sccl01.currency_value));
			}
			else
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.currency_code,sizeof(sccl01.currency_value));
			}

			rcode = txdsapi_get_record ( (pBYTE)&sccl01, sizeof (sccl01), CCL01_DATA, strErrorMessage);
	
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
					case PTEMSG_NOT_FOUND:
					 //m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
					break;
				}
				//m_StrMessageLbl = strErrorMessage;
				UpdateData(FALSE);
				//return FALSE;
			}
		}
	
	m_StrAcquiringInstIDFld   =  sTranRecord.tlf01.acquiring_id;
	m_StrAuthIDResponseFld    =  sTranRecord.tlf01.auth_number;
	m_StrCardNumberFld        =  sTranRecord.tlf01.card_num;
	m_StrForwardingInstIDFld  =  sTranRecord.tlf01.forwarding_institution_id;
	m_StrHostTimeFld          =  sTranRecord.tlf01.host_finish_time;
	m_StrMerchantIDFld        =  sTranRecord.tlf01.merchant_id;
	m_StrPOSConditionCodeFld  =  sTranRecord.tlf01.pos_condition_code; 
	m_StrPOSConditionCodeFld_1 = sTranRecord.tlf01.product_codes[4].quantity;
	m_StrPOSEntryModeFld      =  sTranRecord.tlf01.pos_entry_mode;
	m_StrProcessingCodeFld    =  sTranRecord.tlf01.processing_code;
	m_StrProcessingCodeFld_1  =  sTranRecord.tlf01.product_codes[17].amount;
	m_StrResponseCodeFld      =  sTranRecord.tlf01.response_code;
	m_StrResponseTextFld      =  sTranRecord.tlf01.response_text;
	m_StrRetrievalRefNumFld   =  sTranRecord.tlf01.retrieval_ref_num;
	m_StrSystemTraceNumberFld =  sTranRecord.tlf01.sys_trace_audit_num;
	m_StrTerminalIDFld        =  sTranRecord.tlf01.terminal_id;
	m_StrATP_sbatchtime = sTranRecord.tlf01.kek;
	m_StrHostNameFld          =  sTranRecord.tlf01.redemption_amount;

	if ((true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) ||
		 true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num)) &&
		    0 == strncmp((char *)sTranRecord.tlf01.message_type,"0220",4))
	{
			m_StrPOSEntryModeFld = sTranRecord.tlf01.product_codes[14].code ;
	}	
	strncpy(Invoice_temp,(char *)sTranRecord.tlf01.invoice_number,6);
	 m_StrInvoiceNumberFld = Invoice_temp;

	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr")||
		 0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")) &&
		 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
		 	m_StrProcessingCodeFld_1 = sTranRecord.tlf01.processing_code;
	}
	
	if(	strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"1120",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"0420",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"1420",4) == 0 )
	{
		if(true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) )
		{
			match_ISO_to_AMEX_response_code((char *)sTranRecord.tlf01.product_codes[13].quantity,amex_rescode,sTranRecord.tlf01.tx_key);
			m_StrResponseCodeFldbyScheme = amex_rescode;
		}
		else if( true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
			match_ISO_to_Diners_response_code((char *)sTranRecord.tlf01.product_codes[13].quantity,dci_rescode);
			m_StrResponseCodeFldbyScheme = dci_rescode;
		}
		else
		{
			m_StrResponseCodeFldbyScheme = sTranRecord.tlf01.product_codes[13].quantity;
		}
	}

	if(0 == strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) &&
	  true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num) &&
	   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr",5))
	{
		m_StrResponseCodeFld = "00";
	}
	
	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
		m_StrNRID = sTranRecord.tlf01.product_codes[17].code;
		m_StrNRID += "/";
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[17].amount;  
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].amount;
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].code;
	}
	//if((0==strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncdci",5)) || (0==strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"ncdciA",5)) || (0==strncmp((char *)sTranRecord.tlf01.handler_queue,"ncdciA",5)))
	else if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num)) 
	{
		if(sTranRecord.tlf01.visa_tran_id[0] != '\0')
		{
			m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
		}	
		else
		{ /* In amex txn, TRANID of transaction 0100 is stored in tlf01.visa_tran_id, but for 0220 txn TRANID stored in tlf01.mcard_banknet.
		  In dcpiso after fetching the original, TRANID of original txn is copied to tlf01.mcard_banknet from BCH20,
		  hence check tlf01.visa_tran_id first , if that is empty then check tlf01.mcard_banknet to view the TRANID in tranlookup page.
		  */
			m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
		}
	}
	else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) )
	{
		m_StrNRID = sTranRecord.tlf01.resp_source;
	}
	else if (true == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
	}
	else if(true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
	}


	if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) || true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrActionCode = sTranRecord.tlf01.product_codes[12].code;
	}

	if(true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		
		m_StrExtCondCode=sTranRecord.tlf01.product_codes[13].amount+4;
	}

	checkMaskCardNumber(); //To mask the card number
	if((true   == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num) ||
	true  == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))   &&
	0     == strncmp((char *)sTranRecord.tlf01.product_codes[3].quantity,"RP",2))
	{
		temp = sTranRecord.tlf01.orig_amount;
	}
	else
	{
		temp = sTranRecord.tlf01.total_amount;
	}
	m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);

	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	      true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) &&
		  ( sTranRecord.tlf01.product_codes[19].amount[5] == '0' ||
		    sTranRecord.tlf01.product_codes[19].amount[5] == '4' ))
	{
		m_StrTranAmountFld =  myUtilsObject.GetAmount_not_inDecimal(temp);
	}
	else
	{
		m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	}

	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	      true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) &&
		  ( sTranRecord.tlf01.product_codes[13].code[0] != 0x00))     
	{
		temp = sTranRecord.tlf01.product_codes[7].amount;
		m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	}
	else
	{
		m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	}

	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	     (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5)   ||
		  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6)) &&
		  (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3) ||
		   0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ZIMBABWE_GOLD, 3)))     
	{
		temp = sTranRecord.tlf01.product_codes[7].amount;
		m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	}

	if((true  == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num) ||
		true  == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))   &&
		0     == strncmp((char *)sTranRecord.tlf01.product_codes[3].quantity,"RP",2))
	{
		temp = _T("");
		temp = sTranRecord.tlf01.total_amount;
		m_StrReplacementAmountFld =  myUtilsObject.DisplayAmount(temp);
	}
	
	temp = sTranRecord.tlf01.credit_limit;
	m_StrCreditLimitFld = myUtilsObject.DisplayAmount(temp);
	
	temp = sTranRecord.tlf01.outstanding_balance;
	m_StrOutstandingBalanceFld = myUtilsObject.DisplayAmount(temp);
	
	m_StrExpirationDateFld =  sTranRecord.tlf01.exp_date;
	m_StrTranDateFld       =  sTranRecord.tlf01.date_yyyymmdd;
	m_StrTranTimeFld       =  sTranRecord.tlf01.time_hhmmss;
	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis3",6) ||
	   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncdci",5))
	{
	 // unwanted 00 need not be copied into text box field
	}
	else
	{
#ifdef MC13_OCT2023
		memset(cavv_buff,0x00,sizeof(cavv_buff));
		sprintf (cavv_buff,"\r\n %s  \r\n", sTranRecord.tlf01.voice_auth_text);
		m_StrTextFld = cavv_buff;
#else
	m_StrTextFld           =  sTranRecord.tlf01.voice_auth_text;
#endif
	}
	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	      true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) &&
		  ( sTranRecord.tlf01.product_codes[13].code[0] != 0x00))     
	{
		m_StrCurrencyCodeFld   =  sTranRecord.tlf01.product_codes[7].code;
	}
	else
	{
	m_StrCurrencyCodeFld   =  sTranRecord.tlf01.currency_code;
	}

	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	     (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5)   ||
		  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6)) &&
		  (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3) ||
		   0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ZIMBABWE_GOLD, 3)))     
	{
		m_StrCurrencyCodeFld   =  sTranRecord.tlf01.product_codes[7].code;
	}

	if (true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
	{	// For AMEX 3DS2.0
		m_StrOperatorIDFld = _T("");
	}
	else
	{
		m_StrOperatorIDFld = sTranRecord.tlf01.operator_id;
	}
	m_StrSystemDateFld     =  sTranRecord.tlf01.system_date;
	m_StrCUPIndicatorIDFld =  sTranRecord.tlf01.product_code;
	m_StrSettlementDateFld = sTranRecord.tlf01.settlement_date;

	strSysDt_Tm = m_StrSystemDateFld;

	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr"))||
		(0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")))
	{
		m_StrTextFld += (CString)(char *)"\r\n";
		m_StrTextFld += (CString)(char *) "F48 <";
		m_StrTextFld += (CString)(char *) sTranRecord.tlf01.cvc_data;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *) "F60 <";
		if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0320",4))
		{	
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_message;
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_sys_trace_num;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0200",4))
		{
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_amount;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}

		m_StrTextFld += (CString)(char *) "F61 <";
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.product_code;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F62 <";
		m_StrTextFld += (CString)m_StrInvoiceNumberFld;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F63 <";
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_sales;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.sales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_refunds;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.refund_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbsales;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbsales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbrefunds;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbrefund_amount;
		m_StrTextFld += (CString)(char *)" > \r\n";
	
		if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if(sptsn01.serial_number[0] != '\0')
			{		
				memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nPhysical Terminal Serial Number= %s \r\n" ,sptsn01.serial_number);
					m_StrTextFld += cavv_buff;
			}
		}
	}
    
	else
	{
		if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *)"\r\n";
			m_StrTextFld += (CString)(char *) "F48 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.cvc_data;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *) "F60 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *) "F61 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.product_code;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *)"F63 <";
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_sales;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.sales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_refunds;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.refund_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbsales;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbsales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbrefunds;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbrefund_amount;
			m_StrTextFld += (CString)(char *)" > \r\n";
		}
#if 0
		else if(Check_If_VISA_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if( strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0200",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) == 0 ||
				(strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 &&
				strncmp((char *)sTranRecord.tlf01.pos_condition_code,"59",2) == 0))
			{
				if(sTranRecord.tlf01.cvc != '\0')
				{
					sprintf (cavv_buff,"\r\n CAVV Results Code = %c  \r\n" , sTranRecord.tlf01.cvc);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n POS Environment Field = %c  \r\n" , sTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrTextFld += cavv_buff;
				}
			}

			if(sTranRecord.tlf01.operator_id[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De126.8 Transaction ID = %s%s \r" , sTranRecord.tlf01.operator_id,sTranRecord.tlf01.workstation);
					m_StrTextFld += cavv_buff;
			
			}

			if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.9 pos 1, 3-D Secure Authn Results code= %c%c \r\n" ,sTranRecord.tlf01.dcf01_retired_cwk[0], sTranRecord.tlf01.dcf01_retired_cwk[1] );
					m_StrTextFld += cavv_buff;

			}

			if(sTranRecord.tlf01.dcf01_retired_cwk[2] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.9 pos 2, Authentication method= %c%c \r\n" ,sTranRecord.tlf01.dcf01_retired_cwk[2], sTranRecord.tlf01.dcf01_retired_cwk[3] );
					m_StrTextFld += cavv_buff;

			}

			if('\0' != sTranRecord.tlf01.product_codes[0].code[2])
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.20 3D-Secure Indicator = %c \r\n" , sTranRecord.tlf01.product_codes[0].code[2]);
					m_StrTextFld += cavv_buff;
			}
			

			if(sTranRecord.tlf01.resp_source[0] == 'V' && strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncvisa",6) == 0)
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De44.1 Response source/reason code = %c \r\n" ,sTranRecord.tlf01.resp_source[0]);
					m_StrTextFld += cavv_buff;
			}

			if(sTranRecord.tlf01.resp_source[3] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De44.4 Value = %c \r\n" ,sTranRecord.tlf01.resp_source[3]);
					m_StrTextFld += cavv_buff;
			}
			//HCTS4
			if(sTranRecord.tlf01.product_codes[2].amount[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Visa MVV Value = %s \r\n" ,sTranRecord.tlf01.product_codes[2].amount);
					m_StrTextFld += cavv_buff;
			}

			if ((0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso", 5 ) ||
				 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE", 5 ))&&
				0 == strncmp((char *)sTranRecord.tlf01.acquirer_id, "ncvisa", 6))
			{

				if (sTranRecord.tlf01.card_num[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Account Number = %s \r\n" ,sTranRecord.tlf01.card_num);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.total_amount[0] != '\0')
				{					
					/*memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authorized Amount = %s \r\n" ,sTranRecord.tlf01.total_amount);*/
					temp = _T("");
					temp = myUtilsObject.DisplayAmount(sTranRecord.tlf01.total_amount);
					sprintf (cavv_buff,"\r\n Authorized Amount = %s \r\n" ,temp);
					m_StrTextFld +=  cavv_buff;
					
				}

				if (sTranRecord.tlf01.auth_number[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authorized Code = %s \r\n" ,sTranRecord.tlf01.auth_number);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.visa_char_indicator != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authzn Characterstics Indicator = %c \r\n" ,sTranRecord.tlf01.visa_char_indicator);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.visa_char_indicator == '\0' &&
					sTranRecord.tlf01.kek[3] != '\0')
				{					/* F111 DS56 TG82 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authzn Characterstics Indicator = %c \r\n" ,sTranRecord.tlf01.kek[3]);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.response_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authzn Response Code = %s \r\n" ,sTranRecord.tlf01.response_code);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.currency_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Authzn Currency Code = %s \r\n" ,sTranRecord.tlf01.currency_code);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.kek[0] != '\0')
				{				/* F111 DS56 TG80 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Account Funding Source = %c \r\n" ,sTranRecord.tlf01.kek[0]);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{				/* F60.9*/
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n CardHolder Idnfn(ID)method = %c \r\n" ,sTranRecord.tlf01.product_codes[5].quantity[0]);
					m_StrTextFld += cavv_buff;

				}

				if (sTranRecord.tlf01.product_codes[5].quantity[0] == '\0' &&
					sTranRecord.tlf01.kek[4] != '\0')
				{				/* F111 DS56 TG85 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n CardHolder Idnfn(ID)method = %c \r\n" ,sTranRecord.tlf01.kek[4]);
					m_StrTextFld += cavv_buff;

				}

				if (sTranRecord.tlf01.add_amounts[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Cashback Amount = %s \r\n" ,sTranRecord.tlf01.add_amounts);
					m_StrTextFld += cavv_buff;

				}

				if (sTranRecord.tlf01.product_codes[15].quantity[0] != '\0')
				{				/* F62.4 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Market specific Authzn Data Indcr = %c \r\n" ,sTranRecord.tlf01.product_codes[15].quantity[0]);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.category_code[0] != '\0')
				{				/* F18 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Merchant Category Code = %s \r\n" ,sTranRecord.tlf01.category_code);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.pos_entry_mode[0] != '\0')
				{				/* F22 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  POS Entry Mode = %s \r\n" ,sTranRecord.tlf01.pos_entry_mode);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.product_codes[7].code[0] != '\0')
				{				/* F62.23 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Product ID = %c%c \r\n" ,sTranRecord.tlf01.product_codes[7].code[0],sTranRecord.tlf01.product_codes[7].code[1]);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.visa_tran_id[0] != '\0')
				{				/* F62.2 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Transaction Identifier = %s \r\n" ,sTranRecord.tlf01.visa_tran_id);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.visa_validation_code[0] != '\0')
				{				/* F62.3 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Validation Code = %s \r\n" ,sTranRecord.tlf01.visa_validation_code);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.profile[0] != '\0')
				{				/* F60.8 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  ECI/MOTO Indicator = %s \r\n" ,sTranRecord.tlf01.profile);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.product_codes[7].code[2] != '\0')
				{				/* F62.25 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  Spend Qualification Indicator = %c \r\n" ,sTranRecord.tlf01.product_codes[7].code[2]);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{				/* F126.13 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n  POS Enviroment = %c \r\n" ,sTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrTextFld += cavv_buff;
				}
			}

		}
		else if(Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
#ifdef MC13_OCT2023
				if(sTranRecord.tlf01.product_codes[0].amount[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.product_codes[0].amount), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n %s  \r\n", sTranRecord.tlf01.product_codes[0].amount);
					m_StrTextFld += cavv_buff;
				}
				else if(sTranRecord.tlf01.response_text[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.response_text), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					substr = strstr((char *)sTranRecord.tlf01.response_text,"DE48 SE84:");
					sprintf (cavv_buff,"\r\n %s  \r\n", substr);
					m_StrTextFld += cavv_buff;
				}
#endif

#ifdef MC01_APR2024
				if(sTranRecord.tlf01.product_codes[19].amount[0] != '\0'        && 
				   0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"nceqit",6) &&
			       0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"neqit2",6) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE",5) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n TranLink Identifier(DE105 SE01) = %s  \r\n", sTranRecord.tlf01.product_codes[19].amount);
					m_StrTextFld += cavv_buff;
				}
#endif

#ifdef MC02_APR2024
				if(sTranRecord.tlf01.product_codes[10].amount[0] != '\0' && 
				   ((0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso",6) ||
				     0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE", 5))|| 0 == strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcrd",6)) )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n TranLink Identifier(DE105 SE01) = %s%s  \r\n", sTranRecord.tlf01.product_codes[10].amount, sTranRecord.tlf01.product_codes[1].amount);
					m_StrTextFld += cavv_buff;
				}
#endif

				if( sTranRecord.tlf01.product_codes[11].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Program Protocol = %c  \r\n" , sTranRecord.tlf01.product_codes[11].quantity[0]);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DS Transaction ID = %s%s  \r\n" ,sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF43 = %s  \r\n" ,sTranRecord.tlf01.product_codes[5].quantity);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.orig_auth_number[0] != '\0' && sTranRecord.tlf01.tx_key !=AUTH_REVERSAL_RESPONSE) 
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE37 SF04 = %s \r\n" ,sTranRecord.tlf01.orig_auth_number);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.down_payment[0] != '\0' && sTranRecord.tlf01.total_interest[0] != '\0' && sTranRecord.tlf01.actual_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE56 SE01 SF01= %s%s%s \r\n" ,sTranRecord.tlf01.down_payment,sTranRecord.tlf01.total_interest,sTranRecord.tlf01.actual_amount);
					m_StrTextFld += cavv_buff;

				}
				if(sTranRecord.tlf01.remote_nsp_type[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 TCC Value= %s \r\n" ,sTranRecord.tlf01.remote_nsp_type);
					m_StrTextFld += cavv_buff;

				}
				if(sTranRecord.tlf01.dbrefund_amount[0] != '\0')
				{
					if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue, "dcpiso", 6))
					{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"\r\n DE48 SE37 SF05(Merchant PGY ID) = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
						m_StrTextFld += cavv_buff;
					}
					else
					{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE37 SF05 = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
					m_StrTextFld += cavv_buff;
					}

				}
				if(sTranRecord.tlf01.visa_validation_code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF57 = %s  \r\n" ,sTranRecord.tlf01.visa_validation_code);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[19].quantity[0] != '\0' && sTranRecord.tlf01.product_codes[16].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF71 = %s%s  \r\n" ,sTranRecord.tlf01.product_codes[19].quantity,sTranRecord.tlf01.product_codes[16].quantity);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[10].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE108 SE03 SF05 = %s  \r\n" ,sTranRecord.tlf01.product_codes[10].quantity);
					m_StrTextFld += cavv_buff;
				}
				if((sTranRecord.tlf01.product_codes[8].amount[0] != '\0' || sTranRecord.tlf01.product_codes[14].amount[0] != '\0') &&
					strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE67 SF02 = %s%s  \r\n" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.agent_id_len[0] != '\0' && 
					(strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd", 5) == 0 || 
					 strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3", 6) == 0) &&
					 strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcr2",6)  == 0 )				
				{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"\r\n DE48 SE80 = %s  \r\n" ,sTranRecord.tlf01.agent_id_len);
						m_StrTextFld += cavv_buff;
				}

		}
		else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0' &&
					 0 != strncmp((char *)sTranRecord.tlf01.dcf01_retired_cwk+4, "00", 2))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE24 = %s \r" ,sTranRecord.tlf01.product_codes[0].code);
					m_StrTextFld += cavv_buff;
				}
				if (sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff, 0x00, sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n De44.4 value = %c%c \r" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
					m_StrTextFld += cavv_buff;

				}
				//if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' && sTranRecord.tlf01.operator_id[0] != '\0' && sTranRecord.tlf01.workstation[0] != '\0')
				if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n DE122 value = %s \r\n", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrTextFld += cavv_buff;
				}
		}
		else if(true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF4 = %s  \r", sTranRecord.tlf01.profile);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF6 = %s  \r", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF8 = %s%s\r",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					memset(Cavv_components, 0x00, sizeof(Cavv_components));
					strncpy(Cavv_components,(char *)sTranRecord.tlf01.workstation,12);
					sprintf(cavv_buff,"\r\n 3DS vers = %s\r\n", Cavv_components);
					m_StrTextFld += cavv_buff;
				}
		}
		else if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
		{

				if(sTranRecord.tlf01.product_codes[1].amount[0] != '\0' && sTranRecord.tlf01.product_codes[2].amount[0] != '\0' && sTranRecord.tlf01.product_codes[8].amount[0] != '\0' &&
					sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE61.6 AVN Value= %s%s%s%s \r\n" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					m_StrTextFld += cavv_buff;

				}

				if(sTranRecord.tlf01.product_codes[13].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE125 DSTransaction= %s%s%s \r\n" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					m_StrTextFld += cavv_buff;

				}
				if(sTranRecord.tlf01.product_codes[15].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De125 Tag01 Usage IP= %s \r\n" ,sTranRecord.tlf01.product_codes[15].code);
					m_StrTextFld += cavv_buff;
				}

				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n 3D Secure Version= %s \r\n" ,&sTranRecord.tlf01.product_codes[0].code[3]);
					m_StrTextFld += cavv_buff;
				}

				if(sTranRecord.tlf01.product_codes[5].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n ECI(DE61.4)= %s \r\n" ,sTranRecord.tlf01.product_codes[5].code);
					m_StrTextFld += cavv_buff;
				}		
				if(sTranRecord.tlf01.resp_source[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE 61.6 Usage 5(AM)= %s \r\n" ,sTranRecord.tlf01.resp_source);
					m_StrTextFld += cavv_buff;
				}

			
		}

		else if(true == Check_If_JCB_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{
				strncpy(Cavv_components,(char *)sTranRecord.tlf01.product_codes[3].amount,6);
				strncat(Cavv_components,(char *)&sTranRecord.tlf01.product_codes[6].amount[2],10);
				strncat(Cavv_components,(char *)sTranRecord.tlf01.product_codes[8].amount,6);
				memset(cavv_buff,0x00,sizeof(cavv_buff));
				sprintf (cavv_buff,"\r\nCAVV Components= %s \r\n" ,Cavv_components);
				m_StrTextFld += cavv_buff;
			}
			if(sTranRecord.tlf01.profile[0] != '\0')
			{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nECI= %s \r\n" ,sTranRecord.tlf01.profile);
					m_StrTextFld += cavv_buff;
			}

			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{		
				 strncpy(Version,(char *)&sTranRecord.tlf01.product_codes[3].amount[2],2);
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n3D Secure Version= %s \r\n" ,Version);
					m_StrTextFld += cavv_buff;
			}
			
		}
	if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if(sptsn01.serial_number[0] != '\0')
			{		
				memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nPhysical Terminal Serial Number= %s \r\n" ,sptsn01.serial_number);
					m_StrTextFld += cavv_buff;
			}
		}
#endif

	}
	/*************************************************************************************/
	//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
	//m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
	m_StrECI = sTranRecord.tlf01.profile;
	if( true == Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) && m_StrECI.GetLength() != 0 )
	{
		CString s2 = _T("2");
		m_StrECI= s2 + sTranRecord.tlf01.profile;
	}
	else if(0 == strcmp((char *)sTranRecord.tlf01.pos_condition_code,"59"))
	{
		CString s2 = _T("2");
		m_StrECI= s2 + sTranRecord.tlf01.profile;
	
	}

	if(true == Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) && 
	  strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso",6) == 0)
	{
		 CString s2 = _T("2");
		m_StrECI_1 = s2 + sTranRecord.tlf01.product_codes[12].quantity;
	}
	else
	{
		m_StrECI_1 = m_StrECI;
	}


	char temp_StrOriginalECI_Reason[4] = {0};

	CString temp_OriginalECI_Reason = sTranRecord.tlf01.product_codes[14].code ;
	m_StrOriginalECI_Reason = temp_OriginalECI_Reason.Mid(0,3) ;
	
    int OriginalECI_Reason_len=m_StrOriginalECI_Reason.GetLength();
	if(OriginalECI_Reason_len > 0)
	{
		CString s1 = _T("/"); 
		m_StrOriginalECI_Reason = m_StrOriginalECI_Reason + s1+sTranRecord.tlf01.product_codes[14].quantity;

    }

	
	m_StrMCC =sTranRecord.tlf01.category_code;
	m_StrMerchantName =sTranRecord.tlf01.card_holder_name;
	
	
	//temp = sTranRecord.tlf01.product_codes[7].amount ;
	//m_StrPosData =  myUtilsObject.DisplayAmount(temp);
	m_StrPosData = sTranRecord.tlf01.product_codes[16].amount ;
	if (0 == strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) &&
		strlen((char *)sTranRecord.tlf01.product_codes[5].amount) > 0)
	{
			m_StrPosData = sTranRecord.tlf01.product_codes[5].amount ;
	}
	if (true == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		 m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[17].code ;
	}
	else if(Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) == true || 
			Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num)   == true )
	{
		 m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[16].amount ;
	}
	else
	{
		m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[9].code ;
	}
	
	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr")||
		 0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")) &&
		 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
			temp = _T("");
			m_StrPOSEntryModeFld_1 = temp;
	}
	
	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
	    true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) &&
		( sTranRecord.tlf01.product_codes[13].code[0] != 0x00))     
	{
		m_StrAcquiringCountry = sTranRecord.tlf01.product_codes[7].code;
	}
	else if ((strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
			 (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5)   ||
			  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6)) &&
			 (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3)))
	{
		m_StrAcquiringCountry = sTranRecord.tlf01.product_codes[7].code;
	}
	else if((strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
			 (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6)   ||
			  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6)) )
	{
		m_StrAcquiringCountry = sTranRecord.tlf01.product_codes[18].code;
		if(strlen((char *)sTranRecord.tlf01.num_dbrefunds) > 0 && 0 == m_StrAcquiringCountry.GetLength())
		{
			m_StrAcquiringCountry = sTranRecord.tlf01.num_dbrefunds;
		}
	}
	else
	{
	m_StrAcquiringCountry = sTranRecord.tlf01.currency_code;
	}
	
	if(strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
	{
		if(strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0)
		{
			if((sTranRecord.tlf01.nfi_seq_nbr[0] != 0x00) )
			{
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.nfi_seq_nbr;
				//m_StrOriginalTxnAmount = sTranRecord.tlf01.sales_amount;
				temp = sTranRecord.tlf01.sales_amount;
				//m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				if (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3)) 
				{
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);

					if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
						 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
					{
						temp = sTranRecord.tlf01.refund_amount;
						m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
					}
				}
				else
				{
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}

			}
			else
			{
				//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;

				if(0 == strncmp((char *)sTranRecord.tlf01.currency_code, ICELAND_KRONA_CURRENCY, 3))
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
			}
			
		}
		else
		{
			temp = _T("");
			//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
			if((true   == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num) ||
				true  == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))   &&
				0     == strncmp((char *)sTranRecord.tlf01.product_codes[3].quantity,"RP",2))
			{
				temp = sTranRecord.tlf01.orig_amount;
			}
			else
			{
				temp = sTranRecord.tlf01.total_amount;
			}
			m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);

			if((0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nceqit",6) ||
			    0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"neqit2",6)) )
			{
				if (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3))
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
				}
				else if (0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3) &&
					     0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3))
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.nfi_seq_nbr;
			}

			
			if((strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis3",6) == 0 && 
				 0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3)  &&
				 0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3)) &&
				(0 != strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nceqit",6) &&
			     0 != strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"neqit2",6)) &&
				((0 == strncmp((char *)sTranRecord.tlf01.message_type,"0100",4)) ||
				(0 == strncmp((char *)sTranRecord.tlf01.message_type,"0120",4)) ||
				(0 == strncmp((char *)sTranRecord.tlf01.message_type,"0400",4)) ||
				(0 == strncmp((char *)sTranRecord.tlf01.message_type,"0420",4))))
			{
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.nfi_seq_nbr;
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
			}
			else
			{
				if (m_OrgTxnAmtCurrency == _T(""))
			m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
		}
			
		}
	}
	else
	{
		//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
		temp = sTranRecord.tlf01.total_amount;
		m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
		//m_StrOriginalTxnAmount = 45678;
		m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
	}
	
	if (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0)
	{
		if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6) )
		{
			dec_plcs = atoi ((char *)sccl01.currency_decimal_places);

			if ((char *)sTranRecord.tlf01.nfi_seq_nbr[0] != '\0' &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3))
			{
				if (sccl01.currency_decimal_places[0] == '2' || sccl01.currency_decimal_places[0] == '\0')
				{			/* for 3rd currency stored in nfi_seq_nbr */
					temp = sTranRecord.tlf01.product_codes[7].amount;
					m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.refund_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
				else
				{
					temp = sTranRecord.tlf01.product_codes[7].amount;
					m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.refund_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
				}
			}
			else 
			{
				if (sccl01.currency_decimal_places[0] != '\0')
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
				}
				else
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}

			}

		}
		else if (true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
			dec_plcs = atoi ((char *)sccl01.currency_decimal_places);

			if ((char *)sTranRecord.tlf01.product_codes[13].code[0] != '\0' &&
				0 != strncmp((char *)sTranRecord.tlf01.product_codes[13].code, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.product_codes[13].code, PESO, 3))
			{
				if (sccl01.currency_decimal_places[0] == '2' || sccl01.currency_decimal_places[0] == '\0')
				{				/* for 3rd currency stored in product13_code */
							temp = sTranRecord.tlf01.product_codes[7].amount;
							m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
						temp = sTranRecord.tlf01.orig_amount;
						m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
				else
				{
							temp = sTranRecord.tlf01.product_codes[7].amount;
							m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
						temp = sTranRecord.tlf01.orig_amount;
						m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
				}
			}
			else
			{
				
				if (sccl01.currency_decimal_places[0] != '\0')
				{	
					temp = sTranRecord.tlf01.total_amount;
					m_StrTranAmountFld =  myUtilsObject.GetAmount_not_inDecimal(temp);
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
				}
				else
				{
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
			}

		}
		else if ((char *)sTranRecord.tlf01.nfi_seq_nbr[0] != '\0' &&
			    0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3))
		{
			dec_plcs = atoi ((char *)sccl01.currency_decimal_places);

			/* Txn currency code is not 840 or 608  for other card brands*/
			if (sccl01.currency_decimal_places[0] == '2' || sccl01.currency_decimal_places[0] == '\0')
			{	
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);

			}
			else
			{
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
			}
		}
		else
		{	
			dec_plcs = atoi ((char *)sccl01.currency_decimal_places);
			/* Txn currency code is 840 or 608  for other card brands*/
			if (sccl01.currency_decimal_places[0] != '\0')
			{	
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp, dec_plcs);

			}
			else
			{
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
			}
		}
	}
	
	/*************************************************************************************/
	
	/* Deferred Payment Data */
	if ( 0x00 != sTranRecord.tlf01.deferred_term_length[0] )
	{
		temp = sTranRecord.tlf01.def_gross_amt;
		m_StrGrossAmountFld =  myUtilsObject.DisplayAmount(temp);
				
		temp = sTranRecord.tlf01.monthly_amort_amt;
		m_StrMonthlyAmortFld =  myUtilsObject.DisplayAmount(temp);
		
		m_StrTermLengthFld =  sTranRecord.tlf01.deferred_term_length;
		
		/* Convert Def Factor from nnnnnnn to nnn.nnnn (0050000 = 5%). */
		m_StrFactorFld = sTranRecord.tlf01.deferred_factor;
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			nFactor = atof( m_StrFactorFld ) / 1000000.0;
			sprintf( strFactor, "%8.6f", nFactor );
		}
		else
		{
			nFactor = atof( m_StrFactorFld ) / 10000.0;
			sprintf( strFactor, "%8.4f", nFactor );
		}
		m_StrFactorFld = strFactor;
	}
	
	if (m_bViewEmvData)
	{
		/* Determine the block size */
		int nLength;
		//sscanf((const char *)sTranRecord.emvraw.DATA_LENGTH, "%d", &nLength );		
		nLength = genutil_bcd_to_int( sTranRecord.emvraw.RAW_DATA, 2 );
		
		if (nLength > 0)
		{
			char EmvRawAsciiBuf[600];
			
			nLength += 2;	/*include the length field*/
			genutil_bin_to_ascii(sTranRecord.emvraw.RAW_DATA, nLength, EmvRawAsciiBuf, sizeof(EmvRawAsciiBuf));
			m_StrEmvFld            =  EmvRawAsciiBuf; 
		}
	}

	if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) || true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		if (strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1100",4);
		else if (strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1120",4);
		else if (strncmp((char *)sTranRecord.tlf01.message_type,"0420",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1420",4);
	}

	m_StrTranMessageTypeFld = (CString)(char *)sTranRecord.tlf01.message_type;
	m_StrTranTypeFld = (CString)(char *) sTranRecord.tlf01.tx_key;
	if ( atoi(m_StrTranTypeFld) > 100 )
		strResponse = " RESPONSE";
	else
		strResponse = "";
	
	switch (sTranRecord.tlf01.tx_key)
	{
	case AUTH_INVALID:
		m_StrTranTypeFld = "INVALID" + strResponse;
		break;
		
	case AUTH_SALE:
	case AUTH_SALE_RESPONSE:
			m_StrTranTypeFld = "SALE" + strResponse;
			if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
			{
				m_StrTranTypeFld = "MP SALE" + strResponse;
			}
		break;
		
	case  AUTH_REFUND:
	case  AUTH_REFUND_RESPONSE:
	case AUTH_PRE_AUTHORIZATION_CANCLE:
	case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE:
		m_StrTranTypeFld = "AUTH REFUND" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "MP REFUND" + strResponse;
		}
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
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "MP OFFLINE SALE" + strResponse;
		}
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

	case AUTH_OFFLINE_VOID_SALE:
	case AUTH_OFFLINE_VOID_SALE_RESPONSE:
		m_StrTranTypeFld = "VOID OFFLINE SALE" + strResponse; 
		break;
		
	case AUTH_AUTHORIZATION:
	case AUTH_AUTHORIZATION_RESPONSE:
	case AUTH_MONEY_SEND:
	case AUTH_MONEY_SEND_RESPONSE:
	case AUTH_FAST_FUND:
	case AUTH_FAST_FUND_RESPONSE:
	case AUTH_OCT_TX:
	case AUTH_OCT_TX_RESPONSE:
		{ 
			if ( strcmp ((char *)sTranRecord.tlf01.processing_code, "180000" ) == 0)
			{
				m_StrTranTypeFld = "QUASI CASH" + strResponse;
			}
			else if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
			{
				m_StrTranTypeFld = "DEFERRED PURCHASE AUTHORIZATION RESPONSE";
			}
			/*else if(m_StrCardNumberFld[0]== '6')
			{
		
				m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
			}*/
			else
			{
				if ( ( strcmp((const char *)(sTranRecord.tlf01.voice_auth), "Y") == 0) ||
					( strcmp((const char *)(sTranRecord.tlf01.voice_auth), "y") == 0) )
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
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "VOID MP SALE" + strResponse;
		}
		break;
		
	case AUTH_VOID_REFUND:
	case AUTH_VOID_REFUND_RESPONSE:
		m_StrTranTypeFld = "VOID REFUND" + strResponse; 
		break;
		
	case AUTH_SETTLE:
	case AUTH_SETTLE_RESPONSE:
		m_StrTranTypeFld = "SETTLE" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "SETTLE MP" + strResponse;
		}
		break;
		
	case AUTH_BATCH_UPLOAD:
	case AUTH_BATCH_UPLOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH UPLOAD" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "BATCH UPLOAD MP" + strResponse;
		}
		break;
		
	case AUTH_SETTLE_TRAILER:
	case AUTH_SETTLE_TRAILER_RESPONSE:
		m_StrTranTypeFld = "SETTLE TRAILER" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "SETTLE TRAILER MP" + strResponse;
		}
		break;
		
	case AUTH_STATISTICS:
	case AUTH_STATISTICS_RESPONSE:
		m_StrTranTypeFld = "STATISTICS" + strResponse; 
		break;
		
	case AUTH_REVERSAL:
	case AUTH_REVERSAL_RESPONSE:
		
		m_StrTranTypeFld = "REVERSAL - " ;
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "REVERSAL MP - " ;
		}
		
		if( 0 == strncmp((char *) sTranRecord.tlf01.product_codes[1].quantity, "01",2))
		{
		   m_StrCurrencyCodeFld = "840";
		   m_StrAcquiringCountry = m_StrCurrencyCodeFld;
		}
		if ( strcmp ((char *)sTranRecord.tlf01.processing_code, "700000" ) == 0)
            m_StrTranTypeFld +="DEFERRED";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "180000" ) == 0)
            m_StrTranTypeFld +="QUASI CASH";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0100" ) == 0))
            m_StrTranTypeFld +="AUTHORIZATION";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="PURCHASE";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE COMPLETION";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "200000" ) == 0) 
            m_StrTranTypeFld +="REFUND";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "010000" ) == 0) 
            m_StrTranTypeFld +="CASH";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID SALE";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID REFUND";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE ADJUSTMENT";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="REFUND";
		
		if ( sTranRecord.tlf01.orig_message[0] == 0x00 )
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

	case AUTH_ATM_CONFIRM:
		m_StrTranTypeFld = "REVERSAL" + strResponse; 
	break;
   }
 
   if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) )
   {
		if(strncmp((char *)sTranRecord.tlf01.message_type,"0100",4)   == 0 &&
		  strncmp((char *)sTranRecord.tlf01.processing_code,"33",2)   == 0)
		{
			m_StrTranTypeFld = "Account Verify";
		}
			
   }

   char startTimeBuf[20] = {0};
   if(strlen((char*)sTranRecord.tlf01.tran_start_time) > 14)
   {
	   //we are storing DE12 TransmissionTimeStamp(YYMMDD<epoch-10bytes>) in tran_start_time for some txns
	   strncpy(startTimeBuf, (char*)sTranRecord.tlf01.tran_start_time+6, 10);
   }
   else
   {
	   strncpy(startTimeBuf, (char*)sTranRecord.tlf01.tran_start_time, 10);
   }
   time_t rawtime = atoi(startTimeBuf);
   struct tm  ts;
   char       buf[30];

   //Format time, "hh:mm:ss zzz"
   ts = *localtime(&rawtime);
   strftime(buf, sizeof(buf), "%H:%M:%S", &ts);



   

   m_StrTranDateFld   = m_StrTranDateFld.Right(2)   + "/" + m_StrTranDateFld.Mid(4,2)   + "/" + m_StrTranDateFld.Left(4);
   if(0x00 != sTranRecord.tlf01.time_hhmmss[0])
   {
	   m_StrTranTimeFld   = m_StrTranTimeFld.Left(2)    + ":" + m_StrTranTimeFld.Mid(2,2)   + ":" + m_StrTranTimeFld.Right(2);
   }
   else
   {
	
   }
   m_StrSystemDateFld = m_StrSystemDateFld.Right(2) + "/" + m_StrSystemDateFld.Mid(4,2) + "/" + m_StrSystemDateFld.Left(4);

   //m_StrSystemDateTimeFld = strSysDt_Tm.Left(4) + "-" + strSysDt_Tm.Mid(4,2) + "-" + strSysDt_Tm.Right(2) + " " + m_StrTranTimeFld;
   m_StrSystemDateTimeFld = strSysDt_Tm.Left(4) + "-" + strSysDt_Tm.Mid(4,2) + "-" + strSysDt_Tm.Right(2) + " " + buf;
   
   /*m_StrBdoMercAcqTx = sTranRecord.tlf01.product_codes[4].code;*/
   m_StrAuthProfile  = sTranRecord.tlf01.product_codes[3].code;
   m_StrBinLow		  = sTranRecord.tlf01.product_codes[2].code;
   if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "5.000") == 0)
   {
	   m_StrEarMarkedCurrency = "PHP";
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   m_StrEarmarkedcurrency = "PHP" ;
	   m_StrEarmarkedamount1 =  myUtilsObject.DisplayAmount(temp);
   }
   else if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "7.000") == 0)
   {
	   m_StrEarMarkedCurrency = "USD";
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   m_StrEarmarkedcurrency = "USD" ;
	   m_StrEarmarkedamount1 =  myUtilsObject.DisplayAmount(temp);
   }
   else if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "0.000") == 0)
   {
	   m_StrEarMarkedCurrency = sTranRecord.tlf01.currency_code;
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   m_StrEarmarkedcurrency = sTranRecord.tlf01.currency_code;
	   m_StrEarmarkedamount1 =  myUtilsObject.DisplayAmount(temp);
   }
   else 
   {
	   m_StrEarMarkedCurrency	= sTranRecord.tlf01.currency_code;
	   temp = sTranRecord.tlf01.total_amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   m_StrEarmarkedcurrency = sTranRecord.tlf01.currency_code;
	   m_StrEarmarkedamount1 =  myUtilsObject.DisplayAmount(temp);
	   
   }


   temp = _T("");
   temp =  sTranRecord.tlf01.product_codes[7].amount ;
   m_StrBillingamount =  myUtilsObject.DisplayAmount(temp);
   m_StrBillingCurrency  = sTranRecord.tlf01.product_codes[7].code;
   
   m_StrServiceCode  = sTranRecord.tlf01.service_code;

   m_StrVisaMerchantIdentifier  = sTranRecord.tlf01.product_codes[4].amount;
   m_StrDigitalCommProgramIndicator = sTranRecord.tlf01.product_codes[8].code;
   m_StrDigitalEntryIdentifier = sTranRecord.tlf01.product_codes[5].amount;
   m_StrDigitalEntryIdentifier = _T("");

   /* NII value*/
   if(strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) == 0)
   {
      m_StrNii = sTranRecord.tlf01.nii;
   }
   /********************************************************************************************************************
   if(strcmp((char *)sTranRecord.tlf01.product_codes[1].code, STANDIN) == 0)
   {
   m_ChkAtpStip.SetCheck(1);
   m_ChkSchemeStip.SetCheck(0);
   
	 }
	 else
	 {
	 m_ChkAtpStip.SetCheck(0);
	 m_ChkSchemeStip.SetCheck(1);
	 }
	 
	   if( (sTranRecord.tlf01.tx_key == AUTH_AUTHORIZATION_OVERRIDE) || (sTranRecord.tlf01.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) )
	   {
	   m_ChkVoice.SetCheck(1);
	   }
	   else
	   {
	   m_ChkVoice.SetCheck(0);
	   }
	   
		 if( strlen((char *)sTranRecord.tlf01.nbr_of_prod_codes) > 0)
		 {
		 m_ChkOMD.SetCheck(1);
		 }
		 else
		 {
		 m_ChkOMD.SetCheck(0);
		 }
		 
		   /*if( strlen((char *)sTranRecord.tlf01.security_response_code ) > 0)
		   {
		   m_ChkCVV2.SetCheck(1);
		   }
		   else
		   {
		   m_ChkCVV2.SetCheck(0);
	}*/
   /********************************************************************************************************************/
   
   
   /*********************************************************************************************************
   m_ChkHostProcessed
   ***********************************m_ChkAtpStip
   ***********************************m_ChkSchemeStip
   ***********************************m_ChkVoice
   ***********************************m_ChkOMD
   m_ChkCVVPresent
   m_ChkCvvValidated
   m_ChkCVV2Present
   m_ChkCVV2Validated
   m_ChkPINPresent
   m_ChkPINValidated
   m_ChkARQCPresent
   m_ChkArqcValidated
   m_ChkCAVVPresent
   m_ChkCAVVValidated
   ***********************************m_CntlFGResultFld
   /*********************************************************************************************************/
   
 /*newwwwwwwwwwwwwwww********************************************************************************************************/
   
   m_ChkHostProcessed.SetCheck(0);
   m_ChkSchemeStip.SetCheck(0);
   m_ChkTOAtpStip.SetCheck(0);

   if(strcmp((char *)sTranRecord.tlf01.product_codes[3].quantity, "19") == 0)
   {
	   m_ChkTOAtpStip.SetCheck(1);
   }
   else if(strcmp((char *)sTranRecord.tlf01.product_codes[1].code, STANDIN) == 0)
   {
	   m_ChkAtpStip.SetCheck(1);
   }
   else
   {
	   m_ChkHostProcessed.SetCheck(1);
   }
 
   if( (sTranRecord.tlf01.tx_key == AUTH_AUTHORIZATION_OVERRIDE) || (sTranRecord.tlf01.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) )
   {
	   m_ChkVoice.SetCheck(1);
   }
   else
   {
	   m_ChkVoice.SetCheck(0);
   }
   

   if( strlen((char *)sTranRecord.tlf01.nbr_of_prod_codes) > 0 && strcmp((char *)sTranRecord.tlf01.product_codes[18].code,"OMD")== 0)
   {
	   m_ChkOMD.SetCheck(1);
   }
   else
   {
	   m_ChkOMD.SetCheck(0);
   }

   
   /*if( (char *)sTranRecord.tlf01.cvc_data != '\0')
   {
	   m_ChkCVVPresent.SetCheck(1);
	   m_ChkCVV2Present.SetCheck(1);
   }
   else
   {
	   m_ChkCVVPresent.SetCheck(0);
	   m_ChkCVV2Present.SetCheck(0);
   }*/
   
  /* if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && ( (char *)sTranRecord.tlf01.cvc_data != '\0') )
   {
	   m_ChkCvvValidated.SetCheck(1);
	   m_ChkCVV2Validated.SetCheck(1);
   }
   else
   {
	   m_ChkCvvValidated.SetCheck(0);
	   m_ChkCVV2Validated.SetCheck(0);
   }*/
   
   
   if ( 0 == strlen((char *)sTranRecord.tlf01.pin_block))
	   
   {
	   m_ChkPINPresent.SetCheck(0);
   }
   else
   {
	   m_ChkPINPresent.SetCheck(1);
   }
   
   if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && ( 0 == strlen((char *)sTranRecord.tlf01.pin_block)) )
   {
	   m_ChkPINValidated.SetCheck(1);
   }
   else
   {
	   m_ChkPINValidated.SetCheck(0);
   }
   
   if((char *)sTranRecord.tlf01.pos_entry_mode == "0051" || (char *)sTranRecord.tlf01.pos_entry_mode == "0071" ||
	   (char *)sTranRecord.tlf01.pos_entry_mode == "0081")
   {
	   m_ChkARQCPresent.SetCheck(1);
   }
   else
   {
	   m_ChkARQCPresent.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.tlf01.response_code,"00") == 0)
   {
	   m_ChkArqcValidated.SetCheck(1);
   }
   else
   {
	   m_ChkArqcValidated.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.tlf01.nfi_end_date,"1") == 0)
   {
	   m_ChkCAVVPresent.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVPresent.SetCheck(0);
   }
   
   if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && (strcmp((char *)sTranRecord.tlf01.nfi_end_date,"1") == 0))
   {
	   m_ChkCAVVValidated.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVValidated.SetCheck(0);
   }

   /* For VOICE and Acquired transaction fetch the MRA01 table to display the MRA_SENT_TIME, MRACON Implementation*/
   

  if(strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) == 0 ||
	  strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE",5) == 0)
   {
		//strcpy((char *)mra01_table.transaction_id, (char *)sTranRecord.tlf01.primary_key.transaction_id );
	   strcpy((char *)mra01_table.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id );
   		rcode = txdsapi_get_record ( (pBYTE)&mra01_table, sizeof (mra01_table), MRA01_DATA, strErrorMessage);
		if(rcode == PTEMSG_OK)
		{
			m_Strmrasenttime = mra01_table.transaction_send_time ;
		}
		else
		{
			m_Strmrasenttime = _T("");
		}
	
   }
   else
   {
		// Do nothing
   }
   /*   
   ***********************************m_CntlFGResultFld*/
 /*********************************************************************************************************/
      /* STORE transaction id to fetch FTLF01 details*/
	 strcpy((char *)Transactionid,(char *)sTranRecord.tlf01.primary_key.transaction_id);
	 strncpy((char *)Handler_queue, (char *)sTranRecord.tlf01.handler_queue,7);
	UpdateData(FALSE);

	((CPropertySheet*) this->GetParent())->SetWindowText("Transaction Details");	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	//lpszTitle = lpszTitle + " - Version - ";
	//lpszTitle += appProdVer;
	//((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	//pFraudDetailsPage->PopulateFraudTranDialogFromDatabase(Transactionid);
 
   return TRUE;
}

/*****************Ramya Added For STIP*******************/

BOOL CTransactionDetails::PopulateSTIPTranDialogFromDatabase ( TRANS_STIP_RECORD sTranRecord)
{
	CMyUtils myUtilsObject;
	CHAR     strErrorMessage[200] = "";	
	CHAR     strFactor[8] = "";
	CHAR     strMPFlag[2] ="1";
	CString  temp = _T("");
	CString  strResponse = _T("");
	double   nFactor = 0;
	int		 rcode   = 0;
	char	 cavv_buff[1024] = {0};
	MRA01    mra01_table   ;
	PTSN01   sptsn01 = {0};
	CHAR acii_rescode[5] = {0};
	char amex_rescode[4] = {0};
	char dci_rescode[4] = {0};
	char Cavv_components[7]={0};
	char Version[3]={0};
	CString  strSysDt_Tm = _T("");
	CCL01	sccl01 = {0};
	int		dec_plcs=0;

	rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_STIP_DATA, strErrorMessage);


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


	if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
	{
	  strncpy((char *)sptsn01.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id,sizeof(sptsn01.transaction_id));
		
	  rcode = txdsapi_get_record ( (pBYTE)&sptsn01, sizeof (sptsn01), PTSN01_DATA, strErrorMessage);
	
	  if ( rcode != PTEMSG_OK )
	  {
		switch( rcode )
		{
			case PTEMSG_NOT_FOUND:
			 //m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
			 break;
		}
		//m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		//return FALSE;
	  }
	}

		if (true == Check_If_Issuing_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if (sTranRecord.tlf01.product_codes[13].code[0] != 0x00)
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.product_codes[13].code,sizeof(sccl01.currency_value));
			}
			else if (sTranRecord.tlf01.nfi_seq_nbr[0] != 0x00)
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.nfi_seq_nbr,sizeof(sccl01.currency_value));
			}
			else
			{
				strncpy((char *)sccl01.currency_value,(char *)sTranRecord.tlf01.currency_code,sizeof(sccl01.currency_value));
			}

			rcode = txdsapi_get_record ( (pBYTE)&sccl01, sizeof (sccl01), CCL01_DATA, strErrorMessage);
	
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
					case PTEMSG_NOT_FOUND:
					 //m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
					break;
				}
				//m_StrMessageLbl = strErrorMessage;
				UpdateData(FALSE);
				//return FALSE;
			}
		}
	
	m_StrAcquiringInstIDFld   =  sTranRecord.tlf01.acquiring_id;
	m_StrAuthIDResponseFld    =  sTranRecord.tlf01.auth_number;
	m_StrCardNumberFld        =  sTranRecord.tlf01.card_num;
	m_StrForwardingInstIDFld  =  sTranRecord.tlf01.forwarding_institution_id;
	m_StrHostTimeFld          =  sTranRecord.tlf01.host_finish_time;
	m_StrMerchantIDFld        =  sTranRecord.tlf01.merchant_id;
	m_StrPOSConditionCodeFld  =  sTranRecord.tlf01.pos_condition_code; 
	m_StrPOSConditionCodeFld_1=	sTranRecord.tlf01.product_codes[4].quantity;
	m_StrPOSEntryModeFld      =  sTranRecord.tlf01.pos_entry_mode;
	m_StrProcessingCodeFld    =  sTranRecord.tlf01.processing_code;
	m_StrProcessingCodeFld_1  =  sTranRecord.tlf01.product_codes[17].amount;
	m_StrResponseCodeFld      =  sTranRecord.tlf01.response_code;
	m_StrResponseTextFld      =  sTranRecord.tlf01.response_text;
	m_StrRetrievalRefNumFld   =  sTranRecord.tlf01.retrieval_ref_num;
	m_StrSystemTraceNumberFld =  sTranRecord.tlf01.sys_trace_audit_num;
	m_StrTerminalIDFld        =  sTranRecord.tlf01.terminal_id;
	m_StrHostNameFld          =  sTranRecord.tlf01.redemption_amount;
		
	m_StrATP_sbatchtime = sTranRecord.tlf01.kek;

	if ((true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) ||
		 true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num)) &&
		    0 == strncmp((char *)sTranRecord.tlf01.message_type,"0220",4))
	{
			m_StrPOSEntryModeFld = sTranRecord.tlf01.product_codes[14].code ;
	}

	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr")||
		 0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")) &&
		 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
	 		m_StrProcessingCodeFld_1 = sTranRecord.tlf01.processing_code;
	}

	if(	strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"1120",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"0420",4) == 0 ||
		strncmp((char *)sTranRecord.tlf01.message_type,"1420",4) == 0 )
	{
		if(true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) ||
		  true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		if(true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) )
		{
			match_ISO_to_AMEX_response_code((char *)sTranRecord.tlf01.product_codes[13].quantity,amex_rescode,sTranRecord.tlf01.tx_key);
			m_StrResponseCodeFldbyScheme = amex_rescode;
		}
		else if( true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
			match_ISO_to_Diners_response_code((char *)sTranRecord.tlf01.product_codes[13].quantity,dci_rescode);
			m_StrResponseCodeFldbyScheme = dci_rescode;
		}
		else
		{
			m_StrResponseCodeFldbyScheme = sTranRecord.tlf01.product_codes[13].quantity;
		}
	}

	if(0 == strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) &&
	  true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num) &&
	   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr",5))
	{
		m_StrResponseCodeFld = "00";
	}
	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
		m_StrNRID = sTranRecord.tlf01.product_codes[17].code;
		m_StrNRID += "/";
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[17].amount;  
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].amount;
		m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].code;
	}
	//if((0==strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncdci",5)) || (0==strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"ncdciA",5) ))
	else if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num)) 
	{
		if(sTranRecord.tlf01.visa_tran_id[0] != '\0')
		{
			m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
		}	
		else
		{ /* In amex txn, TRANID of transaction 0100 is stored in tlf01.visa_tran_id, but for 0220 txn TRANID stored in tlf01.mcard_banknet.
		  In dcpiso after fetching the original, TRANID of original txn is copied to tlf01.mcard_banknet from BCH20,
		  hence check tlf01.visa_tran_id first , if that is empty then check tlf01.mcard_banknet to view the TRANID in tranlookup page.
		  */
			m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
		}		
	}
	else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) )
	{
		m_StrNRID = sTranRecord.tlf01.resp_source;
	}
	else if (true == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
	}
	else if(true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
	}

	if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) || true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		m_StrActionCode = sTranRecord.tlf01.product_codes[12].code;
	}

	if(true == Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		
		m_StrExtCondCode=sTranRecord.tlf01.product_codes[13].amount+4;
	}

	checkMaskCardNumber(); //To mask the card number
	temp = sTranRecord.tlf01.total_amount ;
	m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	
	if ( (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
		 true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) &&
		 ( sTranRecord.tlf01.product_codes[19].amount[5] == '0' ||
		   sTranRecord.tlf01.product_codes[19].amount[5] == '4' ))
	{
		m_StrTranAmountFld =  myUtilsObject.GetAmount_not_inDecimal(temp);

	}
	else
	{
		m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	}
	
	m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);

	temp = sTranRecord.tlf01.credit_limit;
	m_StrCreditLimitFld = myUtilsObject.DisplayAmount(temp);
	
	temp = sTranRecord.tlf01.outstanding_balance;
	m_StrOutstandingBalanceFld = myUtilsObject.DisplayAmount(temp);
	
	m_StrExpirationDateFld =  sTranRecord.tlf01.exp_date;
	m_StrTranDateFld       =  sTranRecord.tlf01.date_yyyymmdd;
	m_StrTranTimeFld       =  sTranRecord.tlf01.time_hhmmss;
	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis3",6) ||
	   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncdci",5))
	{
	 // unwanted 00 need not be copied into text box field
	}
	else
	{
	m_StrTextFld           =  sTranRecord.tlf01.voice_auth_text;
	}
	m_StrCurrencyCodeFld   =  sTranRecord.tlf01.currency_code;
	if (true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
	{	// For AMEX 3DS2.0
		m_StrOperatorIDFld = _T("");
	}
	else
	{
		m_StrOperatorIDFld = sTranRecord.tlf01.operator_id;
	}
	m_StrSystemDateFld     =  sTranRecord.tlf01.system_date;
	m_StrCUPIndicatorIDFld =  sTranRecord.tlf01.product_code;
	m_StrSettlementDateFld = sTranRecord.tlf01.settlement_date;
	strSysDt_Tm = m_StrSystemDateFld;
	
	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr"))||
		(0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")))
	{
		m_StrTextFld += (CString)(char *)"\r\n";
		m_StrTextFld += (CString)(char *) "F48 <";
		m_StrTextFld += (CString)(char *) sTranRecord.tlf01.cvc_data;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *) "F60 <";
		if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0320",4))
		{	
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_message;
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_sys_trace_num;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		else if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0200",4))
		{
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.orig_amount;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
		}
		m_StrTextFld += (CString)(char *) "F61 <";
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.product_code;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F62 <";
		m_StrTextFld += (CString)(char *) sTranRecord.tlf01.invoice_number;
		m_StrTextFld += (CString)(char *)" > \r\n" ;
		m_StrTextFld += (CString)(char *)"F63 <";
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_sales;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.sales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_refunds;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.refund_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbsales;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbsales_amount;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbrefunds;
		m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbrefund_amount;
		m_StrTextFld += (CString)(char *)" > \r\n";

			if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if(sptsn01.serial_number[0] != '\0')
			{		
				memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nPhysical Terminal Serial Number= %s \r\n" ,sptsn01.serial_number);
					m_StrTextFld += cavv_buff;
			}
		}
	
	}
    
	else
	{
		if(0 == strncmp((CString)(char *)sTranRecord.tlf01.message_type,"0500",4))
		{
			m_StrTextFld += (CString)(char *)"\r\n";
			m_StrTextFld += (CString)(char *) "F48 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.cvc_data;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *) "F60 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.batch_number;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *) "F61 <";
			m_StrTextFld += (CString)(char *) sTranRecord.tlf01.product_code;
			m_StrTextFld += (CString)(char *)" > \r\n" ;
			m_StrTextFld += (CString)(char *)"F63 <";
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_sales;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.sales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_refunds;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.refund_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbsales;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbsales_amount;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.num_dbrefunds;
			m_StrTextFld += (CString)(char *)sTranRecord.tlf01.dbrefund_amount;
			m_StrTextFld += (CString)(char *)" > \r\n";
		}
#if 0
		else if(Check_If_VISA_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if( strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0200",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) == 0)
			{
				if(sTranRecord.tlf01.cvc != '\0')
				{
					sprintf (cavv_buff,"\r\n CAVV Results Code = %c  \r\n" , sTranRecord.tlf01.cvc);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n POS Environment Field = %c  \r\n" , sTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrTextFld += cavv_buff;
				}
			}
			if(sTranRecord.tlf01.operator_id[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.8 Transaction ID = %s%s \r" , sTranRecord.tlf01.operator_id,sTranRecord.tlf01.workstation);
					m_StrTextFld += cavv_buff;
			
			}

			if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.9 pos 1 3-D Secure Authn Results code = %c%c\r\n" ,sTranRecord.tlf01.dcf01_retired_cwk[0], sTranRecord.tlf01.dcf01_retired_cwk[1] );
					m_StrTextFld += cavv_buff;

			}
			
			if(sTranRecord.tlf01.dcf01_retired_cwk[2] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.9 pos 2 Authentication Method = %c%c\r\n" ,sTranRecord.tlf01.dcf01_retired_cwk[2], sTranRecord.tlf01.dcf01_retired_cwk[3] );
					m_StrTextFld += cavv_buff;

			}
			if('\0' != sTranRecord.tlf01.product_codes[0].code[2])
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE126.20 3D-Secure Indicator = %c \r\n" , sTranRecord.tlf01.product_codes[0].code[2]);
					m_StrTextFld += cavv_buff;
			}

			if(sTranRecord.tlf01.resp_source[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De44.1 Response source/reason code = %c \r\n" ,sTranRecord.tlf01.resp_source[0]);
					m_StrTextFld += cavv_buff;
			}
			if(sTranRecord.tlf01.resp_source[3] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De44.4 Value = %c \r\n" ,sTranRecord.tlf01.resp_source[3]);
					m_StrTextFld += cavv_buff;
			}
			//HCTS4
			if(sTranRecord.tlf01.product_codes[2].amount[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n Visa MVV Value = %s \r\n" ,sTranRecord.tlf01.product_codes[2].amount);
					m_StrTextFld += cavv_buff;
			}

		}
		else if(Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
#ifdef MC01_APR2024
				if(sTranRecord.tlf01.product_codes[19].amount[0] != '\0'        && 
				   0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"nceqit",6) &&
			       0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"neqit2",6) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n TranLink Identifier(DE105 SE01) = %s  \r\n", sTranRecord.tlf01.product_codes[19].amount);
					m_StrTextFld += cavv_buff;
				}
#endif
				if( sTranRecord.tlf01.product_codes[11].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF66 SF1(Program Protocol) = %c  \r\n" , sTranRecord.tlf01.product_codes[11].quantity[0]);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF66 SF2(DS Transaction ID) = %s%s  \r\n" ,sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF43 = %s  \r\n" ,sTranRecord.tlf01.product_codes[5].quantity);
					m_StrTextFld += cavv_buff;
				}
					if(sTranRecord.tlf01.orig_auth_number[0] != '\0' && sTranRecord.tlf01.tx_key !=AUTH_REVERSAL_RESPONSE) 
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE37 SF04 = %s \r\n" ,sTranRecord.tlf01.orig_auth_number);
					m_StrTextFld += cavv_buff;
				}

				if(sTranRecord.tlf01.down_payment[0] != '\0' && sTranRecord.tlf01.total_interest[0] != '\0' && sTranRecord.tlf01.actual_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE56 SE01 SF01= %s%s%s \r\n" ,sTranRecord.tlf01.down_payment,sTranRecord.tlf01.total_interest,sTranRecord.tlf01.actual_amount);
					m_StrTextFld += cavv_buff;

				}
				if(sTranRecord.tlf01.dbrefund_amount[0] != '\0')
				{
					if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue, "dcpiso", 6))
					{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"\r\n DE48 SE37 SF05(Merchant PGY ID) = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
						m_StrTextFld += cavv_buff;
					}
					else
					{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE37 SF05 = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
					m_StrTextFld += cavv_buff;
					}

				}
				if(sTranRecord.tlf01.product_codes[19].quantity[0] != '\0' && sTranRecord.tlf01.product_codes[16].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SF71 = %s%s  \r\n" ,sTranRecord.tlf01.product_codes[19].quantity,sTranRecord.tlf01.product_codes[16].quantity);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.remote_nsp_type[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 TCC Value= %s \r\n" ,sTranRecord.tlf01.remote_nsp_type);
					m_StrTextFld += cavv_buff;

				}
				if(sTranRecord.tlf01.product_codes[10].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE108 SE03 SF05 = %s  \r\n" ,sTranRecord.tlf01.product_codes[10].quantity);
					m_StrTextFld += cavv_buff;
				}
				if((sTranRecord.tlf01.product_codes[8].amount[0] != '\0' || sTranRecord.tlf01.product_codes[14].amount[0] != '\0' )&&
					strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE48 SE67 SF02 = %s%s  \r\n" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					m_StrTextFld += cavv_buff;
				}

				if (sTranRecord.tlf01.agent_id_len[0] != '\0' && 
					(strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd", 5) == 0 || 
					 strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3", 6) == 0) &&
					 strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcr2",6)  == 0 )				
				{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"\r\n DE48 SE80 = %s  \r\n" ,sTranRecord.tlf01.agent_id_len);
						m_StrTextFld += cavv_buff;
				}

		}
		else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
			if(sTranRecord.tlf01.product_codes[0].code[0] != '\0' &&
				 0 != strncmp((char *)sTranRecord.tlf01.dcf01_retired_cwk+4, "00", 2))
			{
				memset(cavv_buff,0x00,sizeof(cavv_buff));
				sprintf (cavv_buff,"\r\n DE24 = %s \r" ,sTranRecord.tlf01.product_codes[0].code);
				m_StrTextFld += cavv_buff;
			}
			if (sTranRecord.tlf01.operator_id[0] != '\0')
			{
				memset(cavv_buff, 0x00, sizeof(cavv_buff));
				sprintf(cavv_buff,"\r\n De44.4 value = %c%c \r" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
				m_StrTextFld += cavv_buff;

			}
			//if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' && sTranRecord.tlf01.operator_id[0] != '\0' && sTranRecord.tlf01.workstation[0] != '\0')
			if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
			{
				memset(cavv_buff,0x00,sizeof(cavv_buff));
				sprintf(cavv_buff,"\r\n DE122 value = %s \r\n", sTranRecord.tlf01.dcf01_retired_cwk);
				m_StrTextFld += cavv_buff;
			}

		}
		else if(true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF4 = %s  \r", sTranRecord.tlf01.profile);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF6 = %s  \r", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf(cavv_buff,"\r\n B61 SF8 = %s%s\r",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					memset(Cavv_components, 0x00, sizeof(Cavv_components));
					strncpy(Cavv_components,(char *)sTranRecord.tlf01.workstation,12);
					sprintf(cavv_buff,"\r\n 3DS vers = %s\r\n", Cavv_components);
					m_StrTextFld += cavv_buff;
				}
		}

		else if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
		{


				if(sTranRecord.tlf01.product_codes[1].amount[0] != '\0' && sTranRecord.tlf01.product_codes[2].amount[0] != '\0' && sTranRecord.tlf01.product_codes[8].amount[0] != '\0' &&
					sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE61.6 AVN Value= %s%s%s%s \r\n" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					m_StrTextFld += cavv_buff;

				}

				if(sTranRecord.tlf01.product_codes[13].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0' && sTranRecord.tlf01.product_codes[19].amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n DE125 DSTransaction= %s%s%s \r\n" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					m_StrTextFld += cavv_buff;

				}

				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n 3D Secure Version= %s \r\n" ,&sTranRecord.tlf01.product_codes[0].code[3]);
					m_StrTextFld += cavv_buff;
				}

				if(sTranRecord.tlf01.product_codes[5].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n ECI(DE61.4)= %s \r\n" ,sTranRecord.tlf01.product_codes[5].code[3]);
					m_StrTextFld += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[15].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n De125 Tag01 Usage IP= %s \r\n" ,sTranRecord.tlf01.product_codes[15].code);
					m_StrTextFld += cavv_buff;
				}

			
		
		}
		else if(true == Check_If_JCB_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{
				strncpy(Cavv_components,(char *)sTranRecord.tlf01.product_codes[3].amount,6);
				strncat(Cavv_components,(char *)&sTranRecord.tlf01.product_codes[6].amount[2],10);
				strncat(Cavv_components,(char *)sTranRecord.tlf01.product_codes[8].amount,6);
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nCAVV Components= %s \r\n" ,Cavv_components);
					m_StrTextFld += cavv_buff;
			}
			if(sTranRecord.tlf01.profile[0] != '\0')
			{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nECI= %s \r\n" ,sTranRecord.tlf01.profile);
					m_StrTextFld += cavv_buff;
			}
			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{		
				 strncpy(Version,(char *)&sTranRecord.tlf01.product_codes[3].amount[2],2);
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\n3D Secure Version= %s \r\n" ,Version);
					m_StrTextFld += cavv_buff;
			}

			
		}
	if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
		{
			if(sptsn01.serial_number[0] != '\0')
			{		
				memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"\r\nPhysical Terminal Serial Number= %s \r\n" ,sptsn01.serial_number);
					m_StrTextFld += cavv_buff;
			}
		}
#endif
}
	/*************************************************************************************/
	//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
	//m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
	m_StrECI = sTranRecord.tlf01.profile;
	if( true == Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) && m_StrECI.GetLength() != 0 )
	{
		CString s2 = _T("2");
		m_StrECI= s2 + sTranRecord.tlf01.profile;
	}
	else if(0 == strcmp((char *)sTranRecord.tlf01.pos_condition_code,"59"))
	{
		CString s2 = _T("2");
		m_StrECI= s2 + sTranRecord.tlf01.profile;
	}

	if(true == Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) && 
	  strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso",6) == 0)
	{
		CString s2 = _T("2");
		m_StrECI_1 = s2 + sTranRecord.tlf01.product_codes[12].quantity;
	}
	else
	{
		m_StrECI_1 = m_StrECI;
	}

	char temp_StrOriginalECI_Reason[4] = {0};

	CString temp_OriginalECI_Reason = sTranRecord.tlf01.product_codes[14].code ;
	m_StrOriginalECI_Reason = temp_OriginalECI_Reason.Mid(0,3) ;
	
    int OriginalECI_Reason_len=m_StrOriginalECI_Reason.GetLength();
	if(OriginalECI_Reason_len > 0)
	{
		CString s1 = _T("/"); 
		m_StrOriginalECI_Reason = m_StrOriginalECI_Reason + s1+sTranRecord.tlf01.product_codes[14].quantity;

    }

	
	m_StrMCC =sTranRecord.tlf01.category_code;
	m_StrMerchantName =sTranRecord.tlf01.card_holder_name;
	
	
	//temp = sTranRecord.tlf01.product_codes[7].amount ;
	//m_StrPosData =  myUtilsObject.DisplayAmount(temp);
	m_StrPosData = sTranRecord.tlf01.product_codes[16].amount ;
	if (0 == strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) &&
		strlen((char *)sTranRecord.tlf01.product_codes[5].amount) > 0 )
	{
			m_StrPosData = sTranRecord.tlf01.product_codes[5].amount ;
	}

	if (true == Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		 m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[17].code ;
	}
	else if(Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) == true || 
			Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num)   == true )
	{
		 m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[16].amount ;
	}
	else
	{
		m_StrPOSEntryModeFld_1 = sTranRecord.tlf01.product_codes[9].code ;
	}

	if( (0 == strcmp((char *)sTranRecord.tlf01.acquirer_id,"nciftr")||
		 0 == strcmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nciftr")) &&
		 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
	{
			temp = _T("");
			m_StrPOSEntryModeFld_1 = temp;
	}

	if ((strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0 ) &&
			 (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6)   ||
			  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6)))
	{
		m_StrAcquiringCountry = sTranRecord.tlf01.product_codes[18].code;

		if(strlen((char *)sTranRecord.tlf01.num_dbrefunds)>0 && 0 == m_StrAcquiringCountry.GetLength())
		{
			m_StrAcquiringCountry = sTranRecord.tlf01.num_dbrefunds;
		}
	}
	else
	{
		m_StrAcquiringCountry = sTranRecord.tlf01.currency_code;
	}

	
	if(strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
	{
		if(strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0)
		{
			if((sTranRecord.tlf01.nfi_seq_nbr[0] != 0x00) )
			{
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.nfi_seq_nbr;
				//m_StrOriginalTxnAmount = sTranRecord.tlf01.sales_amount;
				temp = sTranRecord.tlf01.sales_amount;
				//m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);

				if (0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3)) 				  
				{
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
					
					if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
						 0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
					{
						temp = sTranRecord.tlf01.refund_amount;
						m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
					}
				}
				else
				{
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
			}

			}
			else
			{
				//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
				if (0 == strncmp((char *)sTranRecord.tlf01.currency_code, ICELAND_KRONA_CURRENCY, 3))
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
			}
			
		}
		else
		{
			//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
			temp = sTranRecord.tlf01.total_amount;
			m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
			if((0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nceqit",6) ||
			    0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"neqit2",6)) &&
			    0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3))
			{
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
			}
			m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
		}
	}
	else
	{
		//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
		temp = sTranRecord.tlf01.total_amount;
		m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
		//m_StrOriginalTxnAmount = 45678;
		m_OrgTxnAmtCurrency = sTranRecord.tlf01.currency_code;
	}
	
	if ((strncmp((char *)sTranRecord.tlf01.handler_queue,"ncdci", 5) == 0) &&
		(strncmp((char *)sTranRecord.tlf01.message_type, "0100", 4) == 0 ||
		 strncmp((char *)sTranRecord.tlf01.message_type, "0120", 4) == 0 ||
		 strncmp((char *)sTranRecord.tlf01.message_type, "0420", 4) == 0) &&
		(sTranRecord.tlf01.product_codes[13].code[0] != 0x00))

	{
		//m_StrOriginalTxnAmount = sTranRecord.tlf01.total_amount;
		temp = sTranRecord.tlf01.orig_amount;
		m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
		//m_StrOriginalTxnAmount = 45678;
		m_OrgTxnAmtCurrency = (CString)(char *)sTranRecord.tlf01.product_codes[13].code;

		if(0 == strncmp((char *)sTranRecord.tlf01.product_codes[13].code, ICELAND_KRONA_CURRENCY, 3))
			m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
	}

	if (strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0)
	{
		if (0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6) )
		{
			dec_plcs = atoi ((char *)sccl01.currency_decimal_places);

			if (((char *)sTranRecord.tlf01.nfi_seq_nbr[0] != '\0') &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3))
			{
				if (sccl01.currency_decimal_places[0] == '0')
				{			/* for 3rd currency stored in nfi_seq_nbr */
					temp = sTranRecord.tlf01.product_codes[7].amount;
					m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.refund_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
				}
				else
				{
					if (sccl01.currency_decimal_places[0] == '\0')
					{
						dec_plcs = 2;
					}
					temp = sTranRecord.tlf01.product_codes[7].amount;
					m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.refund_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp,dec_plcs);
				}
			}
			else 
			{		/* Txn currency code is 840 or 608  */
				if (sccl01.currency_decimal_places[0] == '0')
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
				}
				else
				{
					temp = sTranRecord.tlf01.sales_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount_with_decplcs(temp,dec_plcs);
				}

			}

		}
		else if (true == Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
			if (((char *)sTranRecord.tlf01.product_codes[13].code[0] != '\0') &&
				0 != strncmp((char *)sTranRecord.tlf01.product_codes[13].code, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.product_codes[13].code, PESO, 3))
			{
				if (sccl01.currency_decimal_places[0] == '0')
				{			/* for 3rd currency stored in product13_code */
							temp = sTranRecord.tlf01.product_codes[7].amount;
							m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.orig_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
				}
				else
				{
							temp = sTranRecord.tlf01.product_codes[7].amount;
							m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.orig_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
			}
			else
			{
						/* Txn currency code is 840 or 608  */
				if (sccl01.currency_decimal_places[0] == '0')
				{	
					temp = sTranRecord.tlf01.total_amount;
					m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);
				}
				else
				{
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
					temp = sTranRecord.tlf01.total_amount;
					m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
				}
			}

		}
		else if ((char *)sTranRecord.tlf01.nfi_seq_nbr[0] != '\0' &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, USD, 3) &&
				0 != strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, PESO, 3))
		{
			/* Txn currency code is not 840 or 608  for other card brands*/
			if (sccl01.currency_decimal_places[0] == '0')
			{	
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);

			}
			else
			{
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.sales_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
			}
		}
		else
		{	/* Txn currency code is  840 or 608  for other card brands*/
			if (sccl01.currency_decimal_places[0] == '0')
			{	
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.GetAmount_not_inDecimal(temp);
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.GetAmount_not_inDecimal(temp);

			}
			else
			{
				temp = sTranRecord.tlf01.total_amount;
				m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
				temp = sTranRecord.tlf01.total_amount;
				m_StrOriginalTxnAmount =  myUtilsObject.DisplayAmount(temp);
			}
		}
	}


	/*************************************************************************************/
	
	/* Deferred Payment Data */
	if ( 0x00 != sTranRecord.tlf01.deferred_term_length[0] )
	{
		temp = sTranRecord.tlf01.def_gross_amt;
		m_StrGrossAmountFld =  myUtilsObject.DisplayAmount(temp);
				
		temp = sTranRecord.tlf01.monthly_amort_amt;
		m_StrMonthlyAmortFld =  myUtilsObject.DisplayAmount(temp);
		
		m_StrTermLengthFld =  sTranRecord.tlf01.deferred_term_length;
		
		/* Convert Def Factor from nnnnnnn to nnn.nnnn (0050000 = 5%). */
		m_StrFactorFld = sTranRecord.tlf01.deferred_factor;
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			nFactor = atof( m_StrFactorFld ) / 1000000.0;
			sprintf( strFactor, "%8.6f", nFactor );
		}
		else
		{
			nFactor = atof( m_StrFactorFld ) / 10000.0;
			sprintf( strFactor, "%8.4f", nFactor );
		}
		m_StrFactorFld = strFactor;
	}
	
	//if (m_bViewEmvData)
	//{
		/* Determine the block size */
		//int nLength;
		//sscanf((const char *)sTranRecord.emvraw.DATA_LENGTH, "%d", &nLength );		
		//nLength = genutil_bcd_to_int( sTranRecord.emvraw.RAW_DATA, 2 );
		
		//if (nLength > 0)
		//{
		//	char EmvRawAsciiBuf[600];
		//	
		//	nLength += 2;	/*include the length field*/
		//	genutil_bin_to_ascii(sTranRecord.emvraw.RAW_DATA, nLength, EmvRawAsciiBuf, sizeof(EmvRawAsciiBuf));
		//	m_StrEmvFld            =  EmvRawAsciiBuf; 
		//}
	//}

	if  (true ==  Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num) || true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
	{
		if (strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1100",4);
		else if (strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1120",4);
		else if (strncmp((char *)sTranRecord.tlf01.message_type,"0420",4) == 0 )
				strncpy((char *)sTranRecord.tlf01.message_type,"1420",4);
	}

	m_StrTranMessageTypeFld = (CString)(char *)sTranRecord.tlf01.message_type;
	m_StrTranTypeFld = (CString)(char *) sTranRecord.tlf01.tx_key;
	if ( atoi(m_StrTranTypeFld) > 100 )
		strResponse = " RESPONSE";
	else
		strResponse = "";
	
	switch (sTranRecord.tlf01.tx_key)
	{
	case AUTH_INVALID:
		m_StrTranTypeFld = "INVALID" + strResponse;
		break;
		
	case AUTH_SALE:
	case AUTH_SALE_RESPONSE:
			m_StrTranTypeFld = "SALE" + strResponse;
			if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
			{
				m_StrTranTypeFld = "MP SALE" + strResponse;
			}
		break;
		
	case  AUTH_REFUND:
	case  AUTH_REFUND_RESPONSE:
	case AUTH_PRE_AUTHORIZATION_CANCLE:
	case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE:
		m_StrTranTypeFld = "AUTH REFUND" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "MP REFUND" + strResponse;
		}
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
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "MP OFFLINE SALE" + strResponse;
		}
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
	case AUTH_MONEY_SEND:
	case AUTH_MONEY_SEND_RESPONSE:
	case AUTH_FAST_FUND:
	case AUTH_FAST_FUND_RESPONSE:
	case AUTH_OCT_TX:
	case AUTH_OCT_TX_RESPONSE:
		{ 
			if ( strcmp ((char *)sTranRecord.tlf01.processing_code, "180000" ) == 0)
			{
				m_StrTranTypeFld = "QUASI CASH" + strResponse;
			}
			else if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
			{
				m_StrTranTypeFld = "DEFERRED PURCHASE AUTHORIZATION RESPONSE";
			}
			/*else if(m_StrCardNumberFld[0]== '6')
			{
		
				m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
			}*/
			else
			{
				if ( ( strcmp((const char *)(sTranRecord.tlf01.voice_auth), "Y") == 0) ||
					( strcmp((const char *)(sTranRecord.tlf01.voice_auth), "y") == 0) )
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
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "VOID MP SALE" + strResponse;
		}
		break;
		
	case AUTH_VOID_REFUND:
	case AUTH_VOID_REFUND_RESPONSE:
		m_StrTranTypeFld = "VOID REFUND" + strResponse; 
		break;
		
	case AUTH_SETTLE:
	case AUTH_SETTLE_RESPONSE:
		m_StrTranTypeFld = "SETTLE" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "SETTLE MP" + strResponse;
		}
		break;
		
	case AUTH_BATCH_UPLOAD:
	case AUTH_BATCH_UPLOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH UPLOAD" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "BATCH UPLOAD MP" + strResponse;
		}
		break;
		
	case AUTH_SETTLE_TRAILER:
	case AUTH_SETTLE_TRAILER_RESPONSE:
		m_StrTranTypeFld = "SETTLE TRAILER" + strResponse; 
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "SETTLE TRAILER MP" + strResponse;
		}
		break;
		
	case AUTH_STATISTICS:
	case AUTH_STATISTICS_RESPONSE:
		m_StrTranTypeFld = "STATISTICS" + strResponse; 
		break;
		
	case AUTH_REVERSAL:
	case AUTH_REVERSAL_RESPONSE:
		
		m_StrTranTypeFld = "REVERSAL - " ;
		if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
		{
			m_StrTranTypeFld = "REVERSAL MP - " ;
		}
		
		if( 0 == strncmp((char *) sTranRecord.tlf01.product_codes[1].quantity, "01",2))
		{
		   m_StrCurrencyCodeFld = "840";
		   m_StrAcquiringCountry = m_StrCurrencyCodeFld;
		}
		if ( strcmp ((char *)sTranRecord.tlf01.processing_code, "700000" ) == 0)
            m_StrTranTypeFld +="DEFERRED";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "180000" ) == 0)
            m_StrTranTypeFld +="QUASI CASH";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0100" ) == 0))
            m_StrTranTypeFld +="AUTHORIZATION";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="PURCHASE";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"000000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE COMPLETION";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "200000" ) == 0) 
            m_StrTranTypeFld +="REFUND";
		
		else if( strcmp ((char *)sTranRecord.tlf01.processing_code, "010000" ) == 0) 
            m_StrTranTypeFld +="CASH";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID SALE";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0200" ) == 0))
            m_StrTranTypeFld +="VOID REFUND";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"020000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="SALE ADJUSTMENT";
		
		else if((strcmp((char *)sTranRecord.tlf01.processing_code,"220000") == 0) &&
			(strcmp((char *)sTranRecord.tlf01.orig_message, "0220" ) == 0))
            m_StrTranTypeFld +="REFUND";
		
		if ( sTranRecord.tlf01.orig_message[0] == 0x00 )
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
		
	
	case AUTH_ATM_CONFIRM:
		m_StrTranTypeFld = "REVERSAL" + strResponse; 
	break;

   }

   if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) )
   {
		if(strncmp((char *)sTranRecord.tlf01.message_type,"0100",4)   == 0 &&
		  strncmp((char *)sTranRecord.tlf01.processing_code,"33",2)   == 0)
		{
			m_StrTranTypeFld = "Account Verify";
		}
			
   }

   char startTimeBuf[20] = {0};
   if(strlen((char*)sTranRecord.tlf01.tran_start_time) > 14)
   {
	   //we are storing DE12 TransmissionTimeStamp(YYMMDD<epoch-10bytes>) in tran_start_time for some txns
	   strncpy(startTimeBuf, (char*)sTranRecord.tlf01.tran_start_time+6, 10);
   }
   else
   {
	   strncpy(startTimeBuf, (char*)sTranRecord.tlf01.tran_start_time, 10);
   }
   time_t rawtime = atoi(startTimeBuf);
   struct tm  ts;
   char       buf[30];

   //Format time, "hh:mm:ss zzz"
   ts = *localtime(&rawtime);
   strftime(buf, sizeof(buf), "%H:%M:%S", &ts);
 
   m_StrInvoiceNumberFld =  sTranRecord.tlf01.invoice_number;
   
   m_StrTranDateFld   = m_StrTranDateFld.Right(2)   + "/" + m_StrTranDateFld.Mid(4,2)   + "/" + m_StrTranDateFld.Left(4);
   m_StrTranTimeFld   = m_StrTranTimeFld.Left(2)    + ":" + m_StrTranTimeFld.Mid(2,2)   + ":" + m_StrTranTimeFld.Right(2);
   m_StrSystemDateFld = m_StrSystemDateFld.Right(2) + "/" + m_StrSystemDateFld.Mid(4,2) + "/" + m_StrSystemDateFld.Left(4);
   //m_StrSystemDateTimeFld = strSysDt_Tm.Left(4) + "-" + strSysDt_Tm.Mid(4,2) + "-" + strSysDt_Tm.Right(2) + " " + m_StrTranTimeFld;
   m_StrSystemDateTimeFld = strSysDt_Tm.Left(4) + "-" + strSysDt_Tm.Mid(4,2) + "-" + strSysDt_Tm.Right(2) + " " + buf;
   
  /* m_StrBdoMercAcqTx = sTranRecord.tlf01.product_codes[4].code;*/
   m_StrAuthProfile  = sTranRecord.tlf01.product_codes[3].code;
   m_StrBinLow		  = sTranRecord.tlf01.product_codes[2].code;
   if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "5.000") == 0)
   {
	   m_StrEarMarkedCurrency = "PHP";
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

		if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6) ||
		   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis" ,5))
		{
			m_StrEarmarkedamount1 = "PHP";
			m_StrEarmarkedcurrency = myUtilsObject.DisplayAmount(temp);
		}
   }
   else if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "7.000") == 0)
   {
	   m_StrEarMarkedCurrency = "USD";
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6) ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6) ||
		   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis" ,5))
		{
	   		 m_StrEarmarkedcurrency = "USD";
			 m_StrEarmarkedamount1   = myUtilsObject.DisplayAmount(temp);
		}

		if((0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5)   ||
			0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6)) &&
			(0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ICELAND_KRONA_CURRENCY, 3) ||
			 0 == strncmp((char *)sTranRecord.tlf01.nfi_seq_nbr, ZIMBABWE_GOLD, 3)))
		{
			temp = sTranRecord.tlf01.total_amount ;
			m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);
		}
   }
   else if( strcmp( (char *) sTranRecord.tlf01.host_start_time, "0.000") == 0)
   {
	   m_StrEarMarkedCurrency = sTranRecord.tlf01.currency_code;;
	   temp = sTranRecord.tlf01.product_codes[9].amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

	   if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6) ||
		   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6) ||
		  0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis" ,5))
		{
		   m_StrEarmarkedcurrency = sTranRecord.tlf01.currency_code;
		   m_StrEarmarkedamount1   =  myUtilsObject.DisplayAmount(temp);
		}
   }
   else 
   {
	   m_StrEarMarkedCurrency	= sTranRecord.tlf01.currency_code;
	   temp = sTranRecord.tlf01.total_amount ;
	   m_StrEarMarkedAmount =  myUtilsObject.DisplayAmount(temp);

		if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6) ||
		   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6)||
		   0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis" ,5))
		{
		   m_StrEarmarkedcurrency = sTranRecord.tlf01.currency_code;
		   m_StrEarmarkedamount1   =  myUtilsObject.DisplayAmount(temp);
		}
	   
   }

	if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd" ,6) ||
		0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3" ,6) ||
		0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"ncvis" ,5))
	{
		temp = _T("");
		temp =  sTranRecord.tlf01.product_codes[7].amount ;
		m_StrBillingamount =  myUtilsObject.DisplayAmount(temp);
		m_StrBillingCurrency  = sTranRecord.tlf01.product_codes[7].code;
	}

   m_StrServiceCode  = sTranRecord.tlf01.service_code;

   m_StrVisaMerchantIdentifier  = sTranRecord.tlf01.product_codes[4].amount;
   m_StrDigitalCommProgramIndicator = sTranRecord.tlf01.product_codes[8].code;
   m_StrDigitalEntryIdentifier = sTranRecord.tlf01.product_codes[5].amount;
   m_StrDigitalEntryIdentifier = _T("");
      /* NII value*/
   if(strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) == 0)
   {
      m_StrNii = sTranRecord.tlf01.nii;
   }

   /********************************************************************************************************************
   if(strcmp((char *)sTranRecord.tlf01.product_codes[1].code, STANDIN) == 0)
   {
   m_ChkAtpStip.SetCheck(1);
   m_ChkSchemeStip.SetCheck(0);
   
	 }
	 else
	 {
	 m_ChkAtpStip.SetCheck(0);
	 m_ChkSchemeStip.SetCheck(1);
	 }
	 
	   if( (sTranRecord.tlf01.tx_key == AUTH_AUTHORIZATION_OVERRIDE) || (sTranRecord.tlf01.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) )
	   {
	   m_ChkVoice.SetCheck(1);
	   }
	   else
	   {
	   m_ChkVoice.SetCheck(0);
	   }
	   
		 if( strlen((char *)sTranRecord.tlf01.nbr_of_prod_codes) > 0)
		 {
		 m_ChkOMD.SetCheck(1);
		 }
		 else
		 {
		 m_ChkOMD.SetCheck(0);
		 }
		 
		   /*if( strlen((char *)sTranRecord.tlf01.security_response_code ) > 0)
		   {
		   m_ChkCVV2.SetCheck(1);
		   }
		   else
		   {
		   m_ChkCVV2.SetCheck(0);
	}*/
   /********************************************************************************************************************/
   
   
   /*********************************************************************************************************
   m_ChkHostProcessed
   ***********************************m_ChkAtpStip
   ***********************************m_ChkSchemeStip
   ***********************************m_ChkVoice
   ***********************************m_ChkOMD
   m_ChkCVVPresent
   m_ChkCvvValidated
   m_ChkCVV2Present
   m_ChkCVV2Validated
   m_ChkPINPresent
   m_ChkPINValidated
   m_ChkARQCPresent
   m_ChkArqcValidated
   m_ChkCAVVPresent
   m_ChkCAVVValidated
   ***********************************m_CntlFGResultFld
   /*********************************************************************************************************/
   
 /*newwwwwwwwwwwwwwww********************************************************************************************************/
   
   m_ChkHostProcessed.SetCheck(0);
   m_ChkSchemeStip.SetCheck(0);
   m_ChkTOAtpStip.SetCheck(0);
   m_ChkAtpStip.SetCheck(1);

   if(strcmp((char *)sTranRecord.tlf01.product_codes[3].quantity, "19") == 0)
   {
	   m_ChkTOAtpStip.SetCheck(1);
   }
   /*else if(strcmp((char *)sTranRecord.tlf01.product_codes[1].code, STANDIN) == 0)
   {
	   m_ChkAtpStip.SetCheck(1);
   }
   else
   {
	   m_ChkHostProcessed.SetCheck(1);
   }*/
 
   if( (sTranRecord.tlf01.tx_key == AUTH_AUTHORIZATION_OVERRIDE) || (sTranRecord.tlf01.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) )
   {
	   m_ChkVoice.SetCheck(1);
   }
   else
   {
	   m_ChkVoice.SetCheck(0);
   }
   
   if( strlen((char *)sTranRecord.tlf01.nbr_of_prod_codes) > 0 && strcmp((char *)sTranRecord.tlf01.product_codes[18].code,"OMD") == 0)
   {
	   m_ChkOMD.SetCheck(1);
   }
   else
   {
	   m_ChkOMD.SetCheck(0);
   }
   
  /* if( (char *)sTranRecord.tlf01.cvc_data != '\0')
   {
	   m_ChkCVVPresent.SetCheck(1);
	   m_ChkCVV2Present.SetCheck(1);
   }
   else
   {
	   m_ChkCVVPresent.SetCheck(0);
	   m_ChkCVV2Present.SetCheck(0);
   }*/
   
  /* if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && ( (char *)sTranRecord.tlf01.cvc_data != '\0') )
   {
	   m_ChkCvvValidated.SetCheck(1);
	   m_ChkCVV2Validated.SetCheck(1);
   }
   else
   {
	   m_ChkCvvValidated.SetCheck(0);
	   m_ChkCVV2Validated.SetCheck(0);
   }*/
   
   
   if ( 0 == strlen((char *)sTranRecord.tlf01.pin_block))
	   
   {
	   m_ChkPINPresent.SetCheck(0);
   }
   else
   {
	   m_ChkPINPresent.SetCheck(1);
   }
   
   if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && ( 0 == strlen((char *)sTranRecord.tlf01.pin_block)) )
   {
	   m_ChkPINValidated.SetCheck(1);
   }
   else
   {
	   m_ChkPINValidated.SetCheck(0);
   }
   
   if((char *)sTranRecord.tlf01.pos_entry_mode == "0051" || (char *)sTranRecord.tlf01.pos_entry_mode == "0071" ||
	   (char *)sTranRecord.tlf01.pos_entry_mode == "0081")
   {
	   m_ChkARQCPresent.SetCheck(1);
   }
   else
   {
	   m_ChkARQCPresent.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.tlf01.response_code,"00") == 0)
   {
	   m_ChkArqcValidated.SetCheck(1);
   }
   else
   {
	   m_ChkArqcValidated.SetCheck(0);
   }
   
   if(strcmp((char *)sTranRecord.tlf01.nfi_end_date,"1") == 0)
   {
	   m_ChkCAVVPresent.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVPresent.SetCheck(0);
   }
   
   if( (strcmp((char *)sTranRecord.tlf01.security_response_code,"00") == 0) && (strcmp((char *)sTranRecord.tlf01.nfi_end_date,"1") == 0))
   {
	   m_ChkCAVVValidated.SetCheck(1);
   }
   else
   {
	   m_ChkCAVVValidated.SetCheck(0);
   }

	if(strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) == 0 ||
	strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE",5) == 0)
  {
	   strcpy((char *)mra01_table.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id );
   		rcode = txdsapi_get_record ( (pBYTE)&mra01_table, sizeof (mra01_table), MRA01_DATA, strErrorMessage);
		if(rcode == PTEMSG_OK)
		{
			m_Strmrasenttime = mra01_table.transaction_send_time ;
		}
		else
		{
			m_Strmrasenttime = _T("");
		}
  }
   else
   {
		// Do nothing
   }
   /*   
   ***********************************m_CntlFGResultFld*/
 /*********************************************************************************************************/
   
   UpdateData(FALSE);
   
   
   
   return TRUE;
}



/*******************************************************/

BOOL CTransactionDetails::PopulateFGuardDialogFromDatabase ( FGUARD_RECORD sFGuardRecord  )
{
	CMyUtils myUtilsObject;
	CHAR     strErrorMessage[200] = "";	
	CString  temp;
	CString  strResponse;
	
	int rcode = txdsapi_get_record ( (pBYTE)&sFGuardRecord, sizeof (sFGuardRecord), FGUARD_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "ERROR:The FGuard record for this batch was not found.";
			break;
		}
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		return FALSE;
	}
	
	m_StrTranTypeFld          =  "Fraud Guard Response";
	m_StrInvoiceNumberFld     = " ";
	m_StrAcquiringInstIDFld   =  sFGuardRecord.fguard.acquiring_id;
	m_StrAuthIDResponseFld    =  sFGuardRecord.fguard.auth_number;
	m_StrCardNumberFld        =  sFGuardRecord.fguard.card_num;
	m_StrForwardingInstIDFld  =  " ";
	m_StrHostTimeFld          =  " ";
	m_StrMerchantIDFld        =  sFGuardRecord.fguard.merchant_id;
	m_StrPOSConditionCodeFld  =  sFGuardRecord.fguard.pos_condition_code; 
	m_StrPOSEntryModeFld      =  sFGuardRecord.fguard.pos_entry_mode;
	m_StrProcessingCodeFld    =  sFGuardRecord.fguard.processing_code;
	m_StrResponseCodeFld      =  sFGuardRecord.fguard.response_code;
	m_StrResponseTextFld      =  sFGuardRecord.fguard.result_text;
	m_StrRetrievalRefNumFld   =  " ";
	m_StrSystemTraceNumberFld =  sFGuardRecord.fguard.sys_trace_audit_num;
	m_StrTerminalIDFld        =  sFGuardRecord.fguard.terminal_id;
	m_StrFGResultFld			 =  sFGuardRecord.fguard.result_code;
	checkMaskCardNumber(); //To mask the card number
	temp = sFGuardRecord.fguard.amount_peso;
	m_StrTranAmountFld         =  myUtilsObject.DisplayAmount(temp);

	m_StrNRID = sTranRecord.tlf01.resp_source;
	m_StrActionCode = sTranRecord.tlf01.product_codes[12].code;
	m_StrATP_sbatchtime = sTranRecord.tlf01.kek;
	
	m_StrCreditLimitFld		  = " ";
	m_StrOutstandingBalanceFld = " ";
	m_StrExpirationDateFld     = " ";
    
	temp = sFGuardRecord.fguard.timestamp;/* format = yy-mm-ddThh:mm:ss*/
	if(temp.IsEmpty() || temp.GetLength() < sizeof(sFGuardRecord.fguard.timestamp)-1)
	{
		m_StrTranDateFld	      = " ";
		m_StrTranTimeFld       = " ";
	}
	else
	{
		m_StrTranDateFld   = temp.Mid(8,2)   + "/" + temp.Mid(5,2)   + "/" + temp.Left(4);
		m_StrTranTimeFld   = temp.Right(8);
	}
	
	temp = sFGuardRecord.fguard.system_date;/*yyyymmdd*/
	if(temp.IsEmpty() || temp.GetLength() < sizeof(sFGuardRecord.fguard.system_date)-1)
		m_StrSystemDateFld     = " ";
	else
		m_StrSystemDateFld = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
	
	m_StrTextFld               =  sFGuardRecord.fguard.result_text;
	m_StrCurrencyCodeFld       =  sFGuardRecord.fguard.currency_code_txn;
	m_StrOperatorIDFld         =  " ";
	
	
	if (m_bViewRawData)
	{
		/*get record from fg_raw table using transaction id*/
		strcpy((char *)sFGuardRecord.fgraw.transaction_id, (char *)sFGuardRecord.fguard.transaction_id);
		
		int rcode = txdsapi_get_record ( (pBYTE)&sFGuardRecord.fgraw, sizeof (sFGuardRecord.fgraw), FGRAW_DATA, strErrorMessage);
		if ( rcode != PTEMSG_OK )
		{
			switch( rcode )
			{
			case PTEMSG_NOT_FOUND:
				m_StrMessageLbl = "ERROR:The FGuard raw record was not found.";
				break;
			}
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
			return FALSE;
		}
		
		m_StrEmvFld            =  sFGuardRecord.fgraw.raw_data;
		
	}
	
	
	UpdateData(FALSE);
	
	return TRUE;
}/*PopulateFguardDialogFromDatabase*/


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
	m_StrPOSConditionCodeFld_1 = _T("");
	m_StrPOSEntryModeFld = _T("");
	m_StrProcessingCodeFld = _T("");
	m_StrProcessingCodeFld_1 = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrResponseCodeFldbyScheme = _T("");
	m_StrResponseTextFld = _T("");
	m_StrRetrievalRefNumFld = _T("");
	m_StrSystemTraceNumberFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrTranAmountFld = _T("");
	m_StrReplacementAmountFld = _T("");
	m_StrTranDateFld = _T("");
	m_StrTranTimeFld = _T("");
	m_StrTranTypeFld = _T("");	
	m_StrTranMessageTypeFld = _T("");
	m_StrTextFld = _T("");
	m_StrExpirationDateFld = _T("");
	m_StrEmvFld = _T("");

	m_StrNRID = _T("");
	m_StrActionCode = _T("");
	m_StrATP_sbatchtime = _T("");
	m_StrExtCondCode = _T("");	
	m_Strmrasenttime= _T("");
	m_StrHostNameFld= _T("");
}

INT CTransactionDetails :: Check_If_AMEX_Transaction (char *card_num)
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

INT CTransactionDetails :: Check_If_Acquiring_Transaction (char * handler_queue)
{
	if(	strncmp(handler_queue,"dcpiso",6) == 0 ||
		strncmp(handler_queue,"dcpimp",6) == 0 ||
		strncmp(handler_queue,"dcpos",5) == 0 )
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CTransactionDetails :: Check_If_Issuing_Transaction (char * handler_queue)
{
	if((strncmp(handler_queue,"ncvis3",6) == 0 ||
		strncmp(handler_queue,"ncmcr",5) == 0 ||
		strncmp(handler_queue,"ncpos",5) == 0 ||
		strncmp(handler_queue,"ncatm",5) == 0 ||
		strncmp(handler_queue,"ncjcb",5) == 0 ||
		strncmp(handler_queue,"nccup",5) == 0 ||
		strncmp(handler_queue,"ncdci",5) == 0 ) ||
		(strncmp(handler_queue,"ncvis3",6) == 0 &&
		 (0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nceqit",6) ||
		  0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"neqit2",6))))

	{
		return true ;
	}
	else
	{
		return false;
	}
}
void CTransactionDetails :: match_ISO_to_Diners_response_code(char *iso_response_code,char * diners_response_code)
{
	   #define NB 86

   INT  i;
   CHAR *iso_response_codes [NB] =  {"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "08",  "09",  "10",  "11",  "12",  "13",  "14",  "15",
                                       "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",
                                       "24",  "25",  "26",  "27",  "28",  "29",  "30",  "31",
                                       "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",
                                       "40",  "41",  "42",  "43",  "44",  "51",  "52",  "53",
									   "54",  "55",  "58",  "59",  "60",  "61",  "62",  "63",
									   "64",  "65",  "66",  "68",  "75",  "76",  "77",  "78",
									   "79",  "80",  "81",  "82",  "84",  "85",  "86",  "87",
									   "88",  "89",  "90",  "91",  "92",  "93",  "94",  "95",
									   "96",  "97",  "98",  "NO",  "N7",  "00"};

   CHAR *DCI_response_code [NB] =    {"081", "107", "107", "109", "200", "182", "182", "200",
                                        "001", "182", "081", "081", "182", "110", "182", "182",
                                        "081", "182", "909", "182", "909", "182", "909", "182",
                                        "   ", "182", "   ", "   ", "182", "   ", "904", "905",
                                        "182", "101", "102", "182", "204", "203", "206", "182",
                                        "115", "208", "182", "209", "182", "182", "182", "182",
									    "101", "117", "182", "210", "203", "182", "104", "182",
									    "182", "182", "205", "182", "106", "182", "182", "182",
									    "182", "307", "909", "182", "182", "081", "909", "182",
									    "909", "182", "910", "182", "909", "124", "909", "909",
									    "909", "182", "182", "909", "182", "400"};

	for (i=0; i<NB; i++)
	{
		if (0 == strncmp((char *)iso_response_code, iso_response_codes[i], 2))
		{
			strncpy(diners_response_code, DCI_response_code[i], 3);
			break;
		}
	 }

}

void CTransactionDetails ::match_ISO_to_AMEX_response_code(char *iso_response_code,char * amex_response_code,BYTE tx_key)
{
	   #define NB 42

   INT  i;

   CHAR *iso_response_codes [NB] =   {"00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "12",  "13",  "14",  "15",  "19",  "21",  "25",  "28",
                                       "39",  "41",  "43",  "51",  "52",  "53",  "54",  "55",
                                       "57",  "58",  "61",  "62",  "63",  "65",  "75",  "76",
                                       "77",  "80",  "81",  "82",  "85",  "86",  "91",  "92",
                                       "93",  "96"};

   CHAR *auth_advice_amex [NB]   =  {"900", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "904", "904", "904", "904",
                                       "904", "904", "904", "904", "900", "904", "904", "904",
                                       "904", "904"};
  CHAR *auth_reversal_amex [NB]  =  {"400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400", "400", "400", "400", "400", "400", "400",
                                       "400", "400"};

  if (tx_key == AUTH_ADVICE_RESPONSE || tx_key == AUTH_ADVICE )
   {
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp((char *)iso_response_code, (char *)iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_advice_amex[i], 3);
            break;
         }
      }
   }
   else if (tx_key == AUTH_REVERSAL_ADVICE_RESPONSE || tx_key == AUTH_REVERSAL_ADVICE)
   {
      for (i=0; i<NB; i++)
      {
         if (0 == strncmp((char *)iso_response_code, (char *)iso_response_codes[i], 2))
         {
            strncpy(amex_response_code, auth_reversal_amex[i], 3);
            break;
         }
      }
   }
}



INT CTransactionDetails :: Check_If_Diners_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '0') ||
		(card_num[1] == '6') ||
		(card_num[1] == '8') ||
		(card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((card_num[0]=='6') &&
			 ((card_num[1] == '0') ||
			 (card_num[1] == '4') ||
			 (card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}



INT CTransactionDetails :: Check_If_MC_Transaction (char *card_num)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	card_num[0]=='5')
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}


INT CTransactionDetails :: Check_If_VISA_Transaction (char *card_num)
{
	if (card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CTransactionDetails :: Check_If_JCB_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   (card_num[1]=='5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
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

void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }
} /* genutil_bcd_to_asc */

void CTransactionDetails::checkMaskCardNumber()
{
	if(m_bMaskCardNumbers)
	{
		CString str = m_StrCardNumberFld;
		OriginalCardNoList.AddHead(str);
		int len = str.GetLength();
		/*to avoid the crash Issue caused by Log ON Transaction where card number(dummy) will be of 5 digits & 
		masking should not happen for a card number less than 7 digits*/
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

void CTransactionDetails::OnFraudTxnDetailsButton()
{

	CFraudTransactionDetails FraudTransactionDetailsDlg;
		UpdateData();
	FraudTransactionDetailsDlg.DoModal();

	/* fetching Fraud details and displaying is done in Fraud_txn OnInitDialog function*/
}

