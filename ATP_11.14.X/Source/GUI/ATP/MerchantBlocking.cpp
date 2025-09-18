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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantBlocking.cpp  $
   
      Rev 1.13   Jul 08 2004 13:00:38   dirby
   Updated History File logging to make sure Merchant ID is in the
   File_Primary_Key and to make sure fields are not accidently logged
   when no changes have been made.
   SCR 1421
   
      Rev 1.12   Dec 01 2003 11:29:32   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.11   Feb 10 2002 00:34:26   dirby
   Modified PopulateHistoryTable function so the unique_id field
   does not give a constraint violation.  It was frequently giving a
   unique constraint violation when updating the general and
   blocking tabs because it could do more than 1 within the same
   millisecond.  Also, if the data server returned with an error,
   there was an UpdateData(FALSE) command that would crash
   the program because it was trying to update a window that did
   not exist.  I replaced the command with an informational box.
   
      Rev 1.10   Feb 11 2000 16:48:48   iarustam
   SCR #400,401
   
      Rev 1.9   Jan 13 2000 17:07:46   mbysani
   bug fixed
   
      Rev 1.8   Dec 16 1999 09:34:04   mbysani
   corrected some error based on error reports
   
      Rev 1.1   Sep 22 1999 15:51:48   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:40   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:42   iarustam
   Initial Release
   
      Rev 1.5   Jun 04 1999 17:40:28   iarustam
   Bug fixes
   
      Rev 1.4   May 10 1999 11:22:14   dperson
   Standardized error messages
   
      Rev 1.3   Apr 29 1999 14:56:30   iarustam
   Bug fixes
   
      Rev 1.2   Apr 22 1999 15:34:10   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
*
************************************************************************************/
// MerchantBlocking.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantBlocking.h"
#include "MyUtils.h"
#include "SharedMem.h"

extern "C"
{
#include "PteTime.h"
#include "Txutils.h"
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

extern CString  MerchantID;
extern CString  OrganizationID;

/////////////////////////////////////////////////////////////////////////////
// CMerchantBlocking property page

IMPLEMENT_DYNCREATE(CMerchantBlocking, CPropertyPage)

CMerchantBlocking::CMerchantBlocking() : CPropertyPage(CMerchantBlocking::IDD)
{
	//{{AFX_DATA_INIT(CMerchantBlocking)
	m_StrBlockingEndFld = _T("");
	m_StrBlockingStartFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantBlocking::~CMerchantBlocking()
{
}

void CMerchantBlocking::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantBlocking)
	DDX_Control(pDX, IDC_BLOCKING_START_FLD, m_CntlBlockingStartFld);
	DDX_Control(pDX, IDC_BLOCKING_END_FLD, m_CntlBlockingEndFld);
	DDX_Text(pDX, IDC_BLOCKING_END_FLD, m_StrBlockingEndFld);
	DDV_MaxChars(pDX, m_StrBlockingEndFld, 8);
	DDX_Text(pDX, IDC_BLOCKING_START_FLD, m_StrBlockingStartFld);
	DDV_MaxChars(pDX, m_StrBlockingStartFld, 8);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantBlocking, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantBlocking)
	ON_EN_KILLFOCUS(IDC_BLOCKING_START_FLD, OnKillfocusBlockingStartFld)
	ON_EN_KILLFOCUS(IDC_BLOCKING_END_FLD, OnKillfocusBlockingEndFld)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantBlocking message handlers

BOOL CMerchantBlocking::OnInitDialog() 
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

    m_bBlocking = TRUE;
	m_bValidStartDay = TRUE;
	m_bValidEndDay = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableBlockingDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateBlockingDialogFromDatabase ( sMerchantRecordOld );
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantBlocking::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantBlocking::EnableBlockingDialog ( int nStatus )
{
	m_CntlBlockingEndFld.EnableWindow(nStatus);
	m_CntlBlockingStartFld.EnableWindow(nStatus);
}

BOOL CMerchantBlocking::PopulateBlockingDialogFromDatabase ( MCF01 sMerchantRecord )
{
	CString temp;
	CMyUtils myUtilsObject;

	if ( m_nDialogMode != MODE_VIEW && m_bBlocking )	
		EnableBlockingDialog(TRUE);

	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );
	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;

	m_StrBlockingStartFld = sMerchantRecord.block_start_date;
	m_StrBlockingEndFld	  =	sMerchantRecord.block_end_date;

	char yyyymmdd[9];
	LONG julian = atol((LPCTSTR) sMerchantRecord.block_start_date);
	if (julian != 0)
	{
		Txutils_Calculate_Gregorian_Date(julian, yyyymmdd);	
		m_StrBlockingStartFld = yyyymmdd;
	}
	else 
		m_StrBlockingStartFld.Empty();
	julian = atol((LPCTSTR) sMerchantRecord.block_end_date);
	if (julian != 0)
	{
		Txutils_Calculate_Gregorian_Date(julian, yyyymmdd);
		m_StrBlockingEndFld = yyyymmdd;
	}
	else
		m_StrBlockingEndFld.Empty();
	if (!myUtilsObject.IsEmpty (m_StrBlockingStartFld, this))
		m_StrBlockingStartFld = m_StrBlockingStartFld.Right(2) + 
								m_StrBlockingStartFld.Mid(4,2) + 
								m_StrBlockingStartFld.Left(4);

	if (!myUtilsObject.IsEmpty (m_StrBlockingEndFld, this))
		m_StrBlockingEndFld = m_StrBlockingEndFld.Right(2) + 
							   m_StrBlockingEndFld.Mid(4,2) + 	
								m_StrBlockingEndFld.Left(4);
	
	UpdateData(FALSE);

	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));


	return TRUE;

}

void CMerchantBlocking::CleanBlockingDialog()
{
	m_StrBlockingEndFld = _T("");
	m_StrBlockingStartFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");

	UpdateData (FALSE);
}

BOOL  CMerchantBlocking::PopulateDatabaseFromBlockingDialog ( pMCF01 psMerchantRecord )
{
	if ( !m_bBlocking )
		return TRUE;

	if ( m_StrBlockingEndFld.IsEmpty() != TRUE )
		m_StrBlockingEndFld = m_StrBlockingEndFld.Right(4) + 
									m_StrBlockingEndFld.Mid(2,2) + 
									m_StrBlockingEndFld.Left(2);

	if ( m_StrBlockingStartFld.IsEmpty() != TRUE )
		m_StrBlockingStartFld = m_StrBlockingStartFld.Right(4) + 
									m_StrBlockingStartFld.Mid(2,2) + 
									m_StrBlockingStartFld.Left(2);
	
	LONG nJulianDate;
	nJulianDate = Txutils_Calculate_Julian_Date ((BYTE *)(LPCTSTR)m_StrBlockingStartFld) ;
	m_StrBlockingStartFld.Format("%ld",nJulianDate);	

	nJulianDate = Txutils_Calculate_Julian_Date ((BYTE *)(LPCTSTR)m_StrBlockingEndFld) ;
	m_StrBlockingEndFld.Format("%ld",nJulianDate);	


	memcpy ( psMerchantRecord->block_start_date,	m_StrBlockingStartFld,	sizeof ( psMerchantRecord->block_start_date));
	memcpy ( psMerchantRecord->block_end_date,		m_StrBlockingEndFld,	sizeof ( psMerchantRecord->block_end_date));
	

//	memcpy ( &sMerchantRecordNew, psMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

BOOL CMerchantBlocking::CompareStructures()
{
   if ( m_bBlocking == TRUE )
   {
      if (strcmp ( m_StrBlockingStartFld, (char *)sMerchantRecordOld.block_start_date) != 0)
      {
         if ( (0 != strcmp(m_StrBlockingStartFld, NULL_STR)) ||
              (0 != strcmp((char *)sMerchantRecordOld.block_start_date,"0")) )
         {
            memset(m_strCurrentData,  0x00, sizeof (m_strCurrentData) );
            memset(m_strPreviousData, 0x00, sizeof (m_strPreviousData));
            memset(m_strFieldTag,     0x00, sizeof (m_strFieldTag)    );

            memcpy(m_strCurrentData, m_StrBlockingStartFld, sizeof (m_strCurrentData));
            memcpy(m_strPreviousData, sMerchantRecordOld.block_start_date, sizeof (m_strPreviousData));
            memcpy(m_strFieldTag, "Blocking Start Date", sizeof (m_strFieldTag));

            PopulateHistoryTable();
         }
      }

      if (strcmp ( m_StrBlockingEndFld, (char *)sMerchantRecordOld.block_end_date) != 0)
      {
         if ( (0 != strcmp(m_StrBlockingEndFld, NULL_STR)) ||
              (0 != strcmp((char *)sMerchantRecordOld.block_end_date,"0")) )
         {
            memset(m_strCurrentData,  0x00, sizeof (m_strCurrentData) );
            memset(m_strPreviousData, 0x00, sizeof (m_strPreviousData));
            memset(m_strFieldTag,     0x00, sizeof (m_strFieldTag)    );

            memcpy(m_strCurrentData, m_StrBlockingEndFld, sizeof (m_strCurrentData));
            memcpy(m_strPreviousData, sMerchantRecordOld.block_end_date, sizeof (m_strPreviousData));
            memcpy(m_strFieldTag, "Blocking End Date", sizeof (m_strFieldTag));

            PopulateHistoryTable();
         }
      }
   }

	return TRUE;
}

void CMerchantBlocking::PopulateHistoryTable()
{
   HST01 sHistoryData;
   CHAR dest_str[18];   
   CMyUtils myUtilsObject;
   char strErrorMsg[200] = "";
   char strDate[9]       = "";
   char strTime[7]       = "";
   char strPrimaryKey[50]="";
   CHAR s_merch_ctr[4]   = "";
   CHAR strDisplayErrMsg[250];
   CString temp;


   strcpy( strPrimaryKey, MerchantID     );
   strcat( strPrimaryKey, OrganizationID );

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


void CMerchantBlocking::OnKillfocusBlockingStartFld() 
{
/*	CMyUtils myUtilsObject;

	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData (FALSE);


	if ( myUtilsObject.IsEmpty ( m_StrBlockingStartFld, this ))
		return;

	if ( !ValidateDate  ( m_StrBlockingStartFld ) )
	{
		m_CntlBlockingStartFld.SetFocus();
		return;
	}
	ShowWindow(SW_SHOWNORMAL);
	UpdateData (FALSE); */

/*	UpdateData (TRUE);

	CMyUtils myUtilsObject;

	m_StrMessageLbl = "";
	UpdateData (FALSE);

	if ( !m_bValidEndDay )
	{
		m_StrMessageLbl = "ERROR: Invalid End Day";
		UpdateData(FALSE);
		return;
	}


	if ( myUtilsObject.IsEmpty ( m_StrBlockingStartFld, this ))
		return;


	int length;

	length = m_StrBlockingStartFld.GetLength();

	if (length <8 && length != 0)
	{
		m_CntlBlockingStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid date";
		UpdateData(FALSE);
		m_bValidStartDay = FALSE;
		return;

	}

	int res = strspn ( m_StrBlockingStartFld, "1234567890/");

	if ( res < length )
	{
		m_CntlBlockingStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid character";
		m_bValidStartDay = FALSE;
		UpdateData(FALSE);
		return;
	}

	if (!ValidateDate ( m_StrBlockingStartFld ))
	{
		m_CntlBlockingStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid date format";
		m_bValidStartDay = FALSE;
		UpdateData(FALSE);
		return;
	}
	
	if ( length == 8 )
	{
		m_StrBlockingStartFld = m_StrBlockingStartFld.Left(2) + "/" + 
								m_StrBlockingStartFld.Mid(2,2) + "/" +
								m_StrBlockingStartFld.Right(4);
		UpdateData(FALSE);
	}

	if ( length == 9 )
	{
		int pos = m_StrBlockingStartFld.Find ("/");

		if ( pos == 2 )	
		{
			m_StrBlockingStartFld = m_StrBlockingStartFld.Left(5) + "/" + 								
									m_StrBlockingStartFld.Right(4);
			UpdateData(FALSE);
		}

		if ( pos == 4 )	
		{
			m_StrBlockingStartFld = m_StrBlockingStartFld.Left(2) + "/" + 								
									m_StrBlockingStartFld.Right(7);
			UpdateData(FALSE);
		}
	}

	nStartDay = nDay;
	nStartMonth = nMonth;
	nStartYear = nYear;

	m_bValidStartDay = TRUE;*/
}

void CMerchantBlocking::OnKillfocusBlockingEndFld() 
{
	/*CMyUtils myUtilsObject;

	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData (FALSE);

	if ( myUtilsObject.IsEmpty ( m_StrBlockingEndFld, this ))
		return;

	if ( !ValidateDate ( m_StrBlockingEndFld ) )
	{
		m_CntlBlockingEndFld.SetFocus();
		return;
	}*/
/*	CString strEndMonth, strStartMonth;
	CString strEndDay, strStartDay;
	CString strEndYear, strStartYear;

	CMyUtils myUtilsObject;

	int nEndMonth;
	int nEndDay;
	int nEndYear;

	if ( !m_bValidStartDay )
	{
		m_StrMessageLbl = "ERROR: Invalid Start Day";
		UpdateData(FALSE);
		return;
	}

	UpdateData (TRUE);

	if ( myUtilsObject.IsEmpty ( m_StrBlockingEndFld, this ))
		return;

	m_StrMessageLbl = "";
	UpdateData (FALSE);

	int length;

	length = m_StrBlockingEndFld.GetLength();

	if (length <8 && length != 0)
	{
		m_CntlBlockingEndFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid date";
		m_bValidEndDay = FALSE;
		UpdateData(FALSE);
		return;
	}

	int res = strspn ( m_StrBlockingEndFld, "1234567890/");

	if ( res < length )
	{
		m_CntlBlockingEndFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid character";
		m_bValidEndDay = FALSE;
		UpdateData(FALSE);
		return;
	}

	if (!ValidateDate ( m_StrBlockingEndFld ))
	{
	//	m_CntlBlockingEndFld.SetFocus();
		m_bValidEndDay = FALSE;
		UpdateData(FALSE);
		return;
	}
	
	if ( length == 8 )
	{
		m_StrBlockingEndFld = m_StrBlockingEndFld.Left(2) + "/" + 
								m_StrBlockingEndFld.Mid(2,2) + "/" +
								m_StrBlockingEndFld.Right(4);
		UpdateData(FALSE);
	}

	if ( length == 9 )
	{
		int pos = m_StrBlockingEndFld.Find ("/");

		if ( pos == 2 )	
		{
			m_StrBlockingEndFld = m_StrBlockingEndFld.Left(5) + "/" + 								
									m_StrBlockingEndFld.Right(4);
			UpdateData(FALSE);
		}

		if ( pos == 4 )	
		{
			m_StrBlockingEndFld = m_StrBlockingEndFld.Left(2) + "/" + 								
									m_StrBlockingEndFld.Right(7);
			UpdateData(FALSE);
		}
	}

	strEndMonth = m_StrBlockingEndFld.Mid(3,2);
	strEndDay = m_StrBlockingEndFld.Left(2);
	strEndYear = m_StrBlockingEndFld.Right(4);

	strStartMonth = m_StrBlockingStartFld.Mid(3,2);
	strStartDay = m_StrBlockingStartFld.Left(2);
	strStartYear = m_StrBlockingStartFld.Right(4);

	nEndMonth = atoi ( strEndMonth );
	nEndDay = atoi ( strEndDay );
	nEndYear = atoi ( strEndYear );

	nStartMonth = atoi ( strStartMonth );
	nStartDay = atoi ( strStartDay );
	nStartYear = atoi ( strStartYear );

	if ( nEndYear < nStartYear )
	{
		m_StrMessageLbl = "ERROR: Blocking Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		m_bValidOrder = FALSE;
		return;
//		m_CntlBlockingEndFld.SetFocus();
	}

	if ( nEndYear == nStartYear && nEndMonth < nStartMonth )
	{
		m_StrMessageLbl = "ERROR: Blocking Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		m_bValidOrder = FALSE;
		return;
//		m_CntlBlockingEndFld.SetFocus();
	}

	if ( nEndYear == nStartYear && nEndMonth == nStartMonth  && nEndDay < nStartDay)
	{
		m_StrMessageLbl = "ERROR: Blocking Start Day Cannot be greater than End Day";
		UpdateData(FALSE);
		m_bValidOrder = FALSE;
		return;
//		m_CntlBlockingEndFld.SetFocus();
	}

	m_bValidEndDay = TRUE;
   m_bValidOrder = TRUE;
*/
	
}

BOOL CMerchantBlocking::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

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


BOOL CMerchantBlocking::ValidateDates()
{
	CMyUtils myUtilsObject;


	UpdateData (TRUE);

	if ( myUtilsObject.IsEmpty ( m_StrBlockingStartFld, this ) && 
		 myUtilsObject.IsEmpty ( m_StrBlockingEndFld, this ))
	return TRUE;

	if (!ValidateDate ( m_StrBlockingStartFld ))
	{
		m_CntlBlockingStartFld.SetFocus();
		UpdateData(FALSE);
		return FALSE;
	}

	if (!ValidateDate ( m_StrBlockingEndFld ))
	{
		m_CntlBlockingEndFld.SetFocus();
		UpdateData(FALSE);
		return FALSE;
	}

   if ( !myUtilsObject.IsEmpty ( m_StrBlockingStartFld, this ) && 
		  !myUtilsObject.IsEmpty ( m_StrBlockingEndFld, this ))
   {
		if ( m_StrBlockingStartFld.Right(4) > m_StrBlockingEndFld.Right(4) )
	   {
		   m_StrMessageLbl = "ERROR: Start Day cannot be greater than End Day";
		   UpdateData(FALSE);
		   return FALSE;
	   }

	   if ( m_StrBlockingStartFld.Right(4) == m_StrBlockingEndFld.Right(4) && 
		   m_StrBlockingEndFld.Mid(2,2) <  m_StrBlockingStartFld.Mid(2,2) )
	   {
		   m_StrMessageLbl = "ERROR: Start Day cannot be greater than End Day";
		   UpdateData(FALSE);
		   return FALSE;
	   }

	   if ( m_StrBlockingStartFld.Right(4) == m_StrBlockingEndFld.Right(4) && 
		    m_StrBlockingEndFld.Mid(2,2) ==  m_StrBlockingStartFld.Mid(2,2) && 
		    m_StrBlockingEndFld.Left(2) < m_StrBlockingStartFld.Left(2) )
	   {
		   m_StrMessageLbl = "ERROR: Start Day cannot be greater than End Day";
		   UpdateData(FALSE);
		   return FALSE;
	   }
   }
	return TRUE;

}

BOOL CMerchantBlocking::OnSetActive() 
{
//	ValidateDates()	;

	return CPropertyPage::OnSetActive();
}

BOOL CMerchantBlocking::OnApply() 
{
	if ( !ValidateDates() )
		return FALSE;
	else	
		return CPropertyPage::OnApply();
}

void CMerchantBlocking::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CPropertyPage::OnPaint() for painting messages
}
