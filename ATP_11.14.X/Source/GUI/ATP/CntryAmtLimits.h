#if !defined(AFX_CNTRYAMTLIMITS_H__375B8808_520E_4A1C_AE5E_44D9266DB384__INCLUDED_)
#define AFX_CNTRYAMTLIMITS_H__375B8808_520E_4A1C_AE5E_44D9266DB384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CntryAmtLimits.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCntryAmtLimits dialog

class CCntryAmtLimits : public CPropertyPage
{
	DECLARE_DYNCREATE(CCntryAmtLimits)

// Construction
public:
	CCntryAmtLimits();
	~CCntryAmtLimits();

#define MAX_CNT		500 /*max number of cntry codes supported*/

	void CleanCntryLmtsDialog();
	void PopulateCntryLmtDialogFromDatabase(CAF01);
	void PopulateDatabaseFromCnryLmtDialog (CAF01);
	void CompareStructures(COUNTRY_AMT_LIMITS, COUNTRY_AMT_LIMITS);
	void EnableCntryAmtLmtDialog(int);
	void PopulateHistoryTable();

	COUNTRY_AMT_LIMITS sCntryAmtLmtRecord;
	COUNTRY_AMT_LIMITS sCntryAmtLmtRecordOld;

	COUNTRY_AMT_LIMITS_GUI	m_cntry_amt_limits_gui_list;
	COUNTRY_AMT_LIMITS		m_cntry_amt_limits;

    CAF01 sProfileIdOld;

	int nSelectedIndex;


    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	
   int m_nDialogMode;
   BOOL m_bCntryAmtLmt;

	CTime c_time;	

// Dialog Data
	//{{AFX_DATA(CCntryAmtLimits)
	enum { IDD = IDD_CNTRY_AMT_LIMITS };
	CButton	m_CntlUpdateButton;
	CButton	m_CntlDeleteButton;
	CButton	m_CntlAddButton;
	CEdit	m_CntlTypeFld;
	CEdit	m_CntlProfileIdFld;
	CStatic	m_CntlMessageLbl;
	CEdit	m_CntlDollarLimit;
	CEdit	m_CntlCntryCode;
	CListCtrl	m_CntlCntryList;
	CString	m_StrCntryCode;
	CString	m_StrDollarLimit;
	CString	m_StrMessageLbl;
	CString	m_StrProfileIdFld;
	CString	m_StrTypeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCntryAmtLimits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCntryAmtLimits)
	virtual BOOL OnInitDialog();
	afx_msg void OnCntryLmtAdd();
	afx_msg void OnCntryLmtDelete();
	afx_msg void OnCntryLmtUpdate();
	afx_msg void OnClickCntryListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRYAMTLIMITS_H__375B8808_520E_4A1C_AE5E_44D9266DB384__INCLUDED_)
