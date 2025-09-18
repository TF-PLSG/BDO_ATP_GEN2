#if !defined(AFX_BINMCCBLOCKING_H__D4244681_A162_11D5_9F3C_000102CD77DE__INCLUDED_)
#define AFX_BINMCCBLOCKING_H__D4244681_A162_11D5_9F3C_000102CD77DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BinMCCBlocking.h : header file/

/////////////////////////////////////////////////////////////////////////////
// CBinMCCBlocking dialog
class CBinMCCBlocking : public CPropertyPage
{
	DECLARE_DYNCREATE(CBinMCCBlocking)

// Construction
public:
	CBinMCCBlocking();
	~CBinMCCBlocking();

	void CleanBlockingDialog() ;
   BOOL PopulateBinDialogFromDatabase(BIN01 );
   BOOL PopulateDatabaseStructure( pBIN01 );
   BOOL CompareStructures( BIN01);
   BOOL CheckDialogForCompleteness();
   void PopulateHistoryTable();
   void EnableBlockingDialog ( int );

   BIN01 sBinRecord;
    char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	int m_nDialogMode;
    BOOL m_bMCCBlocking;

	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CBinMCCBlocking)
	enum { IDD = IDD_BIN_MCC_BLOCKING };
	CEdit	m_CntlLowRange9;
	CEdit	m_CntlLowRange8;
	CEdit	m_CntlLowRange7;
	CEdit	m_CntlLowRange6;
	CEdit	m_CntlLowRange5;
	CEdit	m_CntlLowRange4;
	CEdit	m_CntlLowRange3;
	CEdit	m_CntlLowRange2;
	CEdit	m_CntlLowRange10;
	CEdit	m_CntlLowRange1;
	CEdit	m_CntlHighRange9;
	CEdit	m_CntlHighRange8;
	CEdit	m_CntlHighRange7;
	CEdit	m_CntlHighRange6;
	CEdit	m_CntlHighRange5;
	CEdit	m_CntlHighRange4;
	CEdit	m_CntlHighRange3;
	CEdit	m_CntlHighRange2;
	CEdit	m_CntlHighRange10;
	CEdit	m_CntlHighRange1;
	CString	m_StrBinLengthFld1;
	CString	m_StrBinLowFld1;
	CString	m_StrHighRange1;
	CString	m_StrHighRange10;
	CString	m_StrHighRange2;
	CString	m_StrHighRange3;
	CString	m_StrHighRange4;
	CString	m_StrHighRange5;
	CString	m_StrHighRange6;
	CString	m_StrHighRange7;
	CString	m_StrHighRange8;
	CString	m_StrHighRange9;
	CString	m_StrLowRange1;
	CString	m_StrLowRange10;
	CString	m_StrLowRange2;
	CString	m_StrLowRange3;
	CString	m_StrLowRange4;
	CString	m_StrLowRange5;
	CString	m_StrLowRange6;
	CString	m_StrLowRange7;
	CString	m_StrLowRange8;
	CString	m_StrLowRange9;
	CString	m_StrMccBinTypeFld1;
	CString	m_StrMessageLbl;
	//}}AFX_DATA



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinMCCBlocking)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinMCCBlocking)
	virtual BOOL OnInitDialog();
	/*afx_msg BOOL OnKillfocusHighRange1();
	afx_msg BOOL OnKillfocusHighRange2();
	afx_msg BOOL OnKillfocusHighRange3();
	afx_msg BOOL OnKillfocusHighRange4();
	afx_msg BOOL OnKillfocusHighRange5();
	afx_msg BOOL OnKillfocusHighRange6();
	afx_msg BOOL OnKillfocusHighRange7();
	afx_msg BOOL OnKillfocusHighRange8();
	afx_msg BOOL OnKillfocusHighRange9();
	afx_msg BOOL OnKillfocusHighRange10();*/

	afx_msg void OnKillfocusHighRange1();
	afx_msg void OnKillfocusHighRange2();
	afx_msg void OnKillfocusHighRange3();
	afx_msg void OnKillfocusHighRange4();
	afx_msg void OnKillfocusHighRange5();
	afx_msg void OnKillfocusHighRange6();
	afx_msg void OnKillfocusHighRange7();
	afx_msg void OnKillfocusHighRange8();
	afx_msg void OnKillfocusHighRange9();
	afx_msg void OnKillfocusHighRange10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BINMCCBLOCKING_H__D4244681_A162_11D5_9F3C_000102CD77DE__INCLUDED_)
