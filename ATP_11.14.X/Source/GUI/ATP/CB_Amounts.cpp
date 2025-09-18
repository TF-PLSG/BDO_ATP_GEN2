// CB_Amounts.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CB_Definition.h"
#include "CB_General.h"
#include "CB_Amounts.h"

#include "ChangeHistory.h"
#include "NameList.h"

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SharedMem.h"

extern CSharedMem shrmem;

extern UCF01 ucf01;
extern CString name;

/////////////////////////////////////////////////////////////////////////////
// CCB_Amounts property page

IMPLEMENT_DYNCREATE(CCB_Amounts, CPropertyPage)

CCB_Amounts::CCB_Amounts() : CPropertyPage(CCB_Amounts::IDD)
{
	//{{AFX_DATA_INIT(CCB_Amounts)
	m_StrAmtHigh1 = _T("");
	m_StrAmtLow1 = _T("");
	m_StrAmtOther1 = _T("");
	m_StrAmtText1 = _T("");
	m_StrAmtHigh2 = _T("");
	m_StrAmtLow2 = _T("");
	m_StrAmtOther2 = _T("");
	m_StrAmtText2 = _T("");
	m_StrAmtHigh3 = _T("");
	m_StrAmtLow3 = _T("");
	m_StrAmtOther3 = _T("");
	m_StrAmtText3 = _T("");
	m_StrAmtHigh4 = _T("");
	m_StrAmtLow4 = _T("");
	m_StrAmtOther4 = _T("");
	m_StrAmtText4 = _T("");
	m_StrAmtHigh5 = _T("");
	m_StrAmtOther5 = _T("");
	m_StrAmtLow5 = _T("");
	m_StrAmtText5 = _T("");
	m_StrCashBonusId = _T("");
	m_StrMessageLbl = _T("");
	m_IntWinOther1Radio = -1;
	m_IntWinOther2Radio = -1;
	m_IntWinOther3Radio = -1;
	m_IntWinOther4Radio = -1;
	m_IntWinOther5Radio = -1;
	m_IntPurchase1Radio = -1;
	m_IntPurchase2Radio = -1;
	m_IntPurchase3Radio = -1;
	m_IntPurchase4Radio = -1;
	m_IntPurchase5Radio = -1;
	//}}AFX_DATA_INIT
}

CCB_Amounts::~CCB_Amounts()
{
}

void CCB_Amounts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCB_Amounts)
	DDX_Control(pDX, IDC_WIN_PURCHASE5_RADIO, m_CntlPurchase5Radio);
	DDX_Control(pDX, IDC_WIN_PURCHASE4_RADIO, m_CntlPurchase4Radio);
	DDX_Control(pDX, IDC_WIN_PURCHASE3_RADIO, m_CntlPurchase3Radio);
	DDX_Control(pDX, IDC_WIN_PURCHASE2_RADIO, m_CntlPurchase2Radio);
	DDX_Control(pDX, IDC_WIN_PURCHASE1_RADIO, m_CntlPurchase1Radio);
	DDX_Control(pDX, IDC_WIN_OTHER5_RADIO, m_CntlWinOther5Radio);
	DDX_Control(pDX, IDC_WIN_OTHER4_RADIO, m_CntlWinOther4Radio);
	DDX_Control(pDX, IDC_WIN_OTHER3_RADIO, m_CntlWinOther3Radio);
	DDX_Control(pDX, IDC_WIN_OTHER2_RADIO, m_CntlWinOther2Radio);
	DDX_Control(pDX, IDC_WIN_OTHER1_RADIO, m_CntlWinOther1Radio);
	DDX_Control(pDX, IDC_CB_ID_FLD, m_CntlCashBonusId);
	DDX_Control(pDX, IDC_AMOUNT5_TEXT, m_CntlAmtText5);
	DDX_Control(pDX, IDC_AMOUNT5_OTHER, m_CntlAmtOther5);
	DDX_Control(pDX, IDC_AMOUNT5_LOW, m_CntlAmtLow5);
	DDX_Control(pDX, IDC_AMOUNT5_HIGH, m_CntlAmtHigh5);
	DDX_Control(pDX, IDC_AMOUNT4_TEXT, m_CntlAmtText4);
	DDX_Control(pDX, IDC_AMOUNT4_OTHER, m_CntlAmtOther4);
	DDX_Control(pDX, IDC_AMOUNT4_LOW, m_CntlAmtLow4);
	DDX_Control(pDX, IDC_AMOUNT4_HIGH, m_CntlAmtHigh4);
	DDX_Control(pDX, IDC_AMOUNT3_TEXT, m_CntlAmtText3);
	DDX_Control(pDX, IDC_AMOUNT3_OTHER, m_CntlAmtOther3);
	DDX_Control(pDX, IDC_AMOUNT3_LOW, m_CntlAmtLow3);
	DDX_Control(pDX, IDC_AMOUNT3_HIGH, m_CntlAmtHigh3);
	DDX_Control(pDX, IDC_AMOUNT2_TEXT, m_CntlAmtText2);
	DDX_Control(pDX, IDC_AMOUNT2_OTHER, m_CntlAmtOther2);
	DDX_Control(pDX, IDC_AMOUNT2_LOW, m_CntlAmtLow2);
	DDX_Control(pDX, IDC_AMOUNT2_HIGH, m_CntlAmtHigh2);
	DDX_Control(pDX, IDC_AMOUNT1_TEXT, m_CntlAmtText1);
	DDX_Control(pDX, IDC_AMOUNT1_OTHER, m_CntlAmtOther1);
	DDX_Control(pDX, IDC_AMOUNT1_LOW, m_CntlAmtLow1);
	DDX_Control(pDX, IDC_AMOUNT1_HIGH, m_CntlAmtHigh1);
	DDX_Text(pDX, IDC_AMOUNT1_HIGH, m_StrAmtHigh1);
	DDV_MaxChars(pDX, m_StrAmtHigh1, 12);
	DDX_Text(pDX, IDC_AMOUNT1_LOW, m_StrAmtLow1);
	DDV_MaxChars(pDX, m_StrAmtLow1, 12);
	DDX_Text(pDX, IDC_AMOUNT1_OTHER, m_StrAmtOther1);
	DDV_MaxChars(pDX, m_StrAmtOther1, 12);
	DDX_Text(pDX, IDC_AMOUNT1_TEXT, m_StrAmtText1);
	DDV_MaxChars(pDX, m_StrAmtText1, 40);
	DDX_Text(pDX, IDC_AMOUNT2_HIGH, m_StrAmtHigh2);
	DDV_MaxChars(pDX, m_StrAmtHigh2, 12);
	DDX_Text(pDX, IDC_AMOUNT2_LOW, m_StrAmtLow2);
	DDV_MaxChars(pDX, m_StrAmtLow2, 12);
	DDX_Text(pDX, IDC_AMOUNT2_OTHER, m_StrAmtOther2);
	DDV_MaxChars(pDX, m_StrAmtOther2, 12);
	DDX_Text(pDX, IDC_AMOUNT2_TEXT, m_StrAmtText2);
	DDV_MaxChars(pDX, m_StrAmtText2, 40);
	DDX_Text(pDX, IDC_AMOUNT3_HIGH, m_StrAmtHigh3);
	DDV_MaxChars(pDX, m_StrAmtHigh3, 12);
	DDX_Text(pDX, IDC_AMOUNT3_LOW, m_StrAmtLow3);
	DDV_MaxChars(pDX, m_StrAmtLow3, 12);
	DDX_Text(pDX, IDC_AMOUNT3_OTHER, m_StrAmtOther3);
	DDV_MaxChars(pDX, m_StrAmtOther3, 12);
	DDX_Text(pDX, IDC_AMOUNT3_TEXT, m_StrAmtText3);
	DDV_MaxChars(pDX, m_StrAmtText3, 40);
	DDX_Text(pDX, IDC_AMOUNT4_HIGH, m_StrAmtHigh4);
	DDV_MaxChars(pDX, m_StrAmtHigh4, 12);
	DDX_Text(pDX, IDC_AMOUNT4_LOW, m_StrAmtLow4);
	DDV_MaxChars(pDX, m_StrAmtLow4, 12);
	DDX_Text(pDX, IDC_AMOUNT4_OTHER, m_StrAmtOther4);
	DDV_MaxChars(pDX, m_StrAmtOther4, 12);
	DDX_Text(pDX, IDC_AMOUNT4_TEXT, m_StrAmtText4);
	DDV_MaxChars(pDX, m_StrAmtText4, 40);
	DDX_Text(pDX, IDC_AMOUNT5_HIGH, m_StrAmtHigh5);
	DDV_MaxChars(pDX, m_StrAmtHigh5, 12);
	DDX_Text(pDX, IDC_AMOUNT5_OTHER, m_StrAmtOther5);
	DDV_MaxChars(pDX, m_StrAmtOther5, 12);
	DDX_Text(pDX, IDC_AMOUNT5_LOW, m_StrAmtLow5);
	DDV_MaxChars(pDX, m_StrAmtLow5, 12);
	DDX_Text(pDX, IDC_AMOUNT5_TEXT, m_StrAmtText5);
	DDV_MaxChars(pDX, m_StrAmtText5, 40);
	DDX_Text(pDX, IDC_CB_ID_FLD, m_StrCashBonusId);
	DDV_MaxChars(pDX, m_StrCashBonusId, 20);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Radio(pDX, IDC_WIN_OTHER1_RADIO, m_IntWinOther1Radio);
	DDX_Radio(pDX, IDC_WIN_OTHER2_RADIO, m_IntWinOther2Radio);
	DDX_Radio(pDX, IDC_WIN_OTHER3_RADIO, m_IntWinOther3Radio);
	DDX_Radio(pDX, IDC_WIN_OTHER4_RADIO, m_IntWinOther4Radio);
	DDX_Radio(pDX, IDC_WIN_OTHER5_RADIO, m_IntWinOther5Radio);
	DDX_Radio(pDX, IDC_WIN_PURCHASE1_RADIO, m_IntPurchase1Radio);
	DDX_Radio(pDX, IDC_WIN_PURCHASE2_RADIO, m_IntPurchase2Radio);
	DDX_Radio(pDX, IDC_WIN_PURCHASE3_RADIO, m_IntPurchase3Radio);
	DDX_Radio(pDX, IDC_WIN_PURCHASE4_RADIO, m_IntPurchase4Radio);
	DDX_Radio(pDX, IDC_WIN_PURCHASE5_RADIO, m_IntPurchase5Radio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCB_Amounts, CPropertyPage)
	//{{AFX_MSG_MAP(CCB_Amounts)
	ON_BN_CLICKED(IDC_WIN_OTHER1_RADIO, OnWinOther1Radio)
	ON_BN_CLICKED(IDC_WIN_OTHER2_RADIO, OnWinOther2Radio)
	ON_BN_CLICKED(IDC_WIN_OTHER3_RADIO, OnWinOther3Radio)
	ON_BN_CLICKED(IDC_WIN_OTHER4_RADIO, OnWinOther4Radio)
	ON_BN_CLICKED(IDC_WIN_OTHER5_RADIO, OnWinOther5Radio)
	ON_BN_CLICKED(IDC_WIN_PURCHASE1_RADIO, OnWinPurchase1Radio)
	ON_BN_CLICKED(IDC_WIN_PURCHASE2_RADIO, OnWinPurchase2Radio)
	ON_BN_CLICKED(IDC_WIN_PURCHASE3_RADIO, OnWinPurchase3Radio)
	ON_BN_CLICKED(IDC_WIN_PURCHASE4_RADIO, OnWinPurchase4Radio)
	ON_BN_CLICKED(IDC_WIN_PURCHASE5_RADIO, OnWinPurchase5Radio)
	ON_EN_KILLFOCUS(IDC_AMOUNT1_LOW, OnKillfocusAmount1Low)
	ON_EN_KILLFOCUS(IDC_AMOUNT1_HIGH, OnKillfocusAmount1High)
	ON_EN_KILLFOCUS(IDC_AMOUNT1_OTHER, OnKillfocusAmount1Other)
	ON_EN_KILLFOCUS(IDC_AMOUNT2_HIGH, OnKillfocusAmount2High)
	ON_EN_KILLFOCUS(IDC_AMOUNT2_LOW, OnKillfocusAmount2Low)
	ON_EN_KILLFOCUS(IDC_AMOUNT2_OTHER, OnKillfocusAmount2Other)
	ON_EN_KILLFOCUS(IDC_AMOUNT3_HIGH, OnKillfocusAmount3High)
	ON_EN_KILLFOCUS(IDC_AMOUNT3_LOW, OnKillfocusAmount3Low)
	ON_EN_KILLFOCUS(IDC_AMOUNT3_OTHER, OnKillfocusAmount3Other)
	ON_EN_KILLFOCUS(IDC_AMOUNT4_HIGH, OnKillfocusAmount4High)
	ON_EN_KILLFOCUS(IDC_AMOUNT4_LOW, OnKillfocusAmount4Low)
	ON_EN_KILLFOCUS(IDC_AMOUNT4_OTHER, OnKillfocusAmount4Other)
	ON_EN_KILLFOCUS(IDC_AMOUNT5_HIGH, OnKillfocusAmount5High)
	ON_EN_KILLFOCUS(IDC_AMOUNT5_LOW, OnKillfocusAmount5Low)
	ON_EN_KILLFOCUS(IDC_AMOUNT5_OTHER, OnKillfocusAmount5Other)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCB_Amounts message handlers


/******************************************************************************************/

void CCB_Amounts::CleanAmountsDialog()
{
   m_StrAmtLow1     = _T("");
   m_StrAmtHigh1    = _T("");
   m_StrAmtOther1   = _T("");
   m_StrAmtText1    = _T("");
   m_StrAmtLow2     = _T("");
   m_StrAmtHigh2    = _T("");
   m_StrAmtOther2   = _T("");
   m_StrAmtText2    = _T("");
   m_StrAmtLow3     = _T("");
   m_StrAmtHigh3    = _T("");
   m_StrAmtOther3   = _T("");
   m_StrAmtText3    = _T("");
   m_StrAmtLow4     = _T("");
   m_StrAmtHigh4    = _T("");
   m_StrAmtOther4   = _T("");
   m_StrAmtText4    = _T("");
   m_StrAmtLow5     = _T("");
   m_StrAmtHigh5    = _T("");
   m_StrAmtOther5   = _T("");
   m_StrAmtText5    = _T("");
   m_StrMessageLbl  = _T("");

   UpdateData(FALSE);
}

/******************************************************************************************/
/******************************************************************************************/

void CCB_Amounts::EnableAmountsDialog(int nStatus)
{
   m_CntlCashBonusId.EnableWindow(FALSE);
   m_CntlAmtLow1.EnableWindow(nStatus);
   m_CntlAmtHigh1.EnableWindow(nStatus);
   m_CntlAmtOther1.EnableWindow(nStatus);
   m_CntlAmtText1.EnableWindow(nStatus);
   m_CntlAmtLow2.EnableWindow(nStatus);
   m_CntlAmtHigh2.EnableWindow(nStatus);
   m_CntlAmtOther2.EnableWindow(nStatus);
   m_CntlAmtText2.EnableWindow(nStatus);
   m_CntlAmtLow3.EnableWindow(nStatus);
   m_CntlAmtHigh3.EnableWindow(nStatus);
   m_CntlAmtOther3.EnableWindow(nStatus);
   m_CntlAmtText3.EnableWindow(nStatus);
   m_CntlAmtLow4.EnableWindow(nStatus);
   m_CntlAmtHigh4.EnableWindow(nStatus);
   m_CntlAmtOther4.EnableWindow(nStatus);
   m_CntlAmtText4.EnableWindow(nStatus);
   m_CntlAmtLow5.EnableWindow(nStatus);
   m_CntlAmtHigh5.EnableWindow(nStatus);
   m_CntlAmtOther5.EnableWindow(nStatus);
   m_CntlAmtText5.EnableWindow(nStatus);
}

/******************************************************************************/
/******************************************************************************/

BOOL CCB_Amounts::PopulateBon02DialogFromDatabase( BON02 sBon02, INT radio )
{
   CHAR   amount[13];
   double d_amt;

   ZeroMemory( &sBon02Old, sizeof(BON02) );  

   m_StrCashBonusId = sBon02.cash_id;
   m_StrAmtLow1     = sBon02.amount_range[0].low_amount;
   m_StrAmtHigh1    = sBon02.amount_range[0].high_amount;
   m_StrAmtOther1   = sBon02.amount_range[0].reward_amount;
   m_StrAmtText1    = sBon02.amount_range[0].resp_text;

   m_StrAmtLow2     = sBon02.amount_range[1].low_amount;
   m_StrAmtHigh2    = sBon02.amount_range[1].high_amount;
   m_StrAmtOther2   = sBon02.amount_range[1].reward_amount;
   m_StrAmtText2    = sBon02.amount_range[1].resp_text;

   m_StrAmtLow3     = sBon02.amount_range[2].low_amount;
   m_StrAmtHigh3    = sBon02.amount_range[2].high_amount;
   m_StrAmtOther3   = sBon02.amount_range[2].reward_amount;
   m_StrAmtText3    = sBon02.amount_range[2].resp_text;

   m_StrAmtLow4     = sBon02.amount_range[3].low_amount;
   m_StrAmtHigh4    = sBon02.amount_range[3].high_amount;
   m_StrAmtOther4   = sBon02.amount_range[3].reward_amount;
   m_StrAmtText4    = sBon02.amount_range[3].resp_text;

   m_StrAmtLow5     = sBon02.amount_range[4].low_amount;
   m_StrAmtHigh5    = sBon02.amount_range[4].high_amount;
   m_StrAmtOther5   = sBon02.amount_range[4].reward_amount;
   m_StrAmtText5    = sBon02.amount_range[4].resp_text;

   memcpy( &sBon02Old, &sBon02, sizeof(BON02) );

   /*************************/
   /* Set the Radio buttons */
   /*************************/

   if ( radio == 1 )
   {
      /* Range 1 */
      if ( sBon02.amount_range[0].win_type[0] == 'O' )
      {
         m_CntlWinOther1Radio.SetCheck(1);
         m_CntlPurchase1Radio.SetCheck(0);
      }
      else
      {
         m_CntlWinOther1Radio.SetCheck(0);
         m_CntlPurchase1Radio.SetCheck(1);
      }

      /* Range 2 */
      if ( sBon02.amount_range[1].win_type[0] == 'O' )
      {
         m_CntlWinOther2Radio.SetCheck(1);
         m_CntlPurchase2Radio.SetCheck(0);
      }
      else
      {
         m_CntlWinOther2Radio.SetCheck(0);
         m_CntlPurchase2Radio.SetCheck(1);
      }

      /* Range 3 */
      if ( sBon02.amount_range[2].win_type[0] == 'O' )
      {
         m_CntlWinOther3Radio.SetCheck(1);
         m_CntlPurchase3Radio.SetCheck(0);
      }
      else
      {
         m_CntlWinOther3Radio.SetCheck(0);
         m_CntlPurchase3Radio.SetCheck(1);
      }

      /* Range 4 */
      if ( sBon02.amount_range[3].win_type[0] == 'O' )
      {
         m_CntlWinOther4Radio.SetCheck(1);
         m_CntlPurchase4Radio.SetCheck(0);
      }
      else
      {
         m_CntlWinOther4Radio.SetCheck(0);
         m_CntlPurchase4Radio.SetCheck(1);
      }

      /* Range 5 */
      if ( sBon02.amount_range[4].win_type[0] == 'O' )
      {
         m_CntlWinOther5Radio.SetCheck(1);
         m_CntlPurchase5Radio.SetCheck(0);
      }
      else
      {
         m_CntlWinOther5Radio.SetCheck(0);
         m_CntlPurchase5Radio.SetCheck(1);
      }
   }

   /*****************************/
   /* Format the amount fields. */
   /*****************************/

   /* Amount Range 1 */
   if ( !m_StrAmtLow1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow1.GetBuffer(0), m_StrAmtLow1.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtLow1 = amount;
   }

   if ( !m_StrAmtHigh1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh1.GetBuffer(0), m_StrAmtHigh1.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtHigh1 = amount;
   }

   if ( !m_StrAmtOther1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther1.GetBuffer(0), m_StrAmtOther1.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtOther1 = amount;
   }

   /* Amount Range 2 */
   if ( !m_StrAmtLow2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow2.GetBuffer(0), m_StrAmtLow2.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtLow2 = amount;
   }

   if ( !m_StrAmtHigh2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh2.GetBuffer(0), m_StrAmtHigh2.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtHigh2 = amount;
   }

   if ( !m_StrAmtOther2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther2.GetBuffer(0), m_StrAmtOther2.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtOther2 = amount;
   }

   /* Amount Range 3 */
   if ( !m_StrAmtLow3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow3.GetBuffer(0), m_StrAmtLow3.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtLow3 = amount;
   }

   if ( !m_StrAmtHigh3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh3.GetBuffer(0), m_StrAmtHigh3.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtHigh3 = amount;
   }

   if ( !m_StrAmtOther3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther3.GetBuffer(0), m_StrAmtOther3.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtOther3 = amount;
   }

   /* Amount Range 4*/
   if ( !m_StrAmtLow4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow4.GetBuffer(0), m_StrAmtLow4.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtLow4 = amount;
   }

   if ( !m_StrAmtHigh4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh4.GetBuffer(0), m_StrAmtHigh4.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtHigh4 = amount;
   }

   if ( !m_StrAmtOther4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther4.GetBuffer(0), m_StrAmtOther4.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtOther4 = amount;
   }

   /* Amount Range 5 */
   if ( !m_StrAmtLow5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow5.GetBuffer(0), m_StrAmtLow5.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtLow5 = amount;
   }

   if ( !m_StrAmtHigh5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh5.GetBuffer(0), m_StrAmtHigh5.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtHigh5 = amount;
   }
   if ( !m_StrAmtOther5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther5.GetBuffer(0), m_StrAmtOther5.GetLength() );
      d_amt = atof( amount ) / 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%.2f", d_amt );
      m_StrAmtOther5 = amount;
   }

   return TRUE;
}

/******************************************************************************/
/******************************************************************************/

BOOL CCB_Amounts::PopulateDatabaseFromBon02Dialog( pBON02 psBon02 )
{
   CHAR   amount[13];
   double d_amt;

   memcpy( psBon02->cash_id, m_StrCashBonusId, sizeof(psBon02->cash_id) );

   memcpy( psBon02->amount_range[0].low_amount,    m_StrAmtLow1,   sizeof(psBon02->amount_range[0].low_amount));
   memcpy( psBon02->amount_range[0].high_amount,   m_StrAmtHigh1,  sizeof(psBon02->amount_range[0].high_amount));
   memcpy( psBon02->amount_range[0].reward_amount, m_StrAmtOther1, sizeof(psBon02->amount_range[0].reward_amount));
   memcpy( psBon02->amount_range[0].resp_text,     m_StrAmtText1,  sizeof(psBon02->amount_range[0].resp_text));

   memcpy( psBon02->amount_range[1].low_amount,    m_StrAmtLow2,   sizeof(psBon02->amount_range[1].low_amount));
   memcpy( psBon02->amount_range[1].high_amount,   m_StrAmtHigh2,  sizeof(psBon02->amount_range[1].high_amount));
   memcpy( psBon02->amount_range[1].reward_amount, m_StrAmtOther2, sizeof(psBon02->amount_range[1].reward_amount));
   memcpy( psBon02->amount_range[1].resp_text,     m_StrAmtText2,  sizeof(psBon02->amount_range[1].resp_text));

   memcpy( psBon02->amount_range[2].low_amount,    m_StrAmtLow3,   sizeof(psBon02->amount_range[2].low_amount));
   memcpy( psBon02->amount_range[2].high_amount,   m_StrAmtHigh3,  sizeof(psBon02->amount_range[2].high_amount));
   memcpy( psBon02->amount_range[2].reward_amount, m_StrAmtOther3, sizeof(psBon02->amount_range[2].reward_amount));
   memcpy( psBon02->amount_range[2].resp_text,     m_StrAmtText3,  sizeof(psBon02->amount_range[2].resp_text));

   memcpy( psBon02->amount_range[3].low_amount,    m_StrAmtLow4,   sizeof(psBon02->amount_range[3].low_amount));
   memcpy( psBon02->amount_range[3].high_amount,   m_StrAmtHigh4,  sizeof(psBon02->amount_range[3].high_amount));
   memcpy( psBon02->amount_range[3].reward_amount, m_StrAmtOther4, sizeof(psBon02->amount_range[3].reward_amount));
   memcpy( psBon02->amount_range[3].resp_text,     m_StrAmtText4,  sizeof(psBon02->amount_range[3].resp_text));

   memcpy( psBon02->amount_range[4].low_amount,    m_StrAmtLow5,   sizeof(psBon02->amount_range[4].low_amount));
   memcpy( psBon02->amount_range[4].high_amount,   m_StrAmtHigh5,  sizeof(psBon02->amount_range[4].high_amount));
   memcpy( psBon02->amount_range[4].reward_amount, m_StrAmtOther5, sizeof(psBon02->amount_range[4].reward_amount));
   memcpy( psBon02->amount_range[4].resp_text,     m_StrAmtText5,  sizeof(psBon02->amount_range[4].resp_text));

   if ( m_bAmounts == TRUE )
   {
      if ( m_CntlPurchase1Radio.GetCheck() )
         psBon02->amount_range[0].win_type[0] = 'P';
      else
         psBon02->amount_range[0].win_type[0] = 'O';

      if ( m_CntlPurchase2Radio.GetCheck() )
         psBon02->amount_range[1].win_type[0] = 'P';
      else
         psBon02->amount_range[1].win_type[0] = 'O';

      if ( m_CntlPurchase3Radio.GetCheck() )
         psBon02->amount_range[2].win_type[0] = 'P';
      else
         psBon02->amount_range[2].win_type[0] = 'O';

      if ( m_CntlPurchase4Radio.GetCheck() )
         psBon02->amount_range[3].win_type[0] = 'P';
      else
         psBon02->amount_range[3].win_type[0] = 'O';

      if ( m_CntlPurchase5Radio.GetCheck() )
         psBon02->amount_range[4].win_type[0] = 'P';
      else
         psBon02->amount_range[4].win_type[0] = 'O';
   }

   /*****************************/
   /* Format the amount fields. */
   /*****************************/

   /* Amount Range 1 */
   if ( !m_StrAmtLow1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow1.GetBuffer(0), m_StrAmtLow1.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[0].low_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[0].low_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtHigh1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh1.GetBuffer(0), m_StrAmtHigh1.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[0].high_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[0].high_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtOther1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther1.GetBuffer(0), m_StrAmtOther1.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[0].reward_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[0].reward_amount, amount, strlen(amount) );
      psBon02->amount_range[0].win_type[0] = 'O';
   }
   else
   {
      psBon02->amount_range[0].win_type[0] = 'P';
   }

   /* Amount Range 2 */
   if ( !m_StrAmtLow2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow2.GetBuffer(0), m_StrAmtLow2.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[1].low_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[1].low_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtHigh2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh2.GetBuffer(0), m_StrAmtHigh2.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[1].high_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[1].high_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtOther2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther2.GetBuffer(0), m_StrAmtOther2.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[1].reward_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[1].reward_amount, amount, strlen(amount) );
      psBon02->amount_range[1].win_type[0] = 'O';
   }
   else
   {
      psBon02->amount_range[1].win_type[0] = 'P';
   }

   /* Amount Range 3 */
   if ( !m_StrAmtLow3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow3.GetBuffer(0), m_StrAmtLow3.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[2].low_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[2].low_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtHigh3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh3.GetBuffer(0), m_StrAmtHigh3.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[2].high_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[2].high_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtOther3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther3.GetBuffer(0), m_StrAmtOther3.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[2].reward_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[2].reward_amount, amount, strlen(amount) );
      psBon02->amount_range[2].win_type[0] = 'O';
   }
   else
   {
      psBon02->amount_range[2].win_type[0] = 'P';
   }

   /* Amount Range 4 */
   if ( !m_StrAmtLow4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow4.GetBuffer(0), m_StrAmtLow4.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[3].low_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[3].low_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtHigh4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh4.GetBuffer(0), m_StrAmtHigh4.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[3].high_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[3].high_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtOther4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther4.GetBuffer(0), m_StrAmtOther4.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[3].reward_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[3].reward_amount, amount, strlen(amount) );
      psBon02->amount_range[3].win_type[0] = 'O';
   }
   else
   {
      psBon02->amount_range[3].win_type[0] = 'P';
   }

   /* Amount Range 5 */
   if ( !m_StrAmtLow5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow5.GetBuffer(0), m_StrAmtLow5.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[4].low_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[4].low_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtHigh5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh5.GetBuffer(0), m_StrAmtHigh5.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[4].high_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[4].high_amount, amount, strlen(amount) );
   }

   if ( !m_StrAmtOther5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtOther5.GetBuffer(0), m_StrAmtOther5.GetLength() );
      d_amt = atof( amount ) * 100.0;
      memset( amount, 0x00, sizeof(amount) );
      sprintf( amount, "%012.0f", d_amt );
      memset( psBon02->amount_range[4].reward_amount, 0x00, sizeof(amount) );
      memcpy( psBon02->amount_range[4].reward_amount, amount, strlen(amount) );
      psBon02->amount_range[4].win_type[0] = 'O';
   }
   else
   {
      psBon02->amount_range[4].win_type[0] = 'P';
   }

   return TRUE;
}

/******************************************************************************/
/******************************************************************************/

BOOL CCB_Amounts::OnInitDialog() 
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

   m_bAmounts = TRUE;

   if (m_nDialogMode != MODE_DONE )
   {
      CleanAmountsDialog();

      if ( m_nDialogMode == MODE_VIEW )
         EnableAmountsDialog(FALSE);
      else
         EnableAmountsDialog(TRUE);

      if ( m_nDialogMode != MODE_ADD )
      {
         PopulateBon02DialogFromDatabase( sBon02Old, 1 );
         UpdateData(FALSE);

         if ( sBon02Old.amount_range[0].win_type[0] == 'P' )
         {
            m_CntlPurchase1Radio.SetCheck(1);
            m_CntlWinOther1Radio.SetCheck(0);
            m_CntlAmtOther1.EnableWindow(FALSE);
         }
         else
         {
            m_CntlPurchase1Radio.SetCheck(0);
            m_CntlWinOther1Radio.SetCheck(1);
            if ( m_nDialogMode != MODE_VIEW )
               m_CntlAmtOther1.EnableWindow(TRUE);
         }

         if ( sBon02Old.amount_range[1].win_type[0] == 'P' )
         {
            m_CntlPurchase2Radio.SetCheck(1);
            m_CntlWinOther2Radio.SetCheck(0);
            m_CntlAmtOther2.EnableWindow(FALSE);
         }
         else
         {
            m_CntlPurchase2Radio.SetCheck(0);
            m_CntlWinOther2Radio.SetCheck(1);
            if ( m_nDialogMode != MODE_VIEW )
               m_CntlAmtOther2.EnableWindow(TRUE);
         }

         if ( sBon02Old.amount_range[2].win_type[0] == 'P' )
         {
            m_CntlPurchase3Radio.SetCheck(1);
            m_CntlWinOther3Radio.SetCheck(0);
            m_CntlAmtOther3.EnableWindow(FALSE);
         }
         else
         {
            m_CntlPurchase3Radio.SetCheck(0);
            m_CntlWinOther3Radio.SetCheck(1);
            if ( m_nDialogMode != MODE_VIEW )
               m_CntlAmtOther3.EnableWindow(TRUE);
         }

         if ( sBon02Old.amount_range[3].win_type[0] == 'P' )
         {
            m_CntlPurchase4Radio.SetCheck(1);
            m_CntlWinOther4Radio.SetCheck(0);
            m_CntlAmtOther4.EnableWindow(FALSE);
         }
         else
         {
            m_CntlPurchase4Radio.SetCheck(0);
            m_CntlWinOther4Radio.SetCheck(1);
            if ( m_nDialogMode != MODE_VIEW )
               m_CntlAmtOther4.EnableWindow(TRUE);
         }

         if ( sBon02Old.amount_range[4].win_type[0] == 'P' )
         {
            m_CntlPurchase5Radio.SetCheck(1);
            m_CntlWinOther5Radio.SetCheck(0);
            m_CntlAmtOther5.EnableWindow(FALSE);
         }
         else
         {
            m_CntlPurchase5Radio.SetCheck(0);
            m_CntlWinOther5Radio.SetCheck(1);
            if ( m_nDialogMode != MODE_VIEW )
               m_CntlAmtOther5.EnableWindow(TRUE);
         }
      }
      else
      {
         m_CntlPurchase1Radio.SetCheck(1);
         m_CntlWinOther1Radio.SetCheck(0);
         m_CntlAmtOther1.EnableWindow(FALSE);
         m_CntlPurchase2Radio.SetCheck(1);
         m_CntlWinOther2Radio.SetCheck(0);
         m_CntlAmtOther2.EnableWindow(FALSE);
         m_CntlPurchase3Radio.SetCheck(1);
         m_CntlWinOther3Radio.SetCheck(0);
         m_CntlAmtOther3.EnableWindow(FALSE);
         m_CntlPurchase4Radio.SetCheck(1);
         m_CntlWinOther4Radio.SetCheck(0);
         m_CntlAmtOther4.EnableWindow(FALSE);
         m_CntlPurchase5Radio.SetCheck(1);
         m_CntlWinOther5Radio.SetCheck(0);
         m_CntlAmtOther5.EnableWindow(FALSE);
      }
      UpdateData(TRUE);
      UpdateData(FALSE);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CCB_Amounts::OnWinOther1Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther1Radio =  1;
      m_IntPurchase1Radio = -1;

      m_CntlWinOther1Radio.SetCheck(1);
      m_CntlPurchase1Radio.SetCheck(0);

      m_CntlAmtOther1.EnableWindow(TRUE);
   }
   else
   {
      if ( m_CntlPurchase1Radio.GetCheck() )
      {
         m_CntlWinOther1Radio.SetCheck(0);
         m_CntlPurchase1Radio.SetCheck(1);
      }
   }
}

void CCB_Amounts::OnWinOther2Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther2Radio =  1;
      m_IntPurchase2Radio = -1;

      m_CntlWinOther2Radio.SetCheck(1);
      m_CntlPurchase2Radio.SetCheck(0);

      m_CntlAmtOther2.EnableWindow(TRUE);
   }
   else
   {
      if ( m_CntlPurchase2Radio.GetCheck() )
      {
         m_CntlWinOther2Radio.SetCheck(0);
         m_CntlPurchase2Radio.SetCheck(1);
      }
   }
}

void CCB_Amounts::OnWinOther3Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther3Radio =  1;
      m_IntPurchase3Radio = -1;

      m_CntlWinOther3Radio.SetCheck(1);
      m_CntlPurchase3Radio.SetCheck(0);

      m_CntlAmtOther3.EnableWindow(TRUE);
   }
   else
   {
      if ( m_CntlPurchase3Radio.GetCheck() )
      {
         m_CntlWinOther3Radio.SetCheck(0);
         m_CntlPurchase3Radio.SetCheck(1);
      }
   }
}

void CCB_Amounts::OnWinOther4Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther4Radio =  1;
      m_IntPurchase4Radio = -1;

      m_CntlWinOther4Radio.SetCheck(1);
      m_CntlPurchase4Radio.SetCheck(0);

      m_CntlAmtOther4.EnableWindow(TRUE);
   }
   else
   {
      if ( m_CntlPurchase4Radio.GetCheck() )
      {
         m_CntlWinOther4Radio.SetCheck(0);
         m_CntlPurchase4Radio.SetCheck(1);
      }
   }
}

void CCB_Amounts::OnWinOther5Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther5Radio =  1;
      m_IntPurchase5Radio = -1;

      m_CntlWinOther5Radio.SetCheck(1);
      m_CntlPurchase5Radio.SetCheck(0);

      m_CntlAmtOther5.EnableWindow(TRUE);
   }
   else
   {
      if ( m_CntlPurchase5Radio.GetCheck() )
      {
         m_CntlWinOther5Radio.SetCheck(0);
         m_CntlPurchase5Radio.SetCheck(1);
      }
   }
}

void CCB_Amounts::OnWinPurchase1Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther1Radio = -1;
      m_IntPurchase1Radio =  1;

      m_CntlPurchase1Radio.SetCheck(1);
      m_CntlWinOther1Radio.SetCheck(0);

      m_CntlAmtOther1.EnableWindow(FALSE);
   }
   else
   {
      if ( m_CntlWinOther1Radio.GetCheck() )
      {
         m_CntlWinOther1Radio.SetCheck(1);
         m_CntlPurchase1Radio.SetCheck(0);
      }
   }
}

void CCB_Amounts::OnWinPurchase2Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther2Radio = -1;
      m_IntPurchase2Radio =  1;

      m_CntlPurchase2Radio.SetCheck(1);
      m_CntlWinOther2Radio.SetCheck(0);

      m_CntlAmtOther2.EnableWindow(FALSE);
   }
   else
   {
      if ( m_CntlWinOther2Radio.GetCheck() )
      {
         m_CntlWinOther2Radio.SetCheck(1);
         m_CntlPurchase2Radio.SetCheck(0);
      }
   }
}

void CCB_Amounts::OnWinPurchase3Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther3Radio = -1;
      m_IntPurchase3Radio =  1;

      m_CntlPurchase3Radio.SetCheck(1);
      m_CntlWinOther3Radio.SetCheck(0);

      m_CntlAmtOther3.EnableWindow(FALSE);
   }
   else
   {
      if ( m_CntlWinOther3Radio.GetCheck() )
      {
         m_CntlWinOther3Radio.SetCheck(1);
         m_CntlPurchase3Radio.SetCheck(0);
      }
   }
}

void CCB_Amounts::OnWinPurchase4Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther4Radio = -1;
      m_IntPurchase4Radio =  1;

      m_CntlPurchase4Radio.SetCheck(1);
      m_CntlWinOther4Radio.SetCheck(0);

      m_CntlAmtOther4.EnableWindow(FALSE);
   }
   else
   {
      if ( m_CntlWinOther4Radio.GetCheck() )
      {
         m_CntlWinOther4Radio.SetCheck(1);
         m_CntlPurchase4Radio.SetCheck(0);
      }
   }
}

void CCB_Amounts::OnWinPurchase5Radio() 
{
   if ( m_nDialogMode != MODE_VIEW )
   {
      m_IntWinOther5Radio = -1;
      m_IntPurchase5Radio =  1;

      m_CntlPurchase5Radio.SetCheck(1);
      m_CntlWinOther5Radio.SetCheck(0);

      m_CntlAmtOther5.EnableWindow(FALSE);
   }
   else
   {
      if ( m_CntlWinOther5Radio.GetCheck() )
      {
         m_CntlWinOther5Radio.SetCheck(1);
         m_CntlPurchase5Radio.SetCheck(0);
      }
   }
}

BOOL CCB_Amounts::CompareStructures()
{
   if ( m_bAmounts == TRUE )
   {
      /*+----------------+
        | Amount Range 1 |
        +----------------+*/

      if ( 0 != strcmp(m_StrAmtLow1, (char *)sBon02Old.amount_range[0].low_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtLow1,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].low_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 Low Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtHigh1, (char *)sBon02Old.amount_range[0].high_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtHigh1,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].high_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 High Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtOther1, (char *)sBon02Old.amount_range[0].reward_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtOther1,                          sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].reward_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 Reward Amount",           sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtText1, (char *)sBon02Old.amount_range[0].resp_text) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtText1,                       sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].resp_text, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 Response Text",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( m_CntlPurchase1Radio.GetCheck() && (sBon02Old.amount_range[0].win_type[0] == 'O') )
      {
         memcpy( m_StrCurrentData,  "P", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
      else if ( m_CntlPurchase1Radio.GetCheck() && (sBon02Old.amount_range[0].win_type[0] == 'P') )
      {
         memcpy( m_StrCurrentData,  "O", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[0].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 1 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      /*+----------------+
        | Amount Range 2 |
        +----------------+*/

      if ( 0 != strcmp(m_StrAmtLow2, (char *)sBon02Old.amount_range[1].low_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtLow2,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].low_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 Low Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtHigh2, (char *)sBon02Old.amount_range[1].high_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtHigh2,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].high_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 High Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtOther2, (char *)sBon02Old.amount_range[1].reward_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtOther2,                          sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].reward_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 Reward Amount",           sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtText2, (char *)sBon02Old.amount_range[1].resp_text) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtText2,                       sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].resp_text, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 Response Text",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( m_CntlPurchase2Radio.GetCheck() && (sBon02Old.amount_range[1].win_type[0] == 'O') )
      {
         memcpy( m_StrCurrentData,  "P", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
      else if ( m_CntlPurchase2Radio.GetCheck() && (sBon02Old.amount_range[1].win_type[0] == 'P') )
      {
         memcpy( m_StrCurrentData,  "O", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[1].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 2 Radio Buttons",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      /*+----------------+
        | Amount Range 3 |
        +----------------+*/

      if ( 0 != strcmp(m_StrAmtLow3, (char *)sBon02Old.amount_range[2].low_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtLow3,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].low_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 Low Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtHigh3, (char *)sBon02Old.amount_range[2].high_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtHigh3,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].high_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 High Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtOther3, (char *)sBon02Old.amount_range[2].reward_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtOther3,                          sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].reward_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 Reward Amount",           sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtText3, (char *)sBon02Old.amount_range[2].resp_text) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtText3,                       sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].resp_text, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 Response Text",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( m_CntlPurchase3Radio.GetCheck() && (sBon02Old.amount_range[2].win_type[0] == 'O') )
      {
         memcpy( m_StrCurrentData,  "P", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
      else if ( m_CntlPurchase3Radio.GetCheck() && (sBon02Old.amount_range[2].win_type[0] == 'P') )
      {
         memcpy( m_StrCurrentData,  "O", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[2].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 3 Radio Buttons",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      /*+----------------+
        | Amount Range 4 |
        +----------------+*/

      if ( 0 != strcmp(m_StrAmtLow4, (char *)sBon02Old.amount_range[3].low_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtLow4,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].low_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 Low Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtHigh4, (char *)sBon02Old.amount_range[3].high_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtHigh4,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].high_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 High Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtOther4, (char *)sBon02Old.amount_range[3].reward_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtOther4,                          sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].reward_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 Reward Amount",           sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtText4, (char *)sBon02Old.amount_range[3].resp_text) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtText4,                       sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].resp_text, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 Response Text",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( m_CntlPurchase4Radio.GetCheck() && (sBon02Old.amount_range[3].win_type[0] == 'O') )
      {
         memcpy( m_StrCurrentData,  "P", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
      else if ( m_CntlPurchase4Radio.GetCheck() && (sBon02Old.amount_range[3].win_type[0] == 'P') )
      {
         memcpy( m_StrCurrentData,  "O", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[3].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 4 Radio Buttons",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      /*+----------------+
        | Amount Range 5 |
        +----------------+*/

      if ( 0 != strcmp(m_StrAmtLow5, (char *)sBon02Old.amount_range[4].low_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtLow5,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].low_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 Low Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtHigh5, (char *)sBon02Old.amount_range[4].high_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtHigh5,                         sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].high_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 High Range",            sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtOther5, (char *)sBon02Old.amount_range[4].reward_amount) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtOther5,                          sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].reward_amount, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 Reward Amount",           sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( 0 != strcmp(m_StrAmtText5, (char *)sBon02Old.amount_range[4].resp_text) )
      {
         memcpy( m_StrCurrentData,  m_StrAmtText5,                       sizeof(m_StrCurrentData));
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].resp_text, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 Response Text",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }

      if ( m_CntlPurchase5Radio.GetCheck() && (sBon02Old.amount_range[4].win_type[0] == 'O') )
      {
         memcpy( m_StrCurrentData,  "P", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 Radio Buttons",      sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
      else if ( m_CntlPurchase5Radio.GetCheck() && (sBon02Old.amount_range[4].win_type[0] == 'P') )
      {
         memcpy( m_StrCurrentData,  "O", 1 );
         memcpy( m_StrPreviousData, sBon02Old.amount_range[4].win_type, sizeof(m_StrPreviousData));
         memcpy( m_StrFieldTag,    "Amount Range 5 Radio Buttons",       sizeof(m_StrFieldTag));
         PopulateHistoryTable();
      }
   }
   return TRUE;
}

/******************************************************************************/
/******************************************************************************/

void CCB_Amounts::PopulateHistoryTable()
{
   HST01    sHistoryData;
   CHAR     dest_str[18];   
   CMyUtils myUtilsObject;
   CHAR     strErrorMsg [200] = "";
   CString  temp;

   CHAR strDate[9] = "";
   CHAR strTime[7] = "";
   CHAR strPrimaryKey[50] = "";
   CHAR wstation[21]; DWORD wsize;

   memset( strDate,       0x00, sizeof (strDate) );
   memset( strTime,       0x00, sizeof (strTime) );
   memset( strPrimaryKey, 0x00, sizeof (strPrimaryKey));

   strcpy( strPrimaryKey, m_StrCashBonusId );

   myUtilsObject.GetSystemDate(strDate); 
   myUtilsObject.GetSystemTime(strTime);

   pintime_get_gmt ( dest_str );

   ZeroMemory ( &sHistoryData, sizeof (HST01));
   memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"BON02", sizeof (sHistoryData.file_name));
   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

   memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
   memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str,      sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified)); 
   memcpy ((LPTSTR) sHistoryData.field_tag,         (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
   memcpy ((LPTSTR) sHistoryData.current_data,      (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
   memcpy ((LPTSTR) sHistoryData.changed_by,        (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));

   int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
   if ( rcode != PTEMSG_OK )
   {
      m_StrMessageLbl = strErrorMsg;
      m_nDialogMode = MODE_DONE;
      UpdateData( FALSE );
   }
}


BOOL CCB_Amounts::OnKillActive() 
{
   #define  LOW_AMT  0
   #define  HIGH_AMT 1

   CHAR   amount[13];
   double l_amt;
   double h_amt;

   INT    i,j;
   INT    overlap;
   double range[5][2];
   CHAR   buffer[100];

   if ( m_nDialogMode == MODE_DONE )
      return( TRUE );

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   /*******************************/
   /* Verify ranges are complete. */
   /*******************************/

   /* Range 1 */
   if ( !m_StrAmtLow1.IsEmpty() && m_StrAmtHigh1.IsEmpty() )
   {
      m_CntlAmtHigh1.SetFocus();
      m_StrMessageLbl = "Missing HIGH amount for Range 1";
      UpdateData(FALSE);
      return( FALSE );
   }
   else if ( m_StrAmtLow1.IsEmpty() && !m_StrAmtHigh1.IsEmpty() )
   {
      m_CntlAmtLow1.SetFocus();
      m_StrMessageLbl = "Missing LOW amount for Range 1";
      UpdateData(FALSE);
      return( FALSE );
   }

   /* Range 2 */
   if ( !m_StrAmtLow2.IsEmpty() && m_StrAmtHigh2.IsEmpty() )
   {
      m_CntlAmtHigh2.SetFocus();
      m_StrMessageLbl = "Missing HIGH amount for Range 2";
      UpdateData(FALSE);
      return( FALSE );
   }
   else if ( m_StrAmtLow2.IsEmpty() && !m_StrAmtHigh2.IsEmpty() )
   {
      m_CntlAmtLow2.SetFocus();
      m_StrMessageLbl = "Missing LOW amount for Range 2";
      UpdateData(FALSE);
      return( FALSE );
   }

   /* Range 3 */
   if ( !m_StrAmtLow3.IsEmpty() && m_StrAmtHigh3.IsEmpty() )
   {
      m_CntlAmtHigh3.SetFocus();
      m_StrMessageLbl = "Missing HIGH amount for Range 3";
      UpdateData(FALSE);
      return( FALSE );
   }
   else if ( m_StrAmtLow3.IsEmpty() && !m_StrAmtHigh3.IsEmpty() )
   {
      m_CntlAmtLow3.SetFocus();
      m_StrMessageLbl = "Missing LOW amount for Range 3";
      UpdateData(FALSE);
      return( FALSE );
   }

   /* Range 4 */
   if ( !m_StrAmtLow4.IsEmpty() && m_StrAmtHigh4.IsEmpty() )
   {
      m_CntlAmtHigh4.SetFocus();
      m_StrMessageLbl = "Missing HIGH amount for Range 4";
      UpdateData(FALSE);
      return( FALSE );
   }
   else if ( m_StrAmtLow4.IsEmpty() && !m_StrAmtHigh4.IsEmpty() )
   {
      m_CntlAmtLow4.SetFocus();
      m_StrMessageLbl = "Missing LOW amount for Range 4";
      UpdateData(FALSE);
      return( FALSE );
   }

   /* Range 5 */
   if ( !m_StrAmtLow5.IsEmpty() && m_StrAmtHigh5.IsEmpty() )
   {
      m_CntlAmtHigh5.SetFocus();
      m_StrMessageLbl = "Missing HIGH amount for Range 5";
      UpdateData(FALSE);
      return( FALSE );
   }
   else if ( m_StrAmtLow5.IsEmpty() && !m_StrAmtHigh5.IsEmpty() )
   {
      m_CntlAmtLow5.SetFocus();
      m_StrMessageLbl = "Missing LOW amount for Range 5";
      UpdateData(FALSE);
      return( FALSE );
   }


   /******************************************************/
   /* Verify High amounts are not less than Low amounts. */
   /******************************************************/

   /* Range 1 */
   if ( !m_StrAmtLow1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow1.GetBuffer(0), m_StrAmtLow1.GetLength() );
      l_amt = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh1.GetBuffer(0), m_StrAmtHigh1.GetLength() );
      h_amt = atof( amount );

      if ( h_amt < l_amt )
      {
         m_CntlAmtHigh1.SetFocus();
         m_StrMessageLbl = "Amount Range 1: High must be not be less than low.";
         UpdateData(FALSE);
         return( FALSE );
      }
   }

   /* Range 2 */
   if ( !m_StrAmtLow2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow2.GetBuffer(0), m_StrAmtLow2.GetLength() );
      l_amt = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh2.GetBuffer(0), m_StrAmtHigh2.GetLength() );
      h_amt = atof( amount );

      if ( h_amt < l_amt )
      {
         m_CntlAmtHigh2.SetFocus();
         m_StrMessageLbl = "Amount Range 2: High must be not be less than low.";
         UpdateData(FALSE);
         return( FALSE );
      }
   }

   /* Range 3 */
   if ( !m_StrAmtLow3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow3.GetBuffer(0), m_StrAmtLow3.GetLength() );
      l_amt = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh3.GetBuffer(0), m_StrAmtHigh3.GetLength() );
      h_amt = atof( amount );

      if ( h_amt < l_amt )
      {
         m_CntlAmtHigh3.SetFocus();
         m_StrMessageLbl = "Amount Range 3: High must be not be less than low.";
         UpdateData(FALSE);
         return( FALSE );
      }
   }

   /* Range 4 */
   if ( !m_StrAmtLow4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow4.GetBuffer(0), m_StrAmtLow4.GetLength() );
      l_amt = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh4.GetBuffer(0), m_StrAmtHigh4.GetLength() );
      h_amt = atof( amount );

      if ( h_amt < l_amt )
      {
         m_CntlAmtHigh4.SetFocus();
         m_StrMessageLbl = "Amount Range 4: High must be not be less than low.";
         UpdateData(FALSE);
         return( FALSE );
      }
   }

   /* Range 5 */
   if ( !m_StrAmtLow5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow5.GetBuffer(0), m_StrAmtLow5.GetLength() );
      l_amt = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh5.GetBuffer(0), m_StrAmtHigh5.GetLength() );
      h_amt = atof( amount );

      if ( h_amt < l_amt )
      {
         m_CntlAmtHigh5.SetFocus();
         m_StrMessageLbl = "Amount Range 5: High must be not be less than low.";
         UpdateData(FALSE);
         return( FALSE );
      }
   }

   /* Check for empty 'Reward Amount' field
    * if 'Purchase' radio is not selected.
    */
   if ( m_CntlWinOther1Radio.GetCheck() && m_StrAmtOther1.IsEmpty() )
   {
      m_CntlAmtOther1.SetFocus();
      m_StrMessageLbl = "Amount Range 1: Must enter a Reward Amount when checked.";
      UpdateData(FALSE);
      return( FALSE );
   }

   if ( m_CntlWinOther2Radio.GetCheck() && m_StrAmtOther2.IsEmpty() )
   {
      m_CntlAmtOther2.SetFocus();
      m_StrMessageLbl = "Amount Range 2: Must enter a Reward Amount when checked.";
      UpdateData(FALSE);
      return( FALSE );
   }

   if ( m_CntlWinOther3Radio.GetCheck() && m_StrAmtOther3.IsEmpty() )
   {
      m_CntlAmtOther3.SetFocus();
      m_StrMessageLbl = "Amount Range 3: Must enter a Reward Amount when checked.";
      UpdateData(FALSE);
      return( FALSE );
   }

   if ( m_CntlWinOther4Radio.GetCheck() && m_StrAmtOther4.IsEmpty() )
   {
      m_CntlAmtOther4.SetFocus();
      m_StrMessageLbl = "Amount Range 4: Must enter a Reward Amount when checked.";
      UpdateData(FALSE);
      return( FALSE );
   }

   if ( m_CntlWinOther5Radio.GetCheck() && m_StrAmtOther5.IsEmpty() )
   {
      m_CntlAmtOther5.SetFocus();
      m_StrMessageLbl = "Amount Range 5: Must enter a Reward Amount when checked.";
      UpdateData(FALSE);
      return( FALSE );
   }


   /***************************************/
   /* Check for overlapping amount ranges */
   /***************************************/

   /* Initialize */
   for( i=0; i<5; i++ )
   {
      range[i][LOW_AMT]  = 0.0;
      range[i][HIGH_AMT] = 0.0;
   }

   /* Get Range 1 Values */
   if ( !m_StrAmtLow1.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow1.GetBuffer(0), m_StrAmtLow1.GetLength() );
      range[0][LOW_AMT] = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh1.GetBuffer(0), m_StrAmtHigh1.GetLength() );
      range[0][HIGH_AMT] = atof( amount );
   }
   else
   {
      range[0][LOW_AMT] = -1.0;
   }

   /* Get Range 2 Values */
   if ( !m_StrAmtLow2.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow2.GetBuffer(0), m_StrAmtLow2.GetLength() );
      range[1][LOW_AMT] = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh2.GetBuffer(0), m_StrAmtHigh2.GetLength() );
      range[1][HIGH_AMT] = atof( amount );
   }
   else
   {
      range[1][LOW_AMT] = -1.0;
   }

   /* Get Range 3 Values */
   if ( !m_StrAmtLow3.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow3.GetBuffer(0), m_StrAmtLow3.GetLength() );
      range[2][LOW_AMT] = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh3.GetBuffer(0), m_StrAmtHigh3.GetLength() );
      range[2][HIGH_AMT] = atof( amount );
   }
   else
   {
      range[2][LOW_AMT] = -1.0;
   }

   /* Get Range 4 Values */
   if ( !m_StrAmtLow4.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow4.GetBuffer(0), m_StrAmtLow4.GetLength() );
      range[3][LOW_AMT] = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh4.GetBuffer(0), m_StrAmtHigh4.GetLength() );
      range[3][HIGH_AMT] = atof( amount );
   }
   else
   {
      range[3][LOW_AMT] = -1.0;
   }

   /* Get Range 5 Values */
   if ( !m_StrAmtLow5.IsEmpty() )
   {
      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtLow5.GetBuffer(0), m_StrAmtLow5.GetLength() );
      range[4][LOW_AMT] = atof( amount );

      memset( amount, 0x00, sizeof(amount) );
      memcpy( amount, m_StrAmtHigh5.GetBuffer(0), m_StrAmtHigh5.GetLength() );
      range[4][HIGH_AMT] = atof( amount );
   }
   else
   {
      range[4][LOW_AMT] = -1.0;
   }

   /* Check for overlapping ranges. */
   for( i=0; i<4; i++ )
   {
      overlap = false;
      if ( range[i][LOW_AMT] >= 0.0 )
      {
         for( j=i+1; j<5; j++ )
         {
            if ( range[j][LOW_AMT] >= 0.0 )
            {
               if ( (range[i][HIGH_AMT] < range[j][LOW_AMT] ) ||
                    (range[i][LOW_AMT]  > range[j][HIGH_AMT])  )
               {
                  overlap = false;
               }
               else
               {
                  overlap = true;
                  break;
               }
            }
         }
      }

      if ( overlap == true )
      {
         switch( j )
         {
            case 1:  m_CntlAmtHigh2.SetFocus(); break;
            case 2:  m_CntlAmtHigh3.SetFocus(); break;
            case 3:  m_CntlAmtHigh4.SetFocus(); break;
            case 4:  m_CntlAmtHigh5.SetFocus(); break;
         }
         memset( buffer, 0x00, sizeof(buffer) );
         sprintf( buffer,
                 "Amount range %d overlaps with amount range %d",
                  i+1, j+1 );
         m_StrMessageLbl = buffer;
         UpdateData(FALSE);
         return( FALSE );
      }
   }

	return CPropertyPage::OnKillActive();
}

void CCB_Amounts::OnKillfocusAmount1Low() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtLow1.IsEmpty() )
   {
      length = m_StrAmtLow1.GetLength();

      res = strspn ( m_StrAmtLow1, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtLow1.SetFocus();
         m_StrMessageLbl = "Range 1 Low Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtLow1.GetBuffer(0), m_StrAmtLow1.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtLow1 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount1High() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtHigh1.IsEmpty() )
   {
      length = m_StrAmtHigh1.GetLength();

      res = strspn ( m_StrAmtHigh1, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtHigh1.SetFocus();
         m_StrMessageLbl = "Range 1 High Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtHigh1.GetBuffer(0), m_StrAmtHigh1.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtHigh1 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount1Other() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtOther1.IsEmpty() )
   {
      length = m_StrAmtOther1.GetLength();

      res = strspn ( m_StrAmtOther1, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtOther1.SetFocus();
         m_StrMessageLbl = "Range 1 Reward Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtOther1.GetBuffer(0), m_StrAmtOther1.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtOther1 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount2High() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtHigh2.IsEmpty() )
   {
      length = m_StrAmtHigh2.GetLength();

      res = strspn ( m_StrAmtHigh2, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtHigh2.SetFocus();
         m_StrMessageLbl = "Range 2 High Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtHigh2.GetBuffer(0), m_StrAmtHigh2.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtHigh2 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount2Low() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtLow2.IsEmpty() )
   {
      length = m_StrAmtLow2.GetLength();

      res = strspn ( m_StrAmtLow2, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtLow2.SetFocus();
         m_StrMessageLbl = "Range 2 Low Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtLow2.GetBuffer(0), m_StrAmtLow2.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtLow2 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount2Other() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtOther2.IsEmpty() )
   {
      length = m_StrAmtOther2.GetLength();

      res = strspn ( m_StrAmtOther2, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtOther2.SetFocus();
         m_StrMessageLbl = "Range 2 Reward Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtOther2.GetBuffer(0), m_StrAmtOther2.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtOther2 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount3High() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtHigh3.IsEmpty() )
   {
      length = m_StrAmtHigh3.GetLength();

      res = strspn ( m_StrAmtHigh3, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtHigh3.SetFocus();
         m_StrMessageLbl = "Range 3 High Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtHigh3.GetBuffer(0), m_StrAmtHigh3.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtHigh3 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount3Low() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtLow3.IsEmpty() )
   {
      length = m_StrAmtLow3.GetLength();

      res = strspn ( m_StrAmtLow3, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtLow3.SetFocus();
         m_StrMessageLbl = "Range 3 Low Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtLow3.GetBuffer(0), m_StrAmtLow3.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtLow3 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount3Other() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtOther3.IsEmpty() )
   {
      length = m_StrAmtOther3.GetLength();

      res = strspn ( m_StrAmtOther3, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtOther3.SetFocus();
         m_StrMessageLbl = "Range 3 Reward Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtOther3.GetBuffer(0), m_StrAmtOther3.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtOther3 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount4High() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtHigh4.IsEmpty() )
   {
      length = m_StrAmtHigh4.GetLength();

      res = strspn ( m_StrAmtHigh4, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtHigh4.SetFocus();
         m_StrMessageLbl = "Range 4 High Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtHigh4.GetBuffer(0), m_StrAmtHigh4.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtHigh4 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount4Low() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtLow4.IsEmpty() )
   {
      length = m_StrAmtLow4.GetLength();

      res = strspn ( m_StrAmtLow4, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtLow4.SetFocus();
         m_StrMessageLbl = "Range 4 Low Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtLow4.GetBuffer(0), m_StrAmtLow4.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtLow4 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount4Other() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtOther4.IsEmpty() )
   {
      length = m_StrAmtOther4.GetLength();

      res = strspn ( m_StrAmtOther4, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtOther4.SetFocus();
         m_StrMessageLbl = "Range 4 Reward Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtOther4.GetBuffer(0), m_StrAmtOther4.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtOther4 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount5High() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtHigh5.IsEmpty() )
   {
      length = m_StrAmtHigh5.GetLength();

      res = strspn ( m_StrAmtHigh5, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtHigh5.SetFocus();
         m_StrMessageLbl = "Range 5 High Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtHigh5.GetBuffer(0), m_StrAmtHigh5.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtHigh5 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount5Low() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtLow5.IsEmpty() )
   {
      length = m_StrAmtLow5.GetLength();

      res = strspn ( m_StrAmtLow5, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtLow5.SetFocus();
         m_StrMessageLbl = "Range 5 Low Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtLow5.GetBuffer(0), m_StrAmtLow5.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtLow5 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_Amounts::OnKillfocusAmount5Other() 
{
   INT    res;
   INT    length;
   CHAR   amount[13];
   double d_amt;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrAmtOther5.IsEmpty() )
   {
      length = m_StrAmtOther5.GetLength();

      res = strspn ( m_StrAmtOther5, "1234567890." );
      if ( res < length )
      {
         m_CntlAmtOther5.SetFocus();
         m_StrMessageLbl = "Range 5 Reward Amount: Invalid Character - Must be numeric.";
      }
      else
      {
         memset( amount, 0x00, sizeof(amount) );
         memcpy( amount, m_StrAmtOther5.GetBuffer(0), m_StrAmtOther5.GetLength() );
         d_amt = atof( amount );

         memset( amount, 0x00, sizeof(amount) );
         sprintf( amount, "%.2f", d_amt );
         m_StrAmtOther5 = amount;
      }

      UpdateData(FALSE);
   }
   return;
}

//
//  This function is called when Amounts is made the active page.
//
BOOL CCB_Amounts::OnSetActive() 
{
   CString cash_id;

   if ( m_nDialogMode == MODE_ADD )
   {
      memset( &sBon02Old, 0x00, sizeof(BON02) );

      m_CntlPurchase1Radio.SetCheck(1);
      m_CntlWinOther1Radio.SetCheck(0);
      m_CntlAmtOther1.EnableWindow(FALSE);
      m_CntlPurchase2Radio.SetCheck(1);
      m_CntlWinOther2Radio.SetCheck(0);
      m_CntlAmtOther2.EnableWindow(FALSE);
      m_CntlPurchase3Radio.SetCheck(1);
      m_CntlWinOther3Radio.SetCheck(0);
      m_CntlAmtOther3.EnableWindow(FALSE);
      m_CntlPurchase4Radio.SetCheck(1);
      m_CntlWinOther4Radio.SetCheck(0);
      m_CntlAmtOther4.EnableWindow(FALSE);
      m_CntlPurchase5Radio.SetCheck(1);
      m_CntlWinOther5Radio.SetCheck(0);
      m_CntlAmtOther5.EnableWindow(FALSE);

      /* If I don't do this, the Cash ID on the Amount
       * window might be that of the previous Cash ID.
       */
      cash_id = m_StrCashBonusId;

      UpdateData(TRUE);

      m_StrCashBonusId = cash_id;
      UpdateData(FALSE);
   }
   else
   {
      PopulateBon02DialogFromDatabase( sBon02Old, 1 );
      UpdateData(TRUE);

      PopulateBon02DialogFromDatabase( sBon02Old, 1 );

      if ( m_CntlPurchase1Radio.GetCheck() )
      {
         m_CntlPurchase1Radio.SetCheck(1);
         m_CntlWinOther1Radio.SetCheck(0);
         m_CntlAmtOther1.EnableWindow(FALSE);
      }
      else
      {
         m_CntlPurchase1Radio.SetCheck(0);
         m_CntlWinOther1Radio.SetCheck(1);
         m_CntlAmtOther1.EnableWindow(TRUE);
      }

      if ( m_CntlPurchase2Radio.GetCheck() )
      {
         m_CntlPurchase2Radio.SetCheck(1);
         m_CntlWinOther2Radio.SetCheck(0);
         m_CntlAmtOther2.EnableWindow(FALSE);
      }
      else
      {
         m_CntlPurchase2Radio.SetCheck(0);
         m_CntlWinOther2Radio.SetCheck(1);
         m_CntlAmtOther2.EnableWindow(TRUE);
      }

      if ( m_CntlPurchase3Radio.GetCheck() )
      {
         m_CntlPurchase3Radio.SetCheck(1);
         m_CntlWinOther3Radio.SetCheck(0);
         m_CntlAmtOther3.EnableWindow(FALSE);
      }
      else
      {
         m_CntlPurchase3Radio.SetCheck(0);
         m_CntlWinOther3Radio.SetCheck(1);
         m_CntlAmtOther3.EnableWindow(TRUE);
      }

      if ( m_CntlPurchase4Radio.GetCheck() )
      {
         m_CntlPurchase4Radio.SetCheck(1);
         m_CntlWinOther4Radio.SetCheck(0);
         m_CntlAmtOther4.EnableWindow(FALSE);
      }
      else
      {
         m_CntlPurchase4Radio.SetCheck(0);
         m_CntlWinOther4Radio.SetCheck(1);
         m_CntlAmtOther4.EnableWindow(TRUE);
      }

      if ( m_CntlPurchase5Radio.GetCheck() )
      {
         m_CntlPurchase5Radio.SetCheck(1);
         m_CntlWinOther5Radio.SetCheck(0);
         m_CntlAmtOther5.EnableWindow(FALSE);
      }
      else
      {
         m_CntlPurchase5Radio.SetCheck(0);
         m_CntlWinOther5Radio.SetCheck(1);
         m_CntlAmtOther5.EnableWindow(TRUE);
      }

      UpdateData(FALSE);
   }

	return CPropertyPage::OnSetActive();
}


BOOL CCB_Amounts::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}
