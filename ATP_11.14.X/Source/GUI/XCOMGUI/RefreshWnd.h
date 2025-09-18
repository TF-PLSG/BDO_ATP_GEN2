#if !defined(AFX_REFRESHWND_H__773129E0_F1D1_49DB_8487_2DA74F019E06__INCLUDED_)
#define AFX_REFRESHWND_H__773129E0_F1D1_49DB_8487_2DA74F019E06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RefreshWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRefreshWnd dialog

class CRefreshWnd : public CDialog
{
// Construction
public:
	CRefreshWnd(CWnd* pParent = NULL);   // standard constructor

	//User Defined Objects	
	CEdit Refresh_Edit_Box;
	char Refresh_Val[10];

// Dialog Data
	//{{AFX_DATA(CRefreshWnd)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRefreshWnd)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRefreshWnd)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REFRESHWND_H__773129E0_F1D1_49DB_8487_2DA74F019E06__INCLUDED_)
