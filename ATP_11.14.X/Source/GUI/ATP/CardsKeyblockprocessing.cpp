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


    
*
************************************************************************************/
// CardsKeyblockProcessing.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsKeyblockProcessing.h"
//#include "CardsPinVerification.h"
//#include "CardsPinVerifVisa.h"
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

IMPLEMENT_DYNCREATE(CCardsKeyblockProcessing, CPropertyPage)

CCardsKeyblockProcessing::CCardsKeyblockProcessing() : CPropertyPage(CCardsKeyblockProcessing::IDD)
{
	//{{AFX_DATA_INIT(CCardsKeyblockProcessing)

	m_StrProfileId_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrMessageLbl = _T("");


	
	m_StrCvk_Mag_KeyblockFld	 =_T("");
	m_StrCvk_Acq_3DS_KeyblockFld =_T("");
	m_StrCvk_Pin_VerificationFld =_T("");
	m_StrCvk_Mkac_KeyblockFld =_T("");
	m_StrCsck_Iss_3ds_KeyblockFld =_T("");
	m_StrCsck_Mag_KeyblockFld =_T("");
	m_StrDci_3ds_Keyblock_Fld =_T("");
	m_StrCvk_3ds_Cak_KeyblockFld =_T("");
	//}}AFX_DATA_INIT
}

CCardsKeyblockProcessing::~CCardsKeyblockProcessing()
{
}

void CCardsKeyblockProcessing::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsProcessing)

	DDX_Control(pDX, IDC_CVK_MAG_KEY_BLOCK, m_CntlCvk_Mag_KeyblockFld);
	DDX_Control(pDX, IDC_CVK_ACQ_KEY_BLOCK, m_CntlCvk_Acq_3DS_KeyblockFld);
	DDX_Control(pDX, IDC_CVK_PIN_KEY_BLOCK, m_CntlCvk_Pin_VerificationFld);
	DDX_Control(pDX, IDC_CVK_MKAC_KEY_BLOCK, m_CntlCvk_Mkac_KeyblockFld);
	DDX_Control(pDX, IDC_CSCK_ISS_KEY_BLOCK, m_CntlCsck_Iss_3ds_KeyblockFld);
	DDX_Control(pDX, IDC_CSCK_MAG_KEY_BLOCK, m_CntlCsck_Mag_KeyblockFld);
	DDX_Control(pDX, IDC_DCI_3DS_KEY_BLOCK, m_CntlDci_3ds_Keyblock_Fld);
	DDX_Control(pDX, IDC_CVK_3DS_KEY_BLOCK, m_CntlCvk_3ds_Cak_KeyblockFld);
	DDX_Control(pDX, IDC_CAF01_KEYBLK_PROFILE, m_CntlProfileId_Fld);
	DDX_Control(pDX, IDC_CAF01_KEYBLK_TYPE, m_CntlType_Fld);


	DDX_Text(pDX, IDC_CAF01_KEYBLK_PROFILE, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);	

	DDX_Text(pDX, IDC_CAF01_KEYBLK_TYPE, m_StrType_Fld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);

	DDX_Text(pDX, IDC_CVK_3DS_KEY_BLOCK, m_StrCvk_3ds_Cak_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCvk_3ds_Cak_KeyblockFld, 128);

	DDX_Text(pDX, IDC_DCI_3DS_KEY_BLOCK, m_StrDci_3ds_Keyblock_Fld);
	DDV_MaxChars(pDX, m_StrDci_3ds_Keyblock_Fld, 128);

	DDX_Text(pDX, IDC_CSCK_MAG_KEY_BLOCK, m_StrCsck_Mag_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCsck_Mag_KeyblockFld, 128);
	DDX_Text(pDX, IDC_CVK_ACQ_KEY_BLOCK, m_StrCvk_Acq_3DS_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCvk_Acq_3DS_KeyblockFld, 128);
	DDX_Text(pDX, IDC_CVK_MAG_KEY_BLOCK, m_StrCvk_Mag_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCvk_Mag_KeyblockFld, 128);
	DDX_Text(pDX, IDC_CVK_PIN_KEY_BLOCK, m_StrCvk_Pin_VerificationFld);
	DDV_MaxChars(pDX, m_StrCvk_Pin_VerificationFld, 128);
	DDX_Text(pDX, IDC_CVK_MKAC_KEY_BLOCK, m_StrCvk_Mkac_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCvk_Mkac_KeyblockFld, 128);
	DDX_Text(pDX, IDC_CSCK_ISS_KEY_BLOCK, m_StrCsck_Iss_3ds_KeyblockFld);
	DDV_MaxChars(pDX, m_StrCsck_Iss_3ds_KeyblockFld, 128);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsKeyblockProcessing, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsKeyblockProcessing)
	/*ON_BN_CLICKED(IDC_PIN_VERIF_BUTTON, OnPinVerifButton)
	ON_CBN_SELCHANGE(IDC_PIN_VERIFICATION_METHOD_FLD, OnSelchangePinVerificationMethodFld)
	ON_BN_CLICKED(IDC_NETWORK_BUTTON, OnNetworkButton)
	ON_BN_CLICKED(IDC_POS_ENTRY_MODES, OnPosEntryModes)
	ON_BN_CLICKED(IDC_POS_ENTRY_MODES, OnCSCKDesc)*/
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsProcessing message handlers

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsKeyblockProcessing::OnInitDialog() 
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
            PopulateProcessingDialogFromDatabase ( sCaf01_keyblkOld );        

    /*if (m_nDialogMode == MODE_ADD )
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
    }*/
		    
	}
    
    if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }

    /*ZeroMemory(&sProfileIdPinVerif, sizeof (CAF01));	*/
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsKeyblockProcessing::PopulateProcessingDialogFromDatabase( CAF01_KEYBLOCK sProfileId )
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
  
    ZeroMemory ( &sCaf01_keyblkOld, sizeof (CAF01_KEYBLOCK));
	/*ZeroMemory ( &sKey3ds01Old, sizeof (KEY3DS01));*/

	m_StrProfileId_Fld = sProfileId.primary_key.caf_profile_id;
	m_StrType_Fld = sProfileId.primary_key.type;

	if ( strcmp (m_StrType_Fld, "C") == 0 )
		m_StrType_Fld = "CREDIT";
	else if ( strcmp (m_StrType_Fld, "F") == 0 )
		m_StrType_Fld = "FLEET";
	else 
		m_StrType_Fld = "DEBIT";
		

	m_StrCvk_Mag_KeyblockFld	  =  sProfileId.Cvk_Mag_Keyblock;
	m_StrCvk_Acq_3DS_KeyblockFld  =  sProfileId.Cvk_Acq_3DS_Keyblock;
	m_StrCvk_Pin_VerificationFld  =  sProfileId.Cvk_Pin_Verification;
	m_StrCvk_Mkac_KeyblockFld 	  =  sProfileId.Cvk_Mkac_Keyblock;
	m_StrCsck_Iss_3ds_KeyblockFld =  sProfileId.Csck_Iss_3ds_Keyblock;
	m_StrCsck_Mag_KeyblockFld 	  =  sProfileId.Csck_Mag_Keyblock;
	m_StrDci_3ds_Keyblock_Fld     =  sProfileId.Dci_3ds_Keyblock;
	m_StrCvk_3ds_Cak_KeyblockFld  =  sProfileId.Cvk_3ds_Cak_Keyblock;
 

    
    UpdateData(FALSE);

    memcpy ( &sCaf01_keyblkOld, &sProfileId, sizeof ( CAF01_KEYBLOCK));
    /*memcpy ( &sProfileIdPinVerif, &sProfileId, sizeof ( CAF01));*/

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsKeyblockProcessing::PopulateDatabaseFromProcessingDialog( pCAF01_KEYBLOCK psProfileId )
{
    char strStatus[2]; 
	char strCvvProcessing[2];
   CMyUtils myUtilsObject;
	
	/*if ( !m_bProcessing )
		return TRUE;*/
  
	memcpy ( psProfileId->Cvk_Mag_Keyblock, m_StrCvk_Mag_KeyblockFld, sizeof ( psProfileId->Cvk_Mag_Keyblock));
	memcpy ( psProfileId->Cvk_Acq_3DS_Keyblock, m_StrCvk_Acq_3DS_KeyblockFld, sizeof ( psProfileId->Cvk_Acq_3DS_Keyblock));
	memcpy ( psProfileId->Cvk_Pin_Verification, m_StrCvk_Pin_VerificationFld, sizeof ( psProfileId->Cvk_Pin_Verification));
	memcpy ( psProfileId->Cvk_Mkac_Keyblock, m_StrCvk_Mkac_KeyblockFld, sizeof ( psProfileId->Cvk_Mkac_Keyblock));
	memcpy ( psProfileId->Csck_Iss_3ds_Keyblock, m_StrCsck_Iss_3ds_KeyblockFld, sizeof ( psProfileId->Csck_Iss_3ds_Keyblock));
	memcpy ( psProfileId->Csck_Mag_Keyblock, m_StrCsck_Mag_KeyblockFld, sizeof ( psProfileId->Csck_Mag_Keyblock));
	memcpy ( psProfileId->Dci_3ds_Keyblock, m_StrDci_3ds_Keyblock_Fld, sizeof ( psProfileId->Dci_3ds_Keyblock));
	memcpy ( psProfileId->Cvk_3ds_Cak_Keyblock, m_StrCvk_3ds_Cak_KeyblockFld, sizeof ( psProfileId->Cvk_3ds_Cak_Keyblock));

    
    return TRUE;

} 

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsKeyblockProcessing::CleanProcessingDialog()
{
	
	m_StrProfileId_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrMessageLbl = _T("");

	m_StrCvk_Mag_KeyblockFld	 =_T("");
	m_StrCvk_Acq_3DS_KeyblockFld =_T("");
	m_StrCvk_Pin_VerificationFld =_T("");
	m_StrCvk_Mkac_KeyblockFld =_T("");
	m_StrCsck_Iss_3ds_KeyblockFld =_T("");
	m_StrCsck_Mag_KeyblockFld =_T("");
	m_StrDci_3ds_Keyblock_Fld =_T("");
	m_StrCvk_3ds_Cak_KeyblockFld =_T("");
	
	UpdateData(FALSE);

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsKeyblockProcessing::EnableProcessingDialog(int nStatus)
{

   /*m_CntlCSCKFld.EnableWindow(nStatus);
   m_CntlFloorLimitFld.EnableWindow(nStatus);
   m_CntlCntryLimitsCheckFlag.EnableWindow(nStatus);
   m_CntlPosEntryModes.EnableWindow(nStatus);
   m_CntlCSCKDesc.EnableWindow(nStatus);
   m_CntlSlen3Dcak.EnableWindow(nStatus);
   m_CntlMKACFld.EnableWindow(nStatus);
   m_CntlMCCAK_Fld.EnableWindow(nStatus);
   m_Cntl3DCAKFld.EnableWindow(nStatus);
   m_CntlPINValidationData.EnableWindow(nStatus);
   m_CntlDecimalisationTable.EnableWindow(nStatus);*/
   
	m_CntlCvk_Mag_KeyblockFld.EnableWindow(nStatus);
	m_CntlCvk_Acq_3DS_KeyblockFld.EnableWindow(nStatus);
	m_CntlCvk_Pin_VerificationFld.EnableWindow(nStatus);
	m_CntlCvk_Mkac_KeyblockFld.EnableWindow(nStatus);
	m_CntlCsck_Iss_3ds_KeyblockFld.EnableWindow(nStatus);
	m_CntlCsck_Mag_KeyblockFld.EnableWindow(nStatus);
	m_CntlDci_3ds_Keyblock_Fld.EnableWindow(nStatus);
	m_CntlType_Fld.EnableWindow(nStatus);
	m_CntlProfileId_Fld.EnableWindow(nStatus);
	m_CntlCvk_3ds_Cak_KeyblockFld.EnableWindow(nStatus);


   return;
}
    
 /************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/   

BOOL CCardsKeyblockProcessing::OnCommand(WPARAM wParam, LPARAM lParam) 
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

BOOL CCardsKeyblockProcessing::CompareStructures()
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

    if (strcmp ( m_StrCvk_Mag_KeyblockFld, (char *)sCaf01_keyblkOld.Cvk_Mag_Keyblock) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCvk_Mag_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Cvk_Mag_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK_Mag_Keyblock", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrCvk_Acq_3DS_KeyblockFld, (char *)sCaf01_keyblkOld.Cvk_Acq_3DS_Keyblock) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCvk_Acq_3DS_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Cvk_Acq_3DS_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK_ACQ_3DS_Keyblock", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strncmp ( m_StrCvk_Pin_VerificationFld, (char *)sCaf01_keyblkOld.Cvk_Pin_Verification, 128) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCvk_Pin_VerificationFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Cvk_Pin_Verification, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK_PIN_Verfcn", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (strncmp ( m_StrCvk_Mkac_KeyblockFld, (char *)sCaf01_keyblkOld.Cvk_Mkac_Keyblock, 128) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCvk_Mkac_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Cvk_Mkac_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK_MKAC_Keyblock", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	 if (strncmp ( m_StrCsck_Iss_3ds_KeyblockFld, (char *)sCaf01_keyblkOld.Csck_Iss_3ds_Keyblock, 128) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCsck_Iss_3ds_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Csck_Iss_3ds_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CSCK_ISS_3DS", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (strncmp ( m_StrCsck_Mag_KeyblockFld, (char *)sCaf01_keyblkOld.Csck_Mag_Keyblock, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCsck_Mag_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Csck_Mag_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CSCK_MAG", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

	if (strncmp ( m_StrDci_3ds_Keyblock_Fld, (char *)sCaf01_keyblkOld.Dci_3ds_Keyblock, 15) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrDci_3ds_Keyblock_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Dci_3ds_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "MC_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }


    if (strcmp ( m_StrCvk_3ds_Cak_KeyblockFld, (char *)sCaf01_keyblkOld.Cvk_3ds_Cak_Keyblock) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrCvk_3ds_Cak_KeyblockFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sCaf01_keyblkOld.Cvk_3ds_Cak_Keyblock, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "CVK_3DS_CAK", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
 
        return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsKeyblockProcessing::PopulateHistoryTable()
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



