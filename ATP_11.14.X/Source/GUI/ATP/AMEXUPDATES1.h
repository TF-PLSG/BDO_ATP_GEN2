#if !defined(AFX_AMEXUPDATES1_H__C1B795B1_1B77_11D8_A44D_009027B1792B__INCLUDED_)
#define AFX_AMEXUPDATES1_H__C1B795B1_1B77_11D8_A44D_009027B1792B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AMEXUPDATES1.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CAMEXUPDATES dialog

class CAMEXUPDATES : public CDialog
{
// Construction
public:
	CAMEXUPDATES(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAMEXUPDATES();

// Dialog Data
	//{{AFX_DATA(CAMEXUPDATES)
	enum { IDD = IDD_AMEXUPDATES };
	CEdit	m_cntlDestQue;
	CEdit	m_cntlAcctNbr;
	CListBox	m_cntlTranInfo;
	CEdit	m_cntlResponseText;
	CEdit	m_cntlResponseCode;
	CStatic	m_cntlMessageLbl;
	CButton	m_cntlExitButton;
	CButton	m_cntlSubmitButton;
	CEdit	m_cntlExpireDate;
	CComboBox	m_cntlRequestType;
	CComboBox	m_cntlActionCode;
	CButton	m_cntlCkDigit;
	CString	m_strAcctNbr;
	BOOL	m_CkDigit;
	CString	m_strActionCode;
	CString	m_strRequestType;
	CString	m_strExpireDate;
	CString	m_strMessageLbl;
	CString	m_strResponseCode;
	CString	m_strResponseText;
	CString	m_strTranInfo;	
	CString	m_strDestQue;
	
	//}}AFX_DATA

	
	int m_nDialogMode;
	
	BOOL PopulateDatabaseStructure();
	BOOL CheckDialogForCompleteness();

	CTime c_time;
	

#define	RECORD_ADD_UPDATE	"301"
#define RECORD_DELETE		"303"

	AMEX_ONLINE_UPDATES sAMEXRecord;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMEXUPDATES)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAMEXUPDATES)
	afx_msg void OnExitButton();
	afx_msg void OnKillfocusAccountNumberFld();
	afx_msg void OnKillfocusActionCode();
	afx_msg void OnKillfocusExpireDateFld();
	afx_msg void OnKillfocusRequestType();
	afx_msg void OnSubmitButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//CDMLock * pLock;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMEXUPDATES1_H__C1B795B1_1B77_11D8_A44D_009027B1792B__INCLUDED_)
