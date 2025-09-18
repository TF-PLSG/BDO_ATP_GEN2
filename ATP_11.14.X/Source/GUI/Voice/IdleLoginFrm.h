#if !defined(AFX_IDLELOGINFRM1_H__91CB052C_00A0_48B4_86A5_DBADEF78DF95__INCLUDED_)
#define AFX_IDLELOGINFRM1_H__91CB052C_00A0_48B4_86A5_DBADEF78DF95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IdleLoginfrm.h : header file
//
#define CANCEL 2
/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog

class CIdleLoginfrm : public CDialog
{
// Construction
public:
	CIdleLoginfrm(CWnd* pParent = NULL);   // standard constructor
	short CheckLoginDetails();
	CString accountstatus;
// Dialog Data
	//{{AFX_DATA(CIdleLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CEdit	m_CntlUID;
	CString	m_userpwd;
	CString	m_userid;
	CString m_szTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdleLoginfrm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIdleLoginfrm)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDLELOGINFRM1_H__91CB052C_00A0_48B4_86A5_DBADEF78DF95__INCLUDED_)
