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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsMerchantLimit.h  $
   
      Rev 1.9   Jun 18 2004 16:38:56   dirby
   Split the Merchant Limit amount field into Peso amount and USD amount.
   SCR 1380
   
      Rev 1.8   Jun 04 2004 11:03:18   lmitchel
   Modified merchant limits table in Authorization Control to add/update/insert MCC limits including new  amount field.  Maximum limit of records changed to 500 from 50.
   
      Rev 1.7   Dec 01 2003 11:28:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Apr 30 2003 12:18:36   lmitchel
   Ver 4.2.0.2 : SCR614 - Allow support for 50 Mercant Type Limits.  Modified Merchant Type Limits dialog - using list control to list all MCC usage codes Auth Profile.  Maximimum number of MCC codes that can be entered is 50 as defined in the database.
   
      Rev 1.5   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:12   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:04   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:12   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:10   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:48   iarustam
    
*
************************************************************************************/
// CardsMerchantLimit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsMerchantLimit dialog
#ifndef CCARDSMERCHANTLIMIT
#define CCARDSMERCHANTLIMIT

#include "app_info.h"

class CCardsMerchantLimit : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsMerchantLimit)

// Construction
public:

#define MAX_MCC_CNT		500 /*max number of mcc codes supported-CAF01 table*/
	CCardsMerchantLimit();
	~CCardsMerchantLimit();

    CAF01 sProfileIdOld;
  //CAF01 sProfileId;

	MCC_LIMITS_GUI	m_mcc_limits_gui_list;
	MCC_LIMITS		m_mcc_limits;


    int m_nDialogMode;
	int nSelectedIndex;

    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

    BOOL m_bMerchant;

    void CleanMerchantDialog();
    void EnableMerchantDialog(int);
    void PopulateHistoryTable();

    BOOL PopulateMerchantDialogFromDatabase( CAF01 sProfileId );    
    BOOL PopulateDatabaseFromMerchantDialog( pCAF01 psProfileId );
    BOOL CompareStructures(pCAF01 psProfileId);
    
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CCardsMerchantLimit)
	enum { IDD = IDD_CARDS_MERCHANT_LIMITS };
	CEdit	m_CntlUSDLimit;
	CEdit	m_CntlAmount;
	CEdit	m_CntlMCC;
	CEdit	m_CntlUses;
	CButton	m_CntlUpdateButton;
	CButton	m_CntlDeleteButton;
	CButton	m_CntlAddButton;
	CListCtrl	m_CntlMCCList;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlProfileId_Fld;
	CString	m_StrMessageLbl;
	CString	m_StrProfileId_Fld;
	CString	m_StrType_Fld;
	CString	m_StrUses;
	CString	m_StrMCC;
	CString	m_StrAmount;
	CString	m_StrUSDLimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsMerchantLimit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsMerchantLimit)
	virtual BOOL OnInitDialog();
	afx_msg void OnMccAddButton();
	afx_msg void OnMccDeleteButton();
	afx_msg void OnMccUpdateButton();
	afx_msg void OnDblclkMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
