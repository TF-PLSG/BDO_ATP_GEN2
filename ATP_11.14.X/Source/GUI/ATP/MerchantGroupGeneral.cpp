// MerchantGroupGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantGroupGeneral.h"
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

extern CString  m_StrMerchantGroupId;
extern CSharedMem shrmem;

static int merch_ctr = 0;

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupGeneral property page

IMPLEMENT_DYNCREATE(CMerchantGroupGeneral, CPropertyPage)

CMerchantGroupGeneral::CMerchantGroupGeneral() : CPropertyPage(CMerchantGroupGeneral::IDD)
{
	//{{AFX_DATA_INIT(CMerchantGroupGeneral)
	m_StrAmount1			= _T("");
	m_StrAmount2			= _T("");
	m_StrAmount3			= _T("");
	m_StrAmount4			= _T("");
							
	m_StrInterestRateA1		= _T("");
	m_StrInterestRateA2		= _T("");
	m_StrInterestRateA3		= _T("");
	m_StrInterestRateA4		= _T("");
	m_StrInterestRateA5		= _T("");
	m_StrInterestRateA6		= _T("");
	m_StrInterestRateA7		= _T("");
	m_StrInterestRateA8		= _T("");
	m_StrInterestRateA9		= _T("");
	m_StrInterestRateA10	= _T("");
	m_StrInterestRateA10A	= _T("");
	m_StrInterestRateA10B	= _T("");
							
	m_StrInterestRateA11	= _T("");
	m_StrInterestRateA12	= _T("");
	m_StrInterestRateA13	= _T("");
	m_StrInterestRateA14	= _T("");
	m_StrInterestRateA15	= _T("");
	m_StrInterestRateA16	= _T("");
	m_StrInterestRateA17	= _T("");
	m_StrInterestRateA18	= _T("");
	m_StrInterestRateA19	= _T("");
	m_StrInterestRateA20	= _T("");
	m_StrInterestRateA20A	= _T("");
	m_StrInterestRateA20B	= _T("");
							
	m_StrInterestRateA21	= _T("");
	m_StrInterestRateA22	= _T("");
	m_StrInterestRateA23	= _T("");
	m_StrInterestRateA24	= _T("");
	m_StrInterestRateA25	= _T("");
	m_StrInterestRateA26	= _T("");
	m_StrInterestRateA27	= _T("");
	m_StrInterestRateA28	= _T("");
	m_StrInterestRateA29	= _T("");
	m_StrInterestRateA30	= _T("");
	m_StrInterestRateA30A	= _T("");
	m_StrInterestRateA30B	= _T("");
							
	m_StrInterestRateA31	= _T("");
	m_StrInterestRateA32	= _T("");
	m_StrInterestRateA33	= _T("");
	m_StrInterestRateA34	= _T("");
	m_StrInterestRateA35	=  _T("");
	m_StrInterestRateA36	= _T("");
	m_StrInterestRateA37	= _T("");
	m_StrInterestRateA38	= _T("");
	m_StrInterestRateA39	= _T("");
	m_StrInterestRateA40	= _T("");
	m_StrInterestRateA40A	= _T("");
	m_StrInterestRateA40B	= _T("");
							
	m_StrMSFRateA1			= _T("");
	m_StrMSFRateA2			= _T("");
	m_StrMSFRateA3			= _T("");
	m_StrMSFRateA4			= _T("");
	m_StrMSFRateA5			= _T("");
	m_StrMSFRateA6			= _T("");
	m_StrMSFRateA7			= _T("");
	m_StrMSFRateA8			= _T("");
	m_StrMSFRateA9			= _T("");
	m_StrMSFRateA10			= _T("");
	m_StrMSFRateA10A		= _T("");
	m_StrMSFRateA10B		= _T("");
							
	m_StrMSFRateA11			= _T("");
	m_StrMSFRateA12			= _T("");
	m_StrMSFRateA13			= _T("");
	m_StrMSFRateA14			= _T("");
	m_StrMSFRateA15			= _T("");
	m_StrMSFRateA16			= _T("");
	m_StrMSFRateA17			= _T("");
	m_StrMSFRateA18			= _T("");
	m_StrMSFRateA19			= _T("");
	m_StrMSFRateA20			= _T("");
	m_StrMSFRateA20A		= _T("");
	m_StrMSFRateA20B		= _T("");
							
	m_StrMSFRateA21			= _T("");
	m_StrMSFRateA22			= _T("");
	m_StrMSFRateA23			= _T("");
	m_StrMSFRateA24			= _T("");
	m_StrMSFRateA25			= _T("");
	m_StrMSFRateA26	 		= _T("");
	m_StrMSFRateA27			= _T("");
	m_StrMSFRateA28			= _T("");
	m_StrMSFRateA29			= _T("");
	m_StrMSFRateA30			= _T("");
	m_StrMSFRateA30A		= _T("");
	m_StrMSFRateA30B		= _T("");
							
	m_StrMSFRateA31			= _T("");
	m_StrMSFRateA32			= _T("");
	m_StrMSFRateA33			= _T("");
	m_StrMSFRateA34			= _T("");
	m_StrMSFRateA35			= _T("");
	m_StrMSFRateA36			= _T("");
	m_StrMSFRateA37			= _T("");
	m_StrMSFRateA38			= _T("");
	m_StrMSFRateA39			= _T("");
	m_StrMSFRateA40			= _T("");
	m_StrMSFRateA40A		= _T("");
	m_StrMSFRateA40B		= _T("");
							
	m_StrAmount1			= _T("");
	m_StrEffectiveDateFld	= _T("");
	m_StrMechantGroupIdFld  = _T("");
	//}}AFX_DATA_INIT
}

CMerchantGroupGeneral::~CMerchantGroupGeneral()
{
}

void CMerchantGroupGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantGroupGeneral)
	DDX_Text(pDX, IDC_AMOUNT1, m_StrAmount1);
	DDV_MaxChars(pDX, m_StrAmount1, 12);
	DDX_Text(pDX, IDC_AMOUNT2, m_StrAmount2);
	DDV_MaxChars(pDX, m_StrAmount2, 12);
	DDX_Text(pDX, IDC_AMOUNT3, m_StrAmount3);
	DDV_MaxChars(pDX, m_StrAmount3, 12);
	DDX_Text(pDX, IDC_AMOUNT4, m_StrAmount4);
	DDV_MaxChars(pDX, m_StrAmount4, 12);
	
	DDX_Text(pDX, IDC_INTEREST_RATE_A1 , m_StrInterestRateA1 );
	DDV_MaxChars(pDX, m_StrInterestRateA1 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A2 , m_StrInterestRateA2 );
	DDV_MaxChars(pDX, m_StrInterestRateA2 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A3 , m_StrInterestRateA3 );
	DDV_MaxChars(pDX, m_StrInterestRateA3 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A4 , m_StrInterestRateA4 );
	DDV_MaxChars(pDX, m_StrInterestRateA4 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A5 , m_StrInterestRateA5 );
	DDV_MaxChars(pDX, m_StrInterestRateA5 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A6 , m_StrInterestRateA6 );
	DDV_MaxChars(pDX, m_StrInterestRateA6 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A7 , m_StrInterestRateA7 );
	DDV_MaxChars(pDX, m_StrInterestRateA7 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A8 , m_StrInterestRateA8 );
	DDV_MaxChars(pDX, m_StrInterestRateA8 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A9 , m_StrInterestRateA9 );
	DDV_MaxChars(pDX, m_StrInterestRateA9 , 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A10, m_StrInterestRateA10);
	DDV_MaxChars(pDX, m_StrInterestRateA10, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A10A, m_StrInterestRateA10A);
	DDV_MaxChars(pDX, m_StrInterestRateA10A, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A10B, m_StrInterestRateA10B);
	DDV_MaxChars(pDX, m_StrInterestRateA10B, 7);

	DDX_Text(pDX, IDC_INTEREST_RATE_A11, m_StrInterestRateA11);
	DDV_MaxChars(pDX, m_StrInterestRateA11, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A12, m_StrInterestRateA12);
	DDV_MaxChars(pDX, m_StrInterestRateA12, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A13, m_StrInterestRateA13);
	DDV_MaxChars(pDX, m_StrInterestRateA13, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A14, m_StrInterestRateA14);
	DDV_MaxChars(pDX, m_StrInterestRateA14, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A15, m_StrInterestRateA15);
	DDV_MaxChars(pDX, m_StrInterestRateA15, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A16, m_StrInterestRateA16);
	DDV_MaxChars(pDX, m_StrInterestRateA16, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A17, m_StrInterestRateA17);
	DDV_MaxChars(pDX, m_StrInterestRateA17, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A18, m_StrInterestRateA18);
	DDV_MaxChars(pDX, m_StrInterestRateA18, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A19, m_StrInterestRateA19);
	DDV_MaxChars(pDX, m_StrInterestRateA19, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A20, m_StrInterestRateA20);
	DDV_MaxChars(pDX, m_StrInterestRateA20, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A20A, m_StrInterestRateA20A);
	DDV_MaxChars(pDX, m_StrInterestRateA20A, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A20B, m_StrInterestRateA20B);
	DDV_MaxChars(pDX, m_StrInterestRateA20B, 7);

	DDX_Text(pDX, IDC_INTEREST_RATE_A21, m_StrInterestRateA21);
	DDV_MaxChars(pDX, m_StrInterestRateA21, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A22, m_StrInterestRateA22);
	DDV_MaxChars(pDX, m_StrInterestRateA22, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A23, m_StrInterestRateA23);
	DDV_MaxChars(pDX, m_StrInterestRateA23, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A24, m_StrInterestRateA24);
	DDV_MaxChars(pDX, m_StrInterestRateA24, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A25, m_StrInterestRateA25);
	DDV_MaxChars(pDX, m_StrInterestRateA25, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A26, m_StrInterestRateA26);
	DDV_MaxChars(pDX, m_StrInterestRateA26, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A27, m_StrInterestRateA27);
	DDV_MaxChars(pDX, m_StrInterestRateA27, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A28, m_StrInterestRateA28);
	DDV_MaxChars(pDX, m_StrInterestRateA28, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A29, m_StrInterestRateA29);
	DDV_MaxChars(pDX, m_StrInterestRateA29, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A30, m_StrInterestRateA30);
	DDV_MaxChars(pDX, m_StrInterestRateA30, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A30A, m_StrInterestRateA30A);
	DDV_MaxChars(pDX, m_StrInterestRateA30A, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A30B, m_StrInterestRateA30B);
	DDV_MaxChars(pDX, m_StrInterestRateA30B, 7);

	DDX_Text(pDX, IDC_INTEREST_RATE_A31, m_StrInterestRateA31);
	DDV_MaxChars(pDX, m_StrInterestRateA31, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A32, m_StrInterestRateA32);
	DDV_MaxChars(pDX, m_StrInterestRateA32, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A33, m_StrInterestRateA33);
	DDV_MaxChars(pDX, m_StrInterestRateA33, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A34, m_StrInterestRateA34);
	DDV_MaxChars(pDX, m_StrInterestRateA34, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A35, m_StrInterestRateA35);
	DDV_MaxChars(pDX, m_StrInterestRateA35, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A36, m_StrInterestRateA36);
	DDV_MaxChars(pDX, m_StrInterestRateA36, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A37, m_StrInterestRateA37);
	DDV_MaxChars(pDX, m_StrInterestRateA37, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A38, m_StrInterestRateA38);
	DDV_MaxChars(pDX, m_StrInterestRateA38, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A39, m_StrInterestRateA39);
	DDV_MaxChars(pDX, m_StrInterestRateA39, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A40, m_StrInterestRateA40);
	DDV_MaxChars(pDX, m_StrInterestRateA40, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A40A, m_StrInterestRateA40A);
	DDV_MaxChars(pDX, m_StrInterestRateA40A, 7);
	DDX_Text(pDX, IDC_INTEREST_RATE_A40B, m_StrInterestRateA40B);
	DDV_MaxChars(pDX, m_StrInterestRateA40B, 7);

	DDX_Text(pDX, IDC_MSF_RATE_A1 , m_StrMSFRateA1 );
	DDV_MaxChars(pDX, m_StrMSFRateA1 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A2 , m_StrMSFRateA2 );
	DDV_MaxChars(pDX, m_StrMSFRateA2 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A3 , m_StrMSFRateA3 );
	DDV_MaxChars(pDX, m_StrMSFRateA3 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A4 , m_StrMSFRateA4 );
	DDV_MaxChars(pDX, m_StrMSFRateA4 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A5 , m_StrMSFRateA5 );
	DDV_MaxChars(pDX, m_StrMSFRateA5 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A6 , m_StrMSFRateA6 );
	DDV_MaxChars(pDX, m_StrMSFRateA6 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A7 , m_StrMSFRateA7 );
	DDV_MaxChars(pDX, m_StrMSFRateA7 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A8 , m_StrMSFRateA8 );
	DDV_MaxChars(pDX, m_StrMSFRateA8 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A9 , m_StrMSFRateA9 );
	DDV_MaxChars(pDX, m_StrMSFRateA9 , 7);
	DDX_Text(pDX, IDC_MSF_RATE_A10, m_StrMSFRateA10);
	DDV_MaxChars(pDX, m_StrMSFRateA10, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A10A, m_StrMSFRateA10A);
	DDV_MaxChars(pDX, m_StrMSFRateA10A, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A10B, m_StrMSFRateA10B);
	DDV_MaxChars(pDX, m_StrMSFRateA10B, 7);

	DDX_Text(pDX, IDC_MSF_RATE_A11, m_StrMSFRateA11);
	DDV_MaxChars(pDX, m_StrMSFRateA11, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A12, m_StrMSFRateA12);
	DDV_MaxChars(pDX, m_StrMSFRateA12, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A13, m_StrMSFRateA13);
	DDV_MaxChars(pDX, m_StrMSFRateA13, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A14, m_StrMSFRateA14);
	DDV_MaxChars(pDX, m_StrMSFRateA14, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A15, m_StrMSFRateA15);
	DDV_MaxChars(pDX, m_StrMSFRateA15, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A16, m_StrMSFRateA16);
	DDV_MaxChars(pDX, m_StrMSFRateA16, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A17, m_StrMSFRateA17);
	DDV_MaxChars(pDX, m_StrMSFRateA17, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A18, m_StrMSFRateA18);
	DDV_MaxChars(pDX, m_StrMSFRateA18, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A19, m_StrMSFRateA19);
	DDV_MaxChars(pDX, m_StrMSFRateA19, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A20, m_StrMSFRateA20);
	DDV_MaxChars(pDX, m_StrMSFRateA20, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A20A, m_StrMSFRateA20A);
	DDV_MaxChars(pDX, m_StrMSFRateA20A, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A20B, m_StrMSFRateA20B);
	DDV_MaxChars(pDX, m_StrMSFRateA20B, 7);

	DDX_Text(pDX, IDC_MSF_RATE_A21, m_StrMSFRateA21);
	DDV_MaxChars(pDX, m_StrMSFRateA21, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A22, m_StrMSFRateA22);
	DDV_MaxChars(pDX, m_StrMSFRateA22, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A23, m_StrMSFRateA23);
	DDV_MaxChars(pDX, m_StrMSFRateA23, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A24, m_StrMSFRateA24);
	DDV_MaxChars(pDX, m_StrMSFRateA24, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A25, m_StrMSFRateA25);
	DDV_MaxChars(pDX, m_StrMSFRateA25, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A26, m_StrMSFRateA26);
	DDV_MaxChars(pDX, m_StrMSFRateA26, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A27, m_StrMSFRateA27);
	DDV_MaxChars(pDX, m_StrMSFRateA27, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A28, m_StrMSFRateA28);
	DDV_MaxChars(pDX, m_StrMSFRateA28, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A29, m_StrMSFRateA29);
	DDV_MaxChars(pDX, m_StrMSFRateA29, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A30, m_StrMSFRateA30);
	DDV_MaxChars(pDX, m_StrMSFRateA30, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A30A, m_StrMSFRateA30A);
	DDV_MaxChars(pDX, m_StrMSFRateA30A, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A30B, m_StrMSFRateA30B);
	DDV_MaxChars(pDX, m_StrMSFRateA30B, 7);

	DDX_Text(pDX, IDC_MSF_RATE_A31, m_StrMSFRateA31);
	DDV_MaxChars(pDX, m_StrMSFRateA31, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A32, m_StrMSFRateA32);
	DDV_MaxChars(pDX, m_StrMSFRateA32, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A33, m_StrMSFRateA33);
	DDV_MaxChars(pDX, m_StrMSFRateA33, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A34, m_StrMSFRateA34);
	DDV_MaxChars(pDX, m_StrMSFRateA34, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A35, m_StrMSFRateA35);
	DDV_MaxChars(pDX, m_StrMSFRateA35, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A36, m_StrMSFRateA36);
	DDV_MaxChars(pDX, m_StrMSFRateA36, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A37, m_StrMSFRateA37);
	DDV_MaxChars(pDX, m_StrMSFRateA37, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A38, m_StrMSFRateA38);
	DDV_MaxChars(pDX, m_StrMSFRateA38, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A39, m_StrMSFRateA39);
	DDV_MaxChars(pDX, m_StrMSFRateA39, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A40, m_StrMSFRateA40);
	DDV_MaxChars(pDX, m_StrMSFRateA40, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A40A, m_StrMSFRateA40A);
	DDV_MaxChars(pDX, m_StrMSFRateA40A, 7);
	DDX_Text(pDX, IDC_MSF_RATE_A40B, m_StrMSFRateA40B);
	DDV_MaxChars(pDX, m_StrMSFRateA40B, 7);

	DDX_Text(pDX, IDC_EFFECTIVE_DATE_FLD, m_StrEffectiveDateFld);
	DDV_MaxChars(pDX, m_StrEffectiveDateFld, 8);
	DDX_Text(pDX, IDC_MERCHANT_GROUP_ID_FLD, m_StrMechantGroupIdFld);
	DDV_MaxChars(pDX, m_StrMechantGroupIdFld, 15);
	DDX_Control(pDX, IDC_MERCHANT_GROUP_ID_FLD, m_CntlMechantGroupIdFld);
	DDX_Control(pDX, IDC_EFFECTIVE_DATE_FLD, m_CntlEffectiveDateFld);
	DDX_Control(pDX, IDC_AMOUNT1, m_CntlAmount1);
	DDX_Control(pDX, IDC_AMOUNT2, m_CntlAmount2);
	DDX_Control(pDX, IDC_AMOUNT3, m_CntlAmount3);
	DDX_Control(pDX, IDC_AMOUNT4, m_CntlAmount4);

	DDX_Control(pDX, IDC_INTEREST_RATE_A1 ,		m_CntlInterestRateA1 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A2 ,		m_CntlInterestRateA2 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A3 ,		m_CntlInterestRateA3 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A4 ,		m_CntlInterestRateA4 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A5 ,		m_CntlInterestRateA5 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A6 ,		m_CntlInterestRateA6 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A7 ,		m_CntlInterestRateA7 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A8 ,		m_CntlInterestRateA8 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A9 ,		m_CntlInterestRateA9 );
	DDX_Control(pDX, IDC_INTEREST_RATE_A10,		m_CntlInterestRateA10);
	DDX_Control(pDX, IDC_INTEREST_RATE_A10A,	m_CntlInterestRateA10A);
	DDX_Control(pDX, IDC_INTEREST_RATE_A10B,	m_CntlInterestRateA10B);

	DDX_Control(pDX, IDC_INTEREST_RATE_A11,		m_CntlInterestRateA11);
	DDX_Control(pDX, IDC_INTEREST_RATE_A12,		m_CntlInterestRateA12);
	DDX_Control(pDX, IDC_INTEREST_RATE_A13,		m_CntlInterestRateA13);
	DDX_Control(pDX, IDC_INTEREST_RATE_A14,		m_CntlInterestRateA14);
	DDX_Control(pDX, IDC_INTEREST_RATE_A15,		m_CntlInterestRateA15);
	DDX_Control(pDX, IDC_INTEREST_RATE_A16,		m_CntlInterestRateA16);
	DDX_Control(pDX, IDC_INTEREST_RATE_A17,		m_CntlInterestRateA17);
	DDX_Control(pDX, IDC_INTEREST_RATE_A18,		m_CntlInterestRateA18);
	DDX_Control(pDX, IDC_INTEREST_RATE_A19,		m_CntlInterestRateA19);
	DDX_Control(pDX, IDC_INTEREST_RATE_A20,		m_CntlInterestRateA20);
	DDX_Control(pDX, IDC_INTEREST_RATE_A20A,	m_CntlInterestRateA20A);
	DDX_Control(pDX, IDC_INTEREST_RATE_A20B,	m_CntlInterestRateA20B);

	DDX_Control(pDX, IDC_INTEREST_RATE_A21,		m_CntlInterestRateA21);
	DDX_Control(pDX, IDC_INTEREST_RATE_A22,		m_CntlInterestRateA22);
	DDX_Control(pDX, IDC_INTEREST_RATE_A23,		m_CntlInterestRateA23);
	DDX_Control(pDX, IDC_INTEREST_RATE_A24,		m_CntlInterestRateA24);
	DDX_Control(pDX, IDC_INTEREST_RATE_A25,		m_CntlInterestRateA25);
	DDX_Control(pDX, IDC_INTEREST_RATE_A26,		m_CntlInterestRateA26);
	DDX_Control(pDX, IDC_INTEREST_RATE_A27,		m_CntlInterestRateA27);
	DDX_Control(pDX, IDC_INTEREST_RATE_A28,		m_CntlInterestRateA28);
	DDX_Control(pDX, IDC_INTEREST_RATE_A29,		m_CntlInterestRateA29);
	DDX_Control(pDX, IDC_INTEREST_RATE_A30,		m_CntlInterestRateA30);
	DDX_Control(pDX, IDC_INTEREST_RATE_A30A,	m_CntlInterestRateA30A);
	DDX_Control(pDX, IDC_INTEREST_RATE_A30B,	m_CntlInterestRateA30B);

	DDX_Control(pDX, IDC_INTEREST_RATE_A31,		m_CntlInterestRateA31);
	DDX_Control(pDX, IDC_INTEREST_RATE_A32,		m_CntlInterestRateA32);
	DDX_Control(pDX, IDC_INTEREST_RATE_A33,		m_CntlInterestRateA33);
	DDX_Control(pDX, IDC_INTEREST_RATE_A34,		m_CntlInterestRateA34);
	DDX_Control(pDX, IDC_INTEREST_RATE_A35,		m_CntlInterestRateA35);
	DDX_Control(pDX, IDC_INTEREST_RATE_A36,		m_CntlInterestRateA36);
	DDX_Control(pDX, IDC_INTEREST_RATE_A37,		m_CntlInterestRateA37);
	DDX_Control(pDX, IDC_INTEREST_RATE_A38,		m_CntlInterestRateA38);
	DDX_Control(pDX, IDC_INTEREST_RATE_A39,		m_CntlInterestRateA39);
	DDX_Control(pDX, IDC_INTEREST_RATE_A40,		m_CntlInterestRateA40);
	DDX_Control(pDX, IDC_INTEREST_RATE_A40A,	m_CntlInterestRateA40A);
	DDX_Control(pDX, IDC_INTEREST_RATE_A40B,	 m_CntlInterestRateA40B);

	DDX_Control(pDX, IDC_MSF_RATE_A1 ,	m_CntlMSFRateA1 );
	DDX_Control(pDX, IDC_MSF_RATE_A2 ,	m_CntlMSFRateA2 );
	DDX_Control(pDX, IDC_MSF_RATE_A3 ,	m_CntlMSFRateA3 );
	DDX_Control(pDX, IDC_MSF_RATE_A4 ,	m_CntlMSFRateA4 );
	DDX_Control(pDX, IDC_MSF_RATE_A5 ,	m_CntlMSFRateA5 );
	DDX_Control(pDX, IDC_MSF_RATE_A6 ,	m_CntlMSFRateA6 );
	DDX_Control(pDX, IDC_MSF_RATE_A7 ,	m_CntlMSFRateA7 );
	DDX_Control(pDX, IDC_MSF_RATE_A8 ,	m_CntlMSFRateA8 );
	DDX_Control(pDX, IDC_MSF_RATE_A9 ,	m_CntlMSFRateA9 );
	DDX_Control(pDX, IDC_MSF_RATE_A10,	m_CntlMSFRateA10);
	DDX_Control(pDX, IDC_MSF_RATE_A10A,	m_CntlMSFRateA10A);
	DDX_Control(pDX, IDC_MSF_RATE_A10B,	m_CntlMSFRateA10B);
										
	DDX_Control(pDX, IDC_MSF_RATE_A11,	m_CntlMSFRateA11);
	DDX_Control(pDX, IDC_MSF_RATE_A12,	m_CntlMSFRateA12);
	DDX_Control(pDX, IDC_MSF_RATE_A13,	m_CntlMSFRateA13);
	DDX_Control(pDX, IDC_MSF_RATE_A14,	m_CntlMSFRateA14);
	DDX_Control(pDX, IDC_MSF_RATE_A15,	m_CntlMSFRateA15);
	DDX_Control(pDX, IDC_MSF_RATE_A16,	m_CntlMSFRateA16);
	DDX_Control(pDX, IDC_MSF_RATE_A17,	m_CntlMSFRateA17);
	DDX_Control(pDX, IDC_MSF_RATE_A18,	m_CntlMSFRateA18);
	DDX_Control(pDX, IDC_MSF_RATE_A19,	m_CntlMSFRateA19);
	DDX_Control(pDX, IDC_MSF_RATE_A20,	m_CntlMSFRateA20);
	DDX_Control(pDX, IDC_MSF_RATE_A20A,	m_CntlMSFRateA20A);
	DDX_Control(pDX, IDC_MSF_RATE_A20B,	m_CntlMSFRateA20B);

	DDX_Control(pDX, IDC_MSF_RATE_A21,	m_CntlMSFRateA21);
	DDX_Control(pDX, IDC_MSF_RATE_A22,	m_CntlMSFRateA22);
	DDX_Control(pDX, IDC_MSF_RATE_A23,	m_CntlMSFRateA23);
	DDX_Control(pDX, IDC_MSF_RATE_A24,	m_CntlMSFRateA24);
	DDX_Control(pDX, IDC_MSF_RATE_A25,	m_CntlMSFRateA25);
	DDX_Control(pDX, IDC_MSF_RATE_A26,	m_CntlMSFRateA26);
	DDX_Control(pDX, IDC_MSF_RATE_A27,	m_CntlMSFRateA27);
	DDX_Control(pDX, IDC_MSF_RATE_A28,	m_CntlMSFRateA28);
	DDX_Control(pDX, IDC_MSF_RATE_A29,	m_CntlMSFRateA29);
	DDX_Control(pDX, IDC_MSF_RATE_A30,	m_CntlMSFRateA30);
	DDX_Control(pDX, IDC_MSF_RATE_A30A,	m_CntlMSFRateA30A);
	DDX_Control(pDX, IDC_MSF_RATE_A30B,	m_CntlMSFRateA30B);

	DDX_Control(pDX, IDC_MSF_RATE_A31,	m_CntlMSFRateA31);
	DDX_Control(pDX, IDC_MSF_RATE_A32,	m_CntlMSFRateA32);
	DDX_Control(pDX, IDC_MSF_RATE_A33,	m_CntlMSFRateA33);
	DDX_Control(pDX, IDC_MSF_RATE_A34,	m_CntlMSFRateA34);
	DDX_Control(pDX, IDC_MSF_RATE_A35,	m_CntlMSFRateA35);
	DDX_Control(pDX, IDC_MSF_RATE_A36,	m_CntlMSFRateA36);
	DDX_Control(pDX, IDC_MSF_RATE_A37,	m_CntlMSFRateA37);
	DDX_Control(pDX, IDC_MSF_RATE_A38,	m_CntlMSFRateA38);
	DDX_Control(pDX, IDC_MSF_RATE_A39,	m_CntlMSFRateA39);
	DDX_Control(pDX, IDC_MSF_RATE_A40,	m_CntlMSFRateA40);
	DDX_Control(pDX, IDC_MSF_RATE_A40A,	m_CntlMSFRateA40A);
	DDX_Control(pDX, IDC_MSF_RATE_A40B,	m_CntlMSFRateA40B);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantGroupGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantGroupGeneral)
	ON_EN_CHANGE(IDC_MSF_RATE_A20B2, OnChangeMsfRateA20B)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupGeneral message handlers

void CMerchantGroupGeneral::OnCancel() 
{
	CPropertyPage::OnCancel();
}


CString CMerchantGroupGeneral::convert_ddmmyyyy_to_yyyymmdd( CString input_date )
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

CString CMerchantGroupGeneral::convert_yyyymmdd_to_ddmmyyyy( CString input_date )
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

BOOL CMerchantGroupGeneral::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return FALSE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CMerchantGroupGeneral::PopulateGeneralDialogFromDatabase ( MGF01 sMerchantGroupRecord)
{
		ZeroMemory ( &sMerchantGroupRecordOld, sizeof ( MGF01) );

	m_StrMechantGroupIdFld = sMerchantGroupRecord.primary_key.merchant_group_id;
	m_StrEffectiveDateFld = sMerchantGroupRecord.effective_date;

	m_StrAmount1          = sMerchantGroupRecord.merchgrp[0].Amount_limit;
	m_StrInterestRateA1   = sMerchantGroupRecord.merchgrp[0].merchgrepint[0].INT_rate;
	m_StrMSFRateA1        = sMerchantGroupRecord.merchgrp[0].merchgrepint[0].MSF_rate;
	m_StrInterestRateA2   = sMerchantGroupRecord.merchgrp[0].merchgrepint[1].INT_rate;
	m_StrMSFRateA2        = sMerchantGroupRecord.merchgrp[0].merchgrepint[1].MSF_rate;
	m_StrInterestRateA3   = sMerchantGroupRecord.merchgrp[0].merchgrepint[2].INT_rate;
	m_StrMSFRateA3        = sMerchantGroupRecord.merchgrp[0].merchgrepint[2].MSF_rate;
	m_StrInterestRateA4   = sMerchantGroupRecord.merchgrp[0].merchgrepint[3].INT_rate;
	m_StrMSFRateA4        = sMerchantGroupRecord.merchgrp[0].merchgrepint[3].MSF_rate;
	m_StrInterestRateA5   = sMerchantGroupRecord.merchgrp[0].merchgrepint[4].INT_rate;
	m_StrMSFRateA5        = sMerchantGroupRecord.merchgrp[0].merchgrepint[4].MSF_rate;
	m_StrInterestRateA6   = sMerchantGroupRecord.merchgrp[0].merchgrepint[5].INT_rate;
	m_StrMSFRateA6        = sMerchantGroupRecord.merchgrp[0].merchgrepint[5].MSF_rate;
	m_StrInterestRateA7   = sMerchantGroupRecord.merchgrp[0].merchgrepint[6].INT_rate;
	m_StrMSFRateA7        = sMerchantGroupRecord.merchgrp[0].merchgrepint[6].MSF_rate;
	m_StrInterestRateA8   = sMerchantGroupRecord.merchgrp[0].merchgrepint[7].INT_rate;
	m_StrMSFRateA8        = sMerchantGroupRecord.merchgrp[0].merchgrepint[7].MSF_rate;
	m_StrInterestRateA9   = sMerchantGroupRecord.merchgrp[0].merchgrepint[8].INT_rate;
	m_StrMSFRateA9        = sMerchantGroupRecord.merchgrp[0].merchgrepint[8].MSF_rate;
	m_StrInterestRateA10  = sMerchantGroupRecord.merchgrp[0].merchgrepint[9].INT_rate;
	m_StrMSFRateA10       = sMerchantGroupRecord.merchgrp[0].merchgrepint[9].MSF_rate;
	m_StrInterestRateA10A = sMerchantGroupRecord.merchgrp[0].merchgrepint[10].INT_rate;
	m_StrMSFRateA10A      = sMerchantGroupRecord.merchgrp[0].merchgrepint[10].MSF_rate;
	m_StrInterestRateA10B = sMerchantGroupRecord.merchgrp[0].merchgrepint[11].INT_rate;
	m_StrMSFRateA10B      = sMerchantGroupRecord.merchgrp[0].merchgrepint[11].MSF_rate;

	m_StrAmount2          = sMerchantGroupRecord.merchgrp[1].Amount_limit;
	m_StrInterestRateA11  = sMerchantGroupRecord.merchgrp[1].merchgrepint[0].INT_rate;
	m_StrMSFRateA11       = sMerchantGroupRecord.merchgrp[1].merchgrepint[0].MSF_rate;
	m_StrInterestRateA12  = sMerchantGroupRecord.merchgrp[1].merchgrepint[1].INT_rate;
	m_StrMSFRateA12       = sMerchantGroupRecord.merchgrp[1].merchgrepint[1].MSF_rate;
	m_StrInterestRateA13  = sMerchantGroupRecord.merchgrp[1].merchgrepint[2].INT_rate;
	m_StrMSFRateA13       = sMerchantGroupRecord.merchgrp[1].merchgrepint[2].MSF_rate;
	m_StrInterestRateA14  = sMerchantGroupRecord.merchgrp[1].merchgrepint[3].INT_rate;
	m_StrMSFRateA14       = sMerchantGroupRecord.merchgrp[1].merchgrepint[3].MSF_rate;
	m_StrInterestRateA15  = sMerchantGroupRecord.merchgrp[1].merchgrepint[4].INT_rate;
	m_StrMSFRateA15       = sMerchantGroupRecord.merchgrp[1].merchgrepint[4].MSF_rate;
	m_StrInterestRateA16  = sMerchantGroupRecord.merchgrp[1].merchgrepint[5].INT_rate;
	m_StrMSFRateA16       = sMerchantGroupRecord.merchgrp[1].merchgrepint[5].MSF_rate;
	m_StrInterestRateA17  = sMerchantGroupRecord.merchgrp[1].merchgrepint[6].INT_rate;
	m_StrMSFRateA17       = sMerchantGroupRecord.merchgrp[1].merchgrepint[6].MSF_rate;
	m_StrInterestRateA18  = sMerchantGroupRecord.merchgrp[1].merchgrepint[7].INT_rate;
	m_StrMSFRateA18       = sMerchantGroupRecord.merchgrp[1].merchgrepint[7].MSF_rate;
	m_StrInterestRateA19  = sMerchantGroupRecord.merchgrp[1].merchgrepint[8].INT_rate;
	m_StrMSFRateA19       = sMerchantGroupRecord.merchgrp[1].merchgrepint[8].MSF_rate;
	m_StrInterestRateA20  = sMerchantGroupRecord.merchgrp[1].merchgrepint[9].INT_rate;
	m_StrMSFRateA20       = sMerchantGroupRecord.merchgrp[1].merchgrepint[9].MSF_rate;
	m_StrInterestRateA20A = sMerchantGroupRecord.merchgrp[1].merchgrepint[10].INT_rate;
	m_StrMSFRateA20A      = sMerchantGroupRecord.merchgrp[1].merchgrepint[10].MSF_rate;
	m_StrInterestRateA20B = sMerchantGroupRecord.merchgrp[1].merchgrepint[11].INT_rate;
	m_StrMSFRateA20B      = sMerchantGroupRecord.merchgrp[1].merchgrepint[11].MSF_rate;

	m_StrAmount3          = sMerchantGroupRecord.merchgrp[2].Amount_limit;
	m_StrInterestRateA21  = sMerchantGroupRecord.merchgrp[2].merchgrepint[0].INT_rate;
	m_StrMSFRateA21       = sMerchantGroupRecord.merchgrp[2].merchgrepint[0].MSF_rate;
	m_StrInterestRateA22  = sMerchantGroupRecord.merchgrp[2].merchgrepint[1].INT_rate;
	m_StrMSFRateA22       = sMerchantGroupRecord.merchgrp[2].merchgrepint[1].MSF_rate;
	m_StrInterestRateA23  = sMerchantGroupRecord.merchgrp[2].merchgrepint[2].INT_rate;
	m_StrMSFRateA23       = sMerchantGroupRecord.merchgrp[2].merchgrepint[2].MSF_rate;
	m_StrInterestRateA24  = sMerchantGroupRecord.merchgrp[2].merchgrepint[3].INT_rate;
	m_StrMSFRateA24       = sMerchantGroupRecord.merchgrp[2].merchgrepint[3].MSF_rate;
	m_StrInterestRateA25  = sMerchantGroupRecord.merchgrp[2].merchgrepint[4].INT_rate;
	m_StrMSFRateA25       = sMerchantGroupRecord.merchgrp[2].merchgrepint[4].MSF_rate;
	m_StrInterestRateA26  = sMerchantGroupRecord.merchgrp[2].merchgrepint[5].INT_rate;
	m_StrMSFRateA26       = sMerchantGroupRecord.merchgrp[2].merchgrepint[5].MSF_rate;
	m_StrInterestRateA27  = sMerchantGroupRecord.merchgrp[2].merchgrepint[6].INT_rate;
	m_StrMSFRateA27       = sMerchantGroupRecord.merchgrp[2].merchgrepint[6].MSF_rate;
	m_StrInterestRateA28  = sMerchantGroupRecord.merchgrp[2].merchgrepint[7].INT_rate;
	m_StrMSFRateA28       = sMerchantGroupRecord.merchgrp[2].merchgrepint[7].MSF_rate;
	m_StrInterestRateA29  = sMerchantGroupRecord.merchgrp[2].merchgrepint[8].INT_rate;
	m_StrMSFRateA29       = sMerchantGroupRecord.merchgrp[2].merchgrepint[8].MSF_rate;
	m_StrInterestRateA30  = sMerchantGroupRecord.merchgrp[2].merchgrepint[9].INT_rate;
	m_StrMSFRateA30       = sMerchantGroupRecord.merchgrp[2].merchgrepint[9].MSF_rate;
	m_StrInterestRateA30A = sMerchantGroupRecord.merchgrp[2].merchgrepint[10].INT_rate;
	m_StrMSFRateA30A      = sMerchantGroupRecord.merchgrp[2].merchgrepint[10].MSF_rate;
	m_StrInterestRateA30B = sMerchantGroupRecord.merchgrp[2].merchgrepint[11].INT_rate;
	m_StrMSFRateA30B      = sMerchantGroupRecord.merchgrp[2].merchgrepint[11].MSF_rate;

	m_StrAmount4          = sMerchantGroupRecord.merchgrp[3].Amount_limit;
	m_StrInterestRateA31  = sMerchantGroupRecord.merchgrp[3].merchgrepint[0].INT_rate;
	m_StrMSFRateA31       = sMerchantGroupRecord.merchgrp[3].merchgrepint[0].MSF_rate;
	m_StrInterestRateA32  = sMerchantGroupRecord.merchgrp[3].merchgrepint[1].INT_rate;
	m_StrMSFRateA32       = sMerchantGroupRecord.merchgrp[3].merchgrepint[1].MSF_rate;
	m_StrInterestRateA33  = sMerchantGroupRecord.merchgrp[3].merchgrepint[2].INT_rate;
	m_StrMSFRateA33       = sMerchantGroupRecord.merchgrp[3].merchgrepint[2].MSF_rate;
	m_StrInterestRateA34  = sMerchantGroupRecord.merchgrp[3].merchgrepint[3].INT_rate;
	m_StrMSFRateA34       = sMerchantGroupRecord.merchgrp[3].merchgrepint[3].MSF_rate;
	m_StrInterestRateA35  = sMerchantGroupRecord.merchgrp[3].merchgrepint[4].INT_rate;
	m_StrMSFRateA35       = sMerchantGroupRecord.merchgrp[3].merchgrepint[4].MSF_rate;
	m_StrInterestRateA36  = sMerchantGroupRecord.merchgrp[3].merchgrepint[5].INT_rate;
	m_StrMSFRateA36       = sMerchantGroupRecord.merchgrp[3].merchgrepint[5].MSF_rate;
	m_StrInterestRateA37  = sMerchantGroupRecord.merchgrp[3].merchgrepint[6].INT_rate;
	m_StrMSFRateA37       = sMerchantGroupRecord.merchgrp[3].merchgrepint[6].MSF_rate;
	m_StrInterestRateA38  = sMerchantGroupRecord.merchgrp[3].merchgrepint[7].INT_rate;
	m_StrMSFRateA38       = sMerchantGroupRecord.merchgrp[3].merchgrepint[7].MSF_rate;
	m_StrInterestRateA39  = sMerchantGroupRecord.merchgrp[3].merchgrepint[8].INT_rate;
	m_StrMSFRateA39       = sMerchantGroupRecord.merchgrp[3].merchgrepint[8].MSF_rate;
	m_StrInterestRateA40  = sMerchantGroupRecord.merchgrp[3].merchgrepint[9].INT_rate;
	m_StrMSFRateA40       = sMerchantGroupRecord.merchgrp[3].merchgrepint[9].MSF_rate;
	m_StrInterestRateA40A = sMerchantGroupRecord.merchgrp[3].merchgrepint[10].INT_rate;
	m_StrMSFRateA40A      = sMerchantGroupRecord.merchgrp[3].merchgrepint[10].MSF_rate;
	m_StrInterestRateA40B = sMerchantGroupRecord.merchgrp[3].merchgrepint[11].INT_rate;
	m_StrMSFRateA40B      = sMerchantGroupRecord.merchgrp[3].merchgrepint[11].MSF_rate;
	
	//UpdateData(FALSE);
	
	memcpy ( &sMerchantGroupRecordOld, &sMerchantGroupRecord, sizeof ( MGF01));
	
	
	return TRUE;
}

void CMerchantGroupGeneral::CleanGeneralDialog()
{
	m_StrMessageLbl = _T("");
	m_StrAmount1 = _T("");
	m_StrAmount2 = _T("");
	m_StrAmount3 = _T("");
	m_StrAmount4 = _T("");
	m_StrInterestRateA1		= _T("");
	m_StrInterestRateA2		= _T("");
	m_StrInterestRateA3		= _T("");
	m_StrInterestRateA4		= _T("");
	m_StrInterestRateA5		= _T("");
	m_StrInterestRateA6		= _T("");
	m_StrInterestRateA7		= _T("");
	m_StrInterestRateA8		= _T("");
	m_StrInterestRateA9		= _T("");
	m_StrInterestRateA10	= _T("");
	m_StrInterestRateA10A	= _T("");
	m_StrInterestRateA10B	= _T("");

	m_StrInterestRateA11	= _T("");
	m_StrInterestRateA12	= _T("");
	m_StrInterestRateA13	= _T("");
	m_StrInterestRateA14	= _T("");
	m_StrInterestRateA15	= _T("");
	m_StrInterestRateA16	= _T("");
	m_StrInterestRateA17	= _T("");
	m_StrInterestRateA18	= _T("");
	m_StrInterestRateA19	= _T("");
	m_StrInterestRateA20	= _T("");
	m_StrInterestRateA20A	= _T("");
	m_StrInterestRateA20B	= _T("");

	m_StrInterestRateA21	= _T("");
	m_StrInterestRateA22	= _T("");
	m_StrInterestRateA23	= _T("");
	m_StrInterestRateA24	= _T("");
	m_StrInterestRateA25	= _T("");
	m_StrInterestRateA26	= _T("");
	m_StrInterestRateA27	= _T("");
	m_StrInterestRateA28	= _T("");
	m_StrInterestRateA29	= _T("");
	m_StrInterestRateA30	= _T("");
	m_StrInterestRateA30A	= _T("");
	m_StrInterestRateA30B	= _T("");

	m_StrInterestRateA31	= _T("");
	m_StrInterestRateA32	= _T("");
	m_StrInterestRateA33	= _T("");
	m_StrInterestRateA34	= _T("");
	m_StrInterestRateA35	= _T("");
	m_StrInterestRateA36	= _T("");
	m_StrInterestRateA37	= _T("");
	m_StrInterestRateA38	= _T("");
	m_StrInterestRateA39	= _T("");
	m_StrInterestRateA40	= _T("");
	m_StrInterestRateA40A	= _T("");
	m_StrInterestRateA40B	= _T("");

	m_StrMSFRateA1			= _T("");
	m_StrMSFRateA2			= _T("");
	m_StrMSFRateA3			= _T("");
	m_StrMSFRateA4			= _T("");
	m_StrMSFRateA5			= _T("");
	m_StrMSFRateA6			= _T("");
	m_StrMSFRateA7			= _T("");
	m_StrMSFRateA8			= _T("");
	m_StrMSFRateA9			= _T("");
	m_StrMSFRateA10			= _T("");
	m_StrMSFRateA10A		= _T("");
	m_StrMSFRateA10B		= _T("");

	m_StrMSFRateA11			= _T("");
	m_StrMSFRateA12			= _T("");
	m_StrMSFRateA13			= _T("");
	m_StrMSFRateA14			= _T("");
	m_StrMSFRateA15			= _T("");
	m_StrMSFRateA16			= _T("");
	m_StrMSFRateA17			= _T("");
	m_StrMSFRateA18			= _T("");
	m_StrMSFRateA19			= _T("");
	m_StrMSFRateA20			= _T("");
	m_StrMSFRateA20A		= _T("");
	m_StrMSFRateA20B		= _T("");
							
	m_StrMSFRateA21			= _T("");
	m_StrMSFRateA22			= _T("");
	m_StrMSFRateA23			= _T("");
	m_StrMSFRateA24			= _T("");
	m_StrMSFRateA25			= _T("");
	m_StrMSFRateA26			= _T("");
	m_StrMSFRateA27			= _T("");
	m_StrMSFRateA28			= _T("");
	m_StrMSFRateA29			= _T("");
	m_StrMSFRateA30			= _T("");
	m_StrMSFRateA30A		= _T("");
	m_StrMSFRateA30B		= _T("");

	m_StrMSFRateA31			= _T("");
	m_StrMSFRateA32			= _T("");
	m_StrMSFRateA33			= _T("");
	m_StrMSFRateA34			= _T("");
	m_StrMSFRateA35			= _T("");
	m_StrMSFRateA36			= _T("");
	m_StrMSFRateA37			= _T("");
	m_StrMSFRateA38			= _T("");
	m_StrMSFRateA39			= _T("");
	m_StrMSFRateA40			= _T("");
	m_StrMSFRateA40A		= _T("");
	m_StrMSFRateA40B		= _T("");
							
	m_StrAmount1			= _T("");
	m_StrEffectiveDateFld	= _T("");
	m_StrMechantGroupIdFld	= _T("");
}

BOOL CMerchantGroupGeneral::PopulateDatabaseFromGeneralDialog ( pMGF01 pMerchantGroupRecord)
{
	UpdateData(TRUE);
	char nbr1[3] = "03";
	char nbr2[3] = "06";
	char nbr3[3] = "09";
	char nbr4[3] = "12";
	char nbr5[3] = "15";
	char nbr6[3] = "18";
	char nbr7[3] = "21";
	char nbr8[3] = "24";
	char nbr9[3] = "27";
	char nbr10[3] = "30";
	char nbr10A[3] = "33";
	char nbr10B[3] = "36";


	memcpy( pMerchantGroupRecord->primary_key.merchant_group_id             , m_StrMechantGroupIdFld,	sizeof(pMerchantGroupRecord->primary_key.merchant_group_id             ));
	memcpy( pMerchantGroupRecord->effective_date                            , m_StrEffectiveDateFld,	sizeof(pMerchantGroupRecord->effective_date                            ));
	memcpy( pMerchantGroupRecord->merchgrp[0].Amount_limit                  , m_StrAmount1         , sizeof(pMerchantGroupRecord->merchgrp[0].Amount_limit                  ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[0].nbr_of_months , nbr1                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[0].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[0].INT_rate      , m_StrInterestRateA1  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[0].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[0].MSF_rate      , m_StrMSFRateA1       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[0].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[1].nbr_of_months , nbr2                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[1].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[1].INT_rate      , m_StrInterestRateA2  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[1].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[1].MSF_rate      , m_StrMSFRateA2       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[1].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[2].nbr_of_months , nbr3                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[2].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[2].INT_rate      , m_StrInterestRateA3  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[2].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[2].MSF_rate      , m_StrMSFRateA3       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[2].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[3].nbr_of_months , nbr4                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[3].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[3].INT_rate      , m_StrInterestRateA4  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[3].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[3].MSF_rate      , m_StrMSFRateA4       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[3].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[4].nbr_of_months , nbr5                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[4].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[4].INT_rate      , m_StrInterestRateA5  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[4].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[4].MSF_rate      , m_StrMSFRateA5       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[4].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[5].nbr_of_months , nbr6                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[5].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[5].INT_rate      , m_StrInterestRateA6  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[5].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[5].MSF_rate      , m_StrMSFRateA6       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[5].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[6].nbr_of_months , nbr7                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[6].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[6].INT_rate      , m_StrInterestRateA7  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[6].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[6].MSF_rate      , m_StrMSFRateA7       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[6].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[7].nbr_of_months , nbr8                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[7].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[7].INT_rate      , m_StrInterestRateA8  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[7].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[7].MSF_rate      , m_StrMSFRateA8       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[7].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[8].nbr_of_months , nbr9                 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[8].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[8].INT_rate      , m_StrInterestRateA9  , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[8].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[8].MSF_rate      , m_StrMSFRateA9       , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[8].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[9].nbr_of_months , nbr10                , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[9].INT_rate      , m_StrInterestRateA10 , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[9].MSF_rate      , m_StrMSFRateA10      , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[9].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[10].nbr_of_months , nbr10A                , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[10].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[10].INT_rate      , m_StrInterestRateA10A , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[10].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[10].MSF_rate      , m_StrMSFRateA10A      , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[10].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[11].nbr_of_months , nbr10B                , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[11].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[11].INT_rate      , m_StrInterestRateA10B , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[11].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[0].merchgrepint[11].MSF_rate      , m_StrMSFRateA10B      , sizeof(pMerchantGroupRecord->merchgrp[0].merchgrepint[11].MSF_rate      ));
	
	memcpy( pMerchantGroupRecord->merchgrp[1].Amount_limit                  , m_StrAmount2         , sizeof(pMerchantGroupRecord->merchgrp[1].Amount_limit                  ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[0].nbr_of_months , nbr1                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[0].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[0].INT_rate      , m_StrInterestRateA11 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[0].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[0].MSF_rate      , m_StrMSFRateA11      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[0].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[1].nbr_of_months , nbr2                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[1].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[1].INT_rate      , m_StrInterestRateA12 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[1].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[1].MSF_rate      , m_StrMSFRateA12      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[1].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[2].nbr_of_months , nbr3                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[2].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[2].INT_rate      , m_StrInterestRateA13 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[2].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[2].MSF_rate      , m_StrMSFRateA13      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[2].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[3].nbr_of_months , nbr4                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[3].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[3].INT_rate      , m_StrInterestRateA14 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[3].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[3].MSF_rate      , m_StrMSFRateA14      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[3].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[4].nbr_of_months , nbr5                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[4].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[4].INT_rate      , m_StrInterestRateA15 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[4].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[4].MSF_rate      , m_StrMSFRateA15      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[4].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[5].nbr_of_months , nbr6                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[5].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[5].INT_rate      , m_StrInterestRateA16 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[5].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[5].MSF_rate      , m_StrMSFRateA16      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[5].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[6].nbr_of_months , nbr7                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[6].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[6].INT_rate      , m_StrInterestRateA17 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[6].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[6].MSF_rate      , m_StrMSFRateA17      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[6].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[7].nbr_of_months , nbr8                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[7].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[7].INT_rate      , m_StrInterestRateA18 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[7].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[7].MSF_rate      , m_StrMSFRateA18      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[7].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[8].nbr_of_months , nbr9                 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[8].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[8].INT_rate      , m_StrInterestRateA19 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[8].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[8].MSF_rate      , m_StrMSFRateA19      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[8].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[9].nbr_of_months , nbr10                , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[9].INT_rate      , m_StrInterestRateA20 , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[9].MSF_rate      , m_StrMSFRateA20      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[9].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[10].nbr_of_months , nbr10A                , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[10].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[10].INT_rate      , m_StrInterestRateA20A , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[10].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[10].MSF_rate      , m_StrMSFRateA20A      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[10].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[11].nbr_of_months , nbr10B                , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[11].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[11].INT_rate      , m_StrInterestRateA20B , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[11].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[1].merchgrepint[11].MSF_rate      , m_StrMSFRateA20B      , sizeof(pMerchantGroupRecord->merchgrp[1].merchgrepint[11].MSF_rate      ));
	

	memcpy( pMerchantGroupRecord->merchgrp[2].Amount_limit                  , m_StrAmount3         , sizeof(pMerchantGroupRecord->merchgrp[2].Amount_limit                  ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[0].nbr_of_months , nbr1                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[0].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[0].INT_rate      , m_StrInterestRateA21 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[0].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[0].MSF_rate      , m_StrMSFRateA21      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[0].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[1].nbr_of_months , nbr2                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[1].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[1].INT_rate      , m_StrInterestRateA22 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[1].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[1].MSF_rate      , m_StrMSFRateA22      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[1].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[2].nbr_of_months , nbr3                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[2].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[2].INT_rate      , m_StrInterestRateA23 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[2].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[2].MSF_rate      , m_StrMSFRateA23      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[2].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[3].nbr_of_months , nbr4                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[3].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[3].INT_rate      , m_StrInterestRateA24 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[3].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[3].MSF_rate      , m_StrMSFRateA24      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[3].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[4].nbr_of_months , nbr5                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[4].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[4].INT_rate      , m_StrInterestRateA25 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[4].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[4].MSF_rate      , m_StrMSFRateA25      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[4].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[5].nbr_of_months , nbr6                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[5].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[5].INT_rate      , m_StrInterestRateA26 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[5].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[5].MSF_rate      , m_StrMSFRateA26      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[5].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[6].nbr_of_months , nbr7                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[6].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[6].INT_rate      , m_StrInterestRateA27 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[6].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[6].MSF_rate      , m_StrMSFRateA27      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[6].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[7].nbr_of_months , nbr8                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[7].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[7].INT_rate      , m_StrInterestRateA28 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[7].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[7].MSF_rate      , m_StrMSFRateA28      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[7].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[8].nbr_of_months , nbr9                 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[8].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[8].INT_rate      , m_StrInterestRateA29 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[8].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[8].MSF_rate      , m_StrMSFRateA29      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[8].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[9].nbr_of_months , nbr10                , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[9].INT_rate      , m_StrInterestRateA30 , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[9].MSF_rate      , m_StrMSFRateA30      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[9].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[10].nbr_of_months , nbr10A                , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[10].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[10].INT_rate      , m_StrInterestRateA30A , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[10].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[10].MSF_rate      , m_StrMSFRateA30A      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[10].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[11].nbr_of_months , nbr10B                , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[11].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[11].INT_rate      , m_StrInterestRateA30B , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[11].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[2].merchgrepint[11].MSF_rate      , m_StrMSFRateA30B      , sizeof(pMerchantGroupRecord->merchgrp[2].merchgrepint[11].MSF_rate      ));
	
	memcpy( pMerchantGroupRecord->merchgrp[3].Amount_limit                  , m_StrAmount4         , sizeof(pMerchantGroupRecord->merchgrp[3].Amount_limit                  ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[0].nbr_of_months , nbr1                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[0].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[0].INT_rate      , m_StrInterestRateA31 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[0].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[0].MSF_rate      , m_StrMSFRateA31      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[0].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[1].nbr_of_months , nbr2                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[1].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[1].INT_rate      , m_StrInterestRateA32 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[1].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[1].MSF_rate      , m_StrMSFRateA32      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[1].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[2].nbr_of_months , nbr3                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[2].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[2].INT_rate      , m_StrInterestRateA33 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[2].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[2].MSF_rate      , m_StrMSFRateA33      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[2].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[3].nbr_of_months , nbr4                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[3].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[3].INT_rate      , m_StrInterestRateA34 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[3].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[3].MSF_rate      , m_StrMSFRateA34      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[3].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[4].nbr_of_months , nbr5                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[4].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[4].INT_rate      , m_StrInterestRateA35 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[4].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[4].MSF_rate      , m_StrMSFRateA35      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[4].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[5].nbr_of_months , nbr6                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[5].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[5].INT_rate      , m_StrInterestRateA36 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[5].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[5].MSF_rate      , m_StrMSFRateA36      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[5].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[6].nbr_of_months , nbr7                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[6].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[6].INT_rate      , m_StrInterestRateA37 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[6].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[6].MSF_rate      , m_StrMSFRateA37      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[6].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[7].nbr_of_months , nbr8                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[7].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[7].INT_rate      , m_StrInterestRateA38 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[7].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[7].MSF_rate      , m_StrMSFRateA38      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[7].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[8].nbr_of_months , nbr9                 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[8].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[8].INT_rate      , m_StrInterestRateA39 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[8].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[8].MSF_rate      , m_StrMSFRateA39      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[8].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[9].nbr_of_months , nbr10                , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[9].INT_rate      , m_StrInterestRateA40 , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[9].MSF_rate      , m_StrMSFRateA40      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[10].nbr_of_months , nbr10A                , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[10].INT_rate      , m_StrInterestRateA40A , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[10].MSF_rate      , m_StrMSFRateA40A      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].MSF_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[11].nbr_of_months , nbr10B                , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].nbr_of_months ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[11].INT_rate      , m_StrInterestRateA40B , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].INT_rate      ));
	memcpy( pMerchantGroupRecord->merchgrp[3].merchgrepint[11].MSF_rate      , m_StrMSFRateA40B      , sizeof(pMerchantGroupRecord->merchgrp[3].merchgrepint[9].MSF_rate      ));


	return TRUE;
}

void CMerchantGroupGeneral::EnableMerchantGroupDialog(BOOL nEnable)
{
	m_CntlMechantGroupIdFld.EnableWindow(nEnable);
	m_CntlEffectiveDateFld.EnableWindow(nEnable);
	m_CntlAmount1.EnableWindow(nEnable);
	m_CntlAmount2.EnableWindow(nEnable);
	m_CntlAmount3.EnableWindow(nEnable);
	m_CntlAmount4.EnableWindow(nEnable);

	m_CntlInterestRateA1.EnableWindow(nEnable);
	m_CntlInterestRateA2.EnableWindow(nEnable);
	m_CntlInterestRateA3.EnableWindow(nEnable);
	m_CntlInterestRateA4.EnableWindow(nEnable);
	m_CntlInterestRateA5.EnableWindow(nEnable);
	m_CntlInterestRateA6.EnableWindow(nEnable);
	m_CntlInterestRateA7.EnableWindow(nEnable);
	m_CntlInterestRateA8.EnableWindow(nEnable);
	m_CntlInterestRateA9.EnableWindow(nEnable);
	m_CntlInterestRateA10.EnableWindow(nEnable);
	m_CntlInterestRateA10A.EnableWindow(nEnable);
	m_CntlInterestRateA10B.EnableWindow(nEnable);

	m_CntlInterestRateA11.EnableWindow(nEnable);
	m_CntlInterestRateA12.EnableWindow(nEnable);
	m_CntlInterestRateA13.EnableWindow(nEnable);
	m_CntlInterestRateA14.EnableWindow(nEnable);
	m_CntlInterestRateA15.EnableWindow(nEnable);
	m_CntlInterestRateA16.EnableWindow(nEnable);
	m_CntlInterestRateA17.EnableWindow(nEnable);
	m_CntlInterestRateA18.EnableWindow(nEnable);
	m_CntlInterestRateA19.EnableWindow(nEnable);
	m_CntlInterestRateA20.EnableWindow(nEnable);
	m_CntlInterestRateA20A.EnableWindow(nEnable);
	m_CntlInterestRateA20B.EnableWindow(nEnable);

	m_CntlInterestRateA21.EnableWindow(nEnable);
	m_CntlInterestRateA22.EnableWindow(nEnable);
	m_CntlInterestRateA23.EnableWindow(nEnable);
	m_CntlInterestRateA24.EnableWindow(nEnable);
	m_CntlInterestRateA25.EnableWindow(nEnable);
	m_CntlInterestRateA26.EnableWindow(nEnable);
	m_CntlInterestRateA27.EnableWindow(nEnable);
	m_CntlInterestRateA28.EnableWindow(nEnable);
	m_CntlInterestRateA29.EnableWindow(nEnable);
	m_CntlInterestRateA30.EnableWindow(nEnable);
	m_CntlInterestRateA30A.EnableWindow(nEnable);
	m_CntlInterestRateA30B.EnableWindow(nEnable);

	m_CntlInterestRateA31.EnableWindow(nEnable);
	m_CntlInterestRateA32.EnableWindow(nEnable);
	m_CntlInterestRateA33.EnableWindow(nEnable);
	m_CntlInterestRateA34.EnableWindow(nEnable);
	m_CntlInterestRateA35.EnableWindow(nEnable);
	m_CntlInterestRateA36.EnableWindow(nEnable);
	m_CntlInterestRateA37.EnableWindow(nEnable);
	m_CntlInterestRateA38.EnableWindow(nEnable);
	m_CntlInterestRateA39.EnableWindow(nEnable);
	m_CntlInterestRateA40.EnableWindow(nEnable);
	m_CntlInterestRateA40A.EnableWindow(nEnable);
	m_CntlInterestRateA40B.EnableWindow(nEnable);
	
	m_CntlMSFRateA1.EnableWindow(nEnable);
	m_CntlMSFRateA2.EnableWindow(nEnable);
	m_CntlMSFRateA3.EnableWindow(nEnable);
	m_CntlMSFRateA4.EnableWindow(nEnable);
	m_CntlMSFRateA5.EnableWindow(nEnable);
	m_CntlMSFRateA6.EnableWindow(nEnable);
	m_CntlMSFRateA7.EnableWindow(nEnable);
	m_CntlMSFRateA8.EnableWindow(nEnable);
	m_CntlMSFRateA9.EnableWindow(nEnable);
	m_CntlMSFRateA10.EnableWindow(nEnable);
	m_CntlMSFRateA10A.EnableWindow(nEnable);
	m_CntlMSFRateA10B.EnableWindow(nEnable);

	m_CntlMSFRateA11.EnableWindow(nEnable);
	m_CntlMSFRateA12.EnableWindow(nEnable);
	m_CntlMSFRateA13.EnableWindow(nEnable);
	m_CntlMSFRateA14.EnableWindow(nEnable);
	m_CntlMSFRateA15.EnableWindow(nEnable);
	m_CntlMSFRateA16.EnableWindow(nEnable);
	m_CntlMSFRateA17.EnableWindow(nEnable);
	m_CntlMSFRateA18.EnableWindow(nEnable);
	m_CntlMSFRateA19.EnableWindow(nEnable);
	m_CntlMSFRateA20.EnableWindow(nEnable);
	m_CntlMSFRateA20A.EnableWindow(nEnable);
	m_CntlMSFRateA20B.EnableWindow(nEnable);

	m_CntlMSFRateA21.EnableWindow(nEnable);
	m_CntlMSFRateA22.EnableWindow(nEnable);
	m_CntlMSFRateA23.EnableWindow(nEnable);
	m_CntlMSFRateA24.EnableWindow(nEnable);
	m_CntlMSFRateA25.EnableWindow(nEnable);
	m_CntlMSFRateA26.EnableWindow(nEnable);
	m_CntlMSFRateA27.EnableWindow(nEnable);
	m_CntlMSFRateA28.EnableWindow(nEnable);
	m_CntlMSFRateA29.EnableWindow(nEnable);
	m_CntlMSFRateA30.EnableWindow(nEnable);
	m_CntlMSFRateA30A.EnableWindow(nEnable);
	m_CntlMSFRateA30B.EnableWindow(nEnable);

	m_CntlMSFRateA31.EnableWindow(nEnable);
	m_CntlMSFRateA32.EnableWindow(nEnable);
	m_CntlMSFRateA33.EnableWindow(nEnable);
	m_CntlMSFRateA34.EnableWindow(nEnable);
	m_CntlMSFRateA35.EnableWindow(nEnable);
	m_CntlMSFRateA36.EnableWindow(nEnable);
	m_CntlMSFRateA37.EnableWindow(nEnable);
	m_CntlMSFRateA38.EnableWindow(nEnable);
	m_CntlMSFRateA39.EnableWindow(nEnable);
	m_CntlMSFRateA40.EnableWindow(nEnable);
	m_CntlMSFRateA40A.EnableWindow(nEnable);
	m_CntlMSFRateA40B.EnableWindow(nEnable);

}


BOOL CMerchantGroupGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_bGeneral = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_ADD )
		{
			PopulateGeneralDialogFromDatabase ( sMerchantGroupRecordOld);
		}
		if(m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
		{
			EnableMerchantGroupDialog(TRUE);
		}
		else
		{
			EnableMerchantGroupDialog(FALSE);
		}
	}

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantGroupGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{

	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantGroupGeneral::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18]="";   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
	CHAR strDisplayErrMsg[250];
    CString temp;
	CHAR s_merch_ctr[4]   = "";
	
	memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrMechantGroupIdFld);
	    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt (dest_str);
	merch_ctr++;
   if ( merch_ctr > 999 )
      merch_ctr = 0;
   sprintf( s_merch_ctr, "%03d", merch_ctr );
   memcpy( &dest_str[14], s_merch_ctr, 3 );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"MGF01", sizeof (sHistoryData.file_name));
	memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    temp = strDate; 
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

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
	/* function PopulateHistoryTable */
}

BOOL CMerchantGroupGeneral::CompareStructures()
{
	CString Deferred_start_date;
    CString Deferred_end_date;
    CMyUtils myUtilsObject;

	if ( m_bGeneral == TRUE )
	{
		if (strcmp ( m_StrAmount1, (char *)sMerchantGroupRecordOld.merchgrp[0].Amount_limit) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrAmount1, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].Amount_limit, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Amount", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA1, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[0].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA1, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[0].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA1, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[0].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA1, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[0].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA2, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[1].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA2, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[1].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA2, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[1].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA2, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[1].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA3, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[2].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA3, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[2].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA3, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[2].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA3, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[2].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA4, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[3].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA4, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[3].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA4, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[3].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA4, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[3].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA5, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[4].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA5, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[4].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA5, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[4].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA5, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[4].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA6, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[5].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA6, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[5].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA6, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[5].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA6, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[5].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA7, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[6].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA7, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[6].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA7, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[6].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA7, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[6].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA8, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[7].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA8, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[7].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA8, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[7].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA8, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[7].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA9, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[8].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA9, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[8].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA9, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[8].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA9, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[8].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA10, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[9].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA10, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[9].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA10, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[9].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA10, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[9].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA10A, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[10].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA10A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[10].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA10A, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[10].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA10A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[10].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA10B, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[11].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA10B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[11].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA10B, (char *)sMerchantGroupRecordOld.merchgrp[0].merchgrepint[11].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA10B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[0].merchgrepint[11].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		/**********************************************************************************************/
		if (strcmp ( m_StrAmount2, (char *)sMerchantGroupRecordOld.merchgrp[1].Amount_limit) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrAmount2, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].Amount_limit, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Amount", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA11, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[0].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA11, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[0].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA11, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[0].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA11, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[0].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA12, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[1].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA12, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[1].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA12, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[1].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA12, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[1].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA13, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[2].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA13, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[2].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA13, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[2].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA13, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[2].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA14, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[3].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA14, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[3].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA14, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[3].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA14, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[3].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA15, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[4].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA15, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[4].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA15, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[4].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA15, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[4].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA16, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[5].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA16, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[5].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA16, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[5].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA16, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[5].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA17, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[6].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA17, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[6].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA17, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[6].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA17, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[6].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA18, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[7].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA18, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[7].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA18, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[7].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA18, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[7].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA19, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[8].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA19, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[8].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA19, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[8].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA19, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[8].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA20, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[9].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA20, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[9].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA20, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[9].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA20, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[9].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA20A, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[10].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA20A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[10].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA20A, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[10].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA20A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[10].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA20B, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[11].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA20B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[11].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA20B, (char *)sMerchantGroupRecordOld.merchgrp[1].merchgrepint[11].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA20B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[1].merchgrepint[11].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		/**********************************************************************************************/
		if (strcmp ( m_StrAmount3, (char *)sMerchantGroupRecordOld.merchgrp[2].Amount_limit) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrAmount3, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].Amount_limit, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Amount", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA21, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[0].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA21, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[0].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA21, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[0].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA21, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[0].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA22, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[1].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA22, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[1].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA22, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[1].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA22, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[1].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA23, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[2].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA23, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[2].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA23, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[2].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA23, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[2].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA24, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[3].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA24, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[3].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA24, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[3].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA24, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[3].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA25, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[4].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA25, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[4].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA25, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[4].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA25, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[4].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA26, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[5].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA26, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[5].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA26, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[5].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA26, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[5].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA27, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[6].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA27, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[6].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA27, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[6].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA27, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[6].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA28, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[7].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA28, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[7].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA28, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[7].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA28, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[7].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA29, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[8].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA29, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[8].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA29, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[8].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA29, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[8].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA30, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[9].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA30, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[9].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA30, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[9].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA30, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[9].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA30A, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[10].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA30A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[10].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA30A, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[10].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA30A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[10].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA30B, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[11].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA30B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[11].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA30B, (char *)sMerchantGroupRecordOld.merchgrp[2].merchgrepint[11].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA30B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[2].merchgrepint[11].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		/**********************************************************************************************/
		if (strcmp ( m_StrAmount4, (char *)sMerchantGroupRecordOld.merchgrp[3].Amount_limit) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrAmount4, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].Amount_limit, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Amount", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA31, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[0].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA31, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[0].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA31, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[0].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA31, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[0].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA32, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[1].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA32, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[1].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA32, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[1].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA32, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[1].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA33, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[2].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA33, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[2].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA33, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[2].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA33, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[2].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA34, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[3].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA34, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[3].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA34, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[3].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA34, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[3].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA35, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[4].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA35, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[4].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA35, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[4].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA35, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[4].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA36, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[5].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA36, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[5].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA36, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[5].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA36, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[5].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA37, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[6].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA37, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[6].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA37, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[6].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA37, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[6].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA38, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[7].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA38, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[7].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA38, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[7].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA38, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[7].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA39, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[8].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA39, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[8].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA39, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[8].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA39, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[8].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrInterestRateA40, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[9].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA40, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[9].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA40, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[9].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA40, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[9].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		

		if (strcmp ( m_StrInterestRateA40A, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[10].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA40A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[10].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA40A, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[10].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA40A, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[10].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrInterestRateA40B, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[11].INT_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrInterestRateA40B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[11].INT_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "INT Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMSFRateA40B, (char *)sMerchantGroupRecordOld.merchgrp[3].merchgrepint[11].MSF_rate) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMSFRateA40B, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantGroupRecordOld.merchgrp[3].merchgrepint[11].MSF_rate, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "MSF Rate ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
	}


	
	return TRUE;
}

void CMerchantGroupGeneral::OnChangeMsfRateA20B() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
