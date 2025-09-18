// BCH20Lookup.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "BCH20Lookup.h"
#include "BCH20Details.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCH20Lookup dialog


CBCH20Lookup::CBCH20Lookup(CWnd* pParent /*=NULL*/)
	: CDialog(CBCH20Lookup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBCH20Lookup)
	m_szBatchNbr = _T("");
	m_szDeviceID = _T("");
	m_szMerchantID = _T("");
	m_szOpenDate = _T("");
	m_szStatus = _T("");
	m_szRetrievalRefNum = _T("");
	//}}AFX_DATA_INIT
	bSearching = false;

	m_pGetTLF = NULL;
	CWinThread *thr = AfxBeginThread(RUNTIME_CLASS(CGetTLFData),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_pGetTLF = dynamic_cast<CGetTLFData *>(thr);
	
	arrColumnNames.Add("DEVICE_ID");
	arrColumnNames.Add("BATCH_NBR");
	arrColumnNames.Add("RETRIEVAL_REF_NUM");
	arrColumnNames.Add("MERCHANT_ID");
	arrColumnNames.Add("PROCESSING_CODE");
	arrColumnNames.Add("TRAN_AMOUNT");
	arrColumnNames.Add("RESPONSE_CODE");
	arrColumnNames.Add("AUTH_NUMBER");
	arrColumnNames.Add("TRAN_DATE");
	arrColumnNames.Add("TRAN_TIME");
	arrColumnNames.Add("TRANSACTION_ID");
	arrColumnNames.Add("MESSAGE_TYPE");
	arrColumnNames.Add("CARD_NBR");
	arrColumnNames.Add("INVOICE_NBR");
	arrColumnNames.Add("SETTLE_FILE_PREFIX");
	arrColumnNames.Add("PRODUCT_CODE");
	arrColumnNames.Add("ODOMETER");
	arrColumnNames.Add("SYS_TRACE_AUDIT_NUM");
	arrColumnNames.Add("TX_KEY");
	arrColumnNames.Add("BIN_TYPE");
	arrColumnNames.Add("TYPE_OF_DATA");
	arrColumnNames.Add("DOWN_PAYMENT");
	arrColumnNames.Add("PERIOD");
	arrColumnNames.Add("INTEREST_RATE");
	arrColumnNames.Add("TOTAL_INTEREST");
	arrColumnNames.Add("REDEMPTION_AMOUNT");
	arrColumnNames.Add("VEHICLE_NUMBER");
	arrColumnNames.Add("POS_ENTRY_MODE");
	arrColumnNames.Add("PROFILE");
	arrColumnNames.Add("NBR_OF_PROD_CODES");
	arrColumnNames.Add("PRODUCT1_CODE");
	arrColumnNames.Add("PRODUCT1_QUANTITY");
	arrColumnNames.Add("PRODUCT1_AMOUNT");
	arrColumnNames.Add("PRODUCT2_CODE");
	arrColumnNames.Add("PRODUCT2_QUANTITY");
	arrColumnNames.Add("PRODUCT2_AMOUNT");
	arrColumnNames.Add("PRODUCT3_CODE");
	arrColumnNames.Add("PRODUCT3_QUANTITY");
	arrColumnNames.Add("PRODUCT3_AMOUNT");
	arrColumnNames.Add("PRODUCT4_CODE");
	arrColumnNames.Add("PRODUCT4_QUANTITY");
	arrColumnNames.Add("PRODUCT4_AMOUNT");
	arrColumnNames.Add("PRODUCT5_CODE");
	arrColumnNames.Add("PRODUCT5_QUANTITY");
	arrColumnNames.Add("PRODUCT5_AMOUNT");
	arrColumnNames.Add("PRODUCT6_CODE");
	arrColumnNames.Add("PRODUCT6_QUANTITY");
	arrColumnNames.Add("PRODUCT6_AMOUNT");
	arrColumnNames.Add("PRODUCT7_CODE");
	arrColumnNames.Add("PRODUCT7_QUANTITY");
	arrColumnNames.Add("PRODUCT7_AMOUNT");
	arrColumnNames.Add("PRODUCT8_CODE");
	arrColumnNames.Add("PRODUCT8_QUANTITY");
	arrColumnNames.Add("PRODUCT8_AMOUNT");
	arrColumnNames.Add("PRODUCT9_CODE");
	arrColumnNames.Add("PRODUCT9_QUANTITY");
	arrColumnNames.Add("PRODUCT9_AMOUNT");
	arrColumnNames.Add("PRODUCT10_CODE");
	arrColumnNames.Add("PRODUCT10_QUANTITY");
	arrColumnNames.Add("PRODUCT10_AMOUNT");
	arrColumnNames.Add("PRODUCT11_CODE");
	arrColumnNames.Add("PRODUCT11_QUANTITY");
	arrColumnNames.Add("PRODUCT11_AMOUNT");
	arrColumnNames.Add("PRODUCT12_CODE");
	arrColumnNames.Add("PRODUCT12_QUANTITY");
	arrColumnNames.Add("PRODUCT12_AMOUNT");
	arrColumnNames.Add("PRODUCT13_CODE");
	arrColumnNames.Add("PRODUCT13_QUANTITY");
	arrColumnNames.Add("PRODUCT13_AMOUNT");
	arrColumnNames.Add("PRODUCT14_CODE");
	arrColumnNames.Add("PRODUCT14_QUANTITY");
	arrColumnNames.Add("PRODUCT14_AMOUNT");
	arrColumnNames.Add("PRODUCT15_CODE");
	arrColumnNames.Add("PRODUCT15_QUANTITY");
	arrColumnNames.Add("PRODUCT15_AMOUNT");
	arrColumnNames.Add("PRODUCT16_CODE");
	arrColumnNames.Add("PRODUCT16_QUANTITY");
	arrColumnNames.Add("PRODUCT16_AMOUNT");
	arrColumnNames.Add("PRODUCT17_CODE");
	arrColumnNames.Add("PRODUCT17_QUANTITY");
	arrColumnNames.Add("PRODUCT17_AMOUNT");
	arrColumnNames.Add("PRODUCT18_CODE");
	arrColumnNames.Add("PRODUCT18_QUANTITY");
	arrColumnNames.Add("PRODUCT18_AMOUNT");
	arrColumnNames.Add("PRODUCT19_CODE");
	arrColumnNames.Add("PRODUCT19_QUANTITY");
	arrColumnNames.Add("PRODUCT19_AMOUNT");
	arrColumnNames.Add("PRODUCT20_CODE");
	arrColumnNames.Add("PRODUCT20_QUANTITY");
	arrColumnNames.Add("PRODUCT20_AMOUNT");
	arrColumnNames.Add("APORTIONMENT_FLAG");
	arrColumnNames.Add("EXP_DATE");
	arrColumnNames.Add("OPEN_DATE");
	arrColumnNames.Add("OPEN_TIME");
	arrColumnNames.Add("CASH_BONUS");
	arrColumnNames.Add("TRANSMISSION_TIMESTAMP");
	arrColumnNames.Add("MCARD BANKNET");
	arrColumnNames.Add("PROCESSED");
	arrColumnNames.Add("TICKET_NBR");
	arrColumnNames.Add("DEF_GROSS_AMT");
	arrColumnNames.Add("TERM");
	arrColumnNames.Add("PURGE DATE");

}


void CBCH20Lookup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCH20Lookup)
	DDX_Control(pDX, IDC_NEXT, m_CntlNext);
	DDX_Control(pDX, IDC_BCH01_LIST, m_CntlBCHList);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Text(pDX, IDC_BATCH_NBR, m_szBatchNbr);
	DDX_Text(pDX, IDC_DEVICE_ID, m_szDeviceID);
	DDX_Text(pDX, IDC_MERCHANT_ID, m_szMerchantID);
	DDX_Text(pDX, IDC_OPEN_DATE, m_szOpenDate);
	DDX_Text(pDX, IDC_STATUS, m_szStatus);
	DDX_Text(pDX, IDC_RETRIEVAL_REF_NUM, m_szRetrievalRefNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCH20Lookup, CDialog)
	//{{AFX_MSG_MAP(CBCH20Lookup)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_NOTIFY(NM_DBLCLK, IDC_BCH01_LIST, OnDblclkBch01List)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCH20Lookup message handlers

void CBCH20Lookup::OnOK() 
{
			CString		temp;
	bool		all_fields_empty = true;
	CMyUtils	myUtilsObject;
	
	UpdateData(TRUE);
	ZeroMemory ( &sBCH20Record, sizeof ( ARCHIVE_BCH20 ) );
	
	m_szStatus= "";
	UpdateData(FALSE);
	
	if ( !myUtilsObject.IsEmpty(m_szBatchNbr, this) )
	{
		all_fields_empty = false;
		memcpy(sBCH20Record.primary_key.batch_nbr, 
			m_szBatchNbr.GetBuffer(sizeof(sBCH20Record.primary_key.batch_nbr)), 
			sizeof(sBCH20Record.primary_key.batch_nbr));
	}
	if ( !myUtilsObject.IsEmpty(m_szDeviceID, this) )
	{
		all_fields_empty = false;
		memcpy(sBCH20Record.primary_key.device_id, 
			m_szDeviceID.GetBuffer(sizeof(sBCH20Record.primary_key.device_id)), 
			sizeof(sBCH20Record.primary_key.device_id));
	}
	if ( !myUtilsObject.IsEmpty(m_szMerchantID, this) )
	{
		all_fields_empty = false;
		memcpy(sBCH20Record.merchant_id, 
			m_szMerchantID.GetBuffer(sizeof(sBCH20Record.merchant_id)), 
			sizeof(sBCH20Record.merchant_id));
	}
	if ( !myUtilsObject.IsEmpty(m_szOpenDate, this) )
	{
		all_fields_empty = false;
		/* Reformat start date from DDMMYYYY into YYYYMMDD. */
		temp = m_szOpenDate.Right(4) + m_szOpenDate.Mid(2,2) + m_szOpenDate.Left(2);
		memcpy(sBCH20Record.open_date, 
			temp, 
			sizeof(sBCH20Record.open_date));
	}
	if ( !myUtilsObject.IsEmpty(m_szRetrievalRefNum, this) )
	{
		all_fields_empty = false;
		memcpy(sBCH20Record.primary_key.retrieval_ref_num, 
			m_szRetrievalRefNum.GetBuffer(sizeof(sBCH20Record.primary_key.retrieval_ref_num)), 
			sizeof(sBCH20Record.primary_key.retrieval_ref_num));
	}

	if ( all_fields_empty == true )
	{
		m_szStatus= "Please enter search criteria...";
		UpdateData(FALSE);	  
	//	m_CntlCardNbr.SetFocus();
		return;
	}
		m_pGetTLF->PostThreadMessage(WM_GET_BCH20_LIST,0,(LPARAM)this);
	
}

BOOL CBCH20Lookup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pGetTLF->ResumeThread();
	m_CntlBCHList.InsertColumn(arrColumnNames, LVCFMT_LEFT, 90);
	UpdateTitleWithVersion(this);
	this->ShowWindow(SW_MAXIMIZE);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBCH20Lookup::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_CntlBCHList.Resize(cx,cy);
	
}

void CBCH20Lookup::OnNext() 
{
	m_pGetTLF->PostThreadMessage(WM_GET_BCH20_NEXT,0,0);
	
}

void CBCH20Lookup::OnDblclkBch01List(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CVector <CString> row;
	CBCH20Details detailsDlg;
	int nItem = m_CntlBCHList.GetFirstSelectedItem();
	m_CntlBCHList.GetSelectedRow(row);

	if(row.GetSize() == 102)
	{
		memcpy( (char *) detailsDlg.sBatchRecord.primary_key.device_id,		row[0]  ,sizeof( detailsDlg.sBatchRecord.primary_key.device_id ));
		memcpy( (char *) detailsDlg.sBatchRecord.primary_key.batch_nbr,		row[1] 	,sizeof( detailsDlg.sBatchRecord.primary_key.batch_nbr ));
		memcpy( (char *) detailsDlg.sBatchRecord.primary_key.retrieval_ref_num,	row[2] 	,sizeof( detailsDlg.sBatchRecord.primary_key.retrieval_ref_num ));
		memcpy( (char *) detailsDlg.sBatchRecord.merchant_id,				row[3] 	,sizeof( detailsDlg.sBatchRecord.merchant_id ));
		memcpy( (char *) detailsDlg.sBatchRecord.processing_code,			row[4] 	,sizeof( detailsDlg.sBatchRecord.processing_code ));
		memcpy( (char *) detailsDlg.sBatchRecord.tran_amount,				row[5] 	,sizeof( detailsDlg.sBatchRecord.tran_amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.response_code,			row[6] 	,sizeof( detailsDlg.sBatchRecord.response_code ));
		memcpy( (char *) detailsDlg.sBatchRecord.auth_number,				row[7] 	,sizeof( detailsDlg.sBatchRecord.auth_number ));
		memcpy( (char *) detailsDlg.sBatchRecord.tran_date,				row[8] 	,sizeof( detailsDlg.sBatchRecord.tran_date ));
		memcpy( (char *) detailsDlg.sBatchRecord.tran_time,				row[9] 	,sizeof( detailsDlg.sBatchRecord.tran_time ));
		memcpy( (char *) detailsDlg.sBatchRecord.transaction_id,			row[10]	,sizeof( detailsDlg.sBatchRecord.transaction_id ));
		memcpy( (char *) detailsDlg.sBatchRecord.message_type,				row[11]	,sizeof( detailsDlg.sBatchRecord.message_type ));
		memcpy( (char *) detailsDlg.sBatchRecord.card_nbr,				row[12]	,sizeof( detailsDlg.sBatchRecord.card_nbr ));
		memcpy( (char *) detailsDlg.sBatchRecord.invoice_nbr,				row[13]	,sizeof( detailsDlg.sBatchRecord.invoice_nbr ));
		memcpy( (char *) detailsDlg.sBatchRecord.settle_file_prefix,			row[14]	,sizeof( detailsDlg.sBatchRecord.settle_file_prefix ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_code,				row[15]	,sizeof( detailsDlg.sBatchRecord.product_code ));
		memcpy( (char *) detailsDlg.sBatchRecord.odometer,				row[16]	,sizeof( detailsDlg.sBatchRecord.odometer ));
		memcpy( (char *) detailsDlg.sBatchRecord.sys_trace_audit_num,			row[17]	,sizeof( detailsDlg.sBatchRecord.sys_trace_audit_num ));
				detailsDlg.sBatchRecord.tx_key = row[18][0];
		//memcpy( (char *) detailsDlg.sBatchRecord.tx_key,				row[18]	,sizeof( detailsDlg.sBatchRecord.tx_key ));
		detailsDlg.sBatchRecord.bin_type = row[19][0];
		//memcpy( (char *) detailsDlg.sBatchRecord.bin_type,				row[19]	,sizeof( detailsDlg.sBatchRecord.bin_type ));
		memcpy( (char *) detailsDlg.sBatchRecord.type_of_data,				row[20]	,sizeof( detailsDlg.sBatchRecord.type_of_data ));
		memcpy( (char *) detailsDlg.sBatchRecord.down_payment,				row[21]	,sizeof( detailsDlg.sBatchRecord.down_payment ));
		memcpy( (char *) detailsDlg.sBatchRecord.period,				row[22]	,sizeof( detailsDlg.sBatchRecord.period ));
		memcpy( (char *) detailsDlg.sBatchRecord.interest_rate,			row[23]	,sizeof( detailsDlg.sBatchRecord.interest_rate ));
		memcpy( (char *) detailsDlg.sBatchRecord.total_interest,			row[24]	,sizeof( detailsDlg.sBatchRecord.total_interest ));
		memcpy( (char *) detailsDlg.sBatchRecord.redemption_amount,			row[25]	,sizeof( detailsDlg.sBatchRecord.redemption_amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.vehicle_number,			row[26]	,sizeof( detailsDlg.sBatchRecord.vehicle_number ));
		memcpy( (char *) detailsDlg.sBatchRecord.pos_entry_mode,			row[27]	,sizeof( detailsDlg.sBatchRecord.pos_entry_mode ));
		memcpy( (char *) detailsDlg.sBatchRecord.profile,				row[28]	,sizeof( detailsDlg.sBatchRecord.profile ));
		memcpy( (char *) detailsDlg.sBatchRecord.nbr_of_prod_codes,			row[29]	,sizeof( detailsDlg.sBatchRecord.nbr_of_prod_codes ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[0].code,		row[30]	,sizeof( detailsDlg.sBatchRecord.product_codes[0].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[0].quantity,		row[31]	,sizeof( detailsDlg.sBatchRecord.product_codes[0].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[0].amount,		row[32]	,sizeof( detailsDlg.sBatchRecord.product_codes[0].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[1].code,		row[33]	,sizeof( detailsDlg.sBatchRecord.product_codes[1].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[1].quantity,		row[34]	,sizeof( detailsDlg.sBatchRecord.product_codes[1].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[1].amount,		row[35]	,sizeof( detailsDlg.sBatchRecord.product_codes[1].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[2].code,		row[36]	,sizeof( detailsDlg.sBatchRecord.product_codes[2].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[2].quantity,		row[37]	,sizeof( detailsDlg.sBatchRecord.product_codes[2].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[2].amount,		row[38]	,sizeof( detailsDlg.sBatchRecord.product_codes[2].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[3].code,		row[39]	,sizeof( detailsDlg.sBatchRecord.product_codes[3].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[3].quantity,		row[40]	,sizeof( detailsDlg.sBatchRecord.product_codes[3].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[3].amount,		row[41]	,sizeof( detailsDlg.sBatchRecord.product_codes[3].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[4].code,		row[42]	,sizeof( detailsDlg.sBatchRecord.product_codes[4].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[4].quantity,		row[43]	,sizeof( detailsDlg.sBatchRecord.product_codes[4].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[4].amount,		row[44]	,sizeof( detailsDlg.sBatchRecord.product_codes[4].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[5].code,		row[45]	,sizeof( detailsDlg.sBatchRecord.product_codes[5].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[5].quantity,		row[46]	,sizeof( detailsDlg.sBatchRecord.product_codes[5].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[5].amount,		row[47]	,sizeof( detailsDlg.sBatchRecord.product_codes[5].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[6].code,		row[48]	,sizeof( detailsDlg.sBatchRecord.product_codes[6].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[6].quantity,		row[49]	,sizeof( detailsDlg.sBatchRecord.product_codes[6].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[6].amount,		row[50]	,sizeof( detailsDlg.sBatchRecord.product_codes[6].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[7].code,		row[51]	,sizeof( detailsDlg.sBatchRecord.product_codes[7].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[7].quantity,		row[52]	,sizeof( detailsDlg.sBatchRecord.product_codes[7].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[7].amount,		row[53]	,sizeof( detailsDlg.sBatchRecord.product_codes[7].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[8].code,		row[54]	,sizeof( detailsDlg.sBatchRecord.product_codes[8].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[8].quantity,		row[55]	,sizeof( detailsDlg.sBatchRecord.product_codes[8].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[8].amount,		row[56]	,sizeof( detailsDlg.sBatchRecord.product_codes[8].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[9].code,		row[57]	,sizeof( detailsDlg.sBatchRecord.product_codes[9].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[9].quantity,		row[58]	,sizeof( detailsDlg.sBatchRecord.product_codes[9].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[9].amount,		row[59]	,sizeof( detailsDlg.sBatchRecord.product_codes[9].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[10].code,		row[60]	,sizeof( detailsDlg.sBatchRecord.product_codes[10].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[10].quantity,		row[61]	,sizeof( detailsDlg.sBatchRecord.product_codes[10].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[10].amount,		row[62]	,sizeof( detailsDlg.sBatchRecord.product_codes[10].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[11].code,		row[63]	,sizeof( detailsDlg.sBatchRecord.product_codes[11].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[11].quantity,		row[64]	,sizeof( detailsDlg.sBatchRecord.product_codes[11].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[11].amount,		row[65]	,sizeof( detailsDlg.sBatchRecord.product_codes[11].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[12].code,		row[66]	,sizeof( detailsDlg.sBatchRecord.product_codes[12].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[12].quantity,		row[67]	,sizeof( detailsDlg.sBatchRecord.product_codes[12].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[12].amount,		row[68]	,sizeof( detailsDlg.sBatchRecord.product_codes[12].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[13].code,		row[69]	,sizeof( detailsDlg.sBatchRecord.product_codes[13].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[13].quantity,		row[70]	,sizeof( detailsDlg.sBatchRecord.product_codes[13].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[13].amount,		row[71]	,sizeof( detailsDlg.sBatchRecord.product_codes[13].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[14].code,		row[72]	,sizeof( detailsDlg.sBatchRecord.product_codes[14].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[14].quantity,		row[73]	,sizeof( detailsDlg.sBatchRecord.product_codes[14].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[14].amount,		row[74]	,sizeof( detailsDlg.sBatchRecord.product_codes[14].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[15].code,		row[75]	,sizeof( detailsDlg.sBatchRecord.product_codes[15].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[15].quantity,		row[76]	,sizeof( detailsDlg.sBatchRecord.product_codes[15].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[15].amount,		row[77]	,sizeof( detailsDlg.sBatchRecord.product_codes[15].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[16].code,		row[78]	,sizeof( detailsDlg.sBatchRecord.product_codes[16].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[16].quantity,		row[79]	,sizeof( detailsDlg.sBatchRecord.product_codes[16].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[16].amount,		row[80]	,sizeof( detailsDlg.sBatchRecord.product_codes[16].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[17].code,		row[81]	,sizeof( detailsDlg.sBatchRecord.product_codes[17].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[17].quantity,		row[82]	,sizeof( detailsDlg.sBatchRecord.product_codes[17].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[17].amount,		row[83]	,sizeof( detailsDlg.sBatchRecord.product_codes[17].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[18].code,		row[84]	,sizeof( detailsDlg.sBatchRecord.product_codes[18].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[18].quantity,		row[85]	,sizeof( detailsDlg.sBatchRecord.product_codes[18].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[18].amount,		row[86]	,sizeof( detailsDlg.sBatchRecord.product_codes[18].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[19].code,		row[87]	,sizeof( detailsDlg.sBatchRecord.product_codes[19].code ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[19].quantity,		row[88]	,sizeof( detailsDlg.sBatchRecord.product_codes[19].quantity ));
		memcpy( (char *) detailsDlg.sBatchRecord.product_codes[19].amount,		row[89]	,sizeof( detailsDlg.sBatchRecord.product_codes[19].amount ));
		memcpy( (char *) detailsDlg.sBatchRecord.aportionment_flag,			row[90]	,sizeof( detailsDlg.sBatchRecord.aportionment_flag ));
		memcpy( (char *) detailsDlg.sBatchRecord.exp_date,				row[91]	,sizeof( detailsDlg.sBatchRecord.exp_date ));
		memcpy( (char *) detailsDlg.sBatchRecord.open_date,				row[92]	,sizeof( detailsDlg.sBatchRecord.open_date ));
		memcpy( (char *) detailsDlg.sBatchRecord.open_time,				row[93]	,sizeof( detailsDlg.sBatchRecord.open_time ));
		memcpy( (char *) detailsDlg.sBatchRecord.cash_bonus,				row[94]	,sizeof( detailsDlg.sBatchRecord.cash_bonus ));
		memcpy( (char *) detailsDlg.sBatchRecord.transmission_timestamp,		row[95]	,sizeof( detailsDlg.sBatchRecord.transmission_timestamp ));
		memcpy( (char *) detailsDlg.sBatchRecord.mcard_banknet,			row[96]	,sizeof( detailsDlg.sBatchRecord.mcard_banknet ));
		memcpy( (char *) detailsDlg.sBatchRecord.processed,				row[97]	,sizeof( detailsDlg.sBatchRecord.processed ));
		memcpy( (char *) detailsDlg.sBatchRecord.ticket_nbr,				row[98]	,sizeof( detailsDlg.sBatchRecord.ticket_nbr ));
		memcpy( (char *) detailsDlg.sBatchRecord.def_gross_amt,			row[99]	,sizeof( detailsDlg.sBatchRecord.def_gross_amt ));
		memcpy( (char *) detailsDlg.sBatchRecord.term,					row[100]	,sizeof( detailsDlg.sBatchRecord.term ));
		memcpy( (char *) detailsDlg.sBatchRecord.purge_date,					row[101]	,sizeof( detailsDlg.sBatchRecord.purge_date ));

		detailsDlg.DoModal();
	}
	else
	{
		m_CntlStatus.SetWindowText("Still populating the records from database");
	}	
	*pResult = 0;
}

void CBCH20Lookup::OnClose() 
{

	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}
	CDialog::OnClose();
}

void CBCH20Lookup::OnCancel() 
{
	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}	
	
	CDialog::OnCancel();
}
