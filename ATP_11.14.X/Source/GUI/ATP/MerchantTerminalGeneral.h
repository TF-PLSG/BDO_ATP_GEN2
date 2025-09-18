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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalGeneral.h  $
   
      Rev 1.6   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:58   iarustam
   Initial Release
   
      Rev 1.2   Jun 02 1999 16:25:28   iarustam
   Bug fixes
   
      Rev 1.1   Apr 29 1999 14:56:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:46   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:42   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:24   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:48   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:02   iarustam
    
*
************************************************************************************/
// MerchantTerminalGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalGeneral dialog
#ifndef TERMINALGEN
#define TERMINALGEN


class CMerchantTerminalGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalGeneral)

// Construction
public:
	CMerchantTerminalGeneral();
	~CMerchantTerminalGeneral();

	void EnableGeneralDialog ( int );
	void CleanGeneralDialog();
	BOOL CompareStructures();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromGeneralDialog ( pDCF01 );
	BOOL PopulateGeneralDialogFromDatabase ( DCF01 );

	DCF01 sTerminalRecordOld;

	BOOL m_bGeneral;
	BOOL m_bValidDay;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	int m_nDialogMode;
	CString strTotalsInd;
	CString strStatisticsInd;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantTerminalGeneral)
	enum { IDD = IDD_TERMINAL_GENERAL };
//	CButton	m_CntlMPDeviceChk;
	//CEdit	m_CntlDinersChargeDescrFld;
	CEdit	m_CntlShortNameFld;
	//CEdit	m_CntlChargeDescFld;
	CEdit	m_CntlABAFld;
	CEdit   m_CntlEMVCapabilityFld;
	CEdit	m_CntlContactFld;
	CEdit	m_CntlCategoryCodeFld;
	CComboBox	m_CntlStatus;
	CEdit	m_CntlPinPadIndicatorFld;
	CEdit	m_CntlIndustryCodeFld;
	CButton	m_CntlEnableTotals;
	CButton	m_CntlEnableStatistics;
	CComboBox	m_CntlDeviceType;
	CEdit	m_CntlDepartmentNumFld;
	CEdit	m_CntlDateAddedFld;
	CString	m_StrDateAddedFld;
	CString	m_StrDepartmentNumFld;
	CString	m_StrDeviceType;
	CString	m_StrIndustryCodeFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrPinPadIndicatorFld;
	CString	m_StrStatus;
	CString	m_StrTerminalIDFld;
	CString	m_StrChargeDescFld;
	CString	m_StrCategoryCodeFld;
	CString	m_StrMessageLbl;
	CString	m_StrABAFld;
	CString	m_StrContactFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrShortNameFld;
	//CString	m_StrDinersChargeDescrFld;
	CString	m_StrEMVCapabilityFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalGeneral)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
