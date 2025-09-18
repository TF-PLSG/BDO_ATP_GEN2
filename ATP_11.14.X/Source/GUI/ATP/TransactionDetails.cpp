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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TransactionDetails.cpp  $
   
      Rev 1.12   Dec 01 2003 11:30:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.11   May 12 2000 14:31:12   iarustam
    
   
      Rev 1.10   Jan 25 2000 14:01:00   iarustam
   bug fixes
   
      Rev 1.9   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:01:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:26   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:34   iarustam
   Initial Release
   
      Rev 1.6   Jun 02 1999 16:25:54   iarustam
   Bug fixes
   
      Rev 1.5   May 13 1999 14:58:06   iarustam
   Bug fixes
   
      Rev 1.4   May 10 1999 11:40:28   dperson
   Standardized error messages
   
      Rev 1.3   Apr 29 1999 14:57:10   iarustam
   Bug fixes
   
      Rev 1.2   Apr 15 1999 14:19:38   iarustam
   Bug fixes
   
   
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
#include "atp.h"
#include "TransactionDetails.h"
#include "MyUtils.h"

#include "SharedMem.h"	



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
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
	m_StrResponseCodeFld = _T("");
	m_StrTextFld = _T("");
	m_StrExpirationDateFld = _T("");
	//}}AFX_DATA_INIT
}


void CTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
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
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_TEXT_FLD, m_StrTextFld);
	DDX_Text(pDX, IDC_EXP_DATE_FLD, m_StrExpirationDateFld);
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
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bTran = TRUE;

	if (m_nDialogMode != MODE_DONE && m_nDialogMode != MODE_ADD)
	{		
		PopulateTranDialogFromDatabase ( sTranRecord );
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTransactionDetails::PopulateTranDialogFromDatabase ( TLF01 sTranRecord  )
{	
	CMyUtils myUtilsObject;
	CHAR strErrorMessage[200] = "";	
	CString  temp;
//	int res, len;
//	char strSum [50];

	memcpy (sTranRecord.primary_key.transaction_id, m_strTranID, sizeof (sTranRecord.primary_key.transaction_id));
	int rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (TLF01), TLF01_DATA, strErrorMessage);        
	if ( rcode != PTEMSG_OK )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
		m_StrMessageLbl = "ERROR: Transaction record for this batch does not exist";
            break;
      }
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
	
	m_StrAcquiringInstIDFld =  sTranRecord.acquiring_id;
	m_StrAuthIDResponseFld =  sTranRecord.auth_number;
	m_StrCardNumberFld =  sTranRecord.card_num;
	m_StrForwardingInstIDFld =  sTranRecord.forwarding_institution_id;
	m_StrHostTimeFld =  sTranRecord.host_finish_time;
	m_StrMerchantIDFld =  sTranRecord.merchant_id;
	m_StrPOSConditionCodeFld = sTranRecord.pos_condition_code; 
	m_StrPOSEntryModeFld =  sTranRecord.pos_entry_mode;
	m_StrProcessingCodeFld =  sTranRecord.processing_code;
	m_StrResponseCodeFld =  sTranRecord.response_code;
	m_StrResponseTextFld =  sTranRecord.response_text;
	m_StrRetrievalRefNumFld =  sTranRecord.retrieval_ref_num;
	m_StrSystemTraceNumberFld =  sTranRecord.sys_trace_audit_num;
	m_StrTerminalIDFld =  sTranRecord.terminal_id;
	m_StrTextFld =  sTranRecord.voice_auth_text;
//	m_StrBatchNumberFld = sTranRecord.batch_number;
	temp = sTranRecord.total_amount ;
	m_StrExpirationDateFld = sTranRecord.exp_date;

	m_StrTranAmountFld =  myUtilsObject.DisplayAmount(temp);
	m_StrTranDateFld =  sTranRecord.date_yyyymmdd;
	m_StrTranTimeFld =  sTranRecord.time_hhmmss;

	CString strResponse;
	//m_StrTranTypeFld =  sTranRecord.tx_key;
	m_StrTranTypeFld = (CString)((char *) sTranRecord.tx_key);
	strResponse = "";
	//if ( m_StrTranTypeFld >100 )
	if ( atoi(m_StrTranTypeFld) > 100 )
		strResponse = " RESPONSE";

	switch (sTranRecord.tx_key)
	{
    case AUTH_INVALID:
		m_StrTranTypeFld = "INVALID" + strResponse;
	break;

    case AUTH_SALE: case AUTH_SALE_RESPONSE:
		m_StrTranTypeFld = "SALE" + strResponse;
	break;

	case  AUTH_REFUND: case  AUTH_REFUND_RESPONSE:
		m_StrTranTypeFld = "REFUND" + strResponse; 
	break;
	case  AUTH_CASH_ADVANCE: case  AUTH_CASH_ADVANCE_RESPONSE:
		m_StrTranTypeFld = "CASH ADVANCE" + strResponse; 
	break;
	case AUTH_PAYMENT: 	case AUTH_PAYMENT_RESPONSE:
		m_StrTranTypeFld = "PAYMENT" + strResponse; 
	break;
	case AUTH_OFFLINE_SALE: case AUTH_OFFLINE_SALE_RESPONSE:
		m_StrTranTypeFld = "OFFLINE SALE" + strResponse; 
	break;
	case AUTH_OFFLINE_REFUND: case AUTH_OFFLINE_REFUND_RESPONSE:
		m_StrTranTypeFld = "OFFLINE REFUND" + strResponse; 
	break;
	case AUTH_SALE_ADJUSTMENT: case AUTH_SALE_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "SALE ADJUSTMENT" + strResponse; 
	break;
	case AUTH_REFUND_ADJUSTMENT: case AUTH_REFUND_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "REFUND ADJUSTMENT" + strResponse; 
	break;
	case AUTH_PAYMENT_ADJUSTMENT: case AUTH_PAYMENT_ADJUSTMENT_RESPONSE:
		m_StrTranTypeFld = "PAYMENT ADJUSTMENT" + strResponse; 
	break;
	case AUTH_PRE_AUTHORIZATION: case AUTH_PRE_AUTHORIZATION_RESPONSE:
		m_StrTranTypeFld = "PRE AUTHORIZATION" + strResponse; 
	break;
	case AUTH_AUTHORIZATION: case AUTH_AUTHORIZATION_RESPONSE:
		m_StrTranTypeFld = "AUTHORIZATION" + strResponse; 
	break;
	case AUTH_BALANCE_INQUIRY: case AUTH_BALANCE_INQUIRY_RESPONSE:
		m_StrTranTypeFld = "BALANCE INQUIRY" + strResponse; 
	break;
	case AUTH_VOID_SALE: case AUTH_VOID_SALE_RESPONSE:
		m_StrTranTypeFld = "VOID SALE" + strResponse; 
	break;
	case AUTH_VOID_REFUND: case AUTH_VOID_REFUND_RESPONSE:
		m_StrTranTypeFld = "VOID REFUND" + strResponse; 
	break;
	case AUTH_SETTLE: case AUTH_SETTLE_RESPONSE:
		m_StrTranTypeFld = "SETTLE" + strResponse; 
	break;
	case AUTH_BATCH_UPLOAD: case AUTH_BATCH_UPLOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH UPLOAD" + strResponse; 
	break;
	case AUTH_SETTLE_TRAILER: case AUTH_SETTLE_TRAILER_RESPONSE:
		m_StrTranTypeFld = "SETTLE TRAILER" + strResponse; 
	break;
	case AUTH_STATISTICS: case AUTH_STATISTICS_RESPONSE:
		m_StrTranTypeFld = "STATISTICS" + strResponse; 
	break;
	case AUTH_REVERSAL: case AUTH_REVERSAL_RESPONSE:
		m_StrTranTypeFld = "REVERSAL" + strResponse; 
	break;
	case AUTH_LOGON: case AUTH_LOGON_RESPONSE:
		m_StrTranTypeFld = "LOGON" + strResponse; 
	break;
	case AUTH_TEST: case AUTH_TEST_RESPONSE:
		m_StrTranTypeFld = "TEST" + strResponse; 
	break;
	case AUTH_PIN_CHANGE: case AUTH_PIN_CHANGE_RESPONSE:
		m_StrTranTypeFld = "PIN CHANGE" + strResponse; 
	break;
	case AUTH_VOID_CASH_ADVANCE: case AUTH_VOID_CASH_ADVANCE_RESPONSE:
		m_StrTranTypeFld = "VOID CASH ADVANCE" + strResponse; 
	break;
	case AUTH_SALE_CASH: case AUTH_SALE_CASH_RESPONSE:
		m_StrTranTypeFld = "SALE CASH" + strResponse; 
	break;
	case AUTH_CHECK_VERIFICATION: case AUTH_CHECK_VERIFICATION_RESPONSE:
		m_StrTranTypeFld = "CHECK VERIFICATION" + strResponse; 
	break;
	case AUTH_CARD_VERIFICATION: case AUTH_CARD_VERIFICATION_RESPONSE:
		m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
	break;
	case AUTH_VOID_PRE_AUTHORIZATION: case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
		m_StrTranTypeFld = "VOID PREAUTHORIZATION" + strResponse;
	break;
	case AUTH_SALES_COMPLETION: case AUTH_SALES_COMPLETION_RESPONSE:
		m_StrTranTypeFld = "SALES COMPLETION" + strResponse; 
	break;
	case AUTH_GUARANTEED_LATE_ARRIVAL: //	case AUTH_GUAREANTEED_LATE_ARRIVAL_RESPONSE:
		m_StrTranTypeFld = "GUARANTEED LATE ARRIVAL" + strResponse; 
	break;
	case AUTH_BATCH_DOWN_LINE_LOAD: case AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE:
		m_StrTranTypeFld = "BATCH DOWN LINE LOAD" + strResponse; 
	break;
	case AUTH_SIGNATURE_DATA: case AUTH_SIGNATURE_DATA_RESPONSE:
		m_StrTranTypeFld = "SIGNATURE DATA" + strResponse; 
	break;
	case AUTH_INITIALIZATION: case AUTH_INITIALIZATION_RESPONSE:
		m_StrTranTypeFld = "INITIALIZATION" + strResponse; 
	break;
	case AUTH_PLEASE_WAIT_ADVICE: case AUTH_PLEASE_WAIT_ADVICE_RESPONSE:
		m_StrTranTypeFld = "PLEASE WAIT ADVICE" + strResponse; 
	break;
	case AUTH_NEGATIVE_DOWNLOAD: case AUTH_NEGATIVE_DOWNLOAD_RESPONSE:
		m_StrTranTypeFld = "NEGATIVE DOWNLOAD" + strResponse; 
	break;
	case AUTH_ADMIN_ADVICE: case AUTH_ADMIN_ADVICE_RESPONSE:
		m_StrTranTypeFld = "ADMIN ADVICE" + strResponse; 
	break;
	case AUTH_VOID_PAYMENT: case AUTH_VOID_PAYMENT_RESPONSE:
		m_StrTranTypeFld = "VOID PAYMENT" + strResponse; 
	break;
	case AUTH_REGISTRATION: case AUTH_REGISTRATION_RESPONSE:
		m_StrTranTypeFld = "REGISTRATION" + strResponse; 
	break;
	case AUTH_CHECK_DL: case AUTH_CHECK_DL_RESPONSE:
		m_StrTranTypeFld = "CHECK DL" + strResponse; 
	break;
	case AUTH_BRANCH_PMT: 
		m_StrTranTypeFld = "BRANCH PMT" + strResponse; 
	break;
	case AUTH_QUASI_CASH: case AUTH_QUASI_CASH_RESPONSE: 
		m_StrTranTypeFld = "QUASI CASH" + strResponse; 
	break;
	case AUTH_CASH_ADVANCE_AUTHORIZATION: case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE: 
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

   case AUTH_OFFLINE_VOID_SALE:
	case AUTH_OFFLINE_VOID_SALE_RESPONSE:
		m_StrTranTypeFld = "OFFLINE VOID SALE" + strResponse; 
	break;

   case AUTH_OFFLINE_VOID_REFUND:
	case AUTH_OFFLINE_VOID_REFUND_RESPONSE:
		m_StrTranTypeFld = "OFFLINE VOID REFUND" + strResponse; 
	break;


	}
	m_StrInvoiceNumberFld =  sTranRecord.invoice_number;

   if ( m_StrTranDateFld.GetLength() > 0 )
	   m_StrTranDateFld = m_StrTranDateFld.Right(2) + "/" + m_StrTranDateFld.Mid(4,2) + "/" + m_StrTranDateFld.Left(4);
	
   if ( m_StrTranTimeFld.GetLength() > 0 )
      m_StrTranTimeFld = m_StrTranTimeFld.Left(2) + ":" + m_StrTranTimeFld.Mid(2,2) + ":" + m_StrTranTimeFld.Right(2);
	
	UpdateData(FALSE);

//	memcpy ( &sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));

	return TRUE;

}

void CTransactionDetails::CleanTranDialog()
{
	m_StrHostTimeFld = _T("");
//	m_StrBatchNumberFld = _T("");
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
	m_StrExpirationDateFld = _T("");
}

BOOL CTransactionDetails::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
