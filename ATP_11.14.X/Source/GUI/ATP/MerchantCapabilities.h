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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantCapabilities.h  $
   
      Rev 1.5   Dec 01 2003 11:29:34   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:48   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:52:06   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:40   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:44   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:30   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:34   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:34   iarustam
    
*
************************************************************************************/
// MerchantCapabilities.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantCapabilities dialog
#ifndef CAPABILITIES
#define CAPABILITIES

class CMerchantCapabilities : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantCapabilities)

// Construction
public:
	CMerchantCapabilities();
	~CMerchantCapabilities();

	void EnableCapabilitiesDialog(int);
	BOOL PopulateCapabilitiesDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromCapabilitiesDialog ( pMCF01 );
	void CleanCapabilitiesDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();


	BOOL m_bCapabilities;
	int m_nDialogMode;
	MCF01 sMerchantRecordOld;
//	MCF01 sMerchantRecordNew;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantCapabilities)
	enum { IDD = IDD_MERCHANT_CAPABILITIES };
	CEdit	m_CntlDebitFloorLimitFld;
	CEdit	m_CntlCreditFloorLimitFld;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlDebitReturnFld;
	CEdit	m_CntlDebitPurchaseFld;
	CEdit	m_CntlDebitManualEntryFld;
	CEdit	m_CntlDebitCashbackFld;
	CEdit	m_CntlDebitCashAdvanceFld;
	CEdit	m_CntlDebitAuthFld;
	CEdit	m_CntlCreditReturnFld;
	CEdit	m_CntlCreditPurchaseFld;
	CEdit	m_CntlCreditManualEntryFld;
	CEdit	m_CntlCreditCashbackFld;
	CEdit	m_CntlCreditCashAdvancedFld;
	CEdit	m_CntlCreditAuthFld;
	CString	m_StrCreditAuthFld;
	CString	m_StrCreditCashAdvancedFld;
	CString	m_StrCreditCashbackFld;
	CString	m_StrCreditManualEntryFld;
	CString	m_StrCreditPurchaseFld;
	CString	m_StrCreditReturnFld;
	CString	m_StrDebitAuthFld;
	CString	m_StrDebitCashAdvanceFld;
	CString	m_StrDebitCashbackFld;
	CString	m_StrDebitManualEntryFld;
	CString	m_StrDebitPurchaseFld;
	CString	m_StrDebitReturnFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrCreditFloorLimitFld;
	CString	m_StrDebitFloorLimitFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantCapabilities)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantCapabilities)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
