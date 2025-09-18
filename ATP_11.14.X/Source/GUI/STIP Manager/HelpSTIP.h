#if !defined(AFX_HELPSTIP_H__ADBF2D7B_DE68_4C44_8031_0C50AA776EE2__INCLUDED_)
#define AFX_HELPSTIP_H__ADBF2D7B_DE68_4C44_8031_0C50AA776EE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpSTIP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelpSTIP dialog

class CHelpSTIP : public CDialog
{
// Construction
public:
	CHelpSTIP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelpSTIP)
	enum { IDD = IDD_DIALOG_HELP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpSTIP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelpSTIP)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPSTIP_H__ADBF2D7B_DE68_4C44_8031_0C50AA776EE2__INCLUDED_)
