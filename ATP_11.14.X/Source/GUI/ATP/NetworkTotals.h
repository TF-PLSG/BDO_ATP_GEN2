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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkTotals.h  $
   
      Rev 1.5   Dec 01 2003 11:30:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:10   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:50   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:26   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:28   iarustam
    
*
************************************************************************************/
// NetworkTotals.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkTotals dialog
#ifndef CNETWORKTOTALS
#define CNETWORKTOTALS


class CNetworkTotals : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkTotals)

// Construction
public:
	CNetworkTotals();
	~CNetworkTotals();

	NCF01 sNetworkId;
	NCF01 sNetworkIdOld;

	NCF02 sNetwork;
	NCF02 sNetworkOld;

	int m_bTotals;
	int m_nDialogMode;

	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];
    char strFilePrimaryKey[50];

	void CleanTotalsDialog();
	void EnableTotalsDialog(int);
	void PopulateHistoryTable();

	BOOL PopulateDatabaseFromTotalsDialog( pNCF02 psNetwork );
	BOOL PopulateTotalsDialogFromDatabase (NCF02 sNetwork);
	BOOL CompareStructures();
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CNetworkTotals)
	enum { IDD = IDD_NETWORK_TOTALS };
	CEdit	m_CntlNetSettlementAmountFld;
	CEdit	m_CntlTransferNumberFld;
	CEdit	m_CntlTransferReversalNumberFld;
	CEdit	m_CntlAuthorizationNumberFld;
	CEdit	m_CntlInquiryNumberFld;
	CEdit	m_CntlCreditAmountOfReversalTransactionsFld;
	CEdit	m_CntlDebitAmountOfReversalTransactionsFld;
	CEdit	m_CntlDebitAmountOfTransactionsFld;
	CEdit	m_CntlCreditAmountOfTransactionsFld;
	CEdit	m_CntlDebitTransactionFeeAmountFld;
	CEdit	m_CntlCreditTransactionFeeAmountFld;
	CEdit	m_CntlNetworkTypeFld;
	CEdit	m_CntlNetworkIdFld;
	CEdit	m_CntlDebitProcessingFeeAmountFld;
	CEdit	m_CntlCreditProcessingFeeAmountFld;
	CEdit	m_CntlDebitReversalNumberFld;
	CEdit	m_CntlCreditReversalNumberFld;
	CEdit	m_CntlDebitNumberFld;
	CEdit	m_CntlCreditNumberFld;
	CString	m_StrCreditNumberFld;
	CString	m_StrDebitNumberFld;
	CString	m_StrCreditReversalNumberFld;
	CString	m_StrDebitReversalNumberFld;
	CString	m_StrCreditProcessingFeeAmountFld;
	CString	m_StrDebitProcessingFeeAmountFld;
	CString	m_StrNetworkIdFld;
	CString	m_StrNetworkTypeFld;
	CString	m_StrCreditTransactionFeeAmountFld;
	CString	m_StrDebitTransactionFeeAmountFld;
	CString	m_StrMessageLbl;
	CString	m_StrCreditAmountOfTransactionsFld;
	CString	m_StrDebitAmountOfTransactionsFld;
	CString	m_StrDebitAmountOfReversalTransactionsFld;
	CString	m_StrCreditAmountOfReversalTransactionsFld;
	CString	m_StrInquiryNumberFld;
	CString	m_StrAuthorizationNumberFld;
	CString	m_StrTransferReversalNumberFld;
	CString	m_StrTransferNumberFld;
	CString	m_StrNetSettlementAmountFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkTotals)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkTotals)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
