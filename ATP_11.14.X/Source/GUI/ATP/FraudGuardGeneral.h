#if !defined(AFX_FRAUDGUARDGENERAL_H__BF479014_DA2A_4BCE_9404_A639B7944FD5__INCLUDED_)
#define AFX_FRAUDGUARDGENERAL_H__BF479014_DA2A_4BCE_9404_A639B7944FD5__INCLUDED_

#include "MyUtils.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FraudGuardGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardGeneral dialog

class CFraudGuardGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CFraudGuardGeneral)

// Construction
public:
	CFraudGuardGeneral();
	~CFraudGuardGeneral();

	FG01 sFGuard;
	FG01 sFGuardOld;

	int m_nDialogMode;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	BOOL m_bGeneral;

	void CleanGeneralDialog();
    void EnableGeneralDialog(int);
    void PopulateHistoryTable();
	BOOL PopulateDatabaseFromFGuardDialog( pFG01 );
	BOOL PopulateFraudGuardDialogFromDatabase( FG01 );
	BOOL CompareStructures();
	
	
	CTime c_time;



// Dialog Data
	//{{AFX_DATA(CFraudGuardGeneral)
	enum { IDD = IDD_FGUARD_GENERAL };
	CComboBox	m_CntlApplicationCombo;
	CEdit	m_CntlUserNameFld;
	CEdit	m_CntlTypeFld;
	CEdit	m_CntlTpduFld;
	CEdit	m_CntlSourceFld;
	CEdit	m_CntlRuleDomainFld;
	CEdit	m_CntlRequestTimerFld;
	CEdit	m_CntlQueNameFld;
	CEdit	m_CntlProductFld;
	CEdit	m_CntlPriorityFld;
	CEdit	m_CntlNameId;
	CStatic	m_CntlMessageLbl;
	CEdit	m_CntlEchoTimerFld;
	CEdit	m_CntlCallerIdFld;
	CString	m_StrCallerIdFld;
	CString	m_StrEchoTimerFld;
	CString	m_StrMessageLbl;
	CString	m_StrNameId;
	CString	m_StrPriorityFld;
	CString	m_StrProductFld;
	CString	m_StrQueNameFld;
	CString	m_StrRequestTimerFld;
	CString	m_StrRuleDomainFld;
	CString	m_StrSourceFld;
	CString	m_StrTpduFld;
	CString	m_StrTypeFld;
	CString	m_StrUserNameFld;
	CString	m_StrApplicationCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFraudGuardGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFraudGuardGeneral)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAUDGUARDGENERAL_H__BF479014_DA2A_4BCE_9404_A639B7944FD5__INCLUDED_)
