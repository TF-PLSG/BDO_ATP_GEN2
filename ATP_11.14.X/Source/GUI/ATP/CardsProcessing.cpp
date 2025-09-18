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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CardsProcessing.cpp  $
   
      Rev 1.24   Dec 07 2005 16:53:46   dirby
   Added option "YES - EXPANDED" to the Expiration Date Check field.
   This will go into the database CAF01 table as 'E'.
   SCR 18700
   
      Rev 1.23   Jun 08 2005 18:12:14   dirby
   Added code to populate CVV Processing dialog from the database.
   This code was accidentally removed in a previous release.
   SCR 15722
   
      Rev 1.22   Apr 13 2005 09:53:52   lmitchel
   Enabled history logging for new country amt limits fields
   
      Rev 1.21   Apr 08 2005 08:47:14   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.20   Apr 01 2005 10:13:34   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.19   Jun 04 2004 11:03:18   lmitchel
   Modified merchant limits table in Authorization Control to add/update/insert MCC limits including new  amount field.  Maximum limit of records changed to 500 from 50.
   
      Rev 1.18   Dec 01 2003 11:28:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.17   Apr 17 2000 09:57:06   iarustam
   added bin blocking
   
   
      Rev 1.16   Mar 02 2000 11:56:18   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.15   Feb 11 2000 16:48:28   iarustam
   SCR #400,401
   
      Rev 1.14   Jan 21 2000 12:22:50   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.13   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.4   Oct 15 1999 17:07:22   iarustam
   bug fixes
   
      Rev 1.3   Oct 06 1999 16:46:00   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.2   Sep 30 1999 16:17:00   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:45:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:08   iarustam
   Initial Release
   

    
*
************************************************************************************/
// CardsProcessing.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsProcessing.h"
#include "CardsPinVerification.h"
#include "CardsPinVerifVisa.h"
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
extern CString name;
extern CSharedMem shrmem;


/////////////////////////////////////////////////////////////////////////////
// CCardsProcessing property page

IMPLEMENT_DYNCREATE(CCardsProcessing, CPropertyPage)

CCardsProcessing::CCardsProcessing() : CPropertyPage(CCardsProcessing::IDD)
{
	//{{AFX_DATA_INIT(CCardsProcessing)
	m_StrAuthLocation_Fld = _T("");
	m_StrAuthMethod_Fld = _T("");
	m_StrContact_Fld = _T("");
	m_StrExpDateCheck_Fld = _T("");
	m_StrExpDateLocation_Fld = _T("");
	m_StrName_Fld = _T("");
	m_StrNumPinRetries_Fld = _T("");
	m_StrPhone_Fld = _T("");
	m_Str3DS_Fld = _T("");
	m_StrPinCapIndicator_Fld = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrRemoteDest_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMerchSame_Fld = _T("");
	m_StrStatus_Fld = _T("");
	m_StrPinVerifMethod_Fld = _T("");
	m_StrProcess_Fld = _T("");
	m_StrCVKAFld = _T("");
	m_StrCVKBFld = _T("");
	m_StrCVVLengthFld = _T("");
	m_StrCVVOffset1Fld = _T("");
	m_StrCVVOffset2Fld = _T("");
	m_StrCodeOffset1Fld = _T("");
	m_StrCodeOffset2Fld = _T("");
	m_StrCVVProcessing = _T("");
	m_StrCSCKFld = _T("");
	m_StrPinOffsetLocation_Fld = _T("");
	m_StrFloorLimitFld = _T("");
	m_StrMKACFld = _T("");
	m_Str3DCAKFld = _T("");
	m_pPosDlg = NULL;
	m_StrMCCAKFld = _T("");
	m_StrPINValidationData = _T("");
	m_StrDecimalisationTable = _T("");
	m_StrKey3ds01 =_T("");
	//}}AFX_DATA_INIT
}

CCardsProcessing::~CCardsProcessing()
{
}

void CCardsProcessing::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsProcessing)

	DDX_Control(pDX, IDC_MCCAK_FLD2, m_CntlMCCAK_Fld);
	DDX_Control(pDX, IDC_3DCAK_FLD, m_Cntl3DCAKFld);
	DDX_Control(pDX, IDC_CSCK_DESC, m_CntlCSCKDesc);
	DDX_Control(pDX, IDC_SLEN_3DCAK, m_CntlSlen3Dcak);
	DDX_Control(pDX, IDC_POS_ENTRY_MODES, m_CntlPosEntryModes);
	DDX_Control(pDX, IDC_CNTRY_LMT_CHECK, m_CntlCntryLimitsCheckFlag);
	DDX_Control(pDX, IDC_MC3D_PAN, m_Cntlmc3dpanFlag);
	DDX_Control(pDX, IDC_MC3D_DS_TXN_ID, m_Cntlmc3ddstxnFlag);
	DDX_Control(pDX, IDC_MC3D_CURRENCY, m_Cntlmc3dcurrencyFlag);
	DDX_Control(pDX, IDC_MC3D_TXN_AMT, m_Cntlmc3dtxnamtFlag);
	DDX_Control(pDX, IDC_MC3D_MERCH_NAME, m_Cntlmc3dmechnameFlag);
	DDX_Control(pDX, IDC_FLOOR_LIMIT_FLD, m_CntlFloorLimitFld);
	DDX_Control(pDX, IDC_CSCK_FLD, m_CntlCSCKFld);
	DDX_Control(pDX, IDC_3DS2KEY_FLD, m_CntlKey3ds01);
	DDX_Control(pDX, IDC_MKAC_FLD, m_CntlMKACFld);
	DDX_Control(pDX, IDC_CVV_PROCESSING, m_CntlCVVProcessing);
	DDX_Control(pDX, IDC_SERVICE_CODE1_FLD, m_CntlCodeOffset1Fld);
	DDX_Control(pDX, IDC_SERVICE_CODE2_FLD, m_CntlCodeOffset2Fld);
	DDX_Control(pDX, IDC_CVV_OFFSET2_FLD, m_CntlCVVOffset2Fld);
	DDX_Control(pDX, IDC_CVV_OFFSET1_FLD, m_CntlCVVOffset1Fld);
	DDX_Control(pDX, IDC_CVV_LENGTH_FLD, m_CntlCVVLengthFld);
	DDX_Control(pDX, IDC_CVKB_FLD, m_CntlCVKBFld);
	DDX_Control(pDX, IDC_CVKA_FLD, m_CntlCVKAFld);
	DDX_Control(pDX, IDC_NETWORK_BUTTON, m_NetworkButton);
	DDX_Control(pDX, IDC_PIN_VERIF_BUTTON, m_PinVerifButton);
	DDX_Control(pDX, IDC_NSP_PROCESS_FLD, m_CntlProcess_Fld);
	DDX_Control(pDX, IDC_PIN_VERIFICATION_METHOD_FLD, m_CntlPinVerifMethod_Fld);
	DDX_Control(pDX, IDC_STATUS_FLD, m_CntlStatus_Fld);
	DDX_Control(pDX, IDC_MERCH_SAME_FLD, m_CntlMerchSame_Fld);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlType_Fld);
	DDX_Control(pDX, IDC_REMOTE_DESTINATION_FLD, m_CntlRemoteDest_Fld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileId_Fld);
	DDX_Control(pDX, IDC_PIN_OFFSET_LOCATION_FLD, m_CntlPinOffsetLocation_Fld);
	DDX_Control(pDX, IDC_PIN_CAPTURE_INDICATOR_FLD, m_CntlPinCapIndicator_Fld);
	DDX_Control(pDX, IDC_PHONE_FLD, m_CntlPhone_Fld);
	DDX_Control(pDX, IDC_3DS2_KEY, m_Cntl3DS_Fld);
	DDX_Control(pDX, IDC_NUMBER_0F_PIN_RETRIES_FLD, m_CntlNumPinRetries_Fld);
	DDX_Control(pDX, IDC_NAME_FLD, m_CntlName_Fld);
	DDX_Control(pDX, IDC_EXPIRATION_DATE_LOCATION_FLD, m_CntlExpDateLocation_Fld);
	DDX_Control(pDX, IDC_EXPIRATION_DATE_CHECK_FLD, m_CntlExpDateCheck_Fld);
	DDX_Control(pDX, IDC_CONTACT_FLD, m_CntlContact_Fld);
	DDX_Control(pDX, IDC_AUTHORIZATION_METHOD_FLD, m_CntlAuthMethod_Fld);
	DDX_Control(pDX, IDC_AUTHORIZATION_LOCATION_FLD, m_CntlAuthLocation_Fld);
	DDX_Control(pDX, IDC_PIN_VALIDATION_DATA, m_CntlPINValidationData);
	DDX_Control(pDX, IDC_DECIMALISATION_TABLE, m_CntlDecimalisationTable);
	DDX_CBString(pDX, IDC_AUTHORIZATION_LOCATION_FLD, m_StrAuthLocation_Fld);
	DDX_CBString(pDX, IDC_AUTHORIZATION_METHOD_FLD, m_StrAuthMethod_Fld);
	DDX_Text(pDX, IDC_CONTACT_FLD, m_StrContact_Fld);
	DDV_MaxChars(pDX, m_StrContact_Fld, 20);
	DDX_CBString(pDX, IDC_EXPIRATION_DATE_CHECK_FLD, m_StrExpDateCheck_Fld);
	DDX_CBString(pDX, IDC_EXPIRATION_DATE_LOCATION_FLD, m_StrExpDateLocation_Fld);
	DDX_Text(pDX, IDC_NAME_FLD, m_StrName_Fld);
	DDV_MaxChars(pDX, m_StrName_Fld, 30);
	DDX_Text(pDX, IDC_NUMBER_0F_PIN_RETRIES_FLD, m_StrNumPinRetries_Fld);
	DDV_MaxChars(pDX, m_StrNumPinRetries_Fld, 1);
	DDX_Text(pDX, IDC_PHONE_FLD, m_StrPhone_Fld);
	DDV_MaxChars(pDX, m_StrPhone_Fld, 17);
	DDX_Text(pDX, IDC_3DS2_KEY, m_Str3DS_Fld);
	DDV_MaxChars(pDX, m_Str3DS_Fld, 32);
	DDX_CBString(pDX, IDC_PIN_CAPTURE_INDICATOR_FLD, m_StrPinCapIndicator_Fld);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);
	DDX_Text(pDX, IDC_REMOTE_DESTINATION_FLD, m_StrRemoteDest_Fld);
	DDV_MaxChars(pDX, m_StrRemoteDest_Fld, 10);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrType_Fld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_MERCH_SAME_FLD, m_StrMerchSame_Fld);
	DDV_MaxChars(pDX, m_StrMerchSame_Fld, 2);
	DDX_CBString(pDX, IDC_STATUS_FLD, m_StrStatus_Fld);
	DDX_CBString(pDX, IDC_PIN_VERIFICATION_METHOD_FLD, m_StrPinVerifMethod_Fld);
	DDX_Text(pDX, IDC_NSP_PROCESS_FLD, m_StrProcess_Fld);
	DDV_MaxChars(pDX, m_StrProcess_Fld, 15);
	DDX_Text(pDX, IDC_CVKA_FLD, m_StrCVKAFld);
	DDV_MaxChars(pDX, m_StrCVKAFld, 16);
	DDX_Text(pDX, IDC_CVKB_FLD, m_StrCVKBFld);
	DDV_MaxChars(pDX, m_StrCVKBFld, 16);
	DDX_Text(pDX, IDC_CVV_LENGTH_FLD, m_StrCVVLengthFld);
	DDV_MaxChars(pDX, m_StrCVVLengthFld, 1);
	DDX_Text(pDX, IDC_CVV_OFFSET1_FLD, m_StrCVVOffset1Fld);
	DDV_MaxChars(pDX, m_StrCVVOffset1Fld, 2);
	DDX_Text(pDX, IDC_CVV_OFFSET2_FLD, m_StrCVVOffset2Fld);
	DDV_MaxChars(pDX, m_StrCVVOffset2Fld, 2);
	DDX_Text(pDX, IDC_SERVICE_CODE1_FLD, m_StrCodeOffset1Fld);
	DDV_MaxChars(pDX, m_StrCodeOffset1Fld, 2);
	DDX_Text(pDX, IDC_SERVICE_CODE2_FLD, m_StrCodeOffset2Fld);
	DDV_MaxChars(pDX, m_StrCodeOffset2Fld, 2);
	DDX_CBString(pDX, IDC_CVV_PROCESSING, m_StrCVVProcessing);
	DDX_Text(pDX, IDC_CSCK_FLD, m_StrCSCKFld);
	DDV_MaxChars(pDX, m_StrCSCKFld, 32);
	DDX_CBString(pDX, IDC_PIN_OFFSET_LOCATION_FLD, m_StrPinOffsetLocation_Fld);
	DDX_Text(pDX, IDC_FLOOR_LIMIT_FLD, m_StrFloorLimitFld);
	DDV_MaxChars(pDX, m_StrFloorLimitFld, 10);
	DDX_Text(pDX, IDC_MKAC_FLD, m_StrMKACFld);
	DDV_MaxChars(pDX, m_StrMKACFld, 32);
	DDX_Text(pDX, IDC_3DCAK_FLD, m_Str3DCAKFld);
	DDV_MaxChars(pDX, m_Str3DCAKFld, 32);
	DDX_Text(pDX, IDC_MCCAK_FLD2, m_StrMCCAKFld);
	DDV_MaxChars(pDX, m_StrMCCAKFld, 32);
	DDX_Text(pDX, IDC_PIN_VALIDATION_DATA, m_StrPINValidationData);
	DDV_MaxChars(pDX, m_StrPINValidationData, 12);
	DDX_Text(pDX, IDC_DECIMALISATION_TABLE, m_StrDecimalisationTable);
	DDV_MaxChars(pDX, m_StrDecimalisationTable, 16);
	DDX_Text(pDX, IDC_3DS2KEY_FLD, m_StrKey3ds01);
	DDV_MaxChars(pDX, m_StrKey3ds01, 80);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsProcessing, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsProcessing)
	ON_BN_CLICKED(IDC_PIN_VERIF_BUTTON, OnPinVerifButton)
	ON_CBN_SELCHANGE(IDC_PIN_VERIFICATION_METHOD_FLD, OnSelchangePinVerificationMethodFld)
	ON_BN_CLICKED(IDC_NETWORK_BUTTON, OnNetworkButton)
	ON_BN_CLICKED(IDC_POS_ENTRY_MODES, OnPosEntryModes)
	ON_BN_CLICKED(IDC_POS_ENTRY_MODES, OnCSCKDesc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsProcessing message handlers

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::OnInitDialog() 
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
    
    m_bProcessing = TRUE;      
	
    if (m_nDialogMode != MODE_DONE )
    {
		if (m_nDialogMode != MODE_VIEW)
        EnableProcessingDialog(TRUE);	

        if ( m_nDialogMode != MODE_ADD )
            PopulateProcessingDialogFromDatabase (sProfileIdOld,sKey3ds01Old);        

    if (m_nDialogMode == MODE_ADD )
    {		
	    m_CntlPinOffsetLocation_Fld.SetCurSel(1);
	    m_CntlPinCapIndicator_Fld.SetCurSel(1);
	    m_CntlExpDateCheck_Fld.SetCurSel(0);
	    m_CntlExpDateLocation_Fld.SetCurSel(1);
	    m_CntlAuthLocation_Fld.SetCurSel(0);
	    m_CntlAuthMethod_Fld.SetCurSel(0);
	    m_CntlStatus_Fld.SetCurSel(0); 
	    m_CntlPinVerifMethod_Fld.SetCurSel(6);
		m_CntlCVVProcessing.SetCurSel(0);
//m_VelocityChecking.SetCheck(0);
//m_PinChecking.SetCheck(0);
    }
		    
	}
    
    if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }

    ZeroMemory(&sProfileIdPinVerif, sizeof (CAF01));	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::PopulateProcessingDialogFromDatabase( CAF01 sProfileId, KEY3DS01 sKey3ds01 )
{
	CString offset;
    CString capture;
    CString datecheck;
    CString dateloc;
    CString authloc;
    CString authmethod;
    CString status;
    CString verifmethod;
	CString strCvvProcessing;
	CString strVelocityChecking;
	CString strPinChecking;
	CString strFloorLimit;
	CString strCntryLimitsChecking;
    CMyUtils myUtilsObject;

    if ( m_nDialogMode != MODE_VIEW )
        EnableProcessingDialog(TRUE);
  
    ZeroMemory ( &sProfileIdOld, sizeof (CAF01));
	ZeroMemory ( &sKey3ds01Old, sizeof (KEY3DS01));

	m_StrProfileId_Fld = sProfileId.primary_key.caf_profile_id;
	m_StrType_Fld = sProfileId.primary_key.type;

	if ( strcmp (m_StrType_Fld, "C") == 0 )
		m_StrType_Fld = "CREDIT";
	else if ( strcmp (m_StrType_Fld, "F") == 0 )
		m_StrType_Fld = "FLEET";
	else 
		m_StrType_Fld = "DEBIT";
 
    m_StrNumPinRetries_Fld      = sProfileId.max_pin_retries;    
	//    m_StrTrackPinOffsetLoc_Fld  = sProfileId.track_offset;  bug #556  
    m_StrRemoteDest_Fld         = sProfileId.destination_name;
    m_StrName_Fld               = sProfileId.name;
    m_StrContact_Fld            = sProfileId.contact;
    m_StrPhone_Fld              = sProfileId.phone;
    m_StrMerchSame_Fld          = sProfileId.merchant_same;
    m_StrProcess_Fld            = sProfileId.nsp_process;
	m_StrCVKAFld                = sProfileId.cvk[0].cvkA;
	m_StrCVKBFld                = sProfileId.cvk[0].cvkB;
	m_StrCodeOffset1Fld         = sProfileId.service_code_offset_tr1;
	m_StrCodeOffset2Fld         = sProfileId.service_code_offset_tr2;
	m_StrCVVOffset1Fld          = sProfileId.cvv_offset_tr1;
	m_StrCVVOffset2Fld          = sProfileId.cvv_offset_tr2;
	m_StrCVVLengthFld           = sProfileId.cvv_length;
	m_StrCSCKFld                = sProfileId.csck;
		m_StrKey3ds01               =sKey3ds01.key3ds;
	m_StrMKACFld				= sProfileId.cvk[5].cvkA;
	
		m_StrMKACFld = m_StrMKACFld + sProfileId.cvk[5].cvkB;
	m_Str3DCAKFld				= sProfileId.cvk[4].cvkA;
		m_Str3DCAKFld = m_Str3DCAKFld + sProfileId.cvk[4].cvkB;
	if (strncmp((char *)sProfileId.destination_name,"nposa2",6) != 0 ||
		strncmp((char *)sProfileId.destination_name,"ncdci2",6) != 0)
	{
		 m_StrMCCAKFld 			= sProfileId.cvk[3].cvkA;
		 m_StrMCCAKFld 			= m_StrMCCAKFld + sProfileId.cvk[3].cvkB;
	}
   strFloorLimit               = sProfileId.floor_limit;
   m_StrFloorLimitFld          = myUtilsObject.GetAmount( strFloorLimit );
   m_StrPINValidationData      = sProfileId.cvk[2].cvkB;
   m_StrDecimalisationTable      = sProfileId.cvk[2].cvkA;

	offset                      = sProfileId.pin_offset_location;                            
    capture                     = sProfileId.pin_capture_ind;
    datecheck                   = sProfileId.expiration_date_check;   
    dateloc                     = sProfileId.expiration_date_location;     
    authloc                     = sProfileId.authorization_location;
    authmethod                  = sProfileId.authorization_method;
    status                      = sProfileId.status;
    verifmethod                 = sProfileId.pin_verification_method;
    strCvvProcessing            = sProfileId.cvv_processing;

	strCntryLimitsChecking      = sProfileId.country_limits_flag;
		m_PosEntryModes[0] = sProfileId.last_maint[0];
		m_PosEntryModes[1] = sProfileId.last_maint[1];
		// 3D secure 2.0 key
	if(0 ==strncmp((char *)sProfileId.destination_name,"ncv",3) ||
	   0 ==strncmp((char *)sProfileId.destination_name,"ncm",3)  ||
	   0 ==strncmp((char *)sProfileId.destination_name,"ncj",3) ||
	   0 ==strncmp((char *)sProfileId.destination_name,"ncc",3))

	{	
		m_Str3DS_Fld = sProfileId.csck;
	}
	else if (0 ==strncmp((char *)sProfileId.destination_name,"nposa2",6) ||
			 0 ==strncmp((char *)sProfileId.destination_name,"ncdci2",6))
	{
		m_Str3DS_Fld = sProfileId.cvk[3].cvkA;
		m_Str3DS_Fld = m_Str3DS_Fld + sProfileId.cvk[3].cvkB;
	}
	else
	{
		m_StrCSCKFld  = sProfileId.csck;
	}

	if(((sProfileId.last_maint[0] & 0x40) == 0x40))
	{
		m_CntlPosEntryModes.SetCheck(TRUE);
	}
	else
	{
		m_CntlPosEntryModes.SetCheck(FALSE);
	}
//	strPinChecking              = sProfileId.pin_checking_flag;


	if(sProfileId.aba_nbr[0] == '0' || sProfileId.aba_nbr[0] == 0)
	{
		m_CntlCSCKDesc.SetCheck(0);
	}
	else
	{
		m_CntlCSCKDesc.SetCheck(1);
	}

	
	// single length key check box, ticked: use single lenght key for diners protect buy unticked: use double length key

	if(sProfileId.aba_nbr[1] == '0' || sProfileId.aba_nbr[1] == 0)
	{
		m_CntlSlen3Dcak.SetCheck(0);
	}
	else
	{
		m_CntlSlen3Dcak.SetCheck(1);
	}



	if ( strCntryLimitsChecking == "Y" )
		m_CntlCntryLimitsCheckFlag.SetCheck(1);
	else
		m_CntlCntryLimitsCheckFlag.SetCheck(0);

	if(sProfileId.cvk[1].cvkA[0]== '1')
	{
		m_Cntlmc3dpanFlag.SetCheck(1);
	}
	else
	{
		m_Cntlmc3dpanFlag.SetCheck(0);
	}
		
	if(sProfileId.cvk[1].cvkA[1]== '1')
	{
		m_Cntlmc3ddstxnFlag.SetCheck(1);
	}
	else
	{
		m_Cntlmc3ddstxnFlag.SetCheck(0);
	}

	if(sProfileId.cvk[1].cvkA[2]== '1')
	{
		m_Cntlmc3dcurrencyFlag.SetCheck(1);
	}
	else
	{
		m_Cntlmc3dcurrencyFlag.SetCheck(0);
	}

	if(sProfileId.cvk[1].cvkA[3]== '1')
	{
		m_Cntlmc3dtxnamtFlag.SetCheck(1);
	}
	else
	{
		m_Cntlmc3dtxnamtFlag.SetCheck(0);
	}

	if(sProfileId.cvk[1].cvkA[4]== '1')
	{
		m_Cntlmc3dmechnameFlag.SetCheck(1);
	}
	else
	{
		m_Cntlmc3dmechnameFlag.SetCheck(0);
	}

   if ( strCvvProcessing == "0" )
      m_StrCVVProcessing = "NO VALIDATION";
   else if ( strCvvProcessing == "1" )
      m_StrCVVProcessing = "VALIDATE, DENY";
   else if ( strCvvProcessing == "2" )
      m_StrCVVProcessing = "VALIDATE, DENY, AND REPORT";
   else if ( strCvvProcessing == "3" )
      m_StrCVVProcessing = "VALIDATE, APPROVE, AND REPORT";
   else
      m_StrCVVProcessing = "NO VALIDATION";


    if ( offset == "C")     
        m_StrPinOffsetLocation_Fld = "CARD RECORD";
    else if ( offset == "T")
        m_StrPinOffsetLocation_Fld = "TRACK DATA";
    else if ( offset == "B")
        m_StrPinOffsetLocation_Fld = "BOTH";
    
    
    if ( capture == "C")
        m_StrPinCapIndicator_Fld = "CAPTURE";
    else if ( capture == "D")
        m_StrPinCapIndicator_Fld = "DECLINE";
    
    
    if ( datecheck == "Y" )
        m_StrExpDateCheck_Fld = "YES";
    else if ( datecheck == "N" )
        m_StrExpDateCheck_Fld = "NO";
    else if ( datecheck == "E" )
        m_StrExpDateCheck_Fld = "YES - EXPANDED";
    
    
    if ( dateloc == "0")
        m_StrExpDateLocation_Fld = "CARD";
    else if ( dateloc == "1")
        m_StrExpDateLocation_Fld = "CARDHOLDER RECORD";
    else if ( dateloc == "2")
        m_StrExpDateLocation_Fld = "BOTH";
    else if ( dateloc == "3")
        m_StrExpDateLocation_Fld = "BOTH WITH REVERSED DATE";

    
    
    if ( authloc == "L")
        m_StrAuthLocation_Fld = "LOCAL";
    else if ( authloc == "A")
        m_StrAuthLocation_Fld = "LOCAL/REMOTE OVERRIDE APPROVALS";
    else if ( authloc == "D")
        m_StrAuthLocation_Fld = "LOCAL/REMOTE OVERRIDE DECLINES";
	else if ( authloc == "S")
        m_StrAuthLocation_Fld = "REMOTE/LOCAL STAND-IN";
    else if ( authloc == "R")
        m_StrAuthLocation_Fld = "REMOTE";
    
    
    if ( authmethod == "P")
        m_StrAuthMethod_Fld = "POSITIVE";
    else if ( authmethod == "B")
        m_StrAuthMethod_Fld = "POSITIVE WITH BALANCES";
    else if ( authmethod == "N")
        m_StrAuthMethod_Fld = "NEGATIVE";
    else if ( authmethod == "D")
        m_StrAuthMethod_Fld = "PRIMARY POSITIVE WITH BALANCES";
    else if ( authmethod == "V")
        m_StrAuthMethod_Fld = "OFFUS VELOCITY";
    else if ( authmethod == "O")
        m_StrAuthMethod_Fld = "NEGATIVE WITH OFFUS VELOCITY";
	else if ( authmethod == "F")
		m_StrAuthMethod_Fld = "NEGATIVE OFFUS FRAUD VELOCITY";
    
    
     
    if ( status == "A")
        m_StrStatus_Fld = "ACTIVE";
    else if ( status == "R")
        m_StrStatus_Fld = "RESTRICTED";
    
    
    if ( verifmethod == "A")
        m_StrPinVerifMethod_Fld = "IDENTIKEY";
    else if ( verifmethod == "B")
        m_StrPinVerifMethod_Fld = "EIDENTIKEY";
    else if ( verifmethod == "C")
        m_StrPinVerifMethod_Fld = "IBM 3624";
    else if ( verifmethod == "D")
        m_StrPinVerifMethod_Fld = "IBM OFFSET";
    else if ( verifmethod == "E")
        m_StrPinVerifMethod_Fld = "VISA";
    else if ( verifmethod == "F")
        m_StrPinVerifMethod_Fld = "DESBILEVEL";
    else if ( verifmethod == "G")
        m_StrPinVerifMethod_Fld = "DEIBOLD";
    else if ( verifmethod == "H")
        m_StrPinVerifMethod_Fld = "NCR";
    //else if ( verifmethod == "I")
    //    m_StrPinVerifMethod_Fld = "CVV/CVC";
	 else if ( verifmethod == "J")
        m_StrPinVerifMethod_Fld = "NONE";
    
    UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));
    memcpy ( &sProfileIdPinVerif, &sProfileId, sizeof ( CAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::PopulateDatabaseFromProcessingDialog( pCAF01 psProfileId, pKEY3DS01 psKey3ds01 )
{
    char strStatus[2]; 
	char strCvvProcessing[2];
   CMyUtils myUtilsObject;
	
	/* fix for bug 149 */
	if ( !m_bProcessing )
		return TRUE;
  
	memcpy ( psProfileId, &sProfileIdPinVerif, sizeof ( CAF01));
    memcpy ( psProfileId->max_pin_retries,m_StrNumPinRetries_Fld,sizeof ( psProfileId->max_pin_retries));
	//    memcpy ( psProfileId->track_offset,m_StrTrackPinOffsetLoc_Fld,sizeof ( psProfileId->track_offset));  bug #556
    memcpy ( psProfileId->destination_name,m_StrRemoteDest_Fld,sizeof ( psProfileId->destination_name));
    memcpy ( psProfileId->name,m_StrName_Fld,sizeof ( psProfileId->name));
    memcpy ( psProfileId->contact,m_StrContact_Fld,sizeof ( psProfileId->contact));
    memcpy ( psProfileId->phone,m_StrPhone_Fld,sizeof ( psProfileId->phone));
    memcpy ( psProfileId->merchant_same,m_StrMerchSame_Fld,sizeof ( psProfileId->merchant_same));
	memcpy ( psProfileId->nsp_process,m_StrProcess_Fld,sizeof ( psProfileId->nsp_process));
	memcpy ( psProfileId->cvk[0].cvkA,m_StrCVKAFld,sizeof ( psProfileId->cvk[0].cvkA));
	memcpy ( psProfileId->cvk[0].cvkB,m_StrCVKBFld,sizeof ( psProfileId->cvk[0].cvkB));
	memcpy ( psProfileId->service_code_offset_tr1,m_StrCodeOffset1Fld,sizeof ( psProfileId->service_code_offset_tr1));
	memcpy ( psProfileId->service_code_offset_tr2,m_StrCodeOffset2Fld,sizeof ( psProfileId->service_code_offset_tr2));
	memcpy ( psProfileId->cvv_offset_tr1,m_StrCVVOffset1Fld,sizeof ( psProfileId->cvv_offset_tr1));
	memcpy ( psProfileId->cvv_offset_tr2,m_StrCVVOffset2Fld,sizeof ( psProfileId->cvv_offset_tr2));
	memcpy ( psProfileId->cvv_length,m_StrCVVLengthFld,sizeof ( psProfileId->cvv_length));
	memcpy ( psProfileId->csck,m_StrCSCKFld,sizeof ( psProfileId->csck));
	memcpy ( psProfileId->cvk[2].cvkB,m_StrPINValidationData,sizeof ( psProfileId->cvk[2].cvkB));
	memcpy ( psProfileId->cvk[2].cvkA,m_StrDecimalisationTable,sizeof ( psProfileId->cvk[2].cvkA));
		memcpy ( psKey3ds01->key3ds,m_StrKey3ds01,sizeof ( psKey3ds01->key3ds));
	
	if(0 ==strncmp((char *)psProfileId->destination_name,"ncv",3) ||
	   0 ==strncmp((char *)psProfileId->destination_name,"ncm",3) ||
	   0 ==strncmp((char *)psProfileId->destination_name,"ncj",3) ||
	   0 ==strncmp((char *)psProfileId->destination_name,"ncc",3))
	{
		memcpy ( psProfileId->csck,m_Str3DS_Fld,sizeof ( psProfileId->csck));
	}
	else if (0 ==strncmp((char *)psProfileId->destination_name,"nposa2", 6) ||
			 0 ==strncmp((char *)psProfileId->destination_name,"ncdci2", 6))
	{

		if(!m_Str3DS_Fld.IsEmpty())
		{
			char *temp = m_Str3DS_Fld.GetBuffer(32);
			memcpy ( psProfileId->cvk[3].cvkA,temp , 16);
			memcpy ( psProfileId->cvk[3].cvkB,temp+16 ,16);
			m_Str3DS_Fld.ReleaseBuffer(32);
		}
		else
		{
			memset ( psProfileId->cvk[3].cvkA,0x00 , sizeof(psProfileId->cvk[3].cvkA));
			memset ( psProfileId->cvk[3].cvkB,0x00 , sizeof(psProfileId->cvk[3].cvkB));
		}

	}
	else
	{
		memcpy ( psProfileId->csck,m_StrCSCKFld,sizeof ( psProfileId->csck));
	}
	if(!m_StrMKACFld.IsEmpty())
	{
		char *temp = m_StrMKACFld.GetBuffer(32);
		memcpy ( psProfileId->cvk[5].cvkA,temp , 16);
		memcpy ( psProfileId->cvk[5].cvkB,temp+16 ,16);
		m_StrMKACFld.ReleaseBuffer(32);
	}
	else
	{
		memset ( psProfileId->cvk[5].cvkA,0x00 , sizeof(psProfileId->cvk[5].cvkA));
		memset ( psProfileId->cvk[5].cvkB,0x00 , sizeof(psProfileId->cvk[5].cvkB));
	}
	if(!m_Str3DCAKFld.IsEmpty())
	{	
		char *temp1 = m_Str3DCAKFld.GetBuffer(32);
		memcpy ( psProfileId->cvk[4].cvkA,temp1 , 16);
		memcpy ( psProfileId->cvk[4].cvkB,temp1+16 ,16);
		m_Str3DCAKFld.ReleaseBuffer(32);
	}
	else
	{
		memset ( psProfileId->cvk[4].cvkA,0x00 , sizeof(psProfileId->cvk[4].cvkA));
		memset ( psProfileId->cvk[4].cvkB,0x00 , sizeof(psProfileId->cvk[4].cvkB));
	}
	if (0 !=strncmp((char *)psProfileId->destination_name,"nposa2", 6) &&
		0 !=strncmp((char *)psProfileId->destination_name,"ncdci2", 6)	)
	{

	if(!m_StrMCCAKFld.IsEmpty())
	{	
		char *temp2 = m_StrMCCAKFld.GetBuffer(32);
		memcpy ( psProfileId->cvk[3].cvkA,temp2 , 16);
		memcpy ( psProfileId->cvk[3].cvkB,temp2+16 ,16);
		m_StrMCCAKFld.ReleaseBuffer(32);
	}
	else
	{
		memset ( psProfileId->cvk[3].cvkA,0x00 , sizeof(psProfileId->cvk[3].cvkA));
		memset ( psProfileId->cvk[3].cvkB,0x00 , sizeof(psProfileId->cvk[3].cvkB));
	}
	}
	
	
	memcpy ( psProfileId->floor_limit,myUtilsObject.GenerateAmount (m_StrFloorLimitFld,12),sizeof ( psProfileId->floor_limit));

	if (m_CntlCntryLimitsCheckFlag.GetCheck() == 1)
		memcpy ( psProfileId->country_limits_flag, "Y", sizeof ( psProfileId->country_limits_flag));
	else
		memcpy ( psProfileId->country_limits_flag, "N", sizeof ( psProfileId->country_limits_flag));

	if (m_Cntlmc3dpanFlag.GetCheck() == 1)
	{
		psProfileId->cvk[1].cvkA[0] = '1';
	}
	else
	{
		psProfileId->cvk[1].cvkA[0] = '0';
	}

	if (m_Cntlmc3ddstxnFlag.GetCheck() == 1)
	{
		psProfileId->cvk[1].cvkA[1] = '1';
	}
	else
	{
		psProfileId->cvk[1].cvkA[1] = '0';
	}

	if (m_Cntlmc3dcurrencyFlag.GetCheck() == 1)
	{
		psProfileId->cvk[1].cvkA[2] = '1';
	}
	else
	{
		psProfileId->cvk[1].cvkA[2] = '0';
	}

	if (m_Cntlmc3dtxnamtFlag.GetCheck() == 1)
	{
		psProfileId->cvk[1].cvkA[3] = '1';
	}
	else
	{
		psProfileId->cvk[1].cvkA[3] = '0';
	}

	if (m_Cntlmc3dmechnameFlag.GetCheck() == 1)
	{
		psProfileId->cvk[1].cvkA[4] = '1';
	}
	else
	{
		psProfileId->cvk[1].cvkA[4] = '0';
	}


	psProfileId->aba_nbr[0] = (BYTE)m_CntlCSCKDesc.GetCheck();

	psProfileId->aba_nbr[1] = (BYTE)m_CntlSlen3Dcak.GetCheck();

	if ( m_CntlCSCKDesc.GetCheck() == 0 && (BYTE)m_CntlSlen3Dcak.GetCheck() == 0)
	{
		psProfileId->aba_nbr[0] = '0';
		psProfileId->aba_nbr[1] = '0';
	}
	else if( m_CntlCSCKDesc.GetCheck() == 0 && (BYTE)m_CntlSlen3Dcak.GetCheck() == 1)
	{
		psProfileId->aba_nbr[0] = '0';
		psProfileId->aba_nbr[1] = '1';
	}
	else if( m_CntlCSCKDesc.GetCheck() == 1 && (BYTE)m_CntlSlen3Dcak.GetCheck() == 0)
	{
		psProfileId->aba_nbr[0] = '1';
		psProfileId->aba_nbr[1] = '0';
	}
	else if(m_CntlCSCKDesc.GetCheck() == 1 && (BYTE)m_CntlSlen3Dcak.GetCheck() == 1)
	{
		psProfileId->aba_nbr[0] = '1';
		psProfileId->aba_nbr[1] = '1';
	}

/*	if ( m_VelocityChecking.GetCheck() == 0 )
		memcpy ( psProfileId->velocity_checking_flag, "N",sizeof ( psProfileId->velocity_checking_flag));
	else
		memcpy ( psProfileId->velocity_checking_flag, "Y",sizeof ( psProfileId->velocity_checking_flag));

	if ( m_PinChecking.GetCheck() == 0 )
		memcpy ( psProfileId->pin_checking_flag, "N",sizeof ( psProfileId->pin_checking_flag));
	else
		memcpy ( psProfileId->pin_checking_flag, "Y",sizeof ( psProfileId->pin_checking_flag));

    memset ( strCvvProcessing, 0, sizeof (strCvvProcessing));*/


	if ( m_StrCVVProcessing == "NO VALIDATION" )
        memcpy ( strCvvProcessing, "0", sizeof (strCvvProcessing));
	else if ( m_StrCVVProcessing == "VALIDATE, DENY" )
        memcpy ( strCvvProcessing, "1", sizeof (strCvvProcessing));
	else if ( m_StrCVVProcessing == "VALIDATE, DENY, AND REPORT" )
        memcpy ( strCvvProcessing, "2", sizeof (strCvvProcessing));
	else if ( m_StrCVVProcessing == "VALIDATE, APPROVE, AND REPORT" )
        memcpy ( strCvvProcessing, "3", sizeof (strCvvProcessing));
	else
        memcpy ( strCvvProcessing, "0", sizeof (strCvvProcessing));/*default - NO VALIDATION*/

	memcpy ( psProfileId->cvv_processing,strCvvProcessing,sizeof ( psProfileId->cvv_processing));
	if(m_CntlPosEntryModes.GetCheck() == 1)
	{
		psProfileId->last_maint[0] = 0x00;
		psProfileId->last_maint[1] = 0x00;
		psProfileId->last_maint[0] = psProfileId->last_maint[0] | 0x40;
		psProfileId->last_maint[0] = psProfileId->last_maint[0] | m_PosEntryModes[0];
		psProfileId->last_maint[1] = psProfileId->last_maint[1] | m_PosEntryModes[1];

	}
	else
	{
		psProfileId->last_maint[0] = 0x00;
		psProfileId->last_maint[1] = 0x00;
	}
     
    memset ( strStatus, 0, sizeof (strStatus));
     
    if (m_StrPinOffsetLocation_Fld == "TRACK DATA")
        memcpy ( strStatus, "T", sizeof (strStatus));
    else if (m_StrPinOffsetLocation_Fld == "CARD RECORD")
        memcpy ( strStatus, "C", sizeof (strStatus));
    else if (m_StrPinOffsetLocation_Fld == "BOTH")
        memcpy ( strStatus, "B", sizeof (strStatus));
    
    memcpy ( psProfileId->pin_offset_location,strStatus,sizeof ( psProfileId->pin_offset_location));
     
    if (m_StrPinCapIndicator_Fld == "DECLINE")
        memcpy ( strStatus, "D", sizeof (strStatus));
    else if (m_StrPinCapIndicator_Fld == "CAPTURE")
        memcpy ( strStatus, "C", sizeof (strStatus));

    memcpy ( psProfileId->pin_capture_ind,strStatus,sizeof ( psProfileId->pin_capture_ind));
          
   if ( m_StrExpDateCheck_Fld == "NO" )
      memcpy( strStatus, "N", sizeof (strStatus) );
   else if ( m_StrExpDateCheck_Fld == "YES" )
      memcpy( strStatus, "Y", sizeof (strStatus) );
   else if ( m_StrExpDateCheck_Fld == "YES - EXPANDED" )
      memcpy( strStatus, "E", sizeof (strStatus) );

   memcpy( psProfileId->expiration_date_check, strStatus, sizeof(psProfileId->expiration_date_check) );
         
    if (m_StrExpDateLocation_Fld == "CARD")
        memcpy ( strStatus, "0", sizeof (strStatus));
    else if (m_StrExpDateLocation_Fld == "CARDHOLDER RECORD")
        memcpy ( strStatus, "1", sizeof (strStatus));
    else if (m_StrExpDateLocation_Fld == "BOTH")
        memcpy ( strStatus, "2", sizeof (strStatus));
     else if (m_StrExpDateLocation_Fld == "BOTH WITH REVERSED DATE")
        memcpy ( strStatus, "3", sizeof (strStatus));
   

    memcpy ( psProfileId->expiration_date_location,strStatus,sizeof ( psProfileId->expiration_date_location));     
     
    if (m_StrAuthLocation_Fld == "LOCAL")
        memcpy ( strStatus, "L", sizeof (strStatus));
    else if (m_StrAuthLocation_Fld == "LOCAL/REMOTE OVERRIDE APPROVALS")
        memcpy ( strStatus, "A", sizeof (strStatus));
    else if (m_StrAuthLocation_Fld == "LOCAL/REMOTE OVERRIDE DECLINES")
        memcpy ( strStatus, "D", sizeof (strStatus));
	else if (m_StrAuthLocation_Fld == "REMOTE/LOCAL STAND-IN")
        memcpy ( strStatus, "S", sizeof (strStatus));
    else if (m_StrAuthLocation_Fld == "REMOTE")
        memcpy ( strStatus, "R", sizeof (strStatus));

    memcpy ( psProfileId->authorization_location,strStatus,sizeof ( psProfileId->authorization_location));   
         
    if (m_StrAuthMethod_Fld == "POSITIVE")
        memcpy ( strStatus, "P", sizeof (strStatus));
    else if (m_StrAuthMethod_Fld == "POSITIVE WITH BALANCES")
        memcpy ( strStatus, "B", sizeof (strStatus));
    else if (m_StrAuthMethod_Fld == "NEGATIVE")
        memcpy ( strStatus, "N", sizeof (strStatus));
    else if (m_StrAuthMethod_Fld == "PRIMARY POSITIVE WITH BALANCES")
        memcpy ( strStatus, "D", sizeof (strStatus));
    else if ( m_StrAuthMethod_Fld == "OFFUS VELOCITY" )
        memcpy ( strStatus, "V", sizeof ( strStatus));
    else if ( m_StrAuthMethod_Fld == "NEGATIVE WITH OFFUS VELOCITY")
        memcpy ( strStatus, "O", sizeof ( strStatus));
	else if ( m_StrAuthMethod_Fld == "NEGATIVE OFFUS FRAUD VELOCITY")
        memcpy ( strStatus, "F", sizeof ( strStatus));
    

    memcpy ( psProfileId->authorization_method,strStatus,sizeof ( psProfileId->authorization_method));  
        
    if (m_StrStatus_Fld == "ACTIVE")
        memcpy ( strStatus, "A", sizeof (strStatus));
    else if (m_StrStatus_Fld == "RESTRICTED")
        memcpy ( strStatus, "R", sizeof (strStatus));
    
    memcpy ( psProfileId->status,strStatus,sizeof ( psProfileId->status));

    if (m_StrPinVerifMethod_Fld == "IDENTIKEY")
         memcpy ( strStatus, "A", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld== "EIDENTIKEY")
         memcpy ( strStatus, "B", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld == "IBM 3624")
         memcpy ( strStatus, "C", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld== "IBM OFFSET")
         memcpy ( strStatus, "D", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld == "VISA")
         memcpy ( strStatus, "E", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld== "DESBILEVEL")
         memcpy ( strStatus, "F", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld == "DEIBOLD")
         memcpy ( strStatus, "G", sizeof (strStatus));
    else if (m_StrPinVerifMethod_Fld== "NCR")
         memcpy ( strStatus, "H", sizeof (strStatus));
    //else if (m_StrPinVerifMethod_Fld == "CVV/CVC")
    //     memcpy ( strStatus, "I", sizeof (strStatus));
	else if (m_StrPinVerifMethod_Fld == "NONE")
         memcpy ( strStatus, "J", sizeof (strStatus));

    memcpy ( psProfileId->pin_verification_method,strStatus, sizeof ( psProfileId->pin_verification_method)); 
    
    return TRUE;

} 

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetPinOffsetLocation()
{
	if ( strcmp (m_StrPinOffsetLocation_Fld, "TRACK DATA") == 0)
         memcpy ((LPTSTR) strStatus,	"T", sizeof(strStatus)); 
	if ( strcmp (m_StrPinOffsetLocation_Fld, "CARD RECORD")== 0)
         memcpy ((LPTSTR) strStatus,	"C", sizeof(strStatus)); 
	if ( strcmp (m_StrPinOffsetLocation_Fld, "BOTH")== 0)
         memcpy ((LPTSTR) strStatus,	"B", sizeof(strStatus)); 
	
	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetPinCaptureIndicator()
{
	if ( strcmp (m_StrPinCapIndicator_Fld, "DECLINE") == 0)
         memcpy ((LPTSTR) strStatus,	"D", sizeof(strStatus)); 
	if ( strcmp (m_StrPinCapIndicator_Fld, "CAPTURE")== 0)
         memcpy ((LPTSTR) strStatus,	"C", sizeof(strStatus)); 

	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetExpirationDateCheck()
{
   if ( strcmp(m_StrExpDateCheck_Fld, "NO") == 0 )
      memcpy( (LPTSTR)strStatus, "N", sizeof(strStatus) );

   else if ( strcmp(m_StrExpDateCheck_Fld, "YES" ) == 0 )
      memcpy( (LPTSTR)strStatus, "Y", sizeof(strStatus) );

   else if ( strcmp(m_StrExpDateCheck_Fld, "YES - EXPANDED" ) == 0 )
      memcpy( (LPTSTR)strStatus, "E", sizeof(strStatus) );
	
	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetExpirationDateLocation()
{
	if ( strcmp (m_StrExpDateLocation_Fld, "CARD") == 0)
         memcpy ((LPTSTR) strStatus,	"0", sizeof(strStatus)); 
	if ( strcmp (m_StrExpDateLocation_Fld, "CARDHOLDER RECORD")== 0)
         memcpy ((LPTSTR) strStatus,	"1", sizeof(strStatus)); 
   if ( strcmp (m_StrExpDateLocation_Fld, "BOTH")== 0)
         memcpy ((LPTSTR) strStatus,	"2", sizeof(strStatus)); 
   if ( strcmp (m_StrExpDateLocation_Fld, "BOTH WITH REVERSED DATE")== 0)
         memcpy ((LPTSTR) strStatus,	"3", sizeof(strStatus)); 

	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetAuthorizationLocation()
{
	if ( strcmp (m_StrAuthLocation_Fld, "LOCAL") == 0)
         memcpy ((LPTSTR) strStatus,	"L", sizeof(strStatus)); 	
	if ( strcmp (m_StrAuthLocation_Fld, "LOCAL/REMOTE OVERRIDE APPROVALS") == 0)   
          memcpy ((LPTSTR) strStatus,	"A", sizeof(strStatus));
	if ( strcmp (m_StrAuthLocation_Fld, "LOCAL/REMOTE OVERRIDE DECLINES")== 0)
          memcpy ((LPTSTR) strStatus,	"D", sizeof(strStatus));
	if ( strcmp (m_StrAuthLocation_Fld, "REMOTE/LOCAL STAND-IN") == 0)
         memcpy ((LPTSTR) strStatus,	"S", sizeof(strStatus));
	if ( strcmp (m_StrAuthLocation_Fld, "REMOTE") == 0)
         memcpy ((LPTSTR) strStatus,	"R", sizeof(strStatus)); 	
	
	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetAuthorizationMethod()
{
	if ( strcmp (m_StrAuthMethod_Fld, "NEGATIVE") == 0)
         memcpy ((LPTSTR) strStatus,	"N", sizeof(strStatus)); 
	else if ( strcmp (m_StrAuthMethod_Fld, "POSITIVE")== 0)
         memcpy ((LPTSTR) strStatus,	"P", sizeof(strStatus)); 
	else if ( strcmp (m_StrAuthMethod_Fld, "POSITIVE WITH BALANCES") == 0)   
          memcpy ((LPTSTR) strStatus,	"B", sizeof(strStatus));
	else if ( strcmp (m_StrAuthMethod_Fld, "PRIMARY POSITIVE WITH BALANCES") == 0)   
          memcpy ((LPTSTR) strStatus,	"D", sizeof(strStatus));
	else if ( strcmp (m_StrAuthMethod_Fld, "OFFUS VELOCITY") == 0)   
          memcpy ((LPTSTR) strStatus,	"V", sizeof(strStatus));
	else if ( strcmp (m_StrAuthMethod_Fld, "NEGATIVE WITH OFFUS VELOCITY") == 0)   
          memcpy ((LPTSTR) strStatus,	"O", sizeof(strStatus));
	else if ( strcmp (m_StrAuthMethod_Fld, "NEGATIVE OFFUS FRAUD VELOCITY") == 0)   
          memcpy ((LPTSTR) strStatus,	"F", sizeof(strStatus));   

	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetStatus()
{
	if ( strcmp (m_StrStatus_Fld, "ACTIVE") == 0)
         memcpy ((LPTSTR) strStatus,	"A", sizeof(strStatus)); 
	if ( strcmp (m_StrStatus_Fld, "RESTRICTED")== 0)
         memcpy ((LPTSTR) strStatus,	"R", sizeof(strStatus)); 

	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::GetPinVerificationMethod()
{
	if ( strcmp (m_StrPinVerifMethod_Fld, "IDENTIKEY") == 0)
         memcpy ((LPTSTR) strStatus,	"A", sizeof(strStatus)); 
	if ( strcmp (m_StrPinVerifMethod_Fld, "EIDENTIKEY")== 0)
         memcpy ((LPTSTR) strStatus,	"B", sizeof(strStatus)); 
	if ( strcmp (m_StrPinVerifMethod_Fld, "IBM 3624") == 0)   
          memcpy ((LPTSTR) strStatus,	"C", sizeof(strStatus));
    if ( strcmp (m_StrPinVerifMethod_Fld, "IBM OFFSET") == 0)
         memcpy ((LPTSTR) strStatus,	"D", sizeof(strStatus)); 
	if ( strcmp (m_StrPinVerifMethod_Fld, "VISA")== 0)
         memcpy ((LPTSTR) strStatus,	"E", sizeof(strStatus)); 
	if ( strcmp (m_StrPinVerifMethod_Fld, "DESBILEVEL") == 0)   
          memcpy ((LPTSTR) strStatus,	"F", sizeof(strStatus));
    if ( strcmp (m_StrPinVerifMethod_Fld, "DEIBOLD") == 0)
         memcpy ((LPTSTR) strStatus,	"G", sizeof(strStatus)); 
	if ( strcmp (m_StrPinVerifMethod_Fld, "NCR")== 0)
         memcpy ((LPTSTR) strStatus,	"H", sizeof(strStatus)); 
	//if ( strcmp (m_StrPinVerifMethod_Fld, "CVV/CVC") == 0)   
    //      memcpy ((LPTSTR) strStatus,	"I", sizeof(strStatus));
	if ( strcmp (m_StrPinVerifMethod_Fld, "NONE") == 0)   
          memcpy ((LPTSTR) strStatus,	"J", sizeof(strStatus));	

	
	return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsProcessing::CleanProcessingDialog()
{
   m_StrAuthLocation_Fld = _T("");
	m_StrAuthMethod_Fld = _T("");
	m_StrContact_Fld = _T("");
	m_StrExpDateCheck_Fld = _T("");
	m_StrExpDateLocation_Fld = _T("");
	m_StrName_Fld = _T("");
	m_StrNumPinRetries_Fld = _T("");
	m_StrPhone_Fld = _T("");
	m_Str3DS_Fld = _T("");
	m_StrPinCapIndicator_Fld = _T("");
	m_StrPinOffsetLocation_Fld = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrRemoteDest_Fld = _T("");
	//	m_StrTrackPinOffsetLoc_Fld = _T(""); bug #556
	m_StrType_Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMerchSame_Fld = _T("");
	m_StrStatus_Fld = _T("");
	m_StrPinVerifMethod_Fld = _T("");
	m_StrProcess_Fld = _T("");
	m_StrCodeOffset1Fld = _T("");
	m_StrCodeOffset2Fld = _T("");
	m_StrCVKAFld = _T("");
	m_StrCVKBFld = _T("");
	m_StrCVVLengthFld = _T("");
	m_StrCVVOffset1Fld = _T("");
	m_StrCVVOffset2Fld = _T("");
	m_StrCVVProcessing = _T("");
 	m_StrFloorLimitFld = _T("");
	m_StrPINValidationData = _T("");
	m_StrDecimalisationTable = _T("");
	m_StrCSCKFld = "";
	m_StrMKACFld = "";
	m_Str3DCAKFld = "";
	m_StrMCCAKFld= "";
	m_StrKey3ds01 = _T("");
	UpdateData(FALSE);


}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsProcessing::EnableProcessingDialog(int nStatus)
{
   m_CntlNumPinRetries_Fld.EnableWindow(nStatus);
   m_CntlRemoteDest_Fld.EnableWindow(nStatus);
   m_CntlName_Fld.EnableWindow(nStatus);
   m_CntlContact_Fld.EnableWindow(nStatus);
   m_CntlPhone_Fld.EnableWindow(nStatus);
   m_Cntl3DS_Fld.EnableWindow(nStatus);
   m_CntlMerchSame_Fld.EnableWindow(nStatus);
   m_CntlProcess_Fld.EnableWindow(nStatus);
   m_CntlPinOffsetLocation_Fld.EnableWindow(nStatus);
   m_CntlPinCapIndicator_Fld.EnableWindow(nStatus);
   m_CntlExpDateCheck_Fld.EnableWindow(nStatus);
   m_CntlExpDateLocation_Fld.EnableWindow(nStatus);
   m_CntlAuthLocation_Fld.EnableWindow(nStatus);
   m_CntlAuthMethod_Fld.EnableWindow(nStatus);
   m_CntlStatus_Fld.EnableWindow(nStatus);
   m_CntlPinVerifMethod_Fld.EnableWindow(nStatus);
    m_CntlKey3ds01.EnableWindow(nStatus);
   m_NetworkButton.EnableWindow(nStatus);
   m_CntlCodeOffset1Fld.EnableWindow(nStatus);
   m_CntlCodeOffset2Fld.EnableWindow(nStatus);
   m_CntlCVKAFld.EnableWindow(nStatus);
   m_CntlCVKBFld.EnableWindow(nStatus);
   m_CntlCVVLengthFld.EnableWindow(nStatus);
   m_CntlCVVOffset1Fld.EnableWindow(nStatus);
   m_CntlCVVOffset2Fld.EnableWindow(nStatus);
   m_CntlCVVProcessing.EnableWindow(nStatus);

   m_CntlCSCKFld.EnableWindow(nStatus);
   m_CntlFloorLimitFld.EnableWindow(nStatus);
   m_CntlCntryLimitsCheckFlag.EnableWindow(nStatus);
   m_CntlPosEntryModes.EnableWindow(nStatus);
   m_CntlCSCKDesc.EnableWindow(nStatus);
   m_CntlSlen3Dcak.EnableWindow(nStatus);
   m_CntlMKACFld.EnableWindow(nStatus);
   m_CntlMCCAK_Fld.EnableWindow(nStatus);
   m_Cntl3DCAKFld.EnableWindow(nStatus);
   m_CntlPINValidationData.EnableWindow(nStatus);
   m_CntlDecimalisationTable.EnableWindow(nStatus);

   if(strncmp(m_StrRemoteDest_Fld,"ncm",3) == 0 || m_nDialogMode == MODE_ADD )
   {
		m_Cntlmc3dpanFlag.EnableWindow(nStatus);
	   m_Cntlmc3ddstxnFlag.EnableWindow(nStatus);
	   m_Cntlmc3dcurrencyFlag.EnableWindow(nStatus);
	   m_Cntlmc3dtxnamtFlag.EnableWindow(nStatus);
	   m_Cntlmc3dmechnameFlag.EnableWindow(nStatus);
   }
   else
   {
	   m_Cntlmc3dpanFlag.EnableWindow(false);
	   m_Cntlmc3ddstxnFlag.EnableWindow(false);
	   m_Cntlmc3dcurrencyFlag.EnableWindow(false);
	   m_Cntlmc3dtxnamtFlag.EnableWindow(false);
	   m_Cntlmc3dmechnameFlag.EnableWindow(false);
   }
   if (m_nDialogMode == MODE_ADD)
   {
      m_StrPinOffsetLocation_Fld = "TRACK DATA";
      m_StrPinCapIndicator_Fld   = "DECLINE";
      m_StrExpDateCheck_Fld      = "NO";
      m_StrExpDateLocation_Fld   = "CARD";
      m_StrAuthLocation_Fld      = "LOCAL";
      m_StrAuthMethod_Fld        = "NEGATIVE";
      m_StrStatus_Fld            = "ACTIVE";
      m_StrPinVerifMethod_Fld    = "VISA";
   }
   return;
}
    
 /************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/   

BOOL CCardsProcessing::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsProcessing::CompareStructures(KEY3DS01 sKey3ds01Old)
{
	CMyUtils myUtilsObject;

	CString offset;
    CString capture;
    CString datecheck;
    CString dateloc;
    CString authloc;
    CString authmethod;
    CString status;
    CString verifmethod;
	CString temp;
	CString strCvvProcessing;

    if (strcmp ( m_StrCVVOffset1Fld, (char *)sProfileIdOld.cvv_offset_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVVOffset1Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvv_offset_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVV Offset Track 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrCVVOffset2Fld, (char *)sProfileIdOld.cvv_offset_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVVOffset2Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvv_offset_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVV Offset Track 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strncmp ( m_StrMKACFld, (char *)sProfileIdOld.cvk[5].cvkA, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrMKACFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[5].cvkA, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MKAC", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (strncmp ( m_StrMKACFld, (char *)sProfileIdOld.cvk[5].cvkB, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrMKACFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[5].cvkB, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MKAC", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	 if (strncmp ( m_Str3DCAKFld, (char *)sProfileIdOld.cvk[4].cvkA, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_Str3DCAKFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[4].cvkA, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "3D_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (strncmp ( m_Str3DCAKFld, (char *)sProfileIdOld.cvk[4].cvkB, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_Str3DCAKFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[4].cvkB, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "3D_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if(0 != strncmp((char *)sProfileIdOld.destination_name,"nposa2",6) ||
	   0 != strncmp((char *)sProfileIdOld.destination_name,"ncdci2",6))
	{

	if (strncmp ( m_StrMCCAKFld, (char *)sProfileIdOld.cvk[3].cvkA, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrMCCAKFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[3].cvkA, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MC_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strncmp ( m_StrMCCAKFld, (char *)sProfileIdOld.cvk[3].cvkB, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrMCCAKFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[3].cvkB, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MC_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	}
	
	if(0 == strncmp((char *)sProfileIdOld.destination_name,"ncm",3) ||
		0 == strncmp((char *)sProfileIdOld.destination_name,"ncv",3) ||
		0 == strncmp((char *)sProfileIdOld.destination_name,"ncc",3) ||
		0 == strncmp((char *)sProfileIdOld.destination_name,"ncj",3))
	{
		if (strncmp ( m_Str3DS_Fld, (char *)sProfileIdOld.csck, 32) != 0)
		{
			memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
			memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
			memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
			memcpy(m_StrCurrentData, m_Str3DS_Fld, sizeof (m_StrCurrentData));
			memcpy(m_StrPreviousData, sProfileIdOld.csck, sizeof (m_StrPreviousData));
			memcpy(m_StrFieldTag, "3DS-2_Code:", sizeof(m_StrFieldTag));

			PopulateHistoryTable();
		}
	}
	
	if(0 == strncmp((char *)sProfileIdOld.destination_name,"nposa2",6) ||
	   0 == strncmp((char *)sProfileIdOld.destination_name,"ncdci2",6))
	{

		if (strncmp ( m_Str3DS_Fld, (char *)sProfileIdOld.cvk[3].cvkA, 15) != 0)
	    {
			memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
		    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
		    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
	        memcpy(m_StrCurrentData, m_Str3DS_Fld, sizeof (m_StrCurrentData));
	        memcpy(m_StrPreviousData, sProfileIdOld.cvk[3].cvkA, sizeof (m_StrPreviousData));
	        memcpy(m_StrFieldTag, "3DS-2_Code:", sizeof(m_StrFieldTag));

	        PopulateHistoryTable();
	    }
		if (strncmp ( m_Str3DS_Fld, (char *)sProfileIdOld.cvk[3].cvkB, 15) != 0)
	    {
			memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
		    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
		    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
	        memcpy(m_StrCurrentData, m_Str3DS_Fld, sizeof (m_StrCurrentData));
	        memcpy(m_StrPreviousData, sProfileIdOld.cvk[3].cvkB, sizeof (m_StrPreviousData));
	        memcpy(m_StrFieldTag, "3DS-2_Code:", sizeof(m_StrFieldTag));

	        PopulateHistoryTable();
	    }
	}

    if (strcmp ( m_StrNumPinRetries_Fld, (char *)sProfileIdOld.max_pin_retries) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrNumPinRetries_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.max_pin_retries, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Num of PIN retries", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( myUtilsObject.GenerateAmount (m_StrFloorLimitFld,12), (char *)sProfileIdOld.floor_limit) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFloorLimitFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.floor_limit, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Floor Limit", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strncmp ( m_StrPINValidationData, (char *)sProfileIdOld.cvk[2].cvkB, 12) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrPINValidationData, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[2].cvkB, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN VAL", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strncmp ( m_StrDecimalisationTable, (char *)sProfileIdOld.cvk[2].cvkA, 16) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrDecimalisationTable, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[2].cvkA, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "DES TAB", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
/*    if (strcmp ( m_StrTrackPinOffsetLoc_Fld, (char *)sProfileIdOld.track_offset) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrTrackPinOffsetLoc_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.track_offset, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Track PIN Offset Loc", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
		}   bug #556 */
    if (strcmp ( m_StrRemoteDest_Fld, (char *)sProfileIdOld.destination_name) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrRemoteDest_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.destination_name, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Remote Destination", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrName_Fld, (char *)sProfileIdOld.name) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrName_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.name, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Name", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrContact_Fld, (char *)sProfileIdOld.contact) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrContact_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.contact, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Contact", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrPhone_Fld, (char *)sProfileIdOld.phone) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrPhone_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.phone, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Phone", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrMerchSame_Fld, (char *)sProfileIdOld.merchant_same) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrMerchSame_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.merchant_same, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Num of Merchants", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrProcess_Fld, (char *)sProfileIdOld.nsp_process) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrProcess_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.nsp_process, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "NSP Process", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCVKAFld, (char *)sProfileIdOld.cvk[0].cvkA) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVKAFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[0].cvkA, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK A", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCVKBFld, (char *)sProfileIdOld.cvk[0].cvkB) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVKBFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvk[0].cvkB, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK B", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrKey3ds01, (char *)sKey3ds01Old.key3ds) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrKey3ds01, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sKey3ds01Old.key3ds, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Extd 3ds2.0 Key", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	/*if (strcmp ( (char *)sProfileId.pin.IbmOffset.pin_key, (char *)sProfileIdOld.pin.IbmOffset.pin_key) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileId.pin.IbmOffset.pin_key, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.IbmOffset.pin_key, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Key", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
		if (strcmp ( (char *)sProfileId.pin.IbmOffset.decimalization_tbl , (char *)sProfileIdOld.pin.IbmOffset.decimalization_tbl) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileId.pin.IbmOffset.decimalization_tbl , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.IbmOffset.decimalization_tbl, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Desim Table", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( (char *)sProfileId.pin.IbmOffset.offset_length, (char *)sProfileIdOld.pin.IbmOffset.offset_length) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileId.pin.IbmOffset.offset_length, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.IbmOffset.offset_length, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Offset Length", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( (char *)sProfileId.pin.IbmOffset.pin_offset_location, (char *)sProfileIdOld.pin.IbmOffset.pin_offset_location) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,sProfileId.pin.IbmOffset.pin_offset_location, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.IbmOffset.pin_offset_location, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Offset Loc", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( (char *)sProfileId.pin.IbmOffset.track_offset, (char *)sProfileIdOld.pin.IbmOffset.track_offset) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileId.pin.IbmOffset.track_offset, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.IbmOffset.track_offset, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Track Offset Loc", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.key_left, (char *)sProfileIdOld.pin.Visa.key_left) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.key_left, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.key_left, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Key Left", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 
	if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.key_right, (char *)sProfileIdOld.pin.Visa.key_right) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.key_right, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.key_right, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Key Right", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvv_location_tr1, (char *)sProfileIdOld.pin.Visa.pvv_location_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvv_location_tr1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvv_location_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVV Location Track1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvv_location_tr2, (char *)sProfileIdOld.pin.Visa.pvv_location_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvv_location_tr2, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvv_location_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVV Location Track2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvv_offset_tr1, (char *)sProfileIdOld.pin.Visa.pvv_offset_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvv_offset_tr1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvv_offset_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVV Offset Track1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvv_offset_tr2, (char *)sProfileIdOld.pin.Visa.pvv_offset_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvv_offset_tr2, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvv_offset_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVV Offset Track2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvkey_ind, (char *)sProfileIdOld.pin.Visa.pvkey_ind) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvkey_ind, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvkey_ind, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PV KEY Indicator", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
   
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvki_location_tr1, (char *)sProfileIdOld.pin.Visa.pvki_location_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvki_location_tr1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvki_location_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVKI Location Tr1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvki_location_tr2, (char *)sProfileIdOld.pin.Visa.pvki_location_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvki_location_tr2, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvki_location_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVKI Location Tr2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvki_length_tr1, (char *)sProfileIdOld.pin.Visa.pvki_length_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvki_length_tr1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvki_length_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVKI Length Tr1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
    if (strcmp ( (char *)sProfileIdPinVerif.pin.Visa.pvki_length_tr2, (char *)sProfileIdOld.pin.Visa.pvki_length_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, sProfileIdPinVerif.pin.Visa.pvki_length_tr2, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin.Visa.pvki_length_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PVKI Length Tr2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
	
	offset                      = sProfileIdOld.pin_offset_location;
    capture                     = sProfileIdOld.pin_capture_ind;
    datecheck                   = sProfileIdOld.expiration_date_check;
    dateloc                     = sProfileIdOld.expiration_date_location;
    authloc                     = sProfileIdOld.authorization_location;
    authmethod                  = sProfileIdOld.authorization_method;
    status                      = sProfileIdOld.status;
    verifmethod                 = sProfileIdOld.pin_verification_method;


	if ( m_StrCVVProcessing == "NO VALIDATION" )
         strCvvProcessing = "0";
	else if ( m_StrCVVProcessing == "VALIDATE, DENY" )
         strCvvProcessing = "1";
	else if ( m_StrCVVProcessing == "VALIDATE, DENY, AND REPORT" )
         strCvvProcessing = "2";
	else if ( m_StrCVVProcessing == "VALIDATE, APPROVE, AND REPORT" )
         strCvvProcessing = "3";

	if (strcmp ( strCvvProcessing, (char *)sProfileIdOld.cvv_processing) != 0)
    {
		strCvvProcessing = sProfileIdOld.cvv_processing;
		if ( strCvvProcessing == "0" )
			strCvvProcessing = "NO VALIDATION";
		else if ( strCvvProcessing == "1" )
			strCvvProcessing = "VALIDATE, DENY";
		else if ( strCvvProcessing == "2" )
			strCvvProcessing = "VALIDATE, DENY, AND REPORT";
		else if ( strCvvProcessing == "3" )
			strCvvProcessing = "VALIDATE, APPROVE, AND REPORT";

		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVVProcessing, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, strCvvProcessing, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVV Processing", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
	
    if (offset == "T")
		temp = "TRACK DATA";
	else if (offset == "C")
		temp = "CARD RECORD";
/*	else if (offset == "B")
		temp = "BOTH";*/

	if (strcmp ( m_StrPinOffsetLocation_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrPinOffsetLocation_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Offset Location", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
		
	if (capture == "D")
		temp = "DECLINE";
	else if (capture == "C")
		temp = "CAPTURE";

	if (strcmp ( m_StrPinCapIndicator_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrPinCapIndicator_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Capture Indicator", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

   if ( datecheck == "N" )
      temp = "NO";
   else if ( datecheck == "Y" )
      temp = "YES";
   else if ( datecheck == "E" )
      temp = "YES - EXPANDED";

   if ( strcmp(m_StrExpDateCheck_Fld, temp) != 0 )
   {
      memset( m_StrCurrentData,  0x00, sizeof (m_StrCurrentData) );
      memset( m_StrPreviousData, 0x00, sizeof (m_StrPreviousData));
      memset( m_StrFieldTag,     0x00, sizeof (m_StrFieldTag)    );

      memcpy( m_StrCurrentData,  m_StrExpDateCheck_Fld,  sizeof(m_StrCurrentData) );
      memcpy( m_StrPreviousData, temp,                   sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Expiration Date Check", sizeof(m_StrFieldTag)    );

      PopulateHistoryTable();
   }

	if (dateloc == "0")
		temp = "CARD";
	else if (dateloc == "1")
		temp = "CARDHOLDER RECORD";	
   else if (dateloc == "2")
		temp = "BOTH";	
   else if (dateloc == "3")
		temp = "BOTH WITH REVERSED DATE";	

	if (strcmp ( m_StrExpDateLocation_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrExpDateLocation_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Expiration Date Location", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (authloc == "L")
		temp = "LOCAL";
	else if (authloc == "A")
		temp = "LOCAL/REMOTE OVERRIDE APPROVALS";
	else if (authloc == "D")
		temp = "LOCAL/REMOTE OVERRIDE DECLINES";	
	else if (authloc == "S")
		temp = "REMOTE/LOCAL STAND-IN";	
	else if (authloc == "R")
		temp = "REMOTE";	

	if (strcmp ( m_StrAuthLocation_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrAuthLocation_Fld , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Auth Location", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	
	if (authmethod == "P")
		temp = "POSITIVE";
	else if (authmethod == "B")
		temp = "POSITIVE WITH BALANCES";
	else if (authmethod == "N")
		temp = "NEGATIVE";
	else if (authmethod == "D")
		temp = "PRIMARY POSITIVE WITH BALANCES";
	else if (authmethod == "V")
		temp = "OFFUS VELOCITY";
	else if (authmethod == "O")
		temp = "NEGATIVE WITH OFFUS VELOCITY";
	else if (authmethod == "F")
		temp = "NEGATIVE OFFUS FRAUD VELOCITY";
  

	if (strcmp ( m_StrAuthMethod_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAuthMethod_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Auth Method", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (status == "A")
		temp = "ACTIVE";
	else if (status == "R")
		temp = "RESTRICTED";	

	if (strcmp ( m_StrStatus_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrStatus_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Status", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
	
	if (verifmethod == "A")
		temp = "IDENTIKEY";
	else if (verifmethod == "B")
		temp = "EIDENTIKEY";
	else if (verifmethod == "C")
		temp = "IBM 3624";	
	else if (verifmethod == "D")
		temp = "IBM OFFSET";	
	else if (verifmethod == "E")
		temp = "VISA";	
	else if (verifmethod == "F")
		temp = "DESBILEVEL";	
	else if (verifmethod == "G")
		temp = "DEIBOLD";	
	else if (verifmethod == "H")
		temp = "NCR";	
	//else if (verifmethod == "I")
	//	temp = "CVV/CVC";
	else if (verifmethod == "J")
		temp = "NONE";	

	if (strcmp ( m_StrPinVerifMethod_Fld, temp) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrPinVerifMethod_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, temp, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Verif Method", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }    
	
	if (strcmp ( m_StrCodeOffset1Fld, (char *)sProfileIdOld.service_code_offset_tr1) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCodeOffset1Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.service_code_offset_tr1, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Service Code Offset Track 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 
	if (strcmp ( m_StrCodeOffset2Fld, (char *)sProfileIdOld.service_code_offset_tr2) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCodeOffset2Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.service_code_offset_tr2, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Service Code Offset Track 2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 
  
	if (strcmp ( m_StrCVVLengthFld, (char *)sProfileIdOld.cvv_length) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCVVLengthFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.cvv_length, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVV Length", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } 
	if (m_Cntlmc3dpanFlag.GetCheck() == 1)
	{
		temp = '1';
	}
	else
	{
		temp = '0';
	}
	if ( temp != sProfileIdOld.cvk[1].cvkA[0] )
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        m_StrPreviousData[0] = sProfileIdOld.cvk[1].cvkA[0];
		memcpy(m_StrFieldTag, "MC 3D PAN", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (m_Cntlmc3ddstxnFlag.GetCheck() == 1)
		{
		temp = '1';
	}
	else
	{
		temp = '0';
	}
	if (temp != sProfileIdOld.cvk[1].cvkA[1] )
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        m_StrPreviousData[0] = sProfileIdOld.cvk[1].cvkA[1];
		memcpy(m_StrFieldTag, "MC 3D DS TXN ID", sizeof(m_StrFieldTag));
	
        PopulateHistoryTable();
    }

	if (m_Cntlmc3dcurrencyFlag.GetCheck() == 1)
		{
		temp = '1';
	}
	else
	{
		temp = '0';
	}
	if (temp != sProfileIdOld.cvk[1].cvkA[2] )
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        m_StrPreviousData[0] = sProfileIdOld.cvk[1].cvkA[2];
		memcpy(m_StrFieldTag, "MC 3D TXN CURRENCY", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (m_Cntlmc3dtxnamtFlag.GetCheck() == 1)
	{
		temp = '1';
	}
	else
	{
		temp = '0';
	}
	if (temp != sProfileIdOld.cvk[1].cvkA[3] )
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        m_StrPreviousData[0] = sProfileIdOld.cvk[1].cvkA[3];
        memcpy(m_StrFieldTag, "MC 3D TXN AMOUNT", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (m_Cntlmc3dmechnameFlag.GetCheck() == 1)
		{
		temp = '1';
	}
	else
	{
		temp = '0';
	}
	if (temp != sProfileIdOld.cvk[1].cvkA[4] )
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        m_StrPreviousData[0] = sProfileIdOld.cvk[1].cvkA[4];
        memcpy(m_StrFieldTag, "MC 3D MERCH NAME", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	
	if (m_CntlCntryLimitsCheckFlag.GetCheck() == 1)
		temp = "Y";
	else
		temp = "N";
	

	
	if (strcmp ( temp, (char *)sProfileIdOld.country_limits_flag) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.country_limits_flag, sizeof (sProfileIdOld.country_limits_flag));
        memcpy(m_StrFieldTag, "Country Limits Flag", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (m_CntlCSCKDesc.GetCheck() == 1)
		temp = "Y";
	else
		temp = "N";
	
	if (strcmp ( temp, (char *)sProfileIdOld.aba_nbr) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.aba_nbr, sizeof (sProfileIdOld.aba_nbr));
        strcpy(m_StrFieldTag, "CSCK Description Flag");

        PopulateHistoryTable();
    }


	if (m_CntlSlen3Dcak.GetCheck() == 1)
		temp = "Y";
	else
		temp = "N";
	
	if (strcmp ( temp, (char *)sProfileIdOld.aba_nbr) != 0)
    {  
        memcpy(m_StrCurrentData,temp , sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.aba_nbr, sizeof (sProfileIdOld.aba_nbr));
        strcpy(m_StrFieldTag, "Single length key flag");

        PopulateHistoryTable();
    }
//	CString temp1;

/*f ( m_PinChecking.GetCheck() == 1 )
		temp1 = "Y";
	else
		temp1 = "N";*/

/*	if (strcmp ( temp1, (char *)sProfileIdOld.pin_checking_flag) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, temp1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.pin_checking_flag, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "PIN Checking", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } */

/*f ( m_VelocityChecking.GetCheck() == 1 )
		temp1 = "Y";
	else
		temp1 = "N";*/

/*	if (strcmp ( temp1, (char *)sProfileIdOld.velocity_checking_flag) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, temp1, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.velocity_checking_flag, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Velocity Checking", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    } */

        return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsProcessing::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
	

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));
	
    strcpy ( strPrimaryKey, m_StrProfileId_Fld);
    strcat ( strPrimaryKey, m_StrType_Fld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,(LPCTSTR)"CAF01",sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
      temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,(LPCTSTR)strPrimaryKey,sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,sizeof (sHistoryData.primary_key.unique_id));    
    memcpy ((LPTSTR) sHistoryData.time_modified,(LPCTSTR)strTime,sizeof (sHistoryData.time_modified));             
    memcpy ((LPTSTR) sHistoryData.current_data,(LPCTSTR)m_StrCurrentData,sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data,(LPCTSTR)m_StrPreviousData,sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,(LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR) sHistoryData.field_tag,(LPCTSTR)m_StrFieldTag,sizeof (sHistoryData.field_tag));
	
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );

	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}


void CCardsProcessing::OnPinVerifButton() 
{
    //CCardsPinVerification pinDlg;
	CCardsPinVerifVisa pinDlgVisa;

    
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);
	/*if ( m_StrPinVerifMethod_Fld == "IBM OFFSET" )
	{	
		ZeroMemory(&sProfileId, sizeof (CAF01));

		memcpy (&pinDlg.sProfileIdOld, &sProfileIdOld,sizeof (CAF01));

		if (m_nDialogMode == MODE_ADD)
		pinDlg.m_nDialogMode = MODE_ADD;
		
		if (m_nDialogMode == MODE_VIEW)
		pinDlg.m_nDialogMode = MODE_VIEW;
		
		if (m_nDialogMode == MODE_EDIT)
		pinDlg.m_nDialogMode = MODE_EDIT;

		if (m_nDialogMode == MODE_DONE)
		pinDlg.m_nDialogMode = MODE_DONE;
		
		nReturnCode = pinDlg.DoModal();

		if ( nReturnCode == IDOK)
			pinDlg.PopulateDatabaseFromPinVerificationDialog(&sProfileId);
	}*/

	if ( m_StrPinVerifMethod_Fld == "IBM OFFSET" )
	{	
		ZeroMemory(&sProfileIdPinVerif, sizeof (CAF01));

		memcpy (&pinDlgVisa.sProfileIdOld, &sProfileIdOld,sizeof (CAF01));

		if (m_nDialogMode == MODE_ADD)
		pinDlgVisa.m_nDialogMode = MODE_ADD;
		
		if (m_nDialogMode == MODE_VIEW)
		pinDlgVisa.m_nDialogMode = MODE_VIEW;
		
		if (m_nDialogMode == MODE_EDIT)
		pinDlgVisa.m_nDialogMode = MODE_EDIT;

		if (m_nDialogMode == MODE_DONE)
		pinDlgVisa.m_nDialogMode = MODE_DONE;
		
		nReturnCode = pinDlgVisa.DoModal();

		if ( nReturnCode == IDOK)
      {
		 pinDlgVisa.PopulateDatabaseFromPinVerifVisaDialog(&sProfileIdPinVerif);
		 memcpy ( &sProfileIdOld.pin.Visa, &sProfileIdPinVerif.pin.Visa, sizeof ( sProfileIdOld.pin.Visa ) );
         pinDlgVisa.m_nDialogMode = MODE_EDIT;
         m_nDialogMode = MODE_EDIT;
      }
	}

	else if ( m_StrPinVerifMethod_Fld == "VISA" )
	{	
		ZeroMemory(&sProfileIdPinVerif, sizeof (CAF01));

		memcpy (&pinDlgVisa.sProfileIdOld, &sProfileIdOld,sizeof (CAF01));

		if (m_nDialogMode == MODE_ADD)
		pinDlgVisa.m_nDialogMode = MODE_ADD;
		
		if (m_nDialogMode == MODE_VIEW)
		pinDlgVisa.m_nDialogMode = MODE_VIEW;
		
		if (m_nDialogMode == MODE_EDIT)
		pinDlgVisa.m_nDialogMode = MODE_EDIT;

		if (m_nDialogMode == MODE_DONE)
		pinDlgVisa.m_nDialogMode = MODE_DONE;
		
		nReturnCode = pinDlgVisa.DoModal();

		if ( nReturnCode == IDOK)
      {
			pinDlgVisa.PopulateDatabaseFromPinVerifVisaDialog(&sProfileIdPinVerif);
         memcpy ( &sProfileIdOld.pin.Visa, &sProfileIdPinVerif.pin.Visa, sizeof ( sProfileIdOld.pin.Visa ) );
         pinDlgVisa.m_nDialogMode = MODE_EDIT;
         m_nDialogMode = MODE_EDIT;
      }
	}
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
        
 }

void CCardsProcessing::OnSelchangePinVerificationMethodFld() 
{

	UpdateData(TRUE);
	if ( m_StrPinVerifMethod_Fld == "NONE" )
		m_PinVerifButton.EnableWindow(FALSE);
	else 
		m_PinVerifButton.EnableWindow(TRUE);
}

void CCardsProcessing::OnNetworkButton() 
{
	CNameList listDlg;
    int nReturnCode;

	UpdateData (TRUE);
	m_StrMessageLbl = "";

    listDlg.m_nIndicator = MODE_NETWORK;
    listDlg.strPrimaryKey1 = m_StrRemoteDest_Fld;
	listDlg.strNetworkType = "I";

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrRemoteDest_Fld = name;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}
void CCardsProcessing::OnCSCKDesc() 
{
}

void CCardsProcessing::OnPosEntryModes() 
{
	if(m_CntlPosEntryModes.GetCheck() == 1)
	{
		if(m_pPosDlg == NULL)
			m_pPosDlg  = new CPosEntryModesListDlg(this);
		m_pPosDlg->SetPosEntryModes(this->m_PosEntryModes[0],this->m_PosEntryModes[1]);
		if(IDCANCEL == m_pPosDlg ->DoModal())
		{
			this->m_PosEntryModes[0] = (unsigned char)0x40;
			this->m_PosEntryModes[1] = (unsigned char)0x00;
		}
		else
		{
			this->m_PosEntryModes[0] = m_pPosDlg->GetPosEntryModesMSB();
			this->m_PosEntryModes[1] = m_pPosDlg->GetPosEntryModesLSB();

		}
		
	}
}
