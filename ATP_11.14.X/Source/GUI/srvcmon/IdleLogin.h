#if !defined(AFX_IDLELOGIN_H__A1A00837_72B1_4E9C_A652_B656190EC178__INCLUDED_)
#define AFX_IDLELOGIN_H__A1A00837_72B1_4E9C_A652_B656190EC178__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IdleLogin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIdleLogin dialog

class CIdleLogin : public CDialog
{
// Construction
public:
	CIdleLogin(CWnd* pParent = NULL);   // standard constructor
	short CheckLoginDetails();
// Dialog Data
	//{{AFX_DATA(CIdleLogin)
	enum { IDD = IDD_DIALOG1 };
	CString	m_userpwd;
	CString	m_userid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdleLogin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIdleLogin)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDLELOGIN_H__A1A00837_72B1_4E9C_A652_B656190EC178__INCLUDED_)
