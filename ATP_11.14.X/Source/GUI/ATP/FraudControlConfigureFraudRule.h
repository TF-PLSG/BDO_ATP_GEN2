#ifndef FRAUD_CONTROL
#define FRAUD_CONTROL

#include "basictyp.h" 
#include "ChangeHistory.h"
#include "FraudControlConfigureFraudRuleGeneral.h"
class CFraudControlConfigurationLink : public CPropertyPage
{
	DECLARE_DYNAMIC(CFraudControlConfigurationLink)

public:
	CFraudControlConfigurationLink();  // * // standard constructor 
	virtual ~CFraudControlConfigurationLink();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt

//	CVector <CString> arrColumnNames;
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromConfigureRuleDialog( pFRULE01);
	BOOL PopulateConfigureRuleDialogFromDatabase( pFRULE01);

	char strFilePrimaryKey[30];
	char strStatus[2];
	char m_strCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	FRULE01 FRULE01_old;
	FRULE01 FRULE01;
	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;
	CFraudControlConfigurationFraudRule* pFraudControlConfigurationFraudRule;
	//enum { IDD = ID_CONFIGURE_FRAUD_RULE_PAGE };  // rqrd 
	enum { IDD = ID_CONFIGURE_FRAUD_RULE_PAGE };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlFraudRuleFld;
	CString m_StrFraudRuleFld;
	CString m_StrMessageLbl;
public:
	virtual void OnCancel();
	virtual BOOL OnApply();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()	

public:
	afx_msg void OnBnClickedConfigureruleAddButton();
	afx_msg void OnBnClickedConfigureruleDeleteButton();
	afx_msg void OnBnClickedConfigureruleUpdateButton();
	afx_msg void OnBnClickedConfigureruleViewButton();
	afx_msg void OnBnClickedConfigureruleListButton();
};

#endif