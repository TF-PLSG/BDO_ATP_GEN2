#if !defined(AFX_JCBUPDATES_H__29A10ED1_D872_11D5_A384_009027B1792B__INCLUDED_)
#define AFX_JCBUPDATES_H__29A10ED1_D872_11D5_A384_009027B1792B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JCBUPDATES.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJCBUPDATES dialog

class CJCBUPDATES : public CDialog
{
// Construction
public:
	CJCBUPDATES(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CJCBUPDATES)
	enum { IDD = IDD_JCBUPDATES };
	CListCtrl	m_CntlIquiryResults;
	CListBox	m_cntlInquiryResults;
	CButton	m_cntlExitButton;
	CButton	m_cntlSubmitButton;
	CEdit	m_cntlAccountNbr;
	CComboBox	m_cntlActionCode;
	CComboBox	m_cntlRequestType;
	CButton	m_cntlStopRegions;
	CEdit	m_cntlRespText;
	CEdit	m_cntlPurgeDate;
	CEdit	m_cntlRespCode;
	CButton	m_cntlUSA;
	CButton	m_cntlLocal;
	CButton	m_cntlJapan;
	CButton	m_cntlEurope;
	CButton	m_cntlCkDigit;
	CButton	m_cntlAsia;
	BOOL	m_Asia;
	BOOL	m_CkDigit;
	BOOL	m_Europe;
	BOOL	m_Japan;
	BOOL	m_Local;
	CString	m_strMessageLbl;
	BOOL	m_USA;
	CString	m_strRespCode;
	CString	m_strPurgeDate;
	CString	m_strRespText;
	BOOL	m_StopRegions;
	CString	m_strRequestType;
	CString	m_strActionCode;
	CString	m_strInquiryResults;
	CString	m_strAccountNbr;
	//}}AFX_DATA


	int m_nDialogMode;
	BOOL PopulateDatabaseStructure();
	BOOL CheckDialogForCompleteness();

	JCB_ONLINE_UPDATES sJCBRecord;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJCBUPDATES)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJCBUPDATES)
	virtual BOOL OnInitDialog();
	afx_msg void OnSubmitButton();
	afx_msg void OnExitButton();
	afx_msg void OnKillfocusAccountNumberFld();
	afx_msg void OnKillfocusRequestType();
	afx_msg void OnStopRegions();
	afx_msg void OnKillfocusActionCode();
	afx_msg void OnKillfocusPurgeDateFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JCBUPDATES_H__29A10ED1_D872_11D5_A384_009027B1792B__INCLUDED_)
