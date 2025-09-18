#if !defined(AFX_STATUSSPLASHWINDOW_H__902880E5_2DE1_4275_8778_B45F3DFC31F0__INCLUDED_)
#define AFX_STATUSSPLASHWINDOW_H__902880E5_2DE1_4275_8778_B45F3DFC31F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusSplashWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusSplashWindow dialog

class CStatusSplashWindow : public CDialog
{
// Construction
public:
	CStatusSplashWindow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStatusSplashWindow)
	enum { IDD = IDD_SPLASH };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusSplashWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatusSplashWindow)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSSPLASHWINDOW_H__902880E5_2DE1_4275_8778_B45F3DFC31F0__INCLUDED_)
