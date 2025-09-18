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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\VoiceAuthResponse.cpp  $
   
      Rev 1.11   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Dec 16 1999 09:34:06   mbysani
   corrected some error based on error reports
   
      Rev 1.2   Sep 30 1999 16:17:56   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 16:02:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:40   iarustam
   Initial Release
   
      Rev 1.7   Jun 16 1999 10:03:28   iarustam
   Bug fixes
   
      Rev 1.6   May 10 1999 11:41:54   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// VoiceAuthResponse.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "VoiceAuthResponse.h"
#include "SharedMem.h"	
extern "C"
{
   #include "PteTime.h"
   #include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthResponse dialog


CVoiceAuthResponse::CVoiceAuthResponse(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiceAuthResponse::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiceAuthResponse)
	m_StrAuthNumFld = _T("");
	m_StrBankNameFld = _T("");
	m_StrCardholderNameFld = _T("");
	m_StrGrossAmountFld = _T("");
	m_StrMerchantNameFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMonthlyAmountFld = _T("");
	m_StrResponseCodeFld = _T("");
	m_StrResponseTextFld = _T("");
	m_StrTextFld = _T("");
	//}}AFX_DATA_INIT
}


void CVoiceAuthResponse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceAuthResponse)
	DDX_Text(pDX, IDC_AUTH_NUM_FLD, m_StrAuthNumFld);
	DDX_Text(pDX, IDC_BANK_NAME_FLD, m_StrBankNameFld);
	DDX_Text(pDX, IDC_CARDHOLDER_NAME_FLD, m_StrCardholderNameFld);
	DDX_Text(pDX, IDC_GROSS_AMOUNT_FLD, m_StrGrossAmountFld);
	DDX_Text(pDX, IDC_MERCHANT_NAME_FLD, m_StrMerchantNameFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_MONTHLY_AMORTIZATION_AMOUNT_FLD, m_StrMonthlyAmountFld);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD, m_StrResponseCodeFld);
	DDX_Text(pDX, IDC_RESPONSE_TEXT_FLD, m_StrResponseTextFld);
	DDX_Text(pDX, IDC_TEXT_FLD, m_StrTextFld);
	DDV_MaxChars(pDX, m_StrTextFld, 240);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVoiceAuthResponse, CDialog)
	//{{AFX_MSG_MAP(CVoiceAuthResponse)
	ON_BN_CLICKED(IDC_OVERRIDE_DENIAL_BUTTON, OnOverrideDenialButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthResponse message handlers

BOOL CVoiceAuthResponse::OnInitDialog() 
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

	m_bOverride = FALSE;
	m_bTransaction = FALSE;

	SendVoiceAuthTransaction();

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVoiceAuthResponse::SendVoiceAuthTransaction()
{
	char strErrorMessage[200];

	if (!GetMCFandDCFStructures())
		return;
	PupulateTLFStructure();  

	m_bTransaction = TRUE;

	if ( m_nDialogMode != MODE_OVERRIDE )
	{
		int rcode = txdsapi_send_voice_auth_tran ( &sAuthTxRecord, AUTH_TX_DATA, strErrorMessage);
		if( PTEMSG_OK != rcode )
		{
		  m_StrMessageLbl = strErrorMessage;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
		else if ( PTEMSG_OK == rcode )
		{
			PopulateDialog();

		} // else
	} // if
	else 
	{
		PopulateDialog();
	/*	int rcode = txdsapi_insert_record ( (pBYTE)&sAuthTxRecord, sizeof (TLF01), TLF01_DATA, strErrorMessage );
		if( PTEMSG_OK != rcode )
		{
		  m_StrMessageLbl = strErrorMessage;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		} // if*/

	} // else


}


BOOL CVoiceAuthResponse::GetMCFandDCFStructures()
{
	char strDeviceID[20];
	char strErrorMessage[200];
	DEF01 sDEFRecord;

	ZeroMemory ( &sMCFRecord, sizeof ( MCF01) );
	ZeroMemory ( &sDCFRecord, sizeof ( DCF01) );
	ZeroMemory ( &sCCFRecord, sizeof ( CCF01) );

	if ( sAuthTxRecord.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION )
		GetPrivateField2(strDeviceID);
	else
		GetPrivateField1(strDeviceID);

	memcpy ( sDCFRecord.primary_key.device_id, strDeviceID, sizeof (sDCFRecord.primary_key.device_id));
	memcpy ( sAuthTxRecord.TLF01_details.terminal_id, strDeviceID, sizeof (sAuthTxRecord.TLF01_details.terminal_id));

	int rcode = txdsapi_get_record( (pBYTE)&sDCFRecord, sizeof (DCF01), DCF01_DATA, strErrorMessage );
	if( PTEMSG_NOT_FOUND == rcode )
    {
      m_StrMessageLbl = "Invalid Device ID";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return FALSE;
    }
	else if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	if (sAuthTxRecord.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION)
	{
		memcpy ( sDEFRecord.primary_key.organization_id, "0000", sizeof (sDEFRecord.primary_key.organization_id));
		memcpy ( sDEFRecord.primary_key.merchant_id, sAuthTxRecord.TLF01_details.merchant_id, sizeof (sDEFRecord.primary_key.merchant_id));
		memcpy ( sDEFRecord.primary_key.term_length, sAuthTxRecord.TLF01_details.deferred_term_length, sizeof (sDEFRecord.primary_key.term_length));

		int rcode = txdsapi_get_record( (pBYTE)&sDEFRecord, sizeof (DEF01), DEF01_DATA, strErrorMessage );
		if( PTEMSG_NOT_FOUND == rcode )
		{
			m_StrMessageLbl = "ERROR: Deferred record does not exist for Merchant and Term specified";

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			return FALSE;
		}
		else if( PTEMSG_OK != rcode )
		{
		  m_StrMessageLbl = strErrorMessage;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
	}

	memcpy ( sMCFRecord.primary_key.merchant_id, sAuthTxRecord.TLF01_details.merchant_id, sizeof (sMCFRecord.primary_key.merchant_id));
	memcpy ( sMCFRecord.primary_key.organization_id, "0000", sizeof (sMCFRecord.primary_key.organization_id));


	rcode = txdsapi_get_record( (pBYTE)&sMCFRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if( PTEMSG_NOT_FOUND == rcode )
    {
      m_StrMessageLbl = "Invalid Merchant ID";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	memcpy ( &sAuthTxRecord.MCF01_details, &sMCFRecord, sizeof (MCF01));

	memcpy ( sCCFRecord.primary_key.card_nbr, sAuthTxRecord.TLF01_details.card_num, sizeof (sCCFRecord.primary_key.card_nbr));
	memcpy ( sCCFRecord.primary_key.card_type, "C", sizeof (sCCFRecord.primary_key.card_type));

	rcode = txdsapi_get_record( (pBYTE)&sCCFRecord, sizeof (CCF01), CCF01_DATA, strErrorMessage );
	if( PTEMSG_NOT_FOUND == rcode )
    {
      m_StrMessageLbl = "Invalid Card Number";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }

	return TRUE;

}

void CVoiceAuthResponse::PopulateDialog()
{
	char temp[20];
	double pGrossAmount, pMonthlyAmount;


	m_StrMerchantNameFld = sMCFRecord.name01;
	m_StrCardholderNameFld = sCCFRecord.name_dba;

	if ( sAuthTxRecord.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION )
	{
		CalculateGrossAmount ( &pGrossAmount );
		CalculateMonthlyAmount ( &pMonthlyAmount );

		gcvt ( pGrossAmount, 15, temp );
		m_StrGrossAmountFld = temp;

		if ( m_StrGrossAmountFld.Right(1) == "." )
			m_StrGrossAmountFld = m_StrGrossAmountFld + "00";

		gcvt ( pMonthlyAmount, 15, temp );
		m_StrMonthlyAmountFld = temp;
	}

	m_StrResponseCodeFld = sAuthTxRecord.TLF01_details.response_code;
	m_StrAuthNumFld = sAuthTxRecord.TLF01_details.auth_number;
	m_StrBankNameFld = sAuthTxRecord.BIN01_details.description;

    if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"00") == 0 )
		m_StrResponseTextFld = "Transaction Approved";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"01") == 0 )
		m_StrResponseTextFld = "Ref.to Card Issuer";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"02") == 0 )
		m_StrResponseTextFld = "RCI Special Condition";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"03") == 0 )
		m_StrResponseTextFld = "Invalid Merchant";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"04") == 0 )
		m_StrResponseTextFld = "Pick_Up Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"05") == 0 )
		m_StrResponseTextFld = "Do Not Honor";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"06") == 0 )
		m_StrResponseTextFld = "Error";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"07") == 0 )
		m_StrResponseTextFld = "Pick_Up Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"12") == 0 )
		m_StrResponseTextFld = "Invalid Transaction";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"13") == 0 )
		m_StrResponseTextFld = "Invalid Amount";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"14") == 0 )
		m_StrResponseTextFld = "Invalid Account No.";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"15") == 0 )
		m_StrResponseTextFld = "FI Not Define";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"19") == 0 )
		m_StrResponseTextFld = "Re-Enter Transacion";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"21") == 0 )
		m_StrResponseTextFld = "No Action Taken";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"25") == 0 )
		m_StrResponseTextFld = "Unable to Loc.Rec.";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"28") == 0 )
		m_StrResponseTextFld = "File Temp.Unavailable";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"39") == 0 )
		m_StrResponseTextFld = "No Credit Account";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"41") == 0 )
		m_StrResponseTextFld = "Lost Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"43") == 0 )
		m_StrResponseTextFld = "Stolen Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"51") == 0 )
		m_StrResponseTextFld = "No Sufficient Funds";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"52") == 0 )
		m_StrResponseTextFld = "No Checking Acct.";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"53") == 0 )
		m_StrResponseTextFld = "No Savings Acct.";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"54") == 0 )
		m_StrResponseTextFld = "Expired Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"55") == 0 )
		m_StrResponseTextFld = "Incorrect PIN";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"57") == 0 )
		m_StrResponseTextFld = "Trxn Not Permitted";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"61") == 0 )
		m_StrResponseTextFld = "Exceeds Amt.Limit";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"62") == 0 )
		m_StrResponseTextFld = "Restricted Card";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"63") == 0 )
		m_StrResponseTextFld = "Security Violation";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"65") == 0 )
		m_StrResponseTextFld = "Exceeds Withdrowal Limit";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"75") == 0 )
		m_StrResponseTextFld = "PIN Tries Exceeded";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"76") == 0 )
		m_StrResponseTextFld = "Unable to locate Prev.Message";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"77") == 0 )
		m_StrResponseTextFld = "Unable to locate Rev.Message";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"80") == 0 )
		m_StrResponseTextFld = "Invalid Date";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"81") == 0 )
		m_StrResponseTextFld = "Crypto Error";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"82") == 0 )
		m_StrResponseTextFld = "Incorrect CVV";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"85") == 0 )
		m_StrResponseTextFld = "Approved";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"86") == 0 )
		m_StrResponseTextFld = "Cannot Verify PIN";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"91") == 0 )
		m_StrResponseTextFld = "Destination Unavailable";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"92") == 0 )
		m_StrResponseTextFld = "Rec Inst ID Invalid";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"93") == 0 )
		m_StrResponseTextFld = "Violation of Law";
    else if ( strcmp ( (char *)sAuthTxRecord.TLF01_details.response_code,"96") == 0 )
		m_StrResponseTextFld = "System Malfunction";
    else 
		m_StrResponseTextFld = "Denied";

	UpdateData(FALSE);
}

void CVoiceAuthResponse::PupulateTLFStructure ()
{
	char dest_str[18];

	pintime_get_gmt ( dest_str );

	memcpy ( sAuthTxRecord.TLF01_details.primary_key.transaction_id, dest_str, sizeof (sAuthTxRecord.TLF01_details.primary_key.transaction_id));

	memcpy ( sAuthTxRecord.TLF01_details.invoice_number, " ", sizeof (sAuthTxRecord.TLF01_details.invoice_number));
	memcpy ( sAuthTxRecord.TLF01_details.processing_code, "000000", sizeof (sAuthTxRecord.TLF01_details.processing_code));
	memcpy ( sAuthTxRecord.TLF01_details.sys_trace_audit_num, " ", sizeof (sAuthTxRecord.TLF01_details.sys_trace_audit_num));
	memcpy ( sAuthTxRecord.TLF01_details.message_type, "0100", sizeof (sAuthTxRecord.TLF01_details.message_type));
	memcpy ( sAuthTxRecord.TLF01_details.category_code, "6011", sizeof (sAuthTxRecord.TLF01_details.category_code));
//	memcpy ( sAuthTxRecord.TLF01_details.organization_id, sDCFRecord.organization_id, sizeof (sAuthTxRecord.TLF01_details.organization_id));
	memcpy ( sAuthTxRecord.TLF01_details.organization_id, "0000", sizeof (sAuthTxRecord.TLF01_details.organization_id));
	memcpy ( sAuthTxRecord.TLF01_details.handler_queue, "VOICE", sizeof (sAuthTxRecord.TLF01_details.handler_queue));
	memcpy ( sAuthTxRecord.TLF01_details.dispensation.origin, "LO", sizeof (sAuthTxRecord.TLF01_details.dispensation.origin));
	memcpy ( sAuthTxRecord.TLF01_details.currency_code, "608",							sizeof (sAuthTxRecord.TLF01_details.currency_code));
	memcpy ( sAuthTxRecord.TLF01_details.verify_mag_stripe, "Y",						sizeof (sAuthTxRecord.TLF01_details.verify_mag_stripe));
	memcpy ( sAuthTxRecord.TLF01_details.voice_auth, "Y",								sizeof (sAuthTxRecord.TLF01_details.voice_auth));
	memcpy ( sAuthTxRecord.TLF01_details.voice_auth_text, m_StrTextFld,					sizeof (sAuthTxRecord.TLF01_details.voice_auth_text));
	memcpy ( sAuthTxRecord.TLF01_details.operator_id, ucf01.primary_key.name,    sizeof (sAuthTxRecord.TLF01_details.operator_id));
	sAuthTxRecord.TLF01_details.entry_type = 1;
}


void CVoiceAuthResponse::OnOK() 
{
   TLF01 sTLF01Record;
   char strErrorMessage[200];

   if ( m_bOverride )
	  m_nDialogMode = MODE_OVERRIDE;
   else
	  m_nDialogMode = MODE_DONE;

   if ( m_bTransaction )
   {

	   UpdateData(TRUE);
  		memcpy (  &sTLF01Record, &sAuthTxRecord.TLF01_details, sizeof ( TLF01));
		memcpy ( sAuthTxRecord.TLF01_details.voice_auth_text, m_StrTextFld,					sizeof (sAuthTxRecord.TLF01_details.voice_auth_text));

		int rcode = txdsapi_insert_record ( (pBYTE)&sAuthTxRecord, sizeof (TLF01), TLF01_DATA, strErrorMessage );
		if( PTEMSG_OK != rcode )
		{
		  m_StrMessageLbl = strErrorMessage;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		} // if
   } // if
	
	CDialog::OnOK();
}

void CVoiceAuthResponse::OnOverrideDenialButton() 
{
	m_bOverride = TRUE;



   OnOK();

}

BYTE CVoiceAuthResponse::CalculateGrossAmount ( double* pGrossAmount )
{
	double nDeferredFactor = 0.0;
	double nTemp = 0.0;

	nDeferredFactor = (strtod ((char *)sAuthTxRecord.TLF01_details.deferred_factor, 0)/1000000.0);
	nTemp = (strtod ( (char *)sAuthTxRecord.TLF01_details.total_amount, 0)/100.0);
	*pGrossAmount = nTemp + (nTemp * nDeferredFactor);
	return TRUE;
}

BYTE CVoiceAuthResponse::CalculateMonthlyAmount ( double* pMonthlyAmount )
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

	


BOOL CVoiceAuthResponse::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
