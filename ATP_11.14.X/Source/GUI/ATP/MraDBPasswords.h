#if !defined(AFX_MRADBPASSWORDS_H__C1A57555_1BA9_457A_9C4A_A41DE957270B__INCLUDED_)
#define AFX_MRADBPASSWORDS_H__C1A57555_1BA9_457A_9C4A_A41DE957270B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MraDBPasswords.h : header file
//
#define DB_MRA_PASSWORD_DATA 185

/////////////////////////////////////////////////////////////////////////////
// CMraDBPasswords dialog

class CMraDBPasswords : public CDialog
{
// Construction
public:
	CMraDBPasswords(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMraDBPasswords)
	enum { IDD = IDD_DBPASSWORDS_MRA };
	CEdit	m_CntlConfirmPassword;
	CEdit	m_CntlUserId;
	CEdit	m_CntlNewPassword;
	CEdit	m_CntlOldPassword;
	CString	m_StrOldPassword;
	CString	m_StrNewPassword;
	CString	m_StrUserId;
	CString	m_StrConfirmPassword;
	CString	m_StrMessageLbl;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMraDBPasswords)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMraDBPasswords)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MRADBPASSWORDS_H__C1A57555_1BA9_457A_9C4A_A41DE957270B__INCLUDED_)
