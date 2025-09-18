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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkLogon.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 13 2000 17:07:46   mbysani
   bug fixed
   
      Rev 1.1   Sep 22 1999 15:56:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:08   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:50   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:50   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:22   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:24   iarustam
    
*
************************************************************************************/
// NetworkLogon.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkLogon.h"
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
// CNetworkLogon property page

IMPLEMENT_DYNCREATE(CNetworkLogon, CPropertyPage)

CNetworkLogon::CNetworkLogon() : CPropertyPage(CNetworkLogon::IDD)
{
	//{{AFX_DATA_INIT(CNetworkLogon)
	m_StrLogon1Fld = _T("");
	m_StrLogon10Fld = _T("");
	m_StrLogon2Fld = _T("");
	m_StrLogon3Fld = _T("");
	m_StrLogon4Fld = _T("");
	m_StrLogon5Fld = _T("");
	m_StrLogon6Fld = _T("");
	m_StrLogon7Fld = _T("");
	m_StrLogon8Fld = _T("");
	m_StrLogon9Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CNetworkLogon::~CNetworkLogon()
{
}

void CNetworkLogon::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkLogon)
	DDX_Control(pDX, IDC_LOGON9_FLD, m_CntlLogon9Fld);
	DDX_Control(pDX, IDC_LOGON8_FLD, m_CntlLogon8Fld);
	DDX_Control(pDX, IDC_LOGON7_FLD, m_CntlLogon7Fld);
	DDX_Control(pDX, IDC_LOGON6_FLD, m_CntlLogon6Fld);
	DDX_Control(pDX, IDC_LOGON5_FLD, m_CntlLogon5Fld);
	DDX_Control(pDX, IDC_LOGON4_FLD, m_CntlLogon4Fld);
	DDX_Control(pDX, IDC_LOGON3_FLD, m_CntlLogon3Fld);
	DDX_Control(pDX, IDC_LOGON2_FLD, m_CntlLogon2Fld);
	DDX_Control(pDX, IDC_LOGON10_FLD, m_CntlLogon10Fld);
	DDX_Control(pDX, IDC_LOGON1_FLD, m_CntlLogon1Fld);
	DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Control(pDX, IDC_NETWORK_ID_FLD, m_CntlNetworkIdFld);
	DDX_Text(pDX, IDC_LOGON1_FLD, m_StrLogon1Fld);
	DDV_MaxChars(pDX, m_StrLogon1Fld, 6);
	DDX_Text(pDX, IDC_LOGON10_FLD, m_StrLogon10Fld);
	DDV_MaxChars(pDX, m_StrLogon10Fld, 6);
	DDX_Text(pDX, IDC_LOGON2_FLD, m_StrLogon2Fld);
	DDV_MaxChars(pDX, m_StrLogon2Fld, 6);
	DDX_Text(pDX, IDC_LOGON3_FLD, m_StrLogon3Fld);
	DDV_MaxChars(pDX, m_StrLogon3Fld, 6);
	DDX_Text(pDX, IDC_LOGON4_FLD, m_StrLogon4Fld);
	DDV_MaxChars(pDX, m_StrLogon4Fld, 6);
	DDX_Text(pDX, IDC_LOGON5_FLD, m_StrLogon5Fld);
	DDV_MaxChars(pDX, m_StrLogon5Fld, 6);
	DDX_Text(pDX, IDC_LOGON6_FLD, m_StrLogon6Fld);
	DDV_MaxChars(pDX, m_StrLogon6Fld, 6);
	DDX_Text(pDX, IDC_LOGON7_FLD, m_StrLogon7Fld);
	DDV_MaxChars(pDX, m_StrLogon7Fld, 6);
	DDX_Text(pDX, IDC_LOGON8_FLD, m_StrLogon8Fld);
	DDV_MaxChars(pDX, m_StrLogon8Fld, 6);
	DDX_Text(pDX, IDC_LOGON9_FLD, m_StrLogon9Fld);
	DDV_MaxChars(pDX, m_StrLogon9Fld, 6);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NETWORK_ID_FLD, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkLogon, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkLogon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkLogon message handlers
/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkLogon::OnInitDialog() 
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

	m_bLogon = TRUE;

	if (m_nDialogMode != MODE_DONE )
    {		
        EnableLogonDialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
            PopulateLogonDialogFromDatabase (sNetworkIdOld);
    } 

	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkLogon::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
    
BOOL CNetworkLogon::PopulateDatabaseFromLogonDialog( pNCF01 psNetworkId )
{	
	memcpy ( psNetworkId->logon_bin[0].identifier,      m_StrLogon1Fld,         sizeof ( psNetworkId->logon_bin[0].identifier));
	memcpy ( psNetworkId->logon_bin[1].identifier,      m_StrLogon2Fld,         sizeof ( psNetworkId->logon_bin[1].identifier));
	memcpy ( psNetworkId->logon_bin[2].identifier,      m_StrLogon3Fld,         sizeof ( psNetworkId->logon_bin[2].identifier));
	memcpy ( psNetworkId->logon_bin[3].identifier,      m_StrLogon4Fld,         sizeof ( psNetworkId->logon_bin[3].identifier));
	memcpy ( psNetworkId->logon_bin[4].identifier,      m_StrLogon5Fld,         sizeof ( psNetworkId->logon_bin[4].identifier));
	memcpy ( psNetworkId->logon_bin[5].identifier,      m_StrLogon6Fld,         sizeof ( psNetworkId->logon_bin[5].identifier));
	memcpy ( psNetworkId->logon_bin[6].identifier,      m_StrLogon7Fld,         sizeof ( psNetworkId->logon_bin[6].identifier));
	memcpy ( psNetworkId->logon_bin[7].identifier,      m_StrLogon8Fld,         sizeof ( psNetworkId->logon_bin[7].identifier));
	memcpy ( psNetworkId->logon_bin[8].identifier,      m_StrLogon9Fld,         sizeof ( psNetworkId->logon_bin[8].identifier));
	memcpy ( psNetworkId->logon_bin[9].identifier,      m_StrLogon10Fld,        sizeof ( psNetworkId->logon_bin[9].identifier));

    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkLogon::PopulateLogonDialogFromDatabase( NCF01 sNetworkId )
{	 
    if ( m_nDialogMode != MODE_VIEW )
        EnableLogonDialog(TRUE);
  
    ZeroMemory ( &sNetworkIdOld, sizeof (NCF01));  

	m_StrNetworkIdFld = sNetworkId.primary_key.network_id;
	m_StrNetworkTypeFld = sNetworkId.primary_key.network_type;

	if ( strcmp ( m_StrNetworkTypeFld, "A" ) == 0)
		m_StrNetworkTypeFld = "ACQUIRER";
	else
		m_StrNetworkTypeFld = "ISSUER";


 
    m_StrLogon1Fld              = sNetworkId.logon_bin[0].identifier;
	m_StrLogon2Fld              = sNetworkId.logon_bin[1].identifier;
	m_StrLogon3Fld              = sNetworkId.logon_bin[2].identifier;
	m_StrLogon4Fld              = sNetworkId.logon_bin[3].identifier;
	m_StrLogon5Fld              = sNetworkId.logon_bin[4].identifier;
	m_StrLogon6Fld              = sNetworkId.logon_bin[5].identifier;
	m_StrLogon7Fld              = sNetworkId.logon_bin[6].identifier;
	m_StrLogon8Fld              = sNetworkId.logon_bin[7].identifier;
	m_StrLogon9Fld              = sNetworkId.logon_bin[8].identifier;
	m_StrLogon10Fld             = sNetworkId.logon_bin[9].identifier;         
     
    UpdateData(FALSE);

    memcpy ( &sNetworkIdOld, &sNetworkId, sizeof ( NCF01));
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
							
BOOL CNetworkLogon::CompareStructures()
{
    if (strcmp ( m_StrLogon1Fld, (char *)sNetworkIdOld.logon_bin[0].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon1Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[0].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 1", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon2Fld, (char *)sNetworkIdOld.logon_bin[1].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon2Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[1].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 2", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon3Fld, (char *)sNetworkIdOld.logon_bin[2].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon3Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[2].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 3", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon4Fld, (char *)sNetworkIdOld.logon_bin[3].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon4Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[3].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 4", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon5Fld, (char *)sNetworkIdOld.logon_bin[4].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon5Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[4].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 5", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon6Fld, (char *)sNetworkIdOld.logon_bin[5].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon6Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[5].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 6", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon7Fld, (char *)sNetworkIdOld.logon_bin[6].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon7Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[6].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 7", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon8Fld, (char *)sNetworkIdOld.logon_bin[7].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon8Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[7].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 8", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon9Fld, (char *)sNetworkIdOld.logon_bin[8].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon9Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[8].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 9", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	if (strcmp ( m_StrLogon10Fld, (char *)sNetworkIdOld.logon_bin[9].identifier) != 0)
    {
        memcpy(m_StrCurrentData, m_StrLogon10Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sNetworkIdOld.logon_bin[9].identifier, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Logon BIN identifier 10", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    
        return TRUE;
}


/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkLogon::CleanLogonDialog()
{
	m_StrLogon1Fld = _T("");
	m_StrLogon10Fld = _T("");
	m_StrLogon2Fld = _T("");
	m_StrLogon3Fld = _T("");
	m_StrLogon4Fld = _T("");
	m_StrLogon5Fld = _T("");
	m_StrLogon6Fld = _T("");
	m_StrLogon7Fld = _T("");
	m_StrLogon8Fld = _T("");
	m_StrLogon9Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNetworkIdFld = _T("");
	m_StrNetworkTypeFld = _T("");
	
	UpdateData(FALSE);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkLogon::EnableLogonDialog(int nStatus)
{
	//if ( m_nDialogMode != MODE_VIEW )
    {  
		m_CntlLogon1Fld.EnableWindow(nStatus);
		m_CntlLogon2Fld.EnableWindow(nStatus);
		m_CntlLogon3Fld.EnableWindow(nStatus);
		m_CntlLogon4Fld.EnableWindow(nStatus);
		m_CntlLogon5Fld.EnableWindow(nStatus);
		m_CntlLogon6Fld.EnableWindow(nStatus);
		m_CntlLogon7Fld.EnableWindow(nStatus);
		m_CntlLogon8Fld.EnableWindow(nStatus);
		m_CntlLogon9Fld.EnableWindow(nStatus);
		m_CntlLogon10Fld.EnableWindow(nStatus);	   	   	   	    
	}
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkLogon::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNetworkIdFld);
    strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"NCF01",               sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));               
    memcpy ((LPTSTR)sHistoryData.current_data,          (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,         (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,            (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag,             (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}	
}


