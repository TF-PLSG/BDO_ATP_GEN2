// DBFToolWindowsDlg.h : header file
//

#if !defined(AFX_DBFTOOLWINDOWSDLG_H__007AC531_7D6F_402D_8C02_CACE959B59FB__INCLUDED_)
#define AFX_DBFTOOLWINDOWSDLG_H__007AC531_7D6F_402D_8C02_CACE959B59FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDBFToolWindowsDlg dialog

class CDBFToolWindowsDlg : public CDialog
{
// Construction
public:
	CDBFToolWindowsDlg(CWnd* pParent = NULL);	// standard constructor

	CString m_szFileExt;

// Dialog Data
	//{{AFX_DATA(CDBFToolWindowsDlg)
	enum { IDD = IDD_DBFTOOLWINDOWS_DIALOG };
	CButton	m_CntlGetVersion;
	CButton	m_CntlDecrypt;
	CButton	m_CntlOpenFile;
	CButton	m_CntlIsKeyEcnrypted;
	CStatic	m_CntlStatus;
	CEdit	m_CntlDesKey;
	CEdit	m_CntlDbfName;
	CString	m_szDbfName;
	CString	m_szDesKey;
	CString	m_szStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFToolWindowsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDBFToolWindowsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	virtual void OnOK();
	afx_msg void OnChangeDesKey();
	afx_msg void OnOpenfile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnGetVersion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_DBFTOOLWINDOWSDLG_H__007AC531_7D6F_402D_8C02_CACE959B59FB__INCLUDED_)
