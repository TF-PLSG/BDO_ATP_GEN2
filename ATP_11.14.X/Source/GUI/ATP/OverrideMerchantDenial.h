#if !defined(AFX_OVERRIDEMERCHANTDENIAL_H__260C2824_07E4_48D0_9895_1571463F32F9__INCLUDED_)
#define AFX_OVERRIDEMERCHANTDENIAL_H__260C2824_07E4_48D0_9895_1571463F32F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverrideMerchantDenial.h : header file
//
#include <afxdtctl.h>
#include "ChangeHistory.h"

/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenial dialog

class COverrideMerchantDenial : public CPropertyPage
{
	DECLARE_DYNCREATE(COverrideMerchantDenial)
private:
	int CheckCardNumber();
	int CheckMerchantId();
	void PopulateHistoryTable();
	void EnableOverrideDialog(int status);
	int GetExpiryDate(CString &strDate);
	int SetExpiryDate(CString strDate);
	BOOL CompareStructures();
	BOOL CleanGeneralDialog();
// Construction
public:
	COverrideMerchantDenial();
	~COverrideMerchantDenial();
	CChangeHistory* pChangeHistoryPage;
	HST01 sHistoryRecord;
	BOOL m_bGeneral;
	OMC01 sOverrideMerchantDenialold;
// Dialog Data
	//{{AFX_DATA(COverrideMerchantDenial)
	enum { IDD = IDD_OVERRIDE_MERCHANT_DENIAL };
	CButton	m_ChkNeverExpires;
	CButton	m_ChkBypassMID;
	CButton	m_ChkBypassMCC;
	CButton	m_ChkBypassGlobal;
	CEdit	m_CntlUsdAmountLimit;
	CEdit	m_CntlPesoAmountLimit;
	CButton	m_CntlUpdate;
	CEdit	m_CntlRepeat;
	CDateTimeCtrl	m_CntlExpiryDate;
	CButton	m_CntlListOverrides;
	CButton	m_CntlListMerchants;
	CButton	m_CntlDelete;
	CButton	m_CntlAdd;
	CComboBox	m_CntlCardType;
	CEdit	m_CntlMerchantId;
	CEdit	m_CntlCardNumber;
	CString	m_StrCardNumber;
	CString	m_StrMerchantId;
	CString	m_StrMessageLbl;
	CString m_strCurrentData;
	CString m_strPreviousData;
	CString m_strFieldTag;
	int		m_nDialogMode;
	CString	m_StrRepeat;
	CString m_StrExpiryDate;
	CString	m_StrOverrideUsed;
	CString	m_StrPesoAmountLimit;
	CString	m_StrUsdAmountLimit;
	CString m_StrBypass;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COverrideMerchantDenial)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COverrideMerchantDenial)
	afx_msg void OnAdd();
	afx_msg void OnListMerchant();
	virtual BOOL OnInitDialog();
	afx_msg void OnListoverride();
	afx_msg void OnDelete();
	afx_msg void OnUpdate();
	afx_msg void OnNeverExpires();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERRIDEMERCHANTDENIAL_H__260C2824_07E4_48D0_9895_1571463F32F9__INCLUDED_)
