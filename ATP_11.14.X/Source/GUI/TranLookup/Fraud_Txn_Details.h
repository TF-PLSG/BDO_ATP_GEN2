/*********************************************************************************

************************************************************************************/
// TransactionDetails.h : header file
//

#ifndef FRAUD_TXN_DETAILS
#define FRAUD_TXN_DETAILS
//#include "CVector.h"
#include "stdafx.h"
#include "TranSearch.h"

typedef struct
{ 
  FTLF01   ftlf01;

}FRT_RECORD;

typedef struct
{ 
  FTLF01   ftlf01;

}FRT_STIP_RECORD;
/////////////////////////////////////////////////////////////////////////////


// CTransactionDetails dialog
class CFraudTransactionDetails : public CPropertyPage
{
// Construction
	DECLARE_DYNCREATE(CFraudTransactionDetails)
private:
	void  checkMaskCardNumber();
public:
	CFraudTransactionDetails();   // standard constructor

	//TLF01 sTranRecord;

	FRT_RECORD sFraudRecord;
	FRT_STIP_RECORD sFraudRecordS;
	void CleanTranDialog();
	
	BOOL PopulateFraudTranDialogFromDatabase (  FRT_RECORD  );
	BOOL PopulateFraudTranSTIPDialogFromDatabase (  FRT_STIP_RECORD  );
	BOOL m_bFraudDetails;
	int m_nDialogMode;	

// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_FRAUD_TRANDETAILS };

	//}}AFX_DATA
	CButton m_Cntltran_id				;         
	CButton m_Cntlsys_date              ;  
	CButton m_Cntlcard_brand            ;  
	CButton m_Cntlhandler_queue         ;
	CButton m_Cntltran_req_sent_time    ;  
	CButton m_Cntltran_resp_recv_time   ;  
	CButton m_Cntltran_req_sent_mode    ;  
	CButton m_Cntltran_resp_recv_mode   ;  
	CButton m_Cntlresponse_text         ;  
	CButton m_Cntldecision_code_1_value ;  
	CButton m_Cntldecision_code_2_value ;  
	CButton m_Cntldecision_code_3_value ;  
	CButton m_Cntldecision_code_4_value ;  
	CButton m_Cntldecision_code_5_value ;  
	CButton m_Cntldecision_code_6_value ;  
	CButton m_Cntldecision_code_7_value ;  
	CButton m_Cntldecision_code_8_value ;  
	CButton m_Cntldecision_code_9_value ;  
	CButton m_Cntldecision_code_10_value;  
	CButton m_Cntlfuture_field1         ;
	CButton m_Cntlfuture_field2         ;
	CButton m_Cntlfuture_field3         ;
	CButton m_Cntlfuture_field4         ;
	CButton m_Cntlfuture_field5         ;
	CString m_Strtran_id               ;  
	CString m_Strsys_date              ;  
	CString m_Strcard_brand            ;  
	CString m_Strhandler_queue         ;
	CString m_Strtran_req_sent_time    ;  
	CString m_Strtran_resp_recv_time   ;  
	CString m_Strtran_req_sent_mode    ;  
	CString m_Strtran_resp_recv_mode   ;  
	CString m_Strresponse_text         ;  
	CString m_Strdecision_code_1_value ;  
	CString m_Strdecision_code_2_value ;  
	CString m_Strdecision_code_3_value ;  
	CString m_Strdecision_code_4_value ;  
	CString m_Strdecision_code_5_value ;  
	CString m_Strdecision_code_6_value ;  
	CString m_Strdecision_code_7_value ;  
	CString m_Strdecision_code_8_value ;  
	CString m_Strdecision_code_9_value ;  
	CString m_Strdecision_code_10_value;  
	CString m_Strfuture_field1         ;
	CString m_Strfuture_field2         ;
	CString m_Strfuture_field3         ;
	CString m_Strfuture_field4         ;
	CString m_Strfuture_field5         ;
	CString m_StrMessageLbl			   ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionDetails)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransactionDetails)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeFraudTxnRcvd();
	afx_msg void OnEnChangeFraudDecisionCode6();
};

#endif