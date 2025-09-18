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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFCapability.h  $
   
      Rev 1.5   Dec 01 2003 11:29:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:58   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:50   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:00   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:20   iarustam
    
*
************************************************************************************/
// CFCapability.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFCapability dialog
#ifndef CAPPAGE
#define CAPPAGE


class CCFCapability : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFCapability)

// Construction
public:
	CCFCapability();
	~CCFCapability();

	int DialogMode;

	void EnableCreditBox( BOOL boolval);
	void EnableDebitBox(BOOL boolval) ;
	void EnableFleetBox(BOOL boolval); 
	void EnableLoyaltyBox(BOOL boolval); 




// Dialog Data
	//{{AFX_DATA(CCFCapability)
	enum { IDD = IDD_CARDCAPABILITY };
	CButton	m_ctrlLoyInquiry;
	CButton	m_ctrlLoyLoad;
	CButton	m_ctrlLoyPurchase;
	CButton	m_ctrlLoyReturn;
	CButton	m_ctrlLoyUnload;
	CButton	m_ctrlFlReturn;
	CButton	m_ctrlFlPurchase;
	CButton	m_ctrlFlPayment;
	CButton	m_ctrlFlInquiry;
	CButton	m_ctrlFlCashAdv;
	CButton	m_ctrlDebReturn;
	CButton	m_ctrlDebPurchase;
	CButton	m_ctrlDebInquiry;
	CButton	m_ctrlDebDeposit;
	CButton	m_ctrlDebCashWithd;
	CButton	m_ctrlDebBalTransf;
	CButton	m_ctrlCrReturn;
	CButton	m_ctrlCrPurchase;
	CButton	m_ctrlCrPayment;
	CButton	m_ctrlCrInquiry;
	CButton	m_ctrlCrCashAdv;
	CButton	m_LoyaltyBox;
	CButton	m_CreditBox;
	CButton	m_DebitBox;
	CButton	m_FleetBox;
	CStatic	m_ComEdit;
	CString	m_strCardNbr;
	CString	m_strCardType;
	BOOL	m_CrCashAdv;
	BOOL	m_CrInquiry;
	BOOL	m_CrPayment;
	BOOL	m_CrPurchase;
	BOOL	m_CrReturn;
	BOOL	m_DebBalTransf;
	BOOL	m_DebCashWithd;
	BOOL	m_DebDeposit;
	BOOL	m_DebInquiry;
	BOOL	m_DebPurchase;
	BOOL	m_DebReturn;
	BOOL	m_FlInquiry;
	BOOL	m_FlPayment;
	BOOL	m_FlPurchase;
	BOOL	m_FlReturn;
	BOOL	m_LoyInquiry;
	BOOL	m_LoyLoad;
	BOOL	m_LoyPurchase;
	BOOL	m_LoyReturn;
	BOOL	m_LoyUnload;
	BOOL	m_FlCashAdv;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFCapability)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFCapability)
	virtual BOOL OnInitDialog();
	afx_msg void OnCrCashadv();
	afx_msg void OnCrInquiry();
	afx_msg void OnCrPayment();
	afx_msg void OnCrPurchase();
	afx_msg void OnCrReturn();
	afx_msg void OnCreditbox();
	afx_msg void OnDebBaltrans();
	afx_msg void OnDebCashwithd();
	afx_msg void OnDebDeposit();
	afx_msg void OnDebInquiry();
	afx_msg void OnDebPurchase();
	afx_msg void OnDebReturn();
	afx_msg void OnDebitbox();
	afx_msg void OnFlCashadv();
	afx_msg void OnFlInquiry();
	afx_msg void OnFlPayment();
	afx_msg void OnFlPurchase();
	afx_msg void OnFlReturn();
	afx_msg void OnFleetbox();
	afx_msg void OnLoyInquiry();
	afx_msg void OnLoyLoad();
	afx_msg void OnLoyPurchase();
	afx_msg void OnLoyReturn();
	afx_msg void OnLoyUnload();
	afx_msg void OnLoyaltybox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
