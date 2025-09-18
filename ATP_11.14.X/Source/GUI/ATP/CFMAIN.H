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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFMAIN.H  $
   
      Rev 1.8   Dec 01 2003 11:29:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Mar 16 2000 10:40:08   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.6   Jan 12 2000 14:14:50   mbysani
   Changes mades acc to bugs
   
      Rev 1.1   Sep 22 1999 16:05:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:58   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:55:52   iarustam
   Bug fixes
   
      Rev 1.1   Mar 22 1999 11:12:02   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:41:46   iarustam
    
*
************************************************************************************/
// CFMain.h : header file
//
#include "CFGeneral.h"
#include "CFCapability.h"
#include "CFCardBalAcc.h"
#include "CFDemogr.h"
#include "CFActivity.h"
#include "CFMerchUsage.h"
#include "CFTxHistory.h"
#include "ChangeHistory.h"

/////////////////////////////////////////////////////////////////////////////
// CCFMain dialog

class CCFMain : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFMain)

// Construction
public:
	CCFMain();
	~CCFMain();

	int DialogMode;
	CPropertySheet *m_pSheet;

	CCFGeneral*		pGenPage;
	CCFCapability*	pCapPage;
	CCFCardBalAcc*  pBalPage;
	CCFDemogr*		pDemogrPage;
	CCFActivity*	pActivPage;
	CCFMerchUsage*	pMerchUsPage;
	CCFTxHistory*	pTxPage;
	CChangeHistory*	pChHistPage;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CCF02P	sOldCCF02Record;
	CCF01	sOldCCF01Record;
	CCF03   sOldCCF03Record;


	void SetModes(int mode) ;
	BOOL KeyEntered();
	void SetKeys();
	BOOL GetCCFData(int DlgMode);
	void PopulateHistoryTable();
	void PopulateHistPage();
	void PopulateCCF(pCCF02P psCCF02Record, pCCF01 psCCF01Record );
	void CleanPages();
	BOOL ValidateDate(CString sDate, pCHAR ErrMsg) ;

	CTime c_time;
		// Dialog Data
	//{{AFX_DATA(CCFMain)
	enum { IDD = IDD_CARDCONFIG };
	CButton	m_Search;
	CComboBox	m_ctrlCardType;
	CStatic	m_ComEdit;
	CButton	m_Add;
	CButton	m_View;
	CButton	m_Update;
	CButton	m_List;
	CButton	m_Delete;
	CEdit	m_ctrlCardNbr;
	CString	m_strCardType;
	CString	m_strCardNbr;
	CString	m_strComEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFMain)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFMain)
	virtual BOOL OnInitDialog();
	afx_msg void OnList();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnView();
	afx_msg void OnUpdate();
	afx_msg void OnChangeCardnbr();
	afx_msg void OnChangeCardtype();
	afx_msg void OnSearchButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

