#if !defined(AFX_MCCRANGEDEFINITION_H__00C3AF85_6AF5_413C_85F9_51E2D6E60874__INCLUDED_)
#define AFX_MCCRANGEDEFINITION_H__00C3AF85_6AF5_413C_85F9_51E2D6E60874__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MCCRangeDefinition.h : header file
//
#include "ListExCtrl.h"
#include <afxdtctl.h>
/////////////////////////////////////////////////////////////////////////////
// CMCCRangeDefinition dialog

class CMCCRangeDefinition : public CDialog
{
// Construction
public:
	void EditMode(bool bEdit);
	int m_nSelectedItem;
	bool m_bListDone;
	bool m_bEditMode;
	bool m_bAdd;
	bool m_bEdit;
	bool m_bView;
	bool m_bDelete;
	CMCCRangeDefinition(CWnd* pParent = NULL);   // standard constructor

	MCC01_GUI_LIST sMCC01List;
	MCC01 sMCC01ListRecord;
	int m_nDialogMode;
//	MCF01 sMerchantRecordNew;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

// Dialog Data
	//{{AFX_DATA(CMCCRangeDefinition)
	enum { IDD = IDD_MCC_RANGE_DEFINITION };
	CButton	m_CntlCancelBtn;
	CEdit	m_CntlLow;
	CEdit	m_CntlHigh;
	CButton	m_CntlNext20Btn;
	CButton	m_CntlAddBtn;
	CButton	m_CntlDeleteBtn;
	CButton	m_CntlEditBtn;
	CListExCtrl	m_CntlRangeList;
	int		m_nHigh;
	int		m_nLow;
	CString	m_StrMessageLbl;
	//}}AFX_DATA

	void LoadListBox();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCCRangeDefinition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCCRangeDefinition)
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkRangeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnNext20();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCCRANGEDEFINITION_H__00C3AF85_6AF5_413C_85F9_51E2D6E60874__INCLUDED_)
