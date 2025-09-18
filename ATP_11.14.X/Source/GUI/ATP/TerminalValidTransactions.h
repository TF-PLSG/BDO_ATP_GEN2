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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TerminalValidTransactions.h  $
   
      Rev 1.6   Jul 16 2004 16:07:32   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.5   Dec 01 2003 11:30:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:01:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:26   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:34   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:24   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:22   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:06   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:28   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:44:58   iarustam
    
*
************************************************************************************/
// TerminalValidTransactions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalValidTransactions dialog
#ifndef VALID_TRAN
#define VALID_TRAN

class CTerminalValidTransactions : public CPropertyPage
{
	DECLARE_DYNCREATE(CTerminalValidTransactions)

// Construction
public:
	CTerminalValidTransactions();
	~CTerminalValidTransactions();

	int m_nDialogMode;
	BOOL m_bTran;
    BOOL m_bOpen;

	void EnableTranDialog ( int );
	void CleanTranDialog( int );
	BOOL CompareStructures( DCF01 );
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromTranDialog ( pDCF01 );
	BOOL PopulateTranDialogFromDatabase ( DCF01 );

	DCF01 sTerminalRecordOld;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];	
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CTerminalValidTransactions)
	enum { IDD = IDD_TERMINAL_VALID_TRANSACTIONS };
	CButton	m_Initialization;
	CButton	m_VoidSaleOnLine;
	CButton	m_VoidRefundOnLine;
	CButton	m_TestTransaction;
	CButton	m_Statistics;
	CButton	m_SignatureData;
	CButton	m_SettlementRequest;
	CButton	m_SettlementAfterUpload;
	CButton	m_SalesCompletion;
	CButton	m_SaleAndCash;
	CButton	m_Sale;
	CButton	m_Reversal;
	CButton	m_Refund;
	CButton	m_ReAuth;
	CButton	m_PreAuthorization;
	CButton	m_PleaseWaitAdvice;
	CButton	m_OffLineSale;
	CButton	m_OffLineRefund;
	CButton	m_Logon;
	CButton	m_GuaranteedLateArrival;
	CButton	m_CheckVerification;
	CButton	m_Cash;
	CButton	m_CardVerification;
	CButton	m_BatchUpload;
	CButton	m_BatchDownLineLoad;
	CButton	m_BalanceInquiry;
	CButton	m_Authorization;
	CButton	m_AdjustSale;
	CButton	m_AdjustRefund;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrTerminalIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTerminalValidTransactions)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTerminalValidTransactions)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
