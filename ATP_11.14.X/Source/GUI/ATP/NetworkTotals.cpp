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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkTotals.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:10   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:50   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:26   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:26   iarustam
    
*
************************************************************************************/
// NetworkTotals.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkTotals.h"
#include "MyUtils.h"
#include "SharedMem.h"
extern "C"
{
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
// CNetworkTotals property page

IMPLEMENT_DYNCREATE(CNetworkTotals, CPropertyPage)

CNetworkTotals::CNetworkTotals() : CPropertyPage(CNetworkTotals::IDD)
{
	//{{AFX_DATA_INIT(CNetworkTotals)
	m_StrCreditNumberFld = _T("");
	m_StrDebitNumberFld = _T("");
	m_StrCreditReversalNumberFld = _T("");
	m_StrDebitReversalNumberFld = _T("");
	m_StrCreditProcessingFeeAmountFld = _T("");
	m_StrDebitProcessingFeeAmountFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrCreditTransactionFeeAmountFld = _T("");
	m_StrDebitTransactionFeeAmountFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrCreditAmountOfTransactionsFld = _T("");
	m_StrDebitAmountOfTransactionsFld = _T("");
	m_StrDebitAmountOfReversalTransactionsFld = _T("");
	m_StrCreditAmountOfReversalTransactionsFld = _T("");
	m_StrInquiryNumberFld = _T("");
	m_StrAuthorizationNumberFld = _T("");
	m_StrTransferReversalNumberFld = _T("");
	m_StrTransferNumberFld = _T("");
	m_StrNetSettlementAmountFld = _T("");
	//}}AFX_DATA_INIT
}

CNetworkTotals::~CNetworkTotals()
{
}

void CNetworkTotals::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkTotals)
	DDX_Control(pDX, IDC_NET_SETTLMENT_AMOUNT_FLD, m_CntlNetSettlementAmountFld);
	DDX_Control(pDX, IDC_TRANSFER_NUMBER_FLD, m_CntlTransferNumberFld);
	DDX_Control(pDX, IDC_TRANSFER_REVERSAL_NUMBER_FLD, m_CntlTransferReversalNumberFld);
	DDX_Control(pDX, IDC_AUTHORIZATION_NUMBER_FLD, m_CntlAuthorizationNumberFld);
	DDX_Control(pDX, IDC_INQUIRY_NUMBER_FLD, m_CntlInquiryNumberFld);
	DDX_Control(pDX, IDC_CREDIT_TOTAL_AMOUNT_OF_REVERSAL_TRANSACTIONS_FLD, m_CntlCreditAmountOfReversalTransactionsFld);
	DDX_Control(pDX, IDC_DEBIT_TOTAL_AMOUNT_OF_REVERSAL_TRANSACTIONS_FLD, m_CntlDebitAmountOfReversalTransactionsFld);
	DDX_Control(pDX, IDC_DEBIT_TOTAL_AMOUNT_OF_TRANSACTIONS_FLD, m_CntlDebitAmountOfTransactionsFld);
	DDX_Control(pDX, IDC_CREDIT_TOTAL_AMOUNT_OF_TRANSACTIONS_FLD, m_CntlCreditAmountOfTransactionsFld);
	DDX_Control(pDX, IDC_DEBIT_TRANSACTION_FEE_AMOUNT_FLD, m_CntlDebitTransactionFeeAmountFld);
	DDX_Control(pDX, IDC_CREDIT_TRANSACTION_FEE_AMOUNT_FLD, m_CntlCreditTransactionFeeAmountFld);
	DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Control(pDX, IDC_NETWORK_ID_FLD, m_CntlNetworkIdFld);
	DDX_Control(pDX, IDC_DEBIT_PROCESSING_FEE_AMOUNT_FLD, m_CntlDebitProcessingFeeAmountFld);
	DDX_Control(pDX, IDC_CREDIT_PROCESSING_FEE_AMOUNT_FLD, m_CntlCreditProcessingFeeAmountFld);
	DDX_Control(pDX, IDC_DEBIT_NUMBER_OF_REVERSAL_TRANSACTIONS_FLD, m_CntlDebitReversalNumberFld);
	DDX_Control(pDX, IDC_CREDIT_NUMBER_OF_REVERSAL_TRANSACTIONS_FLD, m_CntlCreditReversalNumberFld);
	DDX_Control(pDX, IDC_DEBIT_NUMBER_OF_TRANSACTIONS_FLD, m_CntlDebitNumberFld);
	DDX_Control(pDX, IDC_CREDIT_NUMBER_OF_TRANSACTIONS_FLD, m_CntlCreditNumberFld);
	DDX_Text(pDX, IDC_CREDIT_NUMBER_OF_TRANSACTIONS_FLD, m_StrCreditNumberFld);
	DDV_MaxChars(pDX, m_StrCreditNumberFld, 4);
	DDX_Text(pDX, IDC_DEBIT_NUMBER_OF_TRANSACTIONS_FLD, m_StrDebitNumberFld);
	DDV_MaxChars(pDX, m_StrDebitNumberFld, 4);
	DDX_Text(pDX, IDC_CREDIT_NUMBER_OF_REVERSAL_TRANSACTIONS_FLD, m_StrCreditReversalNumberFld);
	DDV_MaxChars(pDX, m_StrCreditReversalNumberFld, 4);
	DDX_Text(pDX, IDC_DEBIT_NUMBER_OF_REVERSAL_TRANSACTIONS_FLD, m_StrDebitReversalNumberFld);
	DDV_MaxChars(pDX, m_StrDebitReversalNumberFld, 4);
	DDX_Text(pDX, IDC_CREDIT_PROCESSING_FEE_AMOUNT_FLD, m_StrCreditProcessingFeeAmountFld);
	DDV_MaxChars(pDX, m_StrCreditProcessingFeeAmountFld, 10);
	DDX_Text(pDX, IDC_DEBIT_PROCESSING_FEE_AMOUNT_FLD, m_StrDebitProcessingFeeAmountFld);
	DDV_MaxChars(pDX, m_StrDebitProcessingFeeAmountFld, 10);
	DDX_Text(pDX, IDC_NETWORK_ID_FLD, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);
	DDX_Text(pDX, IDC_CREDIT_TRANSACTION_FEE_AMOUNT_FLD, m_StrCreditTransactionFeeAmountFld);
	DDV_MaxChars(pDX, m_StrCreditTransactionFeeAmountFld, 10);
	DDX_Text(pDX, IDC_DEBIT_TRANSACTION_FEE_AMOUNT_FLD, m_StrDebitTransactionFeeAmountFld);
	DDV_MaxChars(pDX, m_StrDebitTransactionFeeAmountFld, 10);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CREDIT_TOTAL_AMOUNT_OF_TRANSACTIONS_FLD, m_StrCreditAmountOfTransactionsFld);
	DDV_MaxChars(pDX, m_StrCreditAmountOfTransactionsFld, 10);
	DDX_Text(pDX, IDC_DEBIT_TOTAL_AMOUNT_OF_TRANSACTIONS_FLD, m_StrDebitAmountOfTransactionsFld);
	DDV_MaxChars(pDX, m_StrDebitAmountOfTransactionsFld, 10);
	DDX_Text(pDX, IDC_DEBIT_TOTAL_AMOUNT_OF_REVERSAL_TRANSACTIONS_FLD, m_StrDebitAmountOfReversalTransactionsFld);
	DDV_MaxChars(pDX, m_StrDebitAmountOfReversalTransactionsFld, 10);
	DDX_Text(pDX, IDC_CREDIT_TOTAL_AMOUNT_OF_REVERSAL_TRANSACTIONS_FLD, m_StrCreditAmountOfReversalTransactionsFld);
	DDV_MaxChars(pDX, m_StrCreditAmountOfReversalTransactionsFld, 10);
	DDX_Text(pDX, IDC_INQUIRY_NUMBER_FLD, m_StrInquiryNumberFld);
	DDV_MaxChars(pDX, m_StrInquiryNumberFld, 4);
	DDX_Text(pDX, IDC_AUTHORIZATION_NUMBER_FLD, m_StrAuthorizationNumberFld);
	DDV_MaxChars(pDX, m_StrAuthorizationNumberFld, 4);
	DDX_Text(pDX, IDC_TRANSFER_REVERSAL_NUMBER_FLD, m_StrTransferReversalNumberFld);
	DDV_MaxChars(pDX, m_StrTransferReversalNumberFld, 4);
	DDX_Text(pDX, IDC_TRANSFER_NUMBER_FLD, m_StrTransferNumberFld);
	DDV_MaxChars(pDX, m_StrTransferNumberFld, 4);
	DDX_Text(pDX, IDC_NET_SETTLMENT_AMOUNT_FLD, m_StrNetSettlementAmountFld);
	DDV_MaxChars(pDX, m_StrNetSettlementAmountFld, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkTotals, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkTotals)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkTotals message handlers
/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkTotals::OnInitDialog() 
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

	m_bTotals = TRUE;

	if (m_nDialogMode != MODE_DONE )
    {		
        EnableTotalsDialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
            PopulateTotalsDialogFromDatabase (sNetworkOld);
    } 

	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkTotals::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

/*BOOL CNetworkTotals::PopulateDatabaseFromTotalsDialog( pNCF02 psNetwork )
{
	CMyUtils myUtilsObject;

	m_StrCreditProcessingFeeAmountFld = myUtilsObject.GenerateAmount(m_StrCreditProcessingFeeAmountFld, 12);
	m_StrDebitProcessingFeeAmountFld = myUtilsObject.GenerateAmount(m_StrDebitProcessingFeeAmountFld, 12); 
	m_StrNetSettlementAmountFld = myUtilsObject.GenerateAmount(m_StrNetSettlementAmountFld, 12); 
	m_StrCreditTransactionFeeAmountFld = myUtilsObject.GenerateAmount(m_StrCreditTransactionFeeAmountFld, 12);
	m_StrDebitTransactionFeeAmountFld = myUtilsObject.GenerateAmount(m_StrDebitTransactionFeeAmountFld, 12);
	m_StrCreditAmountOfTransactionsFld = myUtilsObject.GenerateAmount(m_StrCreditAmountOfTransactionsFld, 12);
	m_StrDebitAmountOfTransactionsFld = myUtilsObject.GenerateAmount(m_StrDebitAmountOfTransactionsFld, 12);
	m_StrDebitAmountOfReversalTransactionsFld = myUtilsObject.GenerateAmount(m_StrDebitAmountOfReversalTransactionsFld, 12);
	m_StrCreditAmountOfReversalTransactionsFld = myUtilsObject.GenerateAmount(m_StrCreditAmountOfReversalTransactionsFld, 12);

	memcpy (psNetwork->credit_proc_fee_amt,    m_StrCreditProcessingFeeAmountFld ,        sizeof( psNetwork->credit_proc_fee_amt));
	memcpy (psNetwork->debit_proc_fee_amt,     m_StrDebitProcessingFeeAmountFld ,         sizeof( psNetwork->debit_proc_fee_amt));
	memcpy (psNetwork->net_settlement_amt,     m_StrNetSettlementAmountFld ,              sizeof( psNetwork->net_settlement_amt));
	memcpy (psNetwork->credit_tran_fee_amt,    m_StrCreditTransactionFeeAmountFld ,       sizeof( psNetwork->credit_tran_fee_amt));
	memcpy (psNetwork->debit_tran_fee_amt,     m_StrDebitTransactionFeeAmountFld,         sizeof( psNetwork->debit_tran_fee_amt));
	memcpy (psNetwork->credit_amt,             m_StrCreditAmountOfTransactionsFld ,       sizeof( psNetwork->credit_amt));
	memcpy (psNetwork->debit_amt,              m_StrDebitAmountOfTransactionsFld  ,       sizeof( psNetwork->debit_amt));
	memcpy (psNetwork->debit_reversal_amt,     m_StrDebitAmountOfReversalTransactionsFld ,sizeof( psNetwork->debit_reversal_amt));
	memcpy (psNetwork->credit_reversal_amt,    m_StrCreditAmountOfReversalTransactionsFld,sizeof( psNetwork->credit_reversal_amt));
	memcpy (psNetwork->credit_cnt,             m_StrCreditNumberFld ,                     sizeof( psNetwork->credit_cnt));
	memcpy (psNetwork->debit_cnt,              m_StrDebitNumberFld ,                      sizeof( psNetwork->debit_cnt));
	memcpy (psNetwork->credit_reversal_cnt,    m_StrCreditReversalNumberFld,              sizeof( psNetwork->credit_reversal_cnt));
	memcpy (psNetwork->debit_reversal_cnt,     m_StrDebitReversalNumberFld,               sizeof( psNetwork->debit_reversal_cnt));
	memcpy (psNetwork->inquiries_cnt,          m_StrInquiryNumberFld ,                    sizeof( psNetwork->inquiries_cnt));
	memcpy (psNetwork->authorizations_cnt,     m_StrAuthorizationNumberFld ,              sizeof( psNetwork->authorizations_cnt));
    memcpy (psNetwork->transfer_reversal_cnt,  m_StrTransferReversalNumberFld ,           sizeof( psNetwork->transfer_reversal_cnt));
	memcpy (psNetwork->transfer_cnt,           m_StrTransferNumberFld ,                   sizeof( psNetwork->transfer_cnt));
					
	return TRUE;
}*/

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkTotals::PopulateTotalsDialogFromDatabase ( NCF02 sNetwork)
{
	CMyUtils myUtilsObject;

    if ( m_nDialogMode != MODE_VIEW )
    EnableTotalsDialog(TRUE);
  
    ZeroMemory ( &sNetworkOld, sizeof (NCF02));  

	m_StrNetworkIdFld = sNetwork.primary_key.network_id;
	m_StrNetworkTypeFld = sNetwork.primary_key.network_type;

	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";
    
    m_StrCreditNumberFld                       = sNetwork.credit_cnt;
	m_StrDebitNumberFld                        = sNetwork.debit_cnt;
	m_StrCreditReversalNumberFld               = sNetwork.credit_reversal_cnt;
	m_StrDebitReversalNumberFld                = sNetwork.debit_reversal_cnt;
	m_StrCreditProcessingFeeAmountFld          = sNetwork.credit_proc_fee_amt;
	m_StrDebitProcessingFeeAmountFld           = sNetwork.debit_proc_fee_amt;
	m_StrCreditTransactionFeeAmountFld         = sNetwork.credit_tran_fee_amt;
	m_StrDebitTransactionFeeAmountFld          = sNetwork.debit_tran_fee_amt;
	m_StrCreditAmountOfTransactionsFld         = sNetwork.credit_amt;
	m_StrDebitAmountOfTransactionsFld          = sNetwork.debit_amt;
	m_StrDebitAmountOfReversalTransactionsFld  = sNetwork.debit_reversal_amt;
	m_StrCreditAmountOfReversalTransactionsFld = sNetwork.credit_reversal_amt;
	m_StrInquiryNumberFld                      = sNetwork.inquiries_cnt;
	m_StrAuthorizationNumberFld                = sNetwork.authorizations_cnt;
	m_StrTransferReversalNumberFld             = sNetwork.transfer_reversal_cnt;
	m_StrTransferNumberFld                     = sNetwork.transfer_cnt;
	m_StrNetSettlementAmountFld                = sNetwork.net_settlement_amt;
	
	m_StrCreditProcessingFeeAmountFld = myUtilsObject.GetAmount(m_StrCreditProcessingFeeAmountFld);
	m_StrDebitProcessingFeeAmountFld = myUtilsObject.GetAmount(m_StrDebitProcessingFeeAmountFld); 
	m_StrNetSettlementAmountFld = myUtilsObject.GetAmount(m_StrNetSettlementAmountFld); 
	m_StrCreditTransactionFeeAmountFld = myUtilsObject.GetAmount(m_StrCreditTransactionFeeAmountFld);
	m_StrDebitTransactionFeeAmountFld = myUtilsObject.GetAmount(m_StrDebitTransactionFeeAmountFld);
	m_StrCreditAmountOfTransactionsFld = myUtilsObject.GetAmount(m_StrCreditAmountOfTransactionsFld);
	m_StrDebitAmountOfTransactionsFld = myUtilsObject.GetAmount(m_StrDebitAmountOfTransactionsFld);
	m_StrDebitAmountOfReversalTransactionsFld = myUtilsObject.GetAmount(m_StrDebitAmountOfReversalTransactionsFld);
	m_StrCreditAmountOfReversalTransactionsFld = myUtilsObject.GetAmount(m_StrCreditAmountOfReversalTransactionsFld);
     
    UpdateData(FALSE);

    memcpy ( &sNetworkOld, &sNetwork, sizeof ( NCF02));
	
	return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/	
								
BOOL CNetworkTotals::CompareStructures()
{
    if (strcmp ( m_StrCreditNumberFld, (char *)sNetworkOld.credit_cnt) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCreditNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Credit Transactions", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrDebitNumberFld , (char *)sNetworkOld.debit_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitNumberFld , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Debit Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
    if (strcmp ( m_StrCreditReversalNumberFld, (char *)sNetworkOld.credit_reversal_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCreditReversalNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_reversal_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Credit Reversal Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrDebitReversalNumberFld, (char *)sNetworkOld.debit_reversal_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitReversalNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_reversal_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Debit Reversal Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrCreditProcessingFeeAmountFld, (char *)sNetworkOld.credit_proc_fee_amt) != 0)
    { 
        memcpy(m_StrCurrentData, m_StrCreditProcessingFeeAmountFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_proc_fee_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Credit Processing Fee Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrDebitProcessingFeeAmountFld, (char *)sNetworkOld.debit_proc_fee_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitProcessingFeeAmountFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_proc_fee_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Debit Processing Fee Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrNetSettlementAmountFld, (char *)sNetworkOld.net_settlement_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrNetSettlementAmountFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.net_settlement_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Net Settlement Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCreditTransactionFeeAmountFld, (char *)sNetworkOld.credit_tran_fee_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCreditTransactionFeeAmountFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_tran_fee_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Credit Tran Fee Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrDebitTransactionFeeAmountFld , (char *)sNetworkOld.debit_tran_fee_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitTransactionFeeAmountFld , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_tran_fee_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Debit Tran Fee Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCreditAmountOfTransactionsFld, (char *)sNetworkOld.credit_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCreditAmountOfTransactionsFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Total Amount Of Credit Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrDebitAmountOfTransactionsFld, (char *)sNetworkOld.debit_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitAmountOfTransactionsFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Total Amount Of Debit Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrDebitAmountOfReversalTransactionsFld, (char *)sNetworkOld.debit_reversal_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrDebitAmountOfReversalTransactionsFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.debit_reversal_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Total Amt Reversal Debit Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCreditAmountOfReversalTransactionsFld, (char *)sNetworkOld.credit_reversal_amt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCreditAmountOfReversalTransactionsFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.credit_reversal_amt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Total Amount Of Credit Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrInquiryNumberFld, (char *)sNetworkOld.inquiries_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrInquiryNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.inquiries_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Inquiry Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp (m_StrAuthorizationNumberFld, (char *)sNetworkOld.authorizations_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrAuthorizationNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.authorizations_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Auth Transactions", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrTransferReversalNumberFld, (char *)sNetworkOld.transfer_reversal_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrTransferReversalNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.transfer_reversal_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Transfer Reversal Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrTransferNumberFld, (char *)sNetworkOld.transfer_cnt) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrTransferNumberFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkOld.transfer_cnt, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Number Of Transfer Tran", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }     
        return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkTotals::CleanTotalsDialog()
{
	m_StrCreditNumberFld = _T("");
	m_StrDebitNumberFld = _T("");
	m_StrCreditReversalNumberFld = _T("");
	m_StrDebitReversalNumberFld = _T("");
	m_StrCreditProcessingFeeAmountFld = _T("");
	m_StrDebitProcessingFeeAmountFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrCreditTransactionFeeAmountFld = _T("");
	m_StrDebitTransactionFeeAmountFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrCreditAmountOfTransactionsFld = _T("");
	m_StrDebitAmountOfTransactionsFld = _T("");
	m_StrDebitAmountOfReversalTransactionsFld = _T("");
	m_StrCreditAmountOfReversalTransactionsFld = _T("");
	m_StrInquiryNumberFld = _T("");
	m_StrAuthorizationNumberFld = _T("");
	m_StrTransferReversalNumberFld = _T("");
	m_StrTransferNumberFld = _T("");
	m_StrNetSettlementAmountFld = _T("");
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkTotals::EnableTotalsDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {	
		m_CntlCreditNumberFld.EnableWindow(nStatus);	
		m_CntlDebitNumberFld.EnableWindow(nStatus);	
		m_CntlCreditReversalNumberFld.EnableWindow(nStatus);	
		m_CntlDebitReversalNumberFld.EnableWindow(nStatus);	
		m_CntlCreditProcessingFeeAmountFld.EnableWindow(nStatus);	
		m_CntlDebitProcessingFeeAmountFld.EnableWindow(nStatus);	
		m_CntlNetSettlementAmountFld.EnableWindow(nStatus);	
		m_CntlCreditTransactionFeeAmountFld.EnableWindow(nStatus);	
		m_CntlDebitTransactionFeeAmountFld.EnableWindow(nStatus);	
		m_CntlCreditAmountOfTransactionsFld.EnableWindow(nStatus);	
		m_CntlDebitAmountOfTransactionsFld.EnableWindow(nStatus);	
		m_CntlDebitAmountOfReversalTransactionsFld.EnableWindow(nStatus);	
		m_CntlCreditAmountOfReversalTransactionsFld.EnableWindow(nStatus);	
		m_CntlInquiryNumberFld.EnableWindow(nStatus);	
		m_CntlAuthorizationNumberFld.EnableWindow(nStatus);	
		m_CntlTransferReversalNumberFld.EnableWindow(nStatus);	
		m_CntlTransferNumberFld.EnableWindow(nStatus);
    }
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkTotals::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNetworkIdFld);
    strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,         (LPCTSTR)"NCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_StrCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_StrPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_StrFieldTag, sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}
