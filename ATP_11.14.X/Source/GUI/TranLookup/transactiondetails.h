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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\tran_lookup\transactiondetails.h  $
   
      Rev 1.9   Nov 22 2005 08:57:00   dirby
   Added Deferred Payment fields to the Detail window.
   SCR 16289
   
      Rev 1.8   Jul 18 2005 12:56:22   lmitchel
   Implement Fraud Guard search criterea and resend failed transactions
   
      Rev 1.7   Jun 14 2005 17:32:18   chuang
   Version 4.4.1.2. Fix SCR 15819, add utility to view EMV Raw Data.
   
      Rev 1.6   Jun 08 2001 11:41:06   SYARLAGA
   Added a string m_StrSystemDateFld
   
      Rev 1.5   Mar 14 2000 10:44:20   iarustam
   SCR #n475,478,479,480
   
      Rev 1.4   Mar 02 2000 17:13:32   iarustam
   SCR 462
   
      Rev 1.3   Jun 11 1999 14:16:58   iarustam
   Bug fixes
   
      Rev 1.2   May 24 1999 15:11:14   iarustam
   Bug fixes
   
      Rev 1.1   May 13 1999 15:05:04   iarustam
   Bug fixes
   
      Rev 1.0   Apr 15 1999 17:03:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 23 1999 11:41:24   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:12   iarustam
   Initial Release
   
      Rev 1.1   Dec 18 1998 13:45:00   iarustam
    
*
************************************************************************************/
// TransactionDetails.h : header file
//

#ifndef TRASCN_DETAILS
#define TRASCN_DETAILS

#include "Fraud_Txn_Details.h"
#include "Visa_EDQP_Txn_Details.h"
#include "Addnl_Txn_Details.h"

#define ICELAND_KRONA_CURRENCY		"352"
#define ZIMBABWE_GOLD				"924"
#define USD							"840"
#define PESO						"608"

typedef struct
{ 
  TLF01   tlf01;

  EMV_RAW	emvraw;

} TRANS_RECORD;



typedef struct
{ 
  TLF01   tlf01;
    EMV_RAW	emvraw;
} TRANS_STIP_RECORD;

/*FRAUD Guard*/
typedef struct
{ FGUARD   fguard;
  FG_RAW	fgraw;
} FGUARD_RECORD;

/////////////////////////////////////////////////////////////////////////////


// CTransactionDetails dialog
class CTransactionDetails : public CPropertyPage
{
// Construction
	DECLARE_DYNCREATE(CTransactionDetails)
private:
	void  checkMaskCardNumber();
public:
	CTransactionDetails();   // standard constructor


	//TLF01 sTranRecord;
	TRANS_RECORD sTranRecord;
	FGUARD_RECORD sFGuardRecord;
	PTSN01 sptsn01;

	TRANS_STIP_RECORD sTranRecordS;

	BOOL m_bTranDetails;
	int  m_nDialogMode;


	BOOL PopulateTranDialogFromDatabase(TRANS_RECORD);
	BOOL PopulateSTIPTranDialogFromDatabase(TRANS_STIP_RECORD);
	BOOL PopulateFGuardDialogFromDatabase(FGUARD_RECORD);
	BOOL PopulateFruadDialogFromDatabase(char *);
	INT Check_If_AMEX_Transaction(char *card_num);
	INT Check_If_Diners_Transaction(char *card_num);
	INT Check_If_MC_Transaction (char *card_num);
	INT Check_If_VISA_Transaction (char *card_num);
	INT Check_If_JCB_Transaction (char *card_num);
	INT Check_If_Acquiring_Transaction (char * handler_queue);
	INT Check_If_Issuing_Transaction (char * handler_queue);
	void CleanTranDialog();

	void match_ISO_to_Diners_response_code(char *,char *);
	void match_ISO_to_AMEX_response_code(char *,char *, BYTE);
	//CFraudTransactionDetails *pFraudDetailsPage;

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
	/*CButton	m_ChkCVV2Validated;
	CButton	m_ChkCVV2Present;
	CButton	m_ChkCvvValidated;
	CButton	m_ChkCVVPresent;*/
	CButton	m_ChkHostProcessed;
	CButton	m_ChkVoice;
	CButton	m_ChkSchemeStip;
	CButton	m_ChkOMD;
	CButton	m_ChkAtpStip;
	CButton m_ChkTOAtpStip;
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
	CString m_StrPOSConditionCodeFld_1;
	CString	m_StrPOSEntryModeFld;
	CString	m_StrPOSEntryModeFld_1;
	CString	m_StrProcessingCodeFld;
	CString	m_StrProcessingCodeFld_1;
	CString	m_StrResponseTextFld;
	CString	m_StrRetrievalRefNumFld;
	CString	m_StrSystemTraceNumberFld;
	CString	m_StrTerminalIDFld;
	CString	m_StrTranAmountFld;
	CString m_StrReplacementAmountFld;
	CString	m_StrTranDateFld;
	CString	m_StrTranTimeFld;
	CString	m_StrTranTypeFld;
	CString	m_StrTranMessageTypeFld;
	CString	m_StrResponseCodeFld;
	CString m_StrResponseCodeFldbyScheme;
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
	/*CString	m_StrBdoMercAcqTx;*/
	CString	m_StrBinLow;
	CString	m_StrEarMarkedAmount;
	CString	m_StrEarMarkedCurrency;
	CString	m_StrAuthProfile;
	CString	m_StrServiceCode;
	CString	m_StrOriginalTxnAmount;
	CString	m_OrgTxnAmtCurrency;
	CString	m_StrECI;
	CString	m_StrECI_1;
	CString m_StrOriginalECI_Reason;
	CString	m_StrMerchantName;
	CString	m_StrMCC;
	CString	m_StrPosData;
	CString	m_StrAcquiringCountry;
	CString	m_StrVisaMerchantIdentifier;
	CString	m_StrDigitalCommProgramIndicator;
	CString	m_StrDigitalEntryIdentifier;
	CString	m_StrCUPIndicatorIDFld;
	CString	m_StrSettlementDateFld;
	CString m_StrEarmarkedamount1;
	CString m_StrEarmarkedcurrency;
	CString m_StrBillingamount;
	CString m_StrBillingCurrency;
	CString m_StrNRID;
	CString m_StrActionCode;
	CString m_StrATP_sbatchtime;
	CString m_StrExtCondCode;
	CString m_Strmrasenttime;
	CString m_StrHostNameFld;
	CString m_StrNii;
	CString m_transactionid;
	CString	m_StrSystemDateTimeFld;
	//}}AFX_DATA

	BOOL	m_bViewEmvData;
	BOOL	m_bViewRawData;
	BOOL	m_bMaskCardNumbers;
	CStringList OriginalCardNoList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionDetails)
	protected:
	afx_msg void OnFraudTxnDetailsButton();
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
#endif