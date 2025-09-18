#if !defined(AFX_CB_GENERAL_H__9719B8B6_E332_411A_B80B_673991EF4923__INCLUDED_)
#define AFX_CB_GENERAL_H__9719B8B6_E332_411A_B80B_673991EF4923__INCLUDED_

#include "MyUtils.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CB_General.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCB_General dialog

class CCB_General : public CPropertyPage
{
	DECLARE_DYNCREATE(CCB_General)

// Construction
public:
	CCB_General();
	~CCB_General();

   BON02 sBon02;
   BON02 sBon02Old;

	int  m_nDialogMode;
	BOOL m_bGeneral;
   char m_StrCurrentData[151];
   char m_StrPreviousData[151];
   char m_StrFieldTag[50];

   void CleanGeneralDialog();
   void EnableGeneralDialog(int);
   void PopulateHistoryTable();
	BOOL PopulateBon02DialogFromDatabase( BON02  );
	BOOL PopulateDatabaseFromBon02Dialog( pBON02 );
	BOOL CompareStructures();
   BOOL ValidateDate( CString strDate, pCHAR errmsg );
   BOOL ValidateProgramDuration( pCHAR errbuf );

   CTime c_time;		

// Dialog Data
	//{{AFX_DATA(CCB_General)
	enum { IDD = IDD_CB_GENERAL };
	CEdit	m_CntlTxnCtr;
	CEdit	m_CntlTimePeriod;
	CEdit	m_CntlThreshold;
	CEdit	m_CntlStartDate;
	CEdit	m_CntlProgramWinCtr;
	CEdit	m_CntlCashDesc;
	CEdit	m_CntlPeriodWinCtr;
	CEdit	m_CntlMultiWinnerRespText;
	CEdit	m_CntlMaxProgramWinners;
	CEdit	m_CntlMaxPeriodWins;
	CEdit	m_CntlEndDate;
	CEdit	m_CntlCashBonusId;
	CString	m_StrCashBonusId;
	CString	m_StrMaxPeriodWins;
	CString	m_StrMessageLbl;
	CString	m_StrPeriodWinCtr;
	CString	m_StrEndDate;
	CString	m_StrCashDesc;
	CString	m_StrProgramWinCtr;
	CString	m_StrStartDate;
	CString	m_StrThreshold;
	CString	m_StrTimePeriod;
	CString	m_StrTxnCtr;
	CString	m_StrMaxProgramWinners;
	CString	m_StrMultiWinnerRespText;
	CString	m_StrTagId;
	CButton	m_MultWinsAllowed;
	BOOL	   m_BoolMultWinsAllowed;
	CString  m_StrMultWinsAllowed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCB_General)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCB_General)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEndDate();
	afx_msg void OnKillfocusMaxPeriodWins();
	afx_msg void OnKillfocusMaxProgramWinners();
	afx_msg void OnKillfocusPeriodWinCtr();
	afx_msg void OnKillfocusProgramWinCtr();
	afx_msg void OnKillfocusStartDate();
	afx_msg void OnKillfocusThreshold();
	afx_msg void OnKillfocusTimePeriod();
	afx_msg void OnKillfocusTxnCtr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CB_GENERAL_H__9719B8B6_E332_411A_B80B_673991EF4923__INCLUDED_)
