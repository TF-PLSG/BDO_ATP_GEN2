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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\MerchantTerminalGeneral.cpp  $
   
      Rev 1.12   Feb 10 2005 16:14:28   mbysani
   Added new device type PAY PASS scr 13560
   
      Rev 1.11   Jul 21 2004 12:42:52   lmitchel
   SCR1388: Added new device type  = SMART CARD; dcf01.device_type  = '07'.  Added NONE option to encryption method in Keys dialog.
   
      Rev 1.10   Jul 16 2004 16:07:44   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.9   Dec 01 2003 11:29:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   May 12 2000 14:30:50   iarustam
    
   
      Rev 1.7   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:56   iarustam
   Initial Release
   
      Rev 1.4   Jun 02 1999 16:25:28   iarustam
   Bug fixes
   
      Rev 1.3   Apr 29 1999 14:56:40   iarustam
   Bug fixes
   
      Rev 1.2   Mar 22 1999 11:12:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 16 1999 16:30:08   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:46   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:42   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:24   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:48   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:00   iarustam
    
*
************************************************************************************/
// MerchantTerminalGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalGeneral.h"
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
// CMerchantTerminalGeneral property page

IMPLEMENT_DYNCREATE(CMerchantTerminalGeneral, CPropertyPage)

CMerchantTerminalGeneral::CMerchantTerminalGeneral() : CPropertyPage(CMerchantTerminalGeneral::IDD)
{
	//{{AFX_DATA_INIT(CMerchantTerminalGeneral)
	m_StrDateAddedFld = _T("");
	m_StrDepartmentNumFld = _T("");
	m_StrDeviceType = _T("");
	m_StrIndustryCodeFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPinPadIndicatorFld = _T("");
	m_StrStatus = _T("");
	m_StrTerminalIDFld = _T("");
	//m_StrChargeDescFld = _T("");
	m_StrCategoryCodeFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrABAFld = _T("");
	m_StrContactFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrShortNameFld = _T("");
	//m_StrDinersChargeDescrFld = _T("");
	m_StrEMVCapabilityFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalGeneral::~CMerchantTerminalGeneral()
{
}

void CMerchantTerminalGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalGeneral)
//	DDX_Control(pDX, IDC_MP_DEVICE_CHECK, m_CntlMPDeviceChk);
	//DDX_Control(pDX, IDC_DINERS_CHARGE_DESCR_FLD, m_CntlDinersChargeDescrFld);
	DDX_Control(pDX, IDC_SHORT_NAME_FLD, m_CntlShortNameFld);
	//DDX_Control(pDX, IDC_CHARGE_DESC_FLD, m_CntlChargeDescFld);
	DDX_Control(pDX, IDC_ABA_FLD, m_CntlABAFld);
	DDX_Control(pDX, IDC_EMV_CAPABILITY, m_CntlEMVCapabilityFld);
	DDX_Control(pDX, IDC_CONTACT_FLD, m_CntlContactFld);
	DDX_Control(pDX, IDC_MCC_FLD, m_CntlCategoryCodeFld);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Control(pDX, IDC_PIN_PAD_INDICATOR_FLD, m_CntlPinPadIndicatorFld);
	DDX_Control(pDX, IDC_INDUSTRY_CODE_FLD, m_CntlIndustryCodeFld);
	DDX_Control(pDX, IDC_ENABLE_TOTALS_CHECK, m_CntlEnableTotals);
	DDX_Control(pDX, IDC_ENABLE_STATISTICS_CHECK, m_CntlEnableStatistics);
	DDX_Control(pDX, IDC_DEVICE_TYPE, m_CntlDeviceType);
	DDX_Control(pDX, IDC_DEPARTMENT_NUM_FLD, m_CntlDepartmentNumFld);
	DDX_Control(pDX, IDC_DATE_ADDED_FLD, m_CntlDateAddedFld);
	DDX_Text(pDX, IDC_DATE_ADDED_FLD, m_StrDateAddedFld);
	DDX_Text(pDX, IDC_DEPARTMENT_NUM_FLD, m_StrDepartmentNumFld);
	DDV_MaxChars(pDX, m_StrDepartmentNumFld, 2);
	DDX_CBString(pDX, IDC_DEVICE_TYPE, m_StrDeviceType);
	DDX_Text(pDX, IDC_INDUSTRY_CODE_FLD, m_StrIndustryCodeFld);
	DDV_MaxChars(pDX, m_StrIndustryCodeFld, 3);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_PIN_PAD_INDICATOR_FLD, m_StrPinPadIndicatorFld);
	DDV_MaxChars(pDX, m_StrPinPadIndicatorFld, 1);
	DDX_CBString(pDX, IDC_STATUS, m_StrStatus);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	//DDX_Text(pDX, IDC_CHARGE_DESC_FLD, m_StrChargeDescFld);
	//DDV_MaxChars(pDX, m_StrChargeDescFld, 23);
	DDX_Text(pDX, IDC_MCC_FLD, m_StrCategoryCodeFld);
	DDV_MaxChars(pDX, m_StrCategoryCodeFld, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ABA_FLD, m_StrABAFld);
	DDV_MaxChars(pDX, m_StrABAFld, 9);
	DDX_Text(pDX, IDC_CONTACT_FLD, m_StrContactFld);
	DDV_MaxChars(pDX, m_StrContactFld, 20);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_SHORT_NAME_FLD, m_StrShortNameFld);
	DDV_MaxChars(pDX, m_StrShortNameFld, 9);
	//DDX_Text(pDX, IDC_DINERS_CHARGE_DESCR_FLD, m_StrDinersChargeDescrFld);
	//DDV_MaxChars(pDX, m_StrDinersChargeDescrFld, 23);
	DDX_Text(pDX, IDC_EMV_CAPABILITY, m_StrEMVCapabilityFld);
	DDV_MaxChars(pDX, m_StrEMVCapabilityFld, 6);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalGeneral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalGeneral message handlers

BOOL CMerchantTerminalGeneral::OnInitDialog() 
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
	m_bValidDay = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableGeneralDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateGeneralDialogFromDatabase ( sTerminalRecordOld );
	}  

	m_StrStatus = "ACTIVE";
	m_StrDeviceType = "EFTPOS_ISO";
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantTerminalGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW && m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantTerminalGeneral::EnableGeneralDialog ( int nStatus )
{
	m_CntlDepartmentNumFld.EnableWindow(nStatus);
	m_CntlDeviceType.EnableWindow(nStatus);
	m_CntlIndustryCodeFld.EnableWindow(nStatus);
	m_CntlPinPadIndicatorFld.EnableWindow(nStatus);
	m_CntlStatus.EnableWindow(nStatus);
	//m_CntlChargeDescFld.EnableWindow(nStatus);
	//m_CntlDinersChargeDescrFld.EnableWindow(nStatus);
	m_CntlCategoryCodeFld.EnableWindow(nStatus);
	m_CntlABAFld.EnableWindow(nStatus);
	m_CntlContactFld.EnableWindow(nStatus);
	m_CntlShortNameFld.EnableWindow(nStatus);
	m_CntlEnableStatistics.EnableWindow(nStatus);
	m_CntlEnableTotals.EnableWindow(nStatus);
	m_CntlEMVCapabilityFld.EnableWindow(nStatus);
//   m_CntlNRICFld.EnableWindow(nStatus);
//	m_CntlMPDeviceChk.EnableWindow(nStatus);

	m_CntlStatus.SetCurSel(0);
	m_CntlDeviceType.SetCurSel(0);
	
}

BOOL CMerchantTerminalGeneral::PopulateGeneralDialogFromDatabase ( DCF01 sTerminalRecord )
{
	CString temp;
	CMyUtils myUtilsObject;

	if ( m_nDialogMode != MODE_VIEW && m_bGeneral )	
		EnableGeneralDialog(TRUE);

//	ZeroMemory ( &sTerminalRecord, sizeof ( DCF01) );

	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrDateAddedFld = sTerminalRecord.date_added;
	m_StrDepartmentNumFld = sTerminalRecord.dept_nbr;
	m_StrDeviceType = sTerminalRecord.device_type;
	m_StrIndustryCodeFld = sTerminalRecord.industry_code;
	m_StrMerchantIDFld = sTerminalRecord.merchant_id;
	m_StrPinPadIndicatorFld = sTerminalRecord.pin_type;
	m_StrStatus = sTerminalRecord.status;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;
	//m_StrChargeDescFld = sTerminalRecord.amex_charge_desc;
	//m_StrDinersChargeDescrFld = sTerminalRecord.diners_charge_desc;
	m_StrCategoryCodeFld = sTerminalRecord.category_code;
	m_StrShortNameFld = sTerminalRecord.short_name;
	m_StrABAFld = sTerminalRecord.aba_nbr;
	m_StrContactFld = sTerminalRecord.contact;
	m_StrEMVCapabilityFld = sTerminalRecord.current_bus_date;
//   m_StrNRICFld = sTerminalRecord.nric;

	temp                 = sTerminalRecord.totals_ind;
	if ( temp == "Y" )
		m_CntlEnableTotals.SetCheck(1);
	else if ( temp == "N")
		m_CntlEnableTotals.SetCheck(0);

	temp                 = sTerminalRecord.statistics_ind;
	if ( temp == "Y" )
		m_CntlEnableStatistics.SetCheck(1);
	else if ( temp == "N")
		m_CntlEnableStatistics.SetCheck(0);

	if (!myUtilsObject.IsEmpty (m_StrDateAddedFld, this))
			m_StrDateAddedFld = m_StrDateAddedFld.Right(2) + "/" + m_StrDateAddedFld.Mid(4,2) + "/" + m_StrDateAddedFld.Left(4);

	if ( m_StrStatus == "A")
		m_StrStatus = "ACTIVE";
	else if ( m_StrStatus == "I" )
		m_StrStatus = "INACTIVE";
	else if ( m_StrStatus == "R")
		m_StrStatus = "RESTRICTED";

/*	if ( m_StrDeviceType == "01")
		m_StrDeviceType = "CRIND";*/
	if ( m_StrDeviceType == "02")
		m_StrDeviceType = "EDC";
	else if ( m_StrDeviceType == "03")
		m_StrDeviceType = "EFTPOS ISO";
	else if ( m_StrDeviceType == "04")
		m_StrDeviceType = "EFTPOS VISA I";
	else if ( m_StrDeviceType == "05")
		m_StrDeviceType = "VAT";
	else if ( m_StrDeviceType == "06")
		m_StrDeviceType = "CAT";
	else if ( m_StrDeviceType == "07")
		m_StrDeviceType = "SMART CARD";
	else if ( m_StrDeviceType == "08")
		m_StrDeviceType = "PAY PASS";

	UpdateData(FALSE);

	memcpy ( &sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));

	return TRUE;

}

void CMerchantTerminalGeneral::CleanGeneralDialog()
{
	m_StrOrganizationIDFld = _T("");
	m_StrDateAddedFld = _T("");
	m_StrDepartmentNumFld = _T("");
	m_StrIndustryCodeFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPinPadIndicatorFld = _T("");
	m_StrTerminalIDFld = _T("");
	//m_StrChargeDescFld = _T("");
	//m_StrDinersChargeDescrFld = _T("");
	m_StrCategoryCodeFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrABAFld = _T("");
	m_StrContactFld = _T("");
	m_StrShortNameFld = _T("");
	m_StrStatus = "ACTIVE";
	m_StrDeviceType = "EDC";
	m_StrEMVCapabilityFld = _T("");
//   m_StrNRICFld = "";

	m_CntlEnableStatistics.SetCheck(0);
	m_CntlEnableTotals.SetCheck(0);
//	m_CntlMPDeviceChk.SetCheck(0);

	UpdateData (FALSE);
}

BOOL  CMerchantTerminalGeneral::PopulateDatabaseFromGeneralDialog ( pDCF01 psTerminalRecord )
{
//	ZeroMemory ( &sMerchantRecordNew, sizeof ( MCF01) );
	if (m_StrDateAddedFld.IsEmpty() != TRUE)
	{
		m_StrDateAddedFld = m_StrDateAddedFld.Right(4) + m_StrDateAddedFld.Mid(3,2) + m_StrDateAddedFld.Left(2);
	}
	memcpy ( psTerminalRecord->organization_id,	m_StrOrganizationIDFld,		sizeof ( psTerminalRecord->organization_id));
	memcpy ( psTerminalRecord->date_added,		m_StrDateAddedFld,		sizeof ( psTerminalRecord->date_added));
	memcpy ( psTerminalRecord->dept_nbr,		m_StrDepartmentNumFld,	sizeof ( psTerminalRecord->dept_nbr));
	memcpy ( psTerminalRecord->industry_code,	m_StrIndustryCodeFld,	sizeof ( psTerminalRecord->industry_code));
	memcpy ( psTerminalRecord->merchant_id,		m_StrMerchantIDFld,		sizeof ( psTerminalRecord->merchant_id));
	memcpy ( psTerminalRecord->pin_type,		m_StrPinPadIndicatorFld,sizeof ( psTerminalRecord->pin_type));
	memcpy ( psTerminalRecord->primary_key.device_id,	m_StrTerminalIDFld,	sizeof ( psTerminalRecord->primary_key.device_id));
	//memcpy ( psTerminalRecord->amex_charge_desc,		m_StrChargeDescFld,		sizeof ( psTerminalRecord->amex_charge_desc));
	//memcpy ( psTerminalRecord->diners_charge_desc,		m_StrDinersChargeDescrFld,		sizeof ( psTerminalRecord->diners_charge_desc));
	memcpy ( psTerminalRecord->category_code,	m_StrCategoryCodeFld,	sizeof ( psTerminalRecord->category_code));
	memcpy ( psTerminalRecord->aba_nbr,			m_StrABAFld,			sizeof ( psTerminalRecord->aba_nbr));
	memcpy ( psTerminalRecord->contact,			m_StrContactFld,		sizeof ( psTerminalRecord->contact));
	memcpy ( psTerminalRecord->short_name,			m_StrShortNameFld,		sizeof ( psTerminalRecord->short_name));
//	memcpy ( psTerminalRecord->nric,			m_StrNRICFld,		sizeof ( psTerminalRecord->nric));
	memcpy ( psTerminalRecord->current_bus_date,			m_StrEMVCapabilityFld,			sizeof ( psTerminalRecord->current_bus_date));
	if ( m_StrStatus == "ACTIVE")
		m_StrStatus = "A";
	else if ( m_StrStatus == "INACTIVE" )
		m_StrStatus = "I";
	else if ( m_StrStatus == "RESTRICTED")
		m_StrStatus = "R";

/*	if ( m_StrDeviceType == "CRIND")
		m_StrDeviceType = "01";*/
	if ( m_StrDeviceType == "EDC")
		m_StrDeviceType = "02";
	else if ( m_StrDeviceType == "EFTPOS ISO")
		m_StrDeviceType = "03";
	else if ( m_StrDeviceType == "EFTPOS VISA I")
		m_StrDeviceType = "04";
	else if ( m_StrDeviceType == "VAT")
		m_StrDeviceType = "05";
	else if ( m_StrDeviceType == "CAT")
		m_StrDeviceType = "06";
	else if ( m_StrDeviceType == "SMART CARD")
		m_StrDeviceType = "07";
	else if ( m_StrDeviceType == "PAY PASS")
		m_StrDeviceType = "08";
	

	memcpy ( psTerminalRecord->status,			m_StrStatus,			sizeof ( psTerminalRecord->status));
	memcpy ( psTerminalRecord->device_type,		m_StrDeviceType,		sizeof ( psTerminalRecord->device_type));

	if ( m_bGeneral )
	{
		if ( m_CntlEnableStatistics.GetCheck() == 1)
			memcpy ( psTerminalRecord->statistics_ind, "Y", sizeof ( psTerminalRecord->statistics_ind));
		else
			memcpy ( psTerminalRecord->statistics_ind, "N", sizeof ( psTerminalRecord->statistics_ind));

		if ( m_CntlEnableTotals.GetCheck() == 1)
			memcpy ( psTerminalRecord->totals_ind, "Y", sizeof ( psTerminalRecord->totals_ind));
		else
			memcpy ( psTerminalRecord->totals_ind, "N", sizeof ( psTerminalRecord->totals_ind));

		strTotalsInd = psTerminalRecord->totals_ind;
		strStatisticsInd = psTerminalRecord->statistics_ind;

	}

	return TRUE;
}

BOOL CMerchantTerminalGeneral::CompareStructures()
{
	if (strcmp ( strTotalsInd, (char *)sTerminalRecordOld.totals_ind) != 0)
	{
		memcpy(m_strCurrentData, strTotalsInd, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.totals_ind, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Totals Indicator", sizeof (m_strFieldTag));
		
		PopulateHistoryTable();			
		 
	}
	if (strcmp ( strStatisticsInd, (char *)sTerminalRecordOld.statistics_ind) != 0)
	{
		memcpy(m_strCurrentData, strStatisticsInd, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.statistics_ind, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Statistics Indic", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrShortNameFld, (char *)sTerminalRecordOld.short_name) != 0)
	{
		memcpy(m_strCurrentData, m_StrShortNameFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.short_name, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Short Name", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrPinPadIndicatorFld, (char *)sTerminalRecordOld.pin_type) != 0)
	{
		memcpy(m_strCurrentData, m_StrPinPadIndicatorFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.pin_type, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Pin Pad ind", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrStatus, (char *)sTerminalRecordOld.status) != 0)
	{
		memcpy(m_strCurrentData, m_StrStatus, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.status, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Status", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrDepartmentNumFld, (char *)sTerminalRecordOld.dept_nbr) != 0)
	{
		memcpy(m_strCurrentData, m_StrDepartmentNumFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.dept_nbr, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Department Num", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrDeviceType, (char *)sTerminalRecordOld.device_type) != 0)
	{
		memcpy(m_strCurrentData, m_StrDeviceType, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.device_type, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Device type", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrIndustryCodeFld, (char *)sTerminalRecordOld.industry_code) != 0)
	{
		memcpy(m_strCurrentData, m_StrIndustryCodeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.industry_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Industry code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	/*if (strcmp ( m_StrChargeDescFld, (char *)sTerminalRecordOld.amex_charge_desc) != 0)
	{
		memcpy(m_strCurrentData, m_StrChargeDescFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.amex_charge_desc, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "AMEX Charge description", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrDinersChargeDescrFld, (char *)sTerminalRecordOld.diners_charge_desc) != 0)
	{
		memcpy(m_strCurrentData, m_StrDinersChargeDescrFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.diners_charge_desc, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Diners Charge description", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}*/
	if (strcmp ( m_StrCategoryCodeFld, (char *)sTerminalRecordOld.category_code) != 0)
	{
		memcpy(m_strCurrentData, m_StrCategoryCodeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.category_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Category code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrABAFld, (char *)sTerminalRecordOld.aba_nbr) != 0)
	{
		memcpy(m_strCurrentData, m_StrABAFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.aba_nbr, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "ABA Num", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrEMVCapabilityFld, (char *)sTerminalRecordOld.current_bus_date) != 0)
	{
		memcpy(m_strCurrentData, m_StrEMVCapabilityFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.current_bus_date, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "EMV Capability", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrContactFld, (char *)sTerminalRecordOld.contact) != 0)
	{
		memcpy(m_strCurrentData, m_StrContactFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.contact, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Contact", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
/*   if (strcmp ( m_StrNRICFld, (char *)sTerminalRecordOld.nric) != 0)
	{
		memcpy(m_strCurrentData, m_StrNRICFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.nric, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "NRIC", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}*/
	return TRUE;
}

void CMerchantTerminalGeneral::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrTerminalIDFld);
	//strcat ( strPrimaryKey, m_StrMerchantIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"DCF01",			sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.file_primary_key,		(LPCTSTR)strPrimaryKey,		sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,	(LPCTSTR)dest_str,			sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,			(LPCTSTR)strTime,			sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data,			(LPCTSTR)m_strCurrentData,	sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,			(LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag,				(LPCTSTR)m_strFieldTag,		sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,			(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
}
