#ifndef DINERS3DS2_PARAMETERS
#define DINERS3DS2_PARAMETERS

#include "basictyp.h" 
#include "ChangeHistory.h"

class CDiners3DS2Parameter : public CPropertyPage
{
	DECLARE_DYNAMIC(CDiners3DS2Parameter)

public:
	CDiners3DS2Parameter();  // * // standard constructor 
	virtual ~CDiners3DS2Parameter();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	enum { IDD = ID_DINERS3DS2_PARAMETERS };
	CEdit	m_CntlCAVV_RESULT_PASS;
	CEdit	m_CntlCAVV_RESULT_CODE;
	CEdit	m_CntlAUTHENTICATION_ACTION_CODE;
	CEdit	m_CntlAUTHENTICATION_CAVV_RESULT_CODE;
	CEdit	m_CntlECI6_ACTION_CODE;
	CButton	m_CntlCVV2_VALIDATION_FLAG;
	CComboBox	m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG;


	CButton	m_ViewButton;

	
	CString	m_StrCAVV_RESULT_PASS;
	CString	m_StrCAVV_RESULT_CODE;
	CString	m_StrAUTHENTICATION_ACTION_CODE;
	CString	m_StrAUTHENTICATION_CAVV_RESULT_CODE;
	CString	m_StrECI6_ACTION_CODE;
	CString	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;

	CString m_StrMessageLbl;

	CString	prev_StrCAVV_RESULT_PASS;
	CString	prev_StrCAVV_RESULT_CODE;
	CString	prev_StrAUTHENTICATION_ACTION_CODE;
	CString	prev_StrAUTHENTICATION_CAVV_RESULT_CODE;
	CString	prev_StrECI6_ACTION_CODE;
	CString	prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	prev_StrCVV2_VALIDATION_FLAG;

	CString	new_StrCAVV_RESULT_PASS;
	CString	new_StrCAVV_RESULT_CODE;
	CString	new_StrAUTHENTICATION_ACTION_CODE;
	CString	new_StrAUTHENTICATION_CAVV_RESULT_CODE;
	CString	new_StrECI6_ACTION_CODE;
	CString	new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	new_StrCVV2_VALIDATION_FLAG;



public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	void PopulateControlStructure();
	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();

	BOOL change_CAVV_RESULT_PASS;
	BOOL change_CAVV_RESULT_CODE;
	BOOL change_AUTHENTICATION_ACTION_CODE;
	BOOL change_AUTHENTICATION_CAVV_RESULT_CODE;
	BOOL change_ECI6_ACTION_CODE;
	BOOL change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	BOOL change_StrCVV2_VALIDATION_FLAG;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					

	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()								

public:
	afx_msg void OnCbnSelchangeAuthenticationAttemptVerifyFlag();
	afx_msg void OnBnClickedCvv2ValidationFlag();
};

#endif