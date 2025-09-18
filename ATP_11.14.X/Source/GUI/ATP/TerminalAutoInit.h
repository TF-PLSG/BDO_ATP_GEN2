#if !defined(AFX_TERMINALAUTOINIT_H__E532B9E3_7E6A_465D_8A9E_856605F6F9B7__INCLUDED_)
#define AFX_TERMINALAUTOINIT_H__E532B9E3_7E6A_465D_8A9E_856605F6F9B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerminalAutoInit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalAutoInit dialog

class CTerminalAutoInit : public CPropertyPage
{
	DECLARE_DYNCREATE(CTerminalAutoInit)

// Construction
public:
	CTerminalAutoInit();
	~CTerminalAutoInit();

	int m_nDialogMode;
	BOOL m_bAutoInit;
    BOOL m_bOpen;

	void EnableAutoInitDialog (int);
	void CleanAutoInitDialog();
	BOOL CompareStructures(DCF01);
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromAutoInitDialog ( pDCF01 );
	BOOL PopulateAutoInitDialogFromDatabase ( DCF01 );
	BOOL CheckTime (CString);

	DCF01 sTerminalRecordOld;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	CString strAutoInitAct;
	CString strAutoLoadAct;
	CString strAutoIcepacAct;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CTerminalAutoInit)
	enum { IDD = IDD_TERMINAL_AUTOINIT };
	CEdit	m_CntlAutoInitRef;
	CEdit	m_CntlAutoLoadTime;
	CEdit	m_CntlAutoLoadRef;
	CEdit	m_CntlAutoInitTime;
	CButton	m_CntlAutoLoadCheck;
	CButton	m_CntlAutoInitCheck;
	CButton	m_CntlAutoIcepacCheck;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrTerminalIDFld;
	CString	m_StrAutoInitTime;
	CString	m_StrAutoLoadRef;
	CString	m_StrAutoLoadTime;
	CString	m_StrAutoInitRef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTerminalAutoInit)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTerminalAutoInit)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusAutoLoadTime();
	afx_msg void OnKillfocusAutoInitTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALAUTOINIT_H__E532B9E3_7E6A_465D_8A9E_856605F6F9B7__INCLUDED_)
