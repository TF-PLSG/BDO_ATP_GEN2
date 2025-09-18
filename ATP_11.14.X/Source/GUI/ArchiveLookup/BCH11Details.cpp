// BCH11Details.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "BCH11Details.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCH11Details dialog


CBCH11Details::CBCH11Details(CWnd* pParent /*=NULL*/)
	: CDialog(CBCH11Details::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBCH11Details)
	m_sz_auth_number = _T("");
	m_sz_batch_nbr = _T("");
	m_sz_card_nbr = _T("");
	m_sz_device_id = _T("");
	m_sz_invoice_nbr = _T("");
	m_sz_merchant_id = _T("");
	m_sz_processing_code = _T("");
	m_sz_response_code = _T("");
	m_sz_retrieval_ref_num = _T("");
	m_sz_tran_amount = _T("");
	m_sz_tran_date = _T("");
	m_sz_tran_time = _T("");
	m_sz_transaction_id = _T("");
	m_sz_def_gross_amt = _T("");
	m_sz_exp_date = _T("");
	m_sz_open_date = _T("");
	m_sz_open_time = _T("");
	m_sz_sys_trace_audit_num = _T("");
	m_sz_transmission_timestamp = _T("");
	m_sz_purge_date = _T("");
	//}}AFX_DATA_INIT
}


void CBCH11Details::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCH11Details)
	DDX_Text(pDX, IDC_AUTH_NUMBER, m_sz_auth_number);
	DDX_Text(pDX, IDC_BATCH_NBR1, m_sz_batch_nbr);
	DDX_Text(pDX, IDC_CARD_NBR1, m_sz_card_nbr);
	DDX_Text(pDX, IDC_DEVICE_ID1, m_sz_device_id);
	DDX_Text(pDX, IDC_INVOICE_NBR, m_sz_invoice_nbr);
	DDX_Text(pDX, IDC_MERCHANT_ID, m_sz_merchant_id);
	DDX_Text(pDX, IDC_PROCESSING_CODE, m_sz_processing_code);
	DDX_Text(pDX, IDC_RESPONSE_CODE, m_sz_response_code);
	DDX_Text(pDX, IDC_RETRIEVAL_REF_NUM, m_sz_retrieval_ref_num);
	DDX_Text(pDX, IDC_TRAN_AMOUNT, m_sz_tran_amount);
	DDX_Text(pDX, IDC_TRAN_DATE, m_sz_tran_date);
	DDX_Text(pDX, IDC_TRAN_TIME, m_sz_tran_time);
	DDX_Text(pDX, IDC_TRANSACTION_ID, m_sz_transaction_id);
	DDX_Text(pDX, IDC_DEF_GROSS_AMT, m_sz_def_gross_amt);
	DDX_Text(pDX, IDC_EXPDATE, m_sz_exp_date);
	DDX_Text(pDX, IDC_OPENDATE, m_sz_open_date);
	DDX_Text(pDX, IDC_OPENTIME, m_sz_open_time);
	DDX_Text(pDX, IDC_STAN, m_sz_sys_trace_audit_num);
	DDX_Text(pDX, IDC_TRANSMISSION_TIME, m_sz_transmission_timestamp);
	DDX_Text(pDX, IDC_PURGE_DATE2, m_sz_purge_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCH11Details, CDialog)
	//{{AFX_MSG_MAP(CBCH11Details)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCH11Details message handlers

BOOL CBCH11Details::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateTitleWithVersion(this);
	
	m_sz_auth_number		 = sBatchRecord.auth_number;
	m_sz_batch_nbr			 = sBatchRecord.primary_key.batch_nbr;
	m_sz_card_nbr			 = sBatchRecord.card_nbr;
	m_sz_device_id			 = sBatchRecord.primary_key.device_id;
	m_sz_invoice_nbr		 = sBatchRecord.invoice_nbr;
	m_sz_merchant_id		 = sBatchRecord.merchant_id;
	m_sz_processing_code	 = sBatchRecord.processing_code;
	m_sz_response_code		 = sBatchRecord.response_code;
	m_sz_retrieval_ref_num	 = sBatchRecord.primary_key.retrieval_ref_num;
	m_sz_tran_amount		 = sBatchRecord.tran_amount;
	m_sz_tran_date			 = sBatchRecord.tran_date;
	m_sz_tran_time			 = sBatchRecord.tran_time;   
	m_sz_transaction_id		 = sBatchRecord.transaction_id;
	m_sz_def_gross_amt		 = sBatchRecord.def_gross_amt;
	m_sz_exp_date			 = sBatchRecord.exp_date;
	m_sz_open_date			 = sBatchRecord.open_date;
	m_sz_open_time			 = sBatchRecord.open_time;
	m_sz_sys_trace_audit_num = sBatchRecord.sys_trace_audit_num;
	m_sz_transmission_timestamp	 = sBatchRecord.transmission_timestamp;
	m_sz_purge_date			 = sBatchRecord.purge_date;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
