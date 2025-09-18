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
************************************************************************************/
// TransactionDetails.h : header file
//

#include "CVector.h"

/* Card Types */
#define CARD_AMEX_VALUE             00
#define CARD_VISA_VALUE             01
#define CARD_MASTERCARD_VALUE       02
#define CARD_JCB_VALUE              03
#define CARD_CUP_VALUE              04
#define CARD_DCC_VALUE              05
#define CARD_DINERS_VALUE           06
#define CARD_DUAL_VALUE             07
#define CARD_FLEET_VALUE            8

typedef struct
{ 
  TPOS01   tpos01;

} TRANS_RECORD;


/////////////////////////////////////////////////////////////////////////////


// CTransactionDetails dialog
class CTransactionDetails : public CDialog
{
// Construction
private:
	void  checkMaskCardNumber();
public:
	CTransactionDetails(CWnd* pParent = NULL);   // standard constructor


	//TLF01 sTranRecord;
	TRANS_RECORD sTranRecord;



	BOOL PopulateTranDialogFromDatabase(TRANS_RECORD);

	void CleanTranDialog();

	CString m_strTranID;

// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_TRANDETAILS };
	CButton	m_ChkArqcValidated;
	CButton	m_ChkCAVVValidated;
	CButton	m_ChkCAVVPresent;
	CButton	m_ChkARQCPresent;
	CButton	m_ChkPINValidated;
	CButton	m_ChkPINPresent;
	CButton	m_ChkCVV2Validated;
	CButton	m_ChkCVV2Present;
	CButton	m_ChkCvvValidated;
	CButton	m_ChkCVVPresent;
	CButton	m_ChkHostProcessed;
	CButton	m_ChkVoice;
	CButton	m_ChkSchemeStip;
	CButton	m_ChkOMD;
	CButton	m_ChkAtpStip;
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
	CString	m_StrTranMessageTypeFld;
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
	CString	m_StrBdoMercAcqTx;
	CString	m_StrBinLow;
	CString m_StrBinHigh;
	CString m_StrBinType;
	CString m_StrBinCardFamily;
	CString m_StrNii;
	CString	m_StrEarMarkedAmount;
	CString	m_StrEarMarkedCurrency;
	CString	m_StrAuthProfile;
	CString	m_StrServiceCode;
	CString	m_StrOriginalTxnAmount;
	CString	m_OrgTxnAmtCurrency;
	CString	m_StrECI;
	CString	m_StrMerchantName;
	CString	m_StrMCC;
	CString	m_StrPosData;
	CString	m_StrAcquiringCountry;
	CString	m_StrVisaMerchantIdentifier;
	CString	m_StrDigitalCommProgramIndicator;
	CString	m_StrDigitalEntryIdentifier;
	
	//}}AFX_DATA

	BOOL	m_bViewEmvData;
	BOOL	m_bViewRawData;
	BOOL	m_bMaskCardNumbers;
	CStringList OriginalCardNoList;

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
