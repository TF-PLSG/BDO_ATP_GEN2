#if !defined(AFX_TPSRMAINDLG_H__CBF26212_CD05_4094_BAD5_70DA629616DE__INCLUDED_)
#define AFX_TPSRMAINDLG_H__CBF26212_CD05_4094_BAD5_70DA629616DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TPSRMainDlg.h : header file
//
#include "StatisticsDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CTPSRMainDlg dialog

class CTPSRMainDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CTPSRMainDlg)
// Construction
protected:
	CStatisticsDlg m_StatisticsDlg;
	BOOL m_bRunning;
	HWND m_hConsoleHandle;
	BOOL m_bShowConsole;
public:
	STARTUPINFO m_si;
	PROCESS_INFORMATION m_pi;

	CTPSRMainDlg(CWnd* pParent = NULL);   // standard constructor
	//int m_devIds[999999];
	CUIntArray m_DevIds;
	CUIntArray m_StanIds;
// Dialog Data
	//{{AFX_DATA(CTPSRMainDlg)
	enum { IDD = IDD_TPSR_MAIN };
	CButton	m_CntlShowConsole;
	CEdit	m_CntlResRecv;
	CEdit	m_CntlTxnSent;
	CStatic	m_CntlStaticSent;
	CStatic	m_CntlStaticRecv;
	CButton	m_CntlResetCounters;
	CButton	m_CntlStop;
	CButton	m_CntlRun;
	CString	m_StrSent;
	CString	m_StrReceived;
	CString	m_StrToolStatus;
	CString	m_StrResRecv;
	CString	m_StrTxnSent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPSRMainDlg)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTPSRMainDlg)
	afx_msg void OnRun();
	virtual BOOL OnInitDialog();
	afx_msg void OnStop();
	afx_msg void OnResetcounter();
	afx_msg void OnExit();
	afx_msg void OnShowConsole();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPSRMAINDLG_H__CBF26212_CD05_4094_BAD5_70DA629616DE__INCLUDED_)
