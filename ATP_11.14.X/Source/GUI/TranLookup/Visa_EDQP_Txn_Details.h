/*********************************************************************************

************************************************************************************/
// TransactionDetails.h : header file
//

#ifndef VEDQP_TXN_DETAILS
#define VEDQP_TXN_DETAILS
//#include "CVector.h"
#include "stdafx.h"
#include "TranSearch.h"

typedef struct
{ 
  TLF01   tlf01;

}VEDQP_RECORD;

typedef struct
{ 
  TLF01   tlf01;

}VEDQP_STIP_RECORD;
/////////////////////////////////////////////////////////////////////////////


// CTransactionDetails dialog
class CVisaEDQPTransactionDetails : public CPropertyPage
{
// Construction
	DECLARE_DYNCREATE(CVisaEDQPTransactionDetails)
private:
	void  checkMaskCardNumber();
public:
	CVisaEDQPTransactionDetails();   // standard constructor

	BOOL PopulateVisaEDQPTranDialogFromDatabase ( VEDQP_RECORD vTranRecord  );
	BOOL PopulateVisaEDQPSTIPTranDialogFromDatabase ( VEDQP_STIP_RECORD vTranRecordS  );
	//TLF01 sTranRecord;

	VEDQP_RECORD TranRecord;
	VEDQP_STIP_RECORD TranRecordS;
	void CleanTranDialog();

// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_VEDQP_DATA_FIELDS };

	//}}AFX_DATA
	CButton m_Cntlcard_num				;         
	CButton m_Cntltran_amt              ;  
	CButton m_Cntlauth_number           ;  
	CButton m_Cntlappl_auth_char_indcr  ;
	CButton m_Cntlresponse_code			;  
	CButton m_Cntlcurrency_code			;  
	CButton m_Cntlaccnt_fund_source     ;  
	CButton m_Cntlcard_hldr_id_mthd		;  
	CButton m_Cntlcash_back_amt         ;  
	CButton m_Cntlmrkt_spcfc_data_indcr ;  
	CButton m_Cntlmerch_ctgry_code		;  
	CButton m_Cntlpos_entry_mode		;  
	CButton m_Cntlproduct_id			;  
	CButton m_Cntltranscn_identfr		;  
	CButton m_Cntlvaldn_code			;  
	CButton m_CntlECI_value				;  
	CButton m_Cntlspend_qualfn_indcr    ;  
	CButton m_CntlPOS_env_field			;  

	CString m_Strcard_num              ;  
	CString m_Strtran_amt              ;  
	CString m_Strauth_number           ;  
	CString m_Strappl_auth_char_indcr        ;
	CString m_Strresponse_code    ;  
	CString m_Strcurrency_code   ;  
	CString m_Straccnt_fund_source    ;  
	CString m_Strcard_hldr_id_mthd   ;  
	CString m_Strcash_back_amt         ;  
	CString m_Strmrkt_spcfc_data_indcr ;  
	CString m_Strmerch_ctgry_code ;  
	CString m_Strpos_entry_mode ;  
	CString m_Strproduct_id	 ;  
	CString m_Strtranscn_identfr;  
	CString m_Strvaldn_code ;  
	CString m_StrECI_value	 ;  
	CString m_Strspend_qualfn_indcr ;  
	CString m_StrPOS_env_field ;  
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
};

#endif