#if !defined(AFX_TRANSACTIONDETAILS_H__CD495C28_DE16_4E22_9477_8C57E96F8EED__INCLUDED_)
#define AFX_TRANSACTIONDETAILS_H__CD495C28_DE16_4E22_9477_8C57E96F8EED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransactionDetails.h : header file
//
typedef struct
{ 
  ARCHIVE_TLF01   tlf01;

  EMV_RAW	emvraw;

} TRANS_RECORD;
/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails dialog

class CTransactionDetails : public CDialog
{
// Construction
public:
	CTransactionDetails(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bMaskCard;

// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_TRANDETAILS };
	CButton	m_ChkARQCPresent;
	CButton	m_ChkArqcValidated;
	CButton	m_ChkAtpStip;
	CButton m_ChkTOAtpStip;
	CButton	m_ChkCAVVValidated;
	CButton	m_ChkCAVVPresent;
	CButton	m_ChkVoice;
	CButton	m_ChkSchemeStip;
	CButton	m_ChkPINValidated;
	CButton	m_ChkPINPresent;
	CButton	m_ChkOMD;
	CButton	m_ChkHostProcessed;
	CButton	m_ChkCVV2Validated;
	CButton	m_ChkCVV2Present;
	CButton	m_ChkCvvValidated;
	CButton	m_ChkCVVPresent;
			CEdit	m_CntlFGResultFld;
	CString	m_StrHostTimeFld;
	CString	m_StrAcquiringInstIDFld;
	CString	m_StrAuthIDResponseFld;
	CString	m_StrCardNumberFld;
	CString	m_StrForwardingInstIDFld;
	CString	m_StrInvoiceNumberFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrPOSConditionCodeFld;
	CString	m_StrPOSEntryModeFld;
	CString	m_StrProcessingCodeFld;
	CString	m_StrResponseTextFld;
	CString	m_StrRetrievalRefNumFld;
	CString	m_StrSystemTraceNumberFld;
	CString	m_StrTerminalIDFld;
	CString	m_StrTranAmountFld;
	CString	m_StrTranDateFld;
	CString	m_StrTranTimeFld;
	CString	m_StrTranTypeFld;
	CString m_StrTranMessageTypeFld;
	CString	m_StrResponseCodeFld;
	CString	m_StrTextFld;
	CString	m_StrExpirationDateFld;
	CString	m_StrCreditLimitFld;
	CString	m_StrOutstandingBalanceFld;
	CString	m_StrCurrencyCodeFld;
	CString	m_StrOperatorIDFld;
	CString	m_StrSystemDateFld;
	CString	m_StrEmvFld;
	CString	m_StrFGResultFld;
	CString	m_StrGrossAmountFld;
	CString	m_StrMonthlyAmortFld;
	CString	m_StrTermLengthFld;
	CString	m_StrFactorFld;
	CString	m_StrAcquiringAmtFld;
	CString	m_StrAcquiringCurrencyFld;
	CString	m_StrCardNameFld;
	CString	m_StrCategoryCodeFld;
	CString	m_StrCVCResultCodeFld;
	CString	m_StrServiceCodeFld;
	CString	m_StrStopRegionFld;
	CString	m_StrReasonCodeFld;
	CString	m_StrPurgeDateFld;
	CString	m_StrLocationFld;
	CString	m_StrOriginalTxnAmount;
	CString	m_StrEarMarkedCurrency;
	CString	m_OrgTxnAmtCurrency;
	CString	m_StrECI;
	CString m_StrOriginalECI_Reason;
	CString	m_StrEarMarkedAmount;
	CString	m_StrPosData;
	CString	m_StrBinLow;
	CString	m_StrBdoMercAcqTx;
	CString	m_StrAuthProfile;
	CString	m_StrAcquiringCountry;
	CString	m_StrVisaMerchantIdentifier;
	CString	m_StrDigitalCommProgramIndicator;
	CString	m_StrDigitalEntryIdentifier;
	CString m_StrCUPIndicatorIDFld;
	//}}AFX_DATA

	ARCHIVE_TLF01 sTranRecord;
	BOOL PopulateTranDialogFromDatabase();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionDetails)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransactionDetails)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIONDETAILS_H__CD495C28_DE16_4E22_9477_8C57E96F8EED__INCLUDED_)
