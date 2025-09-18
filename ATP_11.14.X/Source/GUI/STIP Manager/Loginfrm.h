#if !defined(AFX_LOGINFRM_H__E4C9A1F1_781B_4210_A920_36F0AB1BDD23__INCLUDED_)
#define AFX_LOGINFRM_H__E4C9A1F1_781B_4210_A920_36F0AB1BDD23__INCLUDED_

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
	BOOL isSpace(pCHAR string);
	BOOL isalphabet(pCHAR string);
	BOOL isnumeric(pCHAR string);
// Dialog Data
	//{{AFX_DATA(CLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CString	m_userpwd;
	CString	m_userid;
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
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINFRM_H__E4C9A1F1_781B_4210_A920_36F0AB1BDD23__INCLUDED_)
