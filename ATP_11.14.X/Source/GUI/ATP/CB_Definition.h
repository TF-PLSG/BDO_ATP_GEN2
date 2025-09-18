#if !defined(AFX_CB_DEFINITION_H__EE0237CF_64F1_4D80_B1B1_A324176A0DA9__INCLUDED_)
#define AFX_CB_DEFINITION_H__EE0237CF_64F1_4D80_B1B1_A324176A0DA9__INCLUDED_

#include "CB_General.h"
#include "CB_Amounts.h"
#include "ChangeHistory.h"
#include "MyUtils.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CB_Definition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCB_Definition dialog

class CCB_Definition : public CPropertyPage
{
	DECLARE_DYNCREATE(CCB_Definition)

// Construction
public:
	CCB_Definition();
	~CCB_Definition();

	CTime c_time;	

// Dialog Data
	//{{AFX_DATA(CCB_Definition)
	enum { IDD = IDD_CB_DEFINITION };
	CEdit	m_CntlCashBonusId;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_DeleteButton;
	CButton	m_ListButton;
	CButton	m_AddButton;
	CString	m_StrCashBonusId;
	CString	m_StrMessageLbl;
	//}}AFX_DATA

   CCB_General    *pGeneralPage;
   CCB_Amounts    *pAmountsPage;
   CChangeHistory *pChangeHistoryPage;

   int  m_nDialogMode;
   char m_strCurrentData[151];
	char strFilePrimaryKey[30];

	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCB_Definition)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCB_Definition)
	afx_msg void OnCbAddButton();
	afx_msg void OnCbDelButton();
	afx_msg void OnCbListButton();
	afx_msg void OnCbUpdButton();
	afx_msg void OnCbViewButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CB_DEFINITION_H__EE0237CF_64F1_4D80_B1B1_A324176A0DA9__INCLUDED_)
