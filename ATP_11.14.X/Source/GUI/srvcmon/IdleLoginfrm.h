#if !defined(AFX_LOGINFRM_H__1C6CCE66_DD1E_4D16_8AB0_5DB299085C8F__INCLUDED_)
#define AFX_LOGINFRM_H__1C6CCE66_DD1E_4D16_8AB0_5DB299085C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Loginfrm.h : header file
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
// Dialog Data
	//{{AFX_DATA(CIdleLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CString	m_userpwd;
	CString	m_userid;
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
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINFRM_H__1C6CCE66_DD1E_4D16_8AB0_5DB299085C8F__INCLUDED_)
