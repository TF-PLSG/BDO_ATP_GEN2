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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TerminalValidTransactions.cpp  $
   
      Rev 1.6   Dec 01 2003 11:30:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.3   Oct 15 1999 17:22:12   iarustam
   bug fixes
   
      Rev 1.2   Sep 30 1999 16:17:52   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 16:01:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:26   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:34   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:57:10   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:36   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:24   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:20   iarustam
   Bug fixes and core 1_3_0 changes
   

    
*
************************************************************************************/

// TerminalValidTransactions.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "TerminalValidTransactions.h"
#include "MyUtils.h"
#include "SharedMem.h"	

extern "C"{
#include "PteTime.h"
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CTerminalValidTransactions property page

IMPLEMENT_DYNCREATE(CTerminalValidTransactions, CPropertyPage)

CTerminalValidTransactions::CTerminalValidTransactions() : CPropertyPage(CTerminalValidTransactions::IDD)
{
	//{{AFX_DATA_INIT(CTerminalValidTransactions)
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrTerminalIDFld = _T("");
	//}}AFX_DATA_INIT
}

CTerminalValidTransactions::~CTerminalValidTransactions()
{
}

void CTerminalValidTransactions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalValidTransactions)
	DDX_Control(pDX, IDC_INITIALIZATION, m_Initialization);
	DDX_Control(pDX, IDC_VOID_SALE_ON_LINE, m_VoidSaleOnLine);
	DDX_Control(pDX, IDC_VOID_REFUND_ON_LINE, m_VoidRefundOnLine);
	DDX_Control(pDX, IDC_TEST_TRANSACTION, m_TestTransaction);
	DDX_Control(pDX, IDC_STATISTICS, m_Statistics);
	DDX_Control(pDX, IDC_SIGNATURE_DATA, m_SignatureData);
	DDX_Control(pDX, IDC_SETTLEMENT_REQUEST, m_SettlementRequest);
	DDX_Control(pDX, IDC_SETTLEMENT_AFTER_UPLOAD, m_SettlementAfterUpload);
	DDX_Control(pDX, IDC_SALES_COMPLETION, m_SalesCompletion);
	DDX_Control(pDX, IDC_SALE_AND_CASH, m_SaleAndCash);
	DDX_Control(pDX, IDC_SALE, m_Sale);
	DDX_Control(pDX, IDC_REVERSAL, m_Reversal);
	DDX_Control(pDX, IDC_REFUND, m_Refund);
	DDX_Control(pDX, IDC_RE_AUTH, m_ReAuth);
	DDX_Control(pDX, IDC_PRE_AUHTORIZATION, m_PreAuthorization);
	DDX_Control(pDX, IDC_PLEASE_WAIT_ADVICE, m_PleaseWaitAdvice);
	DDX_Control(pDX, IDC_OFF_LINE_SALE, m_OffLineSale);
	DDX_Control(pDX, IDC_OFF_LINE_REFUND, m_OffLineRefund);
	DDX_Control(pDX, IDC_LOGON, m_Logon);
	DDX_Control(pDX, IDC_GUARANTEED_LATE_ARRIVAL, m_GuaranteedLateArrival);
	DDX_Control(pDX, IDC_CHECK_VERIFICATION, m_CheckVerification);
	DDX_Control(pDX, IDC_CASH, m_Cash);
	DDX_Control(pDX, IDC_CARD_VERIFICATION, m_CardVerification);
	DDX_Control(pDX, IDC_BATCH_UPLOAD, m_BatchUpload);
	DDX_Control(pDX, IDC_BATCH_DOWN_LINE_LOAD, m_BatchDownLineLoad);
	DDX_Control(pDX, IDC_BALANCE_INQUIRY, m_BalanceInquiry);
	DDX_Control(pDX, IDC_AUTHORIZATION, m_Authorization);
	DDX_Control(pDX, IDC_ADJUST_SALE, m_AdjustSale);
	DDX_Control(pDX, IDC_ADJUST_REFUND, m_AdjustRefund);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerminalValidTransactions, CPropertyPage)
	//{{AFX_MSG_MAP(CTerminalValidTransactions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalValidTransactions message handlers

BOOL CTerminalValidTransactions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bTran = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableTranDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateTranDialogFromDatabase ( sTerminalRecordOld );
	}  

	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerminalValidTransactions::EnableTranDialog ( int nStatus )
{
	m_VoidSaleOnLine.EnableWindow(nStatus);
	m_VoidRefundOnLine.EnableWindow(nStatus);
	m_TestTransaction.EnableWindow(nStatus);
	m_Statistics.EnableWindow(nStatus);
	m_SignatureData.EnableWindow(nStatus);
	m_SettlementRequest.EnableWindow(nStatus);
	m_SettlementAfterUpload.EnableWindow(nStatus);
	m_SalesCompletion.EnableWindow(nStatus);
	m_SaleAndCash.EnableWindow(nStatus);
	m_Sale.EnableWindow(nStatus);
	m_Reversal.EnableWindow(nStatus);
	m_Refund.EnableWindow(nStatus);
	m_ReAuth.EnableWindow(nStatus);
	m_PreAuthorization.EnableWindow(nStatus);
	m_PleaseWaitAdvice.EnableWindow(nStatus);
	m_OffLineSale.EnableWindow(nStatus);
	m_OffLineRefund.EnableWindow(nStatus);
	m_Logon.EnableWindow(nStatus);
	m_Initialization.EnableWindow(nStatus);
	m_GuaranteedLateArrival.EnableWindow(nStatus);
	m_CheckVerification.EnableWindow(nStatus);
	m_Cash.EnableWindow(nStatus);
	m_CardVerification.EnableWindow(nStatus);
	m_BatchUpload.EnableWindow(nStatus);
	m_BatchDownLineLoad.EnableWindow(nStatus);
	m_BalanceInquiry.EnableWindow(nStatus);
	m_Authorization.EnableWindow(nStatus);
	m_AdjustSale.EnableWindow(nStatus);
	m_AdjustRefund.EnableWindow(nStatus);
}

BOOL CTerminalValidTransactions::PopulateTranDialogFromDatabase ( DCF01 sTerminalRecord )
{
	if ( m_nDialogMode != MODE_VIEW && m_bTran )	
		EnableTranDialog(TRUE);

	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;
	m_StrMerchantIDFld = sTerminalRecord.merchant_id;


    if (sTerminalRecord.valid_transactions[AUTH_VOID_SALE - 1] == '1')
		m_VoidSaleOnLine.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_VOID_REFUND - 1] == '1')
		m_VoidRefundOnLine.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_TEST - 1] == '1')
		m_TestTransaction.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_STATISTICS - 1] == '1')
		m_Statistics.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SIGNATURE_DATA - 1] == '1')
		m_SignatureData.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SETTLE - 1] == '1')
		m_SettlementRequest.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SETTLE_TRAILER - 1] == '1')
		m_SettlementAfterUpload.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SALES_COMPLETION - 1] == '1')
		m_SalesCompletion.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SALE_CASH - 1] == '1')
		m_SaleAndCash.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SALE - 1] == '1')
		m_Sale.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_REVERSAL - 1] == '1')
		m_Reversal.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_REFUND - 1] == '1')
		m_Refund.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1] == '1')
		m_ReAuth.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_PRE_AUTHORIZATION - 1] == '1')
		m_PreAuthorization.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1] == '1')
		m_PleaseWaitAdvice.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_OFFLINE_SALE - 1] == '1')
		m_OffLineSale.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_OFFLINE_REFUND - 1] == '1')
		m_OffLineRefund.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_LOGON - 1] == '1')
		m_Logon.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_INITIALIZATION - 1] == '1')
		m_Initialization.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1] == '1')
		m_GuaranteedLateArrival.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_CHECK_VERIFICATION - 1] == '1')
		m_CheckVerification.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_CASH_ADVANCE - 1] == '1')
		m_Cash.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_CARD_VERIFICATION - 1] == '1')
		m_CardVerification.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_BATCH_UPLOAD - 1] == '1')
		m_BatchUpload.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1] == '1')
		m_BatchDownLineLoad.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_BALANCE_INQUIRY - 1] == '1')
		m_BalanceInquiry.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_AUTHORIZATION - 1] == '1')
		m_Authorization.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_SALE_ADJUSTMENT - 1] == '1')
		m_AdjustSale.SetCheck(1);
    if (sTerminalRecord.valid_transactions[AUTH_REFUND_ADJUSTMENT - 1] == '1')
		m_AdjustRefund.SetCheck(1);




	return TRUE;

}

void CTerminalValidTransactions::CleanTranDialog(int nInd)
{
	m_VoidSaleOnLine.SetCheck(nInd);
	m_VoidRefundOnLine.SetCheck(nInd);
	m_TestTransaction.SetCheck(nInd);
	m_Statistics.SetCheck(nInd);
	m_SignatureData.SetCheck(nInd);
	m_SettlementRequest.SetCheck(nInd);
	m_SettlementAfterUpload.SetCheck(nInd);
	m_SalesCompletion.SetCheck(nInd);
	m_SaleAndCash.SetCheck(nInd);
	m_Sale.SetCheck(nInd);
	m_Reversal.SetCheck(nInd);
	m_Refund.SetCheck(nInd);
	m_ReAuth.SetCheck(nInd);
	m_PreAuthorization.SetCheck(nInd);
	m_PleaseWaitAdvice.SetCheck(nInd);
	m_OffLineSale.SetCheck(nInd);
	m_OffLineRefund.SetCheck(nInd);
	m_Logon.SetCheck(nInd);
	m_Initialization.SetCheck(nInd);
	m_GuaranteedLateArrival.SetCheck(nInd);
	m_CheckVerification.SetCheck(nInd);
	m_Cash.SetCheck(nInd);
	m_CardVerification.SetCheck(nInd);
	m_BatchUpload.SetCheck(nInd);
	m_BatchDownLineLoad.SetCheck(nInd);
	m_BalanceInquiry.SetCheck(nInd);
	m_Authorization.SetCheck(nInd);
	m_AdjustSale.SetCheck(nInd);
	m_AdjustRefund.SetCheck(nInd);

	m_StrMessageLbl = "";

	UpdateData(FALSE);
}

BOOL  CTerminalValidTransactions::PopulateDatabaseFromTranDialog ( pDCF01 psTerminalRecord )
{
	memset (psTerminalRecord->valid_transactions, 0, sizeof (psTerminalRecord->valid_transactions));

	memset (psTerminalRecord->valid_transactions, 49, sizeof (psTerminalRecord->valid_transactions)-1);

	if (m_SalesCompletion.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SALES_COMPLETION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SALES_COMPLETION - 1] = '0';

    if (m_SaleAndCash.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SALE_CASH - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SALE_CASH - 1] = '0';

	
	if (m_VoidSaleOnLine.GetCheck() == 1)		
		psTerminalRecord->valid_transactions[AUTH_VOID_SALE - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_VOID_SALE - 1] = '0';

    if (m_VoidRefundOnLine.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_VOID_REFUND - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_VOID_REFUND - 1] = '0';

    if (m_TestTransaction.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_TEST - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_TEST - 1] = '0';


    if (m_Statistics.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_STATISTICS - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_STATISTICS - 1] = '0';

    if (m_SignatureData.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SIGNATURE_DATA - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SIGNATURE_DATA - 1] = '0';

    if (m_SettlementRequest.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SETTLE - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SETTLE - 1] = '0';


    if (m_SettlementAfterUpload.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SETTLE_TRAILER - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SETTLE_TRAILER - 1] = '0';


    if (m_Sale.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SALE - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_SALE - 1] = '0';

    if (m_Reversal.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_REVERSAL - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_REVERSAL - 1] = '0';

    if (m_Refund.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_REFUND - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_REFUND - 1] = '0';

    if (m_ReAuth.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1] = '0';

    if (m_PreAuthorization.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_PRE_AUTHORIZATION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_PRE_AUTHORIZATION - 1] = '0';

    if (m_PleaseWaitAdvice.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1] = '0';

    if (m_OffLineSale.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_OFFLINE_SALE - 1] = '1';
 	else
		psTerminalRecord->valid_transactions[AUTH_OFFLINE_SALE - 1] = '0';

    if (m_OffLineRefund.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_OFFLINE_REFUND - 1] = '1';
 	else
		psTerminalRecord->valid_transactions[AUTH_OFFLINE_REFUND - 1] = '0';

   if (m_Logon.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_LOGON - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_LOGON - 1] = '0';

   if (m_Initialization.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_INITIALIZATION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_INITIALIZATION - 1] = '0';

	if (m_GuaranteedLateArrival.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1] = '0';

	if (m_CheckVerification.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_CHECK_VERIFICATION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_CHECK_VERIFICATION - 1] = '0';

    if (m_Cash.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_CASH_ADVANCE - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_CASH_ADVANCE - 1] = '0';

    if (m_CardVerification.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_CARD_VERIFICATION - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_CARD_VERIFICATION - 1] = '0';

    if (m_BatchUpload.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_BATCH_UPLOAD - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_BATCH_UPLOAD - 1] = '0';

    if (m_BatchDownLineLoad.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1] = '0';

    if (m_BalanceInquiry.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_BALANCE_INQUIRY - 1] = '1';
	else
		psTerminalRecord->valid_transactions[AUTH_BALANCE_INQUIRY - 1] = '0';

    if (m_Authorization.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_AUTHORIZATION - 1] = '1';
 	else
		psTerminalRecord->valid_transactions[AUTH_AUTHORIZATION - 1] = '0';

   if (m_AdjustSale.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_SALE_ADJUSTMENT - 1] = '1';
  	else
		psTerminalRecord->valid_transactions[AUTH_SALE_ADJUSTMENT - 1] = '0';

	if (m_AdjustRefund.GetCheck() == 1)
		psTerminalRecord->valid_transactions[AUTH_REFUND_ADJUSTMENT - 1] = '1';
  	else
		psTerminalRecord->valid_transactions[AUTH_REFUND_ADJUSTMENT - 1] = '0';


	return TRUE;
}

BOOL CTerminalValidTransactions::CompareStructures(DCF01 sTerminalRecordNew )
{
	if ( sTerminalRecordNew.valid_transactions[AUTH_CHECK_VERIFICATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_CHECK_VERIFICATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_CHECK_VERIFICATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_CHECK_VERIFICATION - 1];
		memcpy(m_strFieldTag, "Check Verification", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_CARD_VERIFICATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_CARD_VERIFICATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_CARD_VERIFICATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_CARD_VERIFICATION - 1];
		memcpy(m_strFieldTag, "Card Verification", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_VOID_PRE_AUTHORIZATION - 1];
		memcpy(m_strFieldTag, "Re-Auth", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SALE_CASH - 1] != sTerminalRecordOld.valid_transactions[AUTH_SALE_CASH - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SALE_CASH - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SALE_CASH - 1];
		memcpy(m_strFieldTag, "Sale & Cash", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SALES_COMPLETION - 1] != sTerminalRecordOld.valid_transactions[AUTH_SALES_COMPLETION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SALES_COMPLETION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SALES_COMPLETION - 1];
		memcpy(m_strFieldTag, "Sales Completion", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1] != sTerminalRecordOld.valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_GUARANTEED_LATE_ARRIVAL - 1];
		memcpy(m_strFieldTag, "Guaranteed Late Arrival", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1] != sTerminalRecordOld.valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_BATCH_DOWN_LINE_LOAD - 1];
		memcpy(m_strFieldTag, "Batch Down Line Load", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SIGNATURE_DATA - 1] != sTerminalRecordOld.valid_transactions[AUTH_SIGNATURE_DATA - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SIGNATURE_DATA - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SIGNATURE_DATA - 1];
		memcpy(m_strFieldTag, "Signature Data", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_INITIALIZATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_INITIALIZATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_INITIALIZATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_INITIALIZATION - 1];
		memcpy(m_strFieldTag, "Initialization", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1] != sTerminalRecordOld.valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_PLEASE_WAIT_ADVICE - 1];
		memcpy(m_strFieldTag, "Please Wait Advice", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_AUTHORIZATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_AUTHORIZATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_AUTHORIZATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_AUTHORIZATION - 1];
		memcpy(m_strFieldTag, "Authorization", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}


	if ( sTerminalRecordNew.valid_transactions[AUTH_PRE_AUTHORIZATION - 1] != sTerminalRecordOld.valid_transactions[AUTH_PRE_AUTHORIZATION - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_PRE_AUTHORIZATION - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_PRE_AUTHORIZATION - 1];
		memcpy(m_strFieldTag, "Pre-Authorization", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_BALANCE_INQUIRY - 1] != sTerminalRecordOld.valid_transactions[AUTH_BALANCE_INQUIRY - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_BALANCE_INQUIRY - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_BALANCE_INQUIRY - 1];
		memcpy(m_strFieldTag, "Balance Inquiry", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SALE - 1] != sTerminalRecordOld.valid_transactions[AUTH_SALE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SALE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SALE - 1];
		memcpy(m_strFieldTag, "Sale", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_REFUND - 1] != sTerminalRecordOld.valid_transactions[AUTH_REFUND - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_REFUND - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_REFUND - 1];
		memcpy(m_strFieldTag, "Refund", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_CASH_ADVANCE - 1] != sTerminalRecordOld.valid_transactions[AUTH_CASH_ADVANCE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_CASH_ADVANCE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_CASH_ADVANCE - 1];
		memcpy(m_strFieldTag, "Cash", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_VOID_SALE - 1] != sTerminalRecordOld.valid_transactions[AUTH_VOID_SALE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_VOID_SALE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_VOID_SALE - 1];
		memcpy(m_strFieldTag, "Void, Sale, on-line", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_VOID_REFUND - 1] != sTerminalRecordOld.valid_transactions[AUTH_VOID_REFUND - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_VOID_REFUND - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_VOID_REFUND - 1];
		memcpy(m_strFieldTag, "Void Refund, on-line", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_REFUND_ADJUSTMENT - 1] != sTerminalRecordOld.valid_transactions[AUTH_REFUND_ADJUSTMENT - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SALE_ADJUSTMENT - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SALE_ADJUSTMENT - 1];
		memcpy(m_strFieldTag, "Adjust, Refund", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_REVERSAL - 1] != sTerminalRecordOld.valid_transactions[AUTH_REVERSAL - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_REVERSAL - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_REVERSAL - 1];
		memcpy(m_strFieldTag, "Reversal", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SETTLE - 1] != sTerminalRecordOld.valid_transactions[AUTH_SETTLE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SETTLE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SETTLE - 1];
		memcpy(m_strFieldTag, "Settlement Request", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SETTLE_TRAILER - 1] != sTerminalRecordOld.valid_transactions[AUTH_SETTLE_TRAILER - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SETTLE_TRAILER - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SETTLE_TRAILER - 1];
		memcpy(m_strFieldTag, "Settlement, after upload", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_BATCH_UPLOAD - 1] != sTerminalRecordOld.valid_transactions[AUTH_BATCH_UPLOAD - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_BATCH_UPLOAD - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_BATCH_UPLOAD - 1];
		memcpy(m_strFieldTag, "Batch Upload", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_STATISTICS - 1] != sTerminalRecordOld.valid_transactions[AUTH_STATISTICS - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_STATISTICS - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_STATISTICS - 1];
		memcpy(m_strFieldTag, "Statistics", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_OFFLINE_SALE - 1] != sTerminalRecordOld.valid_transactions[AUTH_OFFLINE_SALE - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_OFFLINE_SALE - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_OFFLINE_SALE - 1];
		memcpy(m_strFieldTag, "Off-line Sale", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if ( sTerminalRecordNew.valid_transactions[AUTH_OFFLINE_REFUND - 1] != sTerminalRecordOld.valid_transactions[AUTH_OFFLINE_REFUND - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_OFFLINE_REFUND - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_OFFLINE_REFUND - 1];
		memcpy(m_strFieldTag, "Off-line Refund", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_SALE_ADJUSTMENT - 1] != sTerminalRecordOld.valid_transactions[AUTH_SALE_ADJUSTMENT - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_SALE_ADJUSTMENT - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_SALE_ADJUSTMENT - 1];
		memcpy(m_strFieldTag, "Adjust, Sale", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_LOGON - 1] != sTerminalRecordOld.valid_transactions[AUTH_LOGON - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_LOGON - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_LOGON - 1];
		memcpy(m_strFieldTag, "Logon", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sTerminalRecordNew.valid_transactions[AUTH_TEST - 1] != sTerminalRecordOld.valid_transactions[AUTH_TEST - 1])
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		m_strCurrentData[0] = sTerminalRecordNew.valid_transactions[AUTH_TEST - 1];
		m_strPreviousData[0] = sTerminalRecordOld.valid_transactions[AUTH_TEST - 1];
		memcpy(m_strFieldTag, "Test Transaction", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	return TRUE;
}

void CTerminalValidTransactions::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

   memset ( strDate, 0, sizeof (strDate));
   memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrTerminalIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    Sleep(1);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"DCF01",			sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.file_primary_key,		(LPCTSTR)strPrimaryKey,		sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,	(LPCTSTR)dest_str,			sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,			(LPCTSTR)strTime,			sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data,			(LPCTSTR)m_strCurrentData,	sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,			(LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag,				(LPCTSTR)m_strFieldTag,		sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,			(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
}

BOOL CTerminalValidTransactions::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW && m_nDialogMode != MODE_DONE )
		SetModified (TRUE);	
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CTerminalValidTransactions::OnSetActive() 
{
   if ( m_nDialogMode == MODE_ADD && m_bOpen )
   {
      CleanTranDialog (1);
      m_bOpen = FALSE;
   }
	return CPropertyPage::OnSetActive();
}
