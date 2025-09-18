
/*

 Rev        April 19.5.11          Krishna Kiran S@TF
	 Added Reseting Idle time, whenever any event is fired in GUI
*/

// BatchDetails.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "BatchDetails.h"
#include "MyUtils.h"
#include "SharedMem.h"

extern "C"{
   #include "Txutils.h"
   #include "PteTime.h"
   #include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CBatchDetails dialog


CBatchDetails::CBatchDetails(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDetails::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchDetails)
	m_StrAmountFld = _T("");
	m_StrAuthIDFld = _T("");
	m_StrCardNumberFld = _T("");
	m_StrDateFld = _T("");
	m_StrExpDateFld = _T("");
	m_StrInvoiceNumberFld = _T("");
	m_StrTimeFld = _T("");
	m_StrTransactionType = _T("");
	m_StrMessageLbl = _T("");
	m_StrJobNumberFld = _T("");
	m_StrManNumberFld = _T("");
	m_StrRRNFld = _T("");
	m_StrFareAmountFld = _T("");
	m_StrTextFld = _T("");
	//}}AFX_DATA_INIT
}


void CBatchDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchDetails)
	DDX_Control(pDX, IDC_TRANSACTION_TYPE, m_CntlTransactionTypeFld);
	DDX_Control(pDX, IDC_APPLY, m_ApplyButton);
	DDX_Control(pDX, IDC_TEXT, m_CntlTextFld);
	DDX_Control(pDX, IDC_FARE_AMOUNT_FLD, m_CntlFareAmountFld);
	DDX_Control(pDX, IDC_RRN_FLD, m_CntlRRNFld);
	DDX_Control(pDX, IDC_MAN_NUMBER_FLD, m_CntlManNumberFld);
	DDX_Control(pDX, IDC_JOB_NUMBER_FLD, m_CntlJobNumberFld);
	DDX_Control(pDX, IDC_TIME1_FLD, m_CntlTimeFld);
	DDX_Control(pDX, IDC_INVOICE_FLD, m_CntlInvoiceNumberFld);
	DDX_Control(pDX, IDC_EXP_DATE_FLD, m_CntlExpDateFld);
	DDX_Control(pDX, IDC_DATE_FLD, m_CntlDateFld);
	DDX_Control(pDX, IDC_CARD_NUMBER_FLD, m_CntlCardNumberFld);
	DDX_Control(pDX, IDC_AUTH_ID_FLD, m_CntlAuthIDFld);
	DDX_Control(pDX, IDC_AMOUNT_FLD, m_CntlAmountFld);
	DDX_Text(pDX, IDC_AMOUNT_FLD, m_StrAmountFld);
	DDV_MaxChars(pDX, m_StrAmountFld, 13);
	DDX_Text(pDX, IDC_AUTH_ID_FLD, m_StrAuthIDFld);
	DDV_MaxChars(pDX, m_StrAuthIDFld, 6);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDV_MaxChars(pDX, m_StrCardNumberFld, 19);
	DDX_Text(pDX, IDC_DATE_FLD, m_StrDateFld);
	DDV_MaxChars(pDX, m_StrDateFld, 8);
	DDX_Text(pDX, IDC_EXP_DATE_FLD, m_StrExpDateFld);
	DDV_MaxChars(pDX, m_StrExpDateFld, 4);
	DDX_Text(pDX, IDC_INVOICE_FLD, m_StrInvoiceNumberFld);
	DDV_MaxChars(pDX, m_StrInvoiceNumberFld, 8);
	DDX_Text(pDX, IDC_TIME1_FLD, m_StrTimeFld);
	DDV_MaxChars(pDX, m_StrTimeFld, 4);
	DDX_CBString(pDX, IDC_TRANSACTION_TYPE, m_StrTransactionType);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_JOB_NUMBER_FLD, m_StrJobNumberFld);
	DDV_MaxChars(pDX, m_StrJobNumberFld, 6);
	DDX_Text(pDX, IDC_MAN_NUMBER_FLD, m_StrManNumberFld);
	DDV_MaxChars(pDX, m_StrManNumberFld, 7);
	DDX_Text(pDX, IDC_RRN_FLD, m_StrRRNFld);
	DDV_MaxChars(pDX, m_StrRRNFld, 12);
	DDX_Text(pDX, IDC_FARE_AMOUNT_FLD, m_StrFareAmountFld);
	DDV_MaxChars(pDX, m_StrFareAmountFld, 13);
	DDX_Text(pDX, IDC_TEXT, m_StrTextFld);
	DDV_MaxChars(pDX, m_StrTextFld, 30);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchDetails, CDialog)
	//{{AFX_MSG_MAP(CBatchDetails)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchDetails message handlers

BOOL CBatchDetails::OnInitDialog() 
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
	
	m_StrAmountFld = _T("");
	m_StrAuthIDFld = _T("");
	m_StrCardNumberFld = _T("");
	m_StrDateFld = _T("");
	m_StrExpDateFld = _T("");
	m_StrInvoiceNumberFld = _T("");
	m_StrTimeFld = _T("");
	m_CntlTransactionTypeFld.SetCurSel(0);
	m_StrMessageLbl = _T("");
	m_StrJobNumberFld = _T("");
	m_StrManNumberFld = _T("");
	m_StrRRNFld = _T("");
	m_StrFareAmountFld = _T("");
	m_StrTextFld = _T("");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchDetails::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CBatchDetails::OnOK() 
{
   OnApply();
	
   CDialog::OnOK();
}

BOOL CBatchDetails::PopulateStructure()
{
	CString strCategoryCode;
	CString strUniqueStan;
	CMyUtils myUtilsObject;
	CHAR unique_tran_str_id[20] = "";
	CHAR unique_stan[15];
	CHAR strTime[15];
	CHAR wstation[21]; DWORD wsize;
	CHAR strLen[5];
	DCF01 sDCFRecord;
	MCF01 sMCFRecord;
	CHAR strErrorMessage[260];
    CHAR  strTxKey;
	CHAR rrn[13];
    CHAR SystemDate[40] = "";
    CHAR SystemTime[40] = "";
    CString strFareAmount;

		
	ZeroMemory ( &sTLF01Record, sizeof ( TLF01 ) );
	ZeroMemory ( &sDCFRecord, sizeof ( DCF01 ) );
	ZeroMemory ( &sMCFRecord, sizeof ( MCF01 ) );
    ZeroMemory ( &sBCH10Record, sizeof ( BCH10 ) );

	memcpy ( sDCFRecord.primary_key.device_id, strDeviceID, sizeof (sDCFRecord.primary_key.device_id));
	memcpy ( sTLF01Record.terminal_id, strDeviceID, sizeof (sTLF01Record.terminal_id));

	int rcode = txdsapi_get_record( (pBYTE)&sDCFRecord, sizeof (DCF01), DCF01_DATA, strErrorMessage );
	if( PTEMSG_NOT_FOUND == rcode )
    {
      m_StrMessageLbl = "Invalid Driver ID";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return FALSE;
    }
	else if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }

	memcpy ( sMCFRecord.primary_key.merchant_id, sDCFRecord.merchant_id, sizeof (sMCFRecord.primary_key.merchant_id));
	memcpy ( sMCFRecord.primary_key.organization_id, sDCFRecord.organization_id, sizeof (sMCFRecord.primary_key.organization_id));

	rcode = txdsapi_get_record( (pBYTE)&sMCFRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if( PTEMSG_NOT_FOUND == rcode )
    {
      m_StrMessageLbl = "Invalid Merchant ID";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return FALSE;
    }
	else if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	ptetime_get_strtime ( strTime );
	memcpy ( sTLF01Record.tran_start_time, strTime,    sizeof (sTLF01Record.tran_start_time));

	wsize = sizeof( wstation );
	GetComputerName( wstation, &wsize );
//	memcpy ( sTLF01Record.workstation, wstation,    sizeof (sTLF01Record.workstation));
 
	int nLength = m_StrCardNumberFld.GetLength();
	itoa ( nLength, strLen, 10 );

	CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrAmountFld, 12, strErrorMessage );
	if ( strAmount == m_StrAmountFld )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		m_CntlAmountFld.SetFocus();
		return FALSE;
	}

   if ( m_StrFareAmountFld.GetLength() != 0 )
   {
      strFareAmount = myUtilsObject.GenerateTranAmount ( m_StrFareAmountFld, 12, strErrorMessage );
	   if ( strFareAmount == m_StrFareAmountFld )
	   {
		   m_StrMessageLbl = strErrorMessage;
		   UpdateData(FALSE);
		   m_CntlFareAmountFld.SetFocus();
		   return FALSE;
	   }
   }

	if ( m_StrTransactionType == "Sale"  )
	{
		strTxKey = AUTH_SALE;
		memcpy ( sTLF01Record.processing_code, "000000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0200",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.total_amount, strAmount,	     sizeof (sTLF01Record.total_amount));
		sBCH10Record.bin_type = 'C';
	}
	else if (  m_StrTransactionType == "Refund" )
	{
		strTxKey = AUTH_REFUND;
		memcpy ( sTLF01Record.processing_code, "200000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0200",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.total_amount, strAmount,	     sizeof (sTLF01Record.total_amount));
		sBCH10Record.bin_type = 'C';
	}

   else if (  m_StrTransactionType == "Debit Sale" )
	{
		strTxKey = AUTH_SALE;
		memcpy ( sTLF01Record.processing_code, "000000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0200",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.total_amount, strAmount,	     sizeof (sTLF01Record.total_amount));
		sBCH10Record.bin_type = 'D';
	}
	/*else if ( m_StrTransactionType == "Cash Advance" )
	{
		strTxKey = AUTH_CASH_ADVANCE;
		memcpy ( sTLF01Record.processing_code, "010000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0200",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.total_amount, strAmount,	     sizeof (sTLF01Record.total_amount));
	}
	else if ( m_StrTransactionType == "Authorization" )
	{
		strTxKey = AUTH_AUTHORIZATION;
		memcpy ( sTLF01Record.processing_code, "000000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0100",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.total_amount, strAmount,	     sizeof (sTLF01Record.total_amount));
	}
	else if ( m_StrTransactionType == "Offline Sale" )
	{
		strTxKey = AUTH_OFFLINE_SALE;
		memcpy ( sTLF01Record.processing_code, "000000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0220",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.reversal_amount, strAmount,	     sizeof (sTLF01Record.reversal_amount));
	}
	else if ( m_StrTransactionType == "Offline Refund" )
	{
		strTxKey = AUTH_OFFLINE_REFUND;
		memcpy ( sTLF01Record.processing_code, "200000",    sizeof (sTLF01Record.processing_code));
		memcpy ( sTLF01Record.message_type, "0220",    sizeof (sTLF01Record.message_type));
		memcpy ( sTLF01Record.reversal_amount, strAmount,	     sizeof (sTLF01Record.reversal_amount));
	}
*/
	// Populate TLF01 Table	
	memcpy ( sTLF01Record.voice_auth_text, m_StrTextFld,	     sizeof (sTLF01Record.voice_auth_text));
//	memcpy ( sAuthTxRecord.industry_code, sDCFRecord.industry_code,	     sizeof (sAuthTxRecord.industry_code));
	memcpy ( sTLF01Record.merchant_id, sDCFRecord.merchant_id,    sizeof (sTLF01Record.merchant_id));
	memcpy ( sTLF01Record.card_num,    m_StrCardNumberFld, sizeof (sTLF01Record.card_num));
	memcpy ( sTLF01Record.exp_date,    m_StrExpDateFld,    sizeof (sTLF01Record.exp_date));
	sTLF01Record.tx_key = strTxKey;
	memcpy ( sTLF01Record.response_code, "00",    sizeof (sTLF01Record.response_code));
	memcpy ( sTLF01Record.card_num_len, strLen,    sizeof (sTLF01Record.card_num_len));
	memcpy ( sTLF01Record.pos_entry_mode, "010",    sizeof (sTLF01Record.pos_entry_mode));

	ptetime_get_strtime(unique_tran_str_id); 
	memset(sTLF01Record.primary_key.transaction_id, 0, sizeof(sTLF01Record.primary_key.transaction_id));
	memcpy(sTLF01Record.primary_key.transaction_id, sTLF01Record.terminal_id, sizeof(sTLF01Record.primary_key.transaction_id) - 1);
	strncat((char*)sTLF01Record.primary_key.transaction_id, unique_tran_str_id+2, 12);

	ptetime_get_strtime(unique_stan);

	// Take Rigth 7 digits and remove "."
	strUniqueStan = unique_stan;
   if ( strUniqueStan.GetLength() != 0 )
   {
	strUniqueStan = strUniqueStan.Right(7);
	strUniqueStan = strUniqueStan.Left(3) + strUniqueStan.Right(3);
   }
	memcpy(sTLF01Record.sys_trace_audit_num,	strUniqueStan, sizeof(sTLF01Record.sys_trace_audit_num) - 1);
	if (myUtilsObject.IsEmpty (m_StrAuthIDFld, this))
 	   memcpy ( sTLF01Record.auth_number,        strUniqueStan,					sizeof (sTLF01Record.auth_number));
   else
 	   memcpy ( sTLF01Record.auth_number,        m_StrAuthIDFld,					sizeof (sTLF01Record.auth_number));

	if (myUtilsObject.IsEmpty (m_StrInvoiceNumberFld, this))
	   memcpy ( sTLF01Record.invoice_number,     strUniqueStan, sizeof (sTLF01Record.invoice_number));		
   else
 	   memcpy ( sTLF01Record.auth_number,        m_StrInvoiceNumberFld,					sizeof (sTLF01Record.auth_number));

	create_rrn( rrn );
	if (!myUtilsObject.IsEmpty (m_StrRRNFld, this))
	   memcpy ( sTLF01Record.retrieval_ref_num,  m_StrRRNFld, sizeof (sTLF01Record.retrieval_ref_num));		
   else
	   memcpy ( sTLF01Record.retrieval_ref_num,  rrn, sizeof (sTLF01Record.retrieval_ref_num));		
/*	memcpy ( sTLF01Record.job_number,  m_StrJobNumberFld, sizeof (sTLF01Record.job_number));		
	memcpy ( sTLF01Record.man_number,  m_StrManNumberFld, sizeof (sTLF01Record.man_number));	Comfort	*/
//	??memcpy ( sTLF01Record.category_code, sDCFRecord.category_code, sizeof (sTLF01Record.category_code));
//	memcpy ( sTLF01Record.organization_id,    sDCFRecord.organization_id,   sizeof (sTLF01Record.organization_id));
	memcpy ( sTLF01Record.handler_queue,      "MANUAL",    sizeof (sTLF01Record.handler_queue));
   memcpy ( sTLF01Record.dispensation.origin, "LO", sizeof (sTLF01Record.dispensation.origin));
	memcpy ( sTLF01Record.currency_code,      sMCFRecord.currency_code,	sizeof (sTLF01Record.currency_code));
	memcpy ( sTLF01Record.voice_auth,         "N",							sizeof (sTLF01Record.voice_auth));
	memcpy ( sTLF01Record.voice_auth_text, m_StrTextFld,				sizeof (sTLF01Record.voice_auth_text));
	memcpy ( sTLF01Record.operator_id,        ucf01.primary_key.name,		sizeof (sTLF01Record.operator_id));
	memcpy ( sTLF01Record.batch_number,       "ACTIVE",					sizeof (sTLF01Record.batch_number));
      
   CTime t = CTime::GetCurrentTime();
   int day,year,month,hour,min,sec;
   day = t.GetDay();
   year = t.GetYear();
   month = t.GetMonth();
   hour = t.GetHour();
   min = t.GetMinute();
   sec = t.GetSecond();

   itoa ( year, SystemDate, 10 );

   SystemDate[4] = '0';
   if ( month < 10 )
      itoa ( month, SystemDate+5, 10 );
   else
      itoa ( month, SystemDate+4, 10 );

   SystemDate[6] = '0';
   if ( day < 10 )
      itoa ( day, SystemDate+7, 10 );
   else
      itoa ( day, SystemDate+6, 10 );

   SystemTime[0] = '0';
   if ( hour < 10 )
      itoa ( hour, SystemTime+1, 10 );
   else
      itoa ( hour, SystemTime, 10 );

   SystemTime [2] = '0';
   if ( min < 10 )
      itoa ( min, SystemTime+3, 10 );
   else
      itoa ( min, SystemTime+2, 10 );

   SystemTime[4] = '0';
   if ( sec < 10 )
      itoa ( sec, SystemTime+5, 10 );
   else
      itoa ( sec, SystemTime+4, 10 );


   // if date and time fields are empty get system date and time
   if ( m_StrDateFld.GetLength() != 0 )
   {
	   m_StrDateFld = myUtilsObject.GenerateDate(m_StrDateFld);
 	   memcpy ( sTLF01Record.date_yyyymmdd,      m_StrDateFld,					sizeof (sTLF01Record.date_yyyymmdd));
   }
   else
      memcpy ( sTLF01Record.date_yyyymmdd,      SystemDate,	  8);
   
   if ( m_StrTimeFld.GetLength() != 0 )
 	   memcpy ( sTLF01Record.time_hhmmss,        m_StrTimeFld,					sizeof (sTLF01Record.time_hhmmss));
    else
      memcpy ( sTLF01Record.time_hhmmss,      SystemTime,	  6);
 
	sTLF01Record.entry_type = 1;






    memcpy(sBCH10Record.primary_key.device_id,        sTLF01Record.terminal_id, sizeof (sBCH10Record.primary_key.device_id) );
    memcpy(sBCH10Record.primary_key.batch_nbr ,       "ACTIVE", sizeof (sBCH10Record.primary_key.batch_nbr));
    memcpy(sBCH10Record.primary_key.retrieval_ref_num,sTLF01Record.retrieval_ref_num, sizeof (sBCH10Record.primary_key.retrieval_ref_num));
//    memcpy(sBCH10Record.organization_id,  sTLF01Record.organization_id, sizeof (sBCH10Record.organization_id));
//    memcpy(sBCH10Record.fare_amount,      strFareAmount, sizeof (sBCH10Record.fare_amount));

    memcpy(sBCH10Record.merchant_id,      sTLF01Record.merchant_id, sizeof (sBCH10Record.merchant_id));
    memcpy(sBCH10Record.processing_code,  sTLF01Record.processing_code, sizeof (sBCH10Record.processing_code));
    memcpy(sBCH10Record.tran_amount,      sTLF01Record.total_amount, sizeof (sBCH10Record.tran_amount));
    memcpy(sBCH10Record.auth_number,      sTLF01Record.auth_number, sizeof (sBCH10Record.auth_number));
    memcpy(sBCH10Record.tran_date,        sTLF01Record.date_yyyymmdd, sizeof (sBCH10Record.tran_date));
    memcpy(sBCH10Record.tran_time,        sTLF01Record.time_hhmmss, sizeof (sBCH10Record.tran_time));
    memcpy(sBCH10Record.transaction_id,   sTLF01Record.primary_key.transaction_id, sizeof (sBCH10Record.transaction_id));
    memcpy(sBCH10Record.message_type,     sTLF01Record.message_type, sizeof (sBCH10Record.message_type));
    memcpy(sBCH10Record.card_nbr,         sTLF01Record.card_num, sizeof (sBCH10Record.card_nbr));
    memcpy(sBCH10Record.invoice_nbr,      sTLF01Record.invoice_number, sizeof (sBCH10Record.invoice_nbr));
    memcpy(sBCH10Record.settle_file_prefix,sTLF01Record.settle_file_prefix, sizeof (sBCH10Record.settle_file_prefix));
    memcpy(sBCH10Record.product_code,     sTLF01Record.product_code, sizeof (sBCH10Record.product_code));
    memcpy(sBCH10Record.sys_trace_audit_num,sTLF01Record.sys_trace_audit_num, sizeof (sBCH10Record.sys_trace_audit_num));
    sBCH10Record.tx_key =                 sTLF01Record.tx_key;
//	 memcpy ( sBCH10Record.job_number, sTLF01Record.job_number,   sizeof (sBCH10Record.job_number));		
//	 memcpy ( sBCH10Record.man_number, sTLF01Record.man_number,   sizeof (sBCH10Record.man_number));		


    memcpy(sBCH10Record.pos_entry_mode,   sTLF01Record.pos_entry_mode, sizeof (sBCH10Record.pos_entry_mode));
    memcpy(sBCH10Record.profile,          sTLF01Record.profile, sizeof (sBCH10Record.profile));
    memcpy(sBCH10Record.exp_date,         sTLF01Record.exp_date, sizeof (sBCH10Record.exp_date));


return TRUE;

}

BOOL CBatchDetails::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	if (myUtilsObject.IsEmpty (m_StrCardNumberFld, this))
    {
		m_StrMessageLbl = "Please enter Card Number";	
		m_CntlCardNumberFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrAmountFld, this))
    {
		m_StrMessageLbl = "Please enter Amount";	
		m_CntlAmountFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrFareAmountFld, this))
    {
		m_StrMessageLbl = "Please enter Fare Amount";	
		m_CntlFareAmountFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
   
   if (myUtilsObject.IsEmpty (m_StrExpDateFld, this))
    {
		m_StrMessageLbl = "Please enter Expiry Date";	
		m_CntlExpDateFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
   if (myUtilsObject.IsEmpty (m_StrJobNumberFld, this))
    {
		m_StrMessageLbl = "Please enter Job Number";	
		m_CntlJobNumberFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}   if (myUtilsObject.IsEmpty (m_StrManNumberFld, this))
    {
		m_StrMessageLbl = "Please enter Man Number";	
		m_CntlManNumberFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}	
    return TRUE;
}

BOOL CBatchDetails::InsertRecord()
{
   char strErrorMessage[300];

   UpdateData(TRUE);

   int rcode = txdsapi_insert_record ( (pBYTE)&sTLF01Record, sizeof (TLF01), TLF01_DATA, strErrorMessage );
   if( PTEMSG_OK != rcode )
   {
	  m_StrMessageLbl = strErrorMessage;

	  if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	  }
   return FALSE ;
   } // if

   rcode = txdsapi_insert_record ( (pBYTE)&sBCH10Record, sizeof (BCH10), BCH01_BCH10_TLF01_DATA, strErrorMessage );
   if( PTEMSG_OK != rcode )
   {
	  m_StrMessageLbl = strErrorMessage;

	  if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	  }
   return FALSE ;
   } // if
   return TRUE;

}

void CBatchDetails::create_rrn( pCHAR rrn )
{
    CHAR    time_date[25];
    CHAR    date_str[9];
    CHAR    time_str[7];
    CHAR    julian_day[4];

    /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
    ptetime_get_timestamp( time_date );

    /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
    get_date( time_date, date_str );
    get_time( time_date, time_str );

    /* Change date to julian format - DDD */
    calc_julian_day( date_str, julian_day );

    /* Format RRN: "YDDDhmmssjjj" */
    sprintf( rrn, "%01.1s%03.3s%05.5s%03.3s",time_date+3, julian_day,
                 time_str+1, time_date+20 );
}

void CBatchDetails::get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}

 void CBatchDetails::get_time( pCHAR time_date, pCHAR time_str )
 {
    memset ( time_str,  0x00,          7 );
    strncpy( time_str, &time_date[11], 2 );
    strncat( time_str, &time_date[14], 2 );
    strncat( time_str, &time_date[17], 2 );
 }

 void CBatchDetails::calc_julian_day( pCHAR date_str, pCHAR julian_str )
{
    CHAR  year[5];
    CHAR  month[3];
    CHAR  day[3];
    INT   iYear;
    INT   iMonth;
    INT   iDay;
    INT   julian_day;
    INT   i;
    INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


    strncpy( year, date_str, 4 );
    iYear  = atoi( year );

    strncpy( month, &date_str[4], 2 );
    iMonth = atoi( month );

    strncpy( day, &date_str[6], 2 );
    iDay   = atoi( day );

    for( i=0,julian_day=0; i<iMonth-1 && i<12; i++ )
    {
       julian_day += DaysOfMonth[i];
    }
    julian_day += iDay;

    if ( 2 < iMonth )
       if ( 0 == ( iYear % 4 ) )
          julian_day++;

    sprintf( julian_str, "%03d", julian_day );
}


BOOL CBatchDetails::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CBatchDetails::ValidateTime ( CString strTime )
{
	CString strHH;
	CString strMM;
	int nHH, nMM;
	UpdateData(TRUE);
	CMyUtils myUtilsObject;

	if ( myUtilsObject.IsEmpty ( strTime, this ))
		return TRUE;

	int length = strTime.GetLength();	

	if ( length < 4 )
	{
		m_StrMessageLbl = "ERROR: Invalid time format";
		UpdateData(FALSE);
		return FALSE;
	}

   if ( strTime.GetLength() != 0 )   
	   strHH = strTime.Left(2);
   if ( strTime.GetLength() != 0 )
	   strMM = strTime.Right(2);

	nHH = atoi ( strHH );
	nMM = atoi ( strMM );

	if ( nHH == 0 && nMM == 0 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nHH > 23 || nMM >59 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}

	return TRUE;

}

BOOL CBatchDetails::ValidateExpDate ( CString strDate )
{
	CString strMonth;
	CMyUtils myUtilsObject;

   int nMonth;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	

	if ( length < 4 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strMonth = strDate.Right(2);

	nMonth = atoi ( strMonth );

	if (  nMonth == 0 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}


	
	return TRUE;
}

BOOL CBatchDetails::ValidateCardNumber ( CString strCardNumber )
{
		char pCardNbr[20];
		int nLastDigit;
      CMyUtils myUtilsObject;

		int len = strCardNumber.GetLength();
		/*if ( 16 != len )
		{
			m_StrMessageLbl = "ERROR: Card Number Length must be 16";
         if (!UpdateData(FALSE)){
			   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		   }
			return FALSE;

		}*/
		memcpy ((LPTSTR) pCardNbr, strCardNumber, len - 1 );
		nLastDigit = myUtilsObject.GetMod10CheckDigit ( pCardNbr, len-1 );
      memset( pCardNbr, 0, sizeof ( pCardNbr ) );
		memcpy ((LPTSTR) pCardNbr, strCardNumber.Right(1), 1 );

		if ( nLastDigit != atoi ( pCardNbr ) )
      {
			m_StrMessageLbl = "ERROR: Invalid Card Number";
		   if (!UpdateData(FALSE)){
			   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		   }
			return FALSE;

      }
      return TRUE;
}

void CBatchDetails::OnApply() 
{
   BCH10 sBCH10Record;
   BCH01 sBCH01Record;
   char strErrorMessage[300] = "";

   ZeroMemory ( &sBCH10Record, sizeof ( BCH10 ) );

    if ( !CheckDialogForCompleteness() )
		return;

    if ( !ValidateCardNumber ( m_StrCardNumberFld ) )
    {
      m_CntlCardNumberFld.SetFocus();
      return;
    }

    if ( !ValidateDate( m_StrDateFld ) )
    {
       m_CntlDateFld.SetFocus();
       return;
    }

    if ( !ValidateTime ( m_StrTimeFld ) )
    {
       m_CntlTimeFld.SetFocus();
       return;
    }

    if ( !ValidateExpDate ( m_StrExpDateFld ) )
    {
       m_CntlExpDateFld.SetFocus();
       return;
    }

	 if ( !PopulateStructure() )
		 return;

    if ( strOpenDate.GetLength() == 0 )
    {
      memcpy ( sBCH01Record.primary_key.device_id,  strDeviceID, sizeof (sBCH01Record.primary_key.device_id) );
      memcpy ( sBCH01Record.primary_key.batch_nbr, "ACTIVE", sizeof (sBCH01Record.primary_key.batch_nbr) );
      memcpy ( sBCH01Record.open_date, " ", sizeof (sBCH01Record.open_date) );
      memcpy ( sBCH01Record.open_time, " ", sizeof (sBCH01Record.open_time) );

	   int rcode = txdsapi_get_record ( (pBYTE)&sBCH01Record, sizeof (BCH01), BCH01_DATA, strErrorMessage);        
	   if ( rcode != PTEMSG_OK && rcode != PTEMSG_NOT_FOUND )
      {
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
		   return;
		}
      else if ( rcode == PTEMSG_OK )
      {
         strOpenDate = sBCH01Record.open_date;
         strOpenTime = sBCH01Record.open_time;
      }
      
    }
       
    memcpy ( sBCH10Record.open_date, strOpenDate, sizeof (sBCH10Record.open_date) );
    memcpy ( sBCH10Record.open_time, strOpenTime, sizeof (sBCH10Record.open_time) );
    memcpy ( sBCH10Record.primary_key.device_id, strDeviceID, sizeof (sBCH10Record.primary_key.device_id) );
    memcpy ( sBCH10Record.primary_key.batch_nbr, "ACTIVE", sizeof (sBCH10Record.primary_key.batch_nbr) );
    memcpy ( sBCH10Record.primary_key.retrieval_ref_num, m_StrRRNFld, sizeof (sBCH10Record.primary_key.retrieval_ref_num) );

	 int rcode = txdsapi_get_record( (pBYTE)&sBCH10Record, sizeof (BCH10), BCH10_DATA, strErrorMessage );
	   // failed to get the data
	 if( PTEMSG_OK == rcode )
    {
      m_StrMessageLbl = "ERROR: Batch Record already exists";
	   m_CntlRRNFld.SetFocus();

	  if (!UpdateData(FALSE)){
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	  }
	  return ;
    }
   

	 if ( !InsertRecord() )
		 return;	

	m_StrAmountFld = _T("");
	m_StrAuthIDFld = _T("");
	m_StrCardNumberFld = _T("");
	m_StrDateFld = _T("");
	m_StrExpDateFld = _T("");
	m_StrInvoiceNumberFld = _T("");
	m_StrTimeFld = _T("");
	m_StrTransactionType = _T("");
	m_StrJobNumberFld = _T("");
	m_StrManNumberFld = _T("");
	m_StrRRNFld = _T("");
	m_StrFareAmountFld = _T("");
	m_StrTextFld = _T("");
   m_StrMessageLbl = "Record Added";
   m_CntlCardNumberFld.SetFocus();

   UpdateData(FALSE);
}

BOOL CBatchDetails::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
