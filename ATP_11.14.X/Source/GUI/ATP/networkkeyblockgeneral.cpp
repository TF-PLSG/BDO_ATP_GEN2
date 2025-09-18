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
// NetworkKeyblockGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkKeyblockGeneral.h"
#include "MyUtils.h"
#include "NameList.h"
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

IMPLEMENT_DYNCREATE(CNetworkKeyblockGeneral, CPropertyPage)

CNetworkKeyblockGeneral::CNetworkKeyblockGeneral() : CPropertyPage(CNetworkKeyblockGeneral::IDD)
{
	//{{AFX_DATA_INIT(CNetworkKeyblockGeneral)

		m_StrNetworkIdFld = _T("");
		m_Striss_cwk_keyblockFld = _T("");
		m_Striss_zmk_keyblockFld = _T("");
		m_StrMessageLbl = _T("");
		m_Striss_rtdcwk_keyblockFld = _T("");
		m_Striss_kek_keyblockFld = _T("");
		m_Stracq_cwk_keyblockFld = _T("");
		m_Stracq_zmk_keyblockFld = _T("");
		m_Stracq_kek_keyblockFld = _T("");
		m_Striss_prev_src_stationFld = _T("");
		m_StrMessageLbl = _T("");


	//}}AFX_DATA_INIT
}

CNetworkKeyblockGeneral::~CNetworkKeyblockGeneral()
{
}

void CNetworkKeyblockGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkKeyblockGeneral)
	DDX_Control(pDX, IDC_ISS_CWK_KEYBLOCK, m_Cntliss_cwk_keyblockFld);
	DDX_Control(pDX, IDC_ISS_ZMK_KEYBLOCK, m_Cntliss_zmk_keyblockFld);
	DDX_Control(pDX, IDC_ISS_RTDCWK_KEYBLOCK, m_Cntliss_rtdcwk_keyblockFld);
	DDX_Control(pDX, IDC_ISS_KEK_KEYBLOCK, m_Cntliss_kek_keyblockFld);
	DDX_Control(pDX, IDC_ACQ_CWK_KEYBLOCK, m_Cntlacq_cwk_keyblockFld);
	DDX_Control(pDX, IDC_ACQ_ZMK_KEYBLOCK, m_Cntlacq_zmk_keyblockFld);
	DDX_Control(pDX, IDC_ACQ_KEK_KEYBLOCK, m_Cntlacq_kek_keyblockFld);
	DDX_Control(pDX, IDC_ISS_PREV_SRC_STATION, m_Cntliss_prev_src_stationFld);
	DDX_Control(pDX, IDC_NCF01_KEYBLOCK_NETWORK, m_CntlNetworkIdFld);
	
	/*DDX_Control(pDX, IDC_PIN_TYPE_COMBO, m_CntlPinTypeCombo);
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
	DDX_Control(pDX, IDC_CWK_FLD, m_CntlCwkFld);*/
	
	DDX_Text(pDX, IDC_NCF01_KEYBLOCK_NETWORK, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	
	DDX_Text(pDX, IDC_ISS_CWK_KEYBLOCK, m_Striss_cwk_keyblockFld);
	DDV_MaxChars(pDX, m_Striss_cwk_keyblockFld, 128);
	DDX_Text(pDX, IDC_ISS_ZMK_KEYBLOCK, m_Striss_zmk_keyblockFld);
	DDV_MaxChars(pDX, m_Striss_zmk_keyblockFld, 128);
	DDX_Text(pDX, IDC_ISS_RTDCWK_KEYBLOCK, m_Striss_rtdcwk_keyblockFld);
	DDV_MaxChars(pDX, m_Striss_rtdcwk_keyblockFld, 128);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ISS_KEK_KEYBLOCK, m_Striss_kek_keyblockFld);
	DDV_MaxChars(pDX, m_Striss_kek_keyblockFld, 128);


	DDX_Text(pDX, IDC_ACQ_CWK_KEYBLOCK, m_Stracq_cwk_keyblockFld);
	DDV_MaxChars(pDX, m_Stracq_cwk_keyblockFld, 128);
	DDX_Text(pDX, IDC_ACQ_ZMK_KEYBLOCK, m_Stracq_zmk_keyblockFld);
	DDV_MaxChars(pDX, m_Stracq_zmk_keyblockFld, 128);
	DDX_Text(pDX, IDC_ACQ_KEK_KEYBLOCK, m_Stracq_kek_keyblockFld);
	DDV_MaxChars(pDX, m_Stracq_kek_keyblockFld, 128);
	DDX_Text(pDX, IDC_ISS_PREV_SRC_STATION, m_Striss_prev_src_stationFld);
	DDV_MaxChars(pDX, m_Striss_prev_src_stationFld, 128);
	
	/*DDX_Text(pDX, IDC_LOCAL_NSP_TYPE_FLD, m_StrLocalNspFld);
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
	DDV_MaxChars(pDX, m_StrKeyCheckFld, 6);*/
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkKeyblockGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkKeyblockGeneral)
	ON_EN_KILLFOCUS(IDC_CWK_FLD, OnKillfocusCwkFld)
	ON_EN_KILLFOCUS(IDC_ZMK_FLD, OnKillfocusZmkFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkKeyblockGeneral message handlers

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockGeneral::OnInitDialog() 
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

	//m_StrPadCharFld = _T("F");

	if (m_nDialogMode != MODE_DONE )
    {		
        EnableGeneralDialog(TRUE);	
		
        if ( m_nDialogMode != MODE_ADD )
        PopulateGeneralDialogFromDatabase (sNetworkkeyblockOld);
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
 
BOOL CNetworkKeyblockGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
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
    
BOOL CNetworkKeyblockGeneral::PopulateDatabaseFromGeneralDialog( pNCF01_KEYBLOCK psNetworkId )
{	
	memcpy ( psNetworkId->iss_cwk_keyblock,                          m_Striss_cwk_keyblockFld,               sizeof ( psNetworkId->iss_cwk_keyblock));
	memcpy ( psNetworkId->iss_zmk_keyblock,                m_Striss_zmk_keyblockFld,      sizeof ( psNetworkId->iss_zmk_keyblock));	
	memcpy ( psNetworkId->iss_rtdcwk_keyblock,                          m_Striss_rtdcwk_keyblockFld,               sizeof ( psNetworkId->iss_rtdcwk_keyblock));
	memcpy ( psNetworkId->iss_kek_keyblock,                         m_Striss_kek_keyblockFld,              sizeof ( psNetworkId->iss_kek_keyblock));
	memcpy ( psNetworkId->acq_cwk_keyblock,                     m_Stracq_cwk_keyblockFld,           sizeof ( psNetworkId->acq_cwk_keyblock));	
	memcpy ( psNetworkId->acq_zmk_keyblock,                 m_Stracq_zmk_keyblockFld,       sizeof ( psNetworkId->acq_zmk_keyblock));
	memcpy ( psNetworkId->acq_kek_keyblock,       m_Stracq_kek_keyblockFld,  sizeof ( psNetworkId->acq_kek_keyblock));
	memcpy ( psNetworkId->iss_prev_src_station,           m_Striss_prev_src_stationFld,      sizeof ( psNetworkId->iss_prev_src_station));
	
	/*memcpy ( psNetworkId->local_nsp_type,               m_StrLocalNspFld,          sizeof ( psNetworkId->local_nsp_type));	
	memcpy ( psNetworkId->station_id1,                  m_StrStationId1Fld,        sizeof ( psNetworkId->station_id1));
	memcpy ( psNetworkId->station_id2,                  m_StrStationId2Fld,        sizeof ( psNetworkId->station_id2));
	memcpy ( psNetworkId->tpdu_id,						m_StrTpduIDFld,            sizeof ( psNetworkId->tpdu_id));
	memcpy ( psNetworkId->que_name,						m_StrQueNameFld,           sizeof ( psNetworkId->que_name));
	memcpy ( psNetworkId->zmk,						    m_StrZmkFld,               sizeof ( psNetworkId->zmk));
	//memcpy ( psNetworkId->status,                       m_StrStatusCombo,          sizeof ( psNetworkId->status));
	 memcpy ( psNetworkId->prev_src_station,				m_StrKeyCheckFld,		   sizeof ( psNetworkId->prev_src_station));*/

	//if (m_StrStatusCombo == "ON LINE")
   //     memcpy ( strStatus, "0", sizeof (strStatus));
   // else if (m_StrStatusCombo == "OFF LINE")
   //      memcpy ( strStatus, "1", sizeof (strStatus));
//	else if (m_StrStatusCombo == "DOWN")
    //     memcpy ( strStatus, "2", sizeof (strStatus));
	
/*	if (m_StrEncrTypeCombo == "DES")
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



	if (m_CntlTotalsCheck.GetCheck() == 1)
		memcpy ( psNetworkId->totals_ind, "1", sizeof ( psNetworkId->totals_ind));
	else
		memcpy ( psNetworkId->totals_ind, "0", sizeof ( psNetworkId->totals_ind));*/

    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockGeneral::PopulateGeneralDialogFromDatabase( NCF01_KEYBLOCK sNetworkId )
{	 
    CString pin;    
    CString status;    
	CString encr;
	CString logon;                           
    CString statistics;                
	CString totals;  

    if ( m_nDialogMode != MODE_VIEW )
        EnableGeneralDialog(TRUE);
  
    ZeroMemory ( &sNetworkkeyblockOld, sizeof (NCF01_KEYBLOCK));  

	m_StrNetworkIdFld = sNetworkId.network_id;
	//m_StrNetworkTypeFld = sNetworkId.primary_key.network_type;

/*	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";*/

    m_Striss_cwk_keyblockFld        = sNetworkId.iss_cwk_keyblock;
	m_Striss_zmk_keyblockFld        = sNetworkId.iss_zmk_keyblock;
	m_Striss_rtdcwk_keyblockFld     = sNetworkId.iss_rtdcwk_keyblock;	
	m_Striss_kek_keyblockFld	    = sNetworkId.iss_kek_keyblock;
	m_Stracq_cwk_keyblockFld        = sNetworkId.acq_cwk_keyblock;
	m_Stracq_zmk_keyblockFld        = sNetworkId.acq_zmk_keyblock;
	m_Stracq_kek_keyblockFld        = sNetworkId.acq_kek_keyblock;
	m_Striss_prev_src_stationFld    = sNetworkId.iss_prev_src_station;

/*	m_StrLocalNspFld            = sNetworkId.local_nsp_type;	
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
		m_CntlTotalsCheck.SetCheck(1);*/
     
    UpdateData(FALSE);

    memcpy ( &sNetworkkeyblockOld, &sNetworkId, sizeof ( NCF01_KEYBLOCK));
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
							
BOOL CNetworkKeyblockGeneral::CompareStructures()
{	
	CString pin;    
    CString status;    
	CString encr;
	CString logon;                           
    CString statistics;                
	CString totals;  
	CString temp;

    if (strcmp ( m_Striss_cwk_keyblockFld, (char *)sNetworkkeyblockOld.iss_cwk_keyblock) != 0)
    {
        memcpy(m_StrCurrentData, m_Striss_cwk_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.iss_cwk_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ISS_CWK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_Striss_zmk_keyblockFld, (char *)sNetworkkeyblockOld.iss_zmk_keyblock) != 0)
    {
        memcpy(m_StrCurrentData, m_Striss_zmk_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.iss_zmk_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ISS_ZMK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_Striss_rtdcwk_keyblockFld, (char *)sNetworkkeyblockOld.iss_rtdcwk_keyblock) != 0)
    {
        memcpy(m_StrCurrentData, m_Striss_rtdcwk_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.iss_rtdcwk_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ISS_RTWD_CWK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_Striss_kek_keyblockFld, (char *)sNetworkkeyblockOld.iss_kek_keyblock) != 0)
    {
        memcpy(m_StrCurrentData, m_Striss_kek_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.iss_kek_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ISS_KEK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_Stracq_cwk_keyblockFld, (char *)sNetworkkeyblockOld.acq_cwk_keyblock) != 0)
    {  
        memcpy(m_StrCurrentData, m_Stracq_cwk_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.acq_cwk_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ACQ_CWK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
	}     
    if (strcmp ( m_Stracq_zmk_keyblockFld, (char *)sNetworkkeyblockOld.acq_zmk_keyblock) != 0)
    {  
        memcpy(m_StrCurrentData, m_Stracq_zmk_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.acq_zmk_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ACQ_ZMK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }  
    if (strcmp ( m_Stracq_kek_keyblockFld, (char *)sNetworkkeyblockOld.acq_kek_keyblock) != 0)
    {  
        memcpy(m_StrCurrentData, m_Stracq_kek_keyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.acq_kek_keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ACQ_KEK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_Striss_prev_src_stationFld, (char *)sNetworkkeyblockOld.iss_prev_src_station) != 0)
    {  
        memcpy(m_StrCurrentData, m_Striss_prev_src_stationFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkkeyblockOld.iss_prev_src_station, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "ISS_SRC", sizeof(m_StrFieldTag));

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

/*BOOL CNetworkGeneral::GetPin()
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
}*/

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

/*BOOL CNetworkGeneral::GetEncrType()
{
	if ( strcmp (m_StrEncrTypeCombo, "DES") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus));
	
    return TRUE;
}*/

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockGeneral::CleanGeneralDialog()
{
		m_StrNetworkIdFld = _T("");
		m_Striss_cwk_keyblockFld = _T("");
		m_Striss_zmk_keyblockFld = _T("");
		m_StrMessageLbl = _T("");
		m_Striss_rtdcwk_keyblockFld = _T("");
		m_Striss_kek_keyblockFld = _T("");
		m_Stracq_cwk_keyblockFld = _T("");
		m_Stracq_zmk_keyblockFld = _T("");
		m_Stracq_kek_keyblockFld = _T("");
		m_Striss_prev_src_stationFld = _T("");
		m_StrMessageLbl = _T("");


	UpdateData(FALSE);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockGeneral::EnableGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {	
  	    m_Cntliss_cwk_keyblockFld.EnableWindow(nStatus);
	    m_Cntliss_zmk_keyblockFld.EnableWindow(nStatus);
	    m_Cntliss_rtdcwk_keyblockFld.EnableWindow(nStatus);
	    m_Cntliss_kek_keyblockFld.EnableWindow(nStatus);

	    m_Cntlacq_cwk_keyblockFld.EnableWindow(nStatus);  	    	    
	    m_Cntlacq_zmk_keyblockFld.EnableWindow(nStatus);
	    m_Cntlacq_kek_keyblockFld.EnableWindow(nStatus);	    
		m_Cntliss_prev_src_stationFld.EnableWindow(nStatus);	   

	}
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockGeneral::PopulateHistoryTable()
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
    //strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
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
void CNetworkKeyblockGeneral::OnKillfocusZmkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length =0;

    if (!UpdateData(TRUE))
 		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	
	key_length = m_Striss_zmk_keyblockFld.GetLength();

	if(key_length == 0) /*Key Field can be left blank*/
		return;

	
	/*Check ZMK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	/*if(!myUtilsObject.IsHexEntry(m_StrZmkFld))
	{
		AfxMessageBox( "ZMK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", MB_ICONSTOP | MB_OK );
	 	m_CntlZmkFld.SetFocus();
	}*/

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if ( (key_length != 16) && 
		 (key_length != 32) && 
		 (key_length != 128) )
	{
		AfxMessageBox( "ZMK: The number of characters entered should be 128.", MB_ICONSTOP | MB_OK );
		m_Cntliss_zmk_keyblockFld.SetFocus();
	}

	return;
} /*end void CNetworkKeyblockGeneral::OnKillfocusZmkFld() */


/******************************************************************************************/
void CNetworkKeyblockGeneral::OnKillfocusCwkFld() 
{
	CMyUtils myUtilsObject;

	INT key_length = 0;
	INT return_code = 0;

	UpdateData(TRUE);

 	key_length = m_Striss_cwk_keyblockFld.GetLength();

	if(key_length == 0)  /*Key Field can be left blank*/
		return;
	
	/*Check CWK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	/*if(!myUtilsObject.IsHexEntry(m_StrCwkFld))
	{
		return_code = AfxMessageBox( "CWK: Invalid Hex Character Entered. Valid Characters are 0 - 9 and A - F.", MB_ICONSTOP | MB_OK );
 		m_CntlCwkFld.SetFocus();
	}*/

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	if ( (key_length != 16) && 
		 (key_length != 32) && 
		 (key_length != 128) )
	{
		return_code = AfxMessageBox( "CWK: The number of characters entered should be 128.", MB_ICONSTOP | MB_OK );
		m_Cntliss_cwk_keyblockFld.SetFocus();
	}

	return;

} /*end void CNetworkKeyblockGeneral::OnKillfocusCwkFld() */


