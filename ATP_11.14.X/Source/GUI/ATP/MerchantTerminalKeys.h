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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalKeys.h  $
   
      Rev 1.8   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Mar 04 2003 16:28:42   lmitchel
   Validate length of TPK key and for hex characters; allow entry of blank key
   
      Rev 1.6   Feb 27 2003 10:05:12   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.5   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:42   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:58   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:42   iarustam
   Bug fixes
   
      Rev 1.1   Apr 09 1999 16:51:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:42   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:52   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:04   iarustam
    
*
************************************************************************************/
// MerchantTerminalKeys.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalKeys dialog
#ifndef TERMINALKEYS
#define TERMINALKEYS


class CMerchantTerminalKeys : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalKeys)

// Construction
public:
	CMerchantTerminalKeys();
	~CMerchantTerminalKeys();

	void EnableKeysDialog (int);
	BOOL PopulateKeysDialogFromDatabase (DCF01);
	BOOL PopulateDatabaseFromKeysDialog (pDCF01);
	BOOL CompareStructures();
	void CleanKeysDialog();
	void PopulateHistoryTable();

	DCF01 sTerminalRecordOld;

	BOOL m_bKeys;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CString strPinFormat;
	CString strEncryptionMethod; 

	int m_nDialogMode;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantTerminalKeys)
	enum { IDD = IDD_TERMINAL_KEYS };
	CEdit	m_CntlTPKFld;
	CEdit	m_CntlPinKeyCryptogramFld;
	CComboBox	m_PinBlockFormat;
	CEdit	m_CntlPadCharFld;
	CEdit	m_CntlNSPProcessFld;
	CEdit	m_CntlKeyExchangeFld;
	CEdit	m_CntlKekFld;
	CEdit	m_CntlICVDataFld;
	CComboBox	m_EncryptionMethod;
	CEdit	m_CntlCommunicationKeyFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrCommunicationKeyFld;
	CString	m_StrICVDataFld;
	CString	m_StrKekFld;
	CString	m_StrKeyExchangeFld;
	CString	m_StrNSPProcessFld;
	CString	m_StrPadCharFld;
	CString	m_StrPinKeyCryptogramFld;
	CString	m_StrTerminalIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrMerchantIDFld;
	CString	m_StrEncryptionMethod;
	CString	m_StrPinBlockFormat;
	CString	m_StrTPKFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalKeys)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalKeys)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePinBlockFormatFld();
	afx_msg void OnKillfocusTpkFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
