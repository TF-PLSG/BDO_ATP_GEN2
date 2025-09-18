/*********************************************************************************

************************************************************************************/
// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "TranSearch.h"
#include "Fraud_Txn_Details.h"
#include "transactiondetails.h"
#include "MyUtils.h"
#include "GenUtil.h"
#include "TranInquiry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE Transactionid[21];
extern BYTE Handler_queue[7];

extern BOOL m_bTran;
extern BOOL m_bTran_stip;
extern BOOL m_bFGuard;

//CTransactionDetails *pTransactionDetails;

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails dialog

IMPLEMENT_DYNCREATE(CFraudTransactionDetails, CPropertyPage)
CFraudTransactionDetails::CFraudTransactionDetails() : CPropertyPage(CFraudTransactionDetails::IDD)
{
	//{{AFX_DATA_INIT(CTransactionDetails)

	//}}AFX_DATA_INIT
	CString m_Strtran_id               =_T("");  
	CString m_Strsys_date              =_T("");  
	CString m_Strcard_brand            =_T("");  
	CString m_Strhandler_queue         =_T("");
	CString m_Strtran_req_sent_time    =_T("");  
	CString m_Strtran_resp_recv_time   =_T("");  
	CString m_Strtran_req_sent_mode    =_T("");  
	CString m_Strtran_resp_recv_mode   =_T("");  
	CString m_Strresponse_text         =_T("");  
	CString m_Strdecision_code_1_value =_T("");  
	CString m_Strdecision_code_2_value =_T("");  
	CString m_Strdecision_code_3_value =_T("");  
	CString m_Strdecision_code_4_value =_T("");  
	CString m_Strdecision_code_5_value =_T("");  
	CString m_Strdecision_code_6_value =_T("");  
	CString m_Strdecision_code_7_value =_T("");  
	CString m_Strdecision_code_8_value =_T("");  
	CString m_Strdecision_code_9_value =_T("");  
	CString m_Strdecision_code_10_value=_T("");  
	
}


void CFraudTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_FRAUD_TXN_SENT 	  ,m_Cntltran_req_sent_time    );
	DDX_Control(pDX, IDC_FRAUD_TXN_RCVD 	  ,m_Cntltran_resp_recv_time   );
	DDX_Control(pDX, IDC_FRAUD_TXN_SENT_MODE  ,m_Cntltran_req_sent_mode    );
	DDX_Control(pDX, IDC_FRAUD_TXN_RECV_MODE  ,m_Cntltran_resp_recv_mode   );
	DDX_Control(pDX, IDC_FRAUD_RESPONSE_TEXT  ,m_Cntlresponse_text         );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE1 ,m_Cntldecision_code_1_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE2 ,m_Cntldecision_code_2_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE3 ,m_Cntldecision_code_3_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE4 ,m_Cntldecision_code_4_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE5 ,m_Cntldecision_code_5_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE6 ,m_Cntldecision_code_6_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE7 ,m_Cntldecision_code_7_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE8 ,m_Cntldecision_code_8_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE9 ,m_Cntldecision_code_9_value );
	DDX_Control(pDX, IDC_FRAUD_DECISION_CODE10,m_Cntldecision_code_10_value);
	DDX_Control(pDX, IDC_FRAUD_ISSUE_LINK	  ,m_Cntlhandler_queue    );
		DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_FRAUD_TXN_SENT 	   ,m_Strtran_req_sent_time    );
	DDX_Text(pDX, IDC_FRAUD_TXN_RCVD 	   ,m_Strtran_resp_recv_time   );
	DDX_Text(pDX, IDC_FRAUD_TXN_SENT_MODE  ,m_Strtran_req_sent_mode    );
	DDX_Text(pDX, IDC_FRAUD_TXN_RECV_MODE  ,m_Strtran_resp_recv_mode   );
	DDX_Text(pDX, IDC_FRAUD_RESPONSE_TEXT  ,m_Strresponse_text         );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE1 ,m_Strdecision_code_1_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE2 ,m_Strdecision_code_2_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE3 ,m_Strdecision_code_3_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE4 ,m_Strdecision_code_4_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE5 ,m_Strdecision_code_5_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE6 ,m_Strdecision_code_6_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE7 ,m_Strdecision_code_7_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE8 ,m_Strdecision_code_8_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE9 ,m_Strdecision_code_9_value );
	DDX_Text(pDX, IDC_FRAUD_DECISION_CODE10,m_Strdecision_code_10_value);
	DDX_Text(pDX, IDC_FRAUD_ISSUE_LINK 	   ,m_Strhandler_queue    );

		//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFraudTransactionDetails, CPropertyPage)
//{{AFX_MSG_MAP(CTransactionDetails)
//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_FRAUD_TXN_RCVD, &CFraudTransactionDetails::OnEnChangeFraudTxnRcvd)
	ON_EN_CHANGE(IDC_FRAUD_DECISION_CODE6, &CFraudTransactionDetails::OnEnChangeFraudDecisionCode6)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails message handlers

BOOL CFraudTransactionDetails::OnInitDialog() 
{
	
	
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	//FTLF01 sFraudRecord = {0};
	CHAR strErrorMessage[256]= {0};
	strncpy( sFraudRecord.ftlf01.tran_id, (char *)Transactionid ,sizeof(Transactionid));
	int rcode = txdsapi_get_record ( (pBYTE)&sFraudRecord, sizeof (sFraudRecord), FTLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The FAI record for this Transaction was not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}
	if(m_bTran)
	{
			PopulateFraudTranDialogFromDatabase ( sFraudRecord);
	}
	/*else if(m_bFGuard)
			PopulateFGuardDialogFromDatabase(sFGuardRecord);*/
	if(m_bTran_stip)
	{
		PopulateFraudTranSTIPDialogFromDatabase(sFraudRecordS);
	}
		//PopulateFraudTranDialogFromDatabase ( Transactionid );
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFraudTransactionDetails::PopulateFraudTranDialogFromDatabase ( FRT_RECORD sFraudRecord  )
{
	/*FTLF01 *sFraudRecord;
	CHAR strErrorMessage[256]= {0};	
	memset(sFraudRecord, 0x00, sizeof(FTLF01));
	
	memcpy( sFraudRecord->tran_id, (char *)Transactionid ,sizeof(sFraudRecord->tran_id));
	strncpy( sFraudRecord->handler_queue, (char *)Handler_queue ,sizeof(Handler_queue));
	int rcode = txdsapi_get_record ( (pBYTE)sFraudRecord, sizeof (FTLF01), FTLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The FAI record for this Transaction was not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}*/


	m_Strtran_req_sent_time    	= sFraudRecord.ftlf01.tran_req_sent_time;
	m_Strtran_resp_recv_time   	= sFraudRecord.ftlf01.tran_resp_recv_time;
	m_Strtran_req_sent_mode    	= sFraudRecord.ftlf01.tran_req_sent_mode;
	m_Strtran_resp_recv_mode   	= sFraudRecord.ftlf01.tran_resp_recv_mode;
	m_Strresponse_text         	= sFraudRecord.ftlf01.response_text;
	m_Strdecision_code_1_value 	= sFraudRecord.ftlf01.decision_code_1_value;
	m_Strdecision_code_2_value 	= sFraudRecord.ftlf01.decision_code_2_value;
	m_Strdecision_code_3_value 	= sFraudRecord.ftlf01.decision_code_3_value;
	m_Strdecision_code_4_value 	= sFraudRecord.ftlf01.decision_code_4_value;
	m_Strdecision_code_5_value 	= sFraudRecord.ftlf01.decision_code_5_value;
	m_Strdecision_code_6_value 	= sFraudRecord.ftlf01.decision_code_6_value;
	m_Strdecision_code_7_value 	= sFraudRecord.ftlf01.decision_code_7_value;
	m_Strdecision_code_8_value 	= sFraudRecord.ftlf01.decision_code_8_value;
	m_Strdecision_code_9_value 	= sFraudRecord.ftlf01.decision_code_9_value;
	m_Strdecision_code_10_value	= sFraudRecord.ftlf01.decision_code_10_value;
	m_Strhandler_queue			= sFraudRecord.ftlf01.handler_queue;
	UpdateData(FALSE);
   
  
   
   return TRUE;
}

BOOL CFraudTransactionDetails::PopulateFraudTranSTIPDialogFromDatabase ( FRT_STIP_RECORD sFraudRecordS  )
{

	CHAR strErrorMessage[256]= {0};	

	int rcode = txdsapi_get_record ( (pBYTE)&sFraudRecordS, sizeof (sFraudRecordS), FTLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The FAI record for this Transaction was not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}


	m_Strtran_req_sent_time    	= sFraudRecordS.ftlf01.tran_req_sent_time;
	m_Strtran_resp_recv_time   	= sFraudRecordS.ftlf01.tran_resp_recv_time;
	m_Strtran_req_sent_mode    	= sFraudRecordS.ftlf01.tran_req_sent_mode;
	m_Strtran_resp_recv_mode   	= sFraudRecordS.ftlf01.tran_resp_recv_mode;
	m_Strresponse_text         	= sFraudRecordS.ftlf01.response_text;
	m_Strdecision_code_1_value 	= sFraudRecordS.ftlf01.decision_code_1_value;
	m_Strdecision_code_2_value 	= sFraudRecordS.ftlf01.decision_code_2_value;
	m_Strdecision_code_3_value 	= sFraudRecordS.ftlf01.decision_code_3_value;
	m_Strdecision_code_4_value 	= sFraudRecordS.ftlf01.decision_code_4_value;
	m_Strdecision_code_5_value 	= sFraudRecordS.ftlf01.decision_code_5_value;
	m_Strdecision_code_6_value 	= sFraudRecordS.ftlf01.decision_code_6_value;
	m_Strdecision_code_7_value 	= sFraudRecordS.ftlf01.decision_code_7_value;
	m_Strdecision_code_8_value 	= sFraudRecordS.ftlf01.decision_code_8_value;
	m_Strdecision_code_9_value 	= sFraudRecordS.ftlf01.decision_code_9_value;
	m_Strdecision_code_10_value	= sFraudRecordS.ftlf01.decision_code_10_value;
	m_Strhandler_queue			= sFraudRecordS.ftlf01.handler_queue;
	UpdateData(FALSE);
   
  
   
   return TRUE;
}


void CFraudTransactionDetails::OnEnChangeFraudTxnRcvd()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CFraudTransactionDetails::OnEnChangeFraudDecisionCode6()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
