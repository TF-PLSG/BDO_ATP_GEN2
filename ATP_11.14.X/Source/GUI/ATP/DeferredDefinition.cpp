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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredDefinition.cpp  $
   
      Rev 1.12   Dec 01 2003 11:29:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.11   Jul 10 2002 09:56:06   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.10   May 21 2002 13:35:48   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.9   Jan 05 2000 14:53:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.3   Sep 30 1999 16:17:08   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:48:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:18   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:24   iarustam
   Initial Release
   
      Rev 1.6   Jun 04 1999 17:40:14   iarustam
   Bug fixes
   
      Rev 1.5   Jun 02 1999 16:25:06   iarustam
   Bug fixes
   
      Rev 1.4   May 10 1999 11:13:00   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// DeferredDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DeferredDefinition.h"
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
// CDeferredDefinition property page

IMPLEMENT_DYNCREATE(CDeferredDefinition, CPropertyPage)

CDeferredDefinition::CDeferredDefinition() : CPropertyPage(CDeferredDefinition::IDD)
{
	//{{AFX_DATA_INIT(CDeferredDefinition)
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrTermLengthFld = _T("");
	//}}AFX_DATA_INIT
}

CDeferredDefinition::~CDeferredDefinition()
{
}

void CDeferredDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeferredDefinition)
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_TERM_LENGTH_FLD, m_CntlTermLengthFld);
	DDX_Control(pDX, IDC_ORGANIZATION_ID_FLD, m_CntlOrganizationIDFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDV_MaxChars(pDX, m_StrOrganizationIDFld, 4);
	DDX_Text(pDX, IDC_TERM_LENGTH_FLD, m_StrTermLengthFld);
	DDV_MaxChars(pDX, m_StrTermLengthFld, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeferredDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CDeferredDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeferredDefinition message handlers

BOOL CDeferredDefinition::OnInitDialog() 
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

	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrTermLengthFld = _T("");

	// set access rights according UCF01 table
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Merchant Control" ) == 0)
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



	UpdateData(FALSE);		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeferredDefinition::OnAddButton() 
{
	DEF01 sDEFRecord;
	DEF01_GUI_LIST sDEFList;
	MCF01 sMerchantRecord;
	char strDate[9] = "";
	CString temp;
    
	char strErrorMessage[200];
    
    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	// set m_nDialogMode for all pages to MODE_ADD
	 
	pGeneralPage->m_nDialogMode = MODE_ADD;  

	// check if Merchant ID already exists

	ZeroMemory(&sDEFRecord,sizeof(DEF01)) ;


    memcpy ((LPSTR)(sDEFRecord.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sDEFRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sDEFRecord.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sDEFRecord.primary_key.organization_id));  
    memcpy ((LPSTR)(sDEFRecord.primary_key.term_length), m_StrTermLengthFld, sizeof (sDEFRecord.primary_key.term_length));  

	int rcode = txdsapi_get_record( (pBYTE)&sDEFRecord, sizeof (DEF01), DEF01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
      m_StrMessageLbl = "ERROR: Deferred Purchase Record already exists";
      m_nDialogMode = MODE_DONE;
	  m_CntlMerchantIDFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	ZeroMemory(&sMerchantRecord,sizeof(MCF01)) ;

    memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

	rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord,sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if( rcode == PTEMSG_NOT_FOUND )
    {
      m_StrMessageLbl = "ERROR: Invalid Merchant ID";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
    memcpy ((LPSTR)(sDEFRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sDEFRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sDEFRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sDEFRecord.primary_key.organization_id));  
	
	rcode =  txdsapi_get_list ((pBYTE)&sDEFRecord,sizeof(DEF01), (pBYTE)&sDEFList, 
									sizeof (DEF01_GUI_LIST), ST2_DB_SELECT_LIST_BY_PARAM, DEF01_DATA, strErrorMessage);
	if( rcode == PTEMSG_OK )
    {
		int nNumRet = atoi ( (char*)sDEFList.num_returned );
		// 6 - max number of DEF01 records for this merchant and organization
		if ( 6 == nNumRet )
		{
			m_StrMessageLbl = "ERROR: Cannot add Deferred Record.  Maximum number of records reached.";
			m_nDialogMode = MODE_DONE;

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
    }

	// clean and enable all pages
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->UpdateData(FALSE);
		pGeneralPage->CleanGeneralDialog();
	}


	// set Merchant ID and  Organization ID for all pages
	pGeneralPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pGeneralPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
	pGeneralPage->m_StrTermLengthFld = m_StrTermLengthFld;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Deferred Purchase Definition");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);
	m_CntlTermLengthFld.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);			
}

void CDeferredDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    DEF01 sDEFRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Deferred Record for Merchant ID %s, Organization ID %s,Deferred Term Length %s?", 
						(LPCTSTR) m_StrMerchantIDFld, (LPCTSTR) m_StrOrganizationIDFld,
						(LPCTSTR) m_StrTermLengthFld ) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	// try to delete
    memcpy ((LPSTR)(sDEFRecord.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sDEFRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sDEFRecord.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sDEFRecord.primary_key.organization_id));  
    memcpy ((LPSTR)(sDEFRecord.primary_key.term_length), m_StrTermLengthFld, sizeof (sDEFRecord.primary_key.term_length));  

    int rcode = txdsapi_delete_record ((pBYTE)&sDEFRecord, sizeof ( DEF01 ), DEF01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Deferred Purchase Record does not exist" );
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

		m_StrMerchantIDFld = _T("");
		m_StrOrganizationIDFld = _T("");
		m_StrTermLengthFld = _T("");
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

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
	
		m_CntlMerchantIDFld.EnableWindow(TRUE);
		m_CntlOrganizationIDFld.EnableWindow(TRUE);		
		m_CntlTermLengthFld.EnableWindow(TRUE);
}

void CDeferredDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";

	listDlg.m_nDialogMode = MODE_DEFERRED;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrMerchantIDFld = listDlg.strItem0;
		m_StrOrganizationIDFld = listDlg.strItem1;
		m_StrTermLengthFld = listDlg.strItem2;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CDeferredDefinition::OnUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	DEF01 sDEFRecord;
	CString strCardType;

    m_nDialogMode = MODE_EDIT;
	pGeneralPage->m_nDialogMode = MODE_EDIT;  
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;


	// get data from DB
    memcpy ((LPSTR)(sDEFRecord.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sDEFRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sDEFRecord.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sDEFRecord.primary_key.organization_id));  
    memcpy ((LPSTR)(sDEFRecord.primary_key.term_length), m_StrTermLengthFld, sizeof (sDEFRecord.primary_key.term_length));  

	int rcode = txdsapi_get_record( (pBYTE)&sDEFRecord, sizeof (DEF01), DEF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Deferred Purchase Record does not exist" );
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
	strcpy ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.merchant_id);
	strcat ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.organization_id);
	strcat ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.term_length);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), " ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "DEF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
	PopulateDeferredDialogFromDatabase ( sDEFRecord );
 
	// set dialog mode for all pages
	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Deferred Purchase");
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

	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);		
	m_CntlTermLengthFld.EnableWindow(FALSE);
}

void CDeferredDefinition::OnViewButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	DEF01 sDEFRecord;
	CString strCardType;

    m_nDialogMode = MODE_VIEW;
	pGeneralPage->m_nDialogMode = MODE_VIEW;  
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;


	// get data from DB
    memcpy ((LPSTR)(sDEFRecord.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sDEFRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sDEFRecord.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sDEFRecord.primary_key.organization_id));  
    memcpy ((LPSTR)(sDEFRecord.primary_key.term_length), m_StrTermLengthFld, sizeof (sDEFRecord.primary_key.term_length));  

	int rcode = txdsapi_get_record( (pBYTE)&sDEFRecord, sizeof (DEF01), DEF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Deferred Purchase Record does not exist" );
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
	strcpy ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.merchant_id);
	strcat ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.organization_id);
	strcat ( strFilePrimaryKey, (char *)sDEFRecord.primary_key.term_length);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), " ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "DEF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
	PopulateDeferredDialogFromDatabase ( sDEFRecord );
 
	// set dialog mode for all pages
	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	((CPropertySheet*) this->GetParent())->SetWindowText("View Deferred Purchase");
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

	m_CntlMerchantIDFld.EnableWindow(TRUE);
	m_CntlOrganizationIDFld.EnableWindow(TRUE);		
	m_CntlTermLengthFld.EnableWindow(TRUE);
}

BOOL CDeferredDefinition::OnApply() 
{
	DEF01 sDEFDatabaseStructure;
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
*/
	// check for required fields

	
    // ****************************************************************************************
	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sDEFDatabaseStructure, 0, sizeof(DEF01)) ;
		if (!PopulateDeferredDatabaseStructureFromDialog (&sDEFDatabaseStructure)) {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record ( (pBYTE)&sDEFDatabaseStructure, sizeof (DEF01), DEF01_DATA, strErrorMessage );
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

			m_CntlMerchantIDFld.SetFocus();
			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);
			m_CntlTermLengthFld.EnableWindow(TRUE);

			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}
//////////////////////////////////////////////////////////////////////////////////////////
	if (m_nDialogMode == MODE_EDIT)
	{ 
		DEF01 sDEFDatabaseStructure;
        memset(&sDEFDatabaseStructure, 0, sizeof(DEF01)) ;

		if (!PopulateDeferredDatabaseStructureFromDialog (&sDEFDatabaseStructure))
			return FALSE;

		int rcode = txdsapi_update_record ( (pBYTE)&sDEFDatabaseStructure, sizeof (DEF01), DEF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Deferred Purchase Definition");
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
			((CPropertySheet*) this->GetParent())->SetWindowText("Deferred Purchase Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);
			m_CntlTermLengthFld.EnableWindow(TRUE);

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

void CDeferredDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
    CString temp;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrMerchantIDFld);
	strcat ( strPrimaryKey, m_StrOrganizationIDFld);
	strcat ( strPrimaryKey, m_StrTermLengthFld);
   
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"DEF01", sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
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

BOOL CDeferredDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrMerchantIDFld, this))
    {
		m_StrMessageLbl = "Please enter Merchant ID";	
		m_CntlMerchantIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrOrganizationIDFld, this))
    {
		m_StrMessageLbl = "Please enter Organization ID";	
		m_CntlOrganizationIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrTermLengthFld, this))
    {
		m_StrMessageLbl = "Please enter Term Length";	
		m_CntlTermLengthFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	
    return TRUE;

}   /* function CheckDialogForCompleteness */

BOOL CDeferredDefinition::PopulateDeferredDatabaseStructureFromDialog ( pDEF01 psDEFRecord )
{
	CString strType;
	ZeroMemory (psDEFRecord, sizeof(DEF01)) ;	
    

	memcpy ((LPTSTR) psDEFRecord->primary_key.merchant_id,     m_StrMerchantIDFld,     sizeof (psDEFRecord->primary_key.merchant_id));
	memcpy ((LPTSTR) psDEFRecord->primary_key.organization_id, m_StrOrganizationIDFld, sizeof (psDEFRecord->primary_key.organization_id));
	memcpy ((LPTSTR) psDEFRecord->primary_key.term_length,     m_StrTermLengthFld,     sizeof (psDEFRecord->primary_key.term_length));

	if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( psDEFRecord ))
		return FALSE;

	return TRUE;
}  /* function PopulateDeferredDatabaseStructureFromDialog */

void CDeferredDefinition::PopulateDeferredDialogFromDatabase( DEF01 sDEFRecord )
{
	// if page was open - populate fields on the page
	// if page was not open - populate MCF01 structure for that page and
	//                        it going to be populated in OnInitDialog for this page
	if ( !pGeneralPage->m_bGeneral )
		memcpy( &pGeneralPage->sDEFRecordOld, &sDEFRecord, sizeof (DEF01) );
	else
		pGeneralPage->PopulateGeneralDialogFromDatabase(sDEFRecord);

}  /* function PopulateDeferredDialogFromDatabase */

BOOL CDeferredDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
