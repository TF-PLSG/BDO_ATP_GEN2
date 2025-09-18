// EFTPosCorrectionDlg.h : header file
//

#if !defined(AFX_EFTPOSCORRECTIONDLG_H__1AC6E997_D1BE_4976_ADD8_8A0FB72E0A58__INCLUDED_)
#define AFX_EFTPOSCORRECTIONDLG_H__1AC6E997_D1BE_4976_ADD8_8A0FB72E0A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEFTPosCorrectionDlg dialog

class CEFTPosCorrectionDlg : public CDialog
{
// Construction
public:
	CEFTPosCorrectionDlg(CWnd* pParent = NULL);	// standard constructor
void GetFileNameFromDialog(CString &szFile, CString strFilter);

// Dialog Data
	//{{AFX_DATA(CEFTPosCorrectionDlg)
	enum { IDD = IDD_EFTPOSCORRECTION_DIALOG };
	CString	m_StrFileName;
	CString	m_Status;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEFTPosCorrectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEFTPosCorrectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFTPOSCORRECTIONDLG_H__1AC6E997_D1BE_4976_ADD8_8A0FB72E0A58__INCLUDED_)
