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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkgeneral1.cpp  $
   
      Rev 1.15   Dec 01 2003 11:29:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.14   Mar 04 2003 16:31:06   lmitchel
   Validate length of TPK key and for hex characters; allow entry of blank key
   
      Rev 1.13   Feb 27 2003 10:05:22   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.12   Nov 16 2001 13:22:44   lmitchel
   
      Rev 1.11   Jan 21 2000 12:24:12   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.10   Jan 05 2000 14:55:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 06 1999 16:47:02   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.1   Sep 22 1999 16:05:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:56   iarustam
   Initial Release
   
      Rev 1.7   Jun 02 1999 16:26:06   iarustam
   Bug fixes
   
      Rev 1.6   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.5   Apr 22 1999 15:35:46   iarustam
   Bug fixes
   
      Rev 1.4   Apr 15 1999 14:19:48   iarustam
   Bug fixes
   
   
      Rev 1.3   Mar 22 1999 11:13:14   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 16 1999 16:30:46   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:24   iarustam
    
*
************************************************************************************/
// NetworkGeneral1.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkGeneral1.h"
#include "MyUtils.h"

#include "SharedMem.h"	
extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CNetworkGeneral1 property page

IMPLEMENT_DYNCREATE(CNetworkGeneral1, CPropertyPage)

CNetworkGeneral1::CNetworkGeneral1() : CPropertyPage(CNetworkGeneral1::IDD)
{
	//{{AFX_DATA_INIT(CNetworkGeneral1)
	m_StrAdminMessageTimerFld = _T("");
	m_StrAdviceCounterFld = _T("");
	m_StrCountryCodeFld = _T("");
	m_StrCurrencyCashFld = _T("");
	m_StrCurrencyPurchaseFld = _T("");
	m_StrCwkFld = _T("");
	m_StrRetCwkFld = _T("");
	m_StrEchoIntervalFld = _T("");
	m_StrForwardingCodeFld = _T("");
	m_StrKekFld = _T("");
	m_StrLocalNspFld = _T("");
	m_StrMaxActiveTrnxFld = _T("");
	m_StrMaxConsTimeoutsFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNameFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrRequestTimerFld = _T("");
	m_StrStationId1Fld = _T("");
	m_StrStationId2Fld = _T("");
	m_StrAcquirerCodeFld = _T("");
	m_StrEncrTypeCombo = _T("");
	m_StrLineTimerFld = _T("");
	m_StrPinTypeCombo = _T("");
	m_StrRemoteNspCombo = _T("");
	m_StrStatusCombo = _T("");
	m_StrStoreForwProcCombo = _T("");
	m_StrTpduIDFld = _T("");
	m_StrQueNameFld = _T("");
	m_StrZmkFld = _T("");
	m_StrRecievingInstIDFld = _T("");
	m_StrDeviceIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrNegAccessCode = _T("");
	m_StrKeyCheckFld = _T("");
	m_StrBillingCurrency = _T(""); 
	//}}AFX_DATA_INIT
}

CNetworkGeneral1::~CNetworkGeneral1()
{
}

void CNetworkGeneral1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkGeneral1)
	DDX_Control(pDX, IDC_KEY_CHECK_FLD, m_CntlKeyCheckFld);
	DDX_Control(pDX, IDC_NEG_ACCESS_CODE, m_CntlNegAccessCode);
	DDX_Control(pDX, IDC_RECIEVING_INST_ID_FLD, m_CntlRecievingInstIDFld);
	DDX_Control(pDX, IDC_ZMK_FLD, m_CntlZmkFld);
	DDX_Control(pDX, IDC_QUE_NAME_FLD, m_CntlQueNameFld);
	DDX_Control(pDX, IDC_TPDU_ID_FLD, m_CntlTpduIDFld);
	DDX_Control(pDX, IDC_STATION_ID1_FLD, m_CntlStationId1Fld);
	DDX_Control(pDX, IDC_TOTALS_CHECK, m_CntlTotalsCheck);
	DDX_Control(pDX, IDC_STORE_FORWARD_PROC_COMBO, m_CntlStoreForwProcCombo);
	DDX_Control(pDX, IDC_STATUS_COMBO, m_CntlStatusCombo);
	DDX_Control(pDX, IDC_STATICTICS_CHECK, m_CntlStatisticsCheck);
	DDX_Control(pDX, IDC_REMOTE_NSP_COMBO, m_CntlRemoteNspCombo);
	DDX_Control(pDX, IDC_PIN_TYPE_COMBO, m_CntlPinTypeCombo);
	DDX_Control(pDX, IDC_IDLE_LINE_TIMER_FLD, m_CntlLineTimerFld);
	DDX_Control(pDX, IDC_ENCR_TYPE_COMBO, m_CntlEncrTypeCombo);
	DDX_Control(pDX, IDC_AUTO_LOGON_CHECK, m_CntlAutoLogonCheck);
	DDX_Control(pDX, IDC_ACQUIRER_INST_ID_CODE_FLD, m_CntlAcquirerCodeFld);
	DDX_Control(pDX, IDC_STATION_ID2_FLD, m_CntlStationId2Fld);
	DDX_Control(pDX, IDC_REQUEST_TIMER_FLD, m_CntlRequestTimerFld);
	DDX_Control(pDX, IDC_PAD_CHAR_FLD, m_CntlPadCharFld);
	DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Control(pDX, IDC_NETWORK_ID_FLD, m_CntlNetworkIdFld);
	DDX_Control(pDX, IDC_NAME_FLD, m_CntlNameFld);
	DDX_Control(pDX, IDC_MAX_CONS_TIMEOUTS_FLD, m_CntlMaxConsTimeoutsFld);
	DDX_Control(pDX, IDC_MAX_ACTIVE_TXNS_FLD, m_CntlMaxActiveTrnxFld);
	DDX_Control(pDX, IDC_LOCAL_NSP_TYPE_FLD, m_CntlLocalNspFld);
	DDX_Control(pDX, IDC_KEK_FLD, m_CntlKekFld);
	DDX_Control(pDX, IDC_FORWARDING_INST_ID_CODE_FLD, m_CntlForwardingCodeFld);
	DDX_Control(pDX, IDC_ECHO_INTERVAL_FLD, m_CntlEchoIntervalFld);
	DDX_Control(pDX, IDC_CWK_FLD, m_CntlCwkFld);
	DDX_Control(pDX, IDC_CURRENCY_PURCHASE_FLD, m_CntlCurrencyPurchaseFld);
	DDX_Control(pDX, IDC_CURRENCY_CASH_FLD, m_CntlCurrencyCashFld);
	DDX_Control(pDX, IDC_COUNTRY_CODE_FLD, m_CntlCountryCodeFld);
	DDX_Control(pDX, IDC_ADVICE_REPEAT_COUNTER_FLD, m_CntlAdviceCounterFld);
	DDX_Control(pDX, IDC_ADMIN_MESSAGE_TIMER_FLD, m_CntlAdminMessageTimerFld);
	DDX_Control(pDX, IDC_BILLING_CURRENCY_FLD, m_CntlBillingCurrency);
	DDX_Text(pDX, IDC_ADMIN_MESSAGE_TIMER_FLD, m_StrAdminMessageTimerFld);
	DDV_MaxChars(pDX, m_StrAdminMessageTimerFld, 3);
	DDX_Text(pDX, IDC_ADVICE_REPEAT_COUNTER_FLD, m_StrAdviceCounterFld);
	DDV_MaxChars(pDX, m_StrAdviceCounterFld, 2);
	DDX_Text(pDX, IDC_COUNTRY_CODE_FLD, m_StrCountryCodeFld);
	DDV_MaxChars(pDX, m_StrCountryCodeFld, 3);
	DDX_Text(pDX, IDC_CURRENCY_CASH_FLD, m_StrCurrencyCashFld);
	DDV_MaxChars(pDX, m_StrCurrencyCashFld, 3);
	DDX_Text(pDX, IDC_CURRENCY_PURCHASE_FLD, m_StrCurrencyPurchaseFld);
	DDV_MaxChars(pDX, m_StrCurrencyPurchaseFld, 3);
	DDX_Text(pDX, IDC_CWK_FLD, m_StrCwkFld);
	DDV_MaxChars(pDX, m_StrCwkFld, 48);
	DDX_Text(pDX, IDC_RETCWK_FLD, m_StrRetCwkFld);
	DDX_Text(pDX, IDC_ECHO_INTERVAL_FLD, m_StrEchoIntervalFld);
	DDV_MaxChars(pDX, m_StrEchoIntervalFld, 3);
	DDX_Text(pDX, IDC_FORWARDING_INST_ID_CODE_FLD, m_StrForwardingCodeFld);
	DDV_MaxChars(pDX, m_StrForwardingCodeFld, 11);
	DDX_Text(pDX, IDC_KEK_FLD, m_StrKekFld);
	DDV_MaxChars(pDX, m_StrKekFld, 16);
	DDX_Text(pDX, IDC_LOCAL_NSP_TYPE_FLD, m_StrLocalNspFld);
	DDV_MaxChars(pDX, m_StrLocalNspFld, 7);
	DDX_Text(pDX, IDC_MAX_ACTIVE_TXNS_FLD, m_StrMaxActiveTrnxFld);
	DDV_MaxChars(pDX, m_StrMaxActiveTrnxFld, 3);
	DDX_Text(pDX, IDC_MAX_CONS_TIMEOUTS_FLD, m_StrMaxConsTimeoutsFld);
	DDV_MaxChars(pDX, m_StrMaxConsTimeoutsFld, 3);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NAME_FLD, m_StrNameFld);
	DDV_MaxChars(pDX, m_StrNameFld, 29);
	DDX_Text(pDX, IDC_NETWORK_ID_FLD, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);
	DDX_Text(pDX, IDC_PAD_CHAR_FLD, m_StrPadCharFld);
	DDV_MaxChars(pDX, m_StrPadCharFld, 1);
	DDX_Text(pDX, IDC_REQUEST_TIMER_FLD, m_StrRequestTimerFld);
	DDV_MaxChars(pDX, m_StrRequestTimerFld, 3);
	DDX_Text(pDX, IDC_STATION_ID1_FLD, m_StrStationId1Fld);
	DDV_MaxChars(pDX, m_StrStationId1Fld, 6);
	DDX_Text(pDX, IDC_STATION_ID2_FLD, m_StrStationId2Fld);
	DDV_MaxChars(pDX, m_StrStationId2Fld, 6);
	DDX_Text(pDX, IDC_ACQUIRER_INST_ID_CODE_FLD, m_StrAcquirerCodeFld);
	DDV_MaxChars(pDX, m_StrAcquirerCodeFld, 11);
	DDX_CBString(pDX, IDC_ENCR_TYPE_COMBO, m_StrEncrTypeCombo);
	DDX_Text(pDX, IDC_IDLE_LINE_TIMER_FLD, m_StrLineTimerFld);
	DDV_MaxChars(pDX, m_StrLineTimerFld, 3);
	DDX_CBString(pDX, IDC_PIN_TYPE_COMBO, m_StrPinTypeCombo);
	DDX_CBString(pDX, IDC_REMOTE_NSP_COMBO, m_StrRemoteNspCombo);
	DDX_CBString(pDX, IDC_STATUS_COMBO, m_StrStatusCombo);
	DDX_CBString(pDX, IDC_STORE_FORWARD_PROC_COMBO, m_StrStoreForwProcCombo);
	DDX_Text(pDX, IDC_TPDU_ID_FLD, m_StrTpduIDFld);
	DDV_MaxChars(pDX, m_StrTpduIDFld, 15);
	DDX_Text(pDX, IDC_QUE_NAME_FLD, m_StrQueNameFld);
	DDV_MaxChars(pDX, m_StrQueNameFld, 11);
	DDX_Text(pDX, IDC_ZMK_FLD, m_StrZmkFld);
	DDV_MaxChars(pDX, m_StrZmkFld, 48);
	DDX_Text(pDX, IDC_RECIEVING_INST_ID_FLD, m_StrRecievingInstIDFld);
	DDV_MaxChars(pDX, m_StrRecievingInstIDFld, 11);
	DDX_Text(pDX, IDC_NEG_ACCESS_CODE, m_StrNegAccessCode);
	DDV_MaxChars(pDX, m_StrNegAccessCode, 6);
	DDX_Text(pDX, IDC_KEY_CHECK_FLD, m_StrKeyCheckFld);
	DDV_MaxChars(pDX, m_StrKeyCheckFld, 6);
	DDX_Text(pDX, IDC_BILLING_CURRENCY_FLD, m_StrBillingCurrency);
	DDV_MaxChars(pDX, m_StrBillingCurrency, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkGeneral1, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkGeneral1)
	ON_EN_KILLFOCUS(IDC_CWK_FLD, OnKillfocusCwkFld)
	ON_EN_KILLFOCUS(IDC_ZMK_FLD, OnKillfocusZmkFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkGeneral1 message handlers

BOOL CNetworkGeneral1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);    
	

	m_bGeneral1 = TRUE;

    m_StrPadCharFld = _T("F");

	if (m_nDialogMode != MODE_DONE )
    {		
        EnableGeneral1Dialog(TRUE);	
		
        if ( m_nDialogMode != MODE_ADD )
        PopulateGeneral1DialogFromDatabase (sNetworkIdOld);
    }   

	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
    
BOOL CNetworkGeneral1::PopulateDatabaseFromGeneral1Dialog( pNCF01 psNetworkId )
{
	memcpy ( psNetworkId->acquirer_inst_id_code,        m_StrAcquirerCodeFld,      sizeof ( psNetworkId->acquirer_inst_id_code));
	memcpy ( psNetworkId->cwk,                          m_StrCwkFld,               sizeof ( psNetworkId->cwk));
	memcpy ( psNetworkId->echo_interval,                m_StrEchoIntervalFld,      sizeof ( psNetworkId->echo_interval));
	memcpy ( psNetworkId->forwarding_inst_id_code,      m_StrForwardingCodeFld,    sizeof ( psNetworkId->forwarding_inst_id_code));
	memcpy ( psNetworkId->kek,                          m_StrKekFld,               sizeof ( psNetworkId->kek));
	memcpy ( psNetworkId->max_consecutive_timeouts,     m_StrMaxConsTimeoutsFld,   sizeof ( psNetworkId->max_consecutive_timeouts));
	memcpy ( psNetworkId->max_active_txns,              m_StrMaxActiveTrnxFld,     sizeof ( psNetworkId->max_active_txns));
	memcpy ( psNetworkId->name,                         m_StrNameFld,              sizeof ( psNetworkId->name));
	memcpy ( psNetworkId->pad_char,                     m_StrPadCharFld,           sizeof ( psNetworkId->pad_char));
	memcpy ( psNetworkId->admin_message_timer,          m_StrAdminMessageTimerFld, sizeof ( psNetworkId->admin_message_timer));
	memcpy ( psNetworkId->advice_repeat_counter,        m_StrAdviceCounterFld,     sizeof ( psNetworkId->advice_repeat_counter));
	memcpy ( psNetworkId->country_code,                 m_StrCountryCodeFld,       sizeof ( psNetworkId->country_code));
	memcpy ( psNetworkId->currency_code_purchase,       m_StrCurrencyPurchaseFld,  sizeof ( psNetworkId->currency_code_purchase));
	memcpy ( psNetworkId->currency_code_cash,           m_StrCurrencyCashFld,      sizeof ( psNetworkId->currency_code_cash));
	memcpy ( psNetworkId->local_nsp_type,               m_StrLocalNspFld,          sizeof ( psNetworkId->local_nsp_type));
	memcpy ( psNetworkId->request_timer,                m_StrRequestTimerFld,      sizeof ( psNetworkId->request_timer));
	memcpy ( psNetworkId->station_id1,                  m_StrStationId1Fld,        sizeof ( psNetworkId->station_id1));
	memcpy ( psNetworkId->station_id2,                  m_StrStationId2Fld,        sizeof ( psNetworkId->station_id2));
	memcpy ( psNetworkId->idle_line_timer,              m_StrLineTimerFld,         sizeof ( psNetworkId->idle_line_timer));
	memcpy ( psNetworkId->tpdu_id,						m_StrTpduIDFld,            sizeof ( psNetworkId->tpdu_id));
	memcpy ( psNetworkId->zmk,						    m_StrZmkFld,               sizeof ( psNetworkId->zmk));
	memcpy ( psNetworkId->receiving_inst_id_code,		m_StrRecievingInstIDFld,   sizeof ( psNetworkId->receiving_inst_id_code));
	memcpy ( psNetworkId->que_name,					    m_StrQueNameFld,		   sizeof ( psNetworkId->que_name));
	memcpy ( psNetworkId->neg_access_code,				m_StrNegAccessCode,		   sizeof ( psNetworkId->neg_access_code));
	memcpy ( psNetworkId->prev_src_station,				m_StrKeyCheckFld,		   sizeof ( psNetworkId->prev_src_station));

	memcpy ( psNetworkId->allowed[19].processing_code,				m_StrBillingCurrency,		   3);
	//if (m_StrStatusCombo == "ON LINE")
    //     memcpy ( strStatus, "0", sizeof (strStatus));
    //else if (m_StrStatusCombo == "OFF LINE")
    //     memcpy ( strStatus, "1", sizeof (strStatus));
	//else if (m_StrStatusCombo == "DOWN")
    //     memcpy ( strStatus, "2", sizeof (strStatus));
	//memcpy ( psNetworkId->status, strStatus, sizeof ( psNetworkId->status));

		if (m_StrRemoteNspCombo == "OTHER")
         memcpy ( strStatus, "0", sizeof (strStatus));
    else if (m_StrRemoteNspCombo == "ATALLA")
         memcpy ( strStatus, "1", sizeof (strStatus));
	memcpy ( psNetworkId->remote_nsp_type, strStatus, sizeof ( psNetworkId->remote_nsp_type));

	if (m_StrEncrTypeCombo == "DES")
         memcpy ( strStatus, "0", sizeof (strStatus));    
	memcpy ( psNetworkId->encr_type, strStatus, sizeof ( psNetworkId->encr_type));

	if (m_StrStoreForwProcCombo == "WAIT ON FORWARD PROCESSING")
         memcpy ( strStatus, "0", sizeof (strStatus));
    else if (m_StrStoreForwProcCombo == "MIX WITH FORWARD PROCESSING")
         memcpy ( strStatus, "1", sizeof (strStatus));
	memcpy ( psNetworkId->store_forward_processing, strStatus, sizeof ( psNetworkId->store_forward_processing));

	if (m_StrPinTypeCombo == "ANSIX9.8-1982")
         memcpy ( strStatus, "1", sizeof (strStatus));
    else if (m_StrPinTypeCombo == "IBM 3624")
         memcpy ( strStatus, "2", sizeof (strStatus));
	else if (m_StrPinTypeCombo == "PIN/pad or Docutel")
         memcpy ( strStatus, "3", sizeof (strStatus));
	else if (m_StrPinTypeCombo == "IBM PIN pad")
         memcpy ( strStatus, "4", sizeof (strStatus));
	else if (m_StrPinTypeCombo == "Burroughs")
         memcpy ( strStatus, "5", sizeof (strStatus));
	else if (m_StrPinTypeCombo == "IBM 4731")
         memcpy ( strStatus, "6", sizeof (strStatus));    
    memcpy ( psNetworkId->pin_type, strStatus, sizeof ( psNetworkId->pin_type));

	if (m_CntlAutoLogonCheck.GetCheck() == 1)
		memcpy ( psNetworkId->auto_logon, "1", sizeof ( psNetworkId->auto_logon));
	else
		memcpy ( psNetworkId->auto_logon, "0", sizeof ( psNetworkId->auto_logon));


	//if (m_CntlStatisticsCheck.GetCheck() == 1)
	//	memcpy ( psNetworkId->statistics_ind, "1", sizeof ( psNetworkId->statistics_ind));
	//else
	//	memcpy ( psNetworkId->statistics_ind, "0", sizeof ( psNetworkId->statistics_ind));


	if (m_CntlTotalsCheck.GetCheck() == 1)
		memcpy ( psNetworkId->totals_ind, "1", sizeof ( psNetworkId->totals_ind));
	else
		memcpy ( psNetworkId->totals_ind, "0", sizeof ( psNetworkId->totals_ind));



    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::PopulateGeneral1DialogFromDatabase( NCF01 sNetworkId )
{	 
    CString pin;
    CString remote; 
    CString status; 
    CString store;
	CString encr;
	CString logon;
	CString statistics;
	CString totals;

    if ( m_nDialogMode != MODE_VIEW )
        EnableGeneral1Dialog(TRUE);
  
    ZeroMemory ( &sNetworkIdOld, sizeof (NCF01));  

	m_StrNetworkIdFld = sNetworkId.primary_key.network_id;
	m_StrNetworkTypeFld = sNetworkId.primary_key.network_type;

	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";

 
    m_StrCwkFld                 = sNetworkId.cwk;
	m_StrRetCwkFld              = sNetworkId.retired_cwk;
	m_StrEchoIntervalFld        = sNetworkId.echo_interval;
	m_StrAcquirerCodeFld        = sNetworkId.acquirer_inst_id_code;
	m_StrForwardingCodeFld      = sNetworkId.forwarding_inst_id_code;
	m_StrKekFld                 = sNetworkId.kek;
	m_StrMaxActiveTrnxFld       = sNetworkId.max_active_txns;
	m_StrMaxConsTimeoutsFld     = sNetworkId.max_consecutive_timeouts;
	m_StrNameFld	            = sNetworkId.name;
	m_StrPadCharFld             = sNetworkId.pad_char; 
	m_StrAdminMessageTimerFld   = sNetworkId.admin_message_timer;
	m_StrAdviceCounterFld       = sNetworkId.advice_repeat_counter;
	m_StrCountryCodeFld         = sNetworkId.country_code;
	m_StrCurrencyCashFld        = sNetworkId.currency_code_cash;
	m_StrCurrencyPurchaseFld    = sNetworkId.currency_code_purchase;
	m_StrLocalNspFld            = sNetworkId.local_nsp_type;
	m_StrRequestTimerFld        = sNetworkId.request_timer;
	m_StrStationId1Fld          = sNetworkId.station_id1;
	m_StrStationId2Fld          = sNetworkId.station_id2;
	m_StrTpduIDFld              = sNetworkId.tpdu_id;
	m_StrQueNameFld             = sNetworkId.que_name;
	m_StrZmkFld                 = sNetworkId.zmk;
	m_StrRecievingInstIDFld     = sNetworkId.receiving_inst_id_code;
    m_StrLineTimerFld           = sNetworkId.idle_line_timer;
	m_StrNegAccessCode			= sNetworkId.neg_access_code;
	m_StrKeyCheckFld			= sNetworkId.prev_src_station;


    pin                         = sNetworkId.pin_type;
    remote                      = sNetworkId.remote_nsp_type;
    status                      = sNetworkId.status;
	store                       = sNetworkId.store_forward_processing;
	encr                        = sNetworkId.encr_type;

	logon                       = sNetworkId.auto_logon;    
    statistics                  = sNetworkId.statistics_ind;	
	totals                      = sNetworkId.totals_ind;

	if(sNetworkId.allowed[19].processing_code[0] != '\0')
	{
		m_StrBillingCurrency        = sNetworkId.allowed[19].processing_code;  
	}
	else
	{
		m_StrBillingCurrency  = "608";
	}
    

    if ( pin == "1")     
        m_StrPinTypeCombo = "ANSIX9.8-1982";
    else if ( pin == "2")
        m_StrPinTypeCombo = "IBM 3624";
	else if ( pin == "3")
        m_StrPinTypeCombo = "PIN/pad or Docutel";
	else if ( pin == "4")
        m_StrPinTypeCombo = "IBM PIN pad";
	else if ( pin == "5")
        m_StrPinTypeCombo = "Burroughs";
	else if ( pin == "6")
        m_StrPinTypeCombo = "IBM 4731";  
   
    if ( remote == "0")
        m_StrRemoteNspCombo = "OTHER";
    else if ( remote == "1")
        m_StrRemoteNspCombo = "ATALLA";
              
     
    if ( status == "0")
        m_StrStatusCombo = "ON LINE";
    else if ( status == "1")
        m_StrStatusCombo = "OFF LINE";
	else if ( status == "2")
        m_StrStatusCombo = "DOWN";
	
	if ( store == "0")
        m_StrStoreForwProcCombo = "WAIT ON FORWARD PROCESSING";
    else if ( store == "1")
        m_StrStoreForwProcCombo = "MIX WITH FORWARD PROCESSING";

	if ( encr == "0")
        m_StrEncrTypeCombo = "DES";

	if ( logon == '1')
		m_CntlAutoLogonCheck.SetCheck(1);

	if ( statistics == '1')
		m_CntlStatisticsCheck.SetCheck(1);

	if ( totals == '1')
		m_CntlTotalsCheck.SetCheck(1);
     
     
    UpdateData(FALSE);

    memcpy ( &sNetworkIdOld, &sNetworkId, sizeof ( NCF01));
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
							
BOOL CNetworkGeneral1::CompareStructures()
{
	CString pin;
    CString remote; 
    CString status; 
    CString store;
	CString encr;
	CString logon;
	CString statistics;
    CString totals;
    CString temp;
   if (strcmp ( m_StrLineTimerFld, (char *)sNetworkIdOld.idle_line_timer) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLineTimerFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.idle_line_timer, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Line Timer", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 

   if (strcmp ( m_StrNegAccessCode, (char *) sNetworkIdOld.neg_access_code) != 0)
   {
        memcpy(m_StrCurrentData, m_StrNegAccessCode, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.neg_access_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Negative Data Access Code", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
   }

   if (strcmp ( m_StrRecievingInstIDFld, (char *)sNetworkIdOld.receiving_inst_id_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrRecievingInstIDFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.receiving_inst_id_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Recieving Institution ID", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrQueNameFld, (char *)sNetworkIdOld.que_name) != 0)
    {
        memcpy(m_StrCurrentData, m_StrQueNameFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.que_name, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Que Name", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrZmkFld, (char *)sNetworkIdOld.zmk) != 0)
    {
        memcpy(m_StrCurrentData, m_StrZmkFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.zmk, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ZMK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrTpduIDFld, (char *)sNetworkIdOld.tpdu_id) != 0)
    {
        memcpy(m_StrCurrentData, m_StrTpduIDFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.tpdu_id, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "TPDU ID", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrEchoIntervalFld, (char *)sNetworkIdOld.echo_interval) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrEchoIntervalFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.echo_interval, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Echo interval", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
	}    
    if (strcmp ( m_StrForwardingCodeFld, (char *)sNetworkIdOld.forwarding_inst_id_code) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrForwardingCodeFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.forwarding_inst_id_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Forwarding Institution ID Code", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 
    if (strcmp ( m_StrKekFld, (char *)sNetworkIdOld.kek) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrKekFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.kek, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "KEK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrMaxActiveTrnxFld, (char *)sNetworkIdOld.max_active_txns) != 0)
    { 
        memcpy(m_StrCurrentData, m_StrMaxActiveTrnxFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.max_active_txns, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Maximum active transactions", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrMaxConsTimeoutsFld, (char *)sNetworkIdOld.max_consecutive_timeouts) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrMaxConsTimeoutsFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.max_consecutive_timeouts, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Maximum consecutive timeouts", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrNameFld, (char *)sNetworkIdOld.name) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrNameFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.name, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Name", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrPadCharFld, (char *)sNetworkIdOld.pad_char) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrPadCharFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.pad_char, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PAD character", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrAcquirerCodeFld, (char *)sNetworkIdOld.acquirer_inst_id_code) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrAcquirerCodeFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.acquirer_inst_id_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Acquirer Institution ID code", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrAdminMessageTimerFld, (char *)sNetworkIdOld.admin_message_timer) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrAdminMessageTimerFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.admin_message_timer, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Administrative Message Timer", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrAdviceCounterFld, (char *)sNetworkIdOld.advice_repeat_counter) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrAdviceCounterFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.advice_repeat_counter, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Advice Repeat Counter", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCountryCodeFld, (char *)sNetworkIdOld.country_code) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCountryCodeFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.country_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Country Code", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCurrencyCashFld, (char *)sNetworkIdOld.currency_code_cash) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCurrencyCashFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.currency_code_cash, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Currency Code Cash", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCurrencyPurchaseFld, (char *)sNetworkIdOld.currency_code_purchase) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrCurrencyPurchaseFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.currency_code_purchase, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Currency Code Purchase", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLocalNspFld, (char *)sNetworkIdOld.local_nsp_type) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrLocalNspFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.local_nsp_type, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Local NSP Type", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrRequestTimerFld, (char *)sNetworkIdOld.request_timer) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrRequestTimerFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.request_timer, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Request Timer", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrStationId1Fld, (char *)sNetworkIdOld.station_id1) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrStationId1Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.station_id1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Station ID1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrStationId2Fld, (char *)sNetworkIdOld.station_id2) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrStationId2Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.station_id2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Station ID2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrKeyCheckFld, (char *)sNetworkIdOld.prev_src_station) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrKeyCheckFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.prev_src_station, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Key Check", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
		pin = sNetworkIdOld.pin_type;    
        status = sNetworkIdOld.status;	
	    encr = sNetworkIdOld.encr_type;
		remote = sNetworkIdOld.remote_nsp_type;       
    	store = sNetworkIdOld.store_forward_processing;

		if (pin == "1")
		temp = "ANSIX9.8-1982";
		else if (pin == "2")
		temp = "IBM 3624";
		else if (pin == "3")
		temp = "PIN/pad or Docutel";
		else if (pin == "4")
		temp = "IBM PIN pad";
		else if (pin == "5")
		temp = "Burroughs";
		else if (pin == "6")
		temp = "IBM 4731";   

	if (strcmp ( m_StrPinTypeCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrPinTypeCombo, sizeof (m_StrCurrentData));	   
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Type", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

/*	if (status == "0")
		temp = "ON LINE";
	else if (status == "1")
		temp = "OFF LINE";
	else if (status == "2")
		temp = "DOWN";*/

/*	if (strcmp ( m_StrStatusCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrStatusCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Status", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/

	if (encr == "0")
		temp = "DES";

	if (strcmp ( m_StrEncrTypeCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrEncrTypeCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Encryption Type", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (remote == "0")
		temp = "OTHER";
	else if (remote == "1")
		temp = "ATALLA";
    
	if (strcmp ( m_StrRemoteNspCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrRemoteNspCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Remote NSP Type", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (store == "0")
		temp = "WAIT ON FORWARD PROCESSING";
	else if (store == "1")
		temp = "MIX WITH FORWARD PROCESSING";

	if (strcmp ( m_StrStoreForwProcCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrStoreForwProcCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Store Forward Processing", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	logon = sNetworkIdOld.auto_logon;    
    statistics = sNetworkIdOld.statistics_ind;	
	totals = sNetworkIdOld.totals_ind;

	if (m_CntlAutoLogonCheck.GetCheck() == 0)
		temp = "0";
	else if (m_CntlAutoLogonCheck.GetCheck() == 1)
		temp = "1";

	if (strcmp ( temp, logon) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, logon, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Auto Logon", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

/*	if (m_CntlStatisticsCheck.GetCheck() == 0)
		temp = "0";
	else if (m_CntlStatisticsCheck.GetCheck() == 1)
		temp = "1";

	if (strcmp ( temp, statistics) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, statistics, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Statistics Indicator", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/

	if (m_CntlTotalsCheck.GetCheck() == 0)
		temp = "0";
	else if (m_CntlTotalsCheck.GetCheck() == 1)
		temp = "1";

	if (strcmp ( temp, totals) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, totals, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Totals Indicator", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }


        return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

/*BOOL CNetworkGeneral1::GetStatus()
{
	if ( strcmp (m_StrStatusCombo, "ON LINE") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus)); 
	if ( strcmp (m_StrStatusCombo, "OFF LINE")== 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus)); 
	if ( strcmp (m_StrStatusCombo, "DOWN")== 0)
         memcpy ((LPTSTR) strStatus,	"2", sizeof(strStatus)); 

	return TRUE;
}*/


/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::GetRemote()
{
	if ( strcmp (m_StrRemoteNspCombo, "OTHER") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus));
	if ( strcmp (m_StrRemoteNspCombo, "ATALLA")== 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus));
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::GetPin()
{
	if ( strcmp (m_StrPinTypeCombo, "ANSIX9.8-1982") == 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus)); 
	if ( strcmp (m_StrPinTypeCombo, "IBM 3624")== 0)
         memcpy ((LPTSTR) strStatus,	"2", sizeof(strStatus)); 
	if ( strcmp (m_StrPinTypeCombo, "PIN/pad or Docutel")== 0)
         memcpy ((LPTSTR) strStatus,	"3", sizeof(strStatus));
	if ( strcmp (m_StrPinTypeCombo, "IBM PIN pad") == 0)
         memcpy ((LPTSTR) strStatus,	"4", sizeof(strStatus)); 
	if ( strcmp (m_StrPinTypeCombo, "Burroughs")== 0)
         memcpy ((LPTSTR) strStatus,	"5", sizeof(strStatus)); 
	if ( strcmp (m_StrPinTypeCombo, "IBM 4731")== 0)
         memcpy ((LPTSTR) strStatus,	"6", sizeof(strStatus)); 

    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::GetStore()
{
	if ( strcmp (m_StrStoreForwProcCombo, "WAIT ON FORWARD PROCESSING") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus));
	if ( strcmp (m_StrStoreForwProcCombo, "MIX WITH FORWARD PROCESSING")== 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus));
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral1::GetEncrType()
{
	if ( strcmp (m_StrEncrTypeCombo, "DES") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus));
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral1::CleanGeneral1Dialog()
{
	m_StrAdminMessageTimerFld = _T("");
	m_StrAdviceCounterFld = _T("");
	m_StrCountryCodeFld = _T("");
	m_StrCurrencyCashFld = _T("");
	m_StrCurrencyPurchaseFld = _T("");
	m_StrCwkFld = _T("");
	m_StrRetCwkFld = _T("");
	m_StrEchoIntervalFld = _T("");
	m_StrForwardingCodeFld = _T("");
	m_StrKekFld = _T("");
	m_StrLocalNspFld = _T("");
	m_StrMaxActiveTrnxFld = _T("");
	m_StrMaxConsTimeoutsFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNameFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrRequestTimerFld = _T("");
	m_StrStationId1Fld = _T("");
	m_StrStationId2Fld = _T("");
	m_StrAcquirerCodeFld = _T("");
	m_StrZmkFld = _T("");
	m_StrEncrTypeCombo = _T("");
	m_StrLineTimerFld = _T("");
	m_StrPinTypeCombo = _T("");
	m_StrRemoteNspCombo = _T("");
	m_StrStatusCombo = _T("");
	m_StrStoreForwProcCombo = _T("");
	m_StrTpduIDFld = _T("");
	m_StrQueNameFld = _T("");
	m_StrRecievingInstIDFld = "";
	m_StrNegAccessCode = _T("");
	m_StrDeviceIDFld = "";
	m_StrMerchantIDFld = "";
	m_StrKeyCheckFld = _T("");
	m_StrBillingCurrency = _T("");

	m_CntlAutoLogonCheck.SetCheck(0);
	m_CntlStatisticsCheck.SetCheck(0);
	m_CntlTotalsCheck.SetCheck(0);

	UpdateData(FALSE);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral1::EnableGeneral1Dialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {
	    m_CntlAdminMessageTimerFld.EnableWindow(nStatus);      
        m_CntlAdviceCounterFld.EnableWindow(nStatus);
  	    m_CntlCountryCodeFld.EnableWindow(nStatus);
	    m_CntlCurrencyCashFld.EnableWindow(nStatus);
	    m_CntlCurrencyPurchaseFld.EnableWindow(nStatus);
	    m_CntlTpduIDFld.EnableWindow(nStatus);
	    m_CntlEchoIntervalFld.EnableWindow(nStatus);	    
	    m_CntlForwardingCodeFld.EnableWindow(nStatus);	    
	    m_CntlKekFld.EnableWindow(nStatus);
	    m_CntlLocalNspFld.EnableWindow(nStatus);
	    m_CntlMaxActiveTrnxFld.EnableWindow(nStatus);
	    m_CntlMaxConsTimeoutsFld.EnableWindow(nStatus);
		m_CntlNameFld.EnableWindow(nStatus); 	    
	    m_CntlPadCharFld.EnableWindow(nStatus);	    
	    m_CntlRequestTimerFld.EnableWindow(nStatus);	    
	    m_CntlStationId1Fld.EnableWindow(nStatus);
	    m_CntlStationId2Fld.EnableWindow(nStatus);
	    m_CntlZmkFld.EnableWindow(nStatus);
	    m_CntlAcquirerCodeFld.EnableWindow(nStatus);
	    m_CntlEncrTypeCombo.EnableWindow(nStatus);
		m_StrEncrTypeCombo = "DES";
		m_CntlLineTimerFld.EnableWindow(nStatus);	    
	    m_CntlQueNameFld.EnableWindow(nStatus);
	    m_CntlPinTypeCombo.EnableWindow(nStatus);
		m_StrPinTypeCombo = "ANSIX9.8-1982";
	    m_CntlRemoteNspCombo.EnableWindow(nStatus);
		m_StrRemoteNspCombo = "OTHER";
	    m_CntlStatusCombo.EnableWindow(nStatus);
		m_StrStatusCombo = "DOWN";
	    m_CntlStoreForwProcCombo.EnableWindow(nStatus);
		m_StrStoreForwProcCombo = "WAIT ON FORWARD PROCESSING";
		m_CntlAutoLogonCheck.EnableWindow(nStatus);
	    m_CntlCwkFld.EnableWindow(nStatus);
		m_CntlTotalsCheck.EnableWindow(nStatus);
		m_CntlRecievingInstIDFld.EnableWindow(nStatus);
		m_CntlNegAccessCode.EnableWindow(nStatus);
		m_CntlKeyCheckFld.EnableWindow(nStatus);
		m_CntlBillingCurrency.EnableWindow(nStatus);

	}
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral1::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;
    CHAR wstation[21]; DWORD wsize;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNetworkIdFld);
    strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"NCF01",               sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));               
    memcpy ((LPTSTR)sHistoryData.current_data,          (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,         (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,            (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag,             (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}	
}


void CNetworkGeneral1::OnKillfocusCwkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length = 0;
	INT return_code = 0;

	UpdateData(TRUE);

	key_length = m_StrCwkFld.GetLength();

	if(key_length == 0)	/*Key Field can be left blank*/
		return;
	
	/*Check CWK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	if(!myUtilsObject.IsHexEntry(m_StrCwkFld))
	{
		return_code = AfxMessageBox( "CWK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", 
												MB_ICONSTOP | MB_OK );
	 	m_CntlCwkFld.SetFocus();
	}

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if( (key_length != 16) && 
	    (key_length != 32) &&
	    (key_length != 48) )
	{
		return_code = AfxMessageBox( "CWK: The number of characters entered should be 16, 32, or 48.", MB_ICONSTOP | MB_OK  );
    	m_CntlCwkFld.SetFocus();
	}
	
	return;
} /*end void CNetworkGeneral1::OnKillfocusCwkFld() */



void CNetworkGeneral1::OnKillfocusZmkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length = 0;
	INT return_code;

    if (!UpdateData(TRUE))
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	
	key_length = m_StrZmkFld.GetLength();

	if(key_length == 0)	/*Key Field can be left blank*/
		return;

	
	/*Check ZMK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	if(!myUtilsObject.IsHexEntry(m_StrZmkFld))
	{
		return_code = AfxMessageBox( "ZMK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", MB_ICONSTOP | MB_OK );
	 	m_CntlZmkFld.SetFocus();
	}

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if( (key_length != 16) && 
	    (key_length != 32) &&
	    (key_length != 48) )
	{
		return_code = AfxMessageBox( "ZMK: The number of characters entered should be 16, 32, or 48.", MB_ICONSTOP | MB_OK );
		m_CntlZmkFld.SetFocus();
	}

	return;
} /*end void CNetworkGeneral1::OnKillfocusZmkFld() */
