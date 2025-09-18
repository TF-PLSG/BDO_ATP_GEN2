// MerchantGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantGeneral.h"
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
//extern merch_ctr;
extern int merch_ctr;

extern CString  MerchantID;
extern CString  OrganizationID;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantGeneral property page

IMPLEMENT_DYNCREATE(CMerchantGeneral, CPropertyPage)

CMerchantGeneral::CMerchantGeneral() : CPropertyPage(CMerchantGeneral::IDD)
{
	//{{AFX_DATA_INIT(CMerchantGeneral)
	m_StrMallCodeFld = _T("");
	m_StrDivisionNumberFld = _T("");
	m_StrHCCodeFld = _T("");
	m_StrJCBSubcodeFld = _T("");
	m_StrMCFCCCodeFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrPOSCodeFld = _T("");
	m_StrRegionNumberFld = _T("");
	m_StrVIPCodeFld = _T("");
	m_StrVSTCCCodeFld = _T("");
	m_StrSolicitCodeFld = _T("");
	m_StrVoiceCategoryCodeFld = _T("");
	m_StrClassCodeFld = _T("");
	m_BoolDeferredMerchant = FALSE;
	m_BoolHouseCardMerchant = FALSE;
	m_StrDefEndDateFld = _T("");
	m_StrDefStartDateFld = _T("");
	m_StrGroupOutletCodeFld = _T("");
	m_StrGroupTypeFld = _T("");
	m_StrFPIFld = _T("");
	m_StrOwnMerchant = _T("");
	m_StrMasterCardAssignedId = _T("");
	m_StrCUPAcceptorID = _T("");     
	m_StrVisaMVV = _T("");
    m_StrMCMerchantcntryOrigin = _T("");
	//}}AFX_DATA_INIT
}

CMerchantGeneral::~CMerchantGeneral()
{
}

void CMerchantGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantGeneral)
	DDX_Control(pDX, IDC_MC_ASSIGNED_ID, m_CntlMasterCardAssignedId);
	DDX_Control(pDX, IDC_VIP_MERCHANT, m_CntlVIPMerchant);
	DDX_Control(pDX, IDC_OWN_MERCHANT_FLD, m_CntlOwnMerchant);
	DDX_Control(pDX, IDC_FPI_FLD, m_CntlFPIFld);
	DDX_Control(pDX, IDC_RELOAD_MERCHANT, m_cntlReloadMerchant);
	DDX_Control(pDX, IDC_GROUP_TYPE_FLD, m_CntlGroupTypeFld);
	DDX_Control(pDX, IDC_GROUP_OUTLET_FLD, m_CntlGroupOutletCodeFld);
	DDX_Control(pDX, IDC_DEF_START_DATE_FLD, m_CntlDefStartDateFld);
	DDX_Control(pDX, IDC_DEF_END_DATE_FLD, m_CntlDefEndDateFld);
	DDX_Control(pDX, IDC_VOICE_CATEGORY_CODE_FLD, m_CntlVoiceCategoryCodeFld);
	DDX_Control(pDX, IDC_VS_TCC_CODE_FLD, m_CntlVSTCCCodeFld);
	DDX_Control(pDX, IDC_VIP_CODE_FLD, m_CntlVIPCodeFld);
	DDX_Control(pDX, IDC_SOLICIT_CODE_FLD, m_CntlSolicitCodeFld);
	DDX_Control(pDX, IDC_RPS_MERCHANT, m_RPSMerchant);
	DDX_Control(pDX, IDC_REGION_NUMBER_FLD, m_CntlRegionNumberFld);
	DDX_Control(pDX, IDC_PROCUREMENT, m_Procurement);
	DDX_Control(pDX, IDC_POS_CODE_FLD, m_CntlPOSCodeFld);
	DDX_Control(pDX, IDC_MC_FCC_CODE_FLD, m_CntlMCFCCCodeFld);
	DDX_Control(pDX, IDC_MALL_CODE_FLD, m_CntlMallCodeFld);
	DDX_Control(pDX, IDC_JCB_SUBCODE_FLD, m_CntlJCBSubcodeFld);
	DDX_Control(pDX, IDC_HOUSE_CARD_MERCHANT, m_HouseCardMerchant);
	DDX_Control(pDX, IDC_HC_CODE_FLD, m_CntlHCCodeFld);
	DDX_Control(pDX, IDC_DIVISION_NUMBER_FLD, m_CntlDivisionNumberFld);
	DDX_Control(pDX, IDC_DEFERRED_MERCHANT, m_DeferredMerchant);
	DDX_Control(pDX, IDC_CASH_BONUS, m_CashBonus);
	DDX_Control(pDX, IDC_CUP_ACP_ID, m_CntlCUPAcceptorID);
	DDX_Control(pDX, IDC_VISA_MVV, m_CntlVisaMVV);
	DDX_Control(pDX, IDC_MERCHANT_CNTRY_ORIGIN, m_CntlMCMerchantcntryOrigin);
	DDX_Control(pDX, IDC_PREAUTH_COMPLETION_FLAG, m_CntPreauthValidationFlag);
	DDX_Text(pDX, IDC_MALL_CODE_FLD, m_StrMallCodeFld);
	DDV_MaxChars(pDX, m_StrMallCodeFld, 3);
	DDX_Text(pDX, IDC_DIVISION_NUMBER_FLD, m_StrDivisionNumberFld);
	DDV_MaxChars(pDX, m_StrDivisionNumberFld, 3);
	DDX_Text(pDX, IDC_HC_CODE_FLD, m_StrHCCodeFld);
	DDV_MaxChars(pDX, m_StrHCCodeFld, 3);
	DDX_Text(pDX, IDC_JCB_SUBCODE_FLD, m_StrJCBSubcodeFld);
	DDV_MaxChars(pDX, m_StrJCBSubcodeFld, 2);
	DDX_Text(pDX, IDC_MC_FCC_CODE_FLD, m_StrMCFCCCodeFld);
	DDV_MaxChars(pDX, m_StrMCFCCCodeFld, 1);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_POS_CODE_FLD, m_StrPOSCodeFld);
	DDV_MaxChars(pDX, m_StrPOSCodeFld, 1);
	DDX_Text(pDX, IDC_REGION_NUMBER_FLD, m_StrRegionNumberFld);
	DDV_MaxChars(pDX, m_StrRegionNumberFld, 3);
	DDX_Text(pDX, IDC_VIP_CODE_FLD, m_StrVIPCodeFld);
	DDV_MaxChars(pDX, m_StrVIPCodeFld, 4);
	DDX_Text(pDX, IDC_VS_TCC_CODE_FLD, m_StrVSTCCCodeFld);
	DDV_MaxChars(pDX, m_StrVSTCCCodeFld, 1);
	DDX_Text(pDX, IDC_SOLICIT_CODE_FLD, m_StrSolicitCodeFld);
	DDV_MaxChars(pDX, m_StrSolicitCodeFld, 3);
	DDX_Text(pDX, IDC_VOICE_CATEGORY_CODE_FLD, m_StrVoiceCategoryCodeFld);
	DDV_MaxChars(pDX, m_StrVoiceCategoryCodeFld, 4);
	DDX_Text(pDX, IDC_CLASS_CODE_FLD, m_StrClassCodeFld);
	DDX_Check(pDX, IDC_DEFERRED_MERCHANT, m_BoolDeferredMerchant);
	DDX_Check(pDX, IDC_HOUSE_CARD_MERCHANT, m_BoolHouseCardMerchant);
	DDX_Text(pDX, IDC_DEF_END_DATE_FLD, m_StrDefEndDateFld);
	DDV_MaxChars(pDX, m_StrDefEndDateFld, 10);
	DDX_Text(pDX, IDC_DEF_START_DATE_FLD, m_StrDefStartDateFld);
	DDV_MaxChars(pDX, m_StrDefStartDateFld, 10);
	DDX_Text(pDX, IDC_GROUP_OUTLET_FLD, m_StrGroupOutletCodeFld);
	DDX_Text(pDX, IDC_GROUP_TYPE_FLD, m_StrGroupTypeFld);
	DDV_MaxChars(pDX, m_StrGroupTypeFld, 4);
	DDX_Text(pDX, IDC_FPI_FLD, m_StrFPIFld);
	DDX_Text(pDX, IDC_OWN_MERCHANT_FLD, m_StrOwnMerchant);
	DDV_MaxChars(pDX, m_StrOwnMerchant, 10);
	DDX_Text(pDX, IDC_MC_ASSIGNED_ID, m_StrMasterCardAssignedId);
	DDV_MaxChars(pDX, m_StrMasterCardAssignedId, 6);
	DDX_Text(pDX, IDC_CUP_ACP_ID, m_StrCUPAcceptorID);
	DDV_MaxChars(pDX, m_StrCUPAcceptorID, 15);
	DDX_Text(pDX, IDC_VISA_MVV, m_StrVisaMVV);
	DDV_MaxChars(pDX, m_StrVisaMVV, 10);
	DDX_Text(pDX, IDC_MERCHANT_CNTRY_ORIGIN, m_StrMCMerchantcntryOrigin);

	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CMerchantGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantGeneral)
	ON_BN_CLICKED(IDC_DEFERRED_MERCHANT, OnDeferredMerchant)
	ON_BN_CLICKED(IDC_HOUSE_CARD_MERCHANT, OnHouseCardMerchant)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantGeneral message handlers

BOOL CMerchantGeneral::OnInitDialog() 
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

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_ADD )
			PopulateGeneralDialogFromDatabase ( sMerchantRecordOld );
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantGeneral::OnSetActive() 
{
	if ( m_nDialogMode == MODE_VIEW ||  m_nDialogMode == MODE_DONE)
	{
		m_CntlMallCodeFld.EnableWindow(FALSE);
		m_CntlDivisionNumberFld.EnableWindow(FALSE);
		m_CntlHCCodeFld.EnableWindow(FALSE);
		m_CntlJCBSubcodeFld.EnableWindow(FALSE);
		m_CntlMCFCCCodeFld.EnableWindow(FALSE);
		m_CntlPOSCodeFld.EnableWindow(FALSE);
		m_CntlRegionNumberFld.EnableWindow(FALSE);
		m_CntlSolicitCodeFld.EnableWindow(FALSE);
		m_CntlVIPCodeFld.EnableWindow(FALSE);
		m_CntlVSTCCCodeFld.EnableWindow(FALSE);
		m_RPSMerchant.EnableWindow(FALSE);
		m_Procurement.EnableWindow(FALSE);
		m_HouseCardMerchant.EnableWindow(FALSE);
		m_DeferredMerchant.EnableWindow(FALSE);
		m_CashBonus.EnableWindow(FALSE);
		m_CntlVoiceCategoryCodeFld.EnableWindow(FALSE);
		m_CntlDefStartDateFld.EnableWindow(FALSE);
		m_CntlDefEndDateFld.EnableWindow(FALSE);
		m_CntlGroupOutletCodeFld.EnableWindow(FALSE);
		m_CntlGroupTypeFld.EnableWindow(FALSE);
		m_cntlReloadMerchant.EnableWindow(FALSE);
		m_CntlVIPMerchant.EnableWindow(FALSE);
		m_CntlOwnMerchant.EnableWindow(FALSE);
		m_CntlFPIFld.EnableWindow(FALSE);
		m_CntlMasterCardAssignedId.EnableWindow(FALSE);
		m_CntlCUPAcceptorID.EnableWindow(FALSE);
		m_CntlVisaMVV.EnableWindow(FALSE);
		m_CntlMCMerchantcntryOrigin.EnableWindow(FALSE);
		m_CntPreauthValidationFlag.EnableWindow(FALSE);


	}
	else if ( m_nDialogMode == MODE_ADD ||  m_nDialogMode == MODE_EDIT)
	{
		m_CntlMallCodeFld.EnableWindow(TRUE);
		m_CntlDivisionNumberFld.EnableWindow(TRUE);
		m_CntlHCCodeFld.EnableWindow(TRUE);
		m_CntlJCBSubcodeFld.EnableWindow(TRUE);
		m_CntlMCFCCCodeFld.EnableWindow(TRUE);
		m_CntlPOSCodeFld.EnableWindow(TRUE);
		m_CntlRegionNumberFld.EnableWindow(TRUE);
		m_CntlSolicitCodeFld.EnableWindow(TRUE);
		m_CntlVIPCodeFld.EnableWindow(TRUE);
		m_CntlVSTCCCodeFld.EnableWindow(TRUE);
		m_RPSMerchant.EnableWindow(TRUE);
		m_Procurement.EnableWindow(TRUE);
		m_HouseCardMerchant.EnableWindow(TRUE);
		m_DeferredMerchant.EnableWindow(TRUE);
		m_CashBonus.EnableWindow(TRUE);
		m_CntlVoiceCategoryCodeFld.EnableWindow(TRUE);
		m_CntlDefStartDateFld.EnableWindow(TRUE);
		m_CntlDefEndDateFld.EnableWindow(TRUE);
		m_CntlGroupOutletCodeFld.EnableWindow(TRUE);
		m_CntlGroupTypeFld.EnableWindow(TRUE);
		m_cntlReloadMerchant.EnableWindow(TRUE);
		m_CntlVIPMerchant.EnableWindow(TRUE);
		m_CntlOwnMerchant.EnableWindow(TRUE);
		m_CntlFPIFld.EnableWindow(TRUE);
		m_CntlMasterCardAssignedId.EnableWindow(TRUE);
		m_CntlCUPAcceptorID.EnableWindow(TRUE);
		m_CntlVisaMVV.EnableWindow(TRUE);
		m_CntlMCMerchantcntryOrigin.EnableWindow(TRUE);
		m_CntPreauthValidationFlag.EnableWindow(TRUE);

	}
	
	return CPropertyPage::OnSetActive();
}

BOOL CMerchantGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CMerchantGeneral::PopulateGeneralDialogFromDatabase ( MCF01 sMerchantRecord )
{
   CString Deferred_start_date;
   CString Deferred_end_date;
   CString tempstr;   

	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );

	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;


	m_StrMallCodeFld = sMerchantRecord.mall_code;
	m_StrDivisionNumberFld = sMerchantRecord.division_nbr;
	m_StrHCCodeFld = sMerchantRecord.hc_code;
	m_StrJCBSubcodeFld = sMerchantRecord.jcb_sub_code;
	m_StrMCFCCCodeFld = sMerchantRecord.mc_fcc_code;
	m_StrPOSCodeFld = sMerchantRecord.pos_code;
	m_StrRegionNumberFld = sMerchantRecord.region_nbr;
	m_StrSolicitCodeFld = sMerchantRecord.solicit_code;
	m_StrVIPCodeFld = sMerchantRecord.vip_code;
	m_StrVSTCCCodeFld = sMerchantRecord.vs_tcc_code;
	m_StrVoiceCategoryCodeFld = sMerchantRecord.voice_category_code;
	m_StrClassCodeFld = sMerchantRecord.class_code;
	m_StrGroupOutletCodeFld = sMerchantRecord.group_outlet_code;
	m_StrGroupTypeFld       = sMerchantRecord.group_type;
	m_StrOwnMerchant		= sMerchantRecord.negative_group_id;
	m_StrFPIFld				= &(sMerchantRecord.process_control_id[1]);
   if ( sMerchantRecord.deferred_start_date[0] != 0x00 )
   {
      tempstr = sMerchantRecord.deferred_start_date;
	   Deferred_start_date = convert_yyyymmdd_to_ddmmyyyy( tempstr );
      m_StrDefStartDateFld = Deferred_start_date;

      tempstr = sMerchantRecord.deferred_end_date;
	   Deferred_end_date = convert_yyyymmdd_to_ddmmyyyy(tempstr);
      m_StrDefEndDateFld = Deferred_end_date;
   }

   if(sMerchantRecord.process_control_id[0] == 'Y')
	   m_CntlVIPMerchant.SetCheck(1);
   else
	   m_CntlVIPMerchant.SetCheck(0);

	if ( strcmp ( (char *)sMerchantRecord.rps_merchant, "Y" ) == 0 )
		m_RPSMerchant.SetCheck(1);
	if ( strcmp ( (char *)sMerchantRecord.procurement_merchant, "Y" ) == 0 )
		m_Procurement.SetCheck(1);

	if ( strcmp ( (char *)sMerchantRecord.house_card_merchant, "Y" ) == 0 )
	{
		m_HouseCardMerchant.SetCheck(1);
		m_BoolHouseCardMerchant = TRUE;
	}
	else
		m_BoolHouseCardMerchant = FALSE;

	if ( strcmp ( (char *)sMerchantRecord.deferred_merchant, "Y" ) == 0 )
	{
		m_DeferredMerchant.SetCheck(1);
		m_BoolDeferredMerchant = TRUE;
	}
	else
		m_BoolDeferredMerchant = FALSE;

	if ( strcmp ( (char *)sMerchantRecord.cash_bonus_eligible, "Y" ) == 0 )
		m_CashBonus.SetCheck(1);

	if( strcmp ( (char *)sMerchantRecord.reload_merchant, "Y" ) == 0 )
		m_cntlReloadMerchant.SetCheck(1);

	if( strncmp ( (char *)sMerchantRecord.network[7].priority_routing, "Y" ,1) == 0 )
		m_CntPreauthValidationFlag.SetCheck(1);


	//MasterCard Assigned data
	m_StrMasterCardAssignedId = sMerchantRecord.network[2].priority_routing;

	// CUP Accptor ID
	//m_StrCUPAcceptorID = sMerchantRecord.network[3].priority_routing;
	m_StrCUPAcceptorID += (CString)((char *)sMerchantRecord.network[3].priority_routing);
	//m_StrCUPAcceptorID += sMerchantRecord.network[4].priority_routing;
	m_StrCUPAcceptorID += (CString)((char *)sMerchantRecord.network[4].priority_routing);

	// VISA MVV 
	m_StrVisaMVV = (CString)((char *)sMerchantRecord.network[8].priority_routing);
	m_StrVisaMVV += (sMerchantRecord.network[9].priority_routing[0]);

	// MC MERCHANT COUNTRY OF ORIGIN
	m_StrMCMerchantcntryOrigin = sMerchantRecord.password;
	UpdateData(FALSE);

	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

void CMerchantGeneral::CleanGeneralDialog()
{
	m_StrMallCodeFld = _T("");
	m_StrDivisionNumberFld = _T("");
	m_StrHCCodeFld = _T("");
	m_StrJCBSubcodeFld = _T("");
	m_StrMCFCCCodeFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrPOSCodeFld = _T("");
	m_StrRegionNumberFld = _T("");
	m_StrVIPCodeFld = _T("");
	m_StrVSTCCCodeFld = _T("");
	m_StrSolicitCodeFld = _T("");
	m_StrVoiceCategoryCodeFld = _T("");
	m_StrClassCodeFld = _T("");
	m_RPSMerchant.SetCheck(0);
	m_Procurement.SetCheck(0);
	m_HouseCardMerchant.SetCheck(0);
	m_DeferredMerchant.SetCheck(0);
	m_CashBonus.SetCheck(0);
	m_cntlReloadMerchant.SetCheck(0);
	m_CntPreauthValidationFlag.SetCheck(0);
	m_StrDefEndDateFld      = _T("");
	m_StrDefStartDateFld    = _T("");
	m_StrGroupOutletCodeFld = _T("");
	m_StrGroupTypeFld       = _T("");
	m_StrOwnMerchant		= _T("");
	m_StrFPIFld				= _T("");
	m_StrMasterCardAssignedId = _T("");
	m_StrCUPAcceptorID        = _T("");
	m_StrVisaMVV			 =_T("");
	m_StrMCMerchantcntryOrigin = _T("");

	m_CntlVIPMerchant.SetCheck(0);
	UpdateData (FALSE);
}

BOOL  CMerchantGeneral::PopulateDatabaseFromGeneralDialog ( pMCF01 psMerchantRecord )
{
   CString Deferred_start_date;
   CString Deferred_end_date;
   int Cup_acp_id_len=0;
   int Visa_MVV_len=0;
   CHAR Visa_mvv[11] = {0};
   
	if ( !m_bGeneral )
   {
      if ( m_nDialogMode == MODE_ADD )
      {
		   memcpy ( psMerchantRecord->rps_merchant, "N", 1 );
		   memcpy ( psMerchantRecord->procurement_merchant, "N", 1 );
		   memcpy ( psMerchantRecord->house_card_merchant, "N", 1 );
		   memcpy ( psMerchantRecord->deferred_merchant, "N", 1 );
		   memcpy ( psMerchantRecord->cash_bonus_eligible, "N", 1 ); 
		   memcpy ( psMerchantRecord->reload_merchant, "N", 1 );
		   memcpy ( psMerchantRecord->network[7].priority_routing, "N", 1 );
      }
      return TRUE;
   }


	memcpy ( psMerchantRecord->mall_code,		m_StrMallCodeFld,		sizeof ( psMerchantRecord->mall_code));
	memcpy ( psMerchantRecord->division_nbr,	m_StrDivisionNumberFld,	sizeof ( psMerchantRecord->division_nbr));
	memcpy ( psMerchantRecord->hc_code,			m_StrHCCodeFld,			sizeof ( psMerchantRecord->hc_code));
	memcpy ( psMerchantRecord->jcb_sub_code,	m_StrJCBSubcodeFld,		sizeof ( psMerchantRecord->jcb_sub_code));
	memcpy ( psMerchantRecord->mc_fcc_code,		m_StrMCFCCCodeFld,		sizeof ( psMerchantRecord->mc_fcc_code));
	memcpy ( psMerchantRecord->pos_code,		m_StrPOSCodeFld,		sizeof ( psMerchantRecord->pos_code));
	memcpy ( psMerchantRecord->region_nbr,		m_StrRegionNumberFld,	sizeof ( psMerchantRecord->region_nbr));
	memcpy ( psMerchantRecord->vip_code,		m_StrVIPCodeFld,		sizeof ( psMerchantRecord->vip_code));
	memcpy ( psMerchantRecord->vs_tcc_code,		m_StrVSTCCCodeFld,		sizeof ( psMerchantRecord->vs_tcc_code));
	memcpy ( psMerchantRecord->solicit_code,	m_StrSolicitCodeFld,	sizeof ( psMerchantRecord->solicit_code));
	memcpy ( psMerchantRecord->voice_category_code,	m_StrVoiceCategoryCodeFld,	sizeof ( psMerchantRecord->voice_category_code));
	memcpy ( psMerchantRecord->group_outlet_code,	m_StrGroupOutletCodeFld,	sizeof ( psMerchantRecord->group_outlet_code));
	memcpy ( psMerchantRecord->group_type,       	m_StrGroupTypeFld,      	sizeof ( psMerchantRecord->group_type));
	memcpy ( psMerchantRecord->negative_group_id,   m_StrOwnMerchant,      	sizeof ( psMerchantRecord->negative_group_id));
	memcpy ( &(psMerchantRecord->process_control_id[1]),   m_StrFPIFld,      	sizeof ( psMerchantRecord->process_control_id) - 1);

	memcpy( psMerchantRecord->network[2].priority_routing, m_StrMasterCardAssignedId, sizeof ( psMerchantRecord->network[2].priority_routing));

	Cup_acp_id_len = m_StrCUPAcceptorID.GetLength();

	if( Cup_acp_id_len > 7)
	{
		memset( psMerchantRecord->network[3].priority_routing, 0, sizeof ( psMerchantRecord->network[3].priority_routing) );
		memset( psMerchantRecord->network[4].priority_routing, 0, sizeof ( psMerchantRecord->network[4].priority_routing) );
		memcpy( psMerchantRecord->network[3].priority_routing, m_StrCUPAcceptorID, 7);
		memcpy( psMerchantRecord->network[4].priority_routing, m_StrCUPAcceptorID.Mid(7), (Cup_acp_id_len -7));
	}
	else
	{
		memset( psMerchantRecord->network[3].priority_routing, 0, sizeof ( psMerchantRecord->network[3].priority_routing) );
		memset( psMerchantRecord->network[4].priority_routing, 0, sizeof ( psMerchantRecord->network[4].priority_routing) );
		memcpy( psMerchantRecord->network[3].priority_routing, m_StrCUPAcceptorID, Cup_acp_id_len);
	}
	
	// Fixed length 10 bytes
	Visa_MVV_len = m_StrVisaMVV.GetLength();


	memset( psMerchantRecord->network[8].priority_routing, 0x00, sizeof ( psMerchantRecord->network[3].priority_routing) );
	memset( psMerchantRecord->network[9].priority_routing, 0x00, sizeof ( psMerchantRecord->network[4].priority_routing) );
	memcpy(Visa_mvv,m_StrVisaMVV,Visa_MVV_len);
	memcpy( psMerchantRecord->network[8].priority_routing, Visa_mvv, 9);
	psMerchantRecord->network[9].priority_routing[0] = Visa_mvv[9]; 
  



	memcpy ( psMerchantRecord->password, m_StrMCMerchantcntryOrigin, sizeof ( psMerchantRecord->password));
	
   if ( m_StrDefStartDateFld.IsEmpty() )
   {
      memset( psMerchantRecord->deferred_start_date, 0x00, sizeof(psMerchantRecord->deferred_start_date));
      memset( psMerchantRecord->deferred_end_date,   0x00, sizeof(psMerchantRecord->deferred_end_date  ));
   }
   else
   {
      Deferred_start_date = convert_ddmmyyyy_to_yyyymmdd( m_StrDefStartDateFld );
      memcpy( psMerchantRecord->deferred_start_date, Deferred_start_date, sizeof(psMerchantRecord->deferred_start_date));

      Deferred_end_date = convert_ddmmyyyy_to_yyyymmdd( m_StrDefEndDateFld );
	   memcpy( psMerchantRecord->deferred_end_date, Deferred_end_date, sizeof(psMerchantRecord->deferred_end_date));
   }

	if ( m_RPSMerchant.GetCheck() == 1 )
		memcpy ( psMerchantRecord->rps_merchant, "Y", 1 );
	else
		memcpy ( psMerchantRecord->rps_merchant, "N", 1 );

	if ( m_CntlVIPMerchant.GetCheck() == 1)
		psMerchantRecord->process_control_id[0] = 'Y';
	else
		psMerchantRecord->process_control_id[0] = 'N';

	if ( m_Procurement.GetCheck() == 1 )
		memcpy ( psMerchantRecord->procurement_merchant, "Y", 1 );
	else
		memcpy ( psMerchantRecord->procurement_merchant, "N", 1 );

	if ( m_HouseCardMerchant.GetCheck() == 1 )
		memcpy ( psMerchantRecord->house_card_merchant, "Y", 1 );
	else
		memcpy ( psMerchantRecord->house_card_merchant, "N", 1 );

	if ( m_DeferredMerchant.GetCheck() == 1 )
		memcpy ( psMerchantRecord->deferred_merchant, "Y", 1 );
	else
		memcpy ( psMerchantRecord->deferred_merchant, "N", 1 );

	if ( m_CashBonus.GetCheck() == 1 )
		memcpy ( psMerchantRecord->cash_bonus_eligible, "Y", 1 );
	else
		memcpy ( psMerchantRecord->cash_bonus_eligible, "N", 1 );

	if ( m_cntlReloadMerchant.GetCheck() == 1 )
		memcpy ( psMerchantRecord->reload_merchant, "Y", 1 );
	else
		memcpy ( psMerchantRecord->reload_merchant, "N", 1 );

	if ( m_CntPreauthValidationFlag.GetCheck() == 1 )
		memcpy( psMerchantRecord->network[7].priority_routing, "Y", 1);
	else
		memcpy( psMerchantRecord->network[7].priority_routing, "N", 1);

	strRPSMerchant = psMerchantRecord->rps_merchant;
	strHouseMerchant = psMerchantRecord->house_card_merchant;
	strProcurementMerchant = psMerchantRecord->procurement_merchant;
	strDeferredMerchant = psMerchantRecord->deferred_merchant;
	strCashEligible = psMerchantRecord->cash_bonus_eligible;
	strReloadMerchant = psMerchantRecord->reload_merchant;
	strPreauthValidationFlag = psMerchantRecord->network[7].priority_routing;
	//strPreauthValidationFlag=(CString)((char *)psMerchantRecord->network[7].priority_routing[0]);
	

return TRUE;

}

BOOL CMerchantGeneral::CompareStructures()
{
   CString Deferred_start_date;
   CString Deferred_end_date;
   CMyUtils myUtilsObject;
   int Cup_acp_id_len = 0;

   if ( m_bGeneral == TRUE )
   {
      if (strcmp ( strRPSMerchant, (char *)sMerchantRecordOld.rps_merchant) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strRPSMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.rps_merchant, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "RPS Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( strHouseMerchant, (char *)sMerchantRecordOld.house_card_merchant) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strHouseMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.house_card_merchant, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "House Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( strProcurementMerchant, (char *)sMerchantRecordOld.procurement_merchant) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strProcurementMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.procurement_merchant, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Procurement Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( strDeferredMerchant, (char *)sMerchantRecordOld.deferred_merchant) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strDeferredMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.deferred_merchant, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Deferred Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( strCashEligible, (char *)sMerchantRecordOld.cash_bonus_eligible) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strCashEligible, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.cash_bonus_eligible, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Cash Eligible", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( strReloadMerchant, (char *)sMerchantRecordOld.reload_merchant) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strReloadMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.reload_merchant, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Reload Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
	  if (strncmp ( strPreauthValidationFlag, (char *)sMerchantRecordOld.network[7].priority_routing,1) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, strPreauthValidationFlag, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.network[7].priority_routing, 1);
         memcpy(m_strFieldTag, "Pre Auth Completion Validation", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrMallCodeFld, (char *)sMerchantRecordOld.mall_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrMallCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.mall_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Mall Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrDivisionNumberFld, (char *)sMerchantRecordOld.division_nbr) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrDivisionNumberFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.division_nbr, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Division Number", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrHCCodeFld, (char *)sMerchantRecordOld.hc_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrHCCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.hc_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "HC Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrJCBSubcodeFld, (char *)sMerchantRecordOld.jcb_sub_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrJCBSubcodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.jcb_sub_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "JCB Sub Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrMCFCCCodeFld, (char *)sMerchantRecordOld.mc_fcc_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrMCFCCCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.mc_fcc_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "MC FCC Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrPOSCodeFld, (char *)sMerchantRecordOld.pos_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrPOSCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.pos_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "POS Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrRegionNumberFld, (char *)sMerchantRecordOld.region_nbr) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrRegionNumberFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.region_nbr, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Region Number", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrFPIFld, (char *)&(sMerchantRecordOld.process_control_id[1])) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrFPIFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, &(sMerchantRecordOld.process_control_id[1]), sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "FPI", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrOwnMerchant, (char *)sMerchantRecordOld.negative_group_id) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrOwnMerchant, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.negative_group_id, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Own Merchant", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrVIPCodeFld, (char *)sMerchantRecordOld.vip_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrVIPCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.vip_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "VIP Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrVoiceCategoryCodeFld, (char *)sMerchantRecordOld.voice_category_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrVoiceCategoryCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.voice_category_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Voice Category Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrHCCodeFld, (char *)sMerchantRecordOld.hc_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrHCCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.hc_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "HC Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrVSTCCCodeFld, (char *)sMerchantRecordOld.vs_tcc_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrVSTCCCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.vs_tcc_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "VS TCC Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
      if (strcmp ( m_StrSolicitCodeFld, (char *)sMerchantRecordOld.solicit_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrSolicitCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.solicit_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Solicit Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if ( (!myUtilsObject.IsEmpty(m_StrDefStartDateFld, this)) ||
           (0 != strcmp((char *)sMerchantRecordOld.deferred_start_date,NULL_STR)) )
      {
         if ( !myUtilsObject.IsEmpty(m_StrDefStartDateFld, this) )
         {
            Deferred_start_date = m_StrDefStartDateFld;
            Deferred_start_date = convert_ddmmyyyy_to_yyyymmdd( Deferred_start_date );
         }
         else
            Deferred_start_date = NULL_STR;

         if (strcmp ( Deferred_start_date, (char *)sMerchantRecordOld.deferred_start_date) != 0)
         {
            memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
            memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
            memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

            memcpy(m_strCurrentData, m_StrDefStartDateFld, sizeof (m_strCurrentData));
            memcpy(m_strPreviousData, sMerchantRecordOld.deferred_start_date, sizeof (m_strPreviousData));
            memcpy(m_strFieldTag, "Deferred Start Date", sizeof (m_strFieldTag));

            PopulateHistoryTable();
	      }
      }

      if ( (!myUtilsObject.IsEmpty(m_StrDefEndDateFld, this)) ||
           (0 != strcmp((char *)sMerchantRecordOld.deferred_end_date,NULL_STR)) )
      {
         if ( !myUtilsObject.IsEmpty(m_StrDefEndDateFld, this) )
         {
            Deferred_end_date = m_StrDefEndDateFld;
            Deferred_end_date = convert_ddmmyyyy_to_yyyymmdd( Deferred_end_date );
         }
         else
            Deferred_end_date = NULL_STR;

         if (strcmp ( Deferred_end_date, (char *)sMerchantRecordOld.deferred_end_date) != 0)
         {
            memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
            memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
            memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

            memcpy(m_strCurrentData, m_StrDefEndDateFld, sizeof (m_strCurrentData));
            memcpy(m_strPreviousData, sMerchantRecordOld.deferred_end_date, sizeof (m_strPreviousData));
            memcpy(m_strFieldTag, "Deferred End Date", sizeof (m_strFieldTag));

            PopulateHistoryTable();
         }
      }
      if (strcmp ( m_StrGroupOutletCodeFld, (char *)sMerchantRecordOld.group_outlet_code) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrGroupOutletCodeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.group_outlet_code, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Group Outlet Code", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrGroupTypeFld, (char *)sMerchantRecordOld.group_type ) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrGroupTypeFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.group_type, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Group Type", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

	  if (strcmp ( m_StrMasterCardAssignedId, (char *)sMerchantRecordOld.network[2].priority_routing) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrMasterCardAssignedId, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.network[2].priority_routing, sizeof (m_strPreviousData));
         strncpy(m_strFieldTag, "MasterCard Assigned id", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

	  CString OldCupAcceptorID;
	  CString OldVisaMVV;
	  CString OldMCMerchantncntryOrigin;
	  OldCupAcceptorID=sMerchantRecordOld.network[3].priority_routing;
	  //OldCupAcceptorID +=sMerchantRecordOld.network[4].priority_routing;
	  OldCupAcceptorID +=(CString)((char *)sMerchantRecordOld.network[4].priority_routing);
	  if (strcmp ( m_StrCUPAcceptorID, OldCupAcceptorID) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
		 Cup_acp_id_len= m_StrCUPAcceptorID.GetLength();
         memcpy(m_strCurrentData, m_StrCUPAcceptorID, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, OldCupAcceptorID, sizeof(m_strPreviousData));
		 strncpy(m_strFieldTag, "CUP Accptor id", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
	  OldVisaMVV   =sMerchantRecordOld.network[8].priority_routing;
	  OldVisaMVV  +=(CString)((char *)sMerchantRecordOld.network[9].priority_routing);
	  if (strcmp ( m_StrVisaMVV, OldVisaMVV) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
		 //Visa_MVV_len= m_StrVisaMVV.GetLength();
         memcpy(m_strCurrentData, m_StrVisaMVV, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, OldVisaMVV, sizeof(m_strPreviousData));
		 strncpy(m_strFieldTag, "VISA MVV", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

	  OldMCMerchantncntryOrigin =sMerchantRecordOld.password;
	  if (strcmp ( m_StrMCMerchantcntryOrigin, OldMCMerchantncntryOrigin) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrMCMerchantcntryOrigin, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, OldMCMerchantncntryOrigin, sizeof(m_strPreviousData));
		 strncpy(m_strFieldTag, "Merchant Country of Origin", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
   }
	return TRUE;
}

void CMerchantGeneral::PopulateHistoryTable()
{
   HST01 sHistoryData;
   CHAR dest_str[18];   
   CMyUtils myUtilsObject;
   char strErrorMsg[200] = "";
   char strDate[9]       = "";
   char strTime[7]       = "";
   char strPrimaryKey[50]= "";
   CHAR s_merch_ctr[4]   = "";
   CHAR strDisplayErrMsg[250];
   CString temp;


   strcpy( strPrimaryKey, MerchantID     );
   strcat( strPrimaryKey, OrganizationID );

   myUtilsObject.GetSystemDate(strDate);  
   myUtilsObject.GetSystemTime(strTime);

   /* Get unique primary key for history table.
    * Use a static counter, instead of milliseconds,
    * to prevent unique constraint violations.
    */
   pintime_get_gmt ( dest_str );
   merch_ctr++;
   if ( merch_ctr > 999 )
      merch_ctr = 0;
   sprintf( s_merch_ctr, "%03d", merch_ctr );
   memcpy( &dest_str[14], s_merch_ctr, 3 );

   ZeroMemory ( &sHistoryData, sizeof (HST01));
   memcpy ((LPTSTR)sHistoryData.file_name,
           (LPCTSTR)"MCF01",
           sizeof (sHistoryData.file_name));

   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

   CHAR wstation[21]; DWORD wsize;

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

   memcpy ((LPTSTR)sHistoryData.reserved,             (LPTSTR)wstation,               sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.date_modified,       temp,                           sizeof (sHistoryData.date_modified));
   memcpy ((LPTSTR)sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR)sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));              
   memcpy ((LPTSTR)sHistoryData.current_data,         (LPCTSTR)m_strCurrentData,      sizeof (sHistoryData.current_data));
   memcpy ((LPTSTR)sHistoryData.previous_data,        (LPCTSTR)m_strPreviousData,     sizeof (sHistoryData.previous_data));
   memcpy ((LPTSTR)sHistoryData.field_tag,            (LPCTSTR)m_strFieldTag,         sizeof (sHistoryData.field_tag));
   memcpy ((LPTSTR)sHistoryData.changed_by,           (LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

   int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
   if ( rcode != PTEMSG_OK )
   {
      m_nDialogMode = MODE_DONE;
      memset( strDisplayErrMsg, 0x00, sizeof(strDisplayErrMsg) );
      sprintf( strDisplayErrMsg,
              "Unable to update History Table HST01 - %s",
               strErrorMsg );
      AfxMessageBox( strDisplayErrMsg, MB_OK );
   }
   return;
}

void CMerchantGeneral::OnDeferredMerchant() 
{
   UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( m_BoolHouseCardMerchant )
   {
      m_StrMessageLbl = "Merchant cannot be both a Deferred and a House Card";
      UpdateData(FALSE);
      m_DeferredMerchant.SetCheck(0);
   }
}

void CMerchantGeneral::OnHouseCardMerchant() 
{
   UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( m_BoolDeferredMerchant )
   {
      m_StrMessageLbl = "Merchant cannot be both a Deferred and a House Card";
      UpdateData(FALSE);
      m_HouseCardMerchant.SetCheck(0);
   }
	
}


CString CMerchantGeneral::convert_ddmmyyyy_to_yyyymmdd( CString input_date )
{
   char output_date[11]="";
   char instring[11]="";
   CString outstring;

   /* Input  date is format DD/MM/YYYY.
    * Output date is format YYYYMMDD.
    */
   memcpy( instring, input_date, sizeof(instring) );
   memcpy( output_date,   instring+6, 4 );  /* Year  */
   memcpy( output_date+4, instring+3, 2 );  /* Month */
   memcpy( output_date+6, instring,   2 );  /* Day   */

   outstring = output_date;
   return( outstring );
}

CString CMerchantGeneral::convert_yyyymmdd_to_ddmmyyyy( CString input_date )
{
   char output_date[11]="";
   char instring[11]="";
   CString outstring;

   /* Input  date is format YYYYMMDD.
    * Output date is format DD/MM/YYYY.
    */
   memcpy( instring, input_date, sizeof(instring) );
   memcpy( output_date,   instring+6, 2 );  /* Day   */
   strcat( output_date, "/" );
   memcpy( output_date+3, instring+4, 2 );  /* Month */
   strcat( output_date, "/" );
   memcpy( output_date+6, instring,   4 );  /* Year  */

   outstring = output_date;
   return( outstring );
}
