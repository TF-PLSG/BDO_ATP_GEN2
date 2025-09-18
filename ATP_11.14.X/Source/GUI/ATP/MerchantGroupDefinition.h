#if !defined(AFX_MERCHANTGROUPDEFINITION_H__ED83C5E4_A420_424C_972E_B0BFF23F2A01__INCLUDED_)
#define AFX_MERCHANTGROUPDEFINITION_H__ED83C5E4_A420_424C_972E_B0BFF23F2A01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantGroupDefinition.h : header file
//

#include "MerchantGroupGeneral.h"
#include "ChangeHistory.h"


/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupDefinition dialog

class CMerchantGroupDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantGroupDefinition)

// Construction
public:
	CMerchantGroupDefinition();
	~CMerchantGroupDefinition();
public:
	int m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];

	MGF01 sMerchantGroupRecordOld;


	CChangeHistory*             pChangeHistoryPage;
	CMerchantGroupGeneral*		pMerchantGroupGeneral;

	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	BOOL PopulateMerchantDatabaseStructureFromDialog (pMGF01);
	void PopulateMerchantDialogFromDatabase (MGF01);
	BOOL ValidateDate ( CString strDate );
	CTime c_time;
	
// Dialog Data
	//{{AFX_DATA(CMerchantGroupDefinition)
	enum { IDD = IDD_MERCHANT_GROUP_DEFINITION };
	CStatic	m_CntlMessageLbl;
	CEdit	m_CntlMerchantGroupId;
	CString	m_StrMerchantGroupId;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantGroupDefinition)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantGroupDefinition)
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	afx_msg void OnListButton();
	afx_msg void OnSearch();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERCHANTGROUPDEFINITION_H__ED83C5E4_A420_424C_972E_B0BFF23F2A01__INCLUDED_)
