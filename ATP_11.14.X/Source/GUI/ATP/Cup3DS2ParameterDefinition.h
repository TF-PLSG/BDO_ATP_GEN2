#ifndef CUP3DS2_PARAMETERS
#define CUP3DS2_PARAMETERS

#include "basictyp.h" 
#include "ChangeHistory.h"

class CCup3DS2Parameter : public CPropertyPage
{
	DECLARE_DYNAMIC(CCup3DS2Parameter)

public:
	CCup3DS2Parameter();  // * // standard constructor 
	virtual ~CCup3DS2Parameter();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	enum { IDD = ID_CUP3DS2_PARAMETERS };
	CEdit	m_CntlAV_VERIFICATION_RC;
	CEdit	m_CntlAUTHENTICATION_ATTEMPT_RC;
	CButton	m_CntlCVN2_VALIDATION_FLAG;
	CButton	m_CntlAV_VALIDATION_FLAG;
	CComboBox	m_CntlAUTHENTICATION_ATTEMPT_VERIFY_FLAG;

	CButton	m_ViewButton;

	CString	m_StrAV_VERIFICATION_RC;
	CString	m_StrAUTHENTICATION_ATTEMPT_RC;
	CString	m_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString m_StrMessageLbl;

	CString	prev_StrAV_VERIFICATION_RC;
	CString	prev_StrAUTHENTICATION_ATTEMPT_RC;
	CString	prev_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	prev_StrCVN2_VALIDATION_FLAG;
	CString	prev_StrAV_VALIDATION_FLAG;

	CString	new_StrAV_VERIFICATION_RC;
	CString	new_StrAUTHENTICATION_ATTEMPT_RC;
	CString	new_StrAUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	CString	new_StrCVN2_VALIDATION_FLAG;
	CString	new_StrAV_VALIDATION_FLAG;



public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	void PopulateControlStructure();
	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();

	BOOL change_AV_VERIFICATION_RC;
	BOOL change_AUTHENTICATION_ATTEMPT_RC;
	BOOL change_AUTHENTICATION_ATTEMPT_VERIFY_FLAG;
	BOOL change_StrCVN2_VALIDATION_FLAG;
	BOOL	change_StrAV_VALIDATION_FLAG;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					

	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()								

public:
	//afx_msg void OnCbnSelchangeAuthenticationAttemptVerifyFlag();
};

#endif