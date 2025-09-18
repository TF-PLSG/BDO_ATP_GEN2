#if !defined(AFX_MAINMENUDLG_H__DDB5AA60_6E30_4244_B669_189F5F424FB6__INCLUDED_)
#define AFX_MAINMENUDLG_H__DDB5AA60_6E30_4244_B669_189F5F424FB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainMenuDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainMenuDlg dialog

class CMainMenuDlg : public CDialog
{
// Construction
public:
	CMainMenuDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMainMenuDlg)
	enum { IDD = IDD_MAINMENU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainMenuDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainMenuDlg)
	afx_msg void OnTlf01menu();
	afx_msg void OnBch01menu();
	afx_msg void OnBch11menu();
	afx_msg void OnBch20menu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINMENUDLG_H__DDB5AA60_6E30_4244_B669_189F5F424FB6__INCLUDED_)
