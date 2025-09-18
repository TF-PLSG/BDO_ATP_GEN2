#if !defined(AFX_STATISTICSDLG_H__30687E2B_3805_4030_9892_4DD0CCD8AC4D__INCLUDED_)
#define AFX_STATISTICSDLG_H__30687E2B_3805_4030_9892_4DD0CCD8AC4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatisticsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDlg dialog

class CStatisticsDlg : public CDialog
{
// Construction
public:
	CString m_StrSent;
	CString m_StrRecv;
public:
	CStatisticsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStatisticsDlg)
	enum { IDD = IDD_STATISTIC_DIALOG };
	CString	m_StrTotalRecv;
	CString	m_StrTotalSent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatisticsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATISTICSDLG_H__30687E2B_3805_4030_9892_4DD0CCD8AC4D__INCLUDED_)
