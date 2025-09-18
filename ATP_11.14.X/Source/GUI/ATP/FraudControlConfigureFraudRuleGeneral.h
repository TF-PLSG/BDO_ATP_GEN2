#ifndef FRAUD_RULE
#define FRAUD_RULE

#include "basictyp.h" 
#include "ChangeHistory.h"
#include "FraudControlConfigureFraudRuleGeneral.h"

class CFraudControlConfigurationFraudRule : public CPropertyPage
{
	DECLARE_DYNAMIC(CFraudControlConfigurationFraudRule)

public:
	CFraudControlConfigurationFraudRule();  // * // standard constructor 
	virtual ~CFraudControlConfigurationFraudRule();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt
	BOOL m_bFraudGeneral;
//	CVector <CString> arrColumnNames;

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	FRULE01 FRULE01_old;
	FRULE01 FRULE01;

	void EnableConfigurGeneralDialog(int);
	void CleanConfigurGeneralDialog();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromGeneralDialog( pFRULE01 psFrule01 );
	BOOL PopulateGeneralDialogFromDatabase( pFRULE01 psFrule01  );
	BOOL CompareStructures();
	//enum { IDD = ID_CONFIGURE_FRAUD_RULE_GENERAL };  // rqrd 
	enum { IDD = ID_CONFIGURE_FRAUD_RULE_GENERAL };	
	CEdit	m_CntlCard_brand        ; 
	CEdit	m_CntlAcq_country_code  ; 
	CEdit	m_CntlBilling_amt       ; 
	CEdit	m_CntlPos_mode_1        ; 
	CEdit	m_CntlPos_mode_2        ; 
	CEdit	m_CntlPos_mode_3        ; 
	CEdit	m_CntlMcc_1             ; 
	CEdit	m_CntlMcc_2             ; 
	CEdit	m_CntlMcc_3             ; 		
	CString	m_StrCard_brand         ;
	CString	m_StrAcq_country_code   ;
	CString	m_StrBilling_amt        ;
	CString	m_StrPos_mode_1         ;
	CString	m_StrPos_mode_2         ;
	CString	m_StrPos_mode_3         ;
	CString	m_StrMcc_1              ;
	CString	m_StrMcc_2              ;
	CString	m_StrMcc_3              ;
	CString	m_StrMessageLbl			;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnKillfocusBillingAmtFld();
	DECLARE_MESSAGE_MAP()								

};

#endif