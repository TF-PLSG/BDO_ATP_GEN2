/*********************************************************************************
* Module:		VoiceAuth.cpp : implementation file
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\New_voice_auth\VoiceAuth.cpp  $
   
		Rev   1.74     May 11.5.11          TF@Krishna Kiran S
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
	 Added Creation of SHM & Mutex for Synchronisation of dislpalying Login
	 Form from different windows	
      
		Rev 1.73   Jul 13 2009   TF-Ajay 
    Added code to copy the STIP txn status from bch20 	
	  
		Rev 1.72   Jun 07 2006 16:59:34   dirby
   Updated to make sure a null is at the end of card holder name.
   For some reason, Merchant Name is overwriting this field in TLF01.
   Not sure why. So for now, make sure a null ends the field so txn
   can be inserted into TLF01.
   SCR 20743
   
      Rev 1.71   Apr 19 2006 14:07:34   dirby
   1.  Modified to only get CVV2 from field if length is not zero. SCR 20916.
   2.  Modified to always log auth_tx to disc on DB missing null errs,
       and to display to user if problem is not in TLF01. SCR 20743.
   
      Rev 1.70   Apr 18 2006 18:23:24   dirby
   Added checking and logging for when database fails to update
   due to missing NULLs.  Every field in TLF01 is checked to see if
   any element overflows its trailing null.  If a field does, it is displayed
   on the screen for the operator to see.  Also, the entire AUTH_TX 
   is logged into \log\VoiceAuth.log.
   SCR 20743
   
      Rev 1.69   Mar 09 2006 14:12:56   dirby
   Modified to allow a cancellation of a Card Verify by treating it as a Sale Reversal.
   SCR 19889
   
      Rev 1.68   Dec 13 2005 14:56:26   dirby
   Voice Auth was not working (txn was approved, but with an error
   message, and balances were not reversed) for Cancel's when
   MID or OID was not entered.  Corrected this by assuming a value
   of 0000 for OID.  Also corrected the code to get all the merchant
   info when OID is missing.  Was not doing this previously.
   SCR 11996
   
      Rev 1.67   Jul 08 2005 10:56:54   dirby
   Modified to recognize OFF-US transactions for Override Denial passwords.
   SCR 16358
   
      Rev 1.66   Jun 22 2005 12:10:42   chuang
   No check for auth number input for ON-Us cards.
   
      Rev 1.65   Jun 22 2005 10:33:36   chuang
   Cancel operation need auth number input to get original transaction.
   
      Rev 1.64   Jun 21 2005 14:33:24   chuang
   Fix SCR 15724. Don't allow override if the response code is from network.
   
      Rev 1.63   Jun 21 2005 13:13:16   chuang
   Fix SCR 15723, disable auth number input if the transaction is OFFUS.
   
      Rev 1.62   May 17 2005 17:31:42   dirby
   Modified to allow Overrides for remote authorization txns.
   SCR 15420
   
      Rev 1.61   Apr 04 2005 16:19:44   lmitchel
   Rebuild 4_4_1
   
      Rev 1.60   Nov 11 2004 09:29:28   dirby
   Modified to display errors in the Response Text field in addition
   to being in the Label field (where they have always been).  This
   is because on Equitable's workstations, the Label field is off the
   monitor and they cannot see the messages.
   SCR 13001
   
      Rev 1.59   Nov 02 2004 18:12:46   dirby
   Added code to validate merchant card type (MCT01) during the
   Verify process.
   SCR 12950
   
      Rev 1.58   Sep 27 2004 10:19:24   lmitchel
   Version 4.4.0.3; SCR12484: Added host id to voice auth title bar
   
      Rev 1.57   Sep 15 2004 12:55:42   lmitchel
   Ver 4_4_0_2: SCR1446:  Clear Auth Response Fields on database failure to update account or insert to bch20.  Populate Auth Response text with correct response code and response text.
   
      Rev 1.56   Jun 17 2004 15:40:58   dirby
   For cancels, changed code to compare first 2-bytes of processing
   code instead of the whole thing.  This helps to determine if operator
   is cancelling the correct transaction type and since MasterCard
   sets the third byte to '3', Voice should only compare first 2-bytes.
   SCR 1403
   
      Rev 1.55   Jun 03 2004 16:42:46   dirby
   Added one line to clear the Card Status field when the CLEAR
   button is pressed.
   SCR 1385
   
      Rev 1.54   May 21 2004 16:17:18   dirby
   Added card status field and code to get card record followed by
   the card status.  Fixed bug where response text was not being
   displayed.  Fixed bug, by using a temp auth_tx, where some
   scenarios would allow an Override Denial when they should not.
   These scenarios resulted from txcntl rejecting a txn prior to getting
   CAF and Card information.
   SCR 1385
   
      Rev 1.53   May 20 2004 17:47:22   dirby
   Corrected some bugs in the CVC2 validation area.
   SCR 1386
   
      Rev 1.52   May 20 2004 16:37:36   lmitchel
   Added check to disable override button also if bin01.auth_ind = zero (REMOTE).  In some cases like mod-10 check the caf01 record has not been selected yet.
   
      Rev 1.51   May 20 2004 12:20:34   dirby
   Modified the checking on the CVV2 field for these rules:
   1.  ECN Visa/MC/JCB: Mandatory 3-digits
   2.  ECN Amex: Optional 3-digits
   3.  Off-Us Visa/JCB: Optional 3-digits
   4.  Off-Us MC: Mandatory 3-digits
   5.  Off-Us Amex: Optional 4-digits
   6.  Visa Electron: Mandatory 3 digits
   SCR 1386
   
      Rev 1.50   May 19 2004 16:06:54   lmitchel
   SCR1385 - Disabled override button if the cardholder status = 06/lost card
   
      Rev 1.49   Apr 13 2004 14:41:06   dirby
   Modified to make CVV2 option for ON-US Amex cards.
   SCR 1347
   
      Rev 1.48   Feb 13 2004 13:40:02   dirby
   Modified to insert declined record into TLF01 when there is a
   problem with the update all processing.  The data server does
   not insert into TLF01 in this case, so it needs to be done here
   to keep a record of what really happened for the operator.
   SCR 1126
   
      Rev 1.47   Jan 28 2004 16:54:08   dirby
   Added code to get original transaction date and time after
   retrieving original transaction from BCH20 for Cancels.  This is
   needed for Amex transactions.
   SCR 1327
   
      Rev 1.46   Oct 13 2003 16:23:10   dirby
   Modified to make 4DBC/4CSC mandatory for Off-Us Amex.
   SCR 1246
   
      Rev 1.45   Aug 29 2003 11:19:36   dirby
   Increased the length of field StrCVV2Fld from 3 to 4 to allow the
   4 digit Amex DBC/CSC value.  If value has length of 4, store in
   'interest_rate', else store in cvc_data like before.  Cvc_data is
   not large enough to hold 4 digits for Amex, so we must use a
   different field - interest_rate.
   SCR 1177
   
      Rev 1.44   May 20 2003 17:37:00   dirby
   Modified to display gross amount and monthly amort amount
   for RSB cards.  Also modified to not recalculate these same
   values for deferred txns, but use the values from TLF01.
   SCR 1123
   
      Rev 1.43   Apr 30 2003 14:40:46   dirby
   Added global variable TranField to pass the value of the Transaction
   Type field to the OverridePassword file.  This is because the
   password varies depending on transaction type; there are two
   passwords: one for deferred transactions, and one for all others.
   SCR 1109
   
      Rev 1.42   Apr 23 2003 17:12:52   dirby
   Removed a line of code to allow Override Denials on Deferred
   transactions.  Added a line to set focus on the Override Denial
   button; without this, the hot button and tabbing do not work.
   SCR 1100
   
      Rev 1.41   Apr 01 2003 12:05:00   lmitchel
   Ver 4.0.0.2 SCR1020:  Added function to receive and validate operator password on selectino of Override Denial Button.
   
      Rev 1.40   Dec 04 2002 14:04:16   dirby
   Modified to not use date/time of original transaction for cancellations.
   Instead use system time.
   SCR 940
   
      Rev 1.39   Nov 08 2002 11:13:28   dirby
   Modified to not insert record into TLF01.  This is now being done
   by the update_all process.
   SCR 845
   
      Rev 1.38   Oct 17 2002 13:45:10   dirby
   Modified code to compare processing codes when doing
   reversals (cancels).  It compares the processing code of the
   original transaction with that set by the operator for the cancel.
   If they do not match, the cancel request is denied and focus
   is set on the transaction type; a message is also displayed
   indicating that the operator may have selected the wrong tran type.
   SCR 730
   
      Rev 1.37   Sep 06 2002 16:20:50   dirby
   Added function OnChangeCVV2Fld( ) to force reverify if CVV2
   changes after Verify has already been performed.
   SCR 892
   
      Rev 1.36   Sep 06 2002 14:46:22   dirby
   1.  Modified to required CVV2 for Deferred Transactions.  SCR 894
   2.  Modified error msg when bin not found; also to not forward the request.  SCR 895
   
      Rev 1.35   Aug 15 2002 16:37:16   dirby
   Modified to support separate bin ranges for deferred txns.
   Search them first, on deferreds, then if not found, search the
   credit bin ranges.
   SCR 551
   
      Rev 1.34   Aug 08 2002 17:19:52   dirby
   Made the Verify function more modular, at least where fetching
   a database record is concerned. Made various changes to
   get data from original txn for Cancels and populate Auth_Tx and
   the GUI fields.  Modified to make CVV2 required for all Local
   txns, all MasterCard txns, and all Visa Electron txns.  Made sure
   Override Denial is disabled for all Off-Us txns.
   SCR 858
   
      Rev 1.33   Aug 02 2002 11:56:52   dirby
   Modified to validate expiration date (this had been removed
   without intending to).  Also modified to make sure Override
   Denial is not enabled for remote transactions.
   SCR 858
   
      Rev 1.32   Aug 01 2002 16:00:02   lmitchel
   SCR858-voice_auth.cpp - populate mcc code, currency code and rrn for cancelling remote network transactions .
   
      Rev 1.31   Jul 30 2002 10:48:42   dirby
   Updated to give more detailed message when bin record is not found.
   SCR 857
   
      Rev 1.30   Jul 24 2002 16:55:20   dirby
   Added code to get merchant record based on MID from BCH20
   or NCF30 for cancellations.  This is even if the Operator enters
   one - they might enter the incorrect merchant ID.
   SCR 810
   
      Rev 1.29   Jul 12 2002 11:01:10   dirby
   1.  Modified to insert txn into TLF01 when update all fails.
   2.  Modified to not populate cardholder name and merchant name
   fields if they already have data in them.
   SCR 811
   
      Rev 1.28   Jul 10 2002 10:38:50   dirby
   Added code to populate Bank Name on Verify instead of
   waiting until after the commit.
   SCR 586
   
      Rev 1.27   Jul 09 2002 14:12:24   dirby
   Added code to allow for cancellation of network transactions.
   This means going to NCF30 for the original txn, and not
   requiring Merchant information.
   SCR 810
   
      Rev 1.26   May 28 2002 16:39:04   dirby
   Changed function SendVoiceAuthTransaction from void to
   BOOL so that txn does not get inserted into TLF01 when there
   is no response due to time out or applnk socket problem.
   SCR 736
   
      Rev 1.25   Apr 16 2002 16:08:38   dirby
   Added a line of code to store original transmission timestamp
   into orig_local_time_date for reversals.  Some network controllers
   look for it here.
   Discovered this problem while working on SCR 704, but it is
   really related to SCR 700.
   
      Rev 1.24   Mar 15 2002 15:53:10   dirby
   Modified to disable Override Denial if txn is declined due to
   merchant not accepting the card type.
   SCR 622
   
      Rev 1.23   Mar 13 2002 16:42:44   dirby
   Modified to set tx_key for Quasi Cash to the same that is set
   for Cash Advance.  This is so they get processed similarly,
   updating both cash and purchase accounts.
   
   Also corrected a bug from SCR 620 by adding an else statement.
   SCR 617
   
      Rev 1.22   Mar 11 2002 11:21:50   dirby
   Modified to not look in TLF01 for Cancellation of Deferred transactions.
   It was previously looking there to confirm the term, but now
   term is in BCH20, and we need to look there anyways.  This
   change speeds up processing.  Cancellation of deferreds was
   timing out at Equitable.
   SCR 620
   
      Rev 1.21   Jan 30 2002 16:44:18   dirby
   Modified to disable Override Denial button for Deferred Txns.
   SCR 589
   
      Rev 1.20   Jan 29 2002 12:30:40   dirby
   Corrected a copy into response_text field to leave room for the null.
   
      Rev 1.19   Jan 28 2002 13:59:56   dirby
   Modified to store Voice Text data into TLF01 in the Voice_Auth_Text field,
   and to store the response text into
   TLF01 response_text field.  Previously, nothing was being
   stored into voice_auth_text in TLF01.
   SCR 584
   
      Rev 1.18   May 31 2001 10:28:38   dirby
   Modified to get terminal Id from Private_field2 for Cancel Deferred Purchases.
   This is so that reversals of deferreds have the same TID as deferred txns.
   SCR HD315
   
      Rev 1.17   May 18 2001 16:45:56   lmitchel
   Updated file version info in .rc file
   SCR312:  For override denial on deferred transaction, populating gross amount and monthly amortization amount
   SCR314: Disable commit button on denial response
   Enable verify button after card number error after commit
**********************************************************************************/
//#define _WIN32_WINNT 0x0500
#include "stdafx.h"
#include <math.h>
#include "VoiceAuthorization.h"
#include "VoiceAuth.h"
#include "MyUtils.h"
#include "OverridePassword.h"
#include "AfxWin.h"
#include "voiceauthorization.h"
#include "IdleLoginfrm.h"
#include "SharedMem.h"
#include <windows.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#ifdef MessageBox
//#undef MessageBox
//#endif

CSharedMem shrmem;


extern "C"
{
   #include "ntutils.h"
   #include "ptetime.h"
   #include "ntutils.h"
   #include "Txutils.h"
}

#define  REMOTE_ORIGIN  0
#define  LOCAL_ORIGIN   1

#define  OFFUS  0
#define  ONUS   1

#define MC13_OCT2023 1

#ifdef MC13_OCT2023
   char mc_mac_code[41] = "";
#endif


extern CString name;
extern int idleTime;;

extern char strFileRecord[56];
extern	UCF01 ucf01;

char systemDate_mmddyyyy[20] = "";
CHAR	 ini_file[256];
INT      Deferred_Txn;
INT      merchant_record_exists;
INT      cancel_txn_origin = LOCAL_ORIGIN;
INT      PasswordCheckDone;
CString  TranField;

CIdleLoginfrm loginfrm;
CTime cur_time;
/////////////////////////////////////////////////////////////////////////////
// CVoiceAuth dialog


CVoiceAuth::CVoiceAuth(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiceAuth::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiceAuth)
	m_StrAccountNumberFld = _T("");
	m_StrAmountFld = _T("");
	m_StrAuthNumOutputFld = _T("");
	m_StrAuthNumberFld = _T("");
	m_StrBankNameFld = _T("");
	m_StrCardholderNameFld = _T("");
	m_StrCurrencyCodeFld = _T("");
	m_StrCVV2Fld = _T("");
	m_StrDeferredTermFld = _T("");
	m_StrExpiryDateFld = _T("");
	m_StrGrossAmountFld = _T("");
	m_StrMCCFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMerchantNameFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMonthlyAmountFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrResponseTextFld = _T("");
	m_StrTextFld = _T("");
	m_StrTransactionTypeFld = _T("");
	m_BoolMod10Check = FALSE;
	m_StrIndustryCodeFld = _T("");
	m_StrCardStatusFld = _T("");
	//}}AFX_DATA_INIT
	pLock = new CDMLock("IdleLock");
	m_disposition = ONUS;
	m_bEnableOverrideDenial = false;
}

CVoiceAuth::~CVoiceAuth()
{
	delete pLock;
	pLock = NULL;

	delete loginfrm;
}

void CVoiceAuth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceAuth)
	DDX_Control(pDX, IDC_INDUSTRY_CODE, m_CntlIndustryCodeFld);
	DDX_Control(pDX, IDC_MOD10_CHECK_BUTTON, m_CntlMod10CheckButton);
	DDX_Control(pDX, IDC_VERIFY_BUTTON, m_VerifyButton);
	DDX_Control(pDX, IDC_CURRENCY_CODE, m_CntlCurrencyCodeFld);
	DDX_Control(pDX, IDC_AUTH_NUMBER_FLD, m_CntlAuthNumberFld);
	DDX_Control(pDX, IDC_TRANSACTION_TYPE, m_CntlTransactionTypeFld);
	DDX_Control(pDX, IDC_TEXT_FLD, m_CntlTextFld);
	DDX_Control(pDX, IDC_PROCESS_AUTH_BUTTON, m_ProcessButton);
	DDX_Control(pDX, IDC_OVERRIDE_DENIAL_BUTTON, m_OverrideButton);
	DDX_Control(pDX, IDC_ORG_ID_FLD, m_CntlOrganizationIDFld);
	DDX_Control(pDX, IDC_OK1, m_OKButton);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_MCC_FLD, m_CntlMCCFld);
	DDX_Control(pDX, IDC_EXPIRY_DATE_FLD, m_CntlExpiryDateFld);
	DDX_Control(pDX, IDC_DEFERRED_PURCHASE_TERM_FLD, m_CntlDeferredTermFld);
	DDX_Control(pDX, IDC_CVV2_FLD, m_CntlCVV2Fld);
	DDX_Control(pDX, IDC_CLEAR_BUTTON, m_ClearButton);
	DDX_Control(pDX, IDC_AMOUNT, m_CntlAmountFld);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_CntlAccountNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDV_MaxChars(pDX, m_StrAccountNumberFld, 19);
	DDX_Text(pDX, IDC_AMOUNT, m_StrAmountFld);
	DDV_MaxChars(pDX, m_StrAmountFld, 13);
	DDX_Text(pDX, IDC_AUTH_NUM_FLD, m_StrAuthNumOutputFld);
	DDX_Text(pDX, IDC_AUTH_NUMBER_FLD, m_StrAuthNumberFld);
	DDV_MaxChars(pDX, m_StrAuthNumberFld, 6);
	DDX_Text(pDX, IDC_BANK_NAME_FLD, m_StrBankNameFld);
	DDX_Text(pDX, IDC_CARDHOLDER_NAME_FLD, m_StrCardholderNameFld);
	DDX_Text(pDX, IDC_CURRENCY_CODE, m_StrCurrencyCodeFld);
	DDV_MaxChars(pDX, m_StrCurrencyCodeFld, 3);
	DDX_Text(pDX, IDC_CVV2_FLD, m_StrCVV2Fld);
	DDV_MaxChars(pDX, m_StrCVV2Fld, 4);
	DDX_Text(pDX, IDC_DEFERRED_PURCHASE_TERM_FLD, m_StrDeferredTermFld);
	DDV_MaxChars(pDX, m_StrDeferredTermFld, 3);
	DDX_Text(pDX, IDC_EXPIRY_DATE_FLD, m_StrExpiryDateFld);
	DDV_MaxChars(pDX, m_StrExpiryDateFld, 4);
	DDX_Text(pDX, IDC_GROSS_AMOUNT_FLD, m_StrGrossAmountFld);
	DDX_Text(pDX, IDC_MCC_FLD, m_StrMCCFld);
	DDV_MaxChars(pDX, m_StrMCCFld, 4);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MERCHANT_NAME_FLD, m_StrMerchantNameFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_MONTHLY_AMORTIZATION_AMOUNT_FLD, m_StrMonthlyAmountFld);
	DDX_Text(pDX, IDC_ORG_ID_FLD, m_StrOrganizationIDFld);
	DDV_MaxChars(pDX, m_StrOrganizationIDFld, 4);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_RESPONSE_TEXT_FLD, m_StrResponseTextFld);
	DDX_Text(pDX, IDC_TEXT_FLD, m_StrTextFld);
	DDV_MaxChars(pDX, m_StrTextFld, 240);
	DDX_CBString(pDX, IDC_TRANSACTION_TYPE, m_StrTransactionTypeFld);
	DDX_Check(pDX, IDC_MOD10_CHECK_BUTTON, m_BoolMod10Check);
	DDX_Text(pDX, IDC_INDUSTRY_CODE, m_StrIndustryCodeFld);
	DDV_MaxChars(pDX, m_StrIndustryCodeFld, 3);
	DDX_Text(pDX, IDC_CARD_STATUS_FLD, m_StrCardStatusFld);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVoiceAuth, CDialog)
	//{{AFX_MSG_MAP(CVoiceAuth)
	ON_BN_CLICKED(IDC_OK1, OnOk1)
	ON_BN_CLICKED(IDC_OVERRIDE_DENIAL_BUTTON, OnOverrideDenialButton)
	ON_BN_CLICKED(IDC_PROCESS_AUTH_BUTTON, OnProcessAuthButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
	ON_CBN_SELCHANGE(IDC_TRANSACTION_TYPE, OnSelchangeTransactionType)
	ON_BN_CLICKED(IDC_VERIFY_BUTTON, OnVerifyButton)
	ON_BN_CLICKED(IDC_MOD10_CHECK_BUTTON, OnMod10CheckButton)
	ON_EN_CHANGE(IDC_ACCOUNT_NUMBER_FLD, OnChangeAccountNumberFld)
	ON_EN_CHANGE(IDC_MERCHANT_ID_FLD, OnChangeMerchantIdFld)
	ON_EN_CHANGE(IDC_CVV2_FLD, OnChangeCVV2Fld)
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_TCARD()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuth message handlers

BOOL CVoiceAuth::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	UCF01 sUCFRecord;
	CString temp;
/*    CRF01_LIST  sCRF01List;*/  /*08/2000 lm - remove currency code drop list */
/*    CRF01  sCurrencyCode;*/	 /*populate from merchant record               */
/*    int nNumberReturned;*/ 
    CString strCurrencyCode, strCode, strName;
	CString title_buffer, title_text;

    char strErrorMessage[300] = "";	

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_nDialogMode = MODE_DONE;

	m_CntlTransactionTypeFld.SetCurSel(0);
    m_OverrideButton.EnableWindow(FALSE);
	m_ProcessButton.EnableWindow(FALSE);
	m_CntlMod10CheckButton.EnableWindow(TRUE);
	m_BoolMod10Check = TRUE;
	UpdateData(FALSE);
	PasswordCheckDone = FALSE; /*Used for override denial password control*/
/* Validation already done, need to fill up UCF01 structure from exisitng one- Tf Phani*/
	// get the access rights to Voice Authorization screen
//	memset ( &sUCFRecord, 0, sizeof ( UCF01 ) );
//	memcpy ( sUCFRecord.primary_key.name, u_name, sizeof (sUCFRecord.primary_key.name) );
//int rcode = txdsapi_get_record( (pBYTE)&sUCFRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage );
//	#ifdef _DEBUG
	 int  rcode = 0;
//	#endif

	if( PTEMSG_NOT_FOUND == rcode )
    {
      DisplayErrorMsg( "Invalid User ID" );

     m_CntlAccountNumberFld.EnableWindow(FALSE);
	  m_CntlAmountFld.EnableWindow(FALSE);
	  m_CntlAuthNumberFld.EnableWindow(FALSE);
	  m_CntlDeferredTermFld.EnableWindow(FALSE);
	  m_CntlExpiryDateFld.EnableWindow(FALSE);
	  m_CntlMerchantIDFld.EnableWindow(FALSE);
	  m_CntlTransactionTypeFld.EnableWindow(FALSE);
	  m_CntlCVV2Fld.EnableWindow(FALSE);
	  m_CntlOrganizationIDFld.EnableWindow(FALSE);
      m_CntlMCCFld.EnableWindow(FALSE);
      m_CntlCurrencyCodeFld.EnableWindow(FALSE);
      m_CntlIndustryCodeFld.EnableWindow(FALSE);

      m_ProcessButton.EnableWindow(FALSE);
	  m_VerifyButton.EnableWindow(FALSE);
      m_CntlTextFld.EnableWindow(FALSE);
      m_ClearButton.EnableWindow(FALSE);
      m_OKButton.SetButtonStyle(BS_DEFPUSHBUTTON ,1);
      m_OKButton.SetFocus();

		return FALSE;
   }
	else if ( PTEMSG_INVALID_DATATYPE == rcode )
	{
      DisplayErrorMsg( "Please try again" );
		return FALSE;
	}

   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      return FALSE;
   }
/*Remove population of currency code list  08/10/00 LM*/
/*	rcode = txdsapi_get_list((pBYTE)&sCurrencyCode, sizeof (CRF01), 
								  (pBYTE)&sCRF01List,sizeof (CRF01_LIST), ST2_DB_SELECT_LIST_BY_PARAM,
								  CRF01_DATA, strErrorMessage) ;	

   if (rcode == PTEMSG_OK)  
   { // Dataserver returned DBERR_OK
		nNumberReturned = atoi ((char *)&sCRF01List.num_returned);		
		for (  int i = 0; i < nNumberReturned; i++ ) {
		   strCode = sCRF01List.crf01_list[i].primary_key.currency_code ;
		   strName = sCRF01List.crf01_list[i].currency_name ;
         int len = strName.GetLength();
         if ( len != 0 ) 
            strCurrencyCode = strCode + "  *" + strName + "*";
         else 
            strCurrencyCode = strCode;
        
		   m_CntlCurrencyCodeFld.AddString(strCurrencyCode);  			
      }
	}
*/

	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = sUCFRecord.control[i].subsystem_name;
		if (  temp == "Voice Authorization" )
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( '1' == sUCFRecord.control[nIndex].action[0]  &&
         ( '1' == sUCFRecord.control[nIndex].action[1]  ) &&
         ( '1' == sUCFRecord.control[nIndex].action[2]  ) &&
         ( '1' == sUCFRecord.control[nIndex].action[3]  ) )

		{
			m_CntlAccountNumberFld.EnableWindow(TRUE);
			m_CntlAmountFld.EnableWindow(TRUE);
			m_CntlExpiryDateFld.EnableWindow(TRUE);
			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);
			m_CntlTransactionTypeFld.EnableWindow(TRUE);
			m_CntlCurrencyCodeFld.EnableWindow(TRUE);
			m_CntlIndustryCodeFld.EnableWindow(TRUE);
			m_CntlCVV2Fld.EnableWindow(TRUE);
			m_ProcessButton.EnableWindow(FALSE);
			m_ClearButton.EnableWindow(TRUE);
		}
		else
		{
           m_CntlAccountNumberFld.EnableWindow(FALSE);
		   m_CntlAmountFld.EnableWindow(FALSE);
		   m_CntlAuthNumberFld.EnableWindow(FALSE);
		   m_CntlDeferredTermFld.EnableWindow(FALSE);
		   m_CntlExpiryDateFld.EnableWindow(FALSE);
		   m_CntlMerchantIDFld.EnableWindow(FALSE);
		   m_CntlTransactionTypeFld.EnableWindow(FALSE);
		   m_CntlCVV2Fld.EnableWindow(FALSE);
		   m_CntlOrganizationIDFld.EnableWindow(FALSE);
           m_CntlMCCFld.EnableWindow(FALSE);
           m_CntlCurrencyCodeFld.EnableWindow(FALSE);
           m_CntlIndustryCodeFld.EnableWindow(FALSE);

           m_ProcessButton.EnableWindow(FALSE);
		   m_VerifyButton.EnableWindow(FALSE);
           m_CntlTextFld.EnableWindow(FALSE);
           m_ClearButton.EnableWindow(FALSE);
		}
/*		if(!m_bEnableOverrideDenial)
		{
			m_OverrideButton.ShowWindow(SW_HIDE);
		}
*/

	}	
   
	/*Set title bar with site and host id*/ 
	GetWindowText(title_text);
    title_buffer.Format("%s     (%s)", title_text, strFileRecord);
	SetWindowText(title_buffer);
 
	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );

	//Get the Idle TIme from tf.ini
	GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "IDLE_TIMEOUT_VALUE",	   // points to key name 
        "600",					   // points to default string 
        idle_time_val,             // points to destination buffer 
        sizeof(idle_time_val) - 1, // size of destination buffer 
        ini_file );                // points to initialization filename 
		 
	    idle_time_val[sizeof(idle_time_val)] = 0; //Make sure string is terminated
		
		idle_time = atoi(idle_time_val);

	//Start the Timer
	timer_handle = StartTimer(1000);    
   	
	cntr=0;

   shrmem.CreateMutexLock();
   shrmem.CreateSharedMemorySegment();
   shrmem.MapSharedMemorySegment();   

	//Krishna : Get the Current time & store it in SHM
	c_time = CTime::GetCurrentTime();
	shrmem.SetLastUpdateTime(c_time);

   UpdateData(FALSE);   

   	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);


    //*******************

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVoiceAuth::OnOk1() 
{
   //StopTimer();   
   shrmem.DeleteMutexLock();
   OnOK();
}

void CVoiceAuth::OnOverrideDenialButton() 
{
	CHAR strErrorMessage[300] = "";
	CHAR strBuffer[350] = "";
	INT  ret = 0;
	CMyUtils myUtilsObject;
   	CIdleLoginfrm login;
	//CHAR error_msg[256];
	m_bEnableOverrideDenial = false;
	
	login.m_userid = _T("");
	login.m_userpwd = _T("");
	login.m_szTitle = _T("Override Merchant Denial");
	ret = login.CheckLoginDetails();
	if(ret == false || ret == FALSE || ret == CANCEL) 
	{
		return;
	}
	else
	{
		//Voice auth override denial 3rd user authorization.
		int rcode;
		
		UCF01 tmp_ucf01;
		memset(&tmp_ucf01, 0x00, sizeof(ucf01));
		
		strcpy( (char *)tmp_ucf01.primary_key.name, login.m_userid );
		
		rcode = txdsapi_get_record( (pBYTE)&tmp_ucf01,sizeof (UCF01), UCF01_DATA, strErrorMessage );
		if(rcode == PTEMSG_OK)
		{
			m_bEnableOverrideDenial = atoi((LPSTR) tmp_ucf01.user_info_5);
			
		}
		if(!m_bEnableOverrideDenial)
		{
			AfxMessageBox("Not authorized to Override denial");
			return;
		}
		
	}
	
	
	UpdateData(TRUE); /*retrive data*/
	
					  /* Check password for override denial:
					  *    if return ok, continue,
					  *    else Invalid Password is displayed
					  *
					  * Set a global variable 'TranField' to pass the Transaction Type
					  * field to the OverridePassword file.  The password varies based
					  * on transaction type - Deferred Transactions have a different password.
    */
	if ( m_disposition == OFFUS )
		TranField = "OFFUS"; /* Requires different password. */
	else
		TranField = m_StrTransactionTypeFld;
	
		/* if(!PasswordCheckDone)
		{
		COverridePassword OverridePassword;
		OverridePassword.m_strOverridePassword.Empty();
		
		  ret = OverridePassword.DoModal();
		  if( ret == IDCANCEL  )
		  {
		  DisplayErrorMsg( "Password is required for Override." );
		  return;
		  }
		}*/
	
	/*The override password has been checked and approved for this transaction*/
	
	m_nDialogMode = MODE_OVERRIDE;
	PasswordCheckDone = TRUE;
	
	m_StrAuthNumOutputFld = _T("");
	m_StrGrossAmountFld   = _T("");
	m_StrMonthlyAmountFld = _T("");
	m_StrResponseCodeFld  = _T("");
	m_StrResponseTextFld  = _T("");
	
	m_ProcessButton.EnableWindow(FALSE);
	m_VerifyButton.EnableWindow(FALSE);
	
	if ( ( m_disposition == ONUS ) && ( myUtilsObject.IsEmpty( m_StrAuthNumberFld, this ) ) )
	{
		/*Any Override operation for an ONUS transaction, need to have the Authorization Number input*/
		DisplayErrorMsg( "Please enter Authorization Number" );
		m_CntlAuthNumberFld.SetFocus();
	}
	else
	{
		UpdateData(FALSE);
		PopulateTLFStructure();
		
		if ( SendVoiceAuthTransaction() )
		{
			if ( strlen ((char *)sAuthTxRecord.TLF01_details.date_yyyymmdd) < 8 )
				memcpy ( sAuthTxRecord.TLF01_details.date_yyyymmdd,
				GetSystemDate(),
				sizeof(sAuthTxRecord.TLF01_details.date_yyyymmdd)-1 );
			
			if ( strlen ((char *)sAuthTxRecord.TLF01_details.time_hhmmss) < 6 )
				myUtilsObject.GetSystemTime( (pCHAR)(sAuthTxRecord.TLF01_details.time_hhmmss) );
			
			int rcode = txdsapi_send_all_data ( &sAuthTxRecord,  strErrorMessage );
			if( PTEMSG_OK != rcode )
			{
            /* Transaction failed on the update, so insert
			* into TLF01 so we have a record of the failure.
			* (Sept 14, 04 LM SCR1446 fix) Also update response dialog fields
			* to reflect database failure.
				*/
				if ( (rcode == 2) || (rcode == 5) )
				{
					checkTLF01_fields( strErrorMessage );
				}
				sprintf( strBuffer,
                    "DB Update Failure: %s",
					strErrorMessage );
				DisplayErrorMsg( strBuffer );
				m_StrResponseCodeFld = "96";
				m_StrAuthNumOutputFld = _T("");
				m_StrGrossAmountFld = _T("");
				m_StrMonthlyAmountFld = _T("");
				
				memcpy( sAuthTxRecord.TLF01_details.response_code, "96", 2 );
				memcpy( sAuthTxRecord.TLF01_details.response_text, "Database update failure", 23 );
				memset( &sAuthTxRecord.TLF01_details.auth_number, 0x00, sizeof(sAuthTxRecord.TLF01_details.auth_number));
				
				rcode = txdsapi_insert_record( (pBYTE)&sAuthTxRecord,
					sizeof(TLF01),
					TLF01_DATA,
					strErrorMessage );
			}
			else
				m_StrMessageLbl = "Transaction Completed";
		}
		
		m_OverrideButton.EnableWindow(FALSE);
		m_CntlAuthNumberFld.EnableWindow(FALSE);
		m_CntlTextFld.EnableWindow(FALSE);
		PasswordCheckDone = FALSE; /*reset for next transaction*/
	}
	
	UpdateData(FALSE);
	
	return;
}

BOOL CVoiceAuth::PopulateTLFStructure() 
{
	CHAR  strTxKey;
	CString strAmount;
	CHAR strErrorMessage[300];
	CMyUtils myUtilsObject;
	CHAR strTime[15];
	CHAR wstation[21]; 
   DWORD wsize;
   CString strCategoryCode;
   CString strCurrencyCode;
	CString strUniqueStan;
	CHAR strtime[20] = "";
	CHAR strDeviceID[20] = "";
   CHAR strLen[5];
   CHAR temp_processing_code[3];
   CHAR orig_processing_code[3];
   CHAR cancel_tran_type[30];
   CHAR orig_tran_type[30];
   INT  index;
   INT  nLength;


	m_StrMessageLbl = "";
	m_StrResponseTextFld.Empty();
	UpdateData(FALSE);


   /* Start with a clean Auth_Tx structure, unless it is a reversal. */
   if ( m_StrTransactionTypeFld.Left(6) != "Cancel"  )
      ZeroMemory( &sAuthTxRecord, sizeof(AUTH_TX) );

   memcpy( &sAuthTxRecord.MCF01_details, &sMCFRecord, sizeof(MCF01) );

   if ( (m_StrTransactionTypeFld == "Deferred Purchase") ||
        (m_StrTransactionTypeFld == "Cancel Deferred Purchase") )
   {
      GetPrivateField2(strDeviceID);
      Deferred_Txn = true;
   }
   else
   {
      GetPrivateField1(strDeviceID);
      Deferred_Txn = false;
   }

   memcpy( sAuthTxRecord.TLF01_details.terminal_id,
           strDeviceID,
           sizeof(sAuthTxRecord.TLF01_details.terminal_id)-1 );

   memcpy( sAuthTxRecord.TLF01_details.terminal_type,
           "03",
           sizeof(sAuthTxRecord.TLF01_details.terminal_type)-1 );

   ptetime_get_strtime(strtime); 
   memset( sAuthTxRecord.TLF01_details.primary_key.transaction_id, 0,
           sizeof(sAuthTxRecord.TLF01_details.primary_key.transaction_id));

   memcpy( sAuthTxRecord.TLF01_details.primary_key.transaction_id,
           sAuthTxRecord.TLF01_details.terminal_id,
           sizeof(sAuthTxRecord.TLF01_details.primary_key.transaction_id) - 1);

   strncat((char*)sAuthTxRecord.TLF01_details.primary_key.transaction_id,
           strtime+2, 12);

   // Take Right 7 digits and remove "."
   strUniqueStan = strtime;
   strUniqueStan = strUniqueStan.Right(7);
   strUniqueStan = strUniqueStan.Left(3) + strUniqueStan.Right(3);

   memcpy( sAuthTxRecord.TLF01_details.sys_trace_audit_num,
           strUniqueStan,
           sizeof(sAuthTxRecord.TLF01_details.sys_trace_audit_num) - 1 );

   memcpy( sAuthTxRecord.TLF01_details.invoice_number, " ",
           sizeof(sAuthTxRecord.TLF01_details.invoice_number) );

   if ( merchant_record_exists == true )
   {
      if ( myUtilsObject.IsEmpty ( m_StrMCCFld, this ) )
      {
         if ( !m_bMCFRecordNotFound )
         {
            memcpy( sAuthTxRecord.TLF01_details.category_code,
                    sMCFRecord.voice_category_code,
                    sizeof(sAuthTxRecord.TLF01_details.category_code) );
         }
      }
      else
      {
	      memcpy( sAuthTxRecord.TLF01_details.category_code,
                 m_StrMCCFld,
                 sizeof(sAuthTxRecord.TLF01_details.category_code) );
      }

      /* Currency Code */
      index = m_StrCurrencyCodeFld.Find(' ');

      if ( index != -1 )
         strCurrencyCode = m_StrCurrencyCodeFld.Left(index);
      else if ( m_StrCurrencyCodeFld.GetLength() != 0 )
         strCurrencyCode = m_StrCurrencyCodeFld;
      else if ( m_StrCurrencyCodeFld.GetLength() == 0 )
         strCurrencyCode = sMCFRecord.currency_code;
		
      memcpy( sAuthTxRecord.TLF01_details.currency_code,
              strCurrencyCode,
              sizeof(sAuthTxRecord.TLF01_details.currency_code) );

      /* Card Holder Name (We are storing Merchant Chain Name here)*/
      memcpy( sAuthTxRecord.TLF01_details.card_holder_name,
              m_StrMerchantNameFld,
              sizeof(sAuthTxRecord.TLF01_details.card_holder_name)-1 );
   }

   memcpy( sAuthTxRecord.TLF01_details.handler_queue, "VOICE",
           sizeof(sAuthTxRecord.TLF01_details.handler_queue) );

   /* Dispensation.origin and entry type have been set for cancel
    * transactions already.  Set to LO for non-cancel transactions.
    */
   if(cancel_txn_origin != REMOTE_ORIGIN)
   {
      memcpy( sAuthTxRecord.TLF01_details.dispensation.origin, "LO",
              sizeof(sAuthTxRecord.TLF01_details.dispensation.origin) );
      sAuthTxRecord.TLF01_details.entry_type = 1;
   }
   else if ( sAuthTxRecord.TLF01_details.dispensation.origin[0] == 0x00 )
   {
      /* If origin is missing, default to local since most Voices are local. */
      memcpy( sAuthTxRecord.TLF01_details.dispensation.origin, "LO", 2 );
   }

   memcpy( sAuthTxRecord.TLF01_details.voice_auth, "Y",
           sizeof(sAuthTxRecord.TLF01_details.voice_auth) );

   memcpy( sAuthTxRecord.TLF01_details.voice_auth_text, m_StrTextFld,
           sizeof(sAuthTxRecord.TLF01_details.voice_auth_text) );

   memcpy( sAuthTxRecord.TLF01_details.operator_id, name,
           sizeof(sAuthTxRecord.TLF01_details.operator_id) );

   memcpy( sAuthTxRecord.TLF01_details.batch_number, "ACTIVE",
           sizeof(sAuthTxRecord.TLF01_details.batch_number) );


   ptetime_get_strtime ( strTime );
   memcpy( sAuthTxRecord.TLF01_details.tran_start_time, strTime,
           sizeof(sAuthTxRecord.TLF01_details.tran_start_time) );

   /* Get Workstation Name */
   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );
   memcpy( sAuthTxRecord.TLF01_details.workstation, wstation,
           sizeof( sAuthTxRecord.TLF01_details.workstation) );
 
   nLength = m_StrAccountNumberFld.GetLength();
   itoa( nLength, strLen, 10 );

   strAmount = myUtilsObject.GenerateTranAmount( m_StrAmountFld, 12,
                                                 strErrorMessage );
   if ( strAmount == m_StrAmountFld )
   {
      DisplayErrorMsg( strErrorMessage );
      m_CntlAmountFld.SetFocus();
      m_VerifyButton.EnableWindow(TRUE);
      return FALSE;
   }

   if ( m_StrTransactionTypeFld == "Sale"  )
   {
      if ( m_nDialogMode == MODE_OVERRIDE )
         strTxKey = AUTH_AUTHORIZATION_OVERRIDE;
      else
         strTxKey = AUTH_AUTHORIZATION;

      memcpy( sAuthTxRecord.TLF01_details.processing_code, "000000",
              sizeof(sAuthTxRecord.TLF01_details.processing_code) );

      memcpy( sAuthTxRecord.TLF01_details.message_type, "0100",
              sizeof(sAuthTxRecord.TLF01_details.message_type) );

      memcpy( sAuthTxRecord.TLF01_details.total_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.total_amount) );
   }
   else if (  m_StrTransactionTypeFld == "Quasi Cash" )
   {
      if ( m_nDialogMode == MODE_OVERRIDE )
         strTxKey = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
      else
         strTxKey = AUTH_CASH_ADVANCE_AUTHORIZATION;
      memcpy( sAuthTxRecord.TLF01_details.processing_code, "180000",
              sizeof(sAuthTxRecord.TLF01_details.processing_code) );

      memcpy( sAuthTxRecord.TLF01_details.message_type, "0100",
              sizeof(sAuthTxRecord.TLF01_details.message_type) );

      memcpy( sAuthTxRecord.TLF01_details.total_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.total_amount) );
   }
   else if ( m_StrTransactionTypeFld == "Cash Advance" )
   {
      if ( m_nDialogMode == MODE_OVERRIDE )
         strTxKey = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
      else
         strTxKey = AUTH_CASH_ADVANCE_AUTHORIZATION;
      memcpy( sAuthTxRecord.TLF01_details.processing_code, "010000",
              sizeof(sAuthTxRecord.TLF01_details.processing_code) );

      memcpy( sAuthTxRecord.TLF01_details.message_type, "0100",
              sizeof(sAuthTxRecord.TLF01_details.message_type) );

      memcpy( sAuthTxRecord.TLF01_details.total_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.total_amount) );
   }
   else if ( m_StrTransactionTypeFld == "Deferred Purchase" )
   {
      if ( m_nDialogMode == MODE_OVERRIDE )
         strTxKey = AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE;
      else
         strTxKey = AUTH_DEFERRED_PURCHASE_AUTHORIZATION;
      memcpy( sAuthTxRecord.TLF01_details.processing_code, "700000",
              sizeof(sAuthTxRecord.TLF01_details.processing_code) );

      memcpy( sAuthTxRecord.TLF01_details.message_type, "0100",
              sizeof(sAuthTxRecord.TLF01_details.message_type) );

      memcpy( sAuthTxRecord.TLF01_details.total_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.total_amount) );
   }

   if ( m_StrTransactionTypeFld.Left(6) == "Cancel" )
   {
      strTxKey = AUTH_REVERSAL;
      memcpy( sAuthTxRecord.TLF01_details.orig_auth_number, m_StrAuthNumberFld,
              sizeof(sAuthTxRecord.TLF01_details.orig_auth_number) );

      memcpy( sAuthTxRecord.TLF01_details.message_type, "0400",
              sizeof(sAuthTxRecord.TLF01_details.message_type) );

      memcpy( sAuthTxRecord.TLF01_details.reversal_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.reversal_amount) );

      memcpy( sAuthTxRecord.TLF01_details.total_amount, strAmount,
              sizeof(sAuthTxRecord.TLF01_details.total_amount) );

      memset( orig_tran_type,       0x00, sizeof(orig_tran_type)       );
      memset( cancel_tran_type,     0x00, sizeof(cancel_tran_type)     );
      memset( temp_processing_code, 0x00, sizeof(temp_processing_code) );
      memset( orig_processing_code, 0x00, sizeof(orig_processing_code) );
      if ( m_StrTransactionTypeFld == "Cancel Sale" )
      {
         memcpy( temp_processing_code, "00", 2 );
         memcpy( cancel_tran_type, "Cancel Sale", 11 );

         if ( 0 == memcmp("38",sAuthTxRecord.TLF01_details.processing_code,2))
         {
            /* Allow Card Verifies to be Cancelled as a Sale. */
            memcpy( sAuthTxRecord.TLF01_details.processing_code, "00", 2 );
         }
      }
      else if ( m_StrTransactionTypeFld == "Cancel Cash Advance" )
      {
         memcpy( temp_processing_code, "01", 2 );
         memcpy( cancel_tran_type, "Cancel Cash Advance", 19 );
      }
      else if ( m_StrTransactionTypeFld == "Cancel Deferred Purchase" )
      {
         memcpy( temp_processing_code, "70", 2 );
         memcpy( cancel_tran_type, "Cancel Deferred Purchase", 24 );
      }
      else if ( m_StrTransactionTypeFld == "Cancel Quasi Cash" )
      {
         memcpy( temp_processing_code, "18", 2 );
         memcpy( cancel_tran_type, "Cancel Quasi Cash", 17 );
      }

      memcpy( orig_processing_code, sAuthTxRecord.TLF01_details.processing_code, 2);
      if ( 0 != strcmp(temp_processing_code,orig_processing_code) )
      {
         /* Operator is canceling the wrong transaction type.   */
         /* Determine transaction type of original transaction. */
         if ( 0 == strcmp(orig_processing_code, "00") )
            memcpy( orig_tran_type, "Sale", 4 );

         else if ( 0 == strcmp(orig_processing_code, "01") )
            memcpy( orig_tran_type, "Cash Advance", 12 );

         else if ( 0 == strcmp(orig_processing_code, "70") )
            memcpy( orig_tran_type, "Deferred Purchase", 17 );

         else if ( 0 == strcmp(orig_processing_code, "18") )
            memcpy( orig_tran_type, "Quasi Cash", 10 );

         else
            sprintf( orig_tran_type,
                    "Unknown: proc-code=%snnnn",
                     orig_processing_code );

         sprintf( strErrorMessage,
                 "Tran type (%s) does not match original tran type (%s)",
                  cancel_tran_type, orig_tran_type );

         DisplayErrorMsg( strErrorMessage );
         m_CntlTransactionTypeFld.SetFocus();
         m_VerifyButton.EnableWindow(TRUE);
         return FALSE;
      }
   }
   else
   {
      memcpy( sAuthTxRecord.TLF01_details.auth_number, m_StrAuthNumberFld,
              sizeof(sAuthTxRecord.TLF01_details.auth_number) );
   }


   // Populate TLF01 Table	
   sAuthTxRecord.TLF01_details.tx_key = strTxKey;
   memcpy( sAuthTxRecord.industry_code, m_StrIndustryCodeFld,
           sizeof(sAuthTxRecord.industry_code) );

   memcpy( sAuthTxRecord.TLF01_details.merchant_id, sMCFRecord.primary_key.merchant_id,
           sizeof(sAuthTxRecord.TLF01_details.merchant_id) );

   memcpy( sAuthTxRecord.TLF01_details.card_num, m_StrAccountNumberFld,
           sizeof(sAuthTxRecord.TLF01_details.card_num) );

   memcpy( sAuthTxRecord.TLF01_details.exp_date, m_StrExpiryDateFld,
           sizeof(sAuthTxRecord.TLF01_details.exp_date) );

   memcpy( sAuthTxRecord.TLF01_details.deferred_term_length,
           m_StrDeferredTermFld,
           sizeof (sAuthTxRecord.TLF01_details.deferred_term_length) );

   memcpy( sAuthTxRecord.TLF01_details.response_code, m_StrResponseCodeFld,
           sizeof(sAuthTxRecord.TLF01_details.response_code) );

   memcpy( sAuthTxRecord.TLF01_details.card_num_len, strLen,
           sizeof(sAuthTxRecord.TLF01_details.card_num_len) );

   if ( 4 == strlen(m_StrCVV2Fld) )
   {
      /* Amex DBC/CSC value is 4 digits long. Doesn't fit in cvc_data. */
      memcpy( sAuthTxRecord.TLF01_details.interest_rate, m_StrCVV2Fld, 4 );
   }
   else if ( 0 < strlen(m_StrCVV2Fld) )
   {
      memcpy( sAuthTxRecord.TLF01_details.cvc_data, m_StrCVV2Fld,
              sizeof(sAuthTxRecord.TLF01_details.cvc_data) );
   }

   memcpy( sAuthTxRecord.TLF01_details.pos_entry_mode, "0010",
           sizeof(sAuthTxRecord.TLF01_details.pos_entry_mode) ); /* Changed from 0000 to 0010 for manual entry - TF PHANI*/

   memcpy( sAuthTxRecord.TLF01_details.organization_id, m_StrOrganizationIDFld,
           sizeof(sAuthTxRecord.TLF01_details.organization_id) );

   memcpy( sAuthTxRecord.TLF01_details.pos_condition_code, "00",
           sizeof(sAuthTxRecord.TLF01_details.pos_condition_code) );

   if ( !myUtilsObject.IsEmpty ( m_StrCVV2Fld, this ) )
   {
      memcpy( sAuthTxRecord.TLF01_details.verify_mag_stripe, "Y",
              sizeof(sAuthTxRecord.TLF01_details.verify_mag_stripe) );
   }
   else
   {
      memcpy( sAuthTxRecord.TLF01_details.verify_mag_stripe, "N",
              sizeof(sAuthTxRecord.TLF01_details.verify_mag_stripe) );
   }

   if ( m_nDialogMode == MODE_OVERRIDE )
   {
   		/* ATP should not be asking for the authorization number
		 * if it is going to be routed to the network. 
		 * It doesn't matter whether it is "cancel" transaction or not.
		 * For reversals, it has to get verified before it can be authorized or overrided;
		 * so it should have Auth Number input already (to be verified). 
		 */
		m_CntlAuthNumberFld.EnableWindow(( m_disposition == ONUS )? TRUE : FALSE);
		m_CntlOrganizationIDFld.EnableWindow(FALSE);
		m_CntlTransactionTypeFld.EnableWindow(FALSE);
		m_CntlCVV2Fld.EnableWindow(FALSE);
		m_CntlDeferredTermFld.EnableWindow(FALSE);
		m_CntlMerchantIDFld.EnableWindow(FALSE);
		m_CntlExpiryDateFld.EnableWindow(FALSE);
		m_CntlAmountFld.EnableWindow(FALSE);
		m_CntlAccountNumberFld.EnableWindow(FALSE);
   }
   else
   {
      m_nDialogMode = MODE_DONE;
      m_CntlAuthNumberFld.EnableWindow (FALSE); 
      m_CntlDeferredTermFld.EnableWindow(FALSE);
      m_CntlTransactionTypeFld.EnableWindow(TRUE);
      m_CntlCVV2Fld.EnableWindow(TRUE);
      m_CntlMerchantIDFld.EnableWindow(TRUE);
      m_CntlExpiryDateFld.EnableWindow(TRUE);
      m_CntlAmountFld.EnableWindow(TRUE);
      m_CntlAccountNumberFld.EnableWindow(TRUE);
      m_CntlOrganizationIDFld.EnableWindow(TRUE);
      UpdateData(FALSE);
   }

   return TRUE;
}

void CVoiceAuth::OnProcessAuthButton() 
{
   CHAR strErrorMessage[300] = "";
   CHAR strBuffer[350] = "";
   CHAR mid[16] = "";
   CHAR oid[5]  = "";
   INT  rcode;
   CString rrn, orig_rrn;
   CMyUtils myUtilsObject;

   m_nDialogMode = MODE_TRANSACTION;

   UpdateData(TRUE);
   m_StrAuthNumOutputFld = _T("");
   m_StrBankNameFld = _T("");
   m_StrGrossAmountFld = _T("");
   m_StrMessageLbl = _T("");
   m_StrMonthlyAmountFld = _T("");
   m_StrResponseCodeFld = _T("");
   m_StrResponseTextFld = _T(""); 

   m_VerifyButton.EnableWindow(FALSE);
   m_ProcessButton.EnableWindow(FALSE);

   UpdateData(FALSE);

   if ( !PopulateTLFStructure() )
 	   return;

   if ( SendVoiceAuthTransaction() )
   {
      if ( strlen ((char *)sAuthTxRecord.TLF01_details.date_yyyymmdd) < 8 )
      {
         memcpy( sAuthTxRecord.TLF01_details.date_yyyymmdd,
                 GetSystemDate(),
                 sizeof(sAuthTxRecord.TLF01_details.date_yyyymmdd)-1 );
      }

      if ( strlen ((char *)sAuthTxRecord.TLF01_details.time_hhmmss) < 6 )
      {
         myUtilsObject.GetSystemTime( (pCHAR)(sAuthTxRecord.TLF01_details.time_hhmmss) );
      }

      rrn      = sAuthTxRecord.TLF01_details.retrieval_ref_num;
      orig_rrn = sAuthTxRecord.TLF01_details.orig_retrieval_ref_num;

      if ( rrn.GetLength() == 0 )
         if(orig_rrn.GetLength() > 0)
            memcpy( sAuthTxRecord.TLF01_details.retrieval_ref_num,
                    sAuthTxRecord.TLF01_details.orig_retrieval_ref_num,
                    sizeof(sAuthTxRecord.TLF01_details.orig_retrieval_ref_num)-1);
         else
            memcpy( sAuthTxRecord.TLF01_details.retrieval_ref_num, "            ", 12 );

#ifdef MC13_OCT2023  
	  if(strlen(mc_mac_code) > 0 )
	  {
		memcpy(sAuthTxRecord.TLF01_details.voice_auth_text, sAuthTxRecord.TLF01_details.response_text, sizeof(sAuthTxRecord.TLF01_details.voice_auth_text)-1);
		memset(sAuthTxRecord.TLF01_details.response_text, 0, sizeof(sAuthTxRecord.TLF01_details.response_text));
		memcpy(sAuthTxRecord.TLF01_details.response_text, mc_mac_code, strlen(mc_mac_code));
		memset(mc_mac_code, 0, sizeof(mc_mac_code));
	  }
#endif

      rcode = txdsapi_send_all_data ( &sAuthTxRecord,  strErrorMessage );
      if( PTEMSG_OK != rcode )
      {
         /* Transaction failed on the update, so insert
          * into TLF01 so we have a record of the failure.
          * (Sept 14, 04 LM SCR1446 fix) Also update response dialog fields
          * to relect database failure.*/
         if ( (rcode == 2) || (rcode == 5) )
         {
            checkTLF01_fields( strErrorMessage );
         }
         sprintf( strBuffer,
                 "DB Update Failure - %s",
                  strErrorMessage );
         DisplayErrorMsg( strBuffer );
         m_StrResponseCodeFld = "96";
         m_StrAuthNumOutputFld = _T("");
         m_StrGrossAmountFld = _T("");
         m_StrMonthlyAmountFld = _T("");

         memcpy( sAuthTxRecord.TLF01_details.response_code, "96", 2 );
         memcpy( sAuthTxRecord.TLF01_details.response_text, "Database update failure", 23 );
         memset( sAuthTxRecord.TLF01_details.auth_number, 0x00, sizeof(sAuthTxRecord.TLF01_details.auth_number));
         rcode = txdsapi_insert_record( (pBYTE)&sAuthTxRecord,
                                         sizeof(TLF01),
                                         TLF01_DATA,
                                         strErrorMessage );
      }
      else
      {
			/*don't lose the original label message of SendVoiceAuthTransaction - 'can't override'*/
			m_StrMessageLbl += "Transaction Completed";
      }

      UpdateData(FALSE);
   }
   else
   {  
	   m_VerifyButton.EnableWindow(TRUE);
   }// For debug 
   return;
}

BOOL CVoiceAuth::CheckBCH20() 
{
   char strErrorMessage[300] = "";
   CMyUtils myUtilsObject;
   int  bch20_term;
   int  gui_term;

   ZeroMemory ( &sBCH20Record, sizeof ( BCH20) );

   memcpy ( sBCH20Record.card_nbr,    m_StrAccountNumberFld, sizeof (sBCH20Record.card_nbr));
   memcpy ( sBCH20Record.merchant_id, m_StrMerchantIDFld, sizeof (sBCH20Record.merchant_id));

   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrAmountFld, 12, strErrorMessage );
   memcpy ( sBCH20Record.tran_amount, strAmount, sizeof (sBCH20Record.tran_amount));

   memcpy ( sBCH20Record.auth_number, m_StrAuthNumberFld, sizeof (sBCH20Record.auth_number));

   int rcode = txdsapi_get_record_by_subtype2( (pBYTE)&sBCH20Record, sizeof (BCH20), BCH20_DATA, ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER, strErrorMessage );
   if( PTEMSG_OK == rcode )
   {
      /* Verify term length */
      if ( m_StrTransactionTypeFld == "Cancel Deferred Purchase" )
      {
         gui_term   = atoi(m_StrDeferredTermFld);
         bch20_term = atoi((char *)sBCH20Record.term);
         if ( gui_term == bch20_term )
            return TRUE;
         else
         {
            DisplayErrorMsg( "Transaction not found" );
            m_StrResponseCodeFld = sBCH20Record.response_code;
            UpdateData(FALSE);
            return FALSE;
         }
      }
      else
         return TRUE;
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      DisplayErrorMsg( "Please try again" );
      return FALSE;
   }
   else if( PTEMSG_NOT_FOUND == rcode )
   {
      DisplayErrorMsg( "Transaction not found in BCH20. Searching for network record..." );
      m_StrResponseCodeFld = sBCH20Record.response_code;
      UpdateData(FALSE);
      return FALSE;
   }
   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      m_StrResponseCodeFld = sBCH20Record.response_code;
      UpdateData(FALSE);
      return FALSE;
   }
   return FALSE;
}

BOOL CVoiceAuth::CheckNCF30() 
{
   BOOL     ret_val;
   CHAR     strErrorMessage[300] = "";
   CMyUtils myUtilsObject;

   ZeroMemory( &sNCF30Record, sizeof(NCF30) );

   memcpy( sNCF30Record.card_nbr, m_StrAccountNumberFld, sizeof(sNCF30Record.card_nbr));
   memcpy( sNCF30Record.auth_number, m_StrAuthNumberFld, sizeof(sNCF30Record.auth_number) );

   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrAmountFld, 12, strErrorMessage );
   memcpy( sNCF30Record.tran_amount, strAmount, sizeof(sNCF30Record.tran_amount) );

   int rcode = txdsapi_get_record_by_subtype2( (pBYTE)&sNCF30Record, sizeof(NCF30), NCF30_DATA, ST2_DB_SELECT_NCF30_FOR_VOICE, strErrorMessage );
   if( PTEMSG_OK == rcode )
   {
      DisplayErrorMsg( "Verifying NCF30  network transaction..." );
      ret_val = TRUE;
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      DisplayErrorMsg( "Please try again" );
      ret_val = FALSE;
   }
   else if( PTEMSG_NOT_FOUND == rcode )
   {
      DisplayErrorMsg( "Original transaction not found in BCH20 nor in NCF30" );
      m_StrResponseCodeFld = sNCF30Record.response_code;
      UpdateData(FALSE);
      ret_val = FALSE;
   }
   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      m_StrResponseCodeFld = sNCF30Record.response_code;
      UpdateData(FALSE);
      ret_val = FALSE;
   }
   return( ret_val );
}


void CVoiceAuth::OnClearButton() 
{
   m_nDialogMode = MODE_DONE;

   m_StrAccountNumberFld = _T("");
   m_StrAmountFld = _T("");
   m_StrAuthNumOutputFld = _T("");
   m_StrAuthNumberFld = _T("");
   m_StrBankNameFld = _T("");
   m_StrCardholderNameFld = _T("");
   m_StrCVV2Fld = _T("");
   m_StrDeferredTermFld = _T("");
   m_StrExpiryDateFld = _T("");
   m_StrGrossAmountFld = _T("");
   m_StrIndustryCodeFld = _T("");
   m_StrMCCFld = _T("");
   m_StrMerchantIDFld = _T("");
   m_StrMerchantNameFld = _T("");
   m_StrMessageLbl = _T("");
   m_StrMonthlyAmountFld = _T("");
   m_StrResponseCodeFld = _T("");
   m_StrResponseTextFld = _T("");
   m_StrTextFld = _T("");
   m_StrTransactionTypeFld = _T("");
   m_StrCurrencyCodeFld = _T("");
   m_StrOrganizationIDFld = _T("");
   m_StrCardStatusFld = _T("");

   m_ProcessButton.EnableWindow(FALSE);
   m_VerifyButton.EnableWindow(TRUE);
   m_OverrideButton.EnableWindow(FALSE);
   m_CntlMod10CheckButton.EnableWindow(TRUE);
   m_BoolMod10Check = TRUE;

   m_CntlTransactionTypeFld.EnableWindow(TRUE);
   m_CntlCVV2Fld.EnableWindow(TRUE);
   m_CntlAuthNumberFld.EnableWindow(FALSE);
   m_CntlDeferredTermFld.EnableWindow(FALSE);
   m_CntlMerchantIDFld.EnableWindow(TRUE);
   m_CntlExpiryDateFld.EnableWindow(TRUE);
   m_CntlAmountFld.EnableWindow(TRUE);
   m_CntlAccountNumberFld.EnableWindow(TRUE);
   m_CntlMerchantIDFld.SetFocus();
   m_CntlIndustryCodeFld.EnableWindow(TRUE);
   m_CntlOrganizationIDFld.EnableWindow(TRUE);
   m_CntlMCCFld.EnableWindow(TRUE);
   m_CntlCurrencyCodeFld.EnableWindow(TRUE);
   m_CntlTransactionTypeFld.SetCurSel(0);
   m_CntlTextFld.EnableWindow(TRUE);
   PasswordCheckDone = FALSE;

   UpdateData(FALSE);
}

void CVoiceAuth::OnOK() 
{
   CWnd* pwndCtrl = GetFocus();
   CWnd* pwndCtrlNext = pwndCtrl;
   int ctrl_ID = pwndCtrl->GetDlgCtrlID();
	UpdateData(TRUE);

   switch (ctrl_ID) {
      case IDC_MERCHANT_ID_FLD:
          pwndCtrlNext = GetDlgItem(IDC_ORG_ID_FLD);
			 pwndCtrlNext->SetFocus();
          break;
      case IDC_ORG_ID_FLD:
          pwndCtrlNext = GetDlgItem(IDC_ACCOUNT_NUMBER_FLD);
			 pwndCtrlNext->SetFocus();
          break;
      case IDC_ACCOUNT_NUMBER_FLD:
          pwndCtrlNext = GetDlgItem(IDC_EXPIRY_DATE_FLD);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_EXPIRY_DATE_FLD:
          pwndCtrlNext = GetDlgItem(IDC_AMOUNT);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_AMOUNT:
          pwndCtrlNext = GetDlgItem(IDC_CVV2_FLD);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_CVV2_FLD:
          pwndCtrlNext = GetDlgItem(IDC_MCC_FLD);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_MCC_FLD:
          pwndCtrlNext = GetDlgItem(IDC_TRANSACTION_TYPE);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_TRANSACTION_TYPE:
          pwndCtrlNext = GetDlgItem(IDC_INDUSTRY_CODE);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_INDUSTRY_CODE:
          pwndCtrlNext = GetDlgItem(IDC_CURRENCY_CODE);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_CURRENCY_CODE:
				pwndCtrlNext = GetDlgItem(IDC_TEXT_FLD);
          pwndCtrlNext->SetFocus();
          break;
      case IDC_DEFERRED_PURCHASE_TERM_FLD:
          if ( m_nDialogMode == MODE_DONE )
			   pwndCtrlNext = GetDlgItem(IDC_TEXT_FLD);
            pwndCtrlNext->SetFocus();
          break;
      case IDC_AUTH_NUMBER_FLD:
         if ( m_StrTransactionTypeFld.Right(17) == "Deferred Purchase" )
				   pwndCtrlNext = GetDlgItem(IDC_DEFERRED_PURCHASE_TERM_FLD);
         else
            pwndCtrlNext = GetDlgItem(IDC_PROCESS_AUTH_BUTTON);
            pwndCtrlNext->SetFocus();
          break;
      case IDC_TEXT_FLD:
			   pwndCtrlNext = GetDlgItem(IDC_AUTH_NUMBER_FLD);
            pwndCtrlNext->SetFocus();
          break;
      case IDC_PROCESS_AUTH_BUTTON:
			   OnProcessAuthButton();
           break;      
		case IDC_OK1:
             CDialog::OnOK();
           break;
         default:
             break;
     }		
}

void CVoiceAuth::OnSelchangeTransactionType() 
{
	UpdateData(TRUE);

	if ( m_StrTransactionTypeFld.Right(17) == "Deferred Purchase" )
		m_CntlDeferredTermFld.EnableWindow(TRUE);
	else
	{
		m_CntlDeferredTermFld.EnableWindow(FALSE);
		m_StrDeferredTermFld = "";
		UpdateData(FALSE);
	}

	if ( m_StrTransactionTypeFld.Left(6) == "Cancel" )
	{
		/* We need the auth number input to find the original transaction */
		m_CntlAuthNumberFld.EnableWindow(TRUE);
	}
	else
	{
		m_CntlAuthNumberFld.EnableWindow(FALSE);
		m_StrResponseCodeFld = "";
		m_StrAuthNumberFld = "";
		UpdateData(FALSE);
	}	

   // if cash advance or quasi cash transaction is selected - select corresponding currency code
 /*  if ( m_StrTransactionTypeFld.Right(12) == "Cash Advance" )
		m_CntlIndustryCode.SelectString( 0, "900");
   else  if ( m_StrTransactionTypeFld.Right(10) == "Quasi Cash" )
		m_CntlIndustryCode.SelectString( 0, "902");
   else m_CntlIndustryCode.SelectString( 0, "100");*/


}


BOOL CVoiceAuth::ValidateDate ( CString strDate )
{
	CString  strMonth;
	CString  strYear;
	char     strTemp[256] = {0} ;
	INT      nSystemYY = 0 ;			// 00 to 99
	INT      nSystemMM = 0 ;			// 00 to 12
	CMyUtils myUtilsObject;

	int nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	

	if ( length < 4 )
	{
      DisplayErrorMsg( "Invalid Date Format" );
		return FALSE;
	}
	
	strYear = strDate.Left(2);
	strMonth = strDate.Right(2);

	nMonth = atoi ( strMonth );  /* month entered mm*/
	nYear = atoi ( strYear );	 /* year entered yy */


	if ( nMonth > 12 || nMonth == 0)
	{
      DisplayErrorMsg( "ERROR:Invalid Month!" );
		return FALSE;
	}

	/*check if date expired*/
	systemDate_mmddyyyy;


    /* Get the year portion of the current systemDate (mmddyyyy) */
    /* If the current year is 1999, nSystemYY will be 99 and if 2000, nSystemYY will be 0 */
	memcpy (strTemp, systemDate_mmddyyyy+4, 4) ;
	nSystemYY = atoi(strTemp) % 100;

    /* Get the month portion of the current systemDate (mmddyyyy) */
	memset (strTemp, 0, sizeof strTemp) ;
	memcpy (strTemp, systemDate_mmddyyyy, 2) ;
	nSystemMM = (INT)atoi(strTemp) ;

    /* System Date between 2000 and 2020 */
	/*if (nSystemYY >= 0 && nSystemYY <= 20)
		nSystemYY += 100 ;

   if (nSystemYY >= 80 && nSystemYY <= 120 && nYear >= 0 && nYear <= 40)
		nYear += 100 ;*/

   if (nYear < nSystemYY)
   {
      DisplayErrorMsg( "ERROR: Expired Card!" );
      return FALSE; 
   }
   else if (nYear == nSystemYY && (nMonth < nSystemMM))
   {
      DisplayErrorMsg( "ERROR: Expired Card!" );
      return FALSE; 
   }

   return TRUE;
}

BOOL CVoiceAuth::SendVoiceAuthTransaction()
{
   char strErrorMessage[300];
   char strTime[15];
   INT  rcode;
   INT  len;
   INT  txn_approved     = true;
   INT  override_enabled = false;
   BOOL ret_val;

   rcode = txdsapi_send_voice_auth_tran( &sAuthTxRecord,
                                         AUTH_TX_DATA,
                                         strErrorMessage);

#ifdef MC13_OCT2023
    memset(mc_mac_code, 0, sizeof(mc_mac_code));
   	if( (strlen((pCHAR)(sAuthTxRecord.TLF01_details.response_text)) == 12) && 
		(strstr((pCHAR)(sAuthTxRecord.TLF01_details.response_text), "DE48 SE84:") != NULL))
	{
		memcpy(mc_mac_code, sAuthTxRecord.TLF01_details.response_text, strlen((char *)(sAuthTxRecord.TLF01_details.response_text)));
	}
#endif

   /* Salvage original Voice Auth Text.
    * Txcntl overwrote it with response text.
    */
   if ( 0 != strcmp((pCHAR)(sAuthTxRecord.TLF01_details.voice_auth_text), m_StrTextFld) )
   {
      memcpy( sAuthTxRecord.TLF01_details.response_text,
              sAuthTxRecord.TLF01_details.voice_auth_text,
              sizeof(sAuthTxRecord.TLF01_details.response_text)-1 );
   }

   /* Check for line feed at end of string; not sure why it is there. */
   len = strlen( (pCHAR)(sAuthTxRecord.TLF01_details.response_text) );
   if ( sAuthTxRecord.TLF01_details.response_text[len-1] == 0x0a )
   {
      sAuthTxRecord.TLF01_details.response_text[len-1] = 0x00;
   }

   memcpy( sAuthTxRecord.TLF01_details.voice_auth_text,
           m_StrTextFld,
           sizeof(sAuthTxRecord.TLF01_details.voice_auth_text) );

   /* Check result of transaction. */
   if ( PTEMSG_OK == rcode )
   {
      // if transaction is not approved, delete auth_number
      if ( strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "00" ) != 0 )
      {
         txn_approved = false;
         memset (sAuthTxRecord.TLF01_details.auth_number, 0,
                 sizeof(sAuthTxRecord.TLF01_details.auth_number) );

         /*051801 LM - Disable commit button on decline*/
         m_ProcessButton.EnableWindow(FALSE);
         m_VerifyButton.EnableWindow(TRUE); 
         override_enabled = true;
      }
      else
      {
         m_CntlAccountNumberFld.EnableWindow(FALSE);
         m_CntlAmountFld.EnableWindow(FALSE);
         m_CntlAuthNumberFld.EnableWindow(FALSE);
         m_CntlDeferredTermFld.EnableWindow(FALSE);
         m_CntlExpiryDateFld.EnableWindow(FALSE);
         m_CntlMerchantIDFld.EnableWindow(FALSE);
         m_CntlTransactionTypeFld.EnableWindow(FALSE);
         m_CntlCVV2Fld.EnableWindow(FALSE);
         m_CntlOrganizationIDFld.EnableWindow(FALSE);
         m_CntlMCCFld.EnableWindow(FALSE);
         m_CntlCurrencyCodeFld.EnableWindow(FALSE);
         m_CntlIndustryCodeFld.EnableWindow(FALSE);

         m_VerifyButton.EnableWindow(FALSE);
         m_ProcessButton.EnableWindow(FALSE);
         m_OverrideButton.EnableWindow(FALSE);
         m_CntlMod10CheckButton.EnableWindow(FALSE);
         m_CntlTextFld.EnableWindow(FALSE);
         m_ClearButton.SetButtonStyle(BS_DEFPUSHBUTTON ,1);
         m_ClearButton.SetFocus();
         override_enabled = false;
      }
      ptetime_get_strtime ( strTime );
      memcpy ( sAuthTxRecord.TLF01_details.tran_finish_time,  strTime, sizeof (sAuthTxRecord.TLF01_details.tran_finish_time));

      /* SCR 16287 -  ThoughtFocus
	   * Added conditions to keep the override button disabled if the response_code 
	   * is that of a Stolen Card (43) or a Lost Card (41).
	   */
	  
	  if (sAuthTxRecord.TLF01_details.general_status != GS_TXN_CCF02P_NOT_FOUND &&
          sAuthTxRecord.TLF01_details.general_status != GS_TXN_BIN_NOT_FOUND &&
          sAuthTxRecord.TLF01_details.general_status != GS_TXN_PAF_NOT_FOUND &&
          sAuthTxRecord.TLF01_details.general_status != GS_TXN_ACF01_NOT_FOUND &&
          sAuthTxRecord.TLF01_details.general_status != GS_TXN_INVALID_CURRENCY_CODE &&
          m_nDialogMode != MODE_OVERRIDE &&
          (txn_approved == false) &&
          (strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "91" ) != 0) &&
          (strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "96" ) != 0) &&
		  (strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "43" ) != 0) &&
		  (strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "41" ) != 0)
         )
      {
      		if (strcmp((const char *)sAuthTxRecord.TLF01_details.host_finish_time, "") == 0)
      		{
				/*This is the only place that the override button will be enabled*/
					m_OverrideButton.EnableWindow(TRUE);
					override_enabled = true;
			}
			else
			{
				/*if we have a response code coming from the network, don't enable the override button*/
				m_OverrideButton.EnableWindow(FALSE);
				override_enabled = false;
				m_StrMessageLbl = "Can't override the transaction as the response code is from network. ";
			}
			m_CntlAuthNumberFld.EnableWindow(( m_disposition == ONUS )? TRUE : FALSE);
      }
      else
      {
         m_OverrideButton.EnableWindow(FALSE);
         override_enabled = false;
      }

      /* Disable the Override button when a transaction is rejected
       * and it is either rejected due to merchant not accepting the
       * card type or rejected due to the cardholder status = 06.
       * Since txcntl may not return a CAF01 record, use the Temp
       * one already populated by Voice.
       */
      if ( override_enabled == true )
      {
         if ( (txn_approved == false ) &&
              ((sAuthTxRecord.TLF01_details.general_status == GS_TXN_MERCHANT_BIN_NOT_FOUND) ||
               (strcmp((char *)sAuthTxRecordTemp.CCF02P_details.status, "06") == 0) ) )
         {
            m_OverrideButton.EnableWindow(FALSE);
         }
         else
            m_OverrideButton.SetFocus();
      }

	  /* SCR 16287 -  ThoughtFocus
	   * Disable the Override button if the card is found in the NEG01 table (a HOT card).
	   * The txcntl sets up the general_status and response_code for HOT cards.
	   */

	  if ( override_enabled == true )
      {
		 if ( ( (sAuthTxRecord.TLF01_details.general_status == GS_TXN_TRANSACTION_NOT_ALLOWED) &&
                ((strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "05" ) == 0)  || 
				 (strcmp ( ( char * )sAuthTxRecord.TLF01_details.response_code , "41" ) == 0))
			  )
			)  
			{
				m_OverrideButton.EnableWindow(FALSE);
			}
         else
            m_OverrideButton.SetFocus();
      }


      PopulateDialog();
      ret_val = true;
   }
   else
   {
      if ( PTEMSG_INVALID_DATATYPE == rcode )
      {
         DisplayErrorMsg( "Please try again" );
      }
      else 
      {
         if ( 0 == strcmp(NULL_STR, strErrorMessage) )
            DisplayErrorMsg( "Please try again" );
         else
            DisplayErrorMsg( strErrorMessage );
      }
      ret_val = false;
   }
   return( ret_val );
}



void CVoiceAuth::PopulateDialog()
{
	char temp[200] = {0};
	char MaskCvv[5] = {0};
	int len = 0;
	double pGrossAmount, pMonthlyAmount;
	CMyUtils myUtilsObject;


   if ( merchant_record_exists == true )
   {
      if ( myUtilsObject.IsEmpty (m_StrMerchantNameFld, this ) )
         m_StrMerchantNameFld = sMCFRecord.name02;

      if ( myUtilsObject.IsEmpty (m_StrCardholderNameFld, this ) )
         m_StrCardholderNameFld = sCCFRecord.name_dba;
   }

   /* Here we deal with deferred txns and RSB cards. */
   if ( 0 == strcmp((char *)sAuthTxRecord.TLF01_details.response_code, "00" ) )
   {
      if ( sAuthTxRecord.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE  ||
           sAuthTxRecord.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE ||
           sAuthTxRecord.BIN01_details.credit_subtype[0] == 'G' )
      {
         /* This is either a deferred transaction or an RSB card.
          * Need to display Gross Amount and Monthly Amort Amount.
          */
         pGrossAmount = strtod((char *)sAuthTxRecord.TLF01_details.def_gross_amt,0)/100.0;
         sprintf ( temp, "%.2f", pGrossAmount );
         m_StrGrossAmountFld = temp;
         if ( m_StrGrossAmountFld.GetLength() != 0 )
         {
            if ( m_StrGrossAmountFld.Right(1) == "." )
               m_StrGrossAmountFld = m_StrGrossAmountFld + "00";
         }

         pMonthlyAmount = strtod((char *)sAuthTxRecord.TLF01_details.monthly_amort_amt,0)/100.0;
         sprintf ( temp, "%.2f", pMonthlyAmount );
         m_StrMonthlyAmountFld = temp;
         if ( m_StrMonthlyAmountFld.GetLength() != 0 )
         {
            if ( m_StrMonthlyAmountFld.Right(1) == "." )
               m_StrMonthlyAmountFld = m_StrMonthlyAmountFld + "00";
         }
      }
   }

	m_StrResponseCodeFld = sAuthTxRecord.TLF01_details.response_code;
	m_StrAuthNumOutputFld = sAuthTxRecord.TLF01_details.auth_number;
	m_StrAuthNumberFld= sAuthTxRecord.TLF01_details.auth_number;
	m_StrBankNameFld = sAuthTxRecord.BIN01_details.description;
	
	CString strTemp;
	strTemp = sAuthTxRecord.TLF01_details.response_text;

    if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"00") == 0 &&
		 myUtilsObject.IsEmpty ( strTemp, this))
	{	m_StrResponseTextFld = "Transaction Approved";
	         CString Org_AuthNum = sAuthTxRecord.TLF01_details.orig_auth_number;
		 if ( m_StrAuthNumOutputFld.GetLength() == 0 )
	         if(Org_AuthNum.GetLength() > 0)
	         	{
	         	memcpy(sAuthTxRecord.TLF01_details.auth_number,sAuthTxRecord.TLF01_details.orig_auth_number,sizeof(sAuthTxRecord.TLF01_details.auth_number));
				m_StrAuthNumOutputFld = sAuthTxRecord.TLF01_details.orig_auth_number;
	         	}

	}
	else
		m_StrResponseTextFld = sAuthTxRecord.TLF01_details.response_text;

	len = m_StrCVV2Fld.GetLength();
	memset(MaskCvv,'*',len);
	m_StrCVV2Fld = MaskCvv;

	UpdateData(FALSE);
}

BYTE CVoiceAuth::CalculateGrossAmount ( double* pGrossAmount )
{
	double nDeferredFactor = 0.0;
	double nTemp = 0.0;
	int nDeferredMonths = 0;

	nDeferredFactor = strtod ((char *)sAuthTxRecord.TLF01_details.deferred_factor, 0)/1000000.0;
	nDeferredMonths = atoi ((char *)sAuthTxRecord.TLF01_details.deferred_term_length );

	nTemp = strtod ( (char *)sAuthTxRecord.TLF01_details.total_amount, 0)/100.0;

	*pGrossAmount = nTemp * nDeferredFactor * nDeferredMonths;
	return TRUE;
}

BYTE CVoiceAuth::CalculateMonthlyAmount ( double* pMonthlyAmount )
{
	double nGrossAmount = 0.0;
	int nDeferredMonths = 0;

	CalculateGrossAmount ( &nGrossAmount );

	nDeferredMonths = atoi ( (char *)sAuthTxRecord.TLF01_details.deferred_term_length);
	if ( nDeferredMonths == 0)
		nDeferredMonths = 1;
	*pMonthlyAmount = nGrossAmount / ((double)nDeferredMonths);

	return TRUE;
}

CString CVoiceAuth::GetSystemDate ()
{
   CHAR SystemDate[40] = "";
   CString date;

   CTime t = CTime::GetCurrentTime();
   int day,year,month;
   day = t.GetDay();
   year = t.GetYear();
   month = t.GetMonth();

   itoa ( year, SystemDate, 10 );

   SystemDate[4] = '0';
   if ( month < 10 )
      itoa ( month, SystemDate+5, 10 );
   else
      itoa ( month, SystemDate+4, 10 );

   SystemDate[6] = '0';
   if ( day < 10 )
      itoa ( day, SystemDate+7, 10 );
   else
      itoa ( day, SystemDate+6, 10 );

   date = SystemDate;

   return date;


}


VOID CVoiceAuth::OnVerifyButton() 
{
   #define  AMEX_CARD   0
   #define  VISA_CARD   1
   #define  JCB_CARD    2
   #define  MC_CARD     3
   #define  CUP_CARD    4
   #define  DINERS_CARD 6
   #define  TBD_CARD   99

   #define  CVV2_GOOD      1
   #define  CVV2_DECLINED  2

   CHAR     strErrorMessage[300] = "";
   CHAR     destination[11] = "";
   CMyUtils myUtilsObject;
   INT      cancel_flag   = false;
   INT      cvv2_required = false;
   INT      cvv2_len = 0;
   INT      cvv2_status;
   INT      amex = false;
   INT      card_type = TBD_CARD; /* To be determined */
   INT      visa_electron = false;

   m_nDialogMode = MODE_TRANSACTION;

   UpdateData(TRUE);
  
   m_StrAuthNumOutputFld = _T("");
   m_StrBankNameFld = _T("");
   m_StrGrossAmountFld = _T("");
   m_StrMessageLbl = _T("");
   m_StrMonthlyAmountFld = _T("");
   m_StrResponseCodeFld = _T("");
   m_StrResponseTextFld = _T("");
   UpdateData(FALSE);

   merchant_record_exists = true;
	ZeroMemory( &sAuthTxRecord,     sizeof(AUTH_TX) );
	ZeroMemory( &sAuthTxRecordTemp, sizeof(AUTH_TX) );

   /* Initialize flags for cancel transactions. */
   if ( m_StrTransactionTypeFld.Left(6) == "Cancel"  )
   {
      /* Reversal transaction */
      cancel_flag = true;
   }

   /* Default this flag to local origin; all Voice non-cancel transactions
    * are local origin. This flag is used in all transactions to determine
    * transaction origination.  So this flag must be initialized for every
    * transaction.  A Cancel transaction will reset it to either remote or
    * local when it looks up the original transaction.
    */
   cancel_txn_origin = LOCAL_ORIGIN;

   /* Check to see if Merchant and Organization fields are populated.
    * These are optional for Cancel transactions.
    */
	myUtilsObject.GetSystemDate(systemDate_mmddyyyy);
   if ( myUtilsObject.IsEmpty (m_StrMerchantIDFld, this ) )
   {
      merchant_record_exists = false;
      if ( cancel_flag == false )
      {
         DisplayErrorMsg( "Please enter Merchant ID" );
         m_CntlMerchantIDFld.SetFocus();
         return;
      }
   }

   if ( myUtilsObject.IsEmpty (m_StrOrganizationIDFld, this ) )
   {
      merchant_record_exists = false;
      if ( cancel_flag == false )
      {
         DisplayErrorMsg( "Please enter Organization ID" );
         m_CntlOrganizationIDFld.SetFocus();
         return;
      }
   }

   /* ----------------- */
   /* Get Device Record */
   /* ----------------- */
  // if ( GetDCFStructure() == FALSE )
      //return;

   /* ------------------- */
   /* Get Deferred Record */
   /* ------------------- */
   if ( m_StrTransactionTypeFld.Right(17) == "Deferred Purchase" )
   {
      /* This is a Deferred Transaction.  Is the TERM field populated? */
      if ( myUtilsObject.IsEmpty(m_StrDeferredTermFld, this) )
      {
         /* NO.  Error out. */
         DisplayErrorMsg( "Please enter Deferred Purchase Term" );
         m_CntlDeferredTermFld.SetFocus();
         return;
      }
      else
      {
         /* YES.  Get the Deferred Record. */
         if ( merchant_record_exists == true )
         {
            if ( GetDEFStructure() == TRUE )
            {
               /* CVV2 is required for all Deferreds (not cancels, though) */
               if ( cancel_flag == false )
               {
                  if ( myUtilsObject.IsEmpty(m_StrCVV2Fld, this) )
                  {
                     /* CVV2 field is empty. */
                     DisplayErrorMsg( "Please enter CVV2" );
                     m_CntlCVV2Fld.SetFocus();
                     return;
                  }
               }
            }
            else
               return;
         }
         else
         {
            /* Cannot get Deferred record without Merchant and Org IDs */
            DisplayErrorMsg( "Merchant/Org ID are required for Deferred Txns" );
            m_OverrideButton.EnableWindow(FALSE);
            m_CntlMerchantIDFld.SetFocus();
            UpdateData(FALSE);
            return;
         }
      }
   }

   /* ------------------- */
   /* Get Merchant Record */
   /* ------------------- */
   if ( merchant_record_exists == true )
   {
      if ( GetMCFStructure() == TRUE )
      {
         /* Need to check this again. It could get changed in GetMCFStructure.*/
         if ( merchant_record_exists == true )
         {
            /* Got the Merchant record, so populate some fields with its data.*/
            m_StrMCCFld          = sMCFRecord.voice_category_code;
            m_StrCurrencyCodeFld = sMCFRecord.currency_code;
            m_StrMerchantNameFld = sMCFRecord.name02;

            /* Populate industry code field based on mcf01.mc_fcc_code*/
            switch (sMCFRecord.mc_fcc_code[0])
            {
               case 'R': m_StrIndustryCodeFld = "100"; break;			
               case 'F': m_StrIndustryCodeFld = "101"; break;
               case 'O': m_StrIndustryCodeFld = "110"; break;
               case 'X': m_StrIndustryCodeFld = "200"; break;
               case 'H': m_StrIndustryCodeFld = "300"; break;
               case 'A': m_StrIndustryCodeFld = "400"; break;
               case 'T': m_StrIndustryCodeFld = "500"; break;
               case 'C': m_StrIndustryCodeFld = "900"; break;
               default:  m_StrIndustryCodeFld = "100"; break;
            }
         }
      }
      else
         return;
   }

   /* Check for Amount */
   if ( myUtilsObject.IsEmpty (m_StrAmountFld, this ) )
   {
      DisplayErrorMsg( "Please enter Amount" );
      m_CntlAmountFld.SetFocus();
      return;
   }

   /* --------------- */
   /* Get Card Record */
   /* --------------- */

   /* Check for Card Number */
   if ( myUtilsObject.IsEmpty (m_StrAccountNumberFld, this )) 
   {
      DisplayErrorMsg( "Please enter Account Number" );
      m_CntlAccountNumberFld.SetFocus();
      return;
   }

   /* If Mod10 Button is checked; validate check digit*/
   if ( m_BoolMod10Check )
   {
      if ( !ValidateCardNumber(m_StrAccountNumberFld) )
      {
         m_CntlAccountNumberFld.SetFocus();
         return;
      }
   }

   if ( GetCCFStructure() == TRUE )
   {
      /* Populate Card Holder Name field. */
      if (strlen((char *)sCCFRecord.name_dba) > 0)
         m_StrCardholderNameFld = sCCFRecord.name_dba;	
      else
         m_StrCardholderNameFld = "Foreign Card";
   }
   else
      return;


   /* -------------- */
   /* Get BIN Record */
   /* -------------- */
   if ( GetBINStructure() == TRUE )
   {
      if ( sAuthTxRecord.BIN01_details.auth_ind[0] == LOCAL )
      {
         /* -------------- */
         /* Get CAF Record */
         /* -------------- */

         /* Need to find out if Authorization Location is Remote or Local.
          * Need to get CAF record to do this.
          */
         if ( FALSE == GetCAFStructure( (pCHAR)(sBIN01Record.caf_profile_id)) )
            return;
         else
         {
            
            if ( sCAF01Record.authorization_location[0] == LOCAL_AUTH_LOC )
            {
               /* ---------------- */
               /* LOCAL - ECN CARD */
               /* ---------------- */
               m_disposition = ONUS;
            }
            else
            {
               /* ------------------------ */
               /* Remote - With CAF Record */
               /* ------------------------ */
               m_disposition = OFFUS;
               strcpy( destination, (pcCHAR)sCAF01Record.destination_name );
            }
         }
      }
      else
      {
         /* ---------------------- */
         /* Remote - No CAF Record */
         /* ---------------------- */
         m_disposition = OFFUS;
         strcpy( destination,
                (pcCHAR)sAuthTxRecord.BIN01_details.destination_name );
      }

      /* We have determined On-Us or Off-Us. Now determine Card Type. */
      if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_AMEX))
         card_type = AMEX_CARD;

      else if ( NULL != strstr(destination, OFF_US_MASTERCARD) )
         card_type = MC_CARD;
	  else if ( NULL != strstr(destination, OFF_US_MASTERCARD2) ) // For NCMCR2
         card_type = MC_CARD;


      else if ( NULL != strstr(destination, OFF_US_AMEX_ATM) )
         card_type = AMEX_CARD;

      else if ( NULL != strstr(destination, OFF_US_AMEX_POS) )
         card_type = AMEX_CARD;

      else if ( NULL != strstr(destination, OFF_US_AMEX) )
         card_type = AMEX_CARD;

      else if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_JCB))
         card_type = JCB_CARD;

      else if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_VISA))
         card_type = VISA_CARD;

      else if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_MASTERCARD))
         card_type = MC_CARD;
	  else if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_CUP))
         card_type = CUP_CARD;
	  else if ( 0==strcmp((pcCHAR)sAuthTxRecord.BIN01_details.card_family,CARD_DINERS))
         card_type = DINERS_CARD;


      if ( 0 == strncmp((pcCHAR)sBIN01Record.primary_key.low,VE_BIN,6) )
      {
         visa_electron = true;
         card_type = VISA_CARD;
      }

      cvv2_len = strlen( m_StrCVV2Fld );


	  if ( m_StrCVV2Fld[0] == '*' )
      {
          DisplayErrorMsg( "Please enter valid CVV2 value" );
         m_CntlCVV2Fld.SetFocus();
         return;
      }
	  

      /*+--------------------------------------------------------------+
        | VALIDATE CVV2                                                |
        +--------------------------------------------------------------+
        | Here are the rules:                                          |
        | 1.  ECN Visa/MC/JCB only use 3-digit value and is mandatory. |
        | 2.  ECN Amex uses 3-digit and is optional.                   |
        | 3.  Off-Us Visa/JCB do not use 3-digit (optional).           |
        | 4.  Off-Us MC uses 3-digit and is mandatory.                 |
        | 5.  Off-Us Amex uses 4-digit and is optional.                |
        | 6.  Visa Electron cards use 3 digit and is mandatory.        |
        +--------------------------------------------------------------+*/
      if ( m_disposition == ONUS )
      {
         if ( cvv2_len > 0 )
         {
            if ( cvv2_len == 3 )
			{
               cvv2_status = CVV2_GOOD;
			}
			else if ( card_type == DINERS_CARD )
            {
            	 cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage,
                         "Please enter a valid 3-digit CVC2 for Diners card, it can not be blank." );

            }
            else
            {
               cvv2_status = CVV2_DECLINED;
               if ( card_type == AMEX_CARD )
                  strcpy( strErrorMessage,
                         "CVV2 should be 3-digits, but is optional for Amex card." );
               else
                  strcpy( strErrorMessage, "Expecting 3-digit CVC2" );
            }
         }
		 else if ( card_type == DINERS_CARD )
         {
            cvv2_status = CVV2_DECLINED;
            strcpy( strErrorMessage,
                     "Please enter a valid 3-digit CVC2 for Diners card, it can not be blank." );
         }
         else
         {
            if ( card_type == AMEX_CARD || card_type == MC_CARD) // TF Phani - CVV2 not mandatory for MC
               cvv2_status = CVV2_GOOD;
            else
            {
               cvv2_status = CVV2_DECLINED;
               strcpy( strErrorMessage, "Please enter CVC2" );
            }
         }
      }
      else
      {
         /* Disposition is OFF-US */
         if ( (card_type == MC_CARD) || (visa_electron == true) )
         {
            if ( cvv2_len > 0 )
            {
               if ( cvv2_len == 3 )
                  cvv2_status = CVV2_GOOD;
               else
               {
                  cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage, "Expecting 3-digit CVC2" );
               }
            }
            else
            {
               cvv2_status = CVV2_DECLINED;
               if ( card_type == MC_CARD )
               { 
				     cvv2_status = CVV2_GOOD; // TF Phani - CVV2 not mandatory for MC - request for Kenneth
			   // strcpy( strErrorMessage, "Please enter CVC2 - Mandatory for MasterCard" );
			   }
               else
                  strcpy( strErrorMessage, "Please enter CVC2 - Mandatory for Visa Electron" );
            }
         }
         else if ( card_type == AMEX_CARD )
         {
            if ( cvv2_len > 0 )
            {
               if ( cvv2_len == 4 )
                  cvv2_status = CVV2_GOOD;
               else
               {
                  cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage,
                         "Please enter a valid 4-digit CVC2 for Amex card, or leave it blank." );
               }
            }
            else
               cvv2_status = CVV2_GOOD;
         }
		  else if ( card_type == DINERS_CARD )
         {
            if ( cvv2_len > 0 )
            {
               if ( cvv2_len == 3 )
                  cvv2_status = CVV2_GOOD;
               else
               {
                  cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage,
                         "Please enter a valid 3-digit CVC2 for Diners card, it can not be blank." );
               }
            }
            else
			{
				 cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage,
                         "Please enter a valid 3-digit CVC2 for Diners card, it can not be blank." );
			}
         }
         else
         {
            if ( cvv2_len > 0 )
            {
               if ( cvv2_len == 3 )
                  cvv2_status = CVV2_GOOD;
               else
               {
                  cvv2_status = CVV2_DECLINED;
                  strcpy( strErrorMessage, "Expecting 3-digit CVC2" );
               }
            }
            else
               cvv2_status = CVV2_GOOD;
         }
      }

      if ( cvv2_status == CVV2_DECLINED )
      {
         DisplayErrorMsg( strErrorMessage );
         m_CntlCVV2Fld.SetFocus();
         return;
      }
   }
   else
      return;


   /* Validate Expiration Date */
   if ( myUtilsObject.IsEmpty (m_StrExpiryDateFld, this ) )
   {
      if ( cancel_flag == false )
      {
         DisplayErrorMsg( "Please enter Expiry Date" );
         m_CntlExpiryDateFld.SetFocus();
         return;
      }
   }
   else if ( !ValidateDate ( m_StrExpiryDateFld ) )
   {
      m_CntlExpiryDateFld.SetFocus();
      return;
   }


	/* Is Authorization Number required? 
	 * Remove the check of if (m_nDialogMode == MODE_OVERRIDE), as we just set m_nDialogMode = MODE_TRANSACTION;
	 * at the beginning of this function.
	 *
	 * For "cancel ..." transaction, we use Auth Number to find the original transaction, so we need it.
	 */
	if ( cancel_flag == true )
	{
		if ( myUtilsObject.IsEmpty (m_StrAuthNumberFld, this ) )
		{
			DisplayErrorMsg( "Please enter Authorization Number" );
			m_CntlAuthNumberFld.SetFocus();
			return;
		}
	}

   /* ------------------ */
   /* Validate Card Type */
   /* ------------------ */
   if ( cancel_flag == false )
   {
      if ( ValidateCardType() == FALSE )
      {
         /* Merchant does not accept this card type. */
         return;
      }
   }

   /* --------------- */
   /* Get Card Status */
   /* --------------- */
   if ( m_disposition == ONUS )
   {
      if ( GetCardStatus() == TRUE )
      {
         /* Populate Card Status field. */
         m_StrCardStatusFld  = sCSF01Record.primary_key.status;
         m_StrCardStatusFld += " - ";
        // m_StrCardStatusFld += sCSF01Record.description;
		 (CString)m_StrCardStatusFld = (CString)((char *) sCSF01Record.description);
      }
      else
         m_StrCardStatusFld = "Unable to get card status";
   }
   else
   {
      m_StrCardStatusFld = "No status for Off-Us cards";
   }

   UpdateData(FALSE);

   /* Get original transaction for reversals. */
   if ( cancel_flag == true )
   {
      if ( GetOrigTxn() == false )
      {
         /* Can't go any further, including enable the Commit Auth button */
         return;
      }
   }

   /* Leave enabled in event they want to edit and reverify */
   m_ProcessButton.EnableWindow(TRUE);

   /* Save Auth_Tx structure.  The COMMIT button zeroes it out. */
   memcpy( &sAuthTxRecordTemp, &sAuthTxRecord, sizeof(AUTH_TX) );
   return;
}


BOOL CVoiceAuth::ValidateCardNumber ( CString strCardNumber )
{
		char pCardNbr[20];
		int nLastDigit;
        CMyUtils myUtilsObject;

		int len = strCardNumber.GetLength();
		
		memcpy ((LPTSTR) pCardNbr, strCardNumber, len - 1 );
		nLastDigit = myUtilsObject.GetMod10CheckDigit ( pCardNbr, len-1 );
        memset( pCardNbr, 0, sizeof ( pCardNbr ) );
		memcpy ((LPTSTR) pCardNbr, strCardNumber.Right(1), 1 );

		if ( nLastDigit != atoi ( pCardNbr ) )
      {
         DisplayErrorMsg( "ERROR: Invalid Card Number - Check Digit Failed." );
			return FALSE;

      }
      return TRUE;
}


void CVoiceAuth::OnMod10CheckButton() 
{
	// TODO: Add your control notification handler code here
	
}

void CVoiceAuth::OnChangeAccountNumberFld() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	m_ProcessButton.EnableWindow(FALSE);/*If card number field is changed, disable
										  Commit button to force reverify*/
	
}

void CVoiceAuth::OnChangeMerchantIdFld() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	m_ProcessButton.EnableWindow(FALSE);  /*If merchant number field is changed, disable
										  Commit button to force reverify*/
	
}

void CVoiceAuth::OnChangeCVV2Fld() 
{
   /* If CVV2 field is changed, disable
    * COMMIT button to force a reverify.
    */
	m_ProcessButton.EnableWindow(FALSE);
}

BOOL voice_check_If_Diners_Transaction (AUTH_TX sAuthTxRecord)
{
	if((sAuthTxRecord.TLF01_details.card_num[0]=='3') &&
	   ((sAuthTxRecord.TLF01_details.card_num[1] == '0') ||
		(sAuthTxRecord.TLF01_details.card_num[1] == '6') ||
		(sAuthTxRecord.TLF01_details.card_num[1] == '8') ||
		(sAuthTxRecord.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((sAuthTxRecord.TLF01_details.card_num[0]=='6') &&
			 ((sAuthTxRecord.TLF01_details.card_num[1] == '0') ||
			 (sAuthTxRecord.TLF01_details.card_num[1] == '4') ||
			 (sAuthTxRecord.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}


BOOL CVoiceAuth::GetOrigTxn() 
{
   BOOL      ret_val = TRUE;
   CHAR      strErrorMessage[300] = "",stndinString[15]="";
   CHAR      mid[16] = {0};
   CHAR      oid[5]  = {0};
   CMyUtils  myUtilsObject;
   int 		 len = 0;

   UpdateData(TRUE);
   m_StrMessageLbl = _T("");
   m_StrResponseTextFld = _T("");


   /* Verify transaction in BCH20. */
   if ( !CheckBCH20() )
   {
      /* Is this a network txn? Check NCF30. */
      if ( !CheckNCF30() )
      {
         /* Txn is not in BCH20 nor in NCF30.  Exit. */
         ret_val = FALSE;
      }
      else
      {
         /* Copy the data from NCF30 into Auth_Tx. */
         memcpy( sAuthTxRecord.TLF01_details.card_num,               sNCF30Record.card_nbr,                      sizeof(sNCF30Record.card_nbr)-1 ); 
         memcpy( sAuthTxRecord.TLF01_details.processing_code,        sNCF30Record.processing_code,               sizeof(sNCF30Record.processing_code)-1 );
         memcpy( sAuthTxRecord.TLF01_details.orig_retrieval_ref_num, sNCF30Record.primary_key.retrieval_ref_num, sizeof(sNCF30Record.primary_key.retrieval_ref_num)-1 ); 
         memcpy( sAuthTxRecord.TLF01_details.orig_amount,            sNCF30Record.tran_amount,                   sizeof(sNCF30Record.tran_amount)-1 );
         memcpy( sAuthTxRecord.TLF01_details.auth_number,            sNCF30Record.auth_number,                   sizeof(sNCF30Record.auth_number)-1 );
         memcpy( sAuthTxRecord.TLF01_details.response_code,          sNCF30Record.response_code,                 sizeof(sNCF30Record.response_code)-1 );
         memcpy( sAuthTxRecord.TLF01_details.currency_code,          sNCF30Record.currency_code,                 sizeof(sNCF30Record.currency_code)-1 );
         memcpy( sAuthTxRecord.TLF01_details.orig_message,           sNCF30Record.message_type,                  sizeof(sNCF30Record.message_type)-1 );
         memcpy( sAuthTxRecord.TLF01_details.mcard_banknet,          sNCF30Record.network_data,                  sizeof(sNCF30Record.network_data)-1 );
         memcpy( sAuthTxRecord.TLF01_details.invoice_number,         sNCF30Record.invoice_nbr,                   sizeof(sNCF30Record.invoice_nbr)-1 );
         memcpy( sAuthTxRecord.TLF01_details.orig_sys_trace_num,     sNCF30Record.sys_trace_audit_nbr,           sizeof(sNCF30Record.sys_trace_audit_nbr)-1 );
         memcpy( sAuthTxRecord.orig_local_time_date,                 sNCF30Record.transmission_timestamp,        sizeof(sNCF30Record.transmission_timestamp)-1 );
         memcpy( sAuthTxRecord.TLF01_details.settlement_date,        sNCF30Record.settlement_date,               sizeof(sNCF30Record.settlement_date)-1 );
         memcpy( sAuthTxRecord.TLF01_details.tran_fee_amount,        sNCF30Record.tran_fee_amount,               sizeof(sNCF30Record.tran_fee_amount)-1 );
         memcpy( sAuthTxRecord.TLF01_details.merchant_id,            sNCF30Record.merchant_id,                   sizeof(sNCF30Record.merchant_id)-1 );
         memcpy( sAuthTxRecord.TLF01_details.terminal_id,            sNCF30Record.device_id,                     sizeof(sNCF30Record.device_id)-1 );
         memcpy( sAuthTxRecord.TLF01_details.issuer_id,              sNCF30Record.primary_key.network_id,        sizeof(sNCF30Record.primary_key.network_id)-1 );
         memcpy( sAuthTxRecord.TLF01_details.primary_key.transaction_id,sNCF30Record.transaction_id,             sizeof(sNCF30Record.transaction_id)-1 );
         memcpy( sAuthTxRecord.local_date_time,                      sNCF30Record.tran_date+2, 6 );
         memcpy( sAuthTxRecord.local_date_time+6,                    sNCF30Record.tran_time,   6 );
         memcpy( sAuthTxRecord.TLF01_details.dispensation.origin,    "RO", 2);
         sAuthTxRecord.TLF01_details.entry_type = sNCF30Record.primary_key.network_type[0];
         cancel_txn_origin = REMOTE_ORIGIN;
         memcpy( stndinString,         sNCF30Record.invoice_nbr,                   sizeof(sNCF30Record.invoice_nbr)-1 );
		 //stndinString
		 if(strcmp(stndinString,PROCESSED)==0 )
		 {
		  memcpy( sAuthTxRecord.TLF01_details.product_codes[1].code,         sNCF30Record.invoice_nbr,                   sizeof(sNCF30Record.invoice_nbr)-1 );

		 }
         /* Now get original tlf01 record and display additional data not
          * found in ncf30 record.  If txn is not in TLF01, continue with
          * the Voice Cancellation without displaying and logging these values.
          */
         if( CheckTLF01() )
         {
             memcpy( sAuthTxRecord.TLF01_details.category_code, sTLF01Record.category_code, sizeof(sAuthTxRecord.TLF01_details.category_code) -1);
             memcpy( sAuthTxRecord.TLF01_details.currency_code, sTLF01Record.currency_code, sizeof(sAuthTxRecord.TLF01_details.currency_code)-1);
             memcpy( sAuthTxRecord.TLF01_details.exp_date,      sTLF01Record.exp_date,      sizeof(sAuthTxRecord.TLF01_details.exp_date)-1);
             m_StrMCCFld          = sAuthTxRecord.TLF01_details.category_code;
             m_StrCurrencyCodeFld = sAuthTxRecord.TLF01_details.currency_code;
         }

         m_StrResponseTextFld = _T("");
         m_StrMessageLbl = "";
         UpdateData(FALSE);
      }
   }
   else
   {
	  memcpy(sAuthTxRecord.TLF01_details.card_num,               sBCH20Record.card_nbr,            sizeof(sAuthTxRecord.TLF01_details.card_num)-1 );
      memcpy(sAuthTxRecord.TLF01_details.processing_code,        sBCH20Record.processing_code,     sizeof(sAuthTxRecord.TLF01_details.processing_code)-1 );
      memcpy(sAuthTxRecord.TLF01_details.orig_amount,            sBCH20Record.tran_amount,         sizeof(sAuthTxRecord.TLF01_details.orig_amount) - 1);
      memcpy(sAuthTxRecord.TLF01_details.orig_retrieval_ref_num, sBCH20Record.primary_key.retrieval_ref_num,   sizeof(sAuthTxRecord.TLF01_details.orig_retrieval_ref_num) - 1);
      if(sAuthTxRecord.TLF01_details.retrieval_ref_num[0] == 0x00)
	  memcpy(sAuthTxRecord.TLF01_details.retrieval_ref_num, sBCH20Record.primary_key.retrieval_ref_num,   sizeof(sAuthTxRecord.TLF01_details.retrieval_ref_num) - 1);

      memcpy(sAuthTxRecord.TLF01_details.auth_number,            sBCH20Record.auth_number,         sizeof(sAuthTxRecord.TLF01_details.auth_number) - 1);
      memcpy(sAuthTxRecord.TLF01_details.orig_message,           sBCH20Record.message_type,        sizeof(sAuthTxRecord.TLF01_details.orig_message) - 1);
      memcpy(sAuthTxRecord.TLF01_details.orig_sys_trace_num,     sBCH20Record.sys_trace_audit_num, sizeof(sAuthTxRecord.TLF01_details.orig_sys_trace_num) - 1);
      memcpy(sAuthTxRecord.TLF01_details.transmission_timestamp, sBCH20Record.transmission_timestamp, sizeof(sAuthTxRecord.TLF01_details.transmission_timestamp) - 1);
      memcpy(sAuthTxRecord.orig_local_time_date,                 sBCH20Record.transmission_timestamp, sizeof(sAuthTxRecord.TLF01_details.transmission_timestamp) - 1);
      memcpy(sAuthTxRecord.TLF01_details.merchant_id,            sBCH20Record.merchant_id,            sizeof(sBCH20Record.merchant_id)-1 );
      memcpy(sAuthTxRecord.TLF01_details.exp_date,               sBCH20Record.exp_date,               sizeof(sBCH20Record.exp_date)-1 );
      memcpy(sAuthTxRecord.TLF01_details.visa_tran_id,           sBCH20Record.mcard_banknet,               sizeof(sAuthTxRecord.TLF01_details.visa_tran_id)-1 );
	  
	 /* For STIP txn ATP needs STIP txn status. TF-Ajay */
	  memcpy(sAuthTxRecord.TLF01_details.product_codes[1].code,   sBCH20Record.product_codes[1].code,               sizeof(sBCH20Record.product_codes[1].code)-1 );
	  memcpy(sAuthTxRecord.TLF01_details.product_codes[12].amount,   sBCH20Record.product_codes[12].amount,               sizeof(sBCH20Record.product_codes[12].amount)-1 );
      memcpy(sAuthTxRecord.local_date_time,                      sBCH20Record.tran_date+2, 6 );
      memcpy(sAuthTxRecord.local_date_time+6,                    sBCH20Record.tran_time,   6 );
	  memcpy( sAuthTxRecord.orig_local_time_date,				 sBCH20Record.transmission_timestamp,		sizeof(sBCH20Record.transmission_timestamp));				
      memcpy(sAuthTxRecord.TLF01_details.dispensation.origin,	 "LO", 2);
	  memcpy(sAuthTxRecord.TLF01_details.settlement_date,	 sBCH20Record.product_codes[19].code, 4);
	  memcpy(sAuthTxRecord.TLF01_details.product_codes[10].amount,    sBCH20Record.product_codes[10].amount,   10 );
	  memcpy(sAuthTxRecord.TLF01_details.product_codes[1].amount,    sBCH20Record.product_codes[1].amount,   12 );
      cancel_txn_origin = LOCAL_ORIGIN;
      if(voice_check_If_Diners_Transaction(sAuthTxRecord))
      {
		len = strlen ((pCHAR)sBCH20Record.product_codes[13].amount);
		if(len > 0 )
		{
			memcpy(sAuthTxRecord.TLF01_details.resp_source,
					sBCH20Record.product_codes[13].code,6);
			memcpy(sAuthTxRecord.TLF01_details.resp_source +6,
					sBCH20Record.product_codes[13].amount,len);
		}
		else
		{
			len = strlen ((pCHAR)sBCH20Record.product_codes[13].code);
			memcpy(sAuthTxRecord.TLF01_details.resp_source,
					sBCH20Record.product_codes[13].code,len);
		}
      }
   }

   if ( ret_val == TRUE )
   {
      if ( myUtilsObject.IsEmpty (m_StrExpiryDateFld, this ) )
      {
         /* Expiration date field is empty.  Populate it from orig txn. */
         m_StrExpiryDateFld = sAuthTxRecord.TLF01_details.exp_date;
      }

      /* Need to get the merchant record if Operator did not enter it.
       * Also need to get it if the Operator DID enter it, but it is
       * different than the one from BCH20 or NCF30.
       */
      memcpy( oid, m_StrOrganizationIDFld, 4 );
      if ( oid[0] == 0x00 )
      {
         memset( oid, '0', 4 );
         m_StrOrganizationIDFld = oid;
      }

      memcpy( mid, m_StrMerchantIDFld, 15 );
      if ( (0 != strcmp(mid,(const char *)sAuthTxRecord.TLF01_details.merchant_id)) ||
           (merchant_record_exists == false) )
      {
         /* Go get the Merchant Record. */
         m_StrMerchantIDFld     = sAuthTxRecord.TLF01_details.merchant_id;
         merchant_record_exists = true;
         if ( GetMCFStructure() )
         {
            if ( merchant_record_exists == true )
            {
               /* Populate some GUI fields. */
               m_StrMCCFld          = sMCFRecord.voice_category_code;
               m_StrCurrencyCodeFld = sMCFRecord.currency_code;
               m_StrMerchantNameFld = sMCFRecord.name02;

               /* populate industry code field based on mcf01.mc_fcc_code*/
               switch (sMCFRecord.mc_fcc_code[0])
               {
                  case 'R': m_StrIndustryCodeFld = "100"; break;			
                  case 'F': m_StrIndustryCodeFld = "101"; break;
                  case 'O': m_StrIndustryCodeFld = "110"; break;
                  case 'X': m_StrIndustryCodeFld = "200"; break;
                  case 'H': m_StrIndustryCodeFld = "300"; break;
                  case 'A': m_StrIndustryCodeFld = "400"; break;
                  case 'T': m_StrIndustryCodeFld = "500"; break;
                  case 'C': m_StrIndustryCodeFld = "900"; break;
                  default:  m_StrIndustryCodeFld = "100"; break;
               }

               UpdateData(FALSE);

               /* Populate AUTH_TX fields. */
               memcpy( sAuthTxRecord.TLF01_details.category_code,
                       sMCFRecord.voice_category_code,
                       sizeof (sAuthTxRecord.TLF01_details.category_code) );

               memcpy( sAuthTxRecord.TLF01_details.currency_code,
                       sMCFRecord.currency_code,
                       sizeof(sAuthTxRecord.TLF01_details.currency_code) );

               memcpy( sAuthTxRecord.TLF01_details.card_holder_name,
                       m_StrCardholderNameFld,
                       sizeof(sAuthTxRecord.TLF01_details.card_holder_name)-1 );

               memcpy( sAuthTxRecord.TLF01_details.organization_id,
                       oid,
                       sizeof(sAuthTxRecord.TLF01_details.organization_id) );
            }
         }
         else
         {
            if ( cancel_txn_origin == LOCAL_ORIGIN )
            {
               sprintf( strErrorMessage,
                       "Cannot find merchant record '%s' for local txn. Continuing...",
                        sAuthTxRecord.TLF01_details.merchant_id );
            }
            else
            {
               sprintf( strErrorMessage,
                       "Network Txn: Verify Complete...",
                        sAuthTxRecord.TLF01_details.merchant_id );
            }
            DisplayErrorMsg( strErrorMessage );
         }
      }
   }
   return( ret_val );
}

BOOL CVoiceAuth::CheckTLF01() 
{
   char strErrorMessage[300] = "";
   CMyUtils myUtilsObject;

   ZeroMemory ( &sTLF01Record, sizeof ( TLF01) );

   memcpy ( sTLF01Record.primary_key.transaction_id,  sAuthTxRecord.TLF01_details.primary_key.transaction_id, 
														sizeof(sAuthTxRecord.TLF01_details.primary_key.transaction_id));

   int rcode = txdsapi_get_record( (pBYTE)&sTLF01Record, sizeof (TLF01), TLF01_DATA,  strErrorMessage );
   if( PTEMSG_OK == rcode )
   {
      return TRUE;
   }

   return FALSE;
}


BOOL CVoiceAuth::GetDCFStructure()
{
   CHAR strDeviceID[20]      = "";
   CHAR strErrorMessage[290] = "";
   BOOL ret_val = TRUE;
   INT  rcode;

   m_bDCFRecordNotFound = FALSE;
   ZeroMemory( &sDCFRecord, sizeof(DCF01) );

   if ( (m_StrTransactionTypeFld == "Deferred Purchase") ||
        (m_StrTransactionTypeFld == "Cancel Deferred Purchase") )
      GetPrivateField2(strDeviceID);
   else
      GetPrivateField1(strDeviceID);

   /* Get device record */
   memcpy( sDCFRecord.primary_key.device_id,
           strDeviceID,
           sizeof(sDCFRecord.primary_key.device_id) );

   rcode = txdsapi_get_record( (pBYTE)&sDCFRecord,
                               sizeof(DCF01),
                               DCF01_DATA,
                               strErrorMessage );
   if( PTEMSG_NOT_FOUND == rcode )
   {
      DisplayErrorMsg( "Invalid Device ID" );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      m_bDCFRecordNotFound = TRUE;
      ret_val = FALSE;
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      DisplayErrorMsg( "Please try again" );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);	
      ret_val = FALSE;
   }
   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }

   /* Device status R means restricted */
   if ( strcmp ( (char*)sDCFRecord.status, "R" ) == 0 )
   {
      DisplayErrorMsg( "ERROR: Invalid Device status" );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }

   return( ret_val );
}


BOOL CVoiceAuth::GetMCFStructure()
{
   CHAR     strErrorMessage[290]    = "";
   CHAR     systemDate_yyyymmdd[20] = "";
   LONG     julianStart             = 0L;
   LONG     julianEnd               = 0L;
   LONG     julianSystemDate        = 0L;
   BOOL     ret_val     = TRUE;
   INT      cancel_flag = FALSE;
   INT      rcode;
   CMyUtils myUtilsObject;


   m_bMCFRecordNotFound = FALSE;
   ZeroMemory( &sMCFRecord, sizeof(MCF01) );

   if ( m_StrTransactionTypeFld.Left(6) == "Cancel"  )
   {
      cancel_flag = true;
      if ( myUtilsObject.IsEmpty (m_StrOrganizationIDFld, this ) )
      {
         /* Merchant record will not be found if OID is not present.
          * It is not required for cancellations, so we must assume 0000.
          */
         m_StrOrganizationIDFld = "0000";
      }
   }

   /* Get the Merchant Record */
   memcpy ( sMCFRecord.primary_key.merchant_id,
            m_StrMerchantIDFld,
            sizeof(sMCFRecord.primary_key.merchant_id) );

   memcpy( sMCFRecord.primary_key.organization_id,
           m_StrOrganizationIDFld,
           sizeof(sMCFRecord.primary_key.organization_id) );

   rcode = txdsapi_get_record( (pBYTE)&sMCFRecord,
                               sizeof(MCF01),
                               MCF01_DATA,
                               strErrorMessage );
   if( PTEMSG_NOT_FOUND == rcode )
   {
      merchant_record_exists = false;
      if ( cancel_flag == false )
      {
         DisplayErrorMsg( "Invalid Merchant or Organization ID" );
         m_OverrideButton.EnableWindow(FALSE);
         m_CntlMerchantIDFld.SetFocus();
         UpdateData(FALSE);
         m_bMCFRecordNotFound = TRUE;
         ret_val = FALSE;
      }
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      DisplayErrorMsg( "Please try again" );
      m_OverrideButton.EnableWindow(FALSE);
      ret_val = FALSE;
   }
   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }
   else
   {
      merchant_record_exists = true;
      if ( cancel_flag == false )
      {
         if ( strcmp ( (char*)sMCFRecord.status, "A" ) != 0 )
         {
            DisplayErrorMsg( "Invalid Merchant Status" );
            m_CntlMerchantIDFld.SetFocus();
            m_OverrideButton.EnableWindow(FALSE);
            UpdateData(FALSE);
            ret_val = FALSE;
         }

         /* Check merchant blocking dates */
         julianStart = atol ((LPCTSTR)sMCFRecord.block_start_date);
         julianEnd   = atol ((LPCTSTR)sMCFRecord.block_end_date);

         strcpy ( systemDate_yyyymmdd, systemDate_mmddyyyy + 4 );
         strncat( systemDate_yyyymmdd, systemDate_mmddyyyy,  4 );

         julianSystemDate =  Txutils_Calculate_Julian_Date ((BYTE*)systemDate_yyyymmdd);

         if ( ( julianSystemDate <= julianEnd ) && 
              ( julianSystemDate >= julianStart ) )
         {
            DisplayErrorMsg( "Merchant is Blocked!" );
            m_CntlMerchantIDFld.SetFocus();
            m_OverrideButton.EnableWindow(FALSE);
            UpdateData(FALSE);
            ret_val = FALSE;
         }
         else if ( julianEnd == 0L && julianStart != 0L &&
                   julianStart <= julianSystemDate )
         {
            DisplayErrorMsg( "Merchant is Blocked!" );
            m_CntlMerchantIDFld.SetFocus();
            m_OverrideButton.EnableWindow(FALSE);
            UpdateData(FALSE);
            ret_val = FALSE;
         }
      }
      memcpy( &sAuthTxRecord.MCF01_details, &sMCFRecord, sizeof(MCF01));
   }

   return( ret_val );
}


BOOL CVoiceAuth::GetDEFStructure()
{
   char   strErrorMessage[290] = "";
   DEF01  sDEFRecord;
   INT    rcode;
   BOOL   ret_val = TRUE;

   ZeroMemory ( &sDEFRecord, sizeof(DEF01) );

   memcpy( sDEFRecord.primary_key.merchant_id,
            m_StrMerchantIDFld,
            sizeof(sDEFRecord.primary_key.merchant_id) );

   memcpy( sDEFRecord.primary_key.organization_id,
           m_StrOrganizationIDFld,
           sizeof(sDEFRecord.primary_key.organization_id) );

   memcpy( sDEFRecord.primary_key.term_length,
           m_StrDeferredTermFld,
           sizeof(sDEFRecord.primary_key.term_length) );

   rcode = txdsapi_get_record( (pBYTE)&sDEFRecord,
                               sizeof(DEF01),
                               DEF01_DATA,
                               strErrorMessage );
   if( PTEMSG_NOT_FOUND == rcode )
   {
      DisplayErrorMsg( "Deferred record not found for Merchant and Term specified" );
      m_OverrideButton.EnableWindow(FALSE);
      m_bDCFRecordNotFound = TRUE;
      ret_val = FALSE;
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      DisplayErrorMsg( "Please try again" );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);	
      ret_val =  FALSE;
   }
   else if( PTEMSG_OK != rcode )
   {
      DisplayErrorMsg( strErrorMessage );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }

   return( ret_val );
}

BOOL CVoiceAuth::GetCCFStructure()
{
   CHAR strErrorMessage[290] = "";
   CHAR temp_error[290];
   INT  rcode;
   BOOL ret_val = TRUE;

   ZeroMemory( &sCCFRecord, sizeof(CCF01) );

   memcpy( sCCFRecord.primary_key.card_nbr,
           m_StrAccountNumberFld,
           sizeof(sCCFRecord.primary_key.card_nbr) );

   memcpy( sCCFRecord.primary_key.card_type,
           "C",
           sizeof(sCCFRecord.primary_key.card_type) );

   rcode = txdsapi_get_record( (pBYTE)&sCCFRecord,
                               sizeof(CCF01),
                               CCF01_DATA,
                               strErrorMessage );
   if( PTEMSG_OK != rcode && PTEMSG_NOT_FOUND != rcode )
   {
      ZeroMemory( temp_error, sizeof(temp_error) );
      sprintf( temp_error, "Card Err: %s", strErrorMessage );

      DisplayErrorMsg( temp_error );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }

   return( ret_val );
}


BOOL CVoiceAuth::GetBINStructure()
{
   CHAR strErrorMessage[290] = "";
   CHAR cardNbr[22];
   BOOL ret_val;
   INT  rcode;

   ZeroMemory( &sBIN01Record, sizeof(BIN01) );

   /* Get BIN01 record so we can display the Bank Name prior to a commit.
    * Note: Bin type will always be credit.
    */
   memset( cardNbr, 0x00, sizeof(cardNbr) );
   memcpy( cardNbr, m_StrAccountNumberFld, 20 );

   if ( (m_StrTransactionTypeFld == "Deferred Purchase") ||
        (m_StrTransactionTypeFld == "Cancel Deferred Purchase") )
   {
      Deferred_Txn = true;
      memcpy( cardNbr+20, "E", 1 );
   }
   else
   {
      memcpy( cardNbr+20, "C", 1 );
   }
   rcode = txdsapi_find_bin(cardNbr,&sBIN01Record,BIN01_DATA,strErrorMessage);
   if ( rcode == PTEMSG_OK )
   {
      /* Display bank name. */
      m_StrBankNameFld = sBIN01Record.description;
      memcpy( &sAuthTxRecord.BIN01_details, &sBIN01Record, sizeof(BIN01));
      ret_val = TRUE;
   }
   else
   {
      DisplayErrorMsg( "Bin Range not found. Check card number." );
      m_CntlAccountNumberFld.SetFocus();
      ret_val = FALSE;
   }
   UpdateData(FALSE);

   return( ret_val );
}


BOOL CVoiceAuth::GetCAFStructure( pCHAR profile_id )
{
   CHAR strErrorMessage[290] = "";
   CHAR temp_err[290];
   BOOL ret_val = TRUE;
   INT  rcode;

   /* Set up primary key for database search. */
   ZeroMemory ( &sCAF01Record, sizeof(CAF01) );
   memcpy( sCAF01Record.primary_key.caf_profile_id, profile_id, 4 );
   memcpy( sCAF01Record.primary_key.type, "C", 1 );

   /* Go get the Card Authorization Profile record. */
   rcode = txdsapi_get_record( (pBYTE)&sCAF01Record,
                               sizeof(CAF01),
                               CAF01_DATA,
                               strErrorMessage );

   if( PTEMSG_OK != rcode )
   {
      ZeroMemory( temp_err, sizeof(temp_err) );
      sprintf( temp_err, "CAF Err: %s", strErrorMessage );

      DisplayErrorMsg( temp_err );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }
   else
      memcpy( &sAuthTxRecord.CAF01_details, &sCAF01Record, sizeof(CAF01));

   return( ret_val );
}


BOOL CVoiceAuth::GetCardStatus()
{
   BOOL ret_val;

   ret_val = GetCCF02PStructure();
   if ( ret_val == TRUE )
      ret_val = GetCSF01Structure();

   return( ret_val );
}


BOOL CVoiceAuth::GetCCF02PStructure()
{
   CHAR strErrorMessage[290] = "";
   INT  rcode;
   BOOL ret_val = TRUE;

   ZeroMemory( &sCCF02PRecord, sizeof(CCF02P) );

   memcpy( sCCF02PRecord.primary_key.card_nbr,
           m_StrAccountNumberFld,
           sizeof(sCCF02PRecord.primary_key.card_nbr) );

   memcpy( sCCF02PRecord.primary_key.card_type, "C", 1 );

   rcode = txdsapi_get_record( (pBYTE)&sCCF02PRecord,
                               sizeof(CCF02P),
                               CCF02P_DATA,
                               strErrorMessage );

   if ( PTEMSG_OK == rcode )
      memcpy( &sAuthTxRecord.CCF02P_details, &sCCF02PRecord, sizeof(CCF02P));
   else
      ret_val = FALSE;

   return( ret_val );
}


BOOL CVoiceAuth::GetCSF01Structure()
{
   CHAR strErrorMessage[290] = "";
   INT  rcode;
   BOOL ret_val = TRUE;

   ZeroMemory( &sCSF01Record, sizeof(CSF01) );

   memcpy( sCSF01Record.primary_key.status,
           sCCF02PRecord.status,
           sizeof(sCSF01Record.primary_key.status) );

   rcode = txdsapi_get_record( (pBYTE)&sCSF01Record,
                               sizeof(CSF01),
                               CSF01_DATA,
                               strErrorMessage );

   if ( PTEMSG_OK != rcode )
      ret_val = FALSE;

   return( ret_val );
}

BOOL CVoiceAuth::ValidateCardType()
{
   CHAR strErrorMessage[290] = "";
   CHAR temp_err[290];
   INT  rcode;
   BOOL ret_val = TRUE;

   ZeroMemory( &sMCT01Record, sizeof(MCT01) );


   /* Get the Merchant Record */
   /*PHani - POS is sending CUP MID, whcihc should be populated as CUP ID and MID should be replaced*/

	if(m_StrAccountNumberFld[0]== '6')
	{
	//	strncpy(auth_tx.TLF01_details.visa_merchant_id,auth_tx.TLF01_details.merchant_id,sizeof(auth_tx.TLF01_details.merchant_id));
	//	strncpy(auth_tx.TLF01_details.merchant_id,sMCF01.primary_key.merchant_id,sizeof(auth_tx.TLF01_details.merchant_id));
	
			memcpy ( sMCT01Record.primary_key.merchant_id,
            sMCFRecord.primary_key.merchant_id,
            sizeof(sMCFRecord.primary_key.merchant_id) );
	}else

	{
		memcpy ( sMCT01Record.primary_key.merchant_id,
            m_StrMerchantIDFld,
            sizeof(sMCFRecord.primary_key.merchant_id) );
	}
   memcpy( sMCT01Record.primary_key.organization_id,
           m_StrOrganizationIDFld,
           sizeof(sMCFRecord.primary_key.organization_id) );

   memcpy( sMCT01Record.primary_key.card_type,
           sAuthTxRecord.BIN01_details.card_type,
           sizeof(sAuthTxRecord.BIN01_details.card_type) );

   rcode = txdsapi_get_record( (pBYTE)&sMCT01Record,
                               sizeof(MCT01),
                               MCT01_DATA,
                               strErrorMessage );

   if ( PTEMSG_OK != rcode )
   {
      ZeroMemory( temp_err, sizeof(temp_err) );
      sprintf( temp_err,
              "Merchant is not authorized for card type %s",
               sAuthTxRecord.BIN01_details.card_type );

      DisplayErrorMsg( temp_err );
      m_OverrideButton.EnableWindow(FALSE);
      UpdateData(FALSE);
      ret_val = FALSE;
   }
   return( ret_val );
}


void CVoiceAuth::DisplayErrorMsg( pCHAR errstr )
{
   /* Display message in message field at bottom of screen.
    * But also display it in the response text field because
    * the bottom of the screen is unseen at the Equitable
    * workstations due to the sizing of the GUI.
    */
   m_StrMessageLbl = "";
   m_StrResponseTextFld.Empty();

   m_StrMessageLbl      = errstr;
   m_StrResponseTextFld = errstr;

   UpdateData(FALSE);
   return;
}


void CVoiceAuth::checkTLF01_fields( pCHAR errstr )
{
   /* Check all fields for a missing null. */
   CHAR  tempstr[500] = "";

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.primary_key.transaction_id) >=
        sizeof(sAuthTxRecord.TLF01_details.primary_key.transaction_id)  )
   {
      sprintf( tempstr,
              "TLF01.transaction_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.primary_key.transaction_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tpdu_id) >=
        sizeof(sAuthTxRecord.TLF01_details.tpdu_id)  )
   {
      sprintf( tempstr,
              "TLF01.tpdu_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tpdu_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.issuer_id) >=
        sizeof(sAuthTxRecord.TLF01_details.issuer_id)  )
   {
      sprintf( tempstr,
              "TLF01.issuer_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.issuer_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.acquirer_id) >=
        sizeof(sAuthTxRecord.TLF01_details.acquirer_id)  )
   {
      sprintf( tempstr,
              "TLF01.acquirer_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.acquirer_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.terminal_id) >=
        sizeof(sAuthTxRecord.TLF01_details.terminal_id)  )
   {
      sprintf( tempstr,
              "TLF01.terminal_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.terminal_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.terminal_type) >=
        sizeof(sAuthTxRecord.TLF01_details.terminal_type)  )
   {
      sprintf( tempstr,
              "TLF01.terminal_type missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.terminal_type );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.response_text) >=
        sizeof(sAuthTxRecord.TLF01_details.response_text)  )
   {
      sprintf( tempstr,
              "TLF01.response_text missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.response_text );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.card_num_len) >=
        sizeof(sAuthTxRecord.TLF01_details.card_num_len)  )
   {
      sprintf( tempstr,
              "TLF01.card_num_len missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.card_num_len );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.card_num) >=
        sizeof(sAuthTxRecord.TLF01_details.card_num)  )
   {
      sprintf( tempstr,
              "TLF01.card_num missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.card_num );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.exp_date) >=
        sizeof(sAuthTxRecord.TLF01_details.exp_date)  )
   {
      sprintf( tempstr,
              "TLF01.exp_date missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.exp_date );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.total_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.total_amount)  )
   {
      sprintf( tempstr,
              "TLF01.total_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.total_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.add_amounts) >=
        sizeof(sAuthTxRecord.TLF01_details.add_amounts)  )
   {
      sprintf( tempstr,
              "TLF01.add_amounts missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.add_amounts );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.invoice_number) >=
        sizeof(sAuthTxRecord.TLF01_details.invoice_number)  )
   {
      sprintf( tempstr,
              "TLF01.invoice_number missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.invoice_number );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.orig_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.orig_amount)  )
   {
      sprintf( tempstr,
              "TLF01.orig_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.orig_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.auth_number) >=
        sizeof(sAuthTxRecord.TLF01_details.auth_number)  )
   {
      sprintf( tempstr,
              "TLF01.auth_number missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.auth_number );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.processing_code) >=
        sizeof(sAuthTxRecord.TLF01_details.processing_code)  )
   {
      sprintf( tempstr,
              "TLF01.processing_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.processing_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.sys_trace_audit_num) >=
        sizeof(sAuthTxRecord.TLF01_details.sys_trace_audit_num)  )
   {
      sprintf( tempstr,
              "TLF01.sys_trace_audit_num missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.sys_trace_audit_num );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.orig_sys_trace_num) >=
        sizeof(sAuthTxRecord.TLF01_details.orig_sys_trace_num)  )
   {
      sprintf( tempstr,
              "TLF01.orig_sys_trace_num missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.orig_sys_trace_num );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.pos_entry_mode) >=
        sizeof(sAuthTxRecord.TLF01_details.pos_entry_mode)  )
   {
      sprintf( tempstr,
              "TLF01.pos_entry_mode missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.pos_entry_mode );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nii) >=
        sizeof(sAuthTxRecord.TLF01_details.nii)  )
   {
      sprintf( tempstr,
              "TLF01.nii missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nii );
   }

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.pos_condition_code) >=
        sizeof(sAuthTxRecord.TLF01_details.pos_condition_code)  )
   {
      sprintf( tempstr,
              "TLF01.pos_condition_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.pos_condition_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.track1) >=
        sizeof(sAuthTxRecord.TLF01_details.track1)  )
   {
      sprintf( tempstr,
              "TLF01.track1 missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.track1 );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.track2) >=
        sizeof(sAuthTxRecord.TLF01_details.track2)  )
   {
      sprintf( tempstr,
              "TLF01.track2 missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.track2 );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.retrieval_ref_num) >=
        sizeof(sAuthTxRecord.TLF01_details.retrieval_ref_num)  )
   {
      sprintf( tempstr,
              "TLF01.retrieval_ref_num missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.retrieval_ref_num );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.orig_retrieval_ref_num) >=
        sizeof(sAuthTxRecord.TLF01_details.orig_retrieval_ref_num)  )
   {
      sprintf( tempstr,
              "TLF01.orig_retrieval_ref_num missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.orig_retrieval_ref_num );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.merchant_id) >=
        sizeof(sAuthTxRecord.TLF01_details.merchant_id)  )
   {
      sprintf( tempstr,
              "TLF01.merchant_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.merchant_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.orig_message) >=
        sizeof(sAuthTxRecord.TLF01_details.orig_message)  )
   {
      sprintf( tempstr,
              "TLF01.orig_message missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.orig_message );
   }

   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.settlement_total) >=
        sizeof(sAuthTxRecord.TLF01_details.settlement_total)  )
   {
      sprintf( tempstr,
              "TLF01.settlement_total missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.settlement_total );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.message_type) >=
        sizeof(sAuthTxRecord.TLF01_details.message_type)  )
   {
      sprintf( tempstr,
              "TLF01.message_type missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.message_type );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.card_holder_name) >=
        sizeof(sAuthTxRecord.TLF01_details.card_holder_name)  )
   {
      sprintf( tempstr,
              "TLF01.card_holder_name missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.card_holder_name );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.originator_queue) >=
        sizeof(sAuthTxRecord.TLF01_details.originator_queue)  )
   {
      sprintf( tempstr,
              "TLF01.originator_queue missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.originator_queue );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.originator_info) >=
        sizeof(sAuthTxRecord.TLF01_details.originator_info)  )
   {
      sprintf( tempstr,
              "TLF01.originator_info missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.originator_info );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.batch_number) >=
        sizeof(sAuthTxRecord.TLF01_details.batch_number)  )
   {
      sprintf( tempstr,
              "TLF01.batch_number missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.batch_number );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.response_code) >=
        sizeof(sAuthTxRecord.TLF01_details.response_code)  )
   {
      sprintf( tempstr,
              "TLF01.response_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.response_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.date_yyyymmdd) >=
        sizeof(sAuthTxRecord.TLF01_details.date_yyyymmdd)  )
   {
      sprintf( tempstr,
              "TLF01.date_yyyymmdd missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.date_yyyymmdd );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.time_hhmmss) >=
        sizeof(sAuthTxRecord.TLF01_details.time_hhmmss)  )
   {
      sprintf( tempstr,
              "TLF01.time_hhmmss missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.time_hhmmss );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.num_sales) >=
        sizeof(sAuthTxRecord.TLF01_details.num_sales)  )
   {
      sprintf( tempstr,
              "TLF01.num_sales missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.num_sales );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.sales_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.sales_amount)  )
   {
      sprintf( tempstr,
              "TLF01.sales_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.sales_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.num_refunds) >=
        sizeof(sAuthTxRecord.TLF01_details.num_refunds)  )
   {
      sprintf( tempstr,
              "TLF01.num_refunds missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.num_refunds );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.refund_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.refund_amount)  )
   {
      sprintf( tempstr,
              "TLF01.refund_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.refund_amount );
   }

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.num_dbsales) >=
        sizeof(sAuthTxRecord.TLF01_details.num_dbsales)  )
   {
      sprintf( tempstr,
              "TLF01.num_dbsales missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.num_dbsales );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dbsales_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.dbsales_amount)  )
   {
      sprintf( tempstr,
              "TLF01.dbsales_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dbsales_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.num_dbrefunds) >=
        sizeof(sAuthTxRecord.TLF01_details.num_dbrefunds)  )
   {
      sprintf( tempstr,
              "TLF01.num_dbrefunds missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.num_dbrefunds );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dbrefund_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.dbrefund_amount)  )
   {
      sprintf( tempstr,
              "TLF01.dbrefund_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dbrefund_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.pin_block) >=
        sizeof(sAuthTxRecord.TLF01_details.pin_block)  )
   {
      sprintf( tempstr,
              "TLF01.pin_block missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.pin_block );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tax_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.tax_amount)  )
   {
      sprintf( tempstr,
              "TLF01.tax_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tax_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tip_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.tip_amount)  )
   {
      sprintf( tempstr,
              "TLF01.tip_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tip_amount );
   }

   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tran_start_time) >=
        sizeof(sAuthTxRecord.TLF01_details.tran_start_time)  )
   {
      sprintf( tempstr,
              "TLF01.tran_start_time missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tran_start_time );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.host_start_time) >=
        sizeof(sAuthTxRecord.TLF01_details.host_start_time)  )
   {
      sprintf( tempstr,
              "TLF01.host_start_time missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.host_start_time );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.host_finish_time) >=
        sizeof(sAuthTxRecord.TLF01_details.host_finish_time)  )
   {
      sprintf( tempstr,
              "TLF01.host_finish_time missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.host_finish_time );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tran_finish_time) >=
        sizeof(sAuthTxRecord.TLF01_details.tran_finish_time)  )
   {
      sprintf( tempstr,
              "TLF01.tran_finish_time missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tran_finish_time );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.settle_file_prefix) >=
        sizeof(sAuthTxRecord.TLF01_details.settle_file_prefix)  )
   {
      sprintf( tempstr,
              "TLF01.settle_file_prefix missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.settle_file_prefix );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.resp_source_len) >=
        sizeof(sAuthTxRecord.TLF01_details.resp_source_len)  )
   {
      sprintf( tempstr,
              "TLF01.resp_source_len missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.resp_source_len );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.visa_tran_id) >=
        sizeof(sAuthTxRecord.TLF01_details.visa_tran_id)  )
   {
      sprintf( tempstr,
              "TLF01.visa_tran_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.visa_tran_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.visa_validation_code) >=
        sizeof(sAuthTxRecord.TLF01_details.visa_validation_code)  )
   {
      sprintf( tempstr,
              "TLF01.visa_validation_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.visa_validation_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.mcard_banknet_len) >=
        sizeof(sAuthTxRecord.TLF01_details.mcard_banknet_len)  )
   {
      sprintf( tempstr,
              "TLF01.mcard_banknet_len missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.mcard_banknet_len );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.mcard_banknet) >=
        sizeof(sAuthTxRecord.TLF01_details.mcard_banknet)  )
   {
      sprintf( tempstr,
              "TLF01.mcard_banknet missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.mcard_banknet );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.agent_id_len) >=
        sizeof(sAuthTxRecord.TLF01_details.agent_id_len)  )
   {
      sprintf( tempstr,
              "TLF01.agent_id_len missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.agent_id_len );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.agent_id) >=
        sizeof(sAuthTxRecord.TLF01_details.agent_id)  )
   {
      sprintf( tempstr,
              "TLF01.agent_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.agent_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.settlement_date) >=
        sizeof(sAuthTxRecord.TLF01_details.settlement_date)  )
   {
      sprintf( tempstr,
              "TLF01.settlement_date missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.settlement_date );
   }

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.category_code) >=
        sizeof(sAuthTxRecord.TLF01_details.category_code)  )
   {
      sprintf( tempstr,
              "TLF01.category_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.category_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.conversion_rate) >=
        sizeof(sAuthTxRecord.TLF01_details.conversion_rate)  )
   {
      sprintf( tempstr,
              "TLF01.conversion_rate missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.conversion_rate );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_code)  )
   {
      sprintf( tempstr,
              "TLF01.product_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.odometer) >=
        sizeof(sAuthTxRecord.TLF01_details.odometer)  )
   {
      sprintf( tempstr,
              "TLF01.odometer missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.odometer );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.organization_id) >=
        sizeof(sAuthTxRecord.TLF01_details.organization_id)  )
   {
      sprintf( tempstr,
              "TLF01.organization_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.organization_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.source_key) >=
        sizeof(sAuthTxRecord.TLF01_details.source_key)  )
   {
      sprintf( tempstr,
              "TLF01.source_key missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.source_key );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.txn_cnt) >=
        sizeof(sAuthTxRecord.TLF01_details.txn_cnt)  )
   {
      sprintf( tempstr,
              "TLF01.txn_cnt missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.txn_cnt );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dcf01_retired_cwk) >=
        sizeof(sAuthTxRecord.TLF01_details.dcf01_retired_cwk)  )
   {
      sprintf( tempstr,
              "TLF01.dcf01_retired_cwk missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dcf01_retired_cwk );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dest_key) >=
        sizeof(sAuthTxRecord.TLF01_details.dest_key)  )
   {
      sprintf( tempstr,
              "TLF01.dest_key missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dest_key );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.ncf01_retired_cwk) >=
        sizeof(sAuthTxRecord.TLF01_details.ncf01_retired_cwk)  )
   {
      sprintf( tempstr,
              "TLF01.ncf01_retired_cwk missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.ncf01_retired_cwk );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.handler_queue) >=
        sizeof(sAuthTxRecord.TLF01_details.handler_queue)  )
   {
      sprintf( tempstr,
              "TLF01.handler_queue missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.handler_queue );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.authorizing_host_queue) >=
        sizeof(sAuthTxRecord.TLF01_details.authorizing_host_queue)  )
   {
      sprintf( tempstr,
              "TLF01.authorizing_host_queue missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.authorizing_host_queue );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.max_net_consec_tmouts) >=
        sizeof(sAuthTxRecord.TLF01_details.max_net_consec_tmouts)  )
   {
      sprintf( tempstr,
              "TLF01.max_net_consec_tmouts missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.max_net_consec_tmouts );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.max_active_txns) >=
        sizeof(sAuthTxRecord.TLF01_details.max_active_txns)  )
   {
      sprintf( tempstr,
              "TLF01.max_active_txns missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.max_active_txns );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.forwarding_institution_id) >=
        sizeof(sAuthTxRecord.TLF01_details.forwarding_institution_id)  )
   {
      sprintf( tempstr,
              "TLF01.forwarding_institution_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.forwarding_institution_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.remote_nsp_type) >=
        sizeof(sAuthTxRecord.TLF01_details.remote_nsp_type)  )
   {
      sprintf( tempstr,
              "TLF01.remote_nsp_type missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.remote_nsp_type );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.kek) >=
        sizeof(sAuthTxRecord.TLF01_details.kek)  )
   {
      sprintf( tempstr,
              "TLF01.kek missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.kek );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.acquiring_id) >=
        sizeof(sAuthTxRecord.TLF01_details.acquiring_id)  )
   {
      sprintf( tempstr,
              "TLF01.acquiring_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.acquiring_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.outstanding_balance) >=
        sizeof(sAuthTxRecord.TLF01_details.outstanding_balance)  )
   {
      sprintf( tempstr,
              "TLF01.outstanding_balance missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.outstanding_balance );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.credit_limit) >=
        sizeof(sAuthTxRecord.TLF01_details.credit_limit)  )
   {
      sprintf( tempstr,
              "TLF01.credit_limit missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.credit_limit );
   }

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nfi_name) >=
        sizeof(sAuthTxRecord.TLF01_details.nfi_name)  )
   {
      sprintf( tempstr,
              "TLF01.nfi_name missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nfi_name );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nfi_seq_nbr) >=
        sizeof(sAuthTxRecord.TLF01_details.nfi_seq_nbr)  )
   {
      sprintf( tempstr,
              "TLF01.nfi_seq_nbr missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nfi_seq_nbr );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nfi_start_date) >=
        sizeof(sAuthTxRecord.TLF01_details.nfi_start_date)  )
   {
      sprintf( tempstr,
              "TLF01.nfi_start_date missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nfi_start_date );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nfi_end_date) >=
        sizeof(sAuthTxRecord.TLF01_details.nfi_end_date)  )
   {
      sprintf( tempstr,
              "TLF01.nfi_end_date missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nfi_end_date );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.resp_source) >=
        sizeof(sAuthTxRecord.TLF01_details.resp_source)  )
   {
      sprintf( tempstr,
              "TLF01.resp_source missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.resp_source );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dispensation.origin) >=
        sizeof(sAuthTxRecord.TLF01_details.dispensation.origin)  )
   {
      sprintf( tempstr,
              "TLF01.dispensation.origin missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dispensation.origin );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dispensation.auth_1) >=
        sizeof(sAuthTxRecord.TLF01_details.dispensation.auth_1)  )
   {
      sprintf( tempstr,
              "TLF01.dispensation.auth_1 missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dispensation.auth_1 );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dispensation.auth_2) >=
        sizeof(sAuthTxRecord.TLF01_details.dispensation.auth_2)  )
   {
      sprintf( tempstr,
              "TLF01.dispensation.auth_2 missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dispensation.auth_2 );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.update_mask) >=
        sizeof(sAuthTxRecord.TLF01_details.update_mask)  )
   {
      sprintf( tempstr,
              "TLF01.update_mask missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.update_mask );
   }

   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.type_of_data) >=
        sizeof(sAuthTxRecord.TLF01_details.type_of_data)  )
   {
      sprintf( tempstr,
              "TLF01.type_of_data missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.type_of_data );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.down_payment) >=
        sizeof(sAuthTxRecord.TLF01_details.down_payment)  )
   {
      sprintf( tempstr,
              "TLF01.down_payment missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.down_payment );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.period) >=
        sizeof(sAuthTxRecord.TLF01_details.period)  )
   {
      sprintf( tempstr,
              "TLF01.period missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.period );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.interest_rate) >=
        sizeof(sAuthTxRecord.TLF01_details.interest_rate)  )
   {
      sprintf( tempstr,
              "TLF01.interest_rate missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.interest_rate );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.total_interest) >=
        sizeof(sAuthTxRecord.TLF01_details.total_interest)  )
   {
      sprintf( tempstr,
              "TLF01.total_interest missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.total_interest );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.redemption_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.redemption_amount)  )
   {
      sprintf( tempstr,
              "TLF01.redemption_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.redemption_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.vehicle_number) >=
        sizeof(sAuthTxRecord.TLF01_details.vehicle_number)  )
   {
      sprintf( tempstr,
              "TLF01.vehicle_number missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.vehicle_number );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.profile) >=
        sizeof(sAuthTxRecord.TLF01_details.profile)  )
   {
      sprintf( tempstr,
              "TLF01.profile missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.profile );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.nbr_of_prod_codes) >=
        sizeof(sAuthTxRecord.TLF01_details.nbr_of_prod_codes)  )
   {
      sprintf( tempstr,
              "TLF01.nbr_of_prod_codes missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.nbr_of_prod_codes );
   }

   /*---------------------------------------------------------------------------*/
   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[0].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[0].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[0].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[0].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[0].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[0].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[0].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[0].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[0].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[0].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[0].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[0].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[1].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[1].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[1].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[1].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[1].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[1].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[1].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[1].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[1].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[1].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[1].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[1].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[2].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[2].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[2].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[2].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[2].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[2].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[2].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[2].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[2].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[2].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[2].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[2].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[3].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[3].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[3].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[3].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[3].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[3].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[3].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[3].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[3].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[3].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[3].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[3].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[4].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[4].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[4].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[4].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[4].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[4].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[4].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[4].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[4].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[4].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[4].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[4].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[5].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[5].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[5].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[5].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[5].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[5].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[5].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[5].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[5].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[5].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[5].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[5].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[6].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[6].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[6].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[6].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[6].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[6].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[6].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[6].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[6].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[6].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[6].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[6].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[7].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[7].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[7].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[7].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[7].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[7].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[7].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[7].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[7].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[7].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[7].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[7].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[8].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[8].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[8].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[8].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[8].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[8].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[8].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[8].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[8].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[8].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[8].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[8].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[9].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[9].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[9].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[9].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[9].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[9].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[9].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[9].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[9].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[9].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[9].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[9].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[10].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[10].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[10].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[10].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[10].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[10].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[10].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[10].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[10].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[10].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[10].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[10].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[11].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[11].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[11].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[11].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[11].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[11].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[11].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[11].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[11].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[11].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[11].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[11].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[12].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[12].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[12].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[12].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[12].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[12].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[12].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[12].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[12].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[12].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[12].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[12].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[13].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[13].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[13].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[13].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[13].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[13].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[13].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[13].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[13].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[13].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[13].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[13].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[14].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[14].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[14].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[14].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[14].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[14].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[14].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[14].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[14].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[14].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[14].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[14].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[15].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[15].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[15].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[15].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[15].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[15].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[15].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[15].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[15].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[15].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[15].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[15].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[16].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[16].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[16].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[16].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[16].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[16].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[16].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[16].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[16].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[16].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[16].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[16].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[17].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[17].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[17].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[17].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[17].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[17].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[17].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[17].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[17].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[17].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[17].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[17].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[18].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[18].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[18].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[18].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[18].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[18].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[18].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[18].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[18].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[18].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[18].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[18].amount );
   }
   /*---------------------------------------------------------------------------*/
   /*---------------------------------------------------------------------------*/
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[19].code) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[19].code)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[19].code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[19].code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[19].quantity) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[19].quantity)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[19].quantity missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[19].quantity );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.product_codes[19].amount) >=
        sizeof(sAuthTxRecord.TLF01_details.product_codes[19].amount)  )
   {
      sprintf( tempstr,
              "TLF01.product_codes[19].amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.product_codes[19].amount );
   }
   /*---------------------------------------------------------------------------*/

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.visa_merchant_id) >=
        sizeof(sAuthTxRecord.TLF01_details.visa_merchant_id)  )
   {
      sprintf( tempstr,
              "TLF01.visa_merchant_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.visa_merchant_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.cash_bonus) >=
        sizeof(sAuthTxRecord.TLF01_details.cash_bonus)  )
   {
      sprintf( tempstr,
              "TLF01.cash_bonus missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.cash_bonus );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.deferred_factor) >=
        sizeof(sAuthTxRecord.TLF01_details.deferred_factor)  )
   {
      sprintf( tempstr,
              "TLF01.deferred_factor missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.deferred_factor );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.deferred_term_length) >=
        sizeof(sAuthTxRecord.TLF01_details.deferred_term_length)  )
   {
      sprintf( tempstr,
              "TLF01.deferred_term_length missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.deferred_term_length );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.currency_code) >=
        sizeof(sAuthTxRecord.TLF01_details.currency_code)  )
   {
      sprintf( tempstr,
              "TLF01.currency_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.currency_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.transmission_timestamp) >=
        sizeof(sAuthTxRecord.TLF01_details.transmission_timestamp)  )
   {
      sprintf( tempstr,
              "TLF01.transmission_timestamp missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.transmission_timestamp );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.tran_fee_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.tran_fee_amount)  )
   {
      sprintf( tempstr,
              "TLF01.tran_fee_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.tran_fee_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.cvc_data) >=
        sizeof(sAuthTxRecord.TLF01_details.cvc_data)  )
   {
      sprintf( tempstr,
              "TLF01.cvc_data missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.cvc_data );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.verify_mag_stripe) >=
        sizeof(sAuthTxRecord.TLF01_details.verify_mag_stripe)  )
   {
      sprintf( tempstr,
              "TLF01.verify_mag_stripe missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.verify_mag_stripe );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.saf) >=
        sizeof(sAuthTxRecord.TLF01_details.saf)  )
   {
      sprintf( tempstr,
              "TLF01.saf missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.saf );
   }

   if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.security_response_code) >=
        sizeof(sAuthTxRecord.TLF01_details.security_response_code)  )
   {
      sprintf( tempstr,
              "TLF01.security_response_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.security_response_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.service_code) >=
        sizeof(sAuthTxRecord.TLF01_details.service_code)  )
   {
      sprintf( tempstr,
              "TLF01.service_code missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.service_code );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.source_pin_format) >=
        sizeof(sAuthTxRecord.TLF01_details.source_pin_format)  )
   {
      sprintf( tempstr,
              "TLF01.source_pin_format missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.source_pin_format );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.dest_pin_format) >=
        sizeof(sAuthTxRecord.TLF01_details.dest_pin_format)  )
   {
      sprintf( tempstr,
              "TLF01.dest_pin_format missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.dest_pin_format );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.voice_auth) >=
        sizeof(sAuthTxRecord.TLF01_details.voice_auth)  )
   {
      sprintf( tempstr,
              "TLF01.voice_auth missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.voice_auth );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.actual_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.actual_amount)  )
   {
      sprintf( tempstr,
              "TLF01.actual_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.actual_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.reversal_amount) >=
        sizeof(sAuthTxRecord.TLF01_details.reversal_amount)  )
   {
      sprintf( tempstr,
              "TLF01.reversal_amount missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.reversal_amount );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.voice_auth_text) >=
        sizeof(sAuthTxRecord.TLF01_details.voice_auth_text)  )
   {
      sprintf( tempstr,
              "TLF01.voice_auth_text missing trailing null. Field Length: %d, Value: %s",
               strlen((pCHAR)sAuthTxRecord.TLF01_details.voice_auth_text),
               sAuthTxRecord.TLF01_details.voice_auth_text );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.operator_id) >=
        sizeof(sAuthTxRecord.TLF01_details.operator_id)  )
   {
      sprintf( tempstr,
              "TLF01.operator_id missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.operator_id );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.workstation) >=
        sizeof(sAuthTxRecord.TLF01_details.workstation)  )
   {
      sprintf( tempstr,
              "TLF01.workstation missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.workstation );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.orig_auth_number) >=
        sizeof(sAuthTxRecord.TLF01_details.orig_auth_number)  )
   {
      sprintf( tempstr,
              "TLF01.orig_auth_number missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.orig_auth_number );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.ticket_nbr) >=
        sizeof(sAuthTxRecord.TLF01_details.ticket_nbr)  )
   {
      sprintf( tempstr,
              "TLF01.ticket_nbr missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.ticket_nbr );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.system_date) >=
        sizeof(sAuthTxRecord.TLF01_details.system_date)  )
   {
      sprintf( tempstr,
              "TLF01.system_date missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.system_date );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.def_gross_amt) >=
        sizeof(sAuthTxRecord.TLF01_details.def_gross_amt)  )
   {
      sprintf( tempstr,
              "TLF01.def_gross_amt missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.def_gross_amt );
   }
   else if ( strlen((pCHAR)sAuthTxRecord.TLF01_details.monthly_amort_amt) >=
        sizeof(sAuthTxRecord.TLF01_details.monthly_amort_amt)  )
   {
      sprintf( tempstr,
              "TLF01.monthly_amort_amt missing trailing null. Value: %s",
               sAuthTxRecord.TLF01_details.monthly_amort_amt );
   }

   /* Log AUTH_TX to file */
   log_auth_tx();

   /* Tell User which field failed. */
   if ( tempstr[0] != 0x00 )
   {
      AfxMessageBox( tempstr, MB_ICONSTOP | MB_OK );
   }
   else
   {
      AfxMessageBox( "Problem is not in TLF01", MB_ICONSTOP | MB_OK );
   }
   return;
}

void CVoiceAuth::log_auth_tx()
{
   INT      i,j;
   INT      len;
   FILE    *fp = NULL;
   CHAR     log_filename[200]="";
   BYTE     buffer[sizeof(AUTH_TX)];
   CHAR     datetime[25] = "";

   GetAscendentLogDirectory( log_filename );
   strcat( log_filename, "VoiceAuth.log" );

   if ( NULL != (fp = fopen( log_filename, "a+" )) )
   {
      /* Get current date/time. */
      ptetime_get_timestamp( datetime );
      fprintf( fp, "%s\n-----------------------\n\n", datetime );

      len = sizeof(AUTH_TX);
      memcpy( buffer, &sAuthTxRecord, len );
      for ( i=0; i<len; i+=16 )
      {
         fprintf( fp,
                 "%02x %02x %02x %02x %02x %02x %02x %02x - "
                 "%02x %02x %02x %02x %02x %02x %02x %02x\n",
                  buffer[i],
                  buffer[i+1],
                  buffer[i+2],
                  buffer[i+3],
                  buffer[i+4],
                  buffer[i+5],
                  buffer[i+6],
                  buffer[i+7],
                  buffer[i+8],
                  buffer[i+9],
                  buffer[i+10],
                  buffer[i+11],
                  buffer[i+12],
                  buffer[i+13],
                  buffer[i+14],
                  buffer[i+15] );
      }

      fprintf( fp, "\n" );
      for ( j=0; j<3; j++ )
      {
         for ( i=0; i<50; i++ )
         {
            fprintf( fp, "-" );
         }
         fprintf( fp, "\n" );
      }
      fprintf( fp, "\n\n\n" );
      fclose( fp );
   }
   return;
}

void CVoiceAuth::asc_to_bin( pCHAR astr, pBYTE bstr, pINT bstr_len )
{
   INT  i,j;
   INT  len;

   len = strlen( astr );
   *bstr_len = 0;
   for( i=0,j=0; i<len; i+=2,j++ )
   {
      /* Upper nibble */
      if ( astr[i] <= '9' )
         bstr[j] = (astr[i] & 0x0f) * 16;
      else
         bstr[j] = ((astr[i] & 0x0f) + 9) * 16;

      /* Lower nibble */
      if ( astr[i+1] <= '9' )
         bstr[j] |= (astr[i+1] & 0x0f);
      else
         bstr[j] |= ((astr[i+1] & 0x0f) + 9);

      (*bstr_len)++;
   }
   return;
}



void CVoiceAuth::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}


LRESULT CVoiceAuth::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
    {	

     case WM_TIMER:
		{
			cntr++;
			
			shm_time = shrmem.GetLastUpdateTime();

			if(shm_time > c_time)
			{
				c_time = shm_time;
				ResetCntr();
			}
			if ( cntr == idle_time/2 )
			{
				shrmem.SetIdleStatus(0);
			}
			//Check whether Max Idle Time has elapsed
			if(cntr >= idle_time)
			{
				ret = LockScreen();

				if(ret == -1 || ret == -2)
				{				
					::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
				}
			}
		}
		break;
	
		//On X button press of Dialog
		case WM_CLOSE:				
				
				StopTimer();
				//Delete the SHM segment & Delete Mutex Lock
				shrmem.DestroySharedMemorySegment();
				shrmem.DeleteMutexLock();
				
				break;

	 default:break;
			
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}


BOOL CVoiceAuth::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	ResetCntr();

	c_time = CTime::GetCurrentTime();
	shrmem.SetLastUpdateTime(c_time);

	return CDialog::OnCommand(wParam, lParam);
}


int CVoiceAuth::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CVoiceAuth::StopTimer()
{
	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				//::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}


void CVoiceAuth::ResetCntr()
{
	cntr = 0;
}

short CVoiceAuth::LockScreen()
{
	int retcode = 0;

	CIdleLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			//AfxMessageBox("Session Timeout. Please Login");
			::MessageBox(this->m_hWnd,"Session Timeout. Please Login","VoiceAuthorization",MB_OK);
			
			ret = idleloginfrm.CheckLoginDetails();

			if(ret == CANCEL)
			{
				//Update SHM with IdleStatus as 2 meaning to Close all the Apps
				shrmem.SetIdleStatus(2);
				
				retcode = -1;			
			}
			else
			{				
				shrmem.SetIdleStatus(1);
				retcode =  1;
			}
		}
		else if (status == 1)
		{
				retcode = 0;
			// Continue. Password check was successful.
		}
		else if (status == 2)
		{
			retcode =  -1;			
		}
		else
		{
			retcode = -2;
			LogEvent("Unkown SHM Status",ERROR_MSG);
			// TODO:  Error case - log a message. Status is out of sync.
		}
		//After Login Validation is successful, release the Mutex Lock
		pLock->Unlock();
	}

	StartTimer(1000);

	return retcode;
	
}


