#if !defined(AFX_LOGINFRM_H__B907DA8B_4EAB_498A_AA0A_0F4188F6580E__INCLUDED_)
#define AFX_LOGINFRM_H__B907DA8B_4EAB_498A_AA0A_0F4188F6580E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Loginfrm.h : header file
//
#define CANCEL 2
/////////////////////////////////////////////////////////////////////////////
// CLoginfrm dialog

class CLoginfrm : public CDialog
{
// Construction
public:
	CLoginfrm(CWnd* pParent = NULL);   // standard constructor
	short CheckLoginDetails();

// Dialog Data
	//{{AFX_DATA(CLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CString	m_userid;
	CString	m_userpwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginfrm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginfrm)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINFRM_H__B907DA8B_4EAB_498A_AA0A_0F4188F6580E__INCLUDED_)
