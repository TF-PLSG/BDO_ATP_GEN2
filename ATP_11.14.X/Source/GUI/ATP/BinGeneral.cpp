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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinGeneral.cpp  $
   
      Rev 1.19   Sep 28 2006 10:27:46   dirby
   Added a check box to the Bin Range GUI to indicate if a bin range
   is eligible or not for Fraudguard.  If not checked, then txns are
   sent to Fraudguard (if they meet other qualifications).  If checked,
   then txns will not go to Fraudguard (regardless of other qualifications).
   The new check box shares the Mod_Check field with the Mod10
   Check value and the Cash Bonus Eligible value.  Each of these
   three values is either ON or OFF, so they are represented by a
   single bit within the Mod_Check field.
   SCR 21851
   
      Rev 1.18   Oct 04 2005 12:05:40   dirby
   Added ELECTRON as a credit subtype.  This is for Visa Electron
   cards.  The bin range is being widened, so EB felt it would be
   easier to have a subtype so as not to restrict the bin range.
   SCR 17694
   
      Rev 1.17   Jul 21 2004 12:43:12   lmitchel
   SCR1388: Added new device type  = SMART CARD; dcf01.device_type  = '07'.  Added NONE option to encryption method in Keys dialog.
   
      Rev 1.16   Jul 16 2004 16:08:02   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.15   Dec 01 2003 11:28:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.14   Sep 11 2002 16:50:14   dirby
   Modified to support new Cash Bonus check box field.  No new
   database field was created.  This field shares the mod_check
   field in the database with the Mod10 Check GUI field.  Since
   these two fields are flags (on/off), there are only 4 different values
   this database field will have.
   SCR 804
   
      Rev 1.13   Aug 29 2002 12:49:44   dirby
   Modified to use Bin Type of Credit when looking up Auth Profile
   for Deferred card types.     SCR 551
   
      Rev 1.12   Aug 15 2002 17:10:32   dirby
   Modified to support a new bin type called DEFERRED.
   SCR 551
   
      Rev 1.11   Oct 31 2001 15:57:36   lmitchel
   correct assertion failure bin definition; set default to Mod Check = NONE; enable apply button on update for  acquirer blocking screen.
   
      Rev 1.10   Sep 28 2001 11:23:14   SYARLAGA
   Added code for Mod10 check.
   
      Rev 1.9   Mar 02 2000 11:56:04   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.8   Jan 25 2000 13:59:48   iarustam
   bug fixes
   
      Rev 1.7   Jan 13 2000 17:07:44   mbysani
   bug fixed
   
      Rev 1.2   Oct 15 1999 17:02:58   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:41:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:48   iarustam
   Initial Release
   
 
    
*
************************************************************************************/

// BinGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "BinGeneral.h"
#include "MyUtils.h"
#include "BinPropertySheet.h"
#include "NameList.h"
//#include "htmlhelp.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString name;
extern CSharedMem shrmem;
extern MIDBINBLK01	sMidBinBlkRecord_Global ;
extern MIDBINBLK01 sOldMidBinBlkRecord_Global;
/////////////////////////////////////////////////////////////////////////////
// CBinGeneral property page

IMPLEMENT_DYNCREATE(CBinGeneral, CPropertyPage)

CBinGeneral::CBinGeneral() : CPropertyPage(CBinGeneral::IDD)
{
	//{{AFX_DATA_INIT(CBinGeneral)
	m_StrAuthorizationFld = _T("");
	m_StrBinDescriptionFld = _T("");
	m_StrBinHighFld = _T("");
	m_StrBinLengthFld = _T("");
	m_StrBinLowFld = _T("");
	m_StrNetworkFld = _T("");
	m_StrTypeFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrPanLengthFld = _T("");
	m_StrCardTypeFld = _T("");
	m_StrSettleFileFld = _T("");
	m_StrAltDestinationNameFld = _T("");
	m_StrCardFamily = _T("");
	m_StrCreditSubtype = _T("");
	m_StrModCheck = _T("");
	//}}AFX_DATA_INIT
}

CBinGeneral::~CBinGeneral()
{
	
}

void CBinGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinGeneral)
	DDX_Control(pDX, IDC_BIN_FRAUDGUARD, m_BinFraudguardFld);
	DDX_Control(pDX, IDC_BIN_CASH_BONUS, m_BinCashBonusFld);
	DDX_Control(pDX, IDC_MOD_CHECK, m_CntlModCheck);
	DDX_Control(pDX, IDC_LOCAL_CHECK, m_LocalCheck);
	DDX_Control(pDX, IDC_REMOTE_CHECK, m_RemoteCheck);
	DDX_Control(pDX, IDC_CREDIT_SUBTYPE, m_CntlCreditSubtype);
	DDX_Control(pDX, IDC_CARD_FAMILY, m_CntlCardFamily);
	DDX_Control(pDX, IDC_ALT_DESTINATION_NAME_FLD, m_CntlAltDestinationNameFld);
	DDX_Control(pDX, IDC_SETTLE_FILE_FLD, m_CntlSettleFileFld);
	DDX_Control(pDX, IDC_CARD_TYPE_BUTTON, m_CardTypeButton);
	DDX_Control(pDX, IDC_CARD_TYPE, m_CntlCardType);
	DDX_Control(pDX, IDC_VIEW_NETWORK_BUTTON, m_NetworkButton);
	DDX_Control(pDX, IDC_VIEW_AUTH_BUTTON, m_AuthButton);
	DDX_Control(pDX, IDC_LOCAL_RADIO, m_LocalRadio);
	DDX_Control(pDX, IDC_PAN_LENGTH_FLD, m_CntlPanLengthFld);
	DDX_Control(pDX, IDC_REMOTE_RADIO, m_RemoteRadio);
	DDX_Control(pDX, IDC_NETWORK_FLD, m_CntlNetworkFld);
	DDX_Control(pDX, IDC_BIN_LOW_FLD, m_CntlBinLowFld);
	DDX_Control(pDX, IDC_BIN_LENGTH_FLD, m_CntlBinLengthFld);
	DDX_Control(pDX, IDC_BIN_HIGH_FLD, m_CntlBinHighFld);
	DDX_Control(pDX, IDC_BIN_DESCR_FLD, m_CntlBinDescriptionFld);
	DDX_Control(pDX, IDC_AUTHORIZATION_FLD, m_CntlAuthorizationFld);
	DDX_Text(pDX, IDC_AUTHORIZATION_FLD, m_StrAuthorizationFld);
	DDV_MaxChars(pDX, m_StrAuthorizationFld, 4);
	DDX_Text(pDX, IDC_BIN_DESCR_FLD, m_StrBinDescriptionFld);
	DDV_MaxChars(pDX, m_StrBinDescriptionFld, 30);
	DDX_Text(pDX, IDC_BIN_HIGH_FLD, m_StrBinHighFld);
	DDV_MaxChars(pDX, m_StrBinHighFld, 16);
	DDX_Text(pDX, IDC_BIN_LENGTH_FLD, m_StrBinLengthFld);
	DDV_MaxChars(pDX, m_StrBinLengthFld, 2);
	DDX_Text(pDX, IDC_BIN_LOW_FLD, m_StrBinLowFld);
	DDX_Text(pDX, IDC_NETWORK_FLD, m_StrNetworkFld);
	DDV_MaxChars(pDX, m_StrNetworkFld, 10);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrTypeFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PAN_LENGTH_FLD, m_StrPanLengthFld);
	DDV_MaxChars(pDX, m_StrPanLengthFld, 2);
	DDX_Text(pDX, IDC_CARD_TYPE, m_StrCardTypeFld);
	DDV_MaxChars(pDX, m_StrCardTypeFld, 4);
	DDX_Text(pDX, IDC_SETTLE_FILE_FLD, m_StrSettleFileFld);
	DDV_MaxChars(pDX, m_StrSettleFileFld, 6);
	DDX_Text(pDX, IDC_ALT_DESTINATION_NAME_FLD, m_StrAltDestinationNameFld);
	DDV_MaxChars(pDX, m_StrAltDestinationNameFld, 10);
	DDX_CBString(pDX, IDC_CARD_FAMILY, m_StrCardFamily);
	DDX_CBString(pDX, IDC_CREDIT_SUBTYPE, m_StrCreditSubtype);
	DDX_CBString(pDX, IDC_MOD_CHECK, m_StrModCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CBinGeneral)
	ON_BN_CLICKED(IDC_VIEW_AUTH_BUTTON, OnViewAuthButton)
	ON_BN_CLICKED(IDC_LOCAL_RADIO, OnLocalRadio)
	ON_BN_CLICKED(IDC_REMOTE_RADIO, OnRemoteRadio)
	ON_BN_CLICKED(IDC_CARD_TYPE_BUTTON, OnCardTypeButton)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_VIEW_NETWORK_BUTTON, OnViewNetworkButton)
	ON_BN_CLICKED(IDC_LOCAL_CHECK, OnLocalCheck)
	ON_BN_CLICKED(IDC_REMOTE_CHECK, OnRemoteCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinGeneral message handlers


BOOL CBinGeneral::OnInitDialog() 
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

	m_LocalRadio.SetCheck(1);
   m_LocalCheck.SetCheck(1);
	m_CntlNetworkFld.EnableWindow(FALSE);

	if (m_nDialogMode != MODE_DONE)
	{	
		if (m_nDialogMode != MODE_VIEW)
			EnableBinDialog(TRUE);
		SetBinType();
	
	}    
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CBinGeneral::SetBinType()
{
    if (m_nDialogMode != MODE_VIEW)
       EnableBinDialog(TRUE);
    if (strcmp(strType,"C") == 0)
    {
       m_StrTypeFld = "CREDIT";
    }
    if (strcmp(strType, "D") == 0)
    {
        m_StrTypeFld = "DEBIT";
    }
    if (strcmp(strType, "E") == 0)
    {
        m_StrTypeFld = "DEFERRED";
    }
    if (strcmp(strType, "F") == 0)
    {
        m_StrTypeFld = "FLEET";
    }
    if (strcmp(strType, "L") == 0)
    {
        m_StrTypeFld = "LOYALTY";
    }

   UpdateData(FALSE);
}

BOOL CBinGeneral::GetBinType()
{
	if ( strcmp (m_StrTypeFld, "CREDIT") == 0)
         memcpy ((LPTSTR) strType,	"C", sizeof(strType)); 
	if ( strcmp (m_StrTypeFld, "DEBIT")== 0)
         memcpy ((LPTSTR) strType,	"D", sizeof(strType)); 
	if ( strcmp (m_StrTypeFld, "DEFERRED") == 0)
         memcpy ((LPTSTR) strType,	"E", sizeof(strType)); 
	if ( strcmp (m_StrTypeFld, "FLEET") == 0)   
          memcpy ((LPTSTR) strType,	"F", sizeof(strType));
	if ( strcmp (m_StrTypeFld, "LOYALTY")== 0)
          memcpy ((LPTSTR) strType,	"L", sizeof(strType)); 
	
	return TRUE;
}


void CBinGeneral::EnableBinDialog(int nStatus)
{	    
	if ( m_nDialogMode != MODE_EDIT )
		m_CntlBinHighFld.EnableWindow(nStatus);
	
//	if ( m_nDialogMode != MODE_ADD )
//		m_CntlNetworkFld.EnableWindow(nStatus);

	m_CntlAuthorizationFld.EnableWindow(nStatus);
	m_CntlBinLengthFld.EnableWindow(nStatus);
	m_CntlPanLengthFld.EnableWindow(nStatus);
    m_CntlCardType.EnableWindow(nStatus);
	m_CntlBinDescriptionFld.EnableWindow(nStatus);
	m_LocalRadio.EnableWindow(nStatus);
	m_RemoteRadio.EnableWindow(nStatus);
	m_LocalCheck.EnableWindow(nStatus);
	m_RemoteCheck.EnableWindow(nStatus);
//	m_CntlCardType.SetCurSel(0);
	m_AuthButton.EnableWindow(nStatus);
	m_NetworkButton.EnableWindow(nStatus);
	m_CardTypeButton.EnableWindow(nStatus);
	//m_CntlSettleTimeFld.EnableWindow(nStatus);
	m_CntlSettleFileFld.EnableWindow(nStatus);
//	m_CardPrefixButton.EnableWindow(nStatus);
    m_CntlAltDestinationNameFld.EnableWindow(nStatus);
	m_CntlCardFamily.EnableWindow(nStatus);
	m_CntlCreditSubtype.EnableWindow(nStatus);
	m_CntlModCheck.EnableWindow(nStatus);

}


BOOL CBinGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW && m_nDialogMode != MODE_DONE)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CBinGeneral::CleanBinDialog()
{
	m_StrAuthorizationFld = "";
	m_StrBinHighFld = "";
	m_StrBinLengthFld = "";
	m_StrNetworkFld = "";
	m_StrPanLengthFld = "";
   m_StrCardTypeFld = "";
	m_StrTypeFld = "";	
	m_StrBinDescriptionFld = "";
	m_StrBinLowFld = "";	
	m_StrCardTypeFld = "";
//	m_LocalRadio.SetCheck(1);
   m_LocalCheck.SetCheck(1);
   m_RemoteCheck.SetCheck(0);
	m_CntlNetworkFld.EnableWindow(FALSE);
	m_StrMessageLbl = "";
	m_StrSettleFileFld = "";
	m_StrAltDestinationNameFld = _T("");
	m_StrCardFamily = "AMEX";
	m_StrCreditSubtype = _T("");
	m_StrModCheck = "NONE";
   m_BinCashBonusFld.SetCheck(0);

	UpdateData (FALSE);


}

void CBinGeneral::OnViewAuthButton() 
{
	CNameList listDlg;
    int nReturnCode;

	UpdateData (TRUE);
	m_StrMessageLbl = "";

    listDlg.m_nIndicator = MODE_AUTH;
    listDlg.strPrimaryKey1 = m_StrAuthorizationFld;
	UpdateData(TRUE);
	GetBinType();
   if ( 0 == strcmp(strType, "E") )
      strcpy( strType, "C" );
	listDlg.strBinType = strType;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrAuthorizationFld = name;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

BOOL CBinGeneral::CheckDialogForCompleteness()
{

	CMyUtils myUtilsObject;
	int nLowLength;
	int nHighLength;
	int nBinLength;
	int nCurLength;
	int nPanLength;
	CAF01 sAuthRecord;
	CTF01 sCardTypeRecord;
	NCF01 sNetworkRecord;
	char strErrorMessage [200] = "";

	ZeroMemory ( &sAuthRecord, sizeof (CAF01));

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrBinHighFld, this))
    {
		m_StrMessageLbl = "Please enter BIN High";	
		m_CntlBinHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrPanLengthFld, this))
    {
		m_StrMessageLbl = "Please enter PAN Length";	
		m_CntlPanLengthFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

//	nHigh = atoi ( m_StrBinHighFld);
//	nLow = atoi ( m_StrBinLowFld );

	if ( m_StrBinHighFld < m_StrBinLowFld )
	{
		m_StrMessageLbl = "ERROR: BIN High cannot be less than BIN Low";	
		m_CntlBinHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	nHighLength = m_StrBinHighFld.GetLength();
	nLowLength = m_StrBinLowFld.GetLength();

	if ( nHighLength != nLowLength )
	{
		m_StrMessageLbl = "ERROR: Incorrect number of digits in BIN Low/High";	
		m_CntlBinHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
	nPanLength = atoi ( m_StrPanLengthFld );
	nBinLength = atoi ( m_StrBinLengthFld );
	
	if (nPanLength < nBinLength && nPanLength != 0)
    {
		m_StrMessageLbl = "ERROR: PAN Length cannot be less than BIN Length";	
		m_CntlPanLengthFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
  
	if ( nPanLength > 20 )
	{
		m_StrMessageLbl = "ERROR: BIN Length must be less than 20";	
		m_CntlPanLengthFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	nCurLength = m_StrBinHighFld.GetLength();

	if ( nCurLength != nBinLength )
	{
		m_StrMessageLbl = "ERROR: Incorrect number of digits in BIN High";	
		m_CntlBinHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	GetBinType();
	if ( /*m_LocalRadio.GetCheck() == 1*/ m_LocalCheck.GetCheck() == 1)
	{
		if (!myUtilsObject.IsEmpty (m_StrAuthorizationFld, this))
		{
         if ( 0 == strcmp(strType, "E") )
            strcpy( strType, "C" );
			memcpy ( &sAuthRecord.primary_key.caf_profile_id, m_StrAuthorizationFld, sizeof (sAuthRecord.primary_key.caf_profile_id));
			memcpy ( &sAuthRecord.primary_key.type, strType, sizeof (sAuthRecord.primary_key.type));
			int rcode  = txdsapi_get_record ((pBYTE)&sAuthRecord, sizeof (CAF01), CAF01_DATA, strErrorMessage);
			if (rcode != PTEMSG_OK)
			{
					m_StrMessageLbl = "ERROR: Invalid Authorization Profile ID";			
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					m_CntlAuthorizationFld.SetFocus();
					m_nDialogMode = MODE_DONE;
					return FALSE;
			}
		}
		else 
		{
					m_StrMessageLbl = "Please enter Authorization Profile ID";			
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					m_CntlAuthorizationFld.SetFocus();
					m_nDialogMode = MODE_DONE;
					return FALSE;

		}
	}

	if ( m_RemoteCheck.GetCheck() == 1 )
	if (!myUtilsObject.IsEmpty (m_StrNetworkFld, this))
    {
		memcpy ( &sNetworkRecord.primary_key.network_id, m_StrNetworkFld, sizeof (sNetworkRecord.primary_key.network_id));
		memcpy ( &sNetworkRecord.primary_key.network_type, "I", sizeof (sNetworkRecord.primary_key.network_type));
		int rcode  = txdsapi_get_record ((pBYTE)&sNetworkRecord, sizeof (NCF01), NCF01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{
				m_StrMessageLbl = "ERROR: Invalid Network ID";			
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				m_CntlNetworkFld.SetFocus();
				m_nDialogMode = MODE_DONE;
				return FALSE;
		}
	}
		else 
		{
					m_StrMessageLbl = "Please enter Network ID";			
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					m_CntlNetworkFld.SetFocus();
					m_nDialogMode = MODE_DONE;
					return FALSE;

		}

		memcpy ( &sCardTypeRecord.primary_key.card_type, m_StrCardTypeFld, sizeof (CTF01));
	int rcode = txdsapi_get_record ((pBYTE)&sCardTypeRecord, sizeof (CTF01), CTF01_DATA, strErrorMessage);
	if (rcode !=  PTEMSG_OK)
	{
			m_StrMessageLbl = "ERROR: Invalid Card Type";			
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			m_CntlCardType.SetFocus();
			m_nDialogMode = MODE_DONE;
			return FALSE;
	}


    return TRUE;

}

BOOL CBinGeneral::OnApply() 
{
//	if ( !CheckDialogForCompleteness() )
//		return FALSE;
	
	return CPropertyPage::OnApply();
}

BOOL CBinGeneral::PopulateBinDialogFromDatabase ( BIN01 sBinRecord )
{
	if ( m_nDialogMode != MODE_VIEW )	
		EnableBinDialog(TRUE);

	CString strAuthInd;

	m_StrAuthorizationFld = sBinRecord.caf_profile_id;
	m_StrBinHighFld = sBinRecord.high;
	m_StrNetworkFld = sBinRecord.destination_name;
	m_StrPanLengthFld = sBinRecord.pan_length;
    m_StrCardTypeFld = sBinRecord.card_type;
	m_StrBinDescriptionFld = sBinRecord.description;
	m_StrSettleFileFld = sBinRecord.settle_file_prefix;
	m_StrAltDestinationNameFld = sBinRecord.alt_destination_name;
	m_StrCardFamily = sBinRecord.card_family;
	m_StrCreditSubtype = sBinRecord.credit_subtype;
	if ( m_StrCreditSubtype == "F" )
		m_StrCreditSubtype = "FLEET";
	else if ( m_StrCreditSubtype == "L" )
		m_StrCreditSubtype = "LOYALTY";
	else if ( m_StrCreditSubtype == "G" )
		m_StrCreditSubtype = "GUARANTOR";
	else if ( m_StrCreditSubtype == "P" )
		m_StrCreditSubtype = "PREPAID";
	else if ( m_StrCreditSubtype == "E" )
		m_StrCreditSubtype = "ELECTRON";

	if ( m_StrCardFamily == CARD_AMEX )
		m_StrCardFamily = "AMEX";
	else if ( m_StrCardFamily == CARD_VISA )
		m_StrCardFamily = "VISA";
	else if ( m_StrCardFamily == CARD_MASTERCARD )
		m_StrCardFamily = "MASTERCARD";
	else if ( m_StrCardFamily == CARD_JCB )
		m_StrCardFamily = "JCB";
	else if ( m_StrCardFamily == CARD_CUP )
		m_StrCardFamily = "CUP";
	else if ( m_StrCardFamily == CARD_DCC )
		m_StrCardFamily = "DCC";
	else if ( m_StrCardFamily == CARD_DINERS )
		m_StrCardFamily = "DINERS";
	else if ( m_StrCardFamily == CARD_DUAL )
		m_StrCardFamily = "DUAL";
	else if ( m_StrCardFamily == CARD_FLEET )
		m_StrCardFamily = "FLEET";


   /* Get Mod10, Cash Bonus, and Fraudguard values from the same DB field.
    * Use masks to determine if they are set or not.
    */
   if ( (MOD10_ON & sBinRecord.mod_check[0]) == MOD10_ON )
      m_StrModCheck = "MOD10";
   else
      m_StrModCheck = "NONE";

   if ( (CASH_ON & sBinRecord.mod_check[0]) == CASH_ON )
      m_BinCashBonusFld.SetCheck(1);
   else
      m_BinCashBonusFld.SetCheck(0);

   if ( (FGUARD_ON & sBinRecord.mod_check[0]) == FGUARD_ON )
      m_BinFraudguardFld.SetCheck(1);
   else
      m_BinFraudguardFld.SetCheck(0);


   /* Determine Remote or Local */
   strAuthInd = sBinRecord.auth_ind;
   if ( strAuthInd == "1" )
   {
      m_RemoteCheck.SetCheck(0);
      m_LocalCheck.SetCheck(1);
   }
   else if ( strAuthInd == "0" )
   {
      m_RemoteCheck.SetCheck(1);
      m_LocalCheck.SetCheck(0);
   }

	if ( m_nDialogMode != MODE_VIEW )
	{
		if ( /*m_LocalRadio.GetCheck() == 1*/m_LocalCheck.GetCheck() == 1)
		{
			m_NetworkButton.EnableWindow(FALSE);
			m_CntlAuthorizationFld.EnableWindow(TRUE);
			m_CntlNetworkFld.EnableWindow(FALSE);
		}

		if ( /*m_RemoteRadio.GetCheck() == 1*/m_RemoteCheck.GetCheck() == 1)
		{
			m_AuthButton.EnableWindow(FALSE);
			m_CntlAuthorizationFld.EnableWindow(FALSE);
			m_CntlNetworkFld.EnableWindow(TRUE);
		}
	}


	UpdateData(FALSE);

	return TRUE;

}

void CBinGeneral::OnLocalRadio() 
{
	UpdateData(TRUE);
	m_StrNetworkFld = "";
	
	m_CntlAuthorizationFld.EnableWindow(TRUE);
	m_CntlNetworkFld.EnableWindow(FALSE);
	m_AuthButton.EnableWindow(TRUE);
	m_NetworkButton.EnableWindow(FALSE);
	UpdateData(FALSE);
//	m_LocalRadio.SetCheck(0);
   m_LocalCheck.SetCheck(0);
//	m_RemoteRadio.SetCheck(1);
	m_RemoteCheck.SetCheck(1);

}

void CBinGeneral::OnRemoteRadio() 
{
	UpdateData(TRUE);
	m_StrAuthorizationFld = "";
	m_CntlAuthorizationFld.EnableWindow(FALSE);
	m_CntlNetworkFld.EnableWindow(TRUE);
	m_AuthButton.EnableWindow(FALSE);
	m_NetworkButton.EnableWindow(TRUE);

	UpdateData(FALSE);
//	m_LocalRadio.SetCheck(0);
   m_LocalCheck.SetCheck(0);
//	m_RemoteRadio.SetCheck(1);
	m_RemoteCheck.SetCheck(1);
	/*UpdateData(TRUE);
	m_StrMessageLbl = "ERROR: No network defined";			
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
	m_LocalRadio.SetCheck(1);
	m_RemoteRadio.SetCheck(0);*/
}

void CBinGeneral::OnCardTypeButton() 
{
	CNameList listDlg;
    int nReturnCode;

	UpdateData (TRUE);
	m_StrMessageLbl = "";

    listDlg.m_nIndicator = MODE_CARD;
    listDlg.strPrimaryKey1 = m_StrCardTypeFld;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrCardTypeFld = name;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}


BOOL CBinGeneral::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"bin.chm>bin",HH_HELP_CONTEXT,IDD_BIN_GENERAL);	*/
	return TRUE;
}

BOOL CBinGeneral::OnSetActive() 
{
	((CBinPropertySheet*) this->GetParent())->m_nActiveTab = 1;
   if ( m_nDialogMode == MODE_ADD )
      m_CntlCardFamily.SetCurSel(0);
      m_CntlModCheck.SetCurSel(0);
	
	return CPropertyPage::OnSetActive();
}

void CBinGeneral::OnViewNetworkButton() 
{
	CNameList listDlg;
    int nReturnCode;

	UpdateData (TRUE);
	m_StrMessageLbl = "";

    listDlg.m_nIndicator = MODE_NETWORK;
    listDlg.strPrimaryKey1 = m_StrNetworkFld;
	listDlg.strNetworkType = "I";

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrNetworkFld = name;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CBinGeneral::OnLocalCheck() 
{
	UpdateData(TRUE);
	m_StrNetworkFld = "";
	
	m_CntlAuthorizationFld.EnableWindow(TRUE);
	m_CntlNetworkFld.EnableWindow(FALSE);
	m_AuthButton.EnableWindow(TRUE);
	m_NetworkButton.EnableWindow(FALSE);
	UpdateData(FALSE);
   m_LocalCheck.SetCheck(1);
	m_RemoteCheck.SetCheck(0);

	
}

void CBinGeneral::OnRemoteCheck() 
{
	UpdateData(TRUE);
	m_StrAuthorizationFld = "";
	m_CntlAuthorizationFld.EnableWindow(FALSE);
	m_CntlNetworkFld.EnableWindow(TRUE);
	m_AuthButton.EnableWindow(FALSE);
	m_NetworkButton.EnableWindow(TRUE);

	UpdateData(FALSE);
   m_LocalCheck.SetCheck(0);
	m_RemoteCheck.SetCheck(1);	
}
