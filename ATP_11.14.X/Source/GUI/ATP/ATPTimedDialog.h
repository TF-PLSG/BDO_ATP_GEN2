#if !defined(AFX_ATPTIMEDDIALOG_H__CFB98D4B_3BFF_4738_9FF6_EF2E009D019C__INCLUDED_)
#define AFX_ATPTIMEDDIALOG_H__CFB98D4B_3BFF_4738_9FF6_EF2E009D019C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ATPTimedDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CATPTimedDialog dialog

class CATPTimedDialog : public CDialog
{
// Construction
public:
	CATPTimedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CATPTimedDialog)
	enum { IDD = IDD_TIMEDDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATPTimedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CATPTimedDialog)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATPTIMEDDIALOG_H__CFB98D4B_3BFF_4738_9FF6_EF2E009D019C__INCLUDED_)
