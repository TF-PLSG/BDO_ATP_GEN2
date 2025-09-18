// cardsprofile.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "cardsprofile.h"
#include "MyUtils.h"

#include "SharedMem.h"

extern CSharedMem shrmem;

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

/////////////////////////////////////////////////////////////////////////////
// CCardsProfile dialog

IMPLEMENT_DYNCREATE(CCardsProfile, CPropertyPage)

CCardsProfile::CCardsProfile(CWnd* pParent /*=NULL*/)
: CPropertyPage(CCardsProfile::IDD)
{
	//{{AFX_DATA_INIT(CCardsProfile)
	m_StrCCLiCVC = _T("");
	m_StrCCLPanSeq = _T("");
	m_StrCCLServiceCode = _T("");
	m_StrCCTiCVC = _T("");
	m_StrCCTPanSeq = _T("");
	m_StrCCTServiceCode = _T("");
	m_StrMSCatc = _T("");
	m_StrMSCiCVC = _T("");
	m_StrMSCPanSeq = _T("");
	m_StrMSCServiceCode = _T("");
	m_StrMSCUnPredictableNo = _T("");
	m_StrCCLiCVC2 = _T("");
	m_StrCCLPanSeq2 = _T("");
	m_StrCCLServiceCode2 = _T("");
	m_StrCCTiCVC2 = _T("");
	m_StrCCTPanSeq2 = _T("");
	m_StrCCTServiceCode2 = _T("");
	m_StrMSCatc2 = _T("");
	m_StrMSCiCVC2 = _T("");
	m_StrMSCPanSeq2 = _T("");
	m_StrMSCServiceCode2 = _T("");
	m_StrMSCUnPredictableNo2 = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrCardType_Fld = _T("");
	//}}AFX_DATA_INIT
	memset(&sProfileIdOld, 0, sizeof(sProfileIdOld));
}


void CCardsProfile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsProfile)
	DDX_Control(pDX, IDC_CCL_ICVC, m_CntlCCLiCVC);
	DDX_Control(pDX, IDC_CCL_PAN_SEQ, m_CntlCCLPanSeq);
	DDX_Control(pDX, IDC_CCL_SERVICE_CODE, m_CntlCCLServiceCode);
	DDX_Control(pDX, IDC_CCT_ICVC, m_CntlCCTiCVC);
	DDX_Control(pDX, IDC_CCT_PAN_SEQ, m_CntlCCTPanSeq);
	DDX_Control(pDX, IDC_CCT_SERVICE_CODE, m_CntlCCTServiceCode);
	DDX_Control(pDX, IDC_MSC_ATC, m_CntlMSCatc);
	DDX_Control(pDX, IDC_MSC_ICVC, m_CntlMSCiCVC);
	DDX_Control(pDX, IDC_MSC_PAN_SEQ, m_CntlMSCPanSeq);
	DDX_Control(pDX, IDC_MSC_SERVICE_CODE, m_CntlMSCServiceCode);
	DDX_Control(pDX, IDC_MSC_UNPREDICTABLE_NO, m_CntlMSCUnPredictableNo);
	DDX_Text(pDX, IDC_CCL_ICVC, m_StrCCLiCVC);
	DDV_MaxChars(pDX, m_StrCCLiCVC, 2);
	DDX_Text(pDX, IDC_CCL_PAN_SEQ, m_StrCCLPanSeq);
	DDV_MaxChars(pDX, m_StrCCLPanSeq, 2);
	DDX_Text(pDX, IDC_CCL_SERVICE_CODE, m_StrCCLServiceCode);
	DDV_MaxChars(pDX, m_StrCCLServiceCode, 2);
	DDX_Text(pDX, IDC_CCT_ICVC, m_StrCCTiCVC);
	DDV_MaxChars(pDX, m_StrCCTiCVC, 2);
	DDX_Text(pDX, IDC_CCT_PAN_SEQ, m_StrCCTPanSeq);
	DDV_MaxChars(pDX, m_StrCCTPanSeq, 2);
	DDX_Text(pDX, IDC_CCT_SERVICE_CODE, m_StrCCTServiceCode);
	DDV_MaxChars(pDX, m_StrCCTServiceCode, 2);
	DDX_Text(pDX, IDC_MSC_ATC, m_StrMSCatc);
	DDV_MaxChars(pDX, m_StrMSCatc, 2);
	DDX_Text(pDX, IDC_MSC_ICVC, m_StrMSCiCVC);
	DDV_MaxChars(pDX, m_StrMSCiCVC, 2);
	DDX_Text(pDX, IDC_MSC_PAN_SEQ, m_StrMSCPanSeq);
	DDV_MaxChars(pDX, m_StrMSCPanSeq, 2);
	DDX_Text(pDX, IDC_MSC_SERVICE_CODE, m_StrMSCServiceCode);
	DDV_MaxChars(pDX, m_StrMSCServiceCode, 2);
	DDX_Text(pDX, IDC_MSC_UNPREDICTABLE_NO, m_StrMSCUnPredictableNo);
	DDV_MaxChars(pDX, m_StrMSCUnPredictableNo, 2);
	DDX_Control(pDX, IDC_CCL_ICVC2, m_CntlCCLiCVC2);
	DDX_Control(pDX, IDC_CCL_PAN_SEQ2, m_CntlCCLPanSeq2);
	DDX_Control(pDX, IDC_CCL_SERVICE_CODE2, m_CntlCCLServiceCode2);
	DDX_Control(pDX, IDC_CCT_ICVC2, m_CntlCCTiCVC2);
	DDX_Control(pDX, IDC_CCT_PAN_SEQ2, m_CntlCCTPanSeq2);
	DDX_Control(pDX, IDC_CCT_SERVICE_CODE2, m_CntlCCTServiceCode2);
	DDX_Control(pDX, IDC_MSC_ATC2, m_CntlMSCatc2);
	DDX_Control(pDX, IDC_MSC_ICVC2, m_CntlMSCiCVC2);
	DDX_Control(pDX, IDC_MSC_PAN_SEQ2, m_CntlMSCPanSeq2);
	DDX_Control(pDX, IDC_MSC_SERVICE_CODE2, m_CntlMSCServiceCode2);
	DDX_Control(pDX, IDC_MSC_UNPREDICTABLE_NO2, m_CntlMSCUnPredictableNo2);
	DDX_Text(pDX, IDC_CCL_ICVC2, m_StrCCLiCVC2);
	DDV_MaxChars(pDX, m_StrCCLiCVC2, 2);
	DDX_Text(pDX, IDC_CCL_PAN_SEQ2, m_StrCCLPanSeq2);
	DDV_MaxChars(pDX, m_StrCCLPanSeq2, 2);
	DDX_Text(pDX, IDC_CCL_SERVICE_CODE2, m_StrCCLServiceCode2);
	DDV_MaxChars(pDX, m_StrCCLServiceCode2, 2);
	DDX_Text(pDX, IDC_CCT_ICVC2, m_StrCCTiCVC2);
	DDV_MaxChars(pDX, m_StrCCTiCVC2, 2);
	DDX_Text(pDX, IDC_CCT_PAN_SEQ2, m_StrCCTPanSeq2);
	DDV_MaxChars(pDX, m_StrCCTPanSeq2, 2);
	DDX_Text(pDX, IDC_CCT_SERVICE_CODE2, m_StrCCTServiceCode2);
	DDV_MaxChars(pDX, m_StrCCTServiceCode2, 2);
	DDX_Text(pDX, IDC_MSC_ATC2, m_StrMSCatc2);
	DDV_MaxChars(pDX, m_StrMSCatc2, 2);
	DDX_Text(pDX, IDC_MSC_ICVC2, m_StrMSCiCVC2);
	DDV_MaxChars(pDX, m_StrMSCiCVC2, 2);
	DDX_Text(pDX, IDC_MSC_PAN_SEQ2, m_StrMSCPanSeq2);
	DDV_MaxChars(pDX, m_StrMSCPanSeq2, 2);
	DDX_Text(pDX, IDC_MSC_SERVICE_CODE2, m_StrMSCServiceCode2);
	DDV_MaxChars(pDX, m_StrMSCServiceCode2, 2);
	DDX_Text(pDX, IDC_MSC_UNPREDICTABLE_NO2, m_StrMSCUnPredictableNo2);
	DDV_MaxChars(pDX, m_StrMSCUnPredictableNo2, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsProfile, CPropertyPage)
//{{AFX_MSG_MAP(CCardsProfile)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsProfile message handlers

BOOL CCardsProfile::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_bProcessing = TRUE;
	// TODO: Add extra initialization here
	if (m_nDialogMode != MODE_DONE )
	{
		if (m_nDialogMode != MODE_VIEW)
			EnableCardsProfileDialog(TRUE);
		else
			EnableCardsProfileDialog(FALSE);
		
		if(m_nDialogMode != MODE_ADD)
		{
			PopulateCardsProfileDialogFromDatabase (sProfileIdOld);
		}
	}    
	
	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCardsProfile::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CCardsProfile::EnableCardsProfileDialog(int nStatus)
{
	m_CntlCCLiCVC.EnableWindow(nStatus);
	m_CntlCCLPanSeq.EnableWindow(nStatus);
	m_CntlCCLServiceCode.EnableWindow(nStatus);
	m_CntlCCTiCVC.EnableWindow(nStatus);
	m_CntlCCTPanSeq.EnableWindow(nStatus);
	m_CntlCCTServiceCode.EnableWindow(nStatus);
	m_CntlMSCatc.EnableWindow(nStatus);
	m_CntlMSCiCVC.EnableWindow(nStatus);
	m_CntlMSCPanSeq.EnableWindow(nStatus);
	m_CntlMSCServiceCode.EnableWindow(nStatus);
	m_CntlMSCUnPredictableNo.EnableWindow(nStatus);

	m_CntlCCLiCVC2.EnableWindow(nStatus);
	m_CntlCCLPanSeq2.EnableWindow(nStatus);
	m_CntlCCLServiceCode2.EnableWindow(nStatus);
	m_CntlCCTiCVC2.EnableWindow(nStatus);
	m_CntlCCTPanSeq2.EnableWindow(nStatus);
	m_CntlCCTServiceCode2.EnableWindow(nStatus);
	m_CntlMSCatc2.EnableWindow(nStatus);
	m_CntlMSCiCVC2.EnableWindow(nStatus);
	m_CntlMSCPanSeq2.EnableWindow(nStatus);
	m_CntlMSCServiceCode2.EnableWindow(nStatus);
	m_CntlMSCUnPredictableNo2.EnableWindow(nStatus);

}

void CCardsProfile::CleanCardsProfileDialog()
{
}

void CCardsProfile::PopulateHistoryTable()
{
}

BOOL CCardsProfile::PopulateCardsProfileDialogFromDatabase(CAF01_EMV sProfileId)
{
/*	m_StrCCLiCVC			=sProfileId.ccless_tk[0].icvc_offset;
	m_StrCCLPanSeq			=sProfileId.ccless_tk[0].pan_seq_nbr_offset;
	m_StrCCLServiceCode		=sProfileId.ccless_tk[0].service_code_offset;
	
	m_StrCCTiCVC			=sProfileId.cc_tk[0].icvc_offset;
	m_StrCCTPanSeq			=sProfileId.cc_tk[0].pan_seq_nbr_offset;
	m_StrCCTServiceCode		=sProfileId.cc_tk[0].service_code_offset;
	
	m_StrMSCatc				=sProfileId.mcless_tk[0].atc_offset;
	m_StrMSCiCVC			=sProfileId.mcless_tk[0].icvc_offset;
	m_StrMSCPanSeq			=sProfileId.mcless_tk[0].pan_seq_nbr_offset;
	m_StrMSCServiceCode		=sProfileId.mcless_tk[0].service_code_offset;
	m_StrMSCUnPredictableNo	=sProfileId.mcless_tk[0].unpre_nbr_offset;
	
	
	m_StrCCLiCVC2           =sProfileId.ccless_tk[1].icvc_offset;
	m_StrCCLPanSeq2         =sProfileId.ccless_tk[1].pan_seq_nbr_offset;
	m_StrCCLServiceCode2    =sProfileId.ccless_tk[1].service_code_offset;
	
	m_StrCCTiCVC2           =sProfileId.cc_tk[1].icvc_offset;
	m_StrCCTPanSeq2         =sProfileId.cc_tk[1].pan_seq_nbr_offset;
	m_StrCCTServiceCode2    =sProfileId.cc_tk[1].service_code_offset;
	
	m_StrMSCatc2            =sProfileId.mcless_tk[1].atc_offset;
	m_StrMSCiCVC2           =sProfileId.mcless_tk[1].icvc_offset;
	m_StrMSCPanSeq2         =sProfileId.mcless_tk[1].pan_seq_nbr_offset;
	m_StrMSCServiceCode2    =sProfileId.mcless_tk[1].service_code_offset;
	m_StrMSCUnPredictableNo2=sProfileId.mcless_tk[1].unpre_nbr_offset;
	
	UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));
*/
	return TRUE;
}

BOOL CCardsProfile::PopulateDatabaseFromCardsProfile( pCAF01_EMV psProfileId )
{
	if(!m_bProcessing)
		return TRUE;

	
	memcpy(psProfileId->ccless_tk[0].icvc_offset			,m_StrCCLiCVC,				sizeof(psProfileId->ccless_tk[0].icvc_offset));
	memcpy(psProfileId->ccless_tk[0].pan_seq_nbr_offset		,m_StrCCLPanSeq,			sizeof(psProfileId->ccless_tk[0].pan_seq_nbr_offset));
	memcpy(psProfileId->ccless_tk[0].service_code_offset	,m_StrCCLServiceCode,		sizeof(psProfileId->ccless_tk[0].service_code_offset));
	
	memcpy(psProfileId->cc_tk[0].icvc_offset				,m_StrCCTiCVC,				sizeof(psProfileId->cc_tk[0].icvc_offset));
	memcpy(psProfileId->cc_tk[0].pan_seq_nbr_offset			,m_StrCCTPanSeq,			sizeof(psProfileId->cc_tk[0].pan_seq_nbr_offset));
	memcpy(psProfileId->cc_tk[0].service_code_offset		,m_StrCCTServiceCode,		sizeof(psProfileId->cc_tk[0].service_code_offset));
	
	memcpy(psProfileId->mcless_tk[0].atc_offset				,m_StrMSCatc,				sizeof(psProfileId->mcless_tk[0].atc_offset));
	memcpy(psProfileId->mcless_tk[0].icvc_offset			,m_StrMSCiCVC,				sizeof(psProfileId->mcless_tk[0].icvc_offset));
	memcpy(psProfileId->mcless_tk[0].pan_seq_nbr_offset		,m_StrMSCPanSeq,			sizeof(psProfileId->mcless_tk[0].pan_seq_nbr_offset));
	memcpy(psProfileId->mcless_tk[0].service_code_offset	,m_StrMSCServiceCode,		sizeof(psProfileId->mcless_tk[0].service_code_offset));
	memcpy(psProfileId->mcless_tk[0].unpre_nbr_offset		,m_StrMSCUnPredictableNo,	sizeof(psProfileId->mcless_tk[0].unpre_nbr_offset));
	
	
	memcpy(psProfileId->ccless_tk[1].icvc_offset			,m_StrCCLiCVC2,				sizeof(psProfileId->ccless_tk[1].icvc_offset));
	memcpy(psProfileId->ccless_tk[1].pan_seq_nbr_offset		,m_StrCCLPanSeq2,			sizeof(psProfileId->ccless_tk[1].pan_seq_nbr_offset));
	memcpy(psProfileId->ccless_tk[1].service_code_offset	,m_StrCCLServiceCode2,		sizeof(psProfileId->ccless_tk[1].service_code_offset));
	
	memcpy(psProfileId->cc_tk[1].icvc_offset				,m_StrCCTiCVC2,				sizeof(psProfileId->cc_tk[1].icvc_offset));
	memcpy(psProfileId->cc_tk[1].pan_seq_nbr_offset			,m_StrCCTPanSeq2,			sizeof(psProfileId->cc_tk[1].pan_seq_nbr_offset));
	memcpy(psProfileId->cc_tk[1].service_code_offset		,m_StrCCTServiceCode2,		sizeof(psProfileId->cc_tk[1].service_code_offset));
	
	memcpy(psProfileId->mcless_tk[1].atc_offset				,m_StrMSCatc2,				sizeof(psProfileId->mcless_tk[1].atc_offset));
	memcpy(psProfileId->mcless_tk[1].icvc_offset			,m_StrMSCiCVC2,				sizeof(psProfileId->mcless_tk[1].icvc_offset));
	memcpy(psProfileId->mcless_tk[1].pan_seq_nbr_offset		,m_StrMSCPanSeq2,			sizeof(psProfileId->mcless_tk[1].pan_seq_nbr_offset));
	memcpy(psProfileId->mcless_tk[1].service_code_offset	,m_StrMSCServiceCode2,		sizeof(psProfileId->mcless_tk[1].service_code_offset));
	memcpy(psProfileId->mcless_tk[1].unpre_nbr_offset		,m_StrMSCUnPredictableNo2,	sizeof(psProfileId->mcless_tk[1].unpre_nbr_offset));
	return TRUE;
}
