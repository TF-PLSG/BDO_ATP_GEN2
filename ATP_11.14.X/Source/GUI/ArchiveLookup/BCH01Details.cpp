// BCH01Details.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "BCH01Details.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCH01Details dialog


CBCH01Details::CBCH01Details(CWnd* pParent /*=NULL*/)
	: CDialog(CBCH01Details::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBCH01Details)
	m_sz_batch_nbr = _T("");
	m_sz_batch_status = _T("");
	m_sz_device_id = _T("");
	m_sz_merchant_id = _T("");
	m_sz_open_date = _T("");
	m_sz_open_time = _T("");
	m_sz_release_date = _T("");
	m_sz_release_time = _T("");
	m_szStatus = _T("");
	m_sz_total_credit_amt = _T("");
	m_sz_total_credit_cnt = _T("");
	m_sz_total_credit_refund_amt = _T("");
	m_sz_total_credit_refund_cnt = _T("");
	m_sz_total_debit_amt = _T("");
	m_sz_total_debit_refund_amt = _T("");
	m_sz_total_debit_refund_cnt = _T("");
	m_sz_total_debit_cnt = _T("");
	m_sz_total_void_credit_amt = _T("");
	m_sz_total_void_credit_cnt = _T("");
	m_sz_total_void_credit_refund_amt = _T("");
	m_sz_total_void_credit_refund_cnt = _T("");
	m_sz_visa_merchant_id = _T("");
	m_sz_purge_date = _T("");
	//}}AFX_DATA_INIT
}


void CBCH01Details::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCH01Details)
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Text(pDX, IDC_BATCH_NBR, m_sz_batch_nbr);
	DDX_Text(pDX, IDC_BATCH_STATUS, m_sz_batch_status);
	DDX_Text(pDX, IDC_DEVICE_ID, m_sz_device_id);
	DDX_Text(pDX, IDC_MERCHANT_ID, m_sz_merchant_id);
	DDX_Text(pDX, IDC_OPEN_DATE, m_sz_open_date);
	DDX_Text(pDX, IDC_OPEN_TIME, m_sz_open_time);
	DDX_Text(pDX, IDC_REL_DATE, m_sz_release_date);
	DDX_Text(pDX, IDC_REL_TIME, m_sz_release_time);
	DDX_Text(pDX, IDC_STATUS, m_szStatus);
	DDX_Text(pDX, IDC_TOTAL_CRD_AMT, m_sz_total_credit_amt);
	DDX_Text(pDX, IDC_TOTAL_CRD_CNT, m_sz_total_credit_cnt);
	DDX_Text(pDX, IDC_TOTAL_CRD_RE_AMT, m_sz_total_credit_refund_amt);
	DDX_Text(pDX, IDC_TOTAL_CRD_RE_CNT, m_sz_total_credit_refund_cnt);
	DDX_Text(pDX, IDC_TOTAL_DBT_AMT, m_sz_total_debit_amt);
	DDX_Text(pDX, IDC_TOTAL_DBT_RE_AMT, m_sz_total_debit_refund_amt);
	DDX_Text(pDX, IDC_TOTAL_DBT_RE_CNT, m_sz_total_debit_refund_cnt);
	DDX_Text(pDX, IDC_TOTAL_DBT_CNT, m_sz_total_debit_cnt);
	DDX_Text(pDX, IDC_TOTAL_VOID_CRD_AMT, m_sz_total_void_credit_amt);
	DDX_Text(pDX, IDC_TOTAL_VOID_CRD_CNT, m_sz_total_void_credit_cnt);
	DDX_Text(pDX, IDC_TOTAL_VOID_CRD_RE_AMT, m_sz_total_void_credit_refund_amt);
	DDX_Text(pDX, IDC_TOTAL_VOID_CRD_RE_CNT, m_sz_total_void_credit_refund_cnt);
	DDX_Text(pDX, IDC_VISA_ID, m_sz_visa_merchant_id);
	DDX_Text(pDX, IDC_PURGE_DATE, m_sz_purge_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCH01Details, CDialog)
	//{{AFX_MSG_MAP(CBCH01Details)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCH01Details message handlers

BOOL CBCH01Details::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateTitleWithVersion(this);
	// TODO: Add extra initialization here
	m_sz_batch_nbr				= sBatchRecord.primary_key.batch_nbr;
	m_sz_batch_status			= sBatchRecord.batch_status;
	m_sz_device_id				= sBatchRecord.primary_key.device_id;
	m_sz_merchant_id			= sBatchRecord.merchant_id;
	m_sz_open_date				= sBatchRecord.open_date;
	m_sz_open_time				= sBatchRecord.open_time;
	m_sz_release_date			= sBatchRecord.release_date;
	m_sz_release_time			= sBatchRecord.release_time;
	m_sz_total_credit_amt			= sBatchRecord.total_credit_amt;
	m_sz_total_credit_cnt			= sBatchRecord.total_credit_cnt;
	m_sz_total_credit_refund_amt		= sBatchRecord.total_credit_refund_amt;
	m_sz_total_credit_refund_cnt		= sBatchRecord.total_credit_refund_cnt;
	m_sz_total_debit_amt			= sBatchRecord.total_debit_amt;
	m_sz_total_debit_refund_amt		= sBatchRecord.total_debit_refund_amt;
	m_sz_total_debit_refund_cnt		= sBatchRecord.total_debit_refund_cnt;
	m_sz_total_debit_cnt			= sBatchRecord.total_debit_cnt;
	m_sz_total_void_credit_amt		= sBatchRecord.total_void_credit_amt;
	m_sz_total_void_credit_cnt		= sBatchRecord.total_void_credit_cnt;
	m_sz_total_void_credit_refund_amt	= sBatchRecord.total_void_credit_refund_amt;
	m_sz_total_void_credit_refund_cnt	= sBatchRecord.total_void_credit_refund_cnt;
	m_sz_visa_merchant_id			= sBatchRecord.visa_merchant_id; 
	m_sz_purge_date				= sBatchRecord.purge_date; 


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
