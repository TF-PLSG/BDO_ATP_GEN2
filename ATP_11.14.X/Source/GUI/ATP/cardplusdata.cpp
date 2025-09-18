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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\cardplusdata.cpp  $
   
      Rev 1.6   Dec 01 2003 11:28:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:36   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:46   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 13:37:42   dperson
   Updated comments
   
      Rev 1.2   May 10 1999 10:58:58   dperson
   Added PVCS header and 
   standardized error messages
*   
************************************************************************************/
// CardPlusData.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardPlusData.h"
#include "SharedMem.h"

extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardPlusData property page

IMPLEMENT_DYNCREATE(CCardPlusData, CPropertyPage)

CCardPlusData::CCardPlusData() : CPropertyPage(CCardPlusData::IDD)
{
	//{{AFX_DATA_INIT(CCardPlusData)
	m_Accessories = FALSE;
	m_Accident = FALSE;
	m_AutoGas = FALSE;
	m_Battaries = FALSE;
	m_CardNbr = _T("");
	m_CardType = _T("");
	m_CleanGo = FALSE;
	m_CrindAuto = FALSE;
	m_Diesel = FALSE;
	m_Driver = FALSE;
	m_DriverId = FALSE;
	m_FleetId = FALSE;
	m_Gold = FALSE;
	m_LastOdometer = _T("");
	m_LitreLimit = _T("");
	m_Meals = FALSE;
	m_Odometer = FALSE;
	m_Oil = FALSE;
	m_OrderNbr = FALSE;
	m_Parking = FALSE;
	m_RegNbr = _T("");
	m_Repairs = FALSE;
	m_Service = FALSE;
	m_Shop = FALSE;
	m_Signature = FALSE;
	m_Super = FALSE;
	m_SuperLL = FALSE;
	m_TimeLimit = _T("");
	m_Tyres = FALSE;
	m_Unleaded = FALSE;
	m_ValueLimit = _T("");
	m_VehicleId = FALSE;
	m_strComEdit = _T("");
	m_ActionOk = -1;
	m_ActionReturn = -1;
	//}}AFX_DATA_INIT
}

CCardPlusData::~CCardPlusData()
{
}

void CCardPlusData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardPlusData)
	DDX_Control(pDX, IDC_ACTIONRETURN, m_ctrlActionReturn);
	DDX_Control(pDX, IDC_ACTIONOK, m_ctrlActionOk);
	DDX_Control(pDX, IDC_CARDTYPE, m_ctrlCardType);
	DDX_Control(pDX, IDC_CARDNBR, m_ctrlCardNbr);
	DDX_Check(pDX, IDC_ACCESSORIES, m_Accessories);
	DDX_Check(pDX, IDC_ACCIDENT, m_Accident);
	DDX_Check(pDX, IDC_AUTOGAS, m_AutoGas);
	DDX_Check(pDX, IDC_BATTARIES, m_Battaries);
	DDX_Text(pDX, IDC_CARDNBR, m_CardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_CardType);
	DDX_Check(pDX, IDC_CLEANGO, m_CleanGo);
	DDX_Check(pDX, IDC_CRINDAUTO, m_CrindAuto);
	DDX_Check(pDX, IDC_DIESEL, m_Diesel);
	DDX_Check(pDX, IDC_DRIVER, m_Driver);
	DDX_Check(pDX, IDC_DRIVERID, m_DriverId);
	DDX_Check(pDX, IDC_FLEETID, m_FleetId);
	DDX_Check(pDX, IDC_GOLD, m_Gold);
	DDX_Text(pDX, IDC_LASTODOMETER, m_LastOdometer);
	DDX_Text(pDX, IDC_LITRELIMIT, m_LitreLimit);
	DDX_Check(pDX, IDC_MEALS, m_Meals);
	DDX_Check(pDX, IDC_ODOMETER, m_Odometer);
	DDX_Check(pDX, IDC_OIL, m_Oil);
	DDX_Check(pDX, IDC_ORDERNBR, m_OrderNbr);
	DDX_Check(pDX, IDC_PARKING, m_Parking);
	DDX_Text(pDX, IDC_REG_NBR, m_RegNbr);
	DDX_Check(pDX, IDC_REPAIRS, m_Repairs);
	DDX_Check(pDX, IDC_SERVICE, m_Service);
	DDX_Check(pDX, IDC_SHOP, m_Shop);
	DDX_Check(pDX, IDC_SIGNATURE, m_Signature);
	DDX_Check(pDX, IDC_SUPER, m_Super);
	DDX_Check(pDX, IDC_SUPERLL, m_SuperLL);
	DDX_Text(pDX, IDC_TIMELIMIT, m_TimeLimit);
	DDX_Check(pDX, IDC_TYRES, m_Tyres);
	DDX_Check(pDX, IDC_UNLEADED, m_Unleaded);
	DDX_Text(pDX, IDC_VALUELIMIT, m_ValueLimit);
	DDX_Check(pDX, IDC_VEHICLEID, m_VehicleId);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	DDX_Radio(pDX, IDC_ACTIONOK, m_ActionOk);
	DDX_Radio(pDX, IDC_ACTIONRETURN, m_ActionReturn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardPlusData, CPropertyPage)
	//{{AFX_MSG_MAP(CCardPlusData)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardPlusData message handlers

BOOL CCardPlusData::OnSetActive() 
{
	if (m_ActionOk == 1)
   {
      m_ctrlActionOk.SetCheck(1);
      m_ctrlActionReturn.SetCheck(0);
   }

	if (m_ActionReturn == 1)
   {
      m_ctrlActionOk.SetCheck(0);
      m_ctrlActionReturn.SetCheck(1);
   }

	
	return CPropertyPage::OnSetActive();
}

BOOL CCardPlusData::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
