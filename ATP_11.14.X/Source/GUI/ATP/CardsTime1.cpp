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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsTime1.cpp  $
   
      Rev 1.10   Dec 01 2003 11:28:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Jan 05 2000 14:53:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:07:30   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:46:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:08   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:14   iarustam
   Initial Release
   
      Rev 1.6   Jun 11 1999 14:07:26   iarustam
   Bug fixes
   
   
      Rev 1.5   May 10 1999 11:08:04   dperson
   Standardized error messages
   
      Rev 1.4   Apr 29 1999 14:56:06   iarustam
   Bug fixes
   
      Rev 1.3   Mar 16 1999 16:29:40   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:52   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:12   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:20   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:50   iarustam
    
*
************************************************************************************/
// CardsTime1.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsTime1.h"
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
/////////////////////////////////////////////////////////////
// CCardsTime1 property page

IMPLEMENT_DYNCREATE(CCardsTime1, CPropertyPage)

CCardsTime1::CCardsTime1() : CPropertyPage(CCardsTime1::IDD)
{
	//{{AFX_DATA_INIT(CCardsTime1)
	m_StrAmount1_Fld = _T("");
	m_StrAmount1A_Fld = _T("");
	m_StrDays1_Fld = _T("");
	m_StrNumber1_Fld = _T("");
	m_StrNumber1A_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CCardsTime1::~CCardsTime1()
{
}

void CCardsTime1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsTime1)
	DDX_Control(pDX, IDC_AMOUNT1A_FLD, m_CntlAmount1A_Fld);
	DDX_Control(pDX, IDC_AMOUNT1_FLD, m_CntlAmount1_Fld);
	DDX_Control(pDX, IDC_DAYS1_FLD, m_CntlDays1_Fld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileId_Fld);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlType_Fld);
	DDX_Control(pDX, IDC_NUMBER1A_FLD, m_CntlNumber1A_Fld);
	DDX_Control(pDX, IDC_NUMBER1_FLD, m_CntlNumber1_Fld);
	DDX_Text(pDX, IDC_AMOUNT1_FLD, m_StrAmount1_Fld);
	DDV_MaxChars(pDX, m_StrAmount1_Fld, 10);
	DDX_Text(pDX, IDC_AMOUNT1A_FLD, m_StrAmount1A_Fld);
	DDV_MaxChars(pDX, m_StrAmount1A_Fld, 10);
	DDX_Text(pDX, IDC_DAYS1_FLD, m_StrDays1_Fld);
	DDV_MaxChars(pDX, m_StrDays1_Fld, 2);
	DDX_Text(pDX, IDC_NUMBER1_FLD, m_StrNumber1_Fld);
	DDV_MaxChars(pDX, m_StrNumber1_Fld, 3);
	DDX_Text(pDX, IDC_NUMBER1A_FLD, m_StrNumber1A_Fld);
	DDV_MaxChars(pDX, m_StrNumber1A_Fld, 3);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrType_Fld);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsTime1, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsTime1)
	ON_EN_KILLFOCUS(IDC_DAYS1_FLD, OnKillfocusDays1Fld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsTime1 message handlers

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsTime1::OnInitDialog() 
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

	m_bTime1 = TRUE;

	if (m_nDialogMode != MODE_DONE )
    {
		if (m_nDialogMode != MODE_VIEW)
        EnableTime1Dialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
        PopulateTime1DialogFromDatabase (sProfileIdOld);
    }    


	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}
/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsTime1::PopulateTime1DialogFromDatabase( CAF01 sProfileId )
{
    CMyUtils myUtilsObject;

     if ( m_nDialogMode != MODE_VIEW )
     EnableTime1Dialog(TRUE);

    ZeroMemory ( &sProfileIdOld, sizeof (CAF01));

	m_StrProfileId_Fld          = sProfileId.primary_key.caf_profile_id;
    m_StrType_Fld               = sProfileId.primary_key.type;

	if ( strcmp (m_StrType_Fld, "C") == 0 )
		m_StrType_Fld = "CREDIT";
	else if ( strcmp (m_StrType_Fld, "F") == 0 )
		m_StrType_Fld = "FLEET";
	else 
		m_StrType_Fld = "DEBIT";
 
    m_StrDays1_Fld         = sProfileId.velocity[0].day_period;    
    m_StrAmount1_Fld       = sProfileId.velocity[0].amt_usage[0];
    m_StrAmount1A_Fld      = sProfileId.velocity[0].amt_usage[1];
    m_StrNumber1_Fld       = sProfileId.velocity[0].nbr_usage[0];
    m_StrNumber1A_Fld      = sProfileId.velocity[0].nbr_usage[1];
    
    m_StrAmount1_Fld       = myUtilsObject.GetAmount(m_StrAmount1_Fld);
    m_StrAmount1A_Fld      = myUtilsObject.GetAmount(m_StrAmount1A_Fld);

    UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsTime1::PopulateDatabaseFromTime1Dialog( pCAF01 psProfileId )
{
    CMyUtils myUtilsObject;

	// fix for bug 149
	if ( !m_bTime1 ) 
		return TRUE;

    m_StrAmount1_Fld = myUtilsObject.GenerateAmount(m_StrAmount1_Fld, 12 );
    m_StrAmount1A_Fld = myUtilsObject.GenerateAmount(m_StrAmount1A_Fld, 12 );	

    memcpy ( psProfileId->velocity[0].day_period,       m_StrDays1_Fld,      sizeof ( psProfileId->velocity[0].day_period));
    memcpy ( psProfileId->velocity[0].nbr_usage[0],     m_StrNumber1_Fld,      sizeof ( psProfileId->velocity[0].nbr_usage[0])); 
    memcpy ( psProfileId->velocity[0].nbr_usage[1],     m_StrNumber1A_Fld,      sizeof ( psProfileId->velocity[0].nbr_usage[1])); 
    memcpy ( psProfileId->velocity[0].amt_usage[0],     m_StrAmount1_Fld,      sizeof ( psProfileId->velocity[0].amt_usage[0])); 
    memcpy ( psProfileId->velocity[0].amt_usage[1],     m_StrAmount1A_Fld,      sizeof ( psProfileId->velocity[0].amt_usage[1])); 

    
    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsTime1::CleanTime1Dialog()

{
    m_StrAmount1_Fld = _T("");
	m_StrAmount1A_Fld = _T("");
	m_StrDays1_Fld = _T("");
	m_StrNumber1_Fld = _T("");
	m_StrNumber1A_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrMessageLbl = _T("");

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/
    
void CCardsTime1::EnableTime1Dialog(int nStatus)
{   
    m_CntlDays1_Fld.EnableWindow(nStatus);    
    m_CntlAmount1_Fld.EnableWindow(nStatus);
    m_CntlAmount1A_Fld.EnableWindow(nStatus);
    m_CntlNumber1_Fld.EnableWindow(nStatus);
    m_CntlNumber1A_Fld.EnableWindow(nStatus);

	UpdateData(FALSE);


}
 
/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/   

BOOL CCardsTime1::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsTime1::CompareStructures()
{
    if (strcmp ( m_StrDays1_Fld,(char *)sProfileIdOld.velocity[0].day_period) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrDays1_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.velocity[0].day_period,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Days1",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }	
    if (strcmp ( m_StrNumber1_Fld,(char *)sProfileIdOld.velocity[0].nbr_usage[0]) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrNumber1_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.velocity[0].nbr_usage[0],sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Max Number1a",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrNumber1A_Fld,(char *)sProfileIdOld.velocity[0].nbr_usage[1]) != 0)
	{
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrNumber1A_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.velocity[0].nbr_usage[1],sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Max Number1b",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrAmount1_Fld,(char *)sProfileIdOld.velocity[0].amt_usage[0]) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrAmount1_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.velocity[0].amt_usage[0],sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Max Amount1a",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
    if (strcmp ( m_StrAmount1A_Fld,(char *)sProfileIdOld.velocity[0].amt_usage[1]) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	    memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
        memcpy(m_StrCurrentData,m_StrAmount1A_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.velocity[0].amt_usage[1],sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Maximum Amount1b",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
     }
        return TRUE;
}


/******************************************************************************************/
 //////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CCardsTime1::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, m_StrProfileId_Fld);
    strcat ( strPrimaryKey, m_StrType_Fld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CAF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
      temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_StrCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_StrPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_StrFieldTag, sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

void CCardsTime1::OnKillfocusDays1Fld() 
{

	UpdateData(TRUE);
   nDays1 = atoi ( m_StrDays1_Fld );

   if ( nDays1 > 14 )
   {
	   m_StrMessageLbl = "ERROR: Number of Days should not exceed 14";
      UpdateData(FALSE);
      m_CntlDays1_Fld.SetFocus();
   }
   else 
   {
      m_StrMessageLbl = "";
      UpdateData(FALSE);

   }

	
}
