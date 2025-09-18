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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantVelocity.h  $
   
      Rev 1.5   Dec 01 2003 11:29:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:00   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:00   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:08   iarustam
    
*
************************************************************************************/
// MerchantVelocity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantVelocity dialog
#ifndef VELOCITY
#define VELOCITY


class CMerchantVelocity : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantVelocity)

// Construction
public:
	CMerchantVelocity();
	~CMerchantVelocity();

	void EnableVelocityDialog( int );
	BOOL PopulateVelocityDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromVelocityDialog ( pMCF01 );
	void CleanVelocityDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();

	int m_nDialogMode;
	BOOL m_bVelocity;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	MCF01 sMerchantRecordOld;
//	MCF01 sMerchantRecordNew;	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantVelocity)
	enum { IDD = IDD_MERCHANT_VELOCITY };
	CEdit	m_CntlTotalLimitFld;
	CButton	m_SettlementTotals;
	CButton	m_SamePan;
	CEdit	m_CntlReturnAmountFld;
	CButton	m_ReturnAmount;
	CEdit	m_CntlPurchaseAverageFld;
	CButton	m_NumberReturns;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlMaxReturnsFld;
	CEdit	m_CntlMaxPanSameFld;
	CEdit	m_CntlMaxManualEntrysFld;
	CButton	m_ManualEntrys;
	CButton	m_AveragePurchase;
	CString	m_StrMaxManualEntrysFld;
	CString	m_StrMaxPanSameFld;
	CString	m_StrMaxReturnsFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrPurchaseAverageFld;
	CString	m_StrReturnAmountFld;
	CString	m_StrTotalLimitFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantVelocity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantVelocity)
	afx_msg void OnAveragePurchase();
	afx_msg void OnManualEntrys();
	afx_msg void OnNumberReturns();
	afx_msg void OnReturnAmount();
	afx_msg void OnSamePan();
	afx_msg void OnSettlementTotals();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
