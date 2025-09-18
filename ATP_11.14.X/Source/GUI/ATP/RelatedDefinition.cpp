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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\RelatedDefinition.cpp  $
   
      Rev 1.15   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.14   Jul 10 2002 09:56:44   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.13   May 21 2002 13:44:58   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.12   Jan 21 2000 12:23:42   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.11   Jan 05 2000 14:54:18   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.4   Oct 15 1999 17:19:58   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:44   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:59:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:12   iarustam
   Initial Release
   

*   
************************************************************************************/
// RelatedDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "RelatedDefinition.h"
#include "BinList.h"

#include "SharedMem.h"	

#include "MyUtils.h"
//#include "Login.h"

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
//extern CLogin login;
/////////////////////////////////////////////////////////////////////////////
// CRelatedDefinition property page

IMPLEMENT_DYNCREATE(CRelatedDefinition, CPropertyPage)

CRelatedDefinition::CRelatedDefinition() : CPropertyPage(CRelatedDefinition::IDD)
{
	//{{AFX_DATA_INIT(CRelatedDefinition)
	m_StrAccountNumberFld = _T("");
	m_StrAccountTypeFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CRelatedDefinition::~CRelatedDefinition()
{
}

void CRelatedDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRelatedDefinition)
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_ACCOUNT_TYPE, m_CntlAccountTypeFld);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_CntlAccountNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDV_MaxChars(pDX, m_StrAccountNumberFld, 19);
	DDX_CBString(pDX, IDC_ACCOUNT_TYPE, m_StrAccountTypeFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRelatedDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CRelatedDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelatedDefinition message handlers

BOOL CRelatedDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex= -1;

	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrAccountNumberFld = _T("");
	m_StrAccountTypeFld = _T("");
	m_StrMessageLbl = _T("");

	// set access rights according UCF01 table
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}
	else 
	{
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
		m_ViewButton.EnableWindow(FALSE);
		m_UpdateButton.EnableWindow(FALSE);
		m_ListButton.EnableWindow(FALSE);
	}


/*	cntr = 0;
   
    timer_handle = StartTimer(1000);	   

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }
*/
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRelatedDefinition::OnAddButton() 
{
	ACF01 sACFRecord;
	char strDate[9] = "";
    
	char strErrorMessage[200];
    
    CMyUtils myUtilsObject;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	// set m_nDialogMode for all pages to MODE_ADD
	 
	pGeneralPage->m_nDialogMode = MODE_ADD;  

	// check if Merchant ID already exists

	ZeroMemory(&sACFRecord,sizeof(ACF01)) ;

	CString strType;

	if ( m_CntlAccountTypeFld.GetCurSel() == 0 )
		strType = "05";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 1 )
		strType = "06";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 2 )
		strType = "07";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 3 )
		strType = "08";

    memcpy ((LPSTR)(sACFRecord.primary_key.account_type), strType , sizeof (sACFRecord.primary_key.account_type));
    memcpy ((LPSTR)(sACFRecord.primary_key.account_nbr), m_StrAccountNumberFld , sizeof (sACFRecord.primary_key.account_nbr));  

	int rcode = txdsapi_get_record( (pBYTE)&sACFRecord, sizeof (ACF01), ACF01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
      m_StrMessageLbl = "ERROR: Account Number already exists";
      m_nDialogMode = MODE_DONE;
	  m_CntlAccountNumberFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	// clean and enable all pages
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->UpdateData(FALSE);
		pGeneralPage->CleanGeneralDialog();
	}


	// set Merchant ID and  Organization ID for all pages
	pGeneralPage->m_StrAccountTypeFld = m_StrAccountTypeFld;
    pGeneralPage->m_StrAccountNumberFld = m_StrAccountNumberFld;
	myUtilsObject.GetSystemDate(strDate);	
	pGeneralPage->m_StrDateAddedFld = strDate;
	pGeneralPage->m_StrDateAddedFld =	  pGeneralPage->m_StrDateAddedFld.Mid(2,2) + "/" 
										+ pGeneralPage->m_StrDateAddedFld.Left(2) + "/" 
										+ pGeneralPage->m_StrDateAddedFld.Right(4);


	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Related Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlAccountTypeFld.EnableWindow(FALSE);
	m_CntlAccountNumberFld.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);	
}

void CRelatedDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    ACF01 sACFRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Account Number %s,\n Account Type %s?", (LPCTSTR) m_StrAccountNumberFld,(LPCTSTR) m_StrAccountTypeFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	CString strType;
	// try to delete
	if ( m_CntlAccountTypeFld.GetCurSel() == 0 )
		strType = "05";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 1 )
		strType = "06";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 2 )
		strType = "07";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 3 )
		strType = "08";

	memcpy ((LPTSTR)sACFRecord.primary_key.account_type,  strType,   sizeof (sACFRecord.primary_key.account_type)) ;
	memcpy ((LPTSTR)sACFRecord.primary_key.account_nbr,   m_StrAccountNumberFld, sizeof (sACFRecord.primary_key.account_nbr)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sACFRecord, sizeof ( ACF01), ACF01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Account Number %s does not exist", m_StrAccountNumberFld );
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
      memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

      PopulateHistoryTable();

		m_StrAccountNumberFld = _T("");	
		m_StrAccountTypeFld = _T("");
      m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		// clean and disable all pages
		if ( pGeneralPage->m_bGeneral )
		{
			pGeneralPage->CleanGeneralDialog();
			//pGeneralPage->EnableLocationDialog(FALSE);
		}

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
	
		m_CntlAccountTypeFld.EnableWindow(TRUE);
		m_CntlAccountNumberFld.EnableWindow(TRUE);	
}

void CRelatedDefinition::OnUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	ACF01 sACFRecord;
	CString strType;

    m_nDialogMode = MODE_EDIT;
	pGeneralPage->m_nDialogMode = MODE_EDIT;  
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return;

	if ( m_CntlAccountTypeFld.GetCurSel() == 0 )
		strType = "05";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 1 )
		strType = "06";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 2 )
		strType = "07";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 3 )
		strType = "08";

 
	// get data from DB
    memcpy ((LPSTR)(sACFRecord.primary_key.account_type), strType, sizeof (sACFRecord.primary_key.account_type) );
    memcpy ((LPSTR)(sACFRecord.primary_key.account_nbr),  m_StrAccountNumberFld,  sizeof (sACFRecord.primary_key.account_nbr) );   

	int rcode = txdsapi_get_record( (pBYTE)&sACFRecord, sizeof (ACF01), ACF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Account Number %s does not exist", m_StrAccountNumberFld );
            break;
         }

       // all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	// clean and enable all pages before populating them
	if ( pGeneralPage->m_bGeneral )
	    {
			pGeneralPage->CleanGeneralDialog();
		}

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (HST01));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sACFRecord.primary_key.account_nbr);
	strcat ( strFilePrimaryKey, (char *)sACFRecord.primary_key.account_type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), " ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "ACF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
	PopulateRelatedDialogFromDatabase ( sACFRecord );
 
	// set dialog mode for all pages
	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Related Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();


	pGeneralPage->SetModified ( FALSE );	

	m_CntlAccountNumberFld.EnableWindow(FALSE);
	m_CntlAccountTypeFld.EnableWindow(FALSE);
	
}

void CRelatedDefinition::OnViewButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	ACF01 sACFRecord;
	CString strType;

    m_nDialogMode = MODE_VIEW; 
	pGeneralPage->m_nDialogMode = MODE_VIEW;  
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return;

	if ( m_CntlAccountTypeFld.GetCurSel() == 0 )
		strType = "05";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 1 )
		strType = "06";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 2 )
		strType = "07";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 3 )
		strType = "08";

    memcpy ((LPSTR)(sACFRecord.primary_key.account_type), strType, sizeof (sACFRecord.primary_key.account_type) );
    memcpy ((LPSTR)(sACFRecord.primary_key.account_nbr),  m_StrAccountNumberFld,  sizeof (sACFRecord.primary_key.account_nbr) );   

	int rcode = txdsapi_get_record( (pBYTE)&sACFRecord, sizeof (ACF01), ACF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Account Number %s does not exist", m_StrAccountNumberFld );
            break;
         }

       // all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	// clean and enable all pages before populating them
	if ( pGeneralPage->m_bGeneral )
	    {
//			pGeneralPage->CleanGeneralDialog();
		}

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (HST01));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sACFRecord.primary_key.account_nbr);
	strcat ( strFilePrimaryKey, (char *)sACFRecord.primary_key.account_type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), " ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "ACF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	// set dialog mode for all pages
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Related Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateRelatedDialogFromDatabase ( sACFRecord );

	pGeneralPage->SetModified ( FALSE );	

	m_CntlAccountNumberFld.EnableWindow(TRUE);
	m_CntlAccountTypeFld.EnableWindow(TRUE);	
}

void CRelatedDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;
	CString strType;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	if (strcmp (m_StrAccountTypeFld,"US Dollar") == 0)
		strType = "05";
	else if (strcmp (m_StrAccountTypeFld,"Phil.Peso") == 0)
		strType = "06";
	else if (strcmp (m_StrAccountTypeFld,"Deferred") == 0)
		strType = "07";
	else if (strcmp (m_StrAccountTypeFld,"House Card") == 0)
		strType = "08";

	listDlg.m_nDialogMode = MODE_RELATED;
	listDlg.strItem0 = m_StrAccountNumberFld;
	listDlg.strItem1 = strType;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrAccountNumberFld = listDlg.strItem0;
		m_StrAccountTypeFld = listDlg.strItem1;
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CRelatedDefinition::PopulateHistoryTable()
{
   HST01 sHistoryData;
   CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   char strDate[9] = "";
   char strTime[7] = "";
	char strPrimaryKey[50];
   CString strType, temp;
   CHAR wstation[21]; DWORD wsize;

   memset ( strDate, 0, sizeof (strDate));
   memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	if ( m_CntlAccountTypeFld.GetCurSel() == 0 )
		strType = "05";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 1 )
		strType = "06";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 2 )
		strType = "07";
	else if ( m_CntlAccountTypeFld.GetCurSel() == 3 )
		strType = "08";

	strcpy ( strPrimaryKey, m_StrAccountNumberFld);
	strcat ( strPrimaryKey, strType );
    
   myUtilsObject.GetSystemDate(strDate);
   myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

   pintime_get_gmt ( dest_str );

   ZeroMemory ( &sHistoryData, sizeof (HST01));

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"ACF01", sizeof (sHistoryData.file_name));
   memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
   memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
   memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

   int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( PTEMSG_OK != rcode   )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}

	/* function PopulateHistoryTable */
    
}

BOOL CRelatedDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrAccountNumberFld, this))
    {
		m_StrMessageLbl = "Please enter Account Number";	
		m_CntlAccountNumberFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrAccountTypeFld, this))
    {
		m_StrMessageLbl = "Please enter Account Type";	
		m_CntlAccountTypeFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
    return TRUE;

}   /* function CheckDialogForCompleteness */

BOOL CRelatedDefinition::PopulateRelatedDatabaseStructureFromDialog ( pACF01 psACFRecord )
{
	CString strType;
	ZeroMemory (psACFRecord, sizeof(ACF01)) ;	
    

	if (strcmp (m_StrAccountTypeFld,"US Dollar") == 0)
		strType = "05";
	else if (strcmp (m_StrAccountTypeFld,"Phil.Peso") == 0)
		strType = "06";
	else if (strcmp (m_StrAccountTypeFld,"Deferred") == 0)
		strType = "07";
	else if (strcmp (m_StrAccountTypeFld,"House Card") == 0)
		strType = "08";

	memcpy ((LPTSTR) psACFRecord->primary_key.account_type, strType, sizeof (psACFRecord->primary_key.account_type));
	memcpy ((LPTSTR) psACFRecord->primary_key.account_nbr,m_StrAccountNumberFld, sizeof (psACFRecord->primary_key.account_nbr));

	if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( psACFRecord ))
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		return FALSE;
	}

	return TRUE;
}  /* function PopulateRelatedDatabaseStructureFromDialog */

void CRelatedDefinition::PopulateRelatedDialogFromDatabase( ACF01 sACFRecord )
{
	// if page was open - populate fields on the page
	// if page was not open - populate MCF01 structure for that page and
	//                        it going to be populated in OnInitDialog for this page
	if ( !pGeneralPage->m_bGeneral )
		memcpy( &pGeneralPage->sACFRecordOld, &sACFRecord, sizeof (ACF01) );
	else
		pGeneralPage->PopulateGeneralDialogFromDatabase(sACFRecord);

}  /* function PopulateRelatedDialogFromDatabase  */

BOOL CRelatedDefinition::OnApply() 
{
	ACF01 sACFDatabaseStructure;
	char strErrorMessage[200];

	CMyUtils myUtilsObject;

//   CString buttonlabel;
//   CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
/*   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }

*/	// check for required fields

	if ( MODE_VIEW != m_nDialogMode && MODE_DONE != m_nDialogMode )
	{
	
       if ( myUtilsObject.IsEmpty ( pGeneralPage->m_StrCreditLimitFld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pGeneralPage->m_CntlCreditLimitFld.SetFocus();
			pGeneralPage->m_StrMessageLbl = "Please enter Credit Limit";
			pGeneralPage->UpdateData(FALSE);
			return FALSE;
		}


	}
    // ****************************************************************************************
	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sACFDatabaseStructure, 0, sizeof(sACFDatabaseStructure)) ;

		if (!PopulateRelatedDatabaseStructureFromDialog (&sACFDatabaseStructure)) {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record ( (pBYTE)&sACFDatabaseStructure, sizeof (ACF01), ACF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{  	
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;
		}
		else 
		{            
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));		
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pGeneralPage->CleanGeneralDialog();


			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;  
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 

			m_CntlAccountNumberFld.SetFocus();
			m_CntlAccountNumberFld.EnableWindow(TRUE);
			m_CntlAccountTypeFld.EnableWindow(TRUE);

			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}
//////////////////////////////////////////////////////////////////////////////////////////
	if (m_nDialogMode == MODE_EDIT)
	{ 
		ACF01 sACFDatabaseStructure;
        memset(&sACFDatabaseStructure, 0, sizeof(ACF01)) ;

		if (!PopulateRelatedDatabaseStructureFromDialog (&sACFDatabaseStructure))
			return FALSE;

		int rcode = txdsapi_update_record ( (pBYTE)&sACFDatabaseStructure, sizeof (ACF01), ACF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pGeneralPage->CompareStructures();

			pGeneralPage->CleanGeneralDialog();
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Related Account Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlAccountNumberFld.EnableWindow(TRUE);
			m_CntlAccountTypeFld.EnableWindow(TRUE);

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;  
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			//return CPropertyPage::OnApply();
         return FALSE;
		}
	
    }
	m_nDialogMode = MODE_DONE;	
	//return CPropertyPage::OnApply();
   return FALSE;
}

void CRelatedDefinition::OnCancel() 
{
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanGeneralDialog();
	}

	//StopTimer();
	
	CPropertyPage::OnCancel();
} /* function OnCancel */



BOOL CRelatedDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}
