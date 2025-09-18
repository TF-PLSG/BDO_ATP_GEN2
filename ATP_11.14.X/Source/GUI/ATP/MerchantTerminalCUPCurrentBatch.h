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
*
************************************************************************************/
// MerchantTerminalCUPCurrentBatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalCUPCurrentBatch dialog
#ifndef CUPCURBATCH
#define CUPCURBATCH


class CMerchantTerminalCUPCurrentBatch : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalCUPCurrentBatch)

// Construction
public:
	CMerchantTerminalCUPCurrentBatch();
	~CMerchantTerminalCUPCurrentBatch();

	BOOL PopulateCurrentBatchDialogFromDatabase (DCF01  );
	void CleanCurrentBatchDialog();

	BCH01 sBatchRecord;
	DCF01 sTerminalRecord;
	BOOL m_bCurrentBatch;
	int m_nDialogMode;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantTerminalCUPCurrentBatch)
	enum { IDD = IDD_TERMINAL_CURRENT_BATCH_CUP };
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
	CString	m_StrCupAccIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalCUPCurrentBatch)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalCUPCurrentBatch)
	afx_msg void OnDetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseButton();
	afx_msg void OnResetBatchNbr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
