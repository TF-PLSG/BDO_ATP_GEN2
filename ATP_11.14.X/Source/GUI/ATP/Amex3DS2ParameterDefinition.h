#ifndef AMEX3DS2_PARAMETERS
#define AMEX3DS2_PARAMETERS

#include "basictyp.h" 
#include "ChangeHistory.h"

class CAmex3DS2Parameter : public CPropertyPage
{
	DECLARE_DYNAMIC(CAmex3DS2Parameter)

public:
	CAmex3DS2Parameter();  // * // standard constructor 
	virtual ~CAmex3DS2Parameter();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt
//	CVector <CString> arrColumnNames;
	// structure rqrd which include the name of the 
												// options u want for ur window
	//SUBSIDY_QUOTA m_LastSubsidyQuota;
	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	//enum { IDD = IDD_SYSTEMGLOBALPARAMETER };  // rqrd 
	enum { IDD = ID_AMEX3DS2_PARAMETERS };
	CEdit	m_CntlSELF_VALIDATION_AC;
	CEdit	m_CntlAUTHENTICATION_RESULT_CODE_AC;
	CEdit	m_CntlAUTHENTICATION_ATTEMPT_AC;
	CButton	m_CntlCSC_VALIDATION_FLAG;
	CComboBox	m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG;

	CButton	m_ViewButton;


	CString	m_StrSELF_VALIDATION_AC;
	CString	m_StrAUTHENTICATION_RESULT_CODE_AC;
	CString	m_StrAUTHENTICATION_ATTEMPT_AC;
	CString	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString m_StrMessageLbl;

	CString	prev_StrSELF_VALIDATION_AC;
	CString	prev_StrAUTHENTICATION_RESULT_CODE_AC;
	CString	prev_StrAUTHENTICATION_ATTEMPT_AC;
	CString	prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	prev_StrCSC_VALIDATION_FLAG;

	CString	new_StrSELF_VALIDATION_AC;
	CString	new_StrAUTHENTICATION_RESULT_CODE_AC;
	CString	new_StrAUTHENTICATION_ATTEMPT_AC;
	CString	new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	new_StrCSC_VALIDATION_FLAG;


public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	void PopulateControlStructure();
	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();

	BOOL change_SELF_VALIDATION_AC;
	BOOL change_AUTHENTICATION_RESULT_CODE_AC;
	BOOL change_AUTHENTICATION_ATTEMPT_AC;
	BOOL change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	BOOL change_StrCSC_VALIDATION_FLAG;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					

	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()								

public:
	afx_msg void OnCbnSelchangeAuthenticationAttemptVerifyFlag();
};

#endif