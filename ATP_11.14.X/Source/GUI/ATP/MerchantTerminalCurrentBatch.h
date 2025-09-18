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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalCurrentBatch.h  $
   
      Rev 1.6   Jan 09 2004 15:45:50   dirby
   Added controls to support new button that sets batch number to null.
   SCR 1288
   
      Rev 1.5   Dec 01 2003 11:29:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:46   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:40   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:22   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:46   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:00   iarustam
    
*
************************************************************************************/
// MerchantTerminalCurrentBatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalCurrentBatch dialog
#ifndef CURBATCH
#define CURBATCH


class CMerchantTerminalCurrentBatch : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalCurrentBatch)

// Construction
public:
	CMerchantTerminalCurrentBatch();
	~CMerchantTerminalCurrentBatch();

	BOOL PopulateCurrentBatchDialogFromDatabase (DCF01  );
	void CleanCurrentBatchDialog();

	BCH01 sBatchRecord;
	DCF01 sTerminalRecord;
	BOOL m_bCurrentBatch;
	int m_nDialogMode;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantTerminalCurrentBatch)
	enum { IDD = IDD_TERMINAL_CURRENT_BATCH };
	CButton	m_ResetBatchNbr;
	CButton	m_DetailsButton;
	CButton	m_CloseButton;
	CString	m_StrBatchTotalAmountFld;
	CString	m_StrBatchTotalCountFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrCreditRefundsAmountFld;
	CString	m_StrCreditRefundsCountFld;
	CString	m_StrCreditSalesAmountFld;
	CString	m_StrCreditSalesCountFld;
	CString	m_StrDebitrefundsAmountFld;
	CString	m_StrDebitRefundsCountFld;
	CString	m_StrDebitSalesAmountFld;
	CString	m_StrDebitSalesCountFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrTerminalIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalCurrentBatch)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalCurrentBatch)
	afx_msg void OnDetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseButton();
	afx_msg void OnResetBatchNbr();
	afx_msg void OnChangeMerchantIdFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
