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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkgeneral.h  $
   
      Rev 1.9   Dec 01 2003 11:29:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Mar 04 2003 16:30:58   lmitchel
   No change.
   
      Rev 1.7   Feb 27 2003 10:05:20   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.6   Jan 05 2000 14:55:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:05:06   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:56   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.2   Mar 22 1999 11:13:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 16 1999 16:30:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:24   iarustam
    
*
************************************************************************************/
// NetworkGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkGeneral dialog
#ifndef CNETWORKGENERAL
#define CNETWORKGENERAL

#include "app_info.h"

class CNetworkGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkGeneral)

// Construction
public:
	CNetworkGeneral();
	~CNetworkGeneral();
	
	NCF01 sNetworkId;
	NCF01 sNetworkIdOld;

	int m_nDialogMode;
	int m_nNetworkMode;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	BOOL m_bGeneral;

	void CleanGeneralDialog();
    void EnableGeneralDialog(int);
    void PopulateHistoryTable();

    BOOL PopulateGeneralDialogFromDatabase( NCF01 sNetworkId );
    BOOL PopulateDatabaseFromGeneralDialog( pNCF01 psNetworkId );
	BOOL CompareStructures();
	BOOL GetStatus();
	BOOL GetRemote();
	BOOL GetPin();
	BOOL GetEncrType();
	BOOL GetStore();
	CTime c_time;



	      
	//{{AFX_DATA(CNetworkGeneral)
	enum { IDD = IDD_NETWORK_GENERAL };
	CEdit	m_CntlKeyCheckFld;
	CEdit	m_CntlZmkFld;
	CEdit	m_CntlQueNameFld;
	CEdit	m_CntlTpduIDFld;
	CButton	m_CntlStatisticsCheck;
	CButton	m_CntlTotalsCheck;
	CComboBox	m_CntlStatusCombo;
	CEdit	m_CntlStationId2Fld;
	CEdit	m_CntlStationId1Fld;
	CComboBox	m_CntlPinTypeCombo;
	CButton	m_CntlAutoLogonCheck;
	CComboBox	m_CntlEncrTypeCombo;
	CEdit	m_CntlLocalNspFld;
	CEdit	m_CntlCurrencyPurchaseFld;
	CEdit	m_CntlCurrencyCashFld;
	CEdit	m_CntlCountryCodeFld;
	CEdit	m_CntlPadCharFld;
	CEdit	m_CntlNetworkTypeFld;
	CEdit	m_CntlNetworkIdFld;
	CEdit	m_CntlNameFld;
	CEdit	m_CntlKekFld;
	CEdit	m_CntlEchoIntervalFld;
	CEdit	m_CntlCwkFld;
	CString	m_StrCwkFld;
	CString	m_StrEchoIntervalFld;
	CString	m_StrKekFld;
	CString	m_StrMessageLbl;
	CString	m_StrNameFld;
	CString	m_StrNetworkIdFld;
	CString	m_StrNetworkTypeFld;
	CString	m_StrPadCharFld;
	CString	m_StrCountryCodeFld;
	CString	m_StrCurrencyCashFld;
	CString	m_StrCurrencyPurchaseFld;
	CString	m_StrLocalNspFld;
	CString	m_StrEncrTypeCombo;
	CString	m_StrStationId1Fld;
	CString	m_StrStationId2Fld;
	CString	m_StrStatusCombo;
	CString	m_StrPinTypeCombo;
	CString	m_StrTpduIDFld;
	CString	m_StrQueNameFld;
	CString	m_StrZmkFld;
	CString	m_StrKeyCheckFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusCwkFld();
	afx_msg void OnKillfocusZmkFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
