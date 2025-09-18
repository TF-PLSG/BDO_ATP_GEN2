/*********************************************************************************

************************************************************************************/
// TransactionDetails.h : header file
//

#ifndef ADDL_TXN_DETAILS
#define ADDL_TXN_DETAILS
//#include "CVector.h"
#include "stdafx.h"
#include "TranSearch.h"

typedef struct
{ 
  TLF01  tlf01;

}ADDLFLDSTRAN_RECORD;

typedef struct
{ 
  TLF01  tlf01;

}ADDLFLDSTRAN_STIP_RECORD;
/////////////////////////////////////////////////////////////////////////////


// CTransactionDetails dialog
class CAddlFldsTransactionDetails : public CPropertyPage
{
// Construction
		DECLARE_DYNCREATE(CAddlFldsTransactionDetails)
private:
	void  checkMaskCardNumber();
public:
	CAddlFldsTransactionDetails();   // standard constructor

	BOOL PopulateAddlFldsFraudTranDialogFromDatabase ( ADDLFLDSTRAN_RECORD sTranRecord  );
	BOOL PopulateAddlFldsFraudTranSTIPDialogFromDatabase ( ADDLFLDSTRAN_STIP_RECORD sTranRecordS  );
	//TLF01 sTranRecord;

	ADDLFLDSTRAN_RECORD sTranRecord;
	ADDLFLDSTRAN_STIP_RECORD sTranRecordS;
	void CleanTranDialog();
	INT Check_If_AMEX_Transaction(char *card_num);
	INT Check_If_Diners_Transaction(char *card_num);
	INT Check_If_MC_Transaction (char *card_num);
	INT Check_If_VISA_Transaction (char *card_num);
	INT Check_If_JCB_Transaction (char *card_num);
	INT Check_If_Acquiring_Transaction (char * handler_queue);
	INT Check_If_Issuing_Transaction (char * handler_queue);
	

// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_ADDITIONAL_DETAILS_RES_TXT };

	//}}AFX_DATA
	CButton m_CntlCAVV_Result_code			;         
	CButton m_CntlPOS_Env_Field				;  
	CButton m_CntlVisa_3DS2_TranID			;  
	CButton m_CntlVisa_3DS2_Auth_Rslt_code  ;
	CButton m_CntlVisa_3DS2_Auth_mthd		;  
	CButton m_CntlVisa_3DS2_Sec_indcr		;  
	CButton m_CntlVisa_Resp_reason_code		;  
	CButton m_CntlVisa_STIP_reason_code		;  
	CButton m_CntlVisa_MVV_value			;  
	CButton m_CntlMC_Merch_Advice_code		;  
	CButton m_CntlMC_TranLink_ID			;  
	CButton m_CntlMC_Prgrm_prtcl			;  
	CButton m_CntlMC_DS_Tranid				;  
	CButton m_CntlMC_UCAF_value				;  
	CButton m_CntlMC_Merch_Cntry_orgn		;  
	CButton m_CntlMC_Paymt_accnt_ref		;  
	CButton m_CntlMC_TCC_value				;  
	CButton m_CntlMC_Merch_paymt_gwy_id		;  
	CButton m_CntlMC_Sanction_score_addnl_info		;  
	CButton m_CntlMC_OBS_value		        ;
	CButton m_CntlMC_Trscn_purpose			;
	CButton m_CntlDCI_Func_code		        ;
	CButton m_CntlDCI_Err_DE_value          ;
	CButton m_CntlDCI_Addl_Auth_data        ;
	CButton m_CntlAMX_ECI_value				;
	CButton m_CntlAMX_AEVV_value			;
	CButton m_CntlAMX_Trasn_id				;
	CButton m_CntlAMX_3DS2_version			;
	CButton m_CntlUPI_AVN_value				;
	CButton m_CntlUPI_DS_Transn_id			;
	CButton m_CntlUPI_Secrty_Risk_IP_value	;
	CButton m_CntlUPI_3DS2_version			;
	CButton m_CntlUPI_ECI_value				;
	CButton m_CntlUPI_Secrty_Info_AM_value	;
	CButton m_CntlJCB_3DS2_CAVV_value		;
	CButton m_CntlJCB_ECI_value				;
	CButton m_CntlJCB_3DS2_version			;
	CButton m_CntlPhysical_Term_serl_num	;
	CButton m_CntlPIN_service_code			;
	CButton m_CntlSecrty_srvcs_info			;

	CString m_StrCAVV_Result_code			;         
	CString m_StrPOS_Env_Field				;  
	CString m_StrVisa_3DS2_TranID			;  
	CString m_StrVisa_3DS2_Auth_Rslt_code   ;
	CString m_StrVisa_3DS2_Auth_mthd		;  
	CString m_StrVisa_3DS2_Sec_indcr		;  
	CString m_StrVisa_Resp_reason_code		;  
	CString m_StrVisa_STIP_reason_code		;  
	CString m_StrVisa_MVV_value				;  
	CString m_StrMC_Merch_Advice_code		;  
	CString m_StrMC_TranLink_ID				;  
	CString m_StrMC_Prgrm_prtcl				;  
	CString m_StrMC_DS_Tranid				;  
	CString m_StrMC_UCAF_value				;  
	CString m_StrMC_Merch_Cntry_orgn		;  
	CString m_StrMC_Paymt_accnt_ref			;  
	CString m_StrMC_TCC_value				;  
	CString m_StrMC_Merch_paymt_gwy_id		;  
	CString m_StrMC_Sanction_score_addnl_info		;  
	CString m_StrMC_OBS_value		        ;
	CString m_StrMC_Trscn_purpose			;
	CString m_StrDCI_Func_code		        ;
	CString m_StrDCI_Err_DE_value           ;
	CString m_StrDCI_Addl_Auth_data         ;
	CString m_StrAMX_ECI_value				;
	CString m_StrAMX_AEVV_value				;	
	CString m_StrAMX_Trasn_id				;
	CString m_StrAMX_3DS2_version			;
	CString m_StrUPI_AVN_value				;
	CString m_StrUPI_DS_Transn_id			;
	CString m_StrUPI_Secrty_Risk_IP_value	;
	CString m_StrUPI_3DS2_version			;
	CString m_StrUPI_ECI_value				;
	CString m_StrUPI_Secrty_Info_AM_value	;
	CString m_StrJCB_3DS2_CAVV_value		;
	CString m_StrJCB_ECI_value				;
	CString m_StrJCB_3DS2_version			;
	CString m_StrPhysical_Term_serl_num		;
	CString m_StrPIN_service_code			;
	CString m_StrSecrty_srvcs_info			;
	CString m_StrMessageLbl					;

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
	afx_msg void OnEnChangeMcMerchPaymtGwayId();
	afx_msg void OnEnChangeVsMvvValue();
	afx_msg void OnEnChangeMcMerchCtryOfOrigin();
	afx_msg void OnEnChangeAmxTrnsnId();
	afx_msg void OnEnChangeAmxAevvValue();
	afx_msg void OnEnChangePosEnvironmentField();
	afx_msg void OnEnChangeMcTransactionPurpose();
	afx_msg void OnEnChangeCup3ds2Version();
};

#endif