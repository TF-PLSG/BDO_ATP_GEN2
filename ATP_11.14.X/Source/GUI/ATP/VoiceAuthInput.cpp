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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\VoiceAuthInput.cpp  $
   
      Rev 1.11   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:02:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:30   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:40   iarustam
   Initial Release
   
      Rev 1.7   May 10 1999 13:40:54   dperson
   Updated comments
   
      Rev 1.6   May 10 1999 11:41:26   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// VoiceAuthInput.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "VoiceAuthInput.h"
#include "MyUtils.h"
#include "VoiceAuthResponse.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthInput dialog


CVoiceAuthInput::CVoiceAuthInput(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiceAuthInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiceAuthInput)
	m_StrAccountNumberFld = _T("");
	m_StrAmountFld = _T("");
	m_StrAuthNumberFld = _T("");
	m_StrDeferredPurchaseTermFld = _T("");
	m_StrExpiryDateFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrTransactionType = _T("");
	m_StrCVV2Fld = _T("");
	//}}AFX_DATA_INIT
}


void CVoiceAuthInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceAuthInput)
	DDX_Control(pDX, IDC_CVV2_FLD, m_CntlCVV2Fld);
	DDX_Control(pDX, IDC_RESPONSE_CODE, m_CntlResponseCodeFld);
	DDX_Control(pDX, IDC_AUTH_NUMBER_FLD, m_CntlAuthNumberFld);
	DDX_Control(pDX, IDC_DEFERRED_PURCHASE_TERM_FLD, m_CntlDeferredPurchaseTermFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_EXPIRY_DATE_FLD, m_CntlExpiryDateFld);
	DDX_Control(pDX, IDC_AMOUNT, m_CntlAmountFld);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_CntlAccountNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDV_MaxChars(pDX, m_StrAccountNumberFld, 19);
	DDX_Text(pDX, IDC_AMOUNT, m_StrAmountFld);
	DDV_MaxChars(pDX, m_StrAmountFld, 13);
	DDX_Text(pDX, IDC_AUTH_NUMBER_FLD, m_StrAuthNumberFld);
	DDV_MaxChars(pDX, m_StrAuthNumberFld, 6);
	DDX_Text(pDX, IDC_DEFERRED_PURCHASE_TERM_FLD, m_StrDeferredPurchaseTermFld);
	DDV_MaxChars(pDX, m_StrDeferredPurchaseTermFld, 3);
	DDX_Text(pDX, IDC_EXPIRY_DATE_FLD, m_StrExpiryDateFld);
	DDV_MaxChars(pDX, m_StrExpiryDateFld, 4);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_RESPONSE_CODE, m_StrResponseCodeFld);
	DDV_MaxChars(pDX, m_StrResponseCodeFld, 2);
	DDX_CBString(pDX, IDC_TRANSACTION_TYPE, m_StrTransactionType);
	DDX_Text(pDX, IDC_CVV2_FLD, m_StrCVV2Fld);
	DDV_MaxChars(pDX, m_StrCVV2Fld, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVoiceAuthInput, CDialog)
	//{{AFX_MSG_MAP(CVoiceAuthInput)
	ON_BN_CLICKED(IDC_PROCESS_AUTH_BUTTON, OnProcessAuthButton)
	ON_CBN_SELCHANGE(IDC_TRANSACTION_TYPE, OnSelchangeTransactionType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthInput message handlers

BOOL CVoiceAuthInput::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrTransactionType = "Sale";
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVoiceAuthInput::OnProcessAuthButton() 
{
	CVoiceAuthResponse responseDlg;
	char  strTxKey;
	CString strAmount;
	char strErrorMessage[200];
	CMyUtils myUtilsObject;

	if ( !CheckDialogForCompleteness())
		return;

	if ( !ValidateDate ( m_StrExpiryDateFld ) )
		return;

	int nLength = m_StrAccountNumberFld.GetLength();
	char strLen[5];
	itoa ( nLength, strLen, 10 );

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrAmountFld, 12, strErrorMessage );
	if ( strAmount == m_StrAmountFld )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		m_CntlAmountFld.SetFocus();
		return;
	}


	if ( m_StrTransactionType == "Sale" || m_StrTransactionType == "Quasi Cash" )
		strTxKey = AUTH_AUTHORIZATION;
	else if ( m_StrTransactionType == "Cash Advance" )
		strTxKey = AUTH_CASH_ADVANCE_AUTHORIZATION;
	else if ( m_StrTransactionType == "Deferred Purchase" )
		strTxKey = AUTH_DEFERRED_PURCHASE_AUTHORIZATION;


	ZeroMemory ( &responseDlg.sAuthTxRecord, sizeof ( AUTH_TX ) );

	// Populate TLF01 Table	
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.merchant_id, m_StrMerchantIDFld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.merchant_id));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.card_num,    m_StrAccountNumberFld, sizeof (responseDlg.sAuthTxRecord.TLF01_details.card_num));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.exp_date,    m_StrExpiryDateFld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.exp_date));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.total_amount, strAmount,	     sizeof (responseDlg.sAuthTxRecord.TLF01_details.total_amount));
	responseDlg.sAuthTxRecord.TLF01_details.tx_key = strTxKey;
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.deferred_term_length, m_StrDeferredPurchaseTermFld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.deferred_term_length));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.response_code, m_StrResponseCodeFld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.response_code));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.auth_number, m_StrAuthNumberFld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.auth_number));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.card_num_len, strLen,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.card_num_len));
	memcpy ( responseDlg.sAuthTxRecord.TLF01_details.cvc_data, m_StrCVV2Fld,    sizeof (responseDlg.sAuthTxRecord.TLF01_details.cvc_data));

	if ( MODE_OVERRIDE == m_nDialogMode )
		responseDlg.m_nDialogMode = MODE_OVERRIDE;

	responseDlg.DoModal();

	if ( responseDlg.m_nDialogMode == MODE_OVERRIDE )
	{
		m_CntlAuthNumberFld.EnableWindow (TRUE); 
		m_CntlResponseCodeFld.EnableWindow(TRUE);
		m_CntlResponseCodeFld.SetFocus();
		m_nDialogMode = MODE_OVERRIDE;
	}
	else
	{
		m_CntlAuthNumberFld.EnableWindow (FALSE); 
		m_CntlResponseCodeFld.EnableWindow(FALSE);
		if ( responseDlg.m_StrResponseCodeFld == "00" )
		{
			m_StrAccountNumberFld = _T("");
			m_StrAmountFld = _T("");
			m_StrAuthNumberFld = _T("");
			m_StrDeferredPurchaseTermFld = _T("");
			m_StrExpiryDateFld = _T("");
			m_StrMerchantIDFld = _T("");
			m_StrMessageLbl = _T("");
			m_StrResponseCodeFld = _T("");
			m_StrTransactionType = _T("");
			m_StrCVV2Fld = "";
			m_CntlMerchantIDFld.SetFocus();
		}

		m_nDialogMode = MODE_DONE;

		UpdateData(FALSE);

	}


	
}

BOOL CVoiceAuthInput::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty (m_StrMerchantIDFld, this ) )
	{
		m_StrMessageLbl = "Please enter Merchant ID";
		UpdateData(FALSE);
		m_CntlMerchantIDFld.SetFocus();
		return FALSE;
	}

	if ( myUtilsObject.IsEmpty (m_StrAccountNumberFld, this )) 
	{
		m_StrMessageLbl = "Please enter Account Number";
		UpdateData(FALSE);
		m_CntlAccountNumberFld.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrExpiryDateFld, this ) )
	{
		m_StrMessageLbl = "Please enter Expiration Date";
		UpdateData(FALSE);
		m_CntlExpiryDateFld.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrAmountFld, this ) )
	{
		m_StrMessageLbl = "Please enter Amount";
		UpdateData(FALSE);
		m_CntlAmountFld.SetFocus();
		return FALSE;
	}

	if ( myUtilsObject.IsEmpty (m_StrDeferredPurchaseTermFld, this ) 
								&& m_StrTransactionType == "Deferred Purchase")
	{
		m_StrMessageLbl = "Please enter Deferred Purchase Term";
		UpdateData(FALSE);
		m_CntlDeferredPurchaseTermFld.SetFocus();
		return FALSE;
	}

	return TRUE;
}

BOOL CVoiceAuthInput::ValidateDate ( CString strDate )
{
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	

	if ( length < 4 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strYear = strDate.Left(2);
	strMonth = strDate.Right(2);

	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );


	if ( nMonth > 12 || nMonth == 0)
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}
	
	return TRUE;
}
void CVoiceAuthInput::OnSelchangeTransactionType() 
{
	UpdateData(TRUE);

	if ( m_StrTransactionType == "Deferred Purchase" )
		m_CntlDeferredPurchaseTermFld.EnableWindow(TRUE);
	else
		m_CntlDeferredPurchaseTermFld.EnableWindow(FALSE);
	
}

BOOL CVoiceAuthInput::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
