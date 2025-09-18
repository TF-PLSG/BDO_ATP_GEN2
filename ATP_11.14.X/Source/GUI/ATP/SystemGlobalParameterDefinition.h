#ifndef GLOBAL_PARAMETER
#define GLOBAL_PARAMETER

#include "basictyp.h" 
#include "ChangeHistory.h"

class CSystemGlobalParameter : public CPropertyPage
{
	DECLARE_DYNAMIC(CSystemGlobalParameter)

public:
	CSystemGlobalParameter();  // * // standard constructor 
	virtual ~CSystemGlobalParameter();			// *
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
	enum { IDD = ID_SYSTEM_GLOBAL_PARAMETERS };
	CEdit   m_GlblParam;
	//ramya
	CEdit   m_GlblParam2;
	CEdit   m_GlblParam3;
	CEdit   m_GlblParam4;
	CEdit   m_GlblDaysB4Expiry;
	CEdit   m_UserStatsTimeoutVal;
	CButton	m_ViewButton;
	//CButton	m_UpdateButton;
	CString m_Strglblparam;
	CString m_Strglblparam2;
	CString m_Strglblparam3;
	CString m_Strglblparam4;
	CString m_StrglblDaysB4Expiry;
	CString m_StrglblUserStatsTimeoutVal;
	CString m_StrMessageLbl;

	CString prev_days_inactive;
	CString prev_max_prev_passwords_not_allowed;
	CString prev_max_password_retries;
	CString prev_min_password_length;
	CString prev_days_before_expiry;
	CString prev_UserStatsTimeoutVal;

	CString new_days_inactive;
	CString new_max_prev_passwords_not_allowed;
	CString new_max_password_retries;
	CString new_min_password_length;
	CString new_days_before_expiry;
	CString new_UserStatsTimeoutVal;

	//char m_strCurrentData[50];
	//char m_strPreviousData[50];
	//char m_strFieldTag[50];
public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	void PopulateControlStructure();
	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL change_max_prev_passwords_not_allowed;
	BOOL change_max_password_retries;
	BOOL change_min_password_length;
	BOOL change_days_inactive;
	BOOL change_days_before_expiry;
	BOOL change_UserStatsTimeoutVal;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					

	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()								

};

#endif