#if !defined(AFX_MERCHANTGROUPDEFINITION_H__44266734_2A58_4A5F_B5D8_3CA0EA4D8D44__INCLUDED_)
#define AFX_MERCHANTGROUPDEFINITION_H__44266734_2A58_4A5F_B5D8_3CA0EA4D8D44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MerchantGroupGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupGeneral dialog

class CMerchantGroupGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantGroupGeneral)

// Construction
public:
	CMerchantGroupGeneral();
	~CMerchantGroupGeneral();
	void EnableMerchantGroupDialog(int nEnable);

	BOOL PopulateGeneralDialogFromDatabase ( MGF01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pMGF01);

	void CleanGeneralDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();

	CString convert_ddmmyyyy_to_yyyymmdd( CString );
	CString convert_yyyymmdd_to_ddmmyyyy( CString );
	BOOL ValidateDate ( CString strDate );
public:
	BOOL m_bGeneral;
	int m_nDialogMode;
	MGF01 sMerchantGroupRecordOld;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CTime c_time;	


// Dialog Data
	//{{AFX_DATA(CMerchantGroupGeneral)
	enum { IDD = IDD_MERCHANT_GROUP_GENERAL };
	
	CString	m_StrMessageLbl;
	CString	m_StrAmount1;
	CString	m_StrAmount2;
	CString	m_StrAmount3;
	CString	m_StrAmount4;

	CString	m_StrInterestRateA1;
	CString	m_StrInterestRateA2;
	CString	m_StrInterestRateA3;
	CString	m_StrInterestRateA4;
	CString	m_StrInterestRateA5;
	CString	m_StrInterestRateA6;
	CString	m_StrInterestRateA7;
	CString	m_StrInterestRateA8;
	CString	m_StrInterestRateA9;
	CString	m_StrInterestRateA10;
	CString	m_StrInterestRateA10A;
	CString	m_StrInterestRateA10B;

	CString	m_StrInterestRateA11;
	CString	m_StrInterestRateA12;
	CString	m_StrInterestRateA13;
	CString	m_StrInterestRateA14;
	CString	m_StrInterestRateA15;
	CString	m_StrInterestRateA16;
	CString	m_StrInterestRateA17;
	CString	m_StrInterestRateA18;
	CString	m_StrInterestRateA19;
	CString	m_StrInterestRateA20;
	CString	m_StrInterestRateA20A;
	CString	m_StrInterestRateA20B;

	CString	m_StrInterestRateA21;
	CString	m_StrInterestRateA22;
	CString	m_StrInterestRateA23;
	CString	m_StrInterestRateA24;
	CString	m_StrInterestRateA25;
	CString	m_StrInterestRateA26;
	CString	m_StrInterestRateA27;
	CString	m_StrInterestRateA28;
	CString	m_StrInterestRateA29;
	CString	m_StrInterestRateA30;
	CString	m_StrInterestRateA30A;
	CString	m_StrInterestRateA30B;

	CString	m_StrInterestRateA31;
	CString	m_StrInterestRateA32;
	CString	m_StrInterestRateA33;
	CString	m_StrInterestRateA34;
	CString	m_StrInterestRateA35;
	CString	m_StrInterestRateA36;
	CString	m_StrInterestRateA37;
	CString	m_StrInterestRateA38;
	CString	m_StrInterestRateA39;
	CString	m_StrInterestRateA40;
	CString	m_StrInterestRateA40A;
	CString	m_StrInterestRateA40B;

	CString	m_StrMSFRateA1;
	CString	m_StrMSFRateA2;
	CString	m_StrMSFRateA3;
	CString	m_StrMSFRateA4;
	CString	m_StrMSFRateA5;
	CString	m_StrMSFRateA6;
	CString	m_StrMSFRateA7;
	CString	m_StrMSFRateA8;
	CString	m_StrMSFRateA9;
	CString	m_StrMSFRateA10;
	CString	m_StrMSFRateA10A;
	CString	m_StrMSFRateA10B;

	CString	m_StrMSFRateA11;
	CString	m_StrMSFRateA12;
	CString	m_StrMSFRateA13;
	CString	m_StrMSFRateA14;
	CString	m_StrMSFRateA15;
	CString	m_StrMSFRateA16;
	CString	m_StrMSFRateA17;
	CString	m_StrMSFRateA18;
	CString	m_StrMSFRateA19;
	CString	m_StrMSFRateA20;
	CString	m_StrMSFRateA20A;
	CString	m_StrMSFRateA20B;

	CString	m_StrMSFRateA21;
	CString	m_StrMSFRateA22;
	CString	m_StrMSFRateA23;
	CString	m_StrMSFRateA24;
	CString	m_StrMSFRateA25;
	CString	m_StrMSFRateA26;
	CString	m_StrMSFRateA27;
	CString	m_StrMSFRateA28;
	CString	m_StrMSFRateA29;
	CString	m_StrMSFRateA30;
	CString	m_StrMSFRateA30A;
	CString	m_StrMSFRateA30B;

	CString	m_StrMSFRateA31;
	CString	m_StrMSFRateA32;
	CString	m_StrMSFRateA33;
	CString	m_StrMSFRateA34;
	CString	m_StrMSFRateA35;
	CString	m_StrMSFRateA36;
	CString	m_StrMSFRateA37;
	CString	m_StrMSFRateA38;
	CString	m_StrMSFRateA39;
	CString	m_StrMSFRateA40;
	CString	m_StrMSFRateA40A;
	CString	m_StrMSFRateA40B;

	CString	m_StrEffectiveDateFld;
	CString	m_StrMechantGroupIdFld;

	CEdit	m_CntlMechantGroupIdFld;
	CEdit	m_CntlEffectiveDateFld;
	CEdit	m_CntlAmount1;
	CEdit	m_CntlAmount2;
	CEdit	m_CntlAmount3;
	CEdit	m_CntlAmount4;

	CEdit	m_CntlInterestRateA1;
	CEdit	m_CntlInterestRateA2;
	CEdit	m_CntlInterestRateA3;
	CEdit	m_CntlInterestRateA4;
	CEdit	m_CntlInterestRateA5;
	CEdit	m_CntlInterestRateA6;
	CEdit	m_CntlInterestRateA7;
	CEdit	m_CntlInterestRateA8;
	CEdit	m_CntlInterestRateA9;
	CEdit	m_CntlInterestRateA10;
	CEdit	m_CntlInterestRateA10A;
	CEdit	m_CntlInterestRateA10B;

	CEdit	m_CntlInterestRateA11;
	CEdit	m_CntlInterestRateA12;
	CEdit	m_CntlInterestRateA13;
	CEdit	m_CntlInterestRateA14;
	CEdit	m_CntlInterestRateA15;
	CEdit	m_CntlInterestRateA16;
	CEdit	m_CntlInterestRateA17;
	CEdit	m_CntlInterestRateA18;
	CEdit	m_CntlInterestRateA19;
	CEdit	m_CntlInterestRateA20;
	CEdit	m_CntlInterestRateA20A;
	CEdit	m_CntlInterestRateA20B;

	CEdit	m_CntlInterestRateA21;
	CEdit	m_CntlInterestRateA22;
	CEdit	m_CntlInterestRateA23;
	CEdit	m_CntlInterestRateA24;
	CEdit	m_CntlInterestRateA25;
	CEdit	m_CntlInterestRateA26;
	CEdit	m_CntlInterestRateA27;
	CEdit	m_CntlInterestRateA28;
	CEdit	m_CntlInterestRateA29;
	CEdit	m_CntlInterestRateA30;
	CEdit	m_CntlInterestRateA30A;
	CEdit	m_CntlInterestRateA30B;

	CEdit	m_CntlInterestRateA31;
	CEdit	m_CntlInterestRateA32;
	CEdit	m_CntlInterestRateA33;
	CEdit	m_CntlInterestRateA34;
	CEdit	m_CntlInterestRateA35;
	CEdit	m_CntlInterestRateA36;
	CEdit	m_CntlInterestRateA37;
	CEdit	m_CntlInterestRateA38;
	CEdit	m_CntlInterestRateA39;
	CEdit	m_CntlInterestRateA40;
	CEdit	m_CntlInterestRateA40A;
	CEdit	m_CntlInterestRateA40B;
	
	CEdit	m_CntlMSFRateA1;
	CEdit	m_CntlMSFRateA2;
	CEdit	m_CntlMSFRateA3;
	CEdit	m_CntlMSFRateA4;
	CEdit	m_CntlMSFRateA5;
	CEdit	m_CntlMSFRateA6;
	CEdit	m_CntlMSFRateA7;
	CEdit	m_CntlMSFRateA8;
	CEdit	m_CntlMSFRateA9;
	CEdit	m_CntlMSFRateA10;
	CEdit	m_CntlMSFRateA10A;
	CEdit	m_CntlMSFRateA10B;

	CEdit	m_CntlMSFRateA11;
	CEdit	m_CntlMSFRateA12;
	CEdit	m_CntlMSFRateA13;
	CEdit	m_CntlMSFRateA14;
	CEdit	m_CntlMSFRateA15;
	CEdit	m_CntlMSFRateA16;
	CEdit	m_CntlMSFRateA17;
	CEdit	m_CntlMSFRateA18;
	CEdit	m_CntlMSFRateA19;
	CEdit	m_CntlMSFRateA20;
	CEdit	m_CntlMSFRateA20A;
	CEdit	m_CntlMSFRateA20B;


	CEdit	m_CntlMSFRateA21;
	CEdit	m_CntlMSFRateA22;
	CEdit	m_CntlMSFRateA23;
	CEdit	m_CntlMSFRateA24;
	CEdit	m_CntlMSFRateA25;
	CEdit	m_CntlMSFRateA26;
	CEdit	m_CntlMSFRateA27;
	CEdit	m_CntlMSFRateA28;
	CEdit	m_CntlMSFRateA29;
	CEdit	m_CntlMSFRateA30;
	CEdit	m_CntlMSFRateA30A;
	CEdit	m_CntlMSFRateA30B;

	CEdit	m_CntlMSFRateA31;
	CEdit	m_CntlMSFRateA32;
	CEdit	m_CntlMSFRateA33;
	CEdit	m_CntlMSFRateA34;
	CEdit	m_CntlMSFRateA35;
	CEdit	m_CntlMSFRateA36;
	CEdit	m_CntlMSFRateA37;
	CEdit	m_CntlMSFRateA38;
	CEdit	m_CntlMSFRateA39;
	CEdit	m_CntlMSFRateA40;
	CEdit	m_CntlMSFRateA40A;
	CEdit	m_CntlMSFRateA40B;


	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantGroupGeneral)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantGroupGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeMsfRateA20B();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnEnChangeMerchantGroupIdFld();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERCHANTGROUPDEFINITION_H__44266734_2A58_4A5F_B5D8_3CA0EA4D8D44__INCLUDED_)
