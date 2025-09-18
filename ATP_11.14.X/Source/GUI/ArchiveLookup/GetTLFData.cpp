// GetTLFData.cpp : implementation file
//

#include "stdafx.h"
#include "ArchiveLookup.h"
#include "GetTLFData.h"
#include "ArchiveLookupDlg.h"
#include "BatchLookup.h"
#include "BCH11Lookup.h"
#include "BCH20Lookup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CArchiveLookupDlg *_pTLFDlg = NULL;
CBatchLookup *_pBatchDlg = NULL;
CBCH11Lookup *_pBCH11Dlg = NULL;
CBCH20Lookup *_pBCH20Dlg = NULL;

/////////////////////////////////////////////////////////////////////////////
// CGetTLFData

IMPLEMENT_DYNCREATE(CGetTLFData, CWinThread)

CGetTLFData::CGetTLFData()
{
	lTotalCount = 0;
	nSessionCount = 0;
	bError = false;
}

CGetTLFData::~CGetTLFData()
{
}

BOOL CGetTLFData::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CGetTLFData::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGetTLFData, CWinThread)
//{{AFX_MSG_MAP(CGetTLFData)
// NOTE - the ClassWizard will add and remove mapping macros here.
ON_THREAD_MESSAGE(WM_GET_TLF_LIST, OnGetTLFList)
ON_THREAD_MESSAGE(WM_GET_TLF_NEXT, OnGetTLFNext)
ON_THREAD_MESSAGE(WM_GET_TLF_COUNT, OnGetTLFCount)
ON_THREAD_MESSAGE(WM_PAUSE, OnPause)
ON_THREAD_MESSAGE(WM_STOP_SEARCH, OnStop)

ON_THREAD_MESSAGE(WM_GET_BCH01_LIST, OnGetBCH01List)
ON_THREAD_MESSAGE(WM_GET_BCH01_NEXT, OnGetBCH01Next)

ON_THREAD_MESSAGE(WM_GET_BCH11_LIST, OnGetBCH11List)
ON_THREAD_MESSAGE(WM_GET_BCH11_NEXT, OnGetBCH11Next)

ON_THREAD_MESSAGE(WM_GET_BCH20_LIST, OnGetBCH20List)
ON_THREAD_MESSAGE(WM_GET_BCH20_NEXT, OnGetBCH20Next)

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetTLFData message handlers
void CGetTLFData::OnGetTLFList(WPARAM wParam, LPARAM lParam)
{
	char strErrorMessage[200] = {0};
	CString temp, szTranAmt, szCreditLimit, szOutBal, szGross, szMonthlyAmort ;
	char strTemp[20] = {0};
	
	long nRowItem = 0;
	long nActualItem = 0;
	int nNumberReturned=0;
	long nSize=0, nCount=0;
	char entry_type[3] = {0};
	char tx_key[3] = {0};
	char general_status[3] = {0};
	char visa_char_indicator[3] = {0};
	char cvc[3] = {0};
	char dynamic_keychange[3] = {0};

	CMyUtils myUtilsObject;
	bDone = FALSE;
	bStopSearch = false;
	bError = false;

	if(lParam != NULL)
	{
		_pTLFDlg = (CArchiveLookupDlg *) lParam;
		
		_pTLFDlg->m_ArchiveList.DeleteAllItems();
		
		memcpy(&sTLFLimRecord, &_pTLFDlg->sTLFLimRecord, sizeof(sTLFLimRecord));
		lTotalCount = 0;
		nSessionCount = 0;
		nNumberReturned = 0;
		bStopSearch = false;
		bError = false;
	}
	
	_pTLFDlg->m_ArchiveList.EnableSorting(false);
	do
	{
		_pTLFDlg->bSearching = true;
		_pTLFDlg->m_CntlNext.EnableWindow(FALSE);
		CString busyText; 	busyText.Format("Retrieving records from database ... %d",nSessionCount);
		_pTLFDlg->m_CntlStatus.SetWindowText(busyText);
		int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(ARCHIVE_TLF01_LIMITS), (pBYTE)&sTLFList, 
			sizeof (ARCHIVE_TLF01_GUI_LIST),TLF01_DATA,ARCH01_DATA,strErrorMessage);
		
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&sTLFList.num_returned);
			nSize = nNumberReturned;
			if ( 0 == nNumberReturned )
			{
				_pTLFDlg->m_CntlStatus.SetWindowText("No records found.");
			}
			else if( nNumberReturned == ARCH_GUI_MAX_LIST_SIZE)
			{				
				memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sTLFList.tlf01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
				nSize = nNumberReturned - 1;
			}
			
			for (int nItem = 0; nItem <  nSize; nItem++)
			{
				//this->m_vTLFBufferList.Add(sTLFList.tlf01_structure[nItem]);
				entry_type[0]         = sTLFList.tlf01_structure[nItem].entry_type;         
				tx_key[0]             = sTLFList.tlf01_structure[nItem].tx_key;             
				general_status[0]     = sTLFList.tlf01_structure[nItem].general_status;     
				visa_char_indicator[0]= sTLFList.tlf01_structure[nItem].visa_char_indicator;
				cvc[0]                = sTLFList.tlf01_structure[nItem].cvc;                
				dynamic_keychange[0]  = sTLFList.tlf01_structure[nItem].dynamic_keychange;  
				szTranAmt = sTLFList.tlf01_structure[nItem].total_amount;
				szCreditLimit	= sTLFList.tlf01_structure[nItem].credit_limit;
				szOutBal		= sTLFList.tlf01_structure[nItem].outstanding_balance;
				szGross			= sTLFList.tlf01_structure[nItem].def_gross_amt;
				szMonthlyAmort	= sTLFList.tlf01_structure[nItem].monthly_amort_amt;
				char tt[10] = {0};
				CString szCardNum = sTLFList.tlf01_structure[nItem].card_num;
				memcpy(tt, sTLFList.tlf01_structure[nItem].date_yyyymmdd, 9);
				
				_pTLFDlg->tlfCardMap.SetAt((char *) sTLFList.tlf01_structure[nItem].primary_key.transaction_id, 
								 (char *) sTLFList.tlf01_structure[nItem].card_num);
				
				if(_pTLFDlg->m_ChkMaskCard.GetCheck() == 1)
				{
					int len = szCardNum.GetLength();
					if(len >= 12)
					{
						char *ptr = szCardNum.GetBuffer(len);
						memcpy(ptr + 6, "******", 6);
						szCardNum.ReleaseBuffer();
					}
				}
				else
				{
				}

				_pTLFDlg->m_ArchiveList.InsertItem(lTotalCount + nItem,
					(char *) sTLFList.tlf01_structure[nItem].primary_key.transaction_id,
					entry_type,
					(char *) sTLFList.tlf01_structure[nItem].tpdu_id,
					(char *) sTLFList.tlf01_structure[nItem].issuer_id,
					(char *) sTLFList.tlf01_structure[nItem].acquirer_id,
					tx_key,
					(char *) sTLFList.tlf01_structure[nItem].terminal_id,
					(char *) sTLFList.tlf01_structure[nItem].terminal_type,
					(char *) sTLFList.tlf01_structure[nItem].response_text,
					(char *) sTLFList.tlf01_structure[nItem].card_num_len,
					szCardNum,
					(char *) sTLFList.tlf01_structure[nItem].exp_date,
					szTranAmt,
					(char *) sTLFList.tlf01_structure[nItem].add_amounts,
					(char *) sTLFList.tlf01_structure[nItem].invoice_number,
					(char *) sTLFList.tlf01_structure[nItem].orig_amount,
					(char *) sTLFList.tlf01_structure[nItem].auth_number,
					(char *) sTLFList.tlf01_structure[nItem].processing_code,
					(char *) sTLFList.tlf01_structure[nItem].sys_trace_audit_num,
					(char *) sTLFList.tlf01_structure[nItem].orig_sys_trace_num,
					(char *) sTLFList.tlf01_structure[nItem].pos_entry_mode,
					(char *) sTLFList.tlf01_structure[nItem].nii,
					(char *) sTLFList.tlf01_structure[nItem].pos_condition_code,
					(char *) sTLFList.tlf01_structure[nItem].track1,
					(char *) sTLFList.tlf01_structure[nItem].track2,
					(char *) sTLFList.tlf01_structure[nItem].retrieval_ref_num,
					(char *) sTLFList.tlf01_structure[nItem].orig_retrieval_ref_num,
					(char *) sTLFList.tlf01_structure[nItem].merchant_id,
					(char *) sTLFList.tlf01_structure[nItem].orig_message,
					(char *) sTLFList.tlf01_structure[nItem].settlement_total,
					(char *) sTLFList.tlf01_structure[nItem].message_type,
					(char *) sTLFList.tlf01_structure[nItem].card_holder_name,
					general_status,
					(char *) sTLFList.tlf01_structure[nItem].originator_queue,
					(char *) sTLFList.tlf01_structure[nItem].originator_info,
					(char *) sTLFList.tlf01_structure[nItem].batch_number,
					(char *) sTLFList.tlf01_structure[nItem].response_code,
					tt,
					(char *) sTLFList.tlf01_structure[nItem].time_hhmmss,
					(char *) sTLFList.tlf01_structure[nItem].num_sales,
					(char *) sTLFList.tlf01_structure[nItem].sales_amount,
					(char *) sTLFList.tlf01_structure[nItem].num_refunds,
					(char *) sTLFList.tlf01_structure[nItem].refund_amount,
					(char *) sTLFList.tlf01_structure[nItem].num_dbsales,
					(char *) sTLFList.tlf01_structure[nItem].dbsales_amount,
					(char *) sTLFList.tlf01_structure[nItem].num_dbrefunds,
					(char *) sTLFList.tlf01_structure[nItem].dbrefund_amount,
					(char *) sTLFList.tlf01_structure[nItem].pin_block,
					(char *) sTLFList.tlf01_structure[nItem].tax_amount,
					(char *) sTLFList.tlf01_structure[nItem].tip_amount,
					(char *) sTLFList.tlf01_structure[nItem].tran_start_time,
					(char *) sTLFList.tlf01_structure[nItem].host_start_time,
					(char *) sTLFList.tlf01_structure[nItem].host_finish_time,
					(char *) sTLFList.tlf01_structure[nItem].tran_finish_time,
					(char *) sTLFList.tlf01_structure[nItem].settle_file_prefix,
					(char *) sTLFList.tlf01_structure[nItem].resp_source_len,
					visa_char_indicator,
					(char *) sTLFList.tlf01_structure[nItem].visa_tran_id,
					(char *) sTLFList.tlf01_structure[nItem].visa_validation_code,
					(char *) sTLFList.tlf01_structure[nItem].mcard_banknet_len,
					(char *) sTLFList.tlf01_structure[nItem].mcard_banknet,
					(char *) sTLFList.tlf01_structure[nItem].agent_id_len,
					(char *) sTLFList.tlf01_structure[nItem].agent_id,
					cvc,
					(char *) sTLFList.tlf01_structure[nItem].settlement_date,
					(char *) sTLFList.tlf01_structure[nItem].category_code,
					(char *) sTLFList.tlf01_structure[nItem].conversion_rate,
					(char *) sTLFList.tlf01_structure[nItem].product_code,
					(char *) sTLFList.tlf01_structure[nItem].odometer,
					(char *) sTLFList.tlf01_structure[nItem].organization_id,
					dynamic_keychange,
					(char *) sTLFList.tlf01_structure[nItem].source_key,
					(char *) sTLFList.tlf01_structure[nItem].txn_cnt,
					(char *) sTLFList.tlf01_structure[nItem].dcf01_retired_cwk,
					(char *) sTLFList.tlf01_structure[nItem].dest_key,
					(char *) sTLFList.tlf01_structure[nItem].ncf01_retired_cwk,
					(char *) sTLFList.tlf01_structure[nItem].handler_queue,
					(char *) sTLFList.tlf01_structure[nItem].authorizing_host_queue,
					(char *) sTLFList.tlf01_structure[nItem].max_net_consec_tmouts,
					(char *) sTLFList.tlf01_structure[nItem].max_active_txns,
					(char *) sTLFList.tlf01_structure[nItem].forwarding_institution_id,
					(char *) sTLFList.tlf01_structure[nItem].remote_nsp_type,
					(char *) sTLFList.tlf01_structure[nItem].kek,
					(char *) sTLFList.tlf01_structure[nItem].acquiring_id,
					szOutBal,
					szCreditLimit,
					(char *) sTLFList.tlf01_structure[nItem].nfi_name,
					(char *) sTLFList.tlf01_structure[nItem].nfi_seq_nbr,
					(char *) sTLFList.tlf01_structure[nItem].nfi_start_date,
					(char *) sTLFList.tlf01_structure[nItem].nfi_end_date,
					(char *) sTLFList.tlf01_structure[nItem].resp_source,
					(char *) sTLFList.tlf01_structure[nItem].dispensation.origin,
					(char *) sTLFList.tlf01_structure[nItem].dispensation.auth_1,
					(char *) sTLFList.tlf01_structure[nItem].dispensation.auth_2,
					(char *) sTLFList.tlf01_structure[nItem].update_mask,
					(char *) sTLFList.tlf01_structure[nItem].type_of_data,
					(char *) sTLFList.tlf01_structure[nItem].down_payment,
					(char *) sTLFList.tlf01_structure[nItem].period,
					(char *) sTLFList.tlf01_structure[nItem].interest_rate,
					(char *) sTLFList.tlf01_structure[nItem].total_interest,
					(char *) sTLFList.tlf01_structure[nItem].redemption_amount,
					(char *) sTLFList.tlf01_structure[nItem].vehicle_number,
					(char *) sTLFList.tlf01_structure[nItem].profile,
					(char *) sTLFList.tlf01_structure[nItem].nbr_of_prod_codes,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[0].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[0].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[0].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[1].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[1].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[1].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[2].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[2].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[2].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[3].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[3].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[3].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[4].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[4].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[4].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[5].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[5].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[5].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[6].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[6].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[6].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[7].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[7].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[7].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[8].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[8].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[8].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[9].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[9].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[9].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[10].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[10].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[10].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[11].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[11].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[11].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[12].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[12].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[12].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[13].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[13].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[13].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[14].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[14].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[14].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[15].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[15].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[15].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[16].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[16].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[16].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[17].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[17].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[17].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[18].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[18].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[18].amount,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[19].code,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[19].quantity,
					(char *) sTLFList.tlf01_structure[nItem].product_codes[19].amount,
					(char *) sTLFList.tlf01_structure[nItem].visa_merchant_id,
					(char *) sTLFList.tlf01_structure[nItem].cash_bonus,
					(char *) sTLFList.tlf01_structure[nItem].deferred_factor,
					(char *) sTLFList.tlf01_structure[nItem].deferred_term_length,
					(char *) sTLFList.tlf01_structure[nItem].currency_code,
					(char *) sTLFList.tlf01_structure[nItem].transmission_timestamp,
					(char *) sTLFList.tlf01_structure[nItem].tran_fee_amount,
					(char *) sTLFList.tlf01_structure[nItem].cvc_data,
					(char *) sTLFList.tlf01_structure[nItem].verify_mag_stripe,
					(char *) sTLFList.tlf01_structure[nItem].saf,
					(char *) sTLFList.tlf01_structure[nItem].security_response_code,
					(char *) sTLFList.tlf01_structure[nItem].service_code,
					(char *) sTLFList.tlf01_structure[nItem].source_pin_format,
					(char *) sTLFList.tlf01_structure[nItem].dest_pin_format,
					(char *) sTLFList.tlf01_structure[nItem].voice_auth,
					(char *) sTLFList.tlf01_structure[nItem].actual_amount,
					(char *) sTLFList.tlf01_structure[nItem].reversal_amount,
					(char *) sTLFList.tlf01_structure[nItem].voice_auth_text,
					(char *) sTLFList.tlf01_structure[nItem].operator_id,
					(char *) sTLFList.tlf01_structure[nItem].workstation,
					(char *) sTLFList.tlf01_structure[nItem].orig_auth_number,
					(char *) sTLFList.tlf01_structure[nItem].ticket_nbr,
					(char *) sTLFList.tlf01_structure[nItem].system_date,
					szGross,
					szMonthlyAmort,
					(char *) sTLFList.tlf01_structure[nItem].purge_date
					);
			}//end of for
		}
		else
		{
			bError = true;
		}	//end of if(ret_code == 0)
		lTotalCount += nSize;
		nSessionCount += nSize;
		if(MAX_SESSION_COUNT > 0)
		{
			if(nSessionCount >= MAX_SESSION_COUNT)
			{
				_pTLFDlg->m_CntlNext.EnableWindow(TRUE);
				bStopSearch = true;
			}
		}
		Sleep(300);
	}while(nNumberReturned == ARCH_GUI_MAX_LIST_SIZE && !bStopSearch);

	_pTLFDlg->bSearching = false;
	CString szz;
	if(bError)
	{
		szz.Format("ERROR: %s",strErrorMessage);
	}
	else
	{
		szz.Format("Search complete... %ld records found",lTotalCount);
	}
	_pTLFDlg->m_CntlStatus.SetWindowText(szz);
	
	_pTLFDlg->m_CntlSearch.EnableWindow(TRUE);
	if(!bStopSearch)
	{
		
		
		_pTLFDlg->m_CntlPause.EnableWindow(FALSE);
		_pTLFDlg->m_CntlStopSearch.EnableWindow(FALSE);
		_pTLFDlg->m_CntlPause.SetWindowText("Pause");
		
		bStopSearch = true;
		memset(&sTLFLimRecord, 0, sizeof(sTLFLimRecord));
	}
	_pTLFDlg->m_ArchiveList.EnableSorting(true);
}

void CGetTLFData::OnGetTLFNext(WPARAM wParam, LPARAM lParam)
{
	bStopSearch = false;
	nSessionCount = 0;
	OnGetTLFList(0,0);

}

void CGetTLFData::OnGetBCH01List(WPARAM wParam, LPARAM lParam)
{
		char strErrorMessage[200] = {0};
	CString temp;
	char strTemp[20] = {0};
	
	long nRowItem = 0;
	long nActualItem = 0;
	int nNumberReturned=0;
	long nSize=0, nCount=0;
	
	CMyUtils myUtilsObject;
	bDone = FALSE;
	bStopSearch = false;
	bError = false;

	if(lParam != NULL)
	{
		_pBatchDlg = (CBatchLookup *) lParam;
		
		_pBatchDlg->m_CntlBCH01List.DeleteAllItems();
		
		memcpy(&this->sBCH01Record, &_pBatchDlg->sBatchRecord , sizeof(ARCHIVE_BCH01));
		lTotalCount = 0;
		nSessionCount = 0;
		nNumberReturned = 0;
		bStopSearch = false;
		bError = false;
		
	}
	
	_pBatchDlg->m_CntlBCH01List.EnableSorting(false);
	do
	{
		_pBatchDlg->bSearching = true;
		_pBatchDlg->m_CntlNext.EnableWindow(FALSE);
		CString busyText; 	busyText.Format("Retrieving records from database ... %d",nSessionCount);
		_pBatchDlg->m_CntlStatus.SetWindowText(busyText);

		int rcode = txdsapi_get_list ((pBYTE)&this->sBCH01Record,sizeof(ARCHIVE_BCH01), (pBYTE)&this->sBCH01List, 
			sizeof (ARCHIVE_BCH01_GUI_LIST),BCH01_DATA,ARCH01_DATA,strErrorMessage);
		
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&this->sBCH01List.num_returned);
			nSize = nNumberReturned;
			if ( 0 == nNumberReturned )
			{
				_pBatchDlg->m_CntlStatus.SetWindowText("No records found.");
				bStopSearch = true;
			}
			else if( nNumberReturned == ARCH_GUI_MAX_LIST_SIZE)
			{				
				memcpy(this->sBCH01Record.primary_key.batch_nbr, 
					(LPCTSTR) this->sBCH01List.batch_record[nNumberReturned-1].primary_key.batch_nbr, 
					sizeof(sBCH01Record.primary_key.batch_nbr));
				memcpy(this->sBCH01Record.primary_key.device_id, 
					(LPCTSTR) this->sBCH01List.batch_record[nNumberReturned-1].primary_key.device_id, 
					sizeof(sBCH01Record.primary_key.device_id));
				nSize = nNumberReturned - 1;
			}
			
			for (int nItem = 0; nItem <  nSize; nItem++)
			{
				_pBatchDlg->m_CntlBCH01List.InsertItem(lTotalCount + nItem,
					(char *) this->sBCH01List.batch_record[nItem].primary_key.device_id,
					(char *) this->sBCH01List.batch_record[nItem].primary_key.batch_nbr,
					(char *) this->sBCH01List.batch_record[nItem].merchant_id,
					(char *) this->sBCH01List.batch_record[nItem].total_credit_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_credit_amt,
					(char *) this->sBCH01List.batch_record[nItem].total_credit_refund_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_credit_refund_amt,
					(char *) this->sBCH01List.batch_record[nItem].total_debit_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_debit_amt,
					(char *) this->sBCH01List.batch_record[nItem].total_debit_refund_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_debit_refund_amt,
					(char *) this->sBCH01List.batch_record[nItem].total_void_credit_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_void_credit_amt,
					(char *) this->sBCH01List.batch_record[nItem].total_void_credit_refund_cnt,
					(char *) this->sBCH01List.batch_record[nItem].total_void_credit_refund_amt,
					(char *) this->sBCH01List.batch_record[nItem].open_date,
					(char *) this->sBCH01List.batch_record[nItem].open_time,
					(char *) this->sBCH01List.batch_record[nItem].release_date,
					(char *) this->sBCH01List.batch_record[nItem].release_time,
					(char *) this->sBCH01List.batch_record[nItem].batch_status,
					(char *) this->sBCH01List.batch_record[nItem].visa_merchant_id,
					(char *) this->sBCH01List.batch_record[nItem].purge_date
					);
			}//end of for
		}
		else
		{
			bError = true;
		}	//end of if(ret_code == 0)
		lTotalCount += nSize;
		nSessionCount += nSize;
		if(MAX_SESSION_COUNT > 0)
		{
			if(nSessionCount >= MAX_SESSION_COUNT)
			{
				_pBatchDlg->m_CntlNext.EnableWindow(TRUE);
				bStopSearch = true;
			}
		}
		Sleep(300);
	}while(nNumberReturned == ARCH_GUI_MAX_LIST_SIZE && !bStopSearch);

	_pBatchDlg->bSearching = false;
	CString szz;
	if(bError)
	{
		szz.Format("ERROR: %s",strErrorMessage);
	}
	else
	{
		szz.Format("Search complete... %ld records found",lTotalCount);
	}
		_pBatchDlg->m_CntlStatus.SetWindowText(szz);

		
	if(!bStopSearch)
	{
		bStopSearch = true;
		memset(&sBCH01Record, 0, sizeof(ARCHIVE_BCH01));
	}
	_pBatchDlg->m_CntlBCH01List.EnableSorting(true);
}

void CGetTLFData::OnGetBCH01Next(WPARAM wParam, LPARAM lParam)
{
	bStopSearch = false;
	nSessionCount = 0;
	OnGetBCH01List(0,0);

}


void CGetTLFData::OnGetBCH11List(WPARAM wParam, LPARAM lParam)
{
		char strErrorMessage[200] = {0};
	CString temp;
	char strTemp[20] = {0};
	
	long nRowItem = 0;
	long nActualItem = 0;
	int nNumberReturned=0;
	long nSize=0, nCount=0;
	
	CMyUtils myUtilsObject;
	bDone = FALSE;
	bStopSearch = false;
	bError = false;
	char tx_key[3] = {0};
	char bin_type[3] = {0};

	if(lParam != NULL)
	{
		_pBCH11Dlg = (CBCH11Lookup *) lParam;
		
		_pBCH11Dlg->m_CntlBCHList.DeleteAllItems();
		
		memcpy(&this->sBCH11Record, &_pBCH11Dlg->sBCH11Record , sizeof(ARCHIVE_BCH11));
		lTotalCount = 0;
		nSessionCount = 0;
		nNumberReturned = 0;
		bStopSearch = false;
		bError = false;
		
	}
	_pBCH11Dlg->m_CntlBCHList.EnableSorting(false);

	do
	{
		_pBCH11Dlg->bSearching = true;
		_pBCH11Dlg->m_CntlNext.EnableWindow(FALSE);
		CString busyText; 	busyText.Format("Retrieving records from database ... %d",nSessionCount);
		_pBCH11Dlg->m_CntlStatus.SetWindowText(busyText);

		int rcode = txdsapi_get_list ((pBYTE)&this->sBCH11Record,sizeof(ARCHIVE_BCH11), (pBYTE)&this->sBCH11List, 
			sizeof (ARCHIVE_BCH11_GUI_LIST),BCH11_DATA,ARCH01_DATA,strErrorMessage);
		
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&this->sBCH11List.num_returned);
			nSize = nNumberReturned;
			if ( 0 == nNumberReturned )
			{
				_pBCH11Dlg->m_CntlStatus.SetWindowText("No records found.");
				bStopSearch = true;
			}
			else if( nNumberReturned == ARCH_GUI_MAX_LIST_SIZE)
			{				
				memcpy(this->sBCH11Record.transaction_id, 
					(LPCTSTR) this->sBCH11List.batch_record[nNumberReturned-1].transaction_id, 
					sizeof(sBCH11Record.transaction_id));				
				nSize = nNumberReturned - 1;
			}
			
			for (int nItem = 0; nItem <  nSize; nItem++)
			{
				tx_key[0]	= this->sBCH11List.batch_record[nItem].tx_key;
				bin_type[0]	= this->sBCH11List.batch_record[nItem].bin_type;

				_pBCH11Dlg->m_CntlBCHList.InsertItem(lTotalCount + nItem,
					(char *) this->sBCH11List.batch_record[nItem].primary_key.device_id,
					(char *) this->sBCH11List.batch_record[nItem].primary_key.batch_nbr,
					(char *) this->sBCH11List.batch_record[nItem].primary_key.retrieval_ref_num,
					(char *) this->sBCH11List.batch_record[nItem].merchant_id,
					(char *) this->sBCH11List.batch_record[nItem].processing_code,
					(char *) this->sBCH11List.batch_record[nItem].tran_amount,
					(char *) this->sBCH11List.batch_record[nItem].response_code,
					(char *) this->sBCH11List.batch_record[nItem].auth_number,
					(char *) this->sBCH11List.batch_record[nItem].tran_date,
					(char *) this->sBCH11List.batch_record[nItem].tran_time,
					(char *) this->sBCH11List.batch_record[nItem].transaction_id,
					(char *) this->sBCH11List.batch_record[nItem].message_type,
					(char *) this->sBCH11List.batch_record[nItem].card_nbr,
					(char *) this->sBCH11List.batch_record[nItem].invoice_nbr,
					(char *) this->sBCH11List.batch_record[nItem].settle_file_prefix,
					(char *) this->sBCH11List.batch_record[nItem].product_code,
					(char *) this->sBCH11List.batch_record[nItem].odometer,
					(char *) this->sBCH11List.batch_record[nItem].sys_trace_audit_num,
					tx_key,
					bin_type,
					(char *) this->sBCH11List.batch_record[nItem].type_of_data,
					(char *) this->sBCH11List.batch_record[nItem].down_payment,
					(char *) this->sBCH11List.batch_record[nItem].period,
					(char *) this->sBCH11List.batch_record[nItem].interest_rate,
					(char *) this->sBCH11List.batch_record[nItem].total_interest,
					(char *) this->sBCH11List.batch_record[nItem].redemption_amount,
					(char *) this->sBCH11List.batch_record[nItem].vehicle_number,
					(char *) this->sBCH11List.batch_record[nItem].pos_entry_mode,
					(char *) this->sBCH11List.batch_record[nItem].profile,
					(char *) this->sBCH11List.batch_record[nItem].nbr_of_prod_codes,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[0].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[0].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[0].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[1].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[1].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[1].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[2].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[2].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[2].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[3].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[3].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[3].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[4].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[4].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[4].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[5].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[5].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[5].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[6].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[6].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[6].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[7].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[7].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[7].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[8].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[8].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[8].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[9].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[9].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[9].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[10].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[10].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[10].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[11].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[11].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[11].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[12].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[12].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[12].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[13].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[13].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[13].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[14].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[14].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[14].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[15].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[15].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[15].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[16].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[16].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[16].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[17].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[17].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[17].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[18].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[18].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[18].amount,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[19].code,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[19].quantity,
					(char *) this->sBCH11List.batch_record[nItem].product_codes[19].amount,
					(char *) this->sBCH11List.batch_record[nItem].aportionment_flag,
					(char *) this->sBCH11List.batch_record[nItem].exp_date,
					(char *) this->sBCH11List.batch_record[nItem].open_date,
					(char *) this->sBCH11List.batch_record[nItem].open_time,
					(char *) this->sBCH11List.batch_record[nItem].cash_bonus,
					(char *) this->sBCH11List.batch_record[nItem].transmission_timestamp,
					(char *) this->sBCH11List.batch_record[nItem].mcard_banknet,
					(char *) this->sBCH11List.batch_record[nItem].ticket_nbr,
					(char *) this->sBCH11List.batch_record[nItem].def_gross_amt,
					(char *) this->sBCH11List.batch_record[nItem].term,
					(char *) this->sBCH11List.batch_record[nItem].purge_date
					);
			}//end of for
		}
		else
		{
			bError = true;
		}	//end of if(ret_code == 0)
		lTotalCount += nSize;
		nSessionCount += nSize;
		if(MAX_SESSION_COUNT > 0)
		{
			if(nSessionCount >= MAX_SESSION_COUNT)
			{
				_pBCH11Dlg->m_CntlNext.EnableWindow(TRUE);
				bStopSearch = true;
			}
		}
		Sleep(300);
	}while(nNumberReturned == ARCH_GUI_MAX_LIST_SIZE && !bStopSearch);

	_pBCH11Dlg->bSearching = false;
	CString szz;
	if(bError)
	{
		szz.Format("ERROR: %s",strErrorMessage);
	}
	else
	{
		szz.Format("Search complete... %ld records found",lTotalCount);
	}
	_pBCH11Dlg->m_CntlStatus.SetWindowText(szz);
	if(!bStopSearch)
	{
		
		bStopSearch = true;
		memset(&sBCH11Record, 0, sizeof(ARCHIVE_BCH11));
	}
	_pBCH11Dlg->m_CntlBCHList.EnableSorting(true);
}

void CGetTLFData::OnGetBCH11Next(WPARAM wParam, LPARAM lParam)
{
	bStopSearch = false;
	nSessionCount = 0;
	OnGetBCH11List(0,0);

}

void CGetTLFData::OnGetBCH20List(WPARAM wParam, LPARAM lParam)
{
		char strErrorMessage[200] = {0};
	CString temp;
	char strTemp[20] = {0};
	
	long nRowItem = 0;
	long nActualItem = 0;
	int nNumberReturned=0;
	long nSize=0, nCount=0;
	
	CMyUtils myUtilsObject;
	bDone = FALSE;
	bStopSearch = false;
	bError = false;
	char tx_key[3] = {0};
	char bin_type[3] = {0};


	if(lParam != NULL)
	{
		_pBCH20Dlg = (CBCH20Lookup *) lParam;
		
		_pBCH20Dlg->m_CntlBCHList.DeleteAllItems();
		
		memcpy(&this->sBCH20Record, &_pBCH20Dlg->sBCH20Record , sizeof(ARCHIVE_BCH20));
		lTotalCount = 0;
		nSessionCount = 0;
		nNumberReturned = 0;
		bStopSearch = false;
		bError = false;
		
	}
	_pBCH20Dlg->m_CntlBCHList.EnableSorting(false);

	do
	{
		_pBCH20Dlg->bSearching = true;
		_pBCH20Dlg->m_CntlNext.EnableWindow(FALSE);
		CString busyText; 	busyText.Format("Retrieving records from database ... %d",nSessionCount);
		_pBCH20Dlg->m_CntlStatus.SetWindowText(busyText);
		int rcode = txdsapi_get_list ((pBYTE)&this->sBCH20Record,sizeof(ARCHIVE_BCH20), (pBYTE)&this->sBCH20List, 
			sizeof (ARCHIVE_BCH20_GUI_LIST),BCH20_DATA,ARCH01_DATA,strErrorMessage);
		
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&this->sBCH20List.num_returned);
			nSize = nNumberReturned;
			if ( 0 == nNumberReturned )
			{
				_pBCH20Dlg->m_CntlStatus.SetWindowText("No records found.");
				bStopSearch = true;
			}
			else if( nNumberReturned == ARCH_GUI_MAX_LIST_SIZE)
			{				
				memcpy(this->sBCH20Record.transaction_id, 
					(LPCTSTR) this->sBCH20List.batch_record[nNumberReturned-1].transaction_id, 
					sizeof(sBCH20Record.transaction_id));				
				nSize = nNumberReturned - 1;
			}
			
			for (int nItem = 0; nItem <  nSize; nItem++)
			{
				tx_key[0]	= this->sBCH20List.batch_record[nItem].tx_key;
				bin_type[0]	= this->sBCH20List.batch_record[nItem].bin_type;

				_pBCH20Dlg->m_CntlBCHList.InsertItem(lTotalCount + nItem,
					(char *) this->sBCH20List.batch_record[nItem].primary_key.device_id,
					(char *) this->sBCH20List.batch_record[nItem].primary_key.batch_nbr,
					(char *) this->sBCH20List.batch_record[nItem].primary_key.retrieval_ref_num,
					(char *) this->sBCH20List.batch_record[nItem].merchant_id,
					(char *) this->sBCH20List.batch_record[nItem].processing_code,
					(char *) this->sBCH20List.batch_record[nItem].tran_amount,
					(char *) this->sBCH20List.batch_record[nItem].response_code,
					(char *) this->sBCH20List.batch_record[nItem].auth_number,
					(char *) this->sBCH20List.batch_record[nItem].tran_date,
					(char *) this->sBCH20List.batch_record[nItem].tran_time,
					(char *) this->sBCH20List.batch_record[nItem].transaction_id,
					(char *) this->sBCH20List.batch_record[nItem].message_type,
					(char *) this->sBCH20List.batch_record[nItem].card_nbr,
					(char *) this->sBCH20List.batch_record[nItem].invoice_nbr,
					(char *) this->sBCH20List.batch_record[nItem].settle_file_prefix,
					(char *) this->sBCH20List.batch_record[nItem].product_code,
					(char *) this->sBCH20List.batch_record[nItem].odometer,
					(char *) this->sBCH20List.batch_record[nItem].sys_trace_audit_num,
					tx_key,
					bin_type,
					(char *) this->sBCH20List.batch_record[nItem].type_of_data,
					(char *) this->sBCH20List.batch_record[nItem].down_payment,
					(char *) this->sBCH20List.batch_record[nItem].period,
					(char *) this->sBCH20List.batch_record[nItem].interest_rate,
					(char *) this->sBCH20List.batch_record[nItem].total_interest,
					(char *) this->sBCH20List.batch_record[nItem].redemption_amount,
					(char *) this->sBCH20List.batch_record[nItem].vehicle_number,
					(char *) this->sBCH20List.batch_record[nItem].pos_entry_mode,
					(char *) this->sBCH20List.batch_record[nItem].profile,
					(char *) this->sBCH20List.batch_record[nItem].nbr_of_prod_codes,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[0].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[0].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[0].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[1].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[1].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[1].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[2].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[2].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[2].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[3].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[3].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[3].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[4].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[4].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[4].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[5].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[5].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[5].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[6].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[6].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[6].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[7].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[7].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[7].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[8].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[8].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[8].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[9].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[9].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[9].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[10].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[10].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[10].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[11].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[11].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[11].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[12].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[12].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[12].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[13].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[13].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[13].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[14].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[14].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[14].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[15].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[15].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[15].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[16].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[16].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[16].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[17].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[17].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[17].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[18].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[18].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[18].amount,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[19].code,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[19].quantity,
					(char *) this->sBCH20List.batch_record[nItem].product_codes[19].amount,
					(char *) this->sBCH20List.batch_record[nItem].aportionment_flag,
					(char *) this->sBCH20List.batch_record[nItem].exp_date,
					(char *) this->sBCH20List.batch_record[nItem].open_date,
					(char *) this->sBCH20List.batch_record[nItem].open_time,
					(char *) this->sBCH20List.batch_record[nItem].cash_bonus,
					(char *) this->sBCH20List.batch_record[nItem].transmission_timestamp,
					(char *) this->sBCH20List.batch_record[nItem].mcard_banknet,
					(char *) this->sBCH20List.batch_record[nItem].processed,
					(char *) this->sBCH20List.batch_record[nItem].ticket_nbr,
					(char *) this->sBCH20List.batch_record[nItem].def_gross_amt,
					(char *) this->sBCH20List.batch_record[nItem].term,
					(char *) this->sBCH20List.batch_record[nItem].purge_date
					);
			}//end of for
		}
		else
		{
			bError = true;
		}	//end of if(ret_code == 0)
		lTotalCount		+= nSize;
		nSessionCount	+= nSize;
		if(MAX_SESSION_COUNT > 0)
		{
			if(nSessionCount >= MAX_SESSION_COUNT)
			{
				_pBCH20Dlg->m_CntlNext.EnableWindow(TRUE);
				bStopSearch = true;
			}
		}
		Sleep(300);
	}while(nNumberReturned == ARCH_GUI_MAX_LIST_SIZE && !bStopSearch);

	_pBCH20Dlg->bSearching = false;
	CString szz;
	if(bError)
	{
		szz.Format("ERROR: %s",strErrorMessage);
	}
	else
	{
		szz.Format("Search complete... %ld records found",lTotalCount);
	}	_pBCH20Dlg->m_CntlStatus.SetWindowText(szz);
	
	if(!bStopSearch)
	{
		bStopSearch = true;
		memset(&sBCH20Record, 0, sizeof(ARCHIVE_BCH20));
	}
	_pBCH20Dlg->m_CntlBCHList.EnableSorting(true);
}

void CGetTLFData::OnGetBCH20Next(WPARAM wParam, LPARAM lParam)
{
	bStopSearch = false;
	nSessionCount = 0;
	OnGetBCH20List(0,0);

}



void CGetTLFData::OnGetTLFCount(WPARAM wParam, LPARAM lParam)
{
}

void CGetTLFData::OnPause(WPARAM wParam, LPARAM lParam)
{
}

void CGetTLFData::OnStop(WPARAM wParam, LPARAM lParam)
{
	memset(&sTLFLimRecord, 0, sizeof(sTLFLimRecord));
}
