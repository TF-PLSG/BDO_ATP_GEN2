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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalConfig.h  $
   
      Rev 1.6   Jul 16 2004 16:07:44   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.5   Dec 01 2003 11:29:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:40   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:20   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:58   iarustam
    
*
************************************************************************************/
// MerchantTerminalConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalConfig dialog
#ifndef TERMINALCONFIG
#define TERMINALCONFIG

#include "MerchantTerminalGeneral.h"
#include "MerchantTerminalCurrentBatch.h"
#include "MerchantTerminalCUPCurrentBatch.h"
#include "MerchantTerminalBatchHistory.h"
#include "MerchantTerminalKeys.h"
#include "ChangeHistory.h"
#include "TerminalValidTransactions.h"
#include "TerminalAutoInit.h"

//#include "MerchantTerminalPropertySheet.h"

class CMerchantTerminalConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalConfig)

// Construction
public:
	CMerchantTerminalConfig();
	~CMerchantTerminalConfig();

	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL PopulateTerminalDatabaseStructureFromDialog(pDCF01);
	void PopulateTerminalDialogFromDatabase (DCF01);

	CMerchantTerminalGeneral *pGeneralPage;
	CMerchantTerminalCurrentBatch *pCurrentPage;
	CMerchantTerminalCUPCurrentBatch *pCUPCurrentPage;
	CMerchantTerminalBatchHistory *pBatchHistoryPage;
	CMerchantTerminalKeys *pKeysPage;
	CTerminalValidTransactions *pTranPage;
	CTerminalAutoInit *pAutoInitPage;
	CChangeHistory *pChangeHistoryPage;

	DCF01 sTerminalRecordOld;

	int  m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	BOOL m_bDefinition;
	CTime c_time;


//    CMerchantTerminalPropertySheet* m_TerminalSheet;

// Dialog Data
	//{{AFX_DATA(CMerchantTerminalConfig)
	enum { IDD = IDD_MERCHANT_TERMINAL_CONFIGURATION };
	CEdit	m_CntlTerminalIDFld;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlOrganizationIDFld;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrTerminalIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalConfig)
	public:
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalConfig)
	afx_msg void OnTerminalAddButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnTerminalDeleteButton();
	afx_msg void OnTerminalListButton();
	afx_msg void OnTerminalUpdateButton();
	afx_msg void OnTerminalViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
