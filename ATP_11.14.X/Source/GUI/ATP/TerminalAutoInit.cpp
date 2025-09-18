// TerminalAutoInit.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "TerminalAutoInit.h"
#include "MyUtils.h"

#include "SharedMem.h"	
extern UCF01 ucf01;
extern "C"
{
  #include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CTerminalAutoInit property page

IMPLEMENT_DYNCREATE(CTerminalAutoInit, CPropertyPage)

CTerminalAutoInit::CTerminalAutoInit() : CPropertyPage(CTerminalAutoInit::IDD)
{
	//{{AFX_DATA_INIT(CTerminalAutoInit)
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrAutoInitTime = _T("");
	m_StrAutoLoadRef = _T("");
	m_StrAutoLoadTime = _T("");
	m_StrAutoInitRef = _T("");
	//}}AFX_DATA_INIT
}

CTerminalAutoInit::~CTerminalAutoInit()
{
}

void CTerminalAutoInit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalAutoInit)
	DDX_Control(pDX, IDC_AUTO_INIT_REF, m_CntlAutoInitRef);
	DDX_Control(pDX, IDC_AUTO_LOAD_TIME, m_CntlAutoLoadTime);
	DDX_Control(pDX, IDC_AUTO_LOAD_REF, m_CntlAutoLoadRef);
	DDX_Control(pDX, IDC_AUTO_INIT_TIME, m_CntlAutoInitTime);
	DDX_Control(pDX, IDC_AUTOLOAD_CHECK, m_CntlAutoLoadCheck);
	DDX_Control(pDX, IDC_AUTOINIT_CHECK, m_CntlAutoInitCheck);
	DDX_Control(pDX, IDC_AUTOICEPAC_CHECK, m_CntlAutoIcepacCheck);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_AUTO_INIT_TIME, m_StrAutoInitTime);
	DDV_MaxChars(pDX, m_StrAutoInitTime, 5);
	DDX_Text(pDX, IDC_AUTO_LOAD_REF, m_StrAutoLoadRef);
	DDV_MaxChars(pDX, m_StrAutoLoadRef, 6);
	DDX_Text(pDX, IDC_AUTO_LOAD_TIME, m_StrAutoLoadTime);
	DDV_MaxChars(pDX, m_StrAutoLoadTime, 5);
	DDX_Text(pDX, IDC_AUTO_INIT_REF, m_StrAutoInitRef);
	DDV_MaxChars(pDX, m_StrAutoInitRef, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerminalAutoInit, CPropertyPage)
	//{{AFX_MSG_MAP(CTerminalAutoInit)
	ON_EN_KILLFOCUS(IDC_AUTO_LOAD_TIME, OnKillfocusAutoLoadTime)
	ON_EN_KILLFOCUS(IDC_AUTO_INIT_TIME, OnKillfocusAutoInitTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalAutoInit message handlers


BOOL CTerminalAutoInit::CheckTime (CString strTime)
{    
    CString strHour;
    CString strMin;
    int hour, min, len;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if ((len = strTime.GetLength()) == 0) // spaces ok
		return TRUE;

    if ( len < 4 )
	{
		m_StrMessageLbl = "ERROR: Invalid time. Use (hhmm) or (hh:mm)";
		UpdateData(FALSE);
		return FALSE;
	}	

	int pos = strTime.Find (":");

	if ( len == 5 && pos != 2 ) //hh:mm
	{
		m_StrMessageLbl = "ERROR: Invalid time. Use (hhmm) or (hh:mm)";
		UpdateData(FALSE);
		return FALSE;
	}
	else
		if(len == 4 && pos >= 0)
		{
			m_StrMessageLbl = "ERROR: Invalid time. Use (hhmm) or (hh:mm)";
			UpdateData(FALSE);
			return FALSE;
		}

	hour = atoi(strTime.Left(2));
	min  = atoi(strTime.Right(2));
		
   if (hour > 23 || min > 59)   
   {
	    m_StrMessageLbl = "ERROR: Invalid time. Use (hhmm) or (hh:mm)";
		UpdateData(FALSE);
		return FALSE;
   }	
  return TRUE;
}




void CTerminalAutoInit::CleanAutoInitDialog()
{
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrAutoInitTime = _T("");
    m_StrAutoInitRef = _T("");
	m_StrAutoLoadTime = _T("");
	m_StrAutoLoadRef = _T("");

	m_CntlAutoInitCheck.SetCheck(0);
	m_CntlAutoLoadCheck.SetCheck(0);
	m_CntlAutoIcepacCheck.SetCheck(0);

	UpdateData (FALSE);
}


BOOL CTerminalAutoInit::CompareStructures(DCF01 sTerminalRecordNew)
{
	if (strcmp ( (char *)sTerminalRecordNew.auto_init_activation, (char *)sTerminalRecordOld.auto_init_activation) != 0)
	{
		memcpy(m_strCurrentData, sTerminalRecordNew.auto_init_activation, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_init_activation, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Init Activation", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ((char *)sTerminalRecordNew.auto_program_load_activation, (char *)sTerminalRecordOld.auto_program_load_activation) != 0)
	{
		memcpy(m_strCurrentData, sTerminalRecordNew.auto_program_load_activation, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_program_load_activation, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Program Load Activation", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( (char *)sTerminalRecordNew.auto_icepac_activation, (char *)sTerminalRecordOld.auto_icepac_activation) != 0)
	{
		memcpy(m_strCurrentData,  sTerminalRecordNew.auto_icepac_activation, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_icepac_activation, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto ICEPAC Activation", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( (char *)sTerminalRecordNew.auto_init_time, (char *)sTerminalRecordOld.auto_init_time) != 0)
	{
		memcpy(m_strCurrentData,  sTerminalRecordNew.auto_init_time, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_init_time, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Init Time", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( (char *)sTerminalRecordNew.auto_program_load_time, (char *)sTerminalRecordOld.auto_program_load_time) != 0)
	{
		memcpy(m_strCurrentData,  sTerminalRecordNew.auto_program_load_time, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_program_load_time, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Program Load Time", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( (char *)sTerminalRecordNew.auto_init_ref_number, (char *)sTerminalRecordOld.auto_init_ref_number) != 0)
	{
		memcpy(m_strCurrentData, sTerminalRecordNew.auto_init_ref_number, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_init_ref_number, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Init Ref Nbr", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( (char *)sTerminalRecordNew.auto_program_load_ref_number, (char *)sTerminalRecordOld.auto_program_load_ref_number) != 0)
	{
		memcpy(m_strCurrentData, sTerminalRecordNew.auto_program_load_ref_number, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.auto_program_load_ref_number, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Auto Program Load Ref Nbr", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}


	return TRUE;
}


void CTerminalAutoInit::EnableAutoInitDialog ( int nStatus )
{
	m_CntlAutoInitTime.EnableWindow(nStatus);
	m_CntlAutoInitRef.EnableWindow(nStatus);
	m_CntlAutoLoadTime.EnableWindow(nStatus);
	m_CntlAutoLoadRef.EnableWindow(nStatus);

	m_CntlAutoInitCheck.EnableWindow(nStatus);
	m_CntlAutoLoadCheck.EnableWindow(nStatus);
	m_CntlAutoIcepacCheck.EnableWindow(nStatus);
}



BOOL CTerminalAutoInit::OnInitDialog() 
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

    m_bAutoInit = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableAutoInitDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateAutoInitDialogFromDatabase ( sTerminalRecordOld );
	}  


	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTerminalAutoInit::PopulateAutoInitDialogFromDatabase ( DCF01 sTerminalRecord )
{
    CMyUtils myUtilsObject;
	CString temp;
	char sDate[5] = "";

	if ( m_nDialogMode != MODE_VIEW && m_bAutoInit )	
		EnableAutoInitDialog(TRUE);
	else
		EnableAutoInitDialog(FALSE);

	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrMerchantIDFld = sTerminalRecord.merchant_id;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;

	temp = sTerminalRecord.auto_init_activation;
	if ( temp == "1" )
		m_CntlAutoInitCheck.SetCheck(1);
	else 
		m_CntlAutoInitCheck.SetCheck(0);

	temp = sTerminalRecord.auto_program_load_activation;
	if ( temp == "1" )
		m_CntlAutoLoadCheck.SetCheck(1);
	else
		m_CntlAutoLoadCheck.SetCheck(0);

	temp = sTerminalRecord.auto_icepac_activation;
	if ( temp == "1" )
		m_CntlAutoIcepacCheck.SetCheck(1);
	else
		m_CntlAutoIcepacCheck.SetCheck(0);

	if (strlen((char *)sTerminalRecord.auto_init_time) > 0)
		m_StrAutoInitTime.Format("%2.2s:%2.2s", sTerminalRecord.auto_init_time, sTerminalRecord.auto_init_time+2);

	if (strlen((char *)sTerminalRecord.auto_program_load_time) > 0)
		m_StrAutoLoadTime.Format("%2.2s:%2.2s", sTerminalRecord.auto_program_load_time, sTerminalRecord.auto_program_load_time+2);

	m_StrAutoInitRef  = sTerminalRecord.auto_init_ref_number;
	m_StrAutoLoadRef  = sTerminalRecord.auto_program_load_ref_number;

	
//	UpdateData(FALSE);

//	memcpy ( &sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));

	return TRUE;

}

BOOL  CTerminalAutoInit::PopulateDatabaseFromAutoInitDialog ( pDCF01 psTerminalRecord )
{

	CString temp;
		
	temp	= m_StrAutoInitTime.Left(2) + m_StrAutoInitTime.Right(2);
	memcpy ( psTerminalRecord->auto_init_time, temp, 4);
	memcpy ( psTerminalRecord->auto_init_ref_number,	m_StrAutoInitRef,	sizeof ( psTerminalRecord->auto_init_ref_number));

	temp	= m_StrAutoLoadTime.Left(2) + m_StrAutoLoadTime.Right(2);
	memcpy ( psTerminalRecord->auto_program_load_time, temp, 4);
	memcpy ( psTerminalRecord->auto_program_load_ref_number,
														m_StrAutoLoadRef,    sizeof ( psTerminalRecord->auto_program_load_ref_number));

	if (m_CntlAutoInitCheck.GetCheck() == 1)
		memcpy (psTerminalRecord->auto_init_activation, "1", sizeof(psTerminalRecord->auto_init_activation));
	else
		memcpy (psTerminalRecord->auto_init_activation, "0", sizeof(psTerminalRecord->auto_init_activation));

	if (m_CntlAutoLoadCheck.GetCheck() == 1)
		memcpy (psTerminalRecord->auto_program_load_activation, "1", sizeof(psTerminalRecord->auto_program_load_activation));
	else
		memcpy (psTerminalRecord->auto_program_load_activation, "0", sizeof(psTerminalRecord->auto_program_load_activation));

	if (m_CntlAutoIcepacCheck.GetCheck() == 1)
		memcpy (psTerminalRecord->auto_icepac_activation, "1", sizeof(psTerminalRecord->auto_icepac_activation));
	else
		memcpy (psTerminalRecord->auto_icepac_activation, "0", sizeof(psTerminalRecord->auto_icepac_activation));


	
	return TRUE;
}


void CTerminalAutoInit::PopulateHistoryTable()
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
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCSTR)"DCF01",			sizeof (sHistoryData.file_name));
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





BOOL CTerminalAutoInit::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW && m_nDialogMode != MODE_DONE )
		SetModified (TRUE);	

	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CTerminalAutoInit::OnSetActive() 
{
   if ( m_nDialogMode == MODE_ADD && m_bOpen )
   {
      CleanAutoInitDialog ();
      m_bOpen = FALSE;
   }	
	return CPropertyPage::OnSetActive();
}

void CTerminalAutoInit::OnKillfocusAutoLoadTime() 
{
   UpdateData(TRUE);

	if (!CheckTime(m_StrAutoLoadTime))
    {     
        UpdateData(FALSE);
        m_CntlAutoLoadTime.SetFocus();
		return;
    }

    if ( m_StrAutoLoadTime.GetLength() == 4)
    {
        CString temp = m_StrAutoLoadTime.Left(2) + ":" + m_StrAutoLoadTime.Right(2);
        m_StrAutoLoadTime = temp;
        UpdateData(FALSE);
		return;
    }	
}

void CTerminalAutoInit::OnKillfocusAutoInitTime() 
{
   UpdateData(TRUE);

	if (!CheckTime(m_StrAutoInitTime))
    {     
        m_CntlAutoInitTime.SetFocus();
        UpdateData(FALSE);
		return ;
    }

    if ( m_StrAutoInitTime.GetLength() == 4)
    {
        CString temp = m_StrAutoInitTime.Left(2) + ":" + m_StrAutoInitTime.Right(2);
        m_StrAutoInitTime = temp;
        UpdateData(FALSE);
		return ;
    }		
}
