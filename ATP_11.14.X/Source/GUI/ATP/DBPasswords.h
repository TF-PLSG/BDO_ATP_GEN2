/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DBPasswords.h  $
   
      Rev 1.0   Jan 31 2011 11:30:22   TF_Srikanth
   Added DB Passwords Class for Changing Database Connectivity Passwords    
    
*
************************************************************************************/
//
// DBPasswords.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDBPasswords dialog

#ifndef DB_PASSWORDS
#define DB_PASSWORDS

class CDBPasswords : public CPropertyPage
{
	DECLARE_DYNCREATE(CDBPasswords)

// Construction
public:
	CDBPasswords();
	~CDBPasswords();

	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CDBPasswords)
	enum { IDD = IDD_DBPASSWORDS };
	CButton	m_CntlCoreEnable;
	CEdit	m_CntlCorePassword2;
	CEdit	m_CntlCorePassword;
	CEdit	m_CntlAppPassword2;
	CEdit	m_CntlAppPassword;
	CEdit	m_CntlAppoldPassword;
	CEdit	m_CntlCoreOldPassword;
	CButton	m_CntlAppEnable;
	CEdit	m_CntlCoreUserFld;
	CEdit	m_CntlAppUserFld;
	CString	m_StrAppUserFld;
	CString	m_StrCoreUserFld;
	CString	m_StrAppPassword;
	CString	m_StrConfirmAppPassword;
	CString	m_StrCorePassword;
	CString	m_StrConfirmCorePassword;
	CString	m_StrMessageLbl;
	CString	m_StrAppOldPassword;
	CString	m_StrCoreOldPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDBPasswords)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	virtual BOOL CheckDialogForCompleteness();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDBPasswords)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCorePassword();
	afx_msg void OnChangeAppPassword();
	afx_msg void OnChangeAppOldPassword();
	afx_msg void OnChangeCoreOldPassword();
	afx_msg void OnChangeAppPassword2();
	afx_msg void OnChangeCorePassword2();
	afx_msg void OnClose();
	afx_msg void OnCheckCoreenable();
	afx_msg void OnCheckAppenable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif 
