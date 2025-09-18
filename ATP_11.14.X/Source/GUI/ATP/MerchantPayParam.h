#if !defined(AFX_MERCHANTPAYPARAM_H__BB8E1DB5_EE95_4AAE_B249_6AEA979D21A1__INCLUDED_)
#define AFX_MERCHANTPAYPARAM_H__BB8E1DB5_EE95_4AAE_B249_6AEA979D21A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantPayParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParam dialog

class CMerchantPayParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantPayParam)

// Construction
public:
	CMerchantPayParam();
	~CMerchantPayParam();
	void EnableMerchantGroupDialog(int nEnable);

	BOOL PopulateGeneralDialogFromDatabase ( MPPARA01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pMPPARA01);

	void CleanGeneralDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();

	CString convert_ddmmyyyy_to_yyyymmdd( CString );
	CString convert_yyyymmdd_to_ddmmyyyy( CString );
public:
	BOOL m_bGeneral;
	int m_nDialogMode;
	MPPARA01 sMerchantPayParameterRecordOld;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CTime c_time;	
// Dialog Data
	//{{AFX_DATA(CMerchantPayParam)
	enum { IDD = IDD_MERCHANT_PAY_PARAM_GENERAL };
	CEdit	m_CntlProductCode;
	CButton	m_CntlBPICheck;
	CString	m_StrEffectiveDate;
	CEdit	m_CntlEffectiveDate;
	CString	m_StrMinMajorPurchaseAmount;
	CEdit	m_CntlMinMajorPurchaseAmount;
	CString	m_StrMaxMajorPurchaseAmount;
	CEdit	m_CntlMaxMajorPurchaseAmount;
	CString	m_StrMinInstalmentAmount;
	CEdit	m_CntlMinInstalmentAmount;
	CString	m_StrMaxInstalmentAmount;
	CEdit	m_CntlMaxInstalmentAmount;
	CString	m_StrMinMajorPurchaseLimit;
	CEdit	m_CntlMinMajorPurchaseLimit;
	CString	m_StrBPIMinimumAmount;
	CEdit	m_CntlBPIMinimumAmount;
	CString	m_StrBrokenPeriodInterest;
	CEdit	m_CntlBrokenPeriodInterest;
	CString	m_StrMinPurchaseAmtForFee;
	CEdit	m_CntlMinPurchaseAmtForFee;
	CString	m_StrFixedMPFee;
	CEdit	m_CntlFixedMPFee;
	CString	m_StrMPFeePercentage;
	CEdit	m_CntlMPFeePercentage;
	CString	m_StrEarlyRepaymentFeePercentage;
	CEdit	m_CntlEarlyRepaymentFeePercentage;
	CString	m_StrMinPenaltyAmount;
	CEdit	m_CntlMinPenaltyAmount;
	CString	m_StrMaxPenaltyAmount;
	CEdit	m_CntlMaxPenaltyAmount;
	CString	m_StrPenaltyPercentage;
	CEdit	m_CntlPenaltyPercentage;
	CString	m_StrPenaltyDays;
	CEdit	m_CntlPenaltyDays;
	CString	m_StrMinAmtForAging;
	CEdit	m_CntlMinAmtForAging;
	CString	m_StrMonthsToWriteOff;
	CEdit	m_CntlMonthsToWriteOff;
	CString	m_StrErrorUnauthorizedAmount;
	CEdit	m_CntlErrorUnauthorizedAmount;
	CString	m_StrMaxNoOfTransPerDay;
	CEdit	m_CntlMaxNoOfTransPerDay;
	CString	m_StrMaxAmountPerDay;
	CEdit	m_CntlMaxAmountPerDay;
	CString	m_StrAuthPercentOverlimit;
	CEdit	m_CntlAuthPercentOverlimit;
	CString	m_StrRespCode1;
	CEdit	m_CntlRespCode1;
	CString	m_StrMessage1;
	CEdit	m_CntlMessage1;
	CString	m_StrRespCode2;
	CEdit	m_CntlRespCode2;
	CString	m_StrMessage2;
	CEdit	m_CntlMessage2;
	CString	m_StrRespCode3;
	CEdit	m_CntlRespCode3;
	CString	m_StrMessage3;
	CEdit	m_CntlMessage3;
	CString	m_StrProductCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantPayParam)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantPayParam)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERCHANTPAYPARAM_H__BB8E1DB5_EE95_4AAE_B249_6AEA979D21A1__INCLUDED_)
