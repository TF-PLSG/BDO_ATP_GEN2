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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantLocation.cpp  $
   
      Rev 1.9   Jul 08 2004 13:00:38   dirby
   Updated History File logging to make sure Merchant ID is in the
   File_Primary_Key and to make sure fields are not accidently logged
   when no changes have been made.
   SCR 1421
   
      Rev 1.8   Dec 01 2003 11:29:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Feb 10 2002 00:34:58   dirby
   Modified PopulateHistoryTable function so the unique_id field
   does not give a constraint violation.  It was frequently giving a
   unique constraint violation when updating the general and
   blocking tabs because it could do more than 1 within the same
   millisecond.  Also, if the data server returned with an error,
   there was an UpdateData(FALSE) command that would crash
   the program because it was trying to update a window that did
   not exist.  I replaced the command with an informational box.
   
      Rev 1.6   Jan 27 2000 13:06:20   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.5   Jan 05 2000 14:53:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.4   Oct 15 1999 17:14:02   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:24   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:53:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:52   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:36   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:12   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:42   iarustam
    
*
************************************************************************************/
// MerchantLocation.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantLocation.h"
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
extern CSharedMem shrmem;


CString  MerchantID;
CString  OrganizationID;

/////////////////////////////////////////////////////////////////////////////
// CMerchantLocation property page

IMPLEMENT_DYNCREATE(CMerchantLocation, CPropertyPage)

CMerchantLocation::CMerchantLocation() : CPropertyPage(CMerchantLocation::IDD)
{
	//{{AFX_DATA_INIT(CMerchantLocation)
	m_StrAddress1Fld = _T("");
	m_StrAddress2Fld = _T("");
	m_StrCityFld = _T("");
	m_StrCountryFld = _T("");
	m_StrCountyFld = _T("");
	m_StrDateAddedFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMerchantNameFld = _T("");
	m_StrPostalCodeFld = _T("");
	m_StrStateFld = _T("");
	m_StrInvoiceBatchFld = _T("");
	m_StrInvoiceSubcodeFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrChainNameFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrStatusFld = _T("");
	m_StrNegativeGroupIDFld = _T("");
	m_StrCurrencyCodeFld = _T("");
	m_StrMerchantGroupFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantLocation::~CMerchantLocation()
{
}

void CMerchantLocation::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantLocation)
	DDX_Control(pDX, IDC_MERCHANT_GROUP_ID, m_CntlMerchantGroupID);
	DDX_Control(pDX, IDC_CURRENCY_CODE, m_CntlCurrencyCodeFld);
	DDX_Control(pDX, IDC_NEG_GROUP_ID_FLD, m_CntlNegativeGroupIDFld);
	DDX_Control(pDX, IDC_CHAIN_NAME_FLD, m_CntlChainNameFld);
	DDX_Control(pDX, IDC_A_INVOICE_SUBCODE_FLD, m_CntlInvoiceSubcodeFld);
	DDX_Control(pDX, IDC_A_INVOICE_BATCH_FLD, m_CntlInvoiceBatchFld);
	DDX_Control(pDX, IDC_STATUS__FLD, m_CntlStatusFld);
	DDX_Control(pDX, IDC_STATE_FLD, m_CntlStateFld);
	DDX_Control(pDX, IDC_POSTAL_CODE_FLD, m_CntlPostalCodeFld);
	DDX_Control(pDX, IDC_MERCHANT_NAME_FLD, m_CntlMerchantNameFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_ENABLE_TOTALS_CHECK, m_CntlEnableTotals);
	DDX_Control(pDX, IDC_ENABLE_STATISTICES_CHECK, m_CntlEnableStatistics);
	DDX_Control(pDX, IDC_DATE_ADDED_FLD, m_CntlDateAddedFld);
	DDX_Control(pDX, IDC_COUNTY_FLD, m_CntlCountyFld);
	DDX_Control(pDX, IDC_COUNTRY_FLD, m_CntlCountryFld);
	DDX_Control(pDX, IDC_CITY_FLD, m_CntlCityFld);
	DDX_Control(pDX, IDC_ADDRESS2_FLD, m_CntlAddress2Fld);
	DDX_Control(pDX, IDC_ADDRESS1_FLD, m_CntlAddress1Fld);
	DDX_Text(pDX, IDC_ADDRESS1_FLD, m_StrAddress1Fld);
	DDV_MaxChars(pDX, m_StrAddress1Fld, 30);
	DDX_Text(pDX, IDC_ADDRESS2_FLD, m_StrAddress2Fld);
	DDV_MaxChars(pDX, m_StrAddress2Fld, 30);
	DDX_Text(pDX, IDC_CITY_FLD, m_StrCityFld);
	DDV_MaxChars(pDX, m_StrCityFld, 20);
	DDX_Text(pDX, IDC_COUNTRY_FLD, m_StrCountryFld);
	DDV_MaxChars(pDX, m_StrCountryFld, 3);
	DDX_Text(pDX, IDC_COUNTY_FLD, m_StrCountyFld);
	DDV_MaxChars(pDX, m_StrCountyFld, 20);
	DDX_Text(pDX, IDC_DATE_ADDED_FLD, m_StrDateAddedFld);
	DDV_MaxChars(pDX, m_StrDateAddedFld, 10);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MERCHANT_NAME_FLD, m_StrMerchantNameFld);
	DDV_MaxChars(pDX, m_StrMerchantNameFld, 30);
	DDX_Text(pDX, IDC_POSTAL_CODE_FLD, m_StrPostalCodeFld);
	DDV_MaxChars(pDX, m_StrPostalCodeFld, 10);
	DDX_Text(pDX, IDC_STATE_FLD, m_StrStateFld);
	DDV_MaxChars(pDX, m_StrStateFld, 2);
	DDX_Text(pDX, IDC_A_INVOICE_BATCH_FLD, m_StrInvoiceBatchFld);
	DDV_MaxChars(pDX, m_StrInvoiceBatchFld, 3);
	DDX_Text(pDX, IDC_A_INVOICE_SUBCODE_FLD, m_StrInvoiceSubcodeFld);
	DDV_MaxChars(pDX, m_StrInvoiceSubcodeFld, 2);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_CHAIN_NAME_FLD, m_StrChainNameFld);
	DDV_MaxChars(pDX, m_StrChainNameFld, 30);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_CBString(pDX, IDC_STATUS__FLD, m_StrStatusFld);
	DDX_Text(pDX, IDC_NEG_GROUP_ID_FLD, m_StrNegativeGroupIDFld);
	DDV_MaxChars(pDX, m_StrNegativeGroupIDFld, 10);
	DDX_CBString(pDX, IDC_CURRENCY_CODE, m_StrCurrencyCodeFld);
	DDX_CBString(pDX, IDC_MERCHANT_GROUP_ID, m_StrMerchantGroupFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantLocation, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantLocation)
	ON_CBN_SELCHANGE(IDC_MERCHANT_GROUP_ID, OnSelchangeMerchantGroupId)
	ON_CBN_EDITCHANGE(IDC_MERCHANT_GROUP_ID, OnEditchangeMerchantGroupId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantLocation message handlers

BOOL CMerchantLocation::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CMyUtils myUtilsObject;
	CFont m_font;
	LOGFONT lf;
	char strDate[9] = "";

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bLocation = TRUE;

	if (m_nDialogMode != MODE_DONE )
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableLocationDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
		{
			PopulateLocationDialogFromDatabase ( sMerchantRecordOld );
		}
		if ( m_nDialogMode == MODE_ADD )
		{
			myUtilsObject.GetSystemDate(strDate);	
			m_StrDateAddedFld = strDate;
			m_StrDateAddedFld = m_StrDateAddedFld.Mid(2,2) + "/" + m_StrDateAddedFld.Left(2) + "/" + m_StrDateAddedFld.Right(4);
			UpdateData(FALSE);
		}
	}   
	
	if (m_nDialogMode == MODE_ADD)
		m_CntlStatusFld.SetCurSel(0);

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMerchantLocation::EnableLocationDialog( int nStatus )
{
	m_CntlAddress1Fld.EnableWindow(nStatus);
	m_CntlAddress2Fld.EnableWindow(nStatus);
	m_CntlCityFld.EnableWindow(nStatus);
	m_CntlCountryFld.EnableWindow(nStatus);
	m_CntlCountyFld.EnableWindow(nStatus);
//	m_CntlDateAddedFld.EnableWindow(nStatus);
	m_CntlMerchantNameFld.EnableWindow(nStatus);
	m_CntlPostalCodeFld.EnableWindow(nStatus);
	m_CntlStateFld.EnableWindow(nStatus);
	m_CntlStatusFld.EnableWindow(nStatus);
	m_CntlInvoiceBatchFld.EnableWindow(nStatus);
	m_CntlInvoiceSubcodeFld.EnableWindow(nStatus);
	m_CntlChainNameFld.EnableWindow(nStatus);
	m_CntlCurrencyCodeFld.EnableWindow(nStatus);
	m_CntlEnableStatistics.EnableWindow(nStatus);
	m_CntlEnableTotals.EnableWindow(nStatus);
	m_CntlNegativeGroupIDFld.EnableWindow(nStatus);
	m_CntlMerchantGroupID.EnableWindow(FALSE);// WE are disable drop list down as requested by client
}

void CMerchantLocation::CleanLocationDialog()
{
	m_StrAddress1Fld = _T("");
	m_StrAddress2Fld = _T("");
	m_StrCityFld = _T("");
	m_StrCountryFld = _T("");
	m_StrCountyFld = _T("");
	m_StrDateAddedFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMerchantNameFld = _T("");
	m_StrPostalCodeFld = _T("");
	m_StrStateFld = _T("");
	//m_StrStatusFld = -1;
	m_StrStatusFld = "-1";
	m_StrInvoiceBatchFld = _T("");
	m_StrInvoiceSubcodeFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrChainNameFld = _T("");
	m_StrCurrencyCodeFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNegativeGroupIDFld = _T("");
	m_CntlEnableStatistics.SetCheck(0);
	m_CntlEnableTotals.SetCheck(0);
	m_StrMerchantGroupFld = _T("");
	UpdateData (FALSE);
}

BOOL CMerchantLocation::PopulateLocationDialogFromDatabase ( MCF01 sMerchantRecord )
{
	CString temp;
	CMyUtils myUtilsObject;
	int MGRP_len=0;

	if ( m_nDialogMode != MODE_VIEW )	
		EnableLocationDialog(TRUE);

	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );

	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;

	MerchantID     = sMerchantRecord.primary_key.merchant_id;
	OrganizationID = sMerchantRecord.primary_key.organization_id;

	m_StrAddress1Fld	 = sMerchantRecord.address01;
	m_StrAddress2Fld	 = sMerchantRecord.address02;
	m_StrCityFld		 = sMerchantRecord.city;
	m_StrCountryFld		 = sMerchantRecord.intl_code;
	m_StrCountyFld		 = sMerchantRecord.country;
	m_StrDateAddedFld	 = sMerchantRecord.date_added;
	if ( !myUtilsObject.IsEmpty ( m_StrDateAddedFld, this))
			m_StrDateAddedFld = m_StrDateAddedFld.Right(2) + "/" + m_StrDateAddedFld.Mid(4,2) + "/" + m_StrDateAddedFld.Left(4);
	m_StrMerchantNameFld = sMerchantRecord.name01;
	m_StrPostalCodeFld	 = sMerchantRecord.zip_code;
	m_StrStateFld		 = sMerchantRecord.state;
	m_StrInvoiceBatchFld = sMerchantRecord.invoice_batch;
	m_StrInvoiceSubcodeFld = sMerchantRecord.invoice_subcode;
	m_StrChainNameFld	 = sMerchantRecord.name02;

	m_StrCurrencyCodeFld = sMerchantRecord.currency_code;
	m_StrMerchantGroupFld = sMerchantRecord.network[5].priority_routing;
	if(strlen((pCHAR)sMerchantRecord.network[6].priority_routing)>0)
	{
	//m_StrMerchantGroupFld += sMerchantRecord.network[6].priority_routing;
	m_StrMerchantGroupFld += (CString)((char *)sMerchantRecord.network[6].priority_routing);
	}
	m_StrMerchantGroupFld.TrimRight();
    int index = m_CntlCurrencyCodeFld.SelectString ( -1, m_StrCurrencyCodeFld );
    int index1 = m_CntlMerchantGroupID.SelectString ( -1, m_StrMerchantGroupFld );

	int n = m_CntlMerchantGroupID.GetCount();
	//m_StrNegativeGroupIDFld = sMerchantRecord.negative_group_id;
	temp				 = sMerchantRecord.status;
	if ( temp == "A")
		m_StrStatusFld = "ACTIVE";
	else if ( temp == "I")
		m_StrStatusFld = "INACTIVE";
	else if ( temp == "R")
		m_StrStatusFld = "RESTRICTED";

	temp                 = sMerchantRecord.totals_ind;
	if ( temp == "Y" )
		m_CntlEnableTotals.SetCheck(1);
	else if ( temp == "N")
		m_CntlEnableTotals.SetCheck(0);

	temp                 = sMerchantRecord.statistics_ind;
	if ( temp == "Y" )
		m_CntlEnableStatistics.SetCheck(1);
	else if ( temp == "N")
		m_CntlEnableStatistics.SetCheck(0);

	strTotalsInd = sMerchantRecord.totals_ind;
	strStatisticsInd = sMerchantRecord.statistics_ind;



	UpdateData(FALSE);

	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));


	return TRUE;

}

BOOL CMerchantLocation::CompareStructures()
{
	CString status;
	CString temp;
	CString   OldMerchantGroupID;
	status = sMerchantRecordOld.status;

	if (status == "A")
		temp = "ACTIVE";
	else if (status == "R")
		temp = "RESTRICTED";	

   if (strcmp ( m_StrStatusFld, temp) != 0)
   {		
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrStatusFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, temp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Status", sizeof(m_strFieldTag));

		PopulateHistoryTable();
   }	

	if (strcmp ( strTotalsInd, (char *)sMerchantRecordOld.totals_ind) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, strTotalsInd, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.totals_ind, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Totals Indicator", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	/*if (strcmp ( m_StrNegativeGroupIDFld, (char *)sMerchantRecordOld.negative_group_id) != 0)
	{
      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrNegativeGroupIDFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.negative_group_id, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Negative Group ID", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}*/
	if (strcmp ( strStatisticsInd, (char *)sMerchantRecordOld.statistics_ind) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, strStatisticsInd, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.statistics_ind, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Statistics Indic", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrAddress1Fld, (char *)sMerchantRecordOld.address01) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrAddress1Fld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.address01, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Address Line 1", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrAddress2Fld, (char *)sMerchantRecordOld.address02) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrAddress2Fld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.address02, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Address Line 2", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCityFld, (char *)sMerchantRecordOld.city) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCityFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.city, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "City", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCountryFld, (char *)sMerchantRecordOld.intl_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCountryFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.intl_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Country", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCountyFld, (char *)sMerchantRecordOld.country) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrCountyFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.country, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "County", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrMerchantNameFld, (char *)sMerchantRecordOld.name01) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrMerchantNameFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.name01, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Merchant Name", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrPostalCodeFld, (char *)sMerchantRecordOld.zip_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrPostalCodeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.zip_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Postal Code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrStateFld, (char *)sMerchantRecordOld.state) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrStateFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.state, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "State", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrInvoiceBatchFld, (char *)sMerchantRecordOld.invoice_batch) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrInvoiceBatchFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.invoice_batch, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Invoice Batch", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrInvoiceSubcodeFld, (char *)sMerchantRecordOld.invoice_subcode) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrInvoiceSubcodeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.invoice_subcode, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Invoice Subcode", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrChainNameFld, (char *)sMerchantRecordOld.name02) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrChainNameFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.name02, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Chain Name", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	int index = m_StrCurrencyCodeFld.Find(' ');
	CString strCurrencyCode;
	if ( index != -1 )
	   strCurrencyCode =	m_StrCurrencyCodeFld.Left(index);
	else if ( m_StrCurrencyCodeFld.GetLength() != 0 )
	   strCurrencyCode =	m_StrCurrencyCodeFld;

	if (strcmp ( strCurrencyCode, (char *)sMerchantRecordOld.currency_code) != 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, strCurrencyCode, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.currency_code, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Currency Code", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	/* We were having some issue in production system and client asked to ingone this.
	The issue was not reproducable either at our side or client UAT
	/*
	int index1 = m_StrMerchantGroupFld.Find(' ');
	CString strMerchantGroupID;
	if ( index1 != -1 )
		strMerchantGroupID =	m_StrMerchantGroupFld.Left(index1);
	else if ( m_StrMerchantGroupFld.GetLength() != 0 )
		strMerchantGroupID =	m_StrMerchantGroupFld;

	if ( m_StrMerchantGroupFld.GetLength() != 0 )
	{
	OldMerchantGroupID = sMerchantRecordOld.network[5].priority_routing;
	OldMerchantGroupID += sMerchantRecordOld.network[6].priority_routing;

	if(strcmp ( strMerchantGroupID, OldMerchantGroupID )!= 0)
	{
		memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
		memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
		memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, strMerchantGroupID, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, OldMerchantGroupID, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Merchant Group id", sizeof (m_strFieldTag));
		PopulateHistoryTable();
   }
	}*/

	return TRUE;
}


BOOL  CMerchantLocation::PopulateDatabaseFromLocationDialog ( pMCF01 psMerchantRecord )
{
   char strStatus[2] = "";
   int index;
   //int merchantgroupid_len;

   index = m_StrCurrencyCodeFld.Find(' ');

   if ( index != -1 )
	   memcpy ( psMerchantRecord->currency_code,	m_StrCurrencyCodeFld.Left(index),	sizeof ( psMerchantRecord->currency_code));
   else if ( m_StrCurrencyCodeFld.GetLength() != 0 )
	   memcpy ( psMerchantRecord->currency_code,	m_StrCurrencyCodeFld,	sizeof ( psMerchantRecord->currency_code));

   /*
   Merchant Group id will be updated by GUI. User can see the groups but it wont be updated through GUI.
	
   This was done as we were facing an issue @ Production system and not able to reporduce
   in our UAT and Client UAT*/

   /*
   merchantgroupid_len = m_StrMerchantGroupFld.GetLength();

   index = m_StrMerchantGroupFld.Find(' ');

   if ( index != -1 )
   {
     memcpy ((LPSTR)m_strCurrentData, "Invalid Date Format", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
	
	if( merchantgroupid_len > 8)
	{
		memset( psMerchantRecord->network[5].priority_routing, 0, sizeof ( psMerchantRecord->network[5].priority_routing) );
		memset( psMerchantRecord->network[6].priority_routing, 0, sizeof ( psMerchantRecord->network[6].priority_routing) );
		memcpy( psMerchantRecord->network[5].priority_routing, m_StrMerchantGroupFld, (sizeof ( psMerchantRecord->network[5].priority_routing) -1 ));
		memcpy( psMerchantRecord->network[6].priority_routing, m_StrMerchantGroupFld.Mid((sizeof ( psMerchantRecord->network[5].priority_routing) -1 )), (merchantgroupid_len -(sizeof ( psMerchantRecord->network[5].priority_routing) -1 )));
	}
	else
	{
		memset( psMerchantRecord->network[5].priority_routing, 0, sizeof ( psMerchantRecord->network[5].priority_routing) );
		memset( psMerchantRecord->network[6].priority_routing, 0, sizeof ( psMerchantRecord->network[6].priority_routing) );
		memcpy( psMerchantRecord->network[5].priority_routing, m_StrMerchantGroupFld, merchantgroupid_len);
	}
   }
   else if ( m_StrMerchantGroupFld.GetLength() != 0 )
   {
	 
	if( merchantgroupid_len > 8)
	{
		memset( psMerchantRecord->network[5].priority_routing, 0, sizeof ( psMerchantRecord->network[5].priority_routing) );
		memset( psMerchantRecord->network[6].priority_routing, 0, sizeof ( psMerchantRecord->network[6].priority_routing) );
		memcpy( psMerchantRecord->network[5].priority_routing, m_StrMerchantGroupFld, (sizeof ( psMerchantRecord->network[5].priority_routing) -1 ));
		memcpy( psMerchantRecord->network[6].priority_routing, m_StrMerchantGroupFld.Mid((sizeof ( psMerchantRecord->network[5].priority_routing) -1 )), (merchantgroupid_len -(sizeof ( psMerchantRecord->network[5].priority_routing) -1 )));
	}
	else
	{
		memset( psMerchantRecord->network[5].priority_routing, 0, sizeof ( psMerchantRecord->network[5].priority_routing) );
		memset( psMerchantRecord->network[6].priority_routing, 0, sizeof ( psMerchantRecord->network[6].priority_routing) );
		memcpy( psMerchantRecord->network[5].priority_routing, m_StrMerchantGroupFld, merchantgroupid_len);
	}
   }
   
   */
   
	memcpy ( psMerchantRecord->address01,		m_StrAddress1Fld,		sizeof ( psMerchantRecord->address01));
	memcpy ( psMerchantRecord->address02,		m_StrAddress2Fld,		sizeof ( psMerchantRecord->address02));
	memcpy ( psMerchantRecord->city,			m_StrCityFld,			sizeof ( psMerchantRecord->city));
	memcpy ( psMerchantRecord->intl_code,		m_StrCountryFld,		sizeof ( psMerchantRecord->intl_code));
	memcpy ( psMerchantRecord->country,			m_StrCountyFld,			sizeof ( psMerchantRecord->country));
	memcpy ( psMerchantRecord->name01,			m_StrMerchantNameFld,	sizeof ( psMerchantRecord->name01));
	memcpy ( psMerchantRecord->zip_code,		m_StrPostalCodeFld,		sizeof ( psMerchantRecord->zip_code));
	memcpy ( psMerchantRecord->state,			m_StrStateFld,			sizeof ( psMerchantRecord->state));
	memcpy ( psMerchantRecord->name02,			m_StrChainNameFld,		sizeof ( psMerchantRecord->name02));
	memcpy ( psMerchantRecord->invoice_batch,	m_StrInvoiceBatchFld,	sizeof ( psMerchantRecord->invoice_batch));
	memcpy ( psMerchantRecord->invoice_subcode, m_StrInvoiceSubcodeFld, sizeof ( psMerchantRecord->invoice_subcode));
	//memcpy ( psMerchantRecord->negative_group_id, m_StrNegativeGroupIDFld, sizeof ( psMerchantRecord->negative_group_id));
   if ( m_StrDateAddedFld.GetLength() != 0 )
	   m_StrDateAddedFld = m_StrDateAddedFld.Right(4) + m_StrDateAddedFld.Mid(3,2) + m_StrDateAddedFld.Left(2);
	memcpy ( psMerchantRecord->date_added,		m_StrDateAddedFld,		sizeof ( psMerchantRecord->date_added));

	if ( m_StrStatusFld == "ACTIVE" )
		memcpy ( strStatus, "A", sizeof (strStatus));
	else 	if ( m_StrStatusFld == "INACTIVE" )
		memcpy ( strStatus, "I", sizeof (strStatus));
	else 	if ( m_StrStatusFld == "RESTRICTED" )
		memcpy ( strStatus, "R", sizeof (strStatus));


	memcpy ( psMerchantRecord->status, strStatus, sizeof ( psMerchantRecord->status));

	if ( m_bLocation )
	{
		if ( m_CntlEnableStatistics.GetCheck() == 1)
			memcpy ( psMerchantRecord->statistics_ind, "Y", sizeof ( psMerchantRecord->statistics_ind));
		else
			memcpy ( psMerchantRecord->statistics_ind, "N", sizeof ( psMerchantRecord->statistics_ind));

		if ( m_CntlEnableTotals.GetCheck() == 1)
			memcpy ( psMerchantRecord->totals_ind, "Y", sizeof ( psMerchantRecord->totals_ind));
		else
			memcpy ( psMerchantRecord->totals_ind, "N", sizeof ( psMerchantRecord->totals_ind));

		
	}

//	memcpy ( &sMerchantRecordNew, psMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

BOOL CMerchantLocation::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


void CMerchantLocation::PopulateHistoryTable()
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


   strcpy ( strPrimaryKey, m_StrMerchantIDFld);
   strcat ( strPrimaryKey, m_StrOrganizationIDFld );

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

BOOL CMerchantLocation::OnSetActive() 
{
   CRF01_LIST  sCRF01List;
   CRF01  sCurrencyCode;
   MGF01  sMerchantGroupRecord;
   //MGF01_GUI_LIST sMCF01GuiList;
   MGF01_GUI_DROPLIST sMCF01GuiList;
   int nNumberReturned;
   CString strCurrencyCode, strCode, strName;
   char strErrorMessage[300] = "";
   CString strMerchantGroupID;
   CString strCodeMGID;
   CString strCodeMGIDNull = " ";

   ZeroMemory ( &sCRF01List, sizeof ( CRF01_LIST ) );
   ZeroMemory ( &sCurrencyCode, sizeof ( CRF01 ) );
   m_CntlCurrencyCodeFld.ResetContent();
   m_CntlMerchantGroupID.ResetContent();
	if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT || m_nDialogMode ==MODE_VIEW)
   {
      int rcode = txdsapi_get_list((pBYTE)&sCurrencyCode, sizeof (CRF01), 
								  (pBYTE)&sCRF01List,sizeof (CRF01_LIST), ST2_DB_SELECT_LIST_BY_PARAM,
								  CRF01_DATA, strErrorMessage) ;	

		if (rcode == PTEMSG_OK)  
      { // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sCRF01List.num_returned);		
			for (  int i = 0; i < nNumberReturned; i++ ) {
			   strCode = sCRF01List.crf01_list[i].primary_key.currency_code ;
			   strName = sCRF01List.crf01_list[i].currency_name ;
            int len = strName.GetLength();
            if ( len != 0 ) 
               strCurrencyCode = strCode + "  *" + strName + "*";
            else 
               strCurrencyCode = strCode;
           
			   m_CntlCurrencyCodeFld.AddString(strCurrencyCode);  	
         }

		}
		/****************************/
		rcode = txdsapi_get_list((pBYTE)&sMerchantGroupRecord, sizeof (MGF01), 
								  (pBYTE)&sMCF01GuiList,sizeof (MGF01_GUI_DROPLIST), ST2_DB_SELECT_LIST_BY_PARAM,
								  MGF01_DATA, strErrorMessage) ;	

		if (rcode == PTEMSG_OK)  
		{ // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sMCF01GuiList.num_returned);		
			for (  int i = 0; i < nNumberReturned; i++ ) {
			   strCodeMGID = sMCF01GuiList.merchant_group_info[i].merchant_group_id ;
			int len = strCodeMGID.GetLength();
            //if ( len != 0 ) 
              // strMerchantGroupID = strCode + "  *" + strName + "*";
            //else 
               strMerchantGroupID = strCodeMGID;
            m_CntlMerchantGroupID.AddString(strCodeMGID);
			}
			m_CntlMerchantGroupID.AddString(strCodeMGIDNull);

		}
		/******************************/
		
   }

   if ( m_nDialogMode == MODE_ADD )
      m_CntlCurrencyCodeFld.SetCurSel(0);
   if ( m_nDialogMode == MODE_ADD )
      m_CntlMerchantGroupID.SetCurSel(0);
	return CPropertyPage::OnSetActive();
}


//int CMerchantLocation::OnSelchangeMerchantGroupId() 
void CMerchantLocation::OnSelchangeMerchantGroupId() 
{
	// TODO: Add your control notification handler code here
		memcpy ((LPSTR)m_strCurrentData, "Merchant Group ID can not be changed throuh GUI", sizeof (m_strCurrentData));		
		AfxMessageBox (m_strCurrentData);
		UpdateData(FALSE);
	   // return false;
		return;
	
}

void CMerchantLocation::OnEditchangeMerchantGroupId() 
{
	// TODO: Add your control notification handler code here
	
}
