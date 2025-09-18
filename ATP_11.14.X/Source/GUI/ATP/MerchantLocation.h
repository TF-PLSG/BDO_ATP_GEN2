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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantLocation.h  $
   
      Rev 1.5   Dec 01 2003 11:29:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:53:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:52   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:36   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:44   iarustam
    
*
************************************************************************************/
// MerchantLocation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantLocation dialog
#ifndef LOCATION
#define LOCATION

#include "app_info.h"

class CMerchantLocation : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantLocation)

// Construction
public:
	CMerchantLocation();
	~CMerchantLocation();

	void EnableLocationDialog( int );
	BOOL PopulateLocationDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromLocationDialog ( pMCF01 );
	void CleanLocationDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();

	int m_nDialogMode;
	BOOL m_bLocation;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	MCF01 sMerchantRecordOld;
	CString strTotalsInd;
	CString strStatisticsInd;
//	MCF01 sMerchantRecordNew;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantLocation)
	enum { IDD = IDD_MERCHANT_LOCATION };
	CComboBox	m_CntlMerchantGroupID;
	CComboBox	m_CntlCurrencyCodeFld;
	CEdit	m_CntlNegativeGroupIDFld;
	CEdit	m_CntlChainNameFld;
	CEdit	m_CntlInvoiceSubcodeFld;
	CEdit	m_CntlInvoiceBatchFld;
	CComboBox	m_CntlStatusFld;
	CEdit	m_CntlStateFld;
	CEdit	m_CntlPostalCodeFld;
	CEdit	m_CntlMerchantNameFld;
	CEdit	m_CntlMerchantIDFld;
	CButton	m_CntlEnableTotals;
	CButton	m_CntlEnableStatistics;
	CEdit	m_CntlDateAddedFld;
	CEdit	m_CntlCountyFld;
	CEdit	m_CntlCountryFld;
	CEdit	m_CntlCityFld;
	CEdit	m_CntlAddress2Fld;
	CEdit	m_CntlAddress1Fld;
	CString	m_StrAddress1Fld;
	CString	m_StrAddress2Fld;
	CString	m_StrCityFld;
	CString	m_StrCountryFld;
	CString	m_StrCountyFld;
	CString	m_StrDateAddedFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMerchantNameFld;
	CString	m_StrPostalCodeFld;
	CString	m_StrStateFld;
	CString	m_StrInvoiceBatchFld;
	CString	m_StrInvoiceSubcodeFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrChainNameFld;
	CString	m_StrMessageLbl;
	CString	m_StrStatusFld;
	CString	m_StrNegativeGroupIDFld;
	CString	m_StrCurrencyCodeFld;
	CString m_StrMerchantGroupFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantLocation)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantLocation)
	virtual BOOL OnInitDialog();
	//afx_msg int OnSelchangeMerchantGroupId();
	afx_msg void OnSelchangeMerchantGroupId();
	afx_msg void OnEditchangeMerchantGroupId();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
