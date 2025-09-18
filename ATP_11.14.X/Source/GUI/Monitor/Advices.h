#if !defined(AFX_ADVICES_H__2D7036C1_FF5B_11D7_975D_009027B62FD5__INCLUDED_)
#define AFX_ADVICES_H__2D7036C1_FF5B_11D7_975D_009027B62FD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Advices.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Advices dialog

class Advices : public CDialog
{
// Construction
public:
	Advices(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Advices)
	enum { IDD = IDD_ADVICES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Advices)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Advices)
	virtual BOOL OnInitDialog();
	afx_msg void OnExit();
	afx_msg void OnSaf();
	afx_msg void OnStartAdvices();
	afx_msg void OnStopAdvices();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVICES_H__2D7036C1_FF5B_11D7_975D_009027B62FD5__INCLUDED_)
