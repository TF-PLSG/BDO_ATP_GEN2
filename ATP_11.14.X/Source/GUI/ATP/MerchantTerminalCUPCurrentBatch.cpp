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
*
************************************************************************************/
// MerchantTerminalCUPCurrentBatch.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalCUPCurrentBatch.h"
#include "BinList.h"
#include "MyUtils.h"
#include "SharedMem.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* Flag used to set batch number to null.
 *    0 = Leave as is
 *    1 = Set to Null
 */
extern INT  Reset_Batch_Nbr;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalCUPCurrentBatch property page

IMPLEMENT_DYNCREATE(CMerchantTerminalCUPCurrentBatch, CPropertyPage)

CMerchantTerminalCUPCurrentBatch::CMerchantTerminalCUPCurrentBatch() : CPropertyPage(CMerchantTerminalCUPCurrentBatch::IDD)
{
	//{{AFX_DATA_INIT(CMerchantTerminalCUPCurrentBatch)
	m_StrBatchTotalAmountFld = _T("");
	m_StrBatchTotalCountFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrCreditRefundsAmountFld = _T("");
	m_StrCreditRefundsCountFld = _T("");
	m_StrCreditSalesAmountFld = _T("");
	m_StrCreditSalesCountFld = _T("");
	m_StrDebitrefundsAmountFld = _T("");
	m_StrDebitRefundsCountFld = _T("");
	m_StrDebitSalesAmountFld = _T("");
	m_StrDebitSalesCountFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrTerminalIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalCUPCurrentBatch::~CMerchantTerminalCUPCurrentBatch()
{
}

void CMerchantTerminalCUPCurrentBatch::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalCUPCurrentBatch)
	DDX_Control(pDX, IDC_RESET_BATCH_NBR, m_ResetBatchNbr);
	DDX_Control(pDX, IDC_DETAILS_BUTTON, m_DetailsButton);
	DDX_Control(pDX, IDC_CLOSE_BUTTON, m_CloseButton);
	DDX_Text(pDX, IDC_BATCH_TOTAL_AMOUNT_FLD, m_StrBatchTotalAmountFld);
	DDX_Text(pDX, IDC_BATCH_TOTAL_COUNT_FLD, m_StrBatchTotalCountFld);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_CREDIT_REFUNDS_AMOUNT_FLD, m_StrCreditRefundsAmountFld);
	DDX_Text(pDX, IDC_CREDIT_REFUNDS_COUNT_FLD, m_StrCreditRefundsCountFld);
	DDX_Text(pDX, IDC_CREDIT_SALES_AMOUNT_FLD, m_StrCreditSalesAmountFld);
	DDX_Text(pDX, IDC_CREDIT_SALES_COUNT_FLD, m_StrCreditSalesCountFld);
	DDX_Text(pDX, IDC_DEBIT_REFUNDS_AMOUNT_FLD, m_StrDebitrefundsAmountFld);
	DDX_Text(pDX, IDC_DEBIT_REFUNDS_COUNT_FLD, m_StrDebitRefundsCountFld);
	DDX_Text(pDX, IDC_DEBIT_SALES_AMOUNT_FLD, m_StrDebitSalesAmountFld);
	DDX_Text(pDX, IDC_DEBIT_SALES_COUNT_FLD, m_StrDebitSalesCountFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_CUP_ACCEPTOR_ID, m_StrCupAccIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalCUPCurrentBatch, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalCUPCurrentBatch)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnDetailsButton)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	ON_BN_CLICKED(IDC_RESET_BATCH_NBR, OnResetBatchNbr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalCUPCurrentBatch message handlers

BOOL CMerchantTerminalCUPCurrentBatch::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_StrMessageLbl = _T("");
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bCurrentBatch = TRUE;

	if (m_nDialogMode != MODE_DONE && m_nDialogMode != MODE_ADD)
	{		
		PopulateCurrentBatchDialogFromDatabase ( sTerminalRecord );
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMerchantTerminalCUPCurrentBatch::OnDetailsButton() 
{
	CBinList binListdlg;
	CString temp;
	m_StrMessageLbl = _T("");

   UpdateData(FALSE);

	strcpy((char *)binListdlg.sBatchRecord.primary_key.device_id, m_StrTerminalIDFld);
	strcpy((char *)binListdlg.sBatchRecord.merchant_id, m_StrMerchantIDFld);
	strcpy((char *)binListdlg.sBatchRecord.primary_key.batch_nbr, "ACTIVE");
	strcpy((char *)binListdlg.sBatchRecord.open_date, (char *)sBatchRecord.open_date);
	strcpy((char *)binListdlg.sBatchRecord.open_time, (char *)sBatchRecord.open_time);
	strcpy((char *)binListdlg.m_strBatchStatus, "C");

	binListdlg.m_nDialogMode = MODE_BATCH;
   binListdlg.m_nViewMode = m_nDialogMode;

	int nReturnCode =binListdlg.DoModal();

	CleanCurrentBatchDialog();
	PopulateCurrentBatchDialogFromDatabase ( sTerminalRecord );	

	
}

BOOL CMerchantTerminalCUPCurrentBatch::PopulateCurrentBatchDialogFromDatabase (DCF01 sTerminalRecord  )
{
	CString temp1, temp2, temp3, temp4, temp;
	double res;
	int test;
	char strSum [50];
	CMyUtils myUtilsObject;
	char strErrorMessage[200] = "";
	char test1[50];
	MCF01 sMerchantRecord;
	CString	m_StrCUPACCID;


 //  memset(&sBatchRecord, 0, sizeof(sBatchRecord));
	m_StrMerchantIDFld = sTerminalRecord.merchant_id;
	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;

	UpdateData(FALSE);

	//	ZeroMemory ( &sBatchRecord, sizeof ( DCF01) );
	memcpy ( sBatchRecord.merchant_id, m_StrMerchantIDFld, sizeof (sBatchRecord.merchant_id));
	memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

   int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
   if(rcode == PTEMSG_OK)
   {
		//Check if its having CUP Acceptor ID or Not

		int CUPACPL1 = strlen((pCHAR)sMerchantRecord.network[3].priority_routing);
		
		if(CUPACPL1 > 1)
		{
		   // CUP Accptor ID
			m_StrCUPACCID = sMerchantRecord.network[3].priority_routing;
//			m_StrCUPACCID += sMerchantRecord.network[4].priority_routing;
			m_StrCUPACCID +=(CString)((char *) sMerchantRecord.network[4].priority_routing);
			m_StrCupAccIDFld = m_StrCUPACCID;
			memcpy ( sBatchRecord.merchant_id, m_StrCUPACCID, sizeof (sBatchRecord.merchant_id));
			rcode = txdsapi_get_record_by_subtype2 ( (pBYTE)&sBatchRecord, sizeof (BCH01), BCH01_DATA, ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS, strErrorMessage);           
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
					case PTEMSG_NOT_FOUND:
							 m_StrMessageLbl = "ERROR: Current batch record for this terminal does not exist";
							 m_DetailsButton.EnableWindow(FALSE);
							 m_CloseButton.EnableWindow(FALSE);
							break;
      
					   default:
						   m_StrMessageLbl = strErrorMessage;
						   m_nDialogMode = MODE_DONE;
						if (!UpdateData(FALSE)){
							AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
						}

					return FALSE;
				}	
			}
			else
			{
				if ( m_nDialogMode != MODE_VIEW )
				 m_CloseButton.EnableWindow(TRUE);
				temp1 = sBatchRecord.total_credit_refund_amt;
				temp2 = sBatchRecord.total_credit_amt;
				temp3 = sBatchRecord.total_debit_refund_amt;
				temp4 = sBatchRecord.total_debit_amt;

				res = atof(temp2) + atof(temp4) - atof(temp1) - atof(temp3);
				gcvt ( res, 15, strSum );
				temp = strSum;
					m_StrBatchTotalAmountFld = myUtilsObject.DisplayAmount(temp);

				res = atof(temp1);
				gcvt ( res, 15, strSum );
				temp = strSum;
					m_StrCreditRefundsAmountFld = myUtilsObject.DisplayAmount(temp);

		 
				res = atof(temp2);
				gcvt ( res, 15, strSum );
				temp = strSum;
					m_StrCreditSalesAmountFld = myUtilsObject.DisplayAmount(temp);

				res = atof(temp3);
				gcvt ( res, 15, strSum );
				temp = strSum;
					m_StrDebitrefundsAmountFld = myUtilsObject.DisplayAmount(temp);

				res = atof(temp4);
				gcvt ( res, 15, strSum );
				temp = strSum;
					m_StrDebitSalesAmountFld = myUtilsObject.DisplayAmount(temp);

				temp1 = sBatchRecord.total_credit_refund_cnt;
				temp2 = sBatchRecord.total_credit_cnt;
				temp3 = sBatchRecord.total_debit_refund_cnt;
				temp4 = sBatchRecord.total_debit_cnt;

				int res1 = atoi(temp1) + atoi(temp2) + atoi(temp3) + atoi(temp4);
				itoa ( res1, strSum ,10 );
				m_StrBatchTotalCountFld = strSum;

				test = atoi((char *) sBatchRecord.total_credit_cnt);
				itoa(test,test1,10);
				m_StrCreditSalesCountFld = test1;

				test = atoi((char *) sBatchRecord.total_credit_refund_cnt);
				itoa(test,test1,10);
				m_StrCreditRefundsCountFld = test1;

				test = atoi((char *) sBatchRecord.total_debit_cnt);
				itoa(test,test1,10);
				m_StrDebitSalesCountFld = test1;

				test = atoi((char *) sBatchRecord.total_debit_refund_cnt);
				itoa(test,test1,10);
				m_StrDebitRefundsCountFld = test1;
				
				if ( res1 == 0 )
				 m_CloseButton.EnableWindow(FALSE);
			}
		}
		else
		{
			m_StrMessageLbl = "Merchant is not Link with A CUP Acceptor ID";
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
		}
		
		UpdateData(FALSE);
	}
	else
	{
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}

	return TRUE;

}

void CMerchantTerminalCUPCurrentBatch::CleanCurrentBatchDialog()
{
	m_StrBatchTotalAmountFld = _T("");
	m_StrBatchTotalCountFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrCreditRefundsAmountFld = _T("");
	m_StrCreditRefundsCountFld = _T("");
	m_StrCreditSalesAmountFld = _T("");
	m_StrCreditSalesCountFld = _T("");
	m_StrDebitrefundsAmountFld = _T("");
	m_StrDebitRefundsCountFld = _T("");
	m_StrDebitSalesAmountFld = _T("");
	m_StrDebitSalesCountFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrCupAccIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrTerminalIDFld = _T("");

	if ( m_bCurrentBatch)
	UpdateData (FALSE);
}

void CMerchantTerminalCUPCurrentBatch::OnCloseButton() 
{
	char strErrorMessage[200] = "";
   CString strMessage;
   MCF01 sMerchantRecord;
   CString	m_StrCUPACCID;
   m_StrMessageLbl = _T("");
	strMessage.Format ("Are you sure you want to close a batch?") ;

	int nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	memcpy ( sBatchRecord.primary_key.device_id, m_StrTerminalIDFld, sizeof (sBatchRecord.primary_key.device_id));
    memcpy ( sBatchRecord.merchant_id, m_StrMerchantIDFld, sizeof (sBatchRecord.merchant_id));
	memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK)
    {
		//Check if its having CUP Acceptor ID or Not

		int CUPACPL1 = strlen((pCHAR)sMerchantRecord.network[3].priority_routing);
		
		if(CUPACPL1 > 1)
		{
		   // CUP Accptor ID
			m_StrCUPACCID = sMerchantRecord.network[3].priority_routing;
		//	m_StrCUPACCID += sMerchantRecord.network[4].priority_routing;
			m_StrCUPACCID += (CString)((char *)sMerchantRecord.network[4].priority_routing);
			memcpy ( sBatchRecord.visa_merchant_id, m_StrCUPACCID, sizeof (sBatchRecord.merchant_id));

			rcode = txdsapi_update_bch01_record ( (pBYTE)&sBatchRecord, sizeof (BCH01), BCH01_DATA, strErrorMessage);        
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
					 case PTEMSG_NOT_FOUND:
						m_StrMessageLbl = "ERROR: Current batch record for this terminal does not exist";
					  break;
      
					 default:
						m_StrMessageLbl = strErrorMessage;
							m_nDialogMode = MODE_DONE;
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}

					return ;
				}
			}
			else if ( PTEMSG_OK == rcode )
			{
			   m_StrBatchTotalAmountFld = _T("");
			   m_StrBatchTotalCountFld = _T("");
			   m_StrCreditRefundsAmountFld = _T("");
			   m_StrCreditRefundsCountFld = _T("");
			   m_StrCreditSalesAmountFld = _T("");
			   m_StrCreditSalesCountFld = _T("");
			   m_StrDebitrefundsAmountFld = _T("");
			   m_StrDebitRefundsCountFld = _T("");
			   m_StrDebitSalesAmountFld = _T("");
			   m_StrDebitSalesCountFld = _T("");
			   m_StrMessageLbl = "The batch closed";
				m_CloseButton.EnableWindow(FALSE);

			   UpdateData (FALSE);

		   }
		}
   }

	
/*	rcode = txdsapi_update_bch10_record ( (pBYTE)&sBatchRecord, sizeof (BCH10), BCH10_DATA, strErrorMessage);        
	if ( rcode != PTEMSG_OK )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			m_StrMessageLbl = "ERROR: Batch records for this terminal do not exist";
           break;
      
		 default:
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			return ;
		}
	}*/
	
}

BOOL CMerchantTerminalCUPCurrentBatch::OnSetActive() 
{
   CMyUtils myUtilsObject;  
   m_StrMessageLbl = _T("");

   if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW )
   {
      m_DetailsButton.EnableWindow(TRUE);
	  if (( m_StrBatchTotalCountFld.GetLength() == 0 ) && m_nDialogMode == MODE_VIEW )
	    m_DetailsButton.EnableWindow(FALSE);
   }
   else
   {
      m_DetailsButton.EnableWindow(FALSE);
   }

   if (   m_nDialogMode == MODE_EDIT 
      &&  m_StrBatchTotalCountFld != "0" 
      &&  m_StrBatchTotalCountFld.GetLength() != 0 )

      m_CloseButton.EnableWindow(TRUE);
   else 
      m_CloseButton.EnableWindow(FALSE);
	
	return CPropertyPage::OnSetActive();
}

void CMerchantTerminalCUPCurrentBatch::OnResetBatchNbr()
{
   /* The purpose of this function is to set the batch number field
    * to null.  When customers reprogram terminals, they end up getting
    * lots of duplicate batches reported during settlement.  This will
    * make it easy for them to work around this problem.
    */
   Reset_Batch_Nbr = 1;
   m_StrMessageLbl = "Batch Number will be set to Null when APPLY or OK is selected";
   UpdateData( FALSE );
   return;
}


BOOL CMerchantTerminalCUPCurrentBatch::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
