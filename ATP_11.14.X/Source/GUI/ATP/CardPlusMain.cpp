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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardPlusMain.cpp  $
   
      Rev 1.6   Dec 01 2003 11:28:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:06   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 10:59:58   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// CardPlusMain.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardPlusMain.h"
#include "BinList.h"
#include "SharedMem.h"

extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;

/////////////////////////////////////////////////////////////////////////////
// CCardPlusMain property page

IMPLEMENT_DYNCREATE(CCardPlusMain, CPropertyPage)

CCardPlusMain::CCardPlusMain() : CPropertyPage(CCardPlusMain::IDD)
{
	//{{AFX_DATA_INIT(CCardPlusMain)
	m_strCardNbr = _T("");
	m_strCardType = _T("");
	m_strComEdit = _T("");
	//}}AFX_DATA_INIT
}

CCardPlusMain::~CCardPlusMain()
{
}

void CCardPlusMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardPlusMain)
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_CARDTYPE, m_ctrlCardType);
	DDX_Control(pDX, IDC_CARDNBR, m_ctrlCardNbr);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_CBString(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardPlusMain, CPropertyPage)
	//{{AFX_MSG_MAP(CCardPlusMain)
	ON_BN_CLICKED(IDC_LIST, OnList)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_EN_CHANGE(IDC_CARDNBR, OnChangeCardnbr)
	ON_CBN_SELCHANGE(IDC_CARDTYPE, OnSelchangeCardtype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardPlusMain message handlers

void CCardPlusMain::OnList() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	listDlg.m_nDialogMode = MODE_CCF;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strCardNbr = listDlg.m_strCardNbr;
		if (listDlg.m_strCardType == 'C')
			m_ctrlCardType.SetCurSel(0);
		if (listDlg.m_strCardType == 'D')
			m_ctrlCardType.SetCurSel(1);
		if (listDlg.m_strCardType == 'F')
			m_ctrlCardType.SetCurSel(2);
		if (listDlg.m_strCardType == 'L')
			m_ctrlCardType.SetCurSel(3);

		CleanPages();    
	}

    if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
	
}

BOOL CCardPlusMain::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	
	m_strCardNbr.Empty();
	m_strCardType.Empty();
	m_ctrlCardType.SetCurSel(-1);
   CleanPages();

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_COMMENTS)->SetFont (&m_font);

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_View.EnableWindow(FALSE);
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCardPlusMain::OnView() 
{
	if (!GetFCFData())
		return;

	SetKeys();
	m_pCPSheet->SetActivePage(1);

	
}

void CCardPlusMain::SetKeys()
{
	pDataPage->m_CardNbr = m_strCardNbr;
	pDataPage->m_CardType = m_strCardType;

}

BOOL CCardPlusMain::GetFCFData()
{
	char ErrorMsg[200];
	FCF02	sFCF02Record;


	if (! KeyEntered())
		return FALSE;

	ZeroMemory(&sFCF02Record,sizeof(FCF02)) ;

    memcpy ((LPSTR)(sFCF02Record.primary_key.card_type), m_strCardType.Left(1) , sizeof (sFCF02Record.primary_key.card_type));
    memcpy ((LPSTR)(sFCF02Record.primary_key.card_nbr), m_strCardNbr , sizeof (sFCF02Record.primary_key.card_nbr));

	int rcode = txdsapi_get_record((pBYTE)&sFCF02Record, sizeof(FCF02),FCF02_DATA, ErrorMsg);
	if( rcode == PTEMSG_OK )
	{

		// Populate all the FCF fields
		pDataPage->m_ActionReturn = (BOOL)sFCF02Record.action_code[0] - 48;
      pDataPage->m_ActionOk = abs(pDataPage->m_ActionReturn - 1);

		pDataPage->m_RegNbr.Format((LPCTSTR)sFCF02Record.registration_number);
		pDataPage->m_LastOdometer.Format((LPCTSTR)sFCF02Record.last_odometer_reading);
		pDataPage->m_ValueLimit.Format((LPCTSTR)sFCF02Record.value_limits);
		pDataPage->m_LitreLimit.Format((LPCTSTR)sFCF02Record.litre_limits);
		pDataPage->m_TimeLimit.Format((LPCTSTR)sFCF02Record.time_limits);

		pDataPage->m_Accessories = atoi((char *)sFCF02Record.accessories);
		pDataPage->m_Accident = atoi((char *)sFCF02Record.accident_and_damage);
		pDataPage->m_AutoGas = atoi((char *)sFCF02Record.autogas);
		pDataPage->m_Battaries = atoi((char *)sFCF02Record.batteries);
		pDataPage->m_CleanGo = atoi((char *)sFCF02Record.clean_and_go);
		pDataPage->m_CrindAuto = atoi((char *)sFCF02Record.crind_autocard);
		pDataPage->m_Diesel = atoi((char *)sFCF02Record.diesel);
		pDataPage->m_Driver = atoi((char *)sFCF02Record.driver_flag);
		pDataPage->m_DriverId = atoi((char *)sFCF02Record.driver_id_flag);
		pDataPage->m_FleetId = atoi((char *)sFCF02Record.fleet_id_flag);
		pDataPage->m_Gold = atoi((char *)sFCF02Record.accessories);
		pDataPage->m_Meals = atoi((char *)sFCF02Record.meals);
		pDataPage->m_Odometer = atoi((char *)sFCF02Record.odometer_required);
		pDataPage->m_Oil = atoi((char *)sFCF02Record.oil);
		pDataPage->m_OrderNbr = atoi((char *)sFCF02Record.order_number_required);
		pDataPage->m_Parking = atoi((char *)sFCF02Record.parking );
		pDataPage->m_Repairs = atoi((char *)sFCF02Record.repairs_maint);
		pDataPage->m_Service = atoi((char *)sFCF02Record.service);
		pDataPage->m_Shop = atoi((char *)sFCF02Record.shop);
		pDataPage->m_Signature = atoi((char *)sFCF02Record.signature_required);
		pDataPage->m_Super = atoi((char *)sFCF02Record.super);
		pDataPage->m_SuperLL = atoi((char *)sFCF02Record.super_low_lead);
		pDataPage->m_Tyres = atoi((char *)sFCF02Record.tires);
		pDataPage->m_Unleaded = atoi((char *)sFCF02Record.unleaded);
		pDataPage->m_VehicleId = atoi((char *)sFCF02Record.vehicle_id_flag);
				
	}
	return TRUE;
}


BOOL CCardPlusMain::KeyEntered()
{
	UpdateData(TRUE);

	if (m_strCardType.IsEmpty())
	{
		m_ComEdit.SetWindowText("Please enter Card Type");
		m_ctrlCardType.SetFocus();
		return FALSE;
	}

	if (m_strCardNbr.IsEmpty())
	{
		m_ComEdit.SetWindowText("Please enter Card Number");
		m_ctrlCardNbr.SetFocus();
		return FALSE;
	}

	return TRUE;
}

void CCardPlusMain::CleanPages() 
{
	//Clear key variables 
	pDataPage->m_CardNbr.Empty();
	pDataPage->m_CardType.Empty();

	pDataPage->m_RegNbr.Empty();
	pDataPage->m_LastOdometer.Empty();
	pDataPage->m_ValueLimit.Empty();
	pDataPage->m_LitreLimit.Empty();
	pDataPage->m_TimeLimit.Empty();

	//Clear variables of DataPage
	pDataPage->m_Accessories = FALSE;
	pDataPage->m_Accident = FALSE;
	pDataPage->m_AutoGas = FALSE;
	pDataPage->m_Battaries = FALSE;
	pDataPage->m_CleanGo = FALSE;
	pDataPage->m_CrindAuto = FALSE;
	pDataPage->m_Diesel = FALSE;
	pDataPage->m_Driver = FALSE;
	pDataPage->m_DriverId = FALSE;
	pDataPage->m_FleetId = FALSE;
	pDataPage->m_Gold = FALSE;
	pDataPage->m_Meals = FALSE;
	pDataPage->m_Odometer = FALSE;
	pDataPage->m_Oil = FALSE;
	pDataPage->m_OrderNbr = FALSE;
	pDataPage->m_Parking = FALSE;
	pDataPage->m_Repairs = FALSE;
	pDataPage->m_Service = FALSE;
	pDataPage->m_Shop = FALSE;
	pDataPage->m_Signature = FALSE;
	pDataPage->m_Super = FALSE;
	pDataPage->m_SuperLL = FALSE;
	pDataPage->m_Tyres = FALSE;
	pDataPage->m_Unleaded = FALSE;
	pDataPage->m_VehicleId = FALSE;

}

void CCardPlusMain::OnChangeCardnbr() 
{
	CleanPages();
	
}

void CCardPlusMain::OnSelchangeCardtype() 
{
	CleanPages();
	
}

BOOL CCardPlusMain::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
