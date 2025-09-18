#if !defined(AFX_PROCESSDIALOG_H__BC5BE035_AD4A_40B3_B12F_34263ABA33DB__INCLUDED_)
#define AFX_PROCESSDIALOG_H__BC5BE035_AD4A_40B3_B12F_34263ABA33DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessDialog.h : header file
//

#define WM_MSG1	WM_USER+1


/////////////////////////////////////////////////////////////////////////////
// CProcessDialog thread

class CProcessDialog : public CWinThread
{
	DECLARE_DYNCREATE(CProcessDialog)
protected:
	CProcessDialog();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessDialog)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProcessDialog();

	// Generated message map functions
	//{{AFX_MSG(CProcessDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void HandleThreadMessage(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSDIALOG_H__BC5BE035_AD4A_40B3_B12F_34263ABA33DB__INCLUDED_)
