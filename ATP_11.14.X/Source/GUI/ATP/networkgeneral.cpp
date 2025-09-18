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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkgeneral.cpp  $
   
      Rev 1.11   Dec 01 2003 11:29:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Mar 04 2003 16:30:56   lmitchel
   Validate length of CWK AND ZMK  keys and for hex characters; allow entry of blank key
   
      Rev 1.9   Feb 27 2003 10:05:20   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.8   Jan 21 2000 12:24:12   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.7   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:05:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:56   iarustam
   Initial Release
   
      Rev 1.4   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.3   Mar 22 1999 11:13:12   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 16 1999 16:30:44   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:22   iarustam
    
*
************************************************************************************/
// NetworkGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkGeneral.h"
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
// CNetworkGeneral property page

IMPLEMENT_DYNCREATE(CNetworkGeneral, CPropertyPage)

CNetworkGeneral::CNetworkGeneral() : CPropertyPage(CNetworkGeneral::IDD)
{
	//{{AFX_DATA_INIT(CNetworkGeneral)
	m_StrCwkFld = _T("");
	m_StrEchoIntervalFld = _T("");
	m_StrKekFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNameFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrCountryCodeFld = _T("");
	m_StrCurrencyCashFld = _T("");
	m_StrCurrencyPurchaseFld = _T("");
	m_StrLocalNspFld = _T("");
	m_StrEncrTypeCombo = _T("");
	m_StrStationId1Fld = _T("");
	m_StrStationId2Fld = _T("");
	m_StrStatusCombo = _T("");
	m_StrPinTypeCombo = _T("");
	m_StrTpduIDFld = _T("");
	m_StrQueNameFld = _T("");
	m_StrZmkFld = _T("");
	m_StrKeyCheckFld = _T("");
	//}}AFX_DATA_INIT
}

CNetworkGeneral::~CNetworkGeneral()
{
}

void CNetworkGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkGeneral)
	DDX_Control(pDX, IDC_KEY_CHECK_FLD, m_CntlKeyCheckFld);
	DDX_Control(pDX, IDC_ZMK_FLD, m_CntlZmkFld);
	DDX_Control(pDX, IDC_QUE_NAME_FLD, m_CntlQueNameFld);
	DDX_Control(pDX, IDC_TPDU_ID_FLD, m_CntlTpduIDFld);
	DDX_Control(pDX, IDC_STATICTICS_CHECK, m_CntlStatisticsCheck);
	DDX_Control(pDX, IDC_TOTALS_CHECK, m_CntlTotalsCheck);
	DDX_Control(pDX, IDC_STATUS_COMBO, m_CntlStatusCombo);
	DDX_Control(pDX, IDC_STATION_ID2_FLD, m_CntlStationId2Fld);
	DDX_Control(pDX, IDC_STATION_ID1_FLD, m_CntlStationId1Fld);
	DDX_Control(pDX, IDC_PIN_TYPE_COMBO, m_CntlPinTypeCombo);
	DDX_Control(pDX, IDC_AUTO_LOGON_CHECK, m_CntlAutoLogonCheck);
	DDX_Control(pDX, IDC_ENCR_TYPE_COMBO, m_CntlEncrTypeCombo);
	DDX_Control(pDX, IDC_LOCAL_NSP_TYPE_FLD, m_CntlLocalNspFld);
	DDX_Control(pDX, IDC_CURRENCY_PURCHASE_FLD, m_CntlCurrencyPurchaseFld);
	DDX_Control(pDX, IDC_CURRENCY_CASH_FLD, m_CntlCurrencyCashFld);
	DDX_Control(pDX, IDC_COUNTRY_CODE_FLD, m_CntlCountryCodeFld);
	DDX_Control(pDX, IDC_PAD_CHAR_FLD, m_CntlPadCharFld);
	DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Control(pDX, IDC_NETWORK_ID_FLD, m_CntlNetworkIdFld);
	DDX_Control(pDX, IDC_NAME_FLD, m_CntlNameFld);
	DDX_Control(pDX, IDC_KEK_FLD, m_CntlKekFld);
	DDX_Control(pDX, IDC_ECHO_INTERVAL_FLD, m_CntlEchoIntervalFld);
	DDX_Control(pDX, IDC_CWK_FLD, m_CntlCwkFld);
	DDX_Text(pDX, IDC_CWK_FLD, m_StrCwkFld);
	DDV_MaxChars(pDX, m_StrCwkFld, 48);
	DDX_Text(pDX, IDC_ECHO_INTERVAL_FLD, m_StrEchoIntervalFld);
	DDV_MaxChars(pDX, m_StrEchoIntervalFld, 3);
	DDX_Text(pDX, IDC_KEK_FLD, m_StrKekFld);
	DDV_MaxChars(pDX, m_StrKekFld, 16);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NAME_FLD, m_StrNameFld);
	DDV_MaxChars(pDX, m_StrNameFld, 29);
	DDX_Text(pDX, IDC_NETWORK_ID_FLD, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);
	DDX_Text(pDX, IDC_PAD_CHAR_FLD, m_StrPadCharFld);
	DDV_MaxChars(pDX, m_StrPadCharFld, 1);
	DDX_Text(pDX, IDC_COUNTRY_CODE_FLD, m_StrCountryCodeFld);
	DDV_MaxChars(pDX, m_StrCountryCodeFld, 3);
	DDX_Text(pDX, IDC_CURRENCY_CASH_FLD, m_StrCurrencyCashFld);
	DDV_MaxChars(pDX, m_StrCurrencyCashFld, 3);
	DDX_Text(pDX, IDC_CURRENCY_PURCHASE_FLD, m_StrCurrencyPurchaseFld);
	DDV_MaxChars(pDX, m_StrCurrencyPurchaseFld, 3);
	DDX_Text(pDX, IDC_LOCAL_NSP_TYPE_FLD, m_StrLocalNspFld);
	DDV_MaxChars(pDX, m_StrLocalNspFld, 7);
	DDX_CBString(pDX, IDC_ENCR_TYPE_COMBO, m_StrEncrTypeCombo);
	DDX_Text(pDX, IDC_STATION_ID1_FLD, m_StrStationId1Fld);
	DDV_MaxChars(pDX, m_StrStationId1Fld, 6);
	DDX_Text(pDX, IDC_STATION_ID2_FLD, m_StrStationId2Fld);
	DDV_MaxChars(pDX, m_StrStationId2Fld, 6);
	DDX_CBString(pDX, IDC_STATUS_COMBO, m_StrStatusCombo);
	DDX_CBString(pDX, IDC_PIN_TYPE_COMBO, m_StrPinTypeCombo);
	DDX_Text(pDX, IDC_TPDU_ID_FLD, m_StrTpduIDFld);
	DDV_MaxChars(pDX, m_StrTpduIDFld, 15);
	DDX_Text(pDX, IDC_QUE_NAME_FLD, m_StrQueNameFld);
	DDV_MaxChars(pDX, m_StrQueNameFld, 11);
	DDX_Text(pDX, IDC_ZMK_FLD, m_StrZmkFld);
	DDV_MaxChars(pDX, m_StrZmkFld, 48);
	DDX_Text(pDX, IDC_KEY_CHECK_FLD, m_StrKeyCheckFld);
	DDV_MaxChars(pDX, m_StrKeyCheckFld, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkGeneral)
	ON_EN_KILLFOCUS(IDC_CWK_FLD, OnKillfocusCwkFld)
	ON_EN_KILLFOCUS(IDC_ZMK_FLD, OnKillfocusZmkFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkGeneral message handlers

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkGeneral::OnInitDialog() 
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

	m_bGeneral = TRUE;

	m_StrPadCharFld = _T("F");

	if (m_nDialogMode != MODE_DONE )
    {		
        EnableGeneralDialog(TRUE);	
		
        if ( m_nDialogMode != MODE_ADD )
        PopulateGeneralDialogFromDatabase (sNetworkIdOld);
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
 
BOOL CNetworkGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
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
    
BOOL CNetworkGeneral::PopulateDatabaseFromGeneralDialog( pNCF01 psNetworkId )
{	
	memcpy ( psNetworkId->cwk,                          m_StrCwkFld,               sizeof ( psNetworkId->cwk));
	memcpy ( psNetworkId->echo_interval,                m_StrEchoIntervalFld,      sizeof ( psNetworkId->echo_interval));	
	memcpy ( psNetworkId->kek,                          m_StrKekFld,               sizeof ( psNetworkId->kek));
	memcpy ( psNetworkId->name,                         m_StrNameFld,              sizeof ( psNetworkId->name));
	memcpy ( psNetworkId->pad_char,                     m_StrPadCharFld,           sizeof ( psNetworkId->pad_char));	
	memcpy ( psNetworkId->country_code,                 m_StrCountryCodeFld,       sizeof ( psNetworkId->country_code));
	memcpy ( psNetworkId->currency_code_purchase,       m_StrCurrencyPurchaseFld,  sizeof ( psNetworkId->currency_code_purchase));
	memcpy ( psNetworkId->currency_code_cash,           m_StrCurrencyCashFld,      sizeof ( psNetworkId->currency_code_cash));
	memcpy ( psNetworkId->local_nsp_type,               m_StrLocalNspFld,          sizeof ( psNetworkId->local_nsp_type));	
	memcpy ( psNetworkId->station_id1,                  m_StrStationId1Fld,        sizeof ( psNetworkId->station_id1));
	memcpy ( psNetworkId->station_id2,                  m_StrStationId2Fld,        sizeof ( psNetworkId->station_id2));
	memcpy ( psNetworkId->tpdu_id,						m_StrTpduIDFld,            sizeof ( psNetworkId->tpdu_id));
	memcpy ( psNetworkId->que_name,						m_StrQueNameFld,           sizeof ( psNetworkId->que_name));
	memcpy ( psNetworkId->zmk,						    m_StrZmkFld,               sizeof ( psNetworkId->zmk));
	//memcpy ( psNetworkId->status,                       m_StrStatusCombo,          sizeof ( psNetworkId->status));
	 memcpy ( psNetworkId->prev_src_station,				m_StrKeyCheckFld,		   sizeof ( psNetworkId->prev_src_station));

	//if (m_StrStatusCombo == "ON LINE")
   //     memcpy ( strStatus, "0", sizeof (strStatus));
   // else if (m_StrStatusCombo == "OFF LINE")
   //      memcpy ( strStatus, "1", sizeof (strStatus));
//	else if (m_StrStatusCombo == "DOWN")
    //     memcpy ( strStatus, "2", sizeof (strStatus));
	
	if (m_StrEncrTypeCombo == "DES")
         memcpy ( strStatus, "0", sizeof (strStatus));    
	memcpy ( psNetworkId->encr_type, strStatus, sizeof ( psNetworkId->encr_type));


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

BOOL CNetworkGeneral::PopulateGeneralDialogFromDatabase( NCF01 sNetworkId )
{	 
    CString pin;    
    CString status;    
	CString encr;
	CString logon;                           
    CString statistics;                
	CString totals;  

    if ( m_nDialogMode != MODE_VIEW )
        EnableGeneralDialog(TRUE);
  
    ZeroMemory ( &sNetworkIdOld, sizeof (NCF01));  

	m_StrNetworkIdFld = sNetworkId.primary_key.network_id;
	m_StrNetworkTypeFld = sNetworkId.primary_key.network_type;

	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";

    m_StrCwkFld                 = sNetworkId.cwk;
	m_StrEchoIntervalFld        = sNetworkId.echo_interval;
	m_StrKekFld                 = sNetworkId.kek;	
	m_StrNameFld	            = sNetworkId.name;
	m_StrPadCharFld             = sNetworkId.pad_char;
	m_StrCountryCodeFld         = sNetworkId.country_code;
	m_StrCurrencyCashFld        = sNetworkId.currency_code_cash;
	m_StrCurrencyPurchaseFld    = sNetworkId.currency_code_purchase;
	m_StrLocalNspFld            = sNetworkId.local_nsp_type;	
	m_StrStationId1Fld          = sNetworkId.station_id1;
	m_StrStationId2Fld          = sNetworkId.station_id2;
	m_StrStatusCombo            = sNetworkId.status;
	m_StrTpduIDFld              = sNetworkId.tpdu_id;
	m_StrQueNameFld             = sNetworkId.que_name;
	m_StrZmkFld                 = sNetworkId.zmk;
	m_StrKeyCheckFld			= sNetworkId.prev_src_station;

    pin                         = sNetworkId.pin_type;    
    status                      = sNetworkId.status;	
	encr                        = sNetworkId.encr_type;
	
	logon                       = sNetworkId.auto_logon;    
    statistics                  = sNetworkId.statistics_ind;	
	totals                      = sNetworkId.totals_ind;

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
     
    if ( status == "0")
        m_StrStatusCombo = "ON LINE";
    else if ( status == "1")
        m_StrStatusCombo = "OFF LINE";
	else if ( status == "2")
        m_StrStatusCombo = "DOWN";
	

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
							
BOOL CNetworkGeneral::CompareStructures()
{	
	CString pin;    
    CString status;    
	CString encr;
	CString logon;                           
    CString statistics;                
	CString totals;  
	CString temp;

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
    if (strcmp ( m_StrCwkFld, (char *)sNetworkIdOld.cwk) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCwkFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.cwk, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CWK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrEchoIntervalFld, (char *)sNetworkIdOld.echo_interval) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrEchoIntervalFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.echo_interval, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Echo Interval", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
	}     
    if (strcmp ( m_StrKekFld, (char *)sNetworkIdOld.kek) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrKekFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.kek, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "KEK", sizeof(m_StrFieldTag));

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
		temp = "DOWN";
*/
/*	if (strcmp ( m_StrStatusCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrStatusCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Status", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/

/*	if (encr == "0")
		temp = "DES";

	if (strcmp ( m_StrEncrTypeCombo, temp) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrEncrTypeCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Encryption Type", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/

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

/*BOOL CNetworkGeneral::GetStatus()
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

BOOL CNetworkGeneral::GetPin()
{
	if ( strcmp (m_StrPinTypeCombo, "ANSIX9.8-1982") == 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus)); 
	if ( strcmp (m_StrPinTypeCombo, "IBM 3626")== 0)
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

BOOL CNetworkGeneral::GetEncrType()
{
	if ( strcmp (m_StrEncrTypeCombo, "DES") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus));
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral::CleanGeneralDialog()
{
	m_StrCwkFld = _T("");
	m_StrEchoIntervalFld = _T("");
	m_StrKekFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNameFld = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrCountryCodeFld = _T("");
	m_StrCurrencyCashFld = _T("");
	m_StrTpduIDFld = _T("");
	m_StrCurrencyPurchaseFld = _T("");
	m_StrLocalNspFld = _T("");
	m_StrEncrTypeCombo = _T("");
	m_StrStationId1Fld = _T("");
	m_StrStationId2Fld = _T("");
	m_StrStatusCombo = _T("");
	m_StrPinTypeCombo = _T("");
	m_StrQueNameFld = _T("");
	m_StrZmkFld = _T("");
	m_StrKeyCheckFld= _T("");

    m_CntlAutoLogonCheck.SetCheck(0);
	m_CntlStatisticsCheck.SetCheck(0);
	m_CntlTotalsCheck.SetCheck(0);

	UpdateData(FALSE);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral::EnableGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {	
  	    m_CntlCountryCodeFld.EnableWindow(nStatus);
	    m_CntlCurrencyCashFld.EnableWindow(nStatus);
	    m_CntlCurrencyPurchaseFld.EnableWindow(nStatus);
	    m_CntlTpduIDFld.EnableWindow(nStatus);
	    //m_CntlCwkFld.EnableWindow(nStatus);
	    m_CntlEchoIntervalFld.EnableWindow(nStatus);  	    	    
	    m_CntlKekFld.EnableWindow(nStatus);
	    m_CntlLocalNspFld.EnableWindow(nStatus);	    
		m_CntlNameFld.EnableWindow(nStatus);	   
	    m_CntlPadCharFld.EnableWindow(nStatus); 	   	    
	    m_CntlStationId1Fld.EnableWindow(nStatus);
	    m_CntlStationId2Fld.EnableWindow(nStatus);	    
	    m_CntlEncrTypeCombo.EnableWindow(nStatus);
	    m_CntlQueNameFld.EnableWindow(nStatus);
	    m_CntlZmkFld.EnableWindow(nStatus);
		m_StrEncrTypeCombo = "DES";			    
	    m_CntlPinTypeCombo.EnableWindow(nStatus);
		m_StrPinTypeCombo = "ANSIX9.8-1982";	   
	    m_CntlStatusCombo.EnableWindow(nStatus);
	    m_CntlCwkFld.EnableWindow(nStatus);
		m_StrStatusCombo = "DOWN";
		m_CntlAutoLogonCheck.EnableWindow(nStatus);
		//m_CntlStatisticsCheck.EnableWindow(nStatus);
		m_CntlTotalsCheck.EnableWindow(nStatus);
		m_CntlKeyCheckFld.EnableWindow(nStatus);
	}
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkGeneral::PopulateHistoryTable()
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


/******************************************************************************************/
void CNetworkGeneral::OnKillfocusZmkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length =0;

    if (!UpdateData(TRUE))
 		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	
	key_length = m_StrZmkFld.GetLength();

	if(key_length == 0) /*Key Field can be left blank*/
		return;

	
	/*Check ZMK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	if(!myUtilsObject.IsHexEntry(m_StrZmkFld))
	{
		AfxMessageBox( "ZMK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", MB_ICONSTOP | MB_OK );
	 	m_CntlZmkFld.SetFocus();
	}

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if ( (key_length != 16) && 
		 (key_length != 32) && 
		 (key_length != 48) )
	{
		AfxMessageBox( "ZMK: The number of characters entered should be 16, 32, or 48.", MB_ICONSTOP | MB_OK );
		m_CntlZmkFld.SetFocus();
	}

	return;
} /*end void CNetworkGeneral::OnKillfocusZmkFld() */


/******************************************************************************************/
void CNetworkGeneral::OnKillfocusCwkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length = 0;
	INT return_code = 0;

	UpdateData(TRUE);

 	key_length = m_StrCwkFld.GetLength();

	if(key_length == 0)  /*Key Field can be left blank*/
		return;
	
	/*Check CWK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	if(!myUtilsObject.IsHexEntry(m_StrCwkFld))
	{
		return_code = AfxMessageBox( "CWK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", MB_ICONSTOP | MB_OK );
 		m_CntlCwkFld.SetFocus();
	}

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if ( (key_length != 16) && 
		 (key_length != 32) && 
		 (key_length != 48) )
	{
		return_code = AfxMessageBox( "CWK: The number of characters entered should be 16, 32, or 48.", MB_ICONSTOP | MB_OK );
		m_CntlCwkFld.SetFocus();
	}

	return;

} /*end void CNetworkGeneral::OnKillfocusCwkFld() */


