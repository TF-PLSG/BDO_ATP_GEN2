
/*
************************************************************************************/
// UserAccountStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserAccountStatus dialog

#ifndef USER_ACCOUNT_STATUS
#define USER_ACCOUNT_STATUS
#include "afxwin.h"
#include "afxdtctl.h"

class CUserAccountStatus : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserAccountStatus)

// Construction
public:
	CUserAccountStatus();
	~CUserAccountStatus();

	//BOOL PopulateDialogFromDatabase (UCF01);
	BOOL PopulateDialogFromDatabase ();
	BOOL PopulateUserStatusFromDatabase (UCF01);
	BOOL PopulateAddUSerStructure();
	//ramya
	BOOL PopulateUserDialogFromDatabase ( UCF01,UCF01  );
	/******************************************/
	void CleanUserAccountStatusDialog();
   	int m_nDialogMode;
	
	void EnableUserAccountStatusDialog( int );
	CString m_bAccStatus;
	CString m_bUAccStatus;
	
	BOOL m_bStatus;
	BOOL m_bPopulate;
	BOOL m_viewPopulate;
	BOOL m_updatePopulate;
	//BOOL m_bUAccStatus;
	int m_addPopulate;
	CString prev_string;
	CString prev_UserStatus;
	CString new_UserStatus;

	CString prev_ExpDate;
	CString new_ExpDate;

	BOOL change_f;
	BOOL change_UserStatus, change_ExpiryDate;

    void PopulateHistoryTable();
    BOOL PopulateControlStructure (); //changed from this to below function
	BOOL PopulateControlStructure (char *UserStatus, char *ExpiryDate);
	BOOL PopulateUserStatus();

   // BOOL CUserAccess::GetControlStructure ( psControl);	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CUserAccountStatus)
	enum { IDD = IDD_USER_ACCOUNT_STATUS };
	CButton	m_CntlAddUnlock;
	CButton	m_CntlAddLock;
	CButton	m_CntlAddDeleted;
	CString	m_StrMessageLbl;
	CString	m_StrUserNameFld;
	CString	m_StrLabel1;

	CComboBox	m_CntlUserStatus;
	CString	m_StrUserStatus;
	//CDateTimeCtrl m_CntlUserExpiryDate;
	
	CTime m_StrUserExpiryDate;
	CString UCF01_CUserStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserAccountStatus)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserAccountStatus)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnEnChangeUserNameFld();
	afx_msg void OnCbnSelchangeUserStatus();
	afx_msg void OnBnClickedRadioUnlock();
	afx_msg void OnStnClickedMessageLbl();
	afx_msg void OnDtnDatetimechangeExpiryDateAs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetfocusCombo2();
	CComboBox m_Combo2;
//	afx_msg void OnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	CDateTimeCtrl m_DateTimePicker;
	afx_msg void OnDropdownCombo2();
};

#endif
