#if !defined(AFX_IDLELOGINFRM_H__1FE0BE22_F277_487A_8CFF_498B6AD28E09__INCLUDED_)
#define AFX_IDLELOGINFRM_H__1FE0BE22_F277_487A_8CFF_498B6AD28E09__INCLUDED_

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
	//short CheckLoginDetails();
// Dialog Data
	//{{AFX_DATA(CIdleLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CEdit	m_upwd;
	CEdit	m_uid;
	//}}AFX_DATA
	CString	m_userpwd;
	CString	m_userid;
	CString m_szTitle;
	BOOL m_bDisableNameFld;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdleLoginfrm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
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

#endif // !defined(AFX_IDLELOGINFRM_H__1FE0BE22_F277_487A_8CFF_498B6AD28E09__INCLUDED_)
