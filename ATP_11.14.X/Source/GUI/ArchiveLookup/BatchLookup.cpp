// BatchLookup.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "BatchLookup.h"
#include "BCH01Details.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchLookup dialog


CBatchLookup::CBatchLookup(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchLookup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchLookup)
	m_szBatchNbr = _T("");
	m_szCloseDate = _T("");
	m_szDeviceID = _T("");
	m_szMerchantID = _T("");
	m_szOpenDate = _T("");
	m_szStatus = _T("");
	//}}AFX_DATA_INIT
	bSearching = false;
	m_pGetTLF = NULL;
	CWinThread *thr = AfxBeginThread(RUNTIME_CLASS(CGetTLFData),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_pGetTLF = dynamic_cast<CGetTLFData *>(thr);

	arrColumnNames.Add("DEVICE ID");
	arrColumnNames.Add("BATCH NBR");
	arrColumnNames.Add("MERCHANT ID");
	arrColumnNames.Add("TOTAL CREDIT CNT");
	arrColumnNames.Add("TOTAL CREDIT AMT");
	arrColumnNames.Add("TOTAL CREDIT REFUND CNT");
	arrColumnNames.Add("TOTAL CREDIT REFUND AMT");
	arrColumnNames.Add("TOTAL DEBIT CNT");
	arrColumnNames.Add("TOTAL DEBIT AMT");
	arrColumnNames.Add("TOTAL DEBIT REFUND CNT");
	arrColumnNames.Add("TOTAL DEBIT REFUND AMT");
	arrColumnNames.Add("TOTAL VOID CREDIT CNT");
	arrColumnNames.Add("TOTAL VOID CREDIT AMT");
	arrColumnNames.Add("TOTAL VOID CREDIT REFUND CNT");
	arrColumnNames.Add("TOTAL VOID CREDIT REFUND AMT");
	arrColumnNames.Add("OPEN DATE");
	arrColumnNames.Add("OPEN TIME");
	arrColumnNames.Add("RELEASE DATE");
	arrColumnNames.Add("RELEASE TIME");
	arrColumnNames.Add("BATCH STATUS");
	arrColumnNames.Add("VISA MERCHANT ID");
	arrColumnNames.Add("PURGE DATE");

}


void CBatchLookup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchLookup)
	DDX_Control(pDX, IDC_NEXT, m_CntlNext);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Control(pDX, IDC_BCH01_LIST, m_CntlBCH01List);
	DDX_Text(pDX, IDC_BATCH_NBR, m_szBatchNbr);
	DDV_MaxChars(pDX, m_szBatchNbr, 6);
	DDX_Text(pDX, IDC_DEVICE_ID, m_szDeviceID);
	DDV_MaxChars(pDX, m_szDeviceID, 8);
	DDX_Text(pDX, IDC_STATUS, m_szStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchLookup, CDialog)
	//{{AFX_MSG_MAP(CBatchLookup)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_BCH01_LIST, OnDblclkBch01List)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchLookup message handlers

void CBatchLookup::OnOK() 
{
	// TODO: Add extra validation here

		CString		temp;
	bool		all_fields_empty = true;
	CMyUtils	myUtilsObject;
	
	UpdateData(TRUE);
	ZeroMemory ( &sBatchRecord, sizeof ( ARCHIVE_BCH01 ) );
	
	m_szStatus= "";
	UpdateData(FALSE);
	
	if ( !myUtilsObject.IsEmpty(m_szBatchNbr, this) )
	{
		all_fields_empty = false;
		memcpy(sBatchRecord.primary_key.batch_nbr, 
			m_szBatchNbr.GetBuffer(sizeof(sBatchRecord.primary_key.batch_nbr)), 
			sizeof(sBatchRecord.primary_key.batch_nbr));
	}
	if ( !myUtilsObject.IsEmpty(m_szDeviceID, this) )
	{
		all_fields_empty = false;
		memcpy(sBatchRecord.primary_key.device_id, 
			m_szDeviceID.GetBuffer(sizeof(sBatchRecord.primary_key.device_id)), 
			sizeof(sBatchRecord.primary_key.device_id));
	}

	if ( all_fields_empty == true )
	{
		m_szStatus= "Please enter search criteria...";
		UpdateData(FALSE);	  
	//	m_CntlCardNbr.SetFocus();
		return;
	}
		m_pGetTLF->PostThreadMessage(WM_GET_BCH01_LIST,0,(LPARAM)this);
}

BOOL CBatchLookup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pGetTLF->ResumeThread();
	m_CntlBCH01List.InsertColumn(arrColumnNames, LVCFMT_LEFT, 90);
	UpdateTitleWithVersion(this);
	this->ShowWindow(SW_MAXIMIZE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchLookup::OnNext() 
{
	m_pGetTLF->PostThreadMessage(WM_GET_BCH01_NEXT,0,0);	
}

void CBatchLookup::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_CntlBCH01List.Resize(cx, cy);	
}

void CBatchLookup::OnDblclkBch01List(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CBCH01Details dlg;
	int nItem = m_CntlBCH01List.GetFirstSelectedItem();
	CVector <CString> row;
	m_CntlBCH01List.GetSelectedRow(row);
	
	if(row.GetSize() == 22)
	{
		memcpy( (char *) dlg.sBatchRecord.primary_key.device_id,	row[0] 	,sizeof( dlg.sBatchRecord.primary_key.device_id ));
		memcpy( (char *) dlg.sBatchRecord.primary_key.batch_nbr,	row[1] 	,sizeof( dlg.sBatchRecord.primary_key.batch_nbr ));
		memcpy( (char *) dlg.sBatchRecord.merchant_id,			row[2] 	,sizeof( dlg.sBatchRecord.merchant_id ));
		memcpy( (char *) dlg.sBatchRecord.total_credit_cnt,		row[3] 	,sizeof( dlg.sBatchRecord.total_credit_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_credit_amt,		row[4] 	,sizeof( dlg.sBatchRecord.total_credit_amt ));
		memcpy( (char *) dlg.sBatchRecord.total_credit_refund_cnt,	row[5] 	,sizeof( dlg.sBatchRecord.total_credit_refund_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_credit_refund_amt,	row[6] 	,sizeof( dlg.sBatchRecord.total_credit_refund_amt ));
		memcpy( (char *) dlg.sBatchRecord.total_debit_cnt,		row[7] 	,sizeof( dlg.sBatchRecord.total_debit_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_debit_amt,		row[8] 	,sizeof( dlg.sBatchRecord.total_debit_amt ));
		memcpy( (char *) dlg.sBatchRecord.total_debit_refund_cnt,	row[9] 	,sizeof( dlg.sBatchRecord.total_debit_refund_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_debit_refund_amt,	row[10]	,sizeof( dlg.sBatchRecord.total_debit_refund_amt ));
		memcpy( (char *) dlg.sBatchRecord.total_void_credit_cnt,	row[11]	,sizeof( dlg.sBatchRecord.total_void_credit_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_void_credit_amt,	row[12]	,sizeof( dlg.sBatchRecord.total_void_credit_amt ));
		memcpy( (char *) dlg.sBatchRecord.total_void_credit_refund_cnt,	row[13]	,sizeof( dlg.sBatchRecord.total_void_credit_refund_cnt ));
		memcpy( (char *) dlg.sBatchRecord.total_void_credit_refund_amt,	row[14]	,sizeof( dlg.sBatchRecord.total_void_credit_refund_amt ));
		memcpy( (char *) dlg.sBatchRecord.open_date,			row[15]	,sizeof( dlg.sBatchRecord.open_date ));
		memcpy( (char *) dlg.sBatchRecord.open_time,			row[16]	,sizeof( dlg.sBatchRecord.open_time ));
		memcpy( (char *) dlg.sBatchRecord.release_date,			row[17]	,sizeof( dlg.sBatchRecord.release_date ));
		memcpy( (char *) dlg.sBatchRecord.release_time,			row[18]	,sizeof( dlg.sBatchRecord.release_time ));
		memcpy( (char *) dlg.sBatchRecord.batch_status,			row[19]	,sizeof( dlg.sBatchRecord.batch_status ));
		memcpy( (char *) dlg.sBatchRecord.visa_merchant_id,		row[20]	,sizeof( dlg.sBatchRecord.visa_merchant_id ));
		memcpy( (char *) dlg.sBatchRecord.purge_date,		row[21]	,sizeof( dlg.sBatchRecord.purge_date ));
		dlg.DoModal();
	}
	else
	{
		m_CntlStatus.SetWindowText("Still populating the records from database");
	}

	

	*pResult = 0;
}

void CBatchLookup::OnClose() 
{
	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}	
	
	CDialog::OnClose();
}

void CBatchLookup::OnCancel() 
{
	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}	
	
	CDialog::OnCancel();
}
