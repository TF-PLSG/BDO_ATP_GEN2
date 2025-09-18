#if !defined(AFX_USERAUDITLOG1_H__8E82980E_0295_4193_B478_ED62898F3054__INCLUDED_)
#define AFX_USERAUDITLOG1_H__8E82980E_0295_4193_B478_ED62898F3054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserAuditLog1.h : header file
//
#include <afxdtctl.h>
/////////////////////////////////////////////////////////////////////////////
// CUserAuditLog dialog

class CUserAuditLog : public CDialog
{
// Construction
public:
	CUserAuditLog(CWnd* pParent = NULL);   // standard constructor
	int				GetTotalLogs(CStringArray &strArr);
	int				GetUserRecord(CString username);
    HST01_LIST		sHistoryList;
	HST01			sHistoryRecord;
	FILE			*m_pFile;
	UCF01			m_sUserRecord;
	unsigned long	m_lTotalRecords;
	unsigned long	m_lRecordsWritten;
	BOOL			LoadUsers();
	void  Mask_Fields(pCHAR fields);
	void Expand_Permns_from_num_to_strings();  // Function converts the permission 1111 to Add_Delete_Update_View Only 
// Dialog Data
	//{{AFX_DATA(CUserAuditLog)
	enum { IDD = IDD_USER_AUDIT_LOG_DLG };
	CComboBox		m_CntlUserList;
	CDateTimeCtrl	m_CntlToDate;
	CDateTimeCtrl	m_CntlFromDate;
	CButton			m_CntlStartButton;
	CButton			m_CntlCancelButton;
	CStatic			m_CntlStatus;
	CString			m_StrStatus;
	CTime			m_timeFromDate;
	CTime			m_timeToDate;
	CString			m_szSelectedUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserAuditLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserAuditLog)
	afx_msg void OnStart();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERAUDITLOG1_H__8E82980E_0295_4193_B478_ED62898F3054__INCLUDED_)
