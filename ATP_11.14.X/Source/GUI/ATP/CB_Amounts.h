#if !defined(AFX_CB_AMOUNTS_H__9F635E77_9665_4613_923F_4D23ECA9F9A9__INCLUDED_)
#define AFX_CB_AMOUNTS_H__9F635E77_9665_4613_923F_4D23ECA9F9A9__INCLUDED_

#include "CB_Definition.h"
#include "CB_General.h"
#include "ChangeHistory.h"
#include "MyUtils.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CB_Amounts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCB_Amounts dialog

class CCB_Amounts : public CPropertyPage
{
	DECLARE_DYNCREATE(CCB_Amounts)

// Construction
public:
	CCB_Amounts();
	~CCB_Amounts();

	
	CTime c_time;		

// Dialog Data
	//{{AFX_DATA(CCB_Amounts)
	enum { IDD = IDD_CB_AMOUNTS };
	CButton	m_CntlPurchase5Radio;
	CButton	m_CntlPurchase4Radio;
	CButton	m_CntlPurchase3Radio;
	CButton	m_CntlPurchase2Radio;
	CButton	m_CntlPurchase1Radio;
	CButton	m_CntlWinOther5Radio;
	CButton	m_CntlWinOther4Radio;
	CButton	m_CntlWinOther3Radio;
	CButton	m_CntlWinOther2Radio;
	CButton	m_CntlWinOther1Radio;
	CEdit	m_CntlCashBonusId;
	CEdit	m_CntlAmtText5;
	CEdit	m_CntlAmtOther5;
	CEdit	m_CntlAmtLow5;
	CEdit	m_CntlAmtHigh5;
	CEdit	m_CntlAmtText4;
	CEdit	m_CntlAmtOther4;
	CEdit	m_CntlAmtLow4;
	CEdit	m_CntlAmtHigh4;
	CEdit	m_CntlAmtText3;
	CEdit	m_CntlAmtOther3;
	CEdit	m_CntlAmtLow3;
	CEdit	m_CntlAmtHigh3;
	CEdit	m_CntlAmtText2;
	CEdit	m_CntlAmtOther2;
	CEdit	m_CntlAmtLow2;
	CEdit	m_CntlAmtHigh2;
	CEdit	m_CntlAmtText1;
	CEdit	m_CntlAmtOther1;
	CEdit	m_CntlAmtLow1;
	CEdit	m_CntlAmtHigh1;
	CString	m_StrAmtHigh1;
	CString	m_StrAmtLow1;
	CString	m_StrAmtOther1;
	CString	m_StrAmtText1;
	CString	m_StrAmtHigh2;
	CString	m_StrAmtLow2;
	CString	m_StrAmtOther2;
	CString	m_StrAmtText2;
	CString	m_StrAmtHigh3;
	CString	m_StrAmtLow3;
	CString	m_StrAmtOther3;
	CString	m_StrAmtText3;
	CString	m_StrAmtHigh4;
	CString	m_StrAmtLow4;
	CString	m_StrAmtOther4;
	CString	m_StrAmtText4;
	CString	m_StrAmtHigh5;
	CString	m_StrAmtOther5;
	CString	m_StrAmtLow5;
	CString	m_StrAmtText5;
	CString	m_StrCashBonusId;
	CString	m_StrMessageLbl;
	int		m_IntWinOther1Radio;
	int		m_IntWinOther2Radio;
	int		m_IntWinOther3Radio;
	int		m_IntWinOther4Radio;
	int		m_IntWinOther5Radio;
	int		m_IntPurchase1Radio;
	int		m_IntPurchase2Radio;
	int		m_IntPurchase3Radio;
	int		m_IntPurchase4Radio;
	int		m_IntPurchase5Radio;
	//}}AFX_DATA

   CCB_General    *pGeneralPage;
   CCB_Amounts    *pAmountsPage;
   CChangeHistory *pChangeHistoryPage;

   int  m_nDialogMode;
   BOOL m_bAmounts;
   char m_StrCurrentData[151];
   char m_StrPreviousData[151];
   char m_StrFieldTag[50];

   BON02 sBon02;
   BON02 sBon02Old;

   void CleanAmountsDialog();
   void EnableAmountsDialog(int);
   void PopulateHistoryTable();
	BOOL PopulateBon02DialogFromDatabase( BON02, INT );
	BOOL PopulateDatabaseFromBon02Dialog( pBON02 );
	BOOL CompareStructures();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCB_Amounts)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCB_Amounts)
	virtual BOOL OnInitDialog();
	afx_msg void OnWinOther1Radio();
	afx_msg void OnWinOther2Radio();
	afx_msg void OnWinOther3Radio();
	afx_msg void OnWinOther4Radio();
	afx_msg void OnWinOther5Radio();
	afx_msg void OnWinPurchase1Radio();
	afx_msg void OnWinPurchase2Radio();
	afx_msg void OnWinPurchase3Radio();
	afx_msg void OnWinPurchase4Radio();
	afx_msg void OnWinPurchase5Radio();
	afx_msg void OnKillfocusAmount1Low();
	afx_msg void OnKillfocusAmount1High();
	afx_msg void OnKillfocusAmount1Other();
	afx_msg void OnKillfocusAmount2High();
	afx_msg void OnKillfocusAmount2Low();
	afx_msg void OnKillfocusAmount2Other();
	afx_msg void OnKillfocusAmount3High();
	afx_msg void OnKillfocusAmount3Low();
	afx_msg void OnKillfocusAmount3Other();
	afx_msg void OnKillfocusAmount4High();
	afx_msg void OnKillfocusAmount4Low();
	afx_msg void OnKillfocusAmount4Other();
	afx_msg void OnKillfocusAmount5High();
	afx_msg void OnKillfocusAmount5Low();
	afx_msg void OnKillfocusAmount5Other();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CB_AMOUNTS_H__9F635E77_9665_4613_923F_4D23ECA9F9A9__INCLUDED_)
