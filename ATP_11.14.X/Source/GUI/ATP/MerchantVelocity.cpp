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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantVelocity.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Feb 10 2002 00:35:08   dirby
   Modified PopulateHistoryTable function so the unique_id field
   does not give a constraint violation.  It was frequently giving a
   unique constraint violation when updating the general and
   blocking tabs because it could do more than 1 within the same
   millisecond.  Also, if the data server returned with an error,
   there was an UpdateData(FALSE) command that would crash
   the program because it was trying to update a window that did
   not exist.  I replaced the command with an informational box.
   
      Rev 1.6   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:00   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.2   Apr 15 1999 14:19:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 16 1999 16:30:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:30   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:58   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:08   iarustam
    
*
************************************************************************************/
// MerchantVelocity.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantVelocity.h"
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

/////////////////////////////////////////////////////////////////////////////
// CMerchantVelocity property page

IMPLEMENT_DYNCREATE(CMerchantVelocity, CPropertyPage)

CMerchantVelocity::CMerchantVelocity() : CPropertyPage(CMerchantVelocity::IDD)
{
	//{{AFX_DATA_INIT(CMerchantVelocity)
	m_StrMaxManualEntrysFld = _T("");
	m_StrMaxPanSameFld = _T("");
	m_StrMaxReturnsFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPurchaseAverageFld = _T("");
	m_StrReturnAmountFld = _T("");
	m_StrTotalLimitFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CMerchantVelocity::~CMerchantVelocity()
{
}

void CMerchantVelocity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantVelocity)
	DDX_Control(pDX, IDC_TOTAL_LIMIT_FLD, m_CntlTotalLimitFld);
	DDX_Control(pDX, IDC_SETTLEMENT_TOTALS, m_SettlementTotals);
	DDX_Control(pDX, IDC_SAME_PAN, m_SamePan);
	DDX_Control(pDX, IDC_RETURN_AMOUNT_FLD, m_CntlReturnAmountFld);
	DDX_Control(pDX, IDC_RETURN_AMOUNT, m_ReturnAmount);
	DDX_Control(pDX, IDC_PURCHASE_AVERAGE_FLD, m_CntlPurchaseAverageFld);
	DDX_Control(pDX, IDC_NUMBER_RETURNS, m_NumberReturns);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_MAX_RETURNS_FLD, m_CntlMaxReturnsFld);
	DDX_Control(pDX, IDC_MAX_PAN_SAME_FLD, m_CntlMaxPanSameFld);
	DDX_Control(pDX, IDC_MAX_MANUAL_ENTRYS_FLD, m_CntlMaxManualEntrysFld);
	DDX_Control(pDX, IDC_MANUAL_ENTRYS, m_ManualEntrys);
	DDX_Control(pDX, IDC_AVERAGE_PURCHASE, m_AveragePurchase);
	DDX_Text(pDX, IDC_MAX_MANUAL_ENTRYS_FLD, m_StrMaxManualEntrysFld);
	DDV_MaxChars(pDX, m_StrMaxManualEntrysFld, 4);
	DDX_Text(pDX, IDC_MAX_PAN_SAME_FLD, m_StrMaxPanSameFld);
	DDV_MaxChars(pDX, m_StrMaxPanSameFld, 2);
	DDX_Text(pDX, IDC_MAX_RETURNS_FLD, m_StrMaxReturnsFld);
	DDV_MaxChars(pDX, m_StrMaxReturnsFld, 4);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_PURCHASE_AVERAGE_FLD, m_StrPurchaseAverageFld);
	DDV_MaxChars(pDX, m_StrPurchaseAverageFld, 10);
	DDX_Text(pDX, IDC_RETURN_AMOUNT_FLD, m_StrReturnAmountFld);
	DDV_MaxChars(pDX, m_StrReturnAmountFld, 10);
	DDX_Text(pDX, IDC_TOTAL_LIMIT_FLD, m_StrTotalLimitFld);
	DDV_MaxChars(pDX, m_StrTotalLimitFld, 10);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantVelocity, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantVelocity)
	ON_BN_CLICKED(IDC_AVERAGE_PURCHASE, OnAveragePurchase)
	ON_BN_CLICKED(IDC_MANUAL_ENTRYS, OnManualEntrys)
	ON_BN_CLICKED(IDC_NUMBER_RETURNS, OnNumberReturns)
	ON_BN_CLICKED(IDC_RETURN_AMOUNT, OnReturnAmount)
	ON_BN_CLICKED(IDC_SAME_PAN, OnSamePan)
	ON_BN_CLICKED(IDC_SETTLEMENT_TOTALS, OnSettlementTotals)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantVelocity message handlers
BOOL CMerchantVelocity::OnInitDialog() 
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

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableVelocityDialog(TRUE);	
		if ( m_nDialogMode != MODE_ADD )
			PopulateVelocityDialogFromDatabase ( sMerchantRecordOld );

	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantVelocity::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantVelocity::EnableVelocityDialog( int nStatus )
{
	m_AveragePurchase.EnableWindow(nStatus);
	m_ManualEntrys.EnableWindow(nStatus);
	m_NumberReturns.EnableWindow(nStatus);
	m_ReturnAmount.EnableWindow(nStatus);
	m_SamePan.EnableWindow(nStatus);
	m_SettlementTotals.EnableWindow(nStatus);
}

BOOL CMerchantVelocity::PopulateVelocityDialogFromDatabase ( MCF01 sMerchantRecord )
{
	CMyUtils myUtilsObject;

	if ( m_nDialogMode != MODE_VIEW )	
		EnableVelocityDialog(TRUE);

	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );

	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;

	m_StrMaxManualEntrysFld  = sMerchantRecord.number_man_entries;
	m_StrMaxPanSameFld		 = sMerchantRecord.pan_same;
	m_StrMaxReturnsFld		 = sMerchantRecord.max_num_returns;
	m_StrPurchaseAverageFld	 = sMerchantRecord.aver_purch_amount;
	m_StrReturnAmountFld	 = sMerchantRecord.amount_returns;
	m_StrTotalLimitFld		 = sMerchantRecord.amount_deposits;

	m_StrPurchaseAverageFld = myUtilsObject.GetAmount ( m_StrPurchaseAverageFld );
	m_StrReturnAmountFld = myUtilsObject.GetAmount ( m_StrReturnAmountFld );
	m_StrTotalLimitFld = myUtilsObject.GetAmount ( m_StrTotalLimitFld );

	if ( m_nDialogMode != MODE_VIEW )
	{
		if (!myUtilsObject.IsEmpty (m_StrMaxManualEntrysFld, this))
		{
			m_ManualEntrys.SetCheck(1);
			m_CntlMaxManualEntrysFld.EnableWindow(TRUE);
		}

		if (!myUtilsObject.IsEmpty (m_StrMaxPanSameFld, this))
		{
			m_SamePan.SetCheck(1);
			m_CntlMaxPanSameFld.EnableWindow(TRUE);
		}

		if (!myUtilsObject.IsEmpty (m_StrMaxReturnsFld, this))
		{
			m_NumberReturns.SetCheck(1);
			m_CntlMaxReturnsFld.EnableWindow(TRUE);
		}

		if (!myUtilsObject.IsEmpty (m_StrPurchaseAverageFld, this))
		{
			m_AveragePurchase.SetCheck(1);
			m_CntlPurchaseAverageFld.EnableWindow(TRUE);
		}

		if (!myUtilsObject.IsEmpty (m_StrReturnAmountFld, this))
		{
			m_ReturnAmount.SetCheck(1);
			m_CntlReturnAmountFld.EnableWindow(TRUE);
		}

		if (!myUtilsObject.IsEmpty (m_StrTotalLimitFld, this))
		{
			m_SettlementTotals.SetCheck(1);
			m_CntlTotalLimitFld.EnableWindow(TRUE);
		}
	}

	UpdateData(FALSE);

	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

void CMerchantVelocity::CleanVelocityDialog()
{
	m_StrMaxManualEntrysFld = _T("");
	m_StrMaxPanSameFld = _T("");
	m_StrMaxReturnsFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPurchaseAverageFld = _T("");
	m_StrReturnAmountFld = _T("");
	m_StrTotalLimitFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMessageLbl = _T("");

	m_CntlMaxManualEntrysFld.EnableWindow(FALSE);
	m_CntlMaxPanSameFld.EnableWindow(FALSE);
	m_CntlMaxReturnsFld.EnableWindow(FALSE);
	m_CntlPurchaseAverageFld.EnableWindow(FALSE);
	m_CntlReturnAmountFld.EnableWindow(FALSE);
	m_CntlTotalLimitFld.EnableWindow(FALSE);

	m_AveragePurchase.SetCheck(0);
	m_ManualEntrys.SetCheck(0);
	m_NumberReturns.SetCheck(0);
	m_ReturnAmount.SetCheck(0);
	m_SamePan.SetCheck(0);
	m_SettlementTotals.SetCheck(0);

	UpdateData (FALSE);
}

BOOL  CMerchantVelocity::PopulateDatabaseFromVelocityDialog ( pMCF01 psMerchantRecord )
{
	CMyUtils myUtilsObject;

	if ( !m_bVelocity )
		return TRUE;

	m_StrPurchaseAverageFld = myUtilsObject.GenerateAmount ( m_StrPurchaseAverageFld, 12 );
	m_StrReturnAmountFld = myUtilsObject.GenerateAmount ( m_StrReturnAmountFld, 12 );
	m_StrTotalLimitFld = myUtilsObject.GenerateAmount ( m_StrTotalLimitFld, 12 );

	memcpy ( psMerchantRecord->number_man_entries,	m_StrMaxManualEntrysFld,sizeof ( psMerchantRecord->number_man_entries));
	memcpy ( psMerchantRecord->pan_same,			m_StrMaxPanSameFld,		sizeof ( psMerchantRecord->pan_same));
	memcpy ( psMerchantRecord->max_num_returns,		m_StrMaxReturnsFld,		sizeof ( psMerchantRecord->max_num_returns));
	memcpy ( psMerchantRecord->aver_purch_amount,	m_StrPurchaseAverageFld,sizeof ( psMerchantRecord->aver_purch_amount));
	memcpy ( psMerchantRecord->amount_returns,		m_StrReturnAmountFld,	sizeof ( psMerchantRecord->amount_returns));
	memcpy ( psMerchantRecord->amount_deposits,		m_StrTotalLimitFld,		sizeof ( psMerchantRecord->amount_deposits));
	

	return TRUE;
}

BOOL CMerchantVelocity::CompareStructures()
{
	CMyUtils myUtilsObject;
	CString strTemp;

	if (strcmp ( m_StrMaxManualEntrysFld, (char *)sMerchantRecordOld.number_man_entries) != 0)
	{		
      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrMaxManualEntrysFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.number_man_entries, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Max Manual Entrys", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrMaxPanSameFld, (char *)sMerchantRecordOld.pan_same) != 0)
	{
      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrMaxPanSameFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.pan_same, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Max Pan Same", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrMaxReturnsFld, (char *)sMerchantRecordOld.max_num_returns) != 0)
	{
      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrMaxReturnsFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sMerchantRecordOld.max_num_returns, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Max Returns", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrPurchaseAverageFld, (char *)sMerchantRecordOld.aver_purch_amount) != 0)
	{
		m_StrPurchaseAverageFld = myUtilsObject.GetAmount(m_StrPurchaseAverageFld);
		strTemp = sMerchantRecordOld.aver_purch_amount;
		strTemp = myUtilsObject.GetAmount(strTemp);

      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrPurchaseAverageFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Purchase Average", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrReturnAmountFld, (char *)sMerchantRecordOld.amount_returns) != 0)
	{
		m_StrReturnAmountFld = myUtilsObject.GetAmount(m_StrReturnAmountFld);
		strTemp = sMerchantRecordOld.amount_returns;
		strTemp = myUtilsObject.GetAmount(strTemp);

      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrReturnAmountFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Return Amount", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrTotalLimitFld, (char *)sMerchantRecordOld.amount_deposits) != 0)
	{
		m_StrTotalLimitFld = myUtilsObject.GetAmount(m_StrTotalLimitFld);
		strTemp = sMerchantRecordOld.amount_deposits;
		strTemp = myUtilsObject.GetAmount(strTemp);

      memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
      memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
      memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

		memcpy(m_strCurrentData, m_StrTotalLimitFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Total Limit", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	return TRUE;
}

void CMerchantVelocity::PopulateHistoryTable()
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


void CMerchantVelocity::OnAveragePurchase() 
{
		UpdateData(TRUE);

	if (m_AveragePurchase.GetCheck() == 0)
	{
		m_CntlPurchaseAverageFld.EnableWindow(FALSE);
		m_StrPurchaseAverageFld = "";
		UpdateData(FALSE);
	}
	else if (m_AveragePurchase.GetCheck() == 1)
	{
		m_CntlPurchaseAverageFld.EnableWindow(TRUE);
		m_CntlPurchaseAverageFld.SetFocus();
	}
	
}

void CMerchantVelocity::OnManualEntrys() 
{
		UpdateData(TRUE);

	if (m_ManualEntrys.GetCheck() == 0)
	{
		m_CntlMaxManualEntrysFld.EnableWindow(FALSE);
		m_StrMaxManualEntrysFld = "";
		UpdateData(FALSE);
	}
	else if (m_ManualEntrys.GetCheck() == 1)
	{
		m_CntlMaxManualEntrysFld.EnableWindow(TRUE);
		m_CntlMaxManualEntrysFld.SetFocus();
		UpdateData(FALSE);
	}
}

void CMerchantVelocity::OnNumberReturns() 
{
		UpdateData(TRUE);

	if (m_NumberReturns.GetCheck() == 0)
	{
		m_CntlMaxReturnsFld.EnableWindow(FALSE);
		m_StrMaxReturnsFld = "";
		UpdateData(FALSE);
	}
	else if (m_NumberReturns.GetCheck() == 1)
	{
		m_CntlMaxReturnsFld.EnableWindow(TRUE);
		m_CntlMaxReturnsFld.SetFocus();
	}
	
}

void CMerchantVelocity::OnReturnAmount() 
{
		UpdateData(TRUE);

	if (m_ReturnAmount.GetCheck() == 0)
	{
		m_CntlReturnAmountFld.EnableWindow(FALSE);
		m_StrReturnAmountFld = "";
		UpdateData(FALSE);
	}
	else if (m_ReturnAmount.GetCheck() == 1)
	{
		m_CntlReturnAmountFld.EnableWindow(TRUE);
		m_CntlReturnAmountFld.SetFocus();
	}
	
}

void CMerchantVelocity::OnSamePan() 
{
		UpdateData(TRUE);

	if (m_SamePan.GetCheck() == 0)
	{
		m_CntlMaxPanSameFld.EnableWindow(FALSE);
		m_StrMaxPanSameFld = "";
		UpdateData(FALSE);
	}
	else if (m_SamePan.GetCheck() == 1)
	{
		m_CntlMaxPanSameFld.EnableWindow(TRUE);
		m_CntlMaxPanSameFld.SetFocus();
	}
	
}

void CMerchantVelocity::OnSettlementTotals() 
{
	UpdateData(TRUE);
	if (m_SettlementTotals.GetCheck() == 0)
	{
		m_CntlTotalLimitFld.EnableWindow(FALSE);
		m_StrTotalLimitFld = "";
		UpdateData(FALSE);
	}
	else if (m_SettlementTotals.GetCheck() == 1)
	{
		m_CntlTotalLimitFld.EnableWindow(TRUE);
		m_CntlTotalLimitFld.SetFocus();
	}
	
}

