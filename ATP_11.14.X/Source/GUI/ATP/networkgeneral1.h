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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkgeneral1.h  $
   
      Rev 1.10   Dec 01 2003 11:29:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Feb 27 2003 10:05:22   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.8   Nov 16 2001 13:22:46   lmitchel
   
      Rev 1.7   Jan 05 2000 14:55:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 06 1999 16:47:02   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.1   Sep 22 1999 16:05:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:58   iarustam
   Initial Release
   
      Rev 1.4   Apr 29 1999 14:57:24   iarustam
   Bug fixes
   
      Rev 1.3   Apr 15 1999 14:19:48   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 22 1999 11:13:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 16 1999 16:30:46   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:26   iarustam
    
*
************************************************************************************/
// NetworkGeneral1.h : header file
//
#ifndef NETWORKGENERAL1
#define NETWORKGENERAL1

/////////////////////////////////////////////////////////////////////////////
// CNetworkGeneral1 dialog

class CNetworkGeneral1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkGeneral1)

// Construction
public:
	CNetworkGeneral1();
	~CNetworkGeneral1();

	NCF01 sNetworkId;
	NCF01 sNetworkIdOld;

	int m_nDialogMode;
	int m_nNetworkMode;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	BOOL m_bGeneral1;	
	
	void CleanGeneral1Dialog();
    void EnableGeneral1Dialog(int);
    void PopulateHistoryTable();

    BOOL PopulateGeneral1DialogFromDatabase( NCF01 sNetworkId );
    BOOL PopulateDatabaseFromGeneral1Dialog( pNCF01 psNetworkId );
	BOOL CompareStructures();
	BOOL GetStatus();
	BOOL GetRemote();
	BOOL GetPin();
	BOOL GetEncrType();
	BOOL GetStore();
	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CNetworkGeneral1)
	enum { IDD = IDD_NETWORK_GENERAL1 };
	CEdit	m_CntlKeyCheckFld;
	CEdit	m_CntlNegAccessCode;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlDeviceIDFld;
	CEdit	m_CntlRecievingInstIDFld;
	CEdit	m_CntlZmkFld;
	CEdit	m_CntlQueNameFld;
	CEdit	m_CntlTpduIDFld;
	CEdit	m_CntlStationId1Fld;
	CButton	m_CntlTotalsCheck;
	CComboBox	m_CntlStoreForwProcCombo;
	CComboBox	m_CntlStatusCombo;
	CButton	m_CntlStatisticsCheck;
	CComboBox	m_CntlRemoteNspCombo;
	CComboBox	m_CntlPinTypeCombo;
	CEdit	m_CntlLineTimerFld;
	CComboBox	m_CntlEncrTypeCombo;
	CButton	m_CntlAutoLogonCheck;
	CEdit	m_CntlAcquirerCodeFld;
	CEdit	m_CntlStationId2Fld;
	CEdit	m_CntlRequestTimerFld;
	CEdit	m_CntlPadCharFld;
	CEdit	m_CntlNetworkTypeFld;
	CEdit	m_CntlNetworkIdFld;
	CEdit	m_CntlNameFld;
	CEdit	m_CntlMaxConsTimeoutsFld;
	CEdit	m_CntlMaxActiveTrnxFld;
	CEdit	m_CntlLocalNspFld;
	CEdit	m_CntlKekFld;
	CEdit	m_CntlForwardingCodeFld;
	CEdit	m_CntlEchoIntervalFld;
	CEdit	m_CntlCwkFld;
	CEdit	m_CntlCurrencyPurchaseFld;
	CEdit	m_CntlCurrencyCashFld;
	CEdit	m_CntlCountryCodeFld;
	CEdit	m_CntlAdviceCounterFld;
	CEdit	m_CntlAdminMessageTimerFld;
	CEdit	m_CntlBillingCurrency;
	CString	m_StrAdminMessageTimerFld;
	CString	m_StrAdviceCounterFld;
	CString	m_StrCountryCodeFld;
	CString	m_StrCurrencyCashFld;
	CString	m_StrCurrencyPurchaseFld;
	CString	m_StrCwkFld;
	CString	m_StrRetCwkFld;
	CString	m_StrEchoIntervalFld;
	CString	m_StrForwardingCodeFld;
	CString	m_StrKekFld;
	CString	m_StrLocalNspFld;
	CString	m_StrMaxActiveTrnxFld;
	CString	m_StrMaxConsTimeoutsFld;
	CString	m_StrMessageLbl;
	CString	m_StrNameFld;
	CString	m_StrNetworkIdFld;
	CString	m_StrNetworkTypeFld;
	CString	m_StrPadCharFld;
	CString	m_StrRequestTimerFld;
	CString	m_StrStationId1Fld;
	CString	m_StrStationId2Fld;
	CString	m_StrAcquirerCodeFld;
	CString	m_StrEncrTypeCombo;
	CString	m_StrLineTimerFld;
	CString	m_StrPinTypeCombo;
	CString	m_StrRemoteNspCombo;
	CString	m_StrStatusCombo;
	CString	m_StrStoreForwProcCombo;
	CString	m_StrTpduIDFld;
	CString	m_StrQueNameFld;
	CString	m_StrZmkFld;
	CString	m_StrRecievingInstIDFld;
	CString	m_StrDeviceIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrNegAccessCode;
	CString	m_StrKeyCheckFld;
	CString m_StrBillingCurrency;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkGeneral1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkGeneral1)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusCwkFld();
	afx_msg void OnKillfocusZmkFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
