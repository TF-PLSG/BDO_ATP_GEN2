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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredGeneral.cpp  $
   
      Rev 1.11   Dec 01 2003 11:29:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Jan 05 2000 14:53:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:26   iarustam
   Initial Release
   
      Rev 1.7   Jun 11 1999 14:07:34   iarustam
   Bug fixes
   
   
      Rev 1.6   Jun 02 1999 16:25:06   iarustam
   Bug fixes
   
      Rev 1.5   May 10 1999 11:17:04   dperson
   Added PVCS header
*   
************************************************************************************/
// DeferredGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DeferredGeneral.h"
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
// CDeferredGeneral property page

IMPLEMENT_DYNCREATE(CDeferredGeneral, CPropertyPage)

CDeferredGeneral::CDeferredGeneral() : CPropertyPage(CDeferredGeneral::IDD)
{
	//{{AFX_DATA_INIT(CDeferredGeneral)
	m_StrFactorFld = _T("");
	m_StrMaxAmountFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMinAmountFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrTermLengthFld = _T("");
	//}}AFX_DATA_INIT
}

CDeferredGeneral::~CDeferredGeneral()
{
}

void CDeferredGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeferredGeneral)
	DDX_Control(pDX, IDC_MIN_AMOUNT_FLD, m_CntlMinAmountFld);
	DDX_Control(pDX, IDC_MAX_AMOUNT_FLD, m_CntlMaxAmountFld);
	DDX_Control(pDX, IDC_FACTOR_FLD, m_CntlFactorFld);
	DDX_Text(pDX, IDC_FACTOR_FLD, m_StrFactorFld);
	DDV_MaxChars(pDX, m_StrFactorFld, 7);
	DDX_Text(pDX, IDC_MAX_AMOUNT_FLD, m_StrMaxAmountFld);
	DDV_MaxChars(pDX, m_StrMaxAmountFld, 11);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_MIN_AMOUNT_FLD, m_StrMinAmountFld);
	DDV_MaxChars(pDX, m_StrMinAmountFld, 11);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_TERM_LENGTH_FLD, m_StrTermLengthFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeferredGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CDeferredGeneral)
	ON_EN_CHANGE(IDC_FACTOR_FLD, OnChangeFactorFld)
	ON_EN_CHANGE(IDC_MAX_AMOUNT_FLD, OnChangeMaxAmountFld)
	ON_EN_CHANGE(IDC_MIN_AMOUNT_FLD, OnChangeMinAmountFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeferredGeneral message handlers

BOOL CDeferredGeneral::OnInitDialog() 
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
			PopulateGeneralDialogFromDatabase ( sDEFRecordOld );
	}   
	else
	{
		m_StrMessageLbl = _T("");
		m_StrFactorFld = _T("");
		m_StrMaxAmountFld = _T("");
		m_StrMerchantIDFld = _T("");
		m_StrMessageLbl = _T("");
		m_StrMinAmountFld = _T("");
		m_StrOrganizationIDFld = _T("");
		m_StrTermLengthFld = _T("");
	
	}

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeferredGeneral::OnChangeFactorFld() 
{
	SetModified ( TRUE );
	
}

void CDeferredGeneral::OnChangeMaxAmountFld() 
{
	SetModified ( TRUE );
	
}

void CDeferredGeneral::OnChangeMinAmountFld() 
{
	SetModified ( TRUE );
	
}

void CDeferredGeneral::CleanGeneralDialog()
{
	m_StrFactorFld = _T("");
	m_StrMaxAmountFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMinAmountFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrTermLengthFld = _T("");

	UpdateData (FALSE);
}

void CDeferredGeneral::PopulateHistoryTable()
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

	strcpy ( strPrimaryKey, m_StrMerchantIDFld);
	strcat ( strPrimaryKey, m_StrOrganizationIDFld	);
	strcat ( strPrimaryKey, m_StrTermLengthFld );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"DEF01",		sizeof (sHistoryData.file_name));
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
		UpdateData( FALSE );
	}
}

BOOL CDeferredGeneral::PopulateGeneralDialogFromDatabase ( DEF01 sDEFRecord )
{
	CString strMax, strMin;
	CMyUtils myUtilsObject;

	m_StrFactorFld = sDEFRecord.factor;
	m_StrMerchantIDFld = sDEFRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sDEFRecord.primary_key.organization_id;
	m_StrTermLengthFld = sDEFRecord.primary_key.term_length;
	strMin = sDEFRecord.deferred_min_amount;
	strMax = sDEFRecord.deferred_max_amount;
	m_StrMaxAmountFld = myUtilsObject.DisplayAmount( strMax );
	m_StrMinAmountFld = myUtilsObject.DisplayAmount( strMin );

	memcpy ( &sDEFRecordOld, &sDEFRecord, sizeof ( DEF01));

	return TRUE;


}

BOOL CDeferredGeneral::CompareStructures()
{
	CString strAmount;
	CMyUtils myUtilsObject;
	if (strcmp ( m_StrFactorFld, (char *)sDEFRecordOld.factor) != 0)
	{
		memcpy(m_strCurrentData, m_StrFactorFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sDEFRecordOld.factor, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Factor", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	strAmount = sDEFRecordOld.deferred_max_amount;
	strAmount = myUtilsObject.GetAmount ( strAmount );
	if (strcmp ( m_StrMaxAmountFld, strAmount) != 0)
	{
		memcpy(m_strCurrentData, m_StrMaxAmountFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strAmount, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Deferred Maximum Amount", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	strAmount = sDEFRecordOld.deferred_min_amount;
	strAmount = myUtilsObject.GetAmount ( strAmount );
	if (strcmp ( m_StrMinAmountFld, strAmount) != 0)
	{
		memcpy(m_strCurrentData, m_StrMinAmountFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, strAmount, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Deferred Minimum Amount", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	return TRUE;
}

BOOL CDeferredGeneral::OnSetActive() 
{
	if 	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
	{
		m_CntlFactorFld.EnableWindow(FALSE);
		m_CntlMinAmountFld.EnableWindow(FALSE);
		m_CntlMaxAmountFld.EnableWindow(FALSE);
	}
	else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT )
	{
		m_CntlFactorFld.EnableWindow(TRUE);
		m_CntlMinAmountFld.EnableWindow(TRUE);
		m_CntlMaxAmountFld.EnableWindow(TRUE);
	}   
	
	return CPropertyPage::OnSetActive();
}

BOOL  CDeferredGeneral::PopulateDatabaseFromGeneralDialog ( pDEF01 psDEFRecord )
{

	CString strMax, strMin;
	CMyUtils myUtilsObject;
	char ErrorMessage[200];

	strMin = myUtilsObject.GenerateTranAmount(m_StrMinAmountFld, 10, ErrorMessage) ;
	if ( strMin == "Error" )
	{
		m_StrMessageLbl = ErrorMessage;
		UpdateData(FALSE);
		m_CntlMinAmountFld.SetFocus();
		return FALSE;
	}
	strMax = myUtilsObject.GenerateTranAmount(m_StrMaxAmountFld, 10, ErrorMessage) ;
	if ( strMax == "Error" )
	{
		m_StrMessageLbl = ErrorMessage;
		UpdateData(FALSE);
		m_CntlMaxAmountFld.SetFocus();
		return FALSE;
	}

	memcpy ( psDEFRecord->factor, m_StrFactorFld, sizeof (psDEFRecord->factor));
	memcpy ( psDEFRecord->deferred_max_amount, strMax, sizeof (psDEFRecord->deferred_max_amount));
	memcpy ( psDEFRecord->deferred_min_amount, strMin, sizeof (psDEFRecord->deferred_min_amount));

	return TRUE;

}


BOOL CDeferredGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
