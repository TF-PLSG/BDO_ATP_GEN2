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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinGeneral.h  $
   
      Rev 1.9   Sep 28 2006 10:27:46   dirby
   Added a check box to the Bin Range GUI to indicate if a bin range
   is eligible or not for Fraudguard.  If not checked, then txns are
   sent to Fraudguard (if they meet other qualifications).  If checked,
   then txns will not go to Fraudguard (regardless of other qualifications).
   The new check box shares the Mod_Check field with the Mod10
   Check value and the Cash Bonus Eligible value.  Each of these
   three values is either ON or OFF, so they are represented by a
   single bit within the Mod_Check field.
   SCR 21851
   
      Rev 1.8   Dec 01 2003 11:28:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Sep 11 2002 16:57:04   dirby
   Added control for the Cash Bonus button.
   SCR 804
   
      Rev 1.6   Sep 28 2001 12:36:16   SYARLAGA
   Added Mod10Check
   
      Rev 1.5   Jan 25 2000 13:59:48   iarustam
   bug fixes
   
      Rev 1.4   Jan 05 2000 14:52:52   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:41:06   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:48   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:58   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:30   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:12   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:14   iarustam
    
*
************************************************************************************/
// BinGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBinGeneral dialog
#ifndef BINGENERAL
#define BINGENERAL

class CBinGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CBinGeneral)

// Construction
public:
	CBinGeneral();
	~CBinGeneral();

	void CleanBinDialog();
	BOOL CheckDialogForCompleteness();
	BOOL PopulateBinDialogFromDatabase ( BIN01 );
	BOOL GetBinType();
	

    //BOOL GetData (CString, CString, CString);

    char strType[2];
    BOOL m_bGeneral;
    int m_nDialogMode;

    void EnableBinDialog(int);
	void SetBinType();

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CBinGeneral)
	enum { IDD = IDD_BIN_GENERAL };
	CButton	m_BinFraudguardFld;
	CButton	m_BinCashBonusFld;
	CComboBox	m_CntlModCheck;
	CButton	m_LocalCheck;
	CButton	m_RemoteCheck;
	CComboBox	m_CntlCreditSubtype;
	CComboBox	m_CntlCardFamily;
	CEdit	m_CntlAltDestinationNameFld;
	CEdit	m_CntlSettleFileFld;
	CButton	m_CardTypeButton;
	CEdit	m_CntlCardType;
	CButton	m_NetworkButton;
	CButton	m_AuthButton;
	CButton	m_LocalRadio;
	CEdit	m_CntlPanLengthFld;
	CButton	m_RemoteRadio;
	CEdit	m_CntlNetworkFld;
	CEdit	m_CntlBinLowFld;
	CEdit	m_CntlBinLengthFld;
	CEdit	m_CntlBinHighFld;
	CEdit	m_CntlBinDescriptionFld;
	CEdit	m_CntlAuthorizationFld;
	CString	m_StrAuthorizationFld;
	CString	m_StrBinDescriptionFld;
	CString	m_StrBinHighFld;
	CString	m_StrBinLengthFld;
	CString	m_StrBinLowFld;
	CString	m_StrNetworkFld;
	CString	m_StrTypeFld;
	CString	m_StrMessageLbl;
	CString	m_StrPanLengthFld;
	CString	m_StrCardTypeFld;
	CString	m_StrSettleFileFld;
	CString	m_StrAltDestinationNameFld;
	CString	m_StrCardFamily;
	CString	m_StrCreditSubtype;
	CString	m_StrModCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinGeneral)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewAuthButton();
	afx_msg void OnLocalRadio();
	afx_msg void OnRemoteRadio();
	afx_msg void OnCardTypeButton();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnViewNetworkButton();
	afx_msg void OnLocalCheck();
	afx_msg void OnRemoteCheck();
	afx_msg void OnEditchangeCardFamily();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
