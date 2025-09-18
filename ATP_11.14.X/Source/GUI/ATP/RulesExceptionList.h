#if !defined(AFX_RULESEXCEPTIONLIST_H__08E04B61_01DE_11D4_976F_0008C789BA2A__INCLUDED_)
#define AFX_RULESEXCEPTIONLIST_H__08E04B61_01DE_11D4_976F_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RULESEXCEPTIONLIST.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRulesDefinitionExceptionList dialog

class CRulesDefinitionExceptionList : public CPropertyPage
{
	DECLARE_DYNCREATE(CRulesDefinitionExceptionList)

// Construction
public:
	CRulesDefinitionExceptionList();
	~CRulesDefinitionExceptionList();

   void CleanCardBlockingDialog() ;
   void PopulateCardBlockingDialogFromDatabase(CR01 );
   void PopulateDatabaseStructurefromCardBlockingDialog( pCR01 );
   void CompareStructures( CR01, CR01 );

   CR01 sCR01Record ;
   int nOldCount;
   CTime c_time;

// Dialog Data
	//{{AFX_DATA(CBinBlocking)
	enum { IDD = IDD_RULEXP_CARDNUM_BLOCKING };
	CListBox	m_List;
	CEdit	m_CntlExceptionCardNumFld;
	CButton	m_Add;
	CButton	m_Delete;
	CString	m_StrExceptionCardNumFld;
	CString	m_StrMessageLbl;
	CString	m_StrRuleID_Fld;
	//}}AFX_DATA

   int m_nDialogMode;
   BOOL m_bExceptionCardNum;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinBlocking)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinBlocking)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BINBLOCKING_H__08E04B61_01DE_11D4_976F_0008C789BA2A__INCLUDED_)
