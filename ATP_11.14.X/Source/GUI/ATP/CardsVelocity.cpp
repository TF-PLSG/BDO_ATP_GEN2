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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsVelocity.cpp  $
   
      Rev 1.9   Dec 01 2003 11:28:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 05 2000 14:53:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:08:00   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:46:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:16   iarustam
   Initial Release
   
      Rev 1.5   May 10 1999 11:09:12   dperson
   Standardized error messages
   
      Rev 1.4   Apr 29 1999 14:56:08   iarustam
   Bug fixes
   
      Rev 1.3   Mar 16 1999 16:29:42   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:20   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:56   iarustam
    
*
************************************************************************************/
// CardsVelocity.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsVelocity.h"
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
// CCardsVelocity property page

IMPLEMENT_DYNCREATE(CCardsVelocity, CPropertyPage)

CCardsVelocity::CCardsVelocity() : CPropertyPage(CCardsVelocity::IDD)
{
	//{{AFX_DATA_INIT(CCardsVelocity)
	m_StrAmount1_Fld = _T("");
	m_StrAmount2_Fld = _T("");
	m_StrAmount3_Fld = _T("");
	m_StrAmount4_Fld = _T("");
	m_StrAmount5_Fld = _T("");
	m_StrAmount6_Fld = _T("");
	m_StrAmount7_Fld = _T("");
	m_StrAmount8_Fld = _T("");
	m_StrAmount9_Fld = _T("");
	m_StrAmount10_Fld = _T("");
	m_StrFromTime3_Fld = _T("");
	m_StrFromTime2_Fld = _T("");
	m_StrFromTime1_Fld = _T("");
	m_StrFromTime4_Fld = _T("");
	m_StrFromTime5_Fld = _T("");
	m_StrFromTime6_Fld = _T("");
	m_StrFromTime7_Fld = _T("");
	m_StrFromTime8_Fld = _T("");
	m_StrFromTime9_Fld = _T("");
	m_StrFromTime10_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrToTime10_Fld = _T("");
	m_StrToTime2_Fld = _T("");
	m_StrToTime3_Fld = _T("");
	m_StrToTime4_Fld = _T("");
	m_StrToTime5_Fld = _T("");
	m_StrToTime6_Fld = _T("");
	m_StrToTime7_Fld = _T("");
	m_StrToTime8_Fld = _T("");
	m_StrToTime9_Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrToTime1_Fld = _T("");
	//}}AFX_DATA_INIT
}

CCardsVelocity::~CCardsVelocity()
{
}

void CCardsVelocity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsVelocity)
	DDX_Control(pDX, IDC_TO1_FLD, m_CntlToTime1_Fld);
	DDX_Control(pDX, IDC_TO9_FLD, m_CntlToTime9_Fld);
	DDX_Control(pDX, IDC_TO8_FLD, m_CntlToTime8_Fld);
	DDX_Control(pDX, IDC_TO7_FLD, m_CntlToTime7_Fld);
	DDX_Control(pDX, IDC_TO6_FLD, m_CntlToTime6_Fld);
	DDX_Control(pDX, IDC_TO5_FLD, m_CntlToTime5_Fld);
	DDX_Control(pDX, IDC_TO4_FLD, m_CntlToTime4_Fld);
	DDX_Control(pDX, IDC_TO3_FLD, m_CntlToTime3_Fld);
	DDX_Control(pDX, IDC_TO2_FLD, m_CntlToTime2_Fld);
	DDX_Control(pDX, IDC_TO10_FLD, m_CntlToTime10_Fld);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlType_Fld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileId_Fld);
	DDX_Control(pDX, IDC_FROM9_FLD, m_CntlFromTime9_Fld);
	DDX_Control(pDX, IDC_FROM8_FLD, m_CntlFromTime8_Fld);
	DDX_Control(pDX, IDC_FROM7_FLD, m_CntlFromTime7_Fld);
	DDX_Control(pDX, IDC_FROM6_FLD, m_CntlFromTime6_Fld);
	DDX_Control(pDX, IDC_FROM5_FLD, m_CntlFromTime5_Fld);
	DDX_Control(pDX, IDC_FROM4_FLD, m_CntlFromTime4_Fld);
	DDX_Control(pDX, IDC_FROM3_FLD, m_CntlFromTime3_Fld);
	DDX_Control(pDX, IDC_FROM10_FLD, m_CntlFromTime10_Fld);
	DDX_Control(pDX, IDC_FROM1_FLD, m_CntlFromTime1_Fld);
	DDX_Control(pDX, IDC_FROM2_FLD, m_CntlFromTime2_Fld);
	DDX_Control(pDX, IDC_AMOUNT10_FLD, m_CntlAmount10_Fld);
	DDX_Control(pDX, IDC_AMOUNT1_FLD, m_CntlAmount1_Fld);
	DDX_Control(pDX, IDC_AMOUNT9_FLD, m_CntlAmount9_Fld);
	DDX_Control(pDX, IDC_AMOUNT8_FLD, m_CntlAmount8_Fld);
	DDX_Control(pDX, IDC_AMOUNT7_FLD, m_CntlAmount7_Fld);
	DDX_Control(pDX, IDC_AMOUNT6_FLD, m_CntlAmount6_Fld);
	DDX_Control(pDX, IDC_AMOUNT5_FLD, m_CntlAmount5_Fld);
	DDX_Control(pDX, IDC_AMOUNT4_FLD, m_CntlAmount4_Fld);
	DDX_Control(pDX, IDC_AMOUNT3_FLD, m_CntlAmount3_Fld);
	DDX_Control(pDX, IDC_AMOUNT2_FLD, m_CntlAmount2_Fld);
	DDX_Text(pDX, IDC_AMOUNT1_FLD, m_StrAmount1_Fld);
	DDV_MaxChars(pDX, m_StrAmount1_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT2_FLD, m_StrAmount2_Fld);
	DDV_MaxChars(pDX, m_StrAmount2_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT3_FLD, m_StrAmount3_Fld);
	DDV_MaxChars(pDX, m_StrAmount3_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT4_FLD, m_StrAmount4_Fld);
	DDV_MaxChars(pDX, m_StrAmount4_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT5_FLD, m_StrAmount5_Fld);
	DDV_MaxChars(pDX, m_StrAmount5_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT6_FLD, m_StrAmount6_Fld);
	DDV_MaxChars(pDX, m_StrAmount6_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT7_FLD, m_StrAmount7_Fld);
	DDV_MaxChars(pDX, m_StrAmount7_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT8_FLD, m_StrAmount8_Fld);
	DDV_MaxChars(pDX, m_StrAmount8_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT9_FLD, m_StrAmount9_Fld);
	DDV_MaxChars(pDX, m_StrAmount9_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT10_FLD, m_StrAmount10_Fld);
	DDV_MaxChars(pDX, m_StrAmount10_Fld, 10);
	DDX_Text(pDX, IDC_FROM3_FLD, m_StrFromTime3_Fld);
	DDV_MaxChars(pDX, m_StrFromTime3_Fld, 5);
	DDX_Text(pDX, IDC_FROM2_FLD, m_StrFromTime2_Fld);
	DDV_MaxChars(pDX, m_StrFromTime2_Fld, 5);
	DDX_Text(pDX, IDC_FROM1_FLD, m_StrFromTime1_Fld);
	DDV_MaxChars(pDX, m_StrFromTime1_Fld, 5);
	DDX_Text(pDX, IDC_FROM4_FLD, m_StrFromTime4_Fld);
	DDV_MaxChars(pDX, m_StrFromTime4_Fld, 5);
	DDX_Text(pDX, IDC_FROM5_FLD, m_StrFromTime5_Fld);
	DDV_MaxChars(pDX, m_StrFromTime5_Fld, 5);
	DDX_Text(pDX, IDC_FROM6_FLD, m_StrFromTime6_Fld);
	DDV_MaxChars(pDX, m_StrFromTime6_Fld, 5);
	DDX_Text(pDX, IDC_FROM7_FLD, m_StrFromTime7_Fld);
	DDV_MaxChars(pDX, m_StrFromTime7_Fld, 5);
	DDX_Text(pDX, IDC_FROM8_FLD, m_StrFromTime8_Fld);
	DDV_MaxChars(pDX, m_StrFromTime8_Fld, 5);
	DDX_Text(pDX, IDC_FROM9_FLD, m_StrFromTime9_Fld);
	DDV_MaxChars(pDX, m_StrFromTime9_Fld, 5);
	DDX_Text(pDX, IDC_FROM10_FLD, m_StrFromTime10_Fld);
	DDV_MaxChars(pDX, m_StrFromTime10_Fld, 5);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrType_Fld);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);
	DDX_Text(pDX, IDC_TO10_FLD, m_StrToTime10_Fld);
	DDV_MaxChars(pDX, m_StrToTime10_Fld, 5);
	DDX_Text(pDX, IDC_TO2_FLD, m_StrToTime2_Fld);
	DDV_MaxChars(pDX, m_StrToTime2_Fld, 5);
	DDX_Text(pDX, IDC_TO3_FLD, m_StrToTime3_Fld);
	DDV_MaxChars(pDX, m_StrToTime3_Fld, 5);
	DDX_Text(pDX, IDC_TO4_FLD, m_StrToTime4_Fld);
	DDV_MaxChars(pDX, m_StrToTime4_Fld, 5);
	DDX_Text(pDX, IDC_TO5_FLD, m_StrToTime5_Fld);
	DDV_MaxChars(pDX, m_StrToTime5_Fld, 5);
	DDX_Text(pDX, IDC_TO6_FLD, m_StrToTime6_Fld);
	DDV_MaxChars(pDX, m_StrToTime6_Fld, 5);
	DDX_Text(pDX, IDC_TO7_FLD, m_StrToTime7_Fld);
	DDV_MaxChars(pDX, m_StrToTime7_Fld, 5);
	DDX_Text(pDX, IDC_TO8_FLD, m_StrToTime8_Fld);
	DDV_MaxChars(pDX, m_StrToTime8_Fld, 5);
	DDX_Text(pDX, IDC_TO9_FLD, m_StrToTime9_Fld);
	DDV_MaxChars(pDX, m_StrToTime9_Fld, 5);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TO1_FLD, m_StrToTime1_Fld);
	DDV_MaxChars(pDX, m_StrToTime1_Fld, 5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsVelocity, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsVelocity)
	ON_EN_KILLFOCUS(IDC_FROM1_FLD, OnKillfocusFrom1Fld)
	ON_EN_KILLFOCUS(IDC_FROM2_FLD, OnKillfocusFrom2Fld)
	ON_EN_KILLFOCUS(IDC_FROM3_FLD, OnKillfocusFrom3Fld)
	ON_EN_KILLFOCUS(IDC_FROM4_FLD, OnKillfocusFrom4Fld)
	ON_EN_KILLFOCUS(IDC_FROM5_FLD, OnKillfocusFrom5Fld)
	ON_EN_KILLFOCUS(IDC_FROM6_FLD, OnKillfocusFrom6Fld)
	ON_EN_KILLFOCUS(IDC_FROM7_FLD, OnKillfocusFrom7Fld)
	ON_EN_KILLFOCUS(IDC_FROM8_FLD, OnKillfocusFrom8Fld)
	ON_EN_KILLFOCUS(IDC_FROM9_FLD, OnKillfocusFrom9Fld)
	ON_EN_KILLFOCUS(IDC_FROM10_FLD, OnKillfocusFrom10Fld)
	ON_EN_KILLFOCUS(IDC_TO1_FLD, OnKillfocusTo1Fld)
	ON_EN_KILLFOCUS(IDC_TO2_FLD, OnKillfocusTo2Fld)
	ON_EN_KILLFOCUS(IDC_TO3_FLD, OnKillfocusTo3Fld)
	ON_EN_KILLFOCUS(IDC_TO4_FLD, OnKillfocusTo4Fld)
	ON_EN_KILLFOCUS(IDC_TO5_FLD, OnKillfocusTo5Fld)
	ON_EN_KILLFOCUS(IDC_TO6_FLD, OnKillfocusTo6Fld)
	ON_EN_KILLFOCUS(IDC_TO7_FLD, OnKillfocusTo7Fld)
	ON_EN_KILLFOCUS(IDC_TO8_FLD, OnKillfocusTo8Fld)
	ON_EN_KILLFOCUS(IDC_TO9_FLD, OnKillfocusTo9Fld)
	ON_EN_KILLFOCUS(IDC_TO10_FLD, OnKillfocusTo10Fld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsVelocity message handlers

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsVelocity::OnInitDialog() 
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

	m_bVelocity = TRUE;

	if (m_nDialogMode != MODE_DONE )
    {
		if (m_nDialogMode != MODE_VIEW)
        EnableVelocityDialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
        PopulateVelocityDialogFromDatabase ( sFleetCardOld);
    } 

	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsVelocity::PopulateVelocityDialogFromDatabase(  FAF01 sFleetCard)
{
    CMyUtils myUtilsObject;
    if ( m_nDialogMode != MODE_VIEW )
       EnableVelocityDialog(TRUE);

    ZeroMemory ( &sFleetCardOld, sizeof (FAF01));

	m_StrProfileId_Fld          = sProfileId.primary_key.caf_profile_id;
   m_StrType_Fld               = sProfileId.primary_key.type;

	if ( strcmp (m_StrType_Fld, "C") == 0 )
		m_StrType_Fld = "CREDIT";
	else if ( strcmp (m_StrType_Fld, "F") == 0 )
		m_StrType_Fld = "FLEET";
	else 
	m_StrType_Fld = "DEBIT";

    m_StrProfileId_Fld   = sFleetCard.primary_key.caf_profile_id;   
    m_StrFromTime1_Fld   = sFleetCard.velocity[0].from_time;
    m_StrFromTime2_Fld   = sFleetCard.velocity[1].from_time;
    m_StrFromTime3_Fld   = sFleetCard.velocity[2].from_time;
    m_StrFromTime4_Fld   = sFleetCard.velocity[3].from_time;
    m_StrFromTime5_Fld   = sFleetCard.velocity[4].from_time;
    m_StrFromTime6_Fld   = sFleetCard.velocity[5].from_time;
    m_StrFromTime7_Fld   = sFleetCard.velocity[6].from_time;
    m_StrFromTime8_Fld   = sFleetCard.velocity[7].from_time;
    m_StrFromTime9_Fld   = sFleetCard.velocity[8].from_time;
    m_StrFromTime10_Fld  = sFleetCard.velocity[9].from_time;
    m_StrToTime1_Fld     = sFleetCard.velocity[0].to_time;
    m_StrToTime2_Fld     = sFleetCard.velocity[1].to_time;
    m_StrToTime3_Fld     = sFleetCard.velocity[2].to_time;
    m_StrToTime4_Fld     = sFleetCard.velocity[3].to_time;
    m_StrToTime5_Fld     = sFleetCard.velocity[4].to_time;
    m_StrToTime6_Fld     = sFleetCard.velocity[5].to_time;
    m_StrToTime7_Fld     = sFleetCard.velocity[6].to_time;
    m_StrToTime8_Fld     = sFleetCard.velocity[7].to_time;
    m_StrToTime9_Fld     = sFleetCard.velocity[8].to_time;
    m_StrToTime10_Fld    = sFleetCard.velocity[9].to_time;
    m_StrAmount1_Fld     = sFleetCard.velocity[0].amount;
    m_StrAmount2_Fld     = sFleetCard.velocity[1].amount;
    m_StrAmount3_Fld     = sFleetCard.velocity[2].amount;
    m_StrAmount4_Fld     = sFleetCard.velocity[3].amount;
    m_StrAmount5_Fld     = sFleetCard.velocity[4].amount;
    m_StrAmount6_Fld     = sFleetCard.velocity[5].amount;
    m_StrAmount7_Fld     = sFleetCard.velocity[6].amount;
    m_StrAmount8_Fld     = sFleetCard.velocity[7].amount;
    m_StrAmount9_Fld     = sFleetCard.velocity[8].amount;
    m_StrAmount10_Fld    = sFleetCard.velocity[9].amount;

	if (!myUtilsObject.IsEmpty (m_StrFromTime1_Fld, this))
	    m_StrFromTime1_Fld   = m_StrFromTime1_Fld.Left(2) + ":" + m_StrFromTime1_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrFromTime2_Fld, this))
        m_StrFromTime2_Fld   = m_StrFromTime2_Fld.Left(2) + ":" + m_StrFromTime2_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrFromTime3_Fld, this))
        m_StrFromTime3_Fld   = m_StrFromTime3_Fld.Left(2) + ":" + m_StrFromTime3_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrFromTime4_Fld, this))
        m_StrFromTime4_Fld   = m_StrFromTime4_Fld.Left(2) + ":" + m_StrFromTime4_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrFromTime5_Fld, this))
        m_StrFromTime5_Fld   = m_StrFromTime5_Fld.Left(2) + ":" + m_StrFromTime5_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrFromTime6_Fld, this))
        m_StrFromTime6_Fld   = m_StrFromTime6_Fld.Left(2) + ":" + m_StrFromTime6_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrFromTime7_Fld, this))
        m_StrFromTime7_Fld   = m_StrFromTime7_Fld.Left(2) + ":" + m_StrFromTime7_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrFromTime8_Fld, this))
        m_StrFromTime8_Fld   = m_StrFromTime8_Fld.Left(2) + ":" + m_StrFromTime8_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrFromTime9_Fld, this))
        m_StrFromTime9_Fld   = m_StrFromTime9_Fld.Left(2) + ":" + m_StrFromTime9_Fld.Right(2);
  	if (!myUtilsObject.IsEmpty (m_StrFromTime10_Fld, this))
        m_StrFromTime10_Fld  = m_StrFromTime10_Fld.Left(2) + ":" + m_StrFromTime10_Fld.Right(2);
  	if (!myUtilsObject.IsEmpty (m_StrToTime1_Fld, this))
        m_StrToTime1_Fld     = m_StrToTime1_Fld.Left(2) + ":" + m_StrToTime1_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime2_Fld, this))
        m_StrToTime2_Fld     = m_StrToTime2_Fld.Left(2) + ":" + m_StrToTime2_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime3_Fld, this))
        m_StrToTime3_Fld     = m_StrToTime3_Fld.Left(2) + ":" + m_StrToTime3_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime4_Fld, this))
        m_StrToTime4_Fld     = m_StrToTime4_Fld.Left(2) + ":" + m_StrToTime4_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime5_Fld, this))
        m_StrToTime5_Fld     = m_StrToTime5_Fld.Left(2) + ":" + m_StrToTime5_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrToTime6_Fld, this))
        m_StrToTime6_Fld     = m_StrToTime6_Fld.Left(2) + ":" + m_StrToTime6_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime7_Fld, this))
        m_StrToTime7_Fld     = m_StrToTime7_Fld.Left(2) + ":" + m_StrToTime7_Fld.Right(2);
 	if (!myUtilsObject.IsEmpty (m_StrToTime8_Fld, this))
        m_StrToTime8_Fld     = m_StrToTime8_Fld.Left(2) + ":" + m_StrToTime8_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrToTime9_Fld, this))
        m_StrToTime9_Fld     = m_StrToTime9_Fld.Left(2) + ":" + m_StrToTime9_Fld.Right(2);
	if (!myUtilsObject.IsEmpty (m_StrToTime10_Fld, this))
        m_StrToTime10_Fld    = m_StrToTime10_Fld.Left(2) + ":" + m_StrToTime10_Fld.Right(2);    

    m_StrAmount1_Fld     = myUtilsObject.GetAmount(m_StrAmount1_Fld);
    m_StrAmount2_Fld     = myUtilsObject.GetAmount(m_StrAmount2_Fld);
    m_StrAmount3_Fld     = myUtilsObject.GetAmount(m_StrAmount3_Fld);
    m_StrAmount4_Fld     = myUtilsObject.GetAmount(m_StrAmount4_Fld);
    m_StrAmount5_Fld     = myUtilsObject.GetAmount(m_StrAmount5_Fld);
    m_StrAmount6_Fld     = myUtilsObject.GetAmount(m_StrAmount6_Fld);
    m_StrAmount7_Fld     = myUtilsObject.GetAmount(m_StrAmount7_Fld);
    m_StrAmount8_Fld     = myUtilsObject.GetAmount(m_StrAmount8_Fld);
    m_StrAmount9_Fld     = myUtilsObject.GetAmount(m_StrAmount9_Fld);
    m_StrAmount10_Fld    = myUtilsObject.GetAmount(m_StrAmount10_Fld);   

    UpdateData(FALSE);

    memcpy ( &sFleetCardOld, &sFleetCard, sizeof ( FAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsVelocity::PopulateDatabaseFromVelocityDialog( pFAF01 psFleetCard )
{ 
    CMyUtils myUtilsObject;

	if ( !m_bVelocity )
		return TRUE;

	
    m_StrAmount1_Fld = myUtilsObject.GenerateAmount(m_StrAmount1_Fld, 12);
    m_StrAmount2_Fld = myUtilsObject.GenerateAmount(m_StrAmount2_Fld, 12);
    m_StrAmount3_Fld = myUtilsObject.GenerateAmount(m_StrAmount3_Fld, 12);
    m_StrAmount4_Fld = myUtilsObject.GenerateAmount(m_StrAmount4_Fld, 12);
    m_StrAmount5_Fld = myUtilsObject.GenerateAmount(m_StrAmount5_Fld, 12);
    m_StrAmount6_Fld = myUtilsObject.GenerateAmount(m_StrAmount6_Fld, 12);
    m_StrAmount7_Fld = myUtilsObject.GenerateAmount(m_StrAmount7_Fld, 12);
    m_StrAmount8_Fld = myUtilsObject.GenerateAmount(m_StrAmount8_Fld, 12);
    m_StrAmount9_Fld = myUtilsObject.GenerateAmount(m_StrAmount9_Fld, 12);
    m_StrAmount10_Fld = myUtilsObject.GenerateAmount(m_StrAmount10_Fld, 12);

    CString temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10;
    CString temp1A, temp2A, temp3A, temp4A, temp5A, temp6A, temp7A, temp8A, temp9A, temp10A;

    if ( temp1.GetLength() != 0 )
      temp1 = m_StrFromTime1_Fld.Left(2) + m_StrFromTime1_Fld.Right(2);
    m_StrFromTime1_Fld = temp1;
    memcpy ( psFleetCard->velocity[0].from_time,  m_StrFromTime1_Fld,    sizeof ( psFleetCard->velocity[0].from_time));

    if ( temp2.GetLength() != 0 )
    temp2 = m_StrFromTime2_Fld.Left(2) + m_StrFromTime2_Fld.Right(2);
    m_StrFromTime2_Fld = temp2;
    memcpy ( psFleetCard->velocity[1].from_time,  m_StrFromTime2_Fld,    sizeof ( psFleetCard->velocity[1].from_time));

    if ( temp3.GetLength() != 0 )
    temp3 = m_StrFromTime3_Fld.Left(2) + m_StrFromTime3_Fld.Right(2);
    m_StrFromTime3_Fld = temp3;
    memcpy ( psFleetCard->velocity[2].from_time,  m_StrFromTime3_Fld,    sizeof ( psFleetCard->velocity[2].from_time));

    if ( temp4.GetLength() != 0 )
    temp4 = m_StrFromTime4_Fld.Left(2) + m_StrFromTime4_Fld.Right(2);
    m_StrFromTime4_Fld = temp4;
    memcpy ( psFleetCard->velocity[3].from_time,  m_StrFromTime4_Fld,    sizeof ( psFleetCard->velocity[3].from_time));

    if ( temp5.GetLength() != 0 )
    temp5 = m_StrFromTime5_Fld.Left(2) + m_StrFromTime5_Fld.Right(2);
    m_StrFromTime5_Fld = temp5;
    memcpy ( psFleetCard->velocity[4].from_time,  m_StrFromTime5_Fld,    sizeof ( psFleetCard->velocity[4].from_time));

    if ( temp6.GetLength() != 0 )
    temp6 = m_StrFromTime6_Fld.Left(2) + m_StrFromTime6_Fld.Right(2);
    m_StrFromTime6_Fld = temp6;
    memcpy ( psFleetCard->velocity[5].from_time,  m_StrFromTime6_Fld,    sizeof ( psFleetCard->velocity[5].from_time));

    if ( temp7.GetLength() != 0 )
    temp7 = m_StrFromTime7_Fld.Left(2) + m_StrFromTime7_Fld.Right(2);
    m_StrFromTime7_Fld = temp7;
    memcpy ( psFleetCard->velocity[6].from_time,  m_StrFromTime7_Fld,    sizeof ( psFleetCard->velocity[6].from_time));

    if ( temp8.GetLength() != 0 )
    temp8 = m_StrFromTime8_Fld.Left(2) + m_StrFromTime8_Fld.Right(2);
    m_StrFromTime8_Fld = temp8;   
    memcpy ( psFleetCard->velocity[7].from_time,  m_StrFromTime8_Fld,    sizeof ( psFleetCard->velocity[7].from_time));

    if ( temp9.GetLength() != 0 )
    temp9 = m_StrFromTime9_Fld.Left(2) + m_StrFromTime9_Fld.Right(2);
    m_StrFromTime9_Fld = temp9;
    memcpy ( psFleetCard->velocity[8].from_time,  m_StrFromTime9_Fld,    sizeof ( psFleetCard->velocity[8].from_time));

    if ( temp10.GetLength() != 0 )
    temp10 = m_StrFromTime10_Fld.Left(2) + m_StrFromTime10_Fld.Right(2);
    m_StrFromTime10_Fld = temp10;
    memcpy ( psFleetCard->velocity[9].from_time,  m_StrFromTime10_Fld,    sizeof ( psFleetCard->velocity[9].from_time));
    
    if ( temp1A.GetLength() != 0 )
    temp1A = m_StrToTime1_Fld.Left(2) + m_StrToTime1_Fld.Right(2);
    m_StrToTime1_Fld = temp1A;
    memcpy ( psFleetCard->velocity[0].to_time,    m_StrToTime1_Fld,      sizeof ( psFleetCard->velocity[0].to_time));

    if ( temp2A.GetLength() != 0 )
    temp2A = m_StrToTime2_Fld.Left(2) + m_StrToTime2_Fld.Right(2);
    m_StrToTime2_Fld = temp2A;
    memcpy ( psFleetCard->velocity[1].to_time,    m_StrToTime2_Fld,      sizeof ( psFleetCard->velocity[1].to_time));

    if ( temp3A.GetLength() != 0 )
    temp3A = m_StrToTime3_Fld.Left(2) + m_StrToTime3_Fld.Right(2);
    m_StrToTime3_Fld = temp3A;
    memcpy ( psFleetCard->velocity[2].to_time,    m_StrToTime3_Fld,      sizeof ( psFleetCard->velocity[2].to_time));

    if ( temp4A.GetLength() != 0 )
    temp4A = m_StrToTime4_Fld.Left(2) + m_StrToTime4_Fld.Right(2);
    m_StrToTime4_Fld = temp4A;
    memcpy ( psFleetCard->velocity[3].to_time,    m_StrToTime4_Fld,      sizeof ( psFleetCard->velocity[3].to_time));

    if ( temp5A.GetLength() != 0 )
    temp5A = m_StrToTime5_Fld.Left(2) + m_StrToTime5_Fld.Right(2);
    m_StrToTime5_Fld = temp5A;
    memcpy ( psFleetCard->velocity[4].to_time,    m_StrToTime5_Fld,      sizeof ( psFleetCard->velocity[4].to_time));

    if ( temp6A.GetLength() != 0 )
    temp6A = m_StrToTime6_Fld.Left(2) + m_StrToTime6_Fld.Right(2);
    m_StrToTime6_Fld = temp6A;
    memcpy ( psFleetCard->velocity[5].to_time,    m_StrToTime6_Fld,      sizeof ( psFleetCard->velocity[5].to_time));

    if ( temp7A.GetLength() != 0 )
    temp7A = m_StrToTime7_Fld.Left(2) + m_StrToTime7_Fld.Right(2);
    m_StrToTime7_Fld = temp7A;
    memcpy ( psFleetCard->velocity[6].to_time,    m_StrToTime7_Fld,      sizeof ( psFleetCard->velocity[6].to_time));

    if ( temp8A.GetLength() != 0 )
    temp8A = m_StrToTime8_Fld.Left(2) + m_StrToTime8_Fld.Right(2);
    m_StrToTime8_Fld = temp8A;
    memcpy ( psFleetCard->velocity[7].to_time,    m_StrToTime8_Fld,      sizeof ( psFleetCard->velocity[7].to_time));

    if ( temp9A.GetLength() != 0 )
    temp9A = m_StrToTime9_Fld.Left(2) + m_StrToTime9_Fld.Right(2);
    m_StrToTime9_Fld = temp9A;
    memcpy ( psFleetCard->velocity[8].to_time,    m_StrToTime9_Fld,      sizeof ( psFleetCard->velocity[8].to_time));

    if ( temp10A.GetLength() != 0 )
    temp10A = m_StrToTime10_Fld.Left(2) + m_StrToTime10_Fld.Right(2);
    m_StrToTime10_Fld = temp10A;
    memcpy ( psFleetCard->velocity[9].to_time,    m_StrToTime10_Fld,      sizeof ( psFleetCard->velocity[9].to_time));

    memcpy ( psFleetCard->velocity[0].amount,     m_StrAmount1_Fld,      sizeof ( psFleetCard->velocity[0].amount)); 
    memcpy ( psFleetCard->velocity[1].amount,     m_StrAmount2_Fld,      sizeof ( psFleetCard->velocity[1].amount)); 
    memcpy ( psFleetCard->velocity[2].amount,     m_StrAmount3_Fld,      sizeof ( psFleetCard->velocity[2].amount)); 
    memcpy ( psFleetCard->velocity[3].amount,     m_StrAmount4_Fld,      sizeof ( psFleetCard->velocity[3].amount)); 
    memcpy ( psFleetCard->velocity[4].amount,     m_StrAmount5_Fld,      sizeof ( psFleetCard->velocity[4].amount));
    memcpy ( psFleetCard->velocity[5].amount,     m_StrAmount6_Fld,      sizeof ( psFleetCard->velocity[5].amount)); 
    memcpy ( psFleetCard->velocity[6].amount,     m_StrAmount7_Fld,      sizeof ( psFleetCard->velocity[6].amount)); 
    memcpy ( psFleetCard->velocity[7].amount,     m_StrAmount8_Fld,      sizeof ( psFleetCard->velocity[7].amount)); 
    memcpy ( psFleetCard->velocity[8].amount,     m_StrAmount9_Fld,      sizeof ( psFleetCard->velocity[8].amount));  
    memcpy ( psFleetCard->velocity[9].amount,     m_StrAmount10_Fld,     sizeof ( psFleetCard->velocity[9].amount));       
    
  
    return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsVelocity::CleanVelocityDialog()
{
    m_StrProfileId_Fld = "";
    m_StrType_Fld = "";
    m_StrFromTime1_Fld = "";
    m_StrFromTime2_Fld = "";
    m_StrFromTime3_Fld = "";
    m_StrFromTime4_Fld = "";
    m_StrFromTime5_Fld = "";
    m_StrFromTime6_Fld = "";
    m_StrFromTime7_Fld = "";
    m_StrFromTime8_Fld = "";
    m_StrFromTime9_Fld = "";
    m_StrFromTime10_Fld = "";
    m_StrToTime1_Fld = "";
    m_StrToTime2_Fld = "";
    m_StrToTime3_Fld = "";
    m_StrToTime4_Fld = "";
    m_StrToTime5_Fld = "";
    m_StrToTime6_Fld = "";
    m_StrToTime7_Fld = "";
    m_StrToTime8_Fld = "";
    m_StrToTime9_Fld = "";
    m_StrToTime10_Fld = "";
    m_StrAmount1_Fld = "";
    m_StrAmount2_Fld = "";
    m_StrAmount3_Fld = "";
    m_StrAmount4_Fld = "";
    m_StrAmount5_Fld = "";
    m_StrAmount6_Fld = "";
    m_StrAmount7_Fld = "";
    m_StrAmount8_Fld = "";
    m_StrAmount9_Fld = "";
    m_StrAmount10_Fld = "";       
    m_StrMessageLbl = "";

	UpdateData(FALSE);
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsVelocity::EnableVelocityDialog(int nStatus)
{
	  if ( m_nDialogMode != MODE_VIEW )
       {
		m_CntlFromTime1_Fld.EnableWindow(nStatus);
        m_CntlFromTime2_Fld.EnableWindow(nStatus);
        m_CntlFromTime3_Fld.EnableWindow(nStatus);
        m_CntlFromTime4_Fld.EnableWindow(nStatus);
        m_CntlFromTime5_Fld.EnableWindow(nStatus);
        m_CntlFromTime6_Fld.EnableWindow(nStatus);
        m_CntlFromTime7_Fld.EnableWindow(nStatus);
        m_CntlFromTime8_Fld.EnableWindow(nStatus);
        m_CntlFromTime9_Fld.EnableWindow(nStatus);
        m_CntlFromTime10_Fld.EnableWindow(nStatus);
        m_CntlToTime1_Fld.EnableWindow(nStatus);
        m_CntlToTime2_Fld.EnableWindow(nStatus);
        m_CntlToTime3_Fld.EnableWindow(nStatus);
        m_CntlToTime4_Fld.EnableWindow(nStatus);
        m_CntlToTime5_Fld.EnableWindow(nStatus);
        m_CntlToTime6_Fld.EnableWindow(nStatus);
        m_CntlToTime7_Fld.EnableWindow(nStatus);
        m_CntlToTime8_Fld.EnableWindow(nStatus);
        m_CntlToTime9_Fld.EnableWindow(nStatus);
        m_CntlToTime10_Fld.EnableWindow(nStatus);
        m_CntlAmount1_Fld.EnableWindow(nStatus);
        m_CntlAmount2_Fld.EnableWindow(nStatus);
        m_CntlAmount3_Fld.EnableWindow(nStatus);
        m_CntlAmount4_Fld.EnableWindow(nStatus);
        m_CntlAmount5_Fld.EnableWindow(nStatus);
        m_CntlAmount6_Fld.EnableWindow(nStatus);
        m_CntlAmount7_Fld.EnableWindow(nStatus);
        m_CntlAmount8_Fld.EnableWindow(nStatus);
        m_CntlAmount9_Fld.EnableWindow(nStatus);
        m_CntlAmount10_Fld.EnableWindow(nStatus);
      }

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsVelocity::OnCommand(WPARAM wParam, LPARAM lParam) 
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

BOOL CCardsVelocity::CompareStructures()
{
    if (strcmp ( m_StrFromTime1_Fld, (char *)sFleetCardOld.velocity[0].from_time) != 0)
    {
	    memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime1_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[0].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrFromTime2_Fld, (char *)sFleetCardOld.velocity[1].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime2_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[1].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrFromTime3_Fld, (char *)sFleetCardOld.velocity[2].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime3_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[2].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrFromTime4_Fld, (char *)sFleetCardOld.velocity[3].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime4_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[3].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrFromTime5_Fld, (char *)sFleetCardOld.velocity[4].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime5_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[4].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrFromTime6_Fld, (char *)sFleetCardOld.velocity[5].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime6_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[5].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrFromTime7_Fld, (char *)sFleetCardOld.velocity[6].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime7_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[6].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

    if (strcmp ( m_StrFromTime8_Fld, (char *)sFleetCardOld.velocity[7].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime8_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[7].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrFromTime9_Fld, (char *)sFleetCardOld.velocity[8].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime9_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[8].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrFromTime10_Fld, (char *)sFleetCardOld.velocity[9].from_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrFromTime10_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[9].from_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "From time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrToTime1_Fld, (char *)sFleetCardOld.velocity[0].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime1_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[0].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
	}
    if (strcmp ( m_StrToTime2_Fld, (char *)sFleetCardOld.velocity[1].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime2_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[1].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrToTime3_Fld, (char *)sFleetCardOld.velocity[2].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime3_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[2].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrToTime4_Fld, (char *)sFleetCardOld.velocity[3].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime4_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[3].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrToTime5_Fld, (char *)sFleetCardOld.velocity[4].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime5_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[4].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrToTime6_Fld, (char *)sFleetCardOld.velocity[5].to_time) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime6_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[5].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrToTime7_Fld, (char *)sFleetCardOld.velocity[6].to_time) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime7_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[6].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrToTime8_Fld, (char *)sFleetCardOld.velocity[7].to_time) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime8_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[7].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrToTime9_Fld, (char *)sFleetCardOld.velocity[8].to_time) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime9_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[8].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrToTime10_Fld, (char *)sFleetCardOld.velocity[9].to_time) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrToTime10_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[9].to_time, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "To time", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrAmount1_Fld, (char *)sFleetCardOld.velocity[0].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount1_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[0].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrAmount2_Fld, (char *)sFleetCardOld.velocity[1].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount2_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[1].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount3_Fld, (char *)sFleetCardOld.velocity[2].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount3_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[2].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount4_Fld, (char *)sFleetCardOld.velocity[3].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount4_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[3].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrAmount5_Fld, (char *)sFleetCardOld.velocity[4].amount) != 0)
     {
		 memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount5_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[4].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount6_Fld, (char *)sFleetCardOld.velocity[5].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount6_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[5].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount7_Fld, (char *)sFleetCardOld.velocity[6].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount7_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[6].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount8_Fld, (char *)sFleetCardOld.velocity[7].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount8_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[7].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     if (strcmp ( m_StrAmount9_Fld, (char *)sFleetCardOld.velocity[8].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount9_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[8].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }	
     if (strcmp ( m_StrAmount10_Fld, (char *)sFleetCardOld.velocity[9].amount) != 0)
     {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData, m_StrAmount10_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFleetCardOld.velocity[9].amount, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Amount", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
     
    
        return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsVelocity::PopulateHistoryTable()
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
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CAF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_StrCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_StrPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_StrFieldTag, sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

/* void CCardsVelocity::OnUpdateAmountFld(CString myString) 
{   
	int len;
    
	UpdateData(TRUE);

	len = myString.GetLength();

	if ( len == 1)
	{
		myString = "000000000" + myString ;		
		UpdateData(FALSE);
		
	}

    if ( len == 2)
	{
		myString = "00000000" + myString;		
		UpdateData(FALSE);
		
	}

    if ( len == 3)
	{
		myString = "0000000" + myString;		
		UpdateData(FALSE);
		
	}

    if ( len == 4)
	{
		myString = "000000" + myString;
		UpdateData(FALSE);
		
	}

    if ( len == 5)
	{
		myString = "00000" + myString;		
		UpdateData(FALSE);
		
	}

    if ( len == 6)
	{
		myString = "0000" + myString;	
		UpdateData(FALSE);
		
	}

    if ( len == 7)
	{
		myString = "000" + myString;		
		UpdateData(FALSE);
		
	}

    if ( len == 8)
	{
		myString= "00" + myString;		
		UpdateData(FALSE);
		
	}

    if ( len ==9)
	{
		myString = "0" + myString;	
		UpdateData(FALSE);
		
	}
    myString = myString + "00";

	m_StrMessageLbl = "";
	UpdateData(FALSE);
    return; 
	
}*/

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/ 

BOOL CCardsVelocity::CheckTime (CString strTime)
{    
    CString strHour;
    CString strMin;
    int hour, min;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    int len = strTime.GetLength();

    /*if ( len == 0 )
    {
		m_StrMessageLbl = "Please enter time";
		UpdateData(FALSE);
		return FALSE;
	}*/	

   if ( len != 4 )
    {
		m_StrMessageLbl = "ERROR: Invalid time";
		UpdateData(FALSE);
		return FALSE;
	}	

   /*int res = strspn ( strTime, "1234567890:");

	if ( res < len )
	{
		m_StrMessageLbl = "ERROR: Invalid character";

		UpdateData(FALSE);
		      return FALSE;  

	}

	if ( len == 5 )
	{
		int pos = strTime.Find (":");
		if ( pos != 2 )
		{
			m_StrMessageLbl = "ERROR: Invalid time";
			UpdateData(FALSE);
			return FALSE;
		}	

	}*/

   if ( strHour.GetLength() != 0 )
      strHour = strTime.Left(2);
   if ( strMin.GetLength() != 0 )
    strMin = strTime.Right(2);  

    hour = atoi (strHour);
    min = atoi (strMin);
    
   if (hour > 23 || min > 59)   
   {
	    m_StrMessageLbl = "ERROR: Invalid time";
		UpdateData(FALSE);
		return FALSE;
   }	
  return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsVelocity::OnKillfocusFrom1Fld() 
{    
       UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime1_Fld))
    {        
        UpdateData(FALSE);
        m_CntlFromTime1_Fld.SetFocus();
    }

    if ( m_StrFromTime1_Fld.GetLength() == 4)
    {
        m_StrFromTime1_Fld = m_StrFromTime1_Fld.Left(2) + ":" + m_StrFromTime1_Fld.Right(2);
        UpdateData(FALSE);
    }	

    
}

void CCardsVelocity::OnKillfocusFrom2Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime2_Fld))
    {    
        UpdateData(FALSE);
        m_CntlFromTime2_Fld.SetFocus();
    }

    if ( m_StrFromTime2_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime2_Fld.Left(2) + ":" + m_StrFromTime2_Fld.Right(2);
        m_StrFromTime2_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom3Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime3_Fld))
    {   
        UpdateData(FALSE);
        m_CntlFromTime3_Fld.SetFocus();
    }

    if ( m_StrFromTime3_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime3_Fld.Left(2) + ":" + m_StrFromTime3_Fld.Right(2);
        m_StrFromTime3_Fld = temp;
        UpdateData(FALSE);
    }	

	
}

void CCardsVelocity::OnKillfocusFrom4Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime4_Fld))
    {       
        UpdateData(FALSE);
        m_CntlFromTime4_Fld.SetFocus();
    }

    if ( m_StrFromTime4_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime4_Fld.Left(2) + ":" + m_StrFromTime4_Fld.Right(2);
        m_StrFromTime4_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom5Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime5_Fld))
    {       
        UpdateData(FALSE);
        m_CntlFromTime5_Fld.SetFocus();
    }

    if ( m_StrFromTime5_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime5_Fld.Left(2) + ":" + m_StrFromTime5_Fld.Right(2);
        m_StrFromTime5_Fld = temp;
        UpdateData(FALSE);
    }	
	
}

void CCardsVelocity::OnKillfocusFrom6Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime6_Fld))
    {   
        UpdateData(FALSE);
        m_CntlFromTime6_Fld.SetFocus();
    }

    if ( m_StrFromTime6_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime6_Fld.Left(2) + ":" + m_StrFromTime6_Fld.Right(2);
        m_StrFromTime6_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom7Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime7_Fld))
    {       
        UpdateData(FALSE);
        m_CntlFromTime7_Fld.SetFocus();
    }

    if ( m_StrFromTime7_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime7_Fld.Left(2) + ":" + m_StrFromTime7_Fld.Right(2);
        m_StrFromTime7_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom8Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime8_Fld))
    {      
        UpdateData(FALSE);
        m_CntlFromTime8_Fld.SetFocus();
    }

    if ( m_StrFromTime8_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime8_Fld.Left(2) + ":" + m_StrFromTime8_Fld.Right(2);
        m_StrFromTime8_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom9Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime9_Fld))
    {      
        UpdateData(FALSE);
        m_CntlFromTime9_Fld.SetFocus();
    }

    if ( m_StrFromTime9_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime9_Fld.Left(2) + ":" + m_StrFromTime9_Fld.Right(2);
        m_StrFromTime9_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusFrom10Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrFromTime10_Fld))
    {       
        UpdateData(FALSE);
        m_CntlFromTime10_Fld.SetFocus();
    }

    if ( m_StrFromTime10_Fld.GetLength() == 4)
    {
        CString temp = m_StrFromTime10_Fld.Left(2) + ":" + m_StrFromTime10_Fld.Right(2);
        m_StrFromTime10_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo1Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrToTime1_Fld))
    {    
        UpdateData(FALSE);
        m_CntlToTime1_Fld.SetFocus();
    }

    if ( m_StrToTime1_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime1_Fld.Left(2) + ":" + m_StrToTime1_Fld.Right(2);
        m_StrToTime1_Fld = temp;
        UpdateData(FALSE);
    }	

}

void CCardsVelocity::OnKillfocusTo2Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime2_Fld))
    {       
        UpdateData(FALSE);
        m_CntlToTime2_Fld.SetFocus();
    }

    if ( m_StrToTime2_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime2_Fld.Left(2) + ":" + m_StrToTime2_Fld.Right(2);
        m_StrToTime2_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo3Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime3_Fld))
    {      
        UpdateData(FALSE);
        m_CntlToTime3_Fld.SetFocus();
    }

    if ( m_StrToTime3_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime3_Fld.Left(2) + ":" + m_StrToTime3_Fld.Right(2);
        m_StrToTime3_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo4Fld() 
{
   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime4_Fld))
    {       
        UpdateData(FALSE);
        m_CntlToTime4_Fld.SetFocus();
    }

    if ( m_StrToTime4_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime4_Fld.Left(2) + ":" + m_StrToTime4_Fld.Right(2);
        m_StrToTime4_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo5Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime5_Fld))
    {      
        UpdateData(FALSE);
        m_CntlToTime5_Fld.SetFocus();
    }

    if ( m_StrToTime5_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime5_Fld.Left(2) + ":" + m_StrToTime5_Fld.Right(2);
        m_StrToTime5_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo6Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime6_Fld))
    {       
        UpdateData(FALSE);
        m_CntlToTime6_Fld.SetFocus();
    }

    if ( m_StrToTime6_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime6_Fld.Left(2) + ":" + m_StrToTime6_Fld.Right(2);
        m_StrToTime6_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo7Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime7_Fld))
    {        
        UpdateData(FALSE);
        m_CntlToTime7_Fld.SetFocus();
    }

    if ( m_StrToTime7_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime7_Fld.Left(2) + ":" + m_StrToTime7_Fld.Right(2);
        m_StrToTime7_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo8Fld() 
{
       UpdateData(TRUE);

	if (!CheckTime(m_StrToTime8_Fld))
    {     
        UpdateData(FALSE);
        m_CntlToTime8_Fld.SetFocus();
    }

    if ( m_StrToTime8_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime8_Fld.Left(2) + ":" + m_StrToTime8_Fld.Right(2);
        m_StrToTime8_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo9Fld() 
{
	   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime9_Fld))
    {      
        UpdateData(FALSE);
        m_CntlToTime9_Fld.SetFocus();
    }

    if ( m_StrToTime9_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime9_Fld.Left(2) + ":" + m_StrToTime9_Fld.Right(2);
        m_StrToTime9_Fld = temp;
        UpdateData(FALSE);
    }	
}

void CCardsVelocity::OnKillfocusTo10Fld() 
{
   UpdateData(TRUE);

	if (!CheckTime(m_StrToTime10_Fld))
    {     
        UpdateData(FALSE);
        m_CntlToTime10_Fld.SetFocus();
    }

    if ( m_StrToTime10_Fld.GetLength() == 4)
    {
        CString temp = m_StrToTime10_Fld.Left(2) + ":" + m_StrToTime10_Fld.Right(2);
        m_StrToTime10_Fld = temp;
        UpdateData(FALSE);
    }	
}
