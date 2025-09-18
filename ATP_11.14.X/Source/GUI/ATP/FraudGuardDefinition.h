#if !defined(AFX_FRAUDGUARDDEFINITION_H__02114134_6107_420C_B9F0_B10AFA4AF9BB__INCLUDED_)
#define AFX_FRAUDGUARDDEFINITION_H__02114134_6107_420C_B9F0_B10AFA4AF9BB__INCLUDED_

#include "FraudGuardGeneral.h"
#include "ChangeHistory.h"
#include "MyUtils.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FraudGuardDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardDefinition dialog

class CFraudGuardDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CFraudGuardDefinition)

// Construction
public:
	CFraudGuardDefinition();
	~CFraudGuardDefinition();
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CFraudGuardDefinition)
	enum { IDD = IDD_FGUARD_DEFINITION };
	CStatic	m_CntlMessageLbl;
	CEdit	m_CntlType;
	CEdit	m_CntlNameId;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrNameId;
	CString	m_StrType;
	CString	m_StrMessageLbl;
	//}}AFX_DATA

	int m_nDialogMode;

	char m_strCurrentData[151];
	char strFilePrimaryKey[30];

	FG01 sFGuardRecordOld;

//	BOOL PopulateDatabaseFromFGuardDialog( pFG01 );
//	BOOL PopulateFraudGuardDialogFromDatabase( FG01 );
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();



	CFraudGuardGeneral*pGeneralPage;
    CChangeHistory *pChangeHistoryPage;




// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFraudGuardDefinition)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFraudGuardDefinition)
	afx_msg void OnFguardAddButton();
	afx_msg void OnFguardDeleteButton();
	afx_msg void OnFguardListButton();
	afx_msg void OnFguardUpdateButton();
	afx_msg void OnFguardViewButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAUDGUARDDEFINITION_H__02114134_6107_420C_B9F0_B10AFA4AF9BB__INCLUDED_)
