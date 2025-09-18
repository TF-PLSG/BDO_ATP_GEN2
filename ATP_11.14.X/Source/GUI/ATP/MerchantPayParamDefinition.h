#if !defined(AFX_MERCHANTPAYPARAMDEFINITION_H__CD8A3F5F_CD14_4A4F_98C0_3CFACC67894A__INCLUDED_)
#define AFX_MERCHANTPAYPARAMDEFINITION_H__CD8A3F5F_CD14_4A4F_98C0_3CFACC67894A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantPayParamDefinition.h : header file
//

#include "ChangeHistory.h"
#include "MerchantPayParam.h"

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamDefinition dialog

class CMerchantPayParamDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantPayParamDefinition)

// Construction
public:
	CMerchantPayParamDefinition();
	~CMerchantPayParamDefinition();
public:
	int  m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];

	MPPARA01 sMerchantPayParameterRecordOld;

	CChangeHistory*             pChangeHistoryPage;
	CMerchantPayParam*		pMerchantPayParamGeneral;

	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	BOOL PopulateMerchantDatabaseStructureFromDialog (pMPPARA01);
	void PopulateMerchantDialogFromDatabase (MPPARA01);
	BOOL ValidateDate ( CString strDate );
	CTime c_time;
	

// Dialog Data
	//{{AFX_DATA(CMerchantPayParamDefinition)
	enum { IDD = IDD_MERCHANT_PAY_PARAM_DEFINITION };
	CStatic	m_CntlMessageLbl;
	CEdit	m_CntlProductCodeFld;
	CString	m_StrProductCodeFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantPayParamDefinition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMerchantPayParamDefinition)
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

#endif // !defined(AFX_MERCHANTPAYPARAMDEFINITION_H__CD8A3F5F_CD14_4A4F_98C0_3CFACC67894A__INCLUDED_)
