#if !defined(AFX_ARCHIVEDBPASSWORDS_H__27ED190D_5FBA_4B0C_8414_04774B52310B__INCLUDED_)
#define AFX_ARCHIVEDBPASSWORDS_H__27ED190D_5FBA_4B0C_8414_04774B52310B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveDBPasswords.h : header file
//
#define DB_ARCH_PASSWORD_DATA 184
/////////////////////////////////////////////////////////////////////////////
// CArchiveDBPasswords dialog

class CArchiveDBPasswords : public CDialog
{
// Construction
public:
	CArchiveDBPasswords(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArchiveDBPasswords)
	enum { IDD = IDD_DBPASSWORDS_ARCH };
	CEdit	m_CntlConfirmPassword;
	CEdit	m_CntlUserId;
	CEdit	m_CntlNewPassword;
	CEdit	m_CntlOldPassword;
	CString	m_StrOldPassword;
	CString	m_StrNewPassword;
	CString	m_StrUserId;
	CString	m_StrConfirmPassword;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveDBPasswords)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchiveDBPasswords)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVEDBPASSWORDS_H__27ED190D_5FBA_4B0C_8414_04774B52310B__INCLUDED_)
