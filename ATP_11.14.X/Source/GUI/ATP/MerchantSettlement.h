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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantSettlement.h  $
   
      Rev 1.5   Dec 01 2003 11:29:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:53:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:48   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:38   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:52   iarustam
    
*
************************************************************************************/
// MerchantSettlement.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantSettlement dialog
#ifndef SETTLEMENT
#define SETTLEMENT


class CMerchantSettlement : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantSettlement)

// Construction
public:
	CMerchantSettlement();
	~CMerchantSettlement();

	void PopulateSettlementList();
	void EnableSettlementDialog(int);
	void CleanSettlementDialog();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromSettlementDialog ();
	

	BOOL m_bSettlement;
	BOOL bDone;
	int m_nDialogMode;
	MCT01 sMerchantCardRecord;
	MCT01_GUI_LIST sMerchantCardList;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	int nSelectedIndex;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantSettlement)
	enum { IDD = IDD_MERCHANT_SETTLEMENT };
	CComboBox	m_CntlCardTypeFld;
	CListCtrl	m_CntlList;
	CButton	m_UpdateButton;
	CButton	m_NextButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlSettleMerchantIDFld;
	CEdit	m_CntlFloorLimitFld;
	CEdit	m_CntlCorporationIDFld;
	CEdit	m_CntlAquiringIDFld;
	CString	m_StrAquiringIDFld;
	CString	m_StrCorporationIDFld;
	CString	m_StrFloorLimitFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrSettleMerchantIDFld;
	CString	m_StrCardTypeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantSettlement)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantSettlement)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnNextButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusCardType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
