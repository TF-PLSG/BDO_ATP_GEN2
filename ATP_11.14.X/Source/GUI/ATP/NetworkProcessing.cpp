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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkProcessing.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 13 2000 17:07:46   mbysani
   bug fixed
   
      Rev 1.1   Sep 22 1999 15:57:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:10   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:48   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:24   iarustam
    
*
************************************************************************************/
// NetworkProcessing.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkProcessing.h"
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
// CNetworkProcessing property page

IMPLEMENT_DYNCREATE(CNetworkProcessing, CPropertyPage)

CNetworkProcessing::CNetworkProcessing() : CPropertyPage(CNetworkProcessing::IDD)
{
	//{{AFX_DATA_INIT(CNetworkProcessing)
	m_StrCode1Fld = _T("");
	m_StrCode10Fld = _T("");
	m_StrCode11Fld = _T("");
	m_StrCode12Fld = _T("");
	m_StrCode13Fld = _T("");
	m_StrCode14Fld = _T("");
	m_StrCode15Fld = _T("");
	m_StrCode16Fld = _T("");
	m_StrCode17Fld = _T("");
	m_StrCode18Fld = _T("");
	m_StrCode19Fld = _T("");
	m_StrCode2Fld = _T("");
	m_StrCode20Fld = _T("");
	m_StrCode3Fld = _T("");
	m_StrCode4Fld = _T("");
	m_StrCode5Fld = _T("");
	m_StrCode6Fld = _T("");
	m_StrCode7Fld = _T("");
	m_StrCode8Fld = _T("");
	m_StrCode9Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CNetworkProcessing::~CNetworkProcessing()
{
}

void CNetworkProcessing::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkProcessing)
	DDX_Control(pDX, IDC_CODE9_FLD, m_CntlCode9Fld);
	DDX_Control(pDX, IDC_CODE8_FLD, m_CntlCode8Fld);
	DDX_Control(pDX, IDC_CODE7_FLD, m_CntlCode7Fld);
	DDX_Control(pDX, IDC_CODE6_FLD, m_CntlCode6Fld);
	DDX_Control(pDX, IDC_CODE5_FLD, m_CntlCode5Fld);
	DDX_Control(pDX, IDC_CODE4_FLD, m_CntlCode4Fld);
	DDX_Control(pDX, IDC_CODE3_FLD, m_CntlCode3Fld);
	DDX_Control(pDX, IDC_CODE20_FLD, m_CntlCode20Fld);
	DDX_Control(pDX, IDC_CODE2_FLD, m_CntlCode2Fld);
	DDX_Control(pDX, IDC_CODE19_FLD, m_CntlCode19Fld);
	DDX_Control(pDX, IDC_CODE18_FLD, m_CntlCode18Fld);
	DDX_Control(pDX, IDC_CODE17_FLD, m_CntlCode17Fld);
	DDX_Control(pDX, IDC_CODE16_FLD, m_CntlCode16Fld);
	DDX_Control(pDX, IDC_CODE15_FLD, m_CntlCode15Fld);
	DDX_Control(pDX, IDC_CODE14_FLD, m_CntlCode14Fld);
	DDX_Control(pDX, IDC_CODE13_FLD, m_CntlCode13Fld);
	DDX_Control(pDX, IDC_CODE12_FLD, m_CntlCode12Fld);
	DDX_Control(pDX, IDC_CODE11_FLD, m_CntlCode11Fld);
	DDX_Control(pDX, IDC_CODE10_FLD, m_CntlCode10Fld);
	DDX_Control(pDX, IDC_CODE1_FLD, m_CntlCode1Fld);
	DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Control(pDX, IDC_NETWORK_ID_FLD, m_CntlNetworkIdFld);
	DDX_Text(pDX, IDC_CODE1_FLD, m_StrCode1Fld);
	DDV_MaxChars(pDX, m_StrCode1Fld, 6);
	DDX_Text(pDX, IDC_CODE10_FLD, m_StrCode10Fld);
	DDV_MaxChars(pDX, m_StrCode10Fld, 6);
	DDX_Text(pDX, IDC_CODE11_FLD, m_StrCode11Fld);
	DDV_MaxChars(pDX, m_StrCode11Fld, 6);
	DDX_Text(pDX, IDC_CODE12_FLD, m_StrCode12Fld);
	DDV_MaxChars(pDX, m_StrCode12Fld, 6);
	DDX_Text(pDX, IDC_CODE13_FLD, m_StrCode13Fld);
	DDV_MaxChars(pDX, m_StrCode13Fld, 6);
	DDX_Text(pDX, IDC_CODE14_FLD, m_StrCode14Fld);
	DDV_MaxChars(pDX, m_StrCode14Fld, 6);
	DDX_Text(pDX, IDC_CODE15_FLD, m_StrCode15Fld);
	DDV_MaxChars(pDX, m_StrCode15Fld, 6);
	DDX_Text(pDX, IDC_CODE16_FLD, m_StrCode16Fld);
	DDV_MaxChars(pDX, m_StrCode16Fld, 6);
	DDX_Text(pDX, IDC_CODE17_FLD, m_StrCode17Fld);
	DDV_MaxChars(pDX, m_StrCode17Fld, 6);
	DDX_Text(pDX, IDC_CODE18_FLD, m_StrCode18Fld);
	DDV_MaxChars(pDX, m_StrCode18Fld, 6);
	DDX_Text(pDX, IDC_CODE19_FLD, m_StrCode19Fld);
	DDV_MaxChars(pDX, m_StrCode19Fld, 6);
	DDX_Text(pDX, IDC_CODE2_FLD, m_StrCode2Fld);
	DDV_MaxChars(pDX, m_StrCode2Fld, 6);
	DDX_Text(pDX, IDC_CODE20_FLD, m_StrCode20Fld);
	DDV_MaxChars(pDX, m_StrCode20Fld, 6);
	DDX_Text(pDX, IDC_CODE3_FLD, m_StrCode3Fld);
	DDV_MaxChars(pDX, m_StrCode3Fld, 6);
	DDX_Text(pDX, IDC_CODE4_FLD, m_StrCode4Fld);
	DDV_MaxChars(pDX, m_StrCode4Fld, 6);
	DDX_Text(pDX, IDC_CODE5_FLD, m_StrCode5Fld);
	DDV_MaxChars(pDX, m_StrCode5Fld, 6);
	DDX_Text(pDX, IDC_CODE6_FLD, m_StrCode6Fld);
	DDV_MaxChars(pDX, m_StrCode6Fld, 6);
	DDX_Text(pDX, IDC_CODE7_FLD, m_StrCode7Fld);
	DDV_MaxChars(pDX, m_StrCode7Fld, 6);
	DDX_Text(pDX, IDC_CODE8_FLD, m_StrCode8Fld);
	DDV_MaxChars(pDX, m_StrCode8Fld, 6);
	DDX_Text(pDX, IDC_CODE9_FLD, m_StrCode9Fld);
	DDV_MaxChars(pDX, m_StrCode9Fld, 6);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NETWORK_ID_FLD, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkProcessing, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkProcessing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkProcessing message handlers
/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkProcessing::OnInitDialog() 
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
        EnableProcessingDialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
            PopulateProcessingDialogFromDatabase (sNetworkIdOld);
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

BOOL CNetworkProcessing::OnCommand(WPARAM wParam, LPARAM lParam) 
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
    
BOOL CNetworkProcessing::PopulateDatabaseFromProcessingDialog( pNCF01 psNetworkId )
{	
	memcpy ( psNetworkId->allowed[0].processing_code,    m_StrCode1Fld,         sizeof ( psNetworkId->allowed[0].processing_code));
	memcpy ( psNetworkId->allowed[1].processing_code,    m_StrCode2Fld,         sizeof ( psNetworkId->allowed[1].processing_code));
	memcpy ( psNetworkId->allowed[2].processing_code,    m_StrCode3Fld,         sizeof ( psNetworkId->allowed[2].processing_code));
	memcpy ( psNetworkId->allowed[3].processing_code,    m_StrCode4Fld,         sizeof ( psNetworkId->allowed[3].processing_code));
	memcpy ( psNetworkId->allowed[4].processing_code,    m_StrCode5Fld,         sizeof ( psNetworkId->allowed[4].processing_code));
	memcpy ( psNetworkId->allowed[5].processing_code,    m_StrCode6Fld,         sizeof ( psNetworkId->allowed[5].processing_code));
	memcpy ( psNetworkId->allowed[6].processing_code,    m_StrCode7Fld,         sizeof ( psNetworkId->allowed[6].processing_code));
	memcpy ( psNetworkId->allowed[7].processing_code,    m_StrCode8Fld,         sizeof ( psNetworkId->allowed[7].processing_code));
	memcpy ( psNetworkId->allowed[8].processing_code,    m_StrCode9Fld,         sizeof ( psNetworkId->allowed[8].processing_code));
	memcpy ( psNetworkId->allowed[9].processing_code,    m_StrCode10Fld,         sizeof ( psNetworkId->allowed[9].processing_code));
	memcpy ( psNetworkId->allowed[10].processing_code,   m_StrCode11Fld,         sizeof ( psNetworkId->allowed[10].processing_code));
	memcpy ( psNetworkId->allowed[11].processing_code,   m_StrCode12Fld,         sizeof ( psNetworkId->allowed[11].processing_code));
	memcpy ( psNetworkId->allowed[12].processing_code,   m_StrCode13Fld,         sizeof ( psNetworkId->allowed[12].processing_code));
	memcpy ( psNetworkId->allowed[13].processing_code,   m_StrCode14Fld,         sizeof ( psNetworkId->allowed[13].processing_code));
	memcpy ( psNetworkId->allowed[14].processing_code,   m_StrCode15Fld,         sizeof ( psNetworkId->allowed[14].processing_code));
	memcpy ( psNetworkId->allowed[15].processing_code,   m_StrCode16Fld,         sizeof ( psNetworkId->allowed[15].processing_code));
	memcpy ( psNetworkId->allowed[16].processing_code,   m_StrCode17Fld,         sizeof ( psNetworkId->allowed[16].processing_code));
	memcpy ( psNetworkId->allowed[17].processing_code,   m_StrCode18Fld,         sizeof ( psNetworkId->allowed[17].processing_code));
	memcpy ( psNetworkId->allowed[18].processing_code,   m_StrCode19Fld,         sizeof ( psNetworkId->allowed[18].processing_code));
	//memcpy ( psNetworkId->allowed[19].processing_code,   m_StrCode20Fld,         sizeof ( psNetworkId->allowed[19].processing_code));
	
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkProcessing::PopulateProcessingDialogFromDatabase( NCF01 sNetworkId )
{	 
    if ( m_nDialogMode != MODE_VIEW )
        EnableProcessingDialog(TRUE);
  
    ZeroMemory ( &sNetworkIdOld, sizeof (NCF01));  

	m_StrNetworkIdFld = sNetworkId.primary_key.network_id;
	m_StrNetworkTypeFld = sNetworkId.primary_key.network_type;

	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";

 
    m_StrCode1Fld              = sNetworkId.allowed[0].processing_code;
	m_StrCode2Fld              = sNetworkId.allowed[1].processing_code;
	m_StrCode3Fld              = sNetworkId.allowed[2].processing_code;
	m_StrCode4Fld              = sNetworkId.allowed[3].processing_code;
	m_StrCode5Fld              = sNetworkId.allowed[4].processing_code;
	m_StrCode6Fld              = sNetworkId.allowed[5].processing_code;
	m_StrCode7Fld              = sNetworkId.allowed[6].processing_code;
	m_StrCode8Fld              = sNetworkId.allowed[7].processing_code;
	m_StrCode9Fld              = sNetworkId.allowed[8].processing_code;
	m_StrCode10Fld             = sNetworkId.allowed[9].processing_code;
	m_StrCode11Fld             = sNetworkId.allowed[10].processing_code;
	m_StrCode12Fld             = sNetworkId.allowed[11].processing_code;
	m_StrCode13Fld             = sNetworkId.allowed[12].processing_code;
	m_StrCode14Fld             = sNetworkId.allowed[13].processing_code;
	m_StrCode15Fld             = sNetworkId.allowed[14].processing_code;
	m_StrCode16Fld             = sNetworkId.allowed[15].processing_code;
	m_StrCode17Fld             = sNetworkId.allowed[16].processing_code;
	m_StrCode18Fld             = sNetworkId.allowed[17].processing_code;
	m_StrCode19Fld             = sNetworkId.allowed[18].processing_code;
	//m_StrCode20Fld             = sNetworkId.allowed[19].processing_code;
	         
     
    UpdateData(FALSE);

    memcpy ( &sNetworkIdOld, &sNetworkId, sizeof ( NCF01));
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
							
BOOL CNetworkProcessing::CompareStructures()
{
    if (strcmp ( m_StrCode1Fld, (char *)sNetworkIdOld.allowed[0].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode1Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[0].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode2Fld, (char *)sNetworkIdOld.allowed[1].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode2Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[1].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode3Fld, (char *)sNetworkIdOld.allowed[2].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode3Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[2].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode4Fld, (char *)sNetworkIdOld.allowed[3].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode4Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[3].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode5Fld, (char *)sNetworkIdOld.allowed[4].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode5Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[4].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode6Fld, (char *)sNetworkIdOld.allowed[5].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode6Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[5].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode7Fld, (char *)sNetworkIdOld.allowed[6].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode7Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[6].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode8Fld, (char *)sNetworkIdOld.allowed[7].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode8Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[7].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode9Fld, (char *)sNetworkIdOld.allowed[8].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode9Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[8].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode10Fld, (char *)sNetworkIdOld.allowed[9].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode10Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[9].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode11Fld, (char *)sNetworkIdOld.allowed[10].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode11Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[10].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode12Fld, (char *)sNetworkIdOld.allowed[11].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode12Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[11].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode13Fld, (char *)sNetworkIdOld.allowed[12].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode13Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[12].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode14Fld, (char *)sNetworkIdOld.allowed[13].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode14Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[13].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode15Fld, (char *)sNetworkIdOld.allowed[14].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode15Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[14].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode16Fld, (char *)sNetworkIdOld.allowed[15].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode16Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[15].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode17Fld, (char *)sNetworkIdOld.allowed[16].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode17Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[16].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode18Fld, (char *)sNetworkIdOld.allowed[17].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode18Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[17].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrCode19Fld, (char *)sNetworkIdOld.allowed[18].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode19Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[18].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
/*	if (strcmp ( m_StrCode20Fld, (char *)sNetworkIdOld.allowed[19].processing_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCode20Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.allowed[19].processing_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Processing Code 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }*/

    
        return TRUE;
}


/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkProcessing::CleanProcessingDialog()
{
	m_StrCode1Fld = _T("");
	m_StrCode10Fld = _T("");
	m_StrCode11Fld = _T("");
	m_StrCode12Fld = _T("");
	m_StrCode13Fld = _T("");
	m_StrCode14Fld = _T("");
	m_StrCode15Fld = _T("");
	m_StrCode16Fld = _T("");
	m_StrCode17Fld = _T("");
	m_StrCode18Fld = _T("");
	m_StrCode19Fld = _T("");
	m_StrCode2Fld = _T("");
	m_StrCode20Fld = _T("");
	m_StrCode3Fld = _T("");
	m_StrCode4Fld = _T("");
	m_StrCode5Fld = _T("");
	m_StrCode6Fld = _T("");
	m_StrCode7Fld = _T("");
	m_StrCode8Fld = _T("");
	m_StrCode9Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	
	UpdateData(FALSE);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkProcessing::EnableProcessingDialog(int nStatus)
{
//	if ( m_nDialogMode != MODE_VIEW )
    {	
  	    m_CntlCode1Fld.EnableWindow(nStatus);
		m_CntlCode2Fld.EnableWindow(nStatus);
		m_CntlCode3Fld.EnableWindow(nStatus);
		m_CntlCode4Fld.EnableWindow(nStatus);
		m_CntlCode5Fld.EnableWindow(nStatus);
		m_CntlCode6Fld.EnableWindow(nStatus);
		m_CntlCode7Fld.EnableWindow(nStatus);
		m_CntlCode8Fld.EnableWindow(nStatus);
		m_CntlCode9Fld.EnableWindow(nStatus);
		m_CntlCode10Fld.EnableWindow(nStatus);
		m_CntlCode11Fld.EnableWindow(nStatus);
		m_CntlCode12Fld.EnableWindow(nStatus);
		m_CntlCode13Fld.EnableWindow(nStatus);
		m_CntlCode14Fld.EnableWindow(nStatus);
		m_CntlCode15Fld.EnableWindow(nStatus);
		m_CntlCode16Fld.EnableWindow(nStatus);
		m_CntlCode17Fld.EnableWindow(nStatus);
		m_CntlCode18Fld.EnableWindow(nStatus);
		m_CntlCode19Fld.EnableWindow(nStatus);
		m_CntlCode20Fld.EnableWindow(nStatus);		      	    
	}
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkProcessing::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, m_StrNetworkIdFld);
    strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"NCF01",               sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

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
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}	
}



