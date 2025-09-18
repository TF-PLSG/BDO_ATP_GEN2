#if !defined(AFX_OVERRIDEPASSWORD_H__51EF3A36_6145_11D7_A40D_009027B1792B__INCLUDED_)
#define AFX_OVERRIDEPASSWORD_H__51EF3A36_6145_11D7_A40D_009027B1792B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverridePassword.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COverridePassword dialog

class COverridePassword : public CDialog
{
// Construction
public:
	COverridePassword(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COverridePassword)
	enum { IDD = IDD_OVERRIDE_PASSWORD };
	CEdit	m_ctrlOverridepassword;
	CString	m_strOverridePassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COverridePassword)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

#define STR1 "3E4SSU81"
#define STR2 "A4P456YY"
#define STR3 "13W7R561"
#define STR4 "AH0VII0B"

#define DEF_STR1 "B18DEA49"
#define DEF_STR2 "7W230VF1"
#define DEF_STR3 "RP2Q623R"
#define DEF_STR4 "0EDI45SO"

#define NOT_ECN_STR1 "3E4OFU81"
#define NOT_ECN_STR2 "AF7456YY"
#define NOT_ECN_STR3 "13W77U61"
#define NOT_ECN_STR4 "AH0VIS0B"

	// Generated message map functions
	//{{AFX_MSG(COverridePassword)
	virtual void OnCancel();
	virtual void OnOK();
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERRIDEPASSWORD_H__51EF3A36_6145_11D7_A40D_009027B1792B__INCLUDED_)
