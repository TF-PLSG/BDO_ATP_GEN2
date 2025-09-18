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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantGeneral.h  $
   
      Rev 1.12   Jul 16 2004 16:07:46   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.11   Dec 01 2003 11:29:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Aug 20 2002 16:33:44   dirby
   Added variables to support new fields on the Merchant General
   tab to support Online Deferred txns.  Added function prototypes
   to convert date formats; used for the deferred start and end dates.
   SCR 255
   
      Rev 1.9   Jan 25 2000 14:00:30   iarustam
   bug fixes
   
      Rev 1.8   Jan 05 2000 14:53:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:53:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:52   iarustam
   Initial Release
   
      Rev 1.5   May 21 1999 10:17:26   iarustam
   Bug fixes
   
   
      Rev 1.4   May 10 1999 13:39:30   dperson
   Updated comments
   
      Rev 1.3   May 10 1999 11:24:38   dperson
    Added PVCS header
    
*   
************************************************************************************/
// MerchantGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantGeneral dialog
#ifndef MERCHANTGEN
#define MERCHANTGEN

class CMerchantGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantGeneral)

// Construction
public:
	CMerchantGeneral();
	~CMerchantGeneral();

	BOOL PopulateGeneralDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pMCF01);
	void CleanGeneralDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();
   CString convert_ddmmyyyy_to_yyyymmdd( CString );
   CString convert_yyyymmdd_to_ddmmyyyy( CString );

	BOOL m_bGeneral;
	int m_nDialogMode;
	MCF01 sMerchantRecordOld;
//	MCF01 sMerchantRecordNew;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	CString strRPSMerchant;
	CString strHouseMerchant;
	CString strProcurementMerchant;
	CString strDeferredMerchant;
	CString strCashEligible;
	CString strReloadMerchant;
	CString strPreauthValidationFlag;
	CTime c_time;	

// Dialog Data
	//{{AFX_DATA(CMerchantGeneral)
	enum { IDD = IDD_MERCHANT_GENERAL };
	CEdit	m_CntlMasterCardAssignedId;
	CEdit   m_CntlCUPAcceptorID;
	CEdit   m_CntlVisaMVV;
	CEdit   m_CntlMCMerchantcntryOrigin;
	CButton	m_CntlVIPMerchant;
	CEdit	m_CntlOwnMerchant;
	CEdit	m_CntlFPIFld;
	CButton	m_cntlReloadMerchant;
	CButton	m_CntPreauthValidationFlag;
	CEdit	m_CntlGroupTypeFld;
	CEdit	m_CntlGroupOutletCodeFld;
	CEdit	m_CntlDefStartDateFld;
	CEdit	m_CntlDefEndDateFld;
	CEdit	m_CntlVoiceCategoryCodeFld;
	CEdit	m_CntlVSTCCCodeFld;
	CEdit	m_CntlVIPCodeFld;
	CEdit	m_CntlSolicitCodeFld;
	CButton	m_RPSMerchant;
	CEdit	m_CntlRegionNumberFld;
	CButton	m_Procurement;
	CEdit	m_CntlPOSCodeFld;
	CEdit	m_CntlMCFCCCodeFld;
	CEdit	m_CntlMallCodeFld;
	CEdit	m_CntlJCBSubcodeFld;
	CButton	m_HouseCardMerchant;
	CEdit	m_CntlHCCodeFld;
	CEdit	m_CntlDivisionNumberFld;
	CButton	m_DeferredMerchant;
	CButton	m_CashBonus;
	CString	m_StrMallCodeFld;
	CString	m_StrDivisionNumberFld;
	CString	m_StrHCCodeFld;
	CString	m_StrJCBSubcodeFld;
	CString	m_StrMCFCCCodeFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrPOSCodeFld;
	CString	m_StrRegionNumberFld;
	CString	m_StrVIPCodeFld;
	CString	m_StrVSTCCCodeFld;
	CString	m_StrSolicitCodeFld;
	CString	m_StrVoiceCategoryCodeFld;
	CString	m_StrClassCodeFld;
	BOOL	m_BoolDeferredMerchant;
	BOOL	m_BoolHouseCardMerchant;
	CString	m_StrDefEndDateFld;
	CString	m_StrDefStartDateFld;
	CString	m_StrGroupOutletCodeFld;
	CString	m_StrGroupTypeFld;
	CString	m_StrFPIFld;
	CString	m_StrOwnMerchant;
	CString	m_StrMasterCardAssignedId;
	CString m_StrCUPAcceptorID;
	CString m_StrVisaMVV;
	CString m_StrMCMerchantcntryOrigin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantGeneral)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeferredMerchant();
	afx_msg void OnHouseCardMerchant();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
