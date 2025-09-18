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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\relatedgeneral.cpp  $
   
      Rev 1.19   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.18   Jul 12 2002 10:15:02   lmitchel
   scr844 - Disable releated account fields from editing on Update: Oustanding and Cash Oustanding balances
   
      Rev 1.17   Apr 23 2001 09:28:50   SYARLAGA
   Added the sprintf statements and modified the decimal point part for the respective fields in the related account.
   
      Rev 1.16   Apr 17 2000 09:58:42   iarustam
   added bin blocking
   
   
      Rev 1.15   Jan 27 2000 13:08:44   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.14   Jan 21 2000 12:24:14   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.13   Dec 07 1999 10:57:18   mbysani
   changes status code
   
      Rev 1.5   Oct 15 1999 17:26:20   iarustam
   bug fixes
   
      Rev 1.4   Oct 06 1999 16:47:04   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.3   Sep 30 1999 16:18:04   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 16:05:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.11   Jul 08 1999 16:19:54   iarustam
   Bug fixes
   
      Rev 1.0   Jul 01 1999 14:18:44   iarustam
   Initial Release
   

    
*   
************************************************************************************/
// RelatedGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "RelatedGeneral.h"
#include "MyUtils.h"
#include "BinList.h"
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
// CRelatedGeneral property page

IMPLEMENT_DYNCREATE(CRelatedGeneral, CPropertyPage)

CRelatedGeneral::CRelatedGeneral() : CPropertyPage(CRelatedGeneral::IDD)
{
	//{{AFX_DATA_INIT(CRelatedGeneral)
	m_StrAccountNumberFld = _T("");
	m_StrAvailableFld = _T("");
	m_StrCreditLimitFld = _T("");
	m_StrDateAddedFld = _T("");
	m_StrLedgerBalanceFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOutstandingFld = _T("");
	m_StrAccountTypeFld = _T("");
	m_StrCashAvailableFld = _T("");
	m_StrCashLimitFld = _T("");
	m_StrCashOutstandingFld = _T("");
	m_StrStatus = _T("");
	//}}AFX_DATA_INIT
}

CRelatedGeneral::~CRelatedGeneral()
{
}

void CRelatedGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelatedGeneral)
	DDX_Control(pDX, IDC_CASH_AVAILABLE_FLD, m_CntlCashAvailableFld);
	DDX_Control(pDX, IDC_AVAILABLE_FLD, m_CntlAvailableFld);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Control(pDX, IDC_CASH_OUTSTANDING_FLD, m_CntlCashOutstandingFld);
	DDX_Control(pDX, IDC_CASH_LIMIT_FLD, m_CntlCashLimitFld);
	DDX_Control(pDX, IDC_OUTSTANDING_FLD, m_CntlOutstandingFld);
	DDX_Control(pDX, IDC_LEDGER_BALANCE_FLD, m_CntlLedgerBalanceFld);
	DDX_Control(pDX, IDC_CREDIT_LIMIT_FLD, m_CntlCreditLimitFld);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDX_Text(pDX, IDC_AVAILABLE_FLD, m_StrAvailableFld);
	DDX_Text(pDX, IDC_CREDIT_LIMIT_FLD, m_StrCreditLimitFld);
	DDV_MaxChars(pDX, m_StrCreditLimitFld, 13);
	DDX_Text(pDX, IDC_DATE_ADDED_FLD, m_StrDateAddedFld);
	DDX_Text(pDX, IDC_LEDGER_BALANCE_FLD, m_StrLedgerBalanceFld);
	DDV_MaxChars(pDX, m_StrLedgerBalanceFld, 13);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_OUTSTANDING_FLD, m_StrOutstandingFld);
	DDV_MaxChars(pDX, m_StrOutstandingFld, 13);
	DDX_Text(pDX, IDC_ACCOUNT_TYPE_FLD, m_StrAccountTypeFld);
	DDX_Text(pDX, IDC_CASH_AVAILABLE_FLD, m_StrCashAvailableFld);
	DDX_Text(pDX, IDC_CASH_LIMIT_FLD, m_StrCashLimitFld);
	DDV_MaxChars(pDX, m_StrCashLimitFld, 13);
	DDX_Text(pDX, IDC_CASH_OUTSTANDING_FLD, m_StrCashOutstandingFld);
	DDV_MaxChars(pDX, m_StrCashOutstandingFld, 13);
	DDX_CBString(pDX, IDC_STATUS, m_StrStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRelatedGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CRelatedGeneral)
	ON_EN_KILLFOCUS(IDC_CREDIT_LIMIT_FLD, OnKillfocusCreditLimitFld)
	ON_EN_KILLFOCUS(IDC_OUTSTANDING_FLD, OnKillfocusOutstandingFld)
	ON_EN_KILLFOCUS(IDC_CASH_LIMIT_FLD, OnKillfocusCashLimitFld)
	ON_EN_KILLFOCUS(IDC_CASH_OUTSTANDING_FLD, OnKillfocusCashOutstandingFld)
	ON_EN_KILLFOCUS(IDC_LEDGER_BALANCE_FLD, OnKillfocusLedgerBalanceFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelatedGeneral message handlers


BOOL CRelatedGeneral::OnInitDialog() 
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

    m_bGeneral = TRUE;

	if (m_nDialogMode != MODE_DONE )
	{
		if ( m_nDialogMode != MODE_ADD )
			PopulateGeneralDialogFromDatabase ( sACFRecordOld );
		if ( m_nDialogMode == MODE_ADD )
		{
			myUtilsObject.GetSystemDate(strDate);	
			m_StrDateAddedFld = strDate;
			m_StrDateAddedFld = m_StrDateAddedFld.Mid(2,2) + "/" + m_StrDateAddedFld.Left(2) + "/" + m_StrDateAddedFld.Right(4);
			UpdateData(FALSE);
		}
	}
	else
	{
		m_StrAccountNumberFld = _T("");
		m_StrAvailableFld = _T("");
		m_StrCreditLimitFld = _T("");
		m_StrDateAddedFld = _T("");
		m_StrLedgerBalanceFld = _T("");
		m_StrMessageLbl = _T("");
		m_StrOutstandingFld = _T("");
		m_StrAccountTypeFld = _T("");
		m_StrCashAvailableFld = _T("");
		m_StrCashLimitFld = _T("");
		m_StrCashOutstandingFld = _T("");
		m_StrStatus = _T("");
	}
	
   if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_DONE )
	m_StrStatus = "VALID";
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRelatedGeneral::CleanGeneralDialog()
{
	m_StrAccountNumberFld = _T("");
	m_StrAvailableFld = _T("");
	m_StrCreditLimitFld = _T("");
	m_StrDateAddedFld = _T("");
	m_StrLedgerBalanceFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOutstandingFld = _T("");
	m_StrAccountTypeFld = _T("");
	m_StrCashAvailableFld = _T("");
	m_StrCashLimitFld = _T("");
	m_StrCashOutstandingFld = _T("");
	m_StrStatus = "VALID";

	UpdateData (FALSE);
}

BOOL CRelatedGeneral::PopulateGeneralDialogFromDatabase ( ACF01 sACFRecord )
{
	CString temp, strStatus;
	CMyUtils myUtilsObject; 

	m_StrAccountNumberFld = sACFRecord.primary_key.account_nbr;
	m_StrAccountTypeFld = sACFRecord.primary_key.account_type;
	if ( m_StrAccountTypeFld == "05" )
		m_StrAccountTypeFld = "US Dollar";
	else 	if ( m_StrAccountTypeFld == "06" )
		m_StrAccountTypeFld = "Phil.Peso";
	else 	if ( m_StrAccountTypeFld == "07" )
		m_StrAccountTypeFld = "Deferred";
	else 	if ( m_StrAccountTypeFld == "08")
		m_StrAccountTypeFld = "House Card";

	m_StrAvailableFld = sACFRecord.available;
	m_StrCreditLimitFld = sACFRecord.credit_limit;
	m_StrLedgerBalanceFld = sACFRecord.ledger_balance;
	m_StrOutstandingFld = sACFRecord.outstanding;

	strStatus = sACFRecord.status;
	if ( strStatus == "05" )
		m_StrStatus = "INVALID";
	else 	if ( strStatus == "00" )
		m_StrStatus = "VALID";

	m_StrCashAvailableFld = sACFRecord.cash_available;
	m_StrCashLimitFld = sACFRecord.cash_limit;
	m_StrCashOutstandingFld = sACFRecord.cash_outstanding;

	m_StrAvailableFld = myUtilsObject.DisplayAmount ( m_StrAvailableFld );
	m_StrCreditLimitFld = myUtilsObject.DisplayAmount ( m_StrCreditLimitFld );
	m_StrLedgerBalanceFld = myUtilsObject.DisplayAmount ( m_StrLedgerBalanceFld );
	m_StrOutstandingFld = myUtilsObject.DisplayAmount ( m_StrOutstandingFld );
	m_StrCashAvailableFld = myUtilsObject.DisplayAmount ( m_StrCashAvailableFld );
	m_StrCashLimitFld = myUtilsObject.DisplayAmount ( m_StrCashLimitFld );
	m_StrCashOutstandingFld = myUtilsObject.DisplayAmount ( m_StrCashOutstandingFld );


	m_StrDateAddedFld	 = sACFRecord.date_added;

	if ( !myUtilsObject.IsEmpty ( m_StrDateAddedFld, this))
			m_StrDateAddedFld = m_StrDateAddedFld.Right(2) + "/" + m_StrDateAddedFld.Mid(4,2) + "/" + m_StrDateAddedFld.Left(4);

	ZeroMemory ( &sACFRecordOld, sizeof ( ACF01 ) );
	memcpy ( &sACFRecordOld, &sACFRecord, sizeof ( ACF01 ) );

	UpdateData(FALSE);

	return TRUE;

}

BOOL CRelatedGeneral::CompareStructures()
{
	CString strTemp, strStatus;	
	CMyUtils myUtilsObject;

	strStatus = sACFRecordOld.status;

	if ( strStatus == "00" )
		strStatus = "VALID";
	else if ( strStatus == "05" )
		strStatus = "INVALID";

	if ( m_StrStatus != strStatus )
	{
		memcpy(m_strCurrentData, m_StrStatus, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strStatus, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Status", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCreditLimitFld, (char *)sACFRecordOld.credit_limit) != 0)
	{
		//m_StrCreditLimitFld = myUtilsObject.GetAmount(m_StrCreditLimitFld);
		//strTemp = sACFRecordOld.credit_limit;
		//strTemp = myUtilsObject.GetAmount(strTemp);
		m_StrCreditLimitFld = myUtilsObject.DisplayAmount(m_StrCreditLimitFld);
		strTemp = sACFRecordOld.credit_limit;
		strTemp = myUtilsObject.DisplayAmount(strTemp);
		memcpy(m_strCurrentData, m_StrCreditLimitFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Credit Limit", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrOutstandingFld, (char *)sACFRecordOld.outstanding) != 0)
	{
		m_StrOutstandingFld = myUtilsObject.GetAmount(m_StrOutstandingFld);
		strTemp = sACFRecordOld.outstanding;
		strTemp = myUtilsObject.GetAmount(strTemp);
		memcpy(m_strCurrentData, m_StrOutstandingFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Outstanding", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCashLimitFld, (char *)sACFRecordOld.cash_limit) != 0)
	{
		//m_StrCashLimitFld = myUtilsObject.GetAmount(m_StrCashLimitFld);
		//strTemp = sACFRecordOld.cash_limit;
		//strTemp = myUtilsObject.GetAmount(strTemp);
		m_StrCashLimitFld = myUtilsObject.DisplayAmount(m_StrCashLimitFld);
		strTemp = sACFRecordOld.cash_limit;
		strTemp = myUtilsObject.DisplayAmount(strTemp);
		memcpy(m_strCurrentData, m_StrCashLimitFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Cash Limit", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrCashOutstandingFld, (char *)sACFRecordOld.cash_outstanding) != 0)
	{
		m_StrCashOutstandingFld = myUtilsObject.GetAmount(m_StrCashOutstandingFld);
		strTemp = sACFRecordOld.cash_outstanding;
		strTemp = myUtilsObject.GetAmount(strTemp);
		memcpy(m_strCurrentData, m_StrCashOutstandingFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Cash Outstanding", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrLedgerBalanceFld, (char *)sACFRecordOld.ledger_balance) != 0)
	{
		//m_StrLedgerBalanceFld = myUtilsObject.GetAmount(m_StrLedgerBalanceFld);
		//strTemp = sACFRecordOld.ledger_balance;
		//strTemp = myUtilsObject.GetAmount(strTemp);
		m_StrLedgerBalanceFld = myUtilsObject.DisplayAmount(m_StrLedgerBalanceFld);
		strTemp = sACFRecordOld.ledger_balance;
		strTemp = myUtilsObject.DisplayAmount(strTemp);
		memcpy(m_strCurrentData, m_StrLedgerBalanceFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Ledger Balance", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	return TRUE;
}

BOOL CRelatedGeneral::OnSetActive() 
{
	if 	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
	{
		m_CntlCreditLimitFld.EnableWindow(FALSE);
		m_CntlLedgerBalanceFld.EnableWindow(FALSE);
		m_CntlOutstandingFld.EnableWindow(FALSE);
		m_CntlStatus.EnableWindow(FALSE);
//		m_ListButton.EnableWindow(FALSE);
		m_CntlCashLimitFld.EnableWindow(FALSE);
		m_CntlCashOutstandingFld.EnableWindow(FALSE);
	}
	else if ( m_nDialogMode == MODE_ADD )
	{
		m_CntlCreditLimitFld.EnableWindow(TRUE);
		m_CntlLedgerBalanceFld.EnableWindow(TRUE);
		m_CntlOutstandingFld.EnableWindow(TRUE);
		m_CntlStatus.EnableWindow(TRUE);
//		m_ListButton.EnableWindow(TRUE);
		m_CntlCashLimitFld.EnableWindow(TRUE);
		m_CntlCashOutstandingFld.EnableWindow(TRUE);
	}
	else if ( m_nDialogMode == MODE_EDIT ) /*7/12/02 lm SCR844 Disable outstanding fields on edit */
	{
		m_CntlCreditLimitFld.EnableWindow(TRUE);
		m_CntlLedgerBalanceFld.EnableWindow(TRUE);
		m_CntlOutstandingFld.EnableWindow(FALSE);
		m_CntlStatus.EnableWindow(TRUE);
		m_CntlCashLimitFld.EnableWindow(TRUE);
		m_CntlCashOutstandingFld.EnableWindow(FALSE);
	}

	return CPropertyPage::OnSetActive();
}

void CRelatedGeneral::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[300] = "";
    CString strType, temp;
    CHAR wstation[21]; DWORD wsize;
    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	if ( m_StrAccountTypeFld == "US Dollar" )
		strType = "05";
	else if ( m_StrAccountTypeFld == "Phil.Peso" )
		strType = "06";
	else if ( m_StrAccountTypeFld == "Deferred" )
		strType = "07";
	else if ( m_StrAccountTypeFld == "House Card" )
		strType = "08";

	strcpy ( strPrimaryKey, m_StrAccountNumberFld);
	strcat ( strPrimaryKey, strType );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,			(LPCTSTR)"ACF01",			sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,		(LPCTSTR)strPrimaryKey,		sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,			sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,		(LPCTSTR)strTime,			sizeof (sHistoryData.time_modified)); 
             
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

BOOL CRelatedGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL  CRelatedGeneral::PopulateDatabaseFromGeneralDialog ( pACF01 psACFRecord )
{
	CMyUtils myUtilsObject;
	CString strStatus;
	char strErrorMessage[250];
   CString strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrCreditLimitFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCreditLimitFld.SetFocus();
		return FALSE;
	}
   else
      m_StrCreditLimitFld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrAvailableFld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlAvailableFld.SetFocus();
		return FALSE;
	}
   else
      m_StrAvailableFld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrOutstandingFld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlOutstandingFld.SetFocus();
		return FALSE;
	}
   else
      m_StrOutstandingFld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrLedgerBalanceFld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlLedgerBalanceFld.SetFocus();
		return FALSE;
	}
   else
      m_StrLedgerBalanceFld = strAmount;

   strAmount = myUtilsObject.GenerateTranAmount ( m_StrCashLimitFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCashLimitFld.SetFocus();
		return FALSE;
	}
   else
      m_StrCashLimitFld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrCashAvailableFld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCashAvailableFld.SetFocus();
		return FALSE;
	}
   else
      m_StrCashAvailableFld = strAmount;

	strAmount = myUtilsObject.GenerateTranAmount ( m_StrCashOutstandingFld, 12, strErrorMessage );	
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCashOutstandingFld.SetFocus();
		return FALSE;
	}
   else
      m_StrCashOutstandingFld = strAmount;

	if ( m_StrStatus == "VALID" )
		strStatus = "00";
	else if ( m_StrStatus == "INVALID" )
		strStatus = "05";

	m_StrDateAddedFld = m_StrDateAddedFld.Right(4) + m_StrDateAddedFld.Mid(3,2) + m_StrDateAddedFld.Left(2);

   memcpy ( psACFRecord->status,		    strStatus,		       sizeof ( psACFRecord->status));
	memcpy ( psACFRecord->date_added,	    m_StrDateAddedFld,		   sizeof ( psACFRecord->date_added));
	memcpy ( psACFRecord->credit_limit,	    m_StrCreditLimitFld,	   sizeof ( psACFRecord->credit_limit));
	memcpy ( psACFRecord->available,	    m_StrAvailableFld,		   sizeof ( psACFRecord->available));
	memcpy ( psACFRecord->outstanding,	    m_StrOutstandingFld,	   sizeof ( psACFRecord->outstanding));
	memcpy ( psACFRecord->ledger_balance,   m_StrLedgerBalanceFld,	   sizeof ( psACFRecord->ledger_balance));
	memcpy ( psACFRecord->cash_limit,	    m_StrCashLimitFld,		   sizeof ( psACFRecord->cash_limit));
	memcpy ( psACFRecord->cash_available,	m_StrCashAvailableFld,     sizeof ( psACFRecord->cash_available));
	memcpy ( psACFRecord->cash_outstanding,	m_StrCashOutstandingFld,   sizeof ( psACFRecord->cash_outstanding));

	return TRUE;
}

void CRelatedGeneral::OnKillfocusCreditLimitFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;
	double nCreditLimit, nAvailable, nOutstanding;
	char temp[20];
	char  buffer[200];

	//int count;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

	int length = m_StrCreditLimitFld.GetLength();

	int res = strspn ( m_StrCreditLimitFld, "1234567890." );
	if ( res < length )
	{
		m_CntlCreditLimitFld.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
	
   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrCreditLimitFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCreditLimitFld.SetFocus();
		return ;
	}
	nCreditLimit = atof ( m_StrCreditLimitFld );
	nAvailable = atof ( m_StrAvailableFld );
	nOutstanding = atof ( m_StrOutstandingFld );

	nAvailable = nCreditLimit - nOutstanding;

	if ( nAvailable < 0 )
		nAvailable = 0;

    
	/* gcvt ( nAvailable, 15, temp ); */
	sprintf( buffer, "'%s'", temp ); 
	
	sprintf(temp,"%.2f",nAvailable);
	m_StrAvailableFld = temp;

	if ( m_StrAvailableFld.Right(1) == "." )
		m_StrAvailableFld = m_StrAvailableFld.Left(m_StrAvailableFld.GetLength()-1);

	UpdateData(FALSE);

}

void CRelatedGeneral::OnKillfocusOutstandingFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;
	double nCreditLimit, nAvailable, nOutstanding;
	char temp[15];

	UpdateData(TRUE);
	int length = m_StrOutstandingFld.GetLength();

	int res = strspn ( m_StrOutstandingFld, "1234567890." );
	if ( res < length )
	{
		m_CntlOutstandingFld.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrOutstandingFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlOutstandingFld.SetFocus();
		return ;
	}
	
	nCreditLimit = atof ( m_StrCreditLimitFld );
	nAvailable = atof ( m_StrAvailableFld );
	nOutstanding = atof ( m_StrOutstandingFld );

	nAvailable = nCreditLimit - nOutstanding;

	if ( nAvailable < 0 )
		nAvailable = 0;

	/*gcvt ( nAvailable, 15, temp );*/
	sprintf(temp,"%.2f",nAvailable);

	m_StrAvailableFld = temp;
	if ( m_StrAvailableFld.Right(1) == "." )
		m_StrAvailableFld = m_StrAvailableFld.Left(m_StrAvailableFld.GetLength()-1);

	UpdateData(FALSE);

	
}

/*void CRelatedGeneral::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	listDlg.m_nDialogMode = MODE_STATUS;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_StrStatusFld = listDlg.m_strStatus;
		UpdateData(FALSE);
		SetModified(TRUE);
	}	
	
}*/

void CRelatedGeneral::OnKillfocusCashLimitFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;
	double nCashLimit, nAvailable, nOutstanding;
	char temp[15];

	UpdateData(TRUE);
	int length = m_StrCashLimitFld.GetLength();

	int res = strspn ( m_StrCashLimitFld, "1234567890." );
	if ( res < length )
	{
		m_CntlCashLimitFld.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
	
   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrCashLimitFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCashLimitFld.SetFocus();
		return ;
	}

	nCashLimit = atof ( m_StrCashLimitFld );
	nAvailable = atof ( m_StrCashAvailableFld );
	nOutstanding = atof ( m_StrCashOutstandingFld );

	nAvailable = nCashLimit - nOutstanding;

	if ( nAvailable < 0 )
		nAvailable = 0;

/*	gcvt ( nAvailable, 15, temp );*/
	sprintf(temp,"%.2f",nAvailable);

	m_StrCashAvailableFld = temp;

	if ( m_StrCashAvailableFld.Right(1) == "." )
		m_StrCashAvailableFld = m_StrCashAvailableFld.Left(m_StrCashAvailableFld.GetLength()-1);

	UpdateData(FALSE);
	
}

void CRelatedGeneral::OnKillfocusCashOutstandingFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;
	double nCashLimit, nAvailable, nOutstanding;
	char temp[15];

	UpdateData(TRUE);
	int length = m_StrCashOutstandingFld.GetLength();

	int res = strspn ( m_StrCashOutstandingFld, "1234567890." );
	if ( res < length )
	{
		m_CntlCashOutstandingFld.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}

	CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrCashOutstandingFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlCashOutstandingFld.SetFocus();
		return ;
	}


	nCashLimit = atof ( m_StrCashLimitFld );
	nAvailable = atof ( m_StrCashAvailableFld );
	nOutstanding = atof ( m_StrCashOutstandingFld );

	nAvailable = nCashLimit - nOutstanding;

	if ( nAvailable < 0 )
		nAvailable = 0;

	/*gcvt ( nAvailable, 15, temp );*/
	sprintf(temp,"%.2f",nAvailable);

	m_StrCashAvailableFld = temp;
	if ( m_StrCashAvailableFld.Right(1) == "." )
		m_StrCashAvailableFld = m_StrCashAvailableFld.Left(m_StrCashAvailableFld.GetLength()-1);
	UpdateData(FALSE);
	
}

BOOL CRelatedGeneral::OnApply() 
{
	OnKillfocusCashLimitFld();	
	OnKillfocusCashOutstandingFld() ;
	OnKillfocusCreditLimitFld() ;
	OnKillfocusOutstandingFld() ;
	return CPropertyPage::OnApply();
}


void CRelatedGeneral::OnKillfocusLedgerBalanceFld() 
{
   char strErrorMessage[250];
   CMyUtils myUtilsObject;

	UpdateData(TRUE);
	int length = m_StrLedgerBalanceFld.GetLength();

	int res = strspn ( m_StrLedgerBalanceFld, "1234567890." );
	if ( res < length )
	{
		m_CntlLedgerBalanceFld.SetFocus();
		m_StrMessageLbl = "Invalid Character";
		UpdateData(FALSE);
		return;
	}
   CString strAmount = myUtilsObject.GenerateTranAmount ( m_StrLedgerBalanceFld, 12, strErrorMessage );
	if ( strAmount == "Error" )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
      m_CntlLedgerBalanceFld.SetFocus();
		return ;
	}
	
}


