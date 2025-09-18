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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserWorkstation.cpp  $
   
	 
      Rev 1.9   Jan 01 2011 11:30:24   TF-Srikanth
   Bug fixes for last user name in workstation dialog

      Rev 1.8   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:02:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jun 28 1999 13:59:38   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:41:12   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:57:14   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:08   iarustam
    
*
************************************************************************************/
// UserWorkstation.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "UserWorkstation.h"
#include "UserPropertySheet.h"
#include "MyUtils.h"
#include "SharedMem.h"	
extern "C" {
#include "PTETIME.h"
}

extern UCF01 ucf01;
//#include "htmlhelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CUserWorkstation property page

IMPLEMENT_DYNCREATE(CUserWorkstation, CPropertyPage)

CUserWorkstation::CUserWorkstation() : CPropertyPage(CUserWorkstation::IDD)
{
	//{{AFX_DATA_INIT(CUserWorkstation)
	m_StrUserNameFld = _T("");
	m_StrGroupIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrAddWorkstationFld = _T("");
	//}}AFX_DATA_INIT
}

CUserWorkstation::~CUserWorkstation()
{
}

void CUserWorkstation::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserWorkstation)
	DDX_Control(pDX, IDC_ADD_WORKSTATION_FLD, m_CntlAddWorkstationFld);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_CntlDeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_CntlAddButton);
	DDX_Control(pDX, IDC_VALID_WORKSTATIONS_LIST, m_WorkstationList);
	DDX_Control(pDX, IDC_USER_NAME_FLD, m_CntlUserNameFld);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDX_Text(pDX, IDC_GROUP_ID_FLD, m_StrGroupIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ADD_WORKSTATION_FLD, m_StrAddWorkstationFld);
	DDV_MaxChars(pDX, m_StrAddWorkstationFld, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserWorkstation, CPropertyPage)
	//{{AFX_MSG_MAP(CUserWorkstation)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_EN_CHANGE(IDC_ADD_WORKSTATION_FLD, OnChangeAddWorkstationFld)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserWorkstation message handlers
BOOL CUserWorkstation::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    
	CFont m_font;
	LOGFONT lf;
	m_StrUserNameFld="";//Set last username to null
	m_StrMessageLbl="";
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	
		m_bWorkstation = TRUE;

    if (m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW  || m_bGroup)
		PopulateWorkstationDialog(); 
	

		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserWorkstation::PopulateWorkstationDialog()
{

	CString temp;

	if ( m_bWorkstation)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableUserWorkstationDialog (TRUE);
		else
			EnableUserWorkstationDialog (FALSE);

		m_WorkstationList.ResetContent();

		for ( int i = 0; i < 10; i++)
		{
			temp = m_sLogin[i].terminals;
			int nResult = temp.IsEmpty();
			if (nResult == 0)
			m_WorkstationList.AddString((char *)m_sLogin[i].terminals);
			
		}

		nOldCount = m_WorkstationList.GetCount();

		SetModified ( FALSE );
	}
}

void CUserWorkstation::PopulateLoginStructure()
{
    char temp [21];

	if ( m_bWorkstation)
	{
		

		int nCount = m_WorkstationList.GetCount();
		ZeroMemory ( m_sLoginNew, sizeof (m_sLoginNew));
		for ( int i = 0; i < nCount; i++)
		{
			m_WorkstationList.GetText ( i, temp);
			memcpy ((LPTSTR) m_sLoginNew[i].terminals , (LPTSTR)temp, sizeof (m_sLoginNew[i].terminals));
		}
	}
	
}

void CUserWorkstation::OnAddButton() 
{       
    int nCount;
	CMyUtils myUtilsObject;
	ATP02 sWorkstationRecord;
	char strErrorMessage[200] = "";

	if (!UpdateData(TRUE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
			return;
		}
	memcpy ( sWorkstationRecord.primary_key.workstation_id, m_StrAddWorkstationFld, sizeof (sWorkstationRecord.primary_key.workstation_id));

	int rcode = txdsapi_get_record  ( (pBYTE)&sWorkstationRecord, sizeof (ATP02), ATP02_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{   
		m_StrMessageLbl = "ERROR: Invalid Workstation ID";
		m_nDialogMode = MODE_DONE; 

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }

    nCount = m_WorkstationList.GetCount();
    if (nCount < 10) {
		
		if (!myUtilsObject.IsEmpty ( m_StrAddWorkstationFld, this)) 
		{
			if (m_WorkstationList.FindStringExact(-1, m_StrAddWorkstationFld) == LB_ERR)
			{
				m_WorkstationList.AddString (m_StrAddWorkstationFld);
				
				memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
				memset ( m_strFieldTag, 0, sizeof ( m_strFieldTag));				
				m_StrMessageLbl = "";
				m_StrAddWorkstationFld = "";
				SetModified(TRUE);
			
				if (!UpdateData(FALSE))
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
			}
			else 
			{
				m_StrMessageLbl = "ERROR: Workstation ID already exists";	
				m_CntlAddWorkstationFld.SetFocus();
				if (!UpdateData(FALSE))
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				           
			}
		}
		else
		{
			m_StrMessageLbl = "Please enter Workstation ID";	
			m_CntlAddWorkstationFld.SetFocus();
			if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
		}
    }
    else 
	m_StrMessageLbl = "ERROR: You can add only 10 workstations";	
	m_CntlAddWorkstationFld.SetFocus();
	if (!UpdateData(FALSE))
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		
	    
}

void CUserWorkstation::OnDeleteButton() 
{
	int nIndex;
   

	m_StrMessageLbl = "";		
	if (!UpdateData(FALSE))
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);

    nIndex = m_WorkstationList.GetCurSel();
    if (nIndex > -1)
    {
        m_WorkstationList.DeleteString (nIndex);
		SetModified(TRUE);
        
    }	
	else
	{
		m_StrMessageLbl = "Please select Workstation ID to delete";		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	}
}




void CUserWorkstation::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];
    char strDate[9] = "";
    char strTime[7] = "";
    CMyUtils myUtilsObject;
	int nNewCount;
	BOOL bFound = FALSE;
	int nIndex;
	char temp[10] = "";
	int i,j;
	char strErrorMsg[200] = "";
    CString temp1;

	nNewCount = m_WorkstationList.GetCount();

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);

    
	for (  i = 0; i < nOldCount; i++ )
    { 
		bFound = FALSE;
		for (  j = 0; j < nNewCount; j++ )
		{	
			nIndex = i;
			int nResult = strcmp ((char *)m_sLogin[i].terminals,(char *)m_sLoginNew[j].terminals);
			if ( nResult == 0 )
			{
				bFound = TRUE;				
			}
		}
		
		if ( !bFound )
		{
			strcpy (m_strCurrentData, "Record Deleted");
			strcpy (m_strFieldTag, "Workstation");		
			myUtilsObject.GetSystemTime(strTime);
			pintime_get_gmt ( dest_str );
			ZeroMemory ( &sHistoryData, sizeof (HST01));
			memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
			temp1 = strDate;
			temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
    
			CHAR wstation[21]; DWORD wsize;

			wsize = sizeof( wstation );
			GetComputerName( wstation, &wsize );

			memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
			memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
			memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
			memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
			memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
			memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
			memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)m_sLogin[i].terminals, sizeof (sHistoryData.previous_data));	
			memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMsg;
				UpdateData( FALSE );
			}
		}
	}

	bFound = FALSE;
    
	for (  i = 0; i < nNewCount; i++ )
    { 
		bFound = FALSE;
		for ( int j = 0; j < nOldCount; j++ )
		{	
			nIndex = i;
			int nResult = strcmp ((char *)m_sLoginNew[i].terminals,(char *)m_sLogin[j].terminals);
			if ( nResult == 0 )
			{
				bFound = TRUE;
			
			}
		}
	
	if ( !bFound )
	{
		strcpy (m_strCurrentData, "Record Added");
		strcpy (m_strFieldTag, "Workstation ");
		
		myUtilsObject.GetSystemTime(strTime);
		pintime_get_gmt ( dest_str );
		ZeroMemory ( &sHistoryData, sizeof (HST01));
		memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
    temp1 = strDate;
    temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
		memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
		memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
		memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
		memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)m_sLoginNew[i].terminals, sizeof (sHistoryData.previous_data));	
		memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
		memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}
	}
	}
}

void CUserWorkstation::EnableUserWorkstationDialog (int nStatus)
{
	m_CntlAddButton.EnableWindow(nStatus);
	m_CntlDeleteButton.EnableWindow(nStatus);
	m_CntlAddWorkstationFld.EnableWindow(nStatus);
}

void CUserWorkstation::CleanUserWorkstationDialog ()
{
	m_StrUserNameFld = "";
	m_StrGroupIDFld = "";
	m_StrMessageLbl = "";
	m_StrAddWorkstationFld = "";
	UpdateData(FALSE);
	m_WorkstationList.ResetContent();
}



void CUserWorkstation::OnChangeAddWorkstationFld() 
{
	if ( m_nDialogMode != MODE_VIEW )
   {
      SetDefID( IDC_ADD_BUTTON );
		SetModified ( TRUE );
   }
	
}

BOOL CUserWorkstation::OnSetActive() 
{
	((CUserPropertySheet*) this->GetParent())->m_nActiveTab = 2;
	
	return CPropertyPage::OnSetActive();
}

BOOL CUserWorkstation::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	
	
	return TRUE;
}

BOOL CUserWorkstation::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
