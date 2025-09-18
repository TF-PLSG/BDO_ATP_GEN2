/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\relatedgeneral.h  $
   
      Rev 1.9   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 05 2000 14:55:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 06 1999 16:47:04   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.1   Sep 22 1999 16:05:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:58   iarustam
   Initial Release
   
      Rev 1.5   May 10 1999 13:40:14   dperson
    
   
      Rev 1.4   May 10 1999 11:37:16   dperson
    Added PVCS header
    
*   
************************************************************************************/
// RelatedGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRelatedGeneral dialog
#ifndef RELGENERAL
#define RELGENERAL

class CRelatedGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CRelatedGeneral)

// Construction
public:
	CRelatedGeneral();
	~CRelatedGeneral();

	int m_nDialogMode;
	BOOL m_bGeneral;
	BOOL CompareStructures();

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];


	void CleanGeneralDialog();
	BOOL PopulateGeneralDialogFromDatabase ( ACF01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pACF01 );
	void PopulateHistoryTable();

	ACF01 sACFRecordOld;	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CRelatedGeneral)
	enum { IDD = IDD_RELATED_GENERAL };
	CEdit	m_CntlCashAvailableFld;
	CEdit	m_CntlAvailableFld;
	CComboBox	m_CntlStatus;
	CEdit	m_CntlCashOutstandingFld;
	CEdit	m_CntlCashLimitFld;
	CEdit	m_CntlOutstandingFld;
	CEdit	m_CntlLedgerBalanceFld;
	CEdit	m_CntlCreditLimitFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrAvailableFld;
	CString	m_StrCreditLimitFld;
	CString	m_StrDateAddedFld;
	CString	m_StrLedgerBalanceFld;
	CString	m_StrMessageLbl;
	CString	m_StrOutstandingFld;
	CString	m_StrAccountTypeFld;
	CString	m_StrCashAvailableFld;
	CString	m_StrCashLimitFld;
	CString	m_StrCashOutstandingFld;
	CString	m_StrStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRelatedGeneral)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRelatedGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusCreditLimitFld();
	afx_msg void OnKillfocusOutstandingFld();
	afx_msg void OnKillfocusCashLimitFld();
	afx_msg void OnKillfocusCashOutstandingFld();
	afx_msg void OnKillfocusLedgerBalanceFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
